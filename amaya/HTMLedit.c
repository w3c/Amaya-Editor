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

#define buflen 50

#include "css_f.h"
#include "init_f.h"
#include "html2thot_f.h"
#include "AHTURLTools_f.h"
#include "EDITimage_f.h"
#include "EDITORactions_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
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
   int                 length;
   AttributeType       attrType;
   Attribute           attrHREF;
   char               *tempURL;

   attrType.AttrSSchema = TtaGetDocumentSSchema (document);
   attrType.AttrTypeNum = HTML_ATTR_HREF_;
   attrHREF = TtaGetAttribute (element, attrType);
   if (attrHREF == 0)
     {
	/* create an attribute HREF for the element */
	attrHREF = TtaNewAttribute (attrType);
	TtaAttachAttribute (element, attrHREF, document);
     }
   /* build the complete target URL */
   length = 2;
   if (TargetName != NULL)
      length += strlen (TargetName);
   if (document == targetDoc)
     {
	/* internal link */
	tempURL = TtaGetMemory (length);
	if (TargetName == NULL)
	   tempURL[0] = EOS;
	else
	  {
	     tempURL[0] = '#';
	     strcpy (&tempURL[1], TargetName);
	  }
     }
   else
     {
	/* external link */
	if (TargetDocumentURL != NULL)
	   length += strlen (TargetDocumentURL);
	tempURL = TtaGetMemory (length);
	if (TargetDocumentURL != NULL)
	   strcpy (tempURL, TargetDocumentURL);
	else
	   tempURL[0] = EOS;
	if (TargetName != NULL)
	  {
	     strcat (tempURL, "#");
	     strcat (tempURL, TargetName);
	  }
     }
   TtaSetAttributeText (attrHREF, tempURL, element, document);
   TtaFreeMemory (tempURL);
}

/*----------------------------------------------------------------------
   GetNameAttr return the NAME attribute of the enclosing Anchor   
   element or NULL.                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Attribute           GetNameAttr (Document doc, Element selectedElement)
#else  /* __STDC__ */
Attribute           GetNameAttr (doc, selectedElement)
Document            doc;
Element             selectedElement;

#endif /* __STDC__ */
{

   Element             elAnchor;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attrNAME;

   attrNAME = NULL;		/* no NAME attribute yet */
   if (selectedElement != NULL)
     {
	elType = TtaGetElementType (selectedElement);
	elType.ElTypeNum = HTML_EL_Anchor;
	elAnchor = TtaGetTypedAncestor (selectedElement, elType);
	if (elAnchor != NULL)
	  {
	     /* the ascending Anchor element has been found */
	     /* get the NAME attribute of element Anchor */
	     attrType.AttrSSchema = elType.ElSSchema;
	     attrType.AttrTypeNum = HTML_ATTR_NAME;
	     attrNAME = TtaGetAttribute (elAnchor, attrType);
	  }
     }
   return (attrNAME);
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
   Attribute           attrNAME;
   char               *text;

   /* set the default NAME attribute value to the element Label */
   attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
   attrType.AttrTypeNum = HTML_ATTR_NAME;
   attrNAME = TtaGetAttribute (el, attrType);
   if (attrNAME == 0)
     {
	attrNAME = TtaNewAttribute (attrType);
	TtaAttachAttribute (el, attrNAME, doc);
     }
   /* get the Label text */
   text = TtaGetElementLabel (el);
   /* copie the text into the NAME attribute */
   TtaSetAttributeText (attrNAME, text, el, doc);
   /* set this new end-anchor as the new target */
   SetTargetContent (doc, attrNAME);
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
   char               *text;
   int                 length, i, iName;
   char               *value;
   char                documentURL[MAX_LENGTH];
   char                tempURL[MAX_LENGTH];
   char                path[MAX_LENGTH];

   el = event->element;
   doc = event->document;
   CheckPseudoParagraph (el, doc);
   elType = TtaGetElementType (el);
   if (elType.ElTypeNum == HTML_EL_Anchor)
     {
	/* Check attribute NAME in order to make sure that its value unique */
	/* in the document */
	attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
	attrType.AttrTypeNum = HTML_ATTR_NAME;
	attr = TtaGetAttribute (el, attrType);
	if (attr != 0)
	  {
	     /* the pasted element has an attribute NAME */
	     length = TtaGetTextAttributeLength (attr) + 1;
	     value = TtaGetMemory (length);
	     if (value != NULL)
	       {
		  TtaGiveTextAttributeValue (attr, value, &length);
		  /* is this value already in use in the document ? */
		  if (SearchNAMEattribute (doc, value, attr) != NULL)
		    {
		       /* Yes. Avoid duplicate NAMEs */
		       text = TtaGetElementLabel (el);
		       /* copy the element Label into the NAME attribute */
		       TtaSetAttributeText (attr, text, el, doc);
		    }
	       }
	     TtaFreeMemory (value);
	  }
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
			       /* the target element is part of the origin document */
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
			       /* Add the  base content if ncessary */
			       NormalizeURL (documentURL, originDocument, tempURL, path);
			    }
			  if (value[iName] == '#')
			    {
			       if (!strcmp (tempURL, DocumentURLs[doc]))
				  /* convert external link into internal link */
				  strcpy (tempURL, &value[iName]);
			       else
				  strcat (tempURL, &value[iName]);
			    }
			  /* set the new value of attribute HREF */
			  TtaSetAttributeText (attr, tempURL, el, doc);
			  TtaFreeMemory (value);
		       }
		  }
	     }
     }
   else if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
     {
	/* Change attributes SRC if the element comes from another */
	/* document */
	originDocument = (Document) event->position;
	if (originDocument != 0)
	  {
	     if (originDocument != doc)
	       {
		  /* the image has moved from one document to another */
		  /* get the SRC attribute of element IMAGE */
		  attrType.AttrSSchema = elType.ElSSchema;
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
   if (elType.ElTypeNum != HTML_EL_AREA)
      el = TtaGetParent (el);
   SetAreaCoords (event->document, el, event->attributeType.AttrTypeNum);
}


/*----------------------------------------------------------------------
   AttrWidthCreated        An attribute Width__ has been created.  
   Create the corresponding attribute IntWidthPercent or   
   IntWidthPxl.                                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AttrWidthCreated (NotifyAttribute * event)
#else  /* __STDC__ */
void                AttrWidthCreated (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
   char                buffer[buflen];
   int                 length;

   length = buflen - 1;
   TtaGiveTextAttributeValue (event->attribute, buffer, &length);
   CreateAttrWidthPercentPxl (buffer, event->element, event->document);
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
   AttrWidthModified       An attribute Width__ has been modified. 
   Update the corresponding attribute IntWidthPercent or   
   IntWidthPxl.                                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AttrWidthModified (NotifyAttribute * event)
#else  /* __STDC__ */
void                AttrWidthModified (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
   AttrWidthDelete (event);
   AttrWidthCreated (event);
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
   char                buffer[buflen];
   int                 length;

   length = buflen - 1;
   TtaGiveTextAttributeValue (event->attribute, buffer, &length);
   CreateAttrIntSize (buffer, event->element, event->document);
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
   char                value[buflen];
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
   AttrNAMEinMenu                                                  
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
   Element             elFont, parent, prev, next, new, child, last;

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
		  new = TtaNewElement (document, parentType);
		  TtaInsertSibling (new, parent, TRUE, document);
		  child = prev;
		  TtaPreviousSibling (&prev);
		  TtaRemoveTree (child, document);
		  TtaInsertFirstChild (&child, new, document);
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
		  new = TtaNewElement (document, parentType);
		  TtaInsertSibling (new, parent, FALSE, document);
		  child = next;
		  TtaNextSibling (&next);
		  TtaRemoveTree (child, document);
		  TtaInsertFirstChild (&child, new, document);
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
   Element             prev, next, child, new, parent;

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
		       new = TtaNewElement (document, elType);
		       TtaRemoveTree (*elem, document);
		       TtaInsertSibling (new, prev, FALSE, document);
		       TtaInsertFirstChild (elem, new, document);
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
			    new = TtaNewElement (document, elType);
			    TtaInsertSibling (new, next, TRUE, document);
			    TtaInsertFirstChild (elem, new, document);
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
		       new = TtaNewElement (document, elType);
		       TtaInsertFirstChild (&new, parent, document);
		       TtaInsertFirstChild (elem, new, document);
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
   SearchNAMEattribute     search in document doc an element       
   having an attribut NAME whose value is nameVal.         
   Return that element or NULL if not found.               
   If ignore is not NULL, it is an attribute that should   
   be ignored when comparing NAME attributes.              
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
   if (found)
      return elFound;
   else
      return NULL;
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

   attr = NULL;
   elType = TtaGetElementType (element);
   if (link && elType.ElTypeNum == HTML_EL_GRAPHICS_UNIT)
      /* search an ancestor of type AREA */
      typeNum = HTML_EL_AREA;
   else
      /* search an ancestor of type Anchor */
      typeNum = HTML_EL_Anchor;

   if (elType.ElTypeNum == typeNum)
      elAnchor = element;
   else
     {
	elType.ElTypeNum = typeNum;
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
   GetActiveImageInfo returns the URL information if the current      
   element is an image map and NULL if it is not.          
   The non-null returned string has the form "?X,Y"        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char               *GetActiveImageInfo (Document document, Element element)
#else  /* __STDC__ */
char               *GetActiveImageInfo (document, element)
Document            document;
Element             element;

#endif /* __STDC__ */
{
   char               *ptr;
   int                 X, Y;

   ptr = NULL;
   if (element != NULL)
     {
	     /* initialize X and Y. The user may click in any view. If it's not */
	     /* the formatted view (view 1), TtaGiveSelectPosition does not */
	     /* change variables X and Y. */
	     X = Y = 0;
	     /* Get the coordinates of the mouse within the image */
	     TtaGiveSelectPosition (document, element, 1, &X, &Y);
	     if (X < 0)
		X = 0;
	     if (Y < 0)
		Y = 0;
	     /* create the search string to be appended to the URL */
	     ptr = TtaGetMemory (27);
	     sprintf (ptr, "?%d,%d", X, Y);
     }
   return ptr;
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
