/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2002
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
#include "XLink.h"
#include "MathML.h"
#ifdef _SVG
#include "SVG.h"
#endif

static char        *TargetDocumentURL = NULL;
static int          OldWidth;
static int          OldHeight;
#define buflen 50

#include "AHTURLTools_f.h"
#include "css_f.h"
#include "EDITimage_f.h"
#include "EDITORactions_f.h"
#include "fetchHTMLname_f.h"
#include "fetchXMLname_f.h"
#ifdef _SVG
#include "SVGbuilder_f.h"
#ifdef _SVGANIM
#include "anim_f.h"
#endif /* _SVGANIM */
#endif
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLimage_f.h"
#include "HTMLpresentation_f.h"
#include "html2thot_f.h"
#include "init_f.h"
#include "Mathedit_f.h"
#include "MathMLbuilder_f.h"
#include "styleparser_f.h"
#include "tree.h"
#include "XHTMLbuilder_f.h"
#include "XLinkedit_f.h"

#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */

static ThotBool AttrHREFundoable = FALSE;

/*----------------------------------------------------------------------
   SetTargetContent
   Set the new value of Target.                  
  ----------------------------------------------------------------------*/
void SetTargetContent (Document doc, Attribute attrNAME)
{
   int                 length;

   /* the document that issues the command Set target becomes the target doc */
   if (TargetDocumentURL != NULL)
      TtaFreeMemory (TargetDocumentURL);
   if (doc != 0)
      {
       length = strlen (DocumentURLs[doc]);
       TargetDocumentURL = TtaGetMemory (length + 1);
       strcpy (TargetDocumentURL, DocumentURLs[doc]);
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
	TargetName = TtaGetMemory (length + 1);
	/* copy the NAME attribute into TargetName */
	TtaGiveTextAttributeValue (attrNAME, TargetName, &length);
     }
}

/*----------------------------------------------------------------------
   LinkToPreviousTarget
   If current selection is within an anchor, change that link, otherwise
   create a link.
  ----------------------------------------------------------------------*/
void LinkToPreviousTarget (Document doc, View view)
{
   Element             el;
   Attribute           attr;
   int                 firstSelectedChar, i;

   if (!TtaGetDocumentAccessMode (doc))
     /* the document is in ReadOnly mode */
     return;
   if ((TargetName == NULL || TargetName[0] == EOS) &&
       (TargetDocumentURL == NULL || TargetDocumentURL[0] == EOS))
     /* no target available */
     return;
   if ((TargetName == NULL || TargetName[0] == EOS) &&
       !strcmp (TargetDocumentURL, DocumentURLs[doc]))
     /* it's not a valid target */
     return;

   TtaGiveFirstSelectedElement (doc, &el, &firstSelectedChar, &i);
   if (TtaIsReadOnly (el))
     /* the selected element is read-only */
     return;

   if (el != NULL)
     {
       UseLastTarget = TRUE;
       AttrHREFundoable = TRUE;
       /* Look if there is an enclosing anchor element */
       el = SearchAnchor (doc, el, &attr, TRUE);
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
   AddAccessKey
 -----------------------------------------------------------------------*/
void AddAccessKey (NotifyAttribute *event)
{
   char              buffer[2];
   int                 length;

   /* get the access key */
   length = 1;
   TtaGiveTextAttributeValue (event->attribute, buffer, &length);
   if (length > 0)
     TtaAddAccessKey (event->document, (unsigned int)buffer[0], event->element);
}


/*----------------------------------------------------------------------
   RemoveAccessKey
 -----------------------------------------------------------------------*/
ThotBool RemoveAccessKey (NotifyAttribute *event)
{
   char              buffer[2];
   int                 length;

   /* get the access key */
   length = 2;
   TtaGiveTextAttributeValue (event->attribute, buffer, &length);
   if (length > 0)
     TtaRemoveAccessKey (event->document, (int)buffer[0]);
  return FALSE; /* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
   RemoveLink
   Destroy the link element (HTML) or PI element (others) and 
   remove CSS rules when the link points to a CSS file.
  ----------------------------------------------------------------------*/
void RemoveLink (Element el, Document doc)
{
  Element	    elText;
  ElementType	    elType;
  AttributeType     attrType;
  Attribute         attr;
  char             *s, *ptr = NULL, *end = NULL;
  char              buffer[MAX_LENGTH], cssname[MAX_LENGTH];
  char              pathname[MAX_LENGTH], documentname[MAX_LENGTH];   
  int               length, piNum;
  Language          lang;

  /* Search the "nature" of the link */
  elType = TtaGetElementType (el);
  s = TtaGetSSchemaName (elType.ElSSchema);
  if (strcmp (s, "HTML") == 0)
    {
      /* (X)HTML document, well, we search within a hlink element */
      attrType.AttrSSchema = elType.ElSSchema;
      if (IsCSSLink (el, doc))
	{
	  /* it's a link to a style sheet. Remove that style sheet */
	  attrType.AttrTypeNum = HTML_ATTR_HREF_;
	  attr = TtaGetAttribute (el, attrType);
	  if (attr)
	    {
	      /* copy the HREF attribute into the buffer */
	      length = MAX_LENGTH;
	      TtaGiveTextAttributeValue (attr, buffer, &length);
	      NormalizeURL (buffer, doc, pathname, documentname, NULL);
	      RemoveStyleSheet (pathname, doc, TRUE, TRUE, NULL);
	    }
	}
    }
  else
    {
      if (strcmp (s, "MathML") == 0)
	piNum = MathML_EL_XMLPI;
      else if (strcmp (s, "SVG") == 0)
	piNum = SVG_EL_XMLPI;
      else
	piNum = XML_EL_xmlpi;
      if (elType.ElTypeNum == piNum)
	{
	  /* We search within an xml PI element */
	  elType.ElTypeNum = 1;
	  elText = TtaSearchTypedElement (elType, SearchInTree, el);
	  if (elText != NULL)
	    {
	      length = MAX_LENGTH - 1;
	      TtaGiveTextContent (elText, buffer, &length, &lang);
	      buffer[length++] = EOS;
	      printf ("\ncss buffer : '%s'\n", buffer);
	      /* Search the name of the stylesheet */
	      ptr = strstr (buffer, "href");
	      if (ptr != NULL)
		{
		  ptr = strstr (ptr, "\"");
		  ptr++;
		}
	      if (ptr != NULL)
		{
		  end = strstr (ptr, "\"");
		  *end = EOS;
		  strcpy (cssname, ptr);
		}
			
	      NormalizeURL (cssname, doc, pathname, documentname, NULL);
	      RemoveStyleSheet (pathname, doc, TRUE, TRUE, NULL);
	    }
	}
    }
  return;
}

/*----------------------------------------------------------------------
   DeleteLink                                              
  ----------------------------------------------------------------------*/
ThotBool DeleteLink (NotifyElement * event)
{
  RemoveLink (event->element, event->document);
  return FALSE;		/* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
   SetREFattribute
   Set the HREF or CITE attribue of the element to      
   the concatenation of targetURL and targetName.
  ----------------------------------------------------------------------*/
void SetREFattribute (Element element, Document doc, char *targetURL,
		      char *targetName)
{
   ElementType	       elType, piType;
   AttributeType       attrType;
   Attribute           attr;
   Element             piEl;
   SSchema	       HTMLSSchema;
   char               *value, *base, *s;
   char                tempURL[MAX_LENGTH];
   char                buffer[MAX_LENGTH];
   int                 length, piNum;
   ThotBool            new, oldStructureChecking, isHTML;

   if (AttrHREFundoable)
     TtaOpenUndoSequence (doc, element, element, 0, 0);

   HTMLSSchema = TtaGetSSchema ("HTML", doc);
   elType = TtaGetElementType (element);
   if (HTMLSSchema)
     isHTML = TtaSameSSchemas (elType.ElSSchema, HTMLSSchema);
   else
     isHTML = FALSE;

   /* Ii isn't a link to an xml stylesheet */
   if (!LinkAsXmlCSS)
     {
       if (isHTML)
	 {
	   attrType.AttrSSchema = HTMLSSchema;
	   if (elType.ElTypeNum == HTML_EL_Block_Quote ||
	       elType.ElTypeNum == HTML_EL_Quotation ||
	       elType.ElTypeNum == HTML_EL_INS ||
	       elType.ElTypeNum == HTML_EL_DEL)
	     attrType.AttrTypeNum = HTML_ATTR_cite;
	   else if (elType.ElTypeNum == HTML_EL_FRAME)
	     attrType.AttrTypeNum = HTML_ATTR_FrameSrc;
	   else
	     attrType.AttrTypeNum = HTML_ATTR_HREF_;
	 }
       else
	 {
	   /* the origin of the link is not a HTML element */
	   
	   /* create a XLink link */
	   attrType.AttrSSchema = TtaGetSSchema ("XLink", doc);
	   if (!attrType.AttrSSchema)
	     attrType.AttrSSchema = TtaNewNature (doc, TtaGetDocumentSSchema (doc),
						  "XLink", "XLinkP");
	   attrType.AttrTypeNum = XLink_ATTR_href_;
	   if (TtaIsLeaf (elType))
	     element = TtaGetParent (element);
	   if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
	     /* it's not a MathML element (the MathML 2.0 DTD does not use
		the xlink:type attribute) */
	     /* create a xlink:type attribute with value "simple" */
	     SetXLinkTypeSimple (element, doc, AttrHREFundoable);
	 }
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
     }

   /* build the complete target URL */
   if (targetURL && strcmp(targetURL, DocumentURLs[doc]))
     strcpy (tempURL, targetURL);
   else
     tempURL[0] = EOS;
   if (targetName != NULL)
     {
       strcat (tempURL, "#");
       strcat (tempURL, targetName);
     }

   if (LinkAsXmlCSS)
     {
       if ((tempURL[0] != EOS) && (IsCSSName (targetURL)))
	 {
	   /* set the relative value or URL in PI */
	   base = GetBaseURL (doc);
	   value = MakeRelativeURL (tempURL, base);
	   LinkAsXmlCSS = FALSE;

	   /* Load the CSS style sheet */
	   if (elType.ElTypeNum == 1)
	     {
	       piEl = TtaGetParent (element);
	       piEl = TtaGetParent (piEl);
	       piType = TtaGetElementType (piEl);
	       s = TtaGetSSchemaName (piType.ElSSchema);
	       if (strcmp (s, "MathML") == 0)
		 piNum = MathML_EL_XMLPI;
	       else if (strcmp (s, "SVG") == 0)
		 piNum = SVG_EL_XMLPI;
	       else
		 piNum = XML_EL_xmlpi;
	       if (piType.ElTypeNum == piNum)
		 {
		   /* The CSS is linked with the PI element */
		   LoadStyleSheet (targetURL, doc, piEl, NULL, CSS_ALL, FALSE);
		   /* We use the Latin_Script language to avoid the spell_chekcer */
		   /* to check this element */
		   strcpy (buffer, "xml-stylesheet type=\"text/css\" href=\"");
		   if (*value == EOS)
		     strcat (buffer, "./");
		   else
		     strcat (buffer, value);
		   strcat (buffer, "\"");
		   TtaSetTextContent (element, buffer, Latin_Script, doc);
		 }
	     }
	 }
     }
   else
     {
       if (tempURL[0] == EOS)
	 {
	   /* get a buffer for the attribute value */
	   length = TtaGetTextAttributeLength (attr);
	   if (length == 0)
	     /* no given value */
	     TtaSetAttributeText (attr, "XX", element, doc);
	 }
       else
	 {
	   /* set the relative value or URL in attribute HREF */
	   base = GetBaseURL (doc);
	   value = MakeRelativeURL (tempURL, base);
	   TtaFreeMemory (base);
	   if (*value == EOS)
	     TtaSetAttributeText (attr, "./", element, doc);
	   else
	     TtaSetAttributeText (attr, value, element, doc);
	   TtaFreeMemory (value);
	 }

       /* register the new value of the HREF attribute in the undo queue */
       if (AttrHREFundoable && new)
	 TtaRegisterAttributeCreate (attr, element, doc);
     }

   /* is it a html link to a CSS file? */
   if (tempURL[0] != EOS)
      if (elType.ElTypeNum == HTML_EL_LINK && isHTML &&
	  (LinkAsCSS || IsCSSName (targetURL)))
	 {
	   LinkAsCSS = FALSE;
	   LoadStyleSheet (targetURL, doc, element, NULL, CSS_ALL, FALSE);
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
	   TtaSetAttributeText (attr, "stylesheet", element, doc);
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
	   TtaSetAttributeText (attr, "text/css", element, doc);	   
	   if (AttrHREFundoable && new)
	       TtaRegisterAttributeCreate (attr, element, doc);
	 }
   if (AttrHREFundoable)
     {
       TtaCloseUndoSequence (doc);
       AttrHREFundoable = FALSE;
     }
   TtaSetDocumentModified (doc);
   TtaSetStatus (doc, 1, " ", NULL);
}


/*----------------------------------------------------------------------
   ChangeTitle displays a form to change the TITLE for the document.
  ----------------------------------------------------------------------*/
void ChangeTitle (Document doc, View view)
{
   ElementType         elType;
   Element             el, child;
   Language            lang;
#ifdef _I18N_
  unsigned char       *title;
#endif /* _I18N_ */
   int                 length;

   if (!TtaGetDocumentAccessMode (doc))
     /* the document is in ReadOnly mode */
     return;

   /* search the Title element */
   el = TtaGetMainRoot (doc);
   elType.ElSSchema = TtaGetDocumentSSchema (doc);
   if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
     /* it's a HTML document */
     {
       elType.ElTypeNum = HTML_EL_TITLE;
       el = TtaSearchTypedElement (elType, SearchForward, el);
       child = TtaGetFirstChild (el);
       if (child == NULL)
	 {
	   /* insert the text element */
	   elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	   child = TtaNewElement (doc, elType);
	   TtaInsertFirstChild  (&child, el, doc);
	 }
       length = MAX_LENGTH;
       TtaGiveTextContent (child, Answer_text, &length, &lang);
#ifdef _I18N_
       title = TtaConvertMbsToByte (Answer_text, TtaGetDefaultCharset ());
       strcpy (Answer_text, title);
       TtaFreeMemory (title);
#endif /* _I18N_ */

       CurrentDocument = doc;
#ifndef _WINDOWS 
       TtaNewForm (BaseDialog + TitleForm, TtaGetViewFrame (doc, 1),
		   TtaGetMessage (1, BTitle), TRUE, 2, 'L', D_CANCEL);
       TtaNewTextForm (BaseDialog + TitleText, BaseDialog + TitleForm, "",
		       50, 1, FALSE);
       /* initialise the text field in the dialogue box */
       TtaSetTextForm (BaseDialog + TitleText, Answer_text);
       TtaSetDialoguePosition ();
       TtaShowDialogue (BaseDialog + TitleForm, FALSE);
#else /* _WINDOWS */
       CreateTitleDlgWindow (TtaGetViewFrame (doc, view), Answer_text);
#endif /* _WINDOWS */
     }
}

/*----------------------------------------------------------------------
  SetNewTitle stores the new TITLE in the title element and updates
  the windows title.
  ----------------------------------------------------------------------*/
void SetNewTitle (Document doc)
{
  ElementType         elType;
  Element             el, child;
#ifdef _I18N_
  unsigned char       *title;
#endif /* _I18N_ */

  if (!TtaGetDocumentAccessMode (doc))
    /* the document is in ReadOnly mode */
    return;
  /* search the Title element */
  el = TtaGetMainRoot (doc);
  elType.ElSSchema = TtaGetDocumentSSchema (doc);
  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
    {
      elType.ElTypeNum = HTML_EL_TITLE;
      el = TtaSearchTypedElement (elType, SearchForward, el);
      child = TtaGetFirstChild (el);
      if (child)
	{
	  TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
	  TtaRegisterElementReplace (el, doc);
#ifdef _I18N_
	  title = TtaConvertByteToMbs (Answer_text, ISO_8859_1);
	  TtaSetTextContent (child, title, TtaGetDefaultLanguage (), doc);
	  TtaFreeMemory (title);
#else /* _I18N_ */
	  TtaSetTextContent (child, Answer_text, TtaGetDefaultLanguage (),
			     doc);
#endif /* _I18N_ */
	  TtaCloseUndoSequence (doc);
	  TtaSetDocumentModified (doc);
	  SetWindowTitle (doc, doc, 0);
	  if (DocumentSource[doc])
	    SetWindowTitle (doc, DocumentSource[doc], 0);
	}
    }
}

/*----------------------------------------------------------------------
   TitleModified
   The user has modified the contents of element TITLE. Update the    
   the Title field on top of the window.                           
  ----------------------------------------------------------------------*/
void TitleModified (NotifyOnTarget *event)
{
   UpdateTitle (event->element, event->document);
}


/*----------------------------------------------------------------------
   SelectDestination
   Select the destination of the el Anchor.     
  ----------------------------------------------------------------------*/
void SelectDestination (Document doc, Element el, ThotBool withUndo)
{
   Element             targetEl;
   ElementType	       elType;
   Document            targetDoc;
   Attribute           attr;
   AttributeType       attrType;
   char               *buffer = NULL;
   int                 length;
#ifndef _WINDOWS
   int                 i;
   char              s[MAX_LENGTH];
#endif
   ThotBool            isHTML;

   /* ask the user to select target document and target anchor */
   TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_SEL_TARGET), NULL);
   TtaClickElement (&targetDoc, &targetEl);
   if (targetDoc != 0)
     isHTML = !(strcmp (TtaGetSSchemaName (TtaGetDocumentSSchema (targetDoc)),
			 "HTML"));
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

	if (!LinkAsXmlCSS)
	  {
	    /* If the anchor has an HREF attribute, put its value in the form */
	    elType = TtaGetElementType (el);
	    if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
	      /* it's an HTML element */
	      {
		attrType.AttrSSchema = elType.ElSSchema;
		/* search the HREF or CITE attribute */
		if (elType.ElTypeNum == HTML_EL_Quotation ||
		    elType.ElTypeNum == HTML_EL_Block_Quote ||
		    elType.ElTypeNum == HTML_EL_INS ||
		    elType.ElTypeNum == HTML_EL_DEL)
		  attrType.AttrTypeNum = HTML_ATTR_cite;
		else
		  attrType.AttrTypeNum = HTML_ATTR_HREF_;
	      }
	    else
	      {
		attrType.AttrSSchema = TtaGetSSchema ("XLink", doc);
		attrType.AttrTypeNum = XLink_ATTR_href_;
	      }
	    attr = TtaGetAttribute (el, attrType);
	    AttrHREFvalue[0] = EOS;
	    if (attr != 0)
	      {
		/* get a buffer for the attribute value */
		length = TtaGetTextAttributeLength (attr);
		buffer = TtaGetMemory (length + 1);
		/* copy the HREF attribute into the buffer */
		TtaGiveTextAttributeValue (attr, buffer, &length);
		strcpy (AttrHREFvalue, buffer);
		TtaFreeMemory (buffer);
	      }
	  }

#ifndef _WINDOWS
	/* Dialogue form for open URL or local */
	i = 0;
	strcpy (&s[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
	i += strlen (&s[i]) + 1;
	strcpy (&s[i], TtaGetMessage (AMAYA, AM_BROWSE));
	i += strlen (&s[i]) + 1;
	strcpy (&s[i], TtaGetMessage (AMAYA, AM_CLEAR));
	
	TtaNewSheet (BaseDialog + AttrHREFForm, TtaGetViewFrame (doc, 1),
		     TtaGetMessage (AMAYA, AM_ATTRIBUTE), 3, s,
		     TRUE, 2, 'L', D_CANCEL);
	TtaNewTextForm (BaseDialog + AttrHREFText, BaseDialog + AttrHREFForm,
			TtaGetMessage (AMAYA, AM_LOCATION), 50, 1, TRUE);
	TtaNewLabel (BaseDialog + HREFLocalName,
		     BaseDialog + AttrHREFForm, " ");
	/* initialise the text field in the dialogue box */
	TtaSetTextForm (BaseDialog + AttrHREFText, AttrHREFvalue);
	strcpy (s, DirectoryName);
	strcat (s, DIR_STR);
	strcat (s, DocumentName);
	TtaSetDialoguePosition ();
	TtaShowDialogue (BaseDialog + AttrHREFForm, TRUE);
#else  /* _WINDOWS */
    CreateHRefDlgWindow (TtaGetViewFrame (doc, 1), AttrHREFvalue,
			 DocSelect, DirSelect, 2);
#endif  /* _WINDOWS */
     }
}

/*----------------------------------------------------------------------
   GetNameAttr
   Return the NAME attribute of the enclosing Anchor   
   element or the ID attribute of (an ascendant of) the selected element
   or NULL.
  ----------------------------------------------------------------------*/
Attribute GetNameAttr (Document doc, Element selectedElement)
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
	HTMLSSchema = TtaGetSSchema ("HTML", doc);
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
	    if (!attr)
	       {
	       attrType.AttrSSchema = TtaGetSSchema ("MathML", doc);
	       if (attrType.AttrSSchema)
		 {
		 attrType.AttrTypeNum = MathML_ATTR_id;
		 attr = TtaGetAttribute (selectedElement, attrType);
		 }
	       }
#ifdef _SVG
	    if (!attr)
	       {
	       attrType.AttrSSchema = TtaGetSSchema ("SVG", doc);
	       if (attrType.AttrSSchema)
		 {
		 attrType.AttrTypeNum = SVG_ATTR_id;
		 attr = TtaGetAttribute (selectedElement, attrType);
		 }
	       }
#endif
	  }
     }
   return (attr);
}


/*----------------------------------------------------------------------
   CreateTargetAnchor
   Create a NAME or ID attribute with a default value for element el.
   If the withUndo parameter is true, we'll register the undo sequence.
   If the forceID parameter, we'll always use an ID attribute, rather
   than a NAME one in some cases.
  ----------------------------------------------------------------------*/
void CreateTargetAnchor (Document doc, Element el, ThotBool forceID,
			 ThotBool withUndo)
{
   AttributeType       attrType;
   Attribute           attr;
   ElementType         elType;
   Element             elText;
   SSchema	       HTMLSSchema;
   Language            lang;
   char               *text;
   char               *url = TtaGetMemory (MAX_LENGTH);
   int                 length, i, space;
   ThotBool            found;
   ThotBool            withinHTML, new;

   elType = TtaGetElementType (el);
   withinHTML = !strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML");

   /* get a NAME or ID attribute */
   HTMLSSchema = TtaGetSSchema ("HTML", doc);
   attrType.AttrSSchema = HTMLSSchema;
   if (withinHTML && (elType.ElTypeNum == HTML_EL_Anchor ||
		      elType.ElTypeNum == HTML_EL_MAP))
     {
       if (forceID)
	 attrType.AttrTypeNum = HTML_ATTR_ID;
       else
	 attrType.AttrTypeNum = HTML_ATTR_NAME;
     }
   else
     {
     if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
       {
	 attrType.AttrSSchema = elType.ElSSchema;
	 attrType.AttrTypeNum = MathML_ATTR_id;
       }
     else
#ifdef _SVG
     if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "SVG") == 0)
       {
	 attrType.AttrSSchema = elType.ElSSchema;
	 attrType.AttrTypeNum = SVG_ATTR_id;
       }
     else
#endif
       attrType.AttrTypeNum = HTML_ATTR_ID;
     }
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

   /* build a value for the new attribute */
   if (withinHTML && elType.ElTypeNum == HTML_EL_MAP)
     {
       /* mapxxx for a map element */
       strcpy (url, "map");
     }
   else if (withinHTML && elType.ElTypeNum == HTML_EL_LINK)
     {
       /* linkxxx for a link element */
       strcpy (url, "link");
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
			  strcpy (url, &url[space]);
		      }
		    i++;
		    space = i;
		  }
		else if (url[i] == '_' ||
			 (i-space > 0 && url[i] == '.') ||
			 (i-space > 0 && url[i] == '-') ||
			 ((unsigned int) url[i] >= 65 &&          /*  'A'  */
			  (unsigned int) url[i] <= 90) ||         /*  'Z'  */
			 ((unsigned int) url[i] >= 97 &&          /*  'a'  */
			  (unsigned int) url[i] <= 127) ||        /*  'z'  */
			 (i-space > 0 &&
			  (unsigned int) url[i] >= 48 &&          /*  '0'  */
			  (unsigned int) url[i] <= 57))           /*  '9'  */
		  /* valid character for an ID */
		  i++;
		else if (i > 2)
		  url[i] = ' ';
		else
		  /* invalid name for an ID */
		  i = length;
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
	    /* get the element's label if there is no text */
	    text = TtaGetElementLabel (el);
	    strcpy (url, text);
	  }
     }
   /* copie the text into the NAME attribute */
   TtaSetAttributeText (attr, url, el, doc);
   TtaSetDocumentModified (doc);
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
   CreateAnchor
   Create a link or target element.                  
  ----------------------------------------------------------------------*/
void CreateAnchor (Document doc, View view, ThotBool createLink)
{
  Element             first, last, el, next, parent;
  Element             parag, prev, child, anchor, ancestor, duplicate;
  SSchema             HTMLSSchema;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  DisplayMode         dispMode;
  int                 firstChar, lastChar, lg, i, levelFirst, levelLast;
  ThotBool            noAnchor, ok;

  if (!TtaGetDocumentAccessMode (doc))
    /* the document is in ReadOnly mode */
    return;

  parag = NULL;
  HTMLSSchema = TtaGetSSchema ("HTML", doc);
  dispMode = TtaGetDisplayMode (doc);

  /* get the first and last selected element */
  TtaGiveFirstSelectedElement (doc, &first, &firstChar, &i);
  if (TtaIsReadOnly (first))
    /* the selected element is read-only */
    return;
  TtaGiveLastSelectedElement (doc, &last, &i, &lastChar);

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
	el = SearchAnchor (doc, first, &attr, TRUE);
      else
	el = NULL;
      if (el)
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
		  elType.ElTypeNum != HTML_EL_SCRIPT_ &&
		  elType.ElTypeNum != HTML_EL_MAP &&
		  elType.ElTypeNum != HTML_EL_Quotation &&
		  elType.ElTypeNum != HTML_EL_Subscript &&
		  elType.ElTypeNum != HTML_EL_Superscript &&
		  elType.ElTypeNum != HTML_EL_Span &&
		  elType.ElTypeNum != HTML_EL_BDO &&
		  elType.ElTypeNum != HTML_EL_simple_ruby &&
		  elType.ElTypeNum != HTML_EL_complex_ruby &&
		  elType.ElTypeNum != HTML_EL_IFRAME )
		noAnchor = TRUE;
	      if (el == last)
		el = NULL;
	      else
		TtaGiveNextSelectedElement (doc, &el, &i, &i);
	    }
	  
	  if (noAnchor)
	    {
	      if (createLink || el)
		{
		elType = TtaGetElementType (first);
		if (first == last && firstChar == 0 && lastChar == 0 &&
		    createLink &&
		    !TtaSameSSchemas (elType.ElSSchema, HTMLSSchema))
		   /* a single element is selected and it's not a HTML elem
		      nor a character string */
		  {
		    if (UseLastTarget)
		      /* points to the last created target */
		      SetREFattribute (first, doc, TargetDocumentURL,
				       TargetName);
		    else
		      /* select the destination */
		      SelectDestination (doc, first, TRUE);
		  }
		else
		  /* cannot create an anchor here */
		  TtaSetStatus (doc, 1,
				TtaGetMessage (AMAYA, AM_INVALID_ANCHOR1),
				NULL);
		}
	      else
		  /* create an ID for target element */
		{
		  TtaOpenUndoSequence (doc, first, last, firstChar, lastChar);
		  CreateTargetAnchor (doc, first, FALSE, TRUE);
		  TtaCloseUndoSequence (doc);
		}
	      return;
	    }
	  /* check if the anchor to be created is within an anchor element */
	  else
	    {
	      ok = TRUE;
	      ancestor = SearchAnchor (doc, first, &attr, TRUE);
	      if (ancestor)
		{
	          elType = TtaGetElementType (ancestor);
		  if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML")
		      && elType.ElTypeNum == HTML_EL_Anchor)
		    ok = FALSE;
		}
	      if (ok)
		{
		  ancestor = SearchAnchor (doc, last, &attr, TRUE);
		  if (ancestor)
		    {
		      elType = TtaGetElementType (ancestor);
		      if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML")
			  && elType.ElTypeNum == HTML_EL_Anchor)
			ok = FALSE;
		    }
		}
	      if (!ok)
		{
		  TtaSetStatus (doc, 1,
				TtaGetMessage (AMAYA, AM_INVALID_ANCHOR2),
				NULL);
		  return;
		}
	    }

	  /* stop displaying changes that will be made in the document */
	  if (dispMode == DisplayImmediately)
	    TtaSetDisplayMode (doc, DeferredDisplay);
	  /* remove selection before modifications */
	  TtaUnselect (doc);

	  TtaOpenUndoSequence (doc, first, last, firstChar, lastChar);

	  /* process the last selected element */
	  elType = TtaGetElementType (last);
	  /* if its a text leaf which is partly selected, split it */
	  if (lastChar >= 1 && elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	    {
	      if (lastChar < firstChar && first == last)
		/* it's a caret */
		lastChar = firstChar;
	      lg = TtaGetElementVolume (last);
	      if (lastChar <= lg)
		/* split the last text */
		{
		  TtaRegisterElementReplace (last, doc);
		  TtaSplitText (last, lastChar, doc);
		  next = last;
		  TtaNextSibling (&next);
		  TtaRegisterElementCreate (next, doc);
		}
	    }
	  if (last != first)
	    {
	      levelFirst = TtaGetElementLevel (first);
	      levelLast = TtaGetElementLevel (last);
	      if (levelLast > levelFirst)
		/* the last selected element is deeper in the tree than the
		   first one. Split the ancestor of the last element up
		   to the level of the first selected element */
		{
		  ancestor = TtaGetParent (last);
		  while (ancestor)
		    {
		      levelLast = TtaGetElementLevel (ancestor);
		      if (levelLast < levelFirst)
			ancestor = NULL;
		      else
			{
			  next = last;
			  TtaNextSibling (&next);
			  if (next)
			    {
			      TtaRegisterElementReplace (ancestor, doc);
			      elType = TtaGetElementType (ancestor);
			      duplicate = TtaNewElement (doc, elType);
			      TtaInsertSibling (duplicate, ancestor,FALSE,doc);
			      prev = NULL;
			      while (next)
				{
				  child = next;
				  TtaNextSibling (&next);
				  TtaRemoveTree (child, doc);
				  if (prev == NULL)
				    TtaInsertFirstChild (&child,duplicate,doc);
				  else
				    TtaInsertSibling (child, prev, FALSE, doc);
				  prev = child;
				} 
			      TtaRegisterElementCreate (duplicate, doc);
			    }
			  last = ancestor;
			  ancestor = TtaGetParent (ancestor);
			}
		    }
		}
	    }

	  /* process the first selected element */
	  elType = TtaGetElementType (first);
	  if (firstChar > 1 && elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	    {
	      /* split the first selected text element */
	      el = first;
	      lg = TtaGetElementVolume (first);
	      if (firstChar > lg)
		{
		  /* insert an empty box */
		  child = TtaNewTree (doc, elType, "");
		  TtaInsertSibling (child, first, FALSE, doc);
		  lastChar = 0;
		}
	      else
		{
		  /* split the first string */
		  TtaRegisterElementReplace (first, doc);
		  TtaSplitText (first, firstChar, doc);
		}
	      TtaNextSibling (&first);
	      TtaRegisterElementCreate (first, doc);
	      if (last == el)
		/* we have to change last selection because the element
		   was split */
		last = first;
	    }
	  if (last != first)
	    {
	      levelFirst = TtaGetElementLevel (first);
	      levelLast = TtaGetElementLevel (last);
	      if (levelFirst > levelLast)
		/* the first selected element is deeper in the tree than the
		   last one. Split the ancestor of the first element up
		   to the level of the last selected element */
		{
		  ancestor = TtaGetParent (first);
		  while (ancestor)
		    {
		      levelFirst = TtaGetElementLevel (ancestor);
		      if (levelFirst < levelLast)
			ancestor = NULL;
		      else
			{
			  prev = first;
			  TtaPreviousSibling (&prev);
			  if (prev)
			    {
			      TtaRegisterElementReplace (ancestor, doc);
			      elType = TtaGetElementType (ancestor);
			      duplicate = TtaNewElement (doc, elType);
			      TtaInsertSibling (duplicate, ancestor, TRUE,doc);
			      next = NULL;
			      while (prev)
				{
				  child = prev;
				  TtaPreviousSibling (&prev);
				  TtaRemoveTree (child, doc);
				  if (next == NULL)
				    TtaInsertFirstChild (&child,duplicate,doc);
				  else
				    TtaInsertSibling (child, next, TRUE, doc);
				  next = child;
				} 
			      TtaRegisterElementCreate (duplicate, doc);
			    }
			  first = ancestor;
			  ancestor = TtaGetParent (ancestor);
			}
		    }
		}
	    }

	  /* Create the corresponding anchor */
	  elType.ElTypeNum = HTML_EL_Anchor;
	  anchor = TtaNewElement (doc, elType);
	  if (createLink)
	    {
	      /* create an attribute HREF for the new anchor */
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
	      /* prepare the next element in the selection, as the current
		 element will be moved and its successor will no longer
		 be accessible */
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
  if (TtaGetElementVolume (anchor))
    TtaSelectElement (doc, anchor);
  else
    {
      /* prepare a possible insertion of text */
      child = TtaGetLastChild (anchor);
      TtaSelectString (doc, child, 1, 0);
    }
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
      TtaSetAttributeText (attr, "link", anchor, doc);
    }
  else
    CreateTargetAnchor (doc, anchor, FALSE, FALSE);

  if (parag)
    TtaRegisterElementCreate (parag, doc);
  else
    TtaRegisterElementCreate (anchor, doc);
  TtaCloseUndoSequence (doc);
}

/*----------------------------------------------------------------------
   MakeUniqueName
   Check attribute NAME or ID in order to make sure that its value is unique
   in the document.
   If the NAME or ID is already used, add a number at the end of the value.
  ----------------------------------------------------------------------*/
void MakeUniqueName (Element el, Document doc)
{
  ElementType	    elType;
  AttributeType     attrType;
  Attribute         attr;
  Element	    image;
  char             *value;
  char              url[MAX_LENGTH];
  int               length, i;
  ThotBool          change, checkID;

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  checkID = FALSE;
  if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
    {
      /* it's an element from the XHTML namespace */
      if (elType.ElTypeNum == HTML_EL_Anchor ||
	  elType.ElTypeNum == HTML_EL_MAP)
	/* it's an anchor or a map. Look for a NAME attribute */
	{
	  attrType.AttrTypeNum = HTML_ATTR_NAME;
	  attr = TtaGetAttribute (el, attrType);
	  if (attr != 0)
	    /* the element has a NAME attribute. Check it and then check
	       if there is an ID too */
	    checkID = TRUE;
	  else
	    /* no NAME. Look for an ID */
	    attrType.AttrTypeNum = HTML_ATTR_ID;
	}
      else
	/* Look for an ID attribute */
	attrType.AttrTypeNum = HTML_ATTR_ID;
    }
  else if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "MathML"))
    /* it's an element from the MathML namespace, look for the
       id attribute from the same namespace */
    attrType.AttrTypeNum = MathML_ATTR_id;
#ifdef _SVG
  else if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "SVG"))
    /* it's an element from the SVG namespace, look for the
       id attribute from the same namespace */
    attrType.AttrTypeNum = SVG_ATTR_id;
#endif
  else
    attrType.AttrTypeNum = 0;
  
  if (attrType.AttrTypeNum != 0)
    {
      attr = TtaGetAttribute (el, attrType);
      if (attr)
	/* the element has an attribute NAME or ID. Check it */
	{
	  length = TtaGetTextAttributeLength (attr) + 10;
	  value = TtaGetMemory (length);
	  change = FALSE;
	  if (value)
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
		  if (checkID)
		    /* It's an HTML anchor. We have just changed its NAME
		       attribute. Change its ID (if any) accordingly */
		    {
		      attrType.AttrTypeNum = HTML_ATTR_ID;
		      attr = TtaGetAttribute (el, attrType);
		      if (attr)
			TtaSetAttributeText (attr, value, el, doc);
		    }
		  if ((strcmp(TtaGetSSchemaName (elType.ElSSchema),
			       "HTML") == 0) &&
		      elType.ElTypeNum == HTML_EL_MAP)
		    /* it's a MAP element */
		    {
		      /* Search backward the refered image */
		      attrType.AttrTypeNum = HTML_ATTR_USEMAP;
		      TtaSearchAttribute (attrType, SearchBackward, el,
					  &image, &attr);
		      if (!attr)
			/* Not found. Search forward the refered image */
			TtaSearchAttribute (attrType, SearchForward, el,
					    &image, &attr);
		      if (attr && image)
			/* referred image found */
			{
			  i = MAX_LENGTH;
			  TtaGiveTextAttributeValue (attr, url, &i);
			  if (i == length+1 && !strncmp (&url[1], value,
							  length))
			    {
			      /* Change the USEMAP attribute of the image */
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
}

/*----------------------------------------------------------------------
  GetNextNode
  Return the next node in the tree, using a complete traversal algorithm.
  ----------------------------------------------------------------------*/
static Element    GetNextNode (Element curr)
{
  Element el;

  if (!curr)
    return NULL;

  /* get the next child */
  el = TtaGetFirstChild (curr);
  if (!el)
    {
      /* get the next siblign, or parent
	 if there was no other sibling */
      el = TtaGetSuccessor (curr);
    }
  return el;
}

/*----------------------------------------------------------------------
  SearchTypedElementForward
  Search for a typed element and stops when it finds it or if the
  search reaches the last element.
  ----------------------------------------------------------------------*/
static Element SearchTypedElementForward (ElementType elType_search,
					  Element curr, Element last)
{
  ElementType elType;
  Element el;

  /* start by getting the next node */
  el = GetNextNode (curr);
  /*  continue browsing until we get an element
     of the searched type or the end condition is
     reached */
  while (el && el != last)
    {
      elType = TtaGetElementType (el);
      if (TtaSameTypes (elType_search, elType))
	  break;
      el = GetNextNode (el);
    }

  /* don't return el if it's equal to the last element parameter */
  return ((el != last) ? el : NULL);
}

/*----------------------------------------------------------------------
  CreateRemoveIDAttribute
  For all elements elName of a document, this functions eithers adds or 
  deletes an ID attribute. 
  The createID flag tells which operation must be done.
  The inSelection attribute says if we must apply the operation in the
  whole document or just in the current selection.
  If an element already has an ID attribute, a new one won't be created.
  TO DO: Use the thotmsg functions for the dialogs.
  ----------------------------------------------------------------------*/
void         CreateRemoveIDAttribute (char *elName, Document doc, ThotBool createID, ThotBool inSelection)
{
  Element             el, lastEl;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  char             *schema_name;
  DisplayMode         dispMode;
  ThotBool            closeUndo;
  int                 i, j;

  /* the user must select something */
  if (inSelection && !TtaIsDocumentSelected (doc))
    {
      strcpy (IdStatus, "Nothing selected");
      return;
    }
  /* search for the elementType corresponding to the element name given
   by the user */
  GIType (elName, &elType, doc);
  if (elType.ElTypeNum == 0)
    {
      /* element name not found */
      strcpy (IdStatus, "Unknown element");
      return;
    }
  /* in function of the target elType, we choose the correct
     ATTR_ID value and schema */
  attrType.AttrTypeNum = 0;
  schema_name = TtaGetSSchemaName (elType.ElSSchema);
  if (!strcmp (schema_name, "HTML"))
    {
      /* exception handling... we can't add an ID attribute everywhere
       in HTML documents */
      if (!(elType.ElTypeNum == HTML_EL_HTML
	    || elType.ElTypeNum == HTML_EL_HEAD
	    || elType.ElTypeNum == HTML_EL_TITLE
	    || elType.ElTypeNum == HTML_EL_BASE
	    || elType.ElTypeNum == HTML_EL_META
	    || elType.ElTypeNum == HTML_EL_SCRIPT_
	    || elType.ElTypeNum == HTML_EL_STYLE_))
	attrType.AttrTypeNum = HTML_ATTR_ID;
    }
  else if (!strcmp (schema_name, "MathML"))
    attrType.AttrTypeNum = MathML_ATTR_id;
  else if (!strcmp (schema_name, "SVG"))
    attrType.AttrTypeNum = SVG_ATTR_id;

  /* we didn't find an attribute or we can't put an ID attribute
     in this element */
  if (attrType.AttrTypeNum == 0)
    {
      strcpy (IdStatus, "DTD forbids it");
      return;
    }
  attrType.AttrSSchema = elType.ElSSchema;

  /* prepare the environment before doing the operation */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);

  /* set the first and last elements for the search */
  if (inSelection)
    {
      /* get the first and last elements of the selection */
      TtaGiveFirstSelectedElement (doc, &el, &i, &j);
       if (TtaIsSelectionEmpty ())
	 lastEl = el;
       else
	 TtaGiveLastSelectedElement (doc, &lastEl, &j, &i);
       /* and set the last element as the element just after
	  the selection */
       lastEl = TtaGetSuccessor (lastEl);
    }
  else
    {
      el = TtaGetMainRoot (doc);
      lastEl = NULL;
    }

  /*
  ** browse the tree and add the ID if it's missing. Variable i
  ** stores the number of changes we have done.
  */

  /* move to the first element that is of the chosen elementType */
  /* the element where we started is not of the chosen type */
  if (!TtaSameTypes (TtaGetElementType (el), elType))
    el = SearchTypedElementForward (elType, el, lastEl);

  if (TtaPrepareUndo (doc))
      closeUndo = FALSE;
  else
    {
      closeUndo = TRUE;
      TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
    }
  i = 0;
  while (el)
    {
      /* does the element have an ID attribute already? */
      attr = TtaGetAttribute (el, attrType);
      if (!attr && createID) /* add it */
	{
	  /* we reuse an existing Amaya function */
	  CreateTargetAnchor (doc, el, TRUE, TRUE);
	  i++;
	}
      else if (attr && !createID) /* delete it */
	{
	  TtaRegisterAttributeDelete (attr, el, doc);
	  TtaRemoveAttribute (el, attr, doc);
	  i++;
	}
      /* get the next element */
      el = SearchTypedElementForward (elType, el, lastEl);
    }

  if (closeUndo)
    TtaCloseUndoSequence (doc);

  /* reset the state of the document */
  if (i)
    TtaSetDocumentModified (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);
  sprintf (IdStatus, "%d elements changed", i);
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
static void         CheckPseudoParagraph (Element el, Document doc)
{
  Element		prev, next, parent;
  Attribute             attr;
  ElementType		elType;
  
  elType = TtaGetElementType (el);
  if (!TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema ("HTML", doc)))
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
void                ElementCreated (NotifyElement * event)
{
  CheckPseudoParagraph (event->element, event->document);
}

/*----------------------------------------------------------------------
   ElementDeleted
   An element has been deleted. If it was the only child of element
   BODY, create a first paragraph.
 -----------------------------------------------------------------------*/
void ElementDeleted (NotifyElement *event)
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
	child = TtaNewTree (event->document, elType, "");
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
   RegisterURLSavedElements
   Elements from document doc have been saved in the Thot Copy/Cut buffer.
   Save the URL of this document, to allow relative URIs contained
   in these elements to be adapted when they are pasted.
  ----------------------------------------------------------------------*/
void RegisterURLSavedElements (Document doc)
{
  if (SavedDocumentURL)
    TtaFreeMemory (SavedDocumentURL);
  SavedDocumentURL = GetBaseURL (doc);
}

/*----------------------------------------------------------------------
   ChangeURI
   Element el has been pasted in document doc. It comes from document
   originDocument and it has an href attribute (from the HTML or XLink
   namespace) that has to be updated. Update it according to the new
   context.
  ----------------------------------------------------------------------*/
void ChangeURI (Element el, Attribute attr, Document originDocument,
		Document doc)
{
  int      length, i, iName;
  char    *value, *base, *documentURI, *tempURI, *path;

  /* get a buffer for the URI */
  length = TtaGetTextAttributeLength (attr) + 1;
  value = TtaGetMemory (length);
  if (value)
    {
    tempURI = TtaGetMemory (MAX_LENGTH);
    if (tempURI)
      {
      iName = 0;
      /* get the URI itself */
      TtaGiveTextAttributeValue (attr, value, &length);
      if (value[0] == '#')
	  /* the target is in the original document */
	  /* convert the internal link into an external link */
	{
	  if (originDocument == 0)
	    {
	      /* origin document has been unloaded. Get the saved URL */
	      if (SavedDocumentURL == NULL)
		RegisterURLSavedElements (doc);
	      strcpy (tempURI, SavedDocumentURL);
	    }
	  else
	    strcpy (tempURI, DocumentURLs[originDocument]);
	}
      else
	{
	  /* the target element is in another document */
          documentURI = TtaGetMemory (MAX_LENGTH);
	  if (documentURI)
	    {
	    strcpy (documentURI, value);
	    /* looks for a '#' in the value */
	    i = length;
	    while (value[i] != '#' && i > 0)
	      i--;
	    if (i == 0)
		/* there is no '#' in the URI */
		value[0] = EOS;
	    else
	      {
		/* there is a '#' character in the URI */
		/* separate document name and element name */
		documentURI[i] = EOS;
		iName = i;
	      }
	    /* get the complete URI of the referred document */
	    /* Add the base if necessary */
	    path = TtaGetMemory (MAX_LENGTH);
	    if (path)
	      {
	      if (originDocument == 0)
	        NormalizeURL (documentURI, -1, tempURI, path, NULL);
	      else
	        NormalizeURL (documentURI, originDocument, tempURI, path,NULL);
	      TtaFreeMemory (path);
	      }
	    TtaFreeMemory (documentURI);
	    }
	}
      if (value[iName] == '#')
	{
	  if (!strcmp (tempURI, DocumentURLs[doc]))
	    /* convert external link into internal link */
	    strcpy (tempURI, &value[iName]);
	  else
	    strcat (tempURI, &value[iName]);
	}
      /* set the relative value or URI in attribute HREF */
      base = GetBaseURL (doc);
      if (base)
	{
	  TtaFreeMemory (value);
	  value = MakeRelativeURL (tempURI, base);
	  TtaFreeMemory (base);
        }
      TtaSetAttributeText (attr, value, el, doc);
      TtaFreeMemory (tempURI);
      }
    TtaFreeMemory (value);
    }
}

static void CheckDescendants (/* Element el, Document doc */);  /* see below */

/*----------------------------------------------------------------------
   ElementOKforProfile
   This function is called for each element pasted by the user, and for
   each element within the pasted element.
   Check whether element el is valid in the document profile. If it is
   not, delete the element and return FALSE.
   Check also all attributes associated with the element and remove
   the attributes that are not allowed by the profile.
  ----------------------------------------------------------------------*/
ThotBool ElementOKforProfile (Element el, Document doc)
{
  ElementType    elType;
  char           *name;
  AttributeType  attrType;
  int            kind;
  Attribute      attr, nextAttr;
  Element        child;
  ThotBool       ok, record;

  ok = TRUE;
  /* handle only HTML elements */
  elType = TtaGetElementType (el);
  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
    /* it's an element from the HTML namespace */
    if (TtaGetDocumentProfile (doc) != L_Other)
      /* the document profile accepts only certain elements and attributes */
      {
        name = GetXMLElementName (elType, doc);
	if (name == NULL || name[0] == EOS)
	  /* this element type is not acceptend in the document profile */
	  ok = FALSE;
	else
	  /* the element type is OK for the profile */
	  {
	    /* check all attributes of the element */
	    attr = 0;
	    TtaNextAttribute (el, &attr);
	    while (attr)
	      {
		nextAttr = attr;  TtaNextAttribute (el, &nextAttr);
		TtaGiveAttributeType (attr, &attrType, &kind);
		name = GetXMLAttributeName (attrType, elType, doc);
		if (name == NULL || name[0] == EOS)
		  /* this attribute is not valid for this element in the
		     document profile. Delete it */
		  TtaRemoveAttribute (el, attr, doc);
		attr = nextAttr;
	      }
	  }
      }
  if (!ok)
    /* The element type is not acceptend in the document profile.
       Delete the element but keep its children if they are allowed */
    {
      record = FALSE;
      if (TtaGetLastCreatedElemInHistory (doc) == el)
	/* the last item in the undo history is the creation of this element,
	   but this element will be removed. Register its children instead */
	{
	  TtaCancelLastRegisteredOperation (doc);
	  record = TRUE;
	}
      /* move all children right before the element */
      child = TtaGetFirstChild (el);
      while (child)
	{
	  /* check that this child is allowed. If not, delete it */
	  if (ElementOKforProfile (child, doc))
	    /* this child is allowed. Move it and remove all invalid elements
	       and attributes from its decendants */
	    {
	      TtaRemoveTree (child, doc);
	      CheckDescendants (child, doc);
	      TtaInsertSibling (child, el, TRUE, doc);
	      if (record)
		TtaRegisterElementCreate (child, doc);
	    }
	  /* take the first child of el, not the sibling of child, as
	     child may have been deleted by ElementOKforProfile */
	  child = TtaGetFirstChild (el);
	}
      /* the element is now empty. Delete it. */
      TtaDeleteTree (el, doc);
    }
  return ok;
}

/*----------------------------------------------------------------------
   CheckDescendants
   Element el is a valid element in the document profile. Remove the
   invalid elements and attributes from its decendants.
  ----------------------------------------------------------------------*/
static void CheckDescendants (Element el, Document doc)
{
  Element child, nextChild;

  child = TtaGetFirstChild (el);
  while (child)
    {
      nextChild = child;  TtaNextSibling (&nextChild);
      if (ElementOKforProfile (child, doc))
	CheckDescendants (child, doc);
      child = nextChild;
    }
}

/*----------------------------------------------------------------------
   ElementPasted
   This function is called for each element pasted by the user, and for
   each element within the pasted element.
   An element has been pasted in a HTML document.
   Check Pseudo paragraphs.
   If the pasted element has a NAME attribute, change its value if this
   NAME is already used in the document.
   If it's within the TITLE element, update the corresponding field in
   the Formatted window.
  ----------------------------------------------------------------------*/
void ElementPasted (NotifyElement * event)
{
  Document            originDocument, doc;
  Language            lang;
  Element             el, anchor, child, previous, nextchild, parent;
  ElementType         elType, parentType;
  AttributeType       attrType;
  Attribute           attr;
  SSchema             HTMLschema;
  CSSInfoPtr          css;
  char               *value;
  int                 length, oldStructureChecking;

  el = event->element;
  doc = event->document;
  if (!ElementOKforProfile (el, doc))
    return;
  HTMLschema = TtaGetSSchema ("HTML", doc);
  CheckPseudoParagraph (el, doc);
  /* Check attribute NAME or ID in order to make sure that its value */
  /* is unique in the document */
  MakeUniqueName (el, doc);

  elType = TtaGetElementType (el);
  anchor = NULL;
  if (elType.ElSSchema == HTMLschema && elType.ElTypeNum == HTML_EL_Anchor)
      anchor = el;
  else if (elType.ElSSchema == HTMLschema && elType.ElTypeNum == HTML_EL_LINK)
    {
      /* check if it's a CSS link */
      CheckCSSLink (el, doc, HTMLschema);
    }
  else if (elType.ElSSchema == HTMLschema &&
	   elType.ElTypeNum == HTML_EL_STYLE_)
    {
      /* The pasted element is a STYLE element in the HEAD */
      /* Get its content */
      child = TtaGetFirstChild (el);
      length = TtaGetTextLength (child);
      value = TtaGetMemory (length + 1);
      TtaGiveTextContent (child, value, &length, &lang);
      /* parse the content */
      css = AddCSS (doc, doc, CSS_DOCUMENT_STYLE, NULL, NULL, el);
      ReadCSSRules (doc, css, value, NULL, TtaGetElementLineNumber (child),
		    FALSE, el, NULL);
      TtaFreeMemory (value);
    }
  else if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
    {  
      parent = TtaGetParent (event->element);
      parentType = TtaGetElementType (parent);
      if (TtaSameSSchemas (parentType.ElSSchema, HTMLschema) &&
          parentType.ElTypeNum == HTML_EL_TITLE)
         /* the parent of the pasted text is the TITLE element */
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
		      value = TtaGetMemory (MAX_LENGTH);
		      if (value != NULL)
			{
			  /* get the SRC itself */
			  TtaGiveTextAttributeValue (attr, value, &length);
			  /* update value and SRCattribute */
			  ComputeSRCattribute (el, doc, originDocument,
					       attr, value);
			}
		      TtaFreeMemory (value);
		    }
		}
	    }
	}
    }
  
  if (anchor != NULL)
    {
      TtaSetDisplayMode (doc, DeferredDisplay);
      oldStructureChecking = TtaGetStructureChecking (doc);
      TtaSetStructureChecking (0, doc);
      /* Is there a parent anchor? */
      parent = TtaGetTypedAncestor (el, elType);
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
	  /* Change attributes HREF if the element comes from another */
	  /* document */
	  originDocument = (Document) event->position;
	  if (originDocument >= 0 && originDocument != doc)
	    {
	      /* the anchor has moved from one document to another */
	      /* get the HREF attribute of element Anchor */
	      attrType.AttrSSchema = elType.ElSSchema;
	      attrType.AttrTypeNum = HTML_ATTR_HREF_;
	      attr = TtaGetAttribute (anchor, attrType);
	      if (attr != NULL)
                 ChangeURI (anchor, attr, originDocument, doc);
	    }
	}
      TtaSetStructureChecking ((ThotBool)oldStructureChecking, doc);
      TtaSetDisplayMode (doc, DisplayImmediately);
    }
}

/*----------------------------------------------------------------------
   CheckNewLines
   Some new text has been pasted or typed in a text element. Check the
   NewLine characters and replace them by spaces, except in a PRE
 -----------------------------------------------------------------------*/
void CheckNewLines (NotifyOnTarget *event)
{
  Element     ancestor;
  ElementType elType;
  char       *content;
  int         length, i;
  Language    lang;
  ThotBool    changed, pre;

  if (!event->target)
     return;
  length = TtaGetTextLength (event->target);
  if (length == 0)
     return;

  /* is there a preformatted (or equivalent) ancestor? */
  pre = FALSE;
  ancestor = TtaGetParent (event->target);
  while (ancestor && !pre)
    {
    elType = TtaGetElementType (ancestor);
    if ((strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0) &&
        (elType.ElTypeNum == HTML_EL_STYLE_ ||
         elType.ElTypeNum == HTML_EL_SCRIPT_ ||
	 elType.ElTypeNum == HTML_EL_Preformatted ||
	 elType.ElTypeNum == HTML_EL_Text_Area))
       pre = TRUE;
    else
       ancestor = TtaGetParent (ancestor);
    }
  if (pre)
     /* there is a preformatted ancestor. Don't change anything */
     return;

  /* replace every new line in the content of the element by a space */
  length++;
  content = TtaGetMemory (length);
  TtaGiveTextContent (event->target, content, &length, &lang);
  changed = FALSE;
  for (i = 0; i < length; i++)
     if (content[i] == (char) EOL)
       {
        content[i] = SPACE;
	changed = TRUE;
       }
  if (changed)
     /* at least 1 new line has been replaced by a space */
     TtaSetTextContent (event->target, content, lang, event->document);
  TtaFreeMemory (content);
}

/*----------------------------------------------------------------------
   CreateTarget
   Create a target element.                          
  ----------------------------------------------------------------------*/
void CreateTarget (Document doc, View view)
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
void UpdateAttrID (NotifyAttribute * event)
{
   Element	firstChild, lastChild;

   if (event->event == TteAttrDelete)
      /* if the element is a SPAN without any other attribute, remove the SPAN
         element */
      DeleteSpanIfNoAttr (event->element, event->document, &firstChild,
			  &lastChild);
   else
      {
      MakeUniqueName (event->element, event->document);
      if (event->event == TteAttrCreate)
         /* if the ID attribute is on a text string, create a SPAN element that
         encloses this text string and move the ID attribute to that SPAN
         element */
         AttrToSpan (event->element, event->attribute, event->document);
      }
#ifdef _SVGANIM
   Update_element_id_on_timeline (event); 
#endif /* _SVGANIM */
}


/*----------------------------------------------------------------------
   CoordsModified
   Update x_ccord, y_coord, width, height or polyline according to the
   new coords value.             
  ----------------------------------------------------------------------*/
void CoordsModified (NotifyAttribute * event)
{
   ParseAreaCoords (event->element, event->document);
}


/*----------------------------------------------------------------------
   GraphicsModified
   Update coords attribute value according to the new coord value.
  ----------------------------------------------------------------------*/
void GraphicsModified (NotifyAttribute * event)
{
   Element             el;
   ElementType         elType;
   AttributeType       attrType;
   Attribute	       attr;
   char		       buffer[15];

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
	      sprintf (buffer, "%d",
			TtaGetAttributeValue (event->attribute));
	      TtaSetAttributeText (attr, buffer, el, event->document);
	      }
	 }
       else if (event->attributeType.AttrTypeNum == HTML_ATTR_IntHeightPxl)
	 {
	   UpdateImageMap (el, event->document, -1, OldHeight);
	   OldHeight = -1;
	   /* update attribute Height_ */
	   attrType.AttrSSchema = event->attributeType.AttrSSchema;
	   attrType.AttrTypeNum = HTML_ATTR_Height_;
	   attr = TtaGetAttribute (el, attrType);
	   if (attr)
	      {
	      sprintf (buffer, "%d",
			TtaGetAttributeValue (event->attribute));
	      TtaSetAttributeText (attr, buffer, el, event->document);
	      }
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
   StoreWidth
   Attribute IntWidthPxl will be changed, store the old value.
  ----------------------------------------------------------------------*/
ThotBool StoreWidth (NotifyAttribute *event)
{
  ElementType	     elType;
  int                h;

  elType = TtaGetElementType (event->element);
  if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
    TtaGiveBoxSize (event->element, event->document, 1, UnPixel, &OldWidth,&h);
  else
    OldWidth = -1;
  return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   StoreHeight
   Attribute height_ will be changed, store the old value.
  ----------------------------------------------------------------------*/
ThotBool StoreHeight (NotifyAttribute * event)
{
  ElementType	     elType;
  int                w;

  elType = TtaGetElementType (event->element);
  if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
    TtaGiveBoxSize (event->element, event->document, 1, UnPixel, &w,
		    &OldHeight);
  else
     OldHeight = -1;
   return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   AttrHeightDelete
   An attribute Height_ will be deleted.   
   Delete the corresponding attribute IntHeightPercent or   
   IntHeightPxl.                                            
  ----------------------------------------------------------------------*/
ThotBool AttrHeightDelete (NotifyAttribute *event)
{
   AttributeType       attrType;
   Attribute           attr;

   StoreHeight (event);
   attrType = event->attributeType;
   attrType.AttrTypeNum = HTML_ATTR_IntHeightPxl;
   attr = TtaGetAttribute (event->element, attrType);
   if (attr == NULL)
     {
	attrType.AttrTypeNum = HTML_ATTR_IntHeightPercent;
	attr = TtaGetAttribute (event->element, attrType);
     }
   if (attr != NULL)
      TtaRemoveAttribute (event->element, attr, event->document);
   return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   AttrHeightModifed
   An attribute Height_ has been created or modified.
   Create the corresponding attribute IntHeightPercent or IntHeightPxl.
  ----------------------------------------------------------------------*/
void AttrHeightModified (NotifyAttribute *event)
{
  char               *buffer;
  int                 length;

  length = buflen - 1;
  buffer = TtaGetMemory (buflen);
  TtaGiveTextAttributeValue (event->attribute, buffer, &length);
  CreateAttrHeightPercentPxl (buffer, event->element, event->document,
			     OldHeight);
  TtaFreeMemory (buffer);
  OldHeight = -1;
}

/*----------------------------------------------------------------------
   AttrWidthDelete
   An attribute Width__ will be deleted.   
   Delete the corresponding attribute IntWidthPercent or   
   IntWidthPxl.                                            
  ----------------------------------------------------------------------*/
ThotBool AttrWidthDelete (NotifyAttribute *event)
{
   AttributeType       attrType;
   Attribute           attr;

   StoreWidth (event);
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
   AttrWidthModifed
   An attribute Width__ has been created or modified.
   Create the corresponding attribute IntWidthPercent or IntWidthPxl.
  ----------------------------------------------------------------------*/
void AttrWidthModified (NotifyAttribute *event)
{
  char               *buffer;
  int                 length;

  length = buflen - 1;
  buffer = TtaGetMemory (buflen);
  TtaGiveTextAttributeValue (event->attribute, buffer, &length);
  CreateAttrWidthPercentPxl (buffer, event->element, event->document,
			     OldWidth);
  TtaFreeMemory (buffer);
  OldWidth = -1;
}

/*----------------------------------------------------------------------
   AttrFontSizeCreated
   an HTML attribute "size" has been created for a Font element.   
   Create the corresponding internal attribute.                    
  ----------------------------------------------------------------------*/
void AttrFontSizeCreated (NotifyAttribute *event)
{
   char               *buffer = TtaGetMemory (buflen);
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
   AttrFontSizeDelete
   An HTML attribute "size" has been deleted for a Font element.   
   Delete the corresponding internal attribute.                    
  ----------------------------------------------------------------------*/
ThotBool AttrFontSizeDelete (NotifyAttribute * event)
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
   AttrColorCreated
   An attribute color, TextColor or BackgroundColor has been       
   created or modified.                                            
  ----------------------------------------------------------------------*/
void AttrColorCreated (NotifyAttribute * event)
{
   char            *value = TtaGetMemory (buflen);
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
      HTMLSetBackgroundImage (event->document, event->element, STYLE_REPEAT,
			      value);
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
   AttrColorDelete
   An attribute color, TextColor or BackgroundColor is being       
   deleted.                                                        
  ----------------------------------------------------------------------*/
ThotBool AttrColorDelete (NotifyAttribute * event)
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
   ListItemCreated
   An element List_Item has been created or pasted. Set its        
   IntItemStyle attribute according to its surrounding elements.   
  ----------------------------------------------------------------------*/
void ListItemCreated (NotifyElement * event)
{
  if (event->event == TteElemPaste)
    if (!ElementOKforProfile (event->element, event->document))
      return;
   SetAttrIntItemStyle (event->element, event->document);
}

/*----------------------------------------------------------------------
   SetItemStyleSubtree
   Set the IntItemStyle attribute of all List_Item elements in the 
   el subtree.                                                     
  ----------------------------------------------------------------------*/
static void SetItemStyleSubtree (Element el, Document doc)
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
   ListChangedType
   An element Unnumbered_List or Numbered_List has changed type.   
   Set the IntItemStyle attribute for all enclosed List_Items      
  ----------------------------------------------------------------------*/
void ListChangedType (NotifyElement * event)
{
   SetItemStyleSubtree (event->element, event->document);
}

/*----------------------------------------------------------------------
   An attribute BulletStyle or NumberStyle has been created,       
   deleted or modified for a list. Create or updated the           
   corresponding IntItemStyle attribute for all items of the list. 
  ----------------------------------------------------------------------*/
void UpdateAttrIntItemStyle (NotifyAttribute * event)
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
   AttrItemStyle
   An attribute ItemStyle has been created, updated or deleted.    
   Create or update the corresponding IntItemStyle attribute.      
  ----------------------------------------------------------------------*/
void AttrItemStyle (NotifyAttribute * event)
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
   Called by Thot when building the Attributes menu.
   Prevent Thot from including a global attribute in the menu if the selected
   element does not accept this attribute.
  ----------------------------------------------------------------------*/
ThotBool GlobalAttrInMenu (NotifyAttribute * event)
{
   ElementType         elType, parentType;
   Element             parent;
   char               *attr;

   elType = TtaGetElementType (event->element);

   /* don't put any HTML attribute on Thot elements that are not
      HTML elements */
   if (elType.ElTypeNum == HTML_EL_Invalid_element ||
       elType.ElTypeNum == HTML_EL_Unknown_namespace ||
       elType.ElTypeNum == HTML_EL_Comment_ ||
       elType.ElTypeNum == HTML_EL_Comment_line ||
       elType.ElTypeNum == HTML_EL_XMLPI ||
       elType.ElTypeNum == HTML_EL_PI_line ||
       elType.ElTypeNum == HTML_EL_DOCTYPE ||
       elType.ElTypeNum == HTML_EL_DOCTYPE_line)
     return TRUE;

   /* don't put any attribute on text fragments that are within DOCTYPE,
      comments, PIs, etc. */
   if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
     {
       parent = TtaGetParent (event->element);
       if (parent)
	 {
	   parentType = TtaGetElementType (parent);
	   if (parentType.ElTypeNum == HTML_EL_Invalid_element ||
	       parentType.ElTypeNum == HTML_EL_Unknown_namespace ||
	       parentType.ElTypeNum == HTML_EL_Comment_ ||
	       parentType.ElTypeNum == HTML_EL_Comment_line ||
	       parentType.ElTypeNum == HTML_EL_XMLPI ||
	       parentType.ElTypeNum == HTML_EL_PI_line ||
	       parentType.ElTypeNum == HTML_EL_DOCTYPE ||
	       parentType.ElTypeNum == HTML_EL_DOCTYPE_line)
	     return TRUE;
	 }
     }

   attr = GetXMLAttributeName (event->attributeType, elType, event->document);
   if (attr[0] == EOS)
      return TRUE;	/* don't put an invalid attribute in the menu */

   /* handle only Global attributes */
   if (event->attributeType.AttrTypeNum != HTML_ATTR_ID &&
       event->attributeType.AttrTypeNum != HTML_ATTR_Class &&
       event->attributeType.AttrTypeNum != HTML_ATTR_Style_ &&
       event->attributeType.AttrTypeNum != HTML_ATTR_Title &&
       event->attributeType.AttrTypeNum != HTML_ATTR_dir &&
       event->attributeType.AttrTypeNum != HTML_ATTR_onclick &&
       event->attributeType.AttrTypeNum != HTML_ATTR_ondblclick &&
       event->attributeType.AttrTypeNum != HTML_ATTR_onmousedown &&
       event->attributeType.AttrTypeNum != HTML_ATTR_onmouseup &&
       event->attributeType.AttrTypeNum != HTML_ATTR_onmouseover &&
       event->attributeType.AttrTypeNum != HTML_ATTR_onmousemove &&
       event->attributeType.AttrTypeNum != HTML_ATTR_onmouseout &&
       event->attributeType.AttrTypeNum != HTML_ATTR_onkeypress &&
       event->attributeType.AttrTypeNum != HTML_ATTR_onkeydown &&
       event->attributeType.AttrTypeNum != HTML_ATTR_onkeyup &&
       event->attributeType.AttrTypeNum != HTML_ATTR_xml_space)
     /* it's not a global attribute. Accept it */
     return FALSE;

   if (strcmp (TtaGetSSchemaName (elType.ElSSchema),"HTML"))
     /* it's not a HTML element */
     return TRUE;
   else
      /* it's a HTML element */
     {
       /* BASE and SCRIPT do not accept any global attribute */
       if (elType.ElTypeNum == HTML_EL_BASE ||
	   elType.ElTypeNum == HTML_EL_SCRIPT_ ||
	   elType.ElTypeNum == HTML_EL_Element)
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
	 {
	 if (elType.ElTypeNum == HTML_EL_HEAD ||
	     elType.ElTypeNum == HTML_EL_TITLE ||
	     elType.ElTypeNum == HTML_EL_META ||
	     elType.ElTypeNum == HTML_EL_STYLE_ ||
	     elType.ElTypeNum == HTML_EL_HTML)
	   /* HEAD, TITLE, META, STYLE and HTML don't accept coreattrs */
	   return TRUE;
	 else
	   return FALSE; /* let Thot perform normal operation */
	 }
       /* i18n */
       if (event->attributeType.AttrTypeNum == HTML_ATTR_dir ||
	   event->attributeType.AttrTypeNum == HTML_ATTR_Language)
	 {
	 if (elType.ElTypeNum == HTML_EL_BR ||
	     elType.ElTypeNum == HTML_EL_Applet ||
	     elType.ElTypeNum == HTML_EL_Horizontal_Rule ||
	     elType.ElTypeNum == HTML_EL_FRAMESET ||
	     elType.ElTypeNum == HTML_EL_FRAME ||
	     elType.ElTypeNum == HTML_EL_IFRAME)
	   return TRUE;
	 else
	   return FALSE;
	 }
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
       /* xml:space attribute */
       if (event->attributeType.AttrTypeNum == HTML_ATTR_xml_space)
	 /* be careful here -- document may not have been received yet */
	 if (DocumentMeta[event->document] == NULL
	     || DocumentMeta[event->document]->xmlformat == FALSE)
	   return TRUE;	 

       return FALSE;
     }
   return TRUE;	/* don't put an invalid attribute in the menu */
}

/*----------------------------------------------------------------------
   AttrNAMEinMenu
   doesn't display NAME in Reset_Input and Submit_Input
  ----------------------------------------------------------------------*/
ThotBool            AttrNAMEinMenu (NotifyAttribute * event)
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
   SetOnOffEmphasis
   The Emphasis button or menu item has been clicked
  ----------------------------------------------------------------------*/
void SetOnOffEmphasis (Document document, View view)
{
  Element             selectedEl;
  ElementType         elType;
  int                 firstSelectedChar, lastSelectedChar;

  TtaGiveFirstSelectedElement (document, &selectedEl, &firstSelectedChar,
			       &lastSelectedChar);
  if (selectedEl)
    {
    elType = TtaGetElementType (selectedEl);
    if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
       /* it's a HTML element */
       SetCharFontOrPhrase (document, HTML_EL_Emphasis);
    else if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "MathML"))
       /* it's a MathML element */
       SetMathCharFont (document, MathML_ATTR_fontstyle);
    }
}

/*----------------------------------------------------------------------
   SetOnOffStrong
   The Strong button or menu item has been clicked
  ----------------------------------------------------------------------*/
void SetOnOffStrong (Document document, View view)
{
  Element             selectedEl;
  ElementType         elType;
  int                 firstSelectedChar, lastSelectedChar;

  TtaGiveFirstSelectedElement (document, &selectedEl, &firstSelectedChar,
			       &lastSelectedChar);
  if (selectedEl)
    {
    elType = TtaGetElementType (selectedEl);
    if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
       /* it's a HTML element */
       SetCharFontOrPhrase (document, HTML_EL_Strong);
    else if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "MathML"))
       /* it's a MathML element */
       SetMathCharFont (document, MathML_ATTR_fontweight);
    }
}

/*----------------------------------------------------------------------
   SetOnOffCite
  ----------------------------------------------------------------------*/
void SetOnOffCite (Document document, View view)
{
  SetCharFontOrPhrase (document, HTML_EL_Cite);
}


/*----------------------------------------------------------------------
   SetOnOffDefinition
  ----------------------------------------------------------------------*/
void SetOnOffDefinition (Document document, View view)
{
  SetCharFontOrPhrase (document, HTML_EL_Def);
}


/*----------------------------------------------------------------------
   SetOnOffCode
   The Code button or menu item has been clicked
  ----------------------------------------------------------------------*/
void SetOnOffCode (Document document, View view)
{
  Element             selectedEl;
  ElementType         elType;
  int                 firstSelectedChar, lastSelectedChar;

  TtaGiveFirstSelectedElement (document, &selectedEl, &firstSelectedChar,
			       &lastSelectedChar);
  if (selectedEl)
    {
    elType = TtaGetElementType (selectedEl);
    if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
       /* it's a HTML element */
       SetCharFontOrPhrase (document, HTML_EL_Code);
    else if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "MathML"))
       /* it's a MathML element */
       SetMathCharFont (document, MathML_ATTR_fontfamily);
    }
}


/*----------------------------------------------------------------------
  SetOnOffVariable
  ----------------------------------------------------------------------*/
void SetOnOffVariable (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_Variable);
}


/*----------------------------------------------------------------------
  SetOnOffSample
  ----------------------------------------------------------------------*/
void SetOnOffSample (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_Sample);
}


/*----------------------------------------------------------------------
  SetOnOffKeyboard
  ----------------------------------------------------------------------*/
void SetOnOffKeyboard (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_Keyboard);
}


/*----------------------------------------------------------------------
  SetOnOffAbbr
  ----------------------------------------------------------------------*/
void SetOnOffAbbr (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_ABBR);
}


/*----------------------------------------------------------------------
  SetOnOffAcronym
  ----------------------------------------------------------------------*/
void SetOnOffAcronym (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_ACRONYM);
}


/*----------------------------------------------------------------------
  SetOnOffINS
  ----------------------------------------------------------------------*/
void SetOnOffINS (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_INS);
}


/*----------------------------------------------------------------------
  SetOnOffDEL
  ----------------------------------------------------------------------*/
void SetOnOffDEL (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_DEL);
}


/*----------------------------------------------------------------------
  SetOnOffItalic
  ----------------------------------------------------------------------*/
void SetOnOffItalic (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_Italic_text);
}


/*----------------------------------------------------------------------
  SetOnOffBold
  ----------------------------------------------------------------------*/
void SetOnOffBold (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_Bold_text);
}


/*----------------------------------------------------------------------
  SetOnOffTeletype
  ----------------------------------------------------------------------*/
void SetOnOffTeletype (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_Teletype_text);
}


/*----------------------------------------------------------------------
  SetOnOffBig
  ----------------------------------------------------------------------*/
void SetOnOffBig (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_Big_text);
}


/*----------------------------------------------------------------------
  SetOnOffSmall
  ----------------------------------------------------------------------*/
void SetOnOffSmall (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_Small_text);
}


/*----------------------------------------------------------------------
  SetOnOffSub
  ----------------------------------------------------------------------*/
void SetOnOffSub (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_Subscript);
}


/*----------------------------------------------------------------------
  SetOnOffSup
  ----------------------------------------------------------------------*/
void SetOnOffSup (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_Superscript);
}


/*----------------------------------------------------------------------
  SetOnOffQuotation
  ----------------------------------------------------------------------*/
void SetOnOffQuotation (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_Quotation);
}


/*----------------------------------------------------------------------
  SetOnOffBDO
  ----------------------------------------------------------------------*/
void SetOnOffBDO (Document document, View view)
{
   SetCharFontOrPhrase (document, HTML_EL_BDO);
}

/*----------------------------------------------------------------------
  SearchAnchor
  Return the enclosing anchor element.
  If name is true, take into account Anchor(HTML) elements with a name
  attribute.
  ----------------------------------------------------------------------*/
Element SearchAnchor (Document doc, Element element, Attribute *HrefAttr,
		      ThotBool name)
{
   AttributeType       attrType;
   Attribute           attr;
   ElementType         elType;
   Element             elAnchor, ancestor;
   SSchema             XLinkSchema;
   ThotBool            found;
   View                activeView;
   Document            activeDoc;

   elAnchor = NULL;
   *HrefAttr = NULL;
   /* get the XLink SSchema for the document, if the document uses it */
   XLinkSchema = TtaGetSSchema ("XLink", doc);
   /* check the element and its ancestors */
   ancestor = element;
   do
     {
       attr = NULL;
       elType = TtaGetElementType (ancestor);
       if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
	 /* the current element belongs to the HTML namespace */
	 {
	   attrType.AttrSSchema = elType.ElSSchema;
	   if (name && elType.ElTypeNum == HTML_EL_Anchor)
	     /* look for a name attribute */
	     {
	       attrType.AttrTypeNum = HTML_ATTR_NAME;
	       attr = TtaGetAttribute (ancestor, attrType);
	     }
	   if (!attr)
	     {
	       if (elType.ElTypeNum == HTML_EL_LINK ||
		   elType.ElTypeNum == HTML_EL_Anchor ||
		   elType.ElTypeNum == HTML_EL_AREA)
		 {
		   /* look for a href attribute */
		   attrType.AttrTypeNum = HTML_ATTR_HREF_;
		   attr = TtaGetAttribute (ancestor, attrType);
		 }
	       else if (elType.ElTypeNum == HTML_EL_FRAME)
		 {
		   /* look for a src attribute */
		   attrType.AttrTypeNum = HTML_ATTR_FrameSrc;
		   attr = TtaGetAttribute (ancestor, attrType);
		 }
	       else if (elType.ElTypeNum == HTML_EL_Block_Quote ||
			elType.ElTypeNum == HTML_EL_Quotation ||
			elType.ElTypeNum == HTML_EL_INS ||
			elType.ElTypeNum == HTML_EL_DEL)
		 /* look for a cite attribute */
		 {
		   attrType.AttrTypeNum = HTML_ATTR_cite;
		   attr = TtaGetAttribute (ancestor, attrType);
		 }
	     }
	 }
       else if (!attr &&
		!strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG"))
	 /* the current element belongs to the SVG namespace */
	 {
	   found = FALSE;
	   if (elType.ElTypeNum == SVG_EL_a)
	     found = TRUE;
	   else if (elType.ElTypeNum == SVG_EL_use_ ||
		    elType.ElTypeNum == SVG_EL_tref)
	     /* it's a use or tref element. Consider it only if the active
		view is the Structure view, to allow anchors to be activated
		in the main view, even if they contain a use element */
	     {
	       TtaGetActiveView (&activeDoc, &activeView);
	       if (activeDoc == doc && activeView != 0)
		 if (!strcmp (TtaGetViewName (doc, activeView), "Structure_view"))
		   found = TRUE;
	     }
	   if (found)
	     /* look for the corresponding href attribute */
	     {
	       attrType.AttrSSchema = elType.ElSSchema;
	       attrType.AttrTypeNum = SVG_ATTR_xlink_href;
	       attr = TtaGetAttribute (ancestor, attrType);
	     }
	 }
       else if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "XLink"))
	 {
	   attrType.AttrSSchema = elType.ElSSchema;
	   attrType.AttrTypeNum = XLink_ATTR_href_;
	   attr = TtaGetAttribute (ancestor, attrType);
	 }
       if (!attr && XLinkSchema)
	 /* the document uses XLink. Check whether the current element has
	    a xlink:href attribute */
	 {
	   attrType.AttrSSchema = XLinkSchema;
	   attrType.AttrTypeNum = XLink_ATTR_href_;
	   attr = TtaGetAttribute (ancestor, attrType);
	 }
       if (attr)
	 {
	   elAnchor = ancestor;
	   *HrefAttr = attr;
	 }
       else
	 ancestor = TtaGetParent (ancestor);
     }
   while (elAnchor == NULL && ancestor != NULL);
   return elAnchor;
}

/*----------------------------------------------------------------------
   UpdateAtom
   On X-Windows, update the content of atom BROWSER_HISTORY_INFO with
   title and url of current doc  
   cf: http://zenon.inria.fr/koala/colas/browser-history/       
  ----------------------------------------------------------------------*/
void UpdateAtom (Document doc, char *url, char *title)
{
#ifndef _GTK
#ifndef _WINDOWS
   static Atom         property_name = 0;
   char               *v;
   int                 v_size;
   Display            *dpy = TtaGetCurrentDisplay ();
   ThotWindow          win;
   ThotWidget	       frame;

   frame = TtaGetViewFrame (doc, 1);
   if (frame == 0)
      return;
   win = XtWindow (XtParent (XtParent (XtParent (frame))));
   /* 13 is strlen("URL=0TITLE=00") */
   v_size = strlen (title) + strlen (url) + 13;
   v = TtaGetMemory (v_size);
   sprintf (v, "URL=%s%cTITLE=%s%c", url, 0, title, 0);
   if (!property_name)
      property_name = XInternAtom (dpy, "BROWSER_HISTORY_INFO", FALSE);
   XChangeProperty (dpy, win, property_name, XA_STRING, 8, PropModeReplace,
		    v, v_size);
   TtaFreeMemory (v);
#endif /* _WINDOWS */
#endif /* _GTK */
}

