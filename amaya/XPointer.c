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
  XPathItem * next;
};

typedef XPathItem * XPathList;

/*
** Get selected element, print Xpointer expression for both sides of
** the selection
 */

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
  /* get the next sibling in the Thot tree */
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
  XPathList2Str

  Returns an XPath expression corresponding to the list structure.
  It frees the XPath_list while building the expression. It's up to
  the caller to free the returned string.
  Returns NULL in case of error.
  ----------------------------------------------------------------------*/
static char * XPathList2Str (XPathList *xpath_list)
{
  XPathItem *xpath_item, *xpath_tmp;
  static char xpath_expr[500];
  char *typeName;
  int index;

  xpath_item = *xpath_list;
  index = 0;
  while (xpath_item)
    {
      typeName = TtaGetElementTypeName (xpath_item->elType);
      sprintf (&xpath_expr[index], "/%s[%d]", typeName, xpath_item->index);
      index = strlen (xpath_expr);
      xpath_tmp = xpath_item->next;
      TtaFreeMemory (xpath_item);
      xpath_item = xpath_tmp;
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
char *XPointer_ThotEl2XPath (Element start)
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
  
  el = start;
  /* if we chose a hidden element, climb up */
  if (ElIsHidden (el))
    el = GetParent (el);
  /* browse the tree */
  while (el)
    {
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
      el = GetParent (el);
      if (el)
	{
	  xpath_item = TtaGetMemory (sizeof (XPathItem));
	  xpath_item->elType = elType;
	  xpath_item->index = child_count;
	  xpath_item->next = xpath_list;
	  xpath_list = xpath_item;
	}
    }
  
  /* find the xpath expression (this function frees the list while building
     the string) */
  xpath_expr = XPathList2Str (&xpath_list);
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
  char       *firstXpath;
  char       *lastXpath;
  ElementType elType;

  elType.ElSSchema = TtaGetDocumentSSchema (doc);
  /* only do this operation on XML and HTML documents */
  /* @@ JK: should be a function in AHTURLTools */
  if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML"))
      && ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("XHTML"))
      && ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("XML")))
    return NULL;

  TtaGiveFirstSelectedElement (doc, &firstEl, &firstCh, &i);
  TtaGiveLastSelectedElement (doc, &lastEl, &i, &lastCh);
  
  if (firstEl == NULL)
    return NULL; /* ERROR, there is no selection */
  
  firstXpath = XPointer_ThotEl2XPath (firstEl);
  fprintf (stderr, "\nfirst xpointer is %s", firstXpath);

  if (lastEl)
    {
      lastXpath = XPointer_ThotEl2XPath (lastEl);
      fprintf (stderr, "\nlast xpointer is %s\n", lastXpath);
    }

  return NULL;
}
#endif ANNOTATIONS











