/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module contains editing functions for handling MathML objects.
 *
 * Author: I. Vatton
 *         R. Guetari (W3C/INRIA) - Windows routines.
 */
 
/* Included headerfiles */
#define THOT_EXPORT
#include "amaya.h"
#include "css.h"
#include "trans.h"
#include "undo.h"
#include "interface.h"
#include "MathML.h"
#ifdef GRAPHML
#include "GraphML.h"
#endif

#define FormMaths 0
#define MenuMaths 1
#define MAX_MATHS  2

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
static int	MathsDialogue;
static ThotBool	InitMaths;
static ThotBool	IsLastDeletedElement = FALSE;
static Element	LastDeletedElement = NULL;

#include "html2thot_f.h"
#include "fetchXMLname_f.h"
#include "HTMLtable_f.h"
#include "HTMLpresentation_f.h"
#include "MathMLbuilder_f.h"
#include "styleparser_f.h"
#include "trans_f.h"
#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */

/* Function name table */
typedef CHAR_T     functName[10];
static  functName  functionName[] =
{
   TEXT("Imaginary"),
   TEXT("Real"),
   TEXT("arccosh"),
   TEXT("arccos"),
   TEXT("arccoth"),
   TEXT("arccot"),
   TEXT("arccsch"),
   TEXT("arccsc"),
   TEXT("arcsech"),
   TEXT("arcsec"),
   TEXT("arcsinh"),
   TEXT("arcsin"),
   TEXT("arctanh"),
   TEXT("arctan"),
   TEXT("arg"),
   TEXT("cosh"),
   TEXT("cos"),
   TEXT("coth"),
   TEXT("cot"),
   TEXT("csch"),
   TEXT("csc"),
   TEXT("curl"),
   TEXT("det"),
   TEXT("div"),
   TEXT("gcd"),
   TEXT("grad"),
   TEXT("log"),
   TEXT("ln"),
   TEXT("max"),
   TEXT("median"),
   TEXT("min"),
   TEXT("mode"),
   TEXT("mod"),
   TEXT("sech"),
   TEXT("sec"),
   TEXT("sinh"),
   TEXT("sin"),
   TEXT("tanh"),
   TEXT("tan"),
   TEXT("xor"),
   TEXT("")
};

/*----------------------------------------------------------------------
   RemoveAttr
   Remove attribute of type attrTypeNum from element el, if it exists
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void RemoveAttr (Element el, Document doc, int attrTypeNum)
#else /* __STDC__*/
static void RemoveAttr (el, doc, attrTypeNum)
     Element el;
     Document doc;
     int attrTypeNum;
#endif /* __STDC__*/
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
#ifdef __STDC__
static void MathSetAttributes (Element el, Document doc, Element* selEl)
#else /* __STDC__*/
static void MathSetAttributes (el, doc, selEl)
     Element el;
     Document doc;
     Element* selEl;
#endif /* __STDC__*/
{
  ElementType	elType, parentType;
  Element	parent, grandParent;

  elType = TtaGetElementType (el);
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
   Parameter index indicates the position where the text has to be split.
   Return the text element created within the next enclosing element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element      SplitTextInMathML (Document doc, Element el, int index, ThotBool *mrowCreated)
#else
static Element      SplitTextInMathML (doc, el, index, mrowCreated)
Document            doc;
Element             el;
int                 index;
ThotBool            *mrowCreated;
#endif
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

  len = TtaGetTextLength (el);
  /* if it's a mchar, mglyph or an entity, don't split it */
  elType = TtaGetElementType (parent);
  if (elType.ElTypeNum == MathML_EL_MCHAR ||
      elType.ElTypeNum == MathML_EL_MGLYPH)
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
  row = TtaGetParent (parent);
  elType = TtaGetElementType (row);
  withinMrow = (elType.ElTypeNum == MathML_EL_MROW ||
		elType.ElTypeNum == MathML_EL_SqrtBase ||
		elType.ElTypeNum == MathML_EL_MSTYLE ||
		elType.ElTypeNum == MathML_EL_MERROR ||
		elType.ElTypeNum == MathML_EL_MPADDED ||
		elType.ElTypeNum == MathML_EL_MPHANTOM ||
		elType.ElTypeNum == MathML_EL_CellWrapper ||
		elType.ElTypeNum == MathML_EL_MENCLOSE ||
                elType.ElTypeNum == MathML_EL_FencedExpression);

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
       TtaSplitText (el, index-1, doc);
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
#ifdef __STDC__
static void       DeleteIfPlaceholder (Element* el, Document doc, ThotBool record)
#else
static void       DeleteIfPlaceholder (el, doc, record)
Element* el;
Document doc;
ThotBool record;

#endif
{
Attribute	attr;
ElementType	elType;
AttributeType	attrType;

     if (*el == NULL)
       return;
     elType = TtaGetElementType (*el);
     if (elType.ElTypeNum == MathML_EL_Construct)
	{
        attrType.AttrSSchema = elType.ElSSchema;
        attrType.AttrTypeNum = MathML_ATTR_IntPlaceholder;
	attr = TtaGetAttribute (*el, attrType);
	if (attr != NULL)
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
#ifdef __STDC__
static Element       InsertPlaceholder (Element el, ThotBool before, Document doc, ThotBool record)
#else
static Element       InsertPlaceholder (el, before, doc, record)
Element el;
ThotBool before;
Document doc;
ThotBool record;

#endif
{
Element		sibling, placeholderEl;
ElementType	elType;
Attribute	attr;
AttributeType	attrType;
ThotBool		createConstruct, oldStructureChecking;

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
	if (before)
	   TtaPreviousSibling (&sibling);
	else
	   TtaNextSibling (&sibling);
	if (sibling != NULL)
	   if (!ElementNeedsPlaceholder (sibling))
	      createConstruct = FALSE;
	if (createConstruct)
	   {
	   elType = TtaGetElementType (el);
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
  If element el is not a child of a MROW and if it has at least one
  sibling that is not a Construct (place holder), create an enclosing MROW,
  except if el is a child of a MFENCED element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void	   CreateParentMROW (Element el, Document doc)
#else
static void        CreateParentMROW (el, doc)
Element	el;
Document doc;

#endif
{
  Element            sibling, row, parent, firstChild, lastChild, next,
                     previous;
  ElementType        elType;
  int                nChildren;

  /* check whether the parent is a mrow or inferred mrow */
  parent = TtaGetParent (el);
  if (parent == NULL)
     return;
  elType = TtaGetElementType (parent);
  if (elType.ElTypeNum != MathML_EL_MROW &&
      elType.ElTypeNum != MathML_EL_SqrtBase &&
      elType.ElTypeNum != MathML_EL_MSTYLE &&
      elType.ElTypeNum != MathML_EL_MERROR &&
      elType.ElTypeNum != MathML_EL_MPADDED &&
      elType.ElTypeNum != MathML_EL_MPHANTOM &&
      elType.ElTypeNum != MathML_EL_CellWrapper &&
      elType.ElTypeNum != MathML_EL_MENCLOSE &&
      elType.ElTypeNum != MathML_EL_FencedExpression)
	 {
	 sibling = TtaGetFirstChild (parent);
	 nChildren = 0;
	 firstChild = sibling;
	 while (sibling != NULL)
	    {
	    elType = TtaGetElementType (sibling);
	    if (elType.ElTypeNum != MathML_EL_Construct)
	       nChildren++;
	    TtaNextSibling (&sibling);
	    }
	 if (nChildren > 1)
	    {
	      /* generate a new row element to include these elements */
	      elType.ElTypeNum = MathML_EL_MROW;
	      row = TtaNewElement (doc, elType);
	      lastChild = TtaGetLastChild (parent);
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
	        /* move the old element into the new MROW */
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
	    }
	 }
}

/*----------------------------------------------------------------------
   RegenerateFencedSeparators
   el must be a FencedExpression element.
   Delete all existing FencedSeparator elements in el and create new
   ones according to the value of attribute separators of parent MFENCED.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void RegenerateFencedSeparators (Element el, Document doc, ThotBool record)
#else /* __STDC__*/
static void RegenerateFencedSeparators (el, doc, record)
     Element el;
     Document doc;
     ThotBool record;

#endif /* __STDC__*/
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
     if (elType.ElTypeNum == MathML_EL_FencedSeparator)
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
#ifdef __STDC__
static void         AddParen (Element el, Document doc)
#else
static void         AddParen (el, doc)
Element             el;
Document            doc;
#endif
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
#ifdef __STDC__
static void         CreateMathConstruct (int construct)
#else
static void         CreateMathConstruct (construct)
int                 construct;

#endif
{
  Document           doc;
  Element            sibling, last, el, row, child, leaf, placeholderEl,
                     parent, new, next;
  ElementType        newType, elType;
  SSchema            docSchema, mathSchema;
  int                c1, c2, i, j, len, oldStructureChecking;
  ThotBool	     before, ParBlock, emptySel, ok, insertSibling,
		     selectFirstChild, displayTableForm, mrowCreated;

      doc = TtaGetSelectedDocument ();
      TtaGiveLastSelectedElement (doc, &last, &c2, &j);
      TtaGiveFirstSelectedElement (doc, &sibling, &c1, &i); 
    
      /* Get the type of the first selected element */
      elType = TtaGetElementType (sibling);
      docSchema = TtaGetDocumentSSchema (doc);

      if (construct == 1)
	/* Math button */
	{
	if (ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")) == 0)
	   /* selection is in an HTML element */
	   {
           newType.ElTypeNum = HTML_EL_Math;
           newType.ElSSchema = elType.ElSSchema;
           TtaCreateElement (newType, doc);
	   }
#ifdef GRAPHML
	if (ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("GraphML")) == 0)
	   /* selection is in a GraphML element */
	   {
           newType.ElTypeNum = GraphML_EL_Math;
           newType.ElSSchema = elType.ElSSchema;
	   TtaAskFirstCreation ();
           TtaCreateElement (newType, doc);
	   }
#endif /* GRAPHML */
	return;
	}

      emptySel = TtaIsSelectionEmpty ();
      
      TtaSetDisplayMode (doc, DeferredDisplay);

      /* By default, the new element will be inserted before the selected
	 element */
      before = TRUE;

      TtaOpenUndoSequence (doc, sibling, last, c1, c2);
      mrowCreated = FALSE;

      /* Check whether the selected element is a MathML element */
      if (ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("MathML")) == 0)
	{
	  /* current selection is within a MathML element */
	  mathSchema = elType.ElSSchema;
	  if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
	    /* the first selected element is a character string */
	    {
	      len = TtaGetTextLength (sibling);
	      if (c1 > len)
		/* the caret is at the end of that character string */
		{
		next = sibling;  TtaNextSibling (&next);
		if (next)
		  /* there is another character string after that one.
		     split the enclosing mo, mn, mi or mtext */
		   sibling = SplitTextInMathML (doc, sibling, c1, &mrowCreated);
		else
		   /* create the new element after the character string */
		   before = FALSE;
		}
	      else
		/* split the character string before the first selected char */
		sibling = SplitTextInMathML (doc, sibling, c1, &mrowCreated);
	    }
	}
      else
	  /* the selection is not in a MathML element */
	{
	  /* get the MathML schema for this document or associate it to the
	     document if it is not associated yet */
	  mathSchema = TtaNewNature (docSchema, TEXT("MathML"), TEXT("MathMLP"));
	  if (ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")) == 0 &&
	      elType.ElTypeNum != HTML_EL_Math)
	    /* the current selection is in an HTML element, but it's not
	       a Math element */
	    {
	      if (elType.ElTypeNum == HTML_EL_TEXT_UNIT && c1 > 1)
		{
		  len = TtaGetTextLength (sibling);
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
		      TtaSplitText (sibling, c1-1, doc);
		      /* take the second part of the split text element */
		      TtaNextSibling (&sibling);
		      TtaRegisterElementCreate (sibling, doc);
		    }
		}

	      if (before)
		{
		  el = sibling;
		  TtaPreviousSibling (&el);
		  if (el != NULL)
		    {
		      newType = TtaGetElementType (el);
		      if (newType.ElTypeNum == HTML_EL_Math)
			{
			  /* insert at the end of the previous MathML element*/
			  before = FALSE;
			  sibling = TtaGetLastChild (el);		      
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
		      if (newType.ElTypeNum == HTML_EL_Math)
			{
			  /* insert at the beginning of the next MathML element */
			  before = TRUE;
			  sibling = TtaGetFirstChild (el);		      
			}
		    }
		}
	    }

	  if (ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")) == 0 &&
	      elType.ElTypeNum == HTML_EL_Math)
	    /* the current selection is in an HTML element, and it's a
	       Math element */
	    {
	      /* search the first MathML element */
		sibling = TtaGetFirstChild (sibling);
	      if (before)
		el = TtaGetFirstChild (sibling);
	      else
		el = TtaGetLastChild (sibling);		
	      if (el != NULL)
		sibling = el;
	    }
	  else
	    {
	      insertSibling = TRUE;
	      ok = TRUE;
	      /* try to create a Math element at the current position */
	      elType.ElSSchema = TtaGetSSchema (TEXT("HTML"), doc);
	      elType.ElTypeNum = HTML_EL_Math;
              if (emptySel)
		/* selection is empty */
		{
		  /* try first to create a new Math element as a child */
		  if (TtaCanInsertFirstChild (elType, sibling, doc))
		    insertSibling = FALSE;
		  /* if it fails, try to create a new Math element as a sibling */
		  else if (TtaCanInsertSibling (elType, sibling, before, doc))
		    insertSibling = TRUE;
		  else
		    /* complete failure */
		    ok = FALSE;
		}
	      else
		/* some non empty element is selected */
		{
		  /* try first to create a new Math element as a sibling */
		  if (TtaCanInsertSibling (elType, sibling, before, doc))
		    insertSibling = TRUE;
		  /* if it fails, try to create a new Math element as a child */
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
#ifdef GRAPHML
		 elType = TtaGetElementType (sibling);
		 if (ustrcmp (TtaGetSSchemaName (elType.ElSSchema),
			      TEXT("GraphML")) == 0)
		    /* selection is within a GraphML element */
		    {
		    elType.ElTypeNum = GraphML_EL_Math;
	            if (TtaCanInsertSibling (elType, sibling, before, doc))
	               /* insert the new Math element as a sibling element */
	                insertSibling = TRUE;
	            else if (TtaCanInsertFirstChild (elType, sibling,doc))
	               /* insert the new Math element as a child element */
	               insertSibling = FALSE;
	            else if (TtaCanInsertSibling (elType,
					TtaGetParent (sibling), before, doc))
			{
			sibling = TtaGetParent (sibling);
			insertSibling = TRUE;
			}
		    else
			sibling = NULL;
		    }
		 else
		    /* not within a GraphML element */
#endif
		    /* cannot insert any MathML element here */
                    sibling = NULL;
		 }
              if (sibling == NULL)
		/* cannot insert a math element here */
                 {
                 TtaSetDisplayMode (doc, DisplayImmediately);
                 TtaCloseUndoSequence (doc);
                 return;
                 }
              else
                 {
		 /* create a Math element */
                 el = TtaNewTree (doc, elType, "");
                 if (insertSibling)
                    /* insert the new Math element as a sibling element */
                    TtaInsertSibling (el, sibling, before, doc);
                 else
                    /* insert the new Math element as a child element */
                    TtaInsertFirstChild (&el, sibling, doc);
                 sibling = TtaGetFirstChild (el);
		 /* register the new Math element in the Undo queue */
		 TtaRegisterElementCreate (el, doc);
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
#  ifdef _WINDOWS
              CreateMatrixDlgWindow (BaseDialog, TableForm, TableCols,
				     TableRows, NumberCols, NumberRows);
#  else  /* !_WINDOWS */
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
#  endif /* !_WINDOWS */
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
	else
	  if (ParBlock)
	    /* the user wants to create a parenthesized block */
	    /* create two MF elements, as the first and last child of the new
	       MROW */
	    {
	      /* do not check the Thot abstract tree against the structure */
	      /* schema while changing the structure */
	      oldStructureChecking = TtaGetStructureChecking (doc);
	      TtaSetStructureChecking (0, doc);
	      TtaGiveFirstSelectedElement (doc, &el, &c1, &i);
	      AddParen (el, doc);
	      /* check the Thot abstract tree against the structure schema. */
	      TtaSetStructureChecking ((ThotBool)oldStructureChecking, doc);
	    }
	}

      if (emptySel)
	{
	  TtaUnselect (doc);
          el = TtaNewTree (doc, newType, "");
	  /* do not check the Thot abstract tree against the structure */
	  /* schema while changing the structure */
	  oldStructureChecking = TtaGetStructureChecking (doc);
	  TtaSetStructureChecking (0, doc);
	  
	  if (elType.ElTypeNum == MathML_EL_MROW ||
	      elType.ElTypeNum == MathML_EL_MathML)
	    {
	      /* the selected element is a MROW or the MathML element */
	      row = sibling;
	      if (before)
		sibling = TtaGetFirstChild (row);
	      else
		sibling = TtaGetLastChild (row);
	      if (sibling == NULL)
		{
		  /* replace the empty MROW by the new element*/
		  TtaInsertSibling (el, row, TRUE, doc);
		  TtaRegisterElementCreate (el, doc);
		  TtaRegisterElementDelete (row, doc);
		  TtaRemoveTree (row, doc);
		}
	      else
		{
		  /* check whether the selected element is a Construct */
		  elType = TtaGetElementType (sibling);
		  if (elType.ElTypeNum == MathML_EL_Construct)
		    {
		    TtaInsertFirstChild (&el, sibling, doc);
		    RemoveAttr (el, doc, MathML_ATTR_IntPlaceholder);
		    }
		  else
		    TtaInsertSibling (el, sibling, before, doc);
		  TtaRegisterElementCreate (el, doc);
		}
	    }
	  else if (elType.ElTypeNum == MathML_EL_Construct)
	    {
	      /* replace the Construct element */
	      TtaInsertFirstChild (&el, sibling, doc);
	      RemoveAttr (el, doc, MathML_ATTR_IntPlaceholder);
	      TtaRegisterElementCreate (el, doc);
	    }
	  else
	    {
	      /* the selected element is not a MROW */
	      if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
		/* go up to the MN, MI, MO or M_TEXT element */
		sibling = TtaGetParent (sibling);
	      /* insert the new element */
	      TtaInsertSibling (el, sibling, before, doc);
	      if (!mrowCreated)
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
		  while (NumberCols > 1)
		    {
		      new = TtaNewTree (doc, elType, "");
		      TtaInsertSibling (new, child, FALSE, doc);
		      NumberCols--;
		    }
		}
	      if (NumberRows > 1)
		{
		  elType.ElTypeNum = MathML_EL_MTR;
		  row = TtaSearchTypedElement (elType, SearchInTree, el);
		  while (NumberRows > 1)
		    {
		      new = TtaNewTree (doc, elType, "");
		      TtaInsertSibling (new, row, FALSE, doc);
		      NumberRows--;
		    }
		}
	      CheckAllRows (el, doc);
	    }

	  /* if the new element is a child of a FencedExpression element,
	     create the associated FencedSeparator elements */
	  parent = TtaGetParent (el);
	  elType = TtaGetElementType (parent);
	  if (elType.ElTypeNum == MathML_EL_FencedExpression)
	    RegenerateFencedSeparators (parent, doc, TRUE);

	  /* insert placeholders before and/or after the new element if
	    they are needed */
	  placeholderEl = InsertPlaceholder (el, TRUE, doc, !mrowCreated);
	  placeholderEl = InsertPlaceholder (el, FALSE, doc, !mrowCreated);

	  TtaSetDisplayMode (doc, DisplayImmediately);
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
#ifdef __STDC__
static void         CallbackMaths (int ref, int typedata, STRING data)
#else
static void         CallbackMaths (ref, typedata, data)
int                 ref;
int                 typedata;
STRING              data;

#endif
{
  Document           doc;

  switch (ref - MathsDialogue)
    {
    case FormMaths:
      /* the user has clicked the DONE button in the Math dialog box */
      InitMaths = FALSE;
      TtaDestroyDialogue (ref);	   
      break;

    case MenuMaths:
      /* the user has selected an entry in the math menu */
      doc = TtaGetSelectedDocument ();

      if ((int) data == 13)
	/* the user asks for the Symbol palette */
	{
	  TtcDisplayGreekKeyboard (doc, 1);
	  return;
	}
      else if (doc > 0)
	/* there is a selection */
        if (TtaGetDocumentAccessMode (doc))
	   /* the document is in not in ReadOnly mode */
           CreateMathConstruct (((int) data) +1);
      break;

    default:
      break;
    }
}

/*----------------------------------------------------------------------
   CreateMathMenu creates the maths menus.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CreateMathMenu (Document doc, View view)
#else
static void         CreateMathMenu (doc, view)
Document            doc;
View                view;
#endif
{
   if (!TtaGetDocumentAccessMode (doc))
     /* the document is in ReadOnly mode */
     return;

# ifndef _WINDOWS
  if (!InitMaths)
    {
      InitMaths = TRUE;

      /* Dialogue box for the Math palette */
      TtaNewSheet (MathsDialogue + FormMaths, TtaGetViewFrame (doc, view), 
		   TtaGetMessage (AMAYA, AM_BUTTON_MATH),
		   0, NULL, TRUE, 1, 'L', D_DONE);
      TtaNewIconMenu (MathsDialogue + MenuMaths, MathsDialogue + FormMaths, 0,
		   NULL, 14, mIcons, FALSE);
      TtaSetMenuForm (MathsDialogue + MenuMaths, 0);
      TtaSetDialoguePosition ();
    }
  TtaShowDialogue (MathsDialogue + FormMaths, TRUE); 
# else /* _WINDOWS */
  CreateMathDlgWindow (TtaGetViewFrame (doc, view), MathsDialogue, TtaGetThotWindow (GetWindowNumber (doc, view)), TtaGetMessage (AMAYA, AM_BUTTON_MATH));
# endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   AddMathButton        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AddMathButton (Document doc, View view)
#else
void                AddMathButton (doc, view)
Document            doc;
View                view;
#endif
{
  MathButton = TtaAddButton (doc, 1, iconMath, CreateMathMenu, "CreateMathMenu",
			     TtaGetMessage (AMAYA, AM_BUTTON_MATH),
			     TBSTYLE_BUTTON, TRUE);
}

/*----------------------------------------------------------------------
  SwitchIconMath
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void              SwitchIconMath (Document doc, View view, ThotBool state)
#else  /* __STDC__ */
void              SwitchIconMath (doc, view, state)
Document          doc;
 View             view;
ThotBool          state;
#endif /* __STDC__ */
{
  if (state)
    TtaChangeButton (doc, view, MathButton, iconMath, state);
  else
    TtaChangeButton (doc, view, MathButton, iconMathNo, state);
}

/*----------------------------------------------------------------------
  CreateMath
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateMath (Document document, View view)
#else  /* __STDC__ */
void                CreateMath (document, view)
Document            document;
View                view;
 
#endif /* __STDC__ */
{
   CreateMathConstruct (1);
}

/*----------------------------------------------------------------------
  CreateMROOT
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateMROOT (Document document, View view)
#else  /* __STDC__ */
void                CreateMROOT (document, view)
Document            document;
View                view;
 
#endif /* __STDC__ */
{
   CreateMathConstruct (2);
}

/*----------------------------------------------------------------------
  CreateMSQRT
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateMSQRT (Document document, View view)
#else  /* __STDC__ */
void                CreateMSQRT (document, view)
Document            document;
View                view;
 
#endif /* __STDC__ */
{
   CreateMathConstruct (3);
}

/*----------------------------------------------------------------------
  CreateMENCLOSE
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateMENCLOSE (Document document, View view)
#else  /* __STDC__ */
void                CreateMENCLOSE (document, view)
Document            document;
View                view;
 
#endif /* __STDC__ */
{
   CreateMathConstruct (18);
}

/*----------------------------------------------------------------------
  CreateMFRAC
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateMFRAC (Document document, View view)
#else  /* __STDC__ */
void                CreateMFRAC (document, view)
Document            document;
View                view;
 
#endif /* __STDC__ */
{
   CreateMathConstruct (4);
}

/*----------------------------------------------------------------------
  CreateMSUBSUP
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateMSUBSUP (Document document, View view)
#else  /* __STDC__ */
void                CreateMSUBSUP (document, view)
Document            document;
View                view;
 
#endif /* __STDC__ */
{
   CreateMathConstruct (5);
}

/*----------------------------------------------------------------------
  CreateMSUB
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateMSUB (Document document, View view)
#else  /* __STDC__ */
void                CreateMSUB (document, view)
Document            document;
View                view;
 
#endif /* __STDC__ */
{
   CreateMathConstruct (6);
}

/*----------------------------------------------------------------------
  CreateMSUP
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateMSUP (Document document, View view)
#else  /* __STDC__ */
void                CreateMSUP (document, view)
Document            document;
View                view;
 
#endif /* __STDC__ */
{
   CreateMathConstruct (7);
}

/*----------------------------------------------------------------------
  CreateMUNDEROVER
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateMUNDEROVER (Document document, View view)
#else  /* __STDC__ */
void                CreateMUNDEROVER (document, view)
Document            document;
View                view;
 
#endif /* __STDC__ */
{
   CreateMathConstruct (8);
}

/*----------------------------------------------------------------------
  CreateMUNDER
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateMUNDER (Document document, View view)
#else  /* __STDC__ */
void                CreateMUNDER (document, view)
Document            document;
View                view;
 
#endif /* __STDC__ */
{
   CreateMathConstruct (9);
}

/*----------------------------------------------------------------------
  CreateMOVER
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateMOVER (Document document, View view)
#else  /* __STDC__ */
void                CreateMOVER (document, view)
Document            document;
View                view;
 
#endif /* __STDC__ */
{
   CreateMathConstruct (10);
}

/*----------------------------------------------------------------------
  CreateMROW
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateMROW (Document document, View view)
#else  /* __STDC__ */
void                CreateMROW (document, view)
Document            document;
View                view;
 
#endif /* __STDC__ */
{
   CreateMathConstruct (11);
}

/*----------------------------------------------------------------------
  CreateMMULTISCRIPTS
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateMMULTISCRIPTS (Document document, View view)
#else  /* __STDC__ */
void                CreateMMULTISCRIPTS (document, view)
Document            document;
View                view;
 
#endif /* __STDC__ */
{
   CreateMathConstruct (12);
}

/*----------------------------------------------------------------------
  CreateMTABLE
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateMTABLE (Document document, View view)
#else  /* __STDC__ */
void                CreateMTABLE (document, view)
Document            document;
View                view;
 
#endif /* __STDC__ */
{
   CreateMathConstruct (13);
}


/*----------------------------------------------------------------------
 CheckMROW
 If el is a MROW element with only one child, remove the MROW element
 and replace it by its child.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void CheckMROW (Element* el, Document doc)
#else /* __STDC__*/
static void CheckMROW (el, doc)
  Element* el;
  Document doc;
#endif /* __STDC__*/
{
  Element	child, firstChild, next;
  ElementType	elType;
  AttributeType	attrType;
  Attribute	attr;
  int		nChildren;
  int           oldStructureChecking;

  elType = TtaGetElementType (*el);
  if (elType.ElTypeNum == MathML_EL_MROW)
     /* the parent of the deleted element is a MROW */
     {
     firstChild = TtaGetFirstChild (*el);
     child = firstChild;
     /* count all children that are not placeholders */
     nChildren = 0;
     while (child != NULL && nChildren < 2)
	{
	elType = TtaGetElementType (child);
	if (elType.ElTypeNum != MathML_EL_Construct)
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
#ifdef __STDC__
static void RoundSelection (Element *firstSel, Element *lastSel, int *firstChar, int *lastChar)
#else /* __STDC__*/
static void RoundSelection (firstSel, lastSel, firstChar, lastChar)
     Element *firstSel;
     Element *lastSel;
     int *firstChar;
     int *lastChar;
#endif /* __STDC__*/
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
#ifdef __STDC__
static void CreateCharStringElement (int typeNum, Document doc)
#else /* __STDC__*/
static void CreateCharStringElement (typeNum, doc)
     int typeNum;
     Document doc;
#endif /* __STDC__*/
{
   ElementType    elType;
   Element        firstSel, lastSel, first, last, el, newEl, nextEl,
                  leaf, lastLeaf, nextLeaf, parent, selEl;
   int            firstChar, lastChar, i, j, oldStructureChecking;
   ThotBool       nonEmptySel, done, mrowCreated, same;

   done = FALSE;
   /* get the current selection */
   TtaGiveFirstSelectedElement (doc, &firstSel, &firstChar, &i);
   TtaGiveLastSelectedElement (doc, &lastSel, &j, &lastChar);

   /* check whether the selection is empty (just a caret) or contains some
      characters/elements */
   nonEmptySel = !TtaIsSelectionEmpty ();

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
   if (ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("MathML")) != 0)
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
	  if (elType.ElTypeNum == MathML_EL_MTEXT ||
	      elType.ElTypeNum == MathML_EL_MI ||
	      elType.ElTypeNum == MathML_EL_MN ||
	      elType.ElTypeNum == MathML_EL_MO)
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
     if (elType.ElTypeNum == MathML_EL_TEXT_UNIT ||
	 elType.ElTypeNum == MathML_EL_SYMBOL_UNIT ||
	 elType.ElTypeNum == MathML_EL_MCHAR ||
	 elType.ElTypeNum == MathML_EL_MGLYPH)
        first = TtaGetParent (firstSel);
     else
        first = firstSel;
     elType = TtaGetElementType (lastSel);
     if (elType.ElTypeNum == MathML_EL_TEXT_UNIT ||
	 elType.ElTypeNum == MathML_EL_SYMBOL_UNIT ||
	 elType.ElTypeNum == MathML_EL_MCHAR ||
	 elType.ElTypeNum == MathML_EL_MGLYPH)
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
         if (elType.ElTypeNum == MathML_EL_MTEXT ||
	     elType.ElTypeNum == MathML_EL_MI ||
	     elType.ElTypeNum == MathML_EL_MN ||
	     elType.ElTypeNum == MathML_EL_MO)
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
	      lastChar = TtaGetTextLength (lastSel);
	   el = SplitTextInMathML (doc, lastSel, lastChar+1, &mrowCreated);
	   } 
	 else if (elType.ElTypeNum == MathML_EL_MCHAR ||
		  elType.ElTypeNum == MathML_EL_MGLYPH)
	   {
	   if (lastChar == 0)
	      lastChar = TtaGetElementVolume (lastSel);
	   el = SplitTextInMathML (doc, lastSel, lastChar+1, &mrowCreated);
	   }
	 elType = TtaGetElementType (firstSel);
	 if (elType.ElTypeNum == MathML_EL_TEXT_UNIT ||
	     elType.ElTypeNum == MathML_EL_MCHAR ||
	     elType.ElTypeNum == MathML_EL_MGLYPH)
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
#ifdef __STDC__
void CreateMTEXT (Document document, View view)
#else /* __STDC__*/
void CreateMTEXT (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
   CreateCharStringElement (MathML_EL_MTEXT, document);
}

/*----------------------------------------------------------------------
 CreateMI
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void CreateMI (Document document, View view)
#else /* __STDC__*/
void CreateMI (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
   CreateCharStringElement (MathML_EL_MI, document);
}

/*----------------------------------------------------------------------
 CreateMN
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void CreateMN (Document document, View view)
#else /* __STDC__*/
void CreateMN (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
   CreateCharStringElement (MathML_EL_MN, document);
}

/*----------------------------------------------------------------------
CreateMO
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void CreateMO (Document document, View view)
#else /* __STDC__*/
void CreateMO (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
   CreateCharStringElement (MathML_EL_MO, document);
}

/*----------------------------------------------------------------------
   InitMathML initializes MathML context.           
  ----------------------------------------------------------------------*/
void                InitMathML ()
{
#  ifndef _WINDOWS 
   iconMath = TtaCreatePixmapLogo (Math_xpm);
   iconMathNo = TtaCreatePixmapLogo (MathNo_xpm);
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
#  endif /* _WINDOWS */
  MathsDialogue = TtaSetCallback (CallbackMaths, MAX_MATHS);
  KeyboardsLoadResources ();
}

/*----------------------------------------------------------------------
   GetCharType
   returns the type of character c (MN, MI or MO).
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static int GetCharType (UCHAR_T c, CHAR_T alphabet)
#else /* __STDC__*/
static int GetCharType (c, alphabet)
UCHAR_T c;
CHAR_T  alphabet;
#endif /* __STDC__*/
{
  int	ret;

  ret = MathML_EL_MO;
  if (alphabet == 'L')
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
  else if (alphabet == 'G')
     /* Symbol character set */
     {
     if (c >= '0' && c <= '9')
        ret = MathML_EL_MN;
     else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
        ret = MathML_EL_MI;
     else
        ret = MathML_EL_MO;
     }
  return ret;
}

/*----------------------------------------------------------------------
   MergeMathEl
   merge element el2 with element el
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void MergeMathEl (Element el, Element el2, ThotBool before,
			 Element *newSelEl, int *newSelChar, Document doc)
#else /* __STDC__*/
static void MergeMathEl (el, el2, before, newSelEl, newSelChar, doc)
     Element el;
     Element el2;
     ThotBool before;
     Element *newSelEl;
     int *newSelChar;
     Document doc;
#endif /* __STDC__*/
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
        len = TtaGetTextLength (prevEl);
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
	len = TtaGetTextLength (prevEl);
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
#ifdef __STDC__
static Element ClosestLeaf (Element el, int* pos)
#else /* __STDC__*/
static Element ClosestLeaf (el, pos)
     Element el;
     int* pos;
#endif /* __STDC__*/
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
         if (elType.ElTypeNum == MathML_EL_FencedSeparator)
	    /* avoid selecting FencedSeparator elements */
	    TtaPreviousSibling (&prev);
	 }
      if (prev == NULL)
	 {
	 next = parent;  TtaNextSibling (&next);
	 if (next != NULL)
	    {
	    elType = TtaGetElementType (next);
	    if (elType.ElTypeNum == MathML_EL_FencedSeparator)
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
	if (prev != NULL)
	   *pos = TtaGetTextLength (leaf) + 1;
	else
	   *pos = 1;
      }
   return elem;
}

/*----------------------------------------------------------------------
   SeparateFunctionNames
   
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void SeparateFunctionNames (Element *firstEl, Element lastEl, Document doc, Element *newSelEl, int *newSelChar)
#else /* __STDC__*/
static void SeparateFunctionNames (firstEl, lastEl, doc, newSelEl, newSelChar)
   Element *firstEl;
   Element lastEl;
   Document doc;
   Element *newSelEl;
   int *newSelChar;
#endif /* __STDC__*/

{
  ElementType    elType;
  Element        el, nextEl, textEl, nextTextEl, firstTextEl, newEl, newText,
                 cur, prev, next, prevEl, currentMI, prevText;
  Language       lang;
  int            len, flen, firstChar, i, func;
#define BUFLEN 200
  UCHAR_T        text[BUFLEN];
  ThotBool       split, stop, firstElChanged, leafSplit;

  el = *firstEl;
  firstElChanged = FALSE;
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
    if (elType.ElTypeNum == MathML_EL_MI)
       {
       split = FALSE;
       /* if the MI element has to be split, its pieces will replace itself */
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
             len = TtaGetTextLength (textEl);
             /* get the content of this text leaf and analyze it */
             len = BUFLEN - 1;
             TtaGiveTextContent (textEl, text, &len, &lang);
	     firstChar = 0;
	     /* ignore text leaves that are shorter than the shortest
		function name (2 characters) */
	     /* function names can only be written in latin characters */
             if (len > 1 && TtaGetAlphabet (lang) == 'L')
                {
		   /* check all possible substrings of the text leaf */
		   for (i = 0; i < len-1; i++)
		     {
		     func = 0;
		     /* compare a substring with all function names */
		     stop = FALSE;
		     do
		       {
		       flen = ustrlen (functionName[func]);
		       if (ustrncmp (functionName[func], &text[i], flen) == 0) 
			 /* this substring is a function name */
			 {
			 /* this element will be deleted */
			 if (!split)
			    TtaRegisterElementDelete (el, doc);
			 /* create a MI for all text pieces preceding the
			    function name  */
			 newEl = NULL;
			 prev = NULL;
			 if (firstTextEl && firstTextEl != textEl)
			   {
			   elType.ElTypeNum = MathML_EL_MI;
			   newEl = TtaNewElement (doc, elType);
			   TtaInsertSibling (newEl, prevEl, FALSE, doc);
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
			       TtaInsertSibling (cur, prev, FALSE, doc);
			       /* if the selection is in this part of the
				  original element, move it to the new piece
				  of text */
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
			   TtaInsertSibling (newEl, prevEl, FALSE, doc);
			   prevEl = newEl;
			   elType.ElTypeNum = MathML_EL_TEXT_UNIT;
			   newText = TtaNewElement (doc, elType);
			   TtaInsertFirstChild (&newText, newEl, doc);
			   text[i] = EOS;
			   TtaSetTextContent (newText, &text[firstChar], lang,
					      doc);
			   MathSetAttributes (newEl, doc, NULL);
			   TtaRegisterElementCreate (newEl, doc);
			   if (!firstElChanged)
			     {
			     *firstEl = newEl;
			     firstElChanged = TRUE;
			     }
			   /* if the selection is in this part of the original
			      element, move it to the new piece of text */
			   if (textEl == *newSelEl)
			      if (*newSelChar - 1 <= i)
				{
				*newSelEl = newText;
				*newSelChar = *newSelChar - firstChar;
				}
			   }
			 /* create a MI for the function name itself */
			 elType.ElTypeNum = MathML_EL_MI;
			 newEl = TtaNewElement (doc, elType);
			 TtaInsertSibling (newEl, prevEl, FALSE, doc);
			 prevEl = newEl;
			 elType.ElTypeNum = MathML_EL_TEXT_UNIT;
			 newText = TtaNewElement (doc, elType);
			 TtaInsertFirstChild (&newText, newEl, doc);
			 TtaSetTextContent (newText, functionName[func], lang, doc);
			 MathSetAttributes (newEl, doc, NULL);
			 TtaRegisterElementCreate (newEl, doc);
			 if (!firstElChanged)
			   {
			   *firstEl = newEl;
			   firstElChanged = TRUE;
			   }
			 /* if the selection is in this part of the original
			    element, move it to the new piece of text */
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
			    ustrcmp (functionName[func], TEXT("")) != 0);
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
#ifdef __STDC__
static void ParseMathString (Element theText, Element theElem, Document doc)
#else /* __STDC__*/
static void ParseMathString (theText, theElem, doc)
     Element theText;
     Element theElem;
     Document doc;
#endif /* __STDC__*/

{
  Element	el, selEl, prevEl, nextEl, textEl, newEl, lastEl,
		firstEl, newSelEl, prev, next, parent, placeholderEl;
  ElementType	elType, elType2;
  AttributeType attrType;
  Attribute     attr;
  SSchema	MathMLSchema;
  int		firstSelChar, lastSelChar, newSelChar, len, totLen, i, j,
		start;
  CHAR_T	alphabet, c;
  Language	lang;
#define TXTBUFLEN 200
  UCHAR_T       text[TXTBUFLEN];
  Language	language[TXTBUFLEN];
  char          mathType[TXTBUFLEN];
  int           oldStructureChecking;
  ThotBool      empty;

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
  else if (elType2.ElTypeNum == MathML_EL_MCHAR ||
	   elType2.ElTypeNum == MathML_EL_MGLYPH)
     textEl = TtaGetFirstChild (theText);
  else
     textEl = NULL;
  if (textEl != NULL)
       {
       len = TtaGetTextLength (textEl);
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
          TtaGiveTextContent (textEl, &text[i], &len, &lang);
          alphabet = TtaGetAlphabet (lang);
	  for (j = 0; j < len; j++)
	     {
	     language[i+j] = lang;
	     mathType[i+j] = (char)GetCharType (text[i+j], alphabet);
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

  TtaOpenUndoSequence (doc, selEl, selEl, firstSelChar, lastSelChar);
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
	  if (elType.ElTypeNum == MathML_EL_FencedExpression)
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
     if (mathType[i] != mathType[i-1] ||
	 language[i] != language[i-1] ||
	 mathType[i-1] == (char)MathML_EL_MO ||
	 i == totLen)
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
       TtaSetTextContent (textEl, &text[start], language[start], doc);
       text[j] = c;
       lastEl = newEl;
       if (newSelEl != NULL)
	  {
	  newSelEl = textEl;
	  if (newSelChar < j)
	     if (newSelChar < start)
		newSelChar = 1;
	     else
		newSelChar -= start;
	  }
       MathSetAttributes (newEl, doc, &newSelEl);
       start = i;

       if (mathType[i-1] == (char)MathML_EL_MO)
	  /* the new element is an operator */
	  {
	  /* if the new element contains a single SYMBOL, placeholders may
	     be needed before and/or after that operator */
	  placeholderEl = InsertPlaceholder (newEl, TRUE, doc, TRUE);
	  placeholderEl = InsertPlaceholder (newEl, FALSE, doc, TRUE);
	  /* the new contents may be an horizontally stretchable symbol */
	  if (newEl != NULL)
	    {
	    parent = TtaGetParent (newEl);
	    elType = TtaGetElementType (parent);
	    if (elType.ElTypeNum == MathML_EL_UnderOverBase ||
		elType.ElTypeNum == MathML_EL_Underscript ||
		elType.ElTypeNum == MathML_EL_Overscript)
	       SetSingleIntHorizStretchAttr (parent, doc, &newSelEl);
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
     if (elType.ElTypeNum == MathML_EL_FencedExpression)
       RegenerateFencedSeparators (parent, doc, TRUE);

     if (elType.ElTypeNum == MathML_EL_MROW)
       /* delete the parent MROW element if it is no longer useful */
       CheckMROW (&parent, doc);
     else
       /* Create a MROW element that encompasses the new elements if necessary */
       CreateParentMROW (firstEl, doc);
     }

  TtaSetStructureChecking ((ThotBool)oldStructureChecking, doc);
  TtaSetDisplayMode (doc, DisplayImmediately);
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
#ifdef __STDC__
static void SetAttrParseMe (Element el, Document doc)
#else /* __STDC__*/
static void SetAttrParseMe (el, doc)
     Element el;
     Document doc;
#endif /* __STDC__*/
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
 CreateMCHAR
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void CreateMCHAR (Document document, View view)
#else /* __STDC__*/
void CreateMCHAR (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
   Element       firstSel, lastSel, el, el1, leaf, parent, sibling;
   ElementType   elType, elType1;
   Attribute     attr;
   AttributeType attrType;
   int           firstChar, lastChar, i, len;
   ThotBool      before, done;
   CHAR_T        value[8];
   CHAR_T        alphabet;
   Language      lang;

   if (!TtaIsSelectionEmpty ())
      return;
   TtaGiveFirstSelectedElement (document, &firstSel, &firstChar, &i);

   /* if not within a MathML element, nothing to do */
   elType = TtaGetElementType (firstSel);
   if (ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("MathML")) != 0)
      return;
   
   MathMLEntityName[0] = EOS;
#  ifdef _WINDOWS
   CreateMCHARDlgWindow (TtaGetViewFrame (document, view), BaseDialog, MathEntityForm, MathEntityText,
                              MathMLEntityName, TtaGetMessage (1, BMCharacter), TtaGetMessage (AMAYA, AM_MATH_ENTITY_NAME));
#  else
   TtaNewForm (BaseDialog + MathEntityForm, TtaGetViewFrame (document, view), 
	       TtaGetMessage (1, BMCharacter), TRUE, 1, 'L', D_CANCEL);
   TtaNewTextForm (BaseDialog + MathEntityText, BaseDialog + MathEntityForm,
		   TtaGetMessage (AMAYA, AM_MATH_ENTITY_NAME), NAME_LENGTH, 1,
		   FALSE);
   TtaSetTextForm (BaseDialog + MathEntityText, MathMLEntityName);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + MathEntityForm, FALSE);
   TtaWaitShowDialogue ();
#  endif /* _WINDOWS */
   if (MathMLEntityName[0] != EOS)
     {
      if (!TtaIsSelectionEmpty ())
	 return;
      TtaGiveFirstSelectedElement (document, &firstSel, &firstChar, &i);
      /* if not within a MathML element, nothing to do */
      elType = TtaGetElementType (firstSel);
      if (ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("MathML")) != 0)
         return;
      TtaGiveLastSelectedElement (document, &lastSel, &i, &lastChar);
      TtaOpenUndoSequence (document, firstSel, lastSel, firstChar, lastChar);
      TtaUnselect (document);
      done = FALSE;
      /* the new mchar element will be inserted before the first element
	 selected */
      before = TRUE;
      sibling = firstSel;

      elType1.ElSSchema = elType.ElSSchema;
      elType1.ElTypeNum = MathML_EL_MCHAR;
      el = TtaNewElement (document, elType1);
      if (elType.ElTypeNum == MathML_EL_Construct)
	 /*this element is an empty expression. Replace it by a mtext element*/
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
	 {
	 parent = TtaGetParent (firstSel);
	 elType1 = TtaGetElementType (parent);
	 if (elType1.ElTypeNum == MathML_EL_MCHAR ||
	     elType1.ElTypeNum == MathML_EL_MGLYPH)
	    /* the first selected element is within a mchar or mglyph. The new
	       mchar will be inserted as a sibling of this mchar or mglyph */
	    sibling = parent;
	 if (firstChar > 1)
	    {
	     len = TtaGetTextLength (firstSel);
	     if (firstChar > len)
	       /* the caret is at the end of that character string */
	       /* Create the new element after the character string */
	       before = FALSE;
	     else if (elType1.ElTypeNum == MathML_EL_MTEXT)
	       {
		 /* split the text to insert the mchar */
		 TtaRegisterElementReplace (firstSel, document);
		 TtaSplitText (firstSel, firstChar-1, document);
		 /* take the second part of the split text element */
		 sibling = firstSel;
		 TtaNextSibling (&sibling);
		 TtaRegisterElementCreate (sibling, document);
	       }
	    }
	 }
     attrType.AttrSSchema = elType.ElSSchema;
     attrType.AttrTypeNum = MathML_ATTR_name;
     attr = TtaGetAttribute (el, attrType);
     if (!attr)
       {
       attr =  TtaNewAttribute (attrType);
       TtaAttachAttribute (el, attr, document);
       }
     TtaSetAttributeText (attr, MathMLEntityName, el, document);
     if (!done)
       {
	 TtaInsertSibling (el, sibling, before, document);
         TtaRegisterElementCreate (el, document);
       }
     MapMathMLEntity (MathMLEntityName, value, 8, &alphabet);
     if (alphabet == EOS)
       /* unknown entity */
       {
       value[0] = '?';
       value[1] = EOS;
       lang = TtaGetLanguageIdFromAlphabet('L');
       }
     else
       lang = TtaGetLanguageIdFromAlphabet(alphabet);
     elType1.ElTypeNum = MathML_EL_TEXT_UNIT;
     leaf = TtaNewElement (document, elType1);
     TtaInsertFirstChild (&leaf, el, document);
     TtaSetTextContent (leaf, value, lang, document);
     TtaSetAccessRight (leaf, ReadOnly, document);
     len = TtaGetTextLength (leaf);     
     TtaSelectString (document, leaf, len+1, len);
     ParseMathString (el, TtaGetParent (el), document);     
     TtaCloseUndoSequence (document);
     }
}

/*----------------------------------------------------------------------
   SetElementCharFont
   associate attribute attrType with value val to element el if it is
   of type MI, MTEXT, MO,..., or to its descendant of that type.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void SetElementCharFont (Element el, AttributeType attrType, int val, Document doc)
#else /* __STDC__*/
static void SetElementCharFont (el, attrType, val, doc)
     Element el;
     AttributeType attrType;
     int val;
     Document doc;
#endif /* __STDC__*/
{
  ElementType         elType;
  Attribute           attr, intAttr;
  AttributeType       intAttrType;
  Element             child;
  ThotBool            newAttr;

  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == MathML_EL_MTEXT ||
      elType.ElTypeNum == MathML_EL_MI ||
      elType.ElTypeNum == MathML_EL_MO ||
      elType.ElTypeNum == MathML_EL_MN ||
      elType.ElTypeNum == MathML_EL_MS ||
      elType.ElTypeNum == MathML_EL_MSTYLE)
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
#ifdef __STDC__
void SetMathCharFont (Document doc, int attribute)
#else /* __STDC__*/
void SetMathCharFont (doc, attribute)
     Document doc;
     int attribute;
#endif /* __STDC__*/
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
     leaf = SplitTextInMathML (doc, lastSel, lastChar+1, &mrowCreated);
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
  attrType.AttrSSchema = TtaGetSSchema (TEXT("MathML"), doc);
  attrType.AttrTypeNum = attribute;
  attr = TtaGetAttribute (firstSel, attrType);
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
#ifdef __STDC__
void MtextCreated (NotifyElement *event)
#else /* __STDC__*/
void MtextCreated(event)
     NotifyElement *event;
#endif /* __STDC__*/
{
  /* associate a IntParseMe attribute with the new MTEXT element */
  SetAttrParseMe (event->element, event->document);
}

/*----------------------------------------------------------------------
   MathStringModified
   The content of an element MTEXT, MI, MO, MN, or MS has been modified.
   Parse the new content and create the appropriate MI, MO, MN elements.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void MathStringModified (NotifyOnTarget *event)
#else /* __STDC__*/
void MathStringModified (event)
     NotifyOnTarget *event;
#endif /* __STDC__*/
{
  ParseMathString (event->target, event->element, event->document);
}

/*----------------------------------------------------------------------
 NewMathString
 An new text string has been created in a MathML element.
 Parse its contents.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void NewMathString (NotifyElement *event)
#else /* __STDC__*/
void NewMathString(event)
     NotifyElement *event;
#endif /* __STDC__*/
{
   RemoveAttr (event->element, event->document, MathML_ATTR_EntityName);
   if (TtaGetTextLength (event->element) > 0)
      ParseMathString (event->element, TtaGetParent (event->element),
		       event->document);
}

/*----------------------------------------------------------------------
 MathElementPasted
 An element has been pasted in a MathML structure.
 Create placeholders before and after the pasted elements if necessary.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void MathElementPasted (NotifyElement *event)
#else /* __STDC__*/
void MathElementPasted(event)
     NotifyElement *event;
#endif /* __STDC__*/
{
   Element	placeholderEl, parent;
   ElementType	elType, elTypeParent;
   int          oldStructureChecking;


   elType = TtaGetElementType (event->element);
   oldStructureChecking = TtaGetStructureChecking (event->document);
   TtaSetStructureChecking (0, event->document);

   /* if the pasted element is a child of a FencedExpression element,
      create the associated FencedSeparator elements */
   parent = TtaGetParent (event->element);
   elTypeParent = TtaGetElementType (parent);
   if (elTypeParent.ElTypeNum == MathML_EL_FencedExpression)
     RegenerateFencedSeparators (parent, event->document, FALSE/******/);

   /* if the pasted element is a character string within a MI, MN, or MO
      element, parse the new content to isolate identifiers, numbers and
      operators */
   if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
     {
     if (elTypeParent.ElTypeNum == MathML_EL_MI ||
         elTypeParent.ElTypeNum == MathML_EL_MO ||
	 elTypeParent.ElTypeNum == MathML_EL_MN)
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
     }

   TtaSetStructureChecking ((ThotBool)oldStructureChecking, event->document);
}


/*----------------------------------------------------------------------
 MathElementWillBeDeleted
 This function is called by the DELETE command for each selected element
 and for all their descendants.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool MathElementWillBeDeleted (NotifyElement *event)
#else /* __STDC__*/
ThotBool MathElementWillBeDeleted(event)
     NotifyElement *event;
#endif /* __STDC__*/
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
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DeleteMColumn (Document document, View view)
#else  /* __STDC__ */
void                DeleteMColumn (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   Element             el, cell, colHead, selCell, leaf;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;
   Document            refDoc;
   CHAR_T                name[50];
   int                 firstchar, lastchar, len;
   ThotBool            selBefore;

   /* get the first selected element */
   TtaGiveFirstSelectedElement (document, &el, &firstchar, &lastchar);
   if (el != NULL)
     {
       elType = TtaGetElementType (el);
       if (elType.ElSSchema == GetMathMLSSchema (document))
	 {
	   if (elType.ElTypeNum == MathML_EL_MTD)
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
			len = TtaGetTextLength (leaf);
		        TtaSelectString (document, leaf, len+1, len);
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
#ifdef __STDC__
void MathElementDeleted (NotifyElement *event)
#else /* __STDC__*/
void MathElementDeleted(event)
     NotifyElement *event;
#endif /* __STDC__*/
{
   Element	sibling, placeholderEl, parent, child, grandChild;
   ElementType	parentType;
   int		i, newTypeNum;
   int          oldStructureChecking;

   if (event->info == 1)
      /* call from Undo command. Don't do anything */
      return;
   parent = event->element; /* parent of the deleted element */
   parentType = TtaGetElementType (parent);

   if (parentType.ElTypeNum == MathML_EL_FencedExpression)
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
         placeholderEl = InsertPlaceholder (sibling, FALSE, event->document,
					    FALSE/*****/);
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
	if (parentType.ElTypeNum == MathML_EL_MSUBSUP)
	   /* a Subscript in a MSUBSUP. Transform the MSUBSUP into a MSUP */
	   newTypeNum = MathML_EL_MSUP;
	else if (parentType.ElTypeNum == MathML_EL_MSUB)
	   /* a Subscript in a MSUB. Remove the MSUB and the Base */
	   newTypeNum = -1;
	break;

      case MathML_EL_Superscript:	/* a Superscript has been deleted */
	if (parentType.ElTypeNum == MathML_EL_MSUBSUP)
	   /* a Superscript in a MSUBSUP. Transform the MSUBSUP into a MSUB */
	   newTypeNum = MathML_EL_MSUB;
	else if (parentType.ElTypeNum == MathML_EL_MSUP)
	   /* a Superscript in a MSUP. Remove the MSUP and the Base */
	   newTypeNum = -1;
	break;

      case MathML_EL_Underscript:	/* an Underscript has been deleted */
	if (parentType.ElTypeNum == MathML_EL_MUNDEROVER)
	   /* an Underscript in a MUNDEROVER. Transform the MUNDEROVER into
	      a MOVER */
	   newTypeNum = MathML_EL_MOVER;
	else if (parentType.ElTypeNum == MathML_EL_MUNDER)
	   /* an Underscript in a MUNDER. Remove the MUNDER and the
	      UnderOverBase */
	   newTypeNum = -1;
	break;

      case MathML_EL_Overscript:	/* an Overscript has been deleted */
	if (parentType.ElTypeNum == MathML_EL_MUNDEROVER)
	   /* an Overscript in a MUNDEROVER. Transform the MUNDEROVER into
	      a MUNDER */
	   newTypeNum = MathML_EL_MUNDER;
	else if (parentType.ElTypeNum == MathML_EL_MOVER)
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
      ChangeTypeOfElement (parent, event->document, newTypeNum);
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
	   placeholderEl = InsertPlaceholder (grandChild, FALSE, event->document, FALSE/******/);
	   placeholderEl = InsertPlaceholder (grandChild, TRUE, event->document, FALSE/******/);
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
#ifdef __STDC__
void FenceModified (NotifyOnValue *event)
#else /* __STDC__*/
void FenceModified(event)
     NotifyOnValue *event;
#endif /* __STDC__*/
{
  Element	mfencedEl;
  ElementType	elType;
  AttributeType	attrType;
  Attribute	attr;
  UCHAR_T         text[2];

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
  text[0] = (UCHAR_T) event->value;
  text[1] = '\0';
  TtaSetAttributeText (attr, text, mfencedEl, event->document);
}

/*----------------------------------------------------------------------
 MathAttrOtherCreated
 An attribute other has been created by the user. If that attribute is on
 a TEXT element, move it to the parent.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void MathAttrOtherCreated (NotifyAttribute *event)
#else /* __STDC__*/
void MathAttrOtherCreated (event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
   AttrToSpan (event->element, event->attribute, event->document);
}

/*----------------------------------------------------------------------
   LinkAttrInMenu
   Called by Thot when building the Attribute menu.
   Prevent Thot from including attribute link in the menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            LinkAttrInMenu (NotifyAttribute * event)
#else  /* __STDC__ */
ThotBool            LinkAttrInMenu (event)
NotifyAttribute    *event;
 
#endif /* __STDC__ */
{
   return TRUE;
}

/*----------------------------------------------------------------------
 MathAttrFontsizeCreated
 An attribute fontsize has been created or updated by the user.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void MathAttrFontsizeCreated (NotifyAttribute *event)
#else /* __STDC__*/
void MathAttrFontsizeCreated(event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
#define buflen 200
  STRING           value;
  int              length;

  value = TtaAllocString (buflen);
  value[0] = WC_EOS;
  length = TtaGetTextAttributeLength (event->attribute);
  if (length >= buflen)
     length = buflen - 1;
  if (length > 0)
     TtaGiveTextAttributeValue (event->attribute, value, &length);
  /* associate a CSS property font-size with the element */
  SetFontsize (event->document, event->element, value);
  TtaFreeMemory (value);
}
 
 
/*----------------------------------------------------------------------
 MathAttrFontsizeDelete
 The user is deleting an attribute fontsize.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool MathAttrFontsizeDelete (NotifyAttribute *event)
#else /* __STDC__*/
ThotBool MathAttrFontsizeDelete(event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
  /* ask the CSS handler to remove the effect of the CSS property font-size */
  /* in the statement below, "10pt" is meaningless. It's here just to
     make the CSS parser happy */
  ParseHTMLSpecificStyle (event->element, TEXT("font-size: 10pt"), event->document, TRUE);
  return FALSE; /* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
 MathAttrFontfamilyCreated
 An attribute fontfamily has been created or modified by the user.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void MathAttrFontfamilyCreated (NotifyAttribute *event)
#else /* __STDC__*/
void MathAttrFontfamilyCreated (event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
  STRING           value;
  int              length;

  value = TtaAllocString (buflen);
  value[0] = WC_EOS;
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
#ifdef __STDC__
ThotBool MathAttrFontfamilyDelete (NotifyAttribute *event)
#else /* __STDC__*/
ThotBool MathAttrFontfamilyDelete (event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
  /* ask the CSS handler to remove the effect of property font-family */
  /* in the statement below, "serif" is meaningless. It's here just to
     make the CSS parser happy */
  ParseHTMLSpecificStyle (event->element, TEXT("font-family: serif"), event->document, TRUE);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
 MathAttrColorCreated
 An attribute color has been created or modified by the user.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void MathAttrColorCreated (NotifyAttribute *event)
#else /* __STDC__*/
void MathAttrColorCreated (event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
  STRING           value;
  int              length;

  value = TtaAllocString (buflen);
  value[0] = WC_EOS;
  length = TtaGetTextAttributeLength (event->attribute);
  if (length >= buflen)
     length = buflen - 1;
  if (length > 0)
     TtaGiveTextAttributeValue (event->attribute, value, &length);  
  HTMLSetForegroundColor (event->document, event->element, value);
  TtaFreeMemory (value);
}


/*----------------------------------------------------------------------
 MathAttrColorDelete
 The user is deleting an attribute color.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool MathAttrColorDelete (NotifyAttribute *event)
#else /* __STDC__*/
ThotBool MathAttrColorDelete(event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
  HTMLResetForegroundColor (event->document, event->element);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
 MathAttrBackgroundCreated
 An attribute background has been created or modified by the user.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void MathAttrBackgroundCreated (NotifyAttribute *event)
#else /* __STDC__*/
void MathAttrBackgroundCreated (event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
  STRING           value;
  int              length;

  value = TtaAllocString (buflen);
  value[0] = WC_EOS;
  length = TtaGetTextAttributeLength (event->attribute);
  if (length >= buflen)
     length = buflen - 1;
  if (length > 0)
     TtaGiveTextAttributeValue (event->attribute, value, &length);  
  HTMLSetBackgroundColor (event->document, event->element, value);
  TtaFreeMemory (value);
}


/*----------------------------------------------------------------------
 MathAttrBackgroundDelete
 The user is deleting an attribute background.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool MathAttrBackgroundDelete (NotifyAttribute *event)
#else /* __STDC__*/
ThotBool MathAttrBackgroundDelete(event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
  HTMLResetBackgroundColor (event->document, event->element);
  return FALSE; /* let Thot perform normal operation */
}
 
/*----------------------------------------------------------------------
 AttrStretchyChanged
 Attribute stretchy in a MO element has been modified or deleted
 by the user.
 Change the Thot leaf child accordingly.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void AttrStretchyChanged (NotifyAttribute *event)
#else /* __STDC__*/
void AttrStretchyChanged (event)
     NotifyAttribute *event;
#endif /* __STDC__*/
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
 AttrBevelledChanged
 Attribute bevelled in a mfrac element has been modified or deleted
 by the user.
 Change the type of the element (MFRAC/BevelledMFRAC) accordingly.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void AttrBevelledChanged (NotifyAttribute *event)
#else /* __STDC__*/
void AttrBevelledChanged (event)
     NotifyAttribute *event;
#endif /* __STDC__*/
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
 AttrNameChanged
 Attribute name in a MCHAR element has been modified by the user.
 Change the content of the Thot leaf child accordingly.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void AttrNameChanged (NotifyAttribute *event)
#else /* __STDC__*/
void AttrNameChanged (event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
  if (event->element)
    SetMcharContent (event->element, event->document);
}

/*----------------------------------------------------------------------
 AttrOpenCloseChanged
 Attribute open or close in a MFENCED element has been modified or deleted
 by the user. Update the corresponding fence element.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void AttrOpenCloseChanged (NotifyAttribute *event)
#else /* __STDC__*/
void AttrOpenCloseChanged (event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
  Element	fence, content;
  int		length;
  CHAR_T    text[8];

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
	   text[0] = TEXT('(');	/* default value for open */
	else
	   text[0] = TEXT(')');	/* default value for close */
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
#ifdef __STDC__
void FencedSeparatorModified (NotifyOnTarget *event)
#else /* __STDC__*/
void FencedSeparatorModified(event)
     NotifyOnTarget *event;
#endif /* __STDC__*/
{
  Element	mfencedEl, fencedExpEl, child, content;
  Attribute	attr;
  ElementType	elType;
  AttributeType	attrType;
  int		i, len;
  Language	lang;
  UCHAR_T         text[32];

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
     if (elType.ElTypeNum == MathML_EL_FencedSeparator)
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
#ifdef __STDC__
void AttrSeparatorsChanged (NotifyAttribute *event)
#else /* __STDC__*/
void AttrSeparatorsChanged (event)
     NotifyAttribute *event;
#endif /* __STDC__*/
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
    if (elType.ElTypeNum == MathML_EL_FencedExpression)
       fencedExpression = child;
    else
       TtaNextSibling (&child);
    }
  while (fencedExpression == NULL && child != NULL);
  if (fencedExpression != NULL)
    RegenerateFencedSeparators (fencedExpression, event->document, FALSE/****/);
}
