#ifdef MATHML
/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Initialization functions and button functions of Amaya application.
 *
 * Author: I. Vatton
 */

/* Included headerfiles */
#define THOT_EXPORT
#include "amaya.h"
#include "css.h"
#include "trans.h"

#include "html2thot_f.h"

#include "MathML.h"
#include "Math.xpm"
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
#define FormMaths 0
#define MenuMaths 1
#define MAX_MATHS  2

static Pixmap       iconMath;
static Pixmap       mIcons[12];
static int          MathsDialogue;
static boolean      InitMaths;


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
  Element            new, parent, row;
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
  if (elType.ElTypeNum != MathML_EL_MROW)
    {
      /* generates a new row element */
      elType.ElTypeNum = MathML_EL_MROW;
      row = TtaNewElement (doc, elType);
      TtaInsertSibling (row, parent, TRUE, doc);
      TtaRemoveTree (parent, doc);
      /* move the parent into the new MROW */
      TtaInsertFirstChild (&parent, row, doc);
    }

  /* duplicate the parent element (MO, MN, MI or MTEXT) */
  elType = TtaGetElementType (parent);
  new = TtaNewElement (doc, elType);
  TtaInsertSibling (new, parent, FALSE, doc);
  /* take the second part of the split text */
  TtaNextSibling (&el);
  TtaRemoveTree (el, doc);
  /* move the old element into the new MROW */
  TtaInsertFirstChild (&el, new, doc);
  /* check the Thot abstract tree against the structure schema. */
  TtaSetStructureChecking (1, doc);
  return (el);
}

/*----------------------------------------------------------------------
   InsertPlaceholder
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void          InsertPlaceholder (Element el, boolean before, Document doc)
#else
static void          InsertPlaceholder (el, before, doc)
Element el;
boolean before;
Document doc;

#endif
{
ElementType	elType;
Element		sibling;
boolean		createConstruct;

     elType = TtaGetElementType (el);
     if (elType.ElTypeNum == MathML_EL_MROW ||
	 elType.ElTypeNum == MathML_EL_MROOT ||
	 elType.ElTypeNum == MathML_EL_MSQRT ||
	 elType.ElTypeNum == MathML_EL_MFRAC ||
	 elType.ElTypeNum == MathML_EL_MSUBSUP ||
	 elType.ElTypeNum == MathML_EL_MSUB ||
	 elType.ElTypeNum == MathML_EL_MSUP ||
	 elType.ElTypeNum == MathML_EL_MUNDER ||
	 elType.ElTypeNum == MathML_EL_MOVER ||
	 elType.ElTypeNum == MathML_EL_MUNDEROVER ||
	 elType.ElTypeNum == MathML_EL_MMULTISCRIPTS)
        /* this element accepts a Construct as its neighbour */
	{
	sibling = el;
	if (before)
	   TtaPreviousSibling (&sibling);
	else
	TtaNextSibling (&sibling);
	createConstruct = TRUE;
	if (sibling != NULL)
	if (!ElementNeedsPlaceholder (sibling))
	   createConstruct = FALSE;
	if (createConstruct)
	   {
	   elType.ElTypeNum = MathML_EL_Construct;
	   sibling = TtaNewElement (doc, elType);
	   TtaInsertSibling (sibling, el, before, doc);
	   }
	}
}

/*----------------------------------------------------------------------
  CreateParentMROW
  If element el is not a child of a MROW and if it has at least one
  sibling that is not a Construct, create an enclosing MROW.
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
  if (elType.ElTypeNum != MathML_EL_MROW)
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
  Element            sibling, last, el, row, fence, child;
  ElementType        newType, elType;
  SSchema            docSchema, mathSchema;
  int                val, c1, c2, i, j, len;
  boolean	     before, ParBlock, surround, insertSibling;

  val = (int) data;
  ParBlock = FALSE;
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
      if (val == 11)
	{
	  TtcDisplayGreekKeyboard (doc, 1);
	  return;
	}
      else if (doc == 0)
	/* no document selected */
	return;
      /* the new element will be inserted before the selected element */
      before = TRUE;
      TtaGiveLastSelectedElement (doc, &last, &c2, &j);
      TtaGiveFirstSelectedElement (doc, &sibling, &c1, &i); 
    
      /* Check whether the selected element is a text element */
      elType = TtaGetElementType (sibling);
      surround = (last != sibling || 
		  (c1 < i) || 
		  (c1 == 0 && i == 0 && (TtaGetElementVolume (sibling) != 0))
		 );

      
      TtaSetDisplayMode (doc, DeferredDisplay);

      /* Check whether the selected element is a MathML element */
      docSchema = TtaGetDocumentSSchema (doc);
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
      switch (val)
	{
	case 0:
	  newType.ElTypeNum = MathML_EL_MROOT;
	  break;
	case 1:
	  newType.ElTypeNum = MathML_EL_MSQRT;
	  break;
	case 2:
	  newType.ElTypeNum = MathML_EL_MFRAC;
	  break;
	case 3:
	  newType.ElTypeNum = MathML_EL_MSUBSUP;
	  break;
	case 4:
	  newType.ElTypeNum = MathML_EL_MSUP;
	  break;
	case 5:
	  newType.ElTypeNum = MathML_EL_MSUB;
	  break;
	case 6:
	  newType.ElTypeNum = MathML_EL_MUNDEROVER;
	  break;
	case 7:
	  newType.ElTypeNum = MathML_EL_MOVER;
	  break;
	case 8:
	  newType.ElTypeNum = MathML_EL_MUNDER;
	  break;
	case 9:
	  newType.ElTypeNum = MathML_EL_MROW;
	  ParBlock = TRUE;
	  break;
	case 10:
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
		  /* check whether the element is a construction */
		  elType = TtaGetElementType (sibling);
		  if (elType.ElTypeNum == MathML_EL_Construct)
		    TtaInsertFirstChild (&el, sibling, doc);
		  else
		    TtaInsertSibling (el, sibling, before, doc);
		}
	    }
	  else if (elType.ElTypeNum == MathML_EL_Construct)
	    {
	      /* replace the construction choice */
	      TtaInsertFirstChild (&el, sibling, doc);
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
		  fence = TtaNewTree (doc, newType, "");
		  TtaInsertSibling (fence, child, TRUE, doc);
		  fence = TtaNewTree (doc, newType, "");
		  TtaInsertSibling (fence, child, FALSE, doc);
		}
	    }
	  
	  CreateParentMROW (el, doc);
	  
	  InsertPlaceholder (el, TRUE, doc);
	  InsertPlaceholder (el, FALSE, doc);

	  TtaSetDisplayMode (doc, DisplayImmediately);
	  /* check the Thot abstract tree against the structure schema. */
	  TtaSetStructureChecking (1, doc);
	  
	  /* selected the first child of the new element */
	  while (el != NULL)
	    {
	      sibling = el;
	      el = TtaGetFirstChild (sibling);
	    }
	  TtaSelectElement (doc, sibling);
	}
      else if (surround)
	{
	  /* une transformation a marche */
	  TtaSetDisplayMode (doc, DisplayImmediately);
	}
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
  if (!InitMaths)
    {
      InitMaths = TRUE;

      /* Dialogue form for answering text, user name and password */
      TtaNewSheet (MathsDialogue + FormMaths, TtaGetViewFrame (doc, view), 
		   TtaGetMessage (AMAYA, AM_BUTTON_MATH),
		   0, NULL, TRUE, 1, 'L', D_DONE);
      TtaNewIconMenu (MathsDialogue + MenuMaths, MathsDialogue + FormMaths, 0,
		   NULL, 12, mIcons, FALSE);
      TtaSetMenuForm (MathsDialogue + MenuMaths, 0);
      TtaSetDialoguePosition ();
    }
  TtaShowDialogue (MathsDialogue + FormMaths, TRUE);
  KeyboardsLoadResources ();
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
  TtaAddButton (doc, 1, iconMath, CreateMaths, TtaGetMessage (AMAYA, AM_BUTTON_MATH));
  MathsDialogue = TtaSetCallback (CallbackMaths, MAX_MATHS);
}


/*----------------------------------------------------------------------
   InitMathML initializes MathML context.           
  ----------------------------------------------------------------------*/
void                InitMathML ()
{
   iconMath = TtaCreatePixmapLogo (Math_xpm);
   TtaRegisterPixmap("Math", iconMath);
   mIcons[0] = TtaCreatePixmapLogo (root_xpm);
   mIcons[1] = TtaCreatePixmapLogo (sqrt_xpm);
   mIcons[2] = TtaCreatePixmapLogo (frac_xpm);
   mIcons[3] = TtaCreatePixmapLogo (subsup_xpm);
   mIcons[4] = TtaCreatePixmapLogo (sup_xpm);
   mIcons[5] = TtaCreatePixmapLogo (sub_xpm);
   mIcons[6] = TtaCreatePixmapLogo (overunder_xpm);
   mIcons[7] = TtaCreatePixmapLogo (over_xpm);
   mIcons[8] = TtaCreatePixmapLogo (under_xpm);
   mIcons[9] = TtaCreatePixmapLogo (fence_xpm);
   mIcons[10] = TtaCreatePixmapLogo (mscript_xpm);
   mIcons[11] = TtaCreatePixmapLogo (greek_xpm);
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
  Element	firstChild, child;
  ElementType	elType;

  elType = TtaGetElementType (*el);
  if (elType.ElTypeNum == MathML_EL_MROW)
     /* the parent of the deleted element is a MROW */
     {
     firstChild = TtaGetFirstChild (*el);
     child = firstChild;
     TtaNextSibling (&child);
     if (child == NULL)
       /* there is only one element in the MROW. Remove the MROW */
       {
       TtaSetDisplayMode (doc, DeferredDisplay);
       TtaSetStructureChecking (0, doc);
       TtaRemoveTree (firstChild, doc);
       TtaInsertSibling (firstChild, *el, TRUE, doc);
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
     else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
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
static void MathSetAttributes (Element el, Document doc)
#else /* __STDC__*/
static void MathSetAttributes (el, doc)
     Element el;
     Document doc;
#endif /* __STDC__*/
{
  ElementType	elType;

  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == MathML_EL_MI)
     /* is it really an identifier? */
     {
     /* try to separate function names, identifiers and plain text */
     /* TO DO ******/
     }
  if (elType.ElTypeNum == MathML_EL_MO)
     SetAddspaceAttr (el, doc);
  else
     RemoveAttr (el, doc, MathML_ATTR_addspace);
  if (elType.ElTypeNum == MathML_EL_MI)
     SetFontslantAttr (el, doc);
  else
     RemoveAttr (el, doc, MathML_ATTR_fontslant);		
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
  MathSetAttributes (el, doc);
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
      if (prev == NULL)
	 {
	 next = parent;  TtaNextSibling (&next);
	 if (next == NULL)
	    {
	    parent = TtaGetParent (parent);
	    }
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
	   *pos = TtaGetTextLength (leaf) + 1;
	else
	   *pos = 1;
	}
      }
   return elem;
}

/*----------------------------------------------------------------------
   ParseMathString
   The content of an element MTEXT, MI, MO, MN, or MS has been modified.
   Parse the new content and create the appropriate MI, MO, MN elements.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void ParseMathString (NotifyOnTarget *event)
#else /* __STDC__*/
void ParseMathString (event)
     NotifyOnTarget *event;
#endif /* __STDC__*/
{
  Element	el, selEl, theElem, prevEl, nextEl, textEl, newEl, lastEl,
		firstEl, newSelEl, firstTextEl, prev, next, parent;
  Document	doc;
  ElementType	elType, elType2;
  SSchema	MathMLSchema;
  int		firstSelChar, lastSelChar, newSelChar, len, totLen, i, j, start;
  char		alphabet, c;
  Language	lang;
#define TXTBUFLEN 200
  unsigned char text[TXTBUFLEN];
  char		language[TXTBUFLEN];
  unsigned char	mathType[TXTBUFLEN];

  doc = event->document;
  theElem = event->element;  /* the element whose content has been changed */
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
  firstTextEl = TtaGetFirstChild (theElem);
  textEl = firstTextEl;
  while (textEl != NULL)
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
       TtaNextSibling (&textEl);
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
       if it does not contain any other element */
    {
    el = firstTextEl;
    TtaPreviousSibling (&el);
    if (el == NULL)
       {
       el = firstTextEl;
       TtaNextSibling (&el);
       if (el == NULL)
	  /* the text element has no sibling */
	  {
	  if (newSelEl != NULL)
	     newSelEl = ClosestLeaf (theElem, &newSelChar);
	  el = TtaGetParent (theElem);
	  TtaDeleteTree (theElem, doc);
	  theElem = NULL;
	  CheckMROW (&el, doc);
	  if (el != NULL)
	   if (TtaGetFirstChild (el) == NULL)
	      {
	      elType.ElTypeNum = MathML_EL_Construct;
	      newEl = TtaNewElement (doc, elType);
	      TtaInsertFirstChild (&newEl, el, doc);
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
	 i == totLen)
       /* create a new element */
       {
       if (lastEl == NULL)
	  {
	  newEl = theElem;
	  elType = TtaGetElementType (theElem);
	  if (elType.ElTypeNum != mathType[i-1])
	     {
	     prev = theElem;
	     TtaPreviousSibling (&prev);
	     if (prev == NULL)
		{
		next = theElem;
		TtaNextSibling (&next);
		if (next == NULL)
		   parent = TtaGetParent (theElem);
		}
	     TtaRemoveTree (theElem, doc);
	     ChangeElementType (theElem, mathType[i-1]);
	     if (prev != NULL)
	        TtaInsertSibling (theElem, prev, FALSE, doc);
	     else if (next != NULL)
	        TtaInsertSibling (theElem, next, TRUE, doc);
	     else
	        TtaInsertFirstChild (&theElem, parent, doc);
	     
	     }
	  textEl = TtaGetFirstChild (theElem);
	  while (textEl != NULL)
	     /* delete all text elements except the modified element */
	     {
	     next = textEl;
	     TtaNextSibling (&next);
	     if (textEl != event->target)
		TtaDeleteTree (textEl, doc);
	     textEl = next;
	     }
	  textEl = event->target;
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
       start = i;
       lastEl = newEl;
       MathSetAttributes (newEl, doc);
       if (newSelEl != NULL)
	  {
	  newSelEl = textEl;
	  if (newSelChar < j)
	     if (newSelChar < start)
		newSelChar = 1;
	     else
		newSelChar -= start;
	  }
       }

  /* try to merge the first element processed with its previous sibling */
  if (prevEl != NULL && firstEl != NULL)
    {
    elType = TtaGetElementType (prevEl);
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
  /* try to merge the last element processed with its next sibling */
  if (nextEl != NULL && lastEl != NULL)
    {
    elType = TtaGetElementType (nextEl);
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

  /* Create a MROW element that encompasses the new elements if necessary */
  if (firstEl != NULL)
    CreateParentMROW (firstEl, doc);

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
 MathElementPasted
 An element has been pasted in a MathML structure.
 Create place holders before and after the pasted elements if necessary.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void MathElementPasted (NotifyElement *event)
#else /* __STDC__*/
void MathElementPasted(event)
     NotifyElement *event;
#endif /* __STDC__*/
{
   TtaSetStructureChecking (0, event->document);
   InsertPlaceholder (event->element, TRUE, event->document);
   InsertPlaceholder (event->element, FALSE, event->document);
   TtaSetStructureChecking (1, event->document);
}


/*----------------------------------------------------------------------
 MathElementDeleted
 An element has been deleed in a MathML structure.
 Remove the enclosing MROW element if it has only one child.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void MathElementDeleted (NotifyElement *event)
#else /* __STDC__*/
void MathElementDeleted(event)
     NotifyElement *event;
#endif /* __STDC__*/
{
   CheckMROW (&event->element, event->document);
}

#endif /* MATHML */
