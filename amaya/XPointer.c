/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 2000-2008
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
 * To do:
 *
 * XML Escaping... (4.1.2 in the spec)
 */

#undef DEBUG_XPOINTER

#ifdef _WINGUI
#define snprintf _snprintf
#endif /* _WINGUI */

#define THOT_EXPORT extern
#include "amaya.h"
#include "MathML.h"
#include "SVG.h"
#include "XLink.h"
#include "XML.h"
#include "init_f.h"
#ifdef ANNOTATIONS
#include "Annot.h"
#endif /* ANNOTATIONS */
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
#define THOT_TEXT_UNIT   1
#define THOT_GRAPH_UNIT  2
#define THOT_SYMBOL_UNIT 3

typedef enum _selMode {
  SEL_START_POINT=1,
  SEL_END_POINT=16,
  SEL_STRING_RANGE=32,
  SEL_RANGE_TO=64
} selMode;

typedef enum _selType {
  SEL_FIRST_EL=0,
  SEL_LAST_EL=1
} selType;

static char * xptr_buffer; /* temporary buffer where the user may store
                              an XPointer */

static Element RootElement; /* the root of the tree we're parsing (so that
                               we can stop the parsing */

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
              *dest = (char*)status;
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
  CountInLineChars

  returns the number of characters that may be found in inline
  sibling elements from element Mark, such as in <p>an 
  <strong>inline</strong> example</p>.
  If any inline sibling elements are found, the Mark element is updated
  to point to its parent.
  ----------------------------------------------------------------------*/
static int CountInlineChars (Element *mark, int firstCh, selMode *mode)
{
  ElementType elType;
  Element el;
  Element parent;
  int count = 0;

  if (*mark == NULL)
    return 0;

  elType = TtaGetElementType (*mark);
  if (elType.ElTypeNum != THOT_TEXT_UNIT)
    return 0;

  parent = TtaGetParent (*mark);
  el = parent;
  while (1)
    {
      el = TtaSearchTypedElement (elType, SearchForward, el);
      if (el == *mark)
        break;
      count += TtaGetTextLength (el);
    }

  if (count > 0) 
    {

      /* if we were at an empty end text element, but there were some adjacent
         text sibling elements, adjust the selection mode flag */
      if (firstCh == 0)
        {
          *mode = (selMode)((int)*mode | SEL_STRING_RANGE);
          /* START or END point? */
          el = *mark;
          TtaNextSibling (&el);
          if (el)
            {
              elType = TtaGetElementType (el);
              if (elType.ElTypeNum == THOT_TEXT_UNIT)
                *mode = (selMode)((int)*mode | SEL_START_POINT);
              else
                *mode = (selMode)((int)*mode | SEL_END_POINT);
            }
          else
            *mode = (selMode)((int)*mode | SEL_END_POINT);
        }

      /* now point to the parent */
      /* @@ JK: if count? */
      *mark = parent;
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
  ThotBool result;

  if (!el)
    return FALSE;

  elType = TtaGetElementType (el);

  result = TtaHasHiddenException (elType);

  /* ignore all elements that were added by Amaya (they don't have
     a line number */
  if (!result)
    result = (TtaGetElementLineNumber (el) == 0);


  return result;
}

/*----------------------------------------------------------------------
  ElIsXLink

  returns TRUE if the element is our special Xlink element, false
  otherwise/
  ----------------------------------------------------------------------*/
static ThotBool ElIsXLink (Element el)
{
  ElementType elType;
  char *schema_name;

  if (!el)
    return FALSE;

  elType = TtaGetElementType (el);
  schema_name = TtaGetSSchemaName (elType.ElSSchema);

  if (!strcmp (schema_name, "XLink")
      && elType.ElTypeNum == XLink_EL_XLink)
    return TRUE;
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  AdjustSelMode

  Makes sure that the index to a text element doesn't point outside
  of the length of the text.
  If we had selected an XLink element, we chose a correct element and
  selection mode.
  Returns the adjusted index, if any.
  ----------------------------------------------------------------------*/
static void AdjustSelMode (Element *el, int *start, int index, selMode *mode, selType type)
{
  int len;
  ElementType elType;
  Element tmp_el;

  /* skip the hidden XLInk element */
  if (ElIsXLink (*el))
    {
      tmp_el = *el;
      if (type == SEL_FIRST_EL)
        {
          do
            {
              TtaNextSibling (&tmp_el);
            }
          while (tmp_el && ElIsXLink (tmp_el));
          if (tmp_el)
            {
              /* we use the first non XLink sibling. If it's of type
                 text, we use 1 as the value of start */
              elType = TtaGetElementType (tmp_el);
              if (elType.ElTypeNum == THOT_TEXT_UNIT)
                *start = 1;
            }
          else
            {
              /* we use the parent */
              tmp_el = TtaGetParent (*el);
              *start = 0;
            }
        }
      else
        {
          do 
            {
              TtaPreviousSibling (&tmp_el);
            } 
          while (tmp_el && ElIsXLink (tmp_el));

          if (tmp_el)
            {
              /* we use the precedent non-xlink sibling. If it's of
                 type text, we give start the value of the last char
                 of that sibling */
              elType = TtaGetElementType (tmp_el);
              if (elType.ElTypeNum == THOT_TEXT_UNIT)
                {
                  len = TtaGetTextLength (tmp_el);
                  *start = len + 1;
                }
            }
          else
            {
              /* we use the parent */
              tmp_el = TtaGetParent (*el);
              *start = 0;
            }
        }
      *el = tmp_el;
    }
	  
  if (*start > 0)
    {
      len = TtaGetTextLength (*el);
      *mode = (selMode)((int)*mode | SEL_STRING_RANGE);
      if (*start > len)
        {
          *mode = (selMode)((int)*mode | SEL_END_POINT);
          *start = len;
        }
      else if (*start > index)
        {
          if (type == SEL_FIRST_EL)
            *mode = (selMode)((int)*mode | SEL_START_POINT);
          else
            {
              *mode = (selMode)((int)*mode | SEL_END_POINT);
              *start = *start - 1;
            }
        }
    }
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
  char *schema_name;
  char *value;
  int len;

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  schema_name = TtaGetSSchemaName (elType.ElSSchema);
  if (!strcmp (schema_name, "XLink"))
    /* ignore all XLink elements (they are only annotation
       related, and invisible to the document */
    return NULL;
  else if (!strcmp (schema_name, "MathML"))
    attrType.AttrTypeNum = MathML_ATTR_id;
#ifdef _SVG
  else if (!strcmp (schema_name, "SVG"))
    attrType.AttrTypeNum = SVG_ATTR_id;
#endif /* _SVG */
  else if (!strcmp (schema_name, "HTML") || !strcmp (schema_name, "XHTML"))
    attrType.AttrTypeNum = HTML_ATTR_ID;
  else /* assume it is generic XML */
#ifdef XML_GENERIC
    attrType.AttrTypeNum = XML_ATTR_xmlid;
#else /* XML_GENERIC */
  attrType.AttrTypeNum = 0;
#endif /* XML_GENERIC */

  attr = TtaGetAttribute (el, attrType);
  if (attr != NULL)
    {
      /* there's an ID attribute */
      len = TtaGetTextAttributeLength (attr) + 1;
      value = (char *)TtaGetMemory (len);
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
static ThotBool TestIdValue (Element el, const char *val)
{
  char *   id_value;
  ThotBool result = FALSE;

  id_value = GetIdValue (el);

  if (id_value && val)
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
static ThotBool TestElName (Element el, const char *name)
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
  AGetRootElement

  returns the first root element of the document.
  ----------------------------------------------------------------------*/
Element AGetRootElement (Document doc)
{
  Element el;

  if (doc == 0)
    return NULL;

  el = TtaGetRootElement (doc);

#ifdef ANNOTATIONS
  if (DocumentTypes[doc] == docAnnot)
    {
      ElementType elType;

      /* use the first child of the Annotation body as the root element */
      elType = TtaGetElementType (el);
      elType.ElTypeNum = Annot_EL_Body;
      el = TtaSearchTypedElement (elType, SearchInTree, 
                                  el);
      if (el)
        el = TtaGetFirstChild (el);
    }
#endif /* ANNOTATIONS */
  return el;
}

/*----------------------------------------------------------------------
  AGetParent

  returns the first parent element which doesn't have
  an exception, or NULL otherwise
  ----------------------------------------------------------------------*/
static Element AGetParent (Element el)
{
  Element parent;

  /* stop if we're already at the Root Element */
  if (!el || el == RootElement)
    return NULL;

  parent = el;
  do
    parent = TtaGetParent (parent);
  while (parent && ElIsHidden (parent));
  return parent;
}

/*----------------------------------------------------------------------
  AGetLastChild

  returns the last child in a tree
  ----------------------------------------------------------------------*/
static Element AGetLastChild (Element el)
{
  Element child;

  if (!el)
    return NULL;  
  child = el;
  while (child)
    {
      child = TtaGetLastChild (child);
      if (child && !ElIsHidden (child))
        break;
    }

  return child;
}


/*----------------------------------------------------------------------
  PreviousSibling

  returns the first sibling element which doesn't have
  an exception, or NULL otherwise.
  Because of the way that the Thot tree is built, we need a special
  algorithm to get the childs we would find in a DOM tree (proposed by
  VQ):
  starting from an element, we try to find the first sibling.
  If there are no siblings, we get the parent. If the parent is not
  hidden, there are no other brothers, and we return NULL.
  If the parent is hidden, then we try to get the brothers of this parent.
  For each brother of the parent, we try to get the last child.
  ----------------------------------------------------------------------*/
static void PreviousSibling (Element *el)
{
  Element sibling;
  Element tmp_el;

  if (!el || !*el)
    return;

  sibling = *el;
  /* get the previous sibling in the Thot tree */
  TtaPreviousSibling (&sibling);
  if (sibling)
    {
      /* if the element is hidden, return the latest child */
      if (ElIsHidden (sibling))
        {
          tmp_el = AGetLastChild (sibling);
          if (!tmp_el)
            {
              /* there was no child, let's try the next brother */
              PreviousSibling (&sibling);
            }
          else
            sibling = tmp_el;
        }
      *el = sibling;
    }
  else
    {
      /* if there's no child, repeat the algorithm recursively 
         on each parent, element until we find a child or the
         the first non-hidden parent */
      sibling = TtaGetParent (*el);

      /* we only continue searching if the parent is hidden */
      if (sibling && ElIsHidden (sibling))
        PreviousSibling (&sibling);
      else
        sibling = NULL;
      *el = sibling;
    }
  return;
}

/*----------------------------------------------------------------------
  SearchAttrId

  returns the first element that doesn't have an exception and that
  has an ID attribute with value val
  ----------------------------------------------------------------------*/
Element SearchAttrId (Element root, const char *val)
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
      if (el)
        {
          result = SearchAttrId (el, val);
          if (result)
            break;
        }
      /* try the same procedure on all the siblings of root */
      TtaNextSibling (&root);
    }

  return result;
}

/*----------------------------------------------------------------------
  SearchSiblingIndex
  ----------------------------------------------------------------------*/
Element SearchSiblingIndex (Element root, const char *el_name, int *index)
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
      if (!TestElName (sibling, "Annotation")) /* ignore our XLink element */
        {
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
static char * XPathList2Str (XPathList *xpath_list, int firstCh, int len,
                             int mode, ThotBool firstF)
{
  XPathItem *xpath_item, *xpath_tmp;
  char      *buffer = NULL, *name;
  char      *xpath_expr = NULL;
  int        l;

  xpath_item = *xpath_list;
  if  (mode & SEL_STRING_RANGE)
    {
      if (mode & SEL_END_POINT)
        StrACat (&xpath_expr, "end-point(string-range(");
      else if (mode & SEL_START_POINT)
        {
          if (firstF)
            StrACat (&xpath_expr, "start-point(string-range(");
          else
            StrACat (&xpath_expr, "end-point(string-range(");
        }
      else
        StrACat (&xpath_expr, "string-range(");
    }

  while (xpath_item)
    {
      /* @@ how can we detect this in a more generic way? */
      if (xpath_item->elType.ElTypeNum != THOT_TEXT_UNIT)
        {
          if (xpath_item->id_value)
            {
              l = strlen (xpath_item->id_value) + 8;
              buffer = (char *)TtaGetMemory (l);
              strcpy (buffer, "id(\""); 
              strcat (buffer, xpath_item->id_value);
              strcat (buffer, "\")");
            }
          else
            {
              name = TtaGetElementTypeName (xpath_item->elType);
              l = strlen (name) + 14;
              buffer = (char *)TtaGetMemory (l);
              sprintf (buffer, "/%s[%d]", name, xpath_item->index);
            }
          StrACat (&xpath_expr, buffer);
          TtaFreeMemory (buffer);
          buffer = NULL;
        }
      if (xpath_item->id_value)
        TtaFreeMemory (xpath_item->id_value);
      xpath_tmp = xpath_item->next;
      TtaFreeMemory (xpath_item);
      xpath_item = xpath_tmp;
    }

  if (mode & SEL_STRING_RANGE)
    {
      l = 30;
      buffer = (char *)TtaGetMemory (l);
      sprintf (buffer, ",\"\",%d,%d)", firstCh, len);
      StrACat (&xpath_expr, buffer);
      if (mode & SEL_START_POINT || mode & SEL_END_POINT)
        StrACat (&xpath_expr, ")");
      TtaFreeMemory (buffer);
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
static char *XPointer_ThotEl2XPath (Element start, int firstCh, int len,
                                    selMode mode, ThotBool firstF)
{
  Element     el, prev;
  ElementType elType, prevElType;
  int         child_count;
  XPathItem  *xpath_item;
  XPathList   xpath_list = (XPathItem *) NULL;
  char       *xpath_expr;
  char       *id_value = NULL;

  /* if the user selected a text, adjust the start/end indexes according
     to its siblings inlined text */
  firstCh += CountInlineChars (&start, firstCh, &mode);

  el = start;
  elType = TtaGetElementType (el);
  /* if we chose a hidden element or a GRAPH UNIT (SVG), climb up */
  if (ElIsHidden (el) || elType.ElTypeNum == THOT_GRAPH_UNIT)
    el = AGetParent (el);

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
      xpath_item = (XPathItem*)TtaGetMemory (sizeof (XPathItem));
      xpath_item->elType = elType;
      xpath_item->index = child_count;
      xpath_item->id_value = NULL;
      xpath_item->next = xpath_list;
      xpath_list = xpath_item;
      /* climb up one level */
      el = AGetParent (el);
    }

  if (id_value)
    {
      xpath_item = (XPathItem*)TtaGetMemory (sizeof (XPathItem));
      xpath_item->elType = TtaGetElementType (el);	
      xpath_item->id_value = id_value;
      xpath_item->next = xpath_list;
      xpath_list = xpath_item;
    }
  
  /* find the xpath expression (this function frees the list while building
     the string) */
  xpath_expr = XPathList2Str (&xpath_list, firstCh, len, mode, firstF);
  return (xpath_expr);
}

/*----------------------------------------------------------------------
  make the fragment identifier (Char Scheme) based on the selected position
  ----------------------------------------------------------------------*/
char * TextPlainId_build ( Document doc, View view, ThotBool useDocRoot )
{
  Element     firstEl, el, child,el_ex, child_ex;
  ElementType elType;
  int         firstCh, i;  
  int         line, line2, len, len2, Char = 0, time = 0;
  int         len_ex = 0, line_ex;
  int         count_child = 1, len_of_line, length_child, k;
  int        *len_part_of_child, *memory_of_child;
  char       *txtfid_expr = NULL;

  /* @@ debug */
  TtaGiveFirstSelectedElement (doc, &firstEl, &firstCh, &i);  
  line = TtaGetElementLineNumber(firstEl);
  len = TtaGetElementVolume(firstEl);
  
  if (len == 0)
	  line--;

  el_ex = TtaGetMainRoot (doc);
  elType = TtaGetElementType (el_ex);
  elType.ElTypeNum = TextFile_EL_Line_;
  el_ex = TtaSearchTypedElement (elType, SearchForward, el_ex);
  child_ex = TtaGetFirstChild (el_ex);

  for (line_ex=1; line_ex < line; line_ex++)
	  TtaNextSibling(&el_ex);

  child_ex = TtaGetFirstChild(el_ex);    
  len_ex = TtaGetElementVolume(child_ex);
  count_child = 1;

  // adjust firstCh from head of the line
  while (child_ex != firstEl)
    {
      if (count_child % 3 != 2)
        firstCh += len_ex;
      count_child++;
      TtaNextSibling(&child_ex);
      len_ex = TtaGetElementVolume(child_ex);
    }

  el = TtaGetMainRoot (doc);
  elType = TtaGetElementType (el);
  elType.ElTypeNum = TextFile_EL_Line_;
  el = TtaSearchTypedElement (elType, SearchForward, el);
  child = TtaGetFirstChild (el);
  line2 = TtaGetElementLineNumber(child);
  len = TtaGetElementVolume (el);
  len2 = TtaGetElementVolume (child);

  if (len2 == 0)
    line2--;

  len_of_line = len2;    
  if (el)
    {
		  /* Consider "Annotation Icon" if Annotation Icon is on the line */
		  if (len != len_of_line)
	      {
          count_child = 1;
          // analysis the structure of the line including "Annotation Icon"
          while (len != len_of_line)
            {
              TtaNextSibling(&child);
              length_child = TtaGetElementVolume(child);
              len_of_line += length_child;
              count_child++; // count the number of child in the line
            }

          len_part_of_child = (int *)TtaGetMemory( (sizeof(int)) * (count_child + 1) );
          memory_of_child = len_part_of_child;
          child = TtaGetFirstChild(el);

          // input the length of child one by one
          for (k=0; k < count_child; k++)
            {
              length_child = TtaGetElementVolume(child);
              len_part_of_child[k] = length_child;
              TtaNextSibling(&child);
            }

          // adjust the length of line (subtract that of Annotation Icon)
          for (k=1; k < count_child; k+=3)
            len -= len_part_of_child[k];

          child = TtaGetFirstChild(el);
          if (line2 < line)
            free(memory_of_child);
        }
		  
		  // calculate the Char Scheme value
		  while (child)
        {
          Char += len; 
          if (line2 < line)
            {
              TtaNextSibling(&el);
              child = TtaGetFirstChild(el);			  
              line2++;
              len = TtaGetElementVolume (el);
              len2 = TtaGetElementVolume (child);
              len_of_line = len2;			  
              time++;
              count_child=1;
            }

          if (line2 ==line && len == 0)
            {
              TtaNextSibling(&el);
              child = TtaGetFirstChild(el);
              line2 = TtaGetElementLineNumber(child);
              len_of_line = TtaGetElementVolume (child);
              Char += time;			 
              break;
            }

          if (line2 == line && time==0)
            {
              Char = firstCh - 1;
              break;
            }

          if (line2 ==line && firstCh > len && time != 0)
            {
              Char += firstCh - 1 + time;
              break;
            }
          else if (line2 == line && firstCh <= len && time != 0)
            {
              Char += firstCh - 1 + time;
              break;
            }

          /* Consider "Annotation Icon" if Annotation Icon is on the line */
          if (len != len_of_line)
            {		      
              count_child = 1;
              // analysis the structure of the line including "Annotation Icon"
              while (len != len_of_line)
                {
                  TtaNextSibling (&child);
                  length_child = TtaGetElementVolume (child);
                  len_of_line += length_child;
                  count_child++;
                }

              len_part_of_child = (int *)TtaGetMemory ((sizeof(int)) * (count_child + 1));
              memory_of_child = len_part_of_child;
              child = TtaGetFirstChild(el);
              // input the length of child one by one
              for (k=0; k < count_child; k++)
                {
                  length_child = TtaGetElementVolume (child);
                  len_part_of_child[k] = length_child;
                  TtaNextSibling (&child);
                }

              // adjust the length of line (subtract that of Annotation Icon)
              for (k = 1; k < count_child; k += 3)
                len -= len_part_of_child[k];

              child = TtaGetFirstChild(el);
              if (line2 < line)
                free(memory_of_child);
            } 
        }
	  }

  txtfid_expr = (char *)TtaGetMemory (30);      
  sprintf (txtfid_expr, "char=%d", Char); // make the fragment identifier sentence
	return txtfid_expr;
}

/*----------------------------------------------------------------------
  XPointer_build
  If there is a selection in the document, it returns a pointer
  to an  XPointer expression that represents what was selected.
  If useDocRoot is true, then it computes an XPointer for the
  document's root.
  It's up to the caller to free the returned string.
  Returns NULL in case of failure.
  N.B., the view parameter isn't used, but it's included to be coherent
  with the function API.
  ----------------------------------------------------------------------*/
char * XPointer_build (Document doc, View view, ThotBool useDocRoot)
{
  Element     firstEl, lastEl;
  ElementType elType;
  int         firstCh, lastCh, i;
  int         firstLen;
  char       *firstXpath;
  char       *lastXpath = NULL;
  char       *schemaName;

  selMode    firstMode = (selMode)0;
  selMode    lastMode = (selMode)0;

  /* @@ debug */
  char       *xptr_expr = NULL;

  elType.ElSSchema = TtaGetDocumentSSchema (doc);
  /* only do this operation on XML and HTML documents */
  /* @@ JK: should be a function in AHTURLTools */

  schemaName = TtaGetSSchemaName (elType.ElSSchema);
  if (strcmp(schemaName, "HTML")
      && strcmp(schemaName, "XHTML")
      && strcmp(schemaName, "XML")
      && strcmp(schemaName, "MathML")
      && strcmp(schemaName, "SVG")
      && strcmp(schemaName, "Annot")
      && DocumentTypes[doc] != docXml)
    return NULL;
  
  /* is the document selected? */
  if (useDocRoot)
    {
      firstEl = AGetRootElement (doc);
      if (!firstEl)
        return NULL; /* something went wrong */
      firstLen = 0;
      firstCh = 0;
      lastEl = NULL;
      firstMode  = (selMode)((int)firstMode & ~(SEL_START_POINT | SEL_END_POINT));
    }
  else
    {
      if (!TtaIsDocumentSelected (doc))
        /* nothing was selected */
        return NULL;
  
      /* get the first selected element */
      TtaGiveFirstSelectedElement (doc, &firstEl, &firstCh, &i);
#ifdef DEBUG_XPOINTER
      printf ("first Ch is %d, i is %d\n", firstCh, i);
#endif 
      AdjustSelMode (&firstEl, &firstCh, i, &firstMode, SEL_FIRST_EL);
      
      if (firstEl == NULL)
        return NULL; /* ERROR, there is no selection */

      if  (TtaGetElementLineNumber (firstEl) == 0)
        {
          /* ERROR, don't annotate end elements that Amaya added
             internally, but that don't exist in the document */
          InitInfo ("XPointer", TtaGetMessage (AMAYA, AM_NO_SXPOINTER));
          return NULL;
        }

      /* is it a caret or an extension selection? */
      if (TtaIsSelectionEmpty ())
        lastEl = NULL;
      else
        {
          TtaGiveLastSelectedElement (doc, &lastEl, &i, &lastCh);
#ifdef DEBUG_XPOINTER
          printf ("last Ch is %d, i is %d\n", lastCh, i);
#endif 
          AdjustSelMode (&lastEl, &lastCh, i, &lastMode, SEL_LAST_EL);

          if  (TtaGetElementLineNumber (lastEl) == 0)
            {
              /* ERROR, don't annotate end elements that Amaya added
                 internally, but that don't exist in the document */
              InitInfo ("XPointer", TtaGetMessage (AMAYA, AM_NO_EXPOINTER));
              return NULL;
            }

        }

      /* if the selection is in the same element, adjust the first element's
         length */
      if (firstEl == lastEl)
        {
          /* if we have an empty selection, ignore the last element and its data */
          if (lastCh != firstCh)
            {
              firstLen = lastCh - firstCh + 1;
              firstMode  = (selMode)((int)firstMode & ~(SEL_START_POINT | SEL_END_POINT));
            }
          else  /* the selection is only a caret */
            {
              firstLen = 1;
            }
          lastEl = NULL;
        }
      else
        {
          firstLen = 1;
          firstMode = (selMode)((int)firstMode | SEL_RANGE_TO);
          lastMode = (selMode)((int)lastMode | SEL_RANGE_TO);
        }
    }

  /* @@ JK: We don't know yet how to handle annotations on symbols, so we just
     forbid making XPointers on them, for the moment @@ */
  elType = TtaGetElementType (firstEl);
  if (elType.ElTypeNum == THOT_SYMBOL_UNIT)
    return NULL;
  if (lastEl)
    {
      elType = TtaGetElementType (lastEl);
      if (elType.ElTypeNum == THOT_SYMBOL_UNIT)
        return NULL;
    }

  /* remember the root of the tree we are annotating, so that we can stop
     walking the tree when we reach it */
  RootElement = AGetRootElement (doc);

  firstXpath = XPointer_ThotEl2XPath (firstEl, firstCh, firstLen, firstMode, TRUE);
#ifdef DEBUG_XPOINTER
  fprintf (stderr, "\nfirst xpointer is %s", firstXpath);
#endif  
  if (lastEl)
    {
      lastXpath = XPointer_ThotEl2XPath (lastEl, lastCh, 1, lastMode, FALSE);
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
  if (firstXpath)
    {
      i = sizeof ("xpointer()/range-to()") + strlen (firstXpath) 
        + ((lastEl) ? strlen (lastXpath) : 0) + 1;
      xptr_expr = (char *)TtaGetMemory (i);
      
      if (lastEl)
        sprintf (xptr_expr, "xpointer(%s/range-to(%s))", firstXpath, lastXpath);
      else
        sprintf (xptr_expr, "xpointer(%s)", firstXpath);
    }
  
  TtaFreeMemory (firstXpath);
  if (lastEl)
    TtaFreeMemory (lastXpath);

#ifdef DEBUG_XPOINTER
  {
    XPointerContextPtr ctx;
    fprintf (stderr, "final expression is: %s\n", xptr_expr);
    printf ("first el is %p\n", firstEl);
    printf ("last el is %p\n", lastEl);
    /* now, let's try to parse what we generated */
    ctx = XPointer_parse (doc, xptr_expr);
    XPointer_free (ctx);
  }
#endif
  
  return xptr_expr;
}


/*----------------------------------------------------------------------
  XPointer_bufferStore
  Stores into a local buffer the full XPointer corresponding to the
  current selection or caret position.
  ----------------------------------------------------------------------*/
void XPointer_bufferStore (Document doc, View view)
{
  char *xptr;

  /* we need to make a full URL */
  if (!DocumentURLs[doc] || *DocumentURLs[doc] == EOS)
    return;

  /* free the precedent buffer */
  XPointer_bufferFree ();
  /* build  the XPointer */
  xptr = XPointer_build (doc, view, FALSE);
  /* make a full URI */
  if (xptr)
    {
      xptr_buffer = (char *)TtaGetMemory (strlen (DocumentURLs[doc]) + strlen (xptr) + 2);
      sprintf (xptr_buffer, "%s#%s", DocumentURLs[doc], xptr);
    }
}

/*----------------------------------------------------------------------
  XPointer_buffer
  Returns the buffer that may contain a stored XPointer. The buffer
  may be empty.
  ----------------------------------------------------------------------*/
char * XPointer_buffer (void)
{
  return xptr_buffer;
}


/*----------------------------------------------------------------------
  XPointer_bufferFree
  Frees the memory associated to the local buffer
  ----------------------------------------------------------------------*/
void XPointer_bufferFree (void)
{
  if (xptr_buffer)
    {
      TtaFreeMemory (xptr_buffer);
      xptr_buffer = NULL;
    }
}
