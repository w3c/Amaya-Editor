/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Amaya editing functions called form Thot and declared in HTML.A.
 * These functions concern links and other HTML general features.
 *
 * Author: I. Vatton
 *
 */

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "presentdriver.h"

static char        *TargetDocumentURL = NULL;
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
#include "HTMLstyle_f.h"
#include "tree.h"

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
   length = strlen (DocumentURLs[doc]);
   TargetDocumentURL = TtaGetMemory (length + 1);
   strcpy (TargetDocumentURL, DocumentURLs[doc]);

   if (TargetName != NULL)
     {
	TtaFreeMemory (TargetName);
	TargetName = NULL;
     }
   if (attrNAME != NULL)
     {
	/* get a buffer for the NAME */
	length = TtaGetTextAttributeLength (attrNAME);
	TargetName = TtaGetMemory (length + 1);
	/* copy the NAME attribute into TargetName */
	TtaGiveTextAttributeValue (attrNAME, TargetName, &length);
     }
}


/*----------------------------------------------------------------------
   SetHREFattribute  sets the HREF attribue of the element to      
   the current target. If the HREF attribute does not      
   exist the function creates it.                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetHREFattribute (Element element, Document document, Document targetDoc)
#else  /* __STDC__ */
void                SetHREFattribute (element, document, targetDoc)
Element             element;
Document            document;
Document            targetDoc;

#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           attr;
   char               *value, *base;
   char*               tempURL = (char*) TtaGetMemory (sizeof (char) * MAX_LENGTH);

   attrType.AttrSSchema = TtaGetDocumentSSchema (document);
   attrType.AttrTypeNum = HTML_ATTR_HREF_;
   attr = TtaGetAttribute (element, attrType);
   if (attr == 0)
     {
	/* create an attribute HREF for the element */
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (element, attr, document);
     }
   /* build the complete target URL */
   if (document == targetDoc)
     {
	/* internal link */
	if (TargetName == NULL)
	   tempURL[0] = EOS;
	else
	  {
	     tempURL[0] = '#';
	     strcpy (&tempURL[1], TargetName);
	  }
	TtaSetAttributeText (attr, tempURL, element, document);
     }
   else
     {
	/* external link */
	if (TargetDocumentURL != NULL)
	   strcpy (tempURL, TargetDocumentURL);
	else
	   tempURL[0] = EOS;
	if (TargetName != NULL)
	  {
	     strcat (tempURL, "#");
	     strcat (tempURL, TargetName);
	  }
	/* set the relative value or URL in attribute HREF */
	base = GetBaseURL (document);
	value = MakeRelativeURL (tempURL, base);
	TtaSetAttributeText (attr, value, element, document);
	TtaFreeMemory (base);
	TtaFreeMemory (value);
	TtaFreeMemory (tempURL);
     }
}

/*----------------------------------------------------------------------
   GetNameAttr return the NAME attribute of the enclosing Anchor   
   element or the ID attribute of the selected element or NULL.
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

   attr = NULL;		/* no NAME attribute yet */
   if (selectedElement != NULL)
     {
	elType = TtaGetElementType (selectedElement);
	elType.ElTypeNum = HTML_EL_Anchor;
	attrType.AttrSSchema = elType.ElSSchema;
	el = TtaGetTypedAncestor (selectedElement, elType);
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
	     el = selectedElement;
	     while (attr == NULL && el != NULL)
	       {
		 attr = TtaGetAttribute (el, attrType);
		 el = TtaGetParent(el);
	       }
	  }
     }
   return (attr);
}



/*----------------------------------------------------------------------
   CreateTargetAnchor creates the NAME attribute with a default    
   value (Label of the element).                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateTargetAnchor (Document doc, Element el)
#else  /* __STDC__ */
void                CreateTargetAnchor (doc, el)
Document            doc;
Element             el;

#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           attr;
   ElementType         elType;
   Element             elText;
   Language            lang;
   char               *text;
   char*               url = (char*) TtaGetMemory (sizeof (char) * MAX_LENGTH);
   int                 length, i, space;
   boolean             found;
   boolean             withinHTML;

   elType = TtaGetElementType (el);
   withinHTML = (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0);
   /* get the NAME or ID attribute */
   attrType.AttrSSchema = elType.ElSSchema;
   if (withinHTML && (elType.ElTypeNum == HTML_EL_Anchor || elType.ElTypeNum == HTML_EL_MAP))
     attrType.AttrTypeNum = HTML_ATTR_NAME;
   else
     attrType.AttrTypeNum = HTML_ATTR_ID;
   attr = TtaGetAttribute (el, attrType);

   if (attr == 0)
     {
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (el, attr, doc);
     }

   /* get the Label text to build the name value */
   if (withinHTML && elType.ElTypeNum == HTML_EL_MAP)
     {
       /* mapxxx for a map element */
       strcpy (url, "map");
     }
   else if (withinHTML && elType.ElTypeNum == HTML_EL_LINK)
     {
       /* mapxxx for a map element */
       strcpy (url, "link");
     }
   else
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
			  strcpy (url, &url[space]);
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
		strcpy (url, text);
	      }
	  }
	else
	  {
	    /* label of the element */
	    text = TtaGetElementLabel (el);
	    strcpy (url, text);
	  }
     }
   /* copie the text into the NAME attribute */
   TtaSetAttributeText (attr, url, el, doc);
   /* Check attribute NAME in order to make sure that its value unique */
   /* in the document */
   MakeUniqueName (el, doc);
   /* set this new end-anchor as the new target */
   SetTargetContent (doc, attr);
   TtaFreeMemory (url);
}

/*----------------------------------------------------------------------
   CreateAnchor creates a link or target element.                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CreateAnchor (Document doc, View view, boolean createLink)
#else  /* __STDC__ */
static void         CreateAnchor (document, view, createLink)
Document            document;
View                view;
boolean             createLink;

#endif /* __STDC__ */
{
  Element             first, last, el, next;
  Element             parag, prev, child, anchor;
  SSchema            docSchema;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  DisplayMode         dispMode;
  int                 c1, cN, lg, i;
  boolean             noAnchor;

  dispMode = TtaGetDisplayMode (doc);
  /* get the first and last selected element */
  TtaGiveFirstSelectedElement (doc, &first, &c1, &i);
  TtaGiveLastSelectedElement (doc, &last, &i, &cN);
  docSchema = TtaGetDocumentSSchema (doc);

  /* Check whether the selected elements are a valid content for an anchor */
  elType = TtaGetElementType (first);
  if (elType.ElTypeNum == HTML_EL_Anchor && elType.ElSSchema == docSchema
      && first == last)
    /* add an attribute on the current anchor */
    anchor = first;
  else
    {
      /* search if the selection is included into an anchor */
      if (elType.ElSSchema == docSchema)
	el = SearchAnchor (doc, first, !createLink);
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
	      if (elType.ElSSchema != docSchema)
		noAnchor = TRUE;
	      else if (elType.ElTypeNum != HTML_EL_TEXT_UNIT &&
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
		noAnchor = TRUE;
	      if (el == last)
		el = NULL;
	      else
		TtaGiveNextSelectedElement (doc, &el, &i, &i);
	    }
	  
	  if (noAnchor)
	    {
	      if (createLink || el != NULL)
		/* cannot create an anchor here */
		TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_INVALID_ANCHOR1), NULL);
	      else
		{
		  /* create an ID for target element */
		  CreateTargetAnchor (doc, first);
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
  Attribute         attr;
  char             *value;
  char              url[MAX_LENGTH];
  int               length, i;
  boolean           change;

  elType = TtaGetElementType (el);
  if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") != 0)
    return;
  attrType.AttrSSchema = elType.ElSSchema;
   if (elType.ElTypeNum == HTML_EL_Anchor || elType.ElTypeNum == HTML_EL_MAP)
     attrType.AttrTypeNum = HTML_ATTR_NAME;
   else
     attrType.AttrTypeNum = HTML_ATTR_ID;
   attr = TtaGetAttribute (el, attrType);

   if (attr != 0)
     {
       /* the element has an attribute NAME or ID */
       length = TtaGetTextAttributeLength (attr) + 10; /* reverve of 9 chars */
       value = TtaGetMemory (length);
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
	       sprintf (&value[length], "%d", i);
	     }

	   if (change)
	     {
	       /* copy the element Label into the NAME attribute */
	       TtaSetAttributeText (attr, value, el, doc);
	       if (elType.ElTypeNum == HTML_EL_MAP)
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
		       if (i == length+1 && strncmp (&url[1], value, length) == 0)
			 {
			   /* Change the USEMAP of the image */
			   attr = TtaGetAttribute (image, attrType);
			   strcpy (&url[1], value);
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
  ElementType		elType;
  
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
    /* the element is a Pseudo_paragraph */
    {
      prev = el;
      TtaPreviousSibling (&prev);
      if (prev != NULL)
        /* the Pseudo-paragraph is not the first element among its sibling */
        /* turn it into an ordinary paragraph */
        ChangeElementType (el, HTML_EL_Paragraph);
    }
  else if (elType.ElTypeNum == HTML_EL_Paragraph)
    /* the element is a Paragraph */
    {
      prev = el;
      TtaPreviousSibling (&prev);
      if (prev == NULL)
        /* the Paragraph is the first element among its sibling */
        /* turn it into an Pseudo-paragraph if it's in a List_item or a
	   table cell. */
         {
	 parent = TtaGetParent (el);
	 if (parent != NULL)
	    {
	    elType = TtaGetElementType (parent);
	    if (elType.ElTypeNum == HTML_EL_List_Item ||
		elType.ElTypeNum == HTML_EL_Definition ||
		elType.ElTypeNum == HTML_EL_Data_cell ||
		elType.ElTypeNum == HTML_EL_Heading_cell)
		ChangeElementType (el, HTML_EL_Pseudo_paragraph);
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
   ElementPasted
   An element has been pasted in a HTML document.
   Check Pseudo paragraphs.
   If the pasted element has a NAME attribute, change its value if this
   NAME already used in the document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ElementPasted (NotifyElement * event)
#else  /* __STDC__ */
void                ElementPasted (event)
NotifyElement      *event;

#endif /* __STDC__ */
{
  Document            originDocument, doc;
  Element             el;
  AttributeType       attrType;
  Attribute           attr;
  ElementType         elType;
  SSchema             docSchema;
  int                 length, i, iName;
  char               *value, *base;
  char*               documentURL = (char*) TtaGetMemory (sizeof (char) * MAX_LENGTH);
  char*               tempURL     = (char*) TtaGetMemory (sizeof (char) * MAX_LENGTH);
  char*               path        = (char*) TtaGetMemory (sizeof (char) * MAX_LENGTH);

  el = event->element;
  doc = event->document;
  docSchema = TtaGetDocumentSSchema (doc);
  CheckPseudoParagraph (el, doc);
  elType = TtaGetElementType (el);
  if (docSchema == elType.ElSSchema && elType.ElTypeNum == HTML_EL_Anchor)
    {
      /* Check attribute NAME in order to make sure that its value unique */
      /* in the document */
      MakeUniqueName (el, doc);
      /* Change attributes HREF if the element comes from another */
      /* document */
	originDocument = (Document) event->position;
	if (originDocument != 0)
	  if (originDocument != doc)
	    {
	      /* the anchor has moved from one document to another */
	      /* get the HREF attribute of element Anchor */
	      attrType.AttrSSchema = elType.ElSSchema;
	      attrType.AttrTypeNum = HTML_ATTR_HREF_;
	      attr = TtaGetAttribute (el, attrType);
	      if (attr != NULL)
		{
		  /* get a buffer for the URL */
		  length = TtaGetTextAttributeLength (attr) + 1;
		  value = TtaGetMemory (length);
		  if (value != NULL)
		    {
		      /* get the URL itself */
		      TtaGiveTextAttributeValue (attr, value, &length);
		      if (value[0] == '#')
			{
			  /* the target element is local in the document origin */
			  /* convert internal link into external link */
			  strcpy (tempURL, DocumentURLs[originDocument]);
			  iName = 0;
			}
		      else
			{
			  /* the target element is in another document */
			  strcpy (documentURL, value);
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
			  if (!strcmp (tempURL, DocumentURLs[doc]))
			    /* convert external link into internal link */
			    strcpy (tempURL, &value[iName]);
			  else
			    strcat (tempURL, &value[iName]);
			}
		      TtaFreeMemory (value);
		      /* set the relative value or URL in attribute HREF */
		      base = GetBaseURL (doc);
		      value = MakeRelativeURL (tempURL, base);
		      TtaSetAttributeText (attr, value, el, doc);
		      TtaFreeMemory (base);
		      TtaFreeMemory (value);
		    }
		}
	    }
    }
  else if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
    {
      originDocument = (Document) event->position;
      if (originDocument != 0)
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
		  length = TtaGetTextAttributeLength (attr) + 1;
		  if (length > 0)
		    {
		      value = TtaGetMemory (MAX_LENGTH);
			if (value != NULL)
			  {
			    /* get the SRC itself */
			    TtaGiveTextAttributeValue (attr, value, &length);
			    /* update value and SRCattribute */
			    ComputeSRCattribute (el, doc, originDocument, attr, value);
			    TtaFreeMemory (value);
			  }
		      }
		  }
	      }
	  }
     }
  else
    /* Check attribute NAME or ID in order to make sure that its value */
    /* unique in the document */
    MakeUniqueName (el, doc);
  TtaFreeMemory (documentURL);
  TtaFreeMemory (tempURL);
  TtaFreeMemory (path);
}


/*----------------------------------------------------------------------
   CreateLink creates a link element.                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateLink (Document doc, View view)
#else  /* __STDC__ */
void                CreateLink (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateAnchor (doc, view, TRUE);
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
   if (event->event == TteAttrDelete)
      /* if the element is a SPAN without any other attribute, remove the SPAN
         element */
      DeleteSpanIfNoAttr (event->element, event->document);
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

   el = event->element;
   elType = TtaGetElementType (el);
   if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
     {
       /* update the associated map */
       if (event->attributeType.AttrTypeNum == HTML_ATTR_IntWidthPxl)
	 {
	   UpdateImageMap (el, event->document, OldWidth, -1);
	   OldWidth = -1;
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
	 el = TtaGetParent (el);
       if (elType.ElTypeNum == HTML_EL_AREA)
	 SetAreaCoords (event->document, el, event->attributeType.AttrTypeNum);
     }
}

/*----------------------------------------------------------------------
   StoreWidth IntWidthPxl will be changed, store the old value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             StoreWidth (NotifyAttribute * event)
#else  /* __STDC__ */
boolean             StoreWidth (event)
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
boolean             StoreHeight (NotifyAttribute * event)
#else  /* __STDC__ */
boolean             StoreHeight (event)
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
boolean             AttrWidthDelete (NotifyAttribute * event)
#else  /* __STDC__ */
boolean             AttrWidthDelete (event)
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
  char               *buffer;
  int                 length;
   length = buflen - 1;
   buffer = (char*) TtaGetMemory (buflen);
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
   char*               buffer = (char*) TtaGetMemory (sizeof (char) * buflen);
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
boolean             AttrFontSizeDelete (NotifyAttribute * event)
#else  /* __STDC__ */
boolean             AttrFontSizeDelete (event)
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
   char*               value = (char*) TtaGetMemory (sizeof (char) * buflen);
   int                 length;

   value[0] = EOS;
   length = TtaGetTextAttributeLength (event->attribute);
   if (length >= buflen)
      length = buflen - 1;
   if (length > 0)
      TtaGiveTextAttributeValue (event->attribute, value, &length);

   if (event->attributeType.AttrTypeNum == HTML_ATTR_BackgroundColor)
      HTMLSetBackgroundColor (event->document, event->element, value);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_background_)
      HTMLSetBackgroundImage (event->document, event->element, DRIVERP_REPEAT, value);
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
boolean             AttrColorDelete (NotifyAttribute * event)
#else  /* __STDC__ */
boolean             AttrColorDelete (event)
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
   AttrSTYLEinMenu doen't display STYLE in HEAD and MAP
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             AttrSTYLEinMenu (NotifyAttribute * event)
#else  /* __STDC__ */
boolean             AttrSTYLEinMenu (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
   ElementType         elType;
   Element             el;

   elType = TtaGetElementType (event->element);
   if (elType.ElSSchema != TtaGetDocumentSSchema (event->document))
     return FALSE;
   if (elType.ElTypeNum == HTML_EL_HEAD)
     return TRUE;
   else if (elType.ElTypeNum == HTML_EL_MAP)
     return TRUE;
   else
     {
       elType.ElTypeNum = HTML_EL_HEAD;
       el = TtaGetTypedAncestor (event->element, elType);
       if (el != NULL)
	 /* whitin the head */
	 return TRUE;
       else
	 {
	   elType.ElTypeNum = HTML_EL_MAP;
	   el = TtaGetTypedAncestor (event->element, elType);
	   if (el != NULL)
	     /* whitin the head */
	     return TRUE;
	   else
	     return FALSE;		/* let Thot perform normal operation */
	 }
     }
}

/*----------------------------------------------------------------------
   AttrNAMEinMenu doen't display NAME in Reset_Input and Submit_Input
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             AttrNAMEinMenu (NotifyAttribute * event)
#else  /* __STDC__ */
boolean             AttrNAMEinMenu (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType = TtaGetElementType (event->element);
   if (elType.ElTypeNum == HTML_EL_Reset_Input ||
       elType.ElTypeNum == HTML_EL_Submit_Input)
      /* Attribute menu for an element Reset_Input or Submit_Input */
      /* prevent Thot from including an entry for that attribute */
      return TRUE;
   else
      return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   ResetFontOrPhraseOnText: The text element elem should
   not be any longer within an element of type notType.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ResetFontOrPhraseOnText (Document document, Element elem, int notType)
#else  /* __STDC__ */
void                ResetFontOrPhraseOnText (document, elem, notType)
Document            document;
Element             elem;
int                 notType;

#endif /* __STDC__ */
{
   ElementType         elType, parentType;
   Element             elFont, parent, prev, next, added, child, last;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
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
		  child = prev;
		  TtaPreviousSibling (&prev);
		  TtaRemoveTree (child, document);
		  TtaInsertFirstChild (&child, added, document);
		  while (prev != NULL)
		    {
		       last = child;
		       child = prev;
		       TtaPreviousSibling (&prev);
		       TtaRemoveTree (child, document);
		       TtaInsertSibling (child, last, TRUE, document);
		    }
	       }
	     if (next != NULL)
	       {
		  added = TtaNewElement (document, parentType);
		  TtaInsertSibling (added, parent, FALSE, document);
		  child = next;
		  TtaNextSibling (&next);
		  TtaRemoveTree (child, document);
		  TtaInsertFirstChild (&child, added, document);
		  while (next != NULL)
		    {
		       last = child;
		       child = next;
		       TtaNextSibling (&next);
		       TtaRemoveTree (child, document);
		       TtaInsertSibling (child, last, FALSE, document);
		    }
	       }
	     elem = parent;
	  }
	while (elFont != elem);
	child = TtaGetFirstChild (elem);
	TtaRemoveTree (child, document);
	TtaInsertSibling (child, elem, TRUE, document);
	TtaDeleteTree (elem, document);
	TtaSetDocumentModified (document);
     }
}


/*----------------------------------------------------------------------
   SetFontOrPhraseOnText: The text element *elem should be 
   within an element of type newtype.              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetFontOrPhraseOnText (Document document, Element * elem, int newtype)
#else  /* __STDC__ */
void                SetFontOrPhraseOnText (document, elem, newtype)
Document            document;
Element            *elem;
int                 newtype;

#endif /* __STDC__ */
{
   ElementType         elType, siblingType;
   Element             prev, next, child, added, parent;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = newtype;
   /* is this element already within an element of the requested type? */
   if (TtaGetTypedAncestor (*elem, elType) == NULL)
     {
	/* it is not within an element of type newtype */
	prev = *elem;
	TtaPreviousSibling (&prev);
	if (prev != NULL)
	  {
	     siblingType = TtaGetElementType (prev);
	     if (siblingType.ElTypeNum == newtype)
	       {
		  child = TtaGetLastChild (prev);
		  if (child != NULL)
		    {
		       if (TtaCanInsertSibling (TtaGetElementType (*elem), child, FALSE,
						document))
			 {
			    TtaRemoveTree (*elem, document);
			    TtaInsertSibling (*elem, child, FALSE, document);
			    siblingType = TtaGetElementType (child);
			    if (siblingType.ElTypeNum == HTML_EL_TEXT_UNIT)
			      {
				 TtaMergeText (child, document);
				 *elem = child;
			      }
			    TtaSetDocumentModified (document);
			 }
		    }
		  else
		    {
		       if (TtaCanInsertFirstChild (TtaGetElementType (*elem), prev, document))
			 {
			    TtaRemoveTree (*elem, document);
			    TtaInsertFirstChild (elem, prev, document);
			    TtaSetDocumentModified (document);
			 }
		    }
	       }
	     else
	       {
		  if (TtaCanInsertSibling (elType, prev, FALSE, document))
		    {
		       added = TtaNewElement (document, elType);
		       TtaRemoveTree (*elem, document);
		       TtaInsertSibling (added, prev, FALSE, document);
		       TtaInsertFirstChild (elem, added, document);
		       TtaSetDocumentModified (document);
		    }
	       }
	  }
	else
	  {
	     next = *elem;
	     TtaNextSibling (&next);
	     if (next != NULL)
	       {
		  siblingType = TtaGetElementType (next);
		  if (siblingType.ElTypeNum == newtype)
		    {
		       child = TtaGetFirstChild (next);
		       if (child != NULL)
			 {
			    if (TtaCanInsertSibling (TtaGetElementType (*elem), child, TRUE, document))
			      {
				 TtaRemoveTree (*elem, document);
				 TtaInsertSibling (*elem, child, TRUE, document);
				 TtaSetDocumentModified (document);
			      }
			 }
		       else
			 {
			    if (TtaCanInsertFirstChild (TtaGetElementType (*elem), next, document))
			      {
				 TtaRemoveTree (*elem, document);
				 TtaInsertFirstChild (elem, next, document);
				 TtaSetDocumentModified (document);
			      }
			 }
		    }
		  else
		    {
		       if (TtaCanInsertSibling (elType, next, TRUE, document))
			 {
			    TtaRemoveTree (*elem, document);
			    added = TtaNewElement (document, elType);
			    TtaInsertSibling (added, next, TRUE, document);
			    TtaInsertFirstChild (elem, added, document);
			    TtaSetDocumentModified (document);
			 }
		    }
	       }
	     else
	       {
		  parent = TtaGetParent (*elem);
		  if (TtaCanInsertFirstChild (elType, parent, document))
		    {
		       TtaRemoveTree (*elem, document);
		       added = TtaNewElement (document, elType);
		       TtaInsertFirstChild (&added, parent, document);
		       TtaInsertFirstChild (elem, added, document);
		       TtaSetDocumentModified (document);
		    }
	       }
	  }
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateElemEmphasis (Document document, View view)
#else  /* __STDC__ */
void                CreateElemEmphasis (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Emphasis);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateElemStrong (Document document, View view)
#else  /* __STDC__ */
void                CreateElemStrong (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Strong);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateElemCite (Document document, View view)
#else  /* __STDC__ */
void                CreateElemCite (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Cite);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateElemDefinition (Document document, View view)
#else  /* __STDC__ */
void                CreateElemDefinition (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Def);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateElemCode (Document document, View view)
#else  /* __STDC__ */
void                CreateElemCode (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Code);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateElemVariable (Document document, View view)
#else  /* __STDC__ */
void                CreateElemVariable (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Variable);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateElemSample (Document document, View view)
#else  /* __STDC__ */
void                CreateElemSample (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Sample);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateElemKeyboard (Document document, View view)
#else  /* __STDC__ */
void                CreateElemKeyboard (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Keyboard);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateElemItalic (Document document, View view)
#else  /* __STDC__ */
void                CreateElemItalic (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Italic_text);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateElemBold (Document document, View view)
#else  /* __STDC__ */
void                CreateElemBold (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Bold_text);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateElemTeletype (Document document, View view)
#else  /* __STDC__ */
void                CreateElemTeletype (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Teletype_text);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateElemUnderline (Document document, View view)
#else  /* __STDC__ */
void                CreateElemUnderline (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Underlined_text);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateElemStrikeOut (Document document, View view)
#else  /* __STDC__ */
void                CreateElemStrikeOut (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Struck_text);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateElemBig (Document document, View view)
#else  /* __STDC__ */
void                CreateElemBig (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Big_text);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateElemSmall (Document document, View view)
#else  /* __STDC__ */
void                CreateElemSmall (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   SetCharFontOrPhrase (document, HTML_EL_Small_text);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateSub (Document document, View view)
#else  /* __STDC__ */
void                CreateSub (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Subscript;
   TtaCreateElement (elType, document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateSup (Document document, View view)
#else  /* __STDC__ */
void                CreateSup (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Superscript;
   TtaCreateElement (elType, document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateElemFont (Document document, View view)
#else  /* __STDC__ */
void                CreateElemFont (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_Font_;
   TtaCreateElement (elType, document);
}

/*----------------------------------------------------------------------
   SearchAnchor return the enclosing Anchor element with an        
   HREF attribute if link is TRUE or an NAME attribute.    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Element             SearchAnchor (Document doc, Element element, boolean link)
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
   SSchema             docSchema;

   attr = NULL;
   elType = TtaGetElementType (element);
   if (link && elType.ElTypeNum == HTML_EL_GRAPHICS_UNIT)
      /* search an ancestor of type AREA */
      typeNum = HTML_EL_AREA;
   else
      /* search an ancestor of type Anchor */
      typeNum = HTML_EL_Anchor;

   docSchema = TtaGetDocumentSSchema (doc);
   if (elType.ElTypeNum == typeNum && elType.ElSSchema == docSchema)
      elAnchor = element;
   else
     {
	elType.ElTypeNum = typeNum;
	elType.ElSSchema = docSchema;
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
void                UpdateAtom (Document doc, char *url, char *title)
#else  /* __STDC__ */
void                UpdateAtom (doc, url, title)
Document            doc;
char               *url;
char               *title;

#endif /* __STDC__ */
{
#ifndef _WINDOWS
   char               *v;
   int                 v_size;
   ThotWidget	       frame;
   static Atom         property_name = 0;
   Display            *dpy = TtaGetCurrentDisplay ();
   ThotWindow          win;

   frame = TtaGetViewFrame (doc, 1);
   if (frame == 0)
      return;
   win = XtWindow (XtParent (XtParent (XtParent (frame))));
   /* 13 is strlen("URL=0TITLE=00") */
   v_size = strlen (title) + strlen (url) + 13;
   v = (char *) TtaGetMemory (v_size);
   sprintf (v, "URL=%s%cTITLE=%s%c", url, 0, title, 0);
   if (!property_name)
      property_name = XInternAtom (dpy, "BROWSER_HISTORY_INFO", FALSE);
   XChangeProperty (dpy, win, property_name, XA_STRING, 8, PropModeReplace,
		    v, v_size);
   TtaFreeMemory (v);
#endif /* !_WINDOWS */
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
   char               *text;

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
