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
#include "overunder.xpm"
#include "over.xpm"
#include "under.xpm"
#include "fence.xpm"
#include "n.xpm"
#include "o.xpm"
#include "id.xpm"
#include "txt.xpm"
#define FormMaths 0
#define MenuMaths 1
#define MAX_MATHS  2

static Pixmap       iconMath;
static Pixmap       mIcons[14];
static int          MathsDialogue;
static boolean      InitMaths;


/*----------------------------------------------------------------------
   SplitTextInMathML
   Split element el and the enclosing element (MO, MI, MN or MTEXT).
   Parameter index gindicats the position where the text has to be split.
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
  ElementContainsText
  returns TRUE if element el contains some text.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean      ElementContainsText (Element el)
#else
boolean      ElementContainsText (el)
Element el;

#endif
{
  ElementType	elType;

  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == MathML_EL_Construction ||
      elType.ElTypeNum == MathML_EL_TEXT_UNIT ||
      elType.ElTypeNum == MathML_EL_MI  ||
      elType.ElTypeNum == MathML_EL_MO ||
      elType.ElTypeNum == MathML_EL_MN ||
      elType.ElTypeNum == MathML_EL_MS ||
      elType.ElTypeNum == MathML_EL_MTEXT)
     return TRUE;
  else
     return FALSE;
}

/*----------------------------------------------------------------------
   InsertPlaceHolder
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void          InsertPlaceHolder (Element el, boolean before, Document doc)
#else
static void          InsertPlaceHolder (el, before, doc)
Element el;
boolean before;
Document doc;

#endif
{
ElementType	elType;
Element		sibling;
boolean		createConstruction;

     elType = TtaGetElementType (el);
     if (elType.ElTypeNum == MathML_EL_MROW ||
	 elType.ElTypeNum == MathML_EL_MROOT ||
	 elType.ElTypeNum == MathML_EL_MSQRT ||
	 elType.ElTypeNum == MathML_EL_MFRAC ||
	 elType.ElTypeNum == MathML_EL_MSUBSUP ||
	 elType.ElTypeNum == MathML_EL_MSUB ||
	 elType.ElTypeNum == MathML_EL_MSUP ||
	 elType.ElTypeNum == MathML_EL_MUNDER ||
	 elType.ElTypeNum == MathML_EL_MUNDEROVER)
        /* this element accepts a Construction as its neighbour */
	{
	sibling = el;
	if (before)
	   TtaPreviousSibling (&sibling);
	else
	TtaNextSibling (&sibling);
	createConstruction = TRUE;
	if (sibling != NULL)
	if (ElementContainsText (sibling))
	   createConstruction = FALSE;
	if (createConstruction)
	   {
	   elType.ElTypeNum = MathML_EL_Construction;
	   sibling = TtaNewElement (doc, elType);
	   TtaInsertSibling (sibling, el, before, doc);
	   }
	}
}

/*----------------------------------------------------------------------
  CreateParentMROW
  If element el id not a child of a MROW and if it has at least one
  sibling that is not a Construction, create an enclosing MROW.
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
      elType.ElTypeNum != MathML_EL_Block)
	 {
	 sibling = TtaGetFirstChild (parent);
	 nChildren = 0;
	 firstChild = sibling;
	 while (sibling != NULL)
	    {
	    elType = TtaGetElementType (sibling);
	    if (elType.ElTypeNum != MathML_EL_Component &&
		elType.ElTypeNum != MathML_EL_Construction)
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
  Element            sibling, el, row;
  ElementType        newType, elType;
  SSchema            docSchema, mathSchema;
  int                val, c1, i, len;
  boolean	     before;

  val = (int) data;
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
      TtaGiveFirstSelectedElement (doc, &sibling, &c1, &i);
      /* Check whether the selected element is a text element */
      elType = TtaGetElementType (sibling);

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
	  newType.ElTypeNum = MathML_EL_Block;
	  break;
	case 10:
	  newType.ElTypeNum = MathML_EL_MN;
	  break;
	case 11:
	  newType.ElTypeNum = MathML_EL_MO;
	  break;
	case 12:
	  newType.ElTypeNum = MathML_EL_MI;
	  break;
	case 13:
	  newType.ElTypeNum = MathML_EL_MTEXT;
	  break;
	default:
	  return;
	}
      el = TtaNewTree (doc, newType, "");

      /* do not check the Thot abstract tree against the structure */
      /* schema while changing the structure */
      TtaSetStructureChecking (0, doc);
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
	      TtaInsertSibling (el, row, TRUE, doc);
	      TtaRemoveTree (row, doc);
	    }
	  else
	    {
	      /* check whether the element is a construction */
	      elType = TtaGetElementType (sibling);
	      if (elType.ElTypeNum == MathML_EL_Component ||
		  elType.ElTypeNum == MathML_EL_Construction)
		TtaInsertFirstChild (&el, sibling, doc);
	      else
		TtaInsertSibling (el, sibling, before, doc);
	    }
	}
      else if (elType.ElTypeNum == MathML_EL_Component ||
	       elType.ElTypeNum == MathML_EL_Construction)
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

      CreateParentMROW (el, doc);

      InsertPlaceHolder (el, TRUE, doc);
      InsertPlaceHolder (el, FALSE, doc);

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
		   NULL, 14, mIcons, FALSE);
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
   mIcons[6] = TtaCreatePixmapLogo (overunder_xpm);
   mIcons[7] = TtaCreatePixmapLogo (over_xpm);
   mIcons[8] = TtaCreatePixmapLogo (under_xpm);
   mIcons[9] = TtaCreatePixmapLogo (fence_xpm);
   mIcons[10] = TtaCreatePixmapLogo (n_xpm);
   mIcons[11] = TtaCreatePixmapLogo (o_xpm);
   mIcons[12] = TtaCreatePixmapLogo (id_xpm);
   mIcons[13] = TtaCreatePixmapLogo (txt_xpm);
}

/*----------------------------------------------------------------------
   SetFontslantAttr
   The content of a MI element has been created or modified.
   Create or change attribute fontslant for that element accordingly.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void SetFontslantAttr (Element el, Document doc)
#else /* __STDC__*/
void SetFontslantAttr (el, doc)
  Element	el;
  Document	doc;
#endif /* __STDC__*/
{
  Element	textEl;
  ElementType	elType;
  AttributeType	attrType;
  Attribute	attr;
  int		len;

  textEl = TtaGetFirstChild (el);
  if (textEl != NULL)
     {
     /* search the fontslant attribute */
     elType = TtaGetElementType (el);
     attrType.AttrSSchema = elType.ElSSchema;
     attrType.AttrTypeNum = MathML_ATTR_fontslant;
     attr = TtaGetAttribute (el, attrType);
     /* get content length */
     len = TtaGetTextLength (textEl);
     if (len > 1)
        /* put an attribute fontslant = plain */
	{
	if (attr == NULL)
	   {
	   attr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (el, attr, doc);
	   }
	TtaSetAttributeValue (attr, MathML_ATTR_fontslant_VAL_plain, el, doc);
	}
     else
	/* remove attribute fontslant if it exists */
	{
	if (attr != NULL)
	   TtaRemoveAttribute (el, attr, doc);
	}
     }
}

/*----------------------------------------------------------------------
   SetAddspaceAttr
   The content of a MO element has been created or modified.
   Create or change attribute addspace for that element accordingly.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void SetAddspaceAttr (Element el, Document doc)
#else /* __STDC__*/
void SetAddspaceAttr (el, doc)
  Element	el;
  Document	doc;
#endif /* __STDC__*/
{
  Element	textEl, previous;
  ElementType	elType;
  AttributeType	attrType;
  Attribute	attr;
  int		len, val;
#define BUFLEN 10
  unsigned char	text[BUFLEN];
  Language	lang;
  char		alphabet;

  textEl = TtaGetFirstChild (el);
  if (textEl != NULL)
     {
     /* search the addspace attribute */
     elType = TtaGetElementType (el);
     attrType.AttrSSchema = elType.ElSSchema;
     attrType.AttrTypeNum = MathML_ATTR_addspace;
     attr = TtaGetAttribute (el, attrType);
     if (attr == NULL)
	{
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (el, attr, doc);
	}
     val = MathML_ATTR_addspace_VAL_nospace;
     len = TtaGetTextLength (textEl);
     if (len > 0 && len < BUFLEN)
	{
	len = BUFLEN;
	TtaGiveTextContent (textEl, text, &len, &lang);
	alphabet = TtaGetAlphabet (lang);
	if (len == 1)
	   if (alphabet == 'L')
	     /* ISO-Latin 1 character */
	     {
	     if (text[0] == '-')
		/* unary or binary operator? */
		{
		previous = el;
		TtaPreviousSibling (&previous);
		if (previous == NULL)
		   /* no previous sibling => unary operator */
		   val = MathML_ATTR_addspace_VAL_nospace;
		else
		   {
		   elType = TtaGetElementType (previous);
		   if (elType.ElTypeNum == MathML_EL_MO)
		      /* after an operator => unary operator */
		      val = MathML_ATTR_addspace_VAL_nospace;
		   else
		      /* binary operator */
		      val = MathML_ATTR_addspace_VAL_both;
		   }
		}
	     else if (text[0] == '+' ||
	         text[0] == '&' ||
	         text[0] == '*' ||
	         text[0] == '<' ||
	         text[0] == '=' ||
	         text[0] == '>' ||
	         text[0] == '^')
		 /* binary operator */
	         val = MathML_ATTR_addspace_VAL_both;
	     else if (text[0] == ',' ||
		      text[0] == ';')
	         val = MathML_ATTR_addspace_VAL_spaceafter;
	     }
	   else if (alphabet == 'G')
	     /* Symbol character set */
	     if ((int)text[0] == 177)	/* PlusMinus */
		val = MathML_ATTR_addspace_VAL_both;
	/**** to be completed *****/
	}
     TtaSetAttributeValue (attr, val, el, doc);
     }
}

/*----------------------------------------------------------------------
   GetCharType
   returns the type of character c (MN, MI or MO).
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static int GetCharType (unsigned char c)
#else /* __STDC__*/
static int GetCharType (c)
     unsigned char c;
#endif /* __STDC__*/
{
  int	ret;

  if (c >= '0' && c <= '9')
     ret = MathML_EL_MN;
  else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
     ret = MathML_EL_MI;
  else
     ret = MathML_EL_MO;
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
  doc = event->document;
  textEl = TtaGetFirstChild (el);	/* the content of that element */
  if (textEl != NULL)
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
	/* some initializations */
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
	/* parse the content */
	for (i = 0; i < len; i++)
	  {
	  if (text[i] == ' ')
	    spaceBefore = TRUE;
	  else
	    {
	    charType = GetCharType (text[i]);
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
   InsertPlaceHolder (event->element, TRUE, event->document);
   InsertPlaceHolder (event->element, FALSE, event->document);
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
