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


#include "MathML.h"
#include "Math.xpm"
#include "root.xpm"
#include "sqrt.xpm"
#include "frac.xpm"
#include "subsup.xpm"
#include "sup.xpm"
#include "sub.xpm"
#include "fence.xpm"
#include "n.xpm"
#include "o.xpm"
#include "id.xpm"
#include "txt.xpm"
#define FormMaths 0
#define MenuMaths 1
#define MAX_MATHS  2

static Pixmap       iconMath;
static Pixmap       mIcons[11];
static int          MathsDialogue;
static boolean      InitMaths;


/*----------------------------------------------------------------------
   SplitTextInMathML: split the element el and the enclosing element (MO,
   MI, MN or MTEXT).
   The parameter index gives the point where the text has to be cut.
   Return the next created text within the next enclosing element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Element             SplitTextInMathML (Document doc, Element el, int index)
#else
Element             SplitTextInMathML (doc, el, index)
Document            doc;
Element             el;
int                 index;
#endif
{
  Element            new, parent;
  ElementType        elType;

  /* do not check the Thot abstract tree against the structure */
  /* schema while changing the structure */
  TtaSetStructureChecking (0, doc);
  /* split the text to insert the XML element */
  TtaSplitText (el, index-1, doc);
  /* duplicate the parent (MO, MN, MI or MTEXT) */
  parent = TtaGetParent (el);
  elType = TtaGetElementType (parent);
  new = TtaNewElement (doc, elType);
  TtaInsertSibling (new, parent, FALSE, doc);
  /* take the second part of the split text element */
  TtaNextSibling (&el);
  TtaRemoveTree (el, doc);
  /* move the old element into the new MROW */
  TtaInsertFirstChild (&el, new, doc);
  /* check the Thot abstract tree against the structure schema. */
  TtaSetStructureChecking (1, doc);
  return (el);
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
  Element            sibling, el, row;
  ElementType        newType, elType;
  SSchema            docSchema, mathSchema;
  int                val, c1, i, len;
  boolean	     before, addConstruction;

  val = (int) data;
  switch (ref - MathsDialogue)
    {
    case FormMaths:
      InitMaths = FALSE;
      TtaDestroyDialogue (ref);	   
      break;
    case MenuMaths:
      doc = TtaGetSelectedDocument ();
      if (doc == 0)
	/* no document selected */
	return;
      /* the new element will be inserted before the selected element */
      before = TRUE;
      addConstruction = FALSE;
      TtaGiveFirstSelectedElement (doc, &sibling, &c1, &i);
      /* Check whether the selected element is a text element */
      elType = TtaGetElementType (sibling);

      /* Check whether the selected element is a MathML element */
      docSchema = TtaGetDocumentSSchema (doc);
      if (!TtaSameSSchemas (docSchema, elType.ElSSchema))
	{
	  /* the selection concerns a MathML element */
	  mathSchema = elType.ElSSchema;
	  if (elType.ElTypeNum == MathML_EL_TEXT_UNIT && c1 > 1)
	    {
	      len = TtaGetTextLength (sibling);
	      if (c1 > len)
		{
		  /* the caret is at the end of that character string */
		  /* create the new element after the character string */
		  before = FALSE;
		  addConstruction = TRUE;
		}
	      else
		sibling = SplitTextInMathML (doc, sibling, c1);
	    }
	}
      else
	{
	  /* the selection concerns an HTML element */
	  mathSchema = TtaNewNature (docSchema, "MathML", "MathMLP");
	  if (elType.ElTypeNum != HTML_EL_XML)
	    {
	      if (elType.ElTypeNum == HTML_EL_TEXT_UNIT && c1 > 1)
		{
		  len = TtaGetTextLength (sibling);
		  if (c1 > len)
		    {
		      /* the caret is at the end of that character string */
		      /* create the new element after the character string */
		      before = FALSE;
		      addConstruction = TRUE;
		    }
		  else
		    {
		      /* split the text to insert the XML element */
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
		      if (newType.ElTypeNum == HTML_EL_XML)
			{
			  /* move to the end of the previous MathML element */
			  before = FALSE;
			  addConstruction = TRUE;
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
		      if (newType.ElTypeNum == HTML_EL_XML)
			{
			  /* move at the end of the previous MathML element */
			  before = TRUE;
			  addConstruction = FALSE;
			  sibling = TtaGetFirstChild (el);		      
			}
		    }
		}
	    }

	  if (elType.ElTypeNum == HTML_EL_XML)
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
	      /* create the XML element before or after the sibling element */
	      elType.ElTypeNum = HTML_EL_XML;
	      el = TtaNewTree (doc, elType, "");
	      TtaInsertSibling (el, sibling, before, doc);
	      sibling = TtaGetFirstChild (el);
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
	  newType.ElTypeNum = MathML_EL_Block;
	  break;
	case 7:
	  newType.ElTypeNum = MathML_EL_MN;
	  addConstruction = FALSE;
	  break;
	case 8:
	  newType.ElTypeNum = MathML_EL_MO;
	  addConstruction = FALSE;
	  break;
	case 9:
	  newType.ElTypeNum = MathML_EL_MI;
	  addConstruction = FALSE;
	  break;
	case 10:
	  newType.ElTypeNum = MathML_EL_MTEXT;
	  addConstruction = FALSE;
	  break;
	default:
	  return;
	}
      el = TtaNewTree (doc, newType, "");

      if (elType.ElTypeNum == MathML_EL_MROW || elType.ElTypeNum == MathML_EL_MathML)
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
	      /* do not check the Thot abstract tree against the structure */
	      /* schema while changing the structure */
	      TtaSetStructureChecking (0, doc);
	      TtaInsertSibling (el, row, TRUE, doc);
	      TtaRemoveTree (row, doc);
	      /* check the Thot abstract tree against the structure schema. */
	      TtaSetStructureChecking (1, doc);
	    }
	  else
	    {
	      /* check whether the element is a construction */
	      elType = TtaGetElementType (sibling);
	      if ((elType.ElTypeNum == MathML_EL_Component || elType.ElTypeNum == MathML_EL_Construction) && !addConstruction)
		TtaInsertFirstChild (&el, sibling, doc);
	      else
		TtaInsertSibling (el, sibling, before, doc);
	    }
	}
      else if ((elType.ElTypeNum == MathML_EL_Component || elType.ElTypeNum == MathML_EL_Construction) && !addConstruction)
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
	  /* check whether the parent is a row */
	  row = TtaGetParent (sibling);
	  elType = TtaGetElementType (row);
	  if (elType.ElTypeNum != MathML_EL_MathML
	      && elType.ElTypeNum != MathML_EL_MROW)
	    {
	      /* generates a new row element to include both elements */
	      elType.ElTypeNum = MathML_EL_MROW;
	      row = TtaNewElement (doc, elType);
	      /* do not check the Thot abstract tree against the structure */
	      /* schema while changing the structure */
	      TtaSetStructureChecking (0, doc);
	      TtaInsertSibling (row, sibling, TRUE, doc);
	      TtaRemoveTree (sibling, doc);
	      /* move the old element into the new MROW */
	      TtaInsertFirstChild (&sibling, row, doc);
	      /* check the Thot abstract tree against the structure schema. */
	      TtaSetStructureChecking (1, doc);
	    }
	    /* insert the new element */
	    TtaInsertSibling (el, sibling, before, doc);
	}

      if (addConstruction)
	{
	  newType.ElTypeNum = MathML_EL_Construction;
	  sibling = TtaNewTree (doc, newType, "");
	  TtaInsertSibling (sibling, el, FALSE, doc);
	}
      /* selected the first child of the new element */
      while (el != NULL)
	{
	  sibling = el;
	  el = TtaGetFirstChild (sibling);
	}
      TtaSelectElement (doc, sibling);
      elType = TtaGetElementType (sibling);
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
		   NULL, 11, mIcons, FALSE);
      TtaSetMenuForm (MathsDialogue + MenuMaths, 0);
      TtaSetDialoguePosition ();
    }
  TtaShowDialogue (MathsDialogue + FormMaths, TRUE);
  /*TtcDisplayGreekKeyboard (doc, view);*/
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
   mIcons[6] = TtaCreatePixmapLogo (fence_xpm);
   mIcons[7] = TtaCreatePixmapLogo (n_xpm);
   mIcons[8] = TtaCreatePixmapLogo (o_xpm);
   mIcons[9] = TtaCreatePixmapLogo (id_xpm);
   mIcons[10] = TtaCreatePixmapLogo (txt_xpm);
}
#endif /* MATHML */
