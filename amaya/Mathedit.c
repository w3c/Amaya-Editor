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
	if (ElementContainsText (sibling))
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
  boolean	     before, ParBlock, surround;

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
      if (doc == 0)
	/* no document selected */
	return;
      /* the new element will be inserted before the selected element */
      before = TRUE;
      TtaGiveLastSelectedElement (doc, &last, &c2, &j);
      TtaGiveFirstSelectedElement (doc, &sibling, &c1, &i); 
#ifdef DEBUG
      printf("first selected %s :%d ,%d  \n",TtaGetElementLabel(sibling),c1,i);
      printf("last  selected %s :%d ,%d\n\n",TtaGetElementLabel(last),c2,j);
#endif
    
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
		      /* create the new element after the character string */
		      before = FALSE;
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
		      if (newType.ElTypeNum == HTML_EL_Math ||
			  newType.ElTypeNum == HTML_EL_MathDisp)
			{
			  /* move to the end of the previous MathML element */
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
			  /* move at the end of the previous MathML element */
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
	      /* create the XML element before or after the sibling element */
	      elType.ElTypeNum = HTML_EL_Math;
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
	case 11:
	  TtcDisplayGreekKeyboard (doc, 1);
	  return;
	  break;
	default:
	  return;
	}
      if (!surround || !TransformIntoType (newType, doc))
	{
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
  else if (elType.ElTypeNum == MathML_EL_MI)
     SetFontslantAttr (el, doc);		
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

  Element	el, textEl, prevEl, curEl, firstEl, selEl, newSelEl;
  Document	doc;
  ElementType	elType;
  int		len, i, charType, curElType, curLen, firstSelChar, lastSelChar,
		newSelChar;
  char		newChar[3];
#define TXTBUFLEN 200
  unsigned char	text[TXTBUFLEN];
  Language	lang;
  char		alphabet;
  boolean	spaceBefore, before;

  el = event->element;	/* the element whose content has been changed */
  /* process all text elements within the modified element */
  textEl = TtaGetFirstChild (el);
  if (textEl != NULL)
     {
     /* some initializations */
     doc = event->document;
     elType = TtaGetElementType (el);
     curElType = 0;
     curEl = NULL;
     prevEl = el;
     firstEl = NULL;
     spaceBefore = FALSE;
     curLen = 0;
     newSelEl = NULL;
     TtaSetDisplayMode (doc, DeferredDisplay);
     TtaSetStructureChecking (0, doc);
     while (textEl != NULL)
	{
        len = TtaGetTextLength (textEl);
        if (len > 0)
	  {
	  /* get the current selection */
	  TtaGiveFirstSelectedElement (doc, &selEl, &firstSelChar, &lastSelChar);
	  if (selEl != textEl && selEl != el)
	     selEl = NULL;
	  else
	     if (firstSelChar < 1)
	        firstSelChar = 1;
	  /* get the content */
	  len = TXTBUFLEN;
	  TtaGiveTextContent (textEl, text, &len, &lang);
	  alphabet = TtaGetAlphabet (lang);
	  /* parse the content */
	  for (i = 0; i < len; i++)
	    {
	    if (text[i] == ' ')
	      spaceBefore = TRUE;
	    else
	      {
	      charType = GetCharType (text[i], alphabet);
	      if (firstEl == NULL || charType != curElType)
	        /* create a new element */
	        {
	        if (prevEl != el)
		  MathSetAttributes (prevEl, doc);
	        elType.ElTypeNum = charType;
	        curElType = charType;
	        curEl = TtaNewElement (doc, elType);
	        if (firstEl == NULL)
		   {
		   before = TRUE;
		   firstEl = curEl;
		   }
	        else
		   before = FALSE;
	        TtaInsertSibling (curEl, prevEl, before, doc);
	        elType.ElTypeNum = MathML_EL_TEXT_UNIT;
	        textEl = TtaNewElement (doc, elType);
	        TtaInsertFirstChild (&textEl, curEl, doc);
	        TtaSetTextContent (textEl, NULL, lang, doc);
	        curLen = 0;
	        spaceBefore = FALSE;
	        prevEl = curEl;
	        }

	      if (selEl != NULL && newSelEl == NULL)
	        if (i >= firstSelChar - 1)
		  {
		  newSelEl = textEl;
		  newSelChar = curLen + 1;
		  }

	      if (spaceBefore && curLen > 0)
	        {
	        newChar[0] = ' '; newChar[1] = text[i]; newChar[2] = '\0';
	        curLen+= 2;
	        spaceBefore = FALSE;
	        }
	      else
	        {
	        newChar[0] = text[i]; newChar[1] = '\0';
	        curLen++;
	        }
	      TtaAppendTextContent (textEl, newChar, doc);
	      }
	    }
	  /* end of parsing */
	  /* the last element created is now complete. Associate the
	     attribute that fits with its content */
	  if (curEl != NULL)
	    MathSetAttributes (curEl, doc);
	  if (selEl != NULL && newSelEl == NULL)
	     {
	     newSelEl = textEl;
	     newSelChar = curLen + 1;
	     }
	  }
	  TtaNextSibling (&textEl);
	}

     /* remove the element that has been processed */
     TtaDeleteTree (el, doc);

     /* create a MROW element that encompasses the new elements
	if necessary */
     CreateParentMROW (firstEl, doc);

     TtaSetStructureChecking (1, doc);
     TtaSetDisplayMode (doc, DisplayImmediately);

     /* set a new selection */
     if (newSelEl != NULL)
	TtaSelectString (doc, newSelEl, newSelChar, newSelChar);
     }
}

/*----------------------------------------------------------------------
 MathElementPasted
 An element has been pasted in a MathML structure.
 Create place holders before and after if necessary.
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
 An element has been deleted in a MathML structure.
 Remove the enclosing MROW element if it has only one child.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void MathElementDeleted (NotifyElement *event)
#else /* __STDC__*/
void MathElementDeleted(event)
     NotifyElement *event;
#endif /* __STDC__*/
{
  /**** code to be written ****/
}

#endif /* MATHML */
