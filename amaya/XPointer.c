/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 2000
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * XPointer.c : contains all the functions for transforming a
 * node thotlib path into an XPointer and viceversa.
 * Author: J. Kahan
 *
 * Status:
 *
 *   Experimental, only used with annotations for the moment.
 *   Not all of XPath expressions are supported yet.
 *
 */

#undef DEBUG_XPOINTER

#define THOT_EXPORT extern
#include "amaya.h"
#include "XPointer.h"
#include "XPointerparse_f.h"
#undef THOT_EXPORT
#include "XPointer_f.h"

/*#include "parser.h" */

typedef struct _XPathItem XPathItem;

struct _XPathItem {
  ElementType elType;
  int index;
  char *id_value;
  XPathItem * next;
};

typedef XPathItem * XPathList;

/* the thotlib element type used to identify a text node */
#define THOT_TEXT_UNIT  1
/* the thotlib attribute type used to identify an id attribute*/
#define THOT_ATTR_ID    2  

/*----------------------------------------------------------------------
  StrACat

  A not very efficient function that makes a dynamic memory allocation
  strcat
  ----------------------------------------------------------------------*/
static void StrACat (char ** dest, const char * src)
{
 void *status;

  if (src && *src) 
    {
    if (*dest) 
      {
	int length = strlen (*dest);
	status = TtaRealloc (*dest, length + strlen(src) + 1);
        if (status != NULL)
	  {
            *dest = status;
	    strcpy (*dest + length, src);
          }
      } 
    else 
      {
	if ((*dest  = (char  *) TtaGetMemory (strlen(src) + 1)) == NULL)
	  /* @@ what to do? */
	  exit (0);
	strcpy (*dest, src);
      }
    }
}

/*----------------------------------------------------------------------
  AdjustChIndex

  Makes sure that the index to a text element doesn't point outside
  of the length of the text.
  Returns the adjusted index, if any.
  ----------------------------------------------------------------------*/
static int AdjustChIndex (Element el, int index)
{
  int len;

  len = TtaGetTextLength (el);
  if (index > len)
    return (len);
  else
    return (index);
}

/*----------------------------------------------------------------------
  CountInLineChars

  returns the number of characters that may be found in inline
  sibling elements from element Mark, such as in <p>an 
  <strong>inline</strong> example</p>.
  ----------------------------------------------------------------------*/
static int CountInlineChars (Element mark)
{
  ElementType elType;
  Element el;
  int count = 0;

  elType = TtaGetElementType (mark);

  if (elType.ElTypeNum != THOT_TEXT_UNIT)
    return 0;

  el = TtaGetParent (mark);
  while (1)
    {
      el = TtaSearchTypedElement (elType, SearchForward, el);
      if (el == mark)
	break;
      count += TtaGetTextLength (el);
    }

  return count;
}

/*----------------------------------------------------------------------
  ElIsHidden

  returns TRUE if the element is hidden (an internal thotlib one),
  false, otherwise.
  ----------------------------------------------------------------------*/
static ThotBool ElIsHidden (Element el)
{
  ElementType elType;

  elType = TtaGetElementType (el);
  return (TtaHasHiddenException (elType));
}

/*----------------------------------------------------------------------
  GetIdValue

  If the element has an id attribute, the function returns the value of
  this attribute. It's up to the caller to free the string that's returned.
  Returns NULL otherwise
  ----------------------------------------------------------------------*/
static char * GetIdValue (Element el)
{
  Attribute attr;
  AttributeType attrType;
  ElementType elType;
  char *value;
  int len;

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = THOT_ATTR_ID;

  attr = TtaGetAttribute (el, attrType);
  if (attr != NULL)
    {
      /* there's an ID attribute */
      len = TtaGetTextAttributeLength (attr) + 1;
      value = TtaAllocString (len);
      TtaGiveTextAttributeValue (attr, value, &len);
      
    }
  else
    value  = NULL;

  return value;
}

/*----------------------------------------------------------------------
  TestIdValue

  returns TRUE if the element has an attribute ID with value val
  ----------------------------------------------------------------------*/
static ThotBool TestIdValue (Element el, char *val)
{
  char *id_value;
  int result = FALSE;

  id_value = GetIdValue (el);

  if (id_value)
    {
      if (!strcmp (id_value, val))
	result = TRUE;
      TtaFreeMemory (id_value);
    }

  return (result);
}

/*----------------------------------------------------------------------
  TestElName

  returns TRUE if the element has a name equal to name
  ----------------------------------------------------------------------*/
static ThotBool TestElName (Element el, char *name)
{
  ElementType elType;
  char *typeName;

  elType = TtaGetElementType (el);
  typeName = TtaGetElementTypeName (elType);
#ifdef DEBUG_XPOINTER
  printf ("testing element: %s\n", typeName);
#endif
  if (typeName && !strcmp (name, typeName))
      return TRUE;
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  GetParent

  returns the first parent element which doesn't have
  an exception, or NULL otherwise
  ----------------------------------------------------------------------*/
static Element GetParent (Element el)
{
  Element parent;

  if (!el)
    return NULL;

  parent = el;
  do
    {
      parent = TtaGetParent (parent);
    }
  while (parent && ElIsHidden (parent));

  return parent;
}

/*----------------------------------------------------------------------
  PreviousSibling

  returns the first sibling element which doesn't have
  an exception, or NULL otherwise.
  Because of the way that the Thot tree is built, we need a special
  algorithm to get the childs we would find in a DOM tree (proposed by
  VQ):
  starting from an element, we try to find the first brother.
  If there are no brother, we get the parent. If the parent is not
  hidden, there are no other brothers.
  If the parent is hidden, then we try to get the brothers of this parent.
  For each brother of the parent, we try to get the last child.
  ----------------------------------------------------------------------*/
static void PreviousSibling (Element *el)
{
  Element sibling;

  if (!el || !*el)
    return;

  sibling = *el;
  /* get the previous sibling in the Thot tree */
  TtaPreviousSibling (&sibling);
  if (sibling)
    {
      /* if the element is hidden, return the latest child */
      if (ElIsHidden (sibling))
	  sibling = TtaGetLastChild (sibling);
      *el = sibling;
    }
  else
    {
      /* if there's no child, repeat the algorithm recursively 
	 on each parent, element until we find a child or the
	 the first non-hidden parent */
      sibling = TtaGetParent (*el);
      if (ElIsHidden (sibling))
	{
	  PreviousSibling (&sibling);
	  *el = sibling;
	}
      else
	*el = NULL;
    }
  return;
}

/*----------------------------------------------------------------------
  SearchAttrId

  returns the first element that doesn't have an exception and that
  has an ID attribute with value val
  ----------------------------------------------------------------------*/
Element SearchAttrId (Element root, char *val)
{
  Element el, result;

  if (!root && !val && *val == EOS)
    return (Element) NULL;

  result = 0;
  while (root) 
    {
      /* test the current root element */
      if (TestIdValue (root, val))
	{
	  result = root;
	  break;
	}
      
      /* recursive search all the children of root */
      el = TtaGetFirstChild (root);
      if (!el)
	break;
      result = SearchAttrId (el, val);
      if (result)
	break;
      /* try the same procedure on all the siblings of root */
      TtaNextSibling (&root);
    }

  return result;
}

/*----------------------------------------------------------------------
  SearchSiblingIndex
  ----------------------------------------------------------------------*/
Element SearchSiblingIndex (Element root, char *el_name, int *index)
{
  Element sibling, child, result;

  if (!root)
    return NULL;

  /* we consider an index of 0 equivalent to an index of 1 as we don't
     handle node sets for the moment */
  if (*index == 0)
    (*index)++;

  sibling = root;

  /* get the next sibling in the Thot tree */
  while (sibling)
    {
      /* if the element is hidden, call the algorithm recursively from 
	 this point */
      if (ElIsHidden (sibling))
	  {
	    child = TtaGetFirstChild (sibling);
	    result = SearchSiblingIndex (child, el_name, index);
	    if (result)
	      return result;
	  }
      /* test the current node */
      else if (TestElName (sibling, el_name))
	{
	  /* we found the element */
	  if (*index == 1)
	    return sibling;
	  (*index)--;
	}
      /* go to the next sibling */
      TtaNextSibling (&sibling);
    }
  return (sibling);
}

/*----------------------------------------------------------------------
  SearchTextPosition
  
  Searchs for a given text position (taking into account in-line elements)
  and returns the element where the text is found.
  ----------------------------------------------------------------------*/
ThotBool SearchTextPosition (Element *mark, int *firstCh)
{
  ElementType elType;
  Element el, root;
  int pos;
  int len;

  el = root = *mark;
  pos = *firstCh;

  elType = TtaGetElementType (root);
  /* point to the first text unit */
  if (elType.ElTypeNum != THOT_TEXT_UNIT)
    {
      elType.ElTypeNum = THOT_TEXT_UNIT;
      el = TtaSearchTypedElementInTree (elType, SearchForward, root, el);
    }
  else
    elType.ElTypeNum = THOT_TEXT_UNIT;

  while (1)
    {
      if (!el)
	break;
      len = TtaGetTextLength (el);
      if (pos <= len)
	break;
      else
	pos = pos - len;
      el = TtaSearchTypedElementInTree (elType, SearchForward, root, el);
    }

  *mark = el;
  *firstCh = pos;
  return (el) ? TRUE : FALSE;
}

/*----------------------------------------------------------------------
  XPathList2Str

  Returns an XPath expression corresponding to the list structure.
  It frees the XPath_list while building the expression. It's up to
  the caller to free the returned string.
  Returns NULL in case of error.
  ----------------------------------------------------------------------*/
static char * XPathList2Str (XPathList *xpath_list, int firstCh, int len, ThotBool firstF)
{
  XPathItem *xpath_item, *xpath_tmp;
  char buffer[500];
  char *xpath_expr = NULL;

  xpath_item = *xpath_list;
  if  (firstCh > 0)
    {
      if (firstF)
	StrACat (&xpath_expr, "start-point(string-range(");
      else
	StrACat (&xpath_expr, "end-point(string-range(");
    }

  while (xpath_item)
    {
      /* @@ how can we detect this in a more generic way? */
      if (xpath_item->elType.ElTypeNum != THOT_TEXT_UNIT)
	{
	  if (xpath_item->id_value)
	    snprintf (buffer, sizeof (buffer),
		      "id(\"%s\")", xpath_item->id_value);
	  else
	    snprintf (buffer, sizeof (buffer),
		      "/%s[%d]",  
		      TtaGetElementTypeName (xpath_item->elType),
		      xpath_item->index);
	  StrACat (&xpath_expr, buffer);
	}
      if (xpath_item->id_value)
	TtaFreeMemory (xpath_item->id_value);
      xpath_tmp = xpath_item->next;
      TtaFreeMemory (xpath_item);
      xpath_item = xpath_tmp;
    }

  if  (firstCh > 0)
    {
      snprintf (buffer, sizeof (buffer),
		",\"\",%d,%d))", firstCh, len);
      StrACat (&xpath_expr, buffer);
    }

  return (xpath_expr);
}

/***************************************************
 ** Exported functions
 **************************************************/

/*----------------------------------------------------------------------
  XPointer_ThotEl2XPath
  Starting from element, it returns the XPath pointing to that
  element. 
  It's up to the caller to free the returned string.
  Returns NULL in case of failure.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char *XPointer_ThotEl2XPath (Element start, int firstCh, int len, ThotBool firstF)
#else
char *XPointer_ThotEl2XPath (start, firstCh, len, firstF)
Element start;
int firstCh;
int len;
ThotBool firstF;
#endif /* __STDC__ */
{
  Element el, prev;
  ElementType elType, prevElType;
  STRING s;
  int child_count;
  XPathItem *xpath_item;
  XPathList xpath_list = (XPathItem *) NULL;
  char *xpath_expr;
  char *id_value = NULL;

  /* if the user selected a text, adjust the start/end indexes according
     to its siblings inlined text */
  firstCh += CountInlineChars (start);

  el = start;
  /* if we chose a hidden element, climb up */
  if (ElIsHidden (el))
    el = GetParent (el);
  
  /* browse the tree */
  while (el)
    {
      /* stop browsing the tree if we found an id attribute */
      id_value = GetIdValue (el);
      if (id_value)
	break;
      /* sibling browse */
      child_count = 1;
      elType = TtaGetElementType (el);
      prev = el;
      for (PreviousSibling (&prev) ; prev; PreviousSibling (&prev))
	{
	  prevElType = TtaGetElementType (prev);
          if ((elType.ElTypeNum == prevElType.ElTypeNum)
	      && (elType.ElSSchema == prevElType.ElSSchema))
	    {
	      child_count++;
	      el = prev;
	    }
	}
      /* add the info we found to the xpath list*/
      xpath_item = TtaGetMemory (sizeof (XPathItem));
      xpath_item->elType = elType;
      xpath_item->index = child_count;
      xpath_item->id_value = NULL;
      xpath_item->next = xpath_list;
      xpath_list = xpath_item;
      /* climb up one level */
      el = GetParent (el);
    }

  if (id_value)
    {
      xpath_item = TtaGetMemory (sizeof (XPathItem));
      xpath_item->elType = TtaGetElementType (el);	
      xpath_item->id_value = id_value;
      xpath_item->next = xpath_list;
      xpath_list = xpath_item;
    }
  
  /* find the xpath expression (this function frees the list while building
     the string) */
  xpath_expr = XPathList2Str (&xpath_list, firstCh, len, firstF);
  return (xpath_expr);
}

/*----------------------------------------------------------------------
  XPointer_build
  If there is a selection in the document, it returns a pointer
  to an  XPointer expression that represents what was selected.
  It's up to the caller to free the returned string.
  Returns NULL in case of failure.
  N.B., the view parameter isn't used, but it's included to be coherent
  with the function API.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char * XPointer_build (Document doc, View view)
#else
char * XPointer_build (doc, view)
Document doc;
View view;
#endif
{
  Element     firstEl, lastEl;
  Element     firstV, lastV;

  int         firstCh, lastCh, i;
  int         firstLen;

  char       *firstXpath;
  char       *lastXpath = NULL;
  ElementType elType;

  /* @@ debug */
  char       *xptr_expr = NULL;


  elType.ElSSchema = TtaGetDocumentSSchema (doc);
  /* only do this operation on XML and HTML documents */
  /* @@ JK: should be a function in AHTURLTools */
  if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML"))
      && ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("XHTML"))
      && ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("XML")))
    return NULL;

  /* is the document selected? */
  if (!TtaIsDocumentSelected (doc))
    return NULL;
  
  /* get the first selected element */
  TtaGiveFirstSelectedElement (doc, &firstEl, &firstCh, &i);
  firstCh = AdjustChIndex (firstEl, firstCh);
  
  if (firstEl == NULL)
    return NULL; /* ERROR, there is no selection */

  /* is it a caret or an extension selection? */
  if (TtaIsSelectionEmpty ())
    lastEl = NULL;
  else
    {
      TtaGiveLastSelectedElement (doc, &lastEl, &i, &lastCh);
      lastCh = AdjustChIndex (lastEl, lastCh);
    }

  /* if the selection is in the same element, adjust the first element's
   length */
  if (firstEl == lastEl)
    {
      firstLen = lastCh - firstCh + 1;
      lastEl = NULL;
    }
  else
    firstLen = 1;

  firstXpath = XPointer_ThotEl2XPath (firstEl, firstCh, firstLen, TRUE);
#ifdef DEBUG_XPOINTER
  fprintf (stderr, "\nfirst xpointer is %s", firstXpath);
#endif  
  if (lastEl)
    {
      lastXpath = XPointer_ThotEl2XPath (lastEl, lastCh, 1, FALSE);
#ifdef DEBUG_XPOINTER
      fprintf (stderr, "\nlast xpointer is %s\n", lastXpath);
#endif  
    }
  else 
    {
#ifdef DEBUG_XPOINTER
    fprintf (stderr, "\n");
#endif  
    }

  /* calculate the length of the xptr buffer */
  i = sizeof ("xpointer()/range-to()") + strlen (firstXpath) 
	      + ((lastEl) ? strlen (lastXpath) : 0) + 1;
  xptr_expr = TtaGetMemory (i);

  if (lastEl)
      sprintf (xptr_expr, "xpointer(%s/range-to(%s))", firstXpath, lastXpath);
  else
    sprintf (xptr_expr, "xpointer(%s)", firstXpath);

  TtaFreeMemory (firstXpath);
  if (lastEl)
    TtaFreeMemory (lastXpath);

#ifdef ANNOTATIONS  
  {
    parserContextPtr ctx;
    fprintf (stderr, "final expression is: %s\n", xptr_expr);
    printf ("first el is %d\n", firstEl);
    printf ("last el is %d\n", lastEl);
    /* now, let's try to parse what we generated */
    ctx = XPointer_parse (doc, xptr_expr);
    XPointer_free (ctx);
  }
#endif
  
  return xptr_expr;
}




