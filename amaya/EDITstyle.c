/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Authors: I. Vatton
 *          R. Guetari (W3C/INRIA) Windows NT/95
 *
 */


#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "undo.h"
 
#include "css_f.h"
#include "html2thot_f.h"
#include "HTMLpresentation_f.h"
#include "HTMLstyle_f.h"
#include "UIcss_f.h"

#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */

static CHAR         ListBuffer[MAX_CSS_LENGTH + 1];
static int          NbClass = 0;
static CHAR         CurrentClass[80];
static Element      ClassReference;
static Document     DocReference;
static Document	    ApplyClassDoc;

/*----------------------------------------------------------------------
   CleanStylePresentation:  remove the existing style presentation of a
   specific element
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CleanStylePresentation (Element el, Document doc)
#else
static void         CleanStylePresentation (el, doc)
Element             el;
Document            doc;
#endif
{
   PresentationTarget  target;
   SpecificContextBlock block;
   PresentationContext ctxt;
   PresentationValue   unused;
  
   unused.data = 0;
   /* remove all the presentation specific rules applied to the element */
   ctxt = (PresentationContext) & block;
   target = (PresentationTarget) el;
   block.drv = &SpecificStrategy;
   block.doc = doc;
   block.schema = TtaGetDocumentSSchema (doc);
   if (ctxt->drv->CleanPresentation != NULL)
      ctxt->drv->CleanPresentation (target, ctxt, unused);
}


/*----------------------------------------------------------------------
  RemoveStyle : clean all the presentation rules of a given element.
  The parameter removeSpan is True when the span has to be removed.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void              RemoveStyle (Element el, Document doc, boolean removeSpan)
#else
void              RemoveStyle (el, doc, removeSpan)
Element           el;
Document          doc;
boolean           removeSpan;
#endif
{
   Attribute            attr;
   AttributeType        attrType;
   Element		firstChild, lastChild;

   if (el == NULL)
      return;

   attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
   attrType.AttrTypeNum = HTML_ATTR_Style_;
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
   CleanStylePresentation (el, doc);
}

/*----------------------------------------------------------------------
   UpdateStyleDelete : attribute Style will be deleted.            
   remove the existing style presentation.                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             UpdateStyleDelete (NotifyAttribute * event)
#else
boolean             UpdateStyleDelete (event)
NotifyAttribute    *event;
#endif
{
  CleanStylePresentation (event->element, event->document);
  return FALSE;  /* let Thot perform normal operation */
}



/*----------------------------------------------------------------------
  ChangeStyles
  the STYLE element will be changed in the document HEAD.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             ChangeStyles (NotifyElement * event)
#else  /* __STDC__ */
boolean             ChangeStyles (event)
NotifyElement      *event;

#endif /* __STDC__ */
{
  Element             el;
  Language            lang;
  int                 buflen;

  el = TtaGetFirstChild (event->element);
  if (el != NULL)
    {
      /*save buffer contents before any change */
      buflen = MAX_CSS_LENGTH;
      TtaGiveTextContent (el, ListBuffer, &buflen, &lang);
      ListBuffer[MAX_CSS_LENGTH] = EOS;
    }
   return FALSE;  /* let Thot perform normal operation */
}



/*----------------------------------------------------------------------
   StyleChanged
   A STYLE element has been changed in the HEAD
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                StyleChanged (NotifyAttribute * event)
#else
void                StyleChanged (event)
NotifyAttribute    *event;
#endif
{
  Element             el;
  Language            lang;
  STRING              buffer, ptr1, ptr2;
  STRING              pEnd, nEnd;
  CHAR                c;
  int                 buflen, i, j;
  int                 previousEnd, nextEnd;
  int                 braces;

  el = TtaGetFirstChild (event->element);
  if (el != NULL)
    {
      /*save buffer contents after any change */
      buflen = MAX_CSS_LENGTH;
      buffer = TtaGetMemory (buflen+1);
      TtaGiveTextContent (el, buffer, &buflen, &lang);
      buffer[MAX_CSS_LENGTH] = EOS;

      /* compare both srings */
      i = 0;
      ptr1 = buffer;
      previousEnd = i;
      pEnd = ptr1;
      braces = 0;
      while (ListBuffer[i] == *ptr1 && *ptr1 != EOS)
	{
	  if (i > 0 && ListBuffer[i-1] == '{')
	    braces++;
	  if (i > 0 &&
	      (ListBuffer[i-1] == '}' ||
	       ((ListBuffer[i-1] == ';' || ListBuffer[i-1] == '>') && braces == 0)))
	    {
	      if (ListBuffer[i-1] == '}')
		braces--;
	      previousEnd = i;
	      pEnd = ptr1;
	    }
	  i++;
	  ptr1++;
	}
      /* now ptr1 and ListBuffer[i] point different strings */
      if (*ptr1 != EOS)
	{
	  ptr2 = ptr1 + ustrlen (ptr1);
	  j = i + ustrlen (&ListBuffer[i]);
	  nextEnd = j;
	  nEnd = ptr2;
	  braces = 0;
	  while (ListBuffer[j] == *ptr2 && ptr2 != ptr1)
	    {
	      if (j > i && ListBuffer[j-1] == '{')
		braces++;
	      if (j > i &&
		  (ListBuffer[j-1] == '}' ||
		   ((ListBuffer[j-1] == '@' || ListBuffer[j-1] == '<') && braces == 0)))
		{
		  if (ListBuffer[j-1] == '}')
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
	      ListBuffer[nextEnd] = EOS;
	      *nEnd = EOS;

	      /* remove previous rules */
	      ptr1 = &ListBuffer[previousEnd];
	      ptr2 = ptr1;
	      do
		{
		  while (*ptr2 != '}' && *ptr2 != EOS)
		    ptr2++;
		  if (*ptr2 != EOS)
		    ptr2++;
		  /* cut here */
		  c = *ptr2;
		  *ptr2 = EOS;
		  ApplyCSSRules (event->element, ptr1, event->document, TRUE);
		  /**** update image contexts
		    url1 = GetCSSBackgroundURL (ptr1);
		    if (url1 != NUL)
		    {
		    sprintf (path, "%s%s%d%s", TempFileDirectory, DIR_STR, event->document, DIR_STR, url1);
		    pImage = SearchLoadedImage (path, event->document);
		    
		    }
		    ***/
		  *ptr2 = c;
		  ptr1 = ptr2;
		}
	      while (*ptr2 != EOS);

	      /* add new rules */
	      ptr1 = pEnd;
	      ptr2 = ptr1;
	      do
		{
		  while (*ptr2 != '}' && *ptr2 != EOS)
		    ptr2++;
		  if (*ptr2 != EOS)
		    ptr2++;
		  /* cut here */
		  c = *ptr2;
		  *ptr2 = EOS;
		  ApplyCSSRules (event->element, ptr1, event->document, FALSE);
		  *ptr2 = c;
		  ptr1 = ptr2;
		}
	      while (*ptr2 != EOS);
	    }
	}
      
      TtaFreeMemory (buffer);
    }
}


/*----------------------------------------------------------------------
   UpdateStylePost : attribute Style has been updated or created.  
   reflect the new style presentation.                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateStylePost (NotifyAttribute * event)
#else
void                UpdateStylePost (event)
NotifyAttribute    *event;
#endif
{
   Element             el, firstChild, lastChild, oldParent, newParent;
   Document            doc;
   Attribute           at;
   AttributeType       atType;
   STRING              style = NULL;
   int                 len;

   el = event->element;
   doc = event->document;

   /*
    * First remove all the presentation specific rules applied to the element.
    */
   UpdateStyleDelete (event);

   len = TtaGetTextAttributeLength (event->attribute);
   if ((len < 0) || (len > 10000))
      return;
   if (len == 0)
     {
	/*
	 * empty Style attribute. Delete it.
	 */
	atType.AttrSSchema = TtaGetSSchema ("HTML", doc);
	atType.AttrTypeNum = HTML_ATTR_Style_;

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
	/*
	 * parse and apply the new style content.
	 */
	style = (STRING) TtaGetMemory (len + 2);
	if (style == NULL)
	   return;
	TtaGiveTextAttributeValue (event->attribute, style, &len);
	style[len] = EOS;
	/* create a Span element if it's a TEXT leaf */
	oldParent = TtaGetParent (el);
	AttrToSpan (el, event->attribute, doc);
	newParent = TtaGetParent (el);
	if (newParent != oldParent)
	   /* a new SPAN element has been created. Generate the PRules
	      for the SPAN element */
	   el = newParent;
	ParseHTMLSpecificStyle (el, style, doc, FALSE);
	TtaFreeMemory (style);
     }
}

/*----------------------------------------------------------------------
   DoApplyClass
   Put a class attribute on all selected elements
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DoApplyClass (Document doc)
#else  /* __STDC__ */
static void         DoApplyClass (doc)
Document            doc;

#endif /* __STDC__ */
{
  Element             firstSelectedEl, lastSelectedEl, cour, el, span, next,
		      firstChild, lastChild, parent;
  ElementType	      elType;
  Attribute           attr;
  AttributeType       attrType;
  STRING              a_class = CurrentClass;
  int		      firstSelectedChar, lastSelectedChar, i, lg;
  DisplayMode         dispMode;
  boolean	      setClassAttr;

  if (!a_class)
    return;

  /* remove any leading dot in a class definition. */
  if (*a_class == '.')
    a_class++;
  if (*a_class == EOS)
    return;

  TtaGiveFirstSelectedElement (doc, &firstSelectedEl, &firstSelectedChar, &i);
  if (firstSelectedEl == NULL)
     return;
  TtaClearViewSelections ();
  /* stop displaying changes that will be made in the document */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (doc, DeferredDisplay);

  if (ustrcmp (CurrentClass, "default") &&
      !IsImplicitClassName (CurrentClass, doc))
     {
     setClassAttr = TRUE;
     attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
     attrType.AttrTypeNum = HTML_ATTR_Class;
     }
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
	       !ustrcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
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
     /* it's a text element */
     if (firstSelectedChar <= 1)
	/* selection starts at the beginning of the element */
	/* this text element is then entirely selected */
	{
	parent = TtaGetParent (firstSelectedEl);
	elType = TtaGetElementType (parent);
	if (elType.ElTypeNum == HTML_EL_Span &&
	    !ustrcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
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

  TtaOpenUndoSequence (doc, firstSelectedEl, lastSelectedEl, 0, 0);
  /* process all selected elements */
  cour = firstSelectedEl;
  while (cour != NULL)
     {
      /* The current element may be deleted by DeleteSpanIfNoAttr. So, get
	 first the next element to be processed */
      if (cour == lastSelectedEl)
         next = NULL;
      else
	 {
         next = cour;
         TtaGiveNextElement (doc, &next, lastSelectedEl);
	 }

      if (!setClassAttr)
	 {
	 DeleteSpanIfNoAttr (cour, doc, &firstChild, &lastChild);
	 if (firstChild)
	    {
	    if (cour == firstSelectedEl)
	        firstSelectedEl = firstChild;
	    if (cour == lastSelectedEl)
		lastSelectedEl = lastChild;
	    }
	 }
      else
	 {
	  elType = TtaGetElementType (cour);
	  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	     /* that's a text element. Create an enclosing SPAN element */
	     {
	     MakeASpan (cour, &span, doc);
	     if (span)
		/* a SPAN element was created */
		{
		if (cour == firstSelectedEl)
		   {
		   firstSelectedEl = span;
		   if (firstSelectedEl == lastSelectedEl)
		      lastSelectedEl = span;
		   }
		else if (cour == lastSelectedEl)
		   lastSelectedEl = span;
		cour = span;
		}
	     }
	  /* set the Class attribute of the element */
	  attr = TtaGetAttribute (cour, attrType);
	  if (!attr)
	     {
	      attr = TtaNewAttribute (attrType);
	      TtaAttachAttribute (cour, attr, doc);
	      TtaSetAttributeText (attr, a_class, cour, doc);
	      TtaRegisterAttributeCreate (attr, cour, doc);
	     }
	  else
	     {
	     TtaRegisterAttributeReplace (attr, cour, doc);
	     TtaSetAttributeText (attr, a_class, cour, doc);
	     }
	  TtaSetDocumentModified (doc);
	 }
      /* jump to the next element */
      cour = next;
     }
  TtaCloseUndoSequence (doc);

  /* ask Thot to display changes made in the document */
  TtaSetDisplayMode (doc, dispMode);
  TtaSelectElement (doc, firstSelectedEl);
  if (lastSelectedEl != firstSelectedEl)
     TtaExtendSelection (doc, lastSelectedEl, 0);
}

/*----------------------------------------------------------------------
   UpdateClass
   Change or create a class to reflect the Style attribute of the
   selected element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         UpdateClass (Document doc)
#else  /* __STDC__ */
static void         UpdateClass (doc)
Document            doc;

#endif /* __STDC__ */
{
  Attribute           attr;
  AttributeType       attrType;
  ElementType         elType, selType;
  CHAR                stylestring[1000];
  STRING              a_class;
  int                 len, base;

  /* check whether it's the element type or a class name */
  ustrcpy (stylestring, "\n");
  elType = TtaGetElementType (ClassReference);
  GIType (CurrentClass, &selType, doc);
  /* create a string containing the new CSS definition. */
  if (selType.ElTypeNum == 0)
    {
      if (CurrentClass[0] != '.' && CurrentClass[0] != '#')
	/* it's an invalid class name */
	ustrcat (stylestring, ".");
    }
  else if (selType.ElTypeNum != elType.ElTypeNum)
    {
      /* it's an invalid element type */
      TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_INVALID_TYPE), NULL);
      return;
    }
  ustrcat (stylestring, CurrentClass);
  ustrcat (stylestring, " { ");
  base = ustrlen (stylestring);
  len = 1000 - base - 4;
  GetHTMLStyleString (ClassReference, doc, &stylestring[base], &len);
  ustrcat (stylestring, "}");
  
  TtaOpenUndoSequence (doc, ClassReference, ClassReference, 0, 0);

  /* remove the Style attribute */
  RemoveStyle (ClassReference, doc, FALSE);

  /* create the class attribute */
  attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
  if (stylestring[1] == '.')
    {
      a_class = &CurrentClass[0];
      if (*a_class == '.')
	a_class++;
      attrType.AttrTypeNum = HTML_ATTR_Class;
      attr = TtaGetAttribute (ClassReference, attrType);
      if (!attr)
	{
	  attr = TtaNewAttribute (attrType);
	  TtaAttachAttribute (ClassReference, attr, doc);
	}
      TtaSetAttributeText (attr, a_class, ClassReference, doc);
      TtaSetDocumentModified (doc);
    }
  /* parse and apply this new CSS to the current document */
  /*ApplyCSSRules (NULL, stylestring, doc, FALSE);*/
  ReadCSSRules (doc, doc, NULL, stylestring, TRUE);

  TtaCloseUndoSequence (doc);
}

/*----------------------------------------------------------------------
   BuildClassList : Build the whole list of HTML class names in use
   after the first name.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          BuildClassList (Document doc, STRING buf, int size, STRING first)
#else  /* __STDC__ */
static int          BuildClassList (doc, buf, size, first)
Document            doc;
STRING              buf;
int                 size;
STRING              first;

#endif /* __STDC__ */
{
  ElementType         elType;
  Element             el;
  Attribute           attr;
  AttributeType       attrType;
  CHAR                selector[100];
  int                 Free;
  int                 len;
  int                 nb, i;
  int                 index, cur;
  boolean             found;

  /* add the first element if specified */
  buf[0] = EOS;
  nb = 0;
  index = 0;
  Free = size;
  if (first)
    {
      ustrcpy (&buf[index], first);
      len = ustrlen (first);
      len++;
      Free -= len;
      index += len;
      nb++;
    }

  /* list all class values */
  el = TtaGetMainRoot (doc);
  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = HTML_ATTR_Class;
  while (el != NULL)
    {
      TtaSearchAttribute (attrType, SearchForward, el, &el, &attr);
      if (attr != NULL)
	{
	  len = 99;
	  TtaGiveTextAttributeValue (attr, selector, &len);
	  /* compare with all already known class names */
	  cur = 0;
	  found = FALSE;
	  for (i = 0; i < nb && !found; i++)
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
	      if (len > Free)
		return (nb);
	      /* add this new class name + the dot */
	      buf[index++] = '.';
	      ustrcpy (&buf[index], selector);
              len++; /* add the \0 */
              Free -= len;
              index += len;
              nb++;
	    }
	}
    }
  return (nb);
}

/*----------------------------------------------------------------------
   CreateClass
   creates a class from the Style attribute of the selected element
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateClass (Document doc, View view)
#else  /* __STDC__ */
void                CreateClass (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
  Attribute           attr;
  AttributeType       attrType;
  Element             last_elem;
  ElementType         elType;
  CHAR                a_class[50];
  STRING              elHtmlName;
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
  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
    ClassReference = TtaGetParent (ClassReference);
  /* update the class name selector. */
  elHtmlName = GetCSSName (ClassReference, doc);
#  ifndef _WINDOWS
  TtaNewForm (BaseDialog + ClassForm, TtaGetViewFrame (doc, 1), 
	      TtaGetMessage (AMAYA, AM_DEF_CLASS), FALSE, 2, 'L', D_DONE);
#  endif /* !_WINDOWS */
  NbClass = BuildClassList (doc, ListBuffer, MAX_CSS_LENGTH, elHtmlName);
#  ifndef _WINDOWS
  TtaNewSelector (BaseDialog + ClassSelect, BaseDialog + ClassForm,
		  TtaGetMessage (AMAYA, AM_SEL_CLASS),
		  NbClass, ListBuffer, 5, NULL, TRUE, FALSE);
#  endif /* !_WINDOWS */
  
  /* preselect the entry corresponding to the class of the element. */
  attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
  attrType.AttrTypeNum = HTML_ATTR_Class;
  
  attr = TtaGetAttribute (ClassReference, attrType);
  if (attr)
    {
      len = 50;
      TtaGiveTextAttributeValue (attr, a_class, &len);
#   ifndef _WINDOWS
      TtaSetSelector (BaseDialog + ClassSelect, -1, a_class);
#   endif /* _WINDOWS */
      ustrcpy (CurrentClass, a_class);
    }
  else
    {
#   ifndef _WINDOWS
      TtaSetSelector (BaseDialog + ClassSelect, 0, NULL);
#   endif /* _WINDOWS */
      ustrcpy (CurrentClass, elHtmlName);
    }
  
  /* pop-up the dialogue box. */
#  ifndef _WINDOWS
  TtaShowDialogue (BaseDialog + ClassForm, TRUE);
#  else  /* _WINDOWS */
  CreateCreateRuleDlgWindow (TtaGetViewFrame (doc, 1), BaseDialog, ClassForm, ClassSelect, NbClass, ListBuffer);
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   ApplyClass
   Initialize and activate the Apply Class dialogue box.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ApplyClass (Document doc, View view)
#else  /* __STDC__ */
void                ApplyClass (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
  Attribute           attr;
  AttributeType       attrType;
  Element             firstSelectedEl;
  CHAR                a_class[50];
  int                 len;
  int                 firstSelectedChar, lastSelectedChar;

  TtaGiveFirstSelectedElement (doc, &firstSelectedEl,
			       &firstSelectedChar, &lastSelectedChar);
  if (!firstSelectedEl)
     return;
  CurrentClass[0] = EOS;
  ApplyClassDoc = doc;

  /* updating the class name selector. */
#  ifndef _WINDOWS
  TtaNewForm (BaseDialog + AClassForm, TtaGetViewFrame (doc, 1), 
	      TtaGetMessage (AMAYA, AM_APPLY_CLASS), TRUE, 2, 'L', D_DONE);
#  endif /* !_WINDOWS */
  NbClass = BuildClassList (doc, ListBuffer, MAX_CSS_LENGTH, "default");
#  ifndef _WINDOWS
  TtaNewSelector (BaseDialog + AClassSelect, BaseDialog + AClassForm,
		  TtaGetMessage (AMAYA, AM_SEL_CLASS),
		  NbClass, ListBuffer, 5, NULL, FALSE, FALSE);
#  endif /* !_WINDOWS */

  /* preselect the entry corresponding to the class of the first selected
     element. */
  attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
  attrType.AttrTypeNum = HTML_ATTR_Class;
  attr = TtaGetAttribute (firstSelectedEl, attrType);
  if (attr)
    {
      len = 50;
      TtaGiveTextAttributeValue (attr, a_class, &len);
      TtaSetSelector (BaseDialog + AClassSelect, -1, a_class);
      ustrcpy (CurrentClass, a_class);
    }
  else
    {
      TtaSetSelector (BaseDialog + AClassSelect, 0, NULL);
      ustrcpy (CurrentClass, "default");
    }

   /* pop-up the dialogue box. */
#  ifndef _WINDOWS
  TtaShowDialogue (BaseDialog + AClassForm, TRUE);
#  else  /* _WINDOWS */
  CreateApplyClassDlgWindow (TtaGetViewFrame (doc, 1), BaseDialog, AClassForm, AClassSelect, NbClass, ListBuffer);
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   StyleCallbackDialogue : procedure for style dialogue events        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                StyleCallbackDialogue (int ref, int typedata, STRING data)
#else  /* __STDC__ */
void                StyleCallbackDialogue (ref, typedata, data)
int                 ref;
int                 typedata;
STRING              data;

#endif /* __STDC__ */
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
      if (typedata == INTEGER_DATA && val == 1)
	UpdateClass (DocReference);
      TtaDestroyDialogue (BaseDialog + ClassForm);
      break;
    case ClassSelect:
    case AClassSelect:
      if (typedata == STRING_DATA)
	ustrcpy (CurrentClass, data);
      break;
    case AClassForm:
      if (typedata == INTEGER_DATA && val == 1)
	DoApplyClass (ApplyClassDoc);
      TtaDestroyDialogue (BaseDialog + AClassForm);
      break;
    default:
      break;
    }
}
