#ifdef MATHML
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

#include "html2thot_f.h"
#ifdef __STDC__
extern boolean ElementNeedsPlaceholder (Element el);
#else
extern boolean ElementNeedsPlaceholder ();
#endif

#include "MathML.h"
#ifndef _WINDOWS
#include "Math.xpm"
#include "math.xpm"
#include "mathdisp.xpm"
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
#include "greek.xpm"
#endif /* _WINDOWS */
#define FormMaths 0
#define MenuMaths 1
#define MAX_MATHS  2

static Pixmap	iconMath;
static Pixmap	mIcons[14];
static int	MathsDialogue;
static boolean	InitMaths;
static boolean	IsLastDeletedElement = FALSE;
static Element	LastDeletedElement = NULL;

#ifdef _WINDOWS
#define iconMath 21 
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
   SplitTextInMathML
   Split element el and the enclosing element (MO, MI, MN or MTEXT).
   Parameter index indicates the position where the text has to be split.
   Return the next created text within the next enclosing element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element      SplitTextInMathML (Document doc, Element el, int index)
#else
static Element      SplitTextInMathML (doc, el, index)
Document            doc;
Element             el;
int                 index;
#endif
{
  Element            added, parent, row;
  ElementType        elType;

  /* do not check the Thot abstract tree against the structure */
  /* schema while changing the structure */
  TtaSetStructureChecking (0, doc);
  /* split the text to be inserted */
  TtaSplitText (el, index-1, doc);
  /* get the parent element (MO, MN, MI or MTEXT) */
  parent = TtaGetParent (el);

  /* check whether the parent is a child of a MROW */
  row = TtaGetParent (parent);
  elType = TtaGetElementType (row);
  if (elType.ElTypeNum != MathML_EL_MROW &&
      elType.ElTypeNum != MathML_EL_FencedExpression)
    {
      /* generates a new MROW element */
      elType.ElTypeNum = MathML_EL_MROW;
      row = TtaNewElement (doc, elType);
      TtaInsertSibling (row, parent, TRUE, doc);
      TtaRemoveTree (parent, doc);
      /* move the parent into the new MROW */
      TtaInsertFirstChild (&parent, row, doc);
    }

  /* duplicate the parent element (MO, MN, MI or MTEXT) */
  elType = TtaGetElementType (parent);
  added = TtaNewElement (doc, elType);
  TtaInsertSibling (added, parent, FALSE, doc);
  /* take the second part of the split text */
  TtaNextSibling (&el);
  TtaRemoveTree (el, doc);
  /* move the old element into the new MROW */
  TtaInsertFirstChild (&el, added, doc);
  /* check the Thot abstract tree against the structure schema. */
  TtaSetStructureChecking (1, doc);
  return (el);
}

/*----------------------------------------------------------------------
  DeleteIfPlaceholder
  
  Delete element el if it's a placeholder.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void       DeleteIfPlaceholder (Element* el, Document doc)
#else
static void       DeleteIfPlaceholder (el, doc)
Element* el;
Document doc;

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
        attrType.AttrTypeNum = MathML_ATTR_placeholder;
	attr = TtaGetAttribute (*el, attrType);
	if (attr != NULL)
	   /* this element is a placeholder. Delete it */
	   {
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
static Element       InsertPlaceholder (Element el, boolean before, Document doc)
#else
static Element       InsertPlaceholder (el, before, doc)
Element el;
boolean before;
Document doc;

#endif
{
Element		sibling, placeholderEl;
ElementType	elType;
Attribute	attr;
AttributeType	attrType;
boolean		createConstruct, oldStructureChecking;

     placeholderEl = NULL;

     if (!ElementNeedsPlaceholder (el))
	/* this element does not need placeholders.  Delete its previous
	   and next siblings if they are place holders */
	{
	sibling = el;
	TtaPreviousSibling (&sibling);
	DeleteIfPlaceholder (&sibling, doc);
	sibling = el;
	TtaNextSibling (&sibling);
	DeleteIfPlaceholder (&sibling, doc);
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
           attrType.AttrTypeNum = MathML_ATTR_placeholder;
           attr = TtaNewAttribute (attrType);
           TtaAttachAttribute (placeholderEl, attr, doc);
           TtaSetAttributeValue (attr, MathML_ATTR_placeholder_VAL_yes_,
				 placeholderEl, doc);
	   }
	}
     return placeholderEl;
}

/*----------------------------------------------------------------------
  CreateParentMROW
  If element el is not a child of a MROW and if it has at least one
  sibling that is not a Construct, create an enclosing MROW,
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
  Element            sibling, row, parent, firstChild, next, previous;
  ElementType        elType;
  int                nChildren;

  /* check whether the parent is a row */
  parent = TtaGetParent (el);
  if (parent == NULL)
     return;
  elType = TtaGetElementType (parent);
  if (elType.ElTypeNum != MathML_EL_MROW &&
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
	      /* generates a new row element to include these elements */
	      elType.ElTypeNum = MathML_EL_MROW;
	      row = TtaNewElement (doc, elType);
	      TtaInsertSibling (row, firstChild, TRUE, doc);
	      sibling = firstChild;
	      previous = NULL;
	      while (sibling != NULL)
		{
		next = sibling;
		TtaNextSibling (&next);
	        TtaRemoveTree (sibling, doc);
	        /* move the old element into the new MROW */
		if (previous == NULL)
	           TtaInsertFirstChild (&sibling, row, doc);
		else
		   TtaInsertSibling (sibling, previous, FALSE, doc);
		previous = sibling;
		sibling = next;
		}
	    }
	 }
}


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
   RegenerateFencedSeparators
   el must be a FencedExpression element.
   Delete all existing FencedSeparator elements in el and create new
   ones according to the value of attribute separators of parent MFENCED.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void RegenerateFencedSeparators (Element el, Document doc)
#else /* __STDC__*/
static void RegenerateFencedSeparators (el, doc)
     Element el;
     Document doc;
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
	TtaDeleteTree (child, doc);
     child = next;
     }
   /* Create all FencedSeparator elements, according to attribute separators */
   CreateFencedSeparators (el, doc);
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
  Element            sibling, last, el, row, fence, symbol, child, leaf,
		     placeholderEl, parent;
  ElementType        newType, elType, symbType;
  SSchema            docSchema, mathSchema;
  int                c1, c2, i, j, len;
  boolean	     before, ParBlock, surround, insertSibling,
		     selectFirstChild;

      doc = TtaGetSelectedDocument ();
      TtaGiveLastSelectedElement (doc, &last, &c2, &j);
      TtaGiveFirstSelectedElement (doc, &sibling, &c1, &i); 
    
      /* Get the type of the first selected element */
      elType = TtaGetElementType (sibling);
      docSchema = TtaGetDocumentSSchema (doc);

      if (construct == 0 || construct == 1)
	/* button Math or Display Math */
	{
	/* cannot create a Math element within a MathML element */
	if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "MathML") != 0)
	   /* not within a MathML element */
	   {
           if (construct == 0)
              newType.ElTypeNum = HTML_EL_Math;
           else
              newType.ElTypeNum = HTML_EL_MathDisp;
           newType.ElSSchema = docSchema;
           TtaCreateElement (newType, doc);
	   }
	return;
	}

      surround = (last != sibling || 
		  (c1 < i) || 
		  (c1 == 0 && i == 0 && (TtaGetElementVolume (sibling) != 0))
		 );
      
      TtaSetDisplayMode (doc, DeferredDisplay);

      /* the new element will be inserted before the selected element */
      before = TRUE;

      /* Check whether the selected element is a MathML element */
      if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
	{
	  /* the selection concerns a MathML element */
	  mathSchema = elType.ElSSchema;
	  if (elType.ElTypeNum == MathML_EL_TEXT_UNIT && c1 > 1)
	    {
	      len = TtaGetTextLength (sibling);
	      if (c1 > len)
		/* the caret is at the end of that character string */
		/* create the new element after the character string */
		before = FALSE;
	      else
		sibling = SplitTextInMathML (doc, sibling, c1);
	    }
	}
      else
	{
	  /* the selection concerns an HTML element */
	  mathSchema = TtaNewNature (docSchema, "MathML", "MathMLP");
	  if (elType.ElTypeNum != HTML_EL_Math &&
	      elType.ElTypeNum != HTML_EL_MathDisp)
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
		        el = TtaNewTree (doc, elType, "");
		        TtaInsertSibling (el, sibling, FALSE, doc);
			}
		      }
		  else
		    {
		      /* split the text to insert the Math element */
		      TtaSplitText (sibling, c1-1, doc);
		      /* take the second part of the split text element */
		      TtaNextSibling (&sibling);
		    }
		}

	      if (before)
		{
		  el = sibling;
		  TtaPreviousSibling (&el);
		  if (el != NULL)
		    {
		      newType = TtaGetElementType (el);
		      if (newType.ElTypeNum == HTML_EL_Math ||
			  newType.ElTypeNum == HTML_EL_MathDisp)
			{
			  /* insert at the end of the previous MathML element */
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
		      if (newType.ElTypeNum == HTML_EL_Math ||
			  newType.ElTypeNum == HTML_EL_MathDisp)
			{
			  /* insert at the beginning of the next MathML element */
			  before = TRUE;
			  sibling = TtaGetFirstChild (el);		      
			}
		    }
		}
	    }

	  if (elType.ElTypeNum == HTML_EL_Math ||
	      elType.ElTypeNum == HTML_EL_MathDisp)
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
	      surround = FALSE;
	      insertSibling = TRUE;
	      /* try to create a Math or MathDisp element at the current
		 position */
	      elType.ElTypeNum = HTML_EL_Math;
	      if (TtaCanInsertSibling (elType, sibling, before, doc))
		 /* create a new Math element as a sibling */
	         insertSibling = TRUE;
	      else if (TtaCanInsertFirstChild (elType, sibling, doc))
		 /* create a new Math element as a child */
	         insertSibling = FALSE;
	      else
                 /* cannot insert a Math element here. Try to create a MathDisp
                    element */
                 {
                 elType.ElTypeNum = HTML_EL_MathDisp;
                 if (TtaCanInsertSibling (elType, sibling, before, doc))
                    /* insert the new Math element as a sibling element */
                    insertSibling = TRUE;
                 else if (TtaCanInsertFirstChild (elType, sibling, doc))
                    /* insert the new Math element as a child element */
                    insertSibling = FALSE;
                 else
                    /* cannot insert any element here */
                    sibling = NULL;
                 }
              if (sibling == NULL)
                 {
                 TtaSetDisplayMode (doc, DisplayImmediately);
                 return;
                 }
              else
                 {
                 el = TtaNewTree (doc, elType, "");
                 if (insertSibling)
                    /* insert the new Math element as a sibling element */
                    TtaInsertSibling (el, sibling, before, doc);
                 else
                    /* insert the new Math element as a child element */
                    TtaInsertFirstChild (&el, sibling, doc);
                 sibling = TtaGetFirstChild (el);
                 }
	    }
	}

      elType = TtaGetElementType (sibling);
      newType.ElSSchema = mathSchema;
      selectFirstChild = TRUE;
      ParBlock = FALSE;
      switch (construct)
	{
	case 0:	/* create a Math element */
	  /* handled above */
	  return;
	  break;
	case 1:	/* create a MathDisp element */
	  /* handled above */
	  return;
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
	default:
	  return;
	}
      if (!surround || !TransformIntoType (newType, doc))
	{
	  TtaUnselect (doc);

          el = TtaNewTree (doc, newType, "");

	  /* do not check the Thot abstract tree against the structure */
	  /* schema while changing the structure */
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
		  TtaRemoveTree (row, doc);
		}
	      else
		{
		  /* check whether the element is a Construct */
		  elType = TtaGetElementType (sibling);
		  if (elType.ElTypeNum == MathML_EL_Construct)
		    {
		    TtaInsertFirstChild (&el, sibling, doc);
		    RemoveAttr (el, doc, MathML_ATTR_placeholder);
		    }
		  else
		    TtaInsertSibling (el, sibling, before, doc);
		}
	    }
	  else if (elType.ElTypeNum == MathML_EL_Construct)
	    {
	      /* replace the Construct element */
	      TtaInsertFirstChild (&el, sibling, doc);
	      RemoveAttr (el, doc, MathML_ATTR_placeholder);
	    }
	  else
	    {
	      /* the selected element is not a MROW */
	      if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
		/* go up to the MN, MI, MO or M_TEXT element */
		sibling = TtaGetParent (sibling);
	      /* insert the new element */
	      TtaInsertSibling (el, sibling, before, doc);
	    }
	  
	  if (ParBlock)
	    /* user wants to create a parenthesized block */
	    /* create two MF elements, as the first and last child of the new
	       MROW */
	    {
	      child = TtaGetFirstChild (el);
	      if (child != NULL)
		{
		  newType.ElTypeNum = MathML_EL_MF;
		  fence = TtaNewElement (doc, newType);
		  TtaInsertSibling (fence, child, TRUE, doc);
		  symbType.ElSSchema = mathSchema;
		  symbType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
		  symbol = TtaNewElement (doc, symbType);
		  TtaSetGraphicsShape (symbol, '(', doc);
		  TtaInsertFirstChild (&symbol, fence, doc);

		  fence = TtaNewElement (doc, newType);
		  TtaInsertSibling (fence, child, FALSE, doc);
		  symbol = TtaNewElement (doc, symbType);
		  TtaSetGraphicsShape (symbol, ')', doc);
		  TtaInsertFirstChild (&symbol, fence, doc);
		}
	    }
	  
	  CreateParentMROW (el, doc);

	  /* if the new element is a child of a FencedExpression element,
	     create the associated FencedSeparator elements */
	  parent = TtaGetParent (el);
	  elType = TtaGetElementType (parent);
	  if (elType.ElTypeNum == MathML_EL_FencedExpression)
	     RegenerateFencedSeparators (parent, doc);

	 /* insert placeholders before and/or after the new element if
	    they are needed */
	  placeholderEl = InsertPlaceholder (el, TRUE, doc);
	  placeholderEl = InsertPlaceholder (el, FALSE, doc);

	  TtaSetDisplayMode (doc, DisplayImmediately);
	  /* check the Thot abstract tree against the structure schema. */
	  TtaSetStructureChecking (1, doc);
	  
	  /* selected the leaf in the first (or second) child of the new
	     element */
	  child = TtaGetFirstChild (el);
	  if (!selectFirstChild)
	     /* get the seconf child */
	     TtaNextSibling (&child);
	  while (child != NULL)
	    {
	      leaf = child;
	      child = TtaGetFirstChild (child);
	    }
	  TtaSelectElement (doc, leaf);
	}
}

/*----------------------------------------------------------------------
   CallbackMaths: manage Maths dialogue events.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackMaths (int ref, int typedata, char *data)
#else
void                CallbackMaths (ref, typedata, data)
int                 ref;
int                 typedata;
char               *data;

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
        CreateMathConstruct ((int) data);
      break;

    default:
      break;
    }
}

/*----------------------------------------------------------------------
   CreateMaths creates the maths menus.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateMaths (Document doc, View view)
#else
void                CreateMaths (doc, view)
Document            doc;
View                view;
#endif
{
# ifndef _WINDOWS
  if (!InitMaths)
    {
      InitMaths = TRUE;

      /* Dialogue form for answering text, user name and password */
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
  CreateMathDlgWindow (TtaGetViewFrame (doc, view), MathsDialogue, TtaGetThotWindow (GetWindowNumber (doc, view)));
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
# ifndef _WINDOWS
  TtaAddButton (doc, 1, iconMath, CreateMaths, TtaGetMessage (AMAYA, AM_BUTTON_MATH));
# else  /* _WINDOWS */
  WIN_TtaAddButton (doc, 1, iconMath, CreateMaths, TtaGetMessage (AMAYA, AM_BUTTON_MATH), TBSTYLE_BUTTON, TBSTATE_ENABLED);
# endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  CreateInlineMath
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateInlineMath (Document document, View view)
#else  /* __STDC__ */
void                CreateInlineMath (document, view)
Document            document;
View                view;
 
#endif /* __STDC__ */
{
   CreateMathConstruct (0);
}

/*----------------------------------------------------------------------
  CreateDisplayMath
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateDisplayMath (Document document, View view)
#else  /* __STDC__ */
void                CreateDisplayMath (document, view)
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
   InitMathML initializes MathML context.           
  ----------------------------------------------------------------------*/
void                InitMathML ()
{
#  ifndef _WINDOWS 
   iconMath = TtaCreatePixmapLogo (Math_xpm);
   TtaRegisterPixmap("Math", iconMath);
   mIcons[0] = TtaCreatePixmapLogo (math_xpm);
   mIcons[1] = TtaCreatePixmapLogo (mathdisp_xpm);
   mIcons[2] = TtaCreatePixmapLogo (root_xpm);
   mIcons[3] = TtaCreatePixmapLogo (sqrt_xpm);
   mIcons[4] = TtaCreatePixmapLogo (frac_xpm);
   mIcons[5] = TtaCreatePixmapLogo (subsup_xpm);
   mIcons[6] = TtaCreatePixmapLogo (sub_xpm);
   mIcons[7] = TtaCreatePixmapLogo (sup_xpm);
   mIcons[8] = TtaCreatePixmapLogo (overunder_xpm);
   mIcons[9] = TtaCreatePixmapLogo (under_xpm);
   mIcons[10] = TtaCreatePixmapLogo (over_xpm);
   mIcons[11] = TtaCreatePixmapLogo (fence_xpm);
   mIcons[12] = TtaCreatePixmapLogo (mscript_xpm);
   mIcons[13] = TtaCreatePixmapLogo (greek_xpm);
#  endif /* _WINDOWS */
  MathsDialogue = TtaSetCallback (CallbackMaths, MAX_MATHS);
  KeyboardsLoadResources ();
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
	   attrType.AttrTypeNum = MathML_ATTR_placeholder;
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
       TtaSetStructureChecking (0, doc);
       child = firstChild;
       while (child != NULL)
	  {
	  next = child;
	  TtaNextSibling (&next);
          TtaRemoveTree (child, doc);
          TtaInsertSibling (child, *el, TRUE, doc);
	  child = next;
	  }
       TtaDeleteTree (*el, doc);
       *el = NULL;
       TtaSetStructureChecking (1, doc);
       TtaSetDisplayMode (doc, DisplayImmediately);
       }
     }
}


/*----------------------------------------------------------------------
   GetCharType
   returns the type of character c (MN, MI or MO).
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static int GetCharType (unsigned char c, char alphabet)
#else /* __STDC__*/
static int GetCharType (c, alphabet)
     unsigned char c;
     char	   alphabet;
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
  if (elType.ElTypeNum == MathML_EL_MI)
     /* is it really an identifier? */
     {
     /* try to separate function names, identifiers and plain text */
     /****** TO DO ******/
     }
  if (elType.ElTypeNum == MathML_EL_MO)
     {
     SetAddspaceAttr (el, doc);
     parent = TtaGetParent (el);
     if (parent != NULL)
	{
	parentType = TtaGetElementType (parent);
	if (parentType.ElTypeNum != MathML_EL_Base &&
	    parentType.ElTypeNum != MathML_EL_UnderOverBase)
	   SetVertStretchAttr (el, doc, 0, selEl);
	else
	   {
	   grandParent = TtaGetParent (parent);
	   if (grandParent != NULL)
	      SetVertStretchAttr (grandParent, doc, parentType.ElTypeNum, selEl);
	   }
	}
     }
  else
     RemoveAttr (el, doc, MathML_ATTR_addspace);
  if (elType.ElTypeNum == MathML_EL_MI)
     SetFontslantAttr (el, doc);
  else
     RemoveAttr (el, doc, MathML_ATTR_IntFontslant);		
}

/*----------------------------------------------------------------------
   MergeMathEl
   merge element el2 with element el
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void MergeMathEl (Element el, Element el2, boolean before, Document doc)
#else /* __STDC__*/
static void MergeMathEl (el, el2, before, doc)
     Element el;
     Element el2;
     boolean before;
     Document doc;
#endif /* __STDC__*/
{
  Element	textEl2, nextEl, prevEl;

  textEl2 = TtaGetFirstChild (el2);
  if (before)
     prevEl = NULL;
  else
     prevEl = TtaGetLastChild (el);
  while (textEl2 != NULL)
     {
     nextEl = textEl2;
     TtaNextSibling (&nextEl);
     TtaRemoveTree (textEl2, doc);
     if (prevEl == NULL)
	TtaInsertFirstChild (&textEl2, el, doc);
     else
        TtaInsertSibling (textEl2, prevEl, FALSE, doc);
     prevEl = textEl2;
     textEl2 = nextEl;
     }
  TtaDeleteTree (el2, doc);
  MathSetAttributes (el, doc, NULL);
}

/*----------------------------------------------------------------------
   TextLength
   return the total length of the text contained in element el.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static int TextLength (Element el)
#else /* __STDC__*/
static int TextLength (el)
     Element el;
#endif /* __STDC__*/
{
  int len;
  Element child;

  len = 0;
  child = TtaGetFirstChild (el);
  while (child != NULL)
    {
    len += TtaGetTextLength (child);
    TtaNextSibling (&child);
    }
  return len;
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
   ParseMathString
   The content of an element MTEXT, MI, MO, or MN, has been modified
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
  SSchema	MathMLSchema;
  int		firstSelChar, lastSelChar, newSelChar, len, totLen, i, j,
		start;
  char		alphabet, c;
  Language	lang;
#define TXTBUFLEN 200
  unsigned char text[TXTBUFLEN];
  char		language[TXTBUFLEN];
  unsigned char	mathType[TXTBUFLEN];

  /* get the current selection */
  TtaGiveFirstSelectedElement (doc, &selEl, &firstSelChar, &lastSelChar);
  newSelEl = NULL;

  elType = TtaGetElementType (theElem);
  MathMLSchema = elType.ElSSchema;

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
	 elType.ElTypeNum == MathML_EL_MN ||
	 elType.ElTypeNum == MathML_EL_MS))
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
	 elType.ElTypeNum == MathML_EL_MN ||
	 elType.ElTypeNum == MathML_EL_MS))
	nextEl = el;
     }

  i = 0;
  totLen = 0;
  elType = TtaGetElementType (theElem);
  textEl = theText;
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
	     mathType[i+j] = (unsigned char) GetCharType (text[i+j], alphabet);
	     }
	  i+= len;
	  totLen += len;
	  }
       }

  /* try to identify numbers like: 0.123  1,000,000  2.1e10 */
  for (i = 1; i < totLen; i++)
    {
    if ((text[i] == ',' || text[i] == '.' || text[i] == 'e') &&
         mathType[i-1] == MathML_EL_MN &&
	 i < totLen-1 &&mathType[i+1] == MathML_EL_MN)
	/* comma or point between two digits: the comma or point is part of
	   the number */
	  {
	  mathType[i] = (unsigned char) MathML_EL_MN;
	  i++;
	  }
    }

  TtaSetDisplayMode (doc, DeferredDisplay);
  TtaSetStructureChecking (0, doc);
  firstEl = NULL;
  start = 0;
  lastEl = NULL;
  if (totLen == 0)
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
	  TtaDeleteTree (theElem, doc);
	  theElem = NULL;

	  /* The deletion of the parent element may require a Placeholder
	     instead of the deleted element */
	  placeholderEl = NULL;
	  if (prev != NULL)
	     placeholderEl = InsertPlaceholder (prev, FALSE, doc);
	  else if (next != NULL)
	     placeholderEl = InsertPlaceholder (next, TRUE, doc);
	  if (placeholderEl != NULL)
	     newSelEl = placeholderEl;   

	  /* if the deleted element is a child of a FencedExpression element,
	     upate the associated FencedSeparator elements */
	  elType = TtaGetElementType (parent);
	  if (elType.ElTypeNum == MathML_EL_FencedExpression)
	     RegenerateFencedSeparators (parent, doc);

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
	 mathType[i-1] == MathML_EL_MO ||
	 i == totLen)
       /* create a new element */
       {
       if (lastEl == NULL)
	  {
	  elType = TtaGetElementType (theElem);
	  if (elType.ElTypeNum != mathType[i-1] ||
	      mathType[i-1] == MathML_EL_MO)
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
		TtaRemoveTree (textEl, doc);
		TtaInsertFirstChild (&textEl, newEl, doc);
		while (prev != NULL)
		   {
		   next = textEl;
		   textEl = prev;
		   TtaPreviousSibling (&prev);
		   TtaRemoveTree (textEl, doc);
		   TtaInsertSibling (textEl, next, TRUE, doc);
		   }
		MathSetAttributes (newEl, doc, &newSelEl);
		}
	     ChangeTypeOfElement (theElem, doc, mathType[i-1]);
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
	     TtaRemoveTree (textEl, doc);
	     TtaInsertFirstChild (&textEl, newEl, doc);
	     while (next != NULL)
		{
	        prev = textEl;
		textEl = next;
		TtaNextSibling (&next);
		TtaRemoveTree (textEl, doc);
		TtaInsertSibling (textEl, prev, FALSE, doc);
		}
	     MathSetAttributes (newEl, doc, &newSelEl);
	     }
	  newEl = theElem;
	  textEl = theText;
	  firstEl = theElem;
	  }
       else
	  {
          elType.ElTypeNum = mathType[i-1];
          newEl = TtaNewElement (doc, elType);
	  TtaInsertSibling (newEl, lastEl, FALSE, doc);
	  elType.ElTypeNum = MathML_EL_TEXT_UNIT;
	  textEl = TtaNewElement (doc, elType);
	  TtaInsertFirstChild (&textEl, newEl, doc);
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

       if (mathType[i-1] == MathML_EL_MO)
	  /* the new element is an operator */
	  {
	  /* if the new element contains a single SYMBOL, placeholders may
	     be needed before and/or after that operator */
	  placeholderEl = InsertPlaceholder (newEl, TRUE, doc);
	  placeholderEl = InsertPlaceholder (newEl, FALSE, doc);
	  /* the new contents may be an horizontally stretchable symbol */
	  if (newEl != NULL)
	    {
	    parent = TtaGetParent (newEl);
	    elType = TtaGetElementType (parent);
	    if (elType.ElTypeNum == MathML_EL_UnderOverBase ||
		elType.ElTypeNum == MathML_EL_Underscript ||
		elType.ElTypeNum == MathML_EL_Overscript)
	       SetSingleHorizStretchAttr (parent, doc, &newSelEl);
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
         {
         if (newSelEl == prevEl)
	    newSelEl = firstEl;
         else if (newSelEl == firstEl)
	    newSelChar += TextLength (prevEl);
         MergeMathEl (firstEl, prevEl, TRUE, doc);
	 }
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
         {
         if (newSelEl == nextEl)
	    {
	    newSelEl = lastEl;
	    newSelChar += TextLength (lastEl);
	    }
         MergeMathEl (lastEl, nextEl, FALSE, doc);
	 }
       }
    }

  if (firstEl != NULL)
     {
     /* if we are in a FencedExpression element, upate the associated
	FencedSeparator elements */
     parent = TtaGetParent (firstEl);
     elType = TtaGetElementType (parent);
     if (elType.ElTypeNum == MathML_EL_FencedExpression)
	 RegenerateFencedSeparators (parent, doc);

     /* Create a MROW element that encompasses the new elements if necessary */
     CreateParentMROW (firstEl, doc);
     }

  TtaSetStructureChecking (1, doc);
  TtaSetDisplayMode (doc, DisplayImmediately);

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
   ElementType	elType;

   TtaSetStructureChecking (0, event->document);

   /* if the new element is a child of a FencedExpression element,
      create the associated FencedSeparator elements */
   parent = TtaGetParent (event->element);
   elType = TtaGetElementType (parent);
   if (elType.ElTypeNum == MathML_EL_FencedExpression)
      RegenerateFencedSeparators (parent, event->document);

   /* create placeholders before and/or after the new element */
   placeholderEl = InsertPlaceholder (event->element, TRUE, event->document);
   placeholderEl = InsertPlaceholder (event->element, FALSE, event->document);

   TtaSetStructureChecking (1, event->document);
}


/*----------------------------------------------------------------------
 MathElementWillBeDeleted
 This function is called by the DELETE command for each selected element
 and for all their descendants.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
boolean MathElementWillBeDeleted (NotifyElement *event)
#else /* __STDC__*/
boolean MathElementWillBeDeleted(event)
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

   parent = event->element; /* parent of the deleted element */
   parentType = TtaGetElementType (parent);

   if (parentType.ElTypeNum == MathML_EL_FencedExpression)
      /* a child of a FencedExpression element has been deleted,
         re-generate all FencedSeparator elements in that FencedExpression */
      RegenerateFencedSeparators (parent, event->document);

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
        placeholderEl = InsertPlaceholder (sibling, TRUE, event->document);
      }
   else if (IsLastDeletedElement)
      {
      for (i = 1; i < event->position && sibling != NULL; i++)
         TtaNextSibling (&sibling);
      if (sibling != NULL)
         placeholderEl = InsertPlaceholder (sibling, FALSE, event->document);
      }
   IsLastDeletedElement = FALSE;
   LastDeletedElement = NULL;

   /* If there is an enclosing MROW that is no longer needed, remove
      that MROW */
   CheckMROW (&parent, event->document);

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
	/* remove the enclosing MFRAC */
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
	   placeholderEl = InsertPlaceholder (grandChild, FALSE, event->document);
	   placeholderEl = InsertPlaceholder (grandChild, TRUE, event->document);
	   }
	}
      }
   TtaSetStructureChecking (1, event->document);
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
  unsigned char	text[2];

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
  unsigned char	text[8];

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
      TtaSetGraphicsShape (content, text[0], event->document);
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
  unsigned char	text[32];

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
     RegenerateFencedSeparators (fencedExpression, event->document);
}

#endif /* MATHML */
