/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Set of functions to parse CSS rules:
 * Each ParseCSS function calls one or more Thot style API function.
 *
 * Author: I. Vatton (INRIA)
 *
 */

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#ifdef MATHML
#include "MathML.h"
#endif
#ifdef GRAPHML
#include "GraphML.h"
#endif

static STRING       TargetDocumentURL = NULL;
static int          OldWidth;
static int          OldHeight;
#define buflen 50

#include "css_f.h"
#include "init_f.h"
#include "html2thot_f.h"
#include "AHTURLTools_f.h"
#include "EDITimage_f.h"
#include "EDITORactions_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLimage_f.h"
#include "HTMLpresentation_f.h"
#include "HTMLimage_f.h"
#ifdef MATHML
#include "MathMLbuilder_f.h"
#endif
#ifdef GRAPHML
#include "GraphMLbuilder_f.h"
#endif
#include "styleparser_f.h"
#include "tree.h"

#ifdef _WINDOWS
extern HWND currentWindow;
#include "windialogapi_f.h"
#endif /* _WINDOWS */

static ThotBool AttrHREFundoable = FALSE;

/*----------------------------------------------------------------------
   SetTargetContent sets the new value of Target.                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetTargetContent (Document doc, Attribute attrNAME)
#else  /* __STDC__ */
void                SetTargetContent (doc, attrNAME)
Document            doc;
Attribute           attrNAME;
#endif /* __STDC__ */
{
   int                 length;

   /* the document that issues the command Set target becomes the target doc */
   if (TargetDocumentURL != NULL)
      TtaFreeMemory (TargetDocumentURL);
   if (doc != 0)
      {
       length = ustrlen (DocumentURLs[doc]);
       TargetDocumentURL = TtaAllocString (length + 1);
       ustrcpy (TargetDocumentURL, DocumentURLs[doc]);
     }

   if (TargetName != NULL)
     {
	TtaFreeMemory (TargetName);
	TargetName = NULL;
     }
   if (attrNAME != NULL)
     {
	/* get a buffer for the NAME */
	length = TtaGetTextAttributeLength (attrNAME);
	TargetName = TtaAllocString (length + 1);
	/* copy the NAME attribute into TargetName */
	TtaGiveTextAttributeValue (attrNAME, TargetName, &length);
     }
}


/*----------------------------------------------------------------------
   LinkToPreviousTarget
   If current selection is within an anchor, change that link, otherwise
   create a link.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                LinkToPreviousTarget (Document doc, View view)
#else  /* __STDC__ */
void                LinkToPreviousTarget (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
   Element             el;
   int                 firstSelectedChar, i;

   if (!TtaGetDocumentAccessMode (doc))
     /* the document is in ReadOnly mode */
     return;
   if ((TargetName == NULL || TargetName[0] == EOS) &&
       (TargetDocumentURL == NULL || TargetDocumentURL[0] == EOS))
     /* no target available */
     return;
   if ((TargetName == NULL || TargetName[0] == EOS) &&
       !ustrcmp (TargetDocumentURL, DocumentURLs[doc]))
     /* it's not a valid target */
     return;

   UseLastTarget = TRUE;
   TtaGiveFirstSelectedElement (doc, &el, &firstSelectedChar, &i);
   if (el != NULL)
     {
       /* Search the anchor element */
       el = SearchAnchor (doc, el, TRUE);
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
	   /* There is an anchor. Just points to the last created target */
	   SetREFattribute (el, doc, TargetDocumentURL, TargetName);
	 }
     }
}


/*----------------------------------------------------------------------
   RemoveLink: destroy the link element and remove CSS rules when the
   link points to a CSS file.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RemoveLink (Element el, Document doc)
#else
void                RemoveLink (el, doc)
Element             el;
Document            doc;
#endif
{
   ElementType	       elType;
   AttributeType       attrType;
   Attribute           attr;
   CHAR_T              buffer[MAX_LENGTH];
   CHAR_T              pathname[MAX_LENGTH], documentname[MAX_LENGTH];   
   int                 length;

   /* Search the refered image */
   elType = TtaGetElementType (el);
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_REL;
   attr = TtaGetAttribute (el, attrType);
   if (attr != 0)
     {
       /* get a buffer for the attribute value */
       length = MAX_LENGTH;
       TtaGiveTextAttributeValue (attr, buffer, &length);
       if (!ustrcasecmp (buffer, TEXT("STYLESHEET")) || !ustrcasecmp (buffer, TEXT("STYLE")))
	 {
	   /* it's a link to a style sheet. Remove that style sheet */
	   attrType.AttrTypeNum = HTML_ATTR_HREF_;
	   attr = TtaGetAttribute (el, attrType);
	   if (attr != 0)
	     {
	       /* copy the HREF attribute into the buffer */
	       length = MAX_LENGTH;
	       TtaGiveTextAttributeValue (attr, buffer, &length);
	       NormalizeURL (buffer, doc, pathname, documentname, NULL);
	       RemoveStyleSheet (pathname, doc, TRUE);
	     }
	 }
     }
}
/*----------------------------------------------------------------------
   DeleteLink                                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            DeleteLink (NotifyElement * event)
#else
ThotBool            DeleteLink (event)
NotifyElement      *event;

#endif
{
  RemoveLink (event->element, event->document);
  return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   SetXMLlinkAttr attach an xml:link="simple" attribute to element el
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetXMLlinkAttr (Element el, Document doc, ThotBool withUndo)
#else  /* __STDC__ */
static void         SetXMLlinkAttr (el, doc, withUndo)
Element		    el;
Document            doc;
ThotBool		    withUndo;

#endif /* __STDC__ */
{
  AttributeType	attrType;
  ElementType	elType;
  Attribute	attr;
  int		val;
  ThotBool	new;

  attrType.AttrTypeNum = 0;
  elType = TtaGetElementType (el);
#ifdef MATHML
  if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("MathML")) == 0)
     {
     MapMathMLAttribute (TEXT("link"), &attrType, _EMPTYSTR_, doc);
     MapMathMLAttributeValue (TEXT("simple"), attrType, &val);
     }
  else
#endif
#ifdef GRAPHML
  if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), "GraphML") == 0)
     {
     MapGraphMLAttribute ("link", &attrType, "", doc);
     MapGraphMLAttributeValue ("simple", attrType, &val);
     }
#endif
  if (attrType.AttrTypeNum > 0)
     {
     attr = TtaGetAttribute (el, attrType);
     if (attr == NULL)
        {
        attr = TtaNewAttribute (attrType);
        TtaAttachAttribute (el, attr, doc);
	new = TRUE;
        }
     else
	{
	new = FALSE;
	if (withUndo)
	   TtaRegisterAttributeReplace (attr, el, doc);
	}
     TtaSetAttributeValue (attr, val, el, doc);
     if (new && withUndo)
	TtaRegisterAttributeCreate (attr, el, doc);
     }
}

/*----------------------------------------------------------------------
   SetREFattribute  sets the HREF or CITE attribue of the element to      
   the concatenation of targetURL and targetName.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetREFattribute (Element element, Document doc, STRING targetURL, STRING targetName)
#else  /* __STDC__ */
void                SetREFattribute (element, doc, targetURL, targetName)
Element             element;
Document            doc;
STRING              targetURL;
STRING              targetName;

#endif /* __STDC__ */
{
   ElementType	       elType;
   AttributeType       attrType;
   Attribute           attr;
   SSchema	           HTMLSSchema;
   STRING              value, base;
   CHAR_T              tempURL[MAX_LENGTH];
   int                 length;
   ThotBool            new, oldStructureChecking;

   if (AttrHREFundoable)
      TtaOpenUndoSequence (doc, element, element, 0, 0);

   elType = TtaGetElementType (element);
   HTMLSSchema = TtaGetSSchema (TEXT("HTML"), doc);
   attrType.AttrSSchema = HTMLSSchema;
   if (elType.ElTypeNum == HTML_EL_Quotation ||
       elType.ElTypeNum == HTML_EL_Block_Quote ||
       elType.ElTypeNum == HTML_EL_INS ||
       elType.ElTypeNum == HTML_EL_DEL)
     attrType.AttrTypeNum = HTML_ATTR_cite;
   else
     attrType.AttrTypeNum = HTML_ATTR_HREF_;

   attr = TtaGetAttribute (element, attrType);
   if (attr == 0)
     {
       /* create an attribute HREF for the element */
       attr = TtaNewAttribute (attrType);
       /* this element may be in a different namespace, so don't check
	  validity */
       oldStructureChecking = TtaGetStructureChecking (doc);
       TtaSetStructureChecking (0, doc);
       TtaAttachAttribute (element, attr, doc);
       TtaSetStructureChecking (oldStructureChecking, doc);
       new = TRUE;
     }
   else
     {
       new = FALSE;
       if (AttrHREFundoable)
	 TtaRegisterAttributeReplace (attr, element, doc);
     }

   /* build the complete target URL */
   if (targetURL != NULL && ustrcmp(targetURL, DocumentURLs[doc]))
     ustrcpy (tempURL, targetURL);
   else
     tempURL[0] = EOS;
   if (targetName != NULL)
     {
       ustrcat (tempURL, TEXT("#"));
       ustrcat (tempURL, targetName);
     }

   if (tempURL[0] == EOS)
     {
       /* get a buffer for the attribute value */
       length = TtaGetTextAttributeLength (attr);
       if (length == 0)
	 /* no given value */
	 TtaSetAttributeText (attr, TEXT("XX"), element, doc);
     }
   else
     {
       /* set the relative value or URL in attribute HREF */
       base = GetBaseURL (doc);
       value = MakeRelativeURL (tempURL, base);
       TtaFreeMemory (base);
       if (*value == EOS)
	 TtaSetAttributeText (attr, TEXT("./"), element, doc);
       else
	 TtaSetAttributeText (attr, value, element, doc);
       TtaFreeMemory (value);
     }

   /* register the new value of the HREF attribute in the undo queue */
   if (AttrHREFundoable && new)
       TtaRegisterAttributeCreate (attr, element, doc);

   if (!TtaSameSSchemas (elType.ElSSchema, HTMLSSchema))
      /* the origin of the link is not a HTML element */
      /* create a xml:link attribute */
      SetXMLlinkAttr (element, doc, AttrHREFundoable);

   /* is it a link to a CSS file? */
   if (tempURL[0] != EOS)
       if (elType.ElTypeNum == HTML_EL_LINK &&
	   (LinkAsCSS || IsCSSName (targetURL)))
	 {
	   LinkAsCSS = FALSE;
	   LoadStyleSheet (targetURL, doc, element, NULL);
	   attrType.AttrTypeNum = HTML_ATTR_REL;
	   attr = TtaGetAttribute (element, attrType);
	   if (attr == 0)
	     {
	       /* create an attribute HREF for the element */
	       attr = TtaNewAttribute (attrType);
	       TtaAttachAttribute (element, attr, doc);
	       new = TRUE;
	     }
	   else
	     {
	       new = FALSE;
	       if (AttrHREFundoable)
                  TtaRegisterAttributeReplace (attr, element, doc);
	     }
	   TtaSetAttributeText (attr, TEXT("stylesheet"), element, doc);
	   if (AttrHREFundoable && new)
	       TtaRegisterAttributeCreate (attr, element, doc);

	   attrType.AttrTypeNum = HTML_ATTR_Link_type;
	   attr = TtaGetAttribute (element, attrType);
	   if (attr == 0)
	     {
	       /* create an attribute HREF for the element */
	       attr = TtaNewAttribute (attrType);
	       TtaAttachAttribute (element, attr, doc);
	       new = TRUE;
	     }
	   else
	     {
	       new = FALSE;
	       if (AttrHREFundoable)
                  TtaRegisterAttributeReplace (attr, element, doc);
	     }
	   TtaSetAttributeText (attr, TEXT("text/css"), element, doc);	   
	   if (AttrHREFundoable && new)
	       TtaRegisterAttributeCreate (attr, element, doc);
	 }
   if (AttrHREFundoable)
     {
       TtaCloseUndoSequence (doc);
       AttrHREFundoable = FALSE;
     }
   TtaSetDocumentModified (doc);
   TtaSetStatus (doc, 1, TEXT(" "), NULL);
}

/*----------------------------------------------------------------------
   SelectDestination selects the destination of the el Anchor.     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SelectDestination (Document doc, Element el, ThotBool withUndo)
#else  /* __STDC__ */
void                SelectDestination (doc, el, withUndo)
Document            doc;
Element             el;
ThotBool		    withUndo;

#endif /* __STDC__ */
{
   Element             targetEl;
   ElementType	       elType;
   Document            targetDoc;
   Attribute           attr;
   AttributeType       attrType;
   STRING              buffer;
   int                 length;
   ThotBool            isHTML;

   /* ask the user to select target document and target anchor */
   TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_SEL_TARGET), NULL);
   TtaClickElement (&targetDoc, &targetEl);
   if (targetDoc != 0)
     isHTML = !(ustrcmp (TtaGetSSchemaName (TtaGetDocumentSSchema (targetDoc)), TEXT("HTML")));
   else
     isHTML = FALSE;

   if (targetDoc != 0 && targetEl != NULL && DocumentURLs[targetDoc] != NULL)
     {
       if (isHTML)
	 {
	   /* get attrName of the enclosing end anchor */
	   attr = GetNameAttr (targetDoc, targetEl);
	   /* the document becomes the target doc */
	   SetTargetContent (targetDoc, attr);
	 }
       else
	 SetTargetContent (targetDoc, NULL);
     }
   else
     {
	targetDoc = doc;
	SetTargetContent (0, NULL);
     }

   AttrHREFelement = el;
   AttrHREFdocument = doc;
   AttrHREFundoable = withUndo;
   if (doc != targetDoc || TargetName != NULL)
     /* the user has clicked another document or a target element */
     /* create the attribute HREF or CITE */
     SetREFattribute (el, doc, TargetDocumentURL, TargetName);
   else
     /* the user has clicked the same document: pop up a dialogue box
	to allow the user to type the target URI */
     {
	TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_INVALID_TARGET), NULL);
	/* Dialogue form to insert HREF name */
#ifndef _WINDOWS 
	TtaNewForm (BaseDialog + AttrHREFForm, TtaGetViewFrame (doc, 1),  TtaGetMessage (AMAYA, AM_ATTRIBUTE), TRUE, 2, 'L', D_CANCEL);
	TtaNewTextForm (BaseDialog + AttrHREFText, BaseDialog + AttrHREFForm,
			TtaGetMessage (AMAYA, AM_HREF_VALUE), 50, 1, FALSE);
#endif /* !__WINDOWS */

	/* If the anchor has an HREF attribute, put its value in the form */
	elType = TtaGetElementType (el);
	attrType.AttrSSchema = elType.ElSSchema;
	/* search the HREF or CITE attribute */
	if (elType.ElTypeNum == HTML_EL_Quotation ||
	    elType.ElTypeNum == HTML_EL_Block_Quote ||
	    elType.ElTypeNum == HTML_EL_INS ||
	    elType.ElTypeNum == HTML_EL_DEL)
	  attrType.AttrTypeNum = HTML_ATTR_cite;
	else
	  attrType.AttrTypeNum = HTML_ATTR_HREF_;

	attr = TtaGetAttribute (el, attrType);
	if (attr != 0)
	  {
	     /* get a buffer for the attribute value */
	     length = TtaGetTextAttributeLength (attr);
	     buffer = TtaAllocString (length + 1);
	     /* copy the HREF attribute into the buffer */
	     TtaGiveTextAttributeValue (attr, buffer, &length);
	     ustrcpy (AttrHREFvalue, buffer);
#ifndef _WINDOWS
	     /* initialise the text field in the dialogue box */
	     TtaSetTextForm (BaseDialog + AttrHREFText, buffer);
#endif /* _WINDOWS */
	     TtaFreeMemory (buffer);
	  }

#ifndef _WINDOWS
	TtaShowDialogue (BaseDialog + AttrHREFForm, FALSE);
#else  /* _WINDOWS */
	CreateLinkDlgWindow (currentWindow, AttrHREFvalue, BaseDialog, AttrHREFForm, AttrHREFText);
#endif  /* _WINDOWS */
     }
}

/*----------------------------------------------------------------------
   GetNameAttr return the NAME attribute of the enclosing Anchor   
   element or the ID attribute of (an ascendant of) the selected element
   or NULL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Attribute           GetNameAttr (Document doc, Element selectedElement)
#else  /* __STDC__ */
Attribute           GetNameAttr (doc, selectedElement)
Document            doc;
Element             selectedElement;

#endif /* __STDC__ */
{
   Element             el;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;
   SSchema	       HTMLSSchema;

   attr = NULL;		/* no NAME attribute yet */
   if (selectedElement != NULL)
     {
        elType = TtaGetElementType (selectedElement);
	HTMLSSchema = TtaGetSSchema (TEXT("HTML"), doc);
	attrType.AttrSSchema = HTMLSSchema;
	if (elType.ElSSchema == HTMLSSchema &&
	    elType.ElTypeNum == HTML_EL_Anchor)
	  el = selectedElement;
	else
	  {
	    elType.ElSSchema = HTMLSSchema;
	    elType.ElTypeNum = HTML_EL_Anchor;
	    el = TtaGetTypedAncestor (selectedElement, elType);
	  }

	if (el != NULL)
	  {
	     /* the ascending Anchor element has been found */
	     /* get the NAME attribute of element Anchor */
	     attrType.AttrTypeNum = HTML_ATTR_NAME;
	     attr = TtaGetAttribute (el, attrType);
	  }
	else
	  {
	    /* no ascending Anchor element */
	    /* get the ID attribute of the selected element */
	    attrType.AttrTypeNum = HTML_ATTR_ID;
	    attr = TtaGetAttribute (selectedElement, attrType);
#ifdef MATHML
	    if (!attr)
	       {
	       attrType.AttrSSchema = TtaGetSSchema (TEXT("MathML"), doc);
	       if (attrType.AttrSSchema)
		 {
		 attrType.AttrTypeNum = MathML_ATTR_id;
		 attr = TtaGetAttribute (selectedElement, attrType);
		 }
	       }
#endif
#ifdef GRAPHML
	    if (!attr)
	       {
	       attrType.AttrSSchema = TtaGetSSchema ("GraphML", doc);
	       if (attrType.AttrSSchema)
		 {
		 attrType.AttrTypeNum = GraphML_ATTR_id;
		 attr = TtaGetAttribute (selectedElement, attrType);
		 }
	       }
#endif
	  }
     }
   return (attr);
}



/*----------------------------------------------------------------------
   CreateTargetAnchor creates a NAME or ID attribute with a default    
   value for element el.                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateTargetAnchor (Document doc, Element el, ThotBool withUndo)
#else  /* __STDC__ */
void                CreateTargetAnchor (doc, el, withUndo)
Document            doc;
Element             el;
Boolean		    withUndo;

#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           attr;
   ElementType         elType;
   Element             elText;
   SSchema	       HTMLSSchema;
   Language            lang;
   STRING              text;
   STRING               url = TtaAllocString (MAX_LENGTH);
   int                 length, i, space;
   ThotBool            found;
   ThotBool            withinHTML, new;

   elType = TtaGetElementType (el);
   withinHTML = (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")) == 0);

   /* get a NAME or ID attribute */
   HTMLSSchema = TtaGetSSchema (TEXT("HTML"), doc);
   attrType.AttrSSchema = HTMLSSchema;
   if (withinHTML && (elType.ElTypeNum == HTML_EL_Anchor ||
		      elType.ElTypeNum == HTML_EL_MAP))
     attrType.AttrTypeNum = HTML_ATTR_NAME;
   else
     {
#ifdef MATHML
     if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("MathML")) == 0)
       {
	 attrType.AttrSSchema = elType.ElSSchema;
	 attrType.AttrTypeNum = MathML_ATTR_id;
       }
     else
#endif
#ifdef GRAPHML
     if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), "GraphML") == 0)
       {
	 attrType.AttrSSchema = elType.ElSSchema;
	 attrType.AttrTypeNum = GraphML_ATTR_id;
       }
     else
#endif
       attrType.AttrTypeNum = HTML_ATTR_ID;
     }
   attr = TtaGetAttribute (el, attrType);

   if (attr == 0)
     {
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (el, attr, doc);
	new = TRUE;
     }
   else
     {
     new = FALSE;
     if (withUndo)
        TtaRegisterAttributeReplace (attr, el, doc);
     }

   /* build a value for the new attribute */
   if (withinHTML && elType.ElTypeNum == HTML_EL_MAP)
     {
       /* mapxxx for a map element */
       ustrcpy (url, TEXT("map"));
     }
   else if (withinHTML && elType.ElTypeNum == HTML_EL_LINK)
     {
       /* linkxxx for a link element */
       ustrcpy (url, TEXT("link"));
     }
   else
       /* get the content for other elements */
     {
	elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	elText = TtaSearchTypedElement (elType, SearchInTree, el);
	if (elText != NULL)
	  {
	    /* first word longer than 3 characters */
	    length = 50;
	    TtaGiveTextContent (elText, url, &length, &lang);
	    space = 0;
	    i = 0;
	    found = FALSE;
	    url[length++] = EOS;
	    while (!found && i < length)
	      {
		if (url[i] == ' ' || url[i] == EOS)
		  {
		    found = (i - space > 3 || (i != space && url[i] == EOS));
		    if (found)
		      {
			/* url = the word */
			if (i > space + 10)
			  /* limit the word length */
			  i = space + 10;
			url[i] = EOS;
			if (space != 0)
			  ustrcpy (url, &url[space]);
		      }
		    i++;
		    space = i;
		  }
		else
		  i++;
	      }

	    if (!found)
	      {
		/* label of the element */
		text = TtaGetElementLabel (el);
		ustrcpy (url, text);
	      }
	  }
	else
	  {
	    /* get the element's label if there is no text */
	    text = TtaGetElementLabel (el);
	    ustrcpy (url, text);
	  }
     }
   /* copie the text into the NAME attribute */
   TtaSetAttributeText (attr, url, el, doc);
   /* Check the attribute value to make sure that it's unique within */
   /* the document */
   MakeUniqueName (el, doc);
   /* set this new end-anchor as the new target */
   SetTargetContent (doc, attr);
   if (withUndo && new)
       TtaRegisterAttributeCreate (attr, el, doc);
   TtaFreeMemory (url);
}

/*----------------------------------------------------------------------
   CreateAnchor creates a link or target element.                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateAnchor (Document doc, View view, ThotBool createLink)
#else  /* __STDC__ */
void                CreateAnchor (doc, view, createLink)
Document            doc;
View                view;
ThotBool            createLink;

#endif /* __STDC__ */
{
  Element             first, last, el, next, parent;
  Element             parag, prev, child, anchor;
  SSchema             HTMLSSchema;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  DisplayMode         dispMode;
  int                 c1, cN, lg, i;
  ThotBool            noAnchor;

  parag = NULL;
  HTMLSSchema = TtaGetSSchema (TEXT("HTML"), doc);
  dispMode = TtaGetDisplayMode (doc);
  /* get the first and last selected element */
  TtaGiveFirstSelectedElement (doc, &first, &c1, &i);
  TtaGiveLastSelectedElement (doc, &last, &i, &cN);

  /* Check whether the selected elements are a valid content for an anchor */
  elType = TtaGetElementType (first);
  if (elType.ElTypeNum == HTML_EL_Anchor &&
      TtaSameSSchemas (elType.ElSSchema, HTMLSSchema) &&
      first == last)
    /* add an attribute on the current anchor */
    anchor = first;
  else
    {
      /* check whether the selection is within an anchor */
      if (TtaSameSSchemas (elType.ElSSchema, HTMLSSchema))
	el = SearchAnchor (doc, first, (ThotBool)(!createLink));
      else
	el = NULL;
      if (el != NULL)
	/* add an attribute on this anchor */
	anchor = el;
      else
	{
	  el = first;
	  noAnchor = FALSE;
	  
	  while (!noAnchor && el != NULL)
	    {
	      elType = TtaGetElementType (el);
	      if (!TtaSameSSchemas (elType.ElSSchema, HTMLSSchema))
		noAnchor = TRUE;
	      else if (elType.ElTypeNum != HTML_EL_TEXT_UNIT &&
		  elType.ElTypeNum != HTML_EL_Teletype_text &&
		  elType.ElTypeNum != HTML_EL_Italic_text &&
		  elType.ElTypeNum != HTML_EL_Bold_text &&
		  elType.ElTypeNum != HTML_EL_Underlined_text &&
		  elType.ElTypeNum != HTML_EL_Struck_text &&
		  elType.ElTypeNum != HTML_EL_Big_text &&
		  elType.ElTypeNum != HTML_EL_Small_text &&
		  elType.ElTypeNum != HTML_EL_Emphasis &&
		  elType.ElTypeNum != HTML_EL_Strong &&
		  elType.ElTypeNum != HTML_EL_Def &&
		  elType.ElTypeNum != HTML_EL_Code &&
		  elType.ElTypeNum != HTML_EL_Sample &&
		  elType.ElTypeNum != HTML_EL_Keyboard &&
		  elType.ElTypeNum != HTML_EL_Variable &&
		  elType.ElTypeNum != HTML_EL_Cite &&
		  elType.ElTypeNum != HTML_EL_ABBR &&
		  elType.ElTypeNum != HTML_EL_ACRONYM &&
		  elType.ElTypeNum != HTML_EL_INS &&
		  elType.ElTypeNum != HTML_EL_DEL &&
		  elType.ElTypeNum != HTML_EL_PICTURE_UNIT &&
		  elType.ElTypeNum != HTML_EL_Applet &&
		  elType.ElTypeNum != HTML_EL_Object &&
		  elType.ElTypeNum != HTML_EL_Font_ &&
		  elType.ElTypeNum != HTML_EL_SCRIPT &&
		  elType.ElTypeNum != HTML_EL_MAP &&
		  elType.ElTypeNum != HTML_EL_Quotation &&
		  elType.ElTypeNum != HTML_EL_Subscript &&
		  elType.ElTypeNum != HTML_EL_Superscript &&
		  elType.ElTypeNum != HTML_EL_Span &&
		  elType.ElTypeNum != HTML_EL_BDO &&
		  elType.ElTypeNum != HTML_EL_IFRAME )
		noAnchor = TRUE;
	      if (el == last)
		el = NULL;
	      else
		TtaGiveNextSelectedElement (doc, &el, &i, &i);
	    }
	  
	  if (noAnchor)
	    {
	      if (createLink || el != NULL)
		{
		elType = TtaGetElementType (first);
		if (first == last && c1 == 0 && cN == 0 && createLink &&
		    !TtaSameSSchemas (elType.ElSSchema, HTMLSSchema))
		   /* a single element is selected and it's not a HTML elem
		      neither a character string */
		  {
		    if (UseLastTarget)
		      /* points to the last created target */
		      SetREFattribute (el, doc, TargetDocumentURL, TargetName);
		    else
		      /* select the destination */
		      SelectDestination (doc, first, TRUE);
		  }
		else
		  /* cannot create an anchor here */
		  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_INVALID_ANCHOR1), NULL);
		}
	      else
		  /* create an ID for target element */
		{
		  TtaOpenUndoSequence (doc, first, last, c1, cN);
		  CreateTargetAnchor (doc, first, TRUE);
		  TtaCloseUndoSequence (doc);
		}
	      return;
	    }
	  /* check if the anchor to be created is within an anchor element */
	  else if (SearchAnchor (doc, first, TRUE) != NULL ||
		   SearchAnchor (doc, last, TRUE) != NULL)
	    {
	      TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_INVALID_ANCHOR2), NULL);
	      return;
	    }

	  /* stop displaying changes that will be made in the document */
	  if (dispMode == DisplayImmediately)
	    TtaSetDisplayMode (doc, DeferredDisplay);
	  /* remove selection before modifications */
	  TtaUnselect (doc);

	  TtaOpenUndoSequence (doc, first, last, c1, cN);

	  /* process the last selected element */
	  elType = TtaGetElementType (last);
	  if (cN > 1 && elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	    {
	      lg = TtaGetTextLength (last);
	      if (cN < lg)
		/* split the last text */
		{
		TtaRegisterElementReplace (last, doc);
		TtaSplitText (last, cN, doc);
		next = last;
		TtaNextSibling (&next);
		TtaRegisterElementCreate (next, doc);
		}
	    }
	  /* process the first selected element */
	  elType = TtaGetElementType (first);
	  if (c1 > 1 && elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	    {
	      /* split the first selected text element */
	      el = first;
	      TtaRegisterElementReplace (first, doc);
	      TtaSplitText (first, c1 - 1, doc);
	      TtaNextSibling (&first);
	      TtaRegisterElementCreate (first, doc);
	      if (last == el)
		{
		  /* we have to change last selection because the element was split */
		  last = first;
		}
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
	      elType.ElTypeNum == HTML_EL_Object_Content ||
	      elType.ElTypeNum == HTML_EL_Data_cell ||
	      elType.ElTypeNum == HTML_EL_Heading_cell ||
	      elType.ElTypeNum == HTML_EL_Block_Quote)
	    {
	      elType.ElTypeNum = HTML_EL_Pseudo_paragraph;
	      parag = TtaNewElement (doc, elType);
	      TtaInsertSibling (parag, last, FALSE, doc);
	      TtaInsertFirstChild (&anchor, parag, doc);
	    }
	  else
	      TtaInsertSibling (anchor, last, FALSE, doc);
	  
	  /* move the selected elements within the new Anchor element */
	  child = first;
	  prev = NULL;
	  parent = TtaGetParent (anchor);
	  while (child != NULL)
	    {
	      /* prepare the next element in the selection, as the current element */
	      /* will be moved and its successor will no longer be accessible */
	      next = child;
	      TtaNextSibling (&next);
	      /* remove the current element */
	      TtaRegisterElementDelete (child, doc);
	      TtaRemoveTree (child, doc);
	      /* insert it as a child of the new anchor element */
	      if (prev == NULL)
		TtaInsertFirstChild (&child, anchor, doc);
	      else
		TtaInsertSibling (child, prev, FALSE, doc);
	      /* get the next element in the selection */
	      prev = child;
	      if (child == last || next == parent)
		/* avoid to move the previous element or the parent
		   of the anchor into the anchor */
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
  if (createLink )
    {
      if (UseLastTarget)
	/* points to the last created target */
	SetREFattribute (anchor, doc, TargetDocumentURL, TargetName);
      else
	/* Select the destination */
	SelectDestination (doc, anchor, FALSE);
      /* The anchor element must have an HREF attribute */
      /* create an attribute PseudoClass = link */
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = HTML_ATTR_PseudoClass;
      attr = TtaGetAttribute (anchor, attrType);
      if (attr == NULL)
	{
	  attr = TtaNewAttribute (attrType);
	  TtaAttachAttribute (anchor, attr, doc);
	}
      TtaSetAttributeText (attr, TEXT("link"), anchor, doc);
    }
  else
    CreateTargetAnchor (doc, anchor, FALSE);

  if (parag)
    TtaRegisterElementCreate (parag, doc);
  else
    TtaRegisterElementCreate (anchor, doc);
  TtaCloseUndoSequence (doc);
}


/*----------------------------------------------------------------------
   MakeUniqueName
   Check attribute NAME or ID in order to make sure that its value unique
   in the document.
   If the NAME or ID is already used, add a number at the end of the value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         MakeUniqueName (Element el, Document doc)
#else  /* __STDC__ */
void         MakeUniqueName (el, doc)
Element	     el;
Document     doc;

#endif /* __STDC__ */
{
  Element	    image;
  ElementType	    elType;
  AttributeType     attrType;
  SSchema	    HTMLSSchema;
  Attribute         attr;
  STRING            value;
  CHAR_T              url[MAX_LENGTH];
  int               length, i;
  ThotBool          change, isHTML;

  HTMLSSchema = TtaGetSSchema (TEXT("HTML"), doc);
  elType = TtaGetElementType (el);
  isHTML = (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")) == 0);
  attrType.AttrSSchema = HTMLSSchema;
   if (isHTML &&
       (elType.ElTypeNum == HTML_EL_Anchor || elType.ElTypeNum == HTML_EL_MAP))
     attrType.AttrTypeNum = HTML_ATTR_NAME;
   else
     {
#ifdef MATHML
     if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("MathML")) == 0)
       {
       attrType.AttrSSchema = elType.ElSSchema;
       attrType.AttrTypeNum = MathML_ATTR_id;
       }
     else
#endif
#ifdef GRAPHML
     if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), "GraphML") == 0)
       {
       attrType.AttrSSchema = elType.ElSSchema;
       attrType.AttrTypeNum = GraphML_ATTR_id;
       }
     else
#endif
       attrType.AttrTypeNum = HTML_ATTR_ID;
     }
   attr = TtaGetAttribute (el, attrType);

   if (attr != 0)
     {
       /* the element has an attribute NAME or ID */
       length = TtaGetTextAttributeLength (attr) + 10;
       value = TtaAllocString (length);
       change = FALSE;
       if (value != NULL)
	 {
	   TtaGiveTextAttributeValue (attr, value, &length);
	   i = 0;
	   while (SearchNAMEattribute (doc, value, attr) != NULL)
	     {
	       /* Yes. Avoid duplicate NAMEs */
	       change = TRUE;
	       i++;
	       usprintf (&value[length], TEXT("%d"), i);
	     }

	   if (change)
	     {
	       /* copy the element Label into the NAME attribute */
	       TtaSetAttributeText (attr, value, el, doc);
	       if (isHTML && elType.ElTypeNum == HTML_EL_MAP)
		 {
		   /* Search backward the refered image */
		   attrType.AttrTypeNum = HTML_ATTR_USEMAP;
		   TtaSearchAttribute (attrType, SearchBackward, el, &image, &attr);
		   if (attr != NULL && image != NULL)
		     /* Search forward the refered image */
		     TtaSearchAttribute (attrType, SearchForward, el, &image, &attr);
		   if (attr != NULL && image != NULL)
		     {
		       i = MAX_LENGTH;
		       TtaGiveTextAttributeValue (attr, url, &i);
		       if (i == length+1 && ustrncmp (&url[1], value, length) == 0)
			 {
			   /* Change the USEMAP of the image */
			   attr = TtaGetAttribute (image, attrType);
			   ustrcpy (&url[1], value);
			   TtaSetAttributeText (attr, url, image, doc);
			 }
		     }
		 }
	     }
	 }
       TtaFreeMemory (value);
     }
}


/*----------------------------------------------------------------------
   CheckPseudoParagraph
   Element el has been created or pasted. If its a Pseudo_paragraph,
   it is turned into an ordinary Paragraph if it's not the first child
   of its parent.
   If the next sibiling is a Pseudo_paragraph, this sibling is turned into
   an ordinary Paragraph.
   Rule: only the first child of any element can be a Pseudo_paragraph.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CheckPseudoParagraph (Element el, Document doc)
#else  /* __STDC__ */
static void         CheckPseudoParagraph (el, doc)
Element		el;
Document	doc;

#endif /* __STDC__ */
{
  Element		prev, next, parent;
  Attribute             attr;
  ElementType		elType;
  
  elType = TtaGetElementType (el);
  if (!TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema (TEXT("HTML"), doc)))
    /* it's not an HTML element */
    return;

  if (elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
    /* the element is a Pseudo_paragraph */
    {
      prev = el;
      TtaPreviousSibling (&prev);
      attr = NULL;
      TtaNextAttribute (el, &attr);
      if (prev || attr)
        /* the Pseudo-paragraph is not the first element among its sibling */
        /* or it has attributes: turn it into an ordinary paragraph */
	{
	TtaRemoveTree (el, doc);
        ChangeElementType (el, HTML_EL_Paragraph);
	TtaInsertSibling (el, prev, FALSE, doc);
	}
    }
  else if (elType.ElTypeNum == HTML_EL_Paragraph)
    /* the element is a Paragraph */
    {
      prev = el;
      TtaPreviousSibling (&prev);
      attr = NULL;
      TtaNextAttribute (el, &attr);
      if (prev == NULL && attr == NULL)
        /* the Paragraph is the first element among its sibling and it has
	   no attribute */
        /* turn it into an Pseudo-paragraph if it's in a List_item or a
	   table cell. */
         {
	 parent = TtaGetParent (el);
	 if (parent)
	    {
	    elType = TtaGetElementType (parent);
	    if (elType.ElTypeNum == HTML_EL_List_Item ||
		elType.ElTypeNum == HTML_EL_Definition ||
		elType.ElTypeNum == HTML_EL_Data_cell ||
		elType.ElTypeNum == HTML_EL_Heading_cell)
		{
		TtaRemoveTree (el, doc);
		ChangeElementType (el, HTML_EL_Pseudo_paragraph);
		TtaInsertFirstChild (&el, parent, doc);
		}
	    }
	 }
    }
  next = el;
  TtaNextSibling (&next);
  if (next)
    {
      elType = TtaGetElementType (next);
      if (elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
	/* the next element is a Pseudo-paragraph */
	/* turn it into an ordinary paragraph */
	{
	  TtaRegisterElementReplace (next, doc);
	  TtaRemoveTree (next, doc);
	  ChangeElementType (next, HTML_EL_Paragraph);
	  TtaInsertSibling (next, el, FALSE, doc);
	}
    }
}

/*----------------------------------------------------------------------
   ElementCreated
   An element has been created in a HTML document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ElementCreated (NotifyElement * event)
#else  /* __STDC__ */
void                ElementCreated (event)
NotifyElement      *event;

#endif /* __STDC__ */
{
  CheckPseudoParagraph (event->element, event->document);
}

/*----------------------------------------------------------------------
 ElementDeleted
 An element has been deleted. If it was the only child of element
 BODY, create a first paragraph.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void ElementDeleted (NotifyElement *event)
#else /* __STDC__*/
void ElementDeleted(event)
     NotifyElement *event;
#endif /* __STDC__*/
{
  Element	child, el;
  ElementType	elType, childType;
  ThotBool	empty;

  elType = TtaGetElementType (event->element);
  if (elType.ElTypeNum == HTML_EL_BODY)
     {
     child = TtaGetFirstChild (event->element);
     empty = TRUE;
     while (empty && child)
	{
        elType = TtaGetElementType (child);
	if (elType.ElTypeNum != HTML_EL_Comment_ &&
	    elType.ElTypeNum != HTML_EL_Invalid_element)
	   empty = FALSE;
        else
	   TtaNextSibling (&child);
	}
     if (empty)
	{
	elType.ElTypeNum = HTML_EL_Paragraph;
	child = TtaNewTree (event->document, elType, _EMPTYSTR_);
	TtaInsertFirstChild (&child, event->element, event->document);
	TtaRegisterElementCreate (child, event->document);
	do
	   {
	   el = TtaGetFirstChild (child);
	   if (el)
	      child = el;
	   }
	while (el);
	TtaSelectElement (event->document, child);
	}
     }
   /* if the deleted element was the first child of a LI, transform
      the new first child into a Pseudo-Paragraph if it's a Paragraph */
   else if (elType.ElTypeNum == HTML_EL_List_Item)
     /* the parent element is a List_Item */
     if (event->position == 0)
        /* the deleted element was the first child */
        {
        child = TtaGetFirstChild (event->element);
        if (child)
	   {
	   childType = TtaGetElementType (child);
	   if (childType.ElTypeNum == HTML_EL_Paragraph)
	      /* the new first child is a Paragraph */
	      {
	      TtaRegisterElementReplace (child, event->document);
	      TtaRemoveTree (child, event->document);
	      ChangeElementType (child, HTML_EL_Pseudo_paragraph);
	      TtaInsertFirstChild (&child, event->element, event->document);
	      }
	   }
        }
}

/*----------------------------------------------------------------------
   ElementPasted
   An element has been pasted in a HTML document.
   Check Pseudo paragraphs.
   If the pasted element has a NAME attribute, change its value if this
   NAME is already used in the document.
   If it's within the TITLE element, update the corresponding field in
   the Formatted window.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ElementPasted (NotifyElement * event)
#else  /* __STDC__ */
void                ElementPasted (event)
NotifyElement      *event;

#endif /* __STDC__ */
{
  Document            originDocument, doc;
  Language            lang;
  Element             el, anchor, next, child, previous, nextchild, parent;
  ElementType         elType, parentType;
  AttributeType       attrType;
  Attribute           attr;
  SSchema             HTMLschema;
  CSSInfoPtr          css;
  STRING              value, base;
  STRING              documentURL;
  STRING              tempURL;
  STRING              path;
  int                 length, i, iName;
  int                 oldStructureChecking;

  el = event->element;
  doc = event->document;
  HTMLschema = TtaGetSSchema (TEXT("HTML"), doc);
  CheckPseudoParagraph (el, doc);
  elType = TtaGetElementType (el);
  anchor = NULL;
  if (elType.ElSSchema == HTMLschema && elType.ElTypeNum == HTML_EL_Anchor)
      anchor = el;
  else if (elType.ElSSchema == HTMLschema && elType.ElTypeNum == HTML_EL_STYLE_)
    {
      /* The pasted element is a STYLE element in the HEAD.
         Read the text in a buffer */
      child = TtaGetFirstChild (el);
      length = TtaGetTextLength (child);
      value = TtaAllocString (length + 1);
      TtaGiveTextContent (child, value, &length, &lang);
      css = AddCSS (doc, doc, CSS_DOCUMENT_STYLE, NULL, NULL);
      ReadCSSRules (doc, css, value, FALSE);
      TtaFreeMemory (value);
    }
  else if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
    {  
      parent = TtaGetParent (event->element);
      parentType = TtaGetElementType (parent);
      if (TtaSameSSchemas (parentType.ElSSchema, HTMLschema) &&
          parentType.ElTypeNum == HTML_EL_TITLE)
         /* the parent of the pasted text is a TITLE */
         /* That's probably the result of undoing a change in the TITLE */
         UpdateTitle (parent, doc);
    }
  else if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
    {
      originDocument = (Document) event->position;
      if (originDocument > 0)
	{
	  /* remove USEMAP attribute */
	  attrType.AttrSSchema = elType.ElSSchema;
	  attrType.AttrTypeNum = HTML_ATTR_USEMAP;
	  attr = TtaGetAttribute (el, attrType);
	  if (attr != NULL)
	    TtaRemoveAttribute (el, attr, doc);
	  /* Change attributes SRC if the element comes from another */
	  /* document */
	  if (originDocument != doc)
	    {
	      /* the image has moved from one document to another */
	      /* get the SRC attribute of element IMAGE */
	      attrType.AttrTypeNum = HTML_ATTR_SRC;
	      attr = TtaGetAttribute (el, attrType);
	      if (attr != NULL)
		{
		  /* get a buffer for the SRC */
		  length = TtaGetTextAttributeLength (attr);
		  if (length > 0)
		    {
		      value = TtaAllocString (MAX_LENGTH);
			if (value != NULL)
			  {
			    /* get the SRC itself */
			    TtaGiveTextAttributeValue (attr, value, &length);
			    /* update value and SRCattribute */
			    ComputeSRCattribute (el, doc, originDocument, attr, value);
			  }
			TtaFreeMemory (value);
		      }
		  }
	      }
	  }
     }
  else
    {
      /* Check attribute NAME or ID in order to make sure that its value */
      /* unique in the document */
      MakeUniqueName (el, doc);
      elType.ElSSchema = HTMLschema;
      elType.ElTypeNum = HTML_EL_Anchor;
      anchor = TtaSearchTypedElement (elType, SearchInTree, el);
    }

  if (anchor != NULL)
    {
      tempURL = TtaAllocString (MAX_LENGTH);
      documentURL = TtaAllocString (MAX_LENGTH);
      path = TtaAllocString (MAX_LENGTH);
      TtaSetDisplayMode (doc, DeferredDisplay);
      oldStructureChecking = TtaGetStructureChecking (doc);
      TtaSetStructureChecking (0, doc);
      /* Is there a parent anchor */
      parent = TtaGetTypedAncestor (el, elType);
      while (anchor != NULL)
	{
	  /* look for the next pasted anchor */
	  if (anchor != el)
	    next = TtaSearchTypedElementInTree (elType, SearchForward, el, anchor);
	  else
	    next = NULL;
	  
	  if (parent != NULL)
	    {
	      /* Move anchor children and delete the anchor element */
	      child = TtaGetFirstChild (anchor);
	      previous = child;
	      TtaPreviousSibling (&previous);
	      
	      while (child != NULL)
		{
		  nextchild = child;
		  TtaNextSibling (&nextchild);
		  TtaRemoveTree (child, doc);
		  TtaInsertSibling (child, anchor, TRUE, doc);
		  /* if anchor is the pasted element, it has been registered
		     in the editing history for the Undo command.  It will be
		     deleted, so its children have to be registered too. */
		  if (anchor == el)
		     TtaRegisterElementCreate (child, doc);
		  child = nextchild;
		}
	      TtaDeleteTree (anchor, doc);
	    }
	  else
	    {
	      /* Check attribute NAME in order to make sure its value unique */
	      /* in the document */
	      MakeUniqueName (anchor, doc);
	      /* Change attributes HREF if the element comes from another */
	      /* document */
	      originDocument = (Document) event->position;
	      if (originDocument > 0 && originDocument != doc)
		{
		  /* the anchor has moved from one document to another */
		  /* get the HREF attribute of element Anchor */
		  attrType.AttrSSchema = elType.ElSSchema;
		  attrType.AttrTypeNum = HTML_ATTR_HREF_;
		  attr = TtaGetAttribute (anchor, attrType);
		  if (attr != NULL)
		    {
		      /* get a buffer for the URL */
		      length = TtaGetTextAttributeLength (attr) + 1;
		      value = TtaAllocString (length);
		      if (value != NULL)
			{
			  /* get the URL itself */
			  TtaGiveTextAttributeValue (attr, value, &length);
			  if (value[0] == '#')
			    {
			      /* the target element is local in the document */
			      /* origin convert internal link into external link */
			      ustrcpy (tempURL, DocumentURLs[originDocument]);
			      iName = 0;
			    }
			  else
			    {
			      /* the target element is in another document */
			      ustrcpy (documentURL, value);
			      /* looks for a '#' in the value */
			      i = length;
			      while (value[i] != '#' && i > 0)
				i--;
			      if (i == 0)
				{
				  /* there is no '#' in the URL */
				  value[0] = EOS;
				  iName = 0;
				}
			      else
				{
				  /* there is a '#' character in the URL */
				  /* separate document name and element name */
				  documentURL[i] = EOS;
				  iName = i;
				}
			      /* get the complete URL of the referred document */
			      /* Add the  base content if necessary */
			      NormalizeURL (documentURL, originDocument, tempURL, path, NULL);
			    }
			  if (value[iName] == '#')
			    {
			      if (!ustrcmp (tempURL, DocumentURLs[doc]))
				/* convert external link into internal link */
				ustrcpy (tempURL, &value[iName]);
			      else
				ustrcat (tempURL, &value[iName]);
			    }
			  TtaFreeMemory (value);
			  /* set the relative value or URL in attribute HREF */
			  base = GetBaseURL (doc);
			  value = MakeRelativeURL (tempURL, base);
			  TtaSetAttributeText (attr, value, anchor, doc);
			  TtaFreeMemory (base);
			  TtaFreeMemory (value);
			}
		    }
		}
	    }
	  anchor = next;
	}
      TtaFreeMemory (path);
      TtaFreeMemory (documentURL);
      TtaFreeMemory (tempURL);
      TtaSetStructureChecking ((ThotBool)oldStructureChecking, doc);
      TtaSetDisplayMode (doc, DisplayImmediately);
    }
}


/*----------------------------------------------------------------------
   CreateTarget creates a target element.                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateTarget (Document doc, View view)
#else  /* __STDC__ */
void                CreateTarget (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateAnchor (doc, view, FALSE);
}


/*----------------------------------------------------------------------
   UpdateAttrID
   An ID attribute has been created, modified or deleted.
   If it's a creation or modification, check that the ID is a unique name
   in the document.
   If it's a deletion for a SPAN element, remove that element if it's
   not needed.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateAttrID (NotifyAttribute * event)
#else  /* __STDC__ */
void                UpdateAttrID (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
   Element	firstChild, lastChild;

   if (event->event == TteAttrDelete)
      /* if the element is a SPAN without any other attribute, remove the SPAN
         element */
      DeleteSpanIfNoAttr (event->element, event->document, &firstChild, &lastChild);
   else
      {
      MakeUniqueName (event->element, event->document);
      if (event->event == TteAttrCreate)
         /* if the ID attribute is on a text string, create a SPAN element that
         encloses this text string and move the ID attribute to that SPAN
         element */
         AttrToSpan (event->element, event->attribute, event->document);
      }
}


/*----------------------------------------------------------------------
   CoordsModified  updates x_ccord, y_coord, width, height or      
   polyline according to the new coords value.             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CoordsModified (NotifyAttribute * event)
#else  /* __STDC__ */
void                CoordsModified (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
   ParseAreaCoords (event->element, event->document);
}


/*----------------------------------------------------------------------
   GraphicsModified        updates coords attribute value          
   according to the new coord value.                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GraphicsModified (NotifyAttribute * event)
#else  /* __STDC__ */
void                GraphicsModified (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
   Element             el;
   ElementType         elType;
   AttributeType       attrType;
   Attribute	       attr;
   CHAR_T		       buffer[15];

   el = event->element;
   elType = TtaGetElementType (el);
   if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
     {
       /* update the associated map */
       if (event->attributeType.AttrTypeNum == HTML_ATTR_IntWidthPxl)
	 {
	   UpdateImageMap (el, event->document, OldWidth, -1);
	   OldWidth = -1;
	   /* update attribute Width__ */
	   attrType.AttrSSchema = event->attributeType.AttrSSchema;
	   attrType.AttrTypeNum = HTML_ATTR_Width__;
	   attr = TtaGetAttribute (el, attrType);
	   if (attr)
	      {
	      usprintf (buffer, TEXT("%d"), TtaGetAttributeValue (event->attribute));
	      TtaSetAttributeText (attr, buffer, el, event->document);
	      }
	 }
       else
	 {
	   UpdateImageMap (el, event->document, -1, OldHeight);
	   OldHeight = -1;
	 }
     }
   else
     {
       if (elType.ElTypeNum != HTML_EL_AREA)
	 {
	 el = TtaGetParent (el);
	 elType = TtaGetElementType (el);
	 }
       if (elType.ElTypeNum == HTML_EL_AREA)
	 SetAreaCoords (event->document, el, event->attributeType.AttrTypeNum);
     }
}

/*----------------------------------------------------------------------
   StoreWidth IntWidthPxl will be changed, store the old value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            StoreWidth (NotifyAttribute * event)
#else  /* __STDC__ */
ThotBool            StoreWidth (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
  ElementType	     elType;
  int                h;

  elType = TtaGetElementType (event->element);
  if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
    TtaGiveBoxSize (event->element, event->document, 1, UnPixel, &OldWidth, &h);
  else
    OldWidth = -1;
  return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   StoreHeight height_ will be changed, store the old value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            StoreHeight (NotifyAttribute * event)
#else  /* __STDC__ */
ThotBool            StoreHeight (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
  ElementType	     elType;
  int                w;

  elType = TtaGetElementType (event->element);
  if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
    TtaGiveBoxSize (event->element, event->document, 1, UnPixel, &w, &OldHeight);
  else
     OldHeight = -1;
   return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   AttrWidthDelete         An attribute Width__ will be deleted.   
   Delete the corresponding attribute IntWidthPercent or   
   IntWidthPxl.                                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            AttrWidthDelete (NotifyAttribute * event)
#else  /* __STDC__ */
ThotBool            AttrWidthDelete (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           attr;

   StoreHeight (event);
   attrType = event->attributeType;
   attrType.AttrTypeNum = HTML_ATTR_IntWidthPxl;
   attr = TtaGetAttribute (event->element, attrType);
   if (attr == NULL)
     {
	attrType.AttrTypeNum = HTML_ATTR_IntWidthPercent;
	attr = TtaGetAttribute (event->element, attrType);
     }
   if (attr != NULL)
      TtaRemoveAttribute (event->element, attr, event->document);
   return FALSE;		/* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
   AttrWidthModifed  An attribute Width__ has been created or modified.
   Create the corresponding attribute IntWidthPercent or IntWidthPxl.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AttrWidthModified (NotifyAttribute * event)
#else  /* __STDC__ */
void                AttrWidthModifed (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
  STRING              buffer;
  int                 length;
   length = buflen - 1;
   buffer = TtaAllocString (buflen);
   TtaGiveTextAttributeValue (event->attribute, buffer, &length);
   CreateAttrWidthPercentPxl (buffer, event->element, event->document, OldWidth);
   TtaFreeMemory (buffer);
   OldWidth = -1;
}

/*----------------------------------------------------------------------
   an HTML attribute "size" has been created for a Font element.   
   Create the corresponding internal attribute.                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AttrFontSizeCreated (NotifyAttribute * event)
#else  /* __STDC__ */
void                AttrFontSizeCreated (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
   STRING               buffer = TtaAllocString (buflen);
   int                 length;
   DisplayMode         dispMode;

   dispMode = TtaGetDisplayMode (event->document);
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (event->document, DeferredDisplay);

   length = buflen - 1;
   TtaGiveTextAttributeValue (event->attribute, buffer, &length);
   CreateAttrIntSize (buffer, event->element, event->document);
   TtaSetDisplayMode (event->document, dispMode);
   TtaFreeMemory (buffer);
}

/*----------------------------------------------------------------------
   an HTML attribute "size" has been deleted for a Font element.   
   Delete the corresponding internal attribute.                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            AttrFontSizeDelete (NotifyAttribute * event)
#else  /* __STDC__ */
ThotBool            AttrFontSizeDelete (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           attr;

   attrType = event->attributeType;
   attrType.AttrTypeNum = HTML_ATTR_IntSizeIncr;
   attr = TtaGetAttribute (event->element, attrType);
   if (attr == NULL)
     {
	attrType.AttrTypeNum = HTML_ATTR_IntSizeDecr;
	attr = TtaGetAttribute (event->element, attrType);
     }
   if (attr == NULL)
     {
	attrType.AttrTypeNum = HTML_ATTR_IntSizeRel;
	attr = TtaGetAttribute (event->element, attrType);
     }
   if (attr != NULL)
      TtaRemoveAttribute (event->element, attr, event->document);
   return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   an attribute color, TextColor or BackgroundColor has been       
   created or modified.                                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AttrColorCreated (NotifyAttribute * event)
#else  /* __STDC__ */
void                AttrColorCreated (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
   STRING           value = TtaAllocString (buflen);
   int              length;

   value[0] = EOS;
   length = TtaGetTextAttributeLength (event->attribute);
   if (length >= buflen)
      length = buflen - 1;
   if (length > 0)
      TtaGiveTextAttributeValue (event->attribute, value, &length);

   if (event->attributeType.AttrTypeNum == HTML_ATTR_BackgroundColor)
      HTMLSetBackgroundColor (event->document, event->element, value);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_background_)
      HTMLSetBackgroundImage (event->document, event->element, STYLE_REPEAT, value);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_color ||
	    event->attributeType.AttrTypeNum == HTML_ATTR_TextColor)
      HTMLSetForegroundColor (event->document, event->element, value);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_LinkColor)
      HTMLSetAlinkColor (event->document, value);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_VisitedLinkColor)
      HTMLSetAvisitedColor (event->document, value);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_ActiveLinkColor)
      HTMLSetAactiveColor (event->document, value);
   TtaFreeMemory (value);
}


/*----------------------------------------------------------------------
   an attribute color, TextColor or BackgroundColor is being       
   deleted.                                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            AttrColorDelete (NotifyAttribute * event)
#else  /* __STDC__ */
ThotBool            AttrColorDelete (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
   if (event->attributeType.AttrTypeNum == HTML_ATTR_BackgroundColor)
      HTMLResetBackgroundColor (event->document, event->element);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_background_)
      HTMLResetBackgroundImage (event->document, event->element);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_color ||
	    event->attributeType.AttrTypeNum == HTML_ATTR_TextColor)
      HTMLResetForegroundColor (event->document, event->element);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_LinkColor)
      HTMLResetAlinkColor (event->document);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_VisitedLinkColor)
      HTMLResetAvisitedColor (event->document);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_ActiveLinkColor)
      HTMLResetAactiveColor (event->document);
   return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   An element List_Item has been created or pasted. Set its        
   IntItemStyle attribute according to its surrounding elements.   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ListItemCreated (NotifyElement * event)
#else  /* __STDC__ */
void                ListItemCreated (event)
NotifyElement      *event;

#endif /* __STDC__ */
{
   SetAttrIntItemStyle (event->element, event->document);
}

/*----------------------------------------------------------------------
   Set the IntItemStyle attribute of all List_Item elements in the 
   el subtree.                                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetItemStyleSubtree (Element el, Document doc)
#else  /* __STDC__ */
static void         SetItemStyleSubtree (el, doc)
Element             el;
Document            doc;

#endif /* __STDC__ */
{
   ElementType         elType;
   Element             child;

   elType = TtaGetElementType (el);
   if (elType.ElTypeNum == HTML_EL_List_Item)
      SetAttrIntItemStyle (el, doc);
   child = TtaGetFirstChild (el);
   while (child != NULL)
     {
	SetItemStyleSubtree (child, doc);
	TtaNextSibling (&child);
     }
}

/*----------------------------------------------------------------------
   An element Unnumbered_List or Numbered_List has changed type.   
   Set the IntItemStyle attribute for all enclosed List_Items      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ListChangedType (NotifyElement * event)
#else  /* __STDC__ */
void                ListChangedType (event)
NotifyElement      *event;

#endif /* __STDC__ */
{
   SetItemStyleSubtree (event->element, event->document);
}

/*----------------------------------------------------------------------
   An attribute BulletStyle or NumberStyle has been created,       
   deleted or modified for a list. Create or updated the           
   corresponding IntItemStyle attribute for all items of the list. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateAttrIntItemStyle (NotifyAttribute * event)
#else  /* __STDC__ */
void                UpdateAttrIntItemStyle (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
   Element             child;

   child = TtaGetFirstChild (event->element);
   while (child != NULL)
     {
	SetAttrIntItemStyle (child, event->document);
	TtaNextSibling (&child);
     }
}

/*----------------------------------------------------------------------
   An attribute ItemStyle has been created, updated or deleted.    
   Create or update the corresponding IntItemStyle attribute.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AttrItemStyle (NotifyAttribute * event)
#else  /* __STDC__ */
void                AttrItemStyle (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
   Element             el;

   el = event->element;
   while (el != NULL)
     {
	SetAttrIntItemStyle (el, event->document);
	TtaNextSibling (&el);
     }
}

/*----------------------------------------------------------------------
   GlobalAttrInMenu
   Called by Thot when building the Attribute menu.
   Prevent Thot from including a global attribute in the menu if the selected
   element do not accept this attribute.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            GlobalAttrInMenu (NotifyAttribute * event)
#else  /* __STDC__ */
ThotBool            GlobalAttrInMenu (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
   ElementType         elType;
   SSchema	       HTMLSSchema;

   HTMLSSchema = TtaGetSSchema (TEXT("HTML"), event->document);
   elType = TtaGetElementType (event->element);
   if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")))
     /* it's not a HTML document */
      return TRUE;	/* don't put any HTML attribute in the menu */

   if (!TtaSameSSchemas (elType.ElSSchema, HTMLSSchema))
      /* it's not a HTML element */
      return TRUE;	/* don't put any HTML attribute in the menu */

   /* BASE and SCRIPT do not accept any global attribute */
   if (elType.ElTypeNum == HTML_EL_BASE ||
       elType.ElTypeNum == HTML_EL_SCRIPT)
      return TRUE;

   /* BASEFONT and PARAM accept only ID */
   if (elType.ElTypeNum == HTML_EL_BaseFont ||
       elType.ElTypeNum == HTML_EL_Parameter)
      return (event->attributeType.AttrTypeNum != HTML_ATTR_ID);

   /* coreattrs */
   if (event->attributeType.AttrTypeNum == HTML_ATTR_ID ||
       event->attributeType.AttrTypeNum == HTML_ATTR_Class ||
       event->attributeType.AttrTypeNum == HTML_ATTR_Style_ ||
       event->attributeType.AttrTypeNum == HTML_ATTR_Title)
      if (elType.ElTypeNum == HTML_EL_HEAD ||
	  elType.ElTypeNum == HTML_EL_TITLE ||
	  elType.ElTypeNum == HTML_EL_META ||
	  elType.ElTypeNum == HTML_EL_STYLE_ ||
	  elType.ElTypeNum == HTML_EL_HTML)
	 return TRUE;
      else
	 return FALSE;
   /* i18n */
   if (event->attributeType.AttrTypeNum == HTML_ATTR_dir ||
       event->attributeType.AttrTypeNum == HTML_ATTR_Langue)
     if (elType.ElTypeNum == HTML_EL_BR ||
	 elType.ElTypeNum == HTML_EL_Applet ||
	 elType.ElTypeNum == HTML_EL_Horizontal_Rule ||
	 elType.ElTypeNum == HTML_EL_FRAMESET ||
	 elType.ElTypeNum == HTML_EL_FRAME ||
	 elType.ElTypeNum == HTML_EL_IFRAME)
	return TRUE;
     else
	return FALSE;
   /* events */
   if (event->attributeType.AttrTypeNum == HTML_ATTR_onclick ||
       event->attributeType.AttrTypeNum == HTML_ATTR_ondblclick ||
       event->attributeType.AttrTypeNum == HTML_ATTR_onmousedown ||
       event->attributeType.AttrTypeNum == HTML_ATTR_onmouseup ||
       event->attributeType.AttrTypeNum == HTML_ATTR_onmouseover ||
       event->attributeType.AttrTypeNum == HTML_ATTR_onmousemove ||
       event->attributeType.AttrTypeNum == HTML_ATTR_onmouseout ||
       event->attributeType.AttrTypeNum == HTML_ATTR_onkeypress ||
       event->attributeType.AttrTypeNum == HTML_ATTR_onkeydown ||
       event->attributeType.AttrTypeNum == HTML_ATTR_onkeyup)
     if (elType.ElTypeNum == HTML_EL_BDO ||
	 elType.ElTypeNum == HTML_EL_Font_ ||
	 elType.ElTypeNum == HTML_EL_BR ||
	 elType.ElTypeNum == HTML_EL_Applet ||
	 elType.ElTypeNum == HTML_EL_FRAMESET ||
	 elType.ElTypeNum == HTML_EL_FRAME ||
	 elType.ElTypeNum == HTML_EL_IFRAME ||
	 elType.ElTypeNum == HTML_EL_HEAD ||
	 elType.ElTypeNum == HTML_EL_TITLE ||
	 elType.ElTypeNum == HTML_EL_META ||
	 elType.ElTypeNum == HTML_EL_STYLE_ ||
	 elType.ElTypeNum == HTML_EL_HTML ||
	 elType.ElTypeNum == HTML_EL_ISINDEX)
	return TRUE;

   return FALSE;
}

/*----------------------------------------------------------------------
   AttrNAMEinMenu
   doesn't display NAME in Reset_Input and Submit_Input
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            AttrNAMEinMenu (NotifyAttribute * event)
#else  /* __STDC__ */
ThotBool            AttrNAMEinMenu (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           attr;
   ElementType         elType;

   elType = TtaGetElementType (event->element);
   if (elType.ElTypeNum == HTML_EL_Reset_Input ||
       elType.ElTypeNum == HTML_EL_Submit_Input)
      /* Attribute menu for an element Reset_Input or Submit_Input */
      /* prevent Thot from including an entry for that attribute */
      return TRUE;
   else if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
     {
       /* check if it's an input element */
       attrType.AttrSSchema = elType.ElSSchema;
       attrType.AttrTypeNum = HTML_ATTR_IsInput;
       attr = TtaGetAttribute (event->element, attrType);
       if (attr)
	 return FALSE;		/* let Thot perform normal operation */
       else
	 return TRUE;		/* not allowed on standard pictures */
     }
   else
      return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetOnOffEmphasis (Document document, View view)
#else  /* __STDC__ */
void                SetOnOffEmphasis (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
  SetCharFontOrPhrase (document, HTML_EL_Emphasis);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetOnOffStrong (Document document, View view)
#else  /* __STDC__ */
void                SetOnOffStrong (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
  SetCharFontOrPhrase (document, HTML_EL_Strong);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetOnOffCite (Document document, View view)
#else  /* __STDC__ */
void                SetOnOffCite (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
  SetCharFontOrPhrase (document, HTML_EL_Cite);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetOnOffDefinition (Document document, View view)
#else  /* __STDC__ */
void                SetOnOffDefinition (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
  SetCharFontOrPhrase (document, HTML_EL_Def);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetOnOffCode (Document document, View view)
#else  /* __STDC__ */
void                SetOnOffCode (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Code);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetOnOffVariable (Document document, View view)
#else  /* __STDC__ */
void                SetOnOffVariable (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Variable);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetOnOffSample (Document document, View view)
#else  /* __STDC__ */
void                SetOnOffSample (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Sample);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetOnOffKeyboard (Document document, View view)
#else  /* __STDC__ */
void                SetOnOffKeyboard (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Keyboard);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetOnOffAbbr (Document document, View view)
#else  /* __STDC__ */
void                SetOnOffAbbr (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_ABBR);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetOnOffAcronym (Document document, View view)
#else  /* __STDC__ */
void                SetOnOffAcronym (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_ACRONYM);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetOnOffINS (Document document, View view)
#else  /* __STDC__ */
void                SetOnOffINS (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_INS);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetOnOffDEL (Document document, View view)
#else  /* __STDC__ */
void                SetOnOffDEL (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_DEL);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetOnOffItalic (Document document, View view)
#else  /* __STDC__ */
void                SetOnOffItalic (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Italic_text);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetOnOffBold (Document document, View view)
#else  /* __STDC__ */
void                SetOnOffBold (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Bold_text);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetOnOffTeletype (Document document, View view)
#else  /* __STDC__ */
void                SetOnOffTeletype (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Teletype_text);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetOnOffBig (Document document, View view)
#else  /* __STDC__ */
void                SetOnOffBig (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Big_text);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetOnOffSmall (Document document, View view)
#else  /* __STDC__ */
void                SetOnOffSmall (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Small_text);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetOnOffSub (Document document, View view)
#else  /* __STDC__ */
void                SetOnOffSub (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Subscript);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetOnOffSup (Document document, View view)
#else  /* __STDC__ */
void                SetOnOffSup (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Superscript);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetOnOffQuotation (Document document, View view)
#else  /* __STDC__ */
void                SetOnOffQuotation (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Quotation);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetOnOffBDO (Document document, View view)
#else  /* __STDC__ */
void                SetOnOffBDO (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_BDO);
}


/*----------------------------------------------------------------------
   SearchAnchor return the enclosing Anchor element with an        
   HREF attribute if link is TRUE or an NAME attribute.    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Element             SearchAnchor (Document doc, Element element, ThotBool link)
#else  /* __STDC__ */
Element             SearchAnchor (doc, element, link)
Document            doc;
Element             element;
boolaen             link;

#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           attr;
   ElementType         elType;
   Element             elAnchor;
   int                 typeNum;
   SSchema             HTMLschema;

   attr = NULL;
   elType = TtaGetElementType (element);
   if (link && elType.ElTypeNum == HTML_EL_GRAPHICS_UNIT)
      /* search an ancestor of type AREA */
      typeNum = HTML_EL_AREA;
   else
      /* search an ancestor of type Anchor */
      typeNum = HTML_EL_Anchor;

   HTMLschema = TtaGetSSchema (TEXT("HTML"), doc);
   if (elType.ElTypeNum == typeNum && elType.ElSSchema == HTMLschema)
      elAnchor = element;
   else
     {
	elType.ElTypeNum = typeNum;
	elType.ElSSchema = HTMLschema;
	elAnchor = TtaGetTypedAncestor (element, elType);
     }

   attrType.AttrSSchema = elType.ElSSchema;
   if (link)
      attrType.AttrTypeNum = HTML_ATTR_HREF_;
   else
      attrType.AttrTypeNum = HTML_ATTR_NAME;

   while (elAnchor != NULL && attr == NULL)
     {
	/* get the attribute of element Anchor */
	attr = TtaGetAttribute (elAnchor, attrType);
	if (attr == NULL)
	   elAnchor = TtaGetTypedAncestor (elAnchor, elType);
     }
   return elAnchor;
}

/*----------------------------------------------------------------------
   UpdateAtom : on X-Windows, update the content of atom           
   BROWSER_HISTORY_INFO with title and url of current doc  
   c.f: http://zenon.inria.fr/koala/colas/browser-history/       
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                UpdateAtom (Document doc, STRING url, STRING title)
#else  /* __STDC__ */
void                UpdateAtom (doc, url, title)
Document            doc;
STRING              url;
STRING              title;

#endif /* __STDC__ */
{
#ifndef _GTK
#ifndef _WINDOWS
   STRING              v;
   int                 v_size;
   ThotWidget	       frame;
   static Atom         property_name = 0;
   Display            *dpy = TtaGetCurrentDisplay ();
   ThotWindow          win;

   frame = TtaGetViewFrame (doc, 1);
   if (frame == 0)
      return;
   win = XtWindow (XtParent (XtParent (XtParent (frame))));
   /* 13 is ustrlen("URL=0TITLE=00") */
   v_size = ustrlen (title) + ustrlen (url) + 13;
   v = TtaAllocString (v_size);
   sprintf (v, "URL=%s%cTITLE=%s%c", url, 0, title, 0);
   if (!property_name)
      property_name = XInternAtom (dpy, "BROWSER_HISTORY_INFO", FALSE);
   XChangeProperty (dpy, win, property_name, XA_STRING, 8, PropModeReplace,
		    v, v_size);
   TtaFreeMemory (v);
#endif /* !_WINDOWS */
#endif /* !_GTK */
}

/*----------------------------------------------------------------------
   The user has modified the contents of element TITLE. Update the    
   the Title field on top of the window.                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TitleModified (NotifyOnTarget * event)
#else  /* __STDC__ */
void                TitleModified (event)
NotifyOnTarget     *event;

#endif /* __STDC__ */
{
   UpdateTitle (event->element, event->document);
}
