/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2004
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

#ifdef _WX
  #include "wx/wx.h"
#endif /* _WX */

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
#include "EDITstyle_f.h"
#include "EDITORactions_f.h"
#include "fetchHTMLname_f.h"
#include "fetchXMLname_f.h"
#include "SVGbuilder_f.h"
#include "anim_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLimage_f.h"
#include "HTMLpresentation_f.h"
#include "html2thot_f.h"
#include "init_f.h"
#include "Mathedit_f.h"
#include "MathMLbuilder_f.h"
#include "styleparser_f.h"
#include "XHTMLbuilder_f.h"
#include "XLinkedit_f.h"
#include "tree.h"
#include "interface.h"

#ifdef _WINGUI
  #include "wininclude.h"
#endif /* _WINGUI */
#ifdef _WX
  #include "wxdialogapi_f.h"
#endif /* _WX */

/*----------------------------------------------------------------------
  InsertBefore inserts an empty element before the selection
  ----------------------------------------------------------------------*/
void InsertBefore (Document doc, View view)
{
  TtaInsertAnyElement (doc, TRUE);
}

/*----------------------------------------------------------------------
  InsertAfter inserts an empty element after the selection
  ----------------------------------------------------------------------*/
void InsertAfter (Document doc, View view)
{
  TtaInsertAnyElement (doc, FALSE);
}

/*----------------------------------------------------------------------
   SetTargetContent
   Set the new value of Target.                  
  ----------------------------------------------------------------------*/
void SetTargetContent (Document doc, Attribute attrNAME)
{
   int                 length;

   /* the document that issues the command Set target becomes the target doc */
   if (TargetDocumentURL)
      TtaFreeMemory (TargetDocumentURL);
   if (doc)
      {
       length = strlen (DocumentURLs[doc]);
       TargetDocumentURL = (char *)TtaGetMemory (length + 1);
       strcpy (TargetDocumentURL, DocumentURLs[doc]);
     }

   if (TargetName)
     {
	TtaFreeMemory (TargetName);
	TargetName = NULL;
     }
   if (attrNAME)
     {
	/* get a buffer for the NAME */
	length = TtaGetTextAttributeLength (attrNAME);
	TargetName = (char *)TtaGetMemory (length + 1);
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
  if ((strcmp (s, "HTML") == 0) &&
      (elType.ElTypeNum != HTML_EL_XMLPI))
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
	      RemoveStyle (pathname, doc, TRUE, TRUE, NULL, CSS_EXTERNAL_STYLE);
	    }
	}
    }
  else
    {
      if (strcmp (s, "HTML") == 0)
	piNum = HTML_EL_XMLPI;
      else if (strcmp (s, "MathML") == 0)
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
	      TtaGiveTextContent (elText, (unsigned char *)buffer, &length, &lang);
	      buffer[length++] = EOS;
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
	      RemoveStyle (pathname, doc, TRUE, TRUE, NULL, CSS_EXTERNAL_STYLE);
	    }
	}
    }
  return;
}

/*----------------------------------------------------------------------
   DeleteLink                                              
  ----------------------------------------------------------------------*/
ThotBool DeleteLink (NotifyElement *event)
{
  RemoveLink (event->element, event->document);
  return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  CheckMandatory
  Check whether the attribute could be removed.
  ----------------------------------------------------------------------*/
ThotBool CheckMandatory (NotifyAttribute *event)
{
  AttributeType       attrType;
  ElementType         elType;

  attrType.AttrSSchema = event->attributeType.AttrSSchema;
  attrType.AttrTypeNum = event->attributeType.AttrTypeNum;
  elType = TtaGetElementType (event->element);
  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") &&
      elType.ElTypeNum == HTML_EL_Anchor)
    /* it's an anchor. It should have at least a name or href attribute */
    {
      if (attrType.AttrTypeNum == HTML_ATTR_HREF_)
	/* the user wants to remove the href attribute.
	   check if there is a name or an id attribute */
	{
	  attrType.AttrTypeNum = HTML_ATTR_NAME;
	  if (!TtaGetAttribute (event->element, attrType))
	    /* there is no name attribute. Check id */
	    {
	      attrType.AttrTypeNum = HTML_ATTR_ID;
	      if (!TtaGetAttribute (event->element, attrType))
		/* no id attribute. Refuse. */
		return TRUE;
	    }
	}
      else
	/* the user wants to remove the name attribute.
	   check if there is a href or id attribute */
	{
	  attrType.AttrTypeNum = HTML_ATTR_HREF_;
	  if (!TtaGetAttribute (event->element, attrType))
	    /* there is no href attribute. Check id */
	    {
	      attrType.AttrTypeNum = HTML_ATTR_ID;
	      if (!TtaGetAttribute (event->element, attrType))
		/* no id attribute. Refuse. */
		return TRUE;
	    }
	}
    }
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
   char               *value, *base, *s;
   char                tempURL[MAX_LENGTH];
   char                buffer[MAX_LENGTH];
   int                 length, piNum;
   ThotBool            new_, oldStructureChecking;
   ThotBool            isHTML, isSVG;

   attr = 0;
   new_ = FALSE;  
   
   if (AttrHREFundoable)
     TtaOpenUndoSequence (doc, element, element, 0, 0);

   elType = TtaGetElementType (element);
   s = TtaGetSSchemaName (elType.ElSSchema);
   isHTML = !strcmp (s, "HTML");
   isSVG = !strcmp (s, "SVG");

   if (!LinkAsXmlCSS)
     /* It isn't a link to an xml stylesheet */
     {
       if (isHTML)
	 {
	   attrType.AttrSSchema = elType.ElSSchema;
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
#ifdef _SVG
       else if (isSVG)
	 {
	   attrType.AttrSSchema = elType.ElSSchema;
	   attrType.AttrTypeNum = SVG_ATTR_xlink_href;
	 }
#endif /* _SVG */
       else
	 {
	   /* the origin of the link is not a HTML element */
	   /* create a XLink link */
	   attrType.AttrSSchema = TtaGetSSchema ("XLink", doc);
	   if (!attrType.AttrSSchema)
	     attrType.AttrSSchema = TtaNewNature (doc, TtaGetDocumentSSchema (doc),
						  NULL, "XLink", "XLinkP");
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
	   if (!isHTML && !isSVG)
	     {
	       /* Attach the XLink namespace declaration */
	       TtaSetUriSSchema (attrType.AttrSSchema, XLink_URI);
	       TtaSetANamespaceDeclaration (doc, element, XLink_PREFIX, XLink_URI);
	     }
	   new_ = TRUE;
	 }
       else
	 {
	   new_ = FALSE;
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
		   /* don't manage a document used by make book */
		   if (DocumentMeta[doc] == NULL ||
		       DocumentMeta[doc]->method != CE_MAKEBOOK)
		     LoadStyleSheet (targetURL, doc, piEl, NULL, CSS_ALL, FALSE);
		   /* We use the Latin_Script language to avoid the spell_chekcer */
		   /* to check this element */
		   strcpy (buffer, "xml-stylesheet type=\"text/css\" href=\"");
		   if (*value == EOS)
		     strcat (buffer, "./");
		   else
		     strcat (buffer, value);
		   strcat (buffer, "\"");
		   TtaSetTextContent (element, (unsigned char *)buffer, Latin_Script, doc);
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
       if (AttrHREFundoable && new_)
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
	       new_ = TRUE;
	     }
	   else
	     {
	       new_ = FALSE;
	       if (AttrHREFundoable)
                  TtaRegisterAttributeReplace (attr, element, doc);
	     }
	   TtaSetAttributeText (attr, "stylesheet", element, doc);
	   if (AttrHREFundoable && new_)
	       TtaRegisterAttributeCreate (attr, element, doc);

	   attrType.AttrTypeNum = HTML_ATTR_Link_type;
	   attr = TtaGetAttribute (element, attrType);
	   if (attr == 0)
	     {
	       /* create an attribute HREF for the element */
	       attr = TtaNewAttribute (attrType);
	       TtaAttachAttribute (element, attr, doc);
	       new_ = TRUE;
	     }
	   else
	     {
	       new_ = FALSE;
	       if (AttrHREFundoable)
                  TtaRegisterAttributeReplace (attr, element, doc);
	     }
	   TtaSetAttributeText (attr, "text/css", element, doc);	   
	   if (AttrHREFundoable && new_)
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
#ifndef _WX   
   unsigned char       *title;
#endif /* _WX */
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
       TtaGiveTextContent (child, (unsigned char *)Answer_text, &length, &lang);
#ifndef _WX
       // with wxWidgets, we want directly UTF8 for input
       title = TtaConvertMbsToByte ((unsigned char *)Answer_text, TtaGetDefaultCharset ());
       strcpy (Answer_text, (char *)title);
       TtaFreeMemory (title);
#endif /* _WX */
       CurrentDocument = doc;

#if defined(_GTK)
       TtaNewForm (BaseDialog + TitleForm, TtaGetViewFrame (doc, 1),
		   TtaGetMessage (AMAYA, AM_CHANGE_TITLE), TRUE, 2, 'L', D_CANCEL);
       TtaNewTextForm (BaseDialog + TitleText, BaseDialog + TitleForm, "",
		       50, 1, FALSE);
       /* initialise the text field in the dialogue box */
       TtaSetTextForm (BaseDialog + TitleText, Answer_text);
       TtaSetDialoguePosition ();
       TtaShowDialogue (BaseDialog + TitleForm, FALSE);
#endif /* #if defined(_GTK) */
#ifdef _WINGUI       
       CreateTitleDlgWindow (TtaGetViewFrame (doc, view), Answer_text);
#endif /* _WINGUI */
#ifdef _WX
       {
	 ThotBool created;
	 created = CreateTitleDlgWX ( BaseDialog + TitleForm,
				      TtaGetViewFrame (doc, view),
				      Answer_text);
	 if (created)
	   {
	     TtaSetDialoguePosition ();
	     TtaShowDialogue (BaseDialog + TitleForm, FALSE);
	   }
       }   
#endif /* _WX */
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
  unsigned char       *title;

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
#ifdef _WX
	  /* with _WX returned text is UTF8 everytime ! */
	  title = TtaConvertByteToMbs ((unsigned char *)Answer_text, UTF_8);
#else /* _WX */	  
	  title = TtaConvertByteToMbs ((unsigned char *)Answer_text, ISO_8859_1);
#endif /* _WX */
	  TtaSetTextContent (child, (unsigned char *)title,
			     TtaGetDefaultLanguage (), doc);
	  TtaFreeMemory (title);
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
   The parameter clickFirst is TRUE when the user choose to click the
   target.
  ----------------------------------------------------------------------*/
void SelectDestination (Document doc, Element el, ThotBool withUndo,
			ThotBool clickFirst)
{
   Element             targetEl;
   ElementType	       elType;
   Document            targetDoc;
   Attribute           attr;
   AttributeType       attrType;
   char               *buffer = NULL, *name;
   int                 length;

#if defined(_GTK)
   int                 i;
   char                s[MAX_LENGTH];
#endif /* #if defined(_GTK) */
   
   ThotBool            isHTML;
   ThotBool            fromButton = FALSE;

   fromButton = TtaIsButtonActivated (doc, 1);
   if (fromButton || clickFirst)
     {
       /* ask the user to select target document and target anchor */
       TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_SEL_TARGET), NULL);
       TtaClickElement (&targetDoc, &targetEl);
       if (targetDoc != 0)
	 isHTML = !(strcmp (TtaGetSSchemaName (TtaGetDocumentSSchema (targetDoc)),
			    "HTML"));
       else
	 isHTML = FALSE;
       
       if (targetDoc && targetEl && DocumentURLs[targetDoc])
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

       if (clickFirst)
	 {
	   TtaDestroyDialogue (BaseDialog + AttrHREFForm);
	   TtaDestroyDialogue (BaseDialog + FileBrowserForm);
	 }
     
     }
   
   AttrHREFelement = el;
   AttrHREFdocument = doc;
   AttrHREFundoable = withUndo;
   if ((fromButton || clickFirst) && (doc != targetDoc || TargetName != NULL))
     /* the user has clicked another document or a target element */
     /* create the attribute HREF or CITE */
     SetREFattribute (el, doc, TargetDocumentURL, TargetName);
   else
     /* the user has clicked the same document: pop up a dialogue box
	to allow the user to type the target URI */
     {
       if (fromButton)
	 TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_INVALID_TARGET), NULL);
	/* Dialogue form to insert HREF name */

	if (!LinkAsXmlCSS)
	  {
	    /* If the anchor has an HREF attribute, put its value in the form */
	    elType = TtaGetElementType (el);
	    name = TtaGetSSchemaName (elType.ElSSchema);
	    if (!strcmp (name, "HTML"))
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
#ifdef _SVG
	    else if (!strcmp (name, "SVG"))
	      /* it's an SVG element */
	      {
		attrType.AttrSSchema = elType.ElSSchema;
		attrType.AttrTypeNum = SVG_ATTR_xlink_href;
	      }
#endif /* _SVG */
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
		buffer = (char *)TtaGetMemory (length + 1);
		/* copy the HREF attribute into the buffer */
		TtaGiveTextAttributeValue (attr, buffer, &length);
		strcpy (AttrHREFvalue, buffer);
		TtaFreeMemory (buffer);
	      }
	  }

    TtaExtractName (DocumentURLs[doc], DirectoryName, DocumentName);

#if defined(_GTK)
	/* Dialogue form for open URL or local */
	i = 0;
	strcpy (&s[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
	i += strlen (&s[i]) + 1;
	strcpy (&s[i], TtaGetMessage (AMAYA, AM_BROWSE));
	i += strlen (&s[i]) + 1;
	strcpy (&s[i], TtaGetMessage (AMAYA, AM_CLICK));
	i += strlen (&s[i]) + 1;
	strcpy (&s[i], TtaGetMessage (AMAYA, AM_CLEAR));
	
	TtaNewSheet (BaseDialog + AttrHREFForm, TtaGetViewFrame (doc, 1),
		     TtaGetMessage (AMAYA, AM_ATTRIBUTE), 4, s,
		     TRUE, 2, 'L', D_CANCEL);
	TtaNewTextForm (BaseDialog + AttrHREFText, BaseDialog + AttrHREFForm,
			TtaGetMessage (AMAYA, AM_LOCATION), 50, 1, TRUE);
	TtaNewLabel (BaseDialog + HREFLocalName,
		     BaseDialog + AttrHREFForm, " ");
	/* initialise the text field in the dialogue box */
	TtaSetTextForm (BaseDialog + AttrHREFText, AttrHREFvalue);
	/*strcpy (s, DirectoryName);
	strcat (s, DIR_STR);
	strcat (s, DocumentName);*/
	TtaSetDialoguePosition ();
	TtaShowDialogue (BaseDialog + AttrHREFForm, TRUE);
#endif /* #if defined(_GTK) */
#ifdef _WINGUI
	if (LinkAsXmlCSS || LinkAsCSS)
	  /* select a CSS file */
	  CreateHRefDlgWindow (TtaGetViewFrame (doc, 1), AttrHREFvalue,
			       DocSelect, DirSelect, docCSS);
	else
	  /* select any file */
	  CreateHRefDlgWindow (TtaGetViewFrame (doc, 1), AttrHREFvalue,
			       DocSelect, DirSelect, docText);
#endif  /* _WINGUI */
#ifdef _WX
	ThotBool created = FALSE;
	
	if (LinkAsXmlCSS || LinkAsCSS)
	  /* select a CSS file */
	  created = CreateHRefDlgWX ( BaseDialog + AttrHREFForm,
				      TtaGetViewFrame (doc, 1), URL_list, AttrHREFvalue,
				      DocSelect, DirSelect, docCSS );
	else
	  /* select any file */
	  created = CreateHRefDlgWX ( BaseDialog + AttrHREFForm,
				      TtaGetViewFrame (doc, 1), URL_list, AttrHREFvalue,
				      DocSelect, DirSelect, docText );
	if (created)
	  {
	    TtaSetDialoguePosition ();
	    TtaShowDialogue (BaseDialog + AttrHREFForm, TRUE);
	  }
#endif /* _WX */
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
   if (selectedElement)
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

	if (el)
	  {
	     /* the ascending Anchor element has been found */
	     /* get the NAME attribute of element Anchor */
	     attrType.AttrTypeNum = HTML_ATTR_NAME;
	     attr = TtaGetAttribute (el, attrType);
	     if (attr == NULL)
	       {
		 attrType.AttrTypeNum = HTML_ATTR_ID;
		 attr = TtaGetAttribute (el, attrType);
	       }
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
#endif /* _SVG */
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
   char               *text, *url;
   int                 length, i, space;
   ThotBool            found;
   ThotBool            withinHTML, new_;

   elType = TtaGetElementType (el);
   withinHTML = !strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML");

   /* get a NAME or ID attribute */
   HTMLSSchema = TtaGetSSchema ("HTML", doc);
   attrType.AttrSSchema = HTMLSSchema;
   if (withinHTML && (elType.ElTypeNum == HTML_EL_Anchor ||
		      elType.ElTypeNum == HTML_EL_MAP ||
		      elType.ElTypeNum == HTML_EL_map))
     {
       if (forceID || TtaGetDocumentProfile (doc) ==  L_Xhtml11)
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
#endif /* _SVG */
       attrType.AttrTypeNum = HTML_ATTR_ID;
     }
   attr = TtaGetAttribute (el, attrType);

   if (attr == NULL)
     {
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (el, attr, doc);
	new_ = TRUE;
     }
   else
     /* already done */
     return;

   /* build a value for the new attribute */
   url = (char *)TtaGetMemory (MAX_LENGTH);
   if (withinHTML &&
       (elType.ElTypeNum == HTML_EL_MAP || elType.ElTypeNum == HTML_EL_map))
     /* mapxxx for a map element */
     strcpy (url, "map");
   else if (withinHTML && elType.ElTypeNum == HTML_EL_LINK)
     /* linkxxx for a link element */
     strcpy (url, "link");
   else
       /* get the content for other elements */
     {
	elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	elText = TtaSearchTypedElement (elType, SearchInTree, el);
	if (elText != NULL)
	  {
	    /* first word longer than 3 characters */
	    length = 50;
	    TtaGiveTextContent (elText, (unsigned char *)url, &length, &lang);
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
   if (withUndo && new_)
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
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  DisplayMode         dispMode;
  char               *s;
  int                 firstChar, lastChar, lg, i, levelFirst, levelLast, min,
                      max;
  Language            lang;
  CHAR_T              *buffer;
  ThotBool            noAnchor, ok;

  if (!TtaGetDocumentAccessMode (doc))
    /* the document is in ReadOnly mode */
    return;

  parag = NULL;
  dispMode = TtaGetDisplayMode (doc);

  /* get the first and last selected element */
  TtaGiveFirstSelectedElement (doc, &first, &firstChar, &i);
  if (TtaIsReadOnly (first))
    /* the selected element is read-only */
    return;
  TtaGiveLastSelectedElement (doc, &last, &i, &lastChar);

  noAnchor = FALSE;
  /* Check whether the selected elements are a valid content for an anchor */
  elType = TtaGetElementType (first);
  s = TtaGetSSchemaName (elType.ElSSchema);
  if ((elType.ElTypeNum == HTML_EL_Anchor ||
       elType.ElTypeNum == HTML_EL_MAP ||
       elType.ElTypeNum == HTML_EL_map) &&
      !strcmp (s, "HTML") &&
      first == last)
    /* add an attribute on the current anchor */
    anchor = first;
#ifdef _SVG
  else if (elType.ElTypeNum == SVG_EL_a && !strcmp (s, "SVG") &&
      first == last)
    /* add an attribute on the current anchor */
    anchor = first;
#endif /* _SVG */
  else
    {
      /* check whether the selection is within an anchor */
      if (!strcmp (s, "HTML") || !strcmp (s, "SVG"))
	el = SearchAnchor (doc, first, &attr, !createLink);
      else
	el = NULL;
      if (el)
	/* add an attribute on this anchor */
	anchor = el;
      else
	{
	  el = first;

	  while (!noAnchor && el != NULL)
	    {
	      elType = TtaGetElementType (el);
	      s = TtaGetSSchemaName (elType.ElSSchema);
	      if (!strcmp (s, "HTML"))
		{
		  if (elType.ElTypeNum != HTML_EL_TEXT_UNIT &&
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
		      elType.ElTypeNum != HTML_EL_Variable_ &&
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
		      elType.ElTypeNum != HTML_EL_map &&
		      elType.ElTypeNum != HTML_EL_Quotation &&
		      elType.ElTypeNum != HTML_EL_Subscript &&
		      elType.ElTypeNum != HTML_EL_Superscript &&
		      elType.ElTypeNum != HTML_EL_Span &&
		      elType.ElTypeNum != HTML_EL_BDO &&
		      elType.ElTypeNum != HTML_EL_simple_ruby &&
		      elType.ElTypeNum != HTML_EL_complex_ruby &&
		      elType.ElTypeNum != HTML_EL_IFRAME)
		    noAnchor = TRUE;
		}
#ifdef _SVG
	      else if (strcmp (s, "SVG") ||
		       elType.ElTypeNum == SVG_EL_SVG)
#else /* _SVG */
	      else
#endif /* _SVG */
		/* don't accept to generate an anchor here */
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
		    createLink && strcmp (s, "HTML") && strcmp (s, "SVG"))
		   /* a single element is selected and it's not a HTML elem
		      nor a SVG element nor a character string */
		  {
		    if (UseLastTarget)
		      /* points to the last created target */
		      SetREFattribute (first, doc, TargetDocumentURL,
				       TargetName);
		    else
		      /* select the destination */
		      SelectDestination (doc, first, TRUE, FALSE);
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
		  s = TtaGetSSchemaName (elType.ElSSchema);
		  if (!strcmp (s, "HTML") && elType.ElTypeNum == HTML_EL_Anchor)
		    ok = FALSE;
		}
	      if (ok)
		{
		  ancestor = SearchAnchor (doc, last, &attr, TRUE);
		  if (ancestor)
		    {
		      elType = TtaGetElementType (ancestor);
		      s = TtaGetSSchemaName (elType.ElSSchema);
		      if (!strcmp (s, "HTML") &&
			  elType.ElTypeNum == HTML_EL_Anchor)
			ok = FALSE;
		    }
		}

	      elType = TtaGetElementType (first);
	      s = TtaGetSSchemaName (elType.ElSSchema);
	      if (!ok)
		{
		  TtaSetStatus (doc, 1,
				TtaGetMessage (AMAYA, AM_INVALID_ANCHOR2),
				NULL);
		  return;
		}
	      else if (!strcmp (s, "SVG") &&
		       elType.ElTypeNum == HTML_EL_TEXT_UNIT)
		{
		  /* move the selection to the enclosing text of tspan */
		  first = TtaGetParent (first);
		  if (first == NULL)
		    return;
		  last = first;
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
		/* split the last text element */
		{
		  /* exclude trailing spaces from the anchor */
		  if (lg > 0)
		    {
		      lg++;
		      buffer = (CHAR_T *)TtaGetMemory (lg * sizeof(CHAR_T));
		      TtaGiveBufferContent (last, buffer, lg, &lang);
		      if (last == first)
			min = firstChar;
		      else
			min = 1;
		      while (lastChar > min &&
			     buffer[lastChar - 2] == SPACE)
			lastChar--;
		      TtaFreeMemory (buffer);
		    }
		  if (lastChar > 1)
		    {

		      TtaRegisterElementReplace (last, doc);
		      TtaSplitText (last, lastChar, doc);
		      next = last;
		      TtaNextSibling (&next);
		      TtaRegisterElementCreate (next, doc);
		    }
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
		  /* exclude leading spaces from the selection */
		  if (lg > 0)
		    {
		      lg++;
		      buffer = (CHAR_T *)TtaGetMemory (lg * sizeof(CHAR_T));
		      TtaGiveBufferContent (first, buffer, lg, &lang);
		      if (last == first)
			max = lastChar;
		      else
			max = lg;
		      while (firstChar < max &&
			     buffer[firstChar - 1] == SPACE)
			firstChar++;
		      TtaFreeMemory (buffer);
		    }
		  if (firstChar <= lg)
		    /* split the first string */
		    {
		      TtaRegisterElementReplace (first, doc);
		      TtaSplitText (first, firstChar, doc);
		    }
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
	  s = TtaGetSSchemaName (elType.ElSSchema);
#ifdef _SVG
	  if (!strcmp (s, "SVG"))
	    elType.ElTypeNum = SVG_EL_a;
	  else
#endif /* _SVG */
	    elType.ElTypeNum = HTML_EL_Anchor;
	  anchor = TtaNewElement (doc, elType);
	  if (createLink)
	    {
	      /* create an attribute HREF for the new anchor */
	      attrType.AttrSSchema = elType.ElSSchema;
#ifdef _SVG
	      if (!strcmp (s, "SVG"))
		attrType.AttrTypeNum = SVG_ATTR_xlink_href;
	      else
#else /* _SVG */
		attrType.AttrTypeNum = HTML_ATTR_HREF_;
#endif /* _SVG */
	      attr = TtaGetAttribute (anchor, attrType);
	      if (attr == NULL)
		{
		  attr = TtaNewAttribute (attrType);
		  TtaAttachAttribute (anchor, attr, doc);
		}
	    }
	  
	  /* Check if the first element is included within a paragraph */
	  elType = TtaGetElementType (TtaGetParent (first));
	  s = TtaGetSSchemaName (elType.ElSSchema);	  
	  if (!strcmp (s, "HTML") &&
	      (elType.ElTypeNum == HTML_EL_BODY ||
	       elType.ElTypeNum == HTML_EL_Division ||
	       elType.ElTypeNum == HTML_EL_Object_Content ||
	       elType.ElTypeNum == HTML_EL_Data_cell ||
	       elType.ElTypeNum == HTML_EL_Heading_cell ||
	       elType.ElTypeNum == HTML_EL_Block_Quote))
	    {
	      elType.ElTypeNum = HTML_EL_Pseudo_paragraph;
	      parag = TtaNewElement (doc, elType);
	      TtaInsertSibling (parag, last, FALSE, doc);
	      TtaInsertFirstChild (&anchor, parag, doc);
	    }
	  else
	      TtaInsertSibling (anchor, last, FALSE, doc);
	  noAnchor = TRUE;
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

  if (createLink)
    {
      if (UseLastTarget)
	/* points to the last created target */
	SetREFattribute (anchor, doc, TargetDocumentURL, TargetName);
      else
	/* Select the destination */
	SelectDestination (doc, anchor, FALSE, FALSE);
      /* The anchor element must have an HREF attribute */
      /* create an attribute PseudoClass = link */
      s = TtaGetSSchemaName (elType.ElSSchema);	  
      if (!strcmp (s, "HTML"))
	{
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
    }
  else
    {
      if (noAnchor)
	CreateTargetAnchor (doc, anchor, FALSE, FALSE);
      else
	{
	  TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
	  CreateTargetAnchor (doc, anchor, FALSE, TRUE);
	}
    }

  if (noAnchor)
    {
      if (parag)
	TtaRegisterElementCreate (parag, doc);
      else
	TtaRegisterElementCreate (anchor, doc);
    }
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
  ThotBool          change, checkID, checkNAME;

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  checkID = checkNAME = FALSE;
  if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
    {
      /* it's an element from the XHTML namespace */
      if (elType.ElTypeNum == HTML_EL_Anchor ||
	  elType.ElTypeNum == HTML_EL_MAP ||
	  elType.ElTypeNum == HTML_EL_map)
	/* it's an anchor or a map. Look for a NAME attribute */
	{
	  attrType.AttrTypeNum = HTML_ATTR_NAME;
	  attr = TtaGetAttribute (el, attrType);
	  if (attr)
	    /* the element has a NAME attribute. Check it and then check
	       if there is an ID too */
	    checkID = TRUE;
	  else
	    {
	      /* no NAME. Look for an ID */
	      attrType.AttrTypeNum = HTML_ATTR_ID;
	      if (TtaGetDocumentProfile (doc) != L_Xhtml11)
		checkNAME = TRUE;
	    }
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
#endif /* _SVG */
  else
    attrType.AttrTypeNum = 0;
  
  if (attrType.AttrTypeNum != 0)
    {
      attr = TtaGetAttribute (el, attrType);
      if (attr)
	/* the element has an attribute NAME or ID. Check it */
	{
	  length = TtaGetTextAttributeLength (attr) + 10;
	  value = (char *)TtaGetMemory (length);
	  change = FALSE;
	  if (value)
	    {
	      TtaGiveTextAttributeValue (attr, value, &length);
	      i = 0;
	      while (SearchNAMEattribute (doc, value, attr, el))
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
		  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") &&
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
			  if (i == length+1 &&
			      !strncmp (&url[1], value, length))
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
	  if (checkID)
	    {
	      /* Change or insert an ID attribute accordingly */
	      attrType.AttrTypeNum = HTML_ATTR_ID;
	      attr = TtaGetAttribute (el, attrType);
	      if (attr == NULL)
		{
		  attr = TtaNewAttribute (attrType);
		  TtaAttachAttribute (el, attr, doc);
		  change = FALSE;
		}
	      else
		{
		  change = TRUE;
		  TtaRegisterAttributeReplace (attr, el, doc);
		}
	      TtaSetAttributeText (attr, value, el, doc);
	      if (!change)
		TtaRegisterAttributeCreate (attr, el, doc);
	    }
	  else if (checkNAME)
	    {
	      /* Change or insert a NAME attribute accordingly */
	      attrType.AttrTypeNum = HTML_ATTR_NAME;
	      attr = TtaGetAttribute (el, attrType);
	      if (attr == NULL)
		{
		  attr = TtaNewAttribute (attrType);
		  TtaAttachAttribute (el, attr, doc);
		  change = FALSE;
		}
	      else
		{
		  change = TRUE;
		  TtaRegisterAttributeReplace (attr, el, doc);
		}
	      TtaSetAttributeText (attr, value, el, doc);
	      if (!change)
		TtaRegisterAttributeCreate (attr, el, doc);
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
void CreateRemoveIDAttribute (char *elName, Document doc, ThotBool createID,
			      ThotBool inSelection)
{
  Element             el, lastEl;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  char               *schema_name;
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
   Element el has been created or pasted. If it's a Pseudo_paragraph,
   it is turned into an ordinary Paragraph if it's not the first child
   of its parent.
   If the next sibiling is a Pseudo_paragraph, this sibling is turned into
   an ordinary Paragraph.
   Rule: only the first child of any element can be a Pseudo_paragraph.
  ----------------------------------------------------------------------*/
static void CheckPseudoParagraph (Element el, Document doc)
{
  Element		prev, next, parent, sibling, child;
  Attribute             attr;
  ElementType		elType, textType;
  SSchema               htmlSchema;

  elType = TtaGetElementType (el);
  htmlSchema = TtaGetSSchema ("HTML", doc);
  if (!htmlSchema || elType.ElSSchema != htmlSchema)
    /* it's not an HTML element */
    return;

  if (elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
    /* the new element is a Pseudo_paragraph */
    {
      prev = el;
      TtaPreviousSibling (&prev);
      attr = NULL;
      TtaNextAttribute (el, &attr);
      if (prev || attr)
        /* the Pseudo-paragraph is not the first element among its sibling */
        /* or it has attributes: turn it into an ordinary paragraph */
        TtaChangeElementType (el, HTML_EL_Paragraph);
    }
  else if (elType.ElTypeNum == HTML_EL_Paragraph)
    /* the new element is a Paragraph */
    {
      /* get the previous element that is not a comment or a PI */
      prev = el;
      do
	{
	  TtaPreviousSibling (&prev);
	  if (prev)
	    elType = TtaGetElementType (prev);
	 }
      while (prev && elType.ElSSchema == htmlSchema &&
	     (elType.ElTypeNum == HTML_EL_Comment_ ||
	      elType.ElTypeNum == HTML_EL_XMLPI));

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
		elType.ElTypeNum == HTML_EL_Heading_cell ||
		elType.ElTypeNum == HTML_EL_Object_Content ||
		elType.ElTypeNum == HTML_EL_map)
	      TtaChangeTypeOfElement (el, doc, HTML_EL_Pseudo_paragraph);
	    }
	 }
    }
  else if (elType.ElTypeNum == HTML_EL_Block)
    /* the new element is a Block */
    {
      /* if this Block is empty and it is the only child of a List_Item,
	 change it into a Pseudo_Paragraph */
      if (!TtaGetFirstChild (el))
	/* it is empty */
	if (TtaGetElementType (TtaGetParent (el)).ElTypeNum == HTML_EL_List_Item)
	  /* it's a child of a List_Item */
	  {
	    sibling = el;
	    TtaNextSibling (&sibling);
	    if (!sibling)
	      {
		sibling = el;
		TtaPreviousSibling (&sibling);
		if (!sibling)
		  /* no siblings */
		  {
		    TtaChangeElementType (el, HTML_EL_Pseudo_paragraph);
		    /* insert the text element */
		    textType.ElSSchema = elType.ElSSchema;
		    textType.ElTypeNum = HTML_EL_TEXT_UNIT;
		    child = TtaNewElement (doc, textType);
		    TtaInsertFirstChild  (&child, el, doc);
		  }
	      }
	  }
    }

  /* get the next element that is not a comment or a PI */
  next = el;
  do
    {
      TtaNextSibling (&next);
      if (next)
	elType = TtaGetElementType (next);
    }
  while (next && elType.ElSSchema == htmlSchema &&
	 (elType.ElTypeNum == HTML_EL_Comment_ ||
	  elType.ElTypeNum == HTML_EL_XMLPI));

  if (next && elType.ElSSchema == htmlSchema &&
      elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
    /* the next element is a Pseudo-paragraph */
    /* turn it into an ordinary paragraph */
    {
      TtaChangeTypeOfElement (next, doc, HTML_EL_Paragraph);
      TtaRegisterElementTypeChange (next, elType.ElTypeNum, doc);
    }
}

/*----------------------------------------------------------------------
   ElementCreated
   An element has been created in a HTML document.
  ----------------------------------------------------------------------*/
void ElementCreated (NotifyElement *event)
{
  CheckPseudoParagraph (event->element, event->document);
}

/*----------------------------------------------------------------------
   ElementWillBeDeleted
   An HTML element will be deleted.
   Update the namespace declarations associated with that element
  ----------------------------------------------------------------------*/
ThotBool ElementWillBeDeleted (NotifyElement *event)
{
  ElementType	elType;

  elType = TtaGetElementType (event->element);
  if (elType.ElTypeNum != 1)
    TtaFreeElemNamespaceDeclarations (event->document, event->element);
  return FALSE; /* let Thot perform normal operation */
}
 
/*----------------------------------------------------------------------
   ElementDeleted
   An element has been deleted. If it was the only child of element
   BODY, create a first paragraph.
 -----------------------------------------------------------------------*/
void ElementDeleted (NotifyElement *event)
{
  Element	 child, el;
  ElementType	 elType, childType;
  ThotBool	 empty;

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
    {
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
		  TtaChangeElementType (child, HTML_EL_Pseudo_paragraph);
		  TtaInsertFirstChild (&child, event->element, event->document);
		}
	    }
	}
    }
}


/*----------------------------------------------------------------------
   RegisterURLSavedElements
   Elements from document doc have been saved into the Thot Copy/Cut buffer.
   Save the URL of this document, to allow relative URIs contained
   in these elements to be adapted when they are pasted.
  ----------------------------------------------------------------------*/
void RegisterURLSavedElements (Document doc)
{
  if (SavedDocumentURL)
    TtaFreeMemory (SavedDocumentURL);
  SavedDocumentURL = GetBaseURL (doc);
  /* Paste functions in the table menu could be active now */
  UpdateContextSensitiveMenus (doc);
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
  value = (char *)TtaGetMemory (length);
  if (value)
    {
    tempURI = (char *)TtaGetMemory (MAX_LENGTH);
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
          documentURI = (char *)TtaGetMemory (MAX_LENGTH);
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
	    path = (char *)TtaGetMemory (MAX_LENGTH);
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

static void CheckDescendants (Element el, Document doc);

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
   Check Pseudo paragraphs.
   If the pasted element has a NAME attribute, change its value if this
   NAME is already used in the document.
   If it's within the TITLE element, update the corresponding field in
   the Formatted window.
  ----------------------------------------------------------------------*/
void ElementPasted (NotifyElement * event)
{
  Document            originDocument, doc;
  Element             el, anchor, child, previous, nextchild, parent, ancestor,
                      sibling;
  ElementType         elType, parentType;
  AttributeType       attrType;
  Attribute           attr;
  SSchema             HTMLschema;
  char               *value;
  int                 length, oldStructureChecking;
  ThotBool            ok;
  DisplayMode         dispMode;

  el = event->element;
  doc = event->document;
  if (!ElementOKforProfile (el, doc))
    return;
  HTMLschema = TtaGetSSchema ("HTML", doc);

  /* Check pseudo-paragraphs */
  CheckPseudoParagraph (el, doc);

  /* Check attribute NAME or ID in order to make sure that its value */
  /* is unique in the document, except if we are including an external
     document referred by an <object> or <embed> element. In this case, there
     is no need to check IDs as the included document will never be changed
     nor saved */
  if (event->elementType.ElTypeNum > 0)
     MakeUniqueName (el, doc);

  elType = TtaGetElementType (el);
  anchor = NULL;
  if (event->info == 0 &&
      elType.ElSSchema == HTMLschema && elType.ElTypeNum == HTML_EL_Anchor)
      anchor = el;
  else if (elType.ElSSchema == HTMLschema && elType.ElTypeNum == HTML_EL_LINK)
    {
      /* check if it's a CSS link */
      CheckCSSLink (el, doc, HTMLschema);
    }
  else if (elType.ElSSchema == HTMLschema && elType.ElTypeNum == HTML_EL_STYLE_)
    /* The pasted element is a STYLE element in the HEAD */
    /* Get its content */
    EnableStyleElement (doc, el);
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
		      value = (char *)TtaGetMemory (MAX_LENGTH);
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

  if (anchor)
    /* an anchor element has been pasted. Nested anchors are forbidden in HTML.
       Check if there is an anchor among the ancestors of the pasted element */
    {
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
	/* don't set NoComputedDisplay
	   -> it breaks down views formatting when Enter generates new elements  */
	TtaSetDisplayMode (doc, DeferredDisplay);
      oldStructureChecking = TtaGetStructureChecking (doc);
      TtaSetStructureChecking (0, doc);
      /* Is there an anchor ancestor? */
      ancestor = TtaGetTypedAncestor (el, elType);
      if (ancestor)
	/* nested anchors */
	{
	  ok = FALSE;
	  if (ancestor == TtaGetParent (el))
	    /* the enclosing anchor is the parent of the pasted element */
	    {
	      sibling = el; TtaNextSibling (&sibling);
	      if (!sibling)
		/* the pasted anchor is the last child of its parent */
		{
		  /* move the pasted anchor as the next sibling of its
		     parent */
		  TtaRemoveTree (el, doc);
		  TtaInsertSibling (el, ancestor, FALSE, doc);
		  ok = TRUE;
		}
	      else
		{
		  sibling = el; TtaPreviousSibling (&sibling);
		  if (!sibling)
		    /* the pasted anchor is the first child of its parent */
		    {
		      /* move the pasted anchor as the previous sibling of its
			 parent */
		      TtaRemoveTree (el, doc);
		      TtaInsertSibling (el, ancestor, TRUE, doc);
		      ok = TRUE;
		    }
		}
	    }
	  if (!ok)
	    {
	      /* Move anchor children and delete the anchor element */
	      child = TtaGetFirstChild (anchor);
	      previous = child;
	      TtaPreviousSibling (&previous);
	      while (child)
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
	      anchor = NULL;
	    }
	}
      if (anchor)
	{
	  /* the anchor element is allowed here */
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
      /* Restore the display mode */
      if (dispMode == DisplayImmediately)
	TtaSetDisplayMode (doc, dispMode);
    }
}

/*----------------------------------------------------------------------
   CheckNewLines
   Some new text has been pasted or typed in a text element.
   If it's not in a <pre>, check the NewLine characters and replace them by
   spaces. Replace a sequence of spaces by a single space.
   If its in a paragraph, generate a new paragraph for each newline (if
   mode PasteLineByLine is on), or for each pair of newlines (if this mode
   is off).
 -----------------------------------------------------------------------*/
void CheckNewLines (NotifyOnTarget *event)
{
  Element     ancestor, selEl, leaf, newLeaf, firstLeaf, firstParag, el,
              child, orig, prev, next, parent;
  Document    doc;
  ElementType elType;
  CHAR_T      *content, *sibContent;
  int         firstSelChar, lastSelChar, length, i, j, sibLength, start;
  Language    lang;
  ThotBool    pre, para, changed, selChanged, newParagraph, undoSeqExtended,
              prevCharEOL;
  ThotBool    PasteLineByLine;

  TtaGetEnvBoolean ("PRESERVE_SPACE", &pre);
  if (pre)
    return;
  if (!event->target)
    return;
  leaf = event->target;
  length = TtaGetElementVolume (leaf);
  if (length == 0)
    return;
  firstLeaf = leaf;
  doc = event->document;
  if (DocumentTypes[doc] == docText || DocumentTypes[doc] == docCSS ||
      DocumentTypes[doc] == docSource || DocumentTypes[doc] == docLog)
    return;
  undoSeqExtended = FALSE;

  /* is there a preformatted or paragraph (or equivalent) ancestor? */
  pre = FALSE;
  para = FALSE;
  newParagraph = FALSE;
  firstParag = NULL;
  ancestor = TtaGetParent (leaf);
  while (ancestor && !pre && !para)
    {
      if (TtaIsElementWithSpacePreserve (ancestor))
	pre = TRUE;
      else
	{
	  elType = TtaGetElementType (ancestor);
	  if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") != 0)
	    ancestor = NULL;  /* not an HTML element */
	  else if (elType.ElTypeNum == HTML_EL_STYLE_ ||
		   elType.ElTypeNum == HTML_EL_SCRIPT_ ||
		   elType.ElTypeNum == HTML_EL_Preformatted ||
		   elType.ElTypeNum == HTML_EL_Text_Area)
	    pre = TRUE;
	  else if (elType.ElTypeNum == HTML_EL_Paragraph ||
		   elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
	    {
	      para = TRUE;
	      firstParag = ancestor;
	    }
	  else
	    ancestor = TtaGetParent (ancestor);
	}
    }
  if (pre)
    /* there is a <PRE> ancestor. Don't change anything */
    return;

  /* get the user value for the Paste-Line-By-Line option */
  TtaGetEnvBoolean ("PASTE_LINE_BY_LINE", &PasteLineByLine);

  /* replace every new line in the content of the element by a space
     and replace every sequence of spaces by a single space */
  TtaGiveFirstSelectedElement (doc, &selEl, &firstSelChar, &lastSelChar);
  if (selEl != leaf)
    /* the current selection is not within this element. Don't care about
       the selection */
    selEl = NULL;
  length++;
  content = (CHAR_T *)TtaGetMemory (length * sizeof(CHAR_T));
  TtaGiveBufferContent (leaf, content, length, &lang);
  changed = FALSE;
  selChanged = FALSE;
  prevCharEOL = FALSE;
  /* is there a previous sibling element? */
  prev = leaf;
  TtaPreviousSibling (&prev);
  j = 0;
  for (i = 0; i < length; i++)
    {
      if (content[i] == EOL)
	/* the current character is a newline */
	{
	  if (!para)
	    /* We are not within a paragraph. Replace the newline by a space */
	    {
	      content[i] = SPACE;
	      changed = TRUE;
	    }
	  else
	    /* The pasted text is within a paragraph */
	    {
	      if (!PasteLineByLine && !prevCharEOL)
		/* in this mode (!PasteLineByLine), create a paragraph only
                   when there are two newlines (possibly with some spaces) */
		{
		  content[i] = SPACE;
		  changed = TRUE;
		}
	      else
	        /* Break the current paragraph */
		{
		  /* create new elements to duplicate the ancestors of the
		     leaf until the paragraph (included) */
		  child = NULL; newLeaf = NULL; prev = NULL;
		  orig = leaf;
		  while (orig)
		    {
		      el = TtaCopyElement (orig, doc, doc, TtaGetParent(orig));
		      elType = TtaGetElementType (orig);
		      if (elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
			TtaChangeElementType (el, HTML_EL_Paragraph);
		      if (orig == leaf)
			newLeaf = el;
		      if (prev)
			TtaInsertFirstChild (&prev, el, doc);
		      prev = el;
		      if (orig == ancestor)
			orig = NULL;
		      else
			orig = TtaGetParent (orig);
		    }
		  TtaInsertSibling (el, ancestor, FALSE, doc);
		  newParagraph = TRUE;
		  /* extend the previous undo sequence if it's not done */
		  if (!undoSeqExtended)
		    {
		      TtaExtendUndoSequence (doc);
		      undoSeqExtended = TRUE;
		    }
		  TtaRegisterElementCreate (el, doc);
		  if (j > 0 && content[j-1] == SPACE)
		    /* remove trailing space */
		    {
		      j--;
		      if (selEl)
			if (firstSelChar >= j)
			  /* The selection is after the current position.
			     Update it */
			  {
			    firstSelChar--;
			    selChanged = TRUE;
			  }
		    }
		  content[j] = EOS;
		  content[i] = EOS;
		  TtaSetBufferContent (leaf, content, lang, doc);
		  if (selChanged && firstSelChar <= j)
		    /* update the current selection */
		    {
		      TtaSelectString (doc, leaf, firstSelChar,firstSelChar-1);
		      selChanged = FALSE;
		      selEl = NULL;
		    }
		  else if (selEl)
		    /* the current selection is further in the element */
		    {
		      /* substract the characters moved to the new element and
			 the EOL */
		      firstSelChar-= j; 
		      firstSelChar--;
		      selChanged = TRUE;
		    }
		  leaf = newLeaf;
		  j = 0;
		  changed = TRUE;
		  ancestor = el;
		}
	      prevCharEOL = TRUE;
	    }
	}
      if (content[i] == SPACE)
	/* this is a space */
	{
	  if (j == 0)
	    /* beginning of the text element */
	    {
	      /* if this space is after a newline, remove it */
	      if (prevCharEOL ||
		  (prev && TtaGetLastBufferContent (prev) == SPACE))
		{
		  changed = TRUE;
		  if (selEl)
		    /* the selection is in this piece of text */
		    if (firstSelChar >= j)
		      /* it is after the current position. Update it */
		      {
			firstSelChar--;
			selChanged = TRUE;
		      }
		}
	      else
		/* Keep that space */
		{
		  if (i > j)
		    content[j] = content[i];
		  j++;
		}
	    }
	  else
	    {
	      if (content[j-1] != SPACE)
		/* the previous character is not a space nor a newline.
		   Keep that space */
		content[j++] = SPACE;
	      else
		/* the previous character is a space or a new line.
		   Remove the current space */
		{
		  changed = TRUE;
		  if (selEl)
		    /* the selection is in this piece of text */
		    if (firstSelChar >= j)
		      /* it is after the current position. Update it */
		      {
			firstSelChar--;
			selChanged = TRUE;
		      }
		}
	    }
	}
      else
	{
	  if (content[i] != EOS)
	    /* an ordinary character. Keep it */
	    {
	      if (i > j)
		/* some characters have been deleted. Move this character */
		content[j] = content[i];
	      j++;
	      prevCharEOL = FALSE;
	    }
	}
    }

  start = 0;
  /* all the content of the modified element has now been processed */
  /* if we are within an inline element and there is a space at the beginning
     or at the end of the text element, tries to move that space to the
     previous (resp. next) text element */
  parent = TtaGetParent (leaf);
  elType = TtaGetElementType (parent);
  if (elType.ElTypeNum == HTML_EL_Anchor ||
      elType.ElTypeNum == HTML_EL_Font_ ||
      elType.ElTypeNum == HTML_EL_Subscript ||
      elType.ElTypeNum == HTML_EL_Superscript ||
      elType.ElTypeNum == HTML_EL_Span ||
      elType.ElTypeNum == HTML_EL_BDO ||
      elType.ElTypeNum == HTML_EL_Teletype_text ||
      elType.ElTypeNum == HTML_EL_Italic_text ||
      elType.ElTypeNum == HTML_EL_Bold_text ||
      elType.ElTypeNum == HTML_EL_Underlined_text ||
      elType.ElTypeNum == HTML_EL_Struck_text ||
      elType.ElTypeNum == HTML_EL_Big_text ||
      elType.ElTypeNum == HTML_EL_Small_text ||
      elType.ElTypeNum == HTML_EL_Emphasis ||
      elType.ElTypeNum == HTML_EL_Strong ||
      elType.ElTypeNum == HTML_EL_Def ||
      elType.ElTypeNum == HTML_EL_Code ||
      elType.ElTypeNum == HTML_EL_Sample ||
      elType.ElTypeNum == HTML_EL_Keyboard ||
      elType.ElTypeNum == HTML_EL_Variable_ ||
      elType.ElTypeNum == HTML_EL_Cite ||
      elType.ElTypeNum == HTML_EL_ABBR ||
      elType.ElTypeNum == HTML_EL_ACRONYM ||
      elType.ElTypeNum == HTML_EL_rb)
    /* we are within an inline element */
    {
      next = leaf;
      TtaNextSibling (&next);
      if (j > 0 && content[j-1] == SPACE && !next)
	/* the text element has a trailing space and no following sibling */
	/* try to move the trailing space to the beginning of the following
	   text leaf */
	{
	  next = parent;
	  if (elType.ElTypeNum == HTML_EL_rb)
	    next = TtaGetParent (parent);
	  TtaNextSibling (&next);
	  if (next &&
	      TtaGetElementType(next).ElTypeNum == HTML_EL_TEXT_UNIT)
	    {
	      /* the next sibling of the parent element is a character string*/
	      /* remove the trailing space */
	      j--;
	      changed = TRUE;
	      if (selEl)
		if (firstSelChar >= j)
		  /* The selection is after the current position. Update it */
		  {
		    firstSelChar--;
		    selChanged = TRUE;
		  }
	      /* check the next character string */
	      sibLength = TtaGetElementVolume (next);
	      sibLength+= 2;
	      sibContent = (CHAR_T *)TtaGetMemory (sibLength * sizeof(CHAR_T));
	      TtaGiveBufferContent (next, &sibContent[1], sibLength-1, &lang);
	      if (sibContent[1] != SPACE)
		/* no space at the beginning of the next text element */
		{
		  /* insert a space */
		  if (!undoSeqExtended)
		    {
		      TtaExtendUndoSequence (doc);
		      undoSeqExtended = TRUE;
		    }
		  TtaRegisterElementReplace (next, doc);
		  sibContent[0] = SPACE;
		  TtaSetBufferContent (next, sibContent, lang, doc);
		}
	      TtaFreeMemory (sibContent);
	    }
	}
      prev = leaf;
      TtaPreviousSibling (&prev);
      if (content[0] == SPACE && !prev)
	/* the text element has a leading space and no previous sibling */
	/* try to move the leading space to the end of the preceding text
	   leaf */
	{
	  prev = parent;
	  if (elType.ElTypeNum == HTML_EL_rb)
	    prev = TtaGetParent (parent);
	  TtaPreviousSibling (&prev);
	  if (prev &&
	      TtaGetElementType(prev).ElTypeNum == HTML_EL_TEXT_UNIT)
	    {
	      /* the previous sibling of the parent element is a character
		 string. Remove the leading space */
	      start = 1;
	      changed = TRUE;
	      if (selEl)
		/* The selection is after the current position.
		   Update it */
		{
		  firstSelChar--;
		  selChanged = TRUE;
		}
	      /* check the end of the previous character string */
	      sibLength = TtaGetElementVolume (prev);
	      sibLength+= 2;
	      sibContent = (CHAR_T *)TtaGetMemory (sibLength * sizeof(CHAR_T));
	      TtaGiveBufferContent (prev, sibContent, sibLength-1, &lang);
	      if (sibLength > 2 && sibContent[sibLength-3] != SPACE)
		{
		  /* insert a space at the end of the previous text element */
		  if (!undoSeqExtended)
		    {
		      TtaExtendUndoSequence (doc);
		      undoSeqExtended = TRUE;
		    }
		  TtaRegisterElementReplace (prev, doc);
		  sibContent[sibLength-2] = SPACE;
		  sibContent[sibLength-1] = EOS;
		  TtaSetBufferContent (prev, sibContent, lang, doc);
		}
	      TtaFreeMemory (sibContent);
	    }
	}
    }

  if (j < length)
    content[j] = EOS;
  if (changed)
    /* we have made changes in the text buffer, update the element */
    {
      TtaSetBufferContent (leaf, &content[start], lang, doc);
      if (selChanged)
	/* update the current selection */
	TtaSelectString (doc, leaf, firstSelChar, firstSelChar-1);
    }
  TtaFreeMemory (content);

  if (newParagraph)
    /* New paragraphs have been created. Move the content that follows
       the pasted text from the initial paragraph to the last one created */
    {
      orig = firstLeaf;
      prev = leaf;
      while (orig)
	{
	  el = orig; 
	  TtaNextSibling (&el);
	  while (el)
	    {
	      next = el;
	      TtaNextSibling (&next);
	      TtaRegisterElementDelete (el, doc);
	      TtaRemoveTree (el, doc);
	      TtaInsertSibling (el, prev, FALSE, doc);
	      prev = el;
	      el = next;
	    }
	  orig = TtaGetParent (orig);
	  if (orig == firstParag)
	    orig = NULL;
	  else
	    prev = TtaGetParent (prev);
	}
    }
  /* Close the latest undo sequence if it has been extended */
  if (undoSeqExtended)
    TtaCloseUndoSequence (doc);
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
   UpdateAttrNAME
   A NAME attribute has been created or modified.
   If it's a creation or modification, check that the ID is a unique name
   in the document.
   If it's a deletion for a SPAN element, remove that element if it's
   not needed.
  ----------------------------------------------------------------------*/
void UpdateAttrNAME (NotifyAttribute * event)
{
  MakeUniqueName (event->element, event->document);
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
#ifdef _SVG
  Update_element_id_on_timeline (event); 
#endif /* _SVG */
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
  ElementType         elType, childType;
  Element             el, child;

  StoreHeight (event);
  el = event->element;
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == HTML_EL_Object)
    /* the height attribute is attached to an Object element */
    {
      child = TtaGetFirstChild (el);
      if (child)
	{
	  childType = TtaGetElementType (child);
	  if (childType.ElTypeNum == HTML_EL_PICTURE_UNIT)
	    /* the Object element is of type image. Apply the height
	       attribute to the actual image element */
	    el = child;
	}
    }
  attrType = event->attributeType;
  attrType.AttrTypeNum = HTML_ATTR_IntHeightPxl;
  attr = TtaGetAttribute (el, attrType);
  if (attr == NULL)
    {
      attrType.AttrTypeNum = HTML_ATTR_IntHeightPercent;
      attr = TtaGetAttribute (el, attrType);
    }
  if (attr != NULL)
    TtaRemoveAttribute (el, attr, event->document);
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
  buffer = (char *)TtaGetMemory (buflen);
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
  ElementType         elType, childType;
  Element             el, child;

  StoreWidth (event);
  el = event->element;
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == HTML_EL_Object)
    /* the width attribute is attached to an Object element */
    {
      child = TtaGetFirstChild (el);
      if (child)
	{
	  childType = TtaGetElementType (child);
	  if (childType.ElTypeNum == HTML_EL_PICTURE_UNIT)
	    /* the Object element is of type image. Apply the width
	       attribute to the actual image element */
	    el = child;
	}
    }
  attrType = event->attributeType;
  attrType.AttrTypeNum = HTML_ATTR_IntWidthPxl;
  attr = TtaGetAttribute (el, attrType);
  if (attr == NULL)
    {
      attrType.AttrTypeNum = HTML_ATTR_IntWidthPercent;
      attr = TtaGetAttribute (el, attrType);
    }
  if (attr != NULL)
    TtaRemoveAttribute (el, attr, event->document);
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
  buffer = (char *)TtaGetMemory (buflen);
  TtaGiveTextAttributeValue (event->attribute, buffer, &length);
  CreateAttrWidthPercentPxl (buffer, event->element, event->document,
			     OldWidth);
  TtaFreeMemory (buffer);
  OldWidth = -1;
}

/*----------------------------------------------------------------------
   AttrAreaSizeDelete
   An attribute Area_Size (size of an input area) will be deleted.   
   Delete the corresponding attribute IntAreaSize.
  ----------------------------------------------------------------------*/
ThotBool AttrAreaSizeDelete (NotifyAttribute *event)
{
  CreateAttrIntAreaSize (20, event->element, event->document);
  return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   AttrAreaSizeModifed
   An attribute Area_Size (size of an input area) has been created or modified.
   Create the corresponding attribute IntAreaSize.
  ----------------------------------------------------------------------*/
void AttrAreaSizeModified (NotifyAttribute *event)
{
  int                 value;

  value = TtaGetAttributeValue (event->attribute);
  CreateAttrIntAreaSize (value, event->element, event->document);
}

/*----------------------------------------------------------------------
   AttrFontSizeCreated
   an HTML attribute "size" has been created for a Font element.   
   Create the corresponding internal attribute.                    
  ----------------------------------------------------------------------*/
void AttrFontSizeCreated (NotifyAttribute *event)
{
   char               *buffer = (char *)TtaGetMemory (buflen);
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
   char            *value = (char *)TtaGetMemory (buflen);
   int              length;

   value[0] = EOS;
   length = TtaGetTextAttributeLength (event->attribute);
   if (length >= buflen)
      length = buflen - 1;
   if (length > 0)
      TtaGiveTextAttributeValue (event->attribute, value, &length);

   if (event->attributeType.AttrTypeNum == HTML_ATTR_BackgroundColor)
      HTMLSetBackgroundColor (event->document, event->element, 0, value);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_background_)
      HTMLSetBackgroundImage (event->document, event->element, REPEAT,
			      0, value, FALSE);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_color ||
	    event->attributeType.AttrTypeNum == HTML_ATTR_TextColor)
      HTMLSetForegroundColor (event->document, event->element, 0, value);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_LinkColor)
      HTMLSetAlinkColor (event->document, event->element, value);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_VisitedLinkColor)
      HTMLSetAvisitedColor (event->document, event->element, value);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_ActiveLinkColor)
      HTMLSetAactiveColor (event->document, event->element, value);
   TtaFreeMemory (value);
}


/*----------------------------------------------------------------------
   AttrColorDelete
   An attribute color, TextColor or BackgroundColor is being       
   deleted.                                                        
  ----------------------------------------------------------------------*/
ThotBool AttrColorDelete (NotifyAttribute *event)
{
   if (event->attributeType.AttrTypeNum == HTML_ATTR_BackgroundColor)
      HTMLResetBackgroundColor (event->document, event->element);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_background_)
      HTMLResetBackgroundImage (event->document, event->element);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_color ||
	    event->attributeType.AttrTypeNum == HTML_ATTR_TextColor)
      HTMLResetForegroundColor (event->document, event->element);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_LinkColor)
      HTMLResetAlinkColor (event->document, event->element);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_VisitedLinkColor)
      HTMLResetAvisitedColor (event->document, event->element);
   else if (event->attributeType.AttrTypeNum == HTML_ATTR_ActiveLinkColor)
      HTMLResetAactiveColor (event->document, event->element);
   return FALSE;		/* let Thot perform normal operation */
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
       elType.ElTypeNum == HTML_EL_XHTML_Unknown_namespace ||
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
	       parentType.ElTypeNum == HTML_EL_XHTML_Unknown_namespace ||
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
   AttrScriptLanguageinMenu
   Don't display script_language attribute in menu
  ----------------------------------------------------------------------*/
ThotBool  AttrScriptLanguageinMenu (NotifyAttribute * event)
{
   ElementType         elType;

   elType = TtaGetElementType (event->element);
   if (elType.ElTypeNum == HTML_EL_SCRIPT_)
      return TRUE;
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
   SetCharFontOrPhrase (document, HTML_EL_Variable_);
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
  If name is true, take into account elements with a name or an ID attribute.
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
   char               *s;

   elAnchor = NULL;
   *HrefAttr = NULL;
   XLinkSchema = TtaGetSSchema ("XLink", doc);
   /* check the element and its ancestors */
   ancestor = element;
   do
     {
       attr = NULL;
       elType = TtaGetElementType (ancestor);
       s = TtaGetSSchemaName (elType.ElSSchema);
       if (!strcmp (s, "HTML"))
	 /* the current element belongs to the HTML namespace */
	 {
	   attrType.AttrSSchema = elType.ElSSchema;
	   if (name &&
	       (elType.ElTypeNum == HTML_EL_Anchor ||
		elType.ElTypeNum == HTML_EL_MAP ||
		elType.ElTypeNum == HTML_EL_map))
	     /* look for a name attribute */
	     {
	       attrType.AttrTypeNum = HTML_ATTR_NAME;
	       attr = TtaGetAttribute (ancestor, attrType);
	       /* stop the research */
	       elAnchor = ancestor;
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
       else if (!attr && !strcmp (s, "SVG"))
	 /* the current element belongs to the SVG namespace */
	 {
#ifdef _SVG
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
#endif /* _SVG */
	 }
       else if (!strcmp (s, "XLink"))
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
   while (elAnchor == NULL && ancestor);
   return elAnchor;
}


