/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Authors: D. Veillard and I. Vatton
 *          R. Guetari (W3C/INRIA) Windows NT/95
 *
 */


#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
 
/*----------------------------------------------------------------------
   
   CLASS MODIFICATION : USER INTERFACE AND INNER FUNCTIONS           
   
  ----------------------------------------------------------------------*/

#include "css_f.h"
#include "html2thot_f.h"
#include "HTMLpresentation_f.h"
#include "HTMLstyle_f.h"
#include "UIcss_f.h"

/* CSSLEVEL2 adding new features to the standard */
/* DEBUG_STYLES verbose output of style actions */
/* DEBUG_CLASS_INTERF verbose output on class interface actions */

/*
 * specific data :
 *   The Class List contains the list of classe names needed for selection
 *                  by the user.S
 *   NbClass is the corresponding number of elements found.
 *   CurrentClass contains the Class name selected by the user.
 *   ClassReference is the selected element used to update the class properties.
 *   DocReference is the selected document.
 */

static char         ClassList[50 * 80];
static int          NbClass = 0;
static char         CurrentClass[80];
static Element      ClassReference;
static Document     DocReference;
static char         AClassList[50 * 80];
static int          NbAClass = 0;
static char         CurrentAClass[80];
static Element      AClassFirstReference;
static Element      AClassLastReference;
static Document     ADocReference;

/*----------------------------------------------------------------------
  CSSWarning displays a CSS confirm message
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CSSWarning (char *message)
#else
static void         CSSWarning (message)
char               *message;
#endif
{
  TtaDisplayMessage (CONFIRM, TtaGetMessage (AMAYA, AM_CSS_ERROR), message);
}


/*----------------------------------------------------------------------
  GetClassSelector : return the pointer to the class value if the selector
  contains a class or NULL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char *GetClassSelector (char *selector)
#else
static char *GetClassSelector (selector)
char        *selector;
#endif
{
  char *ptr;

  ptr = NULL;
  if (selector != NULL)
    ptr = strchr (selector, '.');
  return (ptr);
}

/*----------------------------------------------------------------------
  CleanGenericPresentation
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CleanGenericPresentation (char *selector, Document doc)
#else
static void         CleanGenericPresentation (selector, doc)
char               *selector;
Document            doc;
#endif
{
  SetHTMLStyleParserDestructiveMode (TRUE);
  ParseCSSGenericStyle (selector, "color : white", doc, GetDocumentStyle (doc));
  SetHTMLStyleParserDestructiveMode (FALSE);
}

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
#ifdef DEBUG_STYLES
   char                *elHtmlName;
#endif

   if (el == NULL)
      return;

#ifdef DEBUG_STYLES
   elHtmlName = GetCSSName (el, doc);
   fprintf (stderr, "RemoveStyle(%s,%d)\n", elHtmlName, doc);
#endif

   /*
    * remove any Class or ImplicitClass associated to the element.
    */
   attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
   attrType.AttrTypeNum = HTML_ATTR_Class;

   attr = TtaGetAttribute (el, attrType);
   if (attr != NULL)
     {
	TtaRemoveAttribute (el, attr, doc);
	if (removeSpan)
	  DeleteSpanIfNoAttr (el, doc);
     }
   attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
   attrType.AttrTypeNum = HTML_ATTR_Style_;

   attr = TtaGetAttribute (el, attrType);
   if (attr != NULL)
     {
	TtaRemoveAttribute (el, attr, doc);
	if (removeSpan)
	  DeleteSpanIfNoAttr (el, doc);
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
  DeleteStyleRule
  A STYLE element will be deleted in the document HEAD.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             DeleteStyleRule (NotifyElement * event)
#else  /* __STDC__ */
boolean             DeleteStyleRule (event)
NotifyElement      *event;

#endif /* __STDC__ */
{
  Attribute           attr;
  AttributeType       attrType;
  Element             elClass;
  Element             el;
  ElementType         elType, selType;
  Document            doc;
  int                 len;
  char                selector[100], value[100];
  char               *class;

  elClass = event->element;
  doc = event->document;
  selType.ElTypeNum = 0;
  /* get the selector */
  attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
  attrType.AttrTypeNum = HTML_ATTR_Selector;
  attr = TtaGetAttribute (elClass, attrType);
  if (attr)
    {
      len = 100;
      TtaGiveTextAttributeValue (attr, selector, &len);
#ifdef DEBUG_STYLES
      fprintf (stderr, "DeleteStyleRule(%d,%s)\n", doc, selector);
#endif
      /* get the name of the class */
      class = GetClassSelector (selector);
      if (class == NULL)
	{
	  /* remove a generic rule */
	  /* TtaGiveTextContent (elClass, cssRule, &len, &lang); */
	  /* use a pseudo cssRule */
	  CleanGenericPresentation (selector, doc);
	}
      else
	{
	  if (class != selector && class[-1] != ' ')
	    {
	      /* remove a specific rule */
	      class[0] = EOS;
	      GIType (selector, &selType, doc);
	    }
	  else
	    /* generic class */
	    CleanGenericPresentation (selector, doc);

	  /* search this class value in the whole document */
	  el = TtaGetMainRoot (doc);
	  attrType.AttrTypeNum = HTML_ATTR_Class;
	  while (el != NULL)
	    {
	      TtaSearchAttribute (attrType, SearchForward, el, &el, &attr);
	      if (attr != NULL)
		{
		  len = 100;
		  TtaGiveTextAttributeValue (attr, value, &len);
		  if (!strcmp (value, &class[1]))
		    if (selType.ElTypeNum == 0)
		      {
			TtaRemoveAttribute (el, attr, doc);
			DeleteSpanIfNoAttr (el, doc);
		      }
		    else
		      {
			elType = TtaGetElementType (el);
			if (selType.ElTypeNum == elType.ElTypeNum)
			  CleanStylePresentation (el, doc);
		      }
		}
	    }
	}
      TtaSetDocumentModified (doc);
    }
#ifdef AMAYA_DEBUG
   else
     fprintf (stderr, "DeleteStyleRule(%d), invalid element\n", doc);
#endif
   return FALSE;  /* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
  DeleteStyleRule
  A STYLE element will be deleted in the document HEAD.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetStyleRule (NotifyElement * event)
#else  /* __STDC__ */
void                SetStyleRule (event)
NotifyElement      *event;

#endif /* __STDC__ */
{
  Attribute           attr;
  AttributeType       attrType;
  Element             elClass;
  Document            doc;
  Language            lang;
  int                 len, base;
  char               *stylestring;

  stylestring = (char*) TtaGetMemory (1000 * sizeof (char));
  elClass = event->element;
  doc = event->document;
  /* get the selector */
  attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
  attrType.AttrTypeNum = HTML_ATTR_Selector;
  attr = TtaGetAttribute (elClass, attrType);
  if (attr)
    {
      len = 100;
      TtaGiveTextAttributeValue (attr, stylestring, &len);
      strcat (stylestring, " { ");
      base = len + 1;
      len = (1000 * sizeof (char)) - base - 4;
      /* remove a generic rule */
      TtaGiveTextContent (elClass, &stylestring[base], &len, &lang);
      strcat (stylestring, "}");
      
      /* parse and apply this new CSS to the current document */
      ParseHTMLStyleHeader (NULL, stylestring, doc, TRUE);
    }
  TtaFreeMemory (stylestring);
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
	atType.AttrSSchema = TtaGetDocumentSSchema (doc);
	atType.AttrTypeNum = HTML_ATTR_Style_;

	at = TtaGetAttribute (el, atType);
	if (at != NULL)
	  {
	     TtaRemoveAttribute (el, at, doc);
	     DeleteSpanIfNoAttr (el, doc);
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

	ParseHTMLSpecificStyle (el, style, doc);
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
  char               *a_class = CurrentAClass;

#ifdef DEBUG_STYLES
  fprintf (stderr, "ApplyClassChange(%d,%s)\n", doc, CurrentAClass);
#endif

  if (!a_class)
    return;

  /* remove any leading dot in a class definition. */
  if (*a_class == '.')
    a_class++;
  if (*a_class == EOS)
    return;

  /* class default : suppress all specific presentation. */
  if (!strcmp (CurrentAClass, "default"))
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
	    AClassLastReference = cour;
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
	  attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
	  if (!IsImplicitClassName (CurrentAClass, doc))
	    {
	      attrType.AttrTypeNum = HTML_ATTR_Class;
	      attr = TtaGetAttribute (cour, attrType);
	      if (!attr)
		{
		  attr = TtaNewAttribute (attrType);
		  TtaAttachAttribute (cour, attr, doc);
		}
	      TtaSetAttributeText (attr, a_class, cour, doc);
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
  stylestring[0] = EOS;
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
      CSSWarning (TtaGetMessage (AMAYA, AM_INVALID_TYPE));
      return;
    }
  strcat (stylestring, CurrentClass);
  strcat (stylestring, " { ");
  base = strlen (stylestring);
  len = (1000 * sizeof (char)) - base - 4;
  GetHTMLStyleString (ClassReference, doc, &stylestring[base], &len);
  strcat (stylestring, "}");
  
  /* change the selected element to be of the new class. */
  RemoveStyle (ClassReference, doc, FALSE);

  attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
  if (stylestring[0] == '.')
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
    }
  /* parse and apply this new CSS to the current document */
    ParseHTMLStyleHeader (NULL, stylestring, doc, TRUE);
}

/*----------------------------------------------------------------------
   BuildClassList : Build the whole list of HTML class names in use  
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
  int                 Free = size;
  int                 len;
  int                 nb = 0;
  int                 index = 0;
  int                 selectoren;

  /* add the first element if specified */
  buf[0] = 0;
  if (first)
    {
      strcpy (&buf[index], first);
      len = strlen (first);
      len++;
      Free -= len;
      index += len;
      nb++;
    }
  elType.ElSSchema = TtaGetDocumentSSchema (doc);
  attrType.AttrSSchema = elType.ElSSchema;
  elType.ElTypeNum = HTML_EL_StyleRule;
  el = TtaSearchTypedElement (elType, SearchInTree, TtaGetMainRoot (doc));

  while (el != NULL)
    {
      attrType.AttrTypeNum = HTML_ATTR_Selector;
      attr = TtaGetAttribute (el, attrType);
      if (attr)
	{
	  selectoren = 100;
	  TtaGiveTextAttributeValue (attr, selector, &selectoren);
	  /*
	   * if the selector uses # this is an ID so don't show it
	   * in the list. if there is a blank in the name, it's probably
	   * not a class name.
	   */
	  if (selector[0] == '.' && (strcmp (selector, first)))
	    {
	      strcpy (&buf[index], selector);
	      len = strlen (selector);
	      len++;
	      Free -= len;
	      index += len;
	      nb++;
	    }
	}
      /* get next StyleRule */
      TtaNextSibling (&el);
    }
#ifdef DEBUG_CLASS_INTERF
  fprintf (stderr, "BuildClassList : found %d class\n", nb);
#endif
  return (nb);
}

/*----------------------------------------------------------------------
   ChangeClass : Change a class to reflect the presentation        
   attributes of the selected element                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ChangeClass (Document doc, View view)
#else  /* __STDC__ */
void                ChangeClass (doc, view)
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
    {
#ifdef DEBUG_CLASS_INTERF
      fprintf (stderr, "first selected != last selected, first char %d, last %d\n",
	       firstSelectedChar, lastSelectedChar);
#endif
      return;
    }
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
  NbClass = BuildClassList (doc, ClassList, sizeof (ClassList), elHtmlName);
#  ifndef _WINDOWS
  TtaNewSelector (BaseDialog + ClassSelect, BaseDialog + ClassForm,
		  TtaGetMessage (AMAYA, AM_SEL_CLASS),
		  NbClass, ClassList, 5, NULL, TRUE, FALSE);
#  endif /* !_WINDOWS */
  
  /* preselect the entry corresponding to the class of the element. */
  attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
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
  CreateCreateRuleDlgWindow (TtaGetViewFrame (doc, 1), BaseDialog, ClassForm, ClassSelect, NbClass, ClassList);
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
  CurrentAClass[0] = 0;
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
  if ((AClassFirstReference == AClassLastReference) &&
      (firstSelectedChar != 0))
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
  NbAClass = BuildClassList (doc, AClassList, sizeof (AClassList), "default");
#  ifndef _WINDOWS
  TtaNewSelector (BaseDialog + AClassSelect, BaseDialog + AClassForm,
		  TtaGetMessage (AMAYA, AM_SEL_CLASS),
		  NbAClass, AClassList, 5, NULL, FALSE, FALSE);
#  endif /* !_WINDOWS */

  /* preselect the entry corresponding to the class of the element. */
  attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
  attrType.AttrTypeNum = HTML_ATTR_Class;
  attr = TtaGetAttribute (AClassFirstReference, attrType);
  if (attr)
    {
      len = 50;
      TtaGiveTextAttributeValue (attr, a_class, &len);
      TtaSetSelector (BaseDialog + AClassSelect, -1, a_class);
      strcpy (CurrentAClass, a_class);
    }
  else
    {
      TtaSetSelector (BaseDialog + AClassSelect, 0, NULL);
      strcpy (CurrentAClass, "default");
    }

   /* pop-up the dialogue box. */
#  ifndef _WINDOWS
  TtaShowDialogue (BaseDialog + AClassForm, TRUE);
#  else  /* _WINDOWS */
  CreateApplyClassDlgWindow (TtaGetViewFrame (doc, 1), BaseDialog, AClassForm, AClassSelect, NbAClass, AClassList);
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
      if (typedata == STRING_DATA)
	strcpy (CurrentClass, data);
      break;
    case AClassForm:
      if (typedata == INTEGER_DATA && val == 1)
	ApplyClassChange (ADocReference);
      TtaDestroyDialogue (BaseDialog + AClassForm);
      break;
    case AClassSelect:
      if (typedata == STRING_DATA)
	strcpy (CurrentAClass, data);
      break;
    default:
      break;
    }
}
