/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2002
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module contains editing functions for handling MathML objects.
 *
 * Authors: I. Vatton, V. Quint
 *          R. Guetari - Windows routines.
 */

/* Included headerfiles */
#define THOT_EXPORT
#include "amaya.h"
#include "css.h"
#include "trans.h"
#include "undo.h"
#include "interface.h"
#include "MathML.h"
#ifdef _SVG
#include "SVG.h"
#endif /* _SVG */

#ifdef _WINDOWS
#define iconMath   21 
#define iconMathNo 21 
#else /* _WINDOWS */
static ThotIcon	   iconMath;
static ThotIcon	   iconMathNo;

#include "Math.xpm"
#include "MathNo.xpm"
#include "Bmath.xpm"
#include "root.xpm"
#include "sqrt.xpm"
#include "frac.xpm"
#include "subsup.xpm"
#include "sup.xpm"
#include "sub.xpm"
#include "overunder.xpm"
#include "over.xpm"
#include "under.xpm"
#include "fence.xpm"
#include "mscript.xpm"
#include "matrix.xpm"
#include "greek.xpm"
#endif /* _WINDOWS */

static int      MathButton;
static Pixmap	mIcons[14];
static ThotBool	InitMaths;
static ThotBool	IsLastDeletedElement = FALSE;
static Element	LastDeletedElement = NULL;

#include "fetchXMLname_f.h"
#include "SVGbuilder_f.h"
#include "html2thot_f.h"
#include "HTMLtable_f.h"
#include "HTMLactions_f.h"
#include "HTMLpresentation_f.h"
#include "MathMLbuilder_f.h"
#include "styleparser_f.h"
#include "trans_f.h"
#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */
#include "XLinkedit_f.h"
#ifdef _GTK
/* used for teh close palette callback*/
ThotWidget CatWidget(int ref);
#endif/*  _GTK */
/* Function name table */
typedef char     functName[10];
static  functName  functionName[] =
{
   "Imaginary",
   "Real",
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
   "cosh",
   "cos",
   "coth",
   "cot",
   "csch",
   "csc",
   "curl",
   "det",
   "div",
   "gcd",
   "grad",
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
  if (elType.ElTypeNum == MathML_EL_MO)
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
	      SetIntVertStretchAttr (grandParent, doc, parentType.ElTypeNum,
				     selEl);
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
  int                oldStructureChecking, len;
  ThotBool           withinMrow, before;

  /* do not check the Thot abstract tree against the structure schema while
     changing the structure */
  oldStructureChecking = TtaGetStructureChecking (doc);
  TtaSetStructureChecking (0, doc);

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
  TtaSetStructureChecking ((ThotBool)oldStructureChecking, doc);
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
  ThotBool	createConstruct, oldStructureChecking, afterMsubsup;

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
	TtaPreviousSibling (&sibling);
      else
	{
	  TtaNextSibling (&sibling);
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
	  TtaSetStructureChecking (0, doc);
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
static void	   CreateParentMROW (Element el, Document doc)
{
  Element            sibling, row, parent, firstChild, lastChild, next,
                     previous;
  ElementType        elType;
  int                nChildren, oldStructureChecking;

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
	  elType.ElTypeNum != MathML_EL_CellWrapper)
	/* element is not an intermediate Thot element nor a text leaf */
	{
	/* count the number of children of parent that are not placeholders */
	sibling = TtaGetFirstChild (parent);
	nChildren = 0;
	firstChild = sibling;
	while (sibling != NULL)
	  {
	    elType = TtaGetElementType (sibling);
	    if (elType.ElTypeNum != MathML_EL_Construct &&
		elType.ElTypeNum != MathML_EL_Construct1)
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
	    TtaSetStructureChecking (0, doc);
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
	    TtaSetStructureChecking ((ThotBool)oldStructureChecking, doc);
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
   AddParen
   Create two MF element at the beginning and at the end of the
   MROW element el.
  ----------------------------------------------------------------------*/
static void         AddParen (Element el, Document doc)
{
  Element     child, fence, symbol, next;
  ElementType elType, symbType;

  if (!el)
     return;
  child = TtaGetFirstChild (el);
  if (child)
    {
      elType = TtaGetElementType (el);
      elType.ElTypeNum = MathML_EL_MF;
      fence = TtaNewElement (doc, elType);
      TtaInsertSibling (fence, child, TRUE, doc);
      symbType.ElSSchema = elType.ElSSchema;
      symbType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
      symbol = TtaNewElement (doc, symbType);
      TtaSetGraphicsShape (symbol, '(', doc);
      TtaInsertFirstChild (&symbol, fence, doc);

      next = child;
      do
	{
          TtaNextSibling (&next);
	  if (next)
	     child = next;
	}
      while (next);
      fence = TtaNewElement (doc, elType);
      TtaInsertSibling (fence, child, FALSE, doc);
      symbol = TtaNewElement (doc, symbType);
      TtaSetGraphicsShape (symbol, ')', doc);
      TtaInsertFirstChild (&symbol, fence, doc);
    }
}

/*----------------------------------------------------------------------
   CreateMathConstruct
   Create a MathML construct at the current position
  ----------------------------------------------------------------------*/
static void         CreateMathConstruct (int construct)
{
  Document           doc;
  Element            sibling, el, row, child, leaf, placeholderEl,
                     parent, new, next, foreignObj;
  ElementType        newType, elType, parentType;
  Attribute          attr;
  AttributeType      attrType;
  SSchema            docSchema, mathSchema;
  char              *name;
  DisplayMode        dispMode;
  int                c1, i, len, oldStructureChecking, col;
  ThotBool	     before, ParBlock, emptySel, ok, insertSibling,
		     selectFirstChild, displayTableForm, registered;

  doc = TtaGetSelectedDocument ();
  if (!TtaGetDocumentAccessMode (doc))
    /* the document is in ReadOnly mode */
    return;

  docSchema = TtaGetDocumentSSchema (doc);
  TtaGiveFirstSelectedElement (doc, &sibling, &c1, &i); 
  emptySel = TtaIsSelectionEmpty ();
  /* Get the type of the first selected element */
  elType = TtaGetElementType (sibling);
  name = TtaGetSSchemaName (elType.ElSSchema);
#ifdef _SVG
  if (construct == 1 && strcmp (name, "SVG"))
    /* Math button and selection is not in a SVG element */
#else /* _SVG */
  if (construct == 1)
    /* Math button */
#endif /* _SVG */
    {
      if (strcmp (name, "MathML"))
	/* selection is not in a MathML element */
	{
	  /* get the MathML schema associated with the current SSchema or
	     associate it with the current SSchema if it is not associated
	     yet */
	  mathSchema = TtaNewNature (doc, elType.ElSSchema, "MathML",
				     "MathMLP");
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
		  new = TtaNewElement (doc, newType);
		  TtaInsertFirstChild (&new, child, doc);
		  TtaRegisterElementCreate (child, doc);
		  TtaSetDocumentModified (doc);
		  TtaSetDisplayMode (doc, dispMode);
		  TtaSelectElement (doc, new);
		  TtaCloseUndoSequence (doc);
		  return;
		}
	    }
	  TtaCreateElement (newType, doc);
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
      if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
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
      mathSchema = TtaNewNature (doc, elType.ElSSchema, "MathML", "MathMLP");

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
	/* the current selection is the MathML root element */
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
		      if (dispMode == DisplayImmediately)
			TtaSetDisplayMode (doc, dispMode);
		      el = TtaNewElement (doc, elType);
		      TtaInsertSibling (el, sibling, FALSE, doc);
		      /* create a foreignObject element and insert it as
			 a child of the new switch element */
		      elType.ElTypeNum = SVG_EL_foreignObject;
		      TtaAskFirstCreation ();
		      foreignObj = TtaNewElement (doc, elType);
		      TtaInsertFirstChild (&foreignObj, el, doc);
		      /* register the new element in the Undo queue */
		      TtaRegisterElementCreate (el, doc);
		      registered = TRUE;
		      /* update depth of SVG elements */
		      SetGraphicDepths (doc, el);
		      elType.ElSSchema = mathSchema;
		      elType.ElTypeNum = MathML_EL_MathML;
		      sibling = foreignObj;
		      insertSibling = FALSE;
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
	      TtaSetStructureChecking (0, doc);
	      if (insertSibling)
		/* insert the new Math element as a sibling element */
		TtaInsertSibling (el, sibling, before, doc);
	      else
		/* insert the new Math element as a child element */
		TtaInsertFirstChild (&el, sibling, doc);
	      /* restore structure checking mode */
	      TtaSetStructureChecking ((ThotBool)oldStructureChecking, doc);
	      sibling = TtaGetFirstChild (el);
	      /* register the new Math element in the Undo queue */
	      if (!registered)
		TtaRegisterElementCreate (el, doc);
	      if (construct == 1)
		/* The <math> element requested is created. Return */
		{
		  TtaSetDocumentModified (doc);
		  TtaSetDisplayMode (doc, dispMode);
		  TtaSelectElement (doc, sibling);
		  TtaCloseUndoSequence (doc);
		  return;
		}
	    }
	}
    }
  
  elType = TtaGetElementType (sibling);
  newType.ElSSchema = mathSchema;
  selectFirstChild = TRUE;
  ParBlock = FALSE;
  switch (construct)
    {
    case 1:	/* create a Math element */
      /* handled above */
      break;
    case 2:
      newType.ElTypeNum = MathML_EL_MROOT;
      selectFirstChild = FALSE;	/* select the Index component */
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
    case 11:
      newType.ElTypeNum = MathML_EL_MROW;
      ParBlock = TRUE;
      selectFirstChild = FALSE;	/* select the second component */
      break;
    case 12:
      newType.ElTypeNum = MathML_EL_MMULTISCRIPTS;
      break;
    case 13:     /* MTABLE */
      displayTableForm = TtaIsSelectionEmpty ();
      if (displayTableForm)
	/* ask the user about the number of rows and columns to be created */
	{
	  NumberRows = 2;
	  NumberCols = 2;
#ifdef _WINDOWS
	  CreateMatrixDlgWindow (NumberCols, NumberRows);
#else  /* !_WINDOWS */
	  TtaNewForm (BaseDialog + TableForm, TtaGetViewFrame (doc, 1),
		      TtaGetMessage (1, BMatrix), TRUE, 1, 'L', D_CANCEL);
	  TtaNewNumberForm (BaseDialog + TableCols, BaseDialog + TableForm,
			    TtaGetMessage (AMAYA, AM_COLS), 1, 50, TRUE);
	  TtaNewNumberForm (BaseDialog + TableRows, BaseDialog + TableForm,
			    TtaGetMessage (AMAYA, AM_ROWS), 1, 200, TRUE);
	  TtaSetNumberForm (BaseDialog + TableCols, NumberCols);
	  TtaSetNumberForm (BaseDialog + TableRows, NumberRows);
	  TtaSetDialoguePosition ();
	  TtaShowDialogue (BaseDialog + TableForm, FALSE);
	  /* wait for an answer */
	  TtaWaitShowDialogue ();
	  if (!UserAnswer || NumberRows == 0 || NumberCols == 0)
	    /* the user decided to abort the command */
	    {
	      TtaCloseUndoSequence (doc);
	      return;
	    }
#endif /* !_WINDOWS */
	}
      else
	{
	  NumberRows = 0;
	  NumberCols = 0;
	}
      
      newType.ElTypeNum = MathML_EL_MTABLE;
      selectFirstChild = FALSE;	/* select the second component */
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
      break;

    default:
      TtaCloseUndoSequence (doc);
      return;
    }
  
  if (!emptySel)
    /* selection is not empty.
       Try to transform it into the requested type*/
    {
      if (!TransformIntoType (newType, doc))
	/* it failed. Try to insert a new element */
	emptySel = TRUE;
    }

  if (emptySel)
    {
      TtaUnselect (doc);
      el = TtaNewTree (doc, newType, "");
      /* do not check the Thot abstract tree against the structure */
      /* schema while changing the structure */
      oldStructureChecking = TtaGetStructureChecking (doc);
      TtaSetStructureChecking (0, doc);
      
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
		/* empty MATH element. Isert the new element as a child */
		{
		  TtaInsertFirstChild (&el, row, doc);
		  if (!registered)
		    TtaRegisterElementCreate (el, doc);
		}
	      else
		{
		  /* replace the empty MROW element by the new element */
		  TtaInsertSibling (el, row, TRUE, doc);
		  if (!registered)
		    {
		      TtaRegisterElementCreate (el, doc);
		      TtaRegisterElementDelete (row, doc);
		    }
		  TtaRemoveTree (row, doc);
		}
	    }
	  else
	    {
	      /* check whether the selected element is a Construct */
	      elType = TtaGetElementType (sibling);
	      if ((elType.ElTypeNum == MathML_EL_Construct ||
		   elType.ElTypeNum == MathML_EL_Construct1)&&
		  elType.ElSSchema == mathSchema)
		{
		  /* replace the Construct element */
		  TtaInsertSibling (el, sibling, FALSE, doc);
		  TtaRemoveTree (sibling, doc);
		}
	      else
		TtaInsertSibling (el, sibling, before, doc);
	      if (!registered)
		TtaRegisterElementCreate (el, doc);
	    }
	}
      else if ((elType.ElTypeNum == MathML_EL_Construct ||
		elType.ElTypeNum == MathML_EL_Construct1) &&
	       elType.ElSSchema == mathSchema)
	{
	  /* replace the Construct element */
	  TtaInsertSibling (el, sibling, FALSE, doc);
	  if (!registered)
	    {
	      TtaRegisterElementCreate (el, doc);
	      TtaRegisterElementDelete (sibling, doc);
	    }
	  TtaRemoveTree (sibling, doc);
	}
      else
	{
	  /* the selected element is not a MROW */
	  if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
	    /* go up to the MN, MI, MO or M_TEXT element */
	    sibling = TtaGetParent (sibling);
	  /* insert the new element */
	  TtaInsertSibling (el, sibling, before, doc);
	  if (!registered)
	    TtaRegisterElementCreate (el, doc);
	}
      
      TtaSetDocumentModified (doc);

      if (ParBlock)
	/* the user wants to create a parenthesized block */
	/* create two MF elements, as the first and last child of the new
	   MROW */
	AddParen (el, doc);
	  
      CreateParentMROW (el, doc);
      
      if (newType.ElTypeNum == MathML_EL_MTABLE &&
	  (NumberRows > 1 || NumberCols >= 1))
	{
	  /* create the required number of columns and rows in the table */
	  if (NumberCols >= 1)
	    {
	      elType.ElTypeNum = MathML_EL_TableRow;
	      child = TtaSearchTypedElement (elType, SearchInTree, el);
	      elType.ElTypeNum = MathML_EL_MTR;
	      new = TtaNewTree (doc, elType, "");
	      TtaInsertFirstChild (&new, child, doc);
	      elType.ElTypeNum = MathML_EL_MTD;
	      child = TtaSearchTypedElement (elType, SearchInTree, el);
	      col = NumberCols;
	      while (col > 1)
		{
		  new = TtaNewTree (doc, elType, "");
		  TtaInsertSibling (new, child, FALSE, doc);
		  col--;
		}
	    }
	  if (NumberRows > 1)
	    {
	      elType.ElTypeNum = MathML_EL_MTR;
	      row = TtaSearchTypedElement (elType, SearchInTree, el);
	      while (NumberRows > 1)
		{
		  elType.ElTypeNum = MathML_EL_MTR;
		  new = TtaNewTree (doc, elType, "");
		  TtaInsertSibling (new, row, FALSE, doc);
		  NumberRows--;
		  /* create cells within the row */
		  elType.ElTypeNum = MathML_EL_MTD;
		  child = TtaSearchTypedElement (elType, SearchInTree,new);
		  col = NumberCols;
		  while (col > 1)
		    {
		      new = TtaNewTree (doc, elType, "");
		      TtaInsertSibling (new, child, FALSE, doc);
		      col--;
		    }
		}
	    }
	  CheckAllRows (el, doc, FALSE, FALSE);
	}

      /* if the new element is a mspace, create a width attribute
	 with a default value */
      if (newType.ElTypeNum == MathML_EL_MSPACE)
	{
#define DEFAULT_MSPACE_WIDTH ".2em"
	  attrType.AttrSSchema = mathSchema;
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
	  elType.ElSSchema == mathSchema)
	RegenerateFencedSeparators (parent, doc, TRUE);

      /* insert placeholders before and/or after the new element if
	 they are needed */
      placeholderEl = InsertPlaceholder (el, TRUE, doc, !registered);
      placeholderEl = InsertPlaceholder (el, FALSE, doc, !registered);

      TtaSetDisplayMode (doc, dispMode);
      /* check the Thot abstract tree against the structure schema. */
      TtaSetStructureChecking ((ThotBool)oldStructureChecking, doc);
	  
      /* selected the leaf in the first (or second) child of the new
	 element */
      child = TtaGetFirstChild (el);
      if (!selectFirstChild)
	/* get the second child */
	TtaNextSibling (&child);
      leaf = NULL;
      while (child != NULL)
	{
	  leaf = child;
	  child = TtaGetFirstChild (child);
	}
      if (leaf)
	TtaSelectElement (doc, leaf);
    }

  TtaCloseUndoSequence (doc);
}

/*----------------------------------------------------------------------
   CallbackMaths: manage Maths dialogue events.
  ----------------------------------------------------------------------*/
static void CallbackMaths (int ref, int typedata, char *data)
{
  Document           doc;
  int                val = (int) data;

  ref -= MathsDialogue;
  if (ref == MenuMaths1)
    {
      ref = MenuMaths;
      val += 7;
    }
  switch (ref)
    {
    case FormMaths:
      /* the user has clicked the DONE button in the Math dialog box */
      InitMaths = FALSE;
      TtaDestroyDialogue (ref);	   
      break;

    case MenuMaths:
      /* the user has selected an entry in the math menu */
      doc = TtaGetSelectedDocument ();

      if (val == 13)
	/* the user asks for the Symbol palette */
	{
	  TtcDisplayGreekKeyboard (doc, 1);
	  return;
	}
      else if (doc > 0)
	/* there is a selection */
        if (TtaGetDocumentAccessMode (doc))
	   /* the document is in not in ReadOnly mode */
           CreateMathConstruct (val + 1);
      break;

    default:
      break;
    }
}

#ifdef _GTK
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
gboolean CloseMathMenu (GtkWidget *widget,
			 GdkEvent  *event,
			 gpointer   data )
{
  InitMaths = FALSE;
  TtaDestroyDialogue ((int) data);
  return TRUE;
}
#endif /* _GTK */

/*----------------------------------------------------------------------
   CreateMathMenu creates the maths menus.           
  ----------------------------------------------------------------------*/
static void CreateMathMenu (Document doc, View view)
{
#ifdef _GTK
  GtkWidget *w;
#endif /*_GTK*/
   if (!TtaGetDocumentAccessMode (doc))
     /* the document is in ReadOnly mode */
     return;

#ifndef _WINDOWS
  if (!InitMaths)
    {
      InitMaths = TRUE;

      /* Dialogue box for the Math palette */
      TtaNewSheet (MathsDialogue + FormMaths, TtaGetViewFrame (doc, view), 
		   TtaGetMessage (AMAYA, AM_BUTTON_MATH),
		   0, NULL, TRUE, 2, 'L', D_DONE);
      TtaNewIconMenu (MathsDialogue + MenuMaths, MathsDialogue + FormMaths, 0,
		   NULL, 7, mIcons, FALSE);
      TtaNewIconMenu (MathsDialogue + MenuMaths1, MathsDialogue + FormMaths, 0,
		   NULL, 7, &mIcons[7], FALSE);
      /* do not need to initialise the selection into the palette */
      /*TtaSetMenuForm (MathsDialogue + MenuMaths, 0);*/
      TtaSetDialoguePosition ();
#ifdef _GTK
      w =   CatWidget (MathsDialogue + FormMaths);
      gtk_signal_connect (GTK_OBJECT (w), 
			"delete_event",
			GTK_SIGNAL_FUNC (CloseMathMenu), 
			(gpointer)(MathsDialogue + FormMaths));

      gtk_signal_connect (GTK_OBJECT (w), 
			"destroy",
			GTK_SIGNAL_FUNC (CloseMathMenu), 
			(gpointer)(MathsDialogue + FormMaths));
#endif /*_GTK*/
    }
  TtaShowDialogue (MathsDialogue + FormMaths, TRUE); 
#else /* _WINDOWS */
  CreateMathDlgWindow (TtaGetViewFrame (doc, view));
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   AddMathButton        
  ----------------------------------------------------------------------*/
void                AddMathButton (Document doc, View view)
{
  MathButton = TtaAddButton (doc, 1, iconMath, CreateMathMenu,
			     "CreateMathMenu",
			     TtaGetMessage (AMAYA, AM_BUTTON_MATH),
			     TBSTYLE_BUTTON, TRUE);
}

/*----------------------------------------------------------------------
  SwitchIconMath
  ----------------------------------------------------------------------*/
void              SwitchIconMath (Document doc, View view, ThotBool state)
{
  if (state)
    TtaChangeButton (doc, view, MathButton, iconMath, state);
  else
    TtaChangeButton (doc, view, MathButton, iconMathNo, state);
}

/*----------------------------------------------------------------------
  CreateMath
  ----------------------------------------------------------------------*/
void                CreateMath (Document document, View view)
{
   CreateMathConstruct (1);
}

/*----------------------------------------------------------------------
  CreateMROOT
  ----------------------------------------------------------------------*/
void                CreateMROOT (Document document, View view)
{
   CreateMathConstruct (2);
}

/*----------------------------------------------------------------------
  CreateMSQRT
  ----------------------------------------------------------------------*/
void                CreateMSQRT (Document document, View view)
{
   CreateMathConstruct (3);
}

/*----------------------------------------------------------------------
  CreateMENCLOSE
  ----------------------------------------------------------------------*/
void                CreateMENCLOSE (Document document, View view)
{
   CreateMathConstruct (18);
}

/*----------------------------------------------------------------------
  CreateMFRAC
  ----------------------------------------------------------------------*/
void                CreateMFRAC (Document document, View view)
{
   CreateMathConstruct (4);
}

/*----------------------------------------------------------------------
  CreateMSUBSUP
  ----------------------------------------------------------------------*/
void                CreateMSUBSUP (Document document, View view)
{
   CreateMathConstruct (5);
}

/*----------------------------------------------------------------------
  CreateMSUB
  ----------------------------------------------------------------------*/
void                CreateMSUB (Document document, View view)
{
   CreateMathConstruct (6);
}

/*----------------------------------------------------------------------
  CreateMSUP
  ----------------------------------------------------------------------*/
void                CreateMSUP (Document document, View view)
{
   CreateMathConstruct (7);
}

/*----------------------------------------------------------------------
  CreateMUNDEROVER
  ----------------------------------------------------------------------*/
void                CreateMUNDEROVER (Document document, View view)
{
   CreateMathConstruct (8);
}

/*----------------------------------------------------------------------
  CreateMUNDER
  ----------------------------------------------------------------------*/
void                CreateMUNDER (Document document, View view)
{
   CreateMathConstruct (9);
}

/*----------------------------------------------------------------------
  CreateMOVER
  ----------------------------------------------------------------------*/
void                CreateMOVER (Document document, View view)
{
   CreateMathConstruct (10);
}

/*----------------------------------------------------------------------
  CreateMROW
  ----------------------------------------------------------------------*/
void                CreateMROW (Document document, View view)
{
   CreateMathConstruct (11);
}

/*----------------------------------------------------------------------
  CreateMMULTISCRIPTS
  ----------------------------------------------------------------------*/
void                CreateMMULTISCRIPTS (Document document, View view)
{
   CreateMathConstruct (12);
}

/*----------------------------------------------------------------------
  CreateMTABLE
  ----------------------------------------------------------------------*/
void                CreateMTABLE (Document document, View view)
{
   CreateMathConstruct (13);
}


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
  int           oldStructureChecking;

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
       TtaSetStructureChecking (0, doc);
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
       TtaSetStructureChecking ((ThotBool)oldStructureChecking, doc);
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
static void CreateCharStringElement (int typeNum, Document doc)
{
   ElementType    elType;
   AttributeType  attrType;
   Element        firstSel, lastSel, first, last, el, newEl, nextEl,
                  leaf, lastLeaf, nextLeaf, parent, selEl;
   int            firstChar, lastChar, i, j, oldStructureChecking;
   ThotBool       nonEmptySel, done, mrowCreated, same;

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
       CreateMathConstruct (i);
       return;
     }

   /* if not within a MathML element, nothing to do */
   elType = TtaGetElementType (firstSel);
   if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") != 0)
      return;

   TtaSetDisplayMode (doc, DeferredDisplay);
   TtaOpenUndoSequence (doc, firstSel, lastSel, firstChar, lastChar);
   oldStructureChecking = TtaGetStructureChecking (doc);
   TtaSetStructureChecking (0, doc);
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
	    TtaRegisterElementReplace (firstSel, doc);
	    ChangeTypeOfElement (firstSel, doc, typeNum);
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
 
   TtaSetStructureChecking ((ThotBool)oldStructureChecking, doc);
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
   CreateCharStringElement (MathML_EL_MTEXT, document);
}

/*----------------------------------------------------------------------
 CreateMI
 -----------------------------------------------------------------------*/
void CreateMI (Document document, View view)
{
   CreateCharStringElement (MathML_EL_MI, document);
}

/*----------------------------------------------------------------------
 CreateMN
 -----------------------------------------------------------------------*/
void CreateMN (Document document, View view)
{
   CreateCharStringElement (MathML_EL_MN, document);
}

/*----------------------------------------------------------------------
CreateMO
 -----------------------------------------------------------------------*/
void CreateMO (Document document, View view)
{
   CreateCharStringElement (MathML_EL_MO, document);
}

/*----------------------------------------------------------------------
CreateMSPACE
 -----------------------------------------------------------------------*/
void CreateMSPACE (Document document, View view)
{
   CreateMathConstruct (19);
}

/*----------------------------------------------------------------------
   A new element has been selected. Synchronize selection in source view.      
  ----------------------------------------------------------------------*/
void MathSelectionChanged (NotifyElement * event)
{
   SynchronizeSourceView (event);
}

/*----------------------------------------------------------------------
   GlobalMathAttrInMenu
   Called by Thot when building the Attributes menu.
   Prevent Thot from including a global attribute in the menu if the selected
   element does not accept this attribute.
  ----------------------------------------------------------------------*/
ThotBool  GlobalMathAttrInMenu (NotifyAttribute * event)
{
   ElementType         elType, parentType;
   Element             parent;
   char               *attr;

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
     return FALSE;

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
   return FALSE;
}

/*----------------------------------------------------------------------
 MathMoveForward
 Moves the caret to the next position in the MathML structure
 -----------------------------------------------------------------------*/
static ThotBool MathMoveForward ()
{
  Document      doc;
  Element       el, nextEl, leaf, ancestor, sibling;
  ElementType   elType;
  int           firstChar, lastChar, len;
  NotifyElement event;
  ThotBool      done, found;

  done = FALSE;
  doc = TtaGetSelectedDocument ();
  TtaGiveLastSelectedElement (doc, &el, &firstChar, &lastChar); 
  elType = TtaGetElementType (el);
  if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
    return (FALSE);
  if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
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
	  done = TRUE;
	}
    }
  if (!done)
    {
      /* get the following element in the tree structure */
      nextEl = TtaGetSuccessor (el);
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
	      TtaNextSibling (&sibling);
	      if (!sibling)
		{
		  el = TtaGetParent (el);
		  elType = TtaGetElementType (el);
		  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema),"MathML")&&
		      (elType.ElTypeNum == MathML_EL_Index ||
		       elType.ElTypeNum == MathML_EL_RootBase))
		    found = TRUE;
		}
	    }
	  while (!sibling && !found && el);
	  if (found)
	    {
	      if (elType.ElTypeNum == MathML_EL_Index)
		{
		  nextEl = el;
		  TtaPreviousSibling (&nextEl);
		}
	      else
		nextEl = TtaGetSuccessor (ancestor);
	    }
	}
      if (nextEl)
	{
	  elType = TtaGetElementType (nextEl);
	  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema),"MathML") &&
	      (elType.ElTypeNum == MathML_EL_MSPACE ||
	       elType.ElTypeNum == MathML_EL_MGLYPH ||
	       elType.ElTypeNum == MathML_EL_MALIGNMARK ||
	       elType.ElTypeNum == MathML_EL_MALIGNGROUP))
	    /* select the element itself, not its contents */
	    {
	      TtaSelectElement (doc, nextEl);
	      done = TRUE;
	    }
	  else
	    {
	      if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") &&
		  elType.ElTypeNum == MathML_EL_MTABLE)
		/* don't select within hidden element MTable_head. Skip it */
		{
		nextEl = TtaGetFirstChild (nextEl);
		if (nextEl)
		  nextEl = TtaGetSuccessor (nextEl);
		}
	      if (nextEl)
		{
		  elType = TtaGetElementType (nextEl);
		  /* if it's a mroot, move to the Index, not the RootBase */
		  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema),"MathML")&&
		      elType.ElTypeNum == MathML_EL_MROOT)
		    nextEl = TtaGetLastChild (nextEl);
		  /* get the first leaf in that element */
		  leaf = TtaGetFirstLeaf (nextEl);
		  if (leaf)
		    {
		      elType = TtaGetElementType (leaf);
		      if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
			/* put the caret before the first character in the
			   string */
			TtaSelectString (doc, leaf, 1, 0);
		      else
			/* select the whole leaf */
			TtaSelectElement (doc, leaf);
		      done = TRUE;
		    }
		}
	    }
	}
    }
  if (done)
    {
      event.document = doc;
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
  Document    doc;
  Element     el, prevEl, leaf, ancestor, sibling;
  ElementType elType;
  int         firstChar, lastChar, len;
  NotifyElement event;
  ThotBool    done, found;

  done = FALSE;  
  doc = TtaGetSelectedDocument ();
  TtaGiveFirstSelectedElement (doc, &el, &firstChar, &lastChar); 
  elType = TtaGetElementType (el);
  if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
    return (FALSE);
  if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
    /* the caret is in a text string */
    {
      if (firstChar > 1)
	/* the caret is not at the beginning of the string. Move it to
	   the previous character in the string */
	{
	  TtaSelectString (doc, el, firstChar - 1, firstChar - 2);
	  done = TRUE;
	}
    }
  if (!done)
    {
      /* get the previous element in the tree structure */
      prevEl = TtaGetPredecessor (el);
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
	      TtaPreviousSibling (&sibling);
	      if (!sibling)
		{
		  el = TtaGetParent (el);
		  elType = TtaGetElementType (el);
		  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema),"MathML")&&
		      (elType.ElTypeNum == MathML_EL_Index ||
		       elType.ElTypeNum == MathML_EL_RootBase))
		    found = TRUE;
		}
	    }
	  while (!sibling && !found && el);
	  if (found)
	    {
	      if (elType.ElTypeNum == MathML_EL_RootBase)
		{
		  prevEl = el;
		  TtaNextSibling (&prevEl);
		}
	      else
		prevEl = TtaGetPredecessor (ancestor);
	    }
	}
      if (prevEl)
	{
	  elType = TtaGetElementType (prevEl);
	  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema),"MathML") &&
	      (elType.ElTypeNum == MathML_EL_MSPACE ||
	       elType.ElTypeNum == MathML_EL_MGLYPH ||
	       elType.ElTypeNum == MathML_EL_MALIGNMARK ||
	       elType.ElTypeNum == MathML_EL_MALIGNGROUP))
	    /* select the element itself, not its contents */
	    {
	      TtaSelectElement (doc, prevEl);
	      done = TRUE;
	    }
	  else
	    {
	      if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") &&
		  elType.ElTypeNum == MathML_EL_MTable_head)
		/* don't select within hidden element MTable_head. Skip it */
		prevEl = TtaGetPredecessor (prevEl);
	      if (prevEl)
		{
		  elType = TtaGetElementType (prevEl);
		  /* if it's a mroot, move to the RootBase, not the Index */
		  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema),"MathML")&&
		      elType.ElTypeNum == MathML_EL_MROOT)
		    prevEl = TtaGetFirstChild (prevEl);
		  /* get the last leaf in that element */
		  leaf = TtaGetLastLeaf (prevEl);
		  if (leaf)
		    {
		      elType = TtaGetElementType (leaf);
		      if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
			/* put the caret at the end of the string */
			{
			  len = TtaGetElementVolume (leaf);
			  TtaSelectString (doc, leaf, len + 1, len);
			}
		      else
			/* select the whole leaf */
			TtaSelectElement (doc, leaf);
		      done = TRUE;
		    }
		}
	    }
	}
    }
  if (done)
    {
      event.document = doc;
      MathSelectionChanged (&event);
    }
  return (done);
}

/*----------------------------------------------------------------------
   InitMathML initializes MathML context.           
  ----------------------------------------------------------------------*/
void InitMathML ()
{
#ifndef _WINDOWS 
   iconMath = (ThotIcon) TtaCreatePixmapLogo (Math_xpm);
   iconMathNo = (ThotIcon) TtaCreatePixmapLogo (MathNo_xpm);
   mIcons[0] = TtaCreatePixmapLogo (Bmath_xpm);
   mIcons[1] = TtaCreatePixmapLogo (root_xpm);
   mIcons[2] = TtaCreatePixmapLogo (sqrt_xpm);
   mIcons[3] = TtaCreatePixmapLogo (frac_xpm);
   mIcons[4] = TtaCreatePixmapLogo (subsup_xpm);
   mIcons[5] = TtaCreatePixmapLogo (sub_xpm);
   mIcons[6] = TtaCreatePixmapLogo (sup_xpm);
   mIcons[7] = TtaCreatePixmapLogo (overunder_xpm);
   mIcons[8] = TtaCreatePixmapLogo (under_xpm);
   mIcons[9] = TtaCreatePixmapLogo (over_xpm);
   mIcons[10] = TtaCreatePixmapLogo (fence_xpm);
   mIcons[11] = TtaCreatePixmapLogo (mscript_xpm);
   mIcons[12] = TtaCreatePixmapLogo (matrix_xpm);
   mIcons[13] = TtaCreatePixmapLogo (greek_xpm);
#endif /* _WINDOWS */
  MathsDialogue = TtaSetCallback (CallbackMaths, MAX_MATHS);
  KeyboardsLoadResources ();
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
#ifdef _I18N_
  if (c >= '0' && c <= '9')
    /* decimal digit */
    ret = MathML_EL_MN;
  else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ' ')
    /* latin letter */
    ret = MathML_EL_MI;
  else if ((c >= 0x0391 && c <= 0x03A9) || (c >= 0x03B1 && c <= 0x03C9) ||
	   (c == 0x03D5))
    /* greek letter */
    ret = MathML_EL_MI;
  else if (c >= 0xC0 && c <= 0xFF &&  /* accented latin letter */
	   c != 0xD7 && c != 0xF7)    /* but not operators times or divide */
    ret = MathML_EL_MI;
  else
    ret = MathML_EL_MO;
#else  /* _I18N_ */
  if (script == 'L')
     /* ISO-Latin 1 */
     {
     if (c >= '0' && c <= '9')
        ret = MathML_EL_MN;
     else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ' ')
        ret = MathML_EL_MI;
     else if (((int) c) >= 192 && ((int) c) <= 255 &&
	      ((int) c) != 215 && ((int) c) != 247)
	ret = MathML_EL_MI;
     else
        ret = MathML_EL_MO;
     }
  else if (script == 'G')
     /* Symbol character set */
     {
     if (c >= '0' && c <= '9')
        ret = MathML_EL_MN;
     else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
        ret = MathML_EL_MI;
     else
        ret = MathML_EL_MO;
     }
#endif  /* _I18N_ */
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

   elem = NULL;
   leaf = NULL;
   prev = NULL;
   next = NULL;
   parent = el;
   do
      {
      prev = parent;  TtaPreviousSibling (&prev);
      if (prev != NULL)
	 {
         elType = TtaGetElementType (prev);
         if (elType.ElTypeNum == MathML_EL_FencedSeparator &&
	     strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
	    /* avoid selecting FencedSeparator elements */
	    TtaPreviousSibling (&prev);
	 }
      if (prev == NULL)
	 {
	 next = parent;  TtaNextSibling (&next);
	 if (next != NULL)
	    {
	    elType = TtaGetElementType (next);
	    if (elType.ElTypeNum == MathML_EL_FencedSeparator &&
		strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
	       /* avoid selecting FencedSeparator elements */
	       TtaNextSibling (&next);
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
		  if (len > 1
#ifndef _I18N_
		      /* function names can only be written in latin
			 characters */
		      && TtaGetScript (lang) == 'L'
#endif
		      )
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
			      if (!strncmp (functionName[func], &text[i],flen))
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
							 &text[firstChar],
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
				  elType.ElTypeNum = MathML_EL_MI;
				  newEl = TtaNewElement (doc, elType);
				  TtaInsertSibling (newEl, prevEl, FALSE, doc);
				  prevEl = newEl;
				  elType.ElTypeNum = MathML_EL_TEXT_UNIT;
				  newText = TtaNewElement (doc, elType);
				  TtaInsertFirstChild (&newText, newEl, doc);
				  TtaSetTextContent (newText,
						     functionName[func],
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
		      TtaSetTextContent (newText, &text[firstChar], lang, doc);
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
  Element	el, selEl, prevEl, nextEl, textEl, newEl, lastEl,
		firstEl, newSelEl, prev, next, parent, placeholderEl;
  ElementType	elType, elType2;
  AttributeType attrType;
  Attribute     attr;
  SSchema	MathMLSchema;
  int		firstSelChar, lastSelChar, newSelChar, len, totLen, i, j,
		start;
  char	        script;
  CHAR_T        c;
  Language	lang;
#define TXTBUFLEN 100
  CHAR_T        text[TXTBUFLEN];
  Language	language[TXTBUFLEN];
  char          mathType[TXTBUFLEN];
  int           oldStructureChecking;
  ThotBool      empty, closeUndoSeq, separate;

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

  i = 0;
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
	   newSelChar = totLen + firstSelChar;
	   newSelEl = textEl;
	   TtaUnselect (doc);
	 }
       /* get the content and analyze it */
       if (len > 0)
          {
          len = TXTBUFLEN - totLen;
          TtaGiveBufferContent (textEl, &text[i], len, &lang);
	  len = ustrlen (&text[i]);
          script = TtaGetScript (lang);
	  for (j = 0; j < len; j++)
	     {
	     language[i+j] = lang;
	     mathType[i+j] = (char)GetCharType (text[i+j], script);
	     }
	  i+= len;
	  totLen += len;
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

  if (TtaPrepareUndo (doc))
    closeUndoSeq = FALSE;
  else
    {
      TtaOpenUndoSequence (doc, selEl, selEl, firstSelChar, lastSelChar);
      closeUndoSeq = TRUE;
    }
  TtaSetDisplayMode (doc, DeferredDisplay);
  oldStructureChecking = TtaGetStructureChecking (doc);
  TtaSetStructureChecking (0, doc);
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

	  prev = theElem;
	  TtaPreviousSibling (&prev);
	  if (prev == NULL)
	     {
	     next = theElem;
	     TtaNextSibling (&next);
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
    for (i = 1; i <= totLen; i++)
      {
      separate = FALSE;
      if (mathType[i] != mathType[i-1] ||
	  language[i] != language[i-1])
	/* different mathematical type or different script.
	   Create a separate element */
	separate = TRUE;
      else if (i == totLen)
	/* end of string. Create an element anyway */
	separate = TRUE;
      else if (mathType[i-1] == (char)MathML_EL_MO)
	/* an operator */
	{
	/* by default create a separate element */
	separate = TRUE;
        /* if successive integral characters, keep them in the same element */
#ifdef _I18N_
	if (text[i] == 0x222B && text[i] == text [i-1])
	  separate = FALSE;
#else
	if ((int)text[i] == 242 && text[i] == text [i-1])
	  {
	  if (TtaGetScript (language[i]) == 'G' &&
	      language[i] == language[i-1])
	    separate = FALSE;
	  }
#endif
	}
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
	     TtaRegisterElementReplace (theElem, doc);
	     ChangeTypeOfElement (theElem, doc, (int)mathType[i-1]);
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
	j = i - 1;
	while (text[j] == ' ' && j > start)
	  j--;
	j++;
	c = text[j];
	text[j] = '\0';
	TtaSetBufferContent (textEl, &text[start], language[start], doc);
	text[j] = c;
	lastEl = newEl;
	if (newSelEl != NULL)
	  {
	  newSelEl = textEl;
	  if (newSelChar < j)
	    {
	     if (newSelChar < start)
		newSelChar = 1;
	     else
		newSelChar -= start;
	    }
	  }
	MathSetAttributes (newEl, doc, &newSelEl);
	start = i;

	if (mathType[i-1] == (char)MathML_EL_MO)
	  /* the new element is an operator */
	  {
	  /* the new element may be a vertically stretchable symbol */
	  CheckFence (newEl, doc);
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
		 elType.ElTypeNum == MathML_EL_Overscript) &&
		elType.ElSSchema == MathMLSchema)
	       SetSingleIntHorizStretchAttr (parent, doc, &newSelEl);
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

  TtaSetStructureChecking ((ThotBool)oldStructureChecking, doc);
  TtaSetDisplayMode (doc, DisplayImmediately);
  if (closeUndoSeq)
    TtaCloseUndoSequence (doc);

  /* set a new selection */
  if (newSelEl != NULL)
     {
     elType = TtaGetElementType (newSelEl);
     if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
        TtaSelectString (doc, newSelEl, newSelChar, newSelChar-1);
     else
	TtaSelectElement (doc, newSelEl);
     }
}

/*----------------------------------------------------------------------
   SetAttrParseMe
   associate a IntParseMe attribute with element el in document doc
 -----------------------------------------------------------------------*/
static void SetAttrParseMe (Element el, Document doc)
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
  ThotBool       found;
  int            value;
  Language       lang;
#ifdef _I18N_   
  unsigned char *ptr;
  int            i;
#endif /* _I18N_ */

  found = MapXMLEntity (MATH_TYPE, entityName, &value);
  if (!found)
    {
      /* Unknown entity */
      bufEntity[0] = '?';
      bufEntity[1] = EOS;
      lang = TtaGetLanguageIdFromScript('L');
    }
#ifdef _I18N_
  else if (value < 1023)
    {
      /* get the UTF-8 string of the unicode character */
      ptr = bufEntity;
      i = TtaWCToMBstring ((wchar_t) value, &ptr);
      bufEntity[i] = EOS;
    }
#endif /* _I18N_ */
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
	GetFallbackCharacter (value, bufEntity, &lang);
    }
  TtaSetTextContent (el, bufEntity, lang, doc);
}

/*----------------------------------------------------------------------
   InsertMathEntity
   Insert an entity at the currently selected position.
   entityName is the name of the entity to be created.
 -----------------------------------------------------------------------*/
static void InsertMathEntity (unsigned char *entityName, Document document)
{
  Element       firstSel, lastSel, el, el1, parent, sibling;
  ElementType   elType, elType1;
  Attribute     attr;
  AttributeType attrType;
  int           firstChar, lastChar, i, len;
  ThotBool      before, done;
  char	        *ptr;
  char          buffer[MAX_LENGTH+2];

  if (!TtaIsSelectionEmpty ())
    return;
  TtaGiveFirstSelectedElement (document, &firstSel, &firstChar, &i);
  /* if not within a MathML element, nothing to do */
  elType = TtaGetElementType (firstSel);
  if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") != 0)
    return;
  TtaGiveLastSelectedElement (document, &lastSel, &i, &lastChar);
  TtaOpenUndoSequence (document, firstSel, lastSel, firstChar, lastChar);
  TtaUnselect (document);
  done = FALSE;
  /* the new text element will be inserted before the first element
     selected */
  before = TRUE;
  sibling = firstSel;
  ptr = NULL;
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
  else if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
    /* current selection is in a text leaf */
    {
      parent = TtaGetParent (firstSel);
      elType1 = TtaGetElementType (parent);
      if (elType1.ElTypeNum == MathML_EL_MGLYPH &&
	  strcmp (TtaGetSSchemaName (elType1.ElSSchema), "MathML") == 0)
	/* the first selected element is within a mglyph. The new text
	   leaf will be inserted as a sibling of this mglyph */
	sibling = parent;
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
	    }
	}
    }
  if (!done)
    {
      TtaInsertSibling (el, sibling, before, document);
      TtaRegisterElementCreate (el, document);
    }
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = MathML_ATTR_EntityName;
  attr =  TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, document);
  strcpy (buffer, "&");
  strcat (buffer, entityName);
  strcat (buffer, ";");
  TtaSetAttributeText (attr, buffer, el, document);
  SetContentAfterEntity (entityName, el, document);
  len = TtaGetElementVolume (el);
  TtaSelectString (document, el, len + 1, len);
  ParseMathString (el, TtaGetParent (el), document);
  TtaSetDocumentModified (document);
  TtaCloseUndoSequence (document);
}

/*----------------------------------------------------------------------
 CreateMathEntity
 Display a dialogue box to allow input of a character entity name
 and create the corresponding entity at the current selection position
 -----------------------------------------------------------------------*/
void CreateMathEntity (Document document, View view)
{
   Element       firstSel;
   ElementType   elType;
   int           firstChar, i;

   if (!TtaGetDocumentAccessMode (document))
      /* the document is in ReadOnly mode */
      return;

   if (!TtaIsSelectionEmpty ())
      return;
   TtaGiveFirstSelectedElement (document, &firstSel, &firstChar, &i);

   /* if not within a MathML element, nothing to do */
   elType = TtaGetElementType (firstSel);
   if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") != 0)
      return;
   
   MathMLEntityName[0] = EOS;
#ifdef _WINDOWS
   CreateMCHARDlgWindow (TtaGetViewFrame (document, view), MathMLEntityName);
#else
   TtaNewForm (BaseDialog + MathEntityForm, TtaGetViewFrame (document, view), 
	       TtaGetMessage (1, BMEntity), TRUE, 1, 'L', D_CANCEL);
   TtaNewTextForm (BaseDialog + MathEntityText, BaseDialog + MathEntityForm,
		   TtaGetMessage (AMAYA, AM_MATH_ENTITY_NAME), NAME_LENGTH, 1,
		   FALSE);
   TtaSetTextForm (BaseDialog + MathEntityText, MathMLEntityName);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + MathEntityForm, FALSE);
   TtaWaitShowDialogue ();
#endif /* _WINDOWS */
   if (MathMLEntityName[0] != EOS)
      InsertMathEntity (MathMLEntityName, document);
}

/*----------------------------------------------------------------------
 CreateInvisibleTimes
 Insert a character entity InvisibleTimes at the current position
 -----------------------------------------------------------------------*/
void CreateInvisibleTimes (Document document, View view)
{
   Element       firstSel;
   ElementType   elType;
   int           firstChar, i;

   if (!TtaGetDocumentAccessMode (document))
      /* the document is in ReadOnly mode */
      return;

   if (!TtaIsSelectionEmpty ())
      return;
   TtaGiveFirstSelectedElement (document, &firstSel, &firstChar, &i);

   /* if not within a MathML element, nothing to do */
   elType = TtaGetElementType (firstSel);
   if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") != 0)
      return;
   InsertMathEntity ("InvisibleTimes", document);
}

/*----------------------------------------------------------------------
 CreateApplyFunction
 Insert a character entity CreateAlppyFunction at the current position
 -----------------------------------------------------------------------*/
void CreateApplyFunction (Document document, View view)
{
   Element       firstSel;
   ElementType   elType;
   int           firstChar, i;

   if (!TtaGetDocumentAccessMode (document))
      /* the document is in ReadOnly mode */
      return;

   if (!TtaIsSelectionEmpty ())
      return;
   TtaGiveFirstSelectedElement (document, &firstSel, &firstChar, &i);

   /* if not within a MathML element, nothing to do */
   elType = TtaGetElementType (firstSel);
   if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") != 0)
      return;
   InsertMathEntity ("ApplyFunction", document);
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
  int                 firstChar, lastChar, i, j, val, oldStructureChecking;
  ThotBool            emptySel, same, mrowCreated;

  if (!TtaGetDocumentAccessMode (doc))
     return;
  TtaGiveFirstSelectedElement (doc, &firstSel, &firstChar, &i);
  if (firstSel == NULL)
     /* no selection available */
     return;
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
  TtaSetStructureChecking (0, doc);
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

  TtaSetStructureChecking ((ThotBool)oldStructureChecking, doc);
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
  /* if the event comes from function BreakElement, don't do anything:
     the user just want to split that character string */
  if (event->targetdocument != 0)
    ParseMathString (event->target, event->element, event->document);
}

/*----------------------------------------------------------------------
 NewMathString
 An new text string has been created in a MathML element.
 Parse its contents.
 -----------------------------------------------------------------------*/
void NewMathString (NotifyElement *event)
{
   RemoveAttr (event->element, event->document, MathML_ATTR_EntityName);
   if (TtaGetElementVolume (event->element) > 0)
      ParseMathString (event->element, TtaGetParent (event->element),
		       event->document);
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
   Element	 placeholderEl, parent, prev;
   ElementType	 elType, elTypeParent;
   Attribute     attr;
   AttributeType attrType;
   int           oldStructureChecking;

   /* if the pasted element is an XLink, update the link */
   XLinkPasted (event);

   elType = TtaGetElementType (event->element);
   oldStructureChecking = TtaGetStructureChecking (event->document);
   TtaSetStructureChecking (0, event->document);

   /* if an enclosing MROW element is needed create it, except if it's a
      call from Undo command */
   if (event->info != 1)
     CreateParentMROW (event->element, event->document);

   /* if the pasted element is a child of a FencedExpression element,
      create the associated FencedSeparator elements */
   parent = TtaGetParent (event->element);
   elTypeParent = TtaGetElementType (parent);
   if (elTypeParent.ElTypeNum == MathML_EL_FencedExpression &&
       strcmp (TtaGetSSchemaName (elTypeParent.ElSSchema), "MathML") == 0)
     RegenerateFencedSeparators (parent, event->document, FALSE/******/);

   /* if the pasted element is a character string within a MI, MN, or MO
      element, parse the new content to isolate identifiers, numbers and
      operators */
   if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
     {
     if ((elTypeParent.ElTypeNum == MathML_EL_MI ||
	  elTypeParent.ElTypeNum == MathML_EL_MO ||
	  elTypeParent.ElTypeNum == MathML_EL_MN) &&
	 strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
       /* if it's a call from Undo command, don't do anything */
       if (event->info != 1)
	 ParseMathString (event->element, parent, event->document);
     }
   else
     {
     /* create placeholders before and/or after the new element */
     placeholderEl = InsertPlaceholder (event->element, TRUE, event->document,
					FALSE/****/);
     placeholderEl = InsertPlaceholder (event->element, FALSE, event->document,
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
	     TtaAttachAttribute (prev, attr, event->document);
	     TtaSetAttributeValue (attr, MathML_ATTR_IntPlaceholder_VAL_yes_,
				   prev, event->document);
	     TtaRegisterElementReplace (prev, event->document);
	     ChangeTypeOfElement (prev, event->document, MathML_EL_Construct);
	   }
       }
     }

   TtaSetStructureChecking ((ThotBool)oldStructureChecking, event->document);
}


/*----------------------------------------------------------------------
 MathElementWillBeDeleted
 This function is called by the DELETE command for each selected element
 and for all their descendants.
 -----------------------------------------------------------------------*/
ThotBool MathElementWillBeDeleted (NotifyElement *event)
{
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
  return FALSE; /* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
  DeleteMColumn
  ----------------------------------------------------------------------*/
void DeleteMColumn (Document document, View view)
{
   Element             el, cell, colHead, selCell, leaf;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;
   Document            refDoc;
   char                name[50];
   int                 firstchar, lastchar, len;
   ThotBool            selBefore;

   if (!TtaGetDocumentAccessMode (document))
      /* the document is in ReadOnly mode */
      return;

   /* get the first selected element */
   TtaGiveFirstSelectedElement (document, &el, &firstchar, &lastchar);
   if (el != NULL)
     {
       elType = TtaGetElementType (el);
       if (elType.ElSSchema == GetMathMLSSchema (document))
	 {
	   if (elType.ElTypeNum == MathML_EL_MTD &&
	       strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
	     cell = el;
	   else
	     {
	       elType.ElTypeNum = MathML_EL_MTD;
	       cell = TtaGetTypedAncestor (el, elType);
	     }
	   if (cell != NULL)
	     {
	       /* prepare the new selection */
	       selCell = cell;
	       TtaNextSibling (&selCell);
	       if (selCell)
		  selBefore = FALSE;
	       else
		  {
		  selCell = cell;
		  TtaPreviousSibling (&selCell);
		  selBefore = TRUE;
		  }
	       /* get current column */
	       attrType.AttrSSchema = elType.ElSSchema;
	       attrType.AttrTypeNum = MathML_ATTR_MRef_column;
	       attr = TtaGetAttribute (cell, attrType);
	       if (attr != NULL)
		 {
		   TtaGiveReferenceAttributeValue (attr, &colHead, name,
						   &refDoc);
		   TtaOpenUndoSequence (document, el, el, firstchar,
					lastchar);
		   /* remove column */
		   RemoveColumn (colHead, document, FALSE, TRUE);
		   
		   /* set new selection */
		   if (selBefore)
		      leaf = TtaGetLastLeaf (selCell);
		   else
		      leaf = TtaGetFirstLeaf (selCell);
		   elType = TtaGetElementType (leaf);
		   if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
		     if (selBefore)
		        {
			len = TtaGetElementVolume (leaf);
		        TtaSelectString (document, leaf, len + 1, len);
			}
		     else
		        TtaSelectString (document, leaf, 1, 0);
		   else
		     TtaSelectElement (document, leaf);

		   TtaCloseUndoSequence (document);
		   TtaSetDocumentModified (document);
		 }
	     }
	 }
     }
}

/*----------------------------------------------------------------------
 MathElementDeleted
 An element has been deleted in a MathML structure.
 Create the necessary placeholders.
 Remove the enclosing MROW element if it has only one child.
 -----------------------------------------------------------------------*/
void MathElementDeleted (NotifyElement *event)
{
   Element	 sibling, placeholderEl, parent, child, grandChild, next, prev;
   ElementType	 parentType, elType;
   AttributeType attrType;
   Attribute     attr;
   int		 i, newTypeNum;
   int           oldStructureChecking;

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
				 TtaRegisterElementReplace (sibling,
							    event->document);
				 ChangeTypeOfElement (sibling, event->document,
						      MathML_EL_Construct1);
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

      default:
	break;
      }

   oldStructureChecking = TtaGetStructureChecking (event->document);
   TtaSetStructureChecking (0, event->document);
   if (newTypeNum > 0)
     /* transform the parent element */
     {
       TtaRegisterElementReplace (parent, event->document);
       ChangeTypeOfElement (parent, event->document, newTypeNum);
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
	   TtaRemoveTree (grandChild, event->document);
	   TtaInsertSibling (grandChild, parent, TRUE, event->document);
	   TtaDeleteTree (parent, event->document);
	   placeholderEl = InsertPlaceholder (grandChild, FALSE,
					      event->document, FALSE/******/);
	   placeholderEl = InsertPlaceholder (grandChild, TRUE,
					      event->document, FALSE/******/);
	   }
	}
      }
   TtaSetStructureChecking ((ThotBool)oldStructureChecking, event->document);
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
  TtaSetAttributeText (attr, text, mfencedEl, event->document);
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
 An attribute EntityName has been modified by the user.
 -----------------------------------------------------------------------*/
void MathEntityModified (NotifyAttribute *event)
{
#define buflen 200
  char          *value;
  int            length, i;
  ThotBool       changed;

  value = TtaGetMemory (buflen);
  value[0] = EOS;
  changed = FALSE;
  length = TtaGetTextAttributeLength (event->attribute);
  if (length >= buflen)
    length = buflen - 2;
  if (length <= 0)
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
  SetContentAfterEntity (&value[1], event->element, event->document);
  value[length - 1] = ';';
  value[length] = EOS;
  if (changed)
    TtaSetAttributeText (event->attribute, value, event->element,
			 event->document);
}

/*----------------------------------------------------------------------
 MathDisplayAttrCreated
 An attribute display has been created by the user.
 -----------------------------------------------------------------------*/
void MathDisplayAttrCreated (NotifyAttribute *event)
{
  MathMLSetDisplayAttr (event->element, event->attribute, event->document,
			FALSE);
}

/*----------------------------------------------------------------------
 MathDisplayAttrDelete
 The user is deleting an attribute display
 -----------------------------------------------------------------------*/
ThotBool MathDisplayAttrDelete (NotifyAttribute *event)
{
  ParseHTMLSpecificStyle (event->element, "display:inline", event->document,
			  0, TRUE);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
 MathPresentAttrCreated
 An attribute fontsize, mathsize, lspace, rspace, linethickness has been
 created or updated by the user.
 -----------------------------------------------------------------------*/
void MathPresentAttrCreated (NotifyAttribute *event)
{
#define buflen 200
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
      value = TtaGetMemory (buflen);
      value[0] = EOS;
      length = TtaGetTextAttributeLength (event->attribute);
      if (length >= buflen)
	length = buflen - 1;
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
    ParseHTMLSpecificStyle (event->element, "font-size: 10pt",
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
  ParseHTMLSpecificStyle (event->element, "padding-left: 10pt",
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
  ParseHTMLSpecificStyle (event->element, "padding-right: 10pt",
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

  value = TtaGetMemory (buflen);
  value[0] = EOS;
  length = TtaGetTextAttributeLength (event->attribute);
  if (length >= buflen)
     length = buflen - 1;
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
  ParseHTMLSpecificStyle (event->element, "font-family: serif",
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
      value = TtaGetMemory (buflen);
      value[0] = EOS;
      length = TtaGetTextAttributeLength (event->attribute);
      if (length >= buflen)
	length = buflen - 1;
      if (length > 0)
	TtaGiveTextAttributeValue (event->attribute, value, &length);  
      HTMLSetForegroundColor (event->document, event->element, value);
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
      value = TtaGetMemory (buflen);
      value[0] = EOS;
      length = TtaGetTextAttributeLength (event->attribute);
      if (length >= buflen)
	length = buflen - 1;
      if (length > 0)
	TtaGiveTextAttributeValue (event->attribute, value, &length);  
      HTMLSetBackgroundColor (event->document, event->element, value);
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
     value = TtaGetMemory (length+1);
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
 The user is deleting an attribute scriptlevel.
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
  ParseHTMLSpecificStyle (event->element, "stroke-width: 1pt",
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
	   ChangeTypeOfElement (event->element, event->document,
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
	      ChangeTypeOfElement (event->element, event->document,
				   MathML_EL_MFRAC);
	  }
	else if (val == MathML_ATTR_bevelled_VAL_true)
	  {
	    /* element type should be BevelledMFRAC */
	    if (elType.ElTypeNum != MathML_EL_BevelledMFRAC)
	      ChangeTypeOfElement (event->element, event->document,
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
     value = TtaGetMemory (length+1);
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
  char    text[8];

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
void FencedSeparatorModified (NotifyOnTarget *event)
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
  i = 0;
  child = TtaGetFirstChild (fencedExpEl);
  while (child != NULL)
     {
     elType = TtaGetElementType (child);
     if (elType.ElTypeNum == MathML_EL_FencedSeparator &&
	 strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
	{
	content = TtaGetFirstChild (child);
        len = 31 - i;
        TtaGiveTextContent (content, &text[i], &len, &lang);
	i++;
	}
     TtaNextSibling (&child);
     }
  text[i] = '\0';
  /* if the last character is repeated, delete the repeated characters */
  if (i > 1)
     {
     i--;
     while (text[i-1] == text[i] && i > 0)
	i--;
     text[i+1] = '\0';
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
  TtaSetAttributeText (attr, text, mfencedEl, event->document);
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
     value = TtaGetMemory (length+1);
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
 DeleteIntRowAlign
 Remove attribute IntRowAlign from element row if there is no rowalign_mtr
 attribut on this element.
 -----------------------------------------------------------------------*/
static void DeleteIntRowAlign (Element row, Document doc)
{
  ElementType   elType;
  AttributeType attrType;
  Attribute     attr;

  elType = TtaGetElementType (row);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = MathML_ATTR_rowalign_mtr;
  attr = TtaGetAttribute (row, attrType);
  if (!attr)
    {
      attrType.AttrTypeNum = MathML_ATTR_IntRowAlign;
      attr = TtaGetAttribute (row, attrType);
      if (attr)
	TtaRemoveAttribute (row, attr, doc);
    }
}

/*----------------------------------------------------------------------
 SetIntRowAlign
 Set attribute IntRowAlign for element row unless this element already has
 a rowalign_mtr attribute
 -----------------------------------------------------------------------*/
static void SetIntRowAlign (Element row, int val, Document doc)
{
  ElementType   elType;
  AttributeType attrType;
  Attribute     attr;

  elType = TtaGetElementType (row);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = MathML_ATTR_rowalign_mtr;
  attr = TtaGetAttribute (row, attrType);
  if (!attr)
    {
      attrType.AttrTypeNum = MathML_ATTR_IntRowAlign;
      attr = TtaGetAttribute (row, attrType);
      if (!attr)
	{
	  attr = TtaNewAttribute (attrType);
	  TtaAttachAttribute (row, attr, doc);
	}
      TtaSetAttributeValue (attr, val, row, doc);
    }
}

/*----------------------------------------------------------------------
   HandleRowalignAttribute
   An attribute rowalign has been created, updated (if !delete) or deleted
   (if delete) for element el in document doc. Update the IntRowAlign
   attributes of all enclosed mrow elements accordingly.
  ----------------------------------------------------------------------*/
void HandleRowalignAttribute (Attribute attr, Element el, Document doc,
			      ThotBool delete)
{
  char            *value;
  char            *ptr;
  int              length, val;
  ElementType      elType;
  Element          row;

  elType = TtaGetElementType (el);
  if (elType.ElTypeNum != MathML_EL_MTABLE ||
      strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
    /* ignore rowalign attribute on mstyle elements */
    /* process it only on mtable elements */
    return;

  value = NULL;
  if (!delete)
    {
      length = TtaGetTextAttributeLength (attr);
      if (length > 0)
	{
	  value = TtaGetMemory (length+1);
	  value[0] = EOS;
	  TtaGiveTextAttributeValue (attr, value, &length);
	}
    }
  /* if attribute rowalign is created or updated but has no value, don't
     do anything */
  if (!delete && !value)
    return;

  ptr = value;
  val = 0;
  elType.ElTypeNum = MathML_EL_TableRow;
  row = TtaSearchTypedElement (elType, SearchInTree, el);
  while (row)
    {
    elType = TtaGetElementType (row);
    /* skip comments and other non row elements */
    if ((elType.ElTypeNum == MathML_EL_MTR ||
	 elType.ElTypeNum == MathML_EL_MLABELEDTR) &&
	strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
      {
      if (delete)
	DeleteIntRowAlign (row, doc);
      else
	{
	  if (*ptr != EOS)
	    {
	      /* get next word in the attribute value */
	      ptr = TtaSkipBlanks (ptr);
	      /* process that word */
	      if (*ptr != EOS && *ptr != ' ')
		{
		  if (!strncasecmp (ptr, "top", 3))
		    val = MathML_ATTR_IntRowAlign_VAL_IntTop;
		  else if (!strncasecmp (ptr, "bottom", 6))
		    val = MathML_ATTR_IntRowAlign_VAL_IntBottom;
		  else if (!strncasecmp (ptr, "center", 6))
		    val = MathML_ATTR_IntRowAlign_VAL_IntCenter;
		  else if (!strncasecmp (ptr, "baseline", 8))
		    val = MathML_ATTR_IntRowAlign_VAL_IntBaseline;
		  else if (!strncasecmp (ptr, "axis", 4))
		    val = MathML_ATTR_IntRowAlign_VAL_IntAxis;
		  else
		    val = 0;
		  /* skip the word that has been processed */
		  while (*ptr != EOS && *ptr != ' ')
		    ptr++;
		}
	    }
	  if (val > 0)
	    SetIntRowAlign (row, val, doc);
	}
      }
    TtaNextSibling (&row);
    }
  if (value)
    TtaFreeMemory (value);
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
 DeleteIntColAlign
 Remove attribute IntColAlign from element cell if there is no columnalign_mtd
 attribut on this element.
 -----------------------------------------------------------------------*/
static void DeleteIntColAlign (Element cell, Document doc)
{
  ElementType   elType;
  AttributeType attrType;
  Attribute     attr;

  elType = TtaGetElementType (cell);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = MathML_ATTR_columnalign_mtd;
  attr = TtaGetAttribute (cell, attrType);
  if (!attr)
    {
      attrType.AttrTypeNum = MathML_ATTR_IntColAlign;
      attr = TtaGetAttribute (cell, attrType);
      if (attr)
	TtaRemoveAttribute (cell, attr, doc);
    }
}

/*----------------------------------------------------------------------
 SetIntColAlign
 Set attribute IntColAlign for element cell unless this element already has
 a columnalign_mtd attribute
 -----------------------------------------------------------------------*/
static void SetIntColAlign (Element cell, int val, Document doc)
{
  ElementType   elType;
  AttributeType attrType;
  Attribute     attr;

  elType = TtaGetElementType (cell);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = MathML_ATTR_columnalign_mtd;
  attr = TtaGetAttribute (cell, attrType);
  if (!attr)
    {
      attrType.AttrTypeNum = MathML_ATTR_IntColAlign;
      attr = TtaGetAttribute (cell, attrType);
      if (!attr)
	{
	  attr = TtaNewAttribute (attrType);
	  TtaAttachAttribute (cell, attr, doc);
	}
      TtaSetAttributeValue (attr, val, cell, doc);
    }
}

/*----------------------------------------------------------------------
 RowWithoutColalignAttr
 if skip: if element row has a columnalign attribute, get the next sibling row
 element without a columnalign attribute and return its first cell
 if not skip: always return the first cell in the row, and the columnalign
 attribute of that row if there is one.
 -----------------------------------------------------------------------*/
static void RowWithoutColalignAttr (Element *row, Element *cell,
				    Attribute *attr, ThotBool skip)
{
  ElementType      elType;
  AttributeType    attrType;

  elType = TtaGetElementType (*row);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = MathML_ATTR_columnalign;
  *cell = NULL;
  *attr = NULL;
  while (*row != NULL && *cell == NULL)
    {
      elType = TtaGetElementType (*row);
      if ((elType.ElTypeNum != MathML_EL_MTR &&
          elType.ElTypeNum != MathML_EL_MLABELEDTR) ||
	  strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
	/* not a row. Skip it */
	TtaNextSibling (row);
      else
	{
	  /* skip that row if it has a columnalign attribute */
	  *attr = TtaGetAttribute (*row, attrType);
	  if (skip && *attr != NULL)
	    {
	    TtaNextSibling (row);
	    *attr = NULL;
	    }
	  else
	    /* it's a row without a columnalign attribute */
	    *cell = TtaGetFirstChild (*row);
	}
    }
}

/*----------------------------------------------------------------------
   HandleColalignAttribute
   An attribute columnalign has been created, updated (if !delete) or deleted
   (if delete) for element el in document doc. Update the IntColAlign
   attributes of all concerned cells accordingly.
   If allRows is TRUE, process also rows that have their own columnalign
   attribute, according to that attribute, otherwise skip those rows.
  ----------------------------------------------------------------------*/
void HandleColalignAttribute (Attribute attr, Element el, Document doc,
			      ThotBool delete, ThotBool allRows)
{
  char            *value, *localValue;
  char            *ptr;
  int              length, val;
  ElementType      elType;
  Element          cell, row;
  Attribute        localAttr;
  ThotBool         fullTable;

  elType = TtaGetElementType (el);
  if ((elType.ElTypeNum != MathML_EL_MTABLE &&
       elType.ElTypeNum != MathML_EL_MTR &&
       elType.ElTypeNum != MathML_EL_MLABELEDTR) ||
      strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
    /* ignore columnalign attribute on mstyle elements */
    /* process it only on mtable elements */
    return;

  fullTable = (elType.ElTypeNum == MathML_EL_MTABLE);
  value = NULL;
  localValue = NULL;
  if (!delete)
    {
      length = TtaGetTextAttributeLength (attr);
      if (length > 0)
	{
	  value = TtaGetMemory (length+1);
	  value[0] = EOS;
	  TtaGiveTextAttributeValue (attr, value, &length);
	}
    }
  /* if attribute columnalign is created or updated but has no value, don't
     do anything */
  if (!delete && !value)
    return;

  ptr = value;
  val = 0;
  /* get the first cell within the element */
  elType.ElTypeNum = MathML_EL_MTD;
  cell = TtaSearchTypedElement (elType, SearchInTree, el);
  if (cell && fullTable)
    {
    elType.ElTypeNum = MathML_EL_TableRow;
    row = TtaGetTypedAncestor (cell, elType);
    RowWithoutColalignAttr (&row, &cell, &localAttr, !allRows);
    if (localAttr)
      {
	length = TtaGetTextAttributeLength (localAttr);
	if (length > 0)
	  {
	    if (localValue)
	      TtaFreeMemory (localValue);
	    localValue = TtaGetMemory (length+1);
	    localValue[0] = EOS;
	    TtaGiveTextAttributeValue (localAttr, localValue, &length);
	    ptr = localValue;
	  }
      }
    }
  while (cell)
    {
    elType = TtaGetElementType (cell);
    /* skip comments and other non cell elements */
    if (elType.ElTypeNum == MathML_EL_MTD &&
	strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
      {
        if (delete)
	  DeleteIntColAlign (cell, doc);
        else
	  {
	  if (*ptr != EOS)
	    {
	      /* get next word in the attribute value */
	      ptr = TtaSkipBlanks (ptr);
	      /* process that word */
	      if (*ptr != EOS && *ptr != ' ')
		{
		  if (!strncasecmp (ptr, "left", 4))
		    val = MathML_ATTR_IntColAlign_VAL_IntLeft;
		  else if (!strncasecmp (ptr, "center", 6))
		    val = MathML_ATTR_IntColAlign_VAL_IntCenter;
		  else if (!strncasecmp (ptr, "right", 5))
		    val = MathML_ATTR_IntColAlign_VAL_IntRight;
		  else
		    val = 0;
		  /* skip the word that has been processed */
		  while (*ptr != EOS && *ptr != ' ')
		    ptr++;
		}
	    }
	  if (val > 0)
	    SetIntColAlign (cell, val, doc);
	  }
      }
    TtaNextSibling (&cell);
    if (!cell && fullTable && row)
      /* no more sibling cell. If the columnalign attribute is for the
         full table, get the first cell in the next row */
      {
      TtaNextSibling (&row);
      if (row)
	{
	  /* parse value of columnalign attribute again from the beginning */
	  ptr = value;
	  RowWithoutColalignAttr (&row, &cell, &localAttr, !allRows);
	  if (localAttr)
	    {
	      length = TtaGetTextAttributeLength (localAttr);
	      if (length > 0)
		{
		  if (localValue)
		    TtaFreeMemory (localValue);
		  localValue = TtaGetMemory (length+1);
		  localValue[0] = EOS;
		  TtaGiveTextAttributeValue (localAttr, localValue, &length);
		  ptr = localValue;
		}
	    }
	}
      }
    }
  if (value)
    TtaFreeMemory (value);
  if (localValue)
    TtaFreeMemory (localValue);
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
   HandleColAndRowAlignAttributes
   if element row is a MathML mtd element, check the rowalign and columnalign
   attributes on the enclosing table element and on the row itself.
  ----------------------------------------------------------------------*/
void HandleColAndRowAlignAttributes (Element row, Document doc)
{
   Element          table;
   ElementType      elType;
   AttributeType    attrType;
   Attribute        attr;

   elType = TtaGetElementType (row);
   if ((elType.ElTypeNum == MathML_EL_MTR ||
	elType.ElTypeNum == MathML_EL_MLABELEDTR) &&
       !strcmp (TtaGetSSchemaName(elType.ElSSchema), "MathML"))
     /* it's a row in a MathML table */
     {
       attrType.AttrSSchema = elType.ElSSchema;
       attrType.AttrTypeNum = MathML_ATTR_columnalign;
       attr = TtaGetAttribute (row, attrType);
       if (attr)
	 /* the row element has a columnalign attribute.
	    applies that attribute again to that row */
	 HandleColalignAttribute (attr, row, doc, FALSE, TRUE);
       elType.ElTypeNum = MathML_EL_MTABLE;
       table = TtaGetTypedAncestor (row, elType);
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
     }
}
