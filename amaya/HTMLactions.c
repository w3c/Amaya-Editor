/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2004
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
#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "trans.h"
#include "XLink.h"
#include "MathML.h"
#ifdef _SVG
#include "SVG.h"
#endif /* _SVG */
#include "XML.h"

#include "anim_f.h"
#include "css_f.h"
#include "init_f.h"
#include "AHTURLTools_f.h"
#include "EDITimage_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLform_f.h"
#include "HTMLhistory_f.h"
#include "HTMLimage_f.h"
#include "HTMLsave_f.h"
#include "html2thot_f.h"
#include "libmanag_f.h"
#include "selection.h"
#include "styleparser_f.h"
#include "trans_f.h"
#include "transparse_f.h"
#include "UIcss_f.h"

#ifdef ANNOTATIONS
#include "annotlib.h"
#include "ANNOTevent_f.h"
#endif /* ANNOTATIONS */
#ifdef BOOKMARKS
#include "bookmarks.h"
#include "BMevent_f.h"
#endif /* BOOKMARKS */
#ifdef DAV
#include "davlib_f.h"
#endif /* DAV */

/* Some prototypes */
static ThotBool     FollowTheLink (Element anchor, Element elSource,
				   Attribute HrefAttr, Document doc);

/* the structure used for the Forward and Backward buttons history callbacks */
typedef struct _FollowTheLink_context {
  Document             doc;
  Element              anchor;
  Element              elSource;
  char                *sourceDocUrl;
  char                *utf8path;
} FollowTheLink_context;


/* info about the last element highlighted when synchronizing with the
   source view */
static Document	    HighlightDocument = 0;
static Element	    HighlightElement = NULL;
static Attribute    HighLightAttribute = NULL;

/*----------------------------------------------------------------------
   ResetFontOrPhraseOnText: The text element elem should
   not be any longer within an element of type notType.
  ----------------------------------------------------------------------*/
static void ResetFontOrPhraseOnText (Document document, Element elem, int notType)
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
static void SetFontOrPhraseOnText (Document document, Element elem,
				 int newtype)
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
		       if (TtaCanInsertSibling (TtaGetElementType (elem),
						child, TRUE, document))
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
		       if (TtaCanInsertFirstChild (TtaGetElementType (elem),
						   next, document))
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
static void SetFontOrPhraseOnElement (Document document, Element elem,
				      int elemtype, ThotBool remove)
{
   Element             child, parent, new_, next;
   ElementType         elType, newType;
   ThotBool            substitute;

   substitute = FALSE;
   child = TtaGetFirstChild (elem);
   if (child == NULL)
     /* empty element. Create a text element in it */
     {
     elType = TtaGetElementType (elem);
     if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
       /* it's an HTML element */
       {
       if (elType.ElTypeNum == HTML_EL_Element ||
	   elType.ElTypeNum == HTML_EL_Block ||
	   elType.ElTypeNum == HTML_EL_Form_Element)
	 /* This is a choice element that has to be transformed into
	    one of it's options. Replace it by a Pseudo_paragraph or a
	    Paragraph */
	 {
	   newType.ElSSchema = elType.ElSSchema;
	   newType.ElTypeNum = HTML_EL_Pseudo_paragraph;
	   if (elType.ElTypeNum == HTML_EL_Form_Element)
	     newType.ElTypeNum = HTML_EL_Paragraph;
	   else if (elType.ElTypeNum == HTML_EL_Element)
	     {
	       parent = TtaGetParent (elem);
	       if (parent)
		 {
		   elType = TtaGetElementType (parent);
		   if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
		     if (elType.ElTypeNum == HTML_EL_NOSCRIPT ||
			 elType.ElTypeNum == HTML_EL_Block_Quote ||
			 elType.ElTypeNum == HTML_EL_MAP)
		       newType.ElTypeNum = HTML_EL_Paragraph;
		 }
	     }

	   new_ = TtaNewElement (document, newType);
	   TtaInsertSibling (new_, elem, TRUE, document);
	   TtaRegisterElementCreate (new_, document);
	   TtaRegisterElementDelete (elem, document);
	   TtaDeleteTree (elem, document);
	   elem = new_;
	 }
       }
     elType.ElTypeNum = HTML_EL_TEXT_UNIT;
     if (TtaCanInsertFirstChild(elType, elem, document))
	{
	child = TtaNewElement(document, elType);
	/* elem may be REPLACED by the new element.
	   Register it in the history */
	TtaRegisterElementDelete (elem, document);
	TtaInsertFirstChild (&child, elem, document);
	if (child == elem)
	  /* the new TEXT element has replaced the existing empty element */
	  substitute = TRUE;
	else
	  /* no replacement. Forget about element elem that was erroneously
	     registered in the history */
	  TtaCancelLastRegisteredOperation (document);
	TtaRegisterElementCreate (child, document);
	}
     }
   while (child)
     {
       if (substitute)
	 /* the empty element has been replaced by a TEXT element. Process
	    only this text element, not its following siblings */
	 next = NULL;
       else
	 {
	   next = child;
	   TtaNextSibling (&next);
	 }
	elType = TtaGetElementType (child);
	/* process only HTML elements */
	if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
	  {
	    if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	      if (remove)
		ResetFontOrPhraseOnText (document, child, elemtype);
	      else
		SetFontOrPhraseOnText (document, child, elemtype);
	    else if (!TtaIsLeaf (elType))
	      SetFontOrPhraseOnElement (document, child, elemtype, remove);
	  }
	child = next; 
     }
}

/*----------------------------------------------------------------------
   GetElemWithAttr
   Search in document doc an element having an attribute of type attrType
   whose value is nameVal.
   Return that element or NULL if not found.
   If ignoreAtt is not NULL, it is an attribute that should be ignored when
   comparing attributes.
   If ignoreEl is not NULL, it is an element that should be ignored when
   comparing attributes.
  ----------------------------------------------------------------------*/
Element GetElemWithAttr (Document doc, AttributeType attrType, char *nameVal,
			 Attribute ignoreAtt, Element ignoreEl)
{
   Element             el, elFound;
   Attribute           nameAttr;
   char               *name;
   int                 length;
   ThotBool            found;

   if (!nameVal)
     return NULL;

   elFound = NULL;
   el = TtaGetMainRoot (doc);
   found = FALSE;
   do
     {
	TtaSearchAttribute (attrType, SearchForward, el, &elFound, &nameAttr);
	if (nameAttr && elFound)
	  {
	    if (nameAttr != ignoreAtt && elFound != ignoreEl)
	      {
		length = TtaGetTextAttributeLength (nameAttr);
		length++;
		name = (char *)TtaGetMemory (length);
		if (name != NULL)
		  {
		    TtaGiveTextAttributeValue (nameAttr, name, &length);
		    /* compare the NAME attribute */
		    found = (strcmp (name, nameVal) == 0);
		    TtaFreeMemory (name);
		  }
	      }
	  }
	if (!found)
	   el = elFound;
     }
   while (!found && elFound);
   if (!found)
      elFound = NULL;
   return elFound;
}

/*----------------------------------------------------------------------
   SearchNAMEattribute
   search in document doc an element having an attribut NAME or ID (defined
   in DTD HTML, MathML or SVG) whose value is nameVal.         
   Return that element or NULL if not found.               
   If ignoreAtt is not NULL, it is an attribute that should be ignored when
   comparing NAME attributes.              
   If ignoreEl is not NULL, it is an element that should be ignored when
   comparing NAME attributes.              
  ----------------------------------------------------------------------*/
Element SearchNAMEattribute (Document doc, char *nameVal, Attribute ignoreAtt,
			     Element ignoreEl)
{
   Element             elFound;
   AttributeType       attrType;

   /* search all elements having an attribute NAME */
   attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
   attrType.AttrTypeNum = HTML_ATTR_NAME;
   elFound = GetElemWithAttr (doc, attrType, nameVal, ignoreAtt, ignoreEl);

   if (!elFound)
     {
       /* search all elements having an attribute ID */
       attrType.AttrTypeNum = HTML_ATTR_ID;
       elFound = GetElemWithAttr (doc, attrType, nameVal, ignoreAtt, ignoreEl);
     }
   if (!elFound)
     {
       /* search all elements having an attribute ID (defined in the
	  MathML DTD) */
       attrType.AttrSSchema = TtaGetSSchema ("MathML", doc);
       if (attrType.AttrSSchema)
	  /* this document uses the MathML DTD */
	  {
          attrType.AttrTypeNum = MathML_ATTR_id;
          elFound = GetElemWithAttr (doc, attrType, nameVal, ignoreAtt, ignoreEl);
	  }
     }
#ifdef _SVG
   if (!elFound)
     {
       /* search all elements having an attribute ID (defined in the
	  SVG DTD) */
       attrType.AttrSSchema = TtaGetSSchema ("SVG", doc);
       if (attrType.AttrSSchema)
	  /* this document uses the SVG DTD */
	  {
          attrType.AttrTypeNum = SVG_ATTR_id;
          elFound = GetElemWithAttr (doc, attrType, nameVal, ignoreAtt, ignoreEl);
	  }
     }
#endif /* _SVG */
#ifdef ANNOTATIONS
   if (!elFound)
     {
       /* search all elements having an attribute ID (defined in the
	  XLink S schema) */
       attrType.AttrSSchema = TtaGetSSchema ("XLink", doc);
       if (attrType.AttrSSchema)
	  /* this document uses the XLink DTD */
	  {
          attrType.AttrTypeNum = XLink_ATTR_id;
          elFound = GetElemWithAttr (doc, attrType, nameVal, ignoreAtt, ignoreEl);
	  }
     }
#endif /* ANNOTATIONS */

   return (elFound);
}


/*----------------------------------------------------------------------
   FollowTheLink_callback
   This function is called when the document is loaded
  ----------------------------------------------------------------------*/
void FollowTheLink_callback (int targetDocument, int status, char *urlName,
			     char *outputfile, AHTHeaders *http_headers,
			     void *context)
{
  Element             elFound=NULL;
  ElementType         elType;
  Element             elSource;
  Document            doc;
  Element             anchor;
  AttributeType       attrType;
  Attribute           PseudoAttr, HrefAttr;
  SSchema             docSchema; 
  View                view;
  FollowTheLink_context  *ctx = (FollowTheLink_context *) context;
  char               *sourceDocUrl, *utf8path;

  /* retrieve the context */
  if (ctx == NULL)
    return;

  doc = ctx->doc;
  sourceDocUrl = ctx->sourceDocUrl;  
  anchor = ctx->anchor;
  utf8path = ctx->utf8path;
  elSource = ctx->elSource;
  if (utf8path[0] == '#' && targetDocument != 0)
    /* attribute HREF contains the NAME of a target anchor */
    elFound = SearchNAMEattribute (targetDocument, &utf8path[1], NULL, NULL);
  if (DocumentURLs[doc] && !strcmp (DocumentURLs[doc], sourceDocUrl))
  {
  elType = TtaGetElementType (anchor);
  if (elType.ElTypeNum == HTML_EL_Anchor &&
      !strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
    {
      /* it's an HTML A element. Change it's color */
      docSchema =   TtaGetSSchema ("HTML", doc);
      if (docSchema && (doc != targetDocument || utf8path[0] == '#') && anchor)
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
	  /* only turn off the link if it points that exists or that we can
	     follow */
	  if (PseudoAttr && status != -1)
	    {
	      if (utf8path[0] == '#')
		{
		  if (targetDocument != 0 && elFound)
		    TtaSetAttributeText (PseudoAttr, "visited", anchor, doc);
		}
	      else
		TtaSetAttributeText (PseudoAttr, "visited", anchor, doc);
	    }
	}
    }
  }

  if (utf8path[0] == '#' && targetDocument != 0)
    {
      if (elFound)
	{
	  elType = TtaGetElementType (elFound);
	  if (elType.ElTypeNum == HTML_EL_LINK &&
	      !strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
	    {
	      /* the target is a HTML link element, follow this link */
	      attrType.AttrSSchema = elType.ElSSchema;
	      attrType.AttrTypeNum = HTML_ATTR_HREF_;
	      HrefAttr = TtaGetAttribute (elFound, attrType);
	      if (HrefAttr)
		FollowTheLink (elFound, elSource, HrefAttr, doc);
	      return;
	    }
	  else
	    {
	      if (targetDocument == doc)
		{
		/* jump in the same document */
		  /* record current position in the history */
		  AddDocHistory (doc, DocumentURLs[doc], 
				 DocumentMeta[doc]->initial_url, 
				 DocumentMeta[doc]->form_data,
				 DocumentMeta[doc]->method);
		}
	      /* show the target element in all views */
	      for (view = 1; view < 6; view++)
		if (TtaIsViewOpen (targetDocument, view))
		  TtaShowElement (targetDocument, view, elFound, 0);
	    }
	}
    }
  TtaFreeMemory (utf8path);
  TtaFreeMemory (sourceDocUrl);
  TtaFreeMemory (ctx);
}

/*----------------------------------------------------------------------
   IsCSSLink returns TRUE is the element links a CSS stylesheet.
  ----------------------------------------------------------------------*/
ThotBool IsCSSLink (Element el, Document doc)
{
  AttributeType       attrType;
  Attribute           attr;
  char                buffer[MAX_LENGTH];
  int                 length;

  attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
  attrType.AttrTypeNum = HTML_ATTR_REL;
  attr = TtaGetAttribute (el, attrType);
  if (attr)
    {
      /* get a buffer for the attribute value */
      length = MAX_LENGTH;
      TtaGiveTextAttributeValue (attr, buffer, &length);
      if (strncasecmp (buffer, "stylesheet", 10) == 0 ||
	  strcasecmp (buffer, "style") == 0)
	{
	  /* now check the type of the stylesheet */
	  attrType.AttrTypeNum = HTML_ATTR_Link_type;
	  attr = TtaGetAttribute (el, attrType);
	  if (attr == NULL)
	    /* by default it's a CSS stylesheet */
	    return TRUE;
	  else
	    {
	      /* get a buffer for the attribute value */
	      length = MAX_LENGTH;
	      TtaGiveTextAttributeValue (attr, buffer, &length);
	      if (!strcasecmp (buffer, "text/css"))
		return TRUE;
	    }
	}
    }
   return FALSE;
}

/*----------------------------------------------------------------------
  FollowTheLink follows the link starting from the anchor element for a
  double click on the elSource element.
  The parameter doc is the document that contains the origin element.
  ----------------------------------------------------------------------*/
static ThotBool FollowTheLink (Element anchor, Element elSource,
			       Attribute HrefAttr, Document doc)
{
   AttributeType          attrType;
   Attribute              PseudoAttr, attr;
   ElementType            elType;
   Document               targetDocument, reldoc;
   SSchema                HTMLSSchema;
   char                  *pathname, *utf8value;
   char                   documentname[MAX_LENGTH];
   char                  *utf8path, *info, *s;
   int                    length;
   int                    method;
   FollowTheLink_context *ctx;
   ThotBool		  isHTML, history;

   if (anchor == NULL || HrefAttr == NULL)
     return FALSE;
   info = pathname = NULL;
   elType = TtaGetElementType (anchor);
   attrType.AttrTypeNum = 0;
   HTMLSSchema = TtaGetSSchema ("HTML", doc);
   if (HTMLSSchema)
     isHTML = TtaSameSSchemas (elType.ElSSchema, HTMLSSchema);
   else
     isHTML = FALSE;

   targetDocument = 0;
   PseudoAttr = NULL;
   /* get a buffer for the target URL */
   length = TtaGetTextAttributeLength (HrefAttr) + 1;
   utf8path = (char *)TtaGetMemory (length);
   if (utf8path)
     {
       elType = TtaGetElementType (anchor);
       if (isHTML && elType.ElTypeNum == HTML_EL_Anchor)
	 {
	   /* it's an HTML anchor */
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
       TtaGiveTextAttributeValue (HrefAttr, utf8path, &length);
       /* suppress white spaces at the end */
       length--;
       while (utf8path[length] == ' ')
	 utf8path[length--] = EOS;

       /* save the context */
       ctx = (FollowTheLink_context*)TtaGetMemory (sizeof (FollowTheLink_context));
       ctx->anchor = anchor;
       ctx->doc = doc;
       ctx->utf8path = utf8path;
       ctx->elSource = elSource;
       /* save the complete URL of the source document */
       ctx->sourceDocUrl = TtaStrdup (DocumentURLs[doc]);
       TtaSetSelectionMode (TRUE);
       if (utf8path[0] == '#')
	 {
	   /* the target element is part of the same document */
	   targetDocument = doc;
	   /* manually invoke the callback */
	   FollowTheLink_callback (targetDocument, 0, NULL, NULL, NULL, 
				   (void *) ctx);
	 }
       else
	 /* the target element seems to be in another document */
	 {
	   /* is the source element an image map? */
	   if (HTMLSSchema)
	     {
	       attrType.AttrSSchema = HTMLSSchema;
	       attrType.AttrTypeNum = HTML_ATTR_ISMAP;
	       attr = TtaGetAttribute (elSource, attrType);
	       if (attr)
		 {
		   /* it's an image map */
		   utf8value = GetActiveImageInfo (doc, elSource);
		   info = (char *)TtaConvertMbsToByte ((unsigned char *)utf8value,
						 TtaGetDefaultCharset ());
		   TtaFreeMemory (utf8value);
		 }
	     }

	   s = (char *)TtaConvertMbsToByte ((unsigned char *)utf8path,
					    TtaGetDefaultCharset ());
	   length = strlen (s);
	   TtaFreeMemory (s);
	   s = NULL;

	   if (info)
	     length += strlen (info);
	   if (length < MAX_LENGTH)
	     length = MAX_LENGTH;
	   pathname = (char *)TtaGetMemory (length);
	   strcpy (pathname, utf8path);
	   /* don't free utf8path as it's stored within the context */
	   utf8path[0] = EOS;
	   if (info)
	     {
	       /* @@ what do we do with the precedent parameters?*/
	       strcat (pathname, info);
	       TtaFreeMemory (info);
	     }
	   /* interrupt current transfer */
	   StopTransfer (doc, 1);	   
	   /* get the referred document */
	   if (!strncmp (pathname, "mailto:", 7))
	     {
	       TtaSetStatus (doc, 1,
			   TtaGetMessage (AMAYA, AM_CANNOT_LOAD),
			   pathname);
	       TtaFreeMemory (pathname);
	       TtaFreeMemory (utf8path);
	       TtaFreeMemory (ctx);
	       return (FALSE);
	     }
#ifdef ANNOTATIONS
	   /* is it an annotation link? */
	   else if (elType.ElSSchema == TtaGetSSchema ("XLink", doc) &&
	       elType.ElTypeNum == XLink_EL_XLink)
	     {
	       /* loading an annotation */
	       reldoc = 0;
	       method = CE_ANNOT;
	       history = FALSE;
	     }
#endif /* ANNOTATIONS */
	   else
	     {
	       reldoc = doc;
	       method = CE_RELATIVE;
	       history = TRUE;
	       if (isHTML && elType.ElTypeNum == HTML_EL_LINK &&
		   IsCSSLink (anchor, doc))
		 {
		   /* opening a CSS */
		   reldoc = 0;
		   method = CE_CSS;
		   history = FALSE;
		   /* normalize the URL */
		   s = (char *)TtaGetMemory (length);
		   strcpy (s, pathname);
		   NormalizeURL (s, doc, pathname, documentname, NULL);
		   TtaFreeMemory (s);
		 }
	     }

	   if (method != CE_RELATIVE || InNewWindow ||
	       CanReplaceCurrentDocument (doc, 1))
	     {
	       if (IsUndisplayedName (pathname))
		 /* it's not necessary to open a new window */
		 InNewWindow = FALSE;
	       /* Load the new document */
	       targetDocument = GetAmayaDoc (pathname, NULL, reldoc, doc, 
					     (ClickEvent)method, history, 
					     (void (*)(int, int, char*, char*, const AHTHeaders*, void*)) FollowTheLink_callback,
					     (void *) ctx);
	     }
	   else
	     {
	       TtaFreeMemory (utf8path);
	       TtaFreeMemory (ctx);
	     }
	   TtaFreeMemory (pathname);
	 }
       return (TRUE);
     }
   return (FALSE);
}

/*----------------------------------------------------------------------
  DblClickOnButton     The user has double-clicked a BUTTON element.         
  ----------------------------------------------------------------------*/
static void DblClickOnButton (Element element, Document document)
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
static ThotBool ActivateElement (Element element, Document document)
{
   AttributeType       attrType;
   Attribute           attr, HrefAttr;
   Element             anchor, elFound;
   ElementType         elType, elType1;
   SSchema             HTMLschema;
   ThotBool	       ok, isHTML, isXLink, isSVG;

   elType = TtaGetElementType (element);
   HTMLschema = TtaGetSSchema ("HTML", document);
   isSVG = FALSE;
   isXLink = FALSE;
   if (HTMLschema)
     isHTML = TtaSameSSchemas (elType.ElSSchema, HTMLschema);
   else
     isHTML = FALSE;
   if (!isHTML)
     {
       isXLink = TtaSameSSchemas (elType.ElSSchema, 
				  TtaGetSSchema ("XLink", document));
       if (!isXLink)
	 isSVG = TtaSameSSchemas (elType.ElSSchema, 
				  TtaGetSSchema ("SVG", document));
     }

   /* Check if the current element is interested in double clicks */
   ok = FALSE;
   if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT ||
       elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
       elType.ElTypeNum == HTML_EL_GRAPHICS_UNIT ||
       elType.ElTypeNum == HTML_EL_SYMBOL_UNIT)
     /* it's a basic element. It is interested whatever its namespace */
     ok = TRUE;
   else if (isHTML &&
	    (elType.ElTypeNum == HTML_EL_LINK ||
	     elType.ElTypeNum == HTML_EL_Anchor ||
	     elType.ElTypeNum == HTML_EL_AREA ||
	     elType.ElTypeNum == HTML_EL_FRAME ||
	     elType.ElTypeNum == HTML_EL_Block_Quote ||
	     elType.ElTypeNum == HTML_EL_Quotation ||
	     elType.ElTypeNum == HTML_EL_INS ||
	     elType.ElTypeNum == HTML_EL_DEL ||
	     elType.ElTypeNum == HTML_EL_C_Empty ||
	     elType.ElTypeNum == HTML_EL_Radio_Input ||
	     elType.ElTypeNum == HTML_EL_Checkbox_Input ||
	     elType.ElTypeNum == HTML_EL_Option_Menu ||
	     elType.ElTypeNum == HTML_EL_Submit_Input ||
	     elType.ElTypeNum == HTML_EL_Reset_Input ||
	     elType.ElTypeNum == HTML_EL_BUTTON_ ||
	     elType.ElTypeNum == HTML_EL_File_Input))
     ok = TRUE;
   else if (isXLink)
     ok = TRUE;
   else if (isSVG)
     ok = TRUE;

   if (!ok)
     /* DoubleClick is disabled for this element type */
     return (FALSE);

   if (isHTML && (elType.ElTypeNum == HTML_EL_Submit_Input ||
		  elType.ElTypeNum == HTML_EL_Reset_Input))
     /* Form button */
     {
	elType1.ElTypeNum = elType.ElTypeNum;
	if (elType1.ElTypeNum == HTML_EL_Submit_Input ||
	    elType1.ElTypeNum == HTML_EL_Reset_Input)
	   /* it 's a double click on a submit or reset button */
	   {
	     /* interrupt current transfer and submit the corresponding form */
	     StopTransfer (document, 1);	   
	     SubmitForm (document, element);
	   }
	else if (elType1.ElTypeNum == HTML_EL_BUTTON_)
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
       elType1.ElTypeNum = HTML_EL_BUTTON_;
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
   anchor = SearchAnchor (document, element, &HrefAttr, FALSE);

   if (anchor && HrefAttr)
     return (FollowTheLink (anchor, element, HrefAttr, document));
   else
     return FALSE;
}

/*----------------------------------------------------------------------
  DisplayUrlAnchor displays the url when an anchor is selected
  ----------------------------------------------------------------------*/
static void DisplayUrlAnchor (Element element, Document document)
{
   Attribute           HrefAttr, titleAttr;
   Element             anchor, ancestor;
   ElementType         elType;
   AttributeType       attrType;
   char                *url, *pathname, *documentname;
   int                 length;

   /* Search an ancestor that acts as a link anchor */
   HrefAttr = NULL;
   anchor = SearchAnchor (document, element, &HrefAttr, FALSE);

   if (anchor && HrefAttr)
     {
       /* Get a buffer for the target URL */
       length = TtaGetTextAttributeLength (HrefAttr);
       length++;
	
       url = (char *)TtaGetMemory (length);
       if (url != NULL)
	 {
	   /* Get the URL */
	   TtaGiveTextAttributeValue (HrefAttr, url, &length);
	   pathname = (char *)TtaGetMemory (MAX_LENGTH);
	   documentname = (char *)TtaGetMemory (MAX_LENGTH);
	   if (url[0] == '#')
	     {
	       strcpy (pathname, DocumentURLs[document]);
	       strcat (pathname, url);
	     }
	   else
	     /* Normalize the URL */
	     NormalizeURL (url, document, pathname, documentname, NULL);

	   /* Display the URL in the status line */
	   /* look for a Title attribute */
	   titleAttr = NULL;
	   ancestor = element;
	   do
	     {
	       elType = TtaGetElementType (ancestor);
	       if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
		 {
		   if (elType.ElTypeNum == HTML_EL_LINK ||
		       elType.ElTypeNum == HTML_EL_Anchor)
		     {
		       attrType.AttrSSchema = elType.ElSSchema;
		       attrType.AttrTypeNum = HTML_ATTR_Title;
		       titleAttr = TtaGetAttribute (ancestor, attrType);
		     }
		 }
	       ancestor = TtaGetParent (ancestor);
	     }
	   while (titleAttr == NULL && ancestor != NULL);
	   if (titleAttr)
	     {
	       if (url)
		 TtaFreeMemory (url);
	       length = TtaGetTextAttributeLength (titleAttr);
	       length ++;
	       url = (char *)TtaGetMemory (length);
	       if (url != NULL)
		 {
		   TtaGiveTextAttributeValue (titleAttr, url, &length);
		   strcat (pathname, " (");
		   strcat (pathname, url);
		   strcat (pathname, ")");
		 }
	     }
       
	   TtaSetStatus (document, 1, pathname, NULL);
	   
	   TtaFreeMemory (pathname);
	   TtaFreeMemory (documentname);
	   if (url)
	     TtaFreeMemory (url);
	 }
     }
}

/*----------------------------------------------------------------------
  DoAction activates the current element from the keyborad
  ----------------------------------------------------------------------*/
void DoAction (Document doc, View view)
{
  Element             firstSel;
  int                 firstChar, lastChar;

  TtaGiveFirstSelectedElement (doc, &firstSel, &firstChar, &lastChar);
  if (firstSel)
    {
      if (!ActivateElement (firstSel, doc))
	TtaSelectWord (firstSel, firstChar, doc, view);
    }
}

/*----------------------------------------------------------------------
  AcceptTab inserts a TAB.
 -----------------------------------------------------------------------*/
ThotBool AcceptTab (NotifyOnTarget *event)
{
  TtcInsertChar (event->document, 1, TAB);
  /* don't let Thot perform it's normal operation */
  return TRUE;
}

/*----------------------------------------------------------------------
  NextLinkOrFormElement selects the next link or form element.
 -----------------------------------------------------------------------*/
void NextLinkOrFormElement (Document doc, View view)
{
  ElementType         elType;
  Element             root, child, next, startEl, el;
  Attribute           attr;
  AttributeType       attrType1, attrType2;
  SSchema             HTMLschema;
  ThotBool            found, cycle;
  int                 i;
  int                 firstChar, lastChar;

  HTMLschema = TtaGetSSchema ("HTML", doc);
  attrType1.AttrTypeNum = HTML_ATTR_NAME;
  attrType1.AttrSSchema = HTMLschema;
  attrType2.AttrTypeNum = HTML_ATTR_HREF_;
  attrType2.AttrSSchema = HTMLschema;
  root = TtaGetRootElement (doc);
  TtaGiveFirstSelectedElement (doc, &el, &firstChar, &lastChar);
  if (el == NULL)
    {
      /* start from the root element */
      el = root;
      /* we don't accept to restart from the beginning */
      cycle = TRUE;
    }
  else
    cycle = FALSE;

  /* don't manage this element */
  startEl = el;
  /* we're looking for a next element */
  TtaSearchAttributes (attrType1, attrType2, SearchForward, el, &el, &attr);
  found = FALSE;
  while (!found)
    {
      if (el == NULL)
	{
	  /* end of the document */
	  el = NULL;
	  attr = NULL;
	  if (!cycle)
	    {
	      /* restart from the beginning of the document */
	      cycle = TRUE;
	      el = root;
	    }
	  else
	    /* stop the search */
	    found = TRUE;
	}  
      else if (el == startEl)
	{
	  /* we made a complete cycle and no other element was found */
	  el = NULL;
	  attr = NULL;
	  found = TRUE;
	}
      else if (attr)
	{
	  elType = TtaGetElementType (el);
	  switch (elType.ElTypeNum)
	    {
	    case HTML_EL_Option_Menu:
	    case HTML_EL_Checkbox_Input:
	    case HTML_EL_Radio_Input:
	    case HTML_EL_Submit_Input:
	    case HTML_EL_Reset_Input:
	    case HTML_EL_Button_Input:
	    case HTML_EL_BUTTON_:
	    case HTML_EL_Anchor:
	      /* no included text: select the element itself */
	      TtaSelectElement (doc, el);
	      found =TRUE;
	      break;
	      
	    case HTML_EL_Text_Area:
	    case HTML_EL_Text_Input:
	    case HTML_EL_File_Input:
	    case HTML_EL_Password_Input:
	      /* look for the last included text */
	      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	      child = TtaSearchTypedElement (elType, SearchForward, el);
	      if (child)
		{
		  next = child;
		  do
		    {
		      child = next;
		      next = TtaSearchTypedElementInTree (elType,
							  SearchForward,
							  el, child);
		    }
		  while (next);
		  i = TtaGetTextLength (child);
		  TtaSelectString (doc, child, i+1, i);
		}
	      found =TRUE;
	      break;
	      
	    default:
	      break;
	    }
	}
      if (!found)
	TtaSearchAttributes (attrType1, attrType2, SearchForward, el, &el, &attr);
    }
}


/*----------------------------------------------------------------------
  PreviousLinkOrFormElement selects the previous link or form element.
 -----------------------------------------------------------------------*/
void PreviousLinkOrFormElement (Document doc, View view)
{
  ElementType         elType;
  Element             root, child, next, startEl, el;
  Attribute           attr;
  AttributeType       attrType1, attrType2;
  SSchema             HTMLschema;
  ThotBool            found, cycle;
  int                 i;
  int                 firstChar, lastChar;

  HTMLschema = TtaGetSSchema ("HTML", doc);
  attrType1.AttrTypeNum = HTML_ATTR_NAME;
  attrType1.AttrSSchema = HTMLschema;
  attrType2.AttrTypeNum = HTML_ATTR_HREF_;
  attrType2.AttrSSchema = HTMLschema;
  /* keep in mind the last element of the document */
  root = TtaGetRootElement (doc);
  el = TtaGetLastChild (root);
  attr = NULL;
  while (el)
    {
      root = el;
      /* check if this element matches */
      attr = TtaGetAttribute (el, attrType1);
      if (attr == NULL)
	attr = TtaGetAttribute (el, attrType2);
      if (attr == NULL)
	el = TtaGetLastChild (root);
      else
	/* a right element is found */
	el = NULL;
    }
  TtaGiveLastSelectedElement (doc, &el, &firstChar, &lastChar);
  if (el == NULL)
    {
      /* start from the end of the document */
      el = root;
      /* we don't accept to restart from the beginning */
      cycle = TRUE;
      /* attr != 0 if this element matches */
      startEl = NULL;
     }
  else
    {
      cycle = FALSE;
      attr = NULL;
      /* don't manage this element */
      startEl = el;
    }

  if (attr == NULL)
    /* we're looking for a next element */
    TtaSearchAttributes (attrType1, attrType2, SearchBackward, el, &el, &attr);
  found = FALSE;
  while (!found)
    {
      if (el == NULL)
	{
	  /* begginning of the document */
	  el = NULL;
	  attr = NULL;
	  if (!cycle)
	    {
	      /* restart from the end of the document */
	      cycle = TRUE;
	      el = root;
	      /* check if this element matches */
	      attr = TtaGetAttribute (el, attrType1);
	      if (attr == NULL)
		attr = TtaGetAttribute (el, attrType2);
	    }
	  else
	    /* stop the search */
	    found = TRUE;
	}  
      else if (el == startEl)
	{
	  /* we made a complete cycle and no other element was found */
	  el = NULL;
	  attr = NULL;
	  found = TRUE;
	}
      else if (attr)
	{
	  elType = TtaGetElementType (el);
	  switch (elType.ElTypeNum)
	    {
	    case HTML_EL_Option_Menu:
	    case HTML_EL_Checkbox_Input:
	    case HTML_EL_Radio_Input:
	    case HTML_EL_Submit_Input:
	    case HTML_EL_Reset_Input:
	    case HTML_EL_Button_Input:
	    case HTML_EL_BUTTON_:
	    case HTML_EL_Anchor:
	      /* no included text: select the element itself */
	      TtaSelectElement (doc, el);
	      found =TRUE;
	      break;
	      
	    case HTML_EL_Text_Area:
	    case HTML_EL_Text_Input:
	    case HTML_EL_File_Input:
	    case HTML_EL_Password_Input:
	      /* look for the last included text */
	      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	      child = TtaSearchTypedElement (elType, SearchForward, el);
	      if (child)
		{
		  next = child;
		  do
		    {
		      child = next;
		      next = TtaSearchTypedElementInTree (elType,
							  SearchForward,
							  el, child);
		    }
		  while (next);
		  i = TtaGetTextLength (child);
		  TtaSelectString (doc, child, i+1, i);
		}
	      found =TRUE;
	      break;
	      
	    default:
	      attr = NULL;
	      break;
	    }
	}
      if (!found && !attr)
	TtaSearchAttributes (attrType1, attrType2, SearchBackward, el, &el, &attr);
    }
}

/*----------------------------------------------------------------------
  AccessKeyHandler handles links or select elements
  ----------------------------------------------------------------------*/
void AccessKeyHandler (Document doc, void *param)
{
  Element             el, child, next;
  ElementType         elType;
  SSchema             HTMLschema;
  int                 i;

  el = (Element)param;
  if (el)
    {
      elType = TtaGetElementType (el);
      HTMLschema = TtaGetSSchema ("HTML", doc);
      if (TtaSameSSchemas (elType.ElSSchema, HTMLschema) &&
	  (elType.ElTypeNum == HTML_EL_LEGEND ||
	   elType.ElTypeNum == HTML_EL_LABEL))
	TtaNextSibling (&el);
      /* activate or select the element */
      if (!ActivateElement (el, doc))
	{
	  /* look for the last included text */
	  elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	  child = TtaSearchTypedElement (elType, SearchForward, el);
	  if (child)
	    {
	      next = child;
	      do
		{
		  child = next;
		  next = TtaSearchTypedElementInTree (elType, SearchForward, el, child);
		}
	      while (next);
	      i = TtaGetTextLength (child);
	      TtaSelectString (doc, child, i+1, i);
	    }
	  else
	    /* no included text: select the element itself */
	    TtaSelectElement (doc, el);
	}
    }
}


/*----------------------------------------------------------------------
  IgnoreEvent       An empty function to be able to ignore events.
  ----------------------------------------------------------------------*/
ThotBool IgnoreEvent (NotifyElement *event)
{
  /* don't let Thot perform it's normal operation */
  return TRUE;
}

/*----------------------------------------------------------------------
  DoubleClick     The user has double-clicked an element.         
  ----------------------------------------------------------------------*/
ThotBool DoubleClick (NotifyElement *event)
{
  ThotBool usedouble;

  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &usedouble);  
  if (usedouble)
    /* don't let Thot perform normal operation */
    return (ActivateElement (event->element, event->document));
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  SimpleClick     The user has clicked an element.         
  ----------------------------------------------------------------------*/
ThotBool SimpleClick (NotifyElement *event)
{
  ThotBool usedouble;

  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &usedouble);
  if (usedouble)
    {
      DisplayUrlAnchor (event->element, event->document);
      return TRUE;
    }
  else
    /* don't let Thot perform normal operation if there is an activation */
    return (ActivateElement (event->element, event->document));
}

/*----------------------------------------------------------------------
  SimpleLClick     The user has clicked an element.         
  ----------------------------------------------------------------------*/
ThotBool SimpleLClick (NotifyElement *event)
{
#ifdef _SVG
  ElementType       elType;
  ThotBool usedouble;

  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &usedouble);
  if (DocumentTypes[event->document] == docLibrary)
    {
      /* Check the sschema of the document (HTML) */
      elType.ElSSchema = TtaGetDocumentSSchema (event->document);
      if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
	{
	  /* check the element type */
	  elType = TtaGetElementType (event->element);
	  if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
	    /* Activate Library dialogue because he has selected picture into the document */
	    /* Now we are going to browse the document tree to take the url models */
	    {
	      /* Browse the document tree to save model url */
	      SaveSVGURL (event->document, event->element);
	      /* Browse the document tree to view line selected */
	      ChangeSVGLibraryLinePresentation (event->document, event->element);
	      /* Show Drop dialog option */
	      CopyOrReference (event->document, 1);
	      return TRUE;
	    }
	}
    }
#endif /* _SVG */
  /* don't let Thot perform normal operation if there is an activation */
  return FALSE;
}


/*----------------------------------------------------------------------
  SimpleRClick     The user has clicked an element.         
  ----------------------------------------------------------------------*/
ThotBool SimpleRClick (NotifyElement *event)
{
  ThotBool done;

  InNewWindow = TRUE;
  done = ActivateElement (event->element, event->document);
  InNewWindow = FALSE;
  /* don't let Thot perform normal operation if there is an activation */
  return done;
}

/*----------------------------------------------------------------------
  AnnotSimpleClick     The user has clicked on an annotation icon
  ----------------------------------------------------------------------*/
ThotBool AnnotSimpleClick (NotifyElement *event)
{
#ifdef ANNOTATIONS
  /* if it's an annotation link, highlight the annotated text  */
  ANNOT_SelectSourceDoc (event->document, event->element);
#endif /* ANNOTATIONS */
  return SimpleClick (event);
}


/*----------------------------------------------------------------------
   UpdateTitle update the content of the Title field on top of the 
   main window, according to the contents of element el.   
  ----------------------------------------------------------------------*/
void UpdateTitle (Element el, Document doc)
{
   Element             textElem, next, sibling;
   ElementType         elType, siblingType;
   Language            lang;
   char               *text, *src;
   int                 length, i, l;

   if (TtaGetViewFrame (doc, 1) == 0)
      /* this document is not displayed */
      return;

   elType = TtaGetElementType (el);
   if (!strcmp (TtaGetSSchemaName (elType.ElSSchema),"SVG") &&
       elType.ElTypeNum == SVG_EL_title)
     /* it's a SVG title */
     {
       if (TtaGetParent (el) != TtaGetRootElement(doc))
	 /* it's not a child of the root SVG element, ignore */
	 return;
       /* this title element is a child of the root element */
       sibling = el;
       do
	 {
	   TtaPreviousSibling (&sibling);
	   if (sibling)
	     siblingType = TtaGetElementType (sibling);
	 }
       while (sibling && (siblingType.ElTypeNum != SVG_EL_title ||
			  siblingType.ElSSchema != elType.ElSSchema));
       if (sibling)
	 /* this title element has a previous title sibling. Ignore */
	 return;
     }

   textElem = TtaGetFirstChild (el);
   if (textElem != NULL)
     {
       /* what is the length of the title? */
       length = 0;
       next = textElem;
       while (next)
	 {
	   elType = TtaGetElementType (next);
	   if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	     length += TtaGetTextLength (next);
	   TtaNextSibling (&next);
	 }
       /* get the text of the title */
       length++;
       text = (char *)TtaGetMemory (length);
       text[0] = EOS;
       next = textElem;
       i = 0;
       while (next)
	 {
	   l = length - i;
	   elType = TtaGetElementType (next);
	   if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	     {
	       TtaGiveTextContent (next, (unsigned char *)&text[i], &l, &lang);
	       i += l;
	     }
	   TtaNextSibling (&next);
	 }

	if (DocumentTypes[doc] != docSource)
	  TtaChangeWindowTitle (doc, 0, text, UTF_8);
    	if (DocumentTypes[doc] == docSource || DocumentSource[doc])
	  {
	    src = (char *)TtaGetMemory (length + 9);
	    sprintf (src, "Source: %s", text);
	    if (DocumentTypes[doc] == docSource)
	      TtaChangeWindowTitle (doc, 1, src, UTF_8);
	    else
	      TtaChangeWindowTitle (DocumentSource[doc], 1, src, UTF_8);
	    TtaFreeMemory (src);
	  }
	TtaFreeMemory (text);
     }

}

/*----------------------------------------------------------------------
  CloseLogs closes all logs attached to the document.
  ----------------------------------------------------------------------*/
void CloseLogs (Document doc)
{
  int		     i;

  /* is there log documents linked to this document? */
  for (i = 1; i < DocumentTableLength; i++)
    if (DocumentURLs[i] && DocumentSource[i] == doc &&
	DocumentTypes[i] == docLog)
      {
	/* close the window of the log file attached to the
	   current document */
	TtaCloseDocument (i);
	TtaFreeMemory (DocumentURLs[i]);
	DocumentURLs[i] = NULL;
	/* switch off the button Show Log file */
	TtaSetItemOff (doc, 1, Views, BShowLogFile);
	DocumentSource[i] = 0;
	/* restore the default document type */
	DocumentTypes[i] = docHTML;
      }
}

/*----------------------------------------------------------------------
  FreeDocumentResource
  ----------------------------------------------------------------------*/
void FreeDocumentResource (Document doc)
{
  Document	     sourceDoc;
  char              *tempdocument;
  int                i;

  if (doc == 0)
    return;

  TtaSetItemOff (doc, 1, Views, BShowLogFile);
  /* unmap the Save as form */
  TtaDestroyDialogue (BaseDialog + SaveForm);
  if (doc == ParsedDoc)
    /* The document to which CSS are to be applied */
    ParsedDoc = 0;
  if (DocumentURLs[doc] != NULL)
    {
      if (DocumentTypes[doc] != docLog)
	{
	  /* remove the temporary copy of the file */
	  tempdocument = GetLocalPath (doc, DocumentURLs[doc]);
	  TtaFileUnlink (tempdocument);
	  TtaFreeMemory (tempdocument);
	  /* remove the Parsing errors file */
	  RemoveParsingErrors (doc);
#ifdef ANNOTATIONS
	  ANNOT_FreeDocumentResource (doc);
#endif /* ANNOTATIONS */
#ifdef BOOKMARKS
	  BM_FreeDocumentResource (doc);
#endif /* BOOKMARKS */
	}
      /* remove the document from the auto save list */
      RemoveAutoSavedDoc (doc);
      TtaFreeMemory (DocumentURLs[doc]);
      DocumentURLs[doc] = NULL;
      ReadOnlyDocument[doc] = FALSE;
      if (DocumentMeta[doc])
	{
	  DocumentMetaClear (DocumentMeta[doc]);
	  TtaFreeMemory (DocumentMeta[doc]);
	  DocumentMeta[doc] = NULL;
	}
      if (HighlightDocument == doc)
	ResetHighlightedElement ();
      if (DocumentTypes[doc] == docLog)
	DocumentSource[doc] = 0;
      else
	{
	  /* free access keys table */
	  TtaRemoveDocAccessKeys (doc);
	  if (DocumentSource[doc])
	    {
	      sourceDoc = DocumentSource[doc];
	      TtcCloseDocument (sourceDoc, 1);
	      FreeDocumentResource (sourceDoc);
	      DocumentSource[doc] = 0;
	    }
	  /* is this document the source of another document? */
	  for (i = 1; i < DocumentTableLength; i++)
	    if (DocumentURLs[i] && DocumentSource[i] == doc)
	      {
		DocumentSource[i] = 0;
		if (DocumentTypes[i] == docLog)
		  {
		    /* close the window of the log file attached to the
		       current document */
		    TtaCloseDocument (i);
		    TtaFreeMemory (DocumentURLs[i]);
		    DocumentURLs[i] = NULL;
		    /* switch off the button Show Log file */
		    TtaSetItemOff (doc, 1, Views, BShowLogFile);
		  }
	      }
	  RemoveDocCSSs (doc);
	  /* avoid to free images of backup documents */
	  if (BackupDocument != doc)
	    RemoveDocumentImages (doc);
	}
      /* restore the default document type */
      DocumentTypes[doc] = docHTML;
    }
}
 
/*----------------------------------------------------------------------
   DocumentClosed                                                  
  ----------------------------------------------------------------------*/
void DocumentClosed (NotifyDialog * event)
{
  Document            doc;
#ifdef _SVG
  View                tm_view;
#endif  /* _SVG */

  if (event == NULL)
    return;
#ifdef DAV
  /* NEED : deal with last document when exiting the application.
   * 
   * Now, when exiting the application, if the document is locked
   * by the user (the lock information must be in the local base),
   * this function will ask whether the user wants to unlock it.
   * If user agrees, an UNLOCK request will be sent. But, under
   * Windows machines, this request will be killed when the application
   * exit, and no unlock will be done.
   */ 
  DAVFreeLock (event->document);
#endif /* DAV */
  
#ifdef _SVG 
  Get_timeline_of_doc (event->document, &doc, &tm_view);
  if (doc)
    {
      TtaCloseView (doc, tm_view);
      Free_timeline_of_doc (event->document);	   
    }
#endif /* _SVG */
  doc = TtaGetSelectedDocument ();
  if (doc == 0 || doc == event->document)
    {
      /* table elements are no longuer selected */
      TableMenuActive = FALSE;
      MTableMenuActive = FALSE;
      SetTableMenuOff (doc, 1);
    }
  FreeDocumentResource (event->document);
  CleanUpParsingErrors ();  
}

/*----------------------------------------------------------------------
   A new element has been selected. Update menus accordingly.      
  ----------------------------------------------------------------------*/
void UpdateContextSensitiveMenus (Document doc)
{
   ElementType         elType, elTypeSel;
   Element             firstSel;
   SSchema             sch;
   int                 firstChar, lastChar;
   ThotBool            newSelInElem, withHTML, withinTable, inMath;

   TtaGiveFirstSelectedElement (doc, &firstSel, &firstChar, &lastChar);
   withHTML = FALSE;
   withinTable = FALSE;
   inMath = FALSE;
   if (firstSel)
     {
       /* check if there is HTML elements */
       sch = TtaGetSSchema ("HTML", doc);
       withHTML = (sch != NULL);

       /* look for an enclosing cell */
       elType = TtaGetElementType (firstSel);
       if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
	 {
	   elType.ElTypeNum = MathML_EL_MTABLE;
	   withinTable = (TtaGetTypedAncestor (firstSel, elType) != NULL);
	   if (withinTable)
	     {
	       elType.ElTypeNum = MathML_EL_RowLabel;
	       if (TtaGetTypedAncestor (firstSel, elType) != NULL)
		 withinTable = FALSE;
	     }
	   inMath = TRUE;
	 }
       elType.ElSSchema = sch;
       if (!withinTable)
	 {
	   elType.ElTypeNum = HTML_EL_Table;
	   withinTable = (TtaGetTypedAncestor (firstSel, elType) != NULL);
	 }

       /* update table menu entries */
       if ((!withinTable || inMath) && TableMenuActive)
	 {
	   TableMenuActive = FALSE;
	   TtaSetItemOff (doc, 1, Types, BCaption);
	   TtaSetItemOff (doc, 1, Types, BColgroup);
	   TtaSetItemOff (doc, 1, Types, BCol);
	   TtaSetItemOff (doc, 1, Types, BTHead);
	   TtaSetItemOff (doc, 1, Types, BTBody);
	   TtaSetItemOff (doc, 1, Types, BTFoot);
	   TtaSetItemOff (doc, 1, Types, BDataCell);
	   TtaSetItemOff (doc, 1, Types, BHeadingCell);
	   TtaSetItemOff (doc, 1, Types, BCellHExtend);
	   TtaSetItemOff (doc, 1, Types, BCellVExtend);
	   TtaSetItemOff (doc, 1, Types, BCellHShrink);
	   TtaSetItemOff (doc, 1, Types, BCellVShrink);
	   TtaSetItemOff (doc, 1, Types, BSelectRow);
	   TtaSetItemOff (doc, 1, Types, BCreateRowB);
	   TtaSetItemOff (doc, 1, Types, BCreateRowA);
	   TtaSetItemOff (doc, 1, Types, BSelectColumn);
	   TtaSetItemOff (doc, 1, Types, BCreateColumnB);
	   TtaSetItemOff (doc, 1, Types, BCreateColumnA);
	   TtaSetItemOff (doc, 1, Types, BPasteBefore);
	   TtaSetItemOff (doc, 1, Types, BPasteAfter);
	 }
       if ((!withinTable || !inMath) && MTableMenuActive)
	 {
	   MTableMenuActive = FALSE;
	   TtaSetItemOff (doc, 1, XMLTypes, BMCellHExtend);
	   TtaSetItemOff (doc, 1, XMLTypes, BMCellVExtend);
	   TtaSetItemOff (doc, 1, XMLTypes, BMCellHShrink);
	   TtaSetItemOff (doc, 1, XMLTypes, BMCellVShrink);
	   TtaSetItemOff (doc, 1, XMLTypes, BMSelectRow);
	   TtaSetItemOff (doc, 1, XMLTypes, BMCreateRowB);
	   TtaSetItemOff (doc, 1, XMLTypes, BMCreateRowA);
	   TtaSetItemOff (doc, 1, XMLTypes, BMSelectColumn);
	   TtaSetItemOff (doc, 1, XMLTypes, BMCreateColumnB);
	   TtaSetItemOff (doc, 1, XMLTypes, BMCreateColumnA);
	   TtaSetItemOff (doc, 1, XMLTypes, BMPasteBefore);
	   TtaSetItemOff (doc, 1, XMLTypes, BMPasteAfter);
	 }
       if (withinTable && !inMath && !TableMenuActive)
	 {
	   TableMenuActive = TRUE;
	   TtaSetItemOn (doc, 1, Types, BCaption);
	   TtaSetItemOn (doc, 1, Types, BColgroup);
	   TtaSetItemOn (doc, 1, Types, BCol);
	   TtaSetItemOn (doc, 1, Types, BTHead);
	   TtaSetItemOn (doc, 1, Types, BTBody);
	   TtaSetItemOn (doc, 1, Types, BTFoot);
	   TtaSetItemOn (doc, 1, Types, BDataCell);
	   TtaSetItemOn (doc, 1, Types, BHeadingCell);
	   TtaSetItemOn (doc, 1, Types, BCellHExtend);
	   TtaSetItemOn (doc, 1, Types, BCellVExtend);
	   TtaSetItemOn (doc, 1, Types, BCellHShrink);
	   TtaSetItemOn (doc, 1, Types, BCellVShrink);
	   TtaSetItemOn (doc, 1, Types, BSelectRow);
	   TtaSetItemOn (doc, 1, Types, BCreateRowB);
	   TtaSetItemOn (doc, 1, Types, BCreateRowA);
	   TtaSetItemOn (doc, 1, Types, BSelectColumn);
	   TtaSetItemOn (doc, 1, Types, BCreateColumnB);
	   TtaSetItemOn (doc, 1, Types, BCreateColumnA);
	 }
       if (withinTable && inMath && !MTableMenuActive)
	 {
	   MTableMenuActive = TRUE;
	   TtaSetItemOn (doc, 1, XMLTypes, BMCellHExtend);
	   TtaSetItemOn (doc, 1, XMLTypes, BMCellVExtend);
	   TtaSetItemOn (doc, 1, XMLTypes, BMCellHShrink);
	   TtaSetItemOn (doc, 1, XMLTypes, BMCellVShrink);
	   TtaSetItemOn (doc, 1, XMLTypes, BMSelectRow);
	   TtaSetItemOn (doc, 1, XMLTypes, BMCreateRowB);
	   TtaSetItemOn (doc, 1, XMLTypes, BMCreateRowA);
	   TtaSetItemOn (doc, 1, XMLTypes, BMSelectColumn);
	   TtaSetItemOn (doc, 1, XMLTypes, BMCreateColumnB);
	   TtaSetItemOn (doc, 1, XMLTypes, BMCreateColumnA);
	 }
       if (withinTable && TtaIsColumnRowSelected (doc))
	 {
	   if (inMath)
	     {
	       TtaSetItemOn (doc, 1, XMLTypes, BMPasteBefore);
	       TtaSetItemOn (doc, 1, XMLTypes, BMPasteAfter);
	     }
	   else
	     {
	       TtaSetItemOn (doc, 1, Types, BPasteBefore);
	       TtaSetItemOn (doc, 1, Types, BPasteAfter);
	     }
	 }
     }
   if (!withHTML)
     return;
   if (firstSel == NULL)
      newSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Preformatted;
	newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }

   /* 
    * elements PICTURE, Object, Applet, Big_text, Small_text, Subscript,
    * Superscript, Font_  are not permitted in a Preformatted element.
    * The corresponding menu entries must be turned off 
    */
   if (newSelInElem != SelectionInPRE)
     {
	SelectionInPRE = newSelInElem;
	if (newSelInElem)
	  {
	     TtaSetItemOff (doc, 1, Types, BImage);
	     TtaSetItemOff (doc, 1, Types, BObject);
	     TtaSetItemOff (doc, 1, Types, TBig);
	     TtaSetItemOff (doc, 1, Types, TSmall);
	     TtaSetItemOff (doc, 1, Types, TSub);
	     TtaSetItemOff (doc, 1, Types, TSup);
	  }
	else
	  {
	     TtaSetItemOn (doc, 1, Types, BImage);
	     TtaSetItemOn (doc, 1, Types, BObject);
	     TtaSetItemOn (doc, 1, Types, TBig);
	     TtaSetItemOn (doc, 1, Types, TSmall);
	     TtaSetItemOn (doc, 1, Types, TSub);
	     TtaSetItemOn (doc, 1, Types, TSup);
	  }
     }
   /* 
    * Disable the "Comment" entry of menu "Context" if current selection
    * is within a comment 
    */
   if (firstSel == NULL)
      newSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Comment_;
	newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (newSelInElem != SelectionInComment)
     {
	SelectionInComment = newSelInElem;
	if (newSelInElem)
	   TtaSetItemOff (doc, 2, StructTypes, BComment);
	else
	   TtaSetItemOn (doc, 2, StructTypes, BComment);
     }
   /* update toggle buttons in menus "Information Type" and */
   /* "Character Element" */
   if (firstSel == NULL)
      newSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Emphasis;
	elTypeSel = TtaGetElementType (firstSel);
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   newSelInElem = TRUE;
	else
	   newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInEM != newSelInElem)
     {
	SelectionInEM = newSelInElem;
	TtaSetToggleItem (doc, 1, Types, TEmphasis, newSelInElem);
	TtaSwitchButton (doc, 1, iI);
     }

   if (firstSel == NULL)
      newSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Strong;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   newSelInElem = TRUE;
	else
	   newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInSTRONG != newSelInElem)
     {
	SelectionInSTRONG = newSelInElem;
	TtaSetToggleItem (doc, 1, Types, TStrong, newSelInElem);
	TtaSwitchButton (doc, 1, iB);
     }

   if (firstSel == NULL)
      newSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Cite;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   newSelInElem = TRUE;
	else
	   newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInCITE != newSelInElem)
     {
	SelectionInCITE = newSelInElem;
	TtaSetToggleItem (doc, 1, Types, TCite, newSelInElem);
     }

   if (firstSel == NULL)
      newSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_ABBR;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   newSelInElem = TRUE;
	else
	   newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInABBR != newSelInElem)
     {
	SelectionInABBR = newSelInElem;
	TtaSetToggleItem (doc, 1, Types, TAbbreviation, newSelInElem);
     }

   if (firstSel == NULL)
      newSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_ACRONYM;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   newSelInElem = TRUE;
	else
	   newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInACRONYM != newSelInElem)
     {
	SelectionInACRONYM = newSelInElem;
	TtaSetToggleItem (doc, 1, Types, TAcronym, newSelInElem);
     }

   if (firstSel == NULL)
      newSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_INS;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   newSelInElem = TRUE;
	else
	   newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInINS != newSelInElem)
     {
	SelectionInINS = newSelInElem;
	TtaSetToggleItem (doc, 1, Types, TInsertion, newSelInElem);
     }

   if (firstSel == NULL)
      newSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_DEL;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   newSelInElem = TRUE;
	else
	   newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInDEL != newSelInElem)
     {
	SelectionInDEL = newSelInElem;
	TtaSetToggleItem (doc, 1, Types, TDeletion, newSelInElem);
     }

   if (firstSel == NULL)
      newSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Def;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   newSelInElem = TRUE;
	else
	   newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInDFN != newSelInElem)
     {
	SelectionInDFN = newSelInElem;
	TtaSetToggleItem (doc, 1, Types, TDefinition, newSelInElem);
     }

   if (firstSel == NULL)
      newSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Code;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   newSelInElem = TRUE;
	else
	   newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInCODE != newSelInElem)
     {
	SelectionInCODE = newSelInElem;
	TtaSetToggleItem (doc, 1, Types, TCode, newSelInElem);
	TtaSwitchButton (doc, 1, iT);
     }

   if (firstSel == NULL)
      newSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Variable_;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   newSelInElem = TRUE;
	else
	   newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInVAR != newSelInElem)
     {
	SelectionInVAR = newSelInElem;
	TtaSetToggleItem (doc, 1, Types, TVariable, newSelInElem);
     }

   if (firstSel == NULL)
      newSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Sample;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   newSelInElem = TRUE;
	else
	   newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInSAMP != newSelInElem)
     {
	SelectionInSAMP = newSelInElem;
	TtaSetToggleItem (doc, 1, Types, TSample, newSelInElem);
     }

   if (firstSel == NULL)
      newSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Keyboard;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   newSelInElem = TRUE;
	else
	   newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInKBD != newSelInElem)
     {
	SelectionInKBD = newSelInElem;
	TtaSetToggleItem (doc, 1, Types, TKeyboard, newSelInElem);
     }

   if (firstSel == NULL)
      newSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Italic_text;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   newSelInElem = TRUE;
	else
	   newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInI != newSelInElem)
     {
	SelectionInI = newSelInElem;
	TtaSetToggleItem (doc, 1, Types, TItalic, newSelInElem);
     }

   if (firstSel == NULL)
      newSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Bold_text;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   newSelInElem = TRUE;
	else
	   newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInB != newSelInElem)
     {
	SelectionInB = newSelInElem;
	TtaSetToggleItem (doc, 1, Types, TBold, newSelInElem);
     }

   if (firstSel == NULL)
      newSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Teletype_text;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   newSelInElem = TRUE;
	else
	   newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInTT != newSelInElem)
     {
	SelectionInTT = newSelInElem;
	TtaSetToggleItem (doc, 1, Types, TTeletype, newSelInElem);
     }

   if (firstSel == NULL)
      newSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Big_text;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   newSelInElem = TRUE;
	else
	   newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInBIG != newSelInElem)
     {
	SelectionInBIG = newSelInElem;
	TtaSetToggleItem (doc, 1, Types, TBig, newSelInElem);
     }

   if (firstSel == NULL)
      newSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Small_text;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   newSelInElem = TRUE;
	else
	   newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInSMALL != newSelInElem)
     {
	SelectionInSMALL = newSelInElem;
	TtaSetToggleItem (doc, 1, Types, TSmall, newSelInElem);
     }

   if (firstSel == NULL)
      newSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Subscript;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   newSelInElem = TRUE;
	else
	   newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInSub != newSelInElem)
     {
	SelectionInSub = newSelInElem;
	TtaSetToggleItem (doc, 1, Types, TSub, newSelInElem);
     }

   if (firstSel == NULL)
      newSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Superscript;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   newSelInElem = TRUE;
	else
	   newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInSup != newSelInElem)
     {
	SelectionInSup = newSelInElem;
	TtaSetToggleItem (doc, 1, Types, TSup, newSelInElem);
     }

   if (firstSel == NULL)
      newSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_Quotation;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   newSelInElem = TRUE;
	else
	   newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInQuote != newSelInElem)
     {
	SelectionInQuote = newSelInElem;
	TtaSetToggleItem (doc, 1, Types, TQuotation, newSelInElem);
     }

   if (firstSel == NULL)
      newSelInElem = FALSE;
   else
     {
	elType.ElTypeNum = HTML_EL_BDO;
	if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
	    elTypeSel.ElSSchema == elType.ElSSchema)
	   newSelInElem = TRUE;
	else
	   newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
     }
   if (SelectionInBDO != newSelInElem)
     {
	SelectionInBDO = newSelInElem;
	TtaSetToggleItem (doc, 1, Types, TBDO, newSelInElem);
     }
}

/*----------------------------------------------------------------------
   LineNumberOfEl
   Returns the line number (position in the source file) of element el.
  ----------------------------------------------------------------------*/
static int LineNumberOfEl (Element el)
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
void ResetHighlightedElement ()
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
void SynchronizeSourceView (NotifyElement *event)
{
  Element             firstSel, el, child, otherEl;
  ElementType         elType;
  AttributeType       attrType;
  Attribute	      attr;
  Document	      doc, otherDoc;
  char                message[50];
  int                 firstChar, lastChar, line, i, view;
  int		      val, x, y, width, height;
  ThotBool	      otherDocIsStruct, done;
   
  if (!event)
    return;
  doc = event->document;
  done = FALSE;
  /* get the other Thot document to be synchronized with the one where the
     user has just clicked */
  otherDoc = 0;
  otherDocIsStruct = FALSE;
  if (DocumentTypes[doc] == docHTML ||
      DocumentTypes[doc] == docLibrary ||
      DocumentTypes[doc] == docMath ||
      DocumentTypes[doc] == docSVG  ||
      DocumentTypes[doc] == docXml)
    /* the user clicked on a structured document, the other doc is the
       corresponding source document */
    otherDoc = DocumentSource[doc];
  else if (DocumentTypes[doc] == docSource)
    /* the user clicked on a source document, the other doc is the
       corresponding structured document */
    {
      otherDocIsStruct = TRUE;
      for (i = 1; i < DocumentTableLength; i++)
	if (DocumentURLs[i] &&
	    (DocumentTypes[i] == docHTML ||
	     DocumentTypes[i] == docLibrary ||
	     DocumentTypes[i] == docMath ||
	     DocumentTypes[i] == docSVG ||
	     DocumentTypes[i] == docXml) &&
	    DocumentSource[i] == doc)
	  {
	    otherDoc = i;
	    i = DocumentTableLength;
	  }
    }

  TtaGiveFirstSelectedElement (doc, &firstSel, &firstChar, &lastChar);
  if (otherDoc)
    /* looks for the element in the other document that corresponds to
       the clicked element */
    {
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
	 elType = TtaGetElementType (el);
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
	 
	 done = (otherEl == HighlightElement);
	 if (otherEl && !done)
	   /* different element found */
	   {
	     /* If an element is currently highlighted, remove its Highlight
		attribute */
	     ResetHighlightedElement ();
	     /* Put a Highlight attribute on the element found */
	     if (otherDocIsStruct)
	       {
		 if (DocumentTypes[otherDoc] == docHTML)
		   {
		     attrType.AttrSSchema = TtaGetSSchema ("HTML",
							   otherDoc);
		     attrType.AttrTypeNum = HTML_ATTR_Highlight;
		     val = HTML_ATTR_Highlight_VAL_Yes_;
		   }
		 else if (DocumentTypes[otherDoc] == docMath)
		   {
		     attrType.AttrSSchema = TtaGetSSchema ("MathML",
							   otherDoc);
		     attrType.AttrTypeNum = MathML_ATTR_Highlight;
		     val = MathML_ATTR_Highlight_VAL_Yes_;
		   }
		 else if (DocumentTypes[otherDoc] == docSVG)
		   {
		     attrType.AttrSSchema = TtaGetSSchema ("SVG",
							   otherDoc);
		     attrType.AttrTypeNum = SVG_ATTR_Highlight;
		     val = SVG_ATTR_Highlight_VAL_Yes_;
		   }
		 else if (DocumentTypes[otherDoc] == docXml)
		   {
		     /* We take the schema of the main root */
		     /* attrType.AttrSSchema = TtaGetSSchema ("XML",
			otherDoc); */
		     attrType.AttrSSchema = elType.ElSSchema;
		     attrType.AttrTypeNum = XML_ATTR_Highlight;
		     val = XML_ATTR_Highlight_VAL_Yes_;
		   }
#ifdef _SVG
		 else if (DocumentTypes[otherDoc] == docLibrary)
		   {
		     attrType.AttrSSchema = TtaGetSSchema ("HTML",
							   otherDoc);
		     attrType.AttrTypeNum = HTML_ATTR_Highlight;
		     val = HTML_ATTR_Highlight_VAL_Yes_;
		   }
#endif /* _SVG */
		 else
		   {
		     attrType.AttrSSchema = NULL;
		     attrType.AttrTypeNum = 0;
		     val = 0;
		   }
	       }
	     else
	       {
		 attrType.AttrSSchema = TtaGetSSchema ("TextFile",
						       otherDoc);
		 attrType.AttrTypeNum = TextFile_ATTR_Highlight;
		 val = TextFile_ATTR_Highlight_VAL_Yes_;
	       }
	     if (attrType.AttrSSchema)
	       {
		 attr = TtaNewAttribute (attrType);
		 TtaAttachAttribute (otherEl, attr, otherDoc);
		 TtaSetAttributeValue (attr, val, otherEl, otherDoc);
		 /* record the highlighted element */
		 HighlightDocument = otherDoc;
		 HighlightElement = otherEl;
		 HighLightAttribute = attr;
		 /* Scroll all views where the element appears to show it */
		 for (view = 1; view < 6; view++)
		   if (TtaIsViewOpen (otherDoc, view))
		     {
		       TtaGiveBoxAbsPosition (otherEl, otherDoc, view, UnPixel,
					      &x, &y);
		       TtaGiveWindowSize (otherDoc, view, UnPixel, &width,
					  &height);
		       if (y < 0 || y > height - 15)
			 TtaShowElement (otherDoc, view, otherEl, 25);
		     }
	       }
	     done = TRUE;
	   }
	}
    }

  if (!done)
    /* If an element is currently highlighted, remove its Highlight
       attribute */
    ResetHighlightedElement ();

  if (firstSel &&
      (DocumentTypes[doc] == docSource ||
       DocumentTypes[doc] == docText ||
       DocumentTypes[doc] == docCSS ||
       DocumentTypes[doc] == docLog))
    {
      /* display the line position of the selection */
      line = TtaGetElementLineNumber (firstSel);
      elType = TtaGetElementType (firstSel);
      if (elType.ElTypeNum == TextFile_EL_TEXT_UNIT)
	{
	  /* take into account previous elements in the same line */
	  el = TtaGetParent (firstSel);
	  el = TtaGetFirstChild (el);
	  while (el && el != firstSel)
	    {
	      /* add characters of previous elements */
	      firstChar += TtaGetElementVolume (el);
	      TtaNextSibling (&el);
	    }
	}
      sprintf (message, "line %d char %d", line, firstChar);
      TtaSetStatus (doc, 1, message, NULL);
    }
}

/*----------------------------------------------------------------------
  GetCurrentLine returns the current selected line char index in the
  source file.
 -----------------------------------------------------------------------*/
void GetCurrentLine (Document doc, int *line, int *index)
{
  Element             el, child, parent;
  ElementType         elType;
  int                 first, last;

  *line = 0;
  *index = 0;
  /* look for a selection in the current document */
  TtaGiveFirstSelectedElement (doc, &el, &first, &last);
  if (el)
    {
      *line = TtaGetElementLineNumber (el);
      *index = first;
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == TextFile_EL_Line_)
	{
	  /* take into account previous elements in the same line */
	  parent = TtaGetParent (el);
	  child = TtaGetFirstChild (parent);
	  while (child != el)
	    {
	      /* add characters of previous elements */
	      *index += TtaGetElementVolume (child);
	      TtaNextSibling (&child);
	    }
	}
    }
}

/*----------------------------------------------------------------------
  GotoLine points the corresponding line and char index in the
  source file.
 -----------------------------------------------------------------------*/
void GotoLine (Document doc, int line, int index, ThotBool selpos)
{
  Element             el, child, prev;
  ElementType         elType;
  char                message[50];
  int                 i, len;

  if (line)
    {
      /* open the source file */
      if (DocumentTypes[doc] != docCSS && DocumentTypes[doc] != docSource)
	{
	  if (DocumentSource[doc] == 0)
	    ShowSource (doc, 1);
	  doc = DocumentSource[doc];
	}
      TtaRaiseView (doc, 1);
      /* look for an element with the same line number in the other doc */
      /* line numbers are increasing in document order */
      el = TtaGetMainRoot (doc);
      elType = TtaGetElementType (el);
      elType.ElTypeNum = TextFile_EL_Line_;
      el = TtaSearchTypedElement (elType, SearchForward, el);
      for (i = 1; i < line; i++)
	TtaNextSibling (&el);
      if (el)
	{
	  child = TtaGetFirstChild (el);
	  if (child)
	    {
	      if (index > 0)
		{
		  i = index;
		  len = TtaGetElementVolume (child);
		  while (child && len < i)
		    {
		      /* skip previous elements in the same line */
		      i -= len;
		      prev = child;
		      TtaNextSibling (&child);
		      if (child == NULL)
			{
			  len = i;
			  child = prev;
			}
		      else
			len = TtaGetElementVolume (child);
		    }
		  if (selpos)
		    TtaSelectString (doc, child, i, i-1);
		  else
		    TtaSelectString (doc, child, i, i);
		}
	      else
		TtaSelectElement (doc, el);
	      sprintf (message, "line %d char %d", line, index);
	      TtaSetStatus (doc, 1, message, NULL);
	    }
	}
      else
	TtaSetStatus (doc, 1, "   ", NULL);
    }
}

/*----------------------------------------------------------------------
  ShowLogLine points the corresponding line.
 -----------------------------------------------------------------------*/
static ThotBool ShowLogLine (Element el, Document doc)
{
  Document	      otherDoc = 0;
  Element             otherEl;
  Language            lang;
  CSSInfoPtr          css;
  PInfoPtr            pInfo;
  char               *utf8value = NULL, *ptr = NULL, *s = NULL;
  int                 len, line = 0, index = 0;

  if (DocumentTypes[doc] == docLog)
    {
      /* get the target line and index from current string */
      if (el)
	 {
	   len = TtaGetTextLength (el);
	   if (len > 0)
	     {
	       utf8value = (char *)TtaGetMemory (len + 1);
	       TtaGiveTextContent (el, (unsigned char *)utf8value, &len, &lang);
	       /* extract the line number and the index within the line */
	       ptr = strstr (utf8value, "line ");
	       if (ptr)
		 sscanf (&ptr[4], "%d", &line);
	       if (ptr)
		 ptr = strstr (ptr, "char");
	       if (ptr)
		 sscanf (&ptr[4], "%d", &index);
	       /* Is there a file name in the current line */
	       ptr = strstr (utf8value, ", File ");
	       if (ptr)
		 ptr += 7;
	     }

	   /* get the target document */
	   otherDoc = DocumentSource[doc];
	   if (ptr == NULL)
	     {
	       otherEl = TtaSearchText (doc, el, FALSE, "***", ISO_8859_1);
	       if (otherEl)
		 {
		   TtaFreeMemory (utf8value);
		   len = TtaGetTextLength (otherEl);
		   utf8value = (char *)TtaGetMemory (len + 1);
		   TtaGiveTextContent (otherEl, (unsigned char *)utf8value,
				       &len, &lang);
		   ptr = strstr (utf8value, " in ");
		   if (ptr)
		     ptr += 4;
		 }
	     }

	   if (ptr)
	     s = (char *)TtaConvertMbsToByte ((unsigned char *)ptr,
					      TtaGetDefaultCharset ());
	   if (DocumentURLs[otherDoc] &&
	       s && strcmp (s, DocumentURLs[otherDoc]))
	     {
	       /* it doesn't concern the source document itself
		look or the target file */
	       for (otherDoc = 1; otherDoc < MAX_DOCUMENTS; otherDoc++)
		 if (DocumentURLs[otherDoc] &&
		     !strcmp (s, DocumentURLs[otherDoc]))
		   break;
	       if (otherDoc == MAX_DOCUMENTS)
		 {
		   /* not found: do we have to open a CSS file */
		   css = SearchCSS (0, s, NULL, &pInfo);
		   if (css)
		     otherDoc = GetAmayaDoc (s, NULL, 0, 0, CE_CSS,
					     FALSE, NULL, NULL);
		 }
	     }
	 }

      TtaFreeMemory (s);
      TtaFreeMemory (utf8value);
      /* skip to the line */
      if (line && otherDoc && otherDoc < MAX_DOCUMENTS)
	GotoLine (otherDoc, line, index, FALSE);
      return TRUE; /* don't let Thot perform normal operation */
    }
  else
    return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  SimpleClickInLog The user has clicked a log message.         
  ----------------------------------------------------------------------*/
ThotBool SimpleClickInLog (NotifyElement *event)
{
  ThotBool usedouble;

  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &usedouble);
  if (usedouble)
    return TRUE;
  else
    /* don't let Thot perform normal operation if there is an activation */
    return (ShowLogLine (event->element, event->document));
}

/*----------------------------------------------------------------------
  DoubleClickInLog The user has double-clicked a log message.         
  ----------------------------------------------------------------------*/
ThotBool DoubleClickInLog (NotifyElement *event)
{
  ThotBool usedouble;

  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &usedouble);  
  if (usedouble)
    /* don't let Thot perform normal operation */
    return (ShowLogLine (event->element, event->document));
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  RightClickInLog The user has right-clicked a log message.         
  ----------------------------------------------------------------------*/
ThotBool RightClickInLog (NotifyElement *event)
{
  return (ShowLogLine (event->element, event->document));
}

/*----------------------------------------------------------------------
   A new element has been selected. Update menus accordingly.      
  ----------------------------------------------------------------------*/
void SelectionChanged (NotifyElement *event)
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
  /* update the displayed style information */
  SynchronizeAppliedStyle (event);
}

/*----------------------------------------------------------------------
  HTMLElementTypeInMenu
 -----------------------------------------------------------------------*/
ThotBool HTMLElementTypeInMenu (NotifyElement *event)
{
  return TRUE; /* prevent Thot from putting this element name in the
		  element creation menu */
}

/*----------------------------------------------------------------------
  SetCharFontOrPhrase
  ----------------------------------------------------------------------*/
void SetCharFontOrPhrase (int document, int elemtype)
{
  Element             selectedEl, elem, firstSelectedElem;
  Element             lastSelectedElem, child, next, elFont, lastEl;
  ElementType         elType, selType;
  DisplayMode         dispMode;
  int                 length, firstSelectedChar, lastSelectedChar, i, max, min;
  Language            lang;
  CHAR_T              *buffer;
  ThotBool            remove, done, toset;

  if (!TtaGetDocumentAccessMode (document))
    /* document is ReadOnly */
    return;

  TtaGiveFirstSelectedElement (document, &selectedEl, &firstSelectedChar,
			       &lastSelectedChar);
  if (selectedEl == NULL)
    /* no selection available */
    return;

  toset = TRUE;
  TtaClearViewSelections ();
  /* don't display immediately every change made to the document structure */
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
  elType.ElSSchema = TtaGetSSchema ("HTML", document);
  elType.ElTypeNum = elemtype;
  remove = (TtaGetTypedAncestor (elem, elType) != NULL);

  TtaGiveLastSelectedElement (document, &lastEl, &i, &lastSelectedChar);
  TtaUnselect (document);

  TtaOpenUndoSequence (document, selectedEl, lastEl, firstSelectedChar,
		       lastSelectedChar);
  
  /* split the last selected elements if it's a text leaf in a HTML element */
  selType = TtaGetElementType (lastEl);
  if (selType.ElTypeNum == HTML_EL_TEXT_UNIT &&
      !strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
    /* the last selected element is a text leaf */
    {
      /* is this element within an element of the requested type? */
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
	  if (lastSelectedChar < firstSelectedChar &&
	      lastEl == firstSelectedElem)
	    /* it's a caret */
	    lastSelectedChar = firstSelectedChar;
	  length = TtaGetElementVolume (lastEl);
	  /* exclude trailing spaces from the selection */
	  if (length > 0)
	    {
	      length++;
	      buffer = (CHAR_T *)TtaGetMemory(length * sizeof(CHAR_T));
	      TtaGiveBufferContent (lastEl, buffer, length, &lang);
	      if (lastEl == firstSelectedElem)
		min = firstSelectedChar;
	      else
		min = 1;
	      while (lastSelectedChar > min &&
		     buffer[lastSelectedChar - 2] == SPACE)
		lastSelectedChar--;
	      TtaFreeMemory (buffer);
	    }
	  if (lastSelectedChar > 1 && lastSelectedChar <= length)
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

  /* split the first selected element if it's a text leaf in a HTML element */
  selType = TtaGetElementType (selectedEl);
  if (selType.ElTypeNum == HTML_EL_TEXT_UNIT &&
      !strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
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
	  elem = selectedEl;
	  /* split that text leaf if it is not entirely selected */
	  if (firstSelectedChar <= 1 && firstSelectedChar == lastSelectedChar)
	    {
	      /* insert an empty box */
	      child = TtaNewTree (document, selType, "");
	      TtaInsertSibling (child, selectedEl, TRUE, document);
	      TtaRegisterElementCreate (child, document);
	      selectedEl = child;
	      firstSelectedElem = child;
	      lastSelectedElem = child;
	      if (elem == lastEl)
		lastEl = child;
	      lastSelectedChar = 0;
	    }
	  else
	    {
	      length = TtaGetElementVolume (selectedEl);
	      if (firstSelectedChar > length)
		{
		  /* append an empty box */
		  child = TtaNewTree (document, selType, "");
		  TtaInsertSibling (child, selectedEl, FALSE, document);
		  TtaRegisterElementCreate (child, document);
		  lastSelectedChar = 0;
		  selectedEl = child;
		  if (lastSelectedElem == firstSelectedElem)
		    lastSelectedElem = selectedEl;
		}
	      else
		{
		  /* exclude leading spaces from the selection */
		  if (length > 0)
		    {
		      length++;
		      buffer = (CHAR_T*)TtaGetMemory (length * sizeof(CHAR_T));
		      TtaGiveBufferContent (selectedEl, buffer, length, &lang);
		      if (lastEl == firstSelectedElem)
			max = lastSelectedChar;
		      else
			max = length;
		      while (firstSelectedChar < max &&
			     buffer[firstSelectedChar - 1] == SPACE)
			firstSelectedChar++;
		      TtaFreeMemory (buffer);
		    }
		  if (firstSelectedChar <= length && firstSelectedChar > 1)
		    /* split the first string */
		    {
		      if (firstSelectedChar < length)
			{
			  TtaRegisterElementReplace (selectedEl, document);
			  TtaSplitText (selectedEl, firstSelectedChar,
					document);
			}
		      else
			{
			  /* append an empty box */
			  child = TtaNewTree (document, selType, "");
			  TtaInsertSibling (child, selectedEl, FALSE,document);
			}
		      TtaNextSibling (&selectedEl);
		      TtaRegisterElementCreate (selectedEl, document);
		      if (lastSelectedElem == firstSelectedElem)
			{
			  lastSelectedElem = selectedEl;
			  lastSelectedChar = lastSelectedChar - firstSelectedChar + 1;
			}
		    }
		}
	      firstSelectedElem = selectedEl;
	      firstSelectedChar = 1;
	      if (elem == lastEl)
		lastEl = selectedEl;
	    }
	}
    }

  /* process all selected elements */
  while (selectedEl)
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
      if (!strcmp (TtaGetSSchemaName (selType.ElSSchema), "HTML"))
	/* process only HTML elements */
	{
	  if (!TtaIsLeaf (selType))
	    {
	      /* this selected element is not a leaf. Process all text */
	      /* leaves of that element */
	      SetFontOrPhraseOnElement ((Document) document, selectedEl,
					elemtype, remove);
	      toset = FALSE;
	    }
	  else if (selType.ElTypeNum == HTML_EL_TEXT_UNIT)
	    /* this selected element is a text leaf */
	    {
	      /* is this element within an element of the requested type ? */
	      done = FALSE;
	      elFont = TtaGetTypedAncestor (selectedEl, elType);
	      if (remove)
		/* the element has to be removed from an element of type
		   elType */
		/* If it is not within such an element, nothing to do */
		done = (elFont == NULL);
	      else
		/* the element should be within an element of type elType */
		/* If it is already within such an element, nothing to do */
		done = (elFont != NULL);
	      if (!done)
		/* process the text leaf */
		{
		  if (remove)
		    ResetFontOrPhraseOnText (document, selectedEl, elemtype);
		  else
		    SetFontOrPhraseOnText (document, selectedEl, elemtype);
		}
	    }
	}
      /* next selected element */
      selectedEl = next;
    }

  TtaCloseUndoSequence (document);

  /* retore the display mode */
  if (dispMode == DisplayImmediately)
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
	case HTML_EL_Variable_:
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

