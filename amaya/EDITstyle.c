/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Authors: I. Vatton
 *          R. Guetari (W3C/INRIA) Unicode and Windows version
 *
 */
 
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "undo.h"
#include "MathML.h"
#ifdef GRAPHML
#include "GraphML.h"
#endif
#include "fetchHTMLname.h"

#include "AHTURLTools_f.h"
#include "HTMLimage_f.h"
#include "HTMLpresentation_f.h"
#include "UIcss_f.h"
#include "css_f.h"
#include "fetchHTMLname_f.h"
#include "fetchXMLname_f.h"
#include "html2thot_f.h"
#include "init_f.h"
#include "styleparser_f.h"

#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */

static CHAR_T       ListBuffer[MAX_CSS_LENGTH];
static STRING       OldBuffer;
static int          NbClass = 0;
static CHAR_T       CurrentClass[80];
static Element      ClassReference;
static Document     DocReference;
static Document	    ApplyClassDoc;

/*----------------------------------------------------------------------
  RemoveElementStyle cleans all the presentation rules of a given element.
  The parameter removeSpan is True when the span has to be removed.
  ----------------------------------------------------------------------*/
static void  RemoveElementStyle (Element el, Document doc, ThotBool removeSpan)
{
   ElementType		elType;
   Attribute            attr;
   AttributeType        attrType;
   Element		firstChild, lastChild;
   STRING               name;

   if (el == NULL)
      return;
   elType = TtaGetElementType (el);
   /* if it's a MathML element, remove the style attribute defined in the
      MathML DTD */
   name = TtaGetSSchemaName (elType.ElSSchema);
   if (!ustrcmp (name, TEXT("MathML")))
      {
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = MathML_ATTR_style_;
      }
   else
#ifdef GRAPHML
   /* if it's a GraphML element, remove the style attribute defined in the
      GraphML DTD */
   if (!ustrcmp (name, TEXT("GraphML")))
      {
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = GraphML_ATTR_style_;
      }
   else
#endif
      /* remove the style attribute defined in the HTML DTD */
      {
      attrType.AttrSSchema = TtaGetSSchema (TEXT("HTML"), doc);
      attrType.AttrTypeNum = HTML_ATTR_Style_;
      }
   attr = TtaGetAttribute (el, attrType);
   if (attr != NULL)
      {
      TtaRegisterAttributeDelete (attr, el, doc);
      TtaRemoveAttribute (el, attr, doc);
      if (removeSpan)
	 DeleteSpanIfNoAttr (el, doc, &firstChild, &lastChild);
      TtaSetDocumentModified (doc);
      }

   /* remove all the specific presentation rules applied to the element */
   TtaCleanStylePresentation (el, NULL, doc);
}


/*----------------------------------------------------------------------
   UpdateCSSBackgroundImage searches strings url() or url("") within
   the styleString and make it relative to the newpath.
   oldpath = old document path
   newpath = new document path
   imgpath = new image directory
   If the image is not moved, the imgpath has to be NULL else the new
   image url is obtained by concatenation of imgpath and the image name.
   Returns NULL or a new allocated styleString.
  ----------------------------------------------------------------------*/
STRING              UpdateCSSBackgroundImage (STRING oldpath, STRING newpath,
					      STRING imgpath,
					      STRING styleString)
{
  STRING              b, e, ptr, oldptr, sString;
  CHAR_T              old_url[MAX_LENGTH];
  CHAR_T              tempname[MAX_LENGTH];
  CHAR_T              imgname[MAX_LENGTH];
  STRING              new_url;
  int                 len;

  ptr = NULL;
  sString = styleString;
  b = ustrstr (sString, TEXT("url"));
  while (b != NULL)
    {
      /* we need to compare this url with the new doc path */
      b += 3;
      b = SkipWCBlanksAndComments (b);
      if (*b == TEXT('('))
	{
	  b++;
	  b = SkipWCBlanksAndComments (b);
	  /*** Caution: Strings can either be written with double quotes or
	       with single quotes. Only double quotes are handled here.
	       Escaped quotes are not handled. See function SkipQuotedString */
	  if (*b == TEXT('"'))
	    {
	      /* search the url end */
	      b++;
	      e = b;
	      while (*e != WC_EOS && *e != TEXT('"'))
		e++;
	    }
	  else
	    {
	      /* search the url end */
	      e = b;
	      while (*e != WC_EOS && *e != TEXT(')'))
		e++;
	    }
	  if (*e != WC_EOS)
	    {
	      len = (int)(e - b);
	      ustrncpy (old_url, b, len);
	      old_url[len] = WC_EOS;
	      /* get the old full image name */
	      NormalizeURL (old_url, 0, tempname, imgname, oldpath);
	      /* build the new full image name */
	      if (imgpath != NULL)
		NormalizeURL (imgname, 0, tempname, imgname, imgpath);
	      new_url = MakeRelativeURL (tempname, newpath);
	      
	      /* generate the new style string */
	      if (ptr != NULL)
		{
		  oldptr = ptr;
		  len = - len + ustrlen (oldptr) + ustrlen (new_url) + 1;
		  ptr = TtaAllocString (len);	  
		  len = (int)(b - oldptr);
		  ustrncpy (ptr, oldptr, len);
		  sString = &ptr[len];
		  /* new name */
		  ustrcpy (sString, new_url);
		  /* following text */
		  ustrcat (sString, e);
		  TtaFreeMemory (oldptr);
		}
	      else
		{
		  len = - len + ustrlen (styleString) + ustrlen (new_url) + 1;
		  ptr = TtaAllocString (len);
		  len = (int)(b - styleString);
		  ustrncpy (ptr, styleString, len);
		  sString = &ptr[len];
		  /* new name */
		  ustrcpy (sString, new_url);
		  /* following text */
		  ustrcat (sString, e);
		}
	      TtaFreeMemory (new_url);
	    }
	  else
	    sString = b;
	}
      else
	sString = b;
      /* next background-image */
      b = ustrstr (sString, TEXT("url")); 
    }
  return (ptr);
}

/*----------------------------------------------------------------------
   UpdateStyleDelete : attribute Style will be deleted.            
   remove the existing style presentation.                      
  ----------------------------------------------------------------------*/
ThotBool            UpdateStyleDelete (NotifyAttribute * event)
{
   ElementType         elType;
   Element             el;
   CHAR_T*             style = NULL;
   int                 len;

   el = event->element;
   /*  A rule applying to BODY is really meant to address the HTML element */
   elType = TtaGetElementType (event->element);
   if (elType.ElTypeNum == HTML_EL_BODY &&
       ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")) == 0)
      el = TtaGetParent (el);
   len = TtaGetTextAttributeLength (event->attribute);
   if ((len < 0) || (len > 10000))
      /* error */
      return FALSE;
   if (len == 0)
      /* empty Style attribute */
      return FALSE;
   else
     {
	/* parse the old content and remove the corresponding presentation
	   rules */
	style = TtaAllocString (len + 2);
	if (!style)
	   return FALSE;
	TtaGiveTextAttributeValue (event->attribute, style, &len);
	style[len] = WC_EOS;
	ParseHTMLSpecificStyle (el, style, event->document, 1, TRUE);
	TtaFreeMemory (style);
     }
  return FALSE;  /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  ChangeStyle
  the STYLE element will be changed in the document HEAD.
  ----------------------------------------------------------------------*/
ThotBool            ChangeStyle (NotifyElement * event)
{
  OldBuffer = GetStyleContents (event->element);
  return FALSE;  /* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
  DeleteStyle
  the STYLE element will be deleted in the document HEAD.
  ----------------------------------------------------------------------*/
ThotBool            DeleteStyle (NotifyElement * event)
{
  RemoveStyleSheet (NULL, event->document, TRUE, TRUE);
  return FALSE;  /* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
  EnableStyleElement
  the STYLE element must be reparsed.
  ----------------------------------------------------------------------*/
void           EnableStyleElement (Document doc)
{
  Element               el;
  ElementType		elType;
  STRING                name;
  STRING                buffer;

  el = TtaGetMainRoot (doc);
  elType = TtaGetElementType (el);
  name = TtaGetSSchemaName (elType.ElSSchema);
#ifdef GRAPHML
   /* if it's a SVG document, remove the style defined in the GraphML DTD */
   if (!ustrcmp (name, TEXT("GraphML")))
     elType.ElTypeNum = GraphML_EL_style__;
   else
#endif
     elType.ElTypeNum = HTML_EL_STYLE_;
   el = TtaSearchTypedElement (elType, SearchForward, el);
   if (el)
     {
       /* get the style element in the document head */
       buffer = GetStyleContents (el);
       ApplyCSSRules (el, buffer, doc, FALSE);
     }
}

/*----------------------------------------------------------------------
  DeleteStyleElement
  the STYLE element will be deleted in the document HEAD.
  ----------------------------------------------------------------------*/
void           DeleteStyleElement (Document doc)
{
  Element               el;
  ElementType		elType;
  STRING                name;

  RemoveStyleSheet (NULL, doc, TRUE, TRUE);
  /* get the style element in the document head */
  el = TtaGetMainRoot (doc);
  elType = TtaGetElementType (el);
  name = TtaGetSSchemaName (elType.ElSSchema);
#ifdef GRAPHML
   /* if it's a SVG document, remove the style defined in the GraphML DTD */
   if (!ustrcmp (name, TEXT("GraphML")))
     elType.ElTypeNum = GraphML_EL_style__;
   else
#endif
     elType.ElTypeNum = HTML_EL_STYLE_;
   el = TtaSearchTypedElement (elType, SearchForward, el);
   if (el)
     {
       TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
       TtaRegisterElementDelete (el, doc);
       TtaDeleteTree (el, doc);
       TtaCloseUndoSequence (doc);
     }
}


/*----------------------------------------------------------------------
   StyleChanged
   A STYLE element has been changed in the HEAD
  ----------------------------------------------------------------------*/
void                StyleChanged (NotifyAttribute * event)
{
  STRING              buffer, ptr1, ptr2;
  STRING              pEnd, nEnd;
  CHAR_T              c;
  int                 i, j;
  int                 previousEnd, nextEnd;
  int                 braces;

  buffer = GetStyleContents (event->element);
  /* compare both strings */
  i = 0;
  ptr1 = buffer;
  previousEnd = i;
  pEnd = ptr1;
  braces = 0;
  if (!OldBuffer)
    /* This is a brand new style element */
    ApplyCSSRules (event->element, ptr1, event->document, FALSE);
  else
    {
      /* handle only differences */
      while (OldBuffer[i] == *ptr1 && *ptr1 != WC_EOS)
	{
	  if (i > 0 && OldBuffer[i-1] == TEXT('{'))
	    braces++;
	  if (i > 0 &&
	      (OldBuffer[i-1] == TEXT('}') ||
	       ((OldBuffer[i-1] == TEXT(';') || OldBuffer[i-1] == TEXT('>')) &&
		braces == 0)))
	    {
	      if (OldBuffer[i-1] == TEXT('}'))
		braces--;
	      previousEnd = i;
	      pEnd = ptr1;
	    }
	  i++;
	ptr1++;
	}
      
      /* now ptr1 and OldBuffer[i] point to different strings */
      if (*ptr1 != WC_EOS)
	{
	  ptr2 = ptr1 + ustrlen (ptr1);
	  j = i + ustrlen (&OldBuffer[i]);
	  nextEnd = j;
	  nEnd = ptr2;
	  braces = 0;
	  while (OldBuffer[j] == *ptr2 && ptr2 != ptr1)
	    {
	      if (j > i && OldBuffer[j-1] == TEXT('{'))
		braces++;
	      if (j > i &&
		  (OldBuffer[j-1] == TEXT('}') ||
           ((OldBuffer[j-1] == TEXT('@') || OldBuffer[j-1] == TEXT('<')) &&
		    braces == 0)))
		{
		  if (OldBuffer[j-1] == TEXT('}'))
		    braces--;
		  nextEnd = j;
		  nEnd = ptr2;
		}
	      j--;
	      ptr2--;
	    }
	  if (ptr1 != ptr2)
	    {
	      /* take complete CSS rules */
	      OldBuffer[nextEnd] = WC_EOS;
	      *nEnd = WC_EOS;
	  
	      /* remove previous rules */
	      ptr1 = &OldBuffer[previousEnd];
	      ptr2 = ptr1;
	      do
		{
		  while (*ptr2 != TEXT('}') && *ptr2 != WC_EOS)
		    ptr2++;
		  if (*ptr2 != WC_EOS)
		    ptr2++;
		  /* cut here */
		  c = *ptr2;
		  *ptr2 = WC_EOS;
		  ApplyCSSRules (event->element, ptr1, event->document, TRUE);
		  /**** update image contexts
			url1 = GetCSSBackgroundURL (ptr1);
			if (url1 != NUL)
			{
			sprintf (path, "%s%s%d%s", TempFileDirectory, DIR_STR,
			         event->document, DIR_STR, url1);
			pImage = SearchLoadedImage (path, event->document);
			
			}
		  ***/
		  *ptr2 = c;
		  ptr1 = ptr2;
		}
	      while (*ptr2 != WC_EOS);
	      
	      /* add new rules */
	      ptr1 = pEnd;
	      ptr2 = ptr1;
	      do
		{
		  while (*ptr2 != TEXT('}') && *ptr2 != WC_EOS)
		    ptr2++;
		  if (*ptr2 != WC_EOS)
		    ptr2++;
		  /* cut here */
		  c = *ptr2;
		  *ptr2 = WC_EOS;
		  ApplyCSSRules (event->element, ptr1, event->document, FALSE);
		  *ptr2 = c;
		  ptr1 = ptr2;
		}
	      while (*ptr2 != WC_EOS);
	    }
	}
      
      TtaFreeMemory (OldBuffer);
      OldBuffer = NULL;
    }
  TtaFreeMemory (buffer);
}


/*----------------------------------------------------------------------
   UpdateStylePost : attribute Style has been updated or created.  
   reflect the new style presentation.                          
  ----------------------------------------------------------------------*/
void                UpdateStylePost (NotifyAttribute * event)
{
   Element             el, firstChild, lastChild, oldParent, newParent;
   ElementType	       elType;
   Document            doc;
   Attribute           at;
   AttributeType       atType;
   CHAR_T*             style = NULL;
   int                 len;

   el = event->element;
   doc = event->document;

   len = TtaGetTextAttributeLength (event->attribute);
   if ((len < 0) || (len > 10000))
      return;
   if (len == 0)
     {
	/* empty Style attribute. Delete it */
	elType = TtaGetElementType (el);
        /* if it's a MathML element, delete the style attribute defined in the
           MathML DTD */
	if (!ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("MathML")))
	   {
	   atType.AttrSSchema = elType.ElSSchema;
	   atType.AttrTypeNum = MathML_ATTR_style_;
	   }
	else
#ifdef GRAPHML
        /* if it's a GraphML element, delete the style attribute defined in the
           GraphML DTD */
	if (!ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("GraphML")))
	   {
	   atType.AttrSSchema = elType.ElSSchema;
	   atType.AttrTypeNum = GraphML_ATTR_style_;
	   }
	else
#endif
	   /* delete the style attribute defined in the HTML DTD */
	   {
	   atType.AttrSSchema = TtaGetSSchema (TEXT("HTML"), doc);
	   atType.AttrTypeNum = HTML_ATTR_Style_;
	   }
	at = TtaGetAttribute (el, atType);
	if (at != NULL)
	  {
	     /* The attribute value change has been registered in the
		Undo queue.  Replace this operation by the removal of the
		attribute */
	     TtaReplaceLastRegisteredAttr (doc);
	     /* Delete the Style attribute */
	     TtaRemoveAttribute (el, at, doc);
	     /* if the Style attribute was associated with a Span element,
		remove this element (but not its content) if it has no
		other attribute */
	     DeleteSpanIfNoAttr (el, doc, &firstChild, &lastChild);
	     TtaSetDocumentModified (doc);
	  }
     }
   else
     {
	/* parse and apply the new style content */
	style = TtaAllocString (len + 2);
	if (style == NULL)
	   return;
	TtaGiveTextAttributeValue (event->attribute, style, &len);
	style[len] = WC_EOS;
	/* create a Span element if it's a TEXT leaf */
	oldParent = TtaGetParent (el);
	AttrToSpan (el, event->attribute, doc);
	newParent = TtaGetParent (el);
	if (newParent != oldParent)
	   /* a new SPAN element has been created. Generate the PRules
	      for the SPAN element */
	   el = newParent;
	ParseHTMLSpecificStyle (el, style, doc, 1, FALSE);
	TtaFreeMemory (style);
     }
}

/*----------------------------------------------------------------------
   DoApplyClass
   Put a class attribute on all selected elements
  ----------------------------------------------------------------------*/
static void         DoApplyClass (Document doc)
{
  Element             firstSelectedEl, lastSelectedEl, curEl, el, span, next,
		      firstChild, lastChild, parent;
  ElementType	      elType;
  Attribute           attr;
  AttributeType       attrType;
  CHAR_T*             a_class = CurrentClass;
  int		      firstSelectedChar, lastSelectedChar, i, lg;
  DisplayMode         dispMode;
  ThotBool	      setClassAttr;

  if (!a_class)
    return;

  /* remove any leading dot in a class definition. */
  if (*a_class == TEXT('.'))
    a_class++;
  if (*a_class == WC_EOS)
    return;

  TtaGiveFirstSelectedElement (doc, &firstSelectedEl, &firstSelectedChar, &i);
  if (firstSelectedEl == NULL)
     return;
  TtaClearViewSelections ();
  /* stop displaying changes that will be made in the document */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (doc, DeferredDisplay);

  if (ustrcmp (CurrentClass, TEXT("default")) &&
      !IsImplicitClassName (CurrentClass, doc))
     setClassAttr = TRUE;
  else
     setClassAttr = FALSE;

  TtaGiveLastSelectedElement (doc, &lastSelectedEl, &i, &lastSelectedChar);
  TtaUnselect (doc);

  /* process the last selected element */
  elType = TtaGetElementType (lastSelectedEl);
  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
     /* it's a text element */
     {
     lg = TtaGetTextLength (lastSelectedEl);
     if (lastSelectedChar < lg && lastSelectedChar != 0)
	/* the last selected element is only partly selected. Split it */
	TtaSplitText (lastSelectedEl, lastSelectedChar, doc);
     else
	/* selection ends at the end of the text element */
	if (lastSelectedEl != firstSelectedEl ||
	    (lastSelectedEl == firstSelectedEl && firstSelectedChar <= 1))
	   /* this text element is entirely selected */
	   {
	   parent = TtaGetParent (lastSelectedEl);
	   elType = TtaGetElementType (parent);
	   if (elType.ElTypeNum == HTML_EL_Span &&
	       !ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")))
	      /* the parent element is a SPAN */
	      if (lastSelectedEl == TtaGetFirstChild (parent) &&
	          lastSelectedEl == TtaGetLastChild (parent))
	         /* this text element is the only child of the SPAN */
	         /* Process the SPAN instead of the text element */
	         {
	         lastSelectedEl = parent;
	         if (firstSelectedEl == lastSelectedEl)
		    firstSelectedEl = parent;
	         }
	}
     }

  /* process the first selected element */
  elType = TtaGetElementType (firstSelectedEl);
  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
    {
      /* it's a text element */
      if (firstSelectedChar <= 1)
	/* selection starts at the beginning of the element */
	/* this text element is then entirely selected */
	{
	  parent = TtaGetParent (firstSelectedEl);
	  elType = TtaGetElementType (parent);
	  if (elType.ElTypeNum == HTML_EL_Span &&
	      !ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")))
	    /* parent is a SPAN element */
	    if (firstSelectedEl == TtaGetFirstChild (parent) &&
		firstSelectedEl == TtaGetLastChild (parent))
	      /* this text element is the only child of the SPAN */
	      /* Process the SPAN instead of the text element */
	      {
		firstSelectedEl = parent;
		if (lastSelectedEl == firstSelectedEl)
		  lastSelectedEl = parent;
	      }
	}
      else
	/* that element is only partly selected. Split it */
	{
	el = firstSelectedEl;
	TtaSplitText (firstSelectedEl, firstSelectedChar - 1, doc);
	TtaNextSibling (&firstSelectedEl);
	if (lastSelectedEl == el)
	   {
	   /* we have to change the end of selection because the last
	      selected element was split */
	   lastSelectedEl = firstSelectedEl;
	   }
	}
    }

  TtaOpenUndoSequence (doc, firstSelectedEl, lastSelectedEl, 0, 0);
  /* process all selected elements */
  curEl = firstSelectedEl;
  while (curEl != NULL)
     {
      /* The current element may be deleted by DeleteSpanIfNoAttr. So, get
	 first the next element to be processed */
      if (curEl == lastSelectedEl)
         next = NULL;
      else
	 {
         next = curEl;
         TtaGiveNextElement (doc, &next, lastSelectedEl);
	 }

      if (!setClassAttr)
	 {
	 DeleteSpanIfNoAttr (curEl, doc, &firstChild, &lastChild);
	 if (firstChild)
	    {
	    if (curEl == firstSelectedEl)
	        firstSelectedEl = firstChild;
	    if (curEl == lastSelectedEl)
		lastSelectedEl = lastChild;
	    }
	 }
      else
	 {
	  elType = TtaGetElementType (curEl);
	  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
	      elType.ElTypeNum == HTML_EL_Basic_Elem)
	    {
	      /* that's a text element */
	      if (ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")))
		/* not a HTML element, move to the parent element */
		curEl = TtaGetParent (curEl);
	      else
	        /* we are in a HTML element. Create an enclosing SPAN element*/
	        {
		  MakeASpan (curEl, &span, doc);
		  if (span)
		    /* a SPAN element was created */
		    {
		      if (curEl == firstSelectedEl)
			{
			  firstSelectedEl = span;
			  if (firstSelectedEl == lastSelectedEl)
			    lastSelectedEl = span;
			}
		      else if (curEl == lastSelectedEl)
			lastSelectedEl = span;
		      curEl = span;
		    }
	        }
	    }
	  if (!ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("MathML")))
	    {
	      attrType.AttrSSchema = elType.ElSSchema;
	      attrType.AttrTypeNum = MathML_ATTR_class;
	    }
	  else
#ifdef GRAPHML
	    if (!ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("GraphML")))
	      {
		attrType.AttrSSchema = elType.ElSSchema;
		attrType.AttrTypeNum = GraphML_ATTR_class;
	      }
	    else
#endif
	      {
		attrType.AttrSSchema = TtaGetSSchema (TEXT("HTML"), doc);
		attrType.AttrTypeNum = HTML_ATTR_Class;
	      }
	  /* set the Class attribute of the element */
	  attr = TtaGetAttribute (curEl, attrType);
	  if (!attr)
	    {
	      attr = TtaNewAttribute (attrType);
	      TtaAttachAttribute (curEl, attr, doc);
	      TtaSetAttributeText (attr, a_class, curEl, doc);
	      TtaRegisterAttributeCreate (attr, curEl, doc);
	     }
	  else
	     {
	     TtaRegisterAttributeReplace (attr, curEl, doc);
	     TtaSetAttributeText (attr, a_class, curEl, doc);
	     }
	  TtaSetDocumentModified (doc);
	 }
      /* jump to the next element */
      curEl = next;
     }
  TtaCloseUndoSequence (doc);

  /* ask Thot to display changes made in the document */
  TtaSetDisplayMode (doc, dispMode);
  TtaSelectElement (doc, firstSelectedEl);
  if (lastSelectedEl != firstSelectedEl)
     TtaExtendSelection (doc, lastSelectedEl, 0);
}

/*----------------------------------------------------------------------
   SpecificSettingsToCSS :  Callback for ApplyAllSpecificSettings,
       enrich the CSS string.
  ----------------------------------------------------------------------*/
static void  SpecificSettingsToCSS (Element el, Document doc,
				    PresentationSetting settings, void *param)
{
  LoadedImageDesc    *imgInfo;
  CHAR_T*             css_rules = param;
  CHAR_T              string[150];
  CHAR_T*             ptr;

  string[0] = WC_EOS;
  if (settings->type == PRBackgroundPicture)
    {
      /* transform absolute URL into relative URL */
      imgInfo = SearchLoadedImage (settings->value.pointer, 0);
      if (imgInfo != NULL)
	ptr = MakeRelativeURL (imgInfo->originalName, DocumentURLs[doc]);
      else
	ptr = MakeRelativeURL (settings->value.pointer, DocumentURLs[doc]);
      settings->value.pointer = ptr;
      PToCss (settings, string, sizeof(string), el);
      TtaFreeMemory (ptr);
    }
  else
    PToCss (settings, string, sizeof(string), el);

  if (string[0] != WC_EOS && *css_rules != WC_EOS)
    ustrcat (css_rules, TEXT("; "));
  if (string[0] != WC_EOS)
    ustrcat (css_rules, string);
}

/*----------------------------------------------------------------------
  GetHTMLStyleString : return a string corresponding to the CSS
  description of the presentation attribute applied to a element.
  For stupid reasons, if the target element is HTML or BODY,
  one returns the concatenation of both element style strings.
  ----------------------------------------------------------------------*/
void        GetHTMLStyleString (Element el, Document doc, STRING buf, int *len)
{
  ElementType        elType;
  STRING             name;

  if (buf == NULL || len == NULL || *len <= 0)
    return;

  /*
   * this will transform all the Specific Settings associated to
   * the element to one CSS string.
   */
  buf[0] = WC_EOS;
  TtaApplyAllSpecificSettings (el, doc, SpecificSettingsToCSS, &buf[0]);
  *len = ustrlen (buf);

  /* BODY / HTML elements specific handling */
  elType = TtaGetElementType (el);
  name = TtaGetSSchemaName (elType.ElSSchema);
  if (ustrcmp(name, TEXT("HTML")) == 0)
    {
      if (elType.ElTypeNum == HTML_EL_HTML)
	{
	  elType.ElTypeNum = HTML_EL_BODY;
	  el = TtaSearchTypedElement(elType, SearchForward, el);
	  if (!el)
	    return;
	  if (*len > 0)
	    ustrcat(buf, TEXT(";"));
	  *len = ustrlen (buf);
	  TtaApplyAllSpecificSettings (el, doc, SpecificSettingsToCSS,
				       &buf[*len]);
	  *len = ustrlen (buf);
	}
      else if (elType.ElTypeNum == HTML_EL_BODY)
	{
	  el = TtaGetParent (el);
	  if (!el)
	    return;
	  if (*len > 0)
	    ustrcat(buf, TEXT(";"));
	  *len = ustrlen (buf);
	  TtaApplyAllSpecificSettings (el, doc, SpecificSettingsToCSS,
				       &buf[*len]);
	  *len = ustrlen (buf);
	}
    }
}

/*----------------------------------------------------------------------
   UpdateClass
   Change or create a class attribute to reflect the Style attribute
   of the selected element.
  ----------------------------------------------------------------------*/
static void         UpdateClass (Document doc)
{
  Attribute           attr;
  AttributeType       attrType;
  Element             el, parent, child, title, head, line, prev;
  ElementType         elType, selType;
  STRING              stylestring;
  STRING              text;
  STRING              a_class;
  STRING              schName;
  int                 len, base, i;
  Language            lang;
  ThotBool            found, empty, insertNewLine;

  /* check whether it's an element type or a class name */
  elType = TtaGetElementType (ClassReference);
  GIType (CurrentClass, &selType, doc);
  if (selType.ElTypeNum != elType.ElTypeNum && selType.ElTypeNum != 0)
    {
      /* it's an invalid element type */
      TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_INVALID_TYPE), NULL);
      return;
    }

  /* get the current style attribute*/
  schName = TtaGetSSchemaName (elType.ElSSchema);
  if (ustrcmp (schName, TEXT("MathML")) == 0)
    {
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = MathML_ATTR_style_;
    }
  else
#ifdef GRAPHML
  if (ustrcmp (schName, TEXT("GraphML")) == 0)
    {
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = GraphML_ATTR_style_;
    }
  else
#endif
    {
      attrType.AttrSSchema = TtaGetSSchema (TEXT("HTML"), doc);
      attrType.AttrTypeNum = HTML_ATTR_Style_;
    }
  attr = TtaGetAttribute (ClassReference, attrType);
  base = ustrlen (CurrentClass) + 2;
  if (attr)
    len = TtaGetTextAttributeLength (attr) + base + 3;
  else
    len = base + 3;
  /* create a string containing the new CSS definition. */
  stylestring = TtaAllocString (len);
  stylestring[0] = WC_EOS;
  if (selType.ElTypeNum == 0)
    {
      /* it's not an element type */
      if (CurrentClass[0] != '.' && CurrentClass[0] != '#')
	{
	  /* it's an invalid class name, insert a dot */
	  ustrcat (stylestring, TEXT("."));
	  base++;
	}
    }
  ustrcat (stylestring, CurrentClass);
  ustrcat (stylestring, TEXT(" {"));
  if (attr)
    {
      len = len - base;
      TtaGiveTextAttributeValue (attr, &stylestring[base], &len);
    }
  ustrcat (stylestring, TEXT("}"));
  TtaOpenUndoSequence (doc, ClassReference, ClassReference, 0, 0);

  /* create the class attribute */
  if (selType.ElTypeNum == 0)
    {
      a_class = &CurrentClass[0];
      if (*a_class == '.')
	 a_class++;
      if (!ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("MathML")))
	 {
	   attrType.AttrSSchema = elType.ElSSchema;
	   attrType.AttrTypeNum = MathML_ATTR_class;
	 }
      else
#ifdef GRAPHML
      if (!ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("GraphML")))
	 {
	   attrType.AttrSSchema = elType.ElSSchema;
	   attrType.AttrTypeNum = GraphML_ATTR_class;
	 }
      else
#endif
	 {
	   attrType.AttrSSchema = TtaGetSSchema (TEXT("HTML"), doc);
	   attrType.AttrTypeNum = HTML_ATTR_Class;
	 }
      attr = TtaGetAttribute (ClassReference, attrType);
      if (!attr)
	{
	  attr = TtaNewAttribute (attrType);
	  TtaAttachAttribute (ClassReference, attr, doc);
          TtaSetAttributeText (attr, a_class, ClassReference, doc);
	  TtaRegisterAttributeCreate (attr, ClassReference, doc);
	}
      else
	{
	  TtaRegisterAttributeReplace (attr, ClassReference, doc);
          TtaSetAttributeText (attr, a_class, ClassReference, doc);
	}
      TtaSetDocumentModified (doc);
    }

  /* remove the Style attribute */
  RemoveElementStyle (ClassReference, doc, FALSE);

  /* generate or update the style element in the document head */
  parent = TtaGetMainRoot (doc);
  elType = TtaGetElementType (parent);
  elType.ElTypeNum = HTML_EL_HEAD;
  head = TtaSearchTypedElement (elType, SearchForward, parent);
  el = head;
  elType.ElTypeNum = HTML_EL_STYLE_;
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = HTML_ATTR_Notation;
  found = FALSE;
  while (!found && el)
    {
      /* is there any style element? */
      el = TtaSearchTypedElementInTree (elType, SearchForward, head, el);
      if (el)
	{
	  /* does this style element have an attribute type="text/css" ? */
	  attr = TtaGetAttribute (el, attrType);
	  if (attr)
	    {
	      len = TtaGetTextAttributeLength (attr);
	      a_class = TtaAllocString (len + 1);
	      TtaGiveTextAttributeValue (attr, a_class, &len);
	      found = (!ustrcmp (a_class, TEXT("text/css")));
	      TtaFreeMemory (a_class);
	    }
	}
    }

  insertNewLine = FALSE;
  if (!found && head)
    {
      /* the STYLE element doesn't exist we create it now */
      el = TtaNewTree (doc, elType, "");
      /* insert the new style element after the title if it exists */
      elType.ElTypeNum = HTML_EL_TITLE;
      title = TtaSearchTypedElementInTree (elType, SearchForward, head, head);
      if (title != NULL)
	TtaInsertSibling (el, title, FALSE, doc);
      else
	TtaInsertFirstChild (&el, head, doc);
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
      TtaSetAttributeText (attr, TEXT("text/css"), el, doc);
    }

  child = TtaGetLastChild (el);
  if (child == NULL)
    {
      /* there is no TEXT element within the STYLE element. We create it now */
      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
      child = TtaNewTree (doc, elType, "");
      TtaInsertFirstChild (&child, el, doc);
      /* remember the element to be registered in the Undo queue */
      el = child;
      found = FALSE;
    }
  else
    {
    elType = TtaGetElementType (child);
    if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
      /* if the last child of the STYLE element is an empty text leaf,
	 skip it */
      {
	len = TtaGetTextLength (child) + 1;
	text = TtaAllocString (len);
	TtaGiveTextContent (child, text, &len, &lang);
	empty = TRUE;
	insertNewLine = TRUE;
	for (i = len - 1; i >= 0 && empty; i--)
	  {
	  empty = text[i] <= SPACE;
          if ((int) text[i] == EOL || (int) text[i] == __CR__)
	     insertNewLine = FALSE;
	  }
	TtaFreeMemory (text);
	if (empty)
	  {
	    prev = child;
	    TtaPreviousSibling (&prev);
	    if (prev)
	      {
		child = prev;
	        elType = TtaGetElementType (child);
	      }
	  }
      }
    if (elType.ElTypeNum != HTML_EL_TEXT_UNIT)
      {
	if (elType.ElTypeNum != HTML_EL_Comment_)
	  /* the last child of the STYLE element is neither a text leaf nor
	     a comment. Don't do anything */
	  child = NULL;
	else
	  /* the last child of the STYLE element is a comment */
	  /* insert the new style rule within the Comment_line */
	  {
	    line = TtaGetLastChild (child);
	    if (line)
	      /* there is already a Comment_line */
	      {
		child = TtaGetLastChild (line);
		len = TtaGetTextLength (child) + 1;
		text = TtaAllocString (len);
		TtaGiveTextContent (child, text, &len, &lang);
		empty = TRUE;
		insertNewLine = TRUE;
		for (i = len - 1; i >= 0 && empty; i--)
		  {
		    empty = text[i] <= SPACE;
		    if ((int) text[i] == EOL || (int) text[i] == __CR__)
		      insertNewLine = FALSE;
		  }
		TtaFreeMemory (text);
	      }
	    else
	      /* create a Comment_line within the Comment */
	      {
		elType.ElTypeNum = HTML_EL_Comment_line;
		line = TtaNewTree (doc, elType, "");
		TtaInsertFirstChild (&line, child, doc);
		child = TtaGetLastChild (line);
		insertNewLine = FALSE;
		/* remember the element to register in the undo queue */
		found = FALSE;
		el = line;
	      }
	  }
      }
    }

  if (child)
    {
    if (found)
      /* Register the previous value of the STYLE element in the Undo queue */
      TtaRegisterElementReplace (child, doc);
    /* update the STYLE element */
    len = TtaGetTextLength (child);
    if (insertNewLine)
      {
       TtaInsertTextContent (child, len, TEXT("\n"), doc);
       len++;
      }
    TtaInsertTextContent (child, len, stylestring, doc);
    /* parse and apply this new CSS to the current document */
    ReadCSSRules (doc, NULL, stylestring, TRUE);
    }

  if (!found)
    /* Register the created STYLE or child element in the Undo queue */
    TtaRegisterElementCreate (el, doc);
  TtaCloseUndoSequence (doc);

  /* free the stylestring now */
  TtaFreeMemory (stylestring);
  stylestring = NULL;
}

/*----------------------------------------------------------------------
   PutClassName
   Put the value of attribute attr at the end of the buff buffer if
   it's not there already.
  ----------------------------------------------------------------------*/
static void PutClassName (Attribute attr, STRING buf, int* index, int* free,
			  int* nb)
{
  int		len, cur, i;
  CHAR_T        selector[100];
  ThotBool      found;

  len = 99;
  TtaGiveTextAttributeValue (attr, selector, &len);
  /* compare with all already known class names */
  cur = 0;
  found = FALSE;
  for (i = 0; i < *nb && !found; i++)
    {
      if (buf[cur] == '.')
	cur++;
      len = ustrlen (&buf[cur]) + 1;
      found = !ustrcmp (selector, &buf[cur]);
      cur += len;
    }
  if (!found)
    {
      len = ustrlen (selector);
      if (len > *free)
	return;
      /* add this new class name and the dot */
      buf[(*index)++] = '.';
      ustrcpy (&buf[*index], selector);
      len++; /* add the \0 */
      *free -= len;
      *index += len;
      (*nb)++;
    }
}

/*----------------------------------------------------------------------
   BuildClassList
   Build the whole list of class names in use after the first name.
  ----------------------------------------------------------------------*/
static int          BuildClassList (Document doc, STRING buf, int size,
				    STRING first)
{
  Element             el;
  Attribute           attr;
  AttributeType       attrType;
  int                 free;
  int                 len;
  int                 nb;
  int                 index;

  /* add the first element if specified */
  buf[0] = WC_EOS;
  nb = 0;
  index = 0;
  free = size;
  if (first)
    {
      ustrcpy (&buf[index], first);
      len = ustrlen (first);
      len++;
      free -= len;
      index += len;
      nb++;
    }

  /* list all class values */
  /* looks first for the Class attribute defined in the HTML DTD */
  attrType.AttrSSchema = TtaGetSSchema (TEXT("HTML"), doc);
  attrType.AttrTypeNum = HTML_ATTR_Class;
  el = TtaGetMainRoot (doc);
  while (el != NULL)
     {
     TtaSearchAttribute (attrType, SearchForward, el, &el, &attr);
     if (attr != NULL)
	PutClassName (attr, buf, &index, &free, &nb);
     }
  /* looks for the class attribute defined in the MathML DTD */
  attrType.AttrSSchema = TtaGetSSchema (TEXT("MathML"), doc);
  if (attrType.AttrSSchema)
     /* there are some MathML elements in this document */
     {
     attrType.AttrTypeNum = MathML_ATTR_class;
     el = TtaGetMainRoot (doc);
     while (el != NULL)
        {
        TtaSearchAttribute (attrType, SearchForward, el, &el, &attr);
        if (attr != NULL)
	   PutClassName (attr, buf, &index, &free, &nb);
        }
     }
#ifdef GRAPHML
  /* looks for the class attribute defined in the GraphML DTD */
  attrType.AttrSSchema = TtaGetSSchema (TEXT("GraphML"), doc);
  if (attrType.AttrSSchema)
     /* there are some GraphML elements in this document */
     {
     attrType.AttrTypeNum = GraphML_ATTR_class;
     el = TtaGetMainRoot (doc);
     while (el != NULL)
        {
        TtaSearchAttribute (attrType, SearchForward, el, &el, &attr);
        if (attr != NULL)
	   PutClassName (attr, buf, &index, &free, &nb);
        }
     }
#endif
  /* look for all class names that are used in the STYLE element and in
     all style sheets currently associated with the document */
  /*** TODO ***/
  return (nb);
}
	    
/*----------------------------------------------------------------------
   CreateClass
   creates a class from the Style attribute of the selected element
  ----------------------------------------------------------------------*/
void                CreateClass (Document doc, View view)
{
  Attribute           attr;
  AttributeType       attrType;
  Element             last_elem;
  ElementType         elType;
  CHAR_T              a_class[50];
  STRING              elHtmlName;
  STRING              schName;
  int                 len, i, j;
  int                 firstSelectedChar, lastSelectedChar;

  DocReference = doc;
  CurrentClass[0] = 0;
  ClassReference = NULL;
  TtaGiveFirstSelectedElement (doc, &ClassReference,
			       &firstSelectedChar, &lastSelectedChar);
  TtaGiveLastSelectedElement (doc, &last_elem, &i, &j);
  
  /* one can only define a style from one element at a time. */
  if (last_elem != ClassReference)
    return;
  if (ClassReference == NULL)
    return;

  /* if only a part of an element is selected, select the parent instead */
  elType = TtaGetElementType (ClassReference);
  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
      elType.ElTypeNum == HTML_EL_GRAPHICS_UNIT)
    {
      ClassReference = TtaGetParent (ClassReference);
      elType = TtaGetElementType (ClassReference);
    }

  /* check if the element has a style attribute */
  schName = TtaGetSSchemaName (elType.ElSSchema);
  if (ustrcmp (schName, TEXT("MathML")) == 0)
    {
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = MathML_ATTR_style_;
    }
  else
#ifdef GRAPHML
  if (ustrcmp (schName, TEXT("GraphML")) == 0)
    {
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = GraphML_ATTR_style_;
    }
  else
#endif
    {
      attrType.AttrSSchema = TtaGetSSchema (TEXT("HTML"), doc);
      attrType.AttrTypeNum = HTML_ATTR_Style_;
    }
  attr = TtaGetAttribute (ClassReference, attrType);
  if (attr == NULL)
    /* no attribute style */
    return;

  /* update the class name selector. */
  elHtmlName =  GetXMLElementName (TtaGetElementType (ClassReference), doc);
  if (elHtmlName[0] == TEXT('?'))
    InitConfirm (doc, 1, TtaGetMessage (AMAYA, AM_SEL_CLASS));
  else
    {
#ifndef _WINDOWS
      TtaNewForm (BaseDialog + ClassForm, TtaGetViewFrame (doc, 1), 
		  TtaGetMessage (AMAYA, AM_DEF_CLASS), FALSE, 2, 'L', D_DONE);
#endif /* !_WINDOWS */
      NbClass = BuildClassList (doc, ListBuffer, MAX_CSS_LENGTH, elHtmlName);
#ifndef _WINDOWS
      TtaNewSelector (BaseDialog + ClassSelect, BaseDialog + ClassForm,
		      TtaGetMessage (AMAYA, AM_SEL_CLASS),
		      NbClass, ListBuffer, 5, NULL, TRUE, FALSE);
#endif /* !_WINDOWS */
      
      /* preselect the entry corresponding to the class of the element. */
      if (!ustrcmp (schName, TEXT("MathML")))
	attrType.AttrTypeNum = MathML_ATTR_class;
      else
#ifdef GRAPHML
	if (!ustrcmp (schName, TEXT("GraphML")))
	  attrType.AttrTypeNum = GraphML_ATTR_class;
	else
#endif
	  attrType.AttrTypeNum = HTML_ATTR_Class;

      attr = TtaGetAttribute (ClassReference, attrType);
      if (attr)
	{
	  len = 50;
	  TtaGiveTextAttributeValue (attr, a_class, &len);
#ifndef _WINDOWS
	  TtaSetSelector (BaseDialog + ClassSelect, -1, a_class);
#endif /* _WINDOWS */
	  ustrcpy (CurrentClass, a_class);
	}
      else
	{
#ifndef _WINDOWS
	  TtaSetSelector (BaseDialog + ClassSelect, 0, NULL);
#endif /* _WINDOWS */
	  ustrcpy (CurrentClass, elHtmlName);
	}
  
      /* pop-up the dialogue box. */
#ifndef _WINDOWS
      TtaShowDialogue (BaseDialog + ClassForm, TRUE);
#else  /* _WINDOWS */
      CreateRuleDlgWindow (TtaGetViewFrame (doc, 1), NbClass, ListBuffer);
#endif /* _WINDOWS */
    }
}

/*----------------------------------------------------------------------
   ApplyClass
   Initialize and activate the Apply Class dialogue box.
  ----------------------------------------------------------------------*/
void                ApplyClass (Document doc, View view)
{
  Attribute           attr;
  AttributeType       attrType;
  Element             firstSelectedEl;
  ElementType	      elType;
  CHAR_T              a_class[50];
  int                 len;
  int                 firstSelectedChar, lastSelectedChar;
#ifndef _WINDOWS
   CHAR_T              bufMenu[MAX_TXT_LEN];
#endif /* _WINDOWS */

  TtaGiveFirstSelectedElement (doc, &firstSelectedEl,
			       &firstSelectedChar, &lastSelectedChar);
  if (!firstSelectedEl)
     return;
  CurrentClass[0] = WC_EOS;
  ApplyClassDoc = doc;

  /* updating the class name selector. */
#ifndef _WINDOWS
   ustrcpy (bufMenu, TtaGetMessage (LIB, TMSG_APPLY));
   TtaNewSheet (BaseDialog + AClassForm, TtaGetViewFrame (doc, 1), 
		TtaGetMessage (AMAYA, AM_APPLY_CLASS), 1,
		bufMenu, FALSE, 2, 'L', D_DONE);
#endif /* !_WINDOWS */
  NbClass = BuildClassList (doc, ListBuffer, MAX_CSS_LENGTH, TEXT("default"));
#ifndef _WINDOWS
  TtaNewSelector (BaseDialog + AClassSelect, BaseDialog + AClassForm,
		  TtaGetMessage (AMAYA, AM_SEL_CLASS),
		  NbClass, ListBuffer, 5, NULL, FALSE, TRUE);
#endif /* !_WINDOWS */

  /* preselect the entry corresponding to the class of the first selected
     element. */
  elType = TtaGetElementType (firstSelectedEl);
  if (!ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("MathML")))
     {
     attrType.AttrSSchema = elType.ElSSchema;
     attrType.AttrTypeNum = MathML_ATTR_class;
     }
  else
#ifdef GRAPHML
  elType = TtaGetElementType (firstSelectedEl);
  if (!ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("GraphML")))
     {
     attrType.AttrSSchema = elType.ElSSchema;
     attrType.AttrTypeNum = GraphML_ATTR_class;
     }
  else
#endif
     {
     attrType.AttrSSchema = TtaGetSSchema (TEXT("HTML"), doc);
     attrType.AttrTypeNum = HTML_ATTR_Class;
     }
  attr = TtaGetAttribute (firstSelectedEl, attrType);
  if (attr)
    {
      len = 50;
      TtaGiveTextAttributeValue (attr, a_class, &len);
#ifndef _WINDOWS
      TtaSetSelector (BaseDialog + AClassSelect, -1, a_class);
#endif /* !_WINDOWS */
      ustrcpy (CurrentClass, a_class);
    }
  else
    {
#ifndef _WINDOWS
      TtaSetSelector (BaseDialog + AClassSelect, 0, NULL);
#endif /* !_WINDOWS */
      ustrcpy (CurrentClass, TEXT("default"));
    }

   /* pop-up the dialogue box. */
#ifndef _WINDOWS
  TtaShowDialogue (BaseDialog + AClassForm, TRUE);
#else  /* _WINDOWS */
  CreateApplyClassDlgWindow (TtaGetViewFrame (doc, 1), NbClass, ListBuffer);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   StyleCallbackDialogue : procedure for style dialogue events        
  ----------------------------------------------------------------------*/
void                StyleCallbackDialogue (int ref, int typedata, CHAR_T* data)
{
  int               val;

  val = (int) data;
#ifdef DEBUG_CLASS_INTERF
  if (typedata == INTEGER_DATA)
    fprintf (stderr, "StyleCallbackDialogue(%d,%d) \n", ref, (int) data);
  else if (typedata == STRING_DATA)
    fprintf (stderr, "StyleCallbackDialogue(%d,\"%s\") \n", ref, (char*) data);
#endif

  switch (ref - BaseDialog)
    {
    case ClassForm:
      if (val == 1)
	UpdateClass (DocReference);
      TtaDestroyDialogue (BaseDialog + ClassForm);
      break;
    case ClassSelect:
      ustrcpy (CurrentClass, data);
      break;
    case AClassSelect:
      ustrcpy (CurrentClass, data);
      DoApplyClass (ApplyClassDoc);
      break;
    case AClassForm:
      if (val == 1)
	DoApplyClass (ApplyClassDoc);
      else
	TtaDestroyDialogue (BaseDialog + AClassForm);
      break;
    default:
      break;
    }
}
