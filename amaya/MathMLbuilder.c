/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2010
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * MathMLbuilder
 *
 * Author: V. Quint
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "html2thot_f.h"
#include "MathML.h"
#include "HTML.h"
#include "parser.h"
#include "styleparser_f.h"
#include "style.h"
#include "undo.h"
#include "Xml2thot_f.h"

/* Define a pointer to let parser functions access the Math entity table */
extern XmlEntity *pMathEntityTable;

#define MaxMsgLength 200

#include "HTMLactions_f.h"
#include "XHTMLbuilder_f.h"
#include "HTMLtable_f.h"
#include "Mathedit_f.h"
#include "styleparser_f.h"
#include "fetchXMLname_f.h"

/*----------------------------------------------------------------------
  IsLargeOp
  Return TRUE if character is listed as a largeop in the MathML 2.0
  Operator dictionary (appendix F.5)
  ----------------------------------------------------------------------*/
static ThotBool IsLargeOp (CHAR_T character)
{
  if (character == 0x7C   || /* vertical bar */
      character == 0x22C1 || /* Vee */
      character == 0x2296 || /* CircleMinus */
      character == 0x2295 || /* CirclePlus */
      character == 0x2211 || /* Sum */
      character == 0x03A3 || /* Sigma */
      character == 0x222A || /* Union */
      character == 0x22C3 || /* n-ary Union */
      character == 0x228E || /* UnionPlus */
      character == 0x2232 || /* ClockwiseContourIntegral */
      character == 0x222E || /* ContourIntegral */
      character == 0x2233 || /* CounterClockwiseContourIntegral */
      character == 0x222F || /* DoubleContourIntegral */
      character == 0x222B || /* Integral */
      //      character == 0x222D || /* TripleIntegral */
      character == 0x22C0 || /* Wedge */
      character == 0x2297 || /* CircleTimes */
      character == 0x2210 || /* Coproduct */
      character == 0x220F || /* Product */
      character == 0x03A0 || /* Pi */
      character == 0x2229 || /* Intersection */
      character == 0x22C2 || /* n-ary Intersection */
      character == 0x2299 )  /* CircleDot */
    /* it's a large operator */
    return TRUE;
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  IsStretchyFence
  Return TRUE if character is listed as a stretchy fence in the MathML 2.0
  Operator dictionary (appendix F.5)
  ----------------------------------------------------------------------*/
static ThotBool IsStretchyFence (CHAR_T character, char script)
{
  if (((character == '(' || character == ')' ||
        character == '[' || character == ']' ||
        character == '{' || character == '}' ||
        character == '|' || character == 0x2223  ||
        character == 0x2956))  ||
      /* strangely enough, appendix F.5 does not
         consider this character as a fence */
      (
       (character == 0x2329 || /* LeftAngleBracket */
        character == 0x27E8 || /* MathematicalLeftAngleBracket */
        /* LeftBracketingBar ??? */
        character == 0x2308 || /* LeftCeiling */
        character == 0x301A || /* LeftDoubleBracket */
        /* LeftDoubleBracketingBar ??? */
        character == 0x230A || /* LeftFloor */
        character == 0x232A || /* RightAngleBracket */
        character == 0x27E9 || /* MathematicalRightAngleBracket */
        /* RightBracketingBar ??? */
        character == 0x2309 || /* RightCeiling */
        character == 0x301B || /* RightDoubleBracket */
        /* RightDoubleBracketingBar ??? */
        character == 0x230B )  /* RightFloor */
       ))
    return TRUE;
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  MapMathMLAttribute
  Search in the Attribute Mapping Table the entry for the
  attribute of name Attr and returns the corresponding Thot attribute type.
  ----------------------------------------------------------------------*/
void MapMathMLAttribute (char *attrName, AttributeType *attrType,
                         char *elementName, ThotBool *level, Document doc)
{
  attrType->AttrSSchema = GetMathMLSSchema (doc);
  MapXMLAttribute (MATH_TYPE, attrName, elementName, level, doc,
                   &(attrType->AttrTypeNum));
}

/*----------------------------------------------------------------------
  MapMathMLAttributeValue
  Search in the Attribute Value Mapping Table the entry for the attribute
  ThotAtt and its value AttrVal. Returns the corresponding Thot value.
  ----------------------------------------------------------------------*/
void MapMathMLAttributeValue (char *attVal, const AttributeType * attrType, int *value)
{
  MapXMLAttributeValue (MATH_TYPE, attVal, attrType, value);
}

/*----------------------------------------------------------------------
  MathMLEntityCreated
  ----------------------------------------------------------------------*/
void MathMLEntityCreated (unsigned char *entityValue, Language lang,
                          char *entityName, Document doc)
{
}

/*----------------------------------------------------------------------
  ElementNeedsPlaceholder
  returns TRUE if element el needs a sibling placeholder.
  ----------------------------------------------------------------------*/
ThotBool     ElementNeedsPlaceholder (Element el)
{
  ElementType   elType;
  Element	child, parent, sibling;
  ThotBool	ret;

  ret = FALSE;
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == MathML_EL_MS ||
      elType.ElTypeNum == MathML_EL_MFRAC ||
      elType.ElTypeNum == MathML_EL_BevelledMFRAC ||
      elType.ElTypeNum == MathML_EL_MSQRT ||
      elType.ElTypeNum == MathML_EL_MROOT ||
      elType.ElTypeNum == MathML_EL_MSTYLE ||
      elType.ElTypeNum == MathML_EL_MERROR ||
      elType.ElTypeNum == MathML_EL_MPADDED ||
      elType.ElTypeNum == MathML_EL_MPHANTOM ||
      elType.ElTypeNum == MathML_EL_MFENCED ||
      elType.ElTypeNum == MathML_EL_MF ||
      elType.ElTypeNum == MathML_EL_MSUB ||
      elType.ElTypeNum == MathML_EL_MSUP ||
      elType.ElTypeNum == MathML_EL_MSUBSUP ||
      elType.ElTypeNum == MathML_EL_MUNDER ||
      elType.ElTypeNum == MathML_EL_MOVER ||
      elType.ElTypeNum == MathML_EL_MUNDEROVER ||
      elType.ElTypeNum == MathML_EL_MMULTISCRIPTS ||
      elType.ElTypeNum == MathML_EL_MTABLE ||
      elType.ElTypeNum == MathML_EL_MACTION)
    ret = TRUE;
  else if (elType.ElTypeNum == MathML_EL_MROW)
    /* a mrow needs a place holder only if it's not the only child of
       an element such as Numerator, Denominator, RootBase, Index, etc. */
    {
      ret = TRUE;
      sibling = el;  TtaNextSibling (&sibling);
      if (!sibling)
        {
          sibling = el;  TtaPreviousSibling (&sibling);
          if (!sibling)
            /* the MROW element has no sibling */
            {
              parent = TtaGetParent (el);
              if (parent)
                {
                  elType = TtaGetElementType (parent);
                  if (elType.ElTypeNum == MathML_EL_Numerator ||
                      elType.ElTypeNum == MathML_EL_Denominator ||
                      elType.ElTypeNum == MathML_EL_RootBase ||
                      elType.ElTypeNum == MathML_EL_Index ||
                      elType.ElTypeNum == MathML_EL_FencedExpression ||
                      elType.ElTypeNum == MathML_EL_Base ||
                      elType.ElTypeNum == MathML_EL_Subscript ||
                      elType.ElTypeNum == MathML_EL_Superscript ||
                      elType.ElTypeNum == MathML_EL_UnderOverBase ||
                      elType.ElTypeNum == MathML_EL_Underscript ||
                      elType.ElTypeNum == MathML_EL_Overscript ||
                      elType.ElTypeNum == MathML_EL_MultiscriptBase ||
                      elType.ElTypeNum == MathML_EL_MSubscript ||
                      elType.ElTypeNum == MathML_EL_MSuperscript)
                    {
                      /* no place holder required, except if the MROW element
                         actually represent a prenthesized block */
                      ret = FALSE;
                      child = TtaGetFirstChild (el);
                      if (child != NULL)
                        {
                          elType = TtaGetElementType (child);
                          if (elType.ElTypeNum == MathML_EL_MF)
                            /* the first child of the MROW element is a MF */
                            /* The MROW element needs a placeholder */
                            ret = TRUE;
                        }
                    } 
                }
            }
        }
    }
  else if (elType.ElTypeNum == MathML_EL_MO ||
           elType.ElTypeNum == MathML_EL_OpeningFence ||
           elType.ElTypeNum == MathML_EL_ClosingFence ||
           elType.ElTypeNum == MathML_EL_FencedSeparator)
    /* an operator that contains a single Symbol needs a placeholder,
       except when it is in a Base or UnderOverBase */
    {
      child = TtaGetFirstChild (el);
      if (child != NULL)
        {
          elType = TtaGetElementType (child);
          if (elType.ElTypeNum == MathML_EL_SYMBOL_UNIT)
            {
              ret = TRUE;
              parent = TtaGetParent (el);
              if (parent != NULL)
                {
                  elType = TtaGetElementType (parent);
                  if (elType.ElTypeNum == MathML_EL_Base ||
                      elType.ElTypeNum == MathML_EL_UnderOverBase)
                    ret = FALSE;
                }
            }
        }
    }
  else if (elType.ElTypeNum == MathML_EL_MSPACE)
    {
      /* in principle mspace needs a placeholder sibling */
      ret = TRUE;
      /* but if it's the only child of a mstyle element, the placeholders
         would change the height and width of the mstyle. Consider for
         instance:
         <mstyle background="#000099">
         <mspace depth="2mm" width=".2in"/>
         </mstyle>  */
      sibling = el;  TtaNextSibling (&sibling);
      if (!sibling)
        {
          sibling = el;  TtaPreviousSibling (&sibling);
          if (!sibling)
            {
              parent = TtaGetParent (el);
              if (parent)
                {
                  elType = TtaGetElementType (parent);
                  if (elType.ElTypeNum == MathML_EL_MSTYLE)
                    ret = FALSE;
                }
            }
        }
    }
  return ret;
}

/*----------------------------------------------------------------------
  CreatePlaceholders
  ----------------------------------------------------------------------*/
static void	CreatePlaceholders (Element el, Document doc)
{
  Element	 sibling, prev, constr, child;
  Attribute	 attr;
  ElementType	 elType;
  AttributeType  attrType;
  ThotBool	 first, create, stretchableSubsup;

  if (!el)
    return;
  elType.ElSSchema = GetMathMLSSchema (doc);
  prev = NULL;
  create = TRUE;
  first = TRUE;
  sibling = el;
  while (sibling != NULL)
    {
      /* skip comments */
      elType = TtaGetElementType (sibling);
      while (sibling && elType.ElTypeNum == MathML_EL_XMLcomment)
	{
	  TtaNextSibling (&sibling);
	  if (sibling)
	    elType = TtaGetElementType (sibling);
	}

      if (sibling)
	/* there is an element that is not a comment */
	{
	  if (!ElementNeedsPlaceholder (sibling))
	    create = FALSE;
	  else
	    {
	      if (first)
		/* first element */
		{
		  elType = TtaGetElementType (sibling);
		  if (elType.ElTypeNum == MathML_EL_MF)
		    /* the first element is a MF. Don't create a placeholder
		       before */
		    create = FALSE;
		  else if (elType.ElTypeNum == MathML_EL_MROW)
		    /* the first element is a MROW */
		    {
		      child = TtaGetFirstChild (sibling);
		      if (child != NULL)
			{
			  elType = TtaGetElementType (child);
			  if (elType.ElTypeNum != MathML_EL_MF)
			    /* the first child of the MROW element is not a MF*/
			    /* Don't create a placeholder before */
			    create = FALSE;
			}
		    }
		}
	      if (create)
		{
		  elType.ElTypeNum = MathML_EL_Construct;
		  constr = TtaNewElement (doc, elType);
		  TtaInsertSibling (constr, sibling, TRUE, doc);
		  attrType.AttrSSchema = elType.ElSSchema;
		  attrType.AttrTypeNum = MathML_ATTR_IntPlaceholder;
		  attr = TtaNewAttribute (attrType);
		  TtaAttachAttribute (constr, attr, doc);
		  TtaSetAttributeValue (attr,
					MathML_ATTR_IntPlaceholder_VAL_yes_,
					constr, doc);
		}
	      create = TRUE;
	    }
	  prev = sibling;
	  TtaNextSibling (&sibling);
	  first = FALSE;
	}
    }

  if (prev != NULL && create)
    {
      stretchableSubsup = FALSE;
      elType = TtaGetElementType (prev);
      /* don't insert a placeholder after the last element if it's a MF */
      if (elType.ElTypeNum == MathML_EL_MF)
        create = FALSE;
      else if (elType.ElTypeNum == MathML_EL_MROW)
        /* the last element is a MROW */
        {
          child = TtaGetLastChild (prev);
          if (child != NULL)
            {
              elType = TtaGetElementType (child);
              if (elType.ElTypeNum != MathML_EL_MF)
                /* the last child of the MROW element is not a MF */
                /* Don't create a placeholder before */
                create = FALSE;
            }
        }
      else if (elType.ElTypeNum == MathML_EL_MSUBSUP ||
               elType.ElTypeNum == MathML_EL_MSUB ||
               elType.ElTypeNum == MathML_EL_MSUP ||
               elType.ElTypeNum == MathML_EL_MUNDEROVER ||
               elType.ElTypeNum == MathML_EL_MUNDER ||
               elType.ElTypeNum == MathML_EL_MUNDEROVER)
        {
          attrType.AttrSSchema = elType.ElSSchema;
          attrType.AttrTypeNum = MathML_ATTR_IntVertStretch;
          if (TtaGetAttribute (prev, attrType))
            stretchableSubsup = TRUE;
        }

      if (create)
        {
          if (stretchableSubsup)
            elType.ElTypeNum = MathML_EL_Construct1;
          else
            elType.ElTypeNum = MathML_EL_Construct;
          constr = TtaNewElement (doc, elType);
          TtaInsertSibling (constr, prev, FALSE, doc);
          attrType.AttrSSchema = elType.ElSSchema;
          attrType.AttrTypeNum = MathML_ATTR_IntPlaceholder;
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (constr, attr, doc);
          TtaSetAttributeValue (attr, MathML_ATTR_IntPlaceholder_VAL_yes_,
                                constr, doc);
        }
    }
}

/*----------------------------------------------------------------------
  NextNotComment
  Return the next sibling of element el that is not an XMLcomment element.
  Return el itself if it's not a comment.
  ----------------------------------------------------------------------*/
static void	NextNotComment (Element* el, Element* prev)
{
  ElementType	elType;

  if (*el == NULL)
    return;
  elType = TtaGetElementType (*el);
  while (*el != NULL && elType.ElTypeNum == MathML_EL_XMLcomment)
    {
      *prev = *el;
      TtaNextSibling (el);
      if (*el != NULL)
        elType = TtaGetElementType (*el);
    }
}

/*----------------------------------------------------------------------
  SetIntMovelimitsAttr
  Put a IntMovelimits attribute on element el (which is a munder, mover
  or munderover) if the current value of IntDisplaystyle is false and
  if el contains a MO element that allows limits to be moved.
  -----------------------------------------------------------------------*/
void SetIntMovelimitsAttr (Element el, Document doc)
{
  Element	ancestor, child, base, operator_, textEl;
  int           value, len;
  ElementType   elType;
  AttributeType attrType;
  Attribute     attr;
  Language      lang;
  CHAR_T        text[10];
  char          buffer[20];
  ThotBool      movable;

  if (el == NULL || doc == 0)
    return;
  movable = FALSE;

  /* first look for an IntDisplaystyle attribute on an ancestor */
  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = MathML_ATTR_IntDisplaystyle;
  ancestor = el;
  do
    {
      attr = TtaGetAttribute (ancestor, attrType);
      if (!attr)
        ancestor = TtaGetParent(ancestor);
    }
  while (!attr && ancestor);
  if (attr)
    /* there is an ancestor with an attribute IntDisplaystyle */
    {
      value = TtaGetAttributeValue (attr);
      if (value == MathML_ATTR_IntDisplaystyle_VAL_false)
        /* an ancestor has an attribute IntDisplaystyle = false */
        {
          /* Check the operator within the base */
          child = TtaGetFirstChild (el);
          base = NULL;
          do
            {
              elType = TtaGetElementType (child);
              if (elType.ElTypeNum == MathML_EL_UnderOverBase)
                base = child;
              else
                TtaNextSibling (&child);
            }
          while (child && !base);
          if (base)
            {
              child = TtaGetFirstChild (base);
              operator_ = NULL;
              do
                {
                  elType = TtaGetElementType (child);
                  if (elType.ElTypeNum == MathML_EL_MO)
                    operator_ = child;
                  else
                    TtaNextSibling (&child);
                }
              while (child && !operator_);
              if (operator_)
                {
                  attrType.AttrTypeNum = MathML_ATTR_movablelimits;
                  attr = TtaGetAttribute (operator_, attrType);
                  if (attr)
                    /* the operator has an attribute movablelimits */
                    {
                      value = TtaGetAttributeValue (attr);
                      if (value == MathML_ATTR_movablelimits_VAL_true)
                        movable = TRUE;
                    }
                  else
                    /* no attribute movablelimits. Look at the content of the
                       operator element */
                    {
                      textEl = TtaGetFirstChild (operator_);
                      if (textEl)
                        {
                          elType = TtaGetElementType (textEl);
                          if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
                            {
                              len = TtaGetElementVolume (textEl);
                              if (len == 3)
                                {
                                  TtaGiveTextContent (textEl, (unsigned char *)buffer,
                                                      &len, &lang);
                                  {
                                    if (!strcmp (buffer, "lim") ||
                                        !strcmp (buffer, "max") ||
                                        !strcmp (buffer, "min"))
                                      movable = TRUE;
                                  }
                                }
                              else if (len == 1)
                                {
                                  TtaGiveBufferContent (textEl, text, len+1, &lang);
                                  if (text[0] == 0x22C1 /* Vee */ ||
                                      text[0] == 0x2296 /* CircleMinus */ ||
                                      text[0] == 0x2295 /* CirclePlus */ ||
                                      text[0] == 0x2211 /* Sum */ ||
                                      text[0] == 0x22C3 /* Union */ ||
                                      text[0] == 0x228E /* UnionPlus */ ||
                                      text[0] == 0x22C0 /* Wedge */ ||
                                      text[0] == 0x2297 /* CircleTimes */ ||
                                      text[0] == 0x2210 /* Coproduct */ ||
                                      text[0] == 0x220F /* Product */ ||
                                      text[0] == 0x22C2 /* Intersection */ ||
                                      text[0] == 0x2299 /* CircleDot */ )
                                    movable = TRUE;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

  attrType.AttrTypeNum = MathML_ATTR_IntMovelimits;
  attr = TtaGetAttribute (el, attrType);
  if (movable)
    {
      if (!attr)
        {
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (el, attr, doc);
        }
      TtaSetAttributeValue (attr, MathML_ATTR_IntMovelimits_VAL_yes_, el, doc);
    }
  else if (attr)
    TtaRemoveAttribute (el, attr, doc);
}

/*----------------------------------------------------------------------
  CheckMinus
  Element el is a MO. If it contains only the character '-', replace this
  character by a minus operator (Unicode x02212)
  ----------------------------------------------------------------------*/
static void  CheckMinus (Element el, Document doc)
{
  Element	      content;
  Language	      lang;
  CHAR_T              text[2];

  content = TtaGetFirstChild (el);
  if (content)
    {
      if (TtaGetElementType (content).ElTypeNum == MathML_EL_TEXT_UNIT)
	{
	  if (TtaGetElementVolume (content) == 1)
	    /* the MO element contains a single character */
	    {
	      TtaGiveBufferContent (content, text, 2, &lang);
	      if (text[0] == '-')
		{
		  text[0] = 0x2212;
                  TtaSetBufferContent (content, text, lang, doc);
		}
	    }
	}
    }
}

/*----------------------------------------------------------------------
  CheckLargeOp
  If el is a MO element,
  if it's a large operator (&Sum; for instance), put a presentation
  rule to enlarge the character.
  ----------------------------------------------------------------------*/
void      CheckLargeOp (Element el, Document doc)
{
  ElementType	      elType, contType;
  Element	      content, ancestor;
  AttributeType       attrType;
  Attribute	      attrLargeop, attr;
  Language	      lang;
  CHAR_T              text[2];
  int                 len, val;
  ThotBool            largeop;

  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == MathML_EL_MO)
    /* the element is a MO */
    {
      content = TtaGetFirstChild (el);
      if (content != NULL)
        {
          contType = TtaGetElementType (content);
          if (contType.ElTypeNum == MathML_EL_TEXT_UNIT)
            {
              len = TtaGetElementVolume (content);
              if (len == 1)
                /* the MO element contains a single character */
                {
                  TtaGiveBufferContent (content, text, len+1, &lang);
                  largeop = FALSE;
                  /* is there an attribute largeop on this MO element? */
                  attrType.AttrSSchema = elType.ElSSchema;
                  attrType.AttrTypeNum = MathML_ATTR_largeop;
                  attrLargeop = TtaGetAttribute (el, attrType);
                  if (attrLargeop)
                    /* there is an attribute largeop. Just take its value */
                    largeop = (TtaGetAttributeValue (attrLargeop) == MathML_ATTR_largeop_VAL_true);
                  else
                    /* no attribute largeop */
                    {
                      /* first look for an IntDisplaystyle attribute on an ancestor */
                      attrType.AttrSSchema = elType.ElSSchema;
                      attrType.AttrTypeNum = MathML_ATTR_IntDisplaystyle;
                      ancestor = el;
                      do
                        {
                          attr = TtaGetAttribute (ancestor, attrType);
                          if (!attr)
                            ancestor = TtaGetParent(ancestor);
                        }
                      while (!attr && ancestor);
                      if (attr)
                        /* there is an ancestor with an attribute IntDisplaystyle */
                        {
                          val = TtaGetAttributeValue (attr);
                          if (val == MathML_ATTR_IntDisplaystyle_VAL_true)
                            /* an ancestor has an attribute IntDisplaystyle = true */
                            /* Look at the symbol */
                            largeop = IsLargeOp (text[0]);
                        }
                    }
                  attrType.AttrTypeNum = MathML_ATTR_IntLargeOp;
                  attrLargeop = TtaGetAttribute (el, attrType);
                  if (largeop)
                    /* it's a large operator. Set the IntLargeOp attribute if
		       it's not already set */
                    {
		      if (!attrLargeop)
			{
			  attrLargeop = TtaNewAttribute (attrType);
			  TtaSetAttributeValue (attrLargeop,
						MathML_ATTR_IntLargeOp_VAL_yes_,
						el, doc);
			  TtaAttachAttribute (el, attrLargeop, doc);
			}
                    }
                  else
                    /* it's not a large operator, remove the IntLargeOp
                       attribute if it's present */
		    if (attrLargeop)
		      TtaRemoveAttribute (el, attrLargeop, doc);
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  ApplyDisplaystyle
  An IntDisplaystyle attribute has been associated with (or removed from)
  element el.
  Handle all large operators in the sub tree.
  Update attribute IntMovelimits for all munder, mover or munderover
  elements in the subtree
  ----------------------------------------------------------------------*/
static void   ApplyDisplaystyle (Element el, Document doc)
{
  ElementType  elType;
  Element      child;

  if (el)
    {
      elType = TtaGetElementType (el);
      if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
        {
          if (elType.ElTypeNum == MathML_EL_MO)
            CheckLargeOp (el, doc);
          else if (elType.ElTypeNum == MathML_EL_MUNDER ||
                   elType.ElTypeNum == MathML_EL_MOVER ||
                   elType.ElTypeNum == MathML_EL_MUNDEROVER)
            SetIntMovelimitsAttr (el, doc);
        }
      child = TtaGetFirstChild (el);
      while (child)
        {
          ApplyDisplaystyle (child, doc);
          TtaNextSibling (&child);
        }
    }
}

/*----------------------------------------------------------------------
  CheckIntDisplaystyle
  Internal element el has just been created by function
  CheckMathSubExpressions. If this new element has an implicit (see MathML.S)
  attribute IntDisplaystyle=false, set the size of large operators and
  update attribute IntMovelimit in the whole subtree.
  ----------------------------------------------------------------------*/
static void  CheckIntDisplaystyle (Element el, Document doc)
{
  ElementType  elType;

  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == MathML_EL_Numerator ||
      elType.ElTypeNum == MathML_EL_Denominator ||
      elType.ElTypeNum == MathML_EL_Index ||
      elType.ElTypeNum == MathML_EL_Subscript ||
      elType.ElTypeNum == MathML_EL_Superscript ||
      elType.ElTypeNum == MathML_EL_Underscript ||
      elType.ElTypeNum == MathML_EL_Overscript ||
      elType.ElTypeNum == MathML_EL_PostscriptPairs ||
      elType.ElTypeNum == MathML_EL_PrescriptPairs ||
      elType.ElTypeNum == MathML_EL_MTable_body)
    {
      ApplyDisplaystyle (el, doc);
    }
}

/*----------------------------------------------------------------------
  CheckMathSubExpressions
  Children of element el should be of type type1, type2, and type3.
  If they are not, wrap them in elements of these types.
  If element el has too many or not enough children, return FALSE.
  ----------------------------------------------------------------------*/
static ThotBool CheckMathSubExpressions (Element el, int type1, int type2, int type3, Document doc)
{
  Element	child, new_, prev;
  ElementType	elType, childType;
  char          msgBuffer[200];
  ThotBool      result;

  result = TRUE;
  elType.ElSSchema = GetMathMLSSchema (doc);
  child = TtaGetFirstChild (el);
  prev = NULL;
  NextNotComment (&child, &prev);
  if (type1 == 0)
    {
      if (child)
        /* no child expected and there is one, error */
        {
          sprintf (msgBuffer, "No subexpression allowed in %s",
                   TtaGetElementTypeName (TtaGetElementType (el)));
          XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
          result = FALSE;
        }
    }
  else
    if (!child)
      /* a first child is expected and it's missing */
      {
        sprintf (msgBuffer, "Missing subexpression in %s",
                 TtaGetElementTypeName (TtaGetElementType (el)));
        XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
        result = FALSE;
      }
    else
      {
        elType.ElTypeNum = type1;
        childType = TtaGetElementType (child);
        if (!TtaSameTypes(childType, elType))
          {
            TtaRemoveTree (child, doc);	
            new_ = TtaNewElement (doc, elType);
            if (prev == NULL)
              TtaInsertFirstChild (&new_, el, doc);
            else
              TtaInsertSibling (new_, prev, FALSE, doc);
            TtaInsertFirstChild (&child, new_, doc);
            CreatePlaceholders (child, doc);
            child = new_;
            CheckIntDisplaystyle (new_, doc);
          }
        prev = child;
        TtaNextSibling (&child);
        NextNotComment (&child, &prev);
        if (type2 == 0)
          {
            if (child)
              /* this second child is not expected, error */
              {
                sprintf (msgBuffer, "Only 1 subexpression allowed in %s",
                         TtaGetElementTypeName (TtaGetElementType (el)));
                XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
                result = FALSE;
              }
          }
        else
          {
            if (!child)
              /* a second child is expected and it's missing */
              {
                sprintf (msgBuffer, "2 subexpressions required in %s",
                         TtaGetElementTypeName (TtaGetElementType (el)));
                XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
                result = FALSE;
              }
            else
              {
                elType.ElTypeNum = type2;
                childType = TtaGetElementType (child);
                if (!TtaSameTypes(childType, elType))
                  {
                    TtaRemoveTree (child, doc);
                    new_ = TtaNewElement (doc, elType);
                    TtaInsertSibling (new_, prev, FALSE, doc);
                    TtaInsertFirstChild (&child, new_, doc);
                    CreatePlaceholders (child, doc);
                    child = new_;
                    CheckIntDisplaystyle (new_, doc);
                  }
                prev = child;
                TtaNextSibling (&child);
                NextNotComment (&child, &prev);
                if (type3 == 0)
                  {
                    if (child)
                      /* this third child is not expected, error */
                      {
                        sprintf (msgBuffer, "Only 2 subexpressions allowed in %s",
                                 TtaGetElementTypeName (TtaGetElementType (el)));
                        XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
                        result = FALSE;
                      }
                  }
                else
                  {
                    if (!child)
                      /* a third child is expected and it's missing */
                      {
                        sprintf (msgBuffer, "3 subexpressions required in %s",
                                 TtaGetElementTypeName (TtaGetElementType (el)));
                        XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
                        result = FALSE;
                      }
                    else
                      {
                        elType.ElTypeNum = type3;
                        childType = TtaGetElementType (child);
                        if (!TtaSameTypes (childType, elType))
                          {
                            TtaRemoveTree (child, doc);
                            new_ = TtaNewElement (doc, elType);
                            TtaInsertSibling (new_, prev, FALSE, doc);
                            TtaInsertFirstChild (&child, new_, doc);
                            CreatePlaceholders (child, doc);
                            child = new_;
                            CheckIntDisplaystyle (new_, doc);
                          }
                      }
                    prev = child;
                    TtaNextSibling (&child);
                    NextNotComment (&child, &prev);
                    if (child)
                      /* this fourth child is unexpected */
                      {
                        sprintf (msgBuffer,"Only 3 subexpressions allowed in %s",
                                 TtaGetElementTypeName (TtaGetElementType (el)));
                        XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
                        result = FALSE;
                      }
                  }
              }
          }
      }
  return result;
}

/*----------------------------------------------------------------------
  SetSingleIntHorizStretchAttr

  Put a IntHorizStretch attribute on element el if it contains only
  a MO element that is a stretchable symbol.
  -----------------------------------------------------------------------*/
void SetSingleIntHorizStretchAttr (Element el, Document doc, Element* selEl)
{
  Element	child, sibling, textEl, symbolEl;
  ElementType	elType, childType, siblingType;
  Attribute	attr;
  AttributeType	attrType;
  Language	lang;
  CHAR_T        text[2];
  char	        script;
  unsigned char c;
  int		len;
  ThotBool      doit;

  if (el == NULL)
    return;
  child = TtaGetFirstChild (el);
  textEl = NULL;
  siblingType.ElTypeNum = 0;
  siblingType.ElSSchema = NULL;
  if (child)
    {
      elType = TtaGetElementType (child);
      /* skip empty Constructs (placeholders) and comments */
      while (child &&
             (elType.ElTypeNum == MathML_EL_Construct ||
              elType.ElTypeNum == MathML_EL_XMLcomment) &&
             !strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
        {
          TtaNextSibling (&child);
          if (child)
            elType = TtaGetElementType (child);
        }
      while (child && elType.ElTypeNum == MathML_EL_MROW &&
             strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
        /* the first significant child is a mrow. Check whether it has a
           single child */
        {
          child = TtaGetFirstChild (child);
          if (child)
            {
              elType = TtaGetElementType (child);
              /* skip empty Constructs (placeholders) and comments */
              while (child &&
                     (elType.ElTypeNum == MathML_EL_Construct ||
                      elType.ElTypeNum == MathML_EL_XMLcomment) &&
                     !strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
                {
                  TtaNextSibling (&child);
                  if (child)
                    elType = TtaGetElementType (child);
                }
              sibling = child;
              TtaNextSibling (&sibling);
              if (sibling)
                /* there are other children */
                {
                  siblingType = TtaGetElementType (sibling);
                  while (sibling &&
                         (siblingType.ElTypeNum == MathML_EL_Construct ||
                          siblingType.ElTypeNum == MathML_EL_XMLcomment) &&
                         !strcmp (TtaGetSSchemaName (siblingType.ElSSchema), "MathML"))
                    /* its an empty construct (placeholder) or a comment, skip it*/
                    {
                      TtaNextSibling (&sibling);
                      if (sibling)
                        siblingType = TtaGetElementType (sibling);
                    }
                  if (sibling)
                    /* there are significant siblings */
                    child = NULL;
                }
            }
        }
      if (child && (elType.ElTypeNum == MathML_EL_MO ||
                    elType.ElTypeNum == MathML_EL_MOVER ||
                    elType.ElTypeNum == MathML_EL_MUNDER))
        /* the first child is a MO, a MUNDER or a MOVER */
        {
          sibling = child;
          TtaNextSibling (&sibling);
          if (sibling)
            siblingType = TtaGetElementType (sibling);
          /* skip empty Constructs (placeholders) and comments */
          while (sibling &&
                 (siblingType.ElTypeNum == MathML_EL_Construct ||
                  siblingType.ElTypeNum == MathML_EL_XMLcomment) &&
                 !strcmp (TtaGetSSchemaName (siblingType.ElSSchema), "MathML"))
            {
              TtaNextSibling (&sibling);
              if (sibling)
                siblingType = TtaGetElementType (sibling);
            }
          if (sibling == NULL)
            /* there is no other significant child */
            {
              c = EOS;
              doit = FALSE;
              attrType.AttrSSchema = elType.ElSSchema;
              attrType.AttrTypeNum = MathML_ATTR_IntHorizStretch;

              if (elType.ElTypeNum == MathML_EL_MOVER ||
                  elType.ElTypeNum == MathML_EL_MUNDER)
                /* check if the UnderOverBase has a IntHorizStretch attribute */
                {
                  childType.ElTypeNum = MathML_EL_UnderOverBase;
                  childType.ElSSchema = elType.ElSSchema;
                  textEl = TtaSearchTypedElement (childType, SearchInTree, child);
                  if (textEl)
                    {
                      if (TtaGetAttribute (textEl, attrType))
                        doit = TRUE;
                    }
                }
              else if (elType.ElTypeNum == MathML_EL_MO)
                {
                  textEl = TtaGetFirstChild (child);
                  childType = TtaGetElementType (textEl);
                  if (childType.ElTypeNum == MathML_EL_TEXT_UNIT)
                    /* the MO child contains a TEXT element */
                    {
                      len = TtaGetElementVolume (textEl);
                      if (len == 1)
                        /* the TEXT element contains a single character */
                        {
                          /* get that character */
                          len = 2;
                          TtaGiveBufferContent (textEl, text, len, &lang);
                          script = TtaGetScript (lang);
                          switch(text[0])
                            {
                            case 0x2500: /* HorizontalLine */
                              c = 8; 
                            break;
                            case 0x0332: /* UnderBar */
                              c = 9;
                            break;
                            case 0xAF: /* OverBar */
                              c = 10;
                            break;
                            case 0x302:
                              c = 'H'; /* Hat */
                            break;
                            case 0x2C7:
                              c = 'k'; /* Hacek */
                            break;
                            case 0x2DC:
                              c = 'T'; /* Diacritical Tilde */
                            break;
                            case 0xFE37:
                              c = 'o';  /* Over brace */
                            break;
                            case 0xFE38:
                              c = 'u';  /* Under brace */
                            break;
                            case 0xFE35:
                              c = 'p';  /* Over parenthesis */
                            break;
                            case 0xFE36:
                              c = 'q';  /* Under parenthesis */
                            break;
                            case 0x23B4:
                              c = 'b';  /* Over bracket */
                            break;
                            case 0x23B5:
                              c = 'B';  /* Under bracket */
                            break;

                            case 0x2190: /* LeftArrow */
                              c = 'L';
                            break;
                            case 0x2192: /* RightArrow */
                              c = 'R';
                            break;
                            case 0x2194: /* LeftRightArrow */
                              c = 'A';
                            break;
#ifdef STRETCHY_ARROWS // workaround for Bug 8890 - fred
                            case 0x21A4: /* LeftTeeArrow */
                              c = 160;
                            break;
                            case 0x21A6: /* RightTeeArrow */
                              c = 162;
                            break;
                            case 0x21BC: /* LeftVector */
                              c = 164;
                            break;
                            case 0x21BD: /* DownLeftVector */
                              c = 165;
                            break;
                            case 0x21C0: /* RightVector */
                              c = 168;
                            break;
                            case 0x21C1: /* DownRightVector */
                              c = 169;
                            break;
                            case 0x21C4: /* RightArrowLeftArrow */
                              c = 172;
                            break;
                            case 0x21C6: /* LeftArrowRightArrow */
                              c = 174;
                            break;
                            case 0x21D0: /* DoubleLeftArrow */
                              c = 175;
                            break;
                            case 0x21D2: /* DoubleRightArrow */
                              c = 177;
                            break;
                            case 0x21D4: /* DoubleLeftRightArrow */
                              c = 179;
                            break;
                            case 0x21E4: /* LeftArrowBar */
                              c = 181;
                            break;
                            case 0x21E5: /* RightArrowBar */
                              c = 182;
                            break;
                            case 0x27F5: /* LongLeftArrow */
                              c = 184;
                            break;
                            case 0x27F6: /* LongRightArrow */
                              c = 185;
                            break;
                            case 0x27F7: /* LongLeftRightArrow */
                              c = 186;
                            break;
                            case 0x27F8: /* DoubleLongLeftArrow */
                              c = 187;
                            break;
                            case 0x27F9: /* DoubleLongRightArrow */
                              c = 188;
                            break;
                            case 0x27FA: /* DoubleLongLeftRightArrow */
                              c = 189;
                            break;
                            case 0x294E : /* LeftRightVector */
                              c = 192;
                            break;
                            case 0x2950: /* DownLeftRightVector */
                              c = 194;
                            break;
                            case 0x2952: /* LeftVectorBar */
                              c = 196;
                            break;
                            case 0x2953: /* RightVectorBar */
                              c = 197;
                            break;
                            case 0x2956: /* DownLeftVectorBar */
                              c = 200;
                            break;
                            case 0x2957: /* DownRightVectorBar */
                              c = 201;
                            break;
                            case 0x295A: /* LeftTeeVector */
                              c = 204;
                            break;
                            case 0x295B: /* RightTeeVector */
                              c = 205;
                            break;
                            case 0x295E: /* DownLeftTeeVector */
                              c = 208;
                            break;
                            case 0x295F: /* DownRightTeeVector */
                              c = 209;
                            break;
                            case 0x21CC: /* Equilibrium */
                              c = 214;
                            break;
                            case 0x21CB: /* ReverseEquilibrium */
                              c = 215;
                            break;
#endif

                            default:
                            break;
                            }

                          if (c != EOS ||
                              (text[0] >= 0x2196 && text[0] <= 0x2199)) /* diagonal arrows */
                            doit = TRUE;
                        }
                    }
                }
              if (doit)
                {
                  /* attach a IntHorizStretch attribute to the element
                     (UnderOverBase, Underscript, or Overscript) */
                  attr = TtaNewAttribute (attrType);
                  TtaSetAttributeValue (attr, MathML_ATTR_IntHorizStretch_VAL_yes_, el, doc);
                  TtaAttachAttribute (el, attr, doc);
                  attr = TtaNewAttribute (attrType);
                  TtaSetAttributeValue (attr, MathML_ATTR_IntHorizStretch_VAL_yes_, child, doc);
                  TtaAttachAttribute (child, attr, doc);
                }
              if (c != EOS)
                {
                  /* replace the TEXT element by a Thot SYMBOL element */
                  childType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
                  symbolEl = TtaNewElement (doc, childType);
                  TtaInsertSibling (symbolEl, textEl, FALSE, doc);
                  if (selEl != NULL)
                    if (*selEl == textEl)
                      *selEl = symbolEl;
                  TtaDeleteTree (textEl, doc);
                  TtaSetGraphicsShape (symbolEl, c, doc);
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  SetIntHorizStretchAttr

  Put a IntHorizStretch attribute on all children of element el that
  contain only a MO element that is a stretchable symbol.
  -----------------------------------------------------------------------*/
static void SetIntHorizStretchAttr (Element el, Document doc)
{
  Element	child;

  if (el == NULL)
    return;
  child = TtaGetFirstChild (el);
  while (child != NULL)
    {
      SetSingleIntHorizStretchAttr (child, doc, NULL);
      TtaNextSibling (&child);
    }
}

/*----------------------------------------------------------------------
  SetIntVertStretchAttr

  Put a IntVertStretch attribute on element el if its base element
  (Base for a MSUBSUP, MSUP or MSUB; UnderOverBase for a MUNDEROVER,
  a MUNDER of a MOVER) contains only a MO element that is a vertically
  stretchable symbol (integral, vertical arrow, etc).
  -----------------------------------------------------------------------*/
void SetIntVertStretchAttr (Element el, Document doc, int base, Element* selEl)
{
  Element	child, sibling, textEl, symbolEl, parent, operator_, next;
  ElementType	elType;
  Attribute	attr;
  AttributeType	attrType;
  SSchema       MathMLSSchema;
  Language	lang;
  char		script;
#define buflen 50
  CHAR_T        text[buflen];
  unsigned char c;
  int		len, i;
  ThotBool      inbase, stretchable;

  if (el == NULL)
    return;
  operator_ = NULL;
  inbase = FALSE;
  MathMLSSchema = TtaGetElementType(el).ElSSchema;
  symbolEl = parent = NULL;
  if (base == 0)
    /* it's a MO */
    {
      parent = TtaGetParent (el);
      if (parent != NULL)
        {
          /* don't process the mo if it is within a base.�It will be processed
             when the enclosing construct is processed (see below) */
          elType = TtaGetElementType (parent);
          if (elType.ElSSchema != MathMLSSchema ||
              (elType.ElTypeNum != MathML_EL_Base &&
               elType.ElTypeNum != MathML_EL_UnderOverBase &&
               elType.ElTypeNum != MathML_EL_MSUBSUP &&
               elType.ElTypeNum != MathML_EL_MSUB &&
               elType.ElTypeNum != MathML_EL_MSUP &&
               elType.ElTypeNum != MathML_EL_MUNDEROVER &&
               elType.ElTypeNum != MathML_EL_MUNDER &&
               elType.ElTypeNum != MathML_EL_MUNDEROVER &&
               elType.ElTypeNum != MathML_EL_MTD))
            operator_ = el;
        }
    }
  else
    /* it's not a MO */
    {
      /* search the Base or UnderOverBase child */
      child = TtaGetFirstChild (el);
      if (child != NULL)
        {
          elType = TtaGetElementType (child);
          /* skip empty Constructs (placeholders) and comments */
          while (child &&
                 (elType.ElTypeNum == MathML_EL_Construct ||
                  elType.ElTypeNum == MathML_EL_XMLcomment) &&
                 elType.ElSSchema == MathMLSSchema)
            {
              TtaNextSibling (&child);
              if (child)
                elType = TtaGetElementType (child);
            }

          if (elType.ElTypeNum == base && elType.ElSSchema == MathMLSSchema)
            /* the first child is a Base or UnderOverBase */
            {
              child = TtaGetFirstChild (child);
              if (child != NULL)
                {
                  elType = TtaGetElementType (child);
                  /* skip empty Constructs (placeholders) and comments */
                  while (child &&
                         (elType.ElTypeNum == MathML_EL_Construct ||
                          elType.ElTypeNum == MathML_EL_XMLcomment) &&
                         elType.ElSSchema == MathMLSSchema)
                    {
                      TtaNextSibling (&child);
                      if (child)
                        elType = TtaGetElementType (child);
                    }

                  if (elType.ElTypeNum == MathML_EL_MO &&
                      elType.ElSSchema == MathMLSSchema)
                    /* its first significant child is a MO */
                    {
                      sibling = child;
                      TtaNextSibling (&sibling);
                      /* skip empty Constructs (placeholders) and comments */
                      while (sibling &&
                             (elType.ElTypeNum == MathML_EL_Construct ||
                              elType.ElTypeNum == MathML_EL_XMLcomment) &&
                             elType.ElSSchema == MathMLSSchema)
                        {
                          TtaNextSibling (&sibling);
                          if (sibling)
                            elType = TtaGetElementType (sibling);
                        }

                      if (sibling == NULL)
                        /* there is no other significant child */
                        {
                          operator_ = child;
                          if (base == MathML_EL_Base ||
                              base == MathML_EL_UnderOverBase)
                            {
                              parent = el;
                              inbase = TRUE;
                            }
                        }
                    }
                }
            }
        }
    }
  if (operator_)
    {
      textEl = TtaGetFirstChild (operator_);
      if (textEl != NULL)
        {
          elType = TtaGetElementType (textEl);
          if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
            {
              len = TtaGetElementVolume (textEl);
              if (len >= 1)
                {
                  if (len >= buflen)
                    len = buflen-1;
                  TtaGiveBufferContent (textEl, text, len+1, &lang);
                  script = TtaGetScript (lang);
                  stretchable = TRUE;
                  for (i = 0; i < len; i++)
                    if (!(
                       (text[i] >= 0x222B && text[i] <= 0x2233) || /* Integrals */
                       text[i] == 0x2191  || /* UpArrow */
                       text[i] == 0x2193  || /* DownArrow */
                       text[i] == 0x2195 || /* UpDownArrow */
                       (text[i] >= 0x2196 && text[i] <= 0x2199) || /* diagonal arrows */
                       text[i] == 0x21A5 || /* UpTeeArrow */
                       text[i] == 0x21A7 || /* DownTeeArrow */
                       text[i] == 0x21BE || /* RightUpVector */
                       text[i] == 0x21BF || /* LeftUpVector */
                       text[i] == 0x21C2 || /* RightDownVector */
                       text[i] == 0x21C3 || /* LeftDownVector */
                       text[i] == 0x21C5 || /* UpArrowDownArrow */
                       text[i] == 0x21D1 || /* DoubleUpArrow */
                       text[i] == 0x21D3 || /* DoubleDownArrow */
                       text[i] == 0x21D5 || /* DoubleUpDownArrow */
                       text[i] == 0x21F5 || /* DownArrowUpArrow */
                       text[i] == 0x2912 || /* UpArrowBar */
                       text[i] == 0x2913 || /* DownArrowBar */
                       text[i] == 0x294F || /* RightUpDownVector */
                       text[i] == 0x2951 || /* LeftUpDownVector */
                       text[i] == 0x2954 || /* RightUpVectorBar */
                       text[i] == 0x2955 || /* RightDownVectorBar */
                       text[i] == 0x2958 || /* LeftUpVectorBar */
                       text[i] == 0x2959 || /* LeftDownVectorBar */
                       text[i] == 0x295C || /* RightUpTeeVector */
                       text[i] == 0x295D || /* RightDownTeeVector */
                       text[i] == 0x2960 || /* LeftUpTeeVector */
                       text[i] == 0x2961 || /* LeftDownTeeVector */
                       text[i] == 0x296E || /* UpEquilibrium */
                       text[i] == 0x296F || /* ReverseUpEquilibrium */
                       text[i] == 0x2758 || /* VerticalSeparator */
                       text[i] == 0x2223 || /* VerticalSeparator */
		       text[i] == 0x007C    /* vertical line */
                       ))/* accept only symbols like simple integral, double or
                         triple integral, contour integral, etc. or vertical
                         arrows (add more arrows *****) */
                      stretchable = FALSE;
                  if (stretchable)
                    /* the operator contains only stretchable symbols */
                    {
                      /* replace the stretchable characters by a Thot SYMBOL
                         element. If there are several such characters in
                         the mo (multiple integral for instance), replace
                         them too. */
                      do
                        {
                          /* replace the TEXT element by a Thot SYMBOL */
                          elType.ElSSchema = MathMLSSchema;
                          elType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
                          for (i = 0; i < len; i++)
                            {
                              switch(text[i])
                                {
                                case 0x222B: /* integral */
                                  c = 'i';
                                break;
                                case 0x222C: /* double integral */
                                  c = 'd';
                                break;
                                case 0x222D: /* triple integral */
                                  c = 't';
                                break;
                                case 0x222E: /* contour integral */
                                  c = 'c';
                                break;
                                case 0x222F: /* double contour integral */
                                  c = 'e';
                                break;
                                case 0x2230: /* triple contour integral */
                                  c = 'f';
                                break;
                                case 0x2231: /* Clockwise Integral */
                                  c = '1';
                                break;
                                case 0x2232: /* Clockwise Contour Integral */
                                  c = '2';
                                break;
                                case 0x2233: /* Counter Clockwise Contour Integral */
                                  c = '3';
                                break;

                                case 0x2191: /* UpArrow */
                                  c = '^';
                                break;
                                case 0x2193: /* DownArrow */
                                  c = 'V';
                                break;
                                case 0x2195: /* UpDownArrow */
                                  c = 155;
                                break;
#ifdef STRETCHY_ARROWS // workaround for Bug 8890 - fred
                                case 0x2196: /* UpperLeftArrow */
                                  c = 156;
                                break;
                                case 0x2197: /* UpperRightArrow */
                                  c = 157;
                                break;
                                case 0x2198: /* LowerRightArrow */
                                  c = 158;
                                break;
                                case 0x2199: /* LowerLeftArrow */
                                  c = 159;
                                break;
                                case 0x21A5: /* UpTeeArrow */
                                  c = 161;
                                break;
                                case 0x21A7: /* DownTeeArrow */
                                  c = 163;
                                break;
                                case 0x21BE: /* RightUpVector */
                                  c = 166;
                                break;
                                case 0x21BF: /* LeftUpVector */
                                  c = 167;
                                break;
                                case 0x21C2: /* RightDownVector */
                                  c = 170;
                                break;
                                case 0x21C3: /* LeftDownVector */
                                  c = 171;
                                break;
                                case 0x21C5: /* UpArrowDownArrow */
                                  c = 173;
                                break;
                                case 0x21D1: /* DoubleUpArrow */
                                  c = 176;
                                break;
                                case 0x21D3: /* DoubleDownArrow */
                                  c = 178;
                                break;
                                case 0x21D5: /* DoubleUpDownArrow */
                                  c = 180;
                                break;
                                case 0x21F5: /* DownArrowUpArrow */
                                  c = 183;
                                break;
                                case 0x2912: /* UpArrowBar */
                                  c = 190;
                                break;
                                case 0x2913 : /* DownArrowBar */
                                  c = 191;
                                break;
                                case 0x294F: /* RightUpDownVector */
                                  c = 193;
                                break;
                                case 0x2951: /* LeftUpDownVector */
                                  c = 195;
                                break;
                                case 0x2954: /* RightUpVectorBar */
                                  c = 198;
                                break;
                                case 0x2955: /* RightDownVectorBar */
                                  c = 199;
                                break;
                                case 0x2958: /* LeftUpVectorBar */
                                  c = 202;
                                break;
                                case 0x2959: /* LeftDownVectorBar */
                                  c = 203;
                                break;
                                case 0x295C: /* RightUpTeeVector */
                                  c = 206;
                                break;
                                case 0x295D: /* RightDownTeeVector */
                                  c = 207;
                                break;
                                case 0x2960: /* LeftUpTeeVector */
                                  c = 210;
                                break;
                                case 0x2961: /* LeftDownTeeVector */
                                  c = 211;
                                break;
                                case 0x296E: /* UpEquilibrium */
                                  c = 212;
                                break;
                                case 0x296F: /* ReverseUpEquilibrium */
                                  c = 213;
                                break;
#endif
                                case 0x2758: /* VerticalSeparator */
                                  c = 7;
				break;
                                case 0x2223: /* VerticalBar */
				case 0x007C: /* Vertical Line */
                                  c = 11;
                                break;
                                case 0x2956: /* DoubleVerticalBar */
                                  c = 12;
                                break;

                                default:
                                  c = (unsigned char) text[i];
                                break;
                                }
                              symbolEl = TtaNewElement (doc, elType);
                              TtaSetGraphicsShape (symbolEl, c, doc);
                              TtaInsertSibling (symbolEl, textEl, TRUE,doc);
                              TtaRegisterElementCreate (symbolEl, doc);
                              if (selEl != NULL)
                                if (*selEl == textEl)
                                  *selEl = symbolEl;
                            }
                          TtaRegisterElementDelete (textEl, doc);
                          TtaDeleteTree (textEl, doc);
                          /* is there another text element after the
                             stretchable symbol? */
                          textEl = symbolEl; TtaNextSibling (&textEl);
                          if (textEl)
                            {
                              elType = TtaGetElementType (textEl);
                              if (elType.ElTypeNum != MathML_EL_TEXT_UNIT)
                                textEl = NULL;
                              else
                                /* there is another text element.
                                   Is it a single stretchable symbol? */
                                {
                                  len = TtaGetElementVolume (textEl);
                                  if (len < 1)
                                    /* not a single character */
                                    textEl = NULL;
                                  else
                                    {
                                      if (len >= buflen)
                                        len = buflen-1;
                                      TtaGiveBufferContent (textEl, text,
                                                            len+1, &lang); 
                                      script = TtaGetScript (lang);
                                      if (text[0] != 0x007C &&  // vertical bar
					  text[0] != 0x2223 &&
					  text[0] != 0x222B &&  // integrals
                                          text[0] != 0x222C &&
                                          text[0] != 0x222D &&
                                          text[0] != 0x222E &&
                                          text[0] != 0x222F &&
                                          text[0] != 0x2230 &&
                                          text[0] != 0x2231 &&
                                          text[0] != 0x2232 &&
                                          text[0] != 0x2233 &&
                                          text[0] != 0x2191 &&  // vertical arrows
                                          text[0] != 0x2193)
                                        /* not a stretchable symbol */
                                        textEl = NULL;
                                    }
                                }
                            }
                        }
                      while (textEl);
		      

                      /* attach a IntVertStretch attribute */
                      attrType.AttrSSchema = MathMLSSchema;
                      attrType.AttrTypeNum = MathML_ATTR_IntVertStretch;
                      attr = TtaNewAttribute (attrType);
                      TtaAttachAttribute (el, attr, doc);
                      TtaSetAttributeValue (attr,
                                            MathML_ATTR_IntVertStretch_VAL_yes_,
                                            el, doc);
                      TtaRegisterAttributeCreate (attr, el, doc);
                      if (inbase)
                        /* it's within a Base or UnderOverBase element */
                        {
                          sibling = parent;
                          TtaNextSibling (&sibling);
                          if (sibling)
                            /* the msubsup of munderover element has a next
                               sibling */
                            {
                              elType = TtaGetElementType (sibling);
                              if (elType.ElTypeNum == MathML_EL_Construct &&
                                  elType.ElSSchema == MathMLSSchema)
                                /* the next sibling is a Construct */
                                {
                                  next = sibling;
                                  TtaNextSibling (&next);
                                  if (!next)
                                    /* there is no other sibling after the
                                       Construct. Change it into Construct1*/
                                    {
                                      TtaRegisterElementDelete (sibling, doc);
                                      TtaRemoveTree (sibling, doc);
                                      TtaChangeElementType (sibling,
                                                            MathML_EL_Construct1);
                                      TtaInsertSibling (sibling, parent,
                                                        FALSE, doc);
                                      TtaRegisterElementCreate (sibling, doc);
                                      /* force the msubsup element to be
                                         reformatted and to take into account
                                         its new next sibling */
                                      TtaRemoveTree (parent, doc);
                                      TtaInsertSibling (parent, sibling, TRUE, doc);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  SetIntPlaceholderAttr

  Put a IntPlaceholder attribute on all Construct elements in the
  subtree of root el.
  -----------------------------------------------------------------------*/
static void SetIntPlaceholderAttr (Element el, Document doc)
{
  Element	child;
  ElementType	elType;
  Attribute	attr;
  AttributeType	attrType;

  if (el == NULL)
    return;
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == MathML_EL_Construct &&
      elType.ElSSchema == GetMathMLSSchema (doc))
    {
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = MathML_ATTR_IntPlaceholder;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
      TtaSetAttributeValue (attr, MathML_ATTR_IntPlaceholder_VAL_yes_, el, doc);
    }
  else
    {
      child = TtaGetFirstChild (el);
      while (child != NULL)
        {
          SetIntPlaceholderAttr (child, doc);
          TtaNextSibling (&child);
        }
    }
}

/*----------------------------------------------------------------------
  BuildMultiscript

  The content of a MMULTISCRIPT element has been created following
  the original MathML structure.  Create all Thot elements defined
  in the MathML S schema.
  -----------------------------------------------------------------------*/
static void BuildMultiscript (Element elMMULTISCRIPT, Document doc)
{
  Element	elem, base, next, group, pair, script, prevPair, prevScript,
                prev, comment;
  ElementType	elType, elTypeGroup, elTypePair, elTypeScript;
  SSchema       MathMLSSchema;
  base = NULL;
  group = NULL;
  prevPair = NULL;
  prevScript = NULL;

  MathMLSSchema = GetMathMLSSchema (doc);
  elTypeGroup.ElSSchema = MathMLSSchema;
  elTypePair.ElSSchema = MathMLSSchema;
  elTypeGroup.ElTypeNum = 0;
  elTypePair.ElTypeNum = 0;
  elTypeScript.ElSSchema = MathMLSSchema;
  prev = NULL;

  /* skip the initial comments in the MMULTISCRIPT element */
  elem = TtaGetFirstChild (elMMULTISCRIPT);
  elType = TtaGetElementType (elem);
  while (elem &&
	 (elType.ElTypeNum == MathML_EL_XMLcomment) &&
	 elType.ElSSchema == MathMLSSchema)
    {
      prev = elem;
      TtaNextSibling (&elem);
      if (elem)
	elType = TtaGetElementType (elem);
    }

  /* process all children of the MMULTISCRIPT element */
  while (elem)
    {
      /* remember the element to be processed after the current one */
      next = elem;
      TtaNextSibling (&next);

      /* remove the current element from the tree */
      TtaRemoveTree (elem, doc);

      if (base == NULL)
        /* the current element is the first child of the MMULTISCRIPT
           element */
        {
          /* Create a MultiscriptBase element as the first child of
             MMULTISCRIPT and move the current element as the first child
             of the MultiscriptBase element */
          elTypeGroup.ElTypeNum = MathML_EL_MultiscriptBase;
          base = TtaNewElement (doc, elTypeGroup);
	  if (prev)
	    TtaInsertSibling (base, prev, FALSE, doc);
	  else
	    TtaInsertFirstChild (&base, elMMULTISCRIPT, doc);
          TtaInsertFirstChild (&elem, base, doc);
        }
      else
        /* the current element is a subscript or a superscript */
        {
          if (group == NULL)
            /* there is no PostscriptPairs element. Create one */
            {
              elTypeGroup.ElTypeNum = MathML_EL_PostscriptPairs;
              group = TtaNewElement (doc, elTypeGroup);
              TtaInsertSibling (group, base, FALSE, doc);
              elTypePair.ElTypeNum = MathML_EL_PostscriptPair;
              /* create a first and a last PostscriptPair as placeholders */
              pair = TtaNewTree (doc, elTypePair, "");
              TtaInsertFirstChild (&pair, group, doc);
              SetIntPlaceholderAttr (pair, doc);
              prevPair = pair;
              pair = TtaNewTree (doc, elTypePair, "");
              TtaInsertSibling (pair, prevPair, FALSE, doc);
              SetIntPlaceholderAttr (pair, doc);
              prevScript = NULL;
            }
          if (prevScript == NULL)
            /* the current element is the first subscript or superscript
               in a pair */
            {
              /* create a PostscriptPair or PrescriptPair element */
              pair = TtaNewElement (doc, elTypePair);
              if (prevPair == NULL)
                TtaInsertFirstChild (&pair, group, doc);
              else
                TtaInsertSibling (pair, prevPair, FALSE, doc);
              prevPair = pair;
              /* create a MSubscript element */
              elTypeScript.ElTypeNum = MathML_EL_MSubscript;
              script = TtaNewElement (doc, elTypeScript);
              TtaInsertFirstChild (&script, pair, doc);
              prevScript = script;	  
            }
          else
            /* the current element is a superscript in a pair */
            {
              /* create a MSuperscript element */
              elTypeScript.ElTypeNum = MathML_EL_MSuperscript;
              script = TtaNewElement (doc, elTypeScript);
              /* insert it as a sibling of the previous MSubscript element */
              TtaInsertSibling (script, prevScript, FALSE, doc);
              prevScript = NULL;	  
            }
          /* insert the current element as a child of the new MSuperscript or
             MSubscript element */
          TtaInsertFirstChild (&elem, script, doc);
	  elType = TtaGetElementType (elem);
	  if (elType.ElTypeNum != MathML_EL_Construct)
	    /* it's not a <none/> element in the souirce code */
	    SetIntPlaceholderAttr (elem, doc);
        }

      CreatePlaceholders (elem, doc);

      /* move also the comments that followed the element we have just moved */
      if (next)
	{
	  prev = elem;
	  elType = TtaGetElementType (next);
	  while (next &&
		 (elType.ElTypeNum == MathML_EL_XMLcomment) &&
		 elType.ElSSchema == MathMLSSchema)
	    {
	      comment = next;
	      TtaNextSibling (&next);
	      TtaRemoveTree (comment, doc);
	      TtaInsertSibling (comment, prev, FALSE, doc);
	      prev = comment;
	      if (next)
		elType = TtaGetElementType (next);
	    }
	}

      /* get next child of the MMULTISCRIPT element */
      elem = next;
      if (elem != NULL)
        {
          elType = TtaGetElementType (elem);
          if (elType.ElSSchema == MathMLSSchema &&
              elType.ElTypeNum == MathML_EL_PrescriptPairs)
            /* the next element is a PrescriptPairs */
            {
              /* if there there is no PostscriptPairs element, create one as a
                 placeholder */
              if (elTypeGroup.ElTypeNum != MathML_EL_PostscriptPairs)
                {
                  elTypeGroup.ElTypeNum = MathML_EL_PostscriptPairs;
                  group = TtaNewTree (doc, elTypeGroup, "");
                  TtaInsertSibling (group, elem, TRUE, doc);
                  SetIntPlaceholderAttr (group, doc);
                }
              /* the following elements will be interpreted as sub- superscripts
                 in PrescriptPair elements, which will be children of this
                 PrescriptPairs element */
              elTypeGroup.ElTypeNum = MathML_EL_PrescriptPairs;
              elTypePair.ElTypeNum = MathML_EL_PrescriptPair;
              group = elem;
              /* create a first and a last PostscriptPair as placeholders */
              pair = TtaNewTree (doc, elTypePair, "");
              TtaInsertFirstChild (&pair, group, doc);
              SetIntPlaceholderAttr (pair, doc);
              prevPair = pair;
              pair = TtaNewTree (doc, elTypePair, "");
              TtaInsertSibling (pair, prevPair, FALSE, doc);
              SetIntPlaceholderAttr (pair, doc);
              prevScript = NULL;
              TtaNextSibling (&elem);
	      /* move the elements that follwed the MPRESCRIPTS element */
	      if (elem)
		{
		  prev = NULL;
		  elType = TtaGetElementType (elem);
		  while (elem &&
			 (elType.ElTypeNum == MathML_EL_XMLcomment) &&
			 elType.ElSSchema == MathMLSSchema)
		    {
		      comment = elem;
		      TtaNextSibling (&elem);
		      TtaRemoveTree (comment, doc);
		      if (prev)
			TtaInsertSibling (comment, prev, FALSE, doc);
		      else
			TtaInsertFirstChild (&comment, group, doc);
		      prev = comment;
		      if (elem)
			elType = TtaGetElementType (elem);
		    }
		}
            }
        }
    }
  /* all children of element MMULTISCRIPTS have been processed */
  /* if the last group processed is not a PrescriptPairs element,
     create one as a placeholder */
  if (elTypeGroup.ElTypeNum != MathML_EL_PrescriptPairs && base != NULL)
    {
      elTypeGroup.ElTypeNum = MathML_EL_PrescriptPairs;
      elem = TtaNewTree (doc, elTypeGroup, "");
      if (group == NULL)
        group = base;
      TtaInsertSibling (elem, group, TRUE, doc);
      SetIntPlaceholderAttr (elem, doc);
    }
}

/*----------------------------------------------------------------------
  CreateWrapper

  Create an element of type wrapperType as a child of element el and
  move all chidren of element el within the new element.
  -----------------------------------------------------------------------*/
static void CreateWrapper (Element el, int wrapperType, Document doc)
{
  Element       wrapper, child, prevChild, nextChild;
  ElementType   elType;

  child = TtaGetFirstChild (el);
  elType.ElSSchema = GetMathMLSSchema (doc);
  elType.ElTypeNum = wrapperType;
  wrapper = TtaNewElement (doc, elType);
  TtaInsertFirstChild (&wrapper, el, doc);
  prevChild = NULL;
  while (child)
    {
      nextChild = child;
      TtaNextSibling (&nextChild);
      TtaRemoveTree (child, doc);
      if (prevChild == NULL)
        TtaInsertFirstChild (&child, wrapper, doc);
      else
        TtaInsertSibling (child, prevChild, FALSE, doc);
      prevChild = child;
      child = nextChild;
    }
}

/*----------------------------------------------------------------------
  CheckMTable

  The content of a MTABLE element has been created following
  the original MathML structure.  Create all Thot elements defined
  in the MathML S schema.
  If placeholder, associate an attribute IntPlaceholder with all
  cells generated in the MathML table.
  -----------------------------------------------------------------------*/
void CheckMTable (Element elMTABLE, Document doc, ThotBool placeholder)
{
  ElementType	elType;
  Element	MTableHead, MTableBody, row, nextRow, el, prevRow, cell,
		nextCell, newMTD, firstColHead, label;
  SSchema	MathMLSSchema;

  MathMLSSchema = GetMathMLSSchema (doc);
  row = TtaGetFirstChild (elMTABLE);

  /* create a MTable_head as the first child of element MTABLE */
  elType.ElSSchema = MathMLSSchema;
  elType.ElTypeNum = MathML_EL_MTable_head;
  MTableHead = TtaNewElement (doc, elType);
  TtaInsertFirstChild (&MTableHead, elMTABLE, doc);
  elType.ElTypeNum = MathML_EL_MColumn_head;
  firstColHead = TtaNewTree (doc, elType, "");
  TtaInsertFirstChild (&firstColHead, MTableHead, doc);

  /* create a MTable_body */
  elType.ElTypeNum = MathML_EL_MTable_body;
  MTableBody = TtaNewElement (doc, elType);
  TtaInsertSibling (MTableBody, MTableHead, FALSE, doc);

  /* move all children of element MTABLE into the new MTable_body element
     and wrap each non-MTR element in a MTR, except comments */
  prevRow = NULL;
  while (row)
    {
      nextRow = row;
      TtaNextSibling (&nextRow);
      elType = TtaGetElementType (row);
      TtaRemoveTree (row, doc);
      if (TtaSameSSchemas (elType.ElSSchema, MathMLSSchema) &&
          (elType.ElTypeNum == MathML_EL_XMLcomment ||
           elType.ElTypeNum == MathML_EL_MTR ||
           elType.ElTypeNum == MathML_EL_MLABELEDTR))
        {
          if (prevRow == NULL)
            TtaInsertFirstChild (&row, MTableBody, doc);
          else
            TtaInsertSibling (row, prevRow, FALSE, doc);
          prevRow = row;
          if (elType.ElTypeNum == MathML_EL_MTR ||
              elType.ElTypeNum == MathML_EL_MLABELEDTR)
            {
              cell = TtaGetFirstChild (row);
              if (elType.ElTypeNum == MathML_EL_MLABELEDTR)
                /* skip the first significant child of the mlabeledtr element */
                {
                  /* skip comments first */
                  do
                    {
                      elType = TtaGetElementType (cell);
                      if (TtaSameSSchemas (elType.ElSSchema, MathMLSSchema) &&
                          elType.ElTypeNum == MathML_EL_XMLcomment)
                        TtaNextSibling (&cell);
                    }
                  while (cell && elType.ElTypeNum == MathML_EL_XMLcomment);
                  /* skip the first element after the comments: it's a label */
                  if (cell)
                    {
                      /* if it's a MTD change its type into LabelCell */
                      if (elType.ElTypeNum == MathML_EL_MTD &&
                          elType.ElSSchema == MathMLSSchema)
                        TtaChangeElementType (cell, MathML_EL_LabelCell);
                      /* wrap this element in a RowLabel element */
                      /* This will allow the P schema to specify the horizontal
                         position of the label */
                      elType.ElSSchema = MathMLSSchema;
                      elType.ElTypeNum = MathML_EL_RowLabel;
                      label = TtaNewElement (doc, elType);
                      TtaInsertSibling (label, cell, TRUE, doc);
                      TtaRemoveTree (cell, doc);
                      TtaInsertFirstChild (&cell, label, doc);
                      cell = label;
                      TtaNextSibling (&cell);
                    }
                } 
            }
          else
            cell = NULL;
        }
      else
        /* this child is not a MTR, MLABELEDTR, or a comment.
           In MathML 2.0, this in an error, but we try to recover by
           creating a MTR element */
        {
          elType.ElSSchema = MathMLSSchema;
          elType.ElTypeNum = MathML_EL_MTR;
          el = TtaNewElement (doc, elType);
          if (prevRow == NULL)
            TtaInsertFirstChild (&el, MTableBody, doc);
          else
            TtaInsertSibling (el, prevRow, FALSE, doc);
          TtaInsertFirstChild (&row, el, doc);
          cell = row;
          prevRow = el;
        }
      while (cell)
        /* check all children of the current MTR element */
        {
          nextCell = cell;
          TtaNextSibling (&nextCell);
          elType = TtaGetElementType (cell);
          if (!TtaSameSSchemas (elType.ElSSchema, MathMLSSchema) ||
              (elType.ElTypeNum != MathML_EL_XMLcomment &&
               elType.ElTypeNum != MathML_EL_MTD))
            /* this is not a MTD nor a comment, create a wrapping MTD */
            {
              elType.ElSSchema = MathMLSSchema;
              elType.ElTypeNum = MathML_EL_MTD;
              newMTD = TtaNewElement (doc, elType);
              TtaInsertSibling (newMTD, cell, TRUE, doc);
              TtaRemoveTree (cell, doc);
              TtaInsertFirstChild (&cell, newMTD, doc);
              cell = newMTD;
            }
          if (elType.ElTypeNum == MathML_EL_MTD)
            /* This is a MTD element. Wrap its contents with a CellWrapper */
            {
              CreateWrapper (cell, MathML_EL_CellWrapper, doc);
              SetIntHorizStretchAttr (cell, doc);
              SetIntVertStretchAttr (cell, doc, MathML_EL_CellWrapper, NULL);
            }
          cell = nextCell;
        }
      row = nextRow;
    }
  CheckAllRows (elMTABLE, doc, placeholder, FALSE);
}

/*----------------------------------------------------------------------
  SetFontstyleAttr
  The content of a MI element has been created or modified.
  Create or change attribute IntFontstyle for that element accordingly.
  -----------------------------------------------------------------------*/
void SetFontstyleAttr (Element el, Document doc)
{
  ElementType	elType;
  AttributeType	attrType, attrType1;
  Attribute	attr, IntAttr;
  Element       ancestor, textEl, child;
  int		len;
  Language      lang;
  CHAR_T        text[2];
  ThotBool      italic;

  if (el != NULL)
    {
      /* search the (deprecated) fontstyle attribute or the mathvariant
         attribute on the element and its ancestors */
      elType = TtaGetElementType (el);
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = MathML_ATTR_fontstyle;
      attrType1.AttrSSchema = elType.ElSSchema;
      attrType1.AttrTypeNum = MathML_ATTR_mathvariant;
      ancestor = el;
      attr = NULL;
      do
        {
          attr = TtaGetAttribute (ancestor, attrType);
          if (!attr)
            attr = TtaGetAttribute (ancestor, attrType1);
          if (!attr)
            {
              ancestor = TtaGetParent (ancestor);
              if (ancestor)
                {
                  elType = TtaGetElementType (ancestor);
                  if (elType.ElSSchema != attrType.AttrSSchema)
                    /* this ancestor is not in the MathML namespace */
                    ancestor = NULL;
                }
            }
        }
      while (ancestor && !attr);

      attrType.AttrTypeNum = MathML_ATTR_IntFontstyle;
      IntAttr = TtaGetAttribute (el, attrType);
      if (attr != NULL)
        /* there is a fontstyle or mathvariant attribute. Remove the
           IntFontstyle internal attribute that is not needed */
        {
          if (IntAttr != NULL)
            TtaRemoveAttribute (el, IntAttr, doc);
        }
      else
        /* there is no fontstyle or mathvariant attribute. Create an internal
           IntFontstyle attribute with a value that depends on the content of
           the MI element */
        {
	  textEl = NULL;
          len = 0;
	  /* if the MI element contains some comments, these should not
	     be counted */
	  child = TtaGetFirstChild (el);
	  while (child && !textEl)
	    {
	      elType = TtaGetElementType (child);
	      if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
		{
		  textEl = child;
		  /* get content length */
		  len = TtaGetElementVolume (textEl);
		}
	      else
		TtaNextSibling (&child);
	    }
          if (len != 1)
            /* put an attribute IntFontstyle = IntNormal */
            {
              if (IntAttr == NULL)
                {
                  IntAttr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (el, IntAttr, doc);
                }
              TtaSetAttributeValue (IntAttr,
                                    MathML_ATTR_IntFontstyle_VAL_IntNormal,
                                    el, doc);
            }
          else
            /* MI contains a single character. Remove attribute IntFontstyle
               if it exists, except if it's ImaginaryI, ExponentialE or
               DifferentialD */
            {
              italic = TRUE;
              if (textEl)
                {
                  elType = TtaGetElementType (textEl);
                  if (elType.ElTypeNum == MathML_EL_MGLYPH)
                    /* the content of the MI element is a MGLYPH element */
                    /* check the length if it's alt attribute */
                    {
                      /* by default, use normal style */
                      italic = FALSE;
                      attrType.AttrTypeNum = MathML_ATTR_alt;
                      attr = TtaGetAttribute (textEl, attrType);
                      if (attr)
                        /* the MGLYPH element has an alt attribute */
                        {
                          len = TtaGetTextAttributeLength (attr);
                          if (len == 1)
                            italic = TRUE;
                        }
                    }
                  else if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
                    {
                      len = TtaGetElementVolume (textEl);
                      if (len == 1)
                        /* the TEXT element contains a single character */
                        {
                          /* get that character */
                          len = 2;
                          TtaGiveBufferContent (textEl, text, len, &lang);
                          if (text[0] >= '0' && text[0] <= '9')
                            /* that's a single digit */
                            italic = FALSE;
                          else
                            /* is this the Unicode character for DifferentialD,
                               ExponentialE or ImaginaryI? */
                            if (text[0] == 0x2146 || text[0] == 0x2147 ||
                                text[0] == 0x2148)
                              italic = FALSE;
                        }
                    }
                  if (italic)
                    {
                      if (IntAttr != NULL)
                        TtaRemoveAttribute (el, IntAttr, doc);
                    }
                  else
                    {
                      /* put an attribute IntFontstyle = IntNormal */
                      if (IntAttr == NULL)
                        {
                          attrType.AttrTypeNum = MathML_ATTR_IntFontstyle;
                          IntAttr = TtaNewAttribute (attrType);
                          TtaAttachAttribute (el, IntAttr, doc);
                        }
                      TtaSetAttributeValue (IntAttr,
                                            MathML_ATTR_IntFontstyle_VAL_IntNormal,
                                            el, doc);
                    }
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  SetIntAddSpaceAttr
  The content of a MO element has been created or modified.
  Create or change attribute IntAddSpace for that element accordingly.
  -----------------------------------------------------------------------*/
void SetIntAddSpaceAttr (Element el, Document doc)
{
  Element	textEl, previous;
  ElementType	elType;
  AttributeType	attrType;
  Attribute	attr, formAttr;
  SSchema       MathMLSSchema;
  int		len, val, form;
  CHAR_T        text[2];
  Language	lang;
  char		script;
  ThotBool      comment;

  MathMLSSchema = TtaGetElementType(el).ElSSchema;
  /* get the content of the mo element */
  textEl = TtaGetFirstChild (el);

  /* skip comments if any */
  if (textEl)
    do
      {
        elType = TtaGetElementType (textEl);
        if (TtaSameSSchemas (elType.ElSSchema, MathMLSSchema) &&
            elType.ElTypeNum == MathML_EL_XMLcomment)
          /* it's a comment, skip it */
          TtaNextSibling (&textEl);
      }
    while (textEl && elType.ElTypeNum == MathML_EL_XMLcomment);

  if (textEl && elType.ElTypeNum == MathML_EL_TEXT_UNIT)
    /* the mo element is not empty */
    {
      /* does the mo element have an IntAddSpace attribute? */
      attrType.AttrSSchema = MathMLSSchema;
      attrType.AttrTypeNum = MathML_ATTR_IntAddSpace;
      attr = TtaGetAttribute (el, attrType);
      if (attr == NULL)
        /* no IntAddSpace Attr, create one */
        {
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (el, attr, doc);
        }
      /* space on both sides by default */
      val = MathML_ATTR_IntAddSpace_VAL_both_;
      /* does the mo element have a form attribute? */
      attrType.AttrTypeNum = MathML_ATTR_form;
      formAttr = TtaGetAttribute (el, attrType);
      if (formAttr)
        /* there is a form attribute */
        {
          form = TtaGetAttributeValue (formAttr);
          switch (form)
            {
            case MathML_ATTR_form_VAL_prefix:
              val = MathML_ATTR_IntAddSpace_VAL_nospace;
              break;
            case MathML_ATTR_form_VAL_infix:
              val = MathML_ATTR_IntAddSpace_VAL_both_;
              break;
            case MathML_ATTR_form_VAL_postfix:
              val = MathML_ATTR_IntAddSpace_VAL_spaceafter;
              break;
            default:
              val = MathML_ATTR_IntAddSpace_VAL_both_;
              break;
            } 
        }
      else
        /* no form attribute. Analyze the content */
        {
          len = TtaGetElementVolume (textEl);
	  if (len == 0)
	    /* empty string */
	    val = MathML_ATTR_IntAddSpace_VAL_nospace;
	  else if (len == 1)
            {
              TtaGiveBufferContent (textEl, text, len+1, &lang);
              script = TtaGetScript (lang);
              /* the mo element contains a single character */
              if (text[0] == '-' || text[0] == 0x2212 /* minus */)
                /* prefix or infix operator? */
                {
                  /* skip preceding comments if any */
                  previous = el;
                  do
                    {
                      comment = FALSE;
                      TtaPreviousSibling (&previous);
                      if (previous)
                        {
                          elType = TtaGetElementType (previous);
                          comment = (TtaSameSSchemas (elType.ElSSchema, MathMLSSchema) &&
                                     elType.ElTypeNum == MathML_EL_XMLcomment);
                        }
                    }
                  while (previous && comment);
		  
                  if (previous == NULL)
                    /* no previous sibling => prefix operator */
                    val = MathML_ATTR_IntAddSpace_VAL_nospace;
                  else
                    {
                      elType = TtaGetElementType (previous);
                      if (elType.ElTypeNum == MathML_EL_MO ||
                          elType.ElTypeNum == MathML_EL_MF ||
                          elType.ElTypeNum == MathML_EL_OpeningFence ||
                          elType.ElTypeNum == MathML_EL_ClosingFence ||
                          elType.ElTypeNum == MathML_EL_FencedSeparator)
                        /* after an operator or parenthesis => prefix operator*/
                        val = MathML_ATTR_IntAddSpace_VAL_nospace;
                      else
                        /* infix operator */
                        val = MathML_ATTR_IntAddSpace_VAL_both_;
                    }
                }
              else if (text[0] == '&' ||
                       text[0] == '*' ||
                       text[0] == '+' ||
                       text[0] == '/' ||
                       text[0] == '<' ||
                       text[0] == '=' ||
                       text[0] == '>' ||
                       text[0] == '^')
                /* infix operator */
                val = MathML_ATTR_IntAddSpace_VAL_both_;
              else if (text[0] == ',' ||
                       text[0] == '!' ||
                       text[0] == '&' ||
                       text[0] == ':' ||
                       text[0] == ';')
                /* separator */
                val = MathML_ATTR_IntAddSpace_VAL_spaceafter;
              else if (text[0] == '(' ||
                       text[0] == ')' ||
                       text[0] == '[' ||
                       text[0] == ']' ||
                       text[0] == '{' ||
                       text[0] == '}' ||
                       text[0] == '.' ||
                       text[0] == '@' ||
                       (int)text[0] == 129 ||  /* thin space */
                       (int)text[0] == 130 ||  /* en space */
                       (int)text[0] == 160)    /* em space */
                val = MathML_ATTR_IntAddSpace_VAL_nospace;
              else if ((int)text[0] == 0x2264 || /* less or equal */
		       (int)text[0] == 0x2265 || /* greater or equal */
		       (int)text[0] == 0x00B1 || /* plus or minus */
		       (int)text[0] == 0x00D7 || /* times */
		       (int)text[0] == 0x00F7 || /* divide */
		       (int)text[0] == 0x2044 || /* frasl */
		       (int)text[0] == 0x2260 || /* not equal */
		       (int)text[0] == 0x2261 || /* identical */
		       (int)text[0] == 0x2248 || /* equivalent */
		       (int)text[0] == 0x2297 || /* circle times */
		       (int)text[0] == 0x2295 || /* circle plus */
		       (int)text[0] == 0x2218 || /* ring operator */
		       (int)text[0] == 0x2229 || /* Intersection */
		       (int)text[0] == 0x222A || /* Union */
		       (int)text[0] == 0x2283 || /* Superset of */
		       (int)text[0] == 0x2287 || /* Superset of or equal to */
		       (int)text[0] == 0x2284 || /* Not a subset of */
		       (int)text[0] == 0x2282 || /* Subset of */
		       (int)text[0] == 0x2286 || /* Subset of or equal to */
		       (int)text[0] == 0x2208 || /* Element of */
		       (int)text[0] == 0x2209 || /* Not an element of */
		       (int)text[0] == 0x2220 || /* Angle */
		       (int)text[0] == 0x2207 || /* Nabla */
		       (int)text[0] == 0x2223 || /* Vertical bar */
		       (int)text[0] == 0x2227 || /* and */
		       (int)text[0] == 0x2228 || /* or */
		       (int)text[0] == 0x2190 || /* left arrow */
		       (int)text[0] == 0x2192 || /* right arrow */
		       (int)text[0] == 0x2194)   /* left right arrow */
		/* infix operator */
		val = MathML_ATTR_IntAddSpace_VAL_both_;
	      else
		val = MathML_ATTR_IntAddSpace_VAL_nospace;
            }
        }
      TtaSetAttributeValue (attr, val, el, doc);
    }
}

/*----------------------------------------------------------------------
  ChildOfMRowOrInferred
  Return TRUE if element el is a child of a MROW element or an
  inferred MROW element
  ----------------------------------------------------------------------*/
ThotBool      ChildOfMRowOrInferred (Element el)
{
  ElementType	 elType;
  Element       parent;
  ThotBool      result;

  result = FALSE;
  parent = TtaGetParent (el);
  if (parent)
    {
      elType = TtaGetElementType (parent);
      result = (elType.ElTypeNum == MathML_EL_MROW ||
                elType.ElTypeNum == MathML_EL_SqrtBase ||
                elType.ElTypeNum == MathML_EL_MSTYLE ||
                elType.ElTypeNum == MathML_EL_MERROR ||
                elType.ElTypeNum == MathML_EL_MENCLOSE ||
                elType.ElTypeNum == MathML_EL_MPADDED ||
                elType.ElTypeNum == MathML_EL_MPHANTOM ||
                elType.ElTypeNum == MathML_EL_MFENCED ||
                elType.ElTypeNum == MathML_EL_CellWrapper ||
                elType.ElTypeNum == MathML_EL_MathML ||
                elType.ElTypeNum == MathML_EL_FencedExpression);
    }
  return result;   
}

/*----------------------------------------------------------------------
  CheckFence
  If el is a MO element or a fence,
  if it's a child of a MROW (or equivalent) element and if it contains
  a single fence character, transform the MO into a MF and the fence
  character into a Thot stretchable symbol.
  ----------------------------------------------------------------------*/
void      CheckFence (Element el, Document doc)
{
  ElementType	       elType, contType;
  Element	       content;
  AttributeType       attrType;
  Attribute	       attr, attrStretchy;
  Language	       lang;
  CHAR_T              text[2];
  char	               script;
  unsigned char       c;
  int                 len, val;
  ThotBool            oldStructureChecking;

  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == MathML_EL_MO ||
      elType.ElTypeNum == MathML_EL_OpeningFence ||
      elType.ElTypeNum == MathML_EL_ClosingFence ||
      elType.ElTypeNum == MathML_EL_FencedSeparator)
    /* the element is a MO or equivalent */
    {
      content = TtaGetFirstChild (el);
      if (content != NULL)
        {
          contType = TtaGetElementType (content);
          if (contType.ElTypeNum == MathML_EL_TEXT_UNIT)
            {
              len = TtaGetElementVolume (content);
              if (len == 1)
                /* the MO or fence element contains a single character */
                {
                  TtaGiveBufferContent (content, text, len+1, &lang);
                  script = TtaGetScript (lang);
                  if (ChildOfMRowOrInferred (el))
                    /* the MO or fence element is a child of a MROW element */
                    /* Is it a stretchable symbol? */
                    {
                      if (IsStretchyFence (text[0], script))
                        /* it's a stretchable parenthesis or equivalent */
                        {
                          /* remove the content of the MO element */
                          TtaDeleteTree (content, doc);
                          /* change the MO element into a MF element */
                          if (elType.ElTypeNum == MathML_EL_MO)
                            TtaChangeTypeOfElement (el, doc, MathML_EL_MF);
                          /* is there an attribute stretchy on this mo element? */
                          attrType.AttrSSchema = elType.ElSSchema;
                          attrType.AttrTypeNum = MathML_ATTR_stretchy;
                          attrStretchy = TtaGetAttribute (el, attrType);
                          if (attrStretchy)
                            val = TtaGetAttributeValue (attrStretchy);
                          else
                            val = MathML_ATTR_stretchy_VAL_true;
                          if (val == MathML_ATTR_stretchy_VAL_true)
                            {
                              /* attach a IntVertStretch attribute to the MF element */
                              attrType.AttrTypeNum = MathML_ATTR_IntVertStretch;
                              attr = TtaGetAttribute (el, attrType);
                              if (attr)
                                TtaSetAttributeValue (attr,
                                           MathML_ATTR_IntVertStretch_VAL_yes_,
                                           el, doc);
                              else
                                {
                                  attr = TtaNewAttribute (attrType);
                                  TtaSetAttributeValue (attr,
                                           MathML_ATTR_IntVertStretch_VAL_yes_,
                                           el, doc);
                                  TtaAttachAttribute (el, attr, doc);
                                }
                            }
                          /* create a new content for the MF element */
                          elType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
                          if (text[0] == 0x27E8)
                            c = '<';    /* mathematical left angle bracket */
                          else if (text[0] == 0x27E9)
                            c = '>';    /* mathematical right angle bracket */
                          else if (text[0] == 0x301a)
                            c = 1;    /* LeftDoubleBracket */
                          else if (text[0] == 0x301b)
                            c = 2;    /* RightDoubleBracket */
                          else if (text[0] == 0x2308)
                            c = 5;    /* LeftCeiling */
                          else if (text[0] == 0x2309)
                            c = 6;    /* RightCeiling */
                          else if (text[0] == 0x230a)
                            c = 3;    /* LeftFloor */
                          else if (text[0] == 0x230b)
                            c = 4;    /* RightFloor */
                          else
                            c = (char) text[0];
                          content = TtaNewElement (doc, elType);
                          /* do not check the Thot abstract tree against the structure
                             schema while inserting this child element  */
                          oldStructureChecking = TtaGetStructureChecking (doc);
                          TtaSetStructureChecking (FALSE, doc);
                          TtaSetGraphicsShape (content, c, doc);
                          TtaInsertFirstChild (&content, el, doc);
                          /* resume structure checking */
                          TtaSetStructureChecking (oldStructureChecking, doc);
                        }
                    }
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  CreateFencedSeparators
  Create FencedSeparator elements within the fencedExpression
  according to attribute separators of the MFENCED element.
  ----------------------------------------------------------------------*/
void CreateFencedSeparators (Element fencedExpression, Document doc, ThotBool record)
{
  ElementType	 elType;
  Element	 child, separator, leaf, next, prev, mfenced;
  AttributeType  attrType;
  Attribute      attr;
  int		 length, sep, i;
  Language	 lang;
  CHAR_T	 text[100], sepValue[4];

  /* get the separators attribute */
  mfenced = TtaGetParent (fencedExpression);
  elType = TtaGetElementType (fencedExpression);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = MathML_ATTR_separators;
  text[0] = ',';	/* default value is  separators=","  */
  text[1] = EOS;
  length = 1;
  attr = TtaGetAttribute (mfenced, attrType);
  if (attr)
    {
      length = 100;
      TtaGiveTextAttributeValue (attr, (char*)text, &length);
    }

  /* create FencedSeparator elements in the FencedExpression */
  prev = NULL;
  sep = 0;
  /* skip leading spaces in attribute separators */
  while (text[sep] <= SPACE && text[sep] != EOS)
    sep++;
  /* if attribute separators is empty or contains only spaces, do not
     insert any separator element */
  if (text[sep] != EOS)
    {
      child = TtaGetFirstChild (fencedExpression);
      while (child != NULL)
        {
          next = child;
          TtaNextSibling (&next);
          elType = TtaGetElementType (child);
          if (elType.ElTypeNum != MathML_EL_Construct &&
	      elType.ElTypeNum != MathML_EL_MSPACE
	      )
            {
              if (prev != NULL)
                {
                  elType.ElTypeNum = MathML_EL_FencedSeparator;
                  separator = TtaNewElement (doc, elType);
                  TtaInsertSibling (separator, prev, FALSE, doc);
                  elType.ElTypeNum = MathML_EL_TEXT_UNIT;
                  leaf = TtaNewElement (doc, elType);
                  TtaInsertFirstChild (&leaf, separator, doc);
                  sepValue[0] = text[sep];
                  sepValue[1] = EOS;
                  lang = TtaGetLanguageIdFromScript('L');
                  TtaSetTextContent (leaf, (unsigned char *)sepValue, lang,
				     doc);
                  SetIntAddSpaceAttr (separator, doc);
                  SetIntVertStretchAttr (separator, doc, 0, NULL);
                  CheckFence (separator, doc);

                  /* is there a following non-space character in separators? */
                  i = sep + 1;
                  while (text[i] <= SPACE && text[i] != EOS)
                    i++;
                  if (text[i] > SPACE && text[i] != EOS)
                    sep = i;
                  if (record)
                    TtaRegisterElementCreate (separator, doc);
                }
              prev = child;
            }
          child = next;
        }
    }
}

/*----------------------------------------------------------------------
  CreateOpeningOrClosingFence
  Create the OpeningFence or ClosingFence element (depending on parameter
  open) for the MFENCED element el which contain the fencedExpression
  element.
  ----------------------------------------------------------------------*/
static void  CreateOpeningOrClosingFence (Element fencedExpression,
                                          Element el, Document doc,
                                          ThotBool open)
{
  ElementType	elType;
  Element       leaf, fence;
  AttributeType attrType;
  Attribute     attr;
  int           length, value;
  char          text[32], *s;
  CHAR_T        val[2];

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  if (open)
    {
      text[0] = '(';    /* default value of attribute 'open' */
      attrType.AttrTypeNum = MathML_ATTR_open;
      elType.ElTypeNum = MathML_EL_OpeningFence;
      length = 1;
    }
  else
    {
      text[0] = ')';    /* default value of attribute 'close' */
      attrType.AttrTypeNum = MathML_ATTR_close;
      elType.ElTypeNum = MathML_EL_ClosingFence;
      length = 1;
    }
  attr = TtaGetAttribute (el, attrType);
  if (attr != NULL)
    {
      length = 31;
      TtaGiveTextAttributeValue (attr, text, &length);
      if (length == 0)
        /* content of attribute open or close should be a single character */
        text[0] = SPACE;
      else if (text[0] == START_ENTITY)
        {
          text[0] = '&';
          text[length-1] = EOS;
          if (MapXMLEntity (MATH_TYPE, &text[1], &value))
            {
              // convert the entity
              val[0] = (CHAR_T) value;
              val[1] = EOS;
              s = (char *)TtaConvertWCToByte ((CHAR_T *)val, UTF_8);
              strcpy (text, s);
              TtaFreeMemory (s);
              length = strlen (text);
            }
          else
            text[length-1] = ';';
        }
    }
  text[length] = EOS;
  fence = TtaNewElement (doc, elType);
  TtaInsertSibling (fence, fencedExpression, open, doc);
  elType.ElTypeNum = MathML_EL_TEXT_UNIT;
  leaf = TtaNewElement (doc, elType);
  TtaInsertFirstChild (&leaf, fence, doc);
  TtaSetTextContent (leaf, (unsigned char *)text, TtaGetLanguageIdFromScript('L'), doc);
  SetIntAddSpaceAttr (fence, doc);
  SetIntVertStretchAttr (fence, doc, 0, NULL);
  CheckFence (fence, doc);
}

/*----------------------------------------------------------------------
  TransformMFENCED
  Transform the content of a MFENCED element: create elements
  OpeningFence, FencedExpression, ClosingFence and FencedSeparator.
  ----------------------------------------------------------------------*/
static void      TransformMFENCED (Element el, Document doc)
{
  ElementType	 elType;
  Element	 child, fencedExpression, next, prev, firstChild;

  child = TtaGetFirstChild (el);
  if (child != NULL)
    elType = TtaGetElementType (child);
  else
    elType.ElTypeNum = 0;
  if (child != NULL && elType.ElTypeNum == MathML_EL_OpeningFence)
    /* The first child of this MFENCED element is an OpeningFence.
       This MFENCED expression has already been transformed, possibly
       by the Transform command */
    {
      TtaNextSibling (&child);
      fencedExpression = child;
      if (fencedExpression != NULL)
        elType = TtaGetElementType (fencedExpression);
      if (elType.ElTypeNum == MathML_EL_FencedExpression)
        /* the second child is a FencedExpression. OK.
           Remove all existing FencedSeparator elements */
        {
          child = TtaGetFirstChild (fencedExpression);
          prev = NULL;
          while (child != NULL)
            {
              elType = TtaGetElementType (child);
              next = child;
              TtaNextSibling (&next);
              if (elType.ElTypeNum == MathML_EL_FencedSeparator)
                /* Remove this separator */
                TtaDeleteTree (child, doc);
              child = next;
            }
          /* create FencedSeparator elements in the FencedExpression */
          CreateFencedSeparators (fencedExpression, doc, FALSE);
        }
    }
  else
    /* this MFENCED element must be transformed */
    {
      /* create a FencedExpression element as a child of the MFENCED elem. */
      elType = TtaGetElementType (el);
      elType.ElTypeNum = MathML_EL_FencedExpression;
      fencedExpression = TtaNewElement (doc, elType);
      TtaInsertFirstChild (&fencedExpression, el, doc);
      if (child == NULL)
        /* empty MFENCED element */
        {
          elType.ElTypeNum = MathML_EL_Construct;
          child = TtaNewElement (doc, elType);
          TtaInsertFirstChild (&child, fencedExpression, doc);
          SetIntPlaceholderAttr (child, doc);
        }
      else
        {
          /* move the content of the MFENCED element within the new
             FencedExpression element */
          prev = NULL;
          firstChild = NULL;
          while (child != NULL)
            {
              next = child;
              TtaNextSibling (&next);
              TtaRemoveTree (child, doc);
              if (prev == NULL)
                {
                  TtaInsertFirstChild (&child, fencedExpression, doc);
                  firstChild = child;
                }
              else
                TtaInsertSibling (child, prev, FALSE, doc);
              prev = child;
              child = next;
            }

          /* create FencedSeparator elements in the FencedExpression */
          CreateFencedSeparators (fencedExpression, doc, FALSE);

          /* Create placeholders within the FencedExpression element */
          CreatePlaceholders (firstChild, doc);
        }

      /* create the OpeningFence element according to the open attribute */
      CreateOpeningOrClosingFence (fencedExpression, el, doc, TRUE);

      /* create the ClosingFence element according to close attribute */
      CreateOpeningOrClosingFence (fencedExpression, el, doc, FALSE);
    }
}

/*----------------------------------------------------------------------
  MathMLScriptShift
  The MathML attribute attr (superscriptshift or subscriptshift) is associated
  with element el (a msub, msup or msubsup).
  If value is not NULL, generate the corresponding Thot VertPos rule for the
  Subscript or  Superscript child of el.
  If value is NULL, remove the Thot VertPos rule.
  -----------------------------------------------------------------------*/
void MathMLScriptShift (Document doc, Element el, char *value, int attr)
{
  ElementType         elType;
  Element             script, child;
  int                 scrType;
  PresentationValue   pval;
  PresentationContext ctxt;

  /* get the Superscript or Subscript child of el */
  if (attr == MathML_ATTR_superscriptshift)
    scrType = MathML_EL_Superscript;
  else if (attr == MathML_ATTR_subscriptshift)
    scrType = MathML_EL_Subscript;
  else
    return;
  script = NULL;
  child = TtaGetFirstChild (el);
  while (!script && child)
    {
      elType = TtaGetElementType (child);
      if (elType.ElTypeNum == scrType)
        script = child;
      else
        TtaNextSibling (&child);
    }
  if (script)
    /* Superscript or Subscript element found */
    {
      ctxt = TtaGetSpecificStyleContext (doc);
      if (!value)
        /* remove the presentation rule */
        {
          ctxt->destroy = TRUE;
          pval.typed_data.value = 0;
          TtaSetStylePresentation (PRVertPos, script, NULL, ctxt, pval);
        }
      else
        {
          ctxt->destroy = FALSE;
          /* parse the attribute value (a number followed by a unit) */
          value = (char*)TtaSkipBlanks (value);
          value = ParseCSSUnit (value, &pval);
          if (pval.typed_data.unit != UNIT_INVALID)
            {
              if (pval.typed_data.unit == UNIT_BOX)
                pval.typed_data.unit = UNIT_EM;
              pval.typed_data.mainValue = TRUE;
              /* the specific presentation to be created is not a CSS rule */
              ctxt->cssSpecificity = 2000;
              if (attr == MathML_ATTR_superscriptshift)
                pval.typed_data.value = - pval.typed_data.value;
              TtaSetStylePresentation (PRVertPos, script, NULL, ctxt, pval);
            }
        }
      TtaFreeMemory (ctxt);
    }
}

/*----------------------------------------------------------------------
  SetScriptShift
  If element el (which is a msup, msub or msubsup) has an attribute
  att (which is subscriptshift or superscriptshift), generate the
  corresponding Thot presentation rule.
  ----------------------------------------------------------------------*/
static void SetScriptShift (Element el, Document doc, int att)
{
  AttributeType     attrType;
  ElementType       elType;
  Attribute         attr;
  char             *value;
  int               length;

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = att;
  attr = TtaGetAttribute (el, attrType);
  if (attr)
    {
      length = TtaGetTextAttributeLength (attr);
      if (length > 0)
        {
          value = (char *)TtaGetMemory (length+1);
          value[0] = EOS;
          TtaGiveTextAttributeValue (attr, value, &length);
          MathMLScriptShift (doc, el, value, att);
          TtaFreeMemory (value);
        }
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
                              ThotBool delete_)
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
  if (!delete_)
    {
      length = TtaGetTextAttributeLength (attr);
      if (length > 0)
        {
          value = (char *)TtaGetMemory (length+1);
          value[0] = EOS;
          TtaGiveTextAttributeValue (attr, value, &length);
        }
    }
  /* if attribute rowalign is created or updated but has no value, don't
     do anything */
  if (!delete_ && !value)
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
          if (delete_)
            DeleteIntRowAlign (row, doc);
          else
            {
              if (*ptr != EOS)
                {
                  /* get next word in the attribute value */
                  ptr = (char*)TtaSkipBlanks (ptr);
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
                              ThotBool delete_, ThotBool allRows)
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
  if (!delete_)
    {
      length = TtaGetTextAttributeLength (attr);
      if (length > 0)
        {
          value = (char *)TtaGetMemory (length+1);
          value[0] = EOS;
          TtaGiveTextAttributeValue (attr, value, &length);
        }
    }
  /* if attribute columnalign is created or updated but has no value, don't
     do anything */
  if (!delete_ && !value)
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
              localValue = (char *)TtaGetMemory (length+1);
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
          if (delete_)
            DeleteIntColAlign (cell, doc);
          else
            {
              if (*ptr != EOS)
                {
                  /* get next word in the attribute value */
                  ptr = (char*)TtaSkipBlanks (ptr);
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
                      localValue = (char *)TtaGetMemory (length+1);
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
  HandleRowspacingAttribute
  An attribute rowspacing has been created, updated or deleted (if delete
  is TRUE) for element el in document doc. Update the top and bottom padding
  of all cells accordingly.
  ----------------------------------------------------------------------*/
void HandleRowspacingAttribute (Attribute attr, Element el, Document doc,
                                ThotBool delete_)
{
  ElementType         elType, rowType, cellType;
  int                 length, val, topVal, topValUnit, bottomVal,
    bottomValUnit, rowspan, cellBottomVal, i;
  char               *value, *ptr, *spanPtr;
  PresentationValue   pval;
  PresentationContext ctxt;
  Element             row, nextRow, cell;
  ThotBool            stop, firstRow;
  AttributeType       rowspanType;
  Attribute           rowspanAttr;

  elType = TtaGetElementType (el);
  if (elType.ElTypeNum != MathML_EL_MTABLE ||
      strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
    /* ignore rowspacing attribute on mstyle elements */
    /* process it only on mtable elements */
    return;

  value = NULL;
  if (!delete_ && attr)
    {
      length = TtaGetTextAttributeLength (attr);
      if (length > 0)
        {
          value = (char *)TtaGetMemory (length+1);
          value[0] = EOS;
          TtaGiveTextAttributeValue (attr, value, &length);
        }
    }

  ctxt = TtaGetSpecificStyleContext (doc);
  /* the specific presentation to be created is not a CSS rule */
  ctxt->cssSpecificity = 2000;
  ptr = value;
  rowspanType.AttrSSchema = elType.ElSSchema;
  rowspanType.AttrTypeNum = MathML_ATTR_rowspan_;

  /* check all rows within the table */
  firstRow = TRUE;
  bottomVal = 0;
  bottomValUnit = UNIT_PT;
  elType.ElTypeNum = MathML_EL_TableRow;
  row = TtaSearchTypedElement (elType, SearchInTree, el);
  while (row)
    {
      /* get the next row to check if the current row is the last one */
      nextRow = row;
      stop = FALSE;
      do
        {
          TtaNextSibling (&nextRow);
          if (!nextRow)
            stop = TRUE;
          else
            {
              rowType = TtaGetElementType (nextRow);
              /* skip comments and other non mrow elements */
              if ((rowType.ElTypeNum == MathML_EL_MTR ||
                   rowType.ElTypeNum == MathML_EL_MLABELEDTR) &&
                  !strcmp (TtaGetSSchemaName (rowType.ElSSchema), "MathML"))
                /* it's the next mrow */
                stop = TRUE;
            }
        }
      while (!stop);

      /* prepare the value of the padding to be associated with the cells
         of that row */
      val = 0;
      if (delete_)
        /* remove the presentation rules */
        pval.typed_data.value = 0;
      else
        {
          if (!value)
            {
              pval.typed_data.unit = UNIT_PT;
              pval.typed_data.value = 0;
              pval.typed_data.real = FALSE;
            }
          else
            {
              /* get the next field in the attribute value (a number followed
                 by a unit) */
              ptr = (char*)TtaSkipBlanks (ptr);
              if (*ptr != EOS)
                {
                  ptr = ParseCSSUnit (ptr, &pval);
                  if (pval.typed_data.unit != UNIT_INVALID)
                    {
                      if (pval.typed_data.unit == UNIT_BOX)
                        pval.typed_data.unit = UNIT_EM;
                      /* if the value is an integer, make it a real to avoid
                         errors in dividing small integers, such as "1cm" */
                      if (!pval.typed_data.real)
                        {
                          pval.typed_data.value *= 1000;
                          pval.typed_data.real = TRUE;
                        }
                      val = pval.typed_data.value / 2;
                    }
                  else
                    val = pval.typed_data.value;
                }
            }
        }

      /* initialize the padding to be set at the top and at the bottom
         of each cell */
      /* the top padding of a row is the same as the bottom padding of the
         previous row */
      topVal = bottomVal;
      topValUnit = bottomValUnit;
      if (!nextRow)
        /* row is the last in the table. It must not have any padding
           at the bottom */
        bottomVal = 0;
      else
        {
          bottomVal = val;
          bottomValUnit = pval.typed_data.unit;
        }

      /* get the first cell of that row (ignoring Label cells) */
      elType.ElTypeNum = MathML_EL_MTD;
      cell = TtaSearchTypedElement (elType, SearchInTree, row);
      /* update attribute MLineBelowtop padding and bottom padding for all
         cells in that row */
      while (cell)
        {
          cellType = TtaGetElementType (cell);
          /* skip comments and other non mtd elements */
          if (cellType.ElTypeNum == MathML_EL_MTD &&
              !strcmp (TtaGetSSchemaName (cellType.ElSSchema), "MathML"))
            /* that's a mtd element. Process it */
            {
              /* by default, use the value for the current row */
              cellBottomVal = bottomVal;
              if (!delete_ && value)
                /* take row spanning into account */
                {
                  /* is there a rowspan attribute on that cell? */
                  rowspanAttr = TtaGetAttribute (cell, rowspanType);
                  if (!rowspanAttr)
                    rowspan = 1;
                  else
                    rowspan = TtaGetAttributeValue (rowspanAttr);
                  if (!delete_)
                    {
                      /* skip rowspan-1 words in the value of attribute
                         rowlines */
                      if (rowspan > 1)
                        {
                          spanPtr = ptr;
                          for (i = 1; i < rowspan && *spanPtr != EOS; i++)
                            {
                              spanPtr = (char*)TtaSkipBlanks (spanPtr);
                              spanPtr = ParseCSSUnit (spanPtr, &pval);
                            }
                          if (pval.typed_data.unit == UNIT_INVALID)
                            {
                              val = 0;
                              cellBottomVal = 0;
                              bottomValUnit = UNIT_PT;
                            }
                          else
                            {
                              if (pval.typed_data.unit == UNIT_BOX)
                                pval.typed_data.unit = UNIT_EM;
                              /* if the value is an integer, make it a real to
                                 avoid errors in dividing small integers,
                                 such as "1cm" */
                              if (!pval.typed_data.real)
                                {
                                  pval.typed_data.value *= 1000;
                                  pval.typed_data.real = TRUE;
                                }
                              val = pval.typed_data.value / 2;
                              cellBottomVal = val;
                              bottomValUnit = pval.typed_data.unit;
                            }
                        }
                    }
                }

              if ((delete_ || !value) && !firstRow)
                ctxt->destroy = TRUE;
              else
                {
                  pval.typed_data.value = topVal;
                  pval.typed_data.unit = topValUnit;
                  ctxt->destroy = FALSE;
                }
              TtaSetStylePresentation (PRPaddingTop, cell, NULL, ctxt, pval);
              if ((delete_ || !value) && nextRow)
                ctxt->destroy = TRUE;
              else
                {
                  pval.typed_data.value = cellBottomVal;
                  pval.typed_data.unit = bottomValUnit;
                  ctxt->destroy = FALSE;
                }
              TtaSetStylePresentation (PRPaddingBottom, cell, NULL, ctxt,pval);
            }
          TtaNextSibling (&cell);
        }
      row = nextRow;
      firstRow = FALSE;
    }

  TtaFreeMemory (ctxt);
  if (value)
    TtaFreeMemory (value);
}

/*----------------------------------------------------------------------
  ConvertNamedSpace
  if name is the name of a space, return the value of this space
  in value, otherwise return an empty string in value.
  -----------------------------------------------------------------------*/
static char* ConvertNamedSpace (char *name, char *value)
{
  if (strcmp (name, "veryverythinmathspace") == 0)
    {
      strcpy (value, "0.0555556em");
      return (name + strlen("veryverythinmathspace"));
    }
  else if (strcmp (name, "verythinmathspace") == 0)
    {
      strcpy (value, "0.111111em");
      return (name + strlen("verythinmathspace"));
    }
  else if (strcmp (name, "thinmathspace") == 0)
    {
      strcpy (value, "0.166667em");
      return (name + strlen("thinmathspace"));
    }
  else if (strcmp (name, "mediummathspace") == 0)
    {
      strcpy (value, "0.222222em");
      return (name + strlen("mediummathspace"));
    }
  else if (strcmp (name, "thickmathspace") == 0)
    {
      strcpy (value, "0.277778em");
      return (name + strlen("thickmathspace"));
    }
  else if (strcmp (name, "verythickmathspace") == 0)
    {
      strcpy (value, "0.333333em");
      return (name + strlen("verythickmathspace"));
    }
  else if (strcmp (name, "veryverythickmathspace") == 0)
    {
      strcpy (value, "0.388889em");
      return (name + strlen("veryverythickmathspace"));
    }
  else
    {
      value[0] = EOS;
      return name;
    }
}
         
/*----------------------------------------------------------------------
  HandleColumnspacingAttribute
  An attribute columnspacing has been created, updated or deleted (if delete
  is TRUE) for element el in document doc. Update the left and right padding
  of all cells accordingly.
  ----------------------------------------------------------------------*/
void HandleColumnspacingAttribute (Attribute attr, Element el, Document doc,
                                   ThotBool delete_)
{
  ElementType         elType;
  int                 length, val, valUnit, leftVal, leftValUnit,
    rightVal, rightValUnit, colspan, i;
  char               *value, *ptr, valueOfNamedSpace[20];
  PresentationValue   pval;
  PresentationContext ctxt;
  Element             row, cell, nextCell;
  ThotBool            stop, firstCell;
  Attribute           spanAttr;
  AttributeType       colspanType;

  elType = TtaGetElementType (el);
  if (elType.ElTypeNum != MathML_EL_MTABLE ||
      strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
    /* ignore columnspacing attribute on mstyle elements */
    /* process it only on mtable elements */
    return;

  value = NULL;
  if (!delete_ && attr)
    {
      length = TtaGetTextAttributeLength (attr);
      if (length > 0)
        {
          value = (char *)TtaGetMemory (length+1);
          value[0] = EOS;
          TtaGiveTextAttributeValue (attr, value, &length);
        }
    }

  ctxt = TtaGetSpecificStyleContext (doc);
  /* the specific presentation to be created is not a CSS rule */
  ctxt->cssSpecificity = 2000;
  val = 0;
  colspanType.AttrSSchema = elType.ElSSchema;
  colspanType.AttrTypeNum = MathML_ATTR_columnspan;
  
  /* check all cells in all rows within the table */
  elType.ElTypeNum = MathML_EL_TableRow;
  row = TtaSearchTypedElement (elType, SearchInTree, el);
  while (row)
    {
      elType = TtaGetElementType (row);
      if ((elType.ElTypeNum == MathML_EL_MTR ||
           elType.ElTypeNum == MathML_EL_MLABELEDTR) &&
          strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
        /* that's a table row. check all its cells */
        {
          firstCell = TRUE;
          rightVal = 0;
          val = 0;
          valUnit = UNIT_PT;
          ptr = value;
          /* get the first cell of that row (ignoring Label cells) */
          elType.ElTypeNum = MathML_EL_MTD;
          cell = TtaSearchTypedElement (elType, SearchInTree, row);
          while (cell)
            {
              /* prepare the value of the padding to be associated with the 
                 cells */
              if (delete_)
                /* remove the presentation rules */
                {
                  pval.typed_data.value = 0;
                  val = 0;
                  valUnit = UNIT_PT;
                }
              else
                {
                  if (!value)
                    {
                      pval.typed_data.unit = UNIT_PT;
                      pval.typed_data.value = 0;
                      pval.typed_data.real = FALSE;
                      val = 0;
                      valUnit = UNIT_PT;
                    }
                  else
                    {
                      /* parse the next field in the attribute value (a number
                         followed by a unit or a named space) */
                      ptr = (char*)TtaSkipBlanks (ptr);
                      if (*ptr != EOS)
                        {
                          /* is there a columnspan attribute on that cell? */
                          spanAttr = TtaGetAttribute (cell, colspanType);
                          if (!spanAttr)
                            colspan = 1;
                          else
                            colspan = TtaGetAttributeValue (spanAttr);
                          /* skip (colspan - 1) words in the attribute */
                          for (i = 1; i <= colspan && *ptr != EOS; i++)
                            {
                              ptr = (char*)TtaSkipBlanks (ptr);
                              ptr = ConvertNamedSpace (ptr, valueOfNamedSpace);
                              if (valueOfNamedSpace[0] != EOS)
                                /* it's a named space */
                                ptr = ParseCSSUnit (valueOfNamedSpace, &pval);
                              else
                                ptr = ParseCSSUnit (ptr, &pval);
                              if (pval.typed_data.unit == UNIT_INVALID)
                                {
                                  val = 0;
                                  valUnit = UNIT_PT;
                                }
                              else
                                {
                                  if (pval.typed_data.unit == UNIT_BOX)
                                    pval.typed_data.unit = UNIT_EM;
                                  /* if the value is an integer, make it a real
                                     to avoid errors in dividing small
                                     integers, such as "1cm" */
                                  if (!pval.typed_data.real)
                                    {
                                      pval.typed_data.value *= 1000;
                                      pval.typed_data.real = TRUE;
                                    }
                                  val = pval.typed_data.value / 2;
                                  valUnit = pval.typed_data.unit;
                                }
                            }
                        }
                    }
                }

              /* get the next cell in the current row to check if the current
                 cell is the last one in the row */
              nextCell = cell;
              stop = FALSE;
              do
                {
                  TtaNextSibling (&nextCell);
                  if (!nextCell)
                    stop = TRUE;
                  else
                    {
                      elType = TtaGetElementType (nextCell);
                      /* skip comments and other non mtd elements */
                      if (elType.ElTypeNum == MathML_EL_MTD &&
                          strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
                        /* it's the next cell */
                        stop = TRUE;
                    }
                }
              while (!stop);

              /* initialize the padding to be set at the right and at the left
                 of each cell */
              /* the leftPadding of a cell is the same as the right padding
                 of the previous cell */
              leftVal = rightVal;
              leftValUnit = rightValUnit = valUnit;
              if (!nextCell)
                /* it's the last cell in the row. It must not have any
                   padding on the right */
                {
                  rightVal = 0;
                  rightValUnit = UNIT_PT;
                }
              else
                {
                  rightVal = val;
                  rightValUnit = valUnit;
                }

              /* set the left and right padding for this cell */
              if ((delete_ || !value) && !firstCell)
                ctxt->destroy = TRUE;
              else
                {
                  pval.typed_data.value = leftVal;
                  pval.typed_data.unit = leftValUnit;
                  ctxt->destroy = FALSE;
                }
              TtaSetStylePresentation (PRPaddingLeft, cell, NULL, ctxt, pval);
              if ((delete_ || !value) && nextCell)
                ctxt->destroy = TRUE;
              else
                {
                  pval.typed_data.value = rightVal;
                  pval.typed_data.unit = rightValUnit;
                  ctxt->destroy = FALSE;
                }
              TtaSetStylePresentation (PRPaddingRight, cell, NULL, ctxt, pval);
              cell = nextCell;
              firstCell = FALSE;
            }
        }
      TtaNextSibling (&row);
    }
  TtaFreeMemory (ctxt);
  if (value)
    TtaFreeMemory (value);
}

/*----------------------------------------------------------------------
  HandleRowlinesAttribute
  An attribute rowlines has been created, updated or deleted (if delete
  is TRUE) for element el in document doc. Update attribute MLineBelow
  of all cells accordingly.
  ----------------------------------------------------------------------*/
void HandleRowlinesAttribute (Attribute attr, Element el, Document doc,
                              ThotBool delete_)
{
  char            *value;
  char            *ptr, *spanPtr;
  int              length, val, rowspan, i, cellVal;
  ElementType      elType, rowType, cellType;
  Element          row, nextRow, cell;
  ThotBool         stop;
  AttributeType    attrType, rowspanType;
  Attribute        intAttr, rowspanAttr;

  elType = TtaGetElementType (el);
  if (elType.ElTypeNum != MathML_EL_MTABLE ||
      strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
    /* ignore rowlines attribute on mstyle elements */
    /* process it only on mtable elements */
    return;

  value = NULL;
  if (!delete_)
    {
      length = TtaGetTextAttributeLength (attr);
      if (length > 0)
        {
          value = (char *)TtaGetMemory (length+1);
          value[0] = EOS;
          TtaGiveTextAttributeValue (attr, value, &length);
        }
    }
  /* if attribute rowlines is created or updated but has no value, don't
     do anything */
  if (!delete_ && !value)
    return;

  ptr = value;
  val = 0;
  attrType.AttrSSchema = elType.ElSSchema;
  rowspanType.AttrSSchema = elType.ElSSchema;
  rowspanType.AttrTypeNum = MathML_ATTR_rowspan_;

  /* check all rows within the table */
  elType.ElTypeNum = MathML_EL_TableRow;
  row = TtaSearchTypedElement (elType, SearchInTree, el);
  while (row)
    {
      /* get the next row to check if the current row is the last one */
      nextRow = row;
      stop = FALSE;
      do
        {
          TtaNextSibling (&nextRow);
          if (!nextRow)
            stop = TRUE;
          else
            {
              rowType = TtaGetElementType (nextRow);
              /* skip comments and other non mrow elements */
              if ((rowType.ElTypeNum == MathML_EL_MTR ||
                   rowType.ElTypeNum == MathML_EL_MLABELEDTR) &&
                  !strcmp (TtaGetSSchemaName (rowType.ElSSchema), "MathML"))
                /* it's the next mrow */
                stop = TRUE;
            }
        }
      while (!stop);

      if (!nextRow)
        /* row is the last in the table. It must not have a line
           at the bottom. Delete it if there is one */
        val = 0;
      else
        {
          if (delete_)
            val = 0;
          else
            if (*ptr != EOS)
              {
                /* get next word in the attribute value */
                ptr = (char*)TtaSkipBlanks (ptr);
                /* process that word */
                if (*ptr != EOS && *ptr != ' ')
                  {
                    if (!strncasecmp (ptr, "none", 4))
                      val = 0;
                    else if (!strncasecmp (ptr, "solid", 5))
                      val = MathML_ATTR_MLineBelow_VAL_solid_;
                    else if (!strncasecmp (ptr, "dashed", 6))
                      val = MathML_ATTR_MLineBelow_VAL_dashed_;
                    else
                      val = 0;
                    /* skip the word that has been processed */
                    while (*ptr != EOS && *ptr != ' ')
                      ptr++;
                  }
              }
        }
      /* get the first cell of that row (ignoring Label cells) */
      elType.ElTypeNum = MathML_EL_MTD;
      cell = TtaSearchTypedElement (elType, SearchInTree, row);
      /* update attribute MLineBelow for all cells in that row */
      while (cell)
        {
          cellType = TtaGetElementType (cell);
          /* skip comments and other non mtd elements */
          if (cellType.ElTypeNum == MathML_EL_MTD &&
              !strcmp (TtaGetSSchemaName (cellType.ElSSchema), "MathML"))
            /* that's a mtd element. Process it */
            {
              /* is there a rowspan attribute on that cell? */
              rowspanAttr = TtaGetAttribute (cell, rowspanType);
              if (!rowspanAttr)
                rowspan = 1;
              else
                rowspan = TtaGetAttributeValue (rowspanAttr);
              /* by default, use the value for the current row */
              cellVal = val;
              if (!delete_)
                {
                  /* skip rowspan-1 words in the value of attribute rowlines */
                  if (rowspan > 1)
                    {
                      spanPtr = ptr;
                      for (i = 1; i < rowspan && *spanPtr != EOS; i++)
                        {
                          spanPtr = (char*)TtaSkipBlanks (spanPtr);
                          if (*spanPtr != EOS && *spanPtr != ' ')
                            {
                              if (!strncasecmp (spanPtr, "none", 4))
                                cellVal = 0;
                              else if (!strncasecmp (spanPtr, "solid", 5))
                                cellVal = MathML_ATTR_MLineBelow_VAL_solid_;
                              else if (!strncasecmp (spanPtr, "dashed", 6))
                                cellVal = MathML_ATTR_MLineBelow_VAL_dashed_;
                              else
                                cellVal = 0;
                            }
                          /* skip the word that has been processed */
                          while (*spanPtr != EOS && *spanPtr != ' ')
                            spanPtr++;
                        }
                    }
                }
              if (rowspan == 1)
                attrType.AttrTypeNum = MathML_ATTR_MLineBelow;
              else
                attrType.AttrTypeNum = MathML_ATTR_MLineBelowExt;
              intAttr = TtaGetAttribute (cell, attrType);
              if (cellVal == 0)
                {
                  if (intAttr)
                    /* remove attribute MLineBelow */
                    TtaRemoveAttribute (cell, intAttr, doc);
                }
              else
                /* set attribute MLineBelow */
                {
                  if (!intAttr)
                    {
                      intAttr = TtaNewAttribute (attrType);
                      TtaAttachAttribute (cell, intAttr, doc);
                    }
                  TtaSetAttributeValue (intAttr, cellVal, cell, doc);
                }
            }
          TtaNextSibling (&cell);
        }
      row = nextRow;
    }
  if (value)
    TtaFreeMemory (value);
}

/*----------------------------------------------------------------------
  HandleColumnlinesAttribute
  An attribute columnlines has been created, updated or deleted (if delete
  is TRUE) for element el in document doc. Update attribute MLineOnTheRight
  of all cells accordingly.
  ----------------------------------------------------------------------*/
void HandleColumnlinesAttribute (Attribute attr, Element el, Document doc,
                                 ThotBool delete_)
{
  char            *value;
  char            *ptr;
  int              length, val, colspan, rowspan, i;
  ElementType      elType;
  Element          row, cell, nextCell;
  ThotBool         stop;
  AttributeType    attrType, colspanType, rowspanType;
  Attribute        intAttr, spanAttr;

  elType = TtaGetElementType (el);
  if (elType.ElTypeNum != MathML_EL_MTABLE ||
      strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
    /* ignore rowlines attribute on mstyle elements */
    /* process it only on mtable elements */
    return;

  value = NULL;
  if (!delete_)
    {
      length = TtaGetTextAttributeLength (attr);
      if (length > 0)
        {
          value = (char *)TtaGetMemory (length+1);
          value[0] = EOS;
          TtaGiveTextAttributeValue (attr, value, &length);
        }
    }
  /* if attribute columnlines is created or updated but has no value, don't
     do anything */
  if (!delete_ && !value)
    return;

  val = 0;
  attrType.AttrSSchema = elType.ElSSchema;
  colspanType.AttrSSchema = elType.ElSSchema;
  colspanType.AttrTypeNum = MathML_ATTR_columnspan;
  rowspanType.AttrSSchema = elType.ElSSchema;
  rowspanType.AttrTypeNum = MathML_ATTR_rowspan_;

  /* check all cells in all rows in the table */
  elType.ElTypeNum = MathML_EL_TableRow;
  row = TtaSearchTypedElement (elType, SearchInTree, el);
  while (row)
    {
      elType = TtaGetElementType (row);
      if ((elType.ElTypeNum == MathML_EL_MTR ||
           elType.ElTypeNum == MathML_EL_MLABELEDTR) &&
          strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
        /* that's a table row. check all its cells */
        {
          /* start from the beginning of the columnlines attribute */
          ptr = value;
          val = 0;
          /* get the first cell of that row (ignoring Label cells) */
          elType.ElTypeNum = MathML_EL_MTD;
          cell = TtaSearchTypedElement (elType, SearchInTree, row);
          while (cell)
            {
              /* get the next cell in the current row to check if the current
                 cell is the last one in the row */
              nextCell = cell;
              stop = FALSE;
              do
                {
                  TtaNextSibling (&nextCell);
                  if (!nextCell)
                    stop = TRUE;
                  else
                    {
                      elType = TtaGetElementType (nextCell);
                      /* skip comments and other non mtd elements */
                      if (elType.ElTypeNum == MathML_EL_MTD &&
                          strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
                        /* it's the next cell */
                        stop = TRUE;
                    }
                }
              while (!stop);

              /* is there a rowspan attribute on that cell? */
              spanAttr = TtaGetAttribute (cell, rowspanType);
              if (!spanAttr)
                rowspan = 1;
              else
                rowspan = TtaGetAttributeValue (spanAttr);

              if (!nextCell)
                /* it's the last cell in the row. It must not have a line
                   on its right edge. Delete it if there is noe. */
                val = 0;
              else
                /* set the attribute MLineOnTheRight for this cell */
                {
                  if (delete_)
                    val = 0;
                  else
                    if (*ptr != EOS)
                      {
                        /* is there a columnspan attribute on that cell? */
                        spanAttr = TtaGetAttribute (cell, colspanType);
                        if (!spanAttr)
                          colspan = 1;
                        else
                          colspan = TtaGetAttributeValue (spanAttr);
                        /* skip (colspan - 1) words in the attribute */
                        for (i = 1; i <= colspan && *ptr != EOS; i++)
                          {
                            /* get next word in the attribute value */
                            ptr = (char*)TtaSkipBlanks (ptr);
                            /* process that word */
                            if (*ptr != EOS && *ptr != ' ')
                              {
                                if (!strncasecmp (ptr, "none", 4))
                                  val = 0;
                                else if (!strncasecmp (ptr, "solid", 5))
                                  val = MathML_ATTR_MLineOnTheRight_VAL_solid_;
                                else if (!strncasecmp (ptr, "dashed", 6))
                                  val = MathML_ATTR_MLineOnTheRight_VAL_dashed_;
                                else
                                  val = 0;
                                /* skip the word that has been processed */
                                while (*ptr != EOS && *ptr != ' ')
                                  ptr++;
                              }
                          }
                      }
                }
              if (rowspan == 1)
                attrType.AttrTypeNum = MathML_ATTR_MLineOnTheRight;
              else
                attrType.AttrTypeNum = MathML_ATTR_MLineOnTheRightExt;
              intAttr = TtaGetAttribute (cell, attrType);
              if (val == 0)
                {
                  if (intAttr)
                    /* remove attribute MLineOnTheRight */
                    TtaRemoveAttribute (cell, intAttr, doc);
                }
              else
                /* set attribute MLineOnTheRight */
                {
                  if (!intAttr)
                    {
                      intAttr = TtaNewAttribute (attrType);
                      TtaAttachAttribute (cell, intAttr, doc);
                    }
                  TtaSetAttributeValue (intAttr, val, cell, doc);
                }
              cell = nextCell;
            }
        }
      TtaNextSibling (&row);
    }
  if (value)
    TtaFreeMemory (value);
}

/*----------------------------------------------------------------------
  HandleFramespacingAttribute
  An attribute framespacing has been created, updated or deleted (if delete
  is TRUE) for element el in document doc. Update attribute the padding
  properties of the concerned table(s).
  ----------------------------------------------------------------------*/
void HandleFramespacingAttribute (Attribute attr, Element el, Document doc,
                                  ThotBool delete_)
{
  ElementType         elType;
  char                *value, *ptr, valueOfNamedSpace[20];
  int                 length, vertPadding, horizPadding, vertPaddingUnit,
    horizPaddingUnit;
  Attribute           attrFrame;
  AttributeType       attrType;
  PresentationValue   pval;
  PresentationContext ctxt;
  ThotBool            vertPaddingReal, horizPaddingReal;

  if ((!delete_ && !attr) || !el)
    return;
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum != MathML_EL_MTABLE ||
      strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
    /* ignore framespacing attribute on mstyle elements */
    /* process it only on mtable elements */
    return;

  value = NULL;
  if (!delete_)
    {
      length = TtaGetTextAttributeLength (attr);
      if (length > 0)
        {
          value = (char *)TtaGetMemory (length+1);
          value[0] = EOS;
          TtaGiveTextAttributeValue (attr, value, &length);
        }
    }
  ctxt = TtaGetSpecificStyleContext (doc);
  /* the specific presentation to be created is not a CSS rule */
  ctxt->cssSpecificity = 2000;
  vertPadding = 0;
  horizPadding = 0;
  vertPaddingUnit = UNIT_PT;
  horizPaddingUnit = UNIT_PT;
  vertPaddingReal = FALSE;
  horizPaddingReal = FALSE;
  /* is there a frame attribute? */
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = MathML_ATTR_frame;
  attrFrame = TtaGetAttribute (el, attrType);
  if (!delete_ && value && attrFrame)
    {
      ptr = value;
      /* parse the first part: horizontal spacing */
      ptr = (char*)TtaSkipBlanks (ptr);
      if (*ptr != EOS)
        {
          ptr = ConvertNamedSpace (ptr, valueOfNamedSpace);
          if (valueOfNamedSpace[0] != EOS)
            /* it's a named space */
            ptr = ParseCSSUnit (valueOfNamedSpace, &pval);
          else
            ptr = ParseCSSUnit (ptr, &pval);
          if (pval.typed_data.unit != UNIT_INVALID)
            {
              if (pval.typed_data.unit == UNIT_BOX)
                pval.typed_data.unit = UNIT_EM;
              horizPadding = pval.typed_data.value;
              horizPaddingUnit = pval.typed_data.unit;
              horizPaddingReal = pval.typed_data.real;
              /* if there is no second part, the vertical spacing is the same
                 as the horizontal spacing */
              vertPadding = horizPadding;
              vertPaddingUnit = horizPaddingUnit;
              vertPaddingReal = horizPaddingReal;
              /* parse the second part, if any */
              ptr = (char*)TtaSkipBlanks (ptr);
              if (*ptr != EOS)
                {
                  ptr = ConvertNamedSpace (ptr, valueOfNamedSpace);
                  if (valueOfNamedSpace[0] != EOS)
                    /* it's a named space */
                    ptr = ParseCSSUnit (valueOfNamedSpace, &pval);
                  else
                    ptr = ParseCSSUnit (ptr, &pval);
                  if (pval.typed_data.unit != UNIT_INVALID)
                    {
                      if (pval.typed_data.unit == UNIT_BOX)
                        pval.typed_data.unit = UNIT_EM;
                      vertPadding = pval.typed_data.value;
                      vertPaddingUnit = pval.typed_data.unit;
                      vertPaddingReal = pval.typed_data.real;
                    }
                }
            }
        }
    }
  if (delete_)
    ctxt->destroy = TRUE;
  else
    {
      ctxt->destroy = FALSE;
      pval.typed_data.value = horizPadding;
      pval.typed_data.unit = horizPaddingUnit;
      pval.typed_data.real = horizPaddingReal;
    }
  TtaSetStylePresentation (PRPaddingLeft, el, NULL, ctxt, pval);
  TtaSetStylePresentation (PRPaddingRight, el, NULL, ctxt, pval);
  if (!delete_)
    {
      pval.typed_data.value = vertPadding;
      pval.typed_data.unit = vertPaddingUnit;
      pval.typed_data.real = vertPaddingReal;
    }
  TtaSetStylePresentation (PRPaddingTop, el, NULL, ctxt, pval);
  TtaSetStylePresentation (PRPaddingBottom, el, NULL, ctxt, pval);

  if (value)
    TtaFreeMemory (value);
}

/*----------------------------------------------------------------------
  SetDisplaystyleMathElement
  Associate a IntDisplaystyle attribute with element el (which is a
  <math> element), and set its value depending on the surrounding context.
  ----------------------------------------------------------------------*/
void   SetDisplaystyleMathElement (Element el, Document doc)
{
  Element               parent, sibling;
  ElementType		elType, parentType;
  Attribute             attr;
  AttributeType         attrType;
  int                   display, val;

  display = MathML_ATTR_IntDisplaystyle_VAL_true;
  /* is there an attribute display? */
  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = MathML_ATTR_display_;
  attr = TtaGetAttribute (el, attrType);
  if (attr)
    /* there is an attribute display. Take its value */
    {
      val = TtaGetAttributeValue (attr);
      if (val == MathML_ATTR_display__VAL_block_)
        display = MathML_ATTR_IntDisplaystyle_VAL_true;
      else
        display = MathML_ATTR_IntDisplaystyle_VAL_false;
    }
  else
    /* no attribute display. Look at the context */
    {
      parent = TtaGetParent (el);
      parentType = TtaGetElementType (parent);
      if (elType.ElSSchema == parentType.ElSSchema)
        /* it's the only <math> element in a MathML document */
        display = MathML_ATTR_IntDisplaystyle_VAL_true;
      else
        /* it's a MathML expression within another vocabulary */
        {
          if (!strcmp (TtaGetSSchemaName (parentType.ElSSchema), "SVG"))
            /* a <math> element in a SVG element */
            display = MathML_ATTR_IntDisplaystyle_VAL_true;
          else if (!strcmp (TtaGetSSchemaName (parentType.ElSSchema), "HTML"))
            /* a <math> element in a HTML element */
            {
              display = MathML_ATTR_IntDisplaystyle_VAL_false;
              if (parentType.ElTypeNum == HTML_EL_BODY ||
                  parentType.ElTypeNum == HTML_EL_Division)
                /* the <math> element is a child of a <body> or <div> */
                display = MathML_ATTR_IntDisplaystyle_VAL_true;
              else if (parentType.ElTypeNum == HTML_EL_Pseudo_paragraph ||
                       parentType.ElTypeNum == HTML_EL_Paragraph)
                /* the <math> element is a child of a <p> or equivalent */
                {
                  sibling = el;
                  TtaPreviousSibling (&sibling);
                  if (!sibling)
                    {
                      sibling = el;
                      TtaNextSibling (&sibling);
                      if (!sibling)
                        /* the <math> element is alone in its paragraph */
                        display = MathML_ATTR_IntDisplaystyle_VAL_true;
                    }
                }
            }
        }
    }
  /* create the IntDisplastyle attribute and set its value */
  attrType.AttrTypeNum = MathML_ATTR_IntDisplaystyle;
  attr = TtaGetAttribute (el, attrType);
  if (!attr)
    {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
    }
  TtaSetAttributeValue (attr, display, el, doc);
  ApplyDisplaystyle (el, doc);
}

/*----------------------------------------------------------------------
  MathMLElementCreated
  The XML parser has just inserted a new element in the abstract tree.
  ----------------------------------------------------------------------*/
void      MathMLElementCreated (Element el, Document doc)
{
  ElementType		elType;

  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == MathML_EL_MathML)
    /* associate a IntDisplaystyle attribute with the element depending
       on its context */
    SetDisplaystyleMathElement (el, doc);    
}

/*----------------------------------------------------------------------
  EvaluateChildRendering
  choose the child of element el (a <semantics> element) that should be
  displayed
  ----------------------------------------------------------------------*/
void EvaluateChildRendering (Element el, Document doc)
{
  ElementType          elType;
  Element              child, renderedChild;
  SSchema              MathMLSSchema;
  AttributeType        attrType;
  Attribute            attr;
  PresentationValue    pval;
  PresentationContext  ctxt;
  char                *value;
  int                  length;

  ctxt = TtaGetSpecificStyleContext (doc);
  ctxt->cssSpecificity = 2000;   /* the presentation rule to be set is not a CSS rule */
  /* prepare the value of the rule */
  pval.typed_data.unit = UNIT_PX;
  pval.typed_data.value = 0;
  pval.typed_data.real = FALSE;
  MathMLSSchema = TtaGetElementType(el).ElSSchema;
  renderedChild = NULL; /* no child to render yet */
  /* check all children of element el */
  child = TtaGetFirstChild (el);
  while (child)
    {
      /* if this child is a comment, a processing instruction, or an annotation
	 we skip it */
      elType = TtaGetElementType (child);
      ctxt->destroy = FALSE; /* we will most probably create a PRule
                                Visibility: 0; for this child */
      if (elType.ElSSchema == MathMLSSchema &&
          elType.ElTypeNum != MathML_EL_XMLcomment &&
          elType.ElTypeNum != MathML_EL_XMLPI &&
          elType.ElTypeNum != MathML_EL_Unknown_namespace &&
          elType.ElTypeNum != MathML_EL_ANNOTATION)
        {
          if (!renderedChild && elType.ElTypeNum == MathML_EL_ANNOTATION_XML)
	    /* it's an <annotation-xml> element */
            {
              /* check its encoding */
              attrType.AttrSSchema = MathMLSSchema;
              attrType.AttrTypeNum = MathML_ATTR_encoding;
              attr = TtaGetAttribute (child, attrType);
	      /* there is an "encoding" attribute */
              if (attr)
                {
                  length = TtaGetTextAttributeLength (attr);
                  if (length > 0)
                    {
                      value = (char *)TtaGetMemory (length+1);
                      value[0] = EOS;
                      TtaGiveTextAttributeValue (attr, value, &length);
                      if (!strncmp (value, "image/svg", 9) ||
                          !strcmp (value, AM_SVG_MIME_TYPE) ||
                          !strncmp (value, "text/htm", 8) ||
                          !strcmp (value, AM_XHTML_MIME_TYPE))
                        {
                          /* we know this encoding. We will display that child*/
                          renderedChild = child;
			  /* remove the Visibility rule if there is one */
                          ctxt->destroy = TRUE;
                        }  
                    }
                }
            }
          /* set or remove a visibility PRule for this child */
          TtaSetStylePresentation (PRVisibility, child, NULL, ctxt, pval);
        }
      TtaNextSibling (&child);
    }
}

/*----------------------------------------------------------------------
  MathMLElementComplete
  Element el has just been completed by the XML parser.
  Check the Thot structure of the MathML element el.
  ----------------------------------------------------------------------*/
void      MathMLElementComplete (ParserData *context, Element el, int *error)
{
  Document             doc;   
  ElementType	       elType, parentType;
  Element	       child, parent, new_, prev, next;
  AttributeType        attrType;
  Attribute            attr;
  SSchema              MathMLSSchema;
  int                  selection, i;
  ThotBool             ok;

  ok = TRUE;
  *error = 0;
  doc = context->doc;
  elType = TtaGetElementType (el);
  MathMLSSchema = GetMathMLSSchema (doc);

  if (elType.ElSSchema == MathMLSSchema)
    {
      switch (elType.ElTypeNum)
        {
        case MathML_EL_MathML:
          /* Create placeholders within the MathML element */
          CreatePlaceholders (TtaGetFirstChild (el), doc);
          break;
        case MathML_EL_MTEXT:
          if (TtaGetFirstChild (el) == NULL)
            /* empty <mtext>. It will have to be parsed when the user enters
               some content */
            SetAttrParseMe (el, doc);
          break;
        case MathML_EL_MI:
          if (TtaGetFirstChild (el) == NULL)
            /* empty <mi> Replace it by an empty Construct */
            TtaChangeTypeOfElement (el, doc, MathML_EL_Construct);
          else
            SetFontstyleAttr (el, doc);
          break;
        case MathML_EL_MO:
	  /* if the content is a single character '-', replace it by the
	     minus operator (x2212) */
	  CheckMinus (el, doc);
          SetIntAddSpaceAttr (el, doc);
          SetIntVertStretchAttr (el, doc, 0, NULL);
          /* if the MO element is a child of a MROW (or equivalent) and if it
             contains a fence character, transform this MO into MF and
             transform the fence character into a Thot SYMBOL */
          CheckFence (el, doc);
          /* if the MO element contains an operator that should be
             large (&sum; for instance), enlarge it */
          CheckLargeOp (el, doc);
          break;
        case MathML_EL_MSPACE:
          break;
        case MathML_EL_MROW:
          /* Create placeholders within the MROW */
          CreatePlaceholders (TtaGetFirstChild (el), doc);
          break;
        case MathML_EL_MFRAC:
        case MathML_EL_BevelledMFRAC:
          /* end of a fraction. Create a Numerator and a Denominator */
          ok = CheckMathSubExpressions (el, MathML_EL_Numerator,
                                        MathML_EL_Denominator, 0, doc);
          break;
        case MathML_EL_MSQRT:
          /* end of a Square Root */
          /* Create placeholders within the element */
          CreatePlaceholders (TtaGetFirstChild (el), doc);
          /* Create a SqrtBase that contains all children of the MSQRT */
          CreateWrapper (el, MathML_EL_SqrtBase, doc);
          break;
        case MathML_EL_MROOT:
          /* end of a Root. Create a RootBase and an Index */
          ok = CheckMathSubExpressions (el, MathML_EL_RootBase,
                                        MathML_EL_Index, 0, doc);
          break;
        case MathML_EL_MENCLOSE:
          /* Create placeholders within the element */
          CreatePlaceholders (TtaGetFirstChild (el), doc);
          break;
        case MathML_EL_MSTYLE:
        case MathML_EL_MERROR:
        case MathML_EL_MPADDED:
        case MathML_EL_MPHANTOM:
          /* Create placeholders within the element */
          CreatePlaceholders (TtaGetFirstChild (el), doc);
          break;
        case MathML_EL_MFENCED:
          TransformMFENCED (el, doc);
          break;
        case MathML_EL_MSUB:
          /* end of a MSUB. Create Base and Subscript */
          ok = CheckMathSubExpressions (el, MathML_EL_Base,
                                        MathML_EL_Subscript, 0, doc);
          SetScriptShift (el, doc, MathML_ATTR_subscriptshift);
          SetIntVertStretchAttr (el, doc, MathML_EL_Base, NULL);
          break;
        case MathML_EL_MSUP:
          /* end of a MSUP. Create Base and Superscript */
          ok = CheckMathSubExpressions (el, MathML_EL_Base,
                                        MathML_EL_Superscript, 0, doc);
          SetScriptShift (el, doc, MathML_ATTR_superscriptshift);
          SetIntVertStretchAttr (el, doc, MathML_EL_Base, NULL);
          break;
        case MathML_EL_MSUBSUP:
          /* end of a MSUBSUP. Create Base, Subscript, and Superscript */
          ok = CheckMathSubExpressions (el, MathML_EL_Base,
                                        MathML_EL_Subscript,
                                        MathML_EL_Superscript, doc);
          SetScriptShift (el, doc, MathML_ATTR_subscriptshift);
          SetScriptShift (el, doc, MathML_ATTR_superscriptshift);
          SetIntVertStretchAttr (el, doc, MathML_EL_Base, NULL);
          break;
        case MathML_EL_MUNDER:
          /* end of a MUNDER. Create UnderOverBase, and Underscript */
          ok = CheckMathSubExpressions (el, MathML_EL_UnderOverBase,
                                        MathML_EL_Underscript, 0, doc);
          SetIntHorizStretchAttr (el, doc);
          SetIntVertStretchAttr (el, doc, MathML_EL_UnderOverBase, NULL);
          SetIntMovelimitsAttr (el, doc);
          break;
        case MathML_EL_MOVER:
          /* end of a MOVER. Create UnderOverBase, and Overscript */
          ok = CheckMathSubExpressions (el, MathML_EL_UnderOverBase,
                                        MathML_EL_Overscript, 0, doc);
          SetIntHorizStretchAttr (el, doc);
          SetIntVertStretchAttr (el, doc, MathML_EL_UnderOverBase, NULL);
          SetIntMovelimitsAttr (el, doc);
          break;
        case MathML_EL_MUNDEROVER:
          /* end of a MUNDEROVER. Create UnderOverBase, Underscript, and
             Overscript */
          ok = CheckMathSubExpressions (el, MathML_EL_UnderOverBase,
                                        MathML_EL_Underscript,
                                        MathML_EL_Overscript, doc);
          SetIntHorizStretchAttr (el, doc);
          SetIntVertStretchAttr (el, doc, MathML_EL_UnderOverBase, NULL);
          SetIntMovelimitsAttr (el, doc);
          break;
        case MathML_EL_MMULTISCRIPTS:
          /* end of a MMULTISCRIPTS. Create all elements defined in the
             MathML S schema */
          BuildMultiscript (el, doc);
          break;
        case MathML_EL_MTABLE:
          /* end of a MTABLE. Create all elements defined in the MathML S
             schema */
          CheckMTable (el, doc, TRUE);
          /* if the table has a rowalign attribute, process it */
          attrType.AttrSSchema = MathMLSSchema;
          attrType.AttrTypeNum = MathML_ATTR_rowalign;
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            HandleRowalignAttribute (attr, el, doc, FALSE);
          /* if the table has a columnalign attribute, process it */
          attrType.AttrTypeNum = MathML_ATTR_columnalign;
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            HandleColalignAttribute (attr, el, doc, FALSE, FALSE);
          /* process the rowspacing attribute, or set the top padding of the
             first row and the bottom padding of the last row to 0. */
          attrType.AttrSSchema = MathMLSSchema;
          attrType.AttrTypeNum = MathML_ATTR_rowspacing;
          attr = TtaGetAttribute (el, attrType);
          HandleRowspacingAttribute (attr, el, doc, FALSE);
          /* process the columnspacing attribute, or set the left padding of
             the first column and the right padding of the last column to 0 */
          attrType.AttrSSchema = MathMLSSchema;
          attrType.AttrTypeNum = MathML_ATTR_columnspacing;
          attr = TtaGetAttribute (el, attrType);
          HandleColumnspacingAttribute (attr, el, doc, FALSE);
          /* if the table has a rowlines attribute, process it */
          attrType.AttrSSchema = MathMLSSchema;
          attrType.AttrTypeNum = MathML_ATTR_rowlines;
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            HandleRowlinesAttribute (attr, el, doc, FALSE);
          /* if the table has a columnlines attribute, process it */
          attrType.AttrTypeNum = MathML_ATTR_columnlines;
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            HandleColumnlinesAttribute (attr, el, doc, FALSE);
          break;
        case MathML_EL_MTR:
          /* if the row has a columnalign attribute, process it */
          attrType.AttrSSchema = MathMLSSchema;
          attrType.AttrTypeNum = MathML_ATTR_columnalign;
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            HandleColalignAttribute (attr, el, doc, FALSE, TRUE);
          break;
        case MathML_EL_MLABELEDTR:
          /* if the row has a columnalign attribute, process it */
          attrType.AttrSSchema = MathMLSSchema;
          attrType.AttrTypeNum = MathML_ATTR_columnalign;
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            HandleColalignAttribute (attr, el, doc, FALSE, TRUE);
          break;
        case MathML_EL_MTD:
          /* Create placeholders within the table cell */
          CreatePlaceholders (TtaGetFirstChild (el), doc);
          break;
        case MathML_EL_MACTION:
	  /* get the 'selection' attribute */
	  attrType.AttrSSchema = MathMLSSchema;
          attrType.AttrTypeNum = MathML_ATTR_selection;
          attr = TtaGetAttribute (el, attrType);
          if (attr)
	    selection = TtaGetAttributeValue (attr);
	  else
	    selection = 1;   /* default value */
          /* put attribute IntHidden on all children of element maction
	     except the one at rank 'selection' */
	  child = TtaGetFirstChild (el);
	  attrType.AttrTypeNum = MathML_ATTR_IntHidden;
	  for (i = 1; child; i++)
	    {
	      if (i != selection)
		{
		  attr = TtaNewAttribute (attrType);
		  TtaAttachAttribute (child, attr, doc);
		  TtaSetAttributeValue (attr, MathML_ATTR_IntHidden_VAL_Yes_,
					child, doc);
		}
	      TtaNextSibling (&child);
	    }
          /* Create placeholders within the MACTION element */
          CreatePlaceholders (TtaGetFirstChild (el), doc);
          break;
        case MathML_EL_SEMANTICS:
          /* it's a <semantics> element */
          /* Evaluate what child element to be rendered */
          EvaluateChildRendering (el, doc);
          break;
        case MathML_EL_MGLYPH:
          CheckMandatoryAttribute (el, doc, MathML_ATTR_alt);
          break;
        default:
          break;
        }
      parent = TtaGetParent (el);
      if (parent)
        {
          parentType = TtaGetElementType (parent);
          if (parentType.ElSSchema != elType.ElSSchema)
            /* root of a MathML (sub-)tree, Create a MathML element if it is
               not present */
            if (elType.ElTypeNum != MathML_EL_MathML)
              {
                elType.ElSSchema = MathMLSSchema;
                elType.ElTypeNum = MathML_EL_MathML;
                new_ = TtaNewElement (doc, elType);
                TtaInsertSibling (new_, el, TRUE, doc);
                next = el;
                TtaNextSibling (&next);
                TtaRemoveTree (el, doc);
                TtaInsertFirstChild (&el, new_, doc);
                prev = el;
                while (next != NULL)
                  {
                    child = next;
                    TtaNextSibling (&next);
                    TtaRemoveTree (child, doc);
                    TtaInsertSibling (child, prev, FALSE, doc);
                    prev = child;
                  }
                /* associate a IntDisplaystyle attribute with the element
                   depending on its context */
                SetDisplaystyleMathElement (new_, doc);
                /* Create placeholders within the MathML element */
                CreatePlaceholders (new_, doc);
              }
        }
    }
  if (!ok)
    /* send an error message */
    *error = 1;
}

/*----------------------------------------------------------------------
  UnknownMathMLNameSpace
  The element doesn't belong to a supported namespace
  ----------------------------------------------------------------------*/
void               UnknownMathMLNameSpace (ParserData *context,
                                           Element *unknownEl,
                                           char* content)
{
  ElementType     elType;
  Element         elText;

  /* Create a new Invalid_element */
  elType.ElSSchema = GetXMLSSchema (MATH_TYPE, context->doc);
  elType.ElTypeNum = MathML_EL_Unknown_namespace;
  *unknownEl = TtaNewElement (context->doc, elType);
  if (*unknownEl != NULL)
    {
      XmlSetElemLineNumber (*unknownEl);
      InsertXmlElement (unknownEl);
      context->lastElementClosed = TRUE;
      elType.ElTypeNum = MathML_EL_TEXT_UNIT;
      elText = TtaNewElement (context->doc, elType);
      XmlSetElemLineNumber (elText);
      TtaInsertFirstChild (&elText, *unknownEl, context->doc);
      TtaSetTextContent (elText, (unsigned char *)content, context->language, context->doc);
      TtaSetAccessRight (elText, ReadOnly, context->doc);
    }
}

/*----------------------------------------------------------------------
  SetFontfamily
  -----------------------------------------------------------------------*/
void SetFontfamily (Document doc, Element el, char *value)
{
#define buflen 50
  char           css_command[buflen+20];
 
  sprintf (css_command, "font-family: %s", value);
  ParseHTMLSpecificStyle (el, css_command, doc, 2000, FALSE);
}

/*----------------------------------------------------------------------
  MathMLlinethickness
  The MathML attribute linthickness is associated with element el. Generate
  the corresponding style property for this element. 
  -----------------------------------------------------------------------*/
void MathMLlinethickness (Document doc, Element el, char *value)
{
#define buflen 50
  char           css_command[buflen+20];

  if (strcmp (value, "thin") == 0)
    strcpy (value, "1pt");
  else if (strcmp (value, "medium") == 0)
    strcpy (value, "1pt");
  else if (strcmp (value, "thick") == 0)
    strcpy (value, "2pt");
  sprintf (css_command, "stroke-width: %s", value);
  ParseHTMLSpecificStyle (el, css_command, doc, 2000, FALSE);
}

/*----------------------------------------------------------------------
  MathMLAttrToStyleProperty
  The MathML attribute attr is associated with element el. Generate
  the corresponding style property for this element.
  -----------------------------------------------------------------------*/
void MathMLAttrToStyleProperty (Document doc, Element el, char *value,
                                int attr)
{
  char           css_command[buflen+20], namedSpaceVal[20];
  int            i;

  switch (attr)
    {
    case MathML_ATTR_fontsize:
      sprintf (css_command, "font-size: %s", value);
      break;
    case MathML_ATTR_mathsize:
      if (strcmp (value, "small") == 0)
        strcpy (value, "80%");
      else if (strcmp (value, "normal") == 0)
        strcpy (value, "100%");
      else if (strcmp (value, "big") == 0)
        strcpy (value, "125%");
      sprintf (css_command, "font-size: %s", value);
      break;
    case MathML_ATTR_lspace:
    case MathML_ATTR_rspace:
      if (attr == MathML_ATTR_lspace)
        strcpy (css_command, "padding-left: ");
      else
        strcpy (css_command, "padding-right: ");
      ConvertNamedSpace (value, namedSpaceVal);
      if (namedSpaceVal[0] != EOS)
        /* it's a named space */
        strcat (css_command, namedSpaceVal);
      else
        {
          strcat (css_command, value);
          /* does the value contain an unit at the end? */
          i = strlen (value) - 1;
          if ((value[i] <= '9' && value[i] >= '0') ||
              value[i] == '.')
            /* it's just a number. Add the (implicit) unit: em */
            strcat (css_command, "em");
        }
      break;
    }
  ParseHTMLSpecificStyle (el, css_command, doc, 2000, FALSE);
}

/*----------------------------------------------------------------------
  MathMLSetScriptLevel
  A scriptlevel attribute with value value is associated with element el.
  Generate the corresponding style property for this element.
  -----------------------------------------------------------------------*/
void MathMLSetScriptLevel (Document doc, Element el, char *value)
{
  PresentationValue   pval;
  PresentationContext ctxt;
  ThotBool            relative;
  int                 percentage;

  ctxt = TtaGetSpecificStyleContext (doc);
  if (!value)
    /* remove the presentation rule */
    {
      ctxt->destroy = TRUE;
      pval.typed_data.value = 0;
      TtaSetStylePresentation (PRSize, el, NULL, ctxt, pval);
    }
  else
    {
      ctxt->destroy = FALSE;
      /* parse the attribute value (an optional sign and an integer) */
      value = (char*)TtaSkipBlanks (value);
      relative = (value[0] == '-' || value[0] == '+');
      value = ParseCSSUnit (value, &pval);
      if (pval.typed_data.unit != UNIT_REL &&
          pval.typed_data.real)
        /* this is an error: it should be an integer without any unit name */
        /* error */;
      else
        {
          if (relative)
            {
              percentage = 100;
              if (pval.typed_data.value == 0)
                /* scriptlevel="+0" */
                percentage = 100;
              else if (pval.typed_data.value == 1)
                /* scriptlevel="+1" */
                percentage = 71;
              else if (pval.typed_data.value == 2)
                /* scriptlevel="+2" */
                percentage = 50;
              else if (pval.typed_data.value >= 3)
                /* scriptlevel="+3" or more */
                percentage = 35;
              else if (pval.typed_data.value == -1)
                /* scriptlevel="-1" */
                percentage = 141;
              else if (pval.typed_data.value == -2)
                /* scriptlevel="-2" */
                percentage = 200;
              else if (pval.typed_data.value <= -3)
                /* scriptlevel="-3" or less */
                percentage = 282;
            }
          else
            /* absolute value */
            {
              percentage = 100;
              if (pval.typed_data.value == 0)
                /* scriptlevel="0" */
                percentage = 100;
              else if (pval.typed_data.value == 1)
                /* scriptlevel="1" */
                percentage = 71;
              else if (pval.typed_data.value == 2)
                /* scriptlevel="2" */
                percentage = 50;
              else if (pval.typed_data.value >= 3)
                /* scriptlevel="3" or more */
                percentage = 35;
            }
	  pval.typed_data.value = percentage;
	  pval.typed_data.unit = UNIT_PERCENT;
	  /* the specific presentation to be created is not a CSS rule */
	  ctxt->cssSpecificity = 2000;
	  TtaSetStylePresentation (PRSize, el, NULL, ctxt, pval);       
        }
    }
  TtaFreeMemory (ctxt);
}

/*----------------------------------------------------------------------
  MathMLSpacingAttr
  The MathML attribute attr (height, width or depth) is associated
  with element el (a mspace or mpadding).
  If value is not NULL, generate the corresponding Thot presentation rule for
  the element.
  If value is NULL, remove the corresponding Thot presentation rule.
  -----------------------------------------------------------------------*/
void MathMLSpacingAttr (Document doc, Element el, const char *value, int attr)
{
  ElementType         elType;
  PresentationValue   pval;
  PresentationContext ctxt;
  int                 ruleType;
  char*               tmp = NULL, *ptr;

  /* provisionally, handles only mspace elements */
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum != MathML_EL_MSPACE &&
      elType.ElTypeNum != MathML_EL_MPADDED &&
      elType.ElTypeNum != MathML_EL_MTABLE)
    return;
  switch (attr)
    {
    case MathML_ATTR_width_:
      ruleType = PRWidth;
      break;
    case MathML_ATTR_height_:
      ruleType = PRPaddingTop;
      break;
    case MathML_ATTR_depth_:
      ruleType = PRPaddingBottom;
      break;
    default:
      return;
    }
  ctxt = TtaGetSpecificStyleContext (doc);
  if (!value || (strcmp (value, "auto") == 0))
    /* remove the presentation rule */
    {
      ctxt->destroy = TRUE;
      pval.typed_data.value = 0;
      TtaSetStylePresentation (ruleType, el, NULL, ctxt, pval);
    }
  else
    {
      ctxt->destroy = FALSE;
      /* parse the attribute value (a number followed by a unit) */
      value = (char*)TtaSkipBlanks (value);
      tmp = TtaStrdup(value);
      ptr = ParseCSSUnit (tmp, &pval);
      /***** we should accept namedspace for width *****/
      if (pval.typed_data.unit != UNIT_INVALID)
        {
          if (pval.typed_data.unit == UNIT_BOX)
            pval.typed_data.unit = UNIT_PX;
          /* the specific presentation to be created is not a CSS rule */
          ctxt->cssSpecificity = 2000;
          TtaSetStylePresentation (ruleType, el, NULL, ctxt, pval);
        }
      TtaFreeMemory(tmp);
    }
  TtaFreeMemory (ctxt);
}

/*----------------------------------------------------------------------
  MathMLSetDisplaystyleAttr
  The attribute displaystyle is associated  with element el (which
  should be a <mstyle> or a <mtable> element).
  Generate or set the corresponding internal attribute accordingly.
  ----------------------------------------------------------------------*/
void MathMLSetDisplaystyleAttr (Element el, Attribute attr, Document doc,
                                ThotBool delete_)
{
  int            val, intVal;
  ElementType    elType;
  AttributeType  attrType;
  Attribute      intAttr;

  intVal = 0;
  /* get the internal attribute */
  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = MathML_ATTR_IntDisplaystyle;
  intAttr = TtaGetAttribute (el, attrType);
  if (delete_)
    /* attribute displaystyle has been deleted */
    {
      if (elType.ElTypeNum == MathML_EL_MSTYLE)
        /* it's a mstyle element. Just remove the internal attribute */
        {
          if (intAttr)
            TtaRemoveAttribute (el, intAttr, doc);
        }
      else if (elType.ElTypeNum == MathML_EL_MTABLE)
        /* it's a matable element, set the default value (false) */
        intVal = MathML_ATTR_IntDisplaystyle_VAL_false;
    }
  else
    {
      val = TtaGetAttributeValue (attr);
      if (val == MathML_ATTR_displaystyle_VAL_true)
        intVal = MathML_ATTR_IntDisplaystyle_VAL_true;
      else
        intVal = MathML_ATTR_IntDisplaystyle_VAL_false;
    }
  /* create the IntDisplaystyle attribute if needed and set its value */
  if (intVal)
    {
      if (!intAttr)
        {
          intAttr = TtaNewAttribute (attrType);
          TtaAttachAttribute (el, intAttr, doc);
        }
      TtaSetAttributeValue (intAttr, intVal, el, doc);
    }
  ApplyDisplaystyle (el, doc);
}

/*----------------------------------------------------------------------
  MathMLAttributeComplete
  The XML parser has completed parsing attribute attr (as well as its value)
  that is associated with element el in document doc.
  ----------------------------------------------------------------------*/
void MathMLAttributeComplete (Attribute attr, Element el, Document doc)
{
  AttributeType     attrType, depAttrType;
  int		     attrKind;
  ElementType       elType;
  char             *value;
  int               val, length;
  Attribute         intAttr;

  /* first get the type of that attribute */
  TtaGiveAttributeType (attr, &attrType, &attrKind);

  if (attrType.AttrTypeNum == MathML_ATTR_bevelled)
    /* it's a bevelled attribute */
    {
      val = TtaGetAttributeValue (attr);
      if (val == MathML_ATTR_bevelled_VAL_true)
        /* bevelled = true.  Transform MFRAC into BevelledMFRAC */
        {
          elType = TtaGetElementType (el);
          if (elType.ElTypeNum == MathML_EL_MFRAC)
            TtaChangeTypeOfElement (el, doc, MathML_EL_BevelledMFRAC);
        }
    }

  else if (attrType.AttrTypeNum == MathML_ATTR_rowalign_mtr)
    {
      /* create an equivalent IntRowAlign attribute on the same element */
      attrType.AttrTypeNum = MathML_ATTR_IntRowAlign;
      intAttr = TtaGetAttribute (el, attrType);
      if (!intAttr)
        /* no IntRowAlign attribute, create one */
        {
          intAttr = TtaNewAttribute (attrType);
          TtaAttachAttribute (el, intAttr, doc);
        }
      val = TtaGetAttributeValue (attr);
      TtaSetAttributeValue (intAttr, val, el, doc);
    }

  else if (attrType.AttrTypeNum == MathML_ATTR_columnalign_mtd)
    {
      /* create an equivalent IntColAlign attribute on the same element */
      attrType.AttrTypeNum = MathML_ATTR_IntColAlign;
      intAttr = TtaGetAttribute (el, attrType);
      if (!intAttr)
        /* no IntColAlign attribute, create one */
        {
          intAttr = TtaNewAttribute (attrType);
          TtaAttachAttribute (el, intAttr, doc);
        }
      val = TtaGetAttributeValue (attr);
      TtaSetAttributeValue (intAttr, val, el, doc);
    }

  /* don't handle attributes columnalign, rowalign, columnlines and rowlines
     now: the table or the row is not complete yet. Handle them when the
     element is complete.
  */

  else if (attrType.AttrTypeNum == MathML_ATTR_display_)
    /* it's a display attribute on element <math>, set the corresponding
       internal attribute IntDisplaystyle */
    SetDisplaystyleMathElement (el, doc);

  else if (attrType.AttrTypeNum == MathML_ATTR_displaystyle)
    /* it's a displaystyle attribute */
    MathMLSetDisplaystyleAttr (el, attr, doc, FALSE);

  else if (attrType.AttrTypeNum == MathML_ATTR_framespacing)
    /* it's a framespacing attribute */
    HandleFramespacingAttribute (attr, el, doc, FALSE);

  else if (attrType.AttrTypeNum == MathML_ATTR_Language)
    {
      if (el == TtaGetRootElement (doc))
        /* it's the lang attribute on the root element */
        /* set the RealLang attribute */
        {
          depAttrType.AttrSSchema = attrType.AttrSSchema ;
          depAttrType.AttrTypeNum = MathML_ATTR_RealLang;
          if (!TtaGetAttribute (el, depAttrType))
            /* it's not present. Add it */
            {
              intAttr = TtaNewAttribute (depAttrType);
              TtaAttachAttribute (el, intAttr, doc);
              TtaSetAttributeValue (intAttr, MathML_ATTR_RealLang_VAL_Yes_,
                                    el, doc);
            }
        }
    }

  else if (attrType.AttrTypeNum == MathML_ATTR_color ||
           attrType.AttrTypeNum == MathML_ATTR_mathcolor ||
           attrType.AttrTypeNum == MathML_ATTR_background_ ||
           attrType.AttrTypeNum == MathML_ATTR_mathbackground ||
           attrType.AttrTypeNum == MathML_ATTR_fontsize ||
           attrType.AttrTypeNum == MathML_ATTR_mathsize ||
           attrType.AttrTypeNum == MathML_ATTR_fontfamily ||
           attrType.AttrTypeNum == MathML_ATTR_linethickness ||
           attrType.AttrTypeNum == MathML_ATTR_lspace ||
           attrType.AttrTypeNum == MathML_ATTR_rspace ||
           attrType.AttrTypeNum == MathML_ATTR_scriptlevel ||
           attrType.AttrTypeNum == MathML_ATTR_width_ ||
           attrType.AttrTypeNum == MathML_ATTR_height_ ||
           attrType.AttrTypeNum == MathML_ATTR_depth_)
    {
      length = TtaGetTextAttributeLength (attr);
      if (length >= buflen)
        length = buflen - 1;
      if (length > 0)
        {
          value = (char *)TtaGetMemory (buflen);
          value[0] = EOS;
          TtaGiveTextAttributeValue (attr, value, &length);
          switch (attrType.AttrTypeNum)
            {
            case MathML_ATTR_color:
              /* deprecated attribute */
              /* if the same element has a mathcolor attribute, ignore
                 the color attribute */
              depAttrType.AttrSSchema = attrType.AttrSSchema ;
              depAttrType.AttrTypeNum = MathML_ATTR_mathcolor;
              if (!TtaGetAttribute (el, depAttrType))
                HTMLSetForegroundColor (doc, el, 2000, value);
              break;
            case MathML_ATTR_mathcolor:
              HTMLSetForegroundColor (doc, el, 2000, value);
              break;
            case MathML_ATTR_background_:
              /* deprecated attribute */
              /* if the same element has a mathbackground attribute, ignore
                 the background attribute */
              depAttrType.AttrSSchema = attrType.AttrSSchema;
              depAttrType.AttrTypeNum = MathML_ATTR_mathbackground;
              if (!TtaGetAttribute (el, depAttrType))
                HTMLSetBackgroundColor (doc, el, 2000, value);
              break;
            case MathML_ATTR_mathbackground:
              HTMLSetBackgroundColor (doc, el, 2000, value);
              break;
            case MathML_ATTR_fontfamily:
              SetFontfamily (doc, el, value);
              break;
            case MathML_ATTR_linethickness:
              MathMLlinethickness (doc, el, value);
              break;
            case MathML_ATTR_fontsize:
              /* deprecated attribute */
              /* if the same element has a mathsize attribute, ignore
                 the fontsize attribute */
              depAttrType.AttrSSchema = attrType.AttrSSchema;
              depAttrType.AttrTypeNum = MathML_ATTR_mathsize;
              if (!TtaGetAttribute (el, depAttrType))
                MathMLAttrToStyleProperty (doc, el, value,
                                           attrType.AttrTypeNum);
              break;
            case MathML_ATTR_mathsize:
            case MathML_ATTR_lspace:
            case MathML_ATTR_rspace:
              MathMLAttrToStyleProperty (doc, el, value,attrType.AttrTypeNum);
              break;
            case MathML_ATTR_scriptlevel:
              MathMLSetScriptLevel (doc, el, value);
              break;
            case MathML_ATTR_width_:
            case MathML_ATTR_height_:
            case MathML_ATTR_depth_:
              MathMLSpacingAttr (doc, el, value, attrType.AttrTypeNum);
              break;
            case MathML_ATTR_id:
              CheckUniqueName (el, doc, attr, attrType);
              break;
            default:
              break;
            }
          TtaFreeMemory (value);
        }
    }
}

/*----------------------------------------------------------------------
  MathMLGetDTDName
  ----------------------------------------------------------------------*/
void MathMLGetDTDName (char *DTDname, char *elementName)
{
  /* no other DTD allowed within MathML elements */
  strcpy (DTDname, "");
}

/* end of module */
