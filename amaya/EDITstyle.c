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
 
#include "css_f.h"
#include "html2thot_f.h"
#include "HTMLpresentation_f.h"
#include "HTMLstyle_f.h"
#include "UIcss_f.h"

#ifdef _WINDOWS
#include "windialogapi_f.h"
#endif /* _WINDOWS */

static char         ListBuffer[MAX_CSS_LENGTH + 1];
static int          NbClass = 0;
static char         CurrentClass[80];
static Element      ClassReference;
static Document     DocReference;
static Element      AClassFirstReference;
static Element      AClassLastReference;
static Document     ADocReference;


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
  RemoveStyle : clean all the presentation attributes of a given element.
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

   if (el == NULL)
      return;

   /*
    * remove any Class or ImplicitClass associated to the element.
    */
   attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
   attrType.AttrTypeNum = HTML_ATTR_Class;
   attr = TtaGetAttribute (el, attrType);
   if (attr != NULL)
     {
	TtaRemoveAttribute (el, attr, doc);
	if (removeSpan)
	  DeleteSpanIfNoAttr (el, doc);
	TtaSetDocumentModified (doc);
     }

   attrType.AttrTypeNum = HTML_ATTR_Style_;
   attr = TtaGetAttribute (el, attrType);
   if (attr != NULL)
     {
	TtaRemoveAttribute (el, attr, doc);
	if (removeSpan)
	  DeleteSpanIfNoAttr (el, doc);
	TtaSetDocumentModified (doc);
     }

   /* remove all the presentation specific rules applied to the element */
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
  char               *buffer, *ptr1, *ptr2;
  char               *pEnd, *nEnd, c;
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
	  ptr2 = ptr1 + strlen (ptr1);
	  j = i + strlen (&ListBuffer[i]);
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
   Element             el;
   Document            doc;
   Attribute           at;
   AttributeType       atType;
   char               *style = NULL;
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
	 * suppress the Style Attribute.
	 */
	atType.AttrSSchema = TtaGetSSchema ("HTML", doc);
	atType.AttrTypeNum = HTML_ATTR_Style_;

	at = TtaGetAttribute (el, atType);
	if (at != NULL)
	  {
	     TtaRemoveAttribute (el, at, doc);
	     DeleteSpanIfNoAttr (el, doc);
	     TtaSetDocumentModified (doc);
	  }
     }
   else
     {
	/*
	 * parse and apply the new style content.
	 */
	style = (char*) TtaGetMemory (len + 2);
	if (style == NULL)
	   return;
	TtaGiveTextAttributeValue (event->attribute, style, &len);
	style[len] = EOS;

	ParseHTMLSpecificStyle (el, style, doc, FALSE);
	TtaFreeMemory (style);
     }
}

/*----------------------------------------------------------------------
   ApplyClassChange : Change all the presentation attributes of    
   the selected elements to reflect their new class                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ApplyClassChange (Document doc)
#else  /* __STDC__ */
static void         ApplyClassChange (doc)
Document            doc;

#endif /* __STDC__ */
{
  Element             cour;
  Attribute           attr;
  AttributeType       attrType;
  char               *a_class = CurrentClass;

  if (!a_class)
    return;

  /* remove any leading dot in a class definition. */
  if (*a_class == '.')
    a_class++;
  if (*a_class == EOS)
    return;

  /* class default : suppress all specific presentation. */
  if (!strcmp (CurrentClass, "default"))
    {
      cour = AClassFirstReference;
      while (cour != NULL)
	{
	  /* remove any style attribute and update the presentation. */
	  RemoveStyle (cour, doc, FALSE);
	  
	  /* jump on next element until last one is reached. */
	  if (cour == AClassLastReference)
	    break;
	  TtaNextSibling (&cour);
	}
      return;
    }
  else
    {
      if (AClassFirstReference == AClassLastReference)
	{
	  /* only one element -> create a SPAN if needed */
	  if (MakeASpan (AClassFirstReference, &cour, doc))
	    {
	      /* re-select the element */
	      TtaSelectElement (doc, cour);
	      AClassLastReference = cour;
	    }
	  else
	    cour = AClassFirstReference;
	}
      else
	cour = AClassFirstReference;

      /* loop on each selected element */
      while (cour != NULL)
	{
	  /* remove any Style attribute left */
	  RemoveStyle (cour, doc, FALSE);
	  
	  /* set the Class attribute of the element */
	  attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
	  if (!IsImplicitClassName (CurrentClass, doc))
	    {
	      attrType.AttrTypeNum = HTML_ATTR_Class;
	      attr = TtaGetAttribute (cour, attrType);
	      if (!attr)
		{
		  attr = TtaNewAttribute (attrType);
		  TtaAttachAttribute (cour, attr, doc);
		}
	      TtaSetAttributeText (attr, a_class, cour, doc);
	      TtaSetDocumentModified (doc);
	    }
	  /* jump on next element until last one is reached. */
	  if (cour == AClassLastReference)
	    break;
	  TtaNextSibling (&cour);
	}
    }
}

/*----------------------------------------------------------------------
   UpdateClass : Change a class to reflect the presentation        
   attributes of the selected elements                             
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
  char                stylestring[1000];
  char               *a_class;
  int                 len, base;

  /* check whether it's the element type or a godd class name */
  strcpy (stylestring, "\n");
  elType = TtaGetElementType (ClassReference);
  GIType (CurrentClass, &selType, doc);
  /* create a string containing the new CSS definition. */
  if (selType.ElTypeNum == 0)
    {
      if (CurrentClass[0] != '.' && CurrentClass[0] != '#')
	/* it's an invalid class name */
	strcat (stylestring, ".");
    }
  else if (selType.ElTypeNum != elType.ElTypeNum)
    {
      /* it's an invalid element type */
      TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_INVALID_TYPE), NULL);
      return;
    }
  strcat (stylestring, CurrentClass);
  strcat (stylestring, " { ");
  base = strlen (stylestring);
  len = 1000 - base - 4;
  GetHTMLStyleString (ClassReference, doc, &stylestring[base], &len);
  strcat (stylestring, "}");
  
  /* change the selected element to be of the new class. */
  RemoveStyle (ClassReference, doc, FALSE);

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
  ReadCSSRules (doc, doc, NULL, stylestring);
}

/*----------------------------------------------------------------------
   BuildClassList : Build the whole list of HTML class names in use
   after the first name.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          BuildClassList (Document doc, char *buf, int size, char *first)
#else  /* __STDC__ */
static int          BuildClassList (doc, buf, size, first)
Document            doc;
char               *buf;
int                 size;
char               *first;

#endif /* __STDC__ */
{
  ElementType         elType;
  Element             el;
  Attribute           attr;
  AttributeType       attrType;
  char                selector[100];
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
      strcpy (&buf[index], first);
      len = strlen (first);
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
	      len = strlen (&buf[cur]) + 1;
	      found = !strcmp (selector, &buf[cur]);
	      cur += len;
	    }

	  if (!found)
	    {
	      len = strlen (selector);
	      if (len > Free)
		return (nb);
	      /* add this new class name + the dot */
	      buf[index++] = '.';
	      strcpy (&buf[index], selector);
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
   CreateClass creates a class to reflect the presentation    
   attributes of the selected element
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
  char                a_class[50];
  char               *elHtmlName;
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
  
  /* if a subset of an element is selected, select the parent instead */
  elType = TtaGetElementType (ClassReference);
  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
    ClassReference = TtaGetParent (ClassReference);
  /* updating the class name selector. */
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
      strcpy (CurrentClass, a_class);
    }
  else
    {
#   ifndef _WINDOWS
      TtaSetSelector (BaseDialog + ClassSelect, 0, NULL);
#   endif /* _WINDOWS */
      strcpy (CurrentClass, elHtmlName);
    }
  
  /* pop-up the dialogue box. */
#  ifndef _WINDOWS
  TtaShowDialogue (BaseDialog + ClassForm, TRUE);
#  else  /* _WINDOWS */
  CreateCreateRuleDlgWindow (TtaGetViewFrame (doc, 1), BaseDialog, ClassForm, ClassSelect, NbClass, ListBuffer);
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   ApplyClass : Use a class to change the presentation             
   attributes of the selected elements                             
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
  Element             cour, parent;
  ElementType         elType;
  char                a_class[50];
  int                 len, i, j;
  int                 firstSelectedChar, lastSelectedChar;
  boolean             select_parent;

  ADocReference = doc;
  CurrentClass[0] = EOS;
  AClassFirstReference = NULL;
  AClassLastReference = NULL;

  /* a class can be applied to many elements. */
  TtaGiveFirstSelectedElement (doc, &AClassFirstReference,
			       &firstSelectedChar, &lastSelectedChar);

  if (AClassFirstReference == NULL)
    return;
  cour = AClassLastReference = AClassFirstReference;
  do
    {
      TtaGiveNextSelectedElement (doc, &cour, &i, &j);
      if (cour != NULL)
	AClassLastReference = cour;
    }
  while (cour != NULL);
  
  /* Case of a substring : need to split the original text. */
  if (AClassFirstReference == AClassLastReference &&
      firstSelectedChar != 0)
    {
      len = TtaGetTextLength (AClassFirstReference);
      if (len <= 0)
	return;
      if (lastSelectedChar < len)
	TtaSplitText (AClassFirstReference, lastSelectedChar, doc);
      if (firstSelectedChar > 1)
	{
	  firstSelectedChar--;
	  TtaSplitText (AClassFirstReference, firstSelectedChar, doc);
	  TtaNextSibling (&AClassFirstReference);
	  AClassLastReference = AClassFirstReference;
	}
      if (AClassFirstReference != NULL)
	/* re-select the element */
	TtaSelectElement (doc, AClassFirstReference);
    }

  if (AClassFirstReference == NULL)
    return;

  /* if all child of an element are selected, select the parent instead */
  parent = TtaGetParent (AClassFirstReference);
  if ((parent == TtaGetParent (AClassLastReference)) &&
      (AClassFirstReference == TtaGetFirstChild(parent)) &&
      (AClassLastReference == TtaGetLastChild(parent)))
    { 
      select_parent = TRUE;
      elType = TtaGetElementType(AClassFirstReference);
      if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	{
	  if (firstSelectedChar > 1)
	    select_parent = FALSE;
	}
      
      elType = TtaGetElementType (AClassLastReference);
      if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	{
	  len = TtaGetTextLength (AClassLastReference);
	  if (lastSelectedChar < len)
	    select_parent = FALSE;
	}
      
      if (select_parent)
	{
	  AClassFirstReference = AClassLastReference = parent;
	  firstSelectedChar = lastSelectedChar = 0;
	}
    }

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

  /* preselect the entry corresponding to the class of the element. */
  attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
  attrType.AttrTypeNum = HTML_ATTR_Class;
  attr = TtaGetAttribute (AClassFirstReference, attrType);
  if (attr)
    {
      len = 50;
      TtaGiveTextAttributeValue (attr, a_class, &len);
      TtaSetSelector (BaseDialog + AClassSelect, -1, a_class);
      strcpy (CurrentClass, a_class);
    }
  else
    {
      TtaSetSelector (BaseDialog + AClassSelect, 0, NULL);
      strcpy (CurrentClass, "default");
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
void                StyleCallbackDialogue (int ref, int typedata, char *data)
#else  /* __STDC__ */
void                StyleCallbackDialogue (ref, typedata, data)
int                 ref;
int                 typedata;
char               *data;

#endif /* __STDC__ */
{
  int               val;

  val = (int) data;
#ifdef DEBUG_CLASS_INTERF
  if (typedata == INTEGER_DATA)
    fprintf (stderr, "StyleCallbackDialogue(%d,%d) \n", ref, (int) data);
  else if (typedata == STRING_DATA)
    fprintf (stderr, "StyleCallbackDialogue(%d,\"%s\") \n", ref, (char *) data);
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
	strcpy (CurrentClass, data);
      break;
    case AClassForm:
      if (typedata == INTEGER_DATA && val == 1)
	ApplyClassChange (ADocReference);
      TtaDestroyDialogue (BaseDialog + AClassForm);
      break;
    default:
      break;
    }
}
