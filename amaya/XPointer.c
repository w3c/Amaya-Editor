/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2000
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

#ifdef ANNOTATIONS

#include "XPointer_f.h"
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "parser.h"

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
static char * StrACat (char ** dest, const char * src)
{
  if (src && *src) 
    {
    if (*dest) 
      {
	int length = strlen (*dest);
	if ((*dest  = (char  *) TtaRealloc (*dest, length + strlen(src) + 1)) == NULL)
	/* @@ what to do ?? */
          exit (0);
	strcpy (*dest + length, src);
      } 
    else 
      {
	if ((*dest  = (char  *) TtaGetMemory (strlen(src) + 1)) == NULL)
	  /* @@ what to do? */
	  exit (0);
	strcpy (*dest, src);
      }
    }
  return (*dest);
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
  int result;

  id_value = GetIdValue (el);

  if (id_value)
    {
      if (!strcmp (id_value, val))
	result = TRUE;
      TtaFreeMemory (id_value);
    }
  else
    result = FALSE;

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
  printf ("testing element: %s\n", typeName);
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
static Element SearchAttrId (Element root, char *val)
{
  Element sibling, result;

  if (root && TestIdValue (root, val))
    return root;
  else if (!root)
    return NULL;
  
  sibling = TtaGetFirstChild (root);
  result = 0;
  while (!result && sibling)
    {
      result = SearchAttrId (sibling, val);
      if (result)
	break;
      TtaNextSibling (&sibling);
    }
  
  return result;
}

/*----------------------------------------------------------------------
  SearchChildIndex
  ----------------------------------------------------------------------*/
static Element SearchChildIndex (Element root, char *el_name, int index)
{
  Element sibling, result;

  if (!root)
    return NULL;
  
  if (index == 0)
    index++;

  sibling = TtaGetFirstChild (root);
  if (!sibling)
    return NULL;

  /* get the next sibling in the Thot tree */
  while (sibling)
    {
      /* if the element call the algorithm recursively from this point */
      if (ElIsHidden (sibling))
	  {
	    result = SearchChildIndex (sibling, el_name, index);
	    if (result)
	      return result;
	    else
	      {
		TtaNextSibling (&sibling);
		continue;
	      }
	  }
      /* test the current node */
      if (TestElName (sibling, el_name))
	{
	  /* we found the element */
	  if (index == 1)
	    return sibling;
	  index--;
	}
      /* go to the next sibling */
	TtaNextSibling (&sibling);
    }
  return (sibling);
}


/*----------------------------------------------------------------------
  XPathList2Str

  Returns an XPath expression corresponding to the list structure.
  It frees the XPath_list while building the expression. It's up to
  the caller to free the returned string.
  Returns NULL in case of error.
  ----------------------------------------------------------------------*/
static char * XPathList2Str (XPathList *xpath_list, int firstCh, int len)
{
  XPathItem *xpath_item, *xpath_tmp;
  char buffer[500];
  char *xpath_expr = NULL;
  char *typeName;

  xpath_item = *xpath_list;
  while (xpath_item)
    {
      if (!xpath_item->id_value)
	typeName = TtaGetElementTypeName (xpath_item->elType);

      if (xpath_item->next 
	  && xpath_item->next->elType.ElTypeNum == THOT_TEXT_UNIT)
	{
	  if (firstCh > 0)
	    {
	      if (xpath_item->id_value)
		snprintf (buffer, sizeof (buffer),
			 "/string-range(id(\"%s\"),\"\",%d,%d)",
			 xpath_item->id_value, firstCh, len);
	      else
		snprintf (buffer, sizeof (buffer),
			 "/string-range(%s[%d],\"\",%d,%d)",
			 typeName, xpath_item->index, firstCh, len);
	    }
	  else
	    {
	      if (xpath_item->id_value)
		snprintf (buffer, sizeof (buffer), 
			  "/string-range(id(\"%s\"),\"\"",
			 xpath_item->id_value);
	      else
		snprintf (buffer, sizeof (buffer),
			  "/string-range(%s[%d],\"\"",
			  typeName, xpath_item->index);
	    }
	  /* we remove the extra element, as we have already used it */
	  xpath_tmp = xpath_item->next->next;
	  TtaFreeMemory (xpath_item->next);
	}
      else
	{
	  if (xpath_item->id_value)
	    snprintf (buffer, sizeof (buffer),
		      "id(\"%s\")", xpath_item->id_value);
	  else
	    snprintf (buffer, sizeof (buffer),
		      "/%s[%d]", typeName, 
		     xpath_item->index);
	  xpath_tmp = xpath_item->next;
	}

      if (xpath_item->id_value)
	TtaFreeMemory (xpath_item->id_value);
      TtaFreeMemory (xpath_item);
      xpath_item = xpath_tmp;

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
char *XPointer_ThotEl2XPath (Element start, int firstCh, int len)
#else
char *XPointer_ThotEl2XPath (start)
Element start;
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
      xpath_item->id_value = id_value;
      xpath_item->next = xpath_list;
      xpath_list = xpath_item;
    }
  
  /* find the xpath expression (this function frees the list while building
     the string) */
  xpath_expr = XPathList2Str (&xpath_list, firstCh, len);
  return (xpath_expr);
}


/*----------------------------------------------------------------------
  XPointer_Xptr2Thot
  Convers an XPointer expression into ... a selection? returns the nodes?
  API yet to be defined.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void XPointer_Xptr2tTot (Document doc, View view, CHAR_T *expr)
#else
void XPointer_Xptr2Thot (doc, view, expr)
Document doc;
View view;
CHAR_T *expr;
#endif
{
}

/*----------------------------------------------------------------------
  XPointer_Thot2Xptr
  If there is a selection in the document, it returns a pointer
  to an  XPointer expression that represents what was selected.
  It's up to the caller to free the returned string.
  Returns NULL in case of failure.
  N.B., the view parameter isn't used, but it's included to be coherent
  with the function API.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char * XPointer_Thot2Xptr (Document doc, View view)
#else
char * XPointer_Thot2Xptr (doc, view)
Document doc;
View view;
#endif
{
  Element     firstEl, lastEl;
  int         firstCh, lastCh, i;
  int         firstLen;

  char       *firstXpath;
  char       *lastXpath;
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

  firstXpath = XPointer_ThotEl2XPath (firstEl, firstCh, firstLen);
  fprintf (stderr, "\nfirst xpointer is %s", firstXpath);
  
  if (lastEl)
    {
      lastXpath = XPointer_ThotEl2XPath (lastEl, lastCh, 1);
      fprintf (stderr, "\nlast xpointer is %s\n", lastXpath);
    }
  else 
    fprintf (stderr, "\n");
  
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

  fprintf (stderr, "final expression is: %s\n", xptr_expr);

  /* @@@ test */
  firstEl = TtaGetMainRoot (doc);
  firstEl = SearchAttrId (firstEl, "jose");
  if (firstEl)
    printf ("found id on element %d\n", firstEl);
  else
    printf ("no attribute found\n");

  /* point to body */
  firstEl = TtaGetMainRoot (doc);
  firstEl = TtaGetFirstChild (firstEl);
  TtaNextSibling (&firstEl);
  /* point to the first child */
  firstEl = TtaGetFirstChild (firstEl);

  firstEl = SearchChildIndex (firstEl, "dt", 3);
  if (firstEl)
    printf ("found element %d\n", firstEl);
  else
    printf ("no element found\n");

  /* @@@ test */

  /* @@ should return xptr_expr */
  TtaFreeMemory (xptr_expr);
  return NULL;
}
#endif ANNOTATIONS
