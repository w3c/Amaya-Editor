/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 *
 * MathMLbuilder
 *
 * Author: V. Quint
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "html2thot_f.h"
#include "HTMLactions_f.h"
#include "MathML.h"
#include "parser.h"
#include "styleparser_f.h"
#include "style.h"
#include "undo.h"
#include "Xml2thot_f.h"

/* Define a pointer to let parser functions access the Math entity table */
extern XmlEntity *pMathEntityTable;

/* mapping table of attribute values */
static AttrValueMapping MathMLAttrValueMappingTable[] =
{
 {MathML_ATTR_accent, "true", MathML_ATTR_accent_VAL_true},
 {MathML_ATTR_accent, "false", MathML_ATTR_accent_VAL_false},

 {MathML_ATTR_accentunder, "true", MathML_ATTR_accentunder_VAL_true},
 {MathML_ATTR_accentunder, "false", MathML_ATTR_accentunder_VAL_false},

 {MathML_ATTR_align, "top", MathML_ATTR_align_VAL_top_},
 {MathML_ATTR_align, "bottom", MathML_ATTR_align_VAL_bottom_},
 {MathML_ATTR_align, "center", MathML_ATTR_align_VAL_center},
 {MathML_ATTR_align, "baseline", MathML_ATTR_align_VAL_baseline},
 {MathML_ATTR_align, "axis", MathML_ATTR_align_VAL_axis},

 {MathML_ATTR_bevelled, "true", MathML_ATTR_bevelled_VAL_true},
 {MathML_ATTR_bevelled, "false", MathML_ATTR_bevelled_VAL_false},

 {MathML_ATTR_columnalign_mtd, "center", MathML_ATTR_columnalign_mtd_VAL_center_},
 {MathML_ATTR_columnalign_mtd, "left", MathML_ATTR_columnalign_mtd_VAL_left_},
 {MathML_ATTR_columnalign_mtd, "right", MathML_ATTR_columnalign_mtd_VAL_right_},

 {MathML_ATTR_denomalign, "center", MathML_ATTR_denomalign_VAL_center_},
 {MathML_ATTR_denomalign, "left", MathML_ATTR_denomalign_VAL_left_},
 {MathML_ATTR_denomalign, "right", MathML_ATTR_denomalign_VAL_right_},

 {MathML_ATTR_display, "block", MathML_ATTR_display_VAL_block},
 {MathML_ATTR_display, "display", MathML_ATTR_display_VAL_block},
 {MathML_ATTR_display, "inline", MathML_ATTR_display_VAL_inline_},

 {MathML_ATTR_displaystyle, "true", MathML_ATTR_displaystyle_VAL_true},
 {MathML_ATTR_displaystyle, "false", MathML_ATTR_displaystyle_VAL_false},

 {MathML_ATTR_edge, "left", MathML_ATTR_edge_VAL_left_},
 {MathML_ATTR_edge, "right", MathML_ATTR_edge_VAL_right_},

 {MathML_ATTR_equalcolumns, "true", MathML_ATTR_equalcolumns_VAL_true},
 {MathML_ATTR_equalcolumns, "false", MathML_ATTR_equalcolumns_VAL_false},

 {MathML_ATTR_equalrows, "true", MathML_ATTR_equalrows_VAL_true},
 {MathML_ATTR_equalrows, "false", MathML_ATTR_equalrows_VAL_false},

 {MathML_ATTR_fence, "true", MathML_ATTR_fence_VAL_true},
 {MathML_ATTR_fence, "false", MathML_ATTR_fence_VAL_false},

 {MathML_ATTR_fontstyle, "italic", MathML_ATTR_fontstyle_VAL_italic},
 {MathML_ATTR_fontstyle, "normal", MathML_ATTR_fontstyle_VAL_normal_},

 {MathML_ATTR_fontweight, "normal", MathML_ATTR_fontweight_VAL_normal_},
 {MathML_ATTR_fontweight, "bold", MathML_ATTR_fontweight_VAL_bold_},

 {MathML_ATTR_form, "prefix", MathML_ATTR_form_VAL_prefix},
 {MathML_ATTR_form, "infix", MathML_ATTR_form_VAL_infix},
 {MathML_ATTR_form, "postfix", MathML_ATTR_form_VAL_postfix},

 {MathML_ATTR_frame, "none", MathML_ATTR_frame_VAL_none_},
 {MathML_ATTR_frame, "solid", MathML_ATTR_frame_VAL_solid_},
 {MathML_ATTR_frame, "dashed", MathML_ATTR_frame_VAL_dashed_},

 {MathML_ATTR_groupalign_malgr, "left", MathML_ATTR_groupalign_malgr_VAL_left_},
 {MathML_ATTR_groupalign_malgr, "center", MathML_ATTR_groupalign_malgr_VAL_center_},
 {MathML_ATTR_groupalign_malgr, "right", MathML_ATTR_groupalign_malgr_VAL_right_},
 {MathML_ATTR_groupalign_malgr, "decimalpoint", MathML_ATTR_groupalign_malgr_VAL_decimalpoint},

 {MathML_ATTR_largeop, "true", MathML_ATTR_largeop_VAL_true},
 {MathML_ATTR_largeop, "false", MathML_ATTR_largeop_VAL_false},

 {MathML_ATTR_linebreak_, "auto", MathML_ATTR_linebreak__VAL_auto_},
 {MathML_ATTR_linebreak_, "newline", MathML_ATTR_linebreak__VAL_newline},
 {MathML_ATTR_linebreak_, "indentingnewline", MathML_ATTR_linebreak__VAL_indentingnewline},
 {MathML_ATTR_linebreak_, "nobreak", MathML_ATTR_linebreak__VAL_nobreak_},
 {MathML_ATTR_linebreak_, "goodbreak", MathML_ATTR_linebreak__VAL_goodbreak},
 {MathML_ATTR_linebreak_, "badbreak", MathML_ATTR_linebreak__VAL_badbreak},

 {MathML_ATTR_mathvariant, "normal", MathML_ATTR_mathvariant_VAL_normal_},
 {MathML_ATTR_mathvariant, "bold", MathML_ATTR_mathvariant_VAL_bold_},
 {MathML_ATTR_mathvariant, "italic", MathML_ATTR_mathvariant_VAL_italic},
 {MathML_ATTR_mathvariant, "bold-italic", MathML_ATTR_mathvariant_VAL_bold_italic},
 {MathML_ATTR_mathvariant, "double-struck", MathML_ATTR_mathvariant_VAL_double_struck},
 {MathML_ATTR_mathvariant, "bold-fraktur", MathML_ATTR_mathvariant_VAL_bold_fraktur},
 {MathML_ATTR_mathvariant, "script", MathML_ATTR_mathvariant_VAL_script},
 {MathML_ATTR_mathvariant, "bold-script", MathML_ATTR_mathvariant_VAL_bold_script},
 {MathML_ATTR_mathvariant, "fraktur", MathML_ATTR_mathvariant_VAL_fraktur},
 {MathML_ATTR_mathvariant, "sans-serif", MathML_ATTR_mathvariant_VAL_sans_serif},
 {MathML_ATTR_mathvariant, "bold-sans-serif", MathML_ATTR_mathvariant_VAL_bold_sans_serif},
 {MathML_ATTR_mathvariant, "sans-serif-italic", MathML_ATTR_mathvariant_VAL_sans_serif_italic},
 {MathML_ATTR_mathvariant, "sans-serif-bold-italic", MathML_ATTR_mathvariant_VAL_sans_serif_bold_italic},
 {MathML_ATTR_mathvariant, "monospace", MathML_ATTR_mathvariant_VAL_monospace},

 {MathML_ATTR_movablelimits, "true", MathML_ATTR_movablelimits_VAL_true},
 {MathML_ATTR_movablelimits, "false", MathML_ATTR_movablelimits_VAL_false},

 {MathML_ATTR_notation, "longdiv", MathML_ATTR_notation_VAL_longdiv},
 {MathML_ATTR_notation, "actuarial", MathML_ATTR_notation_VAL_actuarial},
 {MathML_ATTR_notation, "radical", MathML_ATTR_notation_VAL_radical},

 {MathML_ATTR_numalign, "center", MathML_ATTR_numalign_VAL_center_},
 {MathML_ATTR_numalign, "left", MathML_ATTR_numalign_VAL_left_},
 {MathML_ATTR_numalign, "right", MathML_ATTR_numalign_VAL_right_},

 {MathML_ATTR_overflow, "scroll", MathML_ATTR_overflow_VAL_scroll},
 {MathML_ATTR_overflow, "elide", MathML_ATTR_overflow_VAL_elide},
 {MathML_ATTR_overflow, "truncate", MathML_ATTR_overflow_VAL_truncate},
 {MathML_ATTR_overflow, "scale", MathML_ATTR_overflow_VAL_scale_},

 {MathML_ATTR_rowalign_mtr, "top", MathML_ATTR_rowalign_mtr_VAL_top_},
 {MathML_ATTR_rowalign_mtr, "bottom", MathML_ATTR_rowalign_mtr_VAL_bottom_},
 {MathML_ATTR_rowalign_mtr, "center", MathML_ATTR_rowalign_mtr_VAL_center},
 {MathML_ATTR_rowalign_mtr, "baseline", MathML_ATTR_rowalign_mtr_VAL_baseline},
 {MathML_ATTR_rowalign_mtr, "axis", MathML_ATTR_rowalign_mtr_VAL_axis},

 {MathML_ATTR_separator, "true", MathML_ATTR_separator_VAL_true},
 {MathML_ATTR_separator, "false", MathML_ATTR_separator_VAL_false},

 {MathML_ATTR_side, "left", MathML_ATTR_side_VAL_left_},
 {MathML_ATTR_side, "leftoverlap", MathML_ATTR_side_VAL_leftoverlap},
 {MathML_ATTR_side, "right", MathML_ATTR_side_VAL_right_},
 {MathML_ATTR_side, "rightoverlap", MathML_ATTR_side_VAL_rightoverlap},

 {MathML_ATTR_stretchy, "true", MathML_ATTR_stretchy_VAL_true},
 {MathML_ATTR_stretchy, "false", MathML_ATTR_stretchy_VAL_false},

 {MathML_ATTR_symmetric, "true", MathML_ATTR_symmetric_VAL_true},
 {MathML_ATTR_symmetric, "false", MathML_ATTR_symmetric_VAL_false},

 {MathML_ATTR_xml_space, "default", MathML_ATTR_xml_space_VAL_xml_space_default},
 {MathML_ATTR_xml_space, "preserve", MathML_ATTR_xml_space_VAL_xml_space_preserve},

 {0, "", 0}			/* Last entry. Mandatory */
};

#define MaxMsgLength 200

#include "HTMLtable_f.h"
#include "Mathedit_f.h"
#include "XMLparser_f.h"
#include "styleparser_f.h"
#include "fetchXMLname_f.h"

/*----------------------------------------------------------------------
   MapMathMLAttribute
   Search in the Attribute Mapping Table the entry for the
   attribute of name Attr and returns the corresponding Thot attribute type.
  ----------------------------------------------------------------------*/
void MapMathMLAttribute (char *attrName, AttributeType *attrType,
			 char *elementName, ThotBool *level, Document doc)
{
  attrType->AttrSSchema = GetMathMLSSchema (doc);
  MapXMLAttribute (MATH_TYPE, attrName, elementName, level, doc, &(attrType->AttrTypeNum));
}

/*----------------------------------------------------------------------
   MapMathMLAttributeValue
   Search in the Attribute Value Mapping Table the entry for the attribute
   ThotAtt and its value AttrVal. Returns the corresponding Thot value.
  ----------------------------------------------------------------------*/
void MapMathMLAttributeValue (char *AttrVal, AttributeType attrType,
			      int *value)
{
  int                 i;

  *value = 0;
  i = 0;
  while (MathMLAttrValueMappingTable[i].ThotAttr != attrType.AttrTypeNum &&
	 MathMLAttrValueMappingTable[i].ThotAttr != 0)
    i++;
  if (MathMLAttrValueMappingTable[i].ThotAttr == attrType.AttrTypeNum)
    do
      if (!strcmp (MathMLAttrValueMappingTable[i].XMLattrValue, AttrVal))
	*value = MathMLAttrValueMappingTable[i].ThotAttrValue;
      else
	i++;
    while (*value == 0 &&
	   MathMLAttrValueMappingTable[i].ThotAttr == attrType.AttrTypeNum);
}

/*----------------------------------------------------------------------
   MapMathMLEntity
   Search that entity in the entity table and return the corresponding value.
  ----------------------------------------------------------------------*/
void MapMathMLEntity (char *entityName, char *entityValue,
		      char *alphabet)
{
   int	i;
  ThotBool       found;

  found = FALSE;
  for (i = 0; pMathEntityTable[i].charCode >= 0 && !found; i++)
    found = !strcmp (pMathEntityTable[i].charName, entityName);

  if (found)
    /* entity found */
    {
      i--;
      entityValue[0] = (unsigned char) pMathEntityTable[i].charCode;
      entityValue[1] = EOS;
      *alphabet = 'G';
      /* *alphabet = pMathEntityTable[i].charAlphabet;*/
    }
  else
    {
      entityValue[0] = EOS;
      *alphabet = EOS;
    }
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
  Element	child, parent;
  ThotBool	ret;
 
  ret = FALSE;
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == MathML_EL_MS ||
      elType.ElTypeNum == MathML_EL_MSPACE ||
      elType.ElTypeNum == MathML_EL_MROW ||
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
  else
     if (elType.ElTypeNum == MathML_EL_MO)
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
   AttributeType attrType;
   ThotBool	 create, stretchableSubsup;

   if (!el)
      return;
   elType.ElSSchema = GetMathMLSSchema (doc);
   prev = NULL;
   create = TRUE;
   sibling = el;
   while (sibling != NULL)
      {
      if (!ElementNeedsPlaceholder (sibling))
	 create = FALSE;
      else
	 {
	 if (sibling == el)
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
		     /* the first child of the MROW element is not a MF */
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
	    TtaSetAttributeValue (attr, MathML_ATTR_IntPlaceholder_VAL_yes_,
				  constr, doc);
	    }
	 create = TRUE;
	 }
      prev = sibling;
      TtaNextSibling (&sibling);
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
  CheckMathSubExpressions
  Children of element el should be of type type1, type2, and type3.
  If they are not, wrap them in elements of these types.
  If element el has too many or not enough children, return FALSE.
  ----------------------------------------------------------------------*/
static ThotBool CheckMathSubExpressions (Element el, int type1, int type2, int type3, Document doc)
{
  Element	child, new, prev;
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
	XmlParseError (errorParsing, msgBuffer, 0);
	result = FALSE;
      }
    }
  else
    if (!child)
      /* a first child is expected and it's missing */
      {
	sprintf (msgBuffer, "Missing subexpression in %s",
		 TtaGetElementTypeName (TtaGetElementType (el)));
	XmlParseError (errorParsing, msgBuffer, 0);
	result = FALSE;
      }
    else
      {
      elType.ElTypeNum = type1;
      childType = TtaGetElementType (child);
      if (TtaSameTypes (childType, elType) == 0)
	{
	  TtaRemoveTree (child, doc);	
	  new = TtaNewElement (doc, elType);
	  if (prev == NULL)
	    TtaInsertFirstChild (&new, el, doc);
	  else
	    TtaInsertSibling (new, prev, FALSE, doc);
	  TtaInsertFirstChild (&child, new, doc);
	  CreatePlaceholders (child, doc);
	  child = new;
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
	    XmlParseError (errorParsing, msgBuffer, 0);
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
	      XmlParseError (errorParsing, msgBuffer, 0);
	      result = FALSE;
	    }
	  else
	    {
	      elType.ElTypeNum = type2;
	      childType = TtaGetElementType (child);
	      if (TtaSameTypes (childType, elType) == 0)
		{
		  TtaRemoveTree (child, doc);
		  new = TtaNewElement (doc, elType);
		  TtaInsertSibling (new, prev, FALSE, doc);
		  TtaInsertFirstChild (&child, new, doc);
		  CreatePlaceholders (child, doc);
		  child = new;
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
		    XmlParseError (errorParsing, msgBuffer, 0);
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
		      XmlParseError (errorParsing, msgBuffer, 0);
		      result = FALSE;
		    }
		  else
		    {
		      elType.ElTypeNum = type3;
		      childType = TtaGetElementType (child);
		      if (TtaSameTypes (childType, elType) == 0)
			{
			  TtaRemoveTree (child, doc);
			  new = TtaNewElement (doc, elType);
			  TtaInsertSibling (new, prev, FALSE, doc);
			  TtaInsertFirstChild (&child, new, doc);
			  CreatePlaceholders (child, doc);
			  child = new;
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
		      XmlParseError (errorParsing, msgBuffer, 0);
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
  ElementType	elType;
  Attribute	attr;
  AttributeType	attrType;
  Language	lang;
  CHAR_T        text[2];
  char	        alphabet;
  unsigned char c;
  int		len;

  if (el == NULL)
     return;
  child = TtaGetFirstChild (el);
  if (child)
     {
     elType = TtaGetElementType (child);
     while (elType.ElTypeNum == MathML_EL_MROW && child)
        /* the first child is a mrow. Look whether it contains a single
           child of type mo */
        {
        child = TtaGetFirstChild (child);
	if (child)
	  {
	    sibling = child;
	    TtaNextSibling (&sibling);
	    if (sibling == NULL)
	      /* the mrow element has a single child. Get its type */
	      elType = TtaGetElementType (child);
	    else
	      child = NULL;
	  }
	}
     if (elType.ElTypeNum == MathML_EL_MO && child)
	/* the first child is a MO */
        {
        sibling = child;
        TtaNextSibling (&sibling);
	if (sibling == NULL)
	   /* there is no other child */
	   {
	   textEl = TtaGetFirstChild (child);
	   elType = TtaGetElementType (textEl);
	   if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
	      /* the MO child contains a TEXT element */
	      {
	      len = TtaGetVolume (textEl);
	      if (len == 1)
		 /* the TEXT element contains a single character */
		 {
		 c = EOS;
		 /* get that character */
		 len = 2;
		 TtaGiveBufferContent (textEl, text, len, &lang);
		 alphabet = TtaGetAlphabet (lang);
		 if (alphabet == 'L')
		    {
		    if (text[0] == '-' || text[0] == '_' ||
			text[0] == 175)
		      /* a horizontal line in the middle of the box */
		      c = 'h'; 
		    }
		 else if (alphabet == 'G')
		    /* a single Symbol character */
		    {
		    if (text[0] == 172)
		      c = 'L';  /* arrow left */
		    else if (text[0] == 174)
		      c = 'R';  /* arrow right */
		    else if (text[0] == 45)    /* - (minus) */
		      /* a horizontal line in the middle of the box */
		      c = 'h'; 
		    else if (text[0] == 132)
		      c = 'o';  /* Over brace */
		    else if (text[0] == 133)
		      c = 'u';  /* Under brace */
		    }
		 if (c != EOS)
		    {
		    /* attach a IntHorizStretch attribute to the mo */
		    attrType.AttrSSchema = elType.ElSSchema;
		    attrType.AttrTypeNum = MathML_ATTR_IntHorizStretch;
		    attr = TtaNewAttribute (attrType);
		    TtaAttachAttribute (el, attr, doc);
		    TtaSetAttributeValue (attr, MathML_ATTR_IntHorizStretch_VAL_yes_, el, doc);
		    /* replace the TEXT element by a Thot SYMBOL element */
		    elType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
		    symbolEl = TtaNewElement (doc, elType);
		    TtaInsertSibling (symbolEl, textEl, FALSE, doc);
		    if (selEl != NULL)
		      if (*selEl == textEl)
			*selEl = symbolEl;
		    TtaDeleteTree (textEl, doc);
		    if (c != EOS)
		      TtaSetGraphicsShape (symbolEl, c, doc);
		    }
		 }
	      }
	   }
	}
     }
}
 
/*----------------------------------------------------------------------
   SetIntHorizStretchAttr

   Put a IntHorizStretch attribute on all children of element el which
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
   stretchable symbol (integral).
 -----------------------------------------------------------------------*/
void SetIntVertStretchAttr (Element el, Document doc, int base, Element* selEl)
{
  Element	child, sibling, textEl, symbolEl, parent, operator, next;
  ElementType	elType;
  Attribute	attr;
  AttributeType	attrType;
  SSchema       MathMLSSchema;
  Language	lang;
  char		alphabet;
#define buflen 50
  CHAR_T        text[buflen];
  unsigned char c;
  int		len, i;
  ThotBool      inbase, integral;

  if (el == NULL)
    return;
  operator = NULL;
  inbase = FALSE;
  MathMLSSchema = TtaGetElementType(el).ElSSchema;
    
  if (base == 0)
    /* it's a MO */
    {
      parent = TtaGetParent (el);
      if (parent != NULL)
	{
	  /* don't process the mo if it is within a base. It will be processed
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
	       elType.ElTypeNum != MathML_EL_MUNDEROVER))
	    operator = el;
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
	  if (elType.ElTypeNum == base && elType.ElSSchema == MathMLSSchema)
	    /* the first child is a Base or UnderOverBase */
	    {
	      child = TtaGetFirstChild (child);
	      if (child != NULL)
		{
		  elType = TtaGetElementType (child);
		  if (elType.ElTypeNum == MathML_EL_MO &&
		      elType.ElSSchema == MathMLSSchema)
		    /* its first child is a MO */
		    {
		      sibling = child;
		      TtaNextSibling (&sibling);
		      if (sibling == NULL)
			/* there is no other child */
			{
			  operator = child;
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
  if (operator)
    {
      textEl = TtaGetFirstChild (operator);
      if (textEl != NULL)
        {
	  elType = TtaGetElementType (textEl);
	  if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
	    {
	      len = TtaGetVolume (textEl);
	      if (len >= 1)
		{
		  if (len >= buflen)
		    len = buflen-1;
		  len++;
		  TtaGiveBufferContent (textEl, text, len, &lang);
		  len --;
		  alphabet = TtaGetAlphabet (lang);
		  if (alphabet == 'G')
		    /* Adobe Symbol character set */
		    {
		    integral = TRUE;
		    /* check all characters in this TEXT element */
		    for (i = 0; i < len; i++)
		      if (text[i] != 242)
		        /**** accept also other symbols like double or triple
			      integral, contour integral, etc. ****/
			integral = FALSE;
		    if (integral)
		      /* the operator contains only integral symbols */
		      {
			/* attach a IntVertStretch attribute */
			attrType.AttrSSchema = MathMLSSchema;
			attrType.AttrTypeNum = MathML_ATTR_IntVertStretch;
			attr = TtaNewAttribute (attrType);
			TtaAttachAttribute (el, attr, doc);
			TtaSetAttributeValue (attr,
					   MathML_ATTR_IntVertStretch_VAL_yes_,
					   el, doc);
			TtaRegisterAttributeCreate (attr, el, doc);

			/* replace the Integral characters by a Thot SYMBOL
			   element. If there are several such characters in
			   the mo (multiple integral), replace them too. */
			do
			  {
			    /* replace the TEXT element by a Thot SYMBOL */
			    elType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
			    elType.ElSSchema = MathMLSSchema;
			    for (i = 0; i < len; i++)
			      if (text[i] == 242)
			        {
				symbolEl = TtaNewElement (doc, elType);
				TtaInsertSibling (symbolEl, textEl, TRUE,doc);
				if (selEl != NULL)
				  if (*selEl == textEl)
				    *selEl = symbolEl;
				c = 'i';
				TtaSetGraphicsShape (symbolEl, c, doc);
				TtaRegisterElementCreate (symbolEl, doc);
			        }
			    TtaRegisterElementDelete (textEl, doc);
			    TtaDeleteTree (textEl, doc);
			    /* is there an other text element after the
			       integral symbol? */
			    textEl = symbolEl; TtaNextSibling (&textEl);
			    if (textEl)
			      {
				elType = TtaGetElementType (textEl);
				if (elType.ElTypeNum != MathML_EL_TEXT_UNIT)
				  textEl = NULL;
				else
				  /* there is another text element.
				     Is it a single integral symbol? */
				  {
				    len = TtaGetVolume (textEl);
				    if (len < 1)
				      /* not a single character */
				      textEl = NULL;
				    else
				      {
					if (len >= buflen)
					  len = buflen-1;
					len++;
					TtaGiveBufferContent (textEl, text,
							      len, &lang); 
					alphabet = TtaGetAlphabet (lang);
					if (alphabet != 'G')
					  /* not the right alphabet for an
					     integral*/
					  textEl = NULL;
					else
					  if (text[0] != 242)
					    /* not an integral symbol */
					    textEl = NULL;
				      }
				  }
			      }
			  }
			while (textEl);

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
				       ChangeElementType (sibling,
							 MathML_EL_Construct1);
				       TtaInsertSibling (sibling, parent,
							 FALSE, doc);
				       TtaRegisterElementCreate (sibling, doc);
				       /* force the msubsup element to be
					  reformatted and to take into account
					  its new next sibling */
				       TtaRemoveTree (parent, doc);
				       TtaInsertSibling (parent, sibling, TRUE,
							 doc);
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
  Element	elem, base, next, group, pair, script, prevPair, prevScript;
  ElementType	elType, elTypeGroup, elTypePair, elTypeScript;
  SSchema       MathMLSSchema;
  base = NULL;
  group = NULL;
  prevPair = NULL;
  prevScript = NULL;

  MathMLSSchema = GetMathMLSSchema (doc);
  elTypeGroup.ElSSchema = MathMLSSchema;
  elTypePair.ElSSchema = MathMLSSchema;
  elTypeScript.ElSSchema = MathMLSSchema;

  /* process all children of the MMULTISCRIPT element */
  elem = TtaGetFirstChild (elMMULTISCRIPT);
  while (elem != NULL)
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
	  SetIntPlaceholderAttr (elem, doc);
	}

      CreatePlaceholders (elem, doc);

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
		 in PrescriptPair elements, wich will be children of this
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
		     ChangeElementType (cell, MathML_EL_LabelCell);
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
         CreateWrapper (cell, MathML_EL_CellWrapper, doc);
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
  AttributeType	attrType;
  Attribute	attr, IntAttr;
  Element       textEl;
  int		len;
  char         *value;
  ThotBool      italic;

  if (el != NULL)
     {
     /* search the fontstyle attribute */
     elType = TtaGetElementType (el);
     attrType.AttrSSchema = elType.ElSSchema;
     attrType.AttrTypeNum = MathML_ATTR_fontstyle;
     attr = TtaGetAttribute (el, attrType);
     attrType.AttrTypeNum = MathML_ATTR_IntFontstyle;
     IntAttr = TtaGetAttribute (el, attrType);
     if (attr != NULL)
	/* there is a fontstyle attribute. Remove the corresponding
	   internal attribute that is not needed */
	{
	if (IntAttr != NULL)
	  TtaRemoveAttribute (el, IntAttr, doc);
	}
     else
	/* there is no fontstyle attribute. Create an internal attribute
	   IntFontstyle with a value that depends on the content of the MI */
	{
        /* get content length */
        len = TtaGetElementVolume (el);
        if (len > 1)
           /* put an attribute IntFontstyle = IntNormal */
	   {
	   if (IntAttr == NULL)
	      {
	      IntAttr = TtaNewAttribute (attrType);
	      TtaAttachAttribute (el, IntAttr, doc);
	      }
	   TtaSetAttributeValue (IntAttr, MathML_ATTR_IntFontstyle_VAL_IntNormal,
				 el, doc);
	   }
        else
	   /* MI contains a single character. Remove attribute IntFontstyle
	      if it exists, except if it's ImaginaryI, ExponentialE or
	      DifferentialD */
	   {
	   italic = TRUE;
	   textEl = TtaGetFirstChild (el);
	   if (textEl != NULL)
	     {
	     /* is there an attribute EntityName on that character? */
	     attrType.AttrTypeNum = MathML_ATTR_EntityName;
	     attr = TtaGetAttribute (textEl, attrType);
	     if (attr)
	       {
	       len = TtaGetTextAttributeLength (attr);
	       if (len > 0)
		  {
		  value = TtaGetMemory (len+1);
		  TtaGiveTextAttributeValue (attr, value, &len);
		  if (strcmp (&value[1], "ImaginaryI;") == 0 ||
		      strcmp (&value[1], "ExponentialE;") == 0 ||
		      strcmp (&value[1], "DifferentialD;") == 0)
		    italic = FALSE;
		  TtaFreeMemory (value);
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
		 TtaSetAttributeValue (IntAttr, MathML_ATTR_IntFontstyle_VAL_IntNormal,
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
  int		len, val, form;
#define BUFLEN 10
  unsigned char    	text[BUFLEN];
  Language	lang;
  char		alphabet;

  /* get the content of the mo element */
  textEl = TtaGetFirstChild (el);
  if (textEl != NULL)
     /* the mo element is not empty */
     {
     /* does the mo element have an IntAddSpace attribute? */
     elType = TtaGetElementType (el);
     attrType.AttrSSchema = elType.ElSSchema;
     attrType.AttrTypeNum = MathML_ATTR_IntAddSpace;
     attr = TtaGetAttribute (el, attrType);
     if (attr == NULL)
        /* no IntAddSpace Attr, create one */
	{
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (el, attr, doc);
	}
     /* space on both sides by default */
     val = MathML_ATTR_IntAddSpace_VAL_both;
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
	   val = MathML_ATTR_IntAddSpace_VAL_both;
	   break;
	 case MathML_ATTR_form_VAL_postfix:
	   val = MathML_ATTR_IntAddSpace_VAL_spaceafter;
	   break;
	 default:
	   val = MathML_ATTR_IntAddSpace_VAL_both;
	   break;
	 } 
       }
     else
       /* no form attribute. Analyze the content */
       {
       len = TtaGetTextLength (textEl);
       if (len > 0 && len < BUFLEN)
	  {
	    len = BUFLEN;
	    TtaGiveTextContent (textEl, text, &len, &lang);
	    alphabet = TtaGetAlphabet (lang);
	    if (len == 1)
	      {
	       /* the mo element contains a single character */
	       if (alphabet == 'L')
	          /* ISO-Latin 1 character */
	          {
		  if (text[0] == '-')
		     /* prefix or infix operator? */
		     {
		     previous = el;
		     TtaPreviousSibling (&previous);
		     if (previous == NULL)
		        /* no previous sibling => prefix operator */
		        val = MathML_ATTR_IntAddSpace_VAL_nospace;
		     else
		        {
			elType = TtaGetElementType (previous);
			if (elType.ElTypeNum == MathML_EL_MO)
			   /* after an operator => prefix operator */
		           val = MathML_ATTR_IntAddSpace_VAL_nospace;
			else
			   /* infix operator */
		           val = MathML_ATTR_IntAddSpace_VAL_both;
			}
		     }
		  else if (text[0] == '&' ||
			   text[0] == '*' ||
			   text[0] == '+' ||
			   text[0] == '/' ||
			   text[0] == '<' ||
			   text[0] == '=' ||
			   text[0] == '>' ||
			   text[0] == '^' ||
			   (int)text[0] == 177 || /* plus or minus */
			   (int)text[0] == 215 || /* times */
			   (int)text[0] == 247)   /* divide */
		     /* infix operator */
		     val = MathML_ATTR_IntAddSpace_VAL_both;
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
		  else
		    /* default */
		    val = MathML_ATTR_IntAddSpace_VAL_both;
		  }
	       else if (alphabet == 'G')
		 /* Symbol character set */
		 if ((int)text[0] == 163 || /* less or equal */
		     (int)text[0] == 177 || /* plus or minus */
		     (int)text[0] == 179 || /* greater or equal */
		     (int)text[0] == 180 || /* times */
		     (int)text[0] == 184 || /* divide */
		     (int)text[0] == 185 || /* not equal */
		     (int)text[0] == 186 || /* identical */
		     (int)text[0] == 187 || /* equivalent */
		     (int)text[0] == 196 || /* circle times */
		     (int)text[0] == 197 || /* circle plus */
		     ((int)text[0] >= 199 && (int)text[0] <= 209) || /*  */
		     (int)text[0] == 217 || /* and */
		     (int)text[0] == 218 )  /* or */
		    /* infix operator */
		    val = MathML_ATTR_IntAddSpace_VAL_both;
	         else
		    val = MathML_ATTR_IntAddSpace_VAL_nospace;
	      }
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
		elType.ElTypeNum == MathML_EL_CellWrapper ||
		elType.ElTypeNum == MathML_EL_MathML ||
                elType.ElTypeNum == MathML_EL_FencedExpression);
      }
   return result;   
}

/*----------------------------------------------------------------------
   CheckFence
   If el is a MO element,
    - if it's a large operator (&Sum; for instance), put a presentation
      rule to enlarge the character.
    - if it's a child of a MROW (or equivalent) element and if it contains
      a single fence character, transform the MO into a MF and the fence
      character into a Thot stretchable symbol.
  ----------------------------------------------------------------------*/
void      CheckFence (Element el, Document doc)
{
   ElementType	       elType;
   Element	       content;
   AttributeType       attrType;
   Attribute	       attr, attrStretchy;
   Language	       lang;
   PresentationValue   pval;
   PresentationContext ctxt;
   CHAR_T              text[2];
   char	               alphabet;
   unsigned char       c;
   int                 len, val;

   elType = TtaGetElementType (el);
   if (elType.ElTypeNum == MathML_EL_MO)
     /* the element is a MO */
     {
     content = TtaGetFirstChild (el);
     if (content != NULL)
       {
       elType = TtaGetElementType (content);
       if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
	 {
	 len = TtaGetVolume (content);
	 if (len == 1)
	   /* the MO element contains a single character */
	   {
	   len = 2;
	   TtaGiveBufferContent (content, text, len, &lang);
	   alphabet = TtaGetAlphabet (lang);
	   if ((alphabet == 'G') &&
	       (text[0] == 229 || text[0] == 213))  /* Sigma,  Pi */
	     /* it's a large operator */
	     {
	     ctxt = TtaGetSpecificStyleContext (doc);
	     ctxt->destroy = FALSE;
	     /* the specific presentation to be created is not a CSS rule */
	     ctxt->cssLevel = 0;
	     pval.typed_data.unit = STYLE_UNIT_PERCENT;
	     pval.typed_data.real = FALSE;
	     pval.typed_data.value = 180;
	     TtaSetStylePresentation (PRSize, content, NULL, ctxt, pval);
	     }
	   else if (ChildOfMRowOrInferred (el))
	     /* the MO element is a child of a MROW element */
	      {
	      if (((alphabet == 'L') &&
		   (text[0] == '(' || text[0] == ')' ||
		    text[0] == '[' || text[0] == ']' ||
		    text[0] == '{' || text[0] == '}' ||
		    text[0] == '|'))  ||
		  ((alphabet == 'G') &&
		   (text[0] == 225 || text[0] == 241)))
		/* it's a stretchable parenthesis or equivalent */
		{
		/* remove the content of the MO element */
		TtaDeleteTree (content, doc);
		/* change the MO element into a MF element */
		ChangeTypeOfElement (el, doc, MathML_EL_MF);
		    
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
		  /* attach a IntVertStretch attribute to the MF element*/
		  attrType.AttrTypeNum = MathML_ATTR_IntVertStretch;
		  attr = TtaNewAttribute (attrType);
		  TtaAttachAttribute (el, attr, doc);
		  TtaSetAttributeValue (attr,
					MathML_ATTR_IntVertStretch_VAL_yes_,
					el, doc);
		  }
		/* create a new content for the MF element */
		elType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
		if (alphabet == 'G' && text[0] == 241)
		  c = '>';    /* RightAngleBracket */
		else if (alphabet == 'G' && text[0] == 225)
		  c = '<';    /* LeftAngleBracket */
		else
		  c = (char) text[0];
		content = TtaNewElement (doc, elType);
		TtaInsertFirstChild (&content, el, doc);
		TtaSetGraphicsShape (content, c, doc);
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
   AttributeType attrType;
   Attribute     attr;
   int		 length, sep, i;
   Language	 lang;
   char	 text[32], sepValue[4];

   /* get the separators attribute */
   mfenced = TtaGetParent (fencedExpression);
   elType = TtaGetElementType (fencedExpression);
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = MathML_ATTR_separators;
   text[0] = ',';	/* default value is  sparators=","  */
   text[1] = EOS;
   length = 1;
   attr = TtaGetAttribute (mfenced, attrType);
   if (attr != NULL)
      {
      length = 31;
      TtaGiveTextAttributeValue (attr, text, &length);
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
       if (elType.ElTypeNum != MathML_EL_Construct)
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
           sepValue[1] = SPACE;
           sepValue[2] = EOS;
	   lang = TtaGetLanguageIdFromAlphabet('L');
           TtaSetTextContent (leaf, sepValue, lang, doc);
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
  int           length;
  char          text[32];
  char          c;

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  if (open)
    {
      c = '(';    /* default value of attribute 'open' */
      attrType.AttrTypeNum = MathML_ATTR_open;
      elType.ElTypeNum = MathML_EL_OpeningFence;
    }
  else
    {
      c = ')';    /* default value of attribute 'close' */
      attrType.AttrTypeNum = MathML_ATTR_close;
      elType.ElTypeNum = MathML_EL_ClosingFence;
    }
  attr = TtaGetAttribute (el, attrType);
  if (attr != NULL)
    {
      length = 31;
      TtaGiveTextAttributeValue (attr, text, &length);
      if (length != 1)
	/* content of attribute open or close should be a single character */
	c = '?';
      else
	{
	  c = text[0];
	  /* filter characters that would represent strange symbols, such
	     as root, integrals, arrows, etc. */
	  if (c == 'r' || c == 'i' || c == 'c' || c == 'd' || c == 'S' ||
	      c == 'P' || c == 'I' || c == 'U' || c == 'o' || c == 'u' ||
	      c == 'h' || c == 'v' || c == 'R' || c == '^' || c == 'L' ||
	      c == 'V' || c == 'D')
	    c = '?';
	}
    }
  fence = TtaNewElement (doc, elType);
  TtaInsertSibling (fence, fencedExpression, open, doc);
  elType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
  leaf = TtaNewElement (doc, elType);
  TtaInsertFirstChild (&leaf, fence, doc);
  TtaSetGraphicsShape (leaf, c, doc);
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
       value = TtaSkipBlanks (value);
       value = ParseCSSUnit (value, &pval);
       if (pval.typed_data.unit != STYLE_UNIT_INVALID)
	  {
	  /* the specific presentation to be created is not a CSS rule */
	  ctxt->cssLevel = 0;
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
	 value = TtaGetMemory (length+1);
	 value[0] = EOS;
	 TtaGiveTextAttributeValue (attr, value, &length);
	 MathMLScriptShift (doc, el, value, att);
	 TtaFreeMemory (value);
	 }
      }
}

/*----------------------------------------------------------------------
   MathMLElementComplete
   Check the Thot structure of the MathML element el.
  ----------------------------------------------------------------------*/
void      MathMLElementComplete (Element el, Document doc, int *error)
{
   ElementType		elType, parentType;
   Element		child, parent, new, prev, next;
   AttributeType        attrType;
   Attribute            attr;
   SSchema              MathMLSSchema;
   ThotBool             ok;

   ok = TRUE;
   *error = 0;
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
       case MathML_EL_MI:
	  SetFontstyleAttr (el, doc);
	  break;
       case MathML_EL_MO:
	  SetIntAddSpaceAttr (el, doc);
	  SetIntVertStretchAttr (el, doc, 0, NULL);
	  /* if the MO element is a child of a MROW (or equivalent) and if it
	     contains a fence character, transform this MO into MF and
	     transform the fence character into a Thot SYMBOL */
	  CheckFence (el, doc);
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
	  break;
       case MathML_EL_MOVER:
	  /* end of a MOVER. Create UnderOverBase, and Overscript */
	  ok = CheckMathSubExpressions (el, MathML_EL_UnderOverBase,
					MathML_EL_Overscript, 0, doc);
	  SetIntHorizStretchAttr (el, doc);
	  SetIntVertStretchAttr (el, doc, MathML_EL_UnderOverBase, NULL);
	  break;
       case MathML_EL_MUNDEROVER:
	  /* end of a MUNDEROVER. Create UnderOverBase, Underscript, and
	     Overscript */
	  ok = CheckMathSubExpressions (el, MathML_EL_UnderOverBase,
					MathML_EL_Underscript,
					MathML_EL_Overscript, doc);
	  SetIntHorizStretchAttr (el, doc);
	  SetIntVertStretchAttr (el, doc, MathML_EL_UnderOverBase, NULL);
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
	  /* Create placeholders within the MACTION element */
          CreatePlaceholders (TtaGetFirstChild (el), doc);
	  break;
       default:
	  break;
       }
     parent = TtaGetParent (el);
     if (parent)
       {
	 parentType = TtaGetElementType (parent);
	 if (parentType.ElSSchema != elType.ElSSchema)
	   /* root of a MathML tree, Create a MathML element if there is no */
	   if (elType.ElTypeNum != MathML_EL_MathML)
	     {
	       elType.ElSSchema = MathMLSSchema;
	       elType.ElTypeNum = MathML_EL_MathML;
	       new = TtaNewElement (doc, elType);
	       TtaInsertSibling (new, el, TRUE, doc);
	       next = el;
	       TtaNextSibling (&next);
	       TtaRemoveTree (el, doc);
	       TtaInsertFirstChild (&el, new, doc);
	       prev = el;
	       while (next != NULL)
		 {
		   child = next;
		   TtaNextSibling (&next);
		   TtaRemoveTree (child, doc);
		   TtaInsertSibling (child, prev, FALSE, doc);
		   prev = child;
		 }
	       /* Create placeholders within the MathML element */
	       CreatePlaceholders (el, doc);
	     }
       }
     }
   if (!ok)
     /* send an error message */
     *error = 1;
}

/*----------------------------------------------------------------------
   UnknownMathMLNameSpace
   Create an element that belongs to a non-supported namespace
  ----------------------------------------------------------------------*/
void               UnknownMathMLNameSpace (ParserData *context, char* content)
{
   ElementType     elType;
   Element         elInv, elText;

   /* Create a new Invalid_element */
   elType.ElSSchema = GetXMLSSchema (MATH_TYPE, context->doc);
   elType.ElTypeNum = MathML_EL_Unknown_namespace;
   elInv = TtaNewElement (context->doc, elType);
   if (elInv != NULL)
     {
       XmlSetElemLineNumber (elInv);
       InsertXmlElement (&elInv);
       context->lastElementClosed = TRUE;
       elType.ElTypeNum = MathML_EL_TEXT_UNIT;
       elText = TtaNewElement (context->doc, elType);
       XmlSetElemLineNumber (elText);
       TtaInsertFirstChild (&elText, elInv, context->doc);
       TtaSetTextContent (elText, content, context->language, context->doc);
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
  ParseHTMLSpecificStyle (el, css_command, doc, 0, FALSE);
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
  ParseHTMLSpecificStyle (el, css_command, doc, 0, FALSE);
}

/*----------------------------------------------------------------------
 MathMLAttrToStyleProperty
 The MathML attribute attr is associated with element el. Generate
 the corresponding style property for this element.
 -----------------------------------------------------------------------*/
void MathMLAttrToStyleProperty (Document doc, Element el, char *value, int attr)
{
  char           css_command[buflen+20];

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
       sprintf (css_command, "padding-left: %s", value);
       break;
    case MathML_ATTR_rspace:
       sprintf (css_command, "padding-right: %s", value);
       break;
    }
  ParseHTMLSpecificStyle (el, css_command, doc, 0, FALSE);
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
     value = TtaSkipBlanks (value);
     relative = (value[0] == '-' || value[0] == '+');
     value = ParseCSSUnit (value, &pval);
     if (pval.typed_data.unit != STYLE_UNIT_REL &&
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
	 pval.typed_data.value = percentage;
	 pval.typed_data.unit = STYLE_UNIT_PERCENT;
	 /* the specific presentation to be created is not a CSS rule */
	 ctxt->cssLevel = 0;
	 TtaSetStylePresentation (PRSize, el, NULL, ctxt, pval);       
	 }
       else
	 /* absolute value */
	 {
	   /****  ****/;
	 }
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
void MathMLSpacingAttr (Document doc, Element el, char *value, int attr)
{
  ElementType         elType;
  PresentationValue   pval;
  PresentationContext ctxt;
  int                 ruleType;

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
      value = TtaSkipBlanks (value);
      value = ParseCSSUnit (value, &pval);
      /***** we should accept namedspace for width *****/
      if (pval.typed_data.unit != STYLE_UNIT_INVALID)
	{
	  /* the specific presentation to be created is not a CSS rule */
	  ctxt->cssLevel = 0;
	  TtaSetStylePresentation (ruleType, el, NULL, ctxt, pval);
	}
    }
  TtaFreeMemory (ctxt);
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
#define buflen 50
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
	    ChangeTypeOfElement (el, doc, MathML_EL_BevelledMFRAC);
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

   else if (attrType.AttrTypeNum == MathML_ATTR_rowalign)
     {
       /* parse the attribute value and create a IntRowAlign attribute
	  for each mrow contained in the element */
       HandleRowalignAttribute (attr, el, doc, FALSE);
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

   /* don't handle attribute columnalign now: the table or the row is not
      complete yet. Handle it when the element is complete.
   else if (attrType.AttrTypeNum == MathML_ATTR_columnalign)
   */

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
	    attrType.AttrTypeNum == MathML_ATTR_depth_ )
     {
      length = TtaGetTextAttributeLength (attr);
      if (length >= buflen)
         length = buflen - 1;
      if (length > 0)
	 {
	   value = TtaGetMemory (buflen);
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
                  HTMLSetForegroundColor (doc, el, value);
	       break;
	     case MathML_ATTR_mathcolor:
               HTMLSetForegroundColor (doc, el, value);
	       break;
	     case MathML_ATTR_background_:
	       /* deprecated attribute */
	       /* if the same element has a mathbackground attribute, ignore
		  the background attribute */
	       depAttrType.AttrSSchema = attrType.AttrSSchema;
	       depAttrType.AttrTypeNum = MathML_ATTR_mathbackground;
	       if (!TtaGetAttribute (el, depAttrType))
                  HTMLSetBackgroundColor (doc, el, value);
	       break;
	     case MathML_ATTR_mathbackground:
               HTMLSetBackgroundColor (doc, el, value);
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
