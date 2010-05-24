/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module contains editing functions for handling MathML objects.
 *
 * Authors: I. Vatton, V. Quint
 *          R. Guetari - Windows routines.
 *          F. Wang - MathML panel
 */         

#ifdef _WX
#include "wx/wx.h"
#endif /* _WX */

#include <stdarg.h>

/* Included headerfiles */
#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "trans.h"
#include "undo.h"
#include "interface.h"
#include "MathML.h"
#include "parser.h"
#ifdef _SVG
#include "SVG.h"
#endif /* _SVG */
#include "document.h"
#include "mathedit.h"

/* Global variables for dialogues */
static int Math_occurences = 0;
static int Math_OperatorType = 0;
static int Math_open, Math_close, Math_sep;
static int Math_integral_number, Math_integral_contour, Math_integral_type;
static ThotBool	IsLastDeletedElement = FALSE;
static Element	LastDeletedElement = NULL;
static Element  MathElementSelected = NULL;
static Document DocMathElementSelected = 0;

#include "fetchXMLname_f.h"
#include "SVGbuilder_f.h"
#include "html2thot_f.h"
#include "HTMLtable_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLpresentation_f.h"
#include "MathMLbuilder_f.h"
#include "Mathedit_parser_f.h"
#include "styleparser_f.h"
#include "trans_f.h"
#include "UIcss_f.h"
#include "SVGedit_f.h"
#ifdef _WX
#include "wxdialogapi_f.h"
#include "appdialogue_wx.h"
#include "paneltypes_wx.h"
#endif /* _WX */
#ifdef _WINDOWS
#include <commctrl.h>
#endif /* _WINDOWS */
#include "XLinkedit_f.h"
#include "templateUtils_f.h"
#include "templates.h"
#include "templates_f.h"

int CurrentMathEditMode = DEFAULT_MODE;

/* Function name table */
typedef char     functName[10];
static  functName  functionName[] =
  {
    "arccosh",
    "arccos",
    "arccoth",
    "arccot",
    "arccsch",
    "arccsc",
    "arcsech",
    "arcsec",
    "arcsinh",
    "arcsin",
    "arctanh",
    "arctan",
    "arg",
    "card",
    "codomain",
    "cosh",
    "cos",
    "coth",
    "cot",
    "csch",
    "csc",
    "curl",
    "det",
    "div",
    "domain",
    "exp",
    "gcd",
    "grad",
    "image",
    "lcm",
    "lim",
    "log",
    "ln",
    "max",
    "median",
    "min",
    "mode",
    "mod",
    "sech",
    "sec",
    "sinh",
    "sin",
    "tanh",
    "tan",
    "xor",
    ""
  };

/*----------------------------------------------------------------------
  RemoveAttr
  Remove attribute of type attrTypeNum from element el, if it exists
  -----------------------------------------------------------------------*/
static void RemoveAttr (Element el, Document doc, int attrTypeNum)
{
  ElementType	elType;
  AttributeType attrType;
  Attribute	attr;

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = attrTypeNum;
  attr = TtaGetAttribute (el, attrType);
  if (attr != NULL)
    TtaRemoveAttribute (el, attr, doc);
}

/*----------------------------------------------------------------------
  MathSetAttributes
  Set attributes of element el according to its content.
  -----------------------------------------------------------------------*/
static void MathSetAttributes (Element el, Document doc, Element* selEl)
{
  ElementType	elType, parentType;
  Element	parent, grandParent;

  elType = TtaGetElementType (el);
  if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
    return;
  if (elType.ElTypeNum == MathML_EL_MO ||
      elType.ElTypeNum == MathML_EL_OpeningFence ||
      elType.ElTypeNum == MathML_EL_ClosingFence ||
      elType.ElTypeNum == MathML_EL_FencedSeparator)
    {
      SetIntAddSpaceAttr (el, doc);
      parent = TtaGetParent (el);
      if (parent != NULL)
        {
          parentType = TtaGetElementType (parent);
          if (parentType.ElTypeNum != MathML_EL_Base &&
              parentType.ElTypeNum != MathML_EL_UnderOverBase)
            SetIntVertStretchAttr (el, doc, 0, selEl);
          else
            {
              grandParent = TtaGetParent (parent);
              if (grandParent != NULL)
                {
                  SetIntVertStretchAttr (grandParent, doc, parentType.ElTypeNum,
                                         selEl);
                  if (elType.ElTypeNum == MathML_EL_MO &&
                      parentType.ElTypeNum == MathML_EL_UnderOverBase)
                    /* it's a MO element within a MunderOverBase, look at its
                       content to check if it's an operator with movable limits */
                    SetIntMovelimitsAttr (grandParent, doc);
                }
            }
        }
    }
  else
    /* it's not an operator (mo). Remove all attributes that can be set only
       on operators, except if it's a mstyle element. */
    if (elType.ElTypeNum == MathML_EL_MSTYLE)
      {
        RemoveAttr (el, doc, MathML_ATTR_IntAddSpace);
        RemoveAttr (el, doc, MathML_ATTR_form);
        RemoveAttr (el, doc, MathML_ATTR_fence);
        RemoveAttr (el, doc, MathML_ATTR_separator);
        RemoveAttr (el, doc, MathML_ATTR_lspace);
        RemoveAttr (el, doc, MathML_ATTR_rspace);
        RemoveAttr (el, doc, MathML_ATTR_separator);
        RemoveAttr (el, doc, MathML_ATTR_stretchy);
        RemoveAttr (el, doc, MathML_ATTR_symmetric);
        RemoveAttr (el, doc, MathML_ATTR_maxsize);
        RemoveAttr (el, doc, MathML_ATTR_minsize);
        RemoveAttr (el, doc, MathML_ATTR_largeop);
        RemoveAttr (el, doc, MathML_ATTR_movablelimits);
        RemoveAttr (el, doc, MathML_ATTR_accent);
      }
  if (elType.ElTypeNum == MathML_EL_MI)
    SetFontstyleAttr (el, doc);
  else
    RemoveAttr (el, doc, MathML_ATTR_IntFontstyle);		
}

/*----------------------------------------------------------------------
  SplitTextInMathML
  Split element el and the enclosing element (MO, MI, MN or MTEXT).
  Parameter index indicates the rank of the character before which the
  element must be cut.
  Return the text element created within the next enclosing element.
  ----------------------------------------------------------------------*/
static Element SplitTextInMathML (Document doc, Element el, int index,
                                  ThotBool *mrowCreated)
{
  Element            added, parent, row, leaf, prevLeaf, nextLeaf;
  ElementType        elType;
  AttributeType      attrType;
  Attribute	     attr;
  int                len;
  ThotBool           withinMrow, before, oldStructureChecking;

  /* do not check the Thot abstract tree against the structure schema while
     changing the structure */
  oldStructureChecking = TtaGetStructureChecking (doc);
  TtaSetStructureChecking (FALSE, doc);

  /* get the parent element (MO, MN, MI or MTEXT) */
  parent = TtaGetParent (el);

  len = TtaGetElementVolume (el);
  /* if it's a mchar, mglyph or an entity, don't split it */
  elType = TtaGetElementType (parent);
  if (elType.ElTypeNum == MathML_EL_MGLYPH)
    {
      el = parent;
      parent = TtaGetParent (el);
      if (index <= len)
        index = 0;
    }
  else
    /* is it an entity? */
    {
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = MathML_ATTR_EntityName;
      attr = TtaGetAttribute (el, attrType);
      if (attr != NULL)
        /* it's an entity, don't split it */
        {
          if (index <= len)
            index = 0;
        }
    }

  /* check if the parent is a child of a MROW or equivalent */
  withinMrow = ChildOfMRowOrInferred (parent);

  /* split the text element if needed */
  before = TRUE;
  if (index > len)
    /* split right after element el */
    before = FALSE;
  else
    if (index > 1)
      {
        if (withinMrow)
          TtaRegisterElementReplace (parent, doc);
        TtaSplitText (el, index, doc);
        /* split before the second part of the text */
        TtaNextSibling (&el);
      }

  /* should the enclosing element really be split? */
  if (before)
    {
      prevLeaf = el; TtaPreviousSibling (&prevLeaf);
      if (!prevLeaf)
        return (el);
    }
  else
    {
      nextLeaf = el; TtaNextSibling (&nextLeaf);
      if (nextLeaf)
        el = nextLeaf;
      else
        return (el);
    }

  *mrowCreated = FALSE;
  if (!withinMrow)
    {
      /* generate a new MROW element */
      elType.ElTypeNum = MathML_EL_MROW;
      row = TtaNewElement (doc, elType);
      TtaInsertSibling (row, parent, TRUE, doc);
      TtaRegisterElementCreate (row, doc);
      TtaRegisterElementDelete (parent, doc);
      TtaRemoveTree (parent, doc);
      /* move the parent into the new MROW */
      TtaInsertFirstChild (&parent, row, doc);
      /* the MROW element has been registered in the Undo queue. There is
         no need to register the elements that will be created within
         this MROW element */
      *mrowCreated = TRUE;
    }

  /* duplicate the parent element (MO, MN, MI or MTEXT) */
  elType = TtaGetElementType (parent);
  added = TtaNewElement (doc, elType);
  TtaInsertSibling (added, parent, FALSE, doc);

  /* move the second part of text (and its following siblings) into the
     duplicated parent */
  leaf = el;
  prevLeaf = NULL;
  do
    {
      nextLeaf = leaf;  TtaNextSibling (&nextLeaf);
      TtaRemoveTree (leaf, doc);
      if (!prevLeaf)
        TtaInsertFirstChild (&leaf, added, doc);
      else
        TtaInsertSibling (leaf, prevLeaf, FALSE, doc);
      prevLeaf = leaf;
      leaf = nextLeaf;
    }
  while (leaf);

  MathSetAttributes (parent, doc, NULL);
  MathSetAttributes (added, doc, NULL);
  if (withinMrow)
    TtaRegisterElementCreate (added, doc);
  /* resume structure checking */
  TtaSetStructureChecking (oldStructureChecking, doc);
  return (el);
}

/*----------------------------------------------------------------------
  DeleteIfPlaceholder
  
  Delete element el if it's a placeholder.
  ----------------------------------------------------------------------*/
static void DeleteIfPlaceholder (Element *el, Document doc, ThotBool record)
{
  Attribute	attr;
  ElementType	elType;
  AttributeType	attrType;

  if (*el == NULL)
    return;
  elType = TtaGetElementType (*el);
  if (elType.ElTypeNum == MathML_EL_Construct ||
      elType.ElTypeNum == MathML_EL_Construct1)
    {
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = MathML_ATTR_IntPlaceholder;
      attr = TtaGetAttribute (*el, attrType);
      if (attr != NULL || elType.ElTypeNum == MathML_EL_Construct1)
        /* this element is a placeholder. Delete it */
        {
          if (record)
            TtaRegisterElementDelete (*el, doc);
          TtaDeleteTree (*el, doc);
          *el = NULL;
        }
    }
}

/*----------------------------------------------------------------------
  InsertPlaceholder
  
  Return the new placeholder, if one has been created. Return NULL if
  no placeholder created.
  ----------------------------------------------------------------------*/
static Element InsertPlaceholder (Element el, ThotBool before, Document doc,
                                  ThotBool record)
{
  Element		sibling, placeholderEl;
  ElementType	elType;
  Attribute	attr;
  AttributeType	attrType;
  ThotBool	createConstruct, oldStructureChecking, afterMsubsup, ok;

  placeholderEl = NULL;

  if (!ElementNeedsPlaceholder (el))
    /* this element does not need placeholders.  Delete its previous
       and next siblings if they are place holders */
    {
      sibling = el;
      TtaPreviousSibling (&sibling);
      DeleteIfPlaceholder (&sibling, doc, record);
      sibling = el;
      TtaNextSibling (&sibling);
      DeleteIfPlaceholder (&sibling, doc, record);
    }
  else
    /* this element needs placeholders.  Create placeholders if the
       previous and/or next sibling are absent or need placeholders too */
    {
      createConstruct = TRUE;
      sibling = el;
      afterMsubsup = FALSE;
      if (before)
        {
          /* get the previous sibling that is not a comment */
          TtaPreviousSibling (&sibling);
          if (sibling)
            {
              ok = FALSE;
              do
                {
                  elType = TtaGetElementType (sibling);
                  if (elType.ElTypeNum == MathML_EL_XMLcomment &&
                      !strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
                    TtaPreviousSibling (&sibling);
                  else
                    ok = TRUE;
                }
              while (sibling && !ok);
            }
        }
      else
        {
          /* get the next sibling that is not a comment */
          TtaNextSibling (&sibling);
          if (sibling)
            {
              ok = FALSE;
              do
                {
                  elType = TtaGetElementType (sibling);
                  if (elType.ElTypeNum == MathML_EL_XMLcomment &&
                      !strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
                    TtaNextSibling (&sibling);
                  else
                    ok = TRUE;
                }
              while (sibling && !ok);
            }

          if (!sibling)
            /* the element has no following sibling */
            {
              elType = TtaGetElementType (el);
              if (elType.ElTypeNum == MathML_EL_MSUBSUP ||
                  elType.ElTypeNum == MathML_EL_MSUB ||
                  elType.ElTypeNum == MathML_EL_MSUP ||
                  elType.ElTypeNum == MathML_EL_MUNDEROVER ||
                  elType.ElTypeNum == MathML_EL_MUNDER ||
                  elType.ElTypeNum == MathML_EL_MOVER )
                /* the element is a msubsup, msub, msup, munderover, munder
                   or mover */
                {
                  attrType.AttrSSchema = elType.ElSSchema;
                  attrType.AttrTypeNum = MathML_ATTR_IntVertStretch;
                  if (TtaGetAttribute (el, attrType))
                    /* it has an attribute IntVertStretch */
                    /* the place holder has to be created after this element
                       that stretches vertically and that has no other
                       following sibling, to allow presentation rules to
                       operate correctly.
                       Create a Construct1 element */
                    afterMsubsup = TRUE;
                }
            }
        }
      if (sibling != NULL)
        if (!ElementNeedsPlaceholder (sibling))
          createConstruct = FALSE;
      if (createConstruct)
        {
          elType = TtaGetElementType (el);
          if (afterMsubsup)
            elType.ElTypeNum = MathML_EL_Construct1;
          else
            elType.ElTypeNum = MathML_EL_Construct;
          placeholderEl = TtaNewElement (doc, elType);
          /* do not check the Thot abstract tree against the structure */
          /* schema while inserting the Placeholder */
          oldStructureChecking = TtaGetStructureChecking (doc);
          TtaSetStructureChecking (FALSE, doc);
          TtaInsertSibling (placeholderEl, el, before, doc);
          /* resume structure checking */
          TtaSetStructureChecking (oldStructureChecking, doc);
          attrType.AttrSSchema = elType.ElSSchema;
          attrType.AttrTypeNum = MathML_ATTR_IntPlaceholder;
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (placeholderEl, attr, doc);
          TtaSetAttributeValue (attr, MathML_ATTR_IntPlaceholder_VAL_yes_,
                                placeholderEl, doc);
          if (record)
            TtaRegisterElementCreate (placeholderEl, doc);
        }
    }
  return placeholderEl;
}

/*----------------------------------------------------------------------
  CreateParentMROW
  If element el is not a child of a MROW or an equivalent construct
  and if it has at least one sibling that is not a Construct (place holder),
  create an enclosing MROW, 
  except if el is a child of a MFENCED element.
  ----------------------------------------------------------------------*/
static void CreateParentMROW (Element el, Document doc)
{
  Element            sibling, row, parent, firstChild, lastChild, next,
    previous;
  ElementType        elType;
  int                nChildren;
  ThotBool           oldStructureChecking;

  /* check whether the parent is a mrow or inferred mrow */
  parent = TtaGetParent (el);
  if (parent == NULL)
    return;
  if (!ChildOfMRowOrInferred (el))
    /* parent is not a mrow or inferred mrow */
    {
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum != MathML_EL_TEXT_UNIT &&
          elType.ElTypeNum != MathML_EL_SYMBOL_UNIT &&
          elType.ElTypeNum != MathML_EL_GRAPHICS_UNIT &&
          elType.ElTypeNum != MathML_EL_PICTURE_UNIT &&
          elType.ElTypeNum != MathML_EL_MathMLCharacters &&
          elType.ElTypeNum != MathML_EL_XMLcomment &&
          elType.ElTypeNum != MathML_EL_XMLcomment_line &&
          elType.ElTypeNum != MathML_EL_XMLPI &&
          elType.ElTypeNum != MathML_EL_XMLPI_line &&
          elType.ElTypeNum != MathML_EL_MathML &&
          elType.ElTypeNum != MathML_EL_Numerator &&
          elType.ElTypeNum != MathML_EL_Denominator &&
          elType.ElTypeNum != MathML_EL_SqrtBase &&
          elType.ElTypeNum != MathML_EL_RootBase &&
          elType.ElTypeNum != MathML_EL_Index &&
          elType.ElTypeNum != MathML_EL_FencedExpression &&
          elType.ElTypeNum != MathML_EL_Base &&
          elType.ElTypeNum != MathML_EL_Subscript &&
          elType.ElTypeNum != MathML_EL_Superscript &&
          elType.ElTypeNum != MathML_EL_UnderOverBase &&
          elType.ElTypeNum != MathML_EL_Underscript &&
          elType.ElTypeNum != MathML_EL_Overscript &&
          elType.ElTypeNum != MathML_EL_MultiscriptBase &&
          elType.ElTypeNum != MathML_EL_PostscriptPairs &&
          elType.ElTypeNum != MathML_EL_PostscriptPair &&
          elType.ElTypeNum != MathML_EL_MSubscript &&
          elType.ElTypeNum != MathML_EL_MSuperscript &&
          elType.ElTypeNum != MathML_EL_PrescriptPairs &&
          elType.ElTypeNum != MathML_EL_PrescriptPair &&
          elType.ElTypeNum != MathML_EL_MTable_head &&
          elType.ElTypeNum != MathML_EL_MColumn_head &&
          elType.ElTypeNum != MathML_EL_MC_Head &&
          elType.ElTypeNum != MathML_EL_C_Empty &&
          elType.ElTypeNum != MathML_EL_C_Space &&
          elType.ElTypeNum != MathML_EL_MTable_body &&
          elType.ElTypeNum != MathML_EL_TableRow &&
          elType.ElTypeNum != MathML_EL_MLABELEDTR &&
          elType.ElTypeNum != MathML_EL_MTR &&
          elType.ElTypeNum != MathML_EL_MTD &&
          elType.ElTypeNum != MathML_EL_RowLabel &&
          elType.ElTypeNum != MathML_EL_CellWrapper)
        /* element is not an intermediate Thot element nor a text leaf */
        {
          /* count the number of children of parent that are not placeholders
	     nor comments */
          sibling = TtaGetFirstChild (parent);
          nChildren = 0;
          firstChild = sibling;
          while (sibling != NULL)
            {
              elType = TtaGetElementType (sibling);
              if (elType.ElTypeNum != MathML_EL_Construct &&
                  elType.ElTypeNum != MathML_EL_Construct1 &&
		  elType.ElTypeNum != MathML_EL_XMLcomment)
                /* it's not a placeholder, count it */
                nChildren++;
              TtaNextSibling (&sibling);
            }
          if (nChildren > 1)
            {
              /* generate a new mrow element to include these elements */
              elType.ElTypeNum = MathML_EL_MROW;
              row = TtaNewElement (doc, elType);
              lastChild = TtaGetLastChild (parent);
              oldStructureChecking = TtaGetStructureChecking (doc);
              TtaSetStructureChecking (FALSE, doc);
              TtaInsertSibling (row, lastChild, FALSE, doc);
              TtaRegisterElementCreate (row, doc);
              sibling = firstChild;
              previous = NULL;
              while (sibling != NULL)
                {
                  next = sibling;
                  TtaNextSibling (&next);
                  TtaRegisterElementDelete (sibling, doc);
                  TtaRemoveTree (sibling, doc);
                  /* move the element into the new MROW */
                  if (previous == NULL)
                    TtaInsertFirstChild (&sibling, row, doc);
                  else
                    TtaInsertSibling (sibling, previous, FALSE, doc);
                  previous = sibling;
                  if (next == row)
                    sibling = NULL;
                  else
                    sibling = next;
                }
              /* resume structure checking */
              TtaSetStructureChecking (oldStructureChecking, doc);
            }
        }
    }
}

/*----------------------------------------------------------------------
  RegenerateFencedSeparators
  el must be a FencedExpression element.
  Delete all existing FencedSeparator elements in el and create new
  ones according to the value of attribute separators of parent MFENCED.
  -----------------------------------------------------------------------*/
static void RegenerateFencedSeparators (Element el, Document doc, ThotBool record)
{
  Element	child, next;
  ElementType	elType;

  /* Delete all existing FencedSeparator elements that are children of the
     FencedExpression element */
  child = TtaGetFirstChild (el);
  while (child != NULL)
    {
      next = child;
      TtaNextSibling (&next);
      elType = TtaGetElementType (child);
      if (elType.ElTypeNum == MathML_EL_FencedSeparator &&
          !strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
        {
          if (record)
            TtaRegisterElementDelete (child, doc);
          TtaDeleteTree (child, doc);
        }
      child = next;
    }
  /* Create all FencedSeparator elements, according to attribute separators */
  CreateFencedSeparators (el, doc, record);
}


/*----------------------------------------------------------------------
  InsertEmptyConstruct
  Insert an empty construct after an element
  ----------------------------------------------------------------------*/
void InsertEmptyConstruct (Element *el, int TypeNum, Document doc)
{
  ElementType newType;
  Element     empty;

  newType = TtaGetElementType (*el);
  newType.ElTypeNum = TypeNum;

  empty = TtaNewTree (doc, newType, "");
  TtaInsertSibling (empty, *el, FALSE, doc);
  *el = empty;
}

/* InsertNumber */
void InsertNumber (Element *el, int value, Document doc)
{
  unsigned short i;
  ElementType newType;
  Element     child, child2;
  char str[50];
  CHAR_T text[50];

  sprintf(str,"%d",value);
  for(i = 0; i < strlen(str); i++)text[i] = str[i];
  text[i] = EOS;

  newType = TtaGetElementType (*el);
  newType.ElTypeNum = MathML_EL_MN;

  child = TtaNewTree (doc, newType, "");
  TtaInsertSibling (child, *el, FALSE, doc);

  newType.ElTypeNum = MathML_EL_TEXT_UNIT;
  child2 = TtaNewElement (doc, newType);
  TtaInsertFirstChild (&child2, child, doc);
  TtaSetBufferContent (child2, (CHAR_T *)text, TtaGetLanguageIdFromScript('L'), doc);

  *el = child;
}


/*----------------------------------------------------------------------
  InsertSymbol
  Insert a symbol after an element
  ----------------------------------------------------------------------*/
void InsertSymbol (Element *el, int TypeNum, int symbol, Document doc)
{
  ElementType newType;
  Element op, child;
  CHAR_T text[2];

  newType = TtaGetElementType (*el);
  newType.ElTypeNum = TypeNum;

  op = TtaNewElement (doc, newType);
  TtaInsertSibling (op, *el, FALSE, doc);

  newType.ElTypeNum = MathML_EL_TEXT_UNIT;
  child = TtaNewElement (doc, newType);
  TtaInsertFirstChild (&child, op, doc);
  text[0] = symbol;
  text[1] = EOS;
  TtaSetBufferContent (child, text, TtaGetLanguageIdFromScript('L'), doc);
  *el = op;
}

/*----------------------------------------------------------------------
  InsertSymbolUnit
  Insert a symbol after an element
  ----------------------------------------------------------------------*/
void InsertSymbolUnit (Element *el, int TypeNum, int symbol, Document doc)
{
  ElementType newType;
  Element op, child;
  ThotBool OldStructureChecking = TtaGetStructureChecking (doc);

  newType = TtaGetElementType (*el);
  newType.ElTypeNum = TypeNum;

  op = TtaNewElement (doc, newType);
  TtaInsertSibling (op, *el, FALSE, doc);

  newType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
  child = TtaNewElement (doc, newType);
  TtaSetStructureChecking (FALSE, doc);
  TtaSetGraphicsShape (child, symbol, doc);
  TtaInsertFirstChild (&child, op, doc);
  TtaSetStructureChecking (OldStructureChecking, doc);
  *el = op;
}


/*----------------------------------------------------------------------
  AttachIntVertStretch
  attach an IntVertStretch attribute to the element el
  ----------------------------------------------------------------------*/
void AttachIntVertStretch(Element el, Document doc)
{
  ElementType elType = TtaGetElementType (el);
  Attribute          attr;
  AttributeType      attrType;
  ThotBool      oldStructureChecking = TtaGetStructureChecking (doc);

  TtaSetStructureChecking (FALSE, doc);

  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = MathML_ATTR_IntVertStretch;
  attr = TtaGetAttribute (el, attrType);
  if (!attr)
    {
      attrType.AttrTypeNum = MathML_ATTR_IntVertStretch;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
    }
  TtaSetAttributeValue (attr, MathML_ATTR_IntVertStretch_VAL_yes_, el, doc);
  TtaSetStructureChecking (oldStructureChecking, doc);
}

/*----------------------------------------------------------------------
  AttachIntHorizStretch
  attach an IntVertStretch attribute to the element el
  ----------------------------------------------------------------------*/
void AttachIntHorizStretch(Element el, Document doc)
{
  ElementType elType = TtaGetElementType (el);
  Attribute          attr;
  AttributeType      attrType;
  ThotBool      oldStructureChecking = TtaGetStructureChecking (doc);

  TtaSetStructureChecking (FALSE, doc);

  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = MathML_ATTR_IntHorizStretch;
  attr = TtaGetAttribute (el, attrType);
  if (!attr)
    {
      attrType.AttrTypeNum = MathML_ATTR_IntHorizStretch;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
    }
  TtaSetAttributeValue (attr, MathML_ATTR_IntHorizStretch_VAL_yes_, el, doc);
  TtaSetStructureChecking (oldStructureChecking, doc);
}


/*----------------------------------------------------------------------
  InsertText
  Insert a text after an element
  ----------------------------------------------------------------------*/
void InsertText (Element *el, int TypeNum, unsigned char *text, Document doc)
{
  ElementType newType;
  Element op, child;

  newType = TtaGetElementType (el[0]);
  newType.ElTypeNum = TypeNum;

  op = TtaNewElement (doc, newType);
  TtaInsertSibling(op, el[0], FALSE, doc);
  newType.ElTypeNum = MathML_EL_TEXT_UNIT;
  child = TtaNewElement (doc, newType);
  TtaInsertFirstChild (&child, op, doc);
  TtaInsertTextContent (child, 0, text, doc);

  el[0] = op;
}

/*----------------------------------------------------------------------
  CreateNewMtable
  Create a MTABLE element
  ----------------------------------------------------------------------*/
void CreateNewMtable (Element el, int NumberCols, int NumberRows, Document doc)
{
  ElementType      elType, newType;
  Element          newmtable, child, child2, row;
  int              col;

  newType = TtaGetElementType (el);
  elType.ElSSchema = newType.ElSSchema;

  if (newType.ElTypeNum != MathML_EL_MTABLE)
    {
      /* If the selection isn't in a MTABLE, create any */
      newType.ElTypeNum = MathML_EL_MTABLE;
      newmtable = TtaNewTree (doc, newType, "");
      TtaInsertFirstChild (&newmtable, el, doc);
    }
  else newmtable = el;

  CreateParentMROW (newmtable, doc);

  if (NumberRows > 1 || NumberCols >= 1)
    {
      /* create the required number of columns and rows in the table */
      if (NumberCols >= 1)
        {
          elType.ElTypeNum = MathML_EL_TableRow;
          child = TtaSearchTypedElement (elType, SearchInTree, newmtable);
          elType.ElTypeNum = MathML_EL_MTR;
          child2 = TtaNewTree (doc, elType, "");
          TtaInsertFirstChild (&child2, child, doc);
          elType.ElTypeNum = MathML_EL_MTD;
          child = TtaSearchTypedElement (elType, SearchInTree, newmtable);
          col = NumberCols;
          while (col > 1)
            {
              child2 = TtaNewTree (doc, elType, "");
              TtaInsertSibling (child2, child, FALSE, doc);
              col--;
            }
        }
      if (NumberRows > 1)
        {
          elType.ElTypeNum = MathML_EL_MTR;
          row = TtaSearchTypedElement (elType, SearchInTree, newmtable);
          while (NumberRows > 1)
            {
              elType.ElTypeNum = MathML_EL_MTR;
              child2 = TtaNewTree (doc, elType, "");
              TtaInsertSibling (child2, row, FALSE, doc);
              NumberRows--;
              /* create cells within the row */
              elType.ElTypeNum = MathML_EL_MTD;
              child = TtaSearchTypedElement (elType, SearchInTree,child2);
              col = NumberCols;
              while (col > 1)
                {
                  child2 = TtaNewTree (doc, elType, "");
                  TtaInsertSibling (child2, child, FALSE, doc);
                  col--;
                }
            }
        }
      CheckAllRows (newmtable, doc, FALSE, FALSE);
    }

}

/*----------------------------------------------------------------------
  SelectMtableCell
  Move the selection to the cell of coordonnates (x, y) 
  ----------------------------------------------------------------------*/
Element SelectMtableCell(Element mtable, int x, int y)
{
  int i;
  ElementType  elType;
  Element cell;

  /* Select the row */
  elType = TtaGetElementType (mtable);
  elType.ElTypeNum = MathML_EL_TableRow;
  cell = TtaSearchTypedElement (elType, SearchInTree, mtable);
  for(i = 0; i < y; i++)TtaNextSibling(&cell);

  /* Select the column */
  cell = TtaGetFirstChild(cell);
  for(i = 0; i < x; i++)TtaNextSibling(&cell);

  /* Select the MI element */
  cell = TtaGetFirstChild(cell);
  cell = TtaGetFirstChild(cell);
  return cell;
}

/*----------------------------------------------------------------------
  SetMFencedAttributes
  Set a fenced element and return the fenced element.
  ----------------------------------------------------------------------*/
Element SetMFencedAttributes(Element fenced, unsigned char ope, unsigned char clo, unsigned char sep, Document doc)
{
  ElementType elType;
  Element child, symb;
  AttributeType attrType;
  Attribute attr;
  char text[2];

  /* create open and close symbols */
  elType = TtaGetElementType (fenced);
  elType.ElTypeNum = MathML_EL_OpeningFence;
  child = TtaSearchTypedElement (elType, SearchInTree, fenced);
  symb = TtaGetFirstChild(child);
  TtaSetGraphicsShape (symb, ope, doc);

  elType = TtaGetElementType (fenced);
  elType.ElTypeNum = MathML_EL_ClosingFence;
  child = TtaSearchTypedElement (elType, SearchInTree, fenced);
  symb = TtaGetFirstChild(child);
  TtaSetGraphicsShape (symb, clo, doc);

  /* Attach open, close and separators attributes */
  attrType.AttrSSchema = elType.ElSSchema;

  if (ope != '(')
    {
      attrType.AttrTypeNum = MathML_ATTR_open;
      attr =  TtaNewAttribute (attrType);
      TtaAttachAttribute (fenced, attr, doc);

      text[0] = ope;
      text[1] = '\0';
      TtaSetAttributeText (attr, (char *)text, fenced, doc);
    }

  if (clo != ')')
    {
      attrType.AttrTypeNum = MathML_ATTR_close;
      attr =  TtaNewAttribute (attrType);
      TtaAttachAttribute (fenced, attr, doc);

      text[0] = clo;
      text[1] = '\0';
      TtaSetAttributeText (attr, (char *)text, fenced, doc);
    }

  if (sep != ',')
    {
      attrType.AttrTypeNum = MathML_ATTR_separators;
      attr =  TtaNewAttribute (attrType);
      TtaAttachAttribute (fenced, attr, doc);

      text[0] = sep;
      text[1] = '\0';
      TtaSetAttributeText (attr, (char *)text, fenced, doc);
    }

  /* return fenced expression */
  elType.ElTypeNum = MathML_EL_FencedExpression;
  return TtaSearchTypedElement (elType, SearchInTree, fenced);
}

/*----------------------------------------------------------------------
  InitializeNewConstruct
  A new MathML construct has just been created. Initilize it.
  ----------------------------------------------------------------------*/
static void InitializeNewConstruct (Element el, int NumberRows, int NumberCols,
                                    ThotBool history, Document doc)
{
  ElementType      elType, newType;
  Element          parent, placeholderEl;
  AttributeType    attrType;
  Attribute        attr;

  CreateParentMROW (el, doc);

  newType = TtaGetElementType (el);
  elType.ElSSchema = newType.ElSSchema;

  /* If the new element is a mtable, create this table */
  if (newType.ElTypeNum == MathML_EL_MTABLE)
    CreateNewMtable (el, NumberCols, NumberRows, doc);

  /* if the new element is a mspace, create a width attribute  with a default value */
  if (newType.ElTypeNum == MathML_EL_MSPACE)
    {
#define DEFAULT_MSPACE_WIDTH ".2em"
      attrType.AttrSSchema = newType.ElSSchema;
      attrType.AttrTypeNum = MathML_ATTR_width_;
      attr =  TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
      TtaSetAttributeText (attr, DEFAULT_MSPACE_WIDTH, el, doc);
      MathMLSpacingAttr (doc, el, DEFAULT_MSPACE_WIDTH, attrType.AttrTypeNum);
    }

  /* if the new element is a child of a FencedExpression element,
     create the associated FencedSeparator elements */
  parent = TtaGetParent (el);
  elType = TtaGetElementType (parent);
  if (elType.ElTypeNum == MathML_EL_FencedExpression &&
      elType.ElSSchema == newType.ElSSchema)
    RegenerateFencedSeparators (parent, doc, TRUE);

  /* insert placeholders before and/or after the new element if
     they are needed */
  placeholderEl = InsertPlaceholder (el, TRUE, doc, history);
  placeholderEl = InsertPlaceholder (el, FALSE, doc, history);
}


/*----------------------------------------------------------------------
  AppendEmptyText
  a new <math> element has just been created. If it's within a block
  element and there is no sibling, append an empty text element to
  make addition of new text easier.
  ----------------------------------------------------------------------*/
static Element AppendEmptyText (Element el, Document doc)
{
  Element      parent, emptyLeaf, sibling;
  ElementType  elType, parentType;

  emptyLeaf = NULL;
  elType = TtaGetElementType (el);
  parent = TtaGetParent (el);
  parentType = TtaGetElementType (parent);
  if (elType.ElSSchema == parentType.ElSSchema)
    /* it's the only <math> element in a MathML document */
    return (NULL);
  else
    /* it's a MathML expression within another vocabulary */
    {
      if (!strcmp (TtaGetSSchemaName (parentType.ElSSchema), "HTML"))
        /* a <math> element in a HTML element */
        {
          if (parentType.ElTypeNum == HTML_EL_Pseudo_paragraph ||
              parentType.ElTypeNum == HTML_EL_Paragraph ||
              parentType.ElTypeNum == HTML_EL_H1 ||
              parentType.ElTypeNum == HTML_EL_H2 ||
              parentType.ElTypeNum == HTML_EL_H3 ||
              parentType.ElTypeNum == HTML_EL_H4 ||
              parentType.ElTypeNum == HTML_EL_H5 ||
              parentType.ElTypeNum == HTML_EL_H6 ||
              parentType.ElTypeNum == HTML_EL_Address ||
              parentType.ElTypeNum == HTML_EL_Term ||
              parentType.ElTypeNum == HTML_EL_CAPTION)
            {
              elType.ElSSchema = parentType.ElSSchema;
              elType.ElTypeNum = MathML_EL_TEXT_UNIT;
              sibling = el;
              TtaPreviousSibling (&sibling);
              if (!sibling)
                {
                  emptyLeaf = TtaNewElement (doc, elType);
                  TtaInsertSibling (emptyLeaf, el, TRUE, doc);
                }
              sibling = el;
              TtaNextSibling (&sibling);
              if (!sibling)
                {
                  emptyLeaf = TtaNewElement (doc, elType);
                  TtaInsertSibling (emptyLeaf, el, FALSE, doc);
                }
            }
        }
    }
  return (emptyLeaf);
}

/*----------------------------------------------------------------------
  UnFrameMath
  -----------------------------------------------------------------------*/
void UnFrameMath ()
{
  ElementType      elType;
  Attribute        attr;
  AttributeType    attrType;

  if (MathElementSelected && DocMathElementSelected)
    /* remove attribute IntSelected from the previously selected <math> elem */
    {
      elType = TtaGetElementType (MathElementSelected);
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = MathML_ATTR_IntSelected;
      attr = TtaGetAttribute (MathElementSelected, attrType);
      if (attr)
        TtaRemoveAttribute (MathElementSelected, attr, DocMathElementSelected);
      MathElementSelected = NULL;
      DocMathElementSelected = 0;
    }
}

/*----------------------------------------------------------------------
  ClearMathFrame 
  -----------------------------------------------------------------------*/
void ClearMathFrame (Document doc)
{
  if (doc == DocMathElementSelected)
    {
      MathElementSelected = NULL;
      DocMathElementSelected = 0;
    }  
}

/*----------------------------------------------------------------------
  MathSelectionChanged
  A new element has been selected. Synchronize selection in source view.      
  ----------------------------------------------------------------------*/
void MathSelectionChanged (NotifyElement *event)
{
  Element          el, parent;
  Attribute        attr;
  ElementType      elType, parentType;
  AttributeType    attrType;
  ThotBool         drawFrame;

  CheckSynchronize (event);
  /* update the displayed style information */
  SynchronizeAppliedStyle (event);

  if (!TtaGetEnvBoolean ("ENABLE_MATHFRAME", &drawFrame))
    /* the environment variable is not defined. By default, draw a frame
       around the <math> elements that contain the current selection */
    drawFrame = TRUE;
  if (drawFrame)
    {
      el = event->element;
      elType = TtaGetElementType (el);
      if (strcmp (TtaGetSSchemaName (elType.ElSSchema),"MathML"))
        /* it's not a MathML element */
        UnFrameMath ();
      else
        {
          if (elType.ElTypeNum != MathML_EL_MathML)
            /* get the ancestor <math> element */
            {
              elType.ElTypeNum = MathML_EL_MathML;
              el = TtaGetTypedAncestor (el, elType);
            }
          if (el)
            {
	      /* if the ancestor <math> element is a child of an SVG element,
		 do not draw a frame */
	      parent = TtaGetParent (el);
	      if (parent)
		{
		  parentType = TtaGetElementType (parent);
		  if (!strcmp (TtaGetSSchemaName (parentType.ElSSchema),"SVG"))
		    /* the parent is an SVG element */
		    {
		      UnFrameMath ();
		      el = NULL;
		    }
		}
              /* if another formula is already highlighted, remove its frame
                 and draw a frame around the new one */
              if (el && el != MathElementSelected)
                {
                  UnFrameMath ();
                  /* associate an attribute IntSelected with the new <math>
                     element, except if it's a MathML document (an isolated
                     formula does not need a frame). */
                  if (DocumentTypes[event->document] != docMath)
                    {
                      attrType.AttrSSchema = elType.ElSSchema;
                      attrType.AttrTypeNum = MathML_ATTR_IntSelected;
                      attr = TtaNewAttribute (attrType);
                      if (attr)
                        {
                          TtaSetAttributeValue (attr, MathML_ATTR_IntSelected_VAL_Yes_,
                                                el, event->document);
                          TtaAttachAttribute (el, attr, event->document);
                        }
                      MathElementSelected = el;
                      DocMathElementSelected = event->document;
                    }
                }
            }
        }
    }
  UpdateXmlElementListTool (event->element,event->document);
  TtaSetStatusSelectedElement (event->document, 1, event->element);
#ifdef TEMPLATES
  if (!IsTemplateDocument (event->document))
#endif /* TEMPLATES */
    TtaRaiseDoctypePanels(WXAMAYA_DOCTYPE_MATHML);
}

/*----------------------------------------------------------------------
  GetIntegralType
  ----------------------------------------------------------------------*/
static void GetIntegralType(Document doc)
{
#ifdef _WX
  ThotBool  created;
  
  created = CreateSelectIntegralDlgWX (MathsDialogue + FormMathIntegral,
                                       TtaGetViewFrame (doc, 1));
  if (created)
    {
      TtaSetDialoguePosition ();
      TtaShowDialogue (MathsDialogue + FormMathIntegral, FALSE, TRUE);
      /* wait for an answer */
      TtaWaitShowDialogue ();
    }
#endif  /* _WX */
}

/*----------------------------------------------------------------------
  GetFenceAttributes
  ----------------------------------------------------------------------*/
static void GetFenceAttributes(Document doc)
{
#ifdef _WX
  ThotBool  created;
  
  created = CreateSelectFenceAttributesDlgWX(MathsDialogue + FormMathFenceAttributes,
                                             TtaGetViewFrame (doc, 1));
  if (created)
    {
      TtaSetDialoguePosition ();
      TtaShowDialogue (MathsDialogue + FormMathFenceAttributes, FALSE, TRUE);
      /* wait for an answer */
      TtaWaitShowDialogue ();
    }
#endif  /* _WX */
}

/*----------------------------------------------------------------------
  GetOccurrences: asks and returns the number of occurrences
  ----------------------------------------------------------------------*/
static int GetOccurrences(Document doc, char *label, int val, int mini)
{
#ifdef _WX
  ThotBool created;
  char *title = TtaGetMessage (AMAYA, AM_NUMBER_OCCUR);

  Math_occurences = val;
  created =  CreateNumDlgWX (MathsDialogue + FormMaths, 0,
                             TtaGetViewFrame (doc, 1),
                             title, label, val);
  if (created)
    {
      TtaSetDialoguePosition ();
      TtaShowDialogue (MathsDialogue + FormMaths, FALSE, TRUE);
      /* wait for an answer */
      TtaWaitShowDialogue ();
      val = Math_occurences;
    }
  else val = 0;
#endif  /* _WX */

/* check if val is more than mini */
if (val < mini && val > 0)val = mini;

return val;
}

/*----------------------------------------------------------------------
  GetOperatorType
  ----------------------------------------------------------------------*/
static int GetOperatorType(Document doc)
{
#ifdef _WX
  ThotBool  created;
  Math_OperatorType = 0;

  created = CreateSelectOperatorDlgWX(MathsDialogue + FormMathOperator,
                                      TtaGetViewFrame (doc, 1));
  if (created)
    {
      TtaSetDialoguePosition ();
      TtaShowDialogue (MathsDialogue + FormMathOperator, FALSE, TRUE);
      /* wait for an answer */
      TtaWaitShowDialogue ();
    }
  return Math_OperatorType;
#else  /* _WX */
  return 0;
#endif  /* _WX */
}

/*----------------------------------------------------------------------
  CreateMathConstruct
  Create a MathML construct at the current position
  According to the construct, other parameters can be sent to complete it. 
  ----------------------------------------------------------------------*/
static void CreateMathConstruct (Document doc, View view, int construct, ...)
{
  Element            sibling, el, row, child, child2, leaf, leaf2, next, foreignObj;
  Element            altText, nextToSelect, moveHere, op, previous, root;
  ElementType        newType, elType, parentType;
  Attribute          attr;
  AttributeType      attrType;
  NotifyElement      event;
  SSchema            docSchema, mathSchema;
  char              *name, *tmptext;
  unsigned char     *symbol_name = NULL;
  Language           lang;
  DisplayMode        dispMode;
  int                c1, i, len, profile, lx, ly;
  int                symbol, symbol2, symboltype;
  int                number, type, degree, degreevar, ibefore, par;
  va_list            varpos;
  ThotBool           oldStructureChecking = TRUE;
  ThotBool	         before, emptySel, ok, insertSibling, moveChild, isbinary;
  ThotBool           displayTableForm, registered, insertedMath = FALSE;
  
  if (doc == 0 || !TtaGetDocumentAccessMode (doc))
    {
      /* no selection. Nothing to do */
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_INSERT_POINT);
      return;
    }
  if (DocumentTypes[doc] == docSource ||
      DocumentTypes[doc] == docText || DocumentTypes[doc] == docLog)
    // cannot apply to a text file
    return;
  profile = TtaGetDocumentProfile (doc);
  if (profile == L_Strict || profile == L_Basic)
    {
      /* cannot insert here */
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NOT_ALLOWED);
      return;
    }
  else if (DocumentTypes[doc] != docMath && DocumentMeta[doc])
    {
      DocumentMeta[doc]->compound = TRUE;
      if (!DocumentMeta[doc]->xmlformat)
        {
          // the document becomes an XML document
          DocumentMeta[doc]->xmlformat = TRUE;
          root = TtaGetRootElement (doc);
          TtaSetANamespaceDeclaration (doc, root, NULL, XHTML_URI);
        }
    }
  op = NULL;
  docSchema = TtaGetDocumentSSchema (doc);
  TtaGiveFirstSelectedElement (doc, &sibling, &c1, &i); 
  emptySel = TtaIsSelectionEmpty ();
  /* Get the type of the first selected element */
  elType = TtaGetElementType (sibling);
  name = TtaGetSSchemaName (elType.ElSSchema);

#ifdef _SVG
  if(construct == 1 && !strcmp (name, "SVG"))
    construct = 11;
#endif /* _SVG */

    if (construct == 1)
      /* Math button */
      {
        if (strcmp (name, "MathML"))
          /* selection is not in a MathML element */
          {
            /* get the MathML schema associated with the current SSchema or
               associate it with the current SSchema if it is not associated
               yet */
            mathSchema = TtaNewNature (doc, elType.ElSSchema, NULL, "MathML","MathMLP");
            newType.ElTypeNum = MathML_EL_MathML;
            newType.ElSSchema = mathSchema;
            if (emptySel)
              /* the selected element is empty */
              {
                /* if the empty element is a HTML Block element, turn it into
                   a pseudo-paragraph, to allow a MathML element to be created
                   there */
                parentType = TtaGetElementType (sibling);
                if (parentType.ElTypeNum == HTML_EL_Block &&
                    !strcmp (TtaGetSSchemaName (parentType.ElSSchema), "HTML"))
                  /* it's a HTML Block element. Transform it */
                  {
                    dispMode = TtaGetDisplayMode (doc);
                    /* ask Thot to stop displaying changes made in the document*/
                    if (dispMode == DisplayImmediately)
                      TtaSetDisplayMode (doc, DeferredDisplay);
                    TtaOpenUndoSequence (doc, sibling, sibling, 0, 0);
                    parentType.ElTypeNum = HTML_EL_Pseudo_paragraph;
                    child = TtaNewElement (doc, parentType);
                    TtaRegisterElementDelete (sibling, doc);
                    TtaInsertFirstChild (&child, sibling, doc);
                    child2 = TtaNewElement (doc, newType);
                    TtaInsertFirstChild (&child2, child, doc);
                    SetDisplaystyleMathElement (child2, doc);
                    TtaRegisterElementCreate (child, doc);
                    leaf = AppendEmptyText (child2, doc);
                    if (leaf)
                      TtaRegisterElementCreate (leaf, doc);
                    TtaSetDocumentModified (doc);
                    TtaSetDisplayMode (doc, dispMode);
                    TtaSelectElement (doc, child2);
                    event.document = doc;
                    event.element = child2;
                    MathSelectionChanged (&event);
                    TtaCloseUndoSequence (doc);
                    return;
                  }
              }
            TtaCreateElement (newType, doc);

            /* Get the <math> element that has been created */
            TtaGiveFirstSelectedElement (doc, &el, &c1, &i);
            elType = TtaGetElementType (el);
            if (elType.ElTypeNum != MathML_EL_MathML)
              {
                elType.ElTypeNum = MathML_EL_MathML;
                el = TtaGetTypedAncestor (el, elType);
              }
            if (el)
              {
                /* associate an attribute IntDisplaystyle with the new
                   <math> element */
                SetDisplaystyleMathElement (el, doc);
                AppendEmptyText (el, doc);
                /* Set the MathML namespace declaration */
                elType = TtaGetElementType (el);
                TtaSetUriSSchema (elType.ElSSchema, MathML_URI);
                TtaSetANamespaceDeclaration (doc, el, NULL, MathML_URI);
              }
          }
        return;
      }

  dispMode = TtaGetDisplayMode (doc);
  /* ask Thot to stop displaying changes made in the document */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);
  
  /* By default, the new element will be inserted before the selected
     element */
  before = TRUE;

  TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
  registered = FALSE;
  
  /* Check whether the selected element is a MathML element */
  if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
    {
      /* current selection is within a MathML element */
      mathSchema = elType.ElSSchema;
      if (elType.ElTypeNum == MathML_EL_TEXT_UNIT ||
          elType.ElTypeNum == MathML_EL_SYMBOL_UNIT)
        /* the first selected element is a character string */
        {
          len = TtaGetElementVolume (sibling);
          if (c1 > len)
            /* the caret is at the end of that character string */
            {
              next = sibling;
              TtaNextSibling (&next);
              if (next)
                /* there is another character string after that one.
                   split the enclosing mo, mn, mi or mtext */
                sibling = SplitTextInMathML (doc, sibling, c1, &registered);
              else
                /* create the new element after the character string */
                before = FALSE;
            }
          else
            /* split the character string before the first selected char */
            sibling = SplitTextInMathML (doc, sibling, c1, &registered);
        }
    }
  else
    /* the selection is not in a MathML element */
    {
      ok = FALSE;
      if (elType.ElTypeNum == HTML_EL_TEXT_UNIT && c1 > 1)
        {
          len = TtaGetElementVolume (sibling);
          if (c1 > len)
            /* the caret is at the end of that character string */
            {
              /* the new element has to be created after the character
                 string */
              before = FALSE;
              el = sibling;
              TtaNextSibling (&el);
              if (el == NULL)
                /* the character string is the last child of its
                   parent */
                /* create an empty character string after the
                   Math element to come */
                {
                  el = TtaNewElement (doc, elType);
                  TtaInsertSibling (el, sibling, FALSE, doc);
                  TtaRegisterElementCreate (el, doc);
                }
            }
          else
            {
              /* split the text to insert the Math element */
              TtaRegisterElementReplace (sibling, doc);
              TtaSplitText (sibling, c1, doc);
              /* take the second part of the split text element */
              TtaNextSibling (&sibling);
              TtaRegisterElementCreate (sibling, doc);
            }
        }

      /* get the MathML schema associated with the current SSchema or
         associate it with the current SSchema if it is not associated yet */
      mathSchema = TtaNewNature (doc, elType.ElSSchema, NULL,  "MathML",
                                 "MathMLP");

      if (before)
        {
          el = sibling;
          TtaPreviousSibling (&el);
          if (el != NULL)
            {
              newType = TtaGetElementType (el);
              if (newType.ElTypeNum == MathML_EL_MathML &&
                  newType.ElSSchema == mathSchema)
                {
                  /* insert at the end of the previous MathML element*/
                  before = FALSE;
                  sibling = TtaGetLastChild (el);
                  emptySel = FALSE;
                  ok = TRUE;
                }
            }
        }
      else
        {
          el = sibling;
          TtaNextSibling (&el);
          if (el != NULL)
            {
              newType = TtaGetElementType (el);
              if (newType.ElTypeNum == MathML_EL_MathML &&
                  newType.ElSSchema == mathSchema)
                {
                  /* insert at the beginning of the next MathML element */
                  before = TRUE;
                  sibling = TtaGetFirstChild (el);
                  emptySel = FALSE;
                  ok = TRUE;
                }
            }
        }
      
      if (elType.ElSSchema == mathSchema &&
          elType.ElTypeNum == MathML_EL_MathML)
        /* the current selection is the MathML root element */
        {
          /* search the first or last child of the MathML root element */
          if (before)
            el = TtaGetFirstChild (sibling);
          else
            el = TtaGetLastChild (sibling);		
          if (el != NULL)
            sibling = el;
          ok = TRUE;
        }

      if (!ok)
        {
          insertSibling = TRUE;
          ok = TRUE;
          /* try to create a MathML root element at the current position */
          elType.ElSSchema = mathSchema;
          elType.ElTypeNum = MathML_EL_MathML;
          if (emptySel && !TtaIsLeaf (TtaGetElementType(sibling)))
            /* selection is empty and it's not a basic element */
            {
              /* if the empty element is a HTML Block element, turn it into
                 a pseudo-paragraph, to allow a MathML element to be created
                 there */
              parentType = TtaGetElementType (sibling);
              if (parentType.ElTypeNum == HTML_EL_Block &&
                  !strcmp (TtaGetSSchemaName (parentType.ElSSchema), "HTML"))
                /* it's a HTML Block element. Transform it */
                {
                  parentType.ElTypeNum = HTML_EL_Pseudo_paragraph;
                  child = TtaNewElement (doc, parentType);
                  TtaRegisterElementDelete (sibling, doc);
                  TtaInsertFirstChild (&child, sibling, doc);
                  TtaRegisterElementCreate (child, doc);
                  sibling = child;
                } 
              /* try first to create a new MathML element as a child */
              if (TtaCanInsertFirstChild (elType, sibling, doc))
                insertSibling = FALSE;
              /* if it fails, try to create a new MathML element as a
                 sibling */
              else if (TtaCanInsertSibling (elType, sibling, before, doc))
                insertSibling = TRUE;
              else
                /* complete failure */
                ok = FALSE;
            }
          else
            /* some non empty element is selected */
            {
              /* try first to create a new MathML element as a sibling */
              if (TtaCanInsertSibling (elType, sibling, before, doc))
                insertSibling = TRUE;
              /* if it fails, try to create a new MathML element as a
                 child*/
              else if (TtaCanInsertFirstChild (elType, sibling, doc))
                insertSibling = FALSE;
              else
                /* complete failure */
                ok = FALSE;
            }
          emptySel = TRUE;
          if (!ok)
            /* cannot insert a Math element here */
            {
#ifdef _SVG
              elType = TtaGetElementType (sibling);
              if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG"))
                /* selection is within a SVG element */
                {
		  if(view == 1)
		    {
		      /* We are in formatted view: call the SVG module
		       to ask where the user want to insert the foreignObject */
		      TtaCloseUndoSequence (doc);
		      TtaSelectElement(doc, sibling);
		      CreateGraphicElement (doc, view, 56);
		      TtaSetDisplayMode (doc, DisplayImmediately);
		      TtaGiveFirstSelectedElement (doc, &sibling, &c1, &i);
		      elType.ElSSchema = mathSchema;
		      elType.ElTypeNum = MathML_EL_MathML;
		      registered = TRUE;
		      insertSibling = FALSE;
		      TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
                      TtaSetDisplayMode (doc, DeferredDisplay);
		    }
		  else
		    {
		      elType.ElTypeNum = SVG_EL_switch;
		      if (TtaCanInsertSibling (elType, sibling, FALSE, doc))
			/* insert a switch element as a sibling */
			insertSibling = TRUE;
		      else
			{
			  child = TtaGetLastChild (sibling);
			  if (TtaCanInsertSibling (elType, child, FALSE, doc))
			    {
			      /* insert a switch element as a child */
			      sibling = child;
			      insertSibling = TRUE;
			    }
			  else
			    {
			      sibling = TtaGetParent (sibling);
			      if (TtaCanInsertSibling (elType, sibling, FALSE,doc))
				/* insert a switch element as a sibling of
				   the parent element */
				insertSibling = TRUE;
			      else
				sibling = NULL;
			    }
			}
		      if (sibling)
			{
			  /* create a switch element and insert it */
			  el = TtaNewElement (doc, elType);
			  TtaInsertSibling (el, sibling, FALSE, doc);
			  /* create a foreignObject element and insert it as
			     a child of the new switch element */
			  elType.ElTypeNum = SVG_EL_foreignObject;
			  //TtaAskFirstCreation ();
			  foreignObj = TtaNewElement (doc, elType);
			  TtaInsertFirstChild (&foreignObj, el, doc);
			  /* associate a requiredExtensions attribute with the
			     foreignObject element */
			  attrType.AttrSSchema = elType.ElSSchema;
			  attrType.AttrTypeNum = SVG_ATTR_requiredExtensions;
			  attr = TtaNewAttribute (attrType);
			  TtaAttachAttribute (foreignObj, attr, doc);
			  TtaSetAttributeText (attr, MathML_URI, foreignObj, doc);
			  /* create an alternate SVG text element for viewers
			     that can't display embedded MathML */
			  elType.ElTypeNum = SVG_EL_text_;
			  altText = TtaNewElement (doc, elType);
			  TtaInsertSibling (altText, foreignObj, FALSE, doc);
			  elType.ElTypeNum = SVG_EL_TEXT_UNIT;
			  leaf = TtaNewElement (doc, elType);
			  TtaInsertFirstChild (&leaf, altText, doc);
			  lang = TtaGetLanguageIdFromScript('L');
			  TtaSetTextContent (leaf, (unsigned char *)"embedded MathML not supported", lang, doc);
			  /* set the visibility of the alternate text */
			  EvaluateTestAttrs (el, doc);
			  /* update depth of SVG elements */
			  //SetGraphicDepths (doc, el);
			  /* register the switch element in the Undo queue*/
			  TtaRegisterElementCreate (el, doc);
			  registered = TRUE;
			  elType.ElSSchema = mathSchema;
			  elType.ElTypeNum = MathML_EL_MathML;
			  sibling = foreignObj;
			  insertSibling = FALSE;
			}
		    }
		}
              else
                /* not within a SVG element */
#endif /* _SVG */
                /* cannot insert any MathML element here */
                sibling = NULL;
            }
          if (sibling == NULL)
            /* cannot insert a math element here */
            {
              TtaSetDisplayMode (doc, dispMode);
              TtaCloseUndoSequence (doc);
              return;
            }
          else
            {
              /* create a Math element */
              el = TtaNewTree (doc, elType, "");
              /* do not check the Thot abstract tree against the structure/
                 schema while inserting the Math element */
              oldStructureChecking = TtaGetStructureChecking (doc);
              TtaSetStructureChecking (FALSE, doc);
              if (insertSibling)
                /* insert the new Math element as a sibling element */
                TtaInsertSibling (el, sibling, before, doc);
              else
                /* insert the new Math element as a child element */
                TtaInsertFirstChild (&el, sibling, doc);
              // a new Math element is now inserted
              insertedMath = TRUE;
              leaf = NULL;
              if (elType.ElTypeNum == MathML_EL_MathML)
                {
                  SetDisplaystyleMathElement (el, doc);
                  leaf = AppendEmptyText (el, doc);
                }
              /* Set the MathML namespace declaration */
              TtaSetUriSSchema (elType.ElSSchema, MathML_URI);
              TtaSetANamespaceDeclaration (doc, el, NULL, MathML_URI);
              /* restore structure checking mode */
              TtaSetStructureChecking (oldStructureChecking, doc);
              sibling = TtaGetFirstChild (el);
              /* register the new Math element in the Undo queue */
              if (!registered)
                {
                  TtaRegisterElementCreate (el, doc);
                  if (leaf)
                    TtaRegisterElementCreate (leaf, doc);
                }
              if (construct == 1)
                /* The <math> element requested is created. Return */
                {
                  TtaSetDocumentModified (doc);
                  TtaSetDisplayMode (doc, dispMode);
                  TtaSelectElement (doc, sibling);
                  event.document = doc;
                  event.element = sibling;
                  MathSelectionChanged (&event);
                  TtaCloseUndoSequence (doc);
                  return;
                }
            }
        }
    }
  /* Prepare to read other variables */
  va_start(varpos,construct);

  // Generate a new construct inside/before/after the sibling element
  elType = TtaGetElementType (sibling);
  newType.ElSSchema = mathSchema;

  switch (construct)
    {
    case 1:	/* create a Math element */
      /* handled above */
      break;
    case 2:
      newType.ElTypeNum = MathML_EL_MROOT;
      break;
    case 3:
      newType.ElTypeNum = MathML_EL_MSQRT;
      break;
    case 4:
      newType.ElTypeNum = MathML_EL_MFRAC;
      break;
    case 5:
      newType.ElTypeNum = MathML_EL_MSUBSUP;
      break;
    case 6:
      newType.ElTypeNum = MathML_EL_MSUB;
      break;
    case 7:
      newType.ElTypeNum = MathML_EL_MSUP;
      break;
    case 8:
      newType.ElTypeNum = MathML_EL_MUNDEROVER;
      break;
    case 9:
      newType.ElTypeNum = MathML_EL_MUNDER;
      break;
    case 10:
      newType.ElTypeNum = MathML_EL_MOVER;
      break;
    case 11: /* mrow */
      newType.ElTypeNum = MathML_EL_MROW;
      break;
    case 12:
      newType.ElTypeNum = MathML_EL_MMULTISCRIPTS;
      break;
    case 13:     /* MTABLE */
      displayTableForm = TtaIsSelectionEmpty ();
      if (displayTableForm)
        /* ask the user about the number of rows and columns to be created */
        {
          NumberCols = GetOccurrences (doc, TtaGetMessage (AMAYA, AM_COLS), 2, 1);
          if (NumberCols == 0)
            /* the user decided to abort the command */
            {
              TtaSetDisplayMode (doc, dispMode);
              TtaCloseUndoSequence (doc);
              if (insertedMath)
                TtaUndoNoRedo (doc);
              va_end(varpos);
              return;
            }

          NumberRows = GetOccurrences (doc, TtaGetMessage (AMAYA, AM_ROWS), 2, 1);
          if (NumberRows == 0)
            /* the user decided to abort the command */
            {
              TtaSetDisplayMode (doc, dispMode);
              TtaCloseUndoSequence (doc);
              if (insertedMath)
                TtaUndoNoRedo (doc);
              va_end(varpos);
              return;
            }
        }
      else
        {
          NumberRows = 0;
          NumberCols = 0;
        }
      
      newType.ElTypeNum = MathML_EL_MTABLE;
      break;

    case 14:
      newType.ElTypeNum = MathML_EL_MTEXT;
      break;
    case 15:
      newType.ElTypeNum = MathML_EL_MI;
      break;
    case 16:
      newType.ElTypeNum = MathML_EL_MN;
      break;
    case 17:
      newType.ElTypeNum = MathML_EL_MO;
      break;
      
    case 18:
      newType.ElTypeNum = MathML_EL_MENCLOSE;
      break;

    case 19:
      newType.ElTypeNum = MathML_EL_MSPACE;
      el = TtaNewTree (doc, newType, "");
      child = TtaGetFirstChild (el);
      break;

    case 61: /* mphantom */
      newType.ElTypeNum = MathML_EL_MPHANTOM;
      break;

    case 20: /* Integral msubsup; Integral sub */
    case 21: /* n-ary operation/relation */
    case 22: /* binary operation/relation */
    case 23: /* Unary operation/relation */
    case 24: /* Simple Symbol ; Simple Text */
    case 25: /* Piecewise ; VerticalBrace */
    case 27: /* Binary operation/relation */
    case 28: /* congru */
    case 29: /* forall, exists, exists2 */
    case 33: /* symbol with a under element */
    case 34: /* operator with an under element and an empty square after */
    case 35: /* n-ary operation/relation */
    case 39: /* parenthesis ; interval ; fence2 ; set/list extension */
    case 40: /* vertical fence */
    case 41: /* operator with a sub element and an empty square after (inf, max, min, sup, complementsub) */
    case 43: /* diagonalintersection ; limtendsto ; tendstotendsto */
    case 44: /* elementary classical functions */
    case 45: /* lambda construct */
    case 46: /* quotient */
    case 47: /* map */
    case 48: /* complexcartesian */
    case 54: /* curl ; div ; grad ; laplacian */
    case 55: /* variance */
    case 56: /* moment */
      newType.ElTypeNum = MathML_EL_MROW;
      break;  

    case 26: /* Accents */
    case 59: /* Accents with horizontal strech */
      newType.ElTypeNum = MathML_EL_MOVER;
      break;

    case 30: /* symbol in exposant */
    case 49: /* complexpolar */
      newType.ElTypeNum = MathML_EL_MSUP;
      break;

    case 31: /* symbol with a sub element */
    case 36: /* selector */
      newType.ElTypeNum = MathML_EL_MSUB;
      break;

    case 32: /* combination */
    case 37: /* couple,  n-uple */
    case 38: /* set/list separation */
    case 53: /* vectorrow ; vectorcolomn ; matrix ; determinant2 */
      newType.ElTypeNum = MathML_EL_MFENCED;
      break;

    case 60: /* underbrace */
      newType.ElTypeNum = MathML_EL_MUNDER;
      break;

    case 50: /* diff ; partialdiff */
    case 51: /* partialdiff2 */
    case 58: /* diff3 */
      newType.ElTypeNum = MathML_EL_MFRAC;
      break;

    case 57: /* transpose */
      newType.ElTypeNum = MathML_EL_MMULTISCRIPTS;
      break;

    default:
      TtaSetDisplayMode (doc, dispMode);
      TtaCloseUndoSequence (doc);
      if (insertedMath)
        TtaUndoNoRedo (doc);
      va_end(varpos);
      return;
    }
  
  moveChild = FALSE;
  moveHere = NULL;
  nextToSelect = NULL;

  if (!emptySel)
    /* selection is not empty.
       Try to transform it into the requested type */
    {
      if (!(construct < 20 || construct == 61))
        {
        emptySel = TRUE;
        moveChild = TRUE;
        }
      else
        {
          if (!TransformIntoType (&newType, doc))
            emptySel = TRUE; /* it failed. Try to insert a new element */
        }
    }


  if (emptySel)
    {
      TtaUnselect (doc);
      el = TtaNewTree (doc, newType, "");

      if(construct == 1 || (construct >= 3 && construct <= 10) || construct == 12)
        moveHere = TtaGetFirstChild(TtaGetFirstChild(el));

      if(construct == 2)
        moveHere = TtaGetFirstChild(TtaGetLastChild(el));

      if (construct == 4)
        {
          if (va_arg(varpos, int) == 1)
            {
              // generate the bevelled attribute
              attrType.AttrSSchema = elType.ElSSchema;
              attrType.AttrTypeNum = MathML_ATTR_bevelled;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (el, attr, doc);
              TtaSetAttributeValue (attr, MathML_ATTR_bevelled_VAL_true, el, doc);
              TtaChangeTypeOfElement (el, doc, MathML_EL_BevelledMFRAC);
            }
        }
      
      if(construct == 11 || (construct >= 14 && construct <= 19) || construct == 61)
        moveHere = TtaGetFirstChild(el);

      if (construct == 18)
        {
          if (va_arg(varpos, int) == 1)
            {/* actuarial */
              attrType.AttrSSchema = elType.ElSSchema;
              attrType.AttrTypeNum = MathML_ATTR_notation;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (el, attr, doc);
              TtaSetAttributeValue (attr, MathML_ATTR_notation_VAL_actuarial, el, doc);
            }
        }
      if (construct == 20)
        {
          /* Integral msubsup ; Integral sub ; Integral2 */
          symbol = va_arg(varpos, int);
          if (symbol == -1)
            {
            /* ask the type of integral */
            GetIntegralType(doc);
            if(Math_integral_number == 0)
              /* the user decided to abort the command */
              {
              TtaSetDisplayMode (doc, dispMode);
              TtaCloseUndoSequence (doc);
              if (insertedMath)
                TtaUndoNoRedo (doc);
              va_end(varpos);
              return;
              }

            if (!Math_integral_contour)
              {
              switch(Math_integral_number)
                {
                case 1: symbol = 8747; break;
                case 2: symbol = 8748; break;
                default: symbol = 8749; break;
                }
              }
            else
              {
              switch(Math_integral_number)
                {
                case 1: symbol = 8750; break;
                case 2: symbol = 8751; break;
                default: symbol = 8752; break;
                }
              }
            }
          else
            Math_integral_type = va_arg(varpos, int);

          leaf = TtaGetFirstChild (el);
          child = leaf;
          child2 = leaf;
          InsertEmptyConstruct (&child2, Math_integral_type ? MathML_EL_MSUBSUP : MathML_EL_MSUB, doc);
          TtaDeleteTree (leaf, doc);
          child = TtaGetFirstChild (child2);
   
          AttachIntVertStretch(child2, doc);
          nextToSelect = child;
          TtaNextSibling (&nextToSelect);
          nextToSelect = TtaGetFirstChild (nextToSelect);

          leaf = TtaGetFirstChild (child);
          op = leaf;
          InsertSymbol(&op, MathML_EL_MO, symbol, doc);
          TtaDeleteTree (leaf, doc);

          InsertEmptyConstruct (&child2, MathML_EL_MROW, doc);
          moveHere = TtaGetFirstChild(child2);
        }
      else if (construct == 21 || construct == 22)
        {
          /* n-ary operation/relation */
          type = va_arg(varpos, int);
          symbol = va_arg(varpos, int);
          if (symbol == 0)
            symbol_name = va_arg(varpos, unsigned char*);

          if (type == -1)
            {
            /* ask the user about the way the symbol has to be displayed */
            type = GetOperatorType(doc);
            if(type == 0)
              /* the user decided to abort the command */
              {
              TtaSetDisplayMode (doc, dispMode);
              TtaCloseUndoSequence (doc);
              if (insertedMath)
                TtaUndoNoRedo (doc);
              va_end(varpos);
              return;
              }
            else type--;

            /* replace the large operator by a small symbol if necessary */
            symbol2 = va_arg(varpos, int);
            if(type == 0)symbol = symbol2;
            }
       
          switch (type)
            {
            case 0:
              leaf = TtaGetFirstChild (el);
              child = leaf;
              InsertEmptyConstruct (&child, MathML_EL_MROW, doc);
              TtaDeleteTree (leaf, doc);
              moveHere = TtaGetFirstChild (child);
              /* ask how many the user wants */
              number = GetOccurrences (doc, TtaGetMessage (AMAYA, AM_NUMBER_OPERATORS), 5, 1);
              if(number == 0) 
                { /* the user decided to abort the command */
                TtaSetDisplayMode (doc, dispMode);
                TtaCloseUndoSequence (doc);
                if (insertedMath)
                  TtaUndoNoRedo (doc);
                va_end(varpos);
                return;
                }

              for (i = 0 ; i < number; i++)
                {
                  if (symbol == 0)
                    InsertText (&child, MathML_EL_MO, symbol_name, doc);
                  else
                    InsertSymbol (&child, MathML_EL_MO, symbol, doc);
                  InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
                }
            break;
            case 1:
              /* Operation on a family indexed by (i = ... to ...) */
              leaf = TtaGetFirstChild (el);
              child = leaf;
              InsertEmptyConstruct(&child, MathML_EL_MUNDEROVER, doc);
              child2 = child;
              InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
              moveHere = TtaGetFirstChild(child);
              TtaDeleteTree (leaf, doc);

              child2 = TtaGetFirstChild (child2);
              leaf = TtaGetFirstChild (child2);
              op = leaf;
              if (symbol == 0)
                InsertText (&op, MathML_EL_MO, symbol_name, doc);
              else
                InsertSymbol (&op, MathML_EL_MO, symbol, doc);
              TtaDeleteTree (leaf, doc);

              nextToSelect = child2;
              TtaNextSibling (&nextToSelect);
              nextToSelect = TtaGetFirstChild(nextToSelect);
              construct = 21;
            break;
            case 2:
              /* Operation on a family indexed by a set */
              leaf = TtaGetFirstChild (el);
              child = leaf;
              InsertEmptyConstruct (&child, MathML_EL_MUNDER, doc);
              child2 = child;
              InsertEmptyConstruct (&child, MathML_EL_MROW, doc);
              moveHere = TtaGetFirstChild(child);
              TtaDeleteTree (leaf, doc);

              child2 = TtaGetFirstChild (child2);
              leaf = TtaGetFirstChild (child2);
              op = leaf;
              if (symbol == 0)
                InsertText (&op, MathML_EL_MO, symbol_name, doc);
              else
                InsertSymbol (&op, MathML_EL_MO, symbol, doc);
              TtaDeleteTree (leaf, doc);

              nextToSelect = child2;
              TtaNextSibling (&nextToSelect);
              nextToSelect = TtaGetFirstChild(nextToSelect);
              construct = 21;
            break;
            }
        }
      else if (construct == 23)
        {
          /* Unary operation/relation */
          symbol = va_arg(varpos, int);
          leaf = TtaGetFirstChild (el);
          child = leaf;

          if (symbol != '!')
            {
              if (symbol == 0)
                InsertText (&child, MathML_EL_MO, va_arg(varpos, unsigned char*), doc);
              else 
                InsertSymbol (&child, MathML_EL_MO, symbol, doc);

              /* Union unary */
              if(symbol == 0x22C3)op = child;
            }

          par = va_arg(varpos, int);
          if (par)
            InsertSymbolUnit (&child, MathML_EL_MF, '(', doc);
 
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          moveHere = TtaGetFirstChild(child);
 
          if (par)
            InsertSymbolUnit (&child, MathML_EL_MF, ')', doc);

          if (symbol == '!')
             InsertSymbol (&child, MathML_EL_MO, symbol, doc);

          TtaDeleteTree (leaf, doc);
        }
      else if (construct == 24)
        {
          /* Simple Symbol ; Simple Text */
          symbol = va_arg(varpos, int);
          leaf = el;
          if (symbol == 0)
            InsertText (&el, MathML_EL_MI, va_arg(varpos, unsigned char*), doc);
          else
            InsertSymbol (&el, MathML_EL_MI, symbol, doc);
          TtaDeleteTree (leaf, doc);
          
          /* create a caret */
          newType.ElTypeNum = MathML_EL_TEXT_UNIT;
          child = TtaSearchTypedElement (newType, SearchInTree, el);
          len = TtaGetElementVolume (child);
          TtaSelectString (doc, child, len + 1, len);
          moveHere = NULL;
        }
      else if (construct == 25)
        {
          /* Piecewise ; VerticalBrace */

          /* ask how many the user wants */
          number = GetOccurrences (doc, TtaGetMessage (AMAYA, AM_ROWS), 3, 2);
          if(number == 0)
            {/* the user decided to abort the command */
            TtaSetDisplayMode (doc, dispMode);
            TtaCloseUndoSequence (doc);
            if (insertedMath)
              TtaUndoNoRedo (doc);
            va_end(varpos);
            return;
            }
          leaf = TtaGetFirstChild (el);
          child = leaf;

          InsertSymbolUnit (&child, MathML_EL_MF, '{', doc);
          AttachIntVertStretch (child, doc);

          InsertEmptyConstruct (&child, MathML_EL_MROW, doc);
          TtaDeleteTree (leaf, doc);

          leaf = TtaGetFirstChild (child);TtaDeleteTree (leaf, doc);
          CreateNewMtable (child, 1, number, doc);
          moveHere = SelectMtableCell(child, 0, 0);
        } 
      else if (construct == 26 || construct == 30 || construct == 31)
        {
          /* Accents ; Exposant ; Symbol sub */
          symboltype = (construct == 26 ? MathML_EL_MO : va_arg(varpos, int));
          symbol = va_arg(varpos, int);
          if (construct == 31)
            {
            child = TtaGetFirstChild (el);
            moveHere = TtaGetFirstChild(TtaGetLastChild (el));
            }
          else
            {
            moveHere = TtaGetFirstChild(TtaGetFirstChild (el));
            child = TtaGetLastChild (el);
            }

          leaf = TtaGetFirstChild (child);
          child = leaf;
          if (symbol <=0)
            InsertText (&child, symboltype, va_arg(varpos, unsigned char*), doc);
          else
            InsertSymbol (&child, symboltype, symbol, doc);
        
          TtaDeleteTree (leaf, doc);
        }

      else if (construct == 33)
        {
          /* Symbol under */
          isbinary = (va_arg(varpos, int) == 1);
          symboltype = va_arg(varpos, int);
          symbol = va_arg(varpos, int);

          leaf = TtaGetFirstChild (el); 
          child = leaf;
          if(isbinary)
            {
            InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
            moveHere = TtaGetFirstChild(child);
            }
            
          InsertEmptyConstruct(&child, MathML_EL_MUNDER, doc);
          child2 = child;

          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          if(!isbinary)moveHere = TtaGetFirstChild(child);
          TtaDeleteTree (leaf, doc);

          child = TtaGetFirstChild (child2); 
          if(!isbinary)nextToSelect = TtaGetFirstChild(TtaGetLastChild (child2));

          if (symbol == 'R')
            {/* tendsto */
            AttachIntHorizStretch(child,doc);
            leaf = TtaGetFirstChild (child);
            child = leaf;
            InsertSymbolUnit (&child, symboltype, symbol, doc); 
            AttachIntHorizStretch(child,doc);
            }
          else
            {
            leaf = TtaGetFirstChild (child);
            child = leaf;
            if (symbol <=0)
             InsertText (&child, symboltype, va_arg(varpos, unsigned char*), doc);
            else
              InsertSymbol (&child, symboltype, symbol, doc);
            }
         
          TtaDeleteTree (leaf, doc);
        }
      else if (construct == 27)
        {
          /* Binary operation/relation */
          symbol = va_arg(varpos, int);
          leaf = TtaGetFirstChild (el);
          child = leaf;
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          moveHere = TtaGetFirstChild(child);

          if (symbol == 0)
            InsertText (&child, MathML_EL_MO, va_arg(varpos, unsigned char*), doc);
          else
            InsertSymbol (&child, MathML_EL_MO, symbol, doc);
        
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          TtaDeleteTree (leaf, doc);
        }
      else if (construct == 28)
        {
          /* Congru */
          leaf = TtaGetFirstChild (el);
          child = leaf;
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          moveHere = TtaGetFirstChild(child);
          InsertSymbol (&child, MathML_EL_MO, 8801, doc);
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          InsertSymbolUnit (&child, MathML_EL_MF, '(', doc);
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          InsertSymbolUnit (&child, MathML_EL_MF, ')', doc);
          TtaDeleteTree (leaf, doc);
        }
      else if (construct == 29)
        {
          /* Forall, exists */
          leaf = TtaGetFirstChild (el);
          child = leaf;
          InsertSymbol (&child, MathML_EL_MO, va_arg(varpos, int), doc);
          if (va_arg(varpos, int))
            {
            InsertSymbol (&child, MathML_EL_MO, '!', doc);
            }
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          moveHere = TtaGetFirstChild(child);
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          TtaDeleteTree (leaf, doc);
        }
      else if (construct == 32)
      {/* combination */
          child = SetMFencedAttributes(el, '(', ')', ',', doc);

          leaf = TtaGetFirstChild (child); 
          child = leaf;
          InsertEmptyConstruct (&child, MathML_EL_MFRAC, doc);
          attrType.AttrSSchema = elType.ElSSchema;
          attrType.AttrTypeNum = MathML_ATTR_linethickness;
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (child, attr, doc);
          TtaSetAttributeText (attr, "0", child, doc);
          tmptext = TtaStrdup("0");
          MathMLlinethickness (doc, child, tmptext);
          TtaFreeMemory(tmptext);
        
          moveHere = TtaGetFirstChild (TtaGetFirstChild (child));
          TtaDeleteTree (leaf, doc);
        }
      else if (construct == 34 || construct == 41)
        {
          /* operator with an under element and an empty square after (inf, max, min, sup, complementsub) */
          leaf = TtaGetFirstChild (el);
          child = leaf;
          InsertEmptyConstruct (&child, (construct == 34 ? MathML_EL_MUNDER : MathML_EL_MSUB), doc);

          TtaDeleteTree (leaf, doc);

          child2 = TtaGetFirstChild (child); 
          nextToSelect = TtaGetFirstChild (TtaGetLastChild(child));

          leaf = TtaGetFirstChild (child2);
          child2 = leaf;
          if (construct == 34)
            InsertText (&child2, MathML_EL_MO, va_arg(varpos, unsigned char*), doc);
          else
            InsertSymbol (&child2, MathML_EL_MO, va_arg(varpos, int), doc);
          TtaDeleteTree (leaf, doc);

          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          moveHere = TtaGetFirstChild(child);
        }
      else if (construct == 36)
        {/* selector */

          /* ask number of coordonnates */
          number = GetOccurrences (doc, TtaGetMessage (AMAYA, AM_NUMBER_COORDONNATES), 2, 1);
          if(number == 0)
            { /* the user decided to abort the command */
            TtaSetDisplayMode (doc, dispMode);
            TtaCloseUndoSequence (doc);
            if (insertedMath)
              TtaUndoNoRedo (doc);
            va_end(varpos);
            return;
            }

          moveHere = TtaGetFirstChild(TtaGetFirstChild(el));

          child = TtaGetLastChild (el);
          leaf = TtaGetFirstChild (child);
          child = leaf;
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          TtaDeleteTree (leaf, doc);

          leaf = TtaGetFirstChild (child);
          child = leaf;
          for (i = 0 ; i < number; i++)
            {
              if (i)
                InsertSymbol (&child, MathML_EL_MO, 8291, doc); // invisible comma          
              InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
            }
          TtaDeleteTree (leaf, doc);
        }
      else if (construct == 37)
        {/* couple,  n-uple */
          number = va_arg(varpos, int);
          if (number != 2)
            {/* ask how many the user wants */
              number = GetOccurrences (doc,  TtaGetMessage (AMAYA, AM_NUMBER_NUPLE), 3, 2);
              if(number == 0)
                {  /* the user decided to abort the command */
                TtaSetDisplayMode (doc, dispMode);
                TtaCloseUndoSequence (doc);
                if (insertedMath)
                  TtaUndoNoRedo (doc);
                va_end(varpos);
                return;
                }
            }

          child2 = SetMFencedAttributes(el, '(', ')', ',', doc);
          leaf = TtaGetFirstChild (child2);
          child = leaf;
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          TtaDeleteTree (leaf, doc);
          moveHere = TtaGetFirstChild(child);
          for (i = 1 ; i < number; i++)
            InsertEmptyConstruct(&child, MathML_EL_MROW, doc);

          CreateFencedSeparators (child2, doc, FALSE);
        }
      else if (construct == 38)
        {/* set/list separation */
          Math_open = va_arg(varpos, int);
          Math_close = va_arg(varpos, int);
          child2 = SetMFencedAttributes(el, Math_open, Math_close, '|', doc);
          leaf = TtaGetFirstChild (child2);
          child = leaf;
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          TtaDeleteTree (leaf, doc);
          moveHere = TtaGetFirstChild(child);
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);

          CreateFencedSeparators (child2, doc, FALSE);
        }
      else if (construct == 39)
        {/* parenthesis ; interval ; fence2 ; set/list extension */
          Math_open = va_arg(varpos, int);
          Math_close = va_arg(varpos, int);
          Math_sep = va_arg(varpos, int);
          number = va_arg(varpos, int);
          if (Math_open == -1 || Math_close == -1 || Math_sep == -1)
            {
            /* get types of open/close symbols selected in the panel */
            Math_open = '(';
            Math_close = ')';
            Math_sep = ',';
            GetFenceAttributes (doc);
            if (Math_open == 0)
              {  /* the user decided to abort the command */
              TtaSetDisplayMode (doc, dispMode);
              TtaCloseUndoSequence (doc);
              if (insertedMath)
                TtaUndoNoRedo (doc);
              va_end(varpos);
              return;
              }
            }


          if (number == -1)
            {
            /* ask how many the user wants */
            number = GetOccurrences (doc,  TtaGetMessage (AMAYA, AM_NUMBER_ELEMENTS), 5, 1);
            if(number == 0)
              {  /* the user decided to abort the command */
              TtaSetDisplayMode (doc, dispMode);
              TtaCloseUndoSequence (doc);
              if (insertedMath)
                TtaUndoNoRedo (doc);
              va_end(varpos);
              return;
              }
            }

          leaf = TtaGetFirstChild (el);
          child = leaf;
          InsertSymbolUnit (&child, MathML_EL_MF, Math_open, doc);
          TtaDeleteTree (leaf, doc);
          
          for (i = 0 ; i < number; i++)
            {
            if (i)
              InsertSymbol (&child, MathML_EL_MO, Math_sep, doc);
            InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
            if (!i)
              moveHere = TtaGetFirstChild (child);
            }
          InsertSymbolUnit (&child, MathML_EL_MF, Math_close, doc);
        }
      else if (construct == 40)
        {/* vertical fence */
          Math_open = va_arg(varpos, int);
          Math_close = va_arg(varpos, int);
          leaf = TtaGetFirstChild (el);
          child = leaf;
          if(va_arg(varpos, int))
            {/* stretchy = true */
            InsertSymbolUnit (&child, MathML_EL_MO, Math_open, doc);
            //AttachIntVertStretch(child, doc);
            InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
            moveHere = TtaGetFirstChild(child);
            InsertSymbolUnit (&child, MathML_EL_MO, Math_close, doc);
            //AttachIntVertStretch(child, doc);
            TtaDeleteTree (leaf, doc);
            }
          else
            {/* stretchy = false */
            InsertSymbol (&child, MathML_EL_MO, Math_open, doc);
            TtaDeleteTree (leaf, doc);
            InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
            moveHere = TtaGetFirstChild(child);
            InsertSymbol (&child, MathML_EL_MO, Math_close, doc);
            }
        }
      else if (construct == 43)
        {
          /* diagonalintersection ; limtendsto ; tendstotendsto */
          isbinary = (va_arg(varpos, int) == 1);
          symbol = va_arg(varpos, int);
          symbol2 = va_arg(varpos, int);
          
          leaf = TtaGetFirstChild (el); 
          child = leaf;
          if(isbinary)
            {
            InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
            moveHere = TtaGetFirstChild(child);
            }
            
          InsertEmptyConstruct(&child, MathML_EL_MUNDER, doc);
          child2 = child;

          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          if(!isbinary)moveHere = TtaGetFirstChild(child);
          TtaDeleteTree (leaf, doc);

          /* First element of the MUNDER */
          child = TtaGetFirstChild (child2); 

          if (symbol == 'R')
            AttachIntHorizStretch(child,doc);
          leaf = TtaGetFirstChild (child);
          child = leaf;

          if (symbol == 0)
            InsertText (&child, MathML_EL_MO, va_arg(varpos, unsigned char*), doc);
          else if (symbol == 'R')
            {
            InsertSymbolUnit (&child, MathML_EL_MO, symbol, doc);
            AttachIntHorizStretch(child,doc);
            }
          else
            InsertSymbol (&child, MathML_EL_MO, symbol, doc);

          TtaDeleteTree (leaf, doc);

          /* Second element of the MUNDER */
          child = TtaGetLastChild (child2); 
          leaf = TtaGetFirstChild (child);
          child = leaf;
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          TtaDeleteTree (leaf, doc);

          leaf = TtaGetFirstChild (child);
          child = leaf;
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          if(!isbinary)nextToSelect = TtaGetFirstChild(child);
          if (symbol2 == 0)
            InsertText (&child, MathML_EL_MO, va_arg(varpos, unsigned char*), doc);
          else
            InsertSymbol (&child, MathML_EL_MO, symbol2, doc);
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);

          TtaDeleteTree (leaf, doc);
        }
      else if (construct == 44)
        { /* elementary classical functions */
          leaf = TtaGetFirstChild (el);
          child = leaf;
          InsertText (&child, MathML_EL_MI, va_arg(varpos, unsigned char*), doc);
          InsertSymbol (&child, MathML_EL_MO, 8289, doc); // apply function 
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          moveHere = TtaGetFirstChild(child);
          TtaDeleteTree (leaf, doc);
        }
      else if (construct == 45)
        { /* lambda constuct */

          /* ask the number of variables */
          number = GetOccurrences (doc, TtaGetMessage (AMAYA, AM_NUMBER_VARIABLES), 2, 1);
          if(number == 0)
            {  /* the user decided to abort the command */
            TtaSetDisplayMode (doc, dispMode);
            TtaCloseUndoSequence (doc);
            if (insertedMath)
              TtaUndoNoRedo (doc);
            va_end(varpos);
            return;
            }
          leaf = TtaGetFirstChild (el);
          child = leaf;
          InsertSymbol(&child, MathML_EL_MI, 955, doc);// lambda
          InsertEmptyConstruct(&child, MathML_EL_MFENCED, doc);
          TtaDeleteTree (leaf, doc);

          child2 = SetMFencedAttributes(child, '(', ')', ',', doc);
          leaf = TtaGetFirstChild (child2);
          child = leaf;
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          TtaDeleteTree (leaf, doc);
          moveHere = TtaGetFirstChild(child);
          for (i = 1 ; i <= number; i++)
            InsertEmptyConstruct(&child, MathML_EL_MROW, doc);

          CreateFencedSeparators (child2, doc, FALSE);
        }
      else if (construct == 46)
        { /* quotient */
          leaf = TtaGetFirstChild (el);
          child = leaf;
          InsertSymbolUnit (&child, MathML_EL_MO, 3, doc);
          //AttachIntVertStretch(child, doc);
          InsertEmptyConstruct(&child, MathML_EL_MFRAC, doc);
          moveHere = TtaGetFirstChild(TtaGetFirstChild(child));
          InsertSymbolUnit (&child, MathML_EL_MO, 4, doc);
          //AttachIntVertStretch(child, doc);

          TtaDeleteTree (leaf, doc);
        }
      else if (construct == 47)
        { /* map construct */

          /* a blank for the name of the function */
          leaf = TtaGetFirstChild (el);
          child = leaf;
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          moveHere = TtaGetFirstChild(child);
          TtaDeleteTree (leaf, doc);

          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          leaf = TtaGetFirstChild (child);TtaDeleteTree (leaf, doc);
          InsertSymbolUnit (&child, MathML_EL_MO, '|', doc);
          AttachIntVertStretch(child, doc);
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          leaf = TtaGetFirstChild (child);TtaDeleteTree (leaf, doc);
          CreateNewMtable (child, 1, 2, doc);

          /* first arrow */ 
          child2 = child;
          leaf = SelectMtableCell(child2, 0, 0);
          child = leaf;
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          InsertSymbol (&child, MathML_EL_MO, 8594, doc);
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          TtaDeleteTree (leaf, doc);

          /* second arrow */
          leaf = SelectMtableCell(child2, 0, 1);
          child = leaf;
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          InsertSymbol (&child, MathML_EL_MO, 8614, doc);
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          TtaDeleteTree (leaf, doc);
        }
      else if (construct == 48)
        { /* complexcartesian */
          ibefore = va_arg(varpos, int); 
          leaf = TtaGetFirstChild (el);
          child = leaf;

          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          moveHere = TtaGetFirstChild(child);
          TtaDeleteTree (leaf, doc);

          InsertSymbol(&child, MathML_EL_MO, '+', doc);
          if (ibefore)
            {
              InsertSymbol(&child, MathML_EL_MI, 8520, doc); // i
              InsertSymbol (&child, MathML_EL_MO, 8290, doc);// invisible times
              InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
            }
          else
            {
              InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
              InsertSymbol (&child, MathML_EL_MO, 8290, doc);// invisible times
              InsertSymbol(&child, MathML_EL_MI, 8520, doc); // i
            }
        }
      else if (construct == 49)
        { /* complexpolar */
          child2 = TtaGetFirstChild (el);
          leaf = TtaGetFirstChild (child2);
          child = leaf;
          InsertSymbol(&child, MathML_EL_MI, 8519, doc); // e
          TtaDeleteTree (leaf, doc);

          child2 = TtaGetLastChild (el);
          leaf = TtaGetFirstChild(child2);
          child = leaf;
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          TtaDeleteTree (leaf, doc);

          leaf = TtaGetFirstChild(child);
          child = leaf;
          InsertSymbol(&child, MathML_EL_MI, 8520, doc); // i
          InsertSymbol (&child, MathML_EL_MO, 8290, doc);// invisible times
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          moveHere = TtaGetFirstChild(child);
          TtaDeleteTree (leaf, doc);
        }
      else if (construct == 50)
        { /* diff ; partialdiff */
          symbol = va_arg(varpos, int);
          leaf = TtaGetFirstChild (TtaGetFirstChild (el));
          child = leaf;
          InsertEmptyConstruct (&child, MathML_EL_MROW, doc);
          TtaDeleteTree (leaf, doc);

          leaf = TtaGetFirstChild(child);
          child = leaf;
          InsertSymbol (&child, MathML_EL_MO, symbol, doc);
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          moveHere = TtaGetFirstChild(child);
          TtaDeleteTree (leaf, doc);

          leaf = TtaGetFirstChild (TtaGetLastChild (el));
          child = leaf;
          InsertEmptyConstruct (&child, MathML_EL_MROW, doc);
          TtaDeleteTree (leaf, doc);

          leaf = TtaGetFirstChild(child);
          child = leaf;
          InsertSymbol (&child, MathML_EL_MO, symbol, doc);
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          TtaDeleteTree (leaf, doc);
        }
      else if (construct== 51)
        { /* partialdiff2 */
          symbol2 = 8706;
          degree = 0;
          /* ask the user about the number of variables that have to be differentiated */
          number = GetOccurrences (doc, TtaGetMessage (AMAYA, AM_NUMBER_VARIABLES), 2, 1);
          if(number == 0)
            {  /* the user decided to abort the command */
            TtaSetDisplayMode (doc, dispMode);
            TtaCloseUndoSequence (doc);
            if (insertedMath)
              TtaUndoNoRedo (doc);
            va_end(varpos);
            return;
            }

          /* Denominator */
          leaf = TtaGetFirstChild (TtaGetLastChild (el));
          child = leaf;
          InsertEmptyConstruct (&child, MathML_EL_MROW, doc);
          TtaDeleteTree (leaf, doc);

          leaf = TtaGetFirstChild(child);
          child = leaf;

          tmptext = (char *)TtaGetMemory (50);

          for(i = 0; i < number; i++)
             {
             /* ask the user about the degree of derivation of each variable */
             sprintf (tmptext, TtaGetMessage (AMAYA, AM_DEGREE_VARIABLE), i + 1);
             degreevar = GetOccurrences (doc, tmptext, 1, 1);

             if(degreevar == 0)
               {  /* the user decided to abort the command */
               TtaFreeMemory (tmptext);
               TtaSetDisplayMode (doc, dispMode);
               TtaCloseUndoSequence (doc);
               if (insertedMath)
                 TtaUndoNoRedo (doc);
               va_end(varpos);
               return;
               }

             degree += degreevar;
             if (degreevar == 1)
               {
               InsertSymbol (&child, MathML_EL_MO, symbol2, doc);
               }
             else
               {
               InsertEmptyConstruct(&child, MathML_EL_MSUP, doc);

               leaf2 = TtaGetFirstChild (TtaGetFirstChild (child));
               child2 = leaf2;
               InsertSymbol (&child2, MathML_EL_MO, symbol2, doc);
               TtaDeleteTree (leaf2, doc);

               leaf2 = TtaGetFirstChild (TtaGetLastChild (child));
               child2 = leaf2;
               InsertNumber (&child2, degreevar, doc);
               TtaDeleteTree (leaf2, doc);
               }
             InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
             }

          TtaFreeMemory (tmptext);
          TtaDeleteTree (leaf, doc);

          /* Numerator */
          leaf = TtaGetFirstChild (TtaGetFirstChild (el));
          child = leaf;
          InsertEmptyConstruct (&child, MathML_EL_MROW, doc);
          TtaDeleteTree (leaf, doc);

          leaf = TtaGetFirstChild(child);
          child = leaf;

          if (degree == 1)
            {          
            InsertSymbol (&child, MathML_EL_MO, symbol2, doc);
            }
          else
            {
            InsertEmptyConstruct(&child, MathML_EL_MSUP, doc);

            leaf2 = TtaGetFirstChild (TtaGetFirstChild (child));
            child2 = leaf2;
            InsertSymbol (&child2, MathML_EL_MO, symbol2, doc);
            TtaDeleteTree (leaf2, doc);

            leaf2 = TtaGetFirstChild (TtaGetLastChild (child));
            child2 = leaf2;
            InsertNumber (&child2, degree, doc);
            TtaDeleteTree (leaf2, doc);
            }

          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          moveHere = TtaGetFirstChild(child);
          TtaDeleteTree (leaf, doc);
        }
      else if (construct== 53)
        { /* vectorrow ; vectorcolomn ; matrix ; determinant2 */
          Math_open = va_arg(varpos, int);
          Math_close = va_arg(varpos, int);
          lx = va_arg(varpos, int);
          ly = va_arg(varpos, int);
          child = SetMFencedAttributes(el, Math_open, Math_close, ',', doc);
        
          /* ask the user the number of rows and colomns */
          if(lx == -1 || ly == -1)
            {/* lx = ly */
            lx = GetOccurrences (doc, TtaGetMessage (AMAYA, AM_COLS), 3, 1);
            if(lx == 0)
              {  /* the user decided to abort the command */
              TtaSetDisplayMode (doc, dispMode);
              TtaCloseUndoSequence (doc);
              if (insertedMath)
                TtaUndoNoRedo (doc);
              va_end(varpos);
              return;
              }
            ly = lx;
            }
          else
            {
            if (lx == 0)
              {
              lx = GetOccurrences (doc, TtaGetMessage (AMAYA, AM_COLS), 3, 1);
              if(lx == 0)
                {  /* the user decided to abort the command */
                TtaSetDisplayMode (doc, dispMode);
                TtaCloseUndoSequence (doc);
                if (insertedMath)
                  TtaUndoNoRedo (doc);
                va_end(varpos);
               return;
                }
              }
  
            if (ly == 0)
              {
              ly = GetOccurrences (doc, TtaGetMessage (AMAYA, AM_ROWS), 3, 1);
              if(ly == 0)
                {  /* the user decided to abort the command */
                TtaSetDisplayMode (doc, dispMode);
                TtaCloseUndoSequence (doc);
                if (insertedMath)
                  TtaUndoNoRedo (doc);
                va_end(varpos);
               return;
                }
              }
            }

          /* mtable */
          leaf = TtaGetFirstChild (child);TtaDeleteTree (leaf, doc);
          CreateNewMtable (child, lx, ly, doc);
          moveHere = SelectMtableCell(child, 0, 0);
        }
      else if (construct == 54)
        {/* curl ; div ; grad ; laplacian */
          symbol2 = 8711;
          symbol = va_arg(varpos, int);
          leaf = TtaGetFirstChild (el);
          child = leaf;
          if (symbol == 1)
            {/* Laplacian */
              InsertEmptyConstruct(&child, MathML_EL_MSUP, doc);

              leaf2 = TtaGetFirstChild (TtaGetFirstChild (child));
              child2 = leaf2;
              InsertSymbol (&child2, MathML_EL_MO, symbol2, doc);
              TtaDeleteTree (leaf2, doc);
          
              leaf2 = TtaGetFirstChild(TtaGetLastChild (child));
              child2 = leaf2;
              InsertSymbol (&child2, MathML_EL_MN, '2', doc);
              TtaDeleteTree (leaf2, doc);
            }
          else
            {/* curl ; div ; grad */
              InsertSymbol (&child, MathML_EL_MO, symbol2, doc);
              if (symbol != 0)
                InsertSymbol (&child, MathML_EL_MO, symbol, doc);
            }

          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          moveHere = TtaGetFirstChild(child);
          TtaDeleteTree (leaf, doc);
        }
      else if (construct == 55)
        {/* variance */
          leaf = TtaGetFirstChild (el);
          child = leaf;
          InsertSymbol (&child, MathML_EL_MO, 963, doc);
          InsertEmptyConstruct(&child, MathML_EL_MSUP, doc);
          TtaDeleteTree (leaf, doc);

          {
            leaf = TtaGetFirstChild (TtaGetFirstChild (child));
            child2 = leaf;
            InsertEmptyConstruct(&child2, MathML_EL_MROW, doc);
            TtaDeleteTree (leaf, doc);

            leaf = TtaGetFirstChild (child2);
            child2 = leaf; 
            InsertSymbolUnit (&child2, MathML_EL_MF, '(', doc);
            InsertEmptyConstruct(&child2, MathML_EL_MROW, doc);
            moveHere = TtaGetFirstChild(child2);
            InsertSymbolUnit (&child2, MathML_EL_MF, ')', doc);
            TtaDeleteTree (leaf, doc);

            leaf = TtaGetFirstChild(TtaGetLastChild (child));
            child2 = leaf;
            InsertSymbol (&child2, MathML_EL_MN, '2', doc);
            TtaDeleteTree (leaf, doc);
          }
        
        }
      else if (construct == 56)
        {/* moment */
          leaf = TtaGetFirstChild (el);
          child = leaf;
          InsertSymbol (&child, MathML_EL_MO, 9001, doc);
          InsertEmptyConstruct(&child, MathML_EL_MSUP, doc);
          moveHere = TtaGetFirstChild(TtaGetFirstChild(child));
          InsertSymbol (&child, MathML_EL_MO, 9002, doc);
          TtaDeleteTree (leaf, doc);
        }
      else if (construct == 57)
        {
        /* transpose */
          newType.ElTypeNum = MathML_EL_PrescriptPair;
          child = TtaSearchTypedElement (newType, SearchInTree, el);

          //TtaDeleteTree (TtaGetFirstChild (TtaGetFirstChild (child)), doc);
          leaf = TtaGetFirstChild (TtaGetLastChild (child));
          child2 = leaf;
          InsertSymbol (&child2, MathML_EL_MO, 't', doc);
          TtaDeleteTree (leaf, doc);

          newType.ElTypeNum = MathML_EL_PostscriptPair;
          child = TtaSearchTypedElement (newType, SearchInTree, el);
          //TtaDeleteTree (TtaGetFirstChild (TtaGetFirstChild (child)), doc);
          //TtaDeleteTree (TtaGetFirstChild (TtaGetLastChild (child)), doc);

          moveHere = TtaGetFirstChild (TtaGetFirstChild (el));
        }
      else if (construct == 58)
        {/* diffential */
          /* ask the user about the degree of diffentiation */
          symbol2 = 8518;
          degree = GetOccurrences (doc, TtaGetMessage (AMAYA, AM_DEGREE), 2, 0);
          if(degree == 0)
            {  /* the user decided to abort the command */
            TtaSetDisplayMode (doc, dispMode);
            TtaCloseUndoSequence (doc);
            if (insertedMath)
              TtaUndoNoRedo (doc);
            va_end(varpos);
            return;
            }

          /* Numerator */
          leaf = TtaGetFirstChild (TtaGetFirstChild (el));
          child = leaf;
          InsertEmptyConstruct (&child, MathML_EL_MROW, doc);
          TtaDeleteTree (leaf, doc);

          leaf = TtaGetFirstChild(child);
          child = leaf;

          if (degree == 1)
            InsertSymbol (&child, MathML_EL_MO, symbol2, doc);
          else
            {
            InsertEmptyConstruct(&child, MathML_EL_MSUP, doc);

            leaf2 = TtaGetFirstChild (TtaGetFirstChild (child));
            child2 = leaf2;
            InsertSymbol (&child2, MathML_EL_MO, symbol2, doc);
            TtaDeleteTree (leaf2, doc);

            leaf2 = TtaGetFirstChild (TtaGetLastChild (child));
            child2 = leaf2;
            InsertNumber (&child2, degree, doc);
            TtaDeleteTree (leaf2, doc);
            }

          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          moveHere = TtaGetFirstChild(child);
          TtaDeleteTree (leaf, doc);

          /* Denominator */
          leaf = TtaGetFirstChild (TtaGetLastChild (el));
          child = leaf;
          InsertEmptyConstruct (&child, MathML_EL_MROW, doc);
          TtaDeleteTree (leaf, doc);

          leaf = TtaGetFirstChild(child);
          child = leaf;

          InsertSymbol (&child, MathML_EL_MO, symbol2, doc);
          InsertEmptyConstruct(&child, MathML_EL_MROW, doc);
          TtaDeleteTree (leaf, doc);
        }
      else if (construct == 59 || construct == 60)
        {/* accents with a horizontal strech */
        symbol = va_arg(varpos, int);
        child = TtaGetLastChild(el);
        AttachIntHorizStretch(child,doc);

        leaf = TtaGetFirstChild (child);
        child = leaf;
        InsertSymbolUnit (&child, MathML_EL_MO, symbol, doc);
        AttachIntHorizStretch(child,doc);
        TtaDeleteTree (leaf, doc);
        moveHere = TtaGetFirstChild (TtaGetFirstChild (el));
        }

      /* do not check the Thot abstract tree against the structure */
      /* schema while changing the structure */
      oldStructureChecking = TtaGetStructureChecking (doc);
      TtaSetStructureChecking (FALSE, doc);

      if ((elType.ElTypeNum == MathML_EL_MROW ||
           elType.ElTypeNum == MathML_EL_MathML) &&
          elType.ElSSchema == mathSchema)
        {
          /* the selected element is a MROW or the MathML element */
          row = sibling;
          if (before)
            sibling = TtaGetFirstChild (row);
          else
            sibling = TtaGetLastChild (row);
          
          if (sibling == NULL)
            {
              if (elType.ElTypeNum == MathML_EL_MathML)
                /* empty MATH element. Insert the new element as a child */
                /* moveChild must be FALSE  */
                {
                  TtaInsertFirstChild (&el, row, doc);
                  if (!registered)
                    TtaRegisterElementCreate (el, doc);
                }
              else
                {
                  /* replace the empty MROW element by the new element */
                  /* moveChild must be FALSE  */
                  TtaInsertSibling (el, row, TRUE, doc);

                  if (!registered)
                    {
                      TtaRegisterElementCreate (el, doc);
                      TtaRegisterElementDelete (row, doc);
                    }
                  TtaDeleteTree (row, doc);
                }
            }
          else
            {
              /* check whether the selected element is a Construct */
              parentType = TtaGetElementType (sibling);
              if (elType.ElTypeNum == MathML_EL_MROW &&
                  (parentType.ElTypeNum == MathML_EL_Construct ||
                   parentType.ElTypeNum == MathML_EL_Construct1) &&
                  parentType.ElSSchema == mathSchema)
                {
                  next = sibling;
                  previous = sibling;
                  TtaNextSibling (&next);
                  TtaPreviousSibling (&previous);
                  if (next == NULL && previous == NULL)sibling = row;

                  if(moveChild && moveHere)
                    {
                    sibling = row;
                    child = TtaCopyTree (sibling, doc, doc, TtaGetParent (sibling));
                    TtaInsertSibling(child, moveHere, FALSE, doc);
                    TtaDeleteTree (moveHere, doc);
                    if(nextToSelect == NULL)nextToSelect = child;
                    }                    

                  TtaInsertSibling (el, sibling, before, doc);
                  if (!registered)
                    {
                      TtaRegisterElementCreate (el, doc);
                      TtaRegisterElementDelete (sibling, doc);
                    }
                 TtaDeleteTree (sibling, doc);
                }
              else
                {
                if(moveChild && moveHere)
                  {
                  /* Move the row, except if it is actually a MathML root */
                  if(elType.ElTypeNum != MathML_EL_MathML)sibling = row;

                  child = TtaCopyTree (sibling, doc, doc, TtaGetParent (sibling));
                  TtaInsertSibling (child, moveHere, FALSE, doc);
                  TtaDeleteTree (moveHere, doc);
                  if(nextToSelect == NULL)nextToSelect = child;
                  TtaInsertSibling (el, sibling, before, doc);

                  if (!registered)
                     {
                     TtaRegisterElementCreate (el, doc);
                     TtaRegisterElementDelete (sibling, doc);
                     }

                    TtaDeleteTree (sibling, doc);
                  }                    
                else
                  { /* Insert el inside row */
                  TtaInsertSibling (el, sibling, before, doc);
                 
                  if (!registered)
                    TtaRegisterElementCreate (el, doc);
                  }
                }
            }
        }
      else if ((elType.ElTypeNum == MathML_EL_Construct ||
                elType.ElTypeNum == MathML_EL_Construct1) &&
               elType.ElSSchema == mathSchema)
        {
          next = sibling;
          previous = sibling;
          TtaNextSibling (&next);
          TtaPreviousSibling (&previous);
          if (next == NULL && previous == NULL)
            {
              row = TtaGetParent (sibling);
              parentType = TtaGetElementType (row);
              if (parentType.ElTypeNum == MathML_EL_MROW &&
                  parentType.ElSSchema == mathSchema)
                sibling = row;
            }
          /* replace the Construct element */
          if(moveChild && moveHere)
            {
            child = TtaCopyTree (sibling, doc, doc, TtaGetParent (sibling));
            TtaInsertSibling (child, moveHere, FALSE, doc);
            TtaDeleteTree (moveHere, doc);
            if(nextToSelect == NULL)nextToSelect = child;
            }

          TtaInsertSibling (el, sibling, FALSE, doc);
          if (!registered)
            {
              TtaRegisterElementCreate (el, doc);
              TtaRegisterElementDelete (sibling, doc);
            }
          TtaDeleteTree (sibling, doc);
        }
      else
        {
         /* The selected element is a cell. Select its content. */
          if(elType.ElTypeNum == MathML_EL_MTD)
            sibling = TtaGetFirstChild(TtaGetFirstChild(sibling));

         /* The selected element is a row. Select its parent mtable. */
          if( elType.ElTypeNum == MathML_EL_MTR ||
              elType.ElTypeNum == MathML_EL_MLABELEDTR)
            sibling = TtaGetParent(TtaGetParent(sibling));

          /* the selected element is not a MROW */
          if (elType.ElTypeNum == MathML_EL_TEXT_UNIT ||
              elType.ElTypeNum == MathML_EL_SYMBOL_UNIT)
            /* go up to the MN, MI, MO or M_TEXT element */
            sibling = TtaGetParent (sibling);

           /* insert the new element and move sibling if necessary */
           if(moveChild && moveHere)
             {
             child = TtaCopyTree (sibling, doc, doc, TtaGetParent (sibling));
             TtaInsertSibling (child, moveHere, FALSE, doc);
             TtaDeleteTree (moveHere, doc);
             if(nextToSelect == NULL)nextToSelect = child;
             TtaInsertSibling (el, sibling, before, doc);
 
             if (!registered)
                {
                TtaRegisterElementCreate (el, doc);
                TtaRegisterElementDelete (sibling, doc);
                }
 
             TtaDeleteTree (sibling, doc);
             }                    
           else
             {
             TtaInsertSibling (el, sibling, before, doc);
             if (!registered)
                TtaRegisterElementCreate (el, doc);
             }
        }
      
      if (op)
        {
          MathSetAttributes (op, doc, NULL);
          /* enlarge the symbol according to the context */
          CheckLargeOp (op, doc);
        }

      if (construct == 20 || construct == 21)
        {
          /* move the limits of the MSUBSUP element if it's appropriate */
          SetIntMovelimitsAttr (el, doc);
        }
      /* For construct 13, the cells are created here, so moveChild is supposed to be FALSE */
      InitializeNewConstruct (el, NumberRows, NumberCols, !registered, doc);
      if (construct == 13)
        nextToSelect = SelectMtableCell(el, 0, 0);
 
      /* Take the placeholder moveHere as a default value for nextToSelect */
      if (nextToSelect == NULL)
        nextToSelect = moveHere;
 	  
      if (newType.ElTypeNum == MathML_EL_MSPACE ||
          elType.ElTypeNum == MathML_EL_MGLYPH ||
          elType.ElTypeNum == MathML_EL_MALIGNMARK ||
          elType.ElTypeNum == MathML_EL_MALIGNGROUP)
        /* select the new element itself */
        {
          TtaSelectElement (doc, el);
          nextToSelect = el;
        }
      else
        {
        if(nextToSelect)
          /* select the nextToSelect  */
          TtaSelectElement (doc, nextToSelect);
        }
    }

  if (nextToSelect)
    {
      event.document = doc;
      event.element = nextToSelect;
      MathSelectionChanged (&event);
    }

  TtaSetDisplayMode (doc, dispMode);
  TtaSetStructureChecking (oldStructureChecking, doc);

  TtaSetDocumentModified (doc);
  TtaCloseUndoSequence (doc);
  va_end(varpos);
}

/*----------------------------------------------------------------------
  MathElementCreated
  A new element has just been created by the user with a generic editing
  command.
  -----------------------------------------------------------------------*/
void MathElementCreated (NotifyElement *event)
{
  InitializeNewConstruct (event->element, 2, 2, TRUE, event->document);
  // it's a compound document
  if (DocumentMeta[event->document])
    DocumentMeta[event->document]->compound = TRUE;
}


/*----------------------------------------------------------------------
  CallbackMaths: manage Maths dialogue events.
  ----------------------------------------------------------------------*/
static void CallbackMaths (int ref, int typedata, char *data)
{
  long int           val = (long int) data;

  ref -= MathsDialogue;
  switch (ref)
    {
    case FormMaths:
      Math_occurences = val;
      break;

    case FormMathOperator:
      Math_OperatorType = val;
      break;

    case MathAttributeOpen:
      Math_open = val;
      break;

    case MathAttributeSeparators:
      Math_sep = val;
      break;

    case MathAttributeClose:
      Math_close = val;
      break;

    case MathIntegralNumber:
      Math_integral_number = val;
      break;

    case MathIntegralContour:
      Math_integral_contour = val;
      break;

    case MathIntegralType:
      Math_integral_type = val;
      break;

    case FormMathIntegral:
      if(val == 0)Math_integral_number = 0;
      break;

    case FormMathFenceAttributes:
      if(val == 0)Math_open = 0;

    default:
      break;
    }
}

/*----------------------------------------------------------------------
  SetOnOffChemistry
  ----------------------------------------------------------------------*/
void SetOnOffChemistry(Document document, View view)
{
  if(CurrentMathEditMode == CHEMISTRY_MODE)
    CurrentMathEditMode = DEFAULT_MODE;
  else
    CurrentMathEditMode = CHEMISTRY_MODE;
}

/*----------------------------------------------------------------------
  CreateMath
  ----------------------------------------------------------------------*/
void CreateMath (Document document, View view)
{
  CreateMathConstruct (document, view,  1);
}

/*----------------------------------------------------------------------
  CreateMROOT
  ----------------------------------------------------------------------*/
void CreateMROOT (Document document, View view)
{
  CreateMathConstruct (document, view,  2);
}

/*----------------------------------------------------------------------
  CreateMSQRT
  ----------------------------------------------------------------------*/
void CreateMSQRT (Document document, View view)
{
  CreateMathConstruct (document, view,  3);
}

/*----------------------------------------------------------------------
  CreateMENCLOSE
  ----------------------------------------------------------------------*/
void CreateMENCLOSE (Document document, View view)
{
  CreateMathConstruct (document, view,  18,0);
}

/*----------------------------------------------------------------------
  CreateMENCLOSE
  ----------------------------------------------------------------------*/
void CreateMENCLOSE2 (Document document, View view)
{
  CreateMathConstruct (document, view,  18,1);
}
/*----------------------------------------------------------------------
  CreateMENCLOSE
  ----------------------------------------------------------------------*/
void CreateMPHANTOM (Document document, View view)
{
  CreateMathConstruct (document, view,  61);
}

/*----------------------------------------------------------------------
  CreateMFRAC
  ----------------------------------------------------------------------*/
void CreateMFRAC (Document document, View view)
{
  CreateMathConstruct (document, view,  4, 0);
}

/*----------------------------------------------------------------------
  CreateMLFRAC
  ----------------------------------------------------------------------*/
void CreateMLFRAC (Document document, View view)
{
  CreateMathConstruct (document, view,  4, 1);
}

/*----------------------------------------------------------------------
  CreateMSUBSUP
  ----------------------------------------------------------------------*/
void CreateMSUBSUP (Document document, View view)
{
  CreateMathConstruct (document, view,  5);
}

/*----------------------------------------------------------------------
  CreateMSUB
  ----------------------------------------------------------------------*/
void CreateMSUB (Document document, View view)
{
  CreateMathConstruct (document, view,  6);
}

/*----------------------------------------------------------------------
  CreateMSUP
  ----------------------------------------------------------------------*/
void CreateMSUP (Document document, View view)
{
  CreateMathConstruct (document, view,  7);
}

/*----------------------------------------------------------------------
  CreateMUNDEROVER
  ----------------------------------------------------------------------*/
void CreateMUNDEROVER (Document document, View view)
{
  CreateMathConstruct (document, view,  8);
}

/*----------------------------------------------------------------------
  CreateMUNDER
  ----------------------------------------------------------------------*/
void CreateMUNDER (Document document, View view)
{
  CreateMathConstruct (document, view,  9);
}

/*----------------------------------------------------------------------
  CreateMOVER
  ----------------------------------------------------------------------*/
void CreateMOVER (Document document, View view)
{
  CreateMathConstruct (document, view,  10);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateMOVERARROW (Document document, View view)
{
  CreateMathConstruct (document, view,  59, 'R');
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateMOVERBRACE (Document document, View view)
{
  CreateMathConstruct (document, view,  59, 'o');
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateMUNDERBRACE (Document document, View view)
{
  CreateMathConstruct (document, view,  60, 'u');
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateMOVERBAR (Document document, View view)
{
  CreateMathConstruct (document, view,  59, 'h');
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateMOVERBREVE (Document document, View view)
{
  CreateMathConstruct (document, view,  26, 728);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateMOVERCHECK (Document document, View view) // hacek
{
  CreateMathConstruct (document, view,  59, 'k');
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateMOVERDOT (Document document, View view)
{
  CreateMathConstruct (document, view,  26, 729);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateMOVERHAT (Document document, View view)
{
  CreateMathConstruct (document, view,  59, 'H');
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateMOVERTILDE (Document document, View view)
{
  /* Unicode Character 'TILDE' (U+007E) */
  CreateMathConstruct (document, view,  59, 'T');
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateMOVERFROWN (Document document, View view) // overparenthesis
{
  CreateMathConstruct (document, view,  59, 'p');
}

/*----------------------------------------------------------------------
  CreateMPARENTHESIS
  ----------------------------------------------------------------------*/
void CreateMPARENTHESIS (Document document, View view)
{
  CreateMathConstruct (document, view,  39, '(', ')', ',', 1);
}

/*----------------------------------------------------------------------
  CreateMINTERVAL
  ----------------------------------------------------------------------*/
void CreateMINTERVAL (Document document, View view)
{
  CreateMathConstruct (document, view,  39, -1, -1, ';', 2);
}

/*----------------------------------------------------------------------
  CreateMFENCE
  ----------------------------------------------------------------------*/
void CreateMFENCE (Document document, View view)
{
  CreateMathConstruct (document, view,  39, -1, -1, -1, -1);}


/*----------------------------------------------------------------------
  CreateMROW
  ----------------------------------------------------------------------*/
void CreateMROW (Document document, View view)
{
  CreateMathConstruct (document, view,  11);
}

/*----------------------------------------------------------------------
  CreateMMULTISCRIPTS
  ----------------------------------------------------------------------*/
void CreateMMULTISCRIPTS (Document document, View view)
{
  CreateMathConstruct (document, view,  12);
}

/*----------------------------------------------------------------------
  CreateMTABLE
  ----------------------------------------------------------------------*/
void CreateMTABLE (Document document, View view)
{
  CreateMathConstruct (document, view,  13);
}


/*----------------------------------------------------------------------
  CreateMIntegral

  ----------------------------------------------------------------------*/
void CreateMIntegral (Document document, View view)
{
  CreateMathConstruct (document, view,  20, 8747, TRUE);
}


/*----------------------------------------------------------------------
  CreateMSum
  ----------------------------------------------------------------------*/
void CreateMSum (Document document, View view)
{
  CreateMathConstruct (document, view,  21, 1, 0x2211);
}

/*----------------------------------------------------------------------
  CreateMMATRIX
  ----------------------------------------------------------------------*/
void CreateMMATRIX (Document document, View view)
{
  CreateMathConstruct (document, view,  53, '(', ')', 0, 0);
}

/*----------------------------------------------------------------------
  CreateMABS
  ----------------------------------------------------------------------*/
void CreateMABS (Document document, View view)
{
  CreateMathConstruct (document, view,  40,'|','|', TRUE);}
/*----------------------------------------------------------------------
  CreateMNORM
  ----------------------------------------------------------------------*/
void CreateMNORM (Document document, View view)
{
  /* Unicode Character 'DOUBLE VERTICAL LINE' (U+2016) */
  CreateMathConstruct (document, view,  40, 0x2016, 0x2016, FALSE);
}
/*----------------------------------------------------------------------
  CreateMALEPHSUB
  ----------------------------------------------------------------------*/
void CreateMALEPHSUB (Document document, View view)
{
  CreateMathConstruct (document, view,  31, MathML_EL_MI, 8501);}
/*----------------------------------------------------------------------
  CreateMAND
  ----------------------------------------------------------------------*/
void CreateMAND (Document document, View view)
{
  CreateMathConstruct (document, view,  22, -1, 0x22c0, 0x2227);}
/*----------------------------------------------------------------------
  CreateMANDBINARY
  ----------------------------------------------------------------------*/
void CreateMANDBINARY (Document document, View view)
{
  CreateMathConstruct (document, view,  27, 0x2227);}
/*----------------------------------------------------------------------
  CreateMAPPROX
  ----------------------------------------------------------------------*/
void CreateMAPPROX (Document document, View view)
{
  CreateMathConstruct (document, view,  27,8776);}
/*----------------------------------------------------------------------
  CreateMARG
  ----------------------------------------------------------------------*/
void CreateMARG (Document document, View view)
{
  CreateMathConstruct (document, view,  23, 0, "arg", TRUE);}
/*----------------------------------------------------------------------
  CreateMARROW1
  ----------------------------------------------------------------------*/
void CreateMARROW1 (Document document, View view)
{
  CreateMathConstruct (document, view,  27,8594);}
/*----------------------------------------------------------------------
  CreateMARROW2
  ----------------------------------------------------------------------*/
void CreateMARROW2 (Document document, View view)
{
  CreateMathConstruct (document, view,  27,8614);}
/*----------------------------------------------------------------------
  CreateMCARD
  ----------------------------------------------------------------------*/
void CreateMCARD (Document document, View view)
{
  CreateMathConstruct (document, view,  40,'|','|', TRUE);}
/*----------------------------------------------------------------------
  CreateMCARD2
  ----------------------------------------------------------------------*/
void CreateMCARD2 (Document document, View view)
{
  CreateMathConstruct (document, view,  23, '#', FALSE);}
/*----------------------------------------------------------------------
  CreateMCARTESIANPRODUCT
  ----------------------------------------------------------------------*/
void CreateMCARTESIANPRODUCT (Document document, View view)
{
  CreateMathConstruct (document, view,  22, -1, 0x220F, 215);}
/*----------------------------------------------------------------------
  CreateMCARTESIANPRODUCTBINARY
  ----------------------------------------------------------------------*/
void CreateMCARTESIANPRODUCTBINARY (Document document, View view)
{
  CreateMathConstruct (document, view,  27,215);}
/*----------------------------------------------------------------------
  CreateMCEILING
  ----------------------------------------------------------------------*/
void CreateMCEILING (Document document, View view)
{
  CreateMathConstruct (document, view,  40, 5, 6, TRUE);}
/*----------------------------------------------------------------------
  CreateMCODOMAIN
  ----------------------------------------------------------------------*/
void CreateMCODOMAIN (Document document, View view)
{
  CreateMathConstruct (document, view,  23, 0, "codomain", TRUE);}
/*----------------------------------------------------------------------
  CreateMCOMBINATION
  ----------------------------------------------------------------------*/
void CreateMCOMBINATION (Document document, View view)
{
  CreateMathConstruct (document, view,  32);}
/*----------------------------------------------------------------------
  CreateMCOMPLEMENT
  ----------------------------------------------------------------------*/
void CreateMCOMPLEMENT (Document document, View view)
{
  CreateMathConstruct (document, view,  59, 'h');
}
/*----------------------------------------------------------------------
  CreateMCOMPLEMENTSUB
  ----------------------------------------------------------------------*/
void CreateMCOMPLEMENTSUB (Document document, View view)
{
  CreateMathConstruct (document, view,  41, 8705);} 
/*----------------------------------------------------------------------
  CreateMCOMPLEXCARTESIAN
  ----------------------------------------------------------------------*/
void CreateMCOMPLEXCARTESIAN (Document document, View view)
{
  CreateMathConstruct (document, view,  48, TRUE);}

/*----------------------------------------------------------------------
  CreateMCOMPLEXCARTESIAN2
  ----------------------------------------------------------------------*/
void CreateMCOMPLEXCARTESIAN2 (Document document, View view)
{
  CreateMathConstruct (document, view,  48, FALSE);}
/*----------------------------------------------------------------------
  CreateMCOMPLEXES
  ----------------------------------------------------------------------*/
void CreateMCOMPLEXES (Document document, View view)
{
  CreateMathConstruct (document, view,  24,8450);}
/*----------------------------------------------------------------------
  CreateMCOMPLEXPOLAR
  ----------------------------------------------------------------------*/
void CreateMCOMPLEXPOLAR (Document document, View view)
{
  CreateMathConstruct (document, view,  49);}
/*----------------------------------------------------------------------
  CreateMCOMPOSE
  ----------------------------------------------------------------------*/
void CreateMCOMPOSE (Document document, View view)
{
  CreateMathConstruct (document, view,  22, 0, 8728);}
/*----------------------------------------------------------------------
  CreateMCOMPOSEBINARY
  ----------------------------------------------------------------------*/
void CreateMCOMPOSEBINARY (Document document, View view)
{
  CreateMathConstruct (document, view,  27, 8728);}
/*----------------------------------------------------------------------
  CreateMCONGRU
  ----------------------------------------------------------------------*/
void CreateMCONGRU (Document document, View view)
{
  CreateMathConstruct (document, view,  28);}
/*----------------------------------------------------------------------
  CreateMCONJUGATE
  ----------------------------------------------------------------------*/
void CreateMCONJUGATE (Document document, View view)
{
  CreateMathConstruct (document, view,  59, 'h');
}
/*----------------------------------------------------------------------
  CreateMCOUPLE
  ----------------------------------------------------------------------*/
void CreateMCOUPLE (Document document, View view)
{
  CreateMathConstruct (document, view,  37, 2);}
/*----------------------------------------------------------------------
  CreateMCURL
  ----------------------------------------------------------------------*/
void CreateMCURL (Document document, View view)
{
  CreateMathConstruct (document, view,  54, 215);}
/*----------------------------------------------------------------------
  CreateMDETERMINANT
  ----------------------------------------------------------------------*/
void CreateMDETERMINANT (Document document, View view)
{
  CreateMathConstruct (document, view,  23, 0, "det", FALSE);}
/*----------------------------------------------------------------------
  CreateMDETERMINANT2
  ----------------------------------------------------------------------*/
void CreateMDETERMINANT2 (Document document, View view)
{
  CreateMathConstruct (document, view,  53, '|', '|', -1, -1);}

/*----------------------------------------------------------------------
  CreateMDIAGONALINTERSECTION
  ----------------------------------------------------------------------*/
void CreateMDIAGONALINTERSECTION (Document document, View view)
{
  CreateMathConstruct (document, view,  43, FALSE, 916, '<');}
/*----------------------------------------------------------------------
  CreateMDIFF
  ----------------------------------------------------------------------*/
void CreateMDIFF (Document document, View view)
{
  CreateMathConstruct (document, view,  50, 8518);}
/*----------------------------------------------------------------------
  CreateMDIFF3
  ----------------------------------------------------------------------*/
void CreateMDIFF3 (Document document, View view)
{
  CreateMathConstruct (document, view,  58);}
/*----------------------------------------------------------------------
  CreateMDIFF2
  ----------------------------------------------------------------------*/
void CreateMDIFF2 (Document document, View view)
{CreateMathConstruct (document, view,  23, 8518, FALSE);}

/*----------------------------------------------------------------------
  CreateMDIRECTSUM
  ----------------------------------------------------------------------*/
void CreateMDIRECTSUM (Document document, View view)
{
  CreateMathConstruct (document, view,  27,8853);}
/*----------------------------------------------------------------------
  CreateMDIVERGENCE
  ----------------------------------------------------------------------*/
void CreateMDIVERGENCE (Document document, View view)
{
  CreateMathConstruct (document, view,  54, '.');}
/*----------------------------------------------------------------------
  CreateMDIVIDE
  ----------------------------------------------------------------------*/
void CreateMDIVIDE (Document document, View view)
{
  CreateMathConstruct (document, view,  27,247);}
/*----------------------------------------------------------------------
  CreateMDOMAIN
  ----------------------------------------------------------------------*/
void CreateMDOMAIN (Document document, View view)
{
  CreateMathConstruct (document, view,  23, 0, "domain", TRUE);}
/*----------------------------------------------------------------------
  CreateMEMPTYSET
  ----------------------------------------------------------------------*/
void CreateMEMPTYSET (Document document, View view)
{
  CreateMathConstruct (document, view,  24,8709);}
/*----------------------------------------------------------------------
  CreateMEQ
  ----------------------------------------------------------------------*/
void CreateMEQ (Document document, View view)
{
  CreateMathConstruct (document, view,  22, 0, '=');}
/*----------------------------------------------------------------------
  CreateMEQUIVALENT
  ----------------------------------------------------------------------*/
void CreateMEQUIVALENT (Document document, View view)
{
  CreateMathConstruct (document, view,  22, 0, 8660);}
/*----------------------------------------------------------------------
  CreateMEQUIVALENT2
  ----------------------------------------------------------------------*/
void CreateMEQUIVALENT2 (Document document, View view)
{
  CreateMathConstruct (document, view,  22, 0, 8801);}
/*----------------------------------------------------------------------
  CreateMEQUIVALENT2BINARY
  ----------------------------------------------------------------------*/
void CreateMEQUIVALENT2BINARY (Document document, View view)
{
  CreateMathConstruct (document, view,  27, 8801);}
/*----------------------------------------------------------------------
  CreateMEQUIVALENTBINARY
  ----------------------------------------------------------------------*/
void CreateMEQUIVALENTBINARY (Document document, View view)
{
  CreateMathConstruct (document, view,  27, 8660);}
/*----------------------------------------------------------------------
  CreateMEQUIVALENTUNDER
  ----------------------------------------------------------------------*/
void CreateMEQUIVALENTUNDER (Document document, View view)
{
  /* Unicode Character 'TILDE OPERATOR' (U+223C) */
  CreateMathConstruct (document, view,  33, TRUE, MathML_EL_MO, 0x223C);
}
/*----------------------------------------------------------------------
  CreateMEULERGAMMA
  ----------------------------------------------------------------------*/
void CreateMEULERGAMMA (Document document, View view)
{
  CreateMathConstruct (document, view,  24,947);}
/*----------------------------------------------------------------------
  CreateMEXISTS
  ----------------------------------------------------------------------*/
void CreateMEXISTS (Document document, View view)
{
  CreateMathConstruct (document, view,  29,8707,FALSE);}
/*----------------------------------------------------------------------
  CreateMEXISTS2
  ----------------------------------------------------------------------*/
void CreateMEXISTS2 (Document document, View view)
{
  CreateMathConstruct (document, view,  29,8707,TRUE);}

/*----------------------------------------------------------------------
  CreateMEXPONENTIALE
  ----------------------------------------------------------------------*/
void CreateMEXPONENTIALE (Document document, View view)
{
  CreateMathConstruct (document, view,  24, 8519);}
/*----------------------------------------------------------------------
  CreateMFACTORIAL
  ----------------------------------------------------------------------*/
void CreateMFACTORIAL (Document document, View view)
{
  CreateMathConstruct (document, view,  23,'!',FALSE);}
/*----------------------------------------------------------------------
  CreateMFACTOROF
  ----------------------------------------------------------------------*/
void CreateMFACTOROF (Document document, View view)
{
  CreateMathConstruct (document, view,  27,'|');}
/*----------------------------------------------------------------------
  CreateMFALSE
  ----------------------------------------------------------------------*/
void CreateMFALSE (Document document, View view)
{
  CreateMathConstruct (document, view,  24, 0, "False");}
/*----------------------------------------------------------------------
  CreateMFLOOR
  ----------------------------------------------------------------------*/
void CreateMFLOOR (Document document, View view)
{
  CreateMathConstruct (document, view,  40, 3, 4, TRUE);}
/*----------------------------------------------------------------------
  CreateMFORALL
  ----------------------------------------------------------------------*/
void CreateMFORALL (Document document, View view)
{
  CreateMathConstruct (document, view,  29,8704,FALSE);}
/*----------------------------------------------------------------------
  CreateMGCD
  ----------------------------------------------------------------------*/
void CreateMGCD (Document document, View view)
{
  CreateMathConstruct (document, view,  23, 0, "gcd", TRUE);}
/*----------------------------------------------------------------------
  CreateMGEQ
  ----------------------------------------------------------------------*/
void CreateMGEQ (Document document, View view)
{
  CreateMathConstruct (document, view,  22, 0, 8805);}
/*----------------------------------------------------------------------
  CreateMGEQBINARY
  ----------------------------------------------------------------------*/
void CreateMGEQBINARY (Document document, View view)
{
  CreateMathConstruct (document, view,  27, 8805);}
/*----------------------------------------------------------------------
  CreateMGRAD
  ----------------------------------------------------------------------*/
void CreateMGRAD (Document document, View view)
{
  CreateMathConstruct (document, view,  54, 0);}
/*----------------------------------------------------------------------
  CreateMGT
  ----------------------------------------------------------------------*/
void CreateMGT (Document document, View view)
{
  CreateMathConstruct (document, view,  22, 0, '>');}
/*----------------------------------------------------------------------
  CreateMIDENT
  ----------------------------------------------------------------------*/
void CreateMIDENT (Document document, View view)
{
  CreateMathConstruct (document, view,  24, 0, "Id");}
/*----------------------------------------------------------------------
  CreateMIMAGE
  ----------------------------------------------------------------------*/
void CreateMIMAGE (Document document, View view)
{
  CreateMathConstruct (document, view,  23, 0, "image", TRUE);}
/*----------------------------------------------------------------------
  CreateMIMAGINARY
  ----------------------------------------------------------------------*/
void CreateMIMAGINARY (Document document, View view)
{
  CreateMathConstruct (document, view,  23, 8465 ,TRUE);}
/*----------------------------------------------------------------------
  CreateMIMAGINARYI
  ----------------------------------------------------------------------*/
void CreateMIMAGINARYI (Document document, View view)
{
  CreateMathConstruct (document, view,  24,8520);}
/*----------------------------------------------------------------------
  CreateMIMPLIES
  ----------------------------------------------------------------------*/
void CreateMIMPLIES (Document document, View view)
{
  CreateMathConstruct (document, view,  27,8658);}
/*----------------------------------------------------------------------
  CreateMIN
  ----------------------------------------------------------------------*/
void CreateMIN (Document document, View view)
{
  CreateMathConstruct (document, view,  27,8712);}
/*----------------------------------------------------------------------
  CreateMINF
  ----------------------------------------------------------------------*/
void CreateMINF (Document document, View view)
{
  CreateMathConstruct (document, view,  23, 0, "inf", FALSE);}
/*----------------------------------------------------------------------
  CreateMINFINITY
  ----------------------------------------------------------------------*/
void CreateMINFINITY (Document document, View view)
{
  CreateMathConstruct (document, view,  24,8734);}
/*----------------------------------------------------------------------
  CreateMINFUNDER
  ----------------------------------------------------------------------*/
void CreateMINFUNDER (Document document, View view)
{
  CreateMathConstruct (document, view,  34, "inf");} 
/*----------------------------------------------------------------------
  CreateMINT2
  ----------------------------------------------------------------------*/
void CreateMINT2 (Document document, View view)
{
  CreateMathConstruct (document, view,  20, -1);}
/*----------------------------------------------------------------------
  CreateMINTEGERS
  ----------------------------------------------------------------------*/
void CreateMINTEGERS (Document document, View view)
{
  CreateMathConstruct (document, view,  24,8484);}
/*----------------------------------------------------------------------
  CreateMINTERSECT
  ----------------------------------------------------------------------*/
void CreateMINTERSECT (Document document, View view)
{
  CreateMathConstruct (document, view,  22, -1, 0x22C2, 0x2229);}
/*----------------------------------------------------------------------
  CreateMINTERSECTBINARY
  ----------------------------------------------------------------------*/
void CreateMINTERSECTBINARY (Document document, View view)
{
  CreateMathConstruct (document, view,  27, 0x2229);}
/*----------------------------------------------------------------------
  CreateMINTERSECTUNDER
  ----------------------------------------------------------------------*/
void CreateMINTERSECTUNDER (Document document, View view)
{
  CreateMathConstruct (document, view,  21, 2, 0x22C2);}
/*----------------------------------------------------------------------
  CreateMINTUNDER
  ----------------------------------------------------------------------*/
void CreateMINTUNDER (Document document, View view)
{
  CreateMathConstruct (document, view,  20, 8747, FALSE);}
/*----------------------------------------------------------------------
  CreateMINVERSE
  ----------------------------------------------------------------------*/
void CreateMINVERSE (Document document, View view)
{
  CreateMathConstruct (document, view,  30, MathML_EL_MN, -1, "-1");}
/*----------------------------------------------------------------------
  CreateMISOMORPHIC
  ----------------------------------------------------------------------*/
void CreateMISOMORPHIC (Document document, View view)
{
  CreateMathConstruct (document, view,  27,8773);}
/*----------------------------------------------------------------------
  CreateMLISTEXTENSION
  ----------------------------------------------------------------------*/
void CreateMLISTEXTENSION (Document document, View view)
{
  CreateMathConstruct (document, view,  39, '[', ']', ';', -1);}
/*----------------------------------------------------------------------
  CreateMLCM
  ----------------------------------------------------------------------*/
void CreateMLCM (Document document, View view)
{
  CreateMathConstruct (document, view,  23, 0, "lcm", TRUE);}
/*----------------------------------------------------------------------
  CreateMLAPLACIAN
  ----------------------------------------------------------------------*/
void CreateMLAPLACIAN (Document document, View view)
{
  CreateMathConstruct (document, view,  54, 1);}
/*----------------------------------------------------------------------
  CreateMLEQ
  ----------------------------------------------------------------------*/
void CreateMLEQ (Document document, View view)
{
  CreateMathConstruct (document, view,  22, 0, 8804);}
/*----------------------------------------------------------------------
  CreateMLL
  ----------------------------------------------------------------------*/
void CreateMLL (Document document, View view)
{
  CreateMathConstruct (document, view,  27, 8810);}
/*----------------------------------------------------------------------
  CreateMGG
  ----------------------------------------------------------------------*/
void CreateMGG (Document document, View view)
{
  CreateMathConstruct (document, view,  27, 8811);}


/*----------------------------------------------------------------------
  CreateMLEQBINARY
  ----------------------------------------------------------------------*/
void CreateMLEQBINARY (Document document, View view)
{
  CreateMathConstruct (document, view,  27, 8804);}
/*----------------------------------------------------------------------
  CreateMLISTSEPARATION
  ----------------------------------------------------------------------*/
void CreateMLISTSEPARATION (Document document, View view)
{
  CreateMathConstruct (document, view,  38, '[', ']');}
/*----------------------------------------------------------------------
  CreateMLT
  ----------------------------------------------------------------------*/
void CreateMLT (Document document, View view)
{
  CreateMathConstruct (document, view,  22, 0, '<');}
/*----------------------------------------------------------------------
  CreateMLAMBDA
  ----------------------------------------------------------------------*/
void CreateMLAMBDA (Document document, View view)
{
  CreateMathConstruct (document, view,  45);}

/*----------------------------------------------------------------------
  CreateMLIM
  ----------------------------------------------------------------------*/
void CreateMLIM (Document document, View view)
{
  CreateMathConstruct (document, view,  33, FALSE, MathML_EL_MO, 0, "lim");}

/*----------------------------------------------------------------------
  CreateMLIMTENDSTO
  ----------------------------------------------------------------------*/
void CreateMLIMTENDSTO (Document document, View view)
{
  CreateMathConstruct (document, view,  43, FALSE, 0, 8594, "lim");}

/*----------------------------------------------------------------------
  CreateMMAP
  ----------------------------------------------------------------------*/
void CreateMMAP (Document document, View view)
{
  CreateMathConstruct (document, view,  47);}
/*----------------------------------------------------------------------
  CreateMMAX
  ----------------------------------------------------------------------*/
void CreateMMAX (Document document, View view)
{
  CreateMathConstruct (document, view,  23, 0, "max", FALSE);}
/*----------------------------------------------------------------------
  CreateMMAXUNDER
  ----------------------------------------------------------------------*/
void CreateMMAXUNDER (Document document, View view)
{
  CreateMathConstruct (document, view,  34, "max");} 
/*----------------------------------------------------------------------
  CreateMMEAN
  ----------------------------------------------------------------------*/
void CreateMMEAN (Document document, View view)
{
  CreateMathConstruct (document, view,  40, '<', '>', TRUE);}
/*----------------------------------------------------------------------
  CreateMMEDIAN
  ----------------------------------------------------------------------*/
void CreateMMEDIAN (Document document, View view)
{
  CreateMathConstruct (document, view,  23, 0, "median", TRUE);}
/*----------------------------------------------------------------------
  CreateMMIN
  ----------------------------------------------------------------------*/
void CreateMMIN (Document document, View view)
{
  CreateMathConstruct (document, view,  23, 0, "min", FALSE);}
/*----------------------------------------------------------------------
  CreateMMINUNDER
  ----------------------------------------------------------------------*/
void CreateMMINUNDER (Document document, View view)
{
  CreateMathConstruct (document, view,  34, "min");} 
/*----------------------------------------------------------------------
  CreateMMINUSBINARY
  ----------------------------------------------------------------------*/
void CreateMMINUSBINARY (Document document, View view)
{
  CreateMathConstruct (document, view,  27,8722);}
/*----------------------------------------------------------------------
  CreateMMINUSUNARY
  ----------------------------------------------------------------------*/
void CreateMMINUSUNARY (Document document, View view)
{
  CreateMathConstruct (document, view,  23, 8722, FALSE);}
/*----------------------------------------------------------------------
  CreateMSYMBOLO
  ----------------------------------------------------------------------*/
void CreateMSYMBOLO (Document document, View view)
{
  CreateMathConstruct (document, view,  23, 'o', TRUE);}
/*----------------------------------------------------------------------
  CreateMSYMBOLOO
  ----------------------------------------------------------------------*/
void CreateMSYMBOLOO (Document document, View view)
{
  CreateMathConstruct (document, view,  23, 'O', TRUE);}
/*----------------------------------------------------------------------
  CreateMMODE
  ----------------------------------------------------------------------*/
void CreateMMODE (Document document, View view)
{
  CreateMathConstruct (document, view,  23, 0, "mode", TRUE);}
/*----------------------------------------------------------------------
  CreateMMOMENT
  ----------------------------------------------------------------------*/
void CreateMMOMENT (Document document, View view)
{
  CreateMathConstruct (document, view,  56);}
/*----------------------------------------------------------------------
  CreateMNATURALS
  ----------------------------------------------------------------------*/
void CreateMNATURALS (Document document, View view)
{
  CreateMathConstruct (document, view,  24,8469);}
/*----------------------------------------------------------------------
  CreateMNEQ
  ----------------------------------------------------------------------*/
void CreateMNEQ (Document document, View view)
{
  CreateMathConstruct (document, view,  27,8800);}
/*----------------------------------------------------------------------
  CreateMNOT
  ----------------------------------------------------------------------*/
void CreateMNOT (Document document, View view)
{
  CreateMathConstruct (document, view,  23, 172, FALSE);}
/*----------------------------------------------------------------------
  CreateMNOTANUMBER
  ----------------------------------------------------------------------*/
void CreateMNOTANUMBER (Document document, View view)
{
  CreateMathConstruct (document, view,  24, 0, "NaN");}
/*----------------------------------------------------------------------
  CreateMNOTIN
  ----------------------------------------------------------------------*/
void CreateMNOTIN (Document document, View view)
{
  CreateMathConstruct (document, view,  27,8713);}
/*----------------------------------------------------------------------
  CreateMNOTPRSUBSET
  ----------------------------------------------------------------------*/
void CreateMNOTPRSUBSET (Document document, View view)
{
  CreateMathConstruct (document, view,  27,8836);}
/*----------------------------------------------------------------------
  CreateMNOTSUBSET
  ----------------------------------------------------------------------*/
void CreateMNOTSUBSET (Document document, View view)
{
  CreateMathConstruct (document, view,  27,8840);}
/*----------------------------------------------------------------------
  CreateMNUPLET
  ----------------------------------------------------------------------*/
void CreateMNUPLET (Document document, View view)
{
  CreateMathConstruct (document, view,  37, 0);}
/*----------------------------------------------------------------------
  CreateMOMEGASUB
  ----------------------------------------------------------------------*/
void CreateMOMEGASUB (Document document, View view)
{
  CreateMathConstruct (document, view,  31, MathML_EL_MI, 969);}
/*----------------------------------------------------------------------
  CreateMOR
  ----------------------------------------------------------------------*/
void CreateMOR (Document document, View view)
{
  CreateMathConstruct (document, view,  22, -1, 0x22c1, 0x2228);}
/*----------------------------------------------------------------------
  CreateMORBINARY
  ----------------------------------------------------------------------*/
void CreateMORBINARY (Document document, View view)
{
  CreateMathConstruct (document, view,  27, 0x2228);}
/*----------------------------------------------------------------------
  CreateMORTHOGONAL
  ----------------------------------------------------------------------*/
void CreateMORTHOGONAL (Document document, View view)
{
  CreateMathConstruct (document, view,  27,8869);}
/*----------------------------------------------------------------------
  CreateMPARALLEL
  ----------------------------------------------------------------------*/
void CreateMPARALLEL (Document document, View view)
{
  CreateMathConstruct (document, view,  27,8741);}
/*----------------------------------------------------------------------
  CreateMORTHOGONALCOMPLEMENT
  ----------------------------------------------------------------------*/
void CreateMORTHOGONALCOMPLEMENT (Document document, View view)
{
  CreateMathConstruct (document, view,  30, MathML_EL_MO, 8869);}
/*----------------------------------------------------------------------
  CreateMOUTERPRODUCT
  ----------------------------------------------------------------------*/
void CreateMOUTERPRODUCT (Document document, View view)
{
  CreateMathConstruct (document, view,  27,8855);}
/*----------------------------------------------------------------------
  CreateMPARTIALDIFF
  ----------------------------------------------------------------------*/
void CreateMPARTIALDIFF (Document document, View view)
{
  CreateMathConstruct (document, view,  50, 8706);}
/*----------------------------------------------------------------------
  CreateMPARTIALDIFF2
  ----------------------------------------------------------------------*/
void CreateMPARTIALDIFF2 (Document document, View view)
{
  CreateMathConstruct (document, view,  51);}
/*----------------------------------------------------------------------
  CreateMPI
  ----------------------------------------------------------------------*/
void CreateMPI (Document document, View view)
{
  CreateMathConstruct (document, view,  24,960);}
/*----------------------------------------------------------------------
  CreateMPIECEWISE
  ----------------------------------------------------------------------*/
void CreateMPIECEWISE (Document document, View view)
{
  CreateMathConstruct (document, view,  25);}

/*----------------------------------------------------------------------
  CreateMVERTICALBRACE
  ----------------------------------------------------------------------*/
void CreateMVERTICALBRACE (Document document, View view)
{
  CreateMathConstruct (document, view,  25);}

/*----------------------------------------------------------------------
  CreateMPLUS
  ----------------------------------------------------------------------*/
void CreateMPLUS (Document document, View view)
{
  CreateMathConstruct (document, view,  22, -1, 0x2211, '+');}
/*----------------------------------------------------------------------
  CreateMPOWER
  ----------------------------------------------------------------------*/
void CreateMPOWER (Document document, View view)
{
  CreateMathConstruct (document, view,  7);}
/*----------------------------------------------------------------------
  CreateMPOWERSET
  ----------------------------------------------------------------------*/
void CreateMPOWERSET (Document document, View view)
{
  CreateMathConstruct (document, view,  23, 8472, TRUE);}
/*----------------------------------------------------------------------
  CreateMPRIMES
  ----------------------------------------------------------------------*/
void CreateMPRIMES (Document document, View view)
{
  CreateMathConstruct (document, view,  24,8473);}
/*----------------------------------------------------------------------
  CreateMPRODUNDER
  ----------------------------------------------------------------------*/
void CreateMPRODUNDER (Document document, View view)
{
  CreateMathConstruct (document, view,  21, 2, 0x220F);}
/*----------------------------------------------------------------------
  CreateMPRODUNDEROVER
  ----------------------------------------------------------------------*/
void CreateMPRODUNDEROVER (Document document, View view)
{
  CreateMathConstruct (document, view,  21, 1, 0x220F);}
/*----------------------------------------------------------------------
  CreateMPRSUBSET
  ----------------------------------------------------------------------*/
void CreateMPRSUBSET (Document document, View view)
{
  CreateMathConstruct (document, view,  22, 0, 8834);}
/*----------------------------------------------------------------------
  CreateMPRSUBSETBINARY
  ----------------------------------------------------------------------*/
void CreateMPRSUBSETBINARY (Document document, View view)
{
  CreateMathConstruct (document, view,  27, 8834);}
/*----------------------------------------------------------------------
  CreateMQUOTIENT
  ----------------------------------------------------------------------*/
void CreateMQUOTIENT (Document document, View view)
{
  CreateMathConstruct (document, view,  46);}
/*----------------------------------------------------------------------
  CreateMRATIONNALS
  ----------------------------------------------------------------------*/
void CreateMRATIONNALS (Document document, View view)
{
  CreateMathConstruct (document, view,  24, 8474);}
/*----------------------------------------------------------------------
  CreateMREAL
  ----------------------------------------------------------------------*/
void CreateMREAL (Document document, View view)
{
  CreateMathConstruct (document, view,  23, 8476, TRUE);}
/*----------------------------------------------------------------------
  CreateMREALS
  ----------------------------------------------------------------------*/
void CreateMREALS (Document document, View view)
{
  CreateMathConstruct (document, view,  24,8477);}
/*----------------------------------------------------------------------
  CreateMQUATERNIONS
  ----------------------------------------------------------------------*/
void CreateMQUATERNIONS (Document document, View view)
{
  CreateMathConstruct (document, view,  24,8461);}
/*----------------------------------------------------------------------
  CreateMREM
  ----------------------------------------------------------------------*/
void CreateMREM (Document document, View view)
{
  CreateMathConstruct (document, view,  27, 0, "mod");}
/*----------------------------------------------------------------------
  CreateMSCALARPRODUCT
  ----------------------------------------------------------------------*/
void CreateMSCALARPRODUCT (Document document, View view)
{
  CreateMathConstruct (document, view,  27,'.');}
/*----------------------------------------------------------------------
  CreateMSDEV
  ----------------------------------------------------------------------*/
void CreateMSDEV (Document document, View view)
{
  CreateMathConstruct (document, view,  23, 963, TRUE);}
/*----------------------------------------------------------------------
  CreateMSELECTOR
  ----------------------------------------------------------------------*/
void CreateMSELECTOR (Document document, View view)
{
  CreateMathConstruct (document, view,  36);}
/*----------------------------------------------------------------------
  CreateMSETDIFF
  ----------------------------------------------------------------------*/
void CreateMSETDIFF (Document document, View view)
{
  CreateMathConstruct (document, view,  27,'\\');}
/*----------------------------------------------------------------------
  CreateMSETEXTENSION
  ----------------------------------------------------------------------*/
void CreateMSETEXTENSION (Document document, View view)
{
  CreateMathConstruct (document, view,  39, '{', '}', ';', -1);}
/*----------------------------------------------------------------------
  CreateMSETSEPARATION
  ----------------------------------------------------------------------*/
void CreateMSETSEPARATION (Document document, View view)
{
  CreateMathConstruct (document, view,  38, '{', '}');}
/*----------------------------------------------------------------------
  CreateMSETSYMDIFF
  ----------------------------------------------------------------------*/
void CreateMSETSYMDIFF (Document document, View view)
{
  CreateMathConstruct (document, view,  27, 916);}
/*----------------------------------------------------------------------
  CreateMSUBSET
  ----------------------------------------------------------------------*/
void CreateMSUBSET (Document document, View view)
{
  CreateMathConstruct (document, view,  22, 0, 8838);}
/*----------------------------------------------------------------------
  CreateMSUBSETBINARY
  ----------------------------------------------------------------------*/
void CreateMSUBSETBINARY (Document document, View view)
{
  CreateMathConstruct (document, view,  27, 8838);}
/*----------------------------------------------------------------------
  CreateMSUMUNDER
  ----------------------------------------------------------------------*/
void CreateMSUMUNDER (Document document, View view)
{
  CreateMathConstruct (document, view,  21, 2, 0x2211);}
/*----------------------------------------------------------------------
  CreateMSUP2
  ----------------------------------------------------------------------*/
void CreateMSUP2 (Document document, View view)
{
  CreateMathConstruct (document, view,  23, 0, "sup", FALSE);}
/*----------------------------------------------------------------------
  CreateMSUPMINUS
  ----------------------------------------------------------------------*/
void CreateMSUPMINUS (Document document, View view)
{
  CreateMathConstruct (document, view,  30, MathML_EL_MI, '-');}
/*----------------------------------------------------------------------
  CreateMSUPPLUS
  ----------------------------------------------------------------------*/
void CreateMSUPPLUS (Document document, View view)
{
  CreateMathConstruct (document, view,  30, MathML_EL_MI, '+');}
/*----------------------------------------------------------------------
  CreateMSUPUNDER
  ----------------------------------------------------------------------*/
void CreateMSUPUNDER (Document document, View view)
{
  CreateMathConstruct (document, view,  34, "sup");}
/*----------------------------------------------------------------------
  CreateMTENDSTO
  ----------------------------------------------------------------------*/
void CreateMTENDSTO (Document document, View view)
{
  CreateMathConstruct (document, view,  33, TRUE, MathML_EL_MO, 'R');}
/*----------------------------------------------------------------------
  CreateMTENDSTOTENDSTO
  ----------------------------------------------------------------------*/
void CreateMTENDSTOTENDSTO (Document document, View view)
{
  CreateMathConstruct (document, view,  43, TRUE, 'R', 8594);}
/*----------------------------------------------------------------------
  CreateMTIMES
  ----------------------------------------------------------------------*/
void CreateMTIMES (Document document, View view)
{
  CreateMathConstruct (document, view,  22, -1, 0x220F, 215);}
/*----------------------------------------------------------------------
  CreateMTIMESBINARY
  ----------------------------------------------------------------------*/
void CreateMTIMESBINARY (Document document, View view)
{
  CreateMathConstruct (document, view,  27,215);}
/*----------------------------------------------------------------------
  CreateMTRANSPOSE
  ----------------------------------------------------------------------*/
void CreateMTRANSPOSE (Document document, View view)
{
  CreateMathConstruct (document, view,  57);}
/*----------------------------------------------------------------------
  CreateMTRUE
  ----------------------------------------------------------------------*/
void CreateMTRUE (Document document, View view)
{
  CreateMathConstruct (document, view,  24, 0, "True");}
/*----------------------------------------------------------------------
  CreateMUNION
  ----------------------------------------------------------------------*/
void CreateMUNION (Document document, View view)
{
  CreateMathConstruct (document, view,  22, -1, 0x22C3, 0x222A);}
/*----------------------------------------------------------------------
  CreateMUNIONUNARY
  ----------------------------------------------------------------------*/
void CreateMUNIONUNARY (Document document, View view)
{
  CreateMathConstruct (document, view,  23, 0x22C3, FALSE);}
/*----------------------------------------------------------------------
  CreateMUNIONUNDER
  ----------------------------------------------------------------------*/
void CreateMUNIONUNDER (Document document, View view)
{
  CreateMathConstruct (document, view,  21, 2, 0x22C3);}
/*----------------------------------------------------------------------
  CreateMUNIONBINARY
  ----------------------------------------------------------------------*/
void CreateMUNIONBINARY (Document document, View view)
{
  CreateMathConstruct (document, view,  27, 0x222A);}
/*----------------------------------------------------------------------
  CreateMVARIANCE
  ----------------------------------------------------------------------*/
void CreateMVARIANCE (Document document, View view)
{
  CreateMathConstruct (document, view,  55);}
/*----------------------------------------------------------------------
  CreateMVECTORPRODUCT
  ----------------------------------------------------------------------*/
void CreateMVECTORPRODUCT (Document document, View view)
{
  CreateMathConstruct (document, view,  27,8896);}
/*----------------------------------------------------------------------
  CreateMVECTORPRODUCT
  ----------------------------------------------------------------------*/
void CreateMVECTORROW (Document document, View view)
{
  CreateMathConstruct (document, view,  53, '(', ')', 0, 1);}
/*----------------------------------------------------------------------
  CreateMVECTORPRODUCT
  ----------------------------------------------------------------------*/
void CreateMVECTORCOLUMN (Document document, View view)
{
  CreateMathConstruct (document, view,  53, '(', ')', 1, 0);}
/*----------------------------------------------------------------------
  CreateMXOR
  ----------------------------------------------------------------------*/
void CreateMXOR (Document document, View view)
{CreateMathConstruct (document, view,  22, -1, 0, "xor", 0);}

/*----------------------------------------------------------------------
  CreateMXORBINARY
  ----------------------------------------------------------------------*/
void CreateMXORBINARY (Document document, View view)
{
  CreateMathConstruct (document, view,  27, 0, "xor");}

/*----------------------------------------------------------------------
  CreateMARCCOS
  ----------------------------------------------------------------------*/
void CreateMARCCOS (Document document, View view)
{
CreateMathConstruct (document, view,  44, "arccos");}
/*----------------------------------------------------------------------
  CreateMARCCOSH
  ----------------------------------------------------------------------*/
void CreateMARCCOSH (Document document, View view)
{
CreateMathConstruct (document, view,  44, "arccosh");}
/*----------------------------------------------------------------------
  CreateMARCCOT
  ----------------------------------------------------------------------*/
void CreateMARCCOT (Document document, View view)
{
CreateMathConstruct (document, view,  44, "arccot");}
/*----------------------------------------------------------------------
  CreateMARCCOTH
  ----------------------------------------------------------------------*/
void CreateMARCCOTH (Document document, View view)
{
CreateMathConstruct (document, view,  44, "arccoth");}
/*----------------------------------------------------------------------
  CreateMARCCSC
  ----------------------------------------------------------------------*/
void CreateMARCCSC (Document document, View view)
{
CreateMathConstruct (document, view,  44, "arccsc");}
/*----------------------------------------------------------------------
  CreateMARCCSCH
  ----------------------------------------------------------------------*/
void CreateMARCCSCH (Document document, View view)
{
CreateMathConstruct (document, view,  44, "arccsch");}
/*----------------------------------------------------------------------
  CreateMARCSEC
  ----------------------------------------------------------------------*/
void CreateMARCSEC (Document document, View view)
{
CreateMathConstruct (document, view,  44, "arcsec");}
/*----------------------------------------------------------------------
  CreateMARCSECH
  ----------------------------------------------------------------------*/
void CreateMARCSECH (Document document, View view)
{
CreateMathConstruct (document, view,  44, "arcsech");}
/*----------------------------------------------------------------------
  CreateMARCSIN
  ----------------------------------------------------------------------*/
void CreateMARCSIN (Document document, View view)
{
CreateMathConstruct (document, view,  44, "arcsin");}
/*----------------------------------------------------------------------
  CreateMARCSINH
  ----------------------------------------------------------------------*/
void CreateMARCSINH (Document document, View view)
{
CreateMathConstruct (document, view,  44, "arcsinh");}
/*----------------------------------------------------------------------
  CreateMARCTAN
  ----------------------------------------------------------------------*/
void CreateMARCTAN (Document document, View view)
{
CreateMathConstruct (document, view,  44, "arctan");}
/*----------------------------------------------------------------------
  CreateMARCTANH
  ----------------------------------------------------------------------*/
void CreateMARCTANH (Document document, View view)
{
CreateMathConstruct (document, view,  44, "arctanh");}
/*----------------------------------------------------------------------
  CreateMCOS
  ----------------------------------------------------------------------*/
void CreateMCOS (Document document, View view)
{
CreateMathConstruct (document, view,  44, "cos");}
/*----------------------------------------------------------------------
  CreateMCOSH
  ----------------------------------------------------------------------*/
void CreateMCOSH (Document document, View view)
{
CreateMathConstruct (document, view,  44, "cosh");}
/*----------------------------------------------------------------------
  CreateMCOT
  ----------------------------------------------------------------------*/
void CreateMCOT (Document document, View view)
{
CreateMathConstruct (document, view,  44, "cot");}
/*----------------------------------------------------------------------
  CreateMCOTH
  ----------------------------------------------------------------------*/
void CreateMCOTH (Document document, View view)
{
CreateMathConstruct (document, view,  44, "coth");}
/*----------------------------------------------------------------------
  CreateMCSC
  ----------------------------------------------------------------------*/
void CreateMCSC (Document document, View view)
{
CreateMathConstruct (document, view,  44, "csc");}
/*----------------------------------------------------------------------
  CreateMCSCH
  ----------------------------------------------------------------------*/
void CreateMCSCH (Document document, View view)
{
CreateMathConstruct (document, view,  44, "csch");}
/*----------------------------------------------------------------------
  CreateMEXP
  ----------------------------------------------------------------------*/
void CreateMEXP (Document document, View view)
{
CreateMathConstruct (document, view,  44, "exp");}
/*----------------------------------------------------------------------
  CreateMLN
  ----------------------------------------------------------------------*/
void CreateMLN (Document document, View view)
{
CreateMathConstruct (document, view,  44, "ln");}
/*----------------------------------------------------------------------
  CreateMLOG
  ----------------------------------------------------------------------*/
void CreateMLOG (Document document, View view)
{
CreateMathConstruct (document, view,  44, "log");}
/*----------------------------------------------------------------------
  CreateMSEC
  ----------------------------------------------------------------------*/
void CreateMSEC (Document document, View view)
{
CreateMathConstruct (document, view,  44, "sec");}
/*----------------------------------------------------------------------
  CreateMSECH
  ----------------------------------------------------------------------*/
void CreateMSECH (Document document, View view)
{
CreateMathConstruct (document, view,  44, "sech");}
/*----------------------------------------------------------------------
  CreateMSIN
  ----------------------------------------------------------------------*/
void CreateMSIN (Document document, View view)
{
CreateMathConstruct (document, view,  44, "sin");}
/*----------------------------------------------------------------------
  CreateMSINH
  ----------------------------------------------------------------------*/
void CreateMSINH (Document document, View view)
{
CreateMathConstruct (document, view,  44, "sinh");}
/*----------------------------------------------------------------------
  CreateMTAN
  ----------------------------------------------------------------------*/
void CreateMTAN (Document document, View view)
{
CreateMathConstruct (document, view,  44, "tan");}
/*----------------------------------------------------------------------
  CreateMTANH
  ----------------------------------------------------------------------*/
void CreateMTANH (Document document, View view)
{
CreateMathConstruct (document, view,  44, "tanh");}


/*----------------------------------------------------------------------
  CheckMROW
  If el is a MROW element with only one child, remove the MROW element
  and replace it by its child.
  -----------------------------------------------------------------------*/
static void CheckMROW (Element* el, Document doc)
{
  Element	child, firstChild, next;
  ElementType	elType;
  AttributeType	attrType;
  Attribute	attr;
  int		nChildren;
  ThotBool      oldStructureChecking;

  elType = TtaGetElementType (*el);
  if (elType.ElTypeNum == MathML_EL_MROW &&
      strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
    /* the parent of the deleted element is a MROW */
    {
      firstChild = TtaGetFirstChild (*el);
      child = firstChild;
      /* count all children that are not placeholders */
      nChildren = 0;
      while (child != NULL && nChildren < 2)
        {
          elType = TtaGetElementType (child);
          if (elType.ElTypeNum != MathML_EL_Construct &&
              elType.ElTypeNum != MathML_EL_Construct1)
            /* this is not a Construct */
            nChildren++;
          else
            {
              attrType.AttrSSchema = elType.ElSSchema;
              attrType.AttrTypeNum = MathML_ATTR_IntPlaceholder;
              attr = TtaGetAttribute (child, attrType);
              if (attr == NULL)
                /* this is not a placeholder */
                nChildren++;
            }
          TtaNextSibling (&child);
        }

      if (nChildren == 1)
        /* there is only one element that is not a placeholder in the MROW.
           Remove the MROW */
        {
          TtaSetDisplayMode (doc, DeferredDisplay);
          oldStructureChecking = TtaGetStructureChecking (doc);
          TtaSetStructureChecking (FALSE, doc);
          TtaRegisterElementDelete (*el, doc);
          child = firstChild;
          while (child != NULL)
            {
              next = child;
              TtaNextSibling (&next);
              TtaRemoveTree (child, doc);
              TtaInsertSibling (child, *el, TRUE, doc);
              TtaRegisterElementCreate (child, doc);
              child = next;
            }
          TtaDeleteTree (*el, doc);
          *el = NULL;
          TtaSetStructureChecking (oldStructureChecking, doc);
          TtaSetDisplayMode (doc, DisplayImmediately);
        }
    }
}


/*----------------------------------------------------------------------
  RoundSelection
  -----------------------------------------------------------------------*/
static void RoundSelection (Element *firstSel, Element *lastSel,
                            int *firstChar, int *lastChar)
{
  ElementType  elType;
  Element      sibling;

  /* if the selection starts on the first character of a text string and
     the whole text string is selected, then the selection starts on the
     parent element of that text string */
  elType = TtaGetElementType (*firstSel);
  if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
    if (*firstChar <= 1)
      if (*lastSel != *firstSel || (*firstChar == 0 && *lastChar == 0) ||
          *lastChar >= TtaGetElementVolume (*lastSel))
        {
          sibling = *firstSel;  TtaPreviousSibling (&sibling);
          if (!sibling)
            {
              /* no sibling before the first selected element */
              if (TtaGetParent (*firstSel) != TtaGetParent (*lastSel))
                {
                  *firstSel = TtaGetParent (*firstSel);
                  *firstChar = 0;
                }
              else
                {
                  sibling = *lastSel;  TtaNextSibling (&sibling);
                  if (!sibling)
                    {
                      *firstSel = TtaGetParent (*firstSel);
                      *lastSel = *firstSel;
                      *firstChar = 0;
                      *lastChar = 0;
                    }
                }
            }
        }
   
  /* if the selection ends on the last character of a text string and
     the whole text string is selected, then the selection ends on the
     parent element of that text string */
  elType = TtaGetElementType (*lastSel);
  if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
    if (*lastChar == 0 || *lastChar >=  TtaGetElementVolume (*lastSel))
      if (*lastSel != *firstSel || *firstChar <= 1)
        {
          sibling = *lastSel;  TtaNextSibling (&sibling);
          if (!sibling)
            /* no sibling after the last selected element */
            if (TtaGetParent (*firstSel) != TtaGetParent (*lastSel))
              {
                *lastSel = TtaGetParent (*lastSel);
                *lastChar = 0;
              }
        }
}

/*----------------------------------------------------------------------
  CreateCharStringElement
  -----------------------------------------------------------------------*/
static void CreateCharStringElement (int typeNum, Document doc, View view)
{
  ElementType    elType;
  AttributeType  attrType;
  Element        firstSel, lastSel, first, last, el, newEl, nextEl,
    leaf, lastLeaf, nextLeaf, parent, selEl;
  int            firstChar, lastChar, i, j;
  ThotBool       nonEmptySel, done, mrowCreated, same, oldStructureChecking;

  if (!TtaGetDocumentAccessMode (doc))
    /* the document is in ReadOnly mode */
    return;

  done = FALSE;
  /* get the current selection */
  TtaGiveFirstSelectedElement (doc, &firstSel, &firstChar, &i);
  TtaGiveLastSelectedElement (doc, &lastSel, &j, &lastChar);

  /* check whether the selection is empty (just a caret) or contains some
     characters/elements */
  nonEmptySel = !TtaIsSelectionEmpty ();
  if (!nonEmptySel)
    /* selection seems to be empty. Let's see... */
    {
      elType = TtaGetElementType (firstSel);
      if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0
          && (elType.ElTypeNum == MathML_EL_MTEXT ||
              elType.ElTypeNum == MathML_EL_MI ||
              elType.ElTypeNum == MathML_EL_MN ||
              elType.ElTypeNum == MathML_EL_MO))
        {
          leaf = TtaGetFirstChild (firstSel);
          if (leaf)
            {
              attrType.AttrSSchema = elType.ElSSchema;
              attrType.AttrTypeNum = MathML_ATTR_EntityName;
              if (TtaGetAttribute (leaf, attrType))
                /* it's an entity that is rendered as an empty string */
                nonEmptySel = TRUE;
            }
        }
    }

  if (!nonEmptySel)
    /* just a caret: create the requested element at that position */
    {
      switch (typeNum)
        {
        case MathML_EL_MTEXT:
          i = 14;
          break;
        case MathML_EL_MI:
          i = 15;
          break;
        case MathML_EL_MN:
          i = 16;
          break;
        case MathML_EL_MO:
          i = 17;
          break;
        }
      CreateMathConstruct (doc, view, i);
      return;
    }

  /* if not within a MathML element, nothing to do */
  elType = TtaGetElementType (firstSel);
  if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") != 0)
    return;

  TtaSetDisplayMode (doc, DeferredDisplay);
  TtaOpenUndoSequence (doc, firstSel, lastSel, firstChar, lastChar);
  oldStructureChecking = TtaGetStructureChecking (doc);
  TtaSetStructureChecking (FALSE, doc);
  selEl = NULL;

  RoundSelection (&firstSel, &lastSel, &firstChar, &lastChar);

  if (firstSel == lastSel && firstChar == 0 && lastChar == 0)
    /* a single element is selected and this element is entirely selected */
    {
      elType = TtaGetElementType (firstSel);
      if (elType.ElTypeNum != typeNum)
        /* the type of this element is not the type requested by the user */
        if ((elType.ElTypeNum == MathML_EL_MTEXT ||
             elType.ElTypeNum == MathML_EL_MI ||
             elType.ElTypeNum == MathML_EL_MN ||
             elType.ElTypeNum == MathML_EL_MO) &&
            strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
          /* the selected element is compatible with the type requested */
          /* just change the type of this element */
          {
            TtaUnselect (doc);
            /*TtaRegisterElementReplace (firstSel, doc);*/
            TtaChangeTypeOfElement (firstSel, doc, typeNum);
            TtaRegisterElementTypeChange (firstSel, elType.ElTypeNum, doc);
            MathSetAttributes (firstSel, doc, NULL);
            selEl = firstSel;
            done = TRUE;
          }
    }

  if (!done)
    {
      /* first checks that only siblings elements are selected and that
         all selected elements are MI, MN, MO or MTEXT */
      elType = TtaGetElementType (firstSel);
      if ((elType.ElTypeNum == MathML_EL_TEXT_UNIT ||
           elType.ElTypeNum == MathML_EL_SYMBOL_UNIT ||
           elType.ElTypeNum == MathML_EL_MGLYPH) &&
          strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
        first = TtaGetParent (firstSel);
      else
        first = firstSel;
      elType = TtaGetElementType (lastSel);
      if ((elType.ElTypeNum == MathML_EL_TEXT_UNIT ||
           elType.ElTypeNum == MathML_EL_SYMBOL_UNIT ||
           elType.ElTypeNum == MathML_EL_MGLYPH) &&
          strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
        last = TtaGetParent (lastSel);
      else
        last = lastSel;
      if (TtaGetParent (first) == TtaGetParent (last))
        /* selected elements are all siblings. check their type */
        {
          el = first;
          while (el && !done)
            {
              elType = TtaGetElementType (el);
              if ((elType.ElTypeNum == MathML_EL_MTEXT ||
                   elType.ElTypeNum == MathML_EL_MI ||
                   elType.ElTypeNum == MathML_EL_MN ||
                   elType.ElTypeNum == MathML_EL_MO) &&
                  strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
                {
                  if (el == last)
                    el = NULL;
                  else
                    TtaNextSibling (&el);
                }
              else
                /* can't transform that element. Don't transform anything */
                done = TRUE;
            }
          if (!done)
            /* all selected elements are MTEXT, MI, MN, or MO and they are
               siblings */
            {
              TtaUnselect (doc);
              elType = TtaGetElementType (lastSel);
              if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
                /* the last selected element is a character string.
                   Split it, as well as its parent (mtext, mi, mo, mn) */
                {
                  if (lastChar == 0 && (firstChar == 0 || firstSel != lastSel))
                    lastChar = TtaGetElementVolume (lastSel);
                  el = SplitTextInMathML (doc, lastSel, lastChar, &mrowCreated);
                } 
              else if (elType.ElTypeNum == MathML_EL_MGLYPH)
                {
                  if (lastChar == 0)
                    lastChar = TtaGetElementVolume (lastSel);
                  el = SplitTextInMathML (doc, lastSel, lastChar, &mrowCreated);
                }
              elType = TtaGetElementType (firstSel);
              if ((elType.ElTypeNum == MathML_EL_TEXT_UNIT ||
                   elType.ElTypeNum == MathML_EL_MGLYPH) &&
                  strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
                /* the first selected element is a character string.
                   Split it, as well as its parent (mtext, mi, mo, mn) */
                {
                  same = (first == last);
                  leaf = SplitTextInMathML (doc, firstSel, firstChar, &mrowCreated);
                  if (leaf)
                    {
                      first = TtaGetParent (leaf);
                      if (same)
                        last = first;
                    }
                }
              /* create a new element of the requested type */
              elType.ElTypeNum = typeNum;
              newEl = TtaNewElement (doc, elType);
              /* insert it after the last element selected */
              TtaInsertSibling (newEl, last, FALSE, doc);
              /* move the content of all selected elements into the new element
                 and delete the selected elements */
              el = first;
              lastLeaf = NULL;
              while (el)
                {
                  /* remember the next element to be processed */
                  if (el == last)
                    nextEl = NULL;
                  else
                    {
                      nextEl = el;
                      TtaNextSibling (&nextEl);
                    }
                  /* register in the undo queue the element that will be deleted */
                  TtaRegisterElementDelete (el, doc);
                  /* move the content of that element */
                  leaf = TtaGetFirstChild (el);
                  while (leaf)
                    {
                      nextLeaf = leaf;
                      TtaNextSibling (&nextLeaf);
                      TtaRemoveTree (leaf, doc);
                      if (lastLeaf)
                        TtaInsertSibling (leaf, lastLeaf, FALSE, doc);
                      else
                        TtaInsertFirstChild (&leaf, newEl, doc);
                      lastLeaf = leaf;
                      leaf = nextLeaf;
                    }
                  TtaDeleteTree (el, doc);
                  el = nextEl;
                }
              MathSetAttributes (newEl, doc, NULL);
              TtaRegisterElementCreate (newEl, doc);
              /* if there is a parent MROW element, delete it if it's no longer
                 needed */
              parent = TtaGetParent (newEl);
              CheckMROW (&parent, doc);
              /* select the new element */
              selEl = newEl;
            } 
        }
    }
 
  TtaSetStructureChecking (oldStructureChecking, doc);
  TtaCloseUndoSequence (doc);
  TtaSetDisplayMode (doc, DisplayImmediately);
  if (selEl)
    TtaSelectElement (doc, selEl);
}

/*----------------------------------------------------------------------
  CreateMTEXT
  -----------------------------------------------------------------------*/
void CreateMTEXT (Document document, View view)
{
  CreateCharStringElement (MathML_EL_MTEXT, document, view);
}

/*----------------------------------------------------------------------
  CreateMI
  -----------------------------------------------------------------------*/
void CreateMI (Document document, View view)
{
  CreateCharStringElement (MathML_EL_MI, document, view);
}

/*----------------------------------------------------------------------
  CreateMN
  -----------------------------------------------------------------------*/
void CreateMN (Document document, View view)
{
  CreateCharStringElement (MathML_EL_MN, document, view);
}

/*----------------------------------------------------------------------
  CreateMO
  -----------------------------------------------------------------------*/
void CreateMO (Document document, View view)
{
  CreateCharStringElement (MathML_EL_MO, document, view);
}

/*----------------------------------------------------------------------
  CreateMSPACE
  -----------------------------------------------------------------------*/
void CreateMSPACE (Document document, View view)
{
  CreateMathConstruct (document, view, 19);
}

/*----------------------------------------------------------------------
  GlobalMathAttrInMenu
  Called by Thot when building the Attributes menu.
  Prevent Thot from including a global attribute in the menu if the selected
  element does not accept this attribute.
  ----------------------------------------------------------------------*/
ThotBool  GlobalMathAttrInMenu (NotifyAttribute *event)
{
  ElementType         elType, parentType;
  Element             parent;
  const char         *attr;

  elType = TtaGetElementType (event->element);

  /* don't put any attribute on Thot elements that are not MathML elements */
  if (elType.ElTypeNum == MathML_EL_XMLcomment ||
      elType.ElTypeNum == MathML_EL_XMLcomment_line ||
      elType.ElTypeNum == MathML_EL_XMLPI ||
      elType.ElTypeNum == MathML_EL_XMLPI_line ||
      elType.ElTypeNum == MathML_EL_Unknown_namespace ||
      elType.ElTypeNum == MathML_EL_DOCTYPE ||
      elType.ElTypeNum == MathML_EL_DOCTYPE_line)
    return TRUE;

  /* don't put any attribute on text fragments that are within DOCTYPE,
     comments, PIs, etc. */
  if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
    {
      parent = TtaGetParent (event->element);
      if (parent)
        {
          parentType = TtaGetElementType (parent);
          if (parentType.ElTypeNum == MathML_EL_XMLcomment ||
              parentType.ElTypeNum == MathML_EL_XMLcomment_line ||
              parentType.ElTypeNum == MathML_EL_XMLPI ||
              parentType.ElTypeNum == MathML_EL_XMLPI_line ||
              parentType.ElTypeNum == MathML_EL_Unknown_namespace ||
              parentType.ElTypeNum == MathML_EL_DOCTYPE ||
              parentType.ElTypeNum == MathML_EL_DOCTYPE_line)
            return TRUE;
        }
    }

  attr = GetXMLAttributeName (event->attributeType, elType, event->document);
  if (attr[0] == EOS)
    return TRUE;	/* don't put an invalid attribute in the menu */

  /* handle only Global attributes */
  if (event->attributeType.AttrTypeNum != MathML_ATTR_class &&
      event->attributeType.AttrTypeNum != MathML_ATTR_style_ &&
      event->attributeType.AttrTypeNum != MathML_ATTR_id &&
      event->attributeType.AttrTypeNum != MathML_ATTR_xref &&
      event->attributeType.AttrTypeNum != MathML_ATTR_other &&
      event->attributeType.AttrTypeNum != MathML_ATTR_xml_space)
    /* it's not a global attribute. Accept it */
#ifdef TEMPLATES
    return CheckTemplateAttrInMenu(event);
#else /* TEMPLATES */
    return FALSE;
#endif /* TEMPLATES */

  if (strcmp (TtaGetSSchemaName (elType.ElSSchema),"MathML"))
    /* it's not a MathML element */
    return TRUE;
  else
    /* it's a MathML element */
    {
      /*  Construct, TableRow, and MathMLCharacters do not accept any
          global attribute */
      if (elType.ElTypeNum == MathML_EL_Construct ||
          elType.ElTypeNum == MathML_EL_Construct1 ||
          elType.ElTypeNum == MathML_EL_TableRow ||
          elType.ElTypeNum == MathML_EL_MathMLCharacters)
        return TRUE;
    }
#ifdef TEMPLATES
  return CheckTemplateAttrInMenu(event);
#else /* TEMPLATES */
  return FALSE;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  MathMoveForward
  Moves the caret to the next position in the MathML structure
  -----------------------------------------------------------------------*/
static ThotBool MathMoveForward ()
{
  Document      doc;
  Element       el, nextEl, leaf, ancestor, sibling, selected;
  ElementType   elType, successorType;
  char         *s;
  int           firstChar, lastChar, len, i;
  NotifyElement event;
  ThotBool      done, found, ok;

  done = FALSE;
  selected = NULL;
  doc = TtaGetSelectedDocument ();
  TtaGiveLastSelectedElement (doc, &el, &firstChar, &lastChar); 
  elType = TtaGetElementType (el);
  if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
    return (FALSE);
  if (elType.ElTypeNum == MathML_EL_TEXT_UNIT ||
      elType.ElTypeNum == MathML_EL_SYMBOL_UNIT)
    /* the caret is in a character string */
    {
      len = TtaGetElementVolume (el);
      if (lastChar < len)
        /* the caret is not at the end of the string, move it to the
           next character in the string */
        {
          if (lastChar < firstChar)
            /* a caret */
            TtaSelectString (doc, el, firstChar + 1, firstChar);
          else
            TtaSelectString (doc, el, lastChar + 1, lastChar);
          selected = el;
          done = TRUE;
        }
    }
  if (!done)
    /* get the following element in the tree structure */
    {
      /* if there is an empty Construct leaf among the next few elements,
         choose it  */
      sibling = el;
      nextEl = NULL;
      for (i = 1; i < 4 && sibling && !nextEl; i++)
        {
          sibling = TtaGetFirstLeaf(TtaGetSuccessor (sibling));
	  /* @@@@@ sucessor = comment @@@@ */
          if (sibling)
            {
              successorType = TtaGetElementType (sibling);
              if (strcmp (TtaGetSSchemaName(successorType.ElSSchema),"MathML"))
                /* we are no longer in the math expression. Do not go further*/
                sibling = NULL;
              else
                if (successorType.ElTypeNum == MathML_EL_Construct ||
                    successorType.ElTypeNum == MathML_EL_Construct1)
                  /* this an empy construct */
                  {
		    /* @@@@@@   attrType.AttrSSchema = successorType.ElSSchema;
                    attrType.AttrTypeNum = MathML_ATTR_IntPlaceholder;
                    if (!TtaGetAttribute (sibling, attrType))   @@@@ */
                      /* and it is not a placeholder. Take it. */
                      nextEl = sibling;
                  }
            }
        }
      if (!nextEl)
        nextEl = TtaGetSuccessor (el);
      /* skip comments */
      if (nextEl)
        {
          ok = FALSE;
          do
            {
              successorType = TtaGetElementType (nextEl);
              if ((successorType.ElTypeNum == MathML_EL_XMLcomment ||
                   successorType.ElTypeNum == MathML_EL_Overscript ||
                   successorType.ElTypeNum == MathML_EL_Superscript ||
                   successorType.ElTypeNum == MathML_EL_MO) &&
                  !strcmp (TtaGetSSchemaName (successorType.ElSSchema), "MathML"))
                nextEl = TtaGetSuccessor (nextEl);
              else
                ok = TRUE;
            }
          while (nextEl && !ok);
        }
	
      /* are we in a mroot? */
      elType.ElTypeNum = MathML_EL_MROOT;
      ancestor = TtaGetTypedAncestor (el, elType);
      if (ancestor)
        /* we are in a mroot. Are we at the end of the Index or RootBase? */
        {
          found = FALSE;
          do
            {
              sibling = el;
              elType = TtaGetElementType (el);
              TtaNextSibling (&sibling);
              if (!strcmp (TtaGetSSchemaName (elType.ElSSchema),"MathML") &&
                  (elType.ElTypeNum == MathML_EL_Index ||
                   elType.ElTypeNum == MathML_EL_RootBase))
                found = TRUE;
              else if (!sibling)
                el = TtaGetParent (el);
            }
          while (!sibling && !found && el);
          if (found)
            {
              if (elType.ElTypeNum == MathML_EL_Index)
                {
                  // Index follows RootBase
                  nextEl = el;
                  TtaPreviousSibling (&nextEl);
                }
              else
                nextEl = TtaGetSuccessor (ancestor);
            }
          else
            nextEl = sibling;
        }
      if (nextEl)
        {
          elType = TtaGetElementType (nextEl);
          s = TtaGetSSchemaName (elType.ElSSchema);
          if (!strcmp (s, "MathML") &&
              (elType.ElTypeNum == MathML_EL_MSPACE ||
               elType.ElTypeNum == MathML_EL_MGLYPH ||
               elType.ElTypeNum == MathML_EL_MALIGNMARK ||
               elType.ElTypeNum == MathML_EL_MALIGNGROUP))
            /* select the element itself, not its contents */
            {
              TtaSelectElement (doc, nextEl);
              selected = nextEl;
              done = TRUE;
            }
          else
            {
              if (!strcmp (s, "MathML") &&
                  elType.ElTypeNum == MathML_EL_MTABLE)
                /* don't select within hidden element MTable_head. Skip it */
                {
                  nextEl = TtaGetFirstChild (nextEl);
                  if (nextEl)
                    nextEl = TtaGetSuccessor (nextEl);
                  elType = TtaGetElementType (nextEl);
                  s = TtaGetSSchemaName (elType.ElSSchema);
                }

              if (nextEl)
                {
                  if (elType.ElTypeNum == MathML_EL_TEXT_UNIT ||
                      elType.ElTypeNum == MathML_EL_SYMBOL_UNIT)
                    {
                      /* put the caret before the first character in the
                         string */
                      TtaSelectString (doc, nextEl, 1, 0);
                      selected = nextEl;
                      done = TRUE;
                    }
                  else if (!strcmp (s, "MathML") &&
                           elType.ElTypeNum == MathML_EL_Construct)
                    {
                      /* select the whole leaf */
                      TtaSelectElement (doc, nextEl);
                      selected = nextEl;
                      done = TRUE;
                    }
                  else
                    {
                      /* if it's a mroot, move to the Index, not the RootBase */
                      if (!strcmp (s, "MathML") &&
                          elType.ElTypeNum == MathML_EL_MROOT)
                        nextEl = TtaGetLastChild (nextEl);
                      /* get the first leaf in that element */
                      leaf = TtaGetFirstLeaf (nextEl);
                      if (leaf && leaf != nextEl)
                        {
                          elType = TtaGetElementType (leaf);
                          if (elType.ElTypeNum == MathML_EL_TEXT_UNIT ||
                              elType.ElTypeNum == MathML_EL_SYMBOL_UNIT)
                            /* put the caret before the first character in the
                               string */
                            TtaSelectString (doc, leaf, 1, 0);
                          else
                            /* select the whole leaf */
                            TtaSelectElement (doc, leaf);
                          selected = leaf;
                          done = TRUE;
                        }
                    }
                }
            }
        }
    }
  if (done)
    {
      event.document = doc;
      event.element = selected;
      MathSelectionChanged (&event);
    }
  return (done);
}

/*----------------------------------------------------------------------
  MathMoveBackward
  Moves the caret to the previous position in the MathML structure
  -----------------------------------------------------------------------*/
static ThotBool MathMoveBackward ()
{
  Document      doc;
  Element       el, prevEl, leaf, ancestor, sibling, selected;
  ElementType   elType, predecType;
  char         *s;
  int           firstChar, lastChar, len;
  NotifyElement event;
  ThotBool      done, found, ok;

  done = FALSE;
  selected = NULL; 
  doc = TtaGetSelectedDocument ();
  TtaGiveFirstSelectedElement (doc, &el, &firstChar, &lastChar); 
  elType = TtaGetElementType (el);
  if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
    return (FALSE);
  if (elType.ElTypeNum == MathML_EL_TEXT_UNIT ||
      elType.ElTypeNum == MathML_EL_SYMBOL_UNIT)
    /* the caret is in a text string */
    {
      if (firstChar > 1)
        /* the caret is not at the beginning of the string. Move it to
           the previous character in the string */
        {
          TtaSelectString (doc, el, firstChar - 1, firstChar - 2);
          selected = el;
          done = TRUE;
        }
    }
  if (!done)
    {
      /* get the previous element in the tree structure */
      prevEl = TtaGetPredecessor (el);
      /* skip comments */
      if (prevEl)
        {
          ok = FALSE;
          do
            {
              predecType = TtaGetElementType (prevEl);
              if ((predecType.ElTypeNum == MathML_EL_XMLcomment ||
                   predecType.ElTypeNum == MathML_EL_Overscript ||
                   predecType.ElTypeNum == MathML_EL_Superscript ||
                   predecType.ElTypeNum == MathML_EL_MO) &&
                  !strcmp (TtaGetSSchemaName (predecType.ElSSchema), "MathML"))
                prevEl = TtaGetPredecessor (prevEl);
              else
                ok = TRUE;
            }
          while (prevEl && !ok);
        }
	
      /* are we in a mroot? */
      elType.ElTypeNum = MathML_EL_MROOT;
      ancestor = TtaGetTypedAncestor (el, elType);
      if (ancestor)
        /* we are in a mroot. Are we at the beginning of the Index or
           RootBase? */
        {
          found = FALSE;
          do
            {
              sibling = el;
              elType = TtaGetElementType (el);
              TtaPreviousSibling (&sibling);
              if (!strcmp (TtaGetSSchemaName (elType.ElSSchema),"MathML") &&
                  (elType.ElTypeNum == MathML_EL_Index ||
                   elType.ElTypeNum == MathML_EL_RootBase))
                    found = TRUE;
              else if (!sibling)
                el = TtaGetParent (el);
            }
          while (!sibling && !found && el);
          if (found)
            {
              if (elType.ElTypeNum == MathML_EL_RootBase)
                {
                  // Index follows RootBase
                  prevEl = el;
                  TtaNextSibling (&prevEl);
                }
              else
                prevEl = TtaGetPredecessor (ancestor);
            }
          else
            prevEl = sibling;
        }
      if (prevEl)
        {
          elType = TtaGetElementType (prevEl);
          s = TtaGetSSchemaName (elType.ElSSchema);
          if (!strcmp (s, "MathML") &&
              (elType.ElTypeNum == MathML_EL_MSPACE ||
               elType.ElTypeNum == MathML_EL_MGLYPH ||
               elType.ElTypeNum == MathML_EL_MALIGNMARK ||
               elType.ElTypeNum == MathML_EL_MALIGNGROUP))
            /* select the element itself, not its contents */
            {
              TtaSelectElement (doc, prevEl);
              selected = prevEl;
              done = TRUE;
            }
          else
            {
              if (!strcmp (s, "MathML") &&
                  elType.ElTypeNum == MathML_EL_MTable_head)
                {
                  /* don't select within hidden element MTable_head. Skip it */
                  prevEl = TtaGetPredecessor (prevEl);
                  elType = TtaGetElementType (prevEl);
                  s = TtaGetSSchemaName (elType.ElSSchema);
                }

              if (prevEl)
                {
                  if (elType.ElTypeNum == MathML_EL_TEXT_UNIT ||
                      elType.ElTypeNum == MathML_EL_SYMBOL_UNIT)
                    {
                      /* put the caret before the first character in the
                       string */
                      TtaSelectString (doc, prevEl, 1, 0);
                      selected = prevEl;
                      done = TRUE;
                    }
                  else if (!strcmp (s, "MathML") &&
                           elType.ElTypeNum == MathML_EL_Construct)
                    {
                      /* select the whole leaf */
                      TtaSelectElement (doc, prevEl);
                      selected = prevEl;
                      done = TRUE;
                    }
                  else
                    {
                      /* if it's a mroot, move to the RootBase, not the Index */
                      if (!strcmp (TtaGetSSchemaName (elType.ElSSchema),"MathML")&&
                          elType.ElTypeNum == MathML_EL_MROOT)
                        prevEl = TtaGetFirstChild (prevEl);
                      /* get the last leaf in that element */
                      leaf = TtaGetLastLeaf (prevEl);
                      if (leaf && leaf != prevEl)
                        {
                          elType = TtaGetElementType (leaf);
                          if (elType.ElTypeNum == MathML_EL_TEXT_UNIT ||
                              elType.ElTypeNum == MathML_EL_SYMBOL_UNIT)
                            /* put the caret at the end of the string */
                            {
                              len = TtaGetElementVolume (leaf);
                              TtaSelectString (doc, leaf, len + 1, len);
                            }
                          else
                            /* select the whole leaf */
                            TtaSelectElement (doc, leaf);
                          selected = leaf;
                          done = TRUE;
                        }
                    }
                }
            }
        }
    }
  if (done && selected)
    {
      event.document = doc;
      event.element = selected;
      MathSelectionChanged (&event);
    }
  return (done);
}
/*----------------------------------------------------------------------
  FreeMathML free MathML context.
  ----------------------------------------------------------------------*/
void FreeMathML ()
{
#if defined(_WX)
  /*
    if (iconMath)
    delete iconMath;
    if (iconMathNo)
    delete iconMathNo;
    for (int i = 0 ; i < 14 ; i++)
    {
    if (mIcons[i])
    delete mIcons[i];
    }

    iconMath =   (ThotIcon)NULL;
    iconMathNo = (ThotIcon)NULL;
    memset( mIcons, 0, 14 * sizeof(ThotIcon) );
  */
#endif /* defined(_WX) */
}

/*----------------------------------------------------------------------
  InitMathML initializes MathML context.           
  ----------------------------------------------------------------------*/
void InitMathML ()
{
  MathsDialogue = TtaSetCallback ((Proc)CallbackMaths, MAX_MATHS);
  TtaSetMoveForwardCallback ((Func) MathMoveForward);
  TtaSetMoveBackwardCallback ((Func) MathMoveBackward);
}

/*----------------------------------------------------------------------
  GetCharType
  returns the type of character c (MN, MI or MO).
  -----------------------------------------------------------------------*/
static int GetCharType (CHAR_T c, char script)
{
  int	ret;

  ret = MathML_EL_MO;
  if (c >= '0' && c <= '9')
    /* decimal digit */
    ret = MathML_EL_MN;
  else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ' ')
    /* latin letter */
    ret = MathML_EL_MI;
  else if (((c >= 0x0391 && c <= 0x03A9) || (c >= 0x03B1 && c <= 0x03C9) ||
           (c == 0x03D5)) &&
	   c != 0x03a0 && c != 0x03a3)
    /* greek letter, except capital Sigma and Pi, which are considered as
       operators */
    ret = MathML_EL_MI;
  else if (c >= 0x0410 && c <= 0x044F)
    /* cyrillic letter */
    ret = MathML_EL_MI;
  else if (c >= 0xC0 && c <= 0xFF &&  /* accented latin letter */
           c != 0xD7 && c != 0xF7)    /* but not operators times or divide */
    ret = MathML_EL_MI;
  else
    ret = MathML_EL_MO;
  return ret;
}

/*----------------------------------------------------------------------
  MergeMathEl
  merge element el2 with element el
  -----------------------------------------------------------------------*/
static void MergeMathEl (Element el, Element el2, ThotBool before,
                         Element *newSelEl, int *newSelChar, Document doc)
{
  Element	textEl2, nextEl, prevEl, nEl;
  int           len;

  TtaRegisterElementReplace (el, doc);
  TtaRegisterElementDelete (el2, doc);
  textEl2 = TtaGetFirstChild (el2);
  if (before)
    prevEl = NULL;
  else
    prevEl = TtaGetLastChild (el);
  if (*newSelEl == el2)
    *newSelEl = el;
  while (textEl2 != NULL)
    {
      nextEl = textEl2;
      TtaNextSibling (&nextEl);
      TtaRemoveTree (textEl2, doc);
      if (prevEl == NULL)
        TtaInsertFirstChild (&textEl2, el, doc);
      else
        {
          TtaInsertSibling (textEl2, prevEl, FALSE, doc);
          len = TtaGetElementVolume (prevEl);
          if (TtaMergeText (prevEl, doc))
            {
              if (*newSelEl == textEl2)
                {
                  *newSelEl = prevEl;
                  *newSelChar += len;
                }
              textEl2 = prevEl;
            }
        }
      prevEl = textEl2;
      textEl2 = nextEl;
    }
  if (before && prevEl)
    {
      nEl = prevEl;
      TtaNextSibling (&nEl);
      if (nEl)
        {
          len = TtaGetElementVolume (prevEl);
          if (TtaMergeText (prevEl, doc))
            if (*newSelEl == nEl)
              {
                *newSelEl = prevEl;
                *newSelChar += len;
              }
        }
    }
  TtaDeleteTree (el2, doc);
  MathSetAttributes (el, doc, NULL);
}

/*----------------------------------------------------------------------
  ClosestLeaf
  return the Closest TEXT element for element el;
  -----------------------------------------------------------------------*/
static Element ClosestLeaf (Element el, int* pos)
{
  Element	elem, prev, next, child, leaf, parent;
  ElementType	elType;
  ThotBool     ok;

  elem = NULL;
  leaf = NULL;
  prev = NULL;
  next = NULL;
  parent = el;
  do
    {
      prev = parent;  TtaPreviousSibling (&prev);
      if (prev)
        {
          /* avoid selecting FencedSeparator elements or comments */
          ok = FALSE;
          do
            {
              elType = TtaGetElementType (prev);
              if ((elType.ElTypeNum == MathML_EL_FencedSeparator ||
                   elType.ElTypeNum == MathML_EL_XMLcomment) &&
                  !strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
                TtaPreviousSibling (&prev);
              else
                ok = TRUE;
            }
          while (prev && !ok);
          /* avoid selecting an MF element */
          if (prev)
            if (elType.ElTypeNum == MathML_EL_MF &&
                !strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
              prev = NULL;
        }
      if (!prev)
        {
          next = parent;  TtaNextSibling (&next);
          if (next)
            {
              /* avoid selecting FencedSeparator elements or comments */
              ok = FALSE;
              do
                {
                  elType = TtaGetElementType (next);
                  if ((elType.ElTypeNum == MathML_EL_FencedSeparator ||
                       elType.ElTypeNum == MathML_EL_XMLcomment) &&
                      !strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
                    TtaNextSibling (&next);
                  else
                    ok = TRUE;
                }
              while (next && !ok);
            }
          if (next == NULL)
            parent = TtaGetParent (parent);
        }
    }
  while (next == NULL && prev == NULL && parent != NULL);
  if (prev != NULL)
    {
      child = prev;
      while (child != NULL)
        {
          leaf = child;
          child = TtaGetLastChild (child);
        }
    }
  else
    if (next != NULL)
      {
        child = next;
        while (child != NULL)
          {
            leaf = child;
            child = TtaGetFirstChild (child);
          }
      }
  if (leaf != NULL)
    {
      elem = leaf;
      elType = TtaGetElementType (leaf);
      if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
        {
          if (prev != NULL)
            *pos = TtaGetElementVolume (leaf) + 1;
          else
            *pos = 1;
        }
    }
  return elem;
}

/*----------------------------------------------------------------------
  SeparateFunctionNames
   
  -----------------------------------------------------------------------*/
static void SeparateFunctionNames (Element *firstEl, Element lastEl,
                                   Document doc, Element *newSelEl,
                                   int *newSelChar)
{
  ElementType    elType;
  Element        el, nextEl, textEl, nextTextEl, firstTextEl, newEl, newText,
    cur, prev, next, prevEl, currentMI, prevText;
  Language       lang;
  int            len, flen, firstChar, i, func;
#define BUFLEN 200
  unsigned char  text[BUFLEN];
  ThotBool       split, stop, firstElChanged, leafSplit;

  el = *firstEl;
  firstElChanged = FALSE;
  prevText = NULL;
  /* process all sibling elements from firstEl to lastEl */
  while (el)
    {
      /* check the next element to be processed */
      if (el == lastEl)
        nextEl = NULL;
      else
        {
          nextEl = el;
          TtaNextSibling (&nextEl);
        }
      currentMI = NULL;
      /* only MI elements may contain function names */
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == MathML_EL_MI &&
          strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
        {
          split = FALSE;
          /* if the MI element has to be split, its pieces will replace
             the element itself */
          prevEl = el;
          textEl = TtaGetFirstChild (el);
          firstTextEl = NULL;
          /* process all children of this MI element */
          while (textEl)
            {
              if (!firstTextEl)
                firstTextEl = textEl;
              /* check the next child to be processed */
              nextTextEl = textEl;
              TtaNextSibling (&nextTextEl);
              /* only text leaves are interesting */
              elType = TtaGetElementType (textEl);
              if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
                {
                  leafSplit = FALSE;
                  /* get the content of this text leaf and analyze it */
                  len = BUFLEN - 1;
                  TtaGiveTextContent (textEl, text, &len, &lang);
                  firstChar = 0;
                  /* ignore text leaves that are shorter than the shortest
                     function name (2 characters) */
                  if (len > 1)
                    {
                      /* check all possible substrings of the text leaf */
                      for (i = 0; i < len-1; i++)
                        {
                          func = 0;
                          /* compare a substring with all function names */
                          stop = FALSE;
                          do
                            {
                              flen = strlen (functionName[func]);
                              if (!strncmp ((char *)functionName[func], (char *)&text[i],flen))
                                /* this substring is a function name */
                                {
                                  /* this element will be deleted */
                                  if (!split)
                                    TtaRegisterElementDelete (el, doc);
                                  /* create a MI for all text pieces preceding
                                     the function name  */
                                  newEl = NULL;
                                  prev = NULL;
                                  if (firstTextEl && firstTextEl != textEl)
                                    {
                                      elType.ElTypeNum = MathML_EL_MI;
                                      newEl = TtaNewElement (doc, elType);
                                      TtaInsertSibling (newEl, prevEl, FALSE,
                                                        doc);
                                      prevEl = newEl;
                                      cur = firstTextEl;
                                      next = cur; TtaNextSibling (&next);
                                      TtaRemoveTree (cur, doc);
                                      TtaInsertFirstChild (&cur, newEl, doc);
                                      prev = cur;
                                      cur = next;
                                      while (cur && cur != textEl)
                                        {
                                          next = cur; TtaNextSibling (&next);
                                          TtaRemoveTree (cur, doc);
                                          TtaInsertSibling (cur, prev, FALSE,
                                                            doc);
                                          /* if the selection is in this part
                                             of the original element, move it
                                             to the new piece of text */
                                          if (cur == *newSelEl)
                                            *newSelEl = cur;
                                          prev = cur;
                                          cur = next;
                                        }
                                      TtaRegisterElementCreate (newEl, doc);
                                      if (!firstElChanged)
                                        {
                                          *firstEl = newEl;
                                          firstElChanged = TRUE;
                                        }
                                    }
                                  firstTextEl = NULL;
                                  if (i > firstChar)
                                    {
                                      elType.ElTypeNum = MathML_EL_MI;
                                      newEl = TtaNewElement (doc, elType);
                                      TtaInsertSibling (newEl, prevEl, FALSE,
                                                        doc);
                                      prevEl = newEl;
                                      elType.ElTypeNum = MathML_EL_TEXT_UNIT;
                                      newText = TtaNewElement (doc, elType);
                                      TtaInsertFirstChild (&newText, newEl,
                                                           doc);
                                      text[i] = EOS;
                                      TtaSetTextContent (newText,
                                                         (unsigned char *)&text[firstChar],
                                                         lang, doc);
                                      MathSetAttributes (newEl, doc, NULL);
                                      TtaRegisterElementCreate (newEl, doc);
                                      if (!firstElChanged)
                                        {
                                          *firstEl = newEl;
                                          firstElChanged = TRUE;
                                        }
                                      /* if the selection is in this part of
                                         the original element, move it to the
                                         new piece of text */
                                      if (textEl == *newSelEl)
                                        if (*newSelChar - 1 <= i)
                                          {
                                            *newSelEl = newText;
                                            *newSelChar = *newSelChar - firstChar;
                                          }
                                    }
                                  /* create a MI for the function name itself*/
                                  if (!strcmp(functionName[func], "lim") ||
                                      !strcmp(functionName[func], "min") ||
                                      !strcmp(functionName[func], "max"))
                                    /* create a MO for lim, min or max */
                                    elType.ElTypeNum = MathML_EL_MO;
                                  else
                                    elType.ElTypeNum = MathML_EL_MI;
                                  newEl = TtaNewElement (doc, elType);
                                  TtaInsertSibling (newEl, prevEl, FALSE, doc);
                                  prevEl = newEl;
                                  elType.ElTypeNum = MathML_EL_TEXT_UNIT;
                                  newText = TtaNewElement (doc, elType);
                                  TtaInsertFirstChild (&newText, newEl, doc);
                                  TtaSetTextContent (newText,
                                                     (unsigned char *)functionName[func],
                                                     lang, doc);
                                  MathSetAttributes (newEl, doc, NULL);
                                  TtaRegisterElementCreate (newEl, doc);
                                  if (!firstElChanged)
                                    {
                                      *firstEl = newEl;
                                      firstElChanged = TRUE;
                                    }
                                  /* if the selection is in this part of the
                                     original element, move it to the new
                                     piece of text */
                                  firstChar = i + flen;
                                  if (textEl == *newSelEl)
                                    if (*newSelChar - 1 <= firstChar)
                                      {
                                        *newSelEl = newText;
                                        *newSelChar = *newSelChar - i;
                                      }
                                  i = firstChar - 1;
                                  split = TRUE;
                                  leafSplit = TRUE;
                                  stop = TRUE;
                                }
                              func++; 
                            }
                          while (!stop &&
                                 strcmp (functionName[func], "") != 0);
                        }
                    }
                  /* the whole text leaf has been checked */
                  /* create a MI for the remaining text if any */
                  if (leafSplit && firstChar < len)
                    {
                      elType.ElTypeNum = MathML_EL_MI;
                      newEl = TtaNewElement (doc, elType);
                      TtaInsertSibling (newEl, prevEl, FALSE, doc);
                      prevEl = newEl;
                      elType.ElTypeNum = MathML_EL_TEXT_UNIT;
                      newText = TtaNewElement (doc, elType);
                      TtaInsertFirstChild (&newText, newEl, doc);
                      TtaSetTextContent (newText, (unsigned char *)&text[firstChar], lang, doc);
                      MathSetAttributes (newEl, doc, NULL);
                      TtaRegisterElementCreate (newEl, doc);
                      prevText = newText;
                      currentMI = newEl;
                      /* if the selection is in this part of the original
                         element, move it to the new piece of text */
                      if (textEl == *newSelEl)
                        if (*newSelChar - 1 <= len)
                          {
                            *newSelEl = newText;
                            *newSelChar = *newSelChar - firstChar;
                          }
                    }
                  if (split && !leafSplit)
                    {
                      TtaRemoveTree (textEl, doc);
                      if (!currentMI)
                        {
                          elType.ElTypeNum = MathML_EL_MI;
                          currentMI = TtaNewElement (doc, elType);
                          TtaInsertSibling (currentMI, prevEl, FALSE, doc);
                          TtaInsertFirstChild (&textEl, currentMI, doc);
                          MathSetAttributes (currentMI, doc, NULL);
                          TtaRegisterElementCreate (currentMI, doc);
                          prevEl = currentMI;
                        }
                      else
                        TtaInsertSibling (textEl, prevText, FALSE, doc);
                      prevText = textEl;
                    }
                }
              textEl = nextTextEl;
            }
          if (split)
            /* the original MI element has been replaced by its pieces.
               Delete it */
            TtaDeleteTree (el, doc);
        }
      el = nextEl;
    }
}

/*----------------------------------------------------------------------
  ParseMathString
  The content of an element MTEXT, MI, MO, or MN has been modified
  or created.
  Parse the new content and create the appropriate MI, MO, MN elements
  according to the new contents.
  -----------------------------------------------------------------------*/
static void ParseMathString (Element theText, Element theElem, Document doc)
{
  Element       el, selEl, prevEl, nextEl, textEl, newEl, lastEl;
  Element       firstEl, newSelEl, prev, next, parent, placeholderEl;
  ElementType   elType, elType2;
  AttributeType attrType;
  Attribute     attr;
  SSchema	MathMLSchema;
  int		firstSelChar, lastSelChar, newSelChar, len, totLen, i, j;
  int           start, trailingSpaces;
  char	        script;
  CHAR_T        c;
  Language	lang;
#define TXTBUFLEN 100
  CHAR_T        text[TXTBUFLEN];
  Language      language[TXTBUFLEN];
  char          mathType[TXTBUFLEN];
  ThotBool      oldStructureChecking;
  ThotBool      empty, closeUndoSeq, separate, ok, leadingSpace;

  /************************************************************/
  if (CurrentMathEditMode != DEFAULT_MODE)
    {
      TtaSetDisplayMode (doc, DeferredDisplay);
      oldStructureChecking = TtaGetStructureChecking (doc);
      TtaSetStructureChecking (FALSE, doc);
      newEl = InsertMathElementFromText(theElem, theText, doc);
      if(newEl)
	{
	  nextEl = InsertPlaceholder (newEl, FALSE, doc, TRUE);
	  TtaSelectElement (doc, nextEl);
	}
	
      TtaSetStructureChecking (oldStructureChecking, doc);
      TtaSetDisplayMode (doc, DisplayImmediately);
      return;
    }
  /**************************************************************/

  elType = TtaGetElementType (theElem);
  MathMLSchema = elType.ElSSchema;
  if (elType.ElTypeNum == MathML_EL_MTEXT)
    {
      attrType.AttrSSchema = MathMLSchema;
      attrType.AttrTypeNum = MathML_ATTR_IntParseMe;
      attr = TtaGetAttribute (theElem, attrType);
      if (!attr)
        /* this MTEXT element does not need to be parsed */
        return;
      else
        TtaRemoveAttribute (theElem, attr, doc);
    }

  /* get the current selection */
  TtaGiveFirstSelectedElement (doc, &selEl, &firstSelChar, &lastSelChar);
  newSelEl = NULL;

  prevEl = NULL;
  el = theElem;
  TtaPreviousSibling (&el);
  if (el != NULL)
    {
      elType = TtaGetElementType (el);
      if (elType.ElSSchema == MathMLSchema &&
          (elType.ElTypeNum == MathML_EL_MTEXT ||
           elType.ElTypeNum == MathML_EL_MI ||
           elType.ElTypeNum == MathML_EL_MO ||
           elType.ElTypeNum == MathML_EL_MN))
        prevEl = el;
    }
  nextEl = NULL;
  el = theElem;
  TtaNextSibling (&el);
  if (el != NULL)
    {
      elType = TtaGetElementType (el);
      if (elType.ElSSchema == MathMLSchema &&
          (elType.ElTypeNum == MathML_EL_MTEXT ||
           elType.ElTypeNum == MathML_EL_MI ||
           elType.ElTypeNum == MathML_EL_MO ||
           elType.ElTypeNum == MathML_EL_MN))
        nextEl = el;
    }

  totLen = 0;
  elType = TtaGetElementType (theElem);
  elType2 = TtaGetElementType (theText);
  if (elType2.ElTypeNum == MathML_EL_TEXT_UNIT)
    textEl = theText;
  else if (elType2.ElTypeNum == MathML_EL_MGLYPH &&
           elType2.ElSSchema == MathMLSchema)
    textEl = TtaGetFirstChild (theText);
  else
    textEl = NULL;
  if (textEl != NULL)
    {
      len = TtaGetElementVolume (textEl);
      /* selection */
      if (selEl == textEl)
        {
          newSelChar = firstSelChar;
          newSelEl = textEl;
          TtaUnselect (doc);
        }
      /* get the content and analyze it */
      if (len > 0)
        {
          len = TXTBUFLEN;
          TtaGiveBufferContent (textEl, text, len, &lang);
          len = ustrlen (text);
          script = TtaGetScript (lang);
          leadingSpace = TRUE;
          for (j = 0; j < len; j++)
            {
              language[j] = lang;
              if (text[j] == ' ')
                /* the current character is a space */
                {
                  /* if there is an non-space character before, this space
                     takes the same type as the previous significant character*/
                  if (!leadingSpace)
                    mathType[j] = mathType[j - 1];
                }
              else
                /* this is a significant character */
                {
                  /* Get it's type */
                  mathType[j] = (char)GetCharType (text[j], script);
                  if (mathType[j] == (char)MathML_EL_MO && text[j] == '-')
                    /* transform character '-' into the minus sign */
                    text[j] = 0x2212;

                  if (leadingSpace)
                    /* this is the first significant character */
                    {
                      leadingSpace = FALSE;
                      /* assign the same type to the previous spaces, if any */
                      if (j > 0)
                        for (i = 0; i < j; i++)
                          mathType[i] = mathType[j];
                    }
                }
            }
          totLen = len;
          if (leadingSpace)
            /* only spaces: equivalent to an empty string */
            totLen = 0;
        }
    }
  /* try to identify numbers like: 0.123  1,000,000  2.1e10 */
  for (i = 1; i < totLen; i++)
    {
      if ((text[i] == ',' || text[i] == '.' || text[i] == 'e') &&
          mathType[i-1] == (char)MathML_EL_MN &&
          i < totLen-1 && mathType[i+1] == (char)MathML_EL_MN)
        /* comma, point or 'e' between two digits: this character is part of
           the number */
        {
          mathType[i] = (char)MathML_EL_MN;
          i++;
        }
    }

  if (TtaHasUndoSequence (doc))
    closeUndoSeq = FALSE;
  else
    {
      TtaExtendUndoSequence (doc);
      closeUndoSeq = TRUE;
    }
  TtaSetDisplayMode (doc, DeferredDisplay);
  oldStructureChecking = TtaGetStructureChecking (doc);
  TtaSetStructureChecking (FALSE, doc);
  firstEl = NULL;
  start = 0;
  lastEl = NULL;

  /* check if the text element is empty */
  empty = FALSE;
  if (totLen == 0)
    /* the character string is empty */
    /* mchar and mglyph elements are not considered empty, even if the
       character string they contain is empty */
    if (elType2.ElTypeNum == MathML_EL_TEXT_UNIT)
      /* a text leaf is considered empty only if it's not an entity */
      {
        attrType.AttrSSchema = MathMLSchema;
        attrType.AttrTypeNum = MathML_ATTR_EntityName;
        attr = TtaGetAttribute (theText, attrType);
        if (!attr)
          /* it's not an entity */
          empty = TRUE;
        else
          if (totLen == 0)
            /* it's an entity rendered as an empty string */
            {
              mathType[0] = (char)MathML_EL_MO;
              totLen = 1;
            }
      }

  if (empty)
    /* the character string is empty. Remove the parent element (MI, MN, MO...)
       if the parent does not contain any other element */
    {
      el = theText;
      TtaPreviousSibling (&el);
      if (el == NULL)
        {
          el = theText;
          TtaNextSibling (&el);
          if (el == NULL)
            /* the text element has no sibling, delete its parent */
            {
              if (newSelEl != NULL)
                newSelEl = ClosestLeaf (theElem, &newSelChar);

              /* get the previous sibling that is not a comment */
              prev = theElem; TtaPreviousSibling (&prev);
              if (prev)
                {
                  ok = FALSE;
                  do
                    {
                      elType = TtaGetElementType (prev);
                      if (elType.ElTypeNum == MathML_EL_XMLcomment &&
                          !strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
                        TtaPreviousSibling (&prev);
                      else
                        ok = TRUE;
                    }
                  while (prev && !ok);
                }
	  
              if (!prev)
                /* No previous sibling. Get the next sibling that is not a
                   comment */
                {
                  next = theElem; TtaNextSibling (&next);
                  if (next)
                    {
                      ok = FALSE;
                      do
                        {
                          elType = TtaGetElementType (next);
                          if (elType.ElTypeNum == MathML_EL_XMLcomment &&
                              !strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
                            TtaNextSibling (&next);
                          else
                            ok = TRUE;
                        }
                      while (next && !ok);
                    }
                }

              parent = TtaGetParent (theElem);
              TtaRegisterElementDelete (theElem, doc);
              TtaDeleteTree (theElem, doc);
              theElem = NULL;

              /* The deletion of the parent element may require a Placeholder
                 instead of the deleted element */
              placeholderEl = NULL;
              if (prev != NULL)
                placeholderEl = InsertPlaceholder (prev, FALSE, doc, TRUE);
              else if (next != NULL)
                placeholderEl = InsertPlaceholder (next, TRUE, doc, TRUE);
              if (placeholderEl != NULL)
                newSelEl = placeholderEl;   

              /* if the deleted element is a child of a FencedExpression element,
                 upate the associated FencedSeparator elements */
              elType = TtaGetElementType (parent);
              if (elType.ElTypeNum == MathML_EL_FencedExpression &&
                  elType.ElSSchema == MathMLSchema)
                RegenerateFencedSeparators (parent, doc, TRUE);

              CheckMROW (&parent, doc);
              if (parent != NULL)
                if (TtaGetFirstChild (parent) == NULL)
                  {
                    elType.ElTypeNum = MathML_EL_Construct;
                    newEl = TtaNewElement (doc, elType);
                    TtaInsertFirstChild (&newEl, parent, doc);
                    TtaRegisterElementCreate (newEl, doc);
                    if (newSelEl != NULL)
                      newSelEl = newEl;
                  }
              firstEl = NULL;
              prevEl = NULL;
              nextEl = NULL;
            }
        }
    }
  else
    /* the modified character string is not empty. Parse it */
    {
      leadingSpace = (text[0] == ' ');
      for (i = 1; i <= totLen; i++)
        {
          separate = FALSE;
          if (text[i-1] == ' ' && text[i] != ' ' && !leadingSpace)
            /* consider a sequence of spaces as a separator */
            separate = TRUE;
          else if (mathType[i] != mathType[i-1] || language[i] != language[i-1])
            /* different mathematical type or different script.
               Create a separate element */
            separate = TRUE;
          else if (i == totLen)
            /* end of string. Create an element anyway */
            separate = TRUE;
          else if (mathType[i-1] == (char)MathML_EL_MO && text[i-1] != ' ' &&
                   text[i] != ' ')
            /* an operator */
            {
              /* by default create a separate element */
              separate = TRUE;
              /* if successive integral characters, keep them in the same element */
              if (text[i] == 0x222B && text[i] == text [i-1])
                separate = FALSE;
            }
          if (leadingSpace && text[i] != ' ')
            leadingSpace = FALSE;
          if (separate)
            /* create a new element */
            {
              if (lastEl == NULL)
                {
                  elType = TtaGetElementType (theElem);
                  if (elType.ElTypeNum != (int)mathType[i-1] ||
                      mathType[i-1] == (char)MathML_EL_MO)
                    {
                      prev = theText;
                      TtaPreviousSibling (&prev);
                      if (prev != NULL)
                        {
                          textEl = prev;
                          TtaPreviousSibling (&prev);
                          newEl = TtaNewElement (doc, elType);
                          TtaInsertSibling (newEl, theElem, TRUE, doc);
                          prevEl = newEl;
                          TtaRegisterElementDelete (textEl, doc);
                          TtaRemoveTree (textEl, doc);
                          TtaInsertFirstChild (&textEl, newEl, doc);
                          while (prev != NULL)
                            {
                              next = textEl;
                              textEl = prev;
                              TtaPreviousSibling (&prev);
                              TtaRegisterElementDelete (textEl, doc);
                              TtaRemoveTree (textEl, doc);
                              TtaInsertSibling (textEl, next, TRUE, doc);
                            }
                          MathSetAttributes (newEl, doc, &newSelEl);
                          TtaRegisterElementCreate (newEl, doc);
                        }
                      /*TtaRegisterElementReplace (theElem, doc);*/
                      TtaChangeTypeOfElement (theElem, doc, (int)mathType[i-1]);
                      TtaRegisterElementTypeChange (theElem, elType.ElTypeNum, doc);
                    }
                  next = theText;
                  TtaNextSibling (&next);
                  if (next != NULL)
                    {
                      textEl = next;
                      TtaNextSibling (&next);
                      newEl = TtaNewElement (doc, elType);
                      TtaInsertSibling (newEl, theElem, FALSE, doc);
                      nextEl = newEl;
                      TtaRegisterElementDelete (textEl, doc);
                      TtaRemoveTree (textEl, doc);
                      TtaInsertFirstChild (&textEl, newEl, doc);
                      while (next != NULL)
                        {
                          prev = textEl;
                          textEl = next;
                          TtaNextSibling (&next);
                          TtaRegisterElementDelete (textEl, doc);
                          TtaRemoveTree (textEl, doc);
                          TtaInsertSibling (textEl, prev, FALSE, doc);
                        }
                      MathSetAttributes (newEl, doc, &newSelEl);
                      TtaRegisterElementCreate (newEl, doc);
                    }
                  newEl = theElem;
                  textEl = theText;
                  firstEl = theElem;
                }
              else
                {
                  elType.ElTypeNum = (int)mathType[i-1];
                  newEl = TtaNewElement (doc, elType);
                  TtaInsertSibling (newEl, lastEl, FALSE, doc);
                  elType.ElTypeNum = MathML_EL_TEXT_UNIT;
                  textEl = TtaNewElement (doc, elType);
                  TtaInsertFirstChild (&textEl, newEl, doc);
                  TtaRegisterElementCreate (newEl, doc);
                }
              while (text[start] == ' ')
                start++;
              trailingSpaces = 0;
              j = i - 1;
              while (text[j] == ' ' && j > start)
                {
                  j--;
                  trailingSpaces++;
                }
              j++;
              c = text[j];
              text[j] = EOS;
              TtaSetBufferContent (textEl, &text[start], language[start], doc);
              text[j] = c;
              lastEl = newEl;
              if (newSelEl != NULL)
                {
                  newSelEl = textEl;
                  if (newSelChar <= j || (newSelChar - trailingSpaces == j+1 &&
                                          text[newSelChar - 1] == EOS))
                    {
                      if (newSelChar < start)
                        newSelChar = 1;
                      else
                        newSelChar -= (start + trailingSpaces);
                    }
                }
              MathSetAttributes (newEl, doc, &newSelEl);
              start = i;

              if (mathType[i-1] == (char)MathML_EL_MO)
                /* the new element is an operator */
                {
                  /* the new element may be a vertically stretchable symbol or a
                     large operator */
                  CheckFence (newEl, doc);
                  CheckLargeOp (newEl, doc);
                  /* if the new element contains a single SYMBOL, placeholders may
                     be needed before and/or after that operator */
                  placeholderEl = InsertPlaceholder (newEl, TRUE, doc, TRUE);
                  placeholderEl = InsertPlaceholder (newEl, FALSE, doc, TRUE);
                  /* the new contents may be an horizontally stretchable symbol */
                  if (newEl != NULL)
                    {
                      parent = TtaGetParent (newEl);
                      elType = TtaGetElementType (parent);
                      if ((elType.ElTypeNum == MathML_EL_UnderOverBase ||
                           elType.ElTypeNum == MathML_EL_Underscript ||
                           elType.ElTypeNum == MathML_EL_Overscript ||
                           elType.ElTypeNum == MathML_EL_CellWrapper) &&
                          elType.ElSSchema == MathMLSchema)
                        SetSingleIntHorizStretchAttr (parent, doc, &newSelEl);
                    }
                }
            }
        }
    }

  /* try to merge the first element processed with its previous sibling */
  if (prevEl != NULL && firstEl != NULL)
    {
      elType = TtaGetElementType (prevEl);
      if (elType.ElTypeNum != MathML_EL_MO)
        /* Don't merge operators */
        {
          elType2 = TtaGetElementType (firstEl);
          if (elType.ElTypeNum == elType2.ElTypeNum &&
              elType.ElSSchema == elType2.ElSSchema)
            MergeMathEl (firstEl, prevEl, TRUE, &newSelEl, &newSelChar, doc);
        }
    }
  /* try to merge the last element processed with its next sibling */
  if (nextEl != NULL && lastEl != NULL)
    {
      elType = TtaGetElementType (nextEl);
      if (elType.ElTypeNum != MathML_EL_MO)
        /* Don't merge operators */
        {
          elType2 = TtaGetElementType (lastEl);
          if (elType.ElTypeNum == elType2.ElTypeNum &&
              elType.ElSSchema == elType2.ElSSchema)
            MergeMathEl (lastEl, nextEl, FALSE, &newSelEl, &newSelChar, doc);
        }
    }

  /* try to set function names as separate MI elements */
  SeparateFunctionNames (&firstEl, lastEl, doc, &newSelEl, &newSelChar);

  if (firstEl != NULL)
    {
      /* if we are in a FencedExpression element, upate the associated
         FencedSeparator elements */
      parent = TtaGetParent (firstEl);
      elType = TtaGetElementType (parent);
      if (elType.ElTypeNum == MathML_EL_FencedExpression &&
          elType.ElSSchema == MathMLSchema)
        RegenerateFencedSeparators (parent, doc, TRUE);

      if (elType.ElTypeNum == MathML_EL_MROW &&
          elType.ElSSchema == MathMLSchema)
        /* delete the parent MROW element if it is no longer useful */
        CheckMROW (&parent, doc);
      else
        /* Create a MROW element that encompasses the new elements if necessary */
        CreateParentMROW (firstEl, doc);
    }

  TtaSetStructureChecking (oldStructureChecking, doc);
  TtaSetDisplayMode (doc, DisplayImmediately);
  if (closeUndoSeq)
    TtaCloseUndoSequence (doc);

  /* set a new selection */
  if (newSelEl != NULL)
    {
      elType = TtaGetElementType (newSelEl);
      if (elType.ElTypeNum == MathML_EL_TEXT_UNIT ||
          elType.ElTypeNum == MathML_EL_SYMBOL_UNIT)
        TtaSelectString (doc, newSelEl, newSelChar, newSelChar-1);
      else
        TtaSelectElement (doc, newSelEl);
    }
}

/*----------------------------------------------------------------------
  SetAttrParseMe
  associate a IntParseMe attribute with element el in document doc
  -----------------------------------------------------------------------*/
void SetAttrParseMe (Element el, Document doc)
{
  ElementType    elType;
  AttributeType  attrType;
  Attribute      attr;

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = MathML_ATTR_IntParseMe;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, doc);
  TtaSetAttributeValue (attr, MathML_ATTR_IntParseMe_VAL_yes_, el, doc);
}

/*----------------------------------------------------------------------
  SetContentAfterEntity
  -----------------------------------------------------------------------*/
static void SetContentAfterEntity (char *entityName, Element el, Document doc)
{
  char	         bufEntity[10];
  ThotBool       found, error;
  int            value;
  Language       lang;
  unsigned char *ptr;
  int            i;

  found = FALSE;
  if (entityName[0] == '#')
    /* it's a character reference */
    {
      error = FALSE;
      if (entityName[1] == 'x')
        /* it's a hexadecimal number */
        {
          for (i = 2; entityName[i] != EOS && !error; i++)
            if (entityName[i] < '0' ||
                (entityName[i] > '9' && entityName[i] < 'A') ||
                (entityName[i] > 'F' && entityName[i] < 'a') ||
                entityName[i] > 'f')
              error = TRUE;
          if (!error)
            {
              sscanf (&entityName[2], "%x", &value);
              found = TRUE;
            }
        }
      else
        /* it's a decimal number */
        {
          for (i = 2; entityName[i] != EOS && !error; i++)
            if (entityName[i] < '0' || entityName[i] > '9')
              error = TRUE;
          if (!error)
            {
              sscanf (&entityName[1], "%d", &value);
              found = TRUE;
            }
        }
    }
  else
    /* it's supposed to be the name of the character */
    {
      /* Look first at the MathML entities table */
      found = MapXMLEntity (MATH_TYPE, entityName, &value);
      if (!found)
        /* it's not a known character name. It may be the hex code of the
           character without the leading '#x' */
        {
          error = FALSE;
          for (i = 0; entityName[i] != EOS && !error; i++)
            if (entityName[i] < '0' ||
                (entityName[i] > '9' && entityName[i] < 'A') ||
                (entityName[i] > 'F' && entityName[i] < 'a') ||
                entityName[i] > 'f')
              error = TRUE;
          if (!error)
            /* only hexadecimal digits */
            {
              sscanf (entityName, "%x", &value);
              found = TRUE;
            }
        }
    }
  if (!found)
    {
      /* Unknown entity */
      bufEntity[0] = '?';
      bufEntity[1] = EOS;
      lang = TtaGetLanguageIdFromScript('L');
    }
  else if (value < 1023)
    {
      /* get the UTF-8 string of the unicode character */
      ptr = (unsigned char *)bufEntity;
      i = TtaWCToMBstring ((wchar_t) value, &ptr);
      bufEntity[i] = EOS;
    }
  else
    {
      if (value < 255)
        {
          bufEntity[0] = ((unsigned char) value);
          bufEntity[1] = EOS;
          lang = TtaGetLanguageIdFromScript('L');
        }
      else
        /* Try to find a fallback character */
        GetFallbackCharacter (value, (unsigned char *)bufEntity, &lang);
    }
  TtaSetTextContent (el, (unsigned char *)bufEntity, lang, doc);
}

/*----------------------------------------------------------------------
  InsertMathEntity
  Insert an entity at the currently selected position.
  entityName is the name of the entity to be created.
  -----------------------------------------------------------------------*/
static void InsertMathEntity (unsigned char *entityName, Document document)
{
  Element       firstSel, lastSel, el, el1, parent, sibling, delEl, next;
  ElementType   elType, elType1;
  Attribute     attr;
  AttributeType attrType;
  int           firstChar, lastChar, i, len;
  ThotBool      before, done, emptySel;
  char          buffer[MAX_LENGTH+2];

  if (!TtaGetDocumentAccessMode (document))
    /* the document is in ReadOnly mode, don't do any change */
    return;

  /* if not within a MathML element, nothing to do */
  TtaGiveFirstSelectedElement (document, &firstSel, &firstChar, &i);
  parent = TtaGetParent (firstSel);
  if (!parent)
    return;
  elType = TtaGetElementType (parent);
  if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") != 0)
    return;
  /* an entity can only replace a single element */
  TtaGiveLastSelectedElement (document, &lastSel, &i, &lastChar);
  if (firstSel != lastSel)
    return;
  emptySel = TtaIsSelectionEmpty ();
  TtaOpenUndoSequence (document, firstSel, lastSel, firstChar, lastChar);
  TtaUnselect (document);
  done = FALSE;
  delEl = NULL;
  /* the new text element will be inserted before the first element
     selected */
  before = TRUE;
  sibling = firstSel;
  elType = TtaGetElementType (firstSel);
  /* create a Thot TEXT element */
  elType1.ElSSchema = elType.ElSSchema;
  elType1.ElTypeNum = MathML_EL_TEXT_UNIT;
  el = TtaNewElement (document, elType1);
  if (elType.ElTypeNum == MathML_EL_Construct ||
      elType.ElTypeNum == MathML_EL_Construct1)
    /* the selected element is an empty expression. Replace it by a
       mtext element*/
    {
      TtaRegisterElementDelete (firstSel, document);
      /* if it's a placeholder, delete attribute IntPlaceholder */
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = MathML_ATTR_IntPlaceholder;
      attr = TtaGetAttribute (firstSel, attrType);
      if (attr != NULL)
        RemoveAttr (firstSel, document, MathML_ATTR_IntPlaceholder);
      /* create and insert the mtext element, with a mchar child */
      elType1.ElTypeNum = MathML_EL_MTEXT;
      el1 = TtaNewElement (document, elType1);
      TtaInsertFirstChild (&el1, firstSel, document);
      TtaInsertFirstChild (&el, el1, document);
      SetAttrParseMe (el1, document);
      done = TRUE;
      TtaRegisterElementCreate (el1, document);
    }
  else if (elType.ElTypeNum != MathML_EL_TEXT_UNIT)
    /* it's a construct. Replace it by a mtext element */
    {
      elType1.ElTypeNum = MathML_EL_MTEXT;
      el1 = TtaNewElement (document, elType1);
      TtaInsertSibling (el1, firstSel, TRUE, document);
      TtaInsertFirstChild (&el, el1, document);
      SetAttrParseMe (el1, document);
      delEl = firstSel;
      done = TRUE;
      TtaRegisterElementCreate (el1, document);
    }
  else
    /* current selection is in a text leaf */
    {
      elType1 = TtaGetElementType (parent);
      if (elType1.ElTypeNum == MathML_EL_MGLYPH &&
          strcmp (TtaGetSSchemaName (elType1.ElSSchema), "MathML") == 0)
        /* the first selected element is within a mglyph. The new text
           leaf will be inserted as a sibling of this mglyph */
        sibling = parent;
      if (firstChar == 0 && lastChar == 0)
        /* the whole text string is selected. Delete it */
        delEl = firstSel;
      len = TtaGetElementVolume (firstSel);
      if (firstChar == 1 && !emptySel)
        /* some text has to be deleted */
        {
          if (lastChar > len)
            delEl = firstSel;
          else
            {
              TtaRegisterElementReplace (firstSel, document);
              TtaSplitText (firstSel, lastChar, document);
              delEl = firstSel;
              sibling = firstSel;
              TtaNextSibling (&sibling);
              TtaRegisterElementCreate (sibling, document);
            }
        }
      if (firstChar > 1)
        {
          len = TtaGetElementVolume (firstSel);
          if (firstChar > len)
            /* the caret is at the end of that character string */
            /* Create the new element after the character string */
            before = FALSE;
          else
            {
              /* split the text to insert the new text */
              TtaRegisterElementReplace (firstSel, document);
              TtaSplitText (firstSel, firstChar, document);
              /* take the second part of the split text element */
              sibling = firstSel;
              TtaNextSibling (&sibling);
              TtaRegisterElementCreate (sibling, document);
              if (lastChar > len)
                delEl = sibling;
              else if (lastChar > firstChar)
                {
                  TtaSplitText (sibling, lastChar-firstChar+1, document);
                  delEl = sibling;
                  next = sibling;
                  TtaNextSibling (&next);
                  TtaRegisterElementCreate (next, document);
                }
            }
        }
    }
  if (!done)
    {
      TtaInsertSibling (el, sibling, before, document);
      TtaRegisterElementCreate (el, document);
    }
  if (delEl)
    {
      TtaRegisterElementDelete (delEl, document);
      TtaDeleteTree (delEl, document);
    }
  if (!strncmp ((char *)entityName, "ApplyFunction", 13) ||
      !strncmp ((char *)entityName, "af", 2) ||
      !strncmp ((char *)entityName, "InvisibleTimes", 14) ||
      !strncmp ((char *)entityName, "it", 2) ||
      !strncmp ((char *)entityName, "InvisibleComma", 14) ||
      !strncmp ((char *)entityName, "ic", 2))
    /* make invisible operators visible in the structure view */
    {
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = MathML_ATTR_EntityName;
      attr =  TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, document);
      strcpy (buffer, "&");
      strcat ((char *)buffer, (char *)entityName);
      strcat (buffer, ";");
      TtaSetAttributeText (attr, buffer, el, document);
    }
  SetContentAfterEntity ((char *)entityName, el, document);
  len = TtaGetElementVolume (el);
  TtaSelectString (document, el, len + 1, len);
  ParseMathString (el, TtaGetParent (el), document);
  TtaSetDocumentModified (document);
  TtaCloseUndoSequence (document);
}

/*----------------------------------------------------------------------
  CreateEntity
  Display a dialogue box to allow input of a character entity name
  and create the corresponding entity at the current selection position
  It could be used also outside Math construction
  -----------------------------------------------------------------------*/
void CreateEntity (Document doc, View view)
{
  Element       firstSel, lastSel, parent;
  ElementType   elType;
  int           firstChar, lastChar, i, code;
  ThotBool      newMath = FALSE;

  if (!TtaGetDocumentAccessMode (doc))
    /* the document is in ReadOnly mode, don't do any change */
    return;
  TtaGiveFirstSelectedElement (doc, &firstSel, &firstChar, &i);
  if (!firstSel)
    {
      /* no selection. Nothing to do */
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_INSERT_POINT);
      return;
    }

  MathMLEntityName[0] = EOS;
#ifdef _WX
  CreateTextDlgWX (BaseDialog + MathEntityForm, BaseDialog + MathEntityText,
                   TtaGetViewFrame (doc, view),
                   TtaGetMessage (AMAYA, AM_MEntity),
                   TtaGetMessage (AMAYA, AM_MATH_ENTITY_NAME),
                   "");
  TtaSetDialoguePosition ();
  TtaShowDialogue (BaseDialog + MathEntityForm, FALSE, TRUE);
  TtaWaitShowDialogue ();
#endif /* _WX */

  parent = TtaGetParent (firstSel);
  if (parent)
    elType = TtaGetElementType (parent);
  if (MathMLEntityName[0] == EOS)
    return;
  if (MapXMLEntity (XHTML_TYPE, MathMLEntityName, &code))
    {
      TtcInsertChar (doc, view, code);
      return;
    }
  else if (((MathMLEntityName[0] >= '0' && MathMLEntityName[0] <= '9') ||
           (MathMLEntityName[0] >= 'a' && MathMLEntityName[0] <= 'f') ||
           (MathMLEntityName[0] >= 'A' && MathMLEntityName[0] <= 'F')) &&
           ((MathMLEntityName[1] >= '0' && MathMLEntityName[1] <= '9') ||
            (MathMLEntityName[1] >= 'a' && MathMLEntityName[1] <= 'f') ||
            (MathMLEntityName[1] >= 'A' && MathMLEntityName[1] <= 'F')) &&
           ((MathMLEntityName[2] >= '0' && MathMLEntityName[2] <= '9') ||
            (MathMLEntityName[2] >= 'a' && MathMLEntityName[2] <= 'f') ||
            (MathMLEntityName[2] >= 'A' && MathMLEntityName[2] <= 'F')))
    // insert the entity at that position
      sscanf (MathMLEntityName, "%x", &code);
  if (code >= 32)
    {
      TtcInsertChar (doc, view, code);
      return;
    }
  else if (!parent || strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
    {
      /* if not within a MathML element, create the math element */
      CreateMathConstruct (doc, view, 1);
      TtaGiveFirstSelectedElement (doc, &firstSel, &firstChar, &i);
      if (!firstSel)
        return;
      
      elType = TtaGetElementType (firstSel);
      if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
        return;
      else
        {
          /* math element created */
          TtaExtendUndoSequence (doc);
          newMath = TRUE;
        }
    }

  TtaGiveLastSelectedElement (doc, &lastSel, &i, &lastChar);
  /* an entity can replace only a single element */
  if (firstSel != lastSel)
    return;
  InsertMathEntity ((unsigned char *)MathMLEntityName, doc);
}

/*----------------------------------------------------------------------
  CreateInvisibleTimes
  Insert a character entity InvisibleTimes at the current position
  -----------------------------------------------------------------------*/
void CreateInvisibleTimes (Document document, View view)
{
  InsertMathEntity ((unsigned char *)"InvisibleTimes", document);
}

/*----------------------------------------------------------------------
  CreateApplyFunction
  Insert a character entity CreateAlppyFunction at the current position
  -----------------------------------------------------------------------*/
void CreateApplyFunction (Document document, View view)
{
  InsertMathEntity ((unsigned char *)"ApplyFunction", document);
}

/*----------------------------------------------------------------------
  SetElementCharFont
  associate attribute attrType with value val to element el if it is
  of type MI, MTEXT, MO,..., or to its descendant of that type.
  -----------------------------------------------------------------------*/
static void SetElementCharFont (Element el, AttributeType attrType, int val,
                                Document doc)
{
  ElementType         elType;
  Attribute           attr, intAttr;
  AttributeType       intAttrType;
  Element             child;
  ThotBool            newAttr;

  elType = TtaGetElementType (el);
  if ((elType.ElTypeNum == MathML_EL_MTEXT ||
       elType.ElTypeNum == MathML_EL_MI ||
       elType.ElTypeNum == MathML_EL_MO ||
       elType.ElTypeNum == MathML_EL_MN ||
       elType.ElTypeNum == MathML_EL_MS ||
       elType.ElTypeNum == MathML_EL_MSTYLE) &&
      strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
    /* the required attribute can be associated with that element */
    {
      attr = TtaGetAttribute (el, attrType);
      if (attr)
        {
          newAttr = FALSE;
          TtaRegisterAttributeReplace (attr, el, doc);
        }
      else
        {
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (el, attr, doc);
          newAttr = TRUE;
        }
      TtaSetAttributeValue (attr, val, el, doc);
      if (newAttr)
        {
          TtaRegisterAttributeCreate (attr, el, doc);
          if (attrType.AttrTypeNum == MathML_ATTR_fontstyle)
            /* remove attribute IntFontstyle if any */
            {
              intAttrType.AttrSSchema = elType.ElSSchema;
              intAttrType.AttrTypeNum = MathML_ATTR_IntFontstyle;
              intAttr = TtaGetAttribute (el, intAttrType);
              if (intAttr)
                {
                  TtaRegisterAttributeDelete (intAttr, el, doc);
                  TtaRemoveAttribute (el, intAttr, doc);
                }
            }
        }
    }
  else
    /* put the required attribute on the descendant elements */
    {
      child = TtaGetFirstChild (el);
      while (child)
        {
          SetElementCharFont (child, attrType, val, doc);
          TtaNextSibling (&child);
        }
    }
}

/*----------------------------------------------------------------------
  SetMathCharFont
  The user has clicked one of the buttons: Emphasis, Strong, Code
  -----------------------------------------------------------------------*/
void SetMathCharFont (Document doc, int attribute)
{
  Element             firstSel, lastSel, el, next, leaf;
  ElementType         elType;
  Attribute           attr;
  AttributeType       attrType;
  DisplayMode         dispMode;
  int                 firstChar, lastChar, i, j, val;
  ThotBool            emptySel, same, mrowCreated, oldStructureChecking;

  if (!TtaGetDocumentAccessMode (doc))
    return;
  TtaGiveFirstSelectedElement (doc, &firstSel, &firstChar, &i);
  if (firstSel == NULL)
    {
      /* no selection available */
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_INSERT_POINT);
      return;
    }
  if (attribute == MathML_ATTR_fontfamily)
    /* attribute fontfamily not handled yet */
    return;

  TtaGiveLastSelectedElement (doc, &lastSel, &j, &lastChar);

  /* check whether the selection is empty (just a caret) or contains some
     characters/elements */
  emptySel = TtaIsSelectionEmpty ();

  TtaClearViewSelections ();
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);
  TtaOpenUndoSequence (doc, firstSel, lastSel, firstChar, lastChar);
  oldStructureChecking = TtaGetStructureChecking (doc);
  TtaSetStructureChecking (FALSE, doc);
  TtaUnselect (doc);

  /* move selection at the level of elements MTEXT, MI, etc. if it starts or
     ends on character strings */
  RoundSelection (&firstSel, &lastSel, &firstChar, &lastChar);

  /* if selection starts or ends in the middle of a character string,
     split the parent MI, MN, MO, MTEXT... */
  elType = TtaGetElementType (lastSel);
  if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
    /* the last selected element is a character string. Split it */
    {
      leaf = SplitTextInMathML (doc, lastSel, lastChar, &mrowCreated);
      if (firstSel != lastSel)
        lastSel = TtaGetParent (lastSel);
    }
  elType = TtaGetElementType (firstSel);
  if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
    /* the first selected element is a character string. Split it */
    {
      same = (firstSel == lastSel);
      leaf = SplitTextInMathML (doc, firstSel, firstChar, &mrowCreated);
      if (leaf)
        {
          firstSel = TtaGetParent (leaf);
          if (same)
            lastSel = firstSel;
        }
    }
  attrType.AttrSSchema = TtaGetSSchema ("MathML", doc);
  attrType.AttrTypeNum = attribute;
  attr = TtaGetAttribute (firstSel, attrType);
  val = 1;
  if (attr)
    val = TtaGetAttributeValue (attr);
  if (attribute == MathML_ATTR_fontweight)
    {
      if (!attr)
        val = MathML_ATTR_fontweight_VAL_bold_;
      else
        if (val == MathML_ATTR_fontweight_VAL_bold_)
          val = MathML_ATTR_fontweight_VAL_normal_;
        else
          val = MathML_ATTR_fontweight_VAL_bold_;
    }
  else if (attribute == MathML_ATTR_fontstyle)
    {
      if (!attr)
        val = MathML_ATTR_fontstyle_VAL_italic;
      else
        if (val == MathML_ATTR_fontstyle_VAL_italic)
          val = MathML_ATTR_fontstyle_VAL_normal_;
        else
          val = MathML_ATTR_fontstyle_VAL_italic;
    }

  /* process all selected element */
  el = firstSel;
  while (el)
    {
      /* get the element to be processed after the current element: the */
      /* current element may change during processing */
      if (el == lastSel)
        next = NULL;
      else
        {
          next = el;
          TtaGiveNextElement (doc, &next, lastSel);
        }
      SetElementCharFont (el, attrType, val, doc);
      /* process the next element in the selection */
      el = next;
    }

  TtaSetStructureChecking (oldStructureChecking, doc);
  TtaCloseUndoSequence (doc);
  TtaSetDisplayMode (doc, dispMode);
  TtaSelectElement (doc, firstSel);
  TtaExtendSelection (doc, lastSel, 0);
}

/*----------------------------------------------------------------------
  MtextCreated
  A new MTEXT element has just been created by the user, not with a
  command, but simply by starting to type some text.
  -----------------------------------------------------------------------*/
void MtextCreated (NotifyElement *event)
{
  /* associate a IntParseMe attribute with the new MTEXT element */
  SetAttrParseMe (event->element, event->document);
}

/*----------------------------------------------------------------------
  MathStringModified
  The content of an element MTEXT, MI, MO, MN, or MS has been modified.
  Parse the new content and create the appropriate MI, MO, MN elements.
  -----------------------------------------------------------------------*/
void MathStringModified (NotifyOnTarget *event)
{
  RemoveAttr (event->target, event->document, MathML_ATTR_EntityName);
  /* if the event comes from function BreakElement, don't do anything:
     the user just wants to split that character string */
  if (event->targetdocument != 0)
    {
      /* analyze the new content of the text element */
      ParseMathString (event->target, event->element, event->document);
    }
}

/*----------------------------------------------------------------------
  NewMathString
  An new text string has been created in a MathML element.
  Parse its contents.
  -----------------------------------------------------------------------*/
void NewMathString (NotifyElement *event)
{
  if (TtaGetElementVolume (event->element) > 0)
    ParseMathString (event->element, TtaGetParent (event->element),
                     event->document);
}

/*----------------------------------------------------------------------
  NewMathElement
  An element will be pasted
  -----------------------------------------------------------------------*/
ThotBool NewMathElement (NotifyOnValue *event)
{
  int           profile;

  // is it a compound document?
  profile = TtaGetDocumentProfile (event->document);
  if (profile == L_Strict || profile == L_Basic)
    {
      /* cannot insert here */
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NOT_ALLOWED);
      return TRUE;
    }
  else
    return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  MathElementPasted
  An element has been pasted in a MathML structure.
  Create placeholders before and after the pasted elements if necessary.
  If the element is an XLink, update the link.
  If an enclosing MROW element is needed create it.
  -----------------------------------------------------------------------*/
void MathElementPasted (NotifyElement *event)
{
  Element	      placeholderEl, parent, prev, leaf, root;
  ElementType	  elType, elTypeParent;
  Attribute     attr;
  AttributeType attrType;
  Document      doc;
  int           profile;
  ThotBool      oldStructureChecking;

  /* if the pasted element is an XLink, update the link */
  XLinkPasted (event);
  doc = event->document;
  if (DocumentTypes[doc] != docMath)
    {
      profile = TtaGetDocumentProfile (doc);
      if (DocumentMeta[doc])
        DocumentMeta[doc]->compound = TRUE;
    }

  elType = TtaGetElementType (event->element);
  if (elType.ElTypeNum == MathML_EL_MathML)
    {
      /* Check pseudo-paragraphs */
      parent = TtaGetParent (event->element);
      if (parent)
        {
          elTypeParent = TtaGetElementType (parent);
          if (!strcmp (TtaGetSSchemaName (elTypeParent.ElSSchema),  "HTML"))
            CheckPseudoParagraph (event->element, doc);
        }
      /* It is the <math> element */
      /* Set the IntDisplaystyle attribute according to the context */     
      SetDisplaystyleMathElement (event->element, doc);
      leaf = AppendEmptyText (event->element, doc);
      if (leaf)
        TtaRegisterElementCreate (leaf, doc);
      // force XML document
      if (DocumentMeta[doc])
        {
          DocumentMeta[doc]->compound = TRUE;
          if (!DocumentMeta[doc]->xmlformat)
            {
              // the document becomes an XML document
              DocumentMeta[doc]->xmlformat = TRUE;
              root = TtaGetRootElement (doc);
              TtaSetANamespaceDeclaration (doc, root, NULL, XHTML_URI);
            }
        }
      /* Set the MathML namespace declaration */
      TtaSetUriSSchema (elType.ElSSchema, MathML_URI);
      TtaSetANamespaceDeclaration (doc, event->element, NULL, MathML_URI);
    }

  if (elType.ElTypeNum == MathML_EL_MUNDER ||
      elType.ElTypeNum == MathML_EL_MOVER ||
      elType.ElTypeNum == MathML_EL_MUNDEROVER)
    /* move the limits if it's appropriate */
    SetIntMovelimitsAttr (event->element, doc);

  if (elType.ElTypeNum == MathML_EL_MO)
    /* it's a mo element. It may be a fence separator or a largeop */
    {
      CheckFence (event->element, doc);
      CheckLargeOp (event->element, doc);
    }

  oldStructureChecking = TtaGetStructureChecking (doc);
  TtaSetStructureChecking (FALSE, doc);

  /* if an enclosing MROW element is needed create it, except if it's a
     call from Undo command */
  if (event->info != 1)
    CreateParentMROW (event->element, doc);

  /* if the pasted element is a child of a FencedExpression element,
     create the associated FencedSeparator elements */
  parent = TtaGetParent (event->element);
  elTypeParent = TtaGetElementType (parent);
  if (elTypeParent.ElTypeNum == MathML_EL_FencedExpression &&
      strcmp (TtaGetSSchemaName (elTypeParent.ElSSchema), "MathML") == 0)
    RegenerateFencedSeparators (parent, doc, FALSE/******/);

  /* if the pasted element is a character string within a MI, MN, or MO
     element, parse the new content to isolate identifiers, numbers and
     operators */
  if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
    {
      /* remove all attributes attached to the pasted MathML_EL_TEXT_UNIT */
      RemoveTextAttributes (event->element, doc);
      if ((elTypeParent.ElTypeNum == MathML_EL_MI ||
           elTypeParent.ElTypeNum == MathML_EL_MO ||
           elTypeParent.ElTypeNum == MathML_EL_MN) &&
          strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
        /* if it's a call from Undo command, don't do anything */
        if (event->info != 1)
          ParseMathString (event->element, parent, doc);
    }
  else
    {
      /* create placeholders before and/or after the new element */
      placeholderEl = InsertPlaceholder (event->element, TRUE, doc,
                                         FALSE/****/);
      placeholderEl = InsertPlaceholder (event->element, FALSE, doc,
                                         FALSE/****/);
      /* if the previous sibling is a Construct1, turn it into and
         ordinary placeholder */
      prev = event->element;  TtaPreviousSibling (&prev);
      if (prev)
        {
          elType = TtaGetElementType (prev);
          if (elType.ElTypeNum == MathML_EL_Construct1)
            {
              attrType.AttrSSchema = elType.ElSSchema;
              attrType.AttrTypeNum = MathML_ATTR_IntPlaceholder;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (prev, attr, doc);
              TtaSetAttributeValue (attr, MathML_ATTR_IntPlaceholder_VAL_yes_,
                                    prev, doc);
              /*TtaRegisterElementReplace (prev, doc);*/
              TtaChangeTypeOfElement (prev, doc, MathML_EL_Construct);
              TtaRegisterElementTypeChange (prev, elType.ElTypeNum, doc);
            }
        }
    }

  TtaSetStructureChecking (oldStructureChecking, doc);
  /* Check attribute NAME or ID in order to make sure that its value */
  /* is unique in the document */
  MakeUniqueName (event->element, doc, TRUE, FALSE);
}


/*----------------------------------------------------------------------
  MathElementWillBeDeleted
  This function is called by the DELETE command for each selected element
  and for all their descendants.
  -----------------------------------------------------------------------*/
ThotBool MathElementWillBeDeleted (NotifyElement *event)
{
  ElementType	elType;
  
  /* TTE_STANDARD_DELETE_LAST_ITEM indicates the last element to be
     deleted, but function MathElementWillBeDeleted is called afterwards
     for all decendants of this last selected element, without
     TTE_STANDARD_DELETE_LAST_ITEM.
     Function MathElementDeleted is called only for the selected elements,
     not for their descendants */
  if (!IsLastDeletedElement)
    {
      IsLastDeletedElement = (event->position == TTE_STANDARD_DELETE_LAST_ITEM);
      if (IsLastDeletedElement)
        LastDeletedElement = event->element;
    }
  else
    {
      if (!TtaIsAncestor (event->element, LastDeletedElement))
        {
          LastDeletedElement = NULL;
          IsLastDeletedElement = False;
        }
    }
  
 /* Free the namespace declaration associated with the MathML root */
  elType = TtaGetElementType (event->element);
  if (elType.ElTypeNum == MathML_EL_MathML)
    TtaFreeElemNamespaceDeclarations (event->document, event->element);
   return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  MathElementDeleted
  An element has been deleted in a MathML structure.
  Create the necessary placeholders.
  Remove the enclosing MROW element if it has only one child.
  -----------------------------------------------------------------------*/
void MathElementDeleted (NotifyElement *event)
{
  Element	 sibling, placeholderEl, parent, child, grandChild, next, prev,
    el;
  ElementType	 parentType, elType;
  AttributeType attrType;
  Attribute     attr;
  int		 i, newTypeNum;
  ThotBool      oldStructureChecking;
     
  if (event->info == 1 || event->info == 2)
    /* call from Undo command or Return key. Don't do anything */
    return;
  parent = event->element; /* parent of the deleted element */
  parentType = TtaGetElementType (parent);

  if (parentType.ElTypeNum == MathML_EL_FencedExpression &&
      strcmp (TtaGetSSchemaName (parentType.ElSSchema), "MathML") == 0)
    /* a child of a FencedExpression element has been deleted,
       re-generate all FencedSeparator elements in that FencedExpression */
    RegenerateFencedSeparators (parent, event->document, FALSE/*****/);

  /* If there are several successive placeholders at the place where the
     element has been deleted, remove all unneeded placeholders.
     If the deletion makes a placeholder necessary at the position of the
     deleted element, insert a placeholder at that position */
  sibling = TtaGetFirstChild (parent); /* first sibling of the deleted
                                          element */
  if (event->position == 0)
    {
      /* the first child of parent has been deleted.
         Create a placeholder before the new first child */
      if (sibling != NULL)
        placeholderEl = InsertPlaceholder (sibling, TRUE, event->document,
                                           FALSE/******/);
    }
  else if (IsLastDeletedElement)
    {
      for (i = 1; i < event->position && sibling != NULL; i++)
        TtaNextSibling (&sibling);
      if (sibling != NULL)
        {
          placeholderEl = InsertPlaceholder (sibling, FALSE, event->document,
                                             FALSE/*****/);
          /* if sibling is a placeholder, if it has no next sibling and if its
             previous sibling is a stretchable msubsup, transform this
             placeholder into a Construct1 */
          next = sibling; TtaNextSibling (&next);
          if (!next)
            /* element sibling has no following sibling */
            {
              elType = TtaGetElementType (sibling);
              if (elType.ElTypeNum == MathML_EL_Construct)
                {
                  attrType.AttrTypeNum = MathML_ATTR_IntPlaceholder;
                  attrType.AttrSSchema = elType.ElSSchema;
                  attr = TtaGetAttribute (sibling, attrType);
                  if (attr)
                    /* element sibling is a placeholder */
                    {
                      prev = sibling;  TtaPreviousSibling (&prev);
                      if (prev)
                        {
                          elType = TtaGetElementType (prev);
                          if (elType.ElTypeNum == MathML_EL_MSUBSUP ||
                              elType.ElTypeNum == MathML_EL_MSUB ||
                              elType.ElTypeNum == MathML_EL_MSUP ||
                              elType.ElTypeNum == MathML_EL_MUNDEROVER ||
                              elType.ElTypeNum == MathML_EL_MUNDER ||
                              elType.ElTypeNum == MathML_EL_MOVER )
                            /* the previous element is a msubsup, msub, msup,
                               munderover, munder or mover */
                            {
                              attrType.AttrSSchema = elType.ElSSchema;
                              attrType.AttrTypeNum = MathML_ATTR_IntVertStretch;
                              if (TtaGetAttribute (prev, attrType))
                                /* it has an attribute IntVertStretch */
                                {
                                  /* the place holder has to be transformed into
                                     a Construct1, to allow presentation rules
                                     to operate correctly. */
                                  TtaRemoveAttribute (sibling, attr,
                                                      event->document);
                                  /*TtaRegisterElementReplace (sibling,
                                    event->document);*/
                                  TtaChangeTypeOfElement (sibling, event->document,
                                                          MathML_EL_Construct1);
                                  TtaRegisterElementTypeChange (sibling, 
                                                                elType.ElTypeNum,
                                                                event->document);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
  IsLastDeletedElement = FALSE;
  LastDeletedElement = NULL;

  /* If there is an enclosing MROW that is no longer needed, remove
     that MROW */
  CheckMROW (&parent, event->document); /******/

  /* The deletion of this component may lead to a structure change for its
     siblings and its parent */
  newTypeNum = 0;
  switch (event->elementType.ElTypeNum)
    {
    case MathML_EL_Index:		/* an Index has been deleted */
      /* transform the MROOT into a MSQRT */
      newTypeNum = MathML_EL_MSQRT;
      break;

    case MathML_EL_Numerator:		/* a Numerator has been deleted */
    case MathML_EL_Denominator:	/* a Denominator has been deleted */
      /* remove the enclosing MFRAC or BevelledMFRAC */
      newTypeNum = -1;
      break;

    case MathML_EL_Subscript:		/* a Subscript has been deleted */
      if (parentType.ElTypeNum == MathML_EL_MSUBSUP &&
          strcmp (TtaGetSSchemaName (parentType.ElSSchema), "MathML") == 0)
        /* a Subscript in a MSUBSUP. Transform the MSUBSUP into a MSUP */
        newTypeNum = MathML_EL_MSUP;
      else if (parentType.ElTypeNum == MathML_EL_MSUB &&
               !strcmp (TtaGetSSchemaName (parentType.ElSSchema), "MathML"))
        /* a Subscript in a MSUB. Remove the MSUB and the Base */
        newTypeNum = -1;
      break;

    case MathML_EL_Superscript:	/* a Superscript has been deleted */
      if (parentType.ElTypeNum == MathML_EL_MSUBSUP &&
          strcmp (TtaGetSSchemaName (parentType.ElSSchema), "MathML") == 0)
        /* a Superscript in a MSUBSUP. Transform the MSUBSUP into a MSUB */
        newTypeNum = MathML_EL_MSUB;
      else if (parentType.ElTypeNum == MathML_EL_MSUP &&
               !strcmp (TtaGetSSchemaName (parentType.ElSSchema), "MathML"))
        /* a Superscript in a MSUP. Remove the MSUP and the Base */
        newTypeNum = -1;
      break;

    case MathML_EL_Underscript:	/* an Underscript has been deleted */
      if (parentType.ElTypeNum == MathML_EL_MUNDEROVER &&
          strcmp (TtaGetSSchemaName (parentType.ElSSchema), "MathML") == 0)
        /* an Underscript in a MUNDEROVER. Transform the MUNDEROVER into
           a MOVER */
        newTypeNum = MathML_EL_MOVER;
      else if (parentType.ElTypeNum == MathML_EL_MUNDER &&
               !strcmp (TtaGetSSchemaName (parentType.ElSSchema), "MathML"))
        /* an Underscript in a MUNDER. Remove the MUNDER and the
           UnderOverBase */
        newTypeNum = -1;
      break;

    case MathML_EL_Overscript:	/* an Overscript has been deleted */
      if (parentType.ElTypeNum == MathML_EL_MUNDEROVER &&
          strcmp (TtaGetSSchemaName (parentType.ElSSchema), "MathML") == 0)
        /* an Overscript in a MUNDEROVER. Transform the MUNDEROVER into
           a MUNDER */
        newTypeNum = MathML_EL_MUNDER;
      else if (parentType.ElTypeNum == MathML_EL_MOVER &&
               !strcmp (TtaGetSSchemaName (parentType.ElSSchema), "MathML"))
        /* an Overscript in a MOVER. Remove the MOVER and the
           UnderOverBase */
        newTypeNum = -1;
      break;

    case MathML_EL_LabelCell:
      /* delete the RowLabel parent too */
      el = parent;
      parent = TtaGetParent (parent);
      parentType = TtaGetElementType (parent);
      TtaRegisterElementDelete (el, event->document);
      TtaDeleteTree (el, event->document);
      el = NULL;
      /* NO BREAK: continue to change the type of the mlabeledtr element */
    case MathML_EL_RowLabel:
      /* a label in a mlabeledtr element has been deleted */
      if (parentType.ElTypeNum == MathML_EL_MLABELEDTR &&
          strcmp (TtaGetSSchemaName (parentType.ElSSchema), "MathML") == 0)
        newTypeNum = MathML_EL_MTR;
      break;
	
    default:
      break;
    }

  oldStructureChecking = TtaGetStructureChecking (event->document);
  TtaSetStructureChecking (FALSE, event->document);
  if (newTypeNum > 0)
    /* transform the parent element */
    {
      TtaChangeTypeOfElement (parent, event->document, newTypeNum);
      TtaRegisterElementTypeChange (parent, parentType.ElTypeNum,
                                    event->document);
    }
  else if (newTypeNum < 0)
    /* put the content of the single sibling of the deleted element
       instead of the parent element */
    {
      child = TtaGetFirstChild (parent);
      if (child != NULL)
        {
          grandChild = TtaGetFirstChild (child);
          if (grandChild != NULL)
            {
              TtaRegisterElementDelete (grandChild, event->document);
              TtaRemoveTree (grandChild, event->document);
              TtaInsertSibling (grandChild, parent, TRUE, event->document);
              TtaRegisterElementCreate (grandChild, event->document);
              TtaRegisterElementDelete (parent, event->document);
              TtaDeleteTree (parent, event->document);
              placeholderEl = InsertPlaceholder (grandChild, FALSE,
                                                 event->document, FALSE/******/);
              placeholderEl = InsertPlaceholder (grandChild, TRUE,
                                                 event->document, FALSE/******/);
            }
        }
    }
  TtaSetStructureChecking (oldStructureChecking, event->document);
}

/*----------------------------------------------------------------------
  MathMLElementTypeInMenu
  -----------------------------------------------------------------------*/
ThotBool MathMLElementTypeInMenu (NotifyElement *event)
{
  return TRUE; /* prevent Thot from putting this element name in the
                  element creation menu */
}

/*----------------------------------------------------------------------
  FenceModified
  The opening or closing fence element in a MFENCED element has been modified
  by the user. Update the corresponding open or close attribute.
  -----------------------------------------------------------------------*/
void FenceModified (NotifyOnValue *event)
{
  Element	mfencedEl;
  ElementType	elType;
  AttributeType	attrType;
  Attribute	attr;
  unsigned char text[2];

  mfencedEl = TtaGetParent (event->element);
  elType = TtaGetElementType (event->element);
  if (elType.ElTypeNum == MathML_EL_OpeningFence)
    attrType.AttrTypeNum = MathML_ATTR_open;
  else
    attrType.AttrTypeNum = MathML_ATTR_close;
  attrType.AttrSSchema = elType.ElSSchema;
  attr = TtaGetAttribute (mfencedEl, attrType);
  if (attr == NULL)
    /* no attribute open on this MFENCED element. Create one */
    {
      attr =  TtaNewAttribute (attrType);
      TtaAttachAttribute (mfencedEl, attr, event->document);
    }
  text[0] = (unsigned char) event->value;
  text[1] = '\0';
  TtaSetAttributeText (attr, (char *)text, mfencedEl, event->document);
}

/*----------------------------------------------------------------------
  MathAttrOtherCreated
  An attribute other has been created by the user. If that attribute is on
  a TEXT element, move it to the parent.
  -----------------------------------------------------------------------*/
void MathAttrOtherCreated (NotifyAttribute *event)
{
  AttrToSpan (event->element, event->attribute, event->document);
}

/*----------------------------------------------------------------------
  MathEntityModified
  An attribute EntityName has been modified by the user. Update the
  content of the corresponding element accordingly.
  -----------------------------------------------------------------------*/
void MathEntityModified (NotifyAttribute *event)
{
  Element        el;
  char          *value;
  int            length, i;
  ThotBool       changed;

  value = (char *)TtaGetMemory (BUFLEN);
  value[0] = EOS;
  changed = FALSE;
  length = TtaGetTextAttributeLength (event->attribute);
  if (length >= BUFLEN)
    length = BUFLEN - 2;
  if (length <= 0)
    /* entity name emty. Unknown entity. */
    {
      value[0] = START_ENTITY;
      value[1] = '?';
      length = 3;
      changed = TRUE;
    }
  else
    {
      TtaGiveTextAttributeValue (event->attribute, value, &length);
      /* the first character must always be START_ENTITY */
      if (value[0] != START_ENTITY)
        {
          if (value[0] != '&')
            {
              for (i = length; i > 0; i--)
                value[i] = value[i-1];
              length++;
            }
          value[0] = START_ENTITY;
          changed = TRUE;
        }
      if (value[length-1] != ';')
        {
          length++;
          changed = TRUE;
        }
    }
  value[length - 1] = EOS;
  el = event->element;
  SetContentAfterEntity (&value[1], el, event->document);
  value[length - 1] = ';';
  value[length] = EOS;
  if (changed)
    TtaSetAttributeText (event->attribute, value, el, event->document);
  TtaFreeMemory (value);
  ParseMathString (el, TtaGetParent (el), event->document);
}

/*----------------------------------------------------------------------
  MathDisplayAttrCreated
  An attribute display has been created or modified by the user.
  -----------------------------------------------------------------------*/
void MathDisplayAttrCreated (NotifyAttribute *event)
{
  ParseHTMLSpecificStyle (event->element, (char*)"display:block", event->document,
                          0, TRUE);
  SetDisplaystyleMathElement (event->element, event->document);
}

/*----------------------------------------------------------------------
  MathDisplayAttrDeleted
  The user has deleted an attribute display
  -----------------------------------------------------------------------*/
void MathDisplayAttrDeleted (NotifyAttribute *event)
{
  ParseHTMLSpecificStyle (event->element, (char*)"display:inline", event->document,
                          0, TRUE);
  SetDisplaystyleMathElement (event->element, event->document);
}

/*----------------------------------------------------------------------
  MathDisplaystyleAttrCreated
  An attribute displaystyle has been created or modified by the user.
  -----------------------------------------------------------------------*/
void MathDisplaystyleAttrCreated (NotifyAttribute *event)
{
  MathMLSetDisplaystyleAttr (event->element, event->attribute, event->document,
                             FALSE);
}

/*----------------------------------------------------------------------
  MathDisplaystyleAttrDeleted
  The user has deleted an attribute displaystyle
  -----------------------------------------------------------------------*/
void MathDisplaystyleAttrDeleted (NotifyAttribute *event)
{
  MathMLSetDisplaystyleAttr (event->element, NULL, event->document, TRUE);
}

/*----------------------------------------------------------------------
  MathPresentAttrCreated
  An attribute fontsize, mathsize, lspace, rspace, linethickness has been
  created or updated by the user.
  -----------------------------------------------------------------------*/
void MathPresentAttrCreated (NotifyAttribute *event)
{
  char          *value;
  int            length, attrKind;
  AttributeType  attrType;
  ThotBool       doit;

  doit = TRUE;
  TtaGiveAttributeType (event->attribute, &attrType, &attrKind);
  if (attrType.AttrTypeNum == MathML_ATTR_fontsize)
    /* it's a fontsize attribute. If the same element has a mathsize
       attribute, don't change the element size */
    {
      attrType.AttrTypeNum = MathML_ATTR_mathsize;
      if (TtaGetAttribute (event->element, attrType))
        doit = FALSE;
    }
  if (doit)
    {
      value = (char *)TtaGetMemory (BUFLEN);
      value[0] = EOS;
      length = TtaGetTextAttributeLength (event->attribute);
      if (length >= BUFLEN)
        length = BUFLEN - 1;
      if (length > 0)
        TtaGiveTextAttributeValue (event->attribute, value, &length);
      /* associate a CSS property with the element */
      if (attrType.AttrTypeNum == MathML_ATTR_linethickness)
        MathMLlinethickness (event->document, event->element, value);
      else
        MathMLAttrToStyleProperty (event->document, event->element, value,
                                   attrType.AttrTypeNum);
      TtaFreeMemory (value);
    }
}
 
/*----------------------------------------------------------------------
  MathAttrFontsizeDelete
  The user is deleting an attribute fontsize or mathsize.
  -----------------------------------------------------------------------*/
ThotBool MathAttrFontsizeDelete (NotifyAttribute *event)
{
  int            attrKind;
  AttributeType  attrType;
  ThotBool       doit;

  doit = TRUE;
  TtaGiveAttributeType (event->attribute, &attrType, &attrKind);
  if (attrType.AttrTypeNum == MathML_ATTR_fontsize)
    /* it's a fontsize attribute. If the same element has a mathsize
       attribute, don't change the element size */
    {
      attrType.AttrTypeNum = MathML_ATTR_mathsize;
      if (TtaGetAttribute (event->element, attrType))
        doit = FALSE;
    }
  if (doit)
    /* ask the CSS handler to remove the effect of the CSS property
       font-size */
    /* in the statement below, "10pt" is meaningless. It's here just to
       make the CSS parser happy */
    ParseHTMLSpecificStyle (event->element, (char*)"font-size: 10pt",
                            event->document, 0, TRUE);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  MathStyleAttrInMenu
  Called by Thot when building the Attribute menu.
  Prevent Thot from including a deprecated attribute in the menu.
  ----------------------------------------------------------------------*/
ThotBool            MathStyleAttrInMenu (NotifyAttribute * event)
{
  return TRUE;	/* don't put a deprecated attribute in the menu */
}

/*----------------------------------------------------------------------
  MathAttrLspaceDelete
  The user is deleting an attribute lspace.
  -----------------------------------------------------------------------*/
ThotBool MathAttrLspaceDelete (NotifyAttribute *event)
{
  /* ask the CSS handler to remove the effect of the CSS property
     padding-left */
  /* in the statement below, "10pt" is meaningless. It's here just to
     make the CSS parser happy */
  ParseHTMLSpecificStyle (event->element, (char*)"padding-left: 10pt",
                          event->document, 0, TRUE);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  MathAttrRspaceDelete
  The user is deleting an attribute rspace.
  -----------------------------------------------------------------------*/
ThotBool MathAttrRspaceDelete (NotifyAttribute *event)
{
  /* ask the CSS handler to remove the effect of the CSS property
     padding-right */
  /* in the statement below, "10pt" is meaningless. It's here just to
     make the CSS parser happy */
  ParseHTMLSpecificStyle (event->element, (char*)"padding-right: 10pt",
                          event->document, 0, TRUE);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  MathAttrFontfamilyCreated
  An attribute fontfamily has been created or modified by the user.
  -----------------------------------------------------------------------*/
void MathAttrFontfamilyCreated (NotifyAttribute *event)
{
  char            *value;
  int              length;

  value = (char *)TtaGetMemory (BUFLEN);
  value[0] = EOS;
  length = TtaGetTextAttributeLength (event->attribute);
  if (length >= BUFLEN)
    length = BUFLEN - 1;
  if (length > 0)
    TtaGiveTextAttributeValue (event->attribute, value, &length);  
  SetFontfamily (event->document, event->element, value);
  TtaFreeMemory (value);
}


/*----------------------------------------------------------------------
  MathAttrFontfamilyDelete
  The user is deleting an attribute fontfamily.
  -----------------------------------------------------------------------*/
ThotBool MathAttrFontfamilyDelete (NotifyAttribute *event)
{
  /* ask the CSS handler to remove the effect of property font-family */
  /* in the statement below, "serif" is meaningless. It's here just to
     make the CSS parser happy */
  ParseHTMLSpecificStyle (event->element, (char*)"font-family: serif",
                          event->document, 0, TRUE);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  MathAttrColorCreated
  An attribute color or mathcolor has been created or modified by the user.
  -----------------------------------------------------------------------*/
void MathAttrColorCreated (NotifyAttribute *event)
{
  AttributeType  attrType;
  char          *value;
  int            attrKind, length;
  ThotBool       doit;

  doit = TRUE;
  TtaGiveAttributeType (event->attribute, &attrType, &attrKind);
  if (attrType.AttrTypeNum == MathML_ATTR_color)
    /* it's a color attribute. If the same element has a mathcolor
       attribute, don't change the element color */
    {
      attrType.AttrTypeNum = MathML_ATTR_mathcolor;
      if (TtaGetAttribute (event->element, attrType))
        doit = FALSE;
    }
  if (doit)
    {
      value = (char *)TtaGetMemory (BUFLEN);
      value[0] = EOS;
      length = TtaGetTextAttributeLength (event->attribute);
      if (length >= BUFLEN)
        length = BUFLEN - 1;
      if (length > 0)
        TtaGiveTextAttributeValue (event->attribute, value, &length);  
      HTMLSetForegroundColor (event->document, event->element, 2000, value);
      TtaFreeMemory (value);
    }
}


/*----------------------------------------------------------------------
  MathAttrColorDelete
  The user is deleting an attribute color or mathcolor.
  -----------------------------------------------------------------------*/
ThotBool MathAttrColorDelete (NotifyAttribute *event)
{
  int            attrKind;
  AttributeType  attrType;
  ThotBool       doit;

  doit = TRUE;
  TtaGiveAttributeType (event->attribute, &attrType, &attrKind);
  if (attrType.AttrTypeNum == MathML_ATTR_color)
    /* it's a color attribute. If the same element has a mathcolor
       attribute, don't change the element's color */
    {
      attrType.AttrTypeNum = MathML_ATTR_mathcolor;
      if (TtaGetAttribute (event->element, attrType))
        doit = FALSE;
    }
  if (doit)
    HTMLResetForegroundColor (event->document, event->element);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  MathAttrFormChanged
  An attribute form has been created or modified by the user.
  -----------------------------------------------------------------------*/
void MathAttrFormChanged (NotifyAttribute *event)
{
  SetIntAddSpaceAttr (event->element, event->document);
}

/*----------------------------------------------------------------------
  MathAttrBackgroundCreated
  An attribute background or mathbackground has been created or modified
  by the user.
  -----------------------------------------------------------------------*/
void MathAttrBackgroundCreated (NotifyAttribute *event)
{
  char          *value;
  AttributeType  attrType;
  int            attrKind, length;
  ThotBool       doit;

  doit = TRUE;
  TtaGiveAttributeType (event->attribute, &attrType, &attrKind);
  if (attrType.AttrTypeNum == MathML_ATTR_background_)
    /* it's a background attribute. If the same element has a mathbackground
       attribute, don't change the element background */
    {
      attrType.AttrTypeNum = MathML_ATTR_mathbackground;
      if (TtaGetAttribute (event->element, attrType))
        doit = FALSE;
    }
  if (doit)
    {
      value = (char *)TtaGetMemory (BUFLEN);
      value[0] = EOS;
      length = TtaGetTextAttributeLength (event->attribute);
      if (length >= BUFLEN)
        length = BUFLEN - 1;
      if (length > 0)
        TtaGiveTextAttributeValue (event->attribute, value, &length);  
      HTMLSetBackgroundColor (event->document, event->element, 2000, value);
      TtaFreeMemory (value);
    }
}

/*----------------------------------------------------------------------
  MathAttrBackgroundDelete
  The user is deleting an attribute background or mathbackground.
  -----------------------------------------------------------------------*/
ThotBool MathAttrBackgroundDelete (NotifyAttribute *event)
{
  int            attrKind;
  AttributeType  attrType;
  ThotBool       doit;

  doit = TRUE;
  TtaGiveAttributeType (event->attribute, &attrType, &attrKind);
  if (attrType.AttrTypeNum == MathML_ATTR_background_)
    /* it's a background attribute. If the same element has a mathbackground
       attribute, don't change the element background */
    {
      attrType.AttrTypeNum = MathML_ATTR_mathbackground;
      if (TtaGetAttribute (event->element, attrType))
        doit = FALSE;
    }
  if (doit)
    HTMLResetBackgroundColor (event->document, event->element);
  return FALSE; /* let Thot perform normal operation */
}
 
/*----------------------------------------------------------------------
  AttrStretchyChanged
  Attribute stretchy in a MO element has been modified or deleted
  by the user.
  Change the Thot leaf child accordingly.
  -----------------------------------------------------------------------*/
void AttrStretchyChanged (NotifyAttribute *event)
{
  int		val;
  ElementType   elType;
  AttributeType attrType;
  Attribute     attr;

  if (event->element)
    {
      if (event->attribute == NULL)
        /* Attribute has been deleted */
        val = MathML_ATTR_stretchy_VAL_true;
      else
        /* attribute has been created or modified, get its new value */
        val = TtaGetAttributeValue (event->attribute);
      elType = TtaGetElementType (event->element);
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = MathML_ATTR_IntVertStretch;
      attr = TtaGetAttribute (event->element, attrType);
      if (val == MathML_ATTR_stretchy_VAL_true)
        {
          if (!attr)
            {
              /* attach a IntVertStretch attribute to the MF element */
              attrType.AttrTypeNum = MathML_ATTR_IntVertStretch;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (event->element, attr, event->document);
              TtaSetAttributeValue (attr, MathML_ATTR_IntVertStretch_VAL_yes_,
                                    event->element, event->document);
            }
        }
      else
        if (attr)
          TtaRemoveAttribute (event->element, attr, event->document);
    }
}

/*----------------------------------------------------------------------
  AttrLargeopChanged
  Attribute largeop in a mo or mstyle element has been modified or deleted
  by the user.
  -----------------------------------------------------------------------*/
void AttrLargeopChanged (NotifyAttribute *event)
{
  ElementType   elType;

  /* process only element mo. Should also process element mstyle */
  elType = TtaGetElementType (event->element);
  if (elType.ElTypeNum == MathML_EL_MO)
    CheckLargeOp (event->element, event->document);
}

/*----------------------------------------------------------------------
  AttrMovablelimitsChanged
  Attribute movablelimits in a MO element has been modified or deleted
  by the user.
  It it is within a munderover, a munder or a mover, set attribute
  IntMovelimits accordingly.
  -----------------------------------------------------------------------*/
void AttrMovablelimitsChanged (NotifyAttribute *event)
{
  ElementType   elType;
  Element       el, parent, grandparent;

  el = event->element;
  if (el)
    {
      parent = TtaGetParent (el);
      if (parent)
        {
          elType = TtaGetElementType (parent);
          if (elType.ElTypeNum == MathML_EL_UnderOverBase &&
              !strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
            {
              grandparent = TtaGetParent (parent);
              if (grandparent)
                {
                  elType = TtaGetElementType (grandparent);
                  if ((elType.ElTypeNum == MathML_EL_MUNDER ||
                       elType.ElTypeNum == MathML_EL_MOVER ||
                       elType.ElTypeNum == MathML_EL_MUNDEROVER) &&
                      !strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
                    SetIntMovelimitsAttr (grandparent, event->document);
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  AttrSpacingCreated
  Attribute width, height or depth in a mspace or mpadded element has been
  modified by the user.
  -----------------------------------------------------------------------*/
void AttrSpacingCreated (NotifyAttribute *event)
{
  char            *value;
  int              length, attrKind;
  AttributeType    attrType;

  length = TtaGetTextAttributeLength (event->attribute);
  if (length > 0)
    {
      value = (char *)TtaGetMemory (length+1);
      value[0] = EOS;
      TtaGiveTextAttributeValue (event->attribute, value, &length);
      TtaGiveAttributeType (event->attribute, &attrType, &attrKind);
      MathMLSpacingAttr (event->document, event->element, value,
                         attrType.AttrTypeNum);
      TtaFreeMemory (value);
    }
}

/*----------------------------------------------------------------------
  AttrSpacingDelete
  The user is deleting an attribute width, height, or depth.
  -----------------------------------------------------------------------*/
ThotBool AttrSpacingDelete (NotifyAttribute *event)
{
  int              attrKind;
  AttributeType    attrType;

  TtaGiveAttributeType (event->attribute, &attrType, &attrKind);
  MathMLSpacingAttr (event->document, event->element, NULL,
                     attrType.AttrTypeNum);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  AttrLinethicknessDelete
  The user is deleting an attribute linethickness.
  -----------------------------------------------------------------------*/
ThotBool AttrLinethicknessDelete (NotifyAttribute *event)
{
  /* ask the CSS handler to remove the effect of property stroke-width */
  /* in the statement below, "1pt" is meaningless. It's here just to
     make the CSS parser happy */
  ParseHTMLSpecificStyle (event->element, (char*)"stroke-width: 1pt",
                          event->document, 0, TRUE);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  AttrBevelledChanged
  Attribute bevelled in a mfrac element has been modified or deleted
  by the user.
  Change the type of the element (MFRAC/BevelledMFRAC) accordingly.
  -----------------------------------------------------------------------*/
void AttrBevelledChanged (NotifyAttribute *event)
{
  int		val;
  ElementType   elType;

  if (event->element)
    {
      elType = TtaGetElementType (event->element);
      if (elType.ElTypeNum != MathML_EL_MFRAC &&
          elType.ElTypeNum != MathML_EL_BevelledMFRAC)
        return;
      if (event->attribute == NULL)
        /* Attribute bevelled has been deleted */
        {
          /* type should be MFRAC if it's present */
          if (elType.ElTypeNum != MathML_EL_MFRAC)
            TtaChangeTypeOfElement (event->element, event->document,
                                    MathML_EL_MFRAC);
        }
      else
        /* attribute bevelled has been created or modified */
        {
          /* get its new value */
          val = TtaGetAttributeValue (event->attribute);
          if (val == MathML_ATTR_bevelled_VAL_false)
            {
              /* element type should be MFRAC */
              if (elType.ElTypeNum != MathML_EL_MFRAC)
                TtaChangeTypeOfElement (event->element, event->document,
                                        MathML_EL_MFRAC);
            }
          else if (val == MathML_ATTR_bevelled_VAL_true)
            {
              /* element type should be BevelledMFRAC */
              if (elType.ElTypeNum != MathML_EL_BevelledMFRAC)
                TtaChangeTypeOfElement (event->element, event->document,
                                        MathML_EL_BevelledMFRAC);
            }
        }
    }
}

/*----------------------------------------------------------------------
  AttrScriptlevelCreated
  An attribute scriptlevel has been created or updated by the user.
  -----------------------------------------------------------------------*/
void AttrScriptlevelCreated (NotifyAttribute *event)
{
  char            *value;
  int              length;

  length = TtaGetTextAttributeLength (event->attribute);
  if (length > 0)
    {
      value = (char *)TtaGetMemory (length+1);
      value[0] = EOS;
      TtaGiveTextAttributeValue (event->attribute, value, &length);
      MathMLSetScriptLevel (event->document, event->element, value);
      TtaFreeMemory (value);
    }
}

/*----------------------------------------------------------------------
  AttrScriptlevelDelete
  The user is deleting an attribute scriptlevel.
  -----------------------------------------------------------------------*/
ThotBool AttrScriptlevelDelete (NotifyAttribute *event)
{
  MathMLSetScriptLevel (event->document, event->element, NULL);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  AttrOpenCloseChanged
  Attribute open or close in a MFENCED element has been modified or deleted
  by the user. Update the corresponding fence element.
  -----------------------------------------------------------------------*/
void AttrOpenCloseChanged (NotifyAttribute *event)
{
  Element	fence, content;
  int		length;
  char          text[8];

  if (event->attributeType.AttrTypeNum == MathML_ATTR_open)
    fence = TtaGetFirstChild (event->element);
  else
    fence = TtaGetLastChild (event->element);
  if (fence != NULL)
    {
      content = TtaGetFirstChild (fence);
      if (content != NULL)
        {
          if (event->attribute == NULL)
            /* Attribute has been deleted */
            if (event->attributeType.AttrTypeNum == MathML_ATTR_open)
              text[0] = '(';	/* default value for open */
            else
              text[0] = ')';	/* default value for close */
          else
            /* attribute has been modified, get its new value */
            {
              length = 7;
              TtaGiveTextAttributeValue (event->attribute, text, &length);
            }
          /* set the content of the fence element */
          TtaSetGraphicsShape (content, (char)text[0], event->document); 
        }
    }
}

/*----------------------------------------------------------------------
  FencedSeparatorModified
  The content of a FenceSeparator element has been modified by the user
  in a MFENCED element.  Update the corresponding separators attribute.
  -----------------------------------------------------------------------*/
void FencedSeparatorModified (NotifyOnValue *event)
{
  Element	mfencedEl, fencedExpEl, child, content;
  Attribute	attr;
  ElementType	elType;
  AttributeType	attrType;
  int		i, len;
  Language	lang;
  unsigned char         text[32];

  fencedExpEl = TtaGetParent (event->element);
  if (fencedExpEl == NULL)
    return;
  mfencedEl = TtaGetParent (fencedExpEl);
  if (mfencedEl == NULL)
    return;
  SetIntAddSpaceAttr (event->element, event->document);
  SetIntVertStretchAttr (event->element, event->document, 0, NULL);
  /**** CheckFence (event->element, event->document); ******/
 
  i = 0;
  child = TtaGetFirstChild (fencedExpEl);
  while (child != NULL)
    {
      elType = TtaGetElementType (child);
      if (elType.ElTypeNum == MathML_EL_FencedSeparator &&
          strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
        {
          content = TtaGetFirstChild (child);
          elType = TtaGetElementType (content);
          if (elType.ElTypeNum == MathML_EL_SYMBOL_UNIT)
            text[i] = TtaGetGraphicsShape (content);
          else
            {
              len = 31 - i;
              TtaGiveTextContent (content, &text[i], &len, &lang);
            }
          i++;
        }
      TtaNextSibling (&child);
    }
  text[i] = EOS;
  /* if the last character is repeated, delete the repeated characters */
  if (i > 1)
    {
      i--;
      while (text[i-1] == text[i] && i > 0)
        i--;
      text[i+1] = EOS;
    }
  elType = TtaGetElementType (event->element);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = MathML_ATTR_separators;
  attr = TtaGetAttribute (mfencedEl, attrType);
  if (attr == NULL)
    /* no attribute separators on this MFENCED element. Create one */
    {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (mfencedEl, attr, event->document);
    }
  /* set the value of the separators attribute */
  TtaSetAttributeText (attr, (char *)text, mfencedEl, event->document);
}


/*----------------------------------------------------------------------
  AttrSeparatorsChanged
  An attribute separators has been created, modified or deleted by the user
  for a MFENCED element. Update the corresponding FenceSeparator elements.
  -----------------------------------------------------------------------*/
void AttrSeparatorsChanged (NotifyAttribute *event)
{
  Element	child, fencedExpression;
  ElementType	elType;

  /* get the first child of the MFENCED element */
  child = TtaGetFirstChild (event->element);
  if (child == NULL)
    return;
  /* search the FencedExpression element among the children of MFENCED */
  fencedExpression = NULL;
  do
    {
      elType = TtaGetElementType (child);
      if (elType.ElTypeNum == MathML_EL_FencedExpression &&
          strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
        fencedExpression = child;
      else
        TtaNextSibling (&child);
    }
  while (fencedExpression == NULL && child != NULL);
  if (fencedExpression != NULL)
    RegenerateFencedSeparators (fencedExpression, event->document,FALSE/****/);
}

/*----------------------------------------------------------------------
  AttrScriptShiftCreated
  An attribute subscriptshift or superscriptshift has been created or
  updated by the user.
  -----------------------------------------------------------------------*/
void AttrScriptShiftCreated (NotifyAttribute *event)
{
  char            *value;
  int              length, attrKind;
  AttributeType    attrType;

  length = TtaGetTextAttributeLength (event->attribute);
  if (length > 0)
    {
      value = (char *)TtaGetMemory (length+1);
      value[0] = EOS;
      TtaGiveTextAttributeValue (event->attribute, value, &length);
      TtaGiveAttributeType (event->attribute, &attrType, &attrKind);
      MathMLScriptShift (event->document, event->element, value,
                         attrType.AttrTypeNum);
      TtaFreeMemory (value);
    }
}

/*----------------------------------------------------------------------
  AttrScriptShiftDelete
  The user is deleting an attribute subscriptshift or superscriptshift.
  -----------------------------------------------------------------------*/
ThotBool AttrScriptShiftDelete (NotifyAttribute *event)
{
  int              attrKind;
  AttributeType    attrType;

  TtaGiveAttributeType (event->attribute, &attrType, &attrKind);
  MathMLScriptShift (event->document, event->element, NULL,
                     attrType.AttrTypeNum);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  AttrAltModified
  An attribute ALT has been created or modified by the user.
  -----------------------------------------------------------------------*/
void AttrAltModified (NotifyAttribute *event)
{
  ElementType      elType;
  Element          parent;

  parent = TtaGetParent (event->element);
  if (parent)
    {
      elType = TtaGetElementType (parent);
      if (elType.ElTypeNum == MathML_EL_MI)
        /* the parent element is a mi */
        /* set the internal FonstStyle attribute according to the value
           of the alt attribute */
        SetFontstyleAttr (parent, event->document);
    }
}

/*----------------------------------------------------------------------
  AttrRowAlignCreated
  An attribute rowalign has been created or updated by the user on a mstyle
  or mtable element. Create or update the corresponding IntRowAlign attributes
  for the enclosed mrow elements.
  -----------------------------------------------------------------------*/
void AttrRowAlignCreated (NotifyAttribute *event)
{
  HandleRowalignAttribute (event->attribute, event->element, event->document,
                           FALSE);
}

/*----------------------------------------------------------------------
  AttrRowAlignDeleted
  The user has deleted an attribute rowalign from a mstyle or mtable element.
  Remove the corresponding IntRowAlign attributes attached to the enclosed
  mrow elements.
  -----------------------------------------------------------------------*/
void AttrRowAlignDeleted (NotifyAttribute *event)
{
  HandleRowalignAttribute (NULL, event->element, event->document, TRUE);
}

/*----------------------------------------------------------------------
  AttrRowAlignMtrCreated
  An attribute rowalign_mtr has been created or updated by the user.
  Create or update the corresponding IntRowAlign attribute
  -----------------------------------------------------------------------*/
void AttrRowAlignMtrCreated (NotifyAttribute *event)
{
  MathMLAttributeComplete (event->attribute, event->element, event->document);
}

/*----------------------------------------------------------------------
  AttrRowAlignMtrDeleted
  The user has deleted an attribute rowalign_mtr. Delete the corresponding
  IntRowAlign attribute attached to the same element.
  -----------------------------------------------------------------------*/
void AttrRowAlignMtrDeleted (NotifyAttribute *event)
{
  AttributeType    attrType;
  Attribute        intAttr, attr;
  ElementType      elType;
  Element          asc;

  attrType = event->attributeType;
  attrType.AttrTypeNum = MathML_ATTR_IntRowAlign;
  intAttr = TtaGetAttribute (event->element, attrType);
  if (intAttr)
    TtaRemoveAttribute (event->element, intAttr, event->document);
  /* check if the enclosing mtable has a rowalign attribute that
     could apply to that element */
  elType = TtaGetElementType (event->element);
  elType.ElTypeNum = MathML_EL_MTABLE;
  asc = TtaGetTypedAncestor (event->element, elType);
  if (asc)
    {
      attrType.AttrTypeNum = MathML_ATTR_rowalign;
      attr = TtaGetAttribute (asc, attrType);
      if (attr)
        HandleRowalignAttribute (attr, asc, event->document, FALSE);
    }
}

/*----------------------------------------------------------------------
  AttrColAlignCreated
  An attribute columnalign has been created or updated by the user on a mstyle,
  mtable or mtr element. Create or update the corresponding IntColAlign
  attributes for all concerned cells.
  -----------------------------------------------------------------------*/
void AttrColAlignCreated (NotifyAttribute *event)
{
  HandleColalignAttribute (event->attribute, event->element, event->document,
                           FALSE, FALSE);
}

/*----------------------------------------------------------------------
  AttrColAlignDeleted
  The user has deleted an attribute columnalign from a mstyle, mtable or
  mtr element.
  Remove the corresponding IntColAlign attributes attached to the corresponding
  cells.
  -----------------------------------------------------------------------*/
void AttrColAlignDeleted (NotifyAttribute *event)
{
  ElementType    elType;
  Element        asc;
  Attribute      attr;

  HandleColalignAttribute (NULL, event->element, event->document, TRUE, FALSE);
  /* if the enclosing mtable has a columnalign attribute applies it */
  elType = TtaGetElementType (event->element);
  if ((elType.ElTypeNum == MathML_EL_MTR ||
       elType.ElTypeNum == MathML_EL_MLABELEDTR) &&
      strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
    {
      elType.ElTypeNum = MathML_EL_MTABLE;
      asc = TtaGetTypedAncestor (event->element, elType);
      if (asc)
        {
          attr = TtaGetAttribute (asc, event->attributeType);
          if (attr)
            HandleColalignAttribute (attr, asc, event->document, FALSE, FALSE);
        }
    }
}

/*----------------------------------------------------------------------
  AttrColAlignMtdCreated
  An attribute colalign_mtd has been created or updated by the user.
  Create or update the corresponding IntColAlign attribute
  -----------------------------------------------------------------------*/
void AttrColAlignMtdCreated (NotifyAttribute *event)
{
  MathMLAttributeComplete (event->attribute, event->element, event->document);
}

/*----------------------------------------------------------------------
  AttrColAlignMtdDeleted
  The user has deleted an attribute colalign_mtd. Delete the corresponding
  IntColAlign attribute attached to the same element.
  -----------------------------------------------------------------------*/
void AttrColAlignMtdDeleted (NotifyAttribute *event)
{
  AttributeType    attrType;
  Attribute        intAttr, attr;
  Element          asc;
  ElementType      elType;

  attrType = event->attributeType;
  attrType.AttrTypeNum = MathML_ATTR_IntColAlign;
  intAttr = TtaGetAttribute (event->element, attrType);
  if (intAttr)
    TtaRemoveAttribute (event->element, intAttr, event->document);
  /* check if the enclosing mtd or mtable has a columnalign attribute that
     could apply to that cell */
  elType = TtaGetElementType (event->element);
  elType.ElTypeNum = MathML_EL_TableRow;
  asc = TtaGetTypedAncestor (event->element, elType);
  if (asc)
    {
      attrType.AttrTypeNum = MathML_ATTR_columnalign;
      attr = TtaGetAttribute (asc, attrType);
      if (attr)
        HandleColalignAttribute (attr, asc, event->document, FALSE, FALSE);
      else
        /* no columnalign on the enclosing row. Look at the enclosing table */
        {
          elType.ElTypeNum = MathML_EL_MTABLE;
          asc = TtaGetTypedAncestor (asc, elType);
          if (asc)
            {
              attr = TtaGetAttribute (asc, attrType);
              if (attr)
                HandleColalignAttribute (attr, asc, event->document, FALSE,
                                         FALSE);
            }
        }
    }
}

/*----------------------------------------------------------------------
  AttrRowspacingCreated
  An attribute rowspacing has been created or updated by the user on a mstyle or
  mtable. Create or update the corresponding style for all cells.
  -----------------------------------------------------------------------*/
void AttrRowspacingCreated (NotifyAttribute *event)
{
  HandleRowspacingAttribute (event->attribute, event->element, event->document,
                             FALSE);
}

/*----------------------------------------------------------------------
  AttrRowspacingDeleted
  The user has deleted an attribute rowspacing from a mstyle or mtable
  element.
  Remove the corresponding style from all cells.
  -----------------------------------------------------------------------*/
void AttrRowspacingDeleted (NotifyAttribute *event)
{
  HandleRowspacingAttribute (NULL, event->element, event->document, TRUE);
}

/*----------------------------------------------------------------------
  AttrColumnspacingCreated
  An attribute columnspacing has been created or updated by the user on a
  mstyle or mtable. Create or update the corresponding style for all cells.
  -----------------------------------------------------------------------*/
void AttrColumnspacingCreated (NotifyAttribute *event)
{
  HandleColumnspacingAttribute (event->attribute, event->element,
                                event->document, FALSE);
}

/*----------------------------------------------------------------------
  AttrColumnspacingDeleted
  The user has deleted an attribute columnspacing from a mstyle or mtable
  element.
  Remove the corresponding style from all cells.
  -----------------------------------------------------------------------*/
void AttrColumnspacingDeleted (NotifyAttribute *event)
{
  HandleColumnspacingAttribute (NULL, event->element, event->document, TRUE);
}

/*----------------------------------------------------------------------
  AttrRowlinesCreated
  An attribute rowlines has been created or updated by the user on a mstyle or
  mtable. Create or update the corresponding style for all rows.
  -----------------------------------------------------------------------*/
void AttrRowlinesCreated (NotifyAttribute *event)
{
  HandleRowlinesAttribute (event->attribute, event->element, event->document,
                           FALSE);
}

/*----------------------------------------------------------------------
  AttrRowlinesDeleted
  The user has deleted an attribute rowlines from a mstyle or mtable
  element.
  Remove the corresponding style from all rows.
  -----------------------------------------------------------------------*/
void AttrRowlinesDeleted (NotifyAttribute *event)
{
  HandleRowlinesAttribute (NULL, event->element, event->document, TRUE);
}

/*----------------------------------------------------------------------
  AttrColumnlinesCreated
  An attribute columnlines has been created or updated by the user on a mstyle
  or mtable. Create or update the corresponding style for all columns.
  -----------------------------------------------------------------------*/
void AttrColumnlinesCreated (NotifyAttribute *event)
{
  HandleColumnlinesAttribute (event->attribute, event->element,
                              event->document, FALSE);
}

/*----------------------------------------------------------------------
  AttrColumnlinesDeleted
  The user has deleted an attribute rowlines from a mstyle or mtable
  element.
  Remove the corresponding style from all concerned rows.
  -----------------------------------------------------------------------*/
void AttrColumnlinesDeleted (NotifyAttribute *event)
{
  HandleColumnlinesAttribute (NULL, event->element, event->document, TRUE);
}

/*----------------------------------------------------------------------
  AttrFramespacingCreated
  An attribute framespacing has been created or updated by the user on a mstyle
  or mtable element. Create or update the corresponding style for the table(s).
  -----------------------------------------------------------------------*/
void AttrFramespacingCreated (NotifyAttribute *event)
{
  HandleFramespacingAttribute (event->attribute, event->element,
                               event->document, FALSE);
}

/*----------------------------------------------------------------------
  AttrFramespacingDeleted
  The user has deleted an attribute framespacing from a mstyle or mtable
  element.
  Remove the corresponding style from the concerned table(s).
  -----------------------------------------------------------------------*/
void AttrFramespacingDeleted (NotifyAttribute *event)
{
  HandleFramespacingAttribute (NULL, event->element, event->document, TRUE);
}

/*----------------------------------------------------------------------
  HandleColAndRowAlignAttributes
  If element row is a MathML mtr element, check the rowalign and columnalign
  attributes on the enclosing mtable element and on the row itself. Check
  also the rowlines and columnlines attributes on the enclosing mtable
  element.
  ----------------------------------------------------------------------*/
void HandleColAndRowAlignAttributes (Element row, Document doc)
{
  Element          table;
  ElementType      elType;
  AttributeType    attrType;
  Attribute        attr;

  elType = TtaGetElementType (row);
  if (strcmp (TtaGetSSchemaName(elType.ElSSchema), "MathML"))
    /* it's not an element from the MathML namespace */
    return;
  if (elType.ElTypeNum != MathML_EL_MTR &&
      elType.ElTypeNum != MathML_EL_MLABELEDTR &&
      elType.ElTypeNum != MathML_EL_MTable_body &&
      elType.ElTypeNum != MathML_EL_MTABLE)
    /* it's not a table row nor a table body nor a table */
    return;

  attrType.AttrSSchema = elType.ElSSchema;
  if (elType.ElTypeNum == MathML_EL_MTR ||
      elType.ElTypeNum == MathML_EL_MLABELEDTR)
    /* it's a row in a MathML table. Check its colalign attribute */
    {
      attrType.AttrTypeNum = MathML_ATTR_columnalign;
      attr = TtaGetAttribute (row, attrType);
      if (attr)
        /* the row element has a columnalign attribute.
           applies that attribute again to that row */
        HandleColalignAttribute (attr, row, doc, FALSE, TRUE);
    }

  /* get the enclosing mtable element */
  if (elType.ElTypeNum == MathML_EL_MTABLE)
    table = row;
  else
    {
      elType.ElTypeNum = MathML_EL_MTABLE;
      table = TtaGetTypedAncestor (row, elType);
    }

  attrType.AttrTypeNum = MathML_ATTR_columnalign;
  attr = TtaGetAttribute (table, attrType);
  if (attr)
    /* the enclosing mtable element has a columnalign attribute.
       applies that attribute again to the whole table */
    HandleColalignAttribute (attr, table, doc, FALSE, TRUE);
  attrType.AttrTypeNum = MathML_ATTR_rowalign;
  attr = TtaGetAttribute (table, attrType);
  if (attr)
    /* the enclosing mtable element has a rowalign attribute.
       applies that attribute again to the whole table */
    HandleRowalignAttribute (attr, table, doc, FALSE);
  attrType.AttrTypeNum = MathML_ATTR_rowspacing;
  attr = TtaGetAttribute (table, attrType);
  /* if the enclosing mtable element has a rowspacing attribute, applies that
     attribute again to the whole table, otherwise, just set the external
     padding to 0. */
  HandleRowspacingAttribute (attr, table, doc, FALSE);
  attrType.AttrTypeNum = MathML_ATTR_columnspacing;
  attr = TtaGetAttribute (table, attrType);
  /* if the enclosing mtable element has a columnspacing attribute, applies
     that attribute again to the whole table, otherwise, just set the external
     padding to 0. */
  HandleColumnspacingAttribute (attr, table, doc, FALSE);

  attrType.AttrTypeNum = MathML_ATTR_rowlines;
  attr = TtaGetAttribute (table, attrType);
  if (attr)
    /* the enclosing mtable element has a rowlines attribute.
       applies that attribute again to the whole table */
    HandleRowlinesAttribute (attr, table, doc, FALSE);
  attrType.AttrTypeNum = MathML_ATTR_columnlines;
  attr = TtaGetAttribute (table, attrType);
  if (attr)
    /* the enclosing mtable element has a columnlines attribute.
       applies that attribute again to the whole table */
    HandleColumnlinesAttribute (attr, table, doc, FALSE);
}
