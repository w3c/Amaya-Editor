/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
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
#include "undo.h"
#include "MathML.h"
#include "parser.h"
#include "html2thot_f.h"

typedef CHAR_T  MathEntityName[30];
typedef struct _MathEntity
  {			 /* a Math entity representing an operator char */
     MathEntityName      MentityName;	/* entity name */
     int                 charCode;	/* decimal code of char */
     CHAR_T		 alphabet;	/* 'L' = ISO-Latin-1, 'G' = Symbol */
  }
MathEntity;

static MathEntity        MathEntityTable[] =
{
   /* This table MUST be in alphabetical order */
   /* This table contains characters from the Symbol font plus some
      specific MathML entities */
   {TEXT("Agr"), 65, 'G'},
   {TEXT("And"), 217, 'G'},
   {TEXT("ApplyFunction"), 32, 'L'},  /* render as white space */
   {TEXT("Backslash"), 92, 'L'},
   {TEXT("Bgr"), 66, 'G'},
   {TEXT("Cap"), 199, 'G'},
   {TEXT("CenterDot"), 215, 'G'},
   {TEXT("CirclePlus"), 197, 'G'},
   {TEXT("CircleTimes"), 196, 'G'},
   {TEXT("Colon"), 58, 'G'},
   {TEXT("Congruent"), 64, 'G'},
   {TEXT("Cup"), 200, 'G'},
   {TEXT("Del"), 209, 'G'},
   {TEXT("Delta"), 68, 'G'},
   {TEXT("Diamond"), 168, 'G'},
   {TEXT("DifferentialD"), 100, 'L'},
   {TEXT("DoubleDownArrow"), 223, 'G'},
   {TEXT("DoubleLeftArrow"), 220, 'G'},
   {TEXT("DoubleLeftRightArrow"), 219, 'G'},
   {TEXT("DoubleRightArrow"), 222, 'G'},
   {TEXT("DoubleUpArrow"), 221, 'G'},
   {TEXT("DownArrow"), 175, 'G'},
   {TEXT("DownTee"), 94, 'G'},
   {TEXT("EEgr"), 72, 'G'},
   {TEXT("Egr"), 69, 'G'},
   {TEXT("Element"), 206, 'G'},
   {TEXT("Equal"), 61, 'L'},
   {TEXT("EqualTilde"), 64, 'G'},
   {TEXT("Exists"), 36, 'G'},
   {TEXT("ExponentialE"), 101, 'L'},
   {TEXT("ForAll"), 34, 'G'},
   {TEXT("Gamma"), 71, 'G'},
   {TEXT("GreaterEqual"), 179, 'G'},
   {TEXT("Hat"), 94, 'L'},
   {TEXT("Igr"), 73, 'G'},
   {TEXT("ImaginaryI"), 105, 'L'},
   {TEXT("Integral"), 242, 'G'},
   {TEXT("Intersection"), 199, 'G'},
   {TEXT("InvisibleComma"), 129, 'L'},
   {TEXT("InvisibleTimes"), 0, SPACE},
   {TEXT("Kgr"), 75, 'G'},
   {TEXT("KHgr"), 67, 'G'},
   {TEXT("Lambda"), 76, 'G'},
   {TEXT("LeftArrow"), 172, 'G'},
   {TEXT("LeftCeiling"), 233, 'G'},
   {TEXT("LeftFloor"), 235, 'G'},
   {TEXT("LeftRightArrow"), 171, 'G'},
   {TEXT("Mgr"), 77, 'G'},
   {TEXT("Ngr"), 78, 'G'},
   {TEXT("NonBreakingSpace"), 160, 'L'},
   {TEXT("Not"), 216, 'G'},
   {TEXT("NotElement"), 207, 'G'},
   {TEXT("NotEqual"), 185, 'G'},
   {TEXT("NotSubset"), 203, 'G'},
   {TEXT("Ogr"), 79, 'G'},
   {TEXT("Omega"), 87, 'G'},
   {TEXT("Or"), 218, 'G'},
   {TEXT("OverBar"), 95, 'L'},
   {TEXT("PI"), 213, 'G'},
   {TEXT("PartialD"), 182, 'G'},
   {TEXT("Phi"), 70, 'G'},
   {TEXT("Pi"), 80, 'G'},
   {TEXT("PlusMinus"), 177, 'G'},
   {TEXT("Product"), 213, 'G'},
   {TEXT("Proportional"), 181, 'G'},
   {TEXT("Psi"), 89, 'G'},
   {TEXT("RightAngleBracket"), 241, 'G'},
   {TEXT("RightCeiling"), 249, 'G'},
   {TEXT("RightFloor"), 251, 'G'},
   {TEXT("Rgr"), 82, 'G'},
   {TEXT("RightArrow"), 174, 'G'},
   {TEXT("Sigma"), 83, 'G'},
   {TEXT("Sol"), 164, 'G'},
   {TEXT("Star"), 42, 'L'},
   {TEXT("Subset"), 204, 'G'},
   {TEXT("SubsetEqual"), 205, 'G'},
   {TEXT("SuchThat"), 39, 'G'},
   {TEXT("Sum"), 229, 'G'},
   {TEXT("Superset"), 201, 'G'},
   {TEXT("SupersetEqual"), 202, 'G'},
   {TEXT("Tgr"), 84, 'G'},
   {TEXT("Therefore"), 92, 'G'},
   {TEXT("Theta"), 81, 'G'},
   {TEXT("ThickSpace"), 130, 'L'},
   {TEXT("ThinSpace"), 129, 'L'},
   {TEXT("Tilde"), 126, 'L'},
   {TEXT("TripleDot"), 188, 'G'},
   {TEXT("UnderBar"), 45, 'L'},
   {TEXT("Union"), 200, 'G'},
   {TEXT("UpArrow"), 173, 'G'},
   {TEXT("Upsi"), 85, 'G'},
   {TEXT("Upsi1"), 161, 'G'},
   {TEXT("Vee"), 218, 'G'},
   {TEXT("Verbar"), 189, 'G'},
   {TEXT("VerticalBar"), 124, 'L'},
   {TEXT("Xi"), 88, 'G'},
   {TEXT("Zgr"), 90, 'G'},
   {TEXT("af"), 32, 'L'},             /* render as white space */
   {TEXT("aleph"), 192, 'G'},
   {TEXT("alpha"), 97, 'G'},
   {TEXT("and"), 217, 'G'},
   {TEXT("angle"), 208, 'G'},
   {TEXT("ap"), 187, 'G'},
   {TEXT("beta"), 98, 'G'},
   {TEXT("bot"), 94, 'G'},
   {TEXT("bottom"), 94, 'G'},
   {TEXT("bull"), 183, 'G'},
   {TEXT("cap"), 199, 'G'},
   {TEXT("chi"), 99, 'G'},
   {TEXT("circ"), 94, 'L'},
   {TEXT("clubs"), 167, 'G'},
   {TEXT("cong"), 64, 'G'},
   {TEXT("copysf"), 211, 'G'},
   {TEXT("copyssf"), 227, 'G'},
   {TEXT("cr"), 191, 'G'},
   {TEXT("cup"), 200, 'G'},
   {TEXT("dArr"), 223, 'G'},
   {TEXT("darr"), 175, 'G'},
   {TEXT("dd"), 100, 'L'},
   {TEXT("deg"), 176, 'G'},
   {TEXT("delta"), 100, 'G'},
   {TEXT("diams"), 168, 'G'},
   {TEXT("divide"), 184, 'G'},
   {TEXT("dtri"), 209, 'G'},
   {TEXT("ee"), 101, 'L'},
   {TEXT("empty"), 198, 'G'},
   {TEXT("emptyset"), 198, 'G'},
   {TEXT("emsp"), 32, 'G'},
   {TEXT("epsiv"), 101, 'G'},
   {TEXT("equiv"), 186, 'G'},
   {TEXT("eta"), 104, 'G'},
   {TEXT("exist"), 36, 'G'},
   {TEXT("florin"), 166, 'G'},
   {TEXT("forall"), 34, 'G'},
   {TEXT("gamma"), 103, 'G'},
   {TEXT("ge"), 179, 'G'},
   {TEXT("geq"), 179, 'G'},
   {TEXT("gt"), 62, 'L'},
   {TEXT("hearts"), 169, 'G'},
   {TEXT("horbar"), 190, 'G'},
   {TEXT("iff"), 219, 'G'},
   {TEXT("ifraktur"), 193, 'G'},
   {TEXT("in"), 206, 'G'},
   {TEXT("infin"), 165, 'G'},
   {TEXT("int"), 242, 'G'},
   {TEXT("iota"), 105, 'G'},
   {TEXT("isin"), 206, 'G'},
   {TEXT("it"), 0, SPACE},
   {TEXT("kappa"), 107, 'G'},
   {TEXT("lArr"), 220, 'G'},
   {TEXT("lambda"), 108, 'G'},
   {TEXT("lang"), 225, 'G'},
   {TEXT("langle"), 225, 'G'},
   {TEXT("larr"), 172, 'G'},
   {TEXT("lbrace"), 123, 'L'},
   {TEXT("lceil"), 233, 'G'},
   {TEXT("lcub"), 123, 'L'},
   {TEXT("le"), 163, 'G'},
   {TEXT("leq"), 163, 'G'},
   {TEXT("lfloor"), 235, 'G'},
   {TEXT("lowbar"), 95, 'G'},
   {TEXT("loz"), 224, 'G'},
   {TEXT("lrArr"), 219, 'G'},
   {TEXT("lrarr"), 171, 'G'},
   {TEXT("lsqb"), 91, 'G'},
   {TEXT("lt"), 60, 'L'},
   {TEXT("middot"), 215, 'G'},
   {TEXT("mldr"), 188, 'G'},
   {TEXT("mu"), 109, 'G'},
   {TEXT("nabla"), 209, 'G'},
   {TEXT("ne"), 185, 'G'},
   {TEXT("neq"), 185, 'G'},
   {TEXT("not"), 216, 'G'},
   {TEXT("notin"), 207, 'G'},
   {TEXT("nu"), 110, 'G'},
   {TEXT("ogr"), 111, 'G'},
   {TEXT("omega"), 119, 'G'},
   {TEXT("oplus"), 197, 'G'},
   {TEXT("or"), 218, 'G'},
   {TEXT("otimes"), 196, 'G'},
   {TEXT("part"), 182, 'G'},
   {TEXT("perp"), 94, 'G'},
   {TEXT("phi"), 102, 'G'},
   {TEXT("phiv"), 106, 'G'},
   {TEXT("pi"), 112, 'G'},
   {TEXT("piv"), 118, 'G'},
   {TEXT("prop"), 181, 'G'},
   {TEXT("psi"), 121, 'G'},
   {TEXT("rArr"), 222, 'G'},
   {TEXT("radic"), 214, 'G'},
   {TEXT("rang"), 241, 'G'},
   {TEXT("rangle"), 241, 'G'},
   {TEXT("rarr"), 174, 'G'},
   {TEXT("rbrace"), 125, 'L'},
   {TEXT("rceil"), 249, 'G'},
   {TEXT("rcub"), 125, 'L'},
   {TEXT("rdquo"), 178, 'G'},
   {TEXT("regsf"), 210, 'G'},
   {TEXT("regssf"), 226, 'G'},
   {TEXT("rfloor"), 251, 'G'},
   {TEXT("rfraktur"), 194, 'G'},
   {TEXT("rho"), 114, 'G'},
   {TEXT("rsqb"), 93, 'G'},
   {TEXT("sigma"), 115, 'G'},
   {TEXT("sigmav"), 86, 'G'},
   {TEXT("spades"), 170, 'G'},
   {TEXT("sub"), 204, 'G'},
   {TEXT("sube"), 205, 'G'},
   {TEXT("subne"), 203, 'G'},
   {TEXT("subset"), 204, 'G'},
   {TEXT("sum"), 229, 'G'},
   {TEXT("sup"), 201, 'G'},
   {TEXT("supe"), 202, 'G'},
   {TEXT("supset"), 201, 'G'},
   {TEXT("supseteq"), 202, 'G'},
   {TEXT("supseteqq"), 202, 'G'},
   {TEXT("tau"), 116, 'G'},
   {TEXT("there4"), 92, 'G'},
   {TEXT("therefore"), 92, 'G'},
   {TEXT("theta"), 113, 'G'},
   {TEXT("thetav"), 74, 'G'},
   {TEXT("thickspace"), 32, 'L'},
   {TEXT("times"), 180, 'G'},
   {TEXT("trade"), 212, 'G'},
   {TEXT("tradesf"), 212, 'G'},
   {TEXT("tradessf"), 228, 'G'},
   {TEXT("uArr"), 221, 'G'},
   {TEXT("uarr"), 173, 'G'},
   {TEXT("upsi"), 117, 'G'},
   {TEXT("vee"), 218, 'G'},
   {TEXT("weierp"), 195, 'G'},
   {TEXT("xi"), 120, 'G'},
   {TEXT("zeta"), 122, 'G'},
   {TEXT("zzzz"), -1, SPACE}		/* this last entry is required */

};

static AttributeMapping MathMLAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   /* The rest of this table MUST be in alphabetical order */
   {TEXT("unknown_attr"), TEXT(""), 'A', MathML_ATTR_Invalid_attribute},
   {TEXT("accent"), TEXT(""), 'A', MathML_ATTR_accent},
   {TEXT("accentunder"), TEXT(""), 'A', MathML_ATTR_accentunder},
   {TEXT("actiontype"), TEXT(""), 'A', MathML_ATTR_actiontype},
   {TEXT("align"), TEXT(""), 'A', MathML_ATTR_align},
   {TEXT("alignmentscope"), TEXT(""), 'A', MathML_ATTR_alignmentscope},
   {TEXT("alt"), TEXT(""), 'A', MathML_ATTR_alt},
   {TEXT("background"), TEXT(""), 'A', MathML_ATTR_background_},
   {TEXT("bevelled"), TEXT(""), 'A', MathML_ATTR_bevelled},
   {TEXT("class"), TEXT(""), 'A', MathML_ATTR_class},
   {TEXT("close"), TEXT(""), 'A', MathML_ATTR_close},
   {TEXT("columnalign"), TEXT(""), 'A', MathML_ATTR_columnalign},
   {TEXT("columnlines"), TEXT(""), 'A', MathML_ATTR_columnlines},
   {TEXT("columnspacing"), TEXT(""), 'A', MathML_ATTR_columnspacing},
   {TEXT("columnspan"), TEXT(""), 'A', MathML_ATTR_columnspan},
   {TEXT("columnwidth"), TEXT(""), 'A', MathML_ATTR_columnwidth},
   {TEXT("color"), TEXT(""), 'A', MathML_ATTR_color},
   {TEXT("denomalign"), TEXT(""), 'A', MathML_ATTR_denomalign},
   {TEXT("depth"), TEXT(""), 'A', MathML_ATTR_depth_},
   {TEXT("displaystyle"), TEXT(""), 'A', MathML_ATTR_displaystyle},
   {TEXT("edge"), TEXT(""), 'A', MathML_ATTR_edge},
   {TEXT("equalcolumns"), TEXT(""), 'A', MathML_ATTR_equalcolumns},
   {TEXT("equalrows"), TEXT(""), 'A', MathML_ATTR_equalrows},
   {TEXT("fence"), TEXT(""), 'A', MathML_ATTR_fence},
   {TEXT("fontfamily"), TEXT(""), 'A', MathML_ATTR_fontfamily},
   {TEXT("fontstyle"), TEXT(""), 'A', MathML_ATTR_fontstyle},
   {TEXT("fontsize"), TEXT(""), 'A', MathML_ATTR_fontsize},
   {TEXT("fontweight"), TEXT(""), 'A', MathML_ATTR_fontweight},
   {TEXT("form"), TEXT(""), 'A', MathML_ATTR_form},
   {TEXT("frame"), TEXT(""), 'A', MathML_ATTR_frame},
   {TEXT("framespacing"), TEXT(""), 'A', MathML_ATTR_framespacing},
   {TEXT("groupalign"), TEXT(""), 'A', MathML_ATTR_groupalign},
   {TEXT("height"), TEXT(""), 'A', MathML_ATTR_height_},
   {TEXT("id"), TEXT(""), 'A', MathML_ATTR_id},
   {TEXT("index"), TEXT(""), 'A', MathML_ATTR_index},
   {TEXT("largeop"), TEXT(""), 'A', MathML_ATTR_largeop},
   {TEXT("linethickness"), TEXT(""), 'A', MathML_ATTR_linethickness},
   {TEXT("link"), TEXT(""), 'A', MathML_ATTR_link},
   {TEXT("lquote"), TEXT(""), 'A', MathML_ATTR_lquote},
   {TEXT("lspace"), TEXT(""), 'A', MathML_ATTR_lspace},
   {TEXT("maxsize"), TEXT(""), 'A', MathML_ATTR_maxsize},
   {TEXT("minlabelspacing"), TEXT(""), 'A', MathML_ATTR_minlabelspacing},
   {TEXT("minsize"), TEXT(""), 'A', MathML_ATTR_minsize},
   {TEXT("movablelimits"), TEXT(""), 'A', MathML_ATTR_movablelimits},
   {TEXT("name"), TEXT(""), 'A', MathML_ATTR_name},
   {TEXT("notation"), TEXT(""), 'A', MathML_ATTR_notation},
   {TEXT("numalign"), TEXT(""), 'A', MathML_ATTR_numalign},
   {TEXT("open"), TEXT(""), 'A', MathML_ATTR_open},
   {TEXT("other"), TEXT(""), 'A', MathML_ATTR_other},
   {TEXT("rowalign"), TEXT(""), 'A', MathML_ATTR_rowalign},
   {TEXT("rowlines"), TEXT(""), 'A', MathML_ATTR_rowlines},
   {TEXT("rowspacing"), TEXT(""), 'A', MathML_ATTR_rowspacing},
   {TEXT("rowspan"), TEXT(""), 'A', MathML_ATTR_rowspan_},
   {TEXT("rquote"), TEXT(""), 'A', MathML_ATTR_rquote},
   {TEXT("rspace"), TEXT(""), 'A', MathML_ATTR_rspace},
   {TEXT("scriptlevel"), TEXT(""), 'A', MathML_ATTR_scriptlevel},
   {TEXT("scriptminsize"), TEXT(""), 'A', MathML_ATTR_scriptminsize},
   {TEXT("scriptsizemultiplier"), TEXT(""), 'A', MathML_ATTR_scriptsizemultiplier},
   {TEXT("selection"), TEXT(""), 'A', MathML_ATTR_selection},
   {TEXT("separator"), TEXT(""), 'A', MathML_ATTR_separator},
   {TEXT("separators"), TEXT(""), 'A', MathML_ATTR_separators},
   {TEXT("side"), TEXT(""), 'A', MathML_ATTR_side},
   {TEXT("stretchy"), TEXT(""), 'A', MathML_ATTR_stretchy},
   {TEXT("style"), TEXT(""), 'A', MathML_ATTR_style_},
   {TEXT("subscriptshift"), TEXT(""), 'A', MathML_ATTR_subscriptshift},
   {TEXT("superscriptshift"), TEXT(""), 'A', MathML_ATTR_superscriptshift},
   {TEXT("symmetric"), TEXT(""), 'A', MathML_ATTR_symmetric},
   {TEXT("width"), TEXT(""), 'A', MathML_ATTR_width_},
   {TEXT("xref"), TEXT(""), 'A', MathML_ATTR_xref},
   {TEXT("zzghost"), TEXT(""), 'A', MathML_ATTR_Ghost_restruct},

   {TEXT(""), TEXT(""), EOS, 0}		/* Last entry. Mandatory */
};

/* mapping table of attribute values */

static AttrValueMapping MathMLAttrValueMappingTable[] =
{
 {MathML_ATTR_accent, TEXT("true"), MathML_ATTR_accent_VAL_true},
 {MathML_ATTR_accent, TEXT("false"), MathML_ATTR_accent_VAL_false},

 {MathML_ATTR_accentunder, TEXT("true"), MathML_ATTR_accentunder_VAL_true},
 {MathML_ATTR_accentunder, TEXT("false"), MathML_ATTR_accentunder_VAL_false},

 {MathML_ATTR_bevelled, TEXT("true"), MathML_ATTR_bevelled_VAL_true},
 {MathML_ATTR_bevelled, TEXT("false"), MathML_ATTR_bevelled_VAL_false},

 {MathML_ATTR_denomalign, TEXT("center"), MathML_ATTR_denomalign_VAL_center_},
 {MathML_ATTR_denomalign, TEXT("left"), MathML_ATTR_denomalign_VAL_left_},
 {MathML_ATTR_denomalign, TEXT("right"), MathML_ATTR_denomalign_VAL_right_},

 {MathML_ATTR_displaystyle, TEXT("true"), MathML_ATTR_displaystyle_VAL_true},
 {MathML_ATTR_displaystyle, TEXT("false"), MathML_ATTR_displaystyle_VAL_false},

 {MathML_ATTR_edge, TEXT("left"), MathML_ATTR_edge_VAL_left_},
 {MathML_ATTR_edge, TEXT("right"), MathML_ATTR_edge_VAL_right_},

 {MathML_ATTR_fence, TEXT("true"), MathML_ATTR_fence_VAL_true},
 {MathML_ATTR_fence, TEXT("false"), MathML_ATTR_fence_VAL_false},

 {MathML_ATTR_fontstyle, TEXT("italic"), MathML_ATTR_fontstyle_VAL_italic},
 {MathML_ATTR_fontstyle, TEXT("normal"), MathML_ATTR_fontstyle_VAL_normal_},

 {MathML_ATTR_fontweight, TEXT("normal"), MathML_ATTR_fontweight_VAL_normal_},
 {MathML_ATTR_fontweight, TEXT("bold"), MathML_ATTR_fontweight_VAL_bold_},

 {MathML_ATTR_form, TEXT("prefix"), MathML_ATTR_form_VAL_prefix},
 {MathML_ATTR_form, TEXT("infix"), MathML_ATTR_form_VAL_infix},
 {MathML_ATTR_form, TEXT("postfix"), MathML_ATTR_form_VAL_postfix},

 {MathML_ATTR_frame, TEXT("none"), MathML_ATTR_frame_VAL_none_},
 {MathML_ATTR_frame, TEXT("solid"), MathML_ATTR_frame_VAL_solid_},
 {MathML_ATTR_frame, TEXT("dashed"), MathML_ATTR_frame_VAL_dashed_},

 {MathML_ATTR_largeop, TEXT("true"), MathML_ATTR_largeop_VAL_true},
 {MathML_ATTR_largeop, TEXT("false"), MathML_ATTR_largeop_VAL_false},

 {MathML_ATTR_link, TEXT("document"), MathML_ATTR_link_VAL_document},
 {MathML_ATTR_link, TEXT("extended"), MathML_ATTR_link_VAL_extended},
 {MathML_ATTR_link, TEXT("group"), MathML_ATTR_link_VAL_group},
 {MathML_ATTR_link, TEXT("locator"), MathML_ATTR_link_VAL_locator},
 {MathML_ATTR_link, TEXT("simple"), MathML_ATTR_link_VAL_simple},

 {MathML_ATTR_movablelimits, TEXT("true"), MathML_ATTR_movablelimits_VAL_true},
 {MathML_ATTR_movablelimits, TEXT("false"), MathML_ATTR_movablelimits_VAL_false},

 {MathML_ATTR_notation, TEXT("longdiv"), MathML_ATTR_notation_VAL_longdiv},
 {MathML_ATTR_notation, TEXT("actuarial"), MathML_ATTR_notation_VAL_actuarial},
 {MathML_ATTR_notation, TEXT("radical"), MathML_ATTR_notation_VAL_radical},

 {MathML_ATTR_numalign, TEXT("center"), MathML_ATTR_numalign_VAL_center_},
 {MathML_ATTR_numalign, TEXT("left"), MathML_ATTR_numalign_VAL_left_},
 {MathML_ATTR_numalign, TEXT("right"), MathML_ATTR_numalign_VAL_right_},

 {MathML_ATTR_separator, TEXT("true"), MathML_ATTR_separator_VAL_true},
 {MathML_ATTR_separator, TEXT("false"), MathML_ATTR_separator_VAL_false},

 {MathML_ATTR_side, TEXT("left"), MathML_ATTR_side_VAL_left_},
 {MathML_ATTR_side, TEXT("leftoverlap"), MathML_ATTR_side_VAL_leftoverlap},
 {MathML_ATTR_side, TEXT("right"), MathML_ATTR_side_VAL_right_},
 {MathML_ATTR_side, TEXT("rightoverlap"), MathML_ATTR_side_VAL_rightoverlap},

 {MathML_ATTR_stretchy, TEXT("true"), MathML_ATTR_stretchy_VAL_true},
 {MathML_ATTR_stretchy, TEXT("false"), MathML_ATTR_stretchy_VAL_false},

 {MathML_ATTR_symmetric, TEXT("true"), MathML_ATTR_symmetric_VAL_true},
 {MathML_ATTR_symmetric, TEXT("false"), MathML_ATTR_symmetric_VAL_false},

 {0, TEXT(""), 0}			/* Last entry. Mandatory */
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
#ifdef __STDC__
void          MapMathMLAttribute (CHAR_T* Attr, AttributeType *attrType, STRING elementName, Document doc)
#else
void          MapMathMLAttribute (Attr, attrType, elementName, doc)
STRING              Attr;
AttributeType      *attrType;
STRING		    elementName;
Document            doc;
#endif
{
   int                 i;

   attrType->AttrTypeNum = 0;
   attrType->AttrSSchema = NULL;
   i = 0;
   do
      if (ustrcasecmp (MathMLAttributeMappingTable[i].XMLattribute, Attr))
	 i++;
      else
	 if (MathMLAttributeMappingTable[i].XMLelement[0] == EOS)
	       {
	       attrType->AttrTypeNum = MathMLAttributeMappingTable[i].ThotAttribute;
	       attrType->AttrSSchema = GetMathMLSSchema (doc);
	       }
	 else if (!ustrcasecmp (MathMLAttributeMappingTable[i].XMLelement,
			       elementName))
	       {
	       attrType->AttrTypeNum = MathMLAttributeMappingTable[i].ThotAttribute;
	       attrType->AttrSSchema = GetMathMLSSchema (doc);
	       }
	 else
	       i++;
   while (attrType->AttrTypeNum <= 0 && MathMLAttributeMappingTable[i].AttrOrContent != EOS);
}

/*----------------------------------------------------------------------
   MapMathMLAttributeValue
   Search in the Attribute Value Mapping Table the entry for the attribute
   ThotAtt and its value AttrVal. Returns the corresponding Thot value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                MapMathMLAttributeValue (STRING AttrVal, AttributeType attrType, int *value)
#else
void                MapMathMLAttributeValue (AttrVal, attrType, value)
STRING              AttrVal;
AttributeType       attrType;
int		   *value;
#endif
{
   int                 i;

   *value = 0;
   i = 0;
   while (MathMLAttrValueMappingTable[i].ThotAttr != attrType.AttrTypeNum &&
	  MathMLAttrValueMappingTable[i].ThotAttr != 0)
       i++;
   if (MathMLAttrValueMappingTable[i].ThotAttr == attrType.AttrTypeNum)
       do
	   if (!ustrcasecmp (MathMLAttrValueMappingTable[i].XMLattrValue, AttrVal))
	       *value = MathMLAttrValueMappingTable[i].ThotAttrValue;
	   else
	       i++;
       while (*value <= 0 && MathMLAttrValueMappingTable[i].ThotAttr != 0);
}

/*----------------------------------------------------------------------
   MapMathMLEntity
   Search that entity in the entity table and return the corresponding value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void	MapMathMLEntity (STRING entityName, STRING entityValue, int valueLength, STRING alphabet)
#else
void	MapMathMLEntity (entityName, entityValue, valueLength, alphabet)
STRING entityName;
STRING entityValue;
int valueLength;
STRING alphabet;

#endif

{
   int	i;

   for (i = 0; MathEntityTable[i].charCode >= 0 &&
	       ustrcmp (MathEntityTable[i].MentityName, entityName);
	       i++);
   if (!ustrcmp (MathEntityTable[i].MentityName, entityName))
      /* entity found */
      {
      entityValue[0] = (UCHAR_T) MathEntityTable[i].charCode;
      entityValue[1] = EOS;
      *alphabet = MathEntityTable[i].alphabet;
      }
   else
      {
      entityValue[0] = EOS;
      *alphabet = EOS;
      }
}

/*----------------------------------------------------------------------
   MathMLEntityCreated
   A MathML entity has been created by the XML parser.
   Create an attribute EntityName containing the entity name.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void        MathMLEntityCreated (USTRING entityValue, Language lang, STRING entityName, Document doc)
#else
void        MathMLEntityCreated (entityValue, lang, entityName, doc)
USTRING entityValue;
Language lang;
STRING  entityName;
Document doc;

#endif
{
   ElementType	 elType;
   Element	 elText;
   AttributeType attrType;
   Attribute	 attr;
   int		 len, code;
#define MAX_ENTITY_LENGTH 80
   CHAR_T	 buffer[MAX_ENTITY_LENGTH];

   if (lang < 0)
     /* unknown entity */
     {
       /* by default display a question mark */
       entityValue[0] = '?';
       entityValue[1] = EOS;
       lang = TtaGetLanguageIdFromAlphabet('L');
       /* let's see if we can do more */
       if (entityName[0] == '#')
	  /* it's a number */
	  {
	  if (entityName[1] == 'x')
	     /* it's a hexadecimal number */
	     usscanf (&entityName[2], TEXT("%x"), &code);
	  else
	     /* it's a decimal number */
	     usscanf (&entityName[1], TEXT("%d"), &code);
	  GetFallbackCharacter (code, entityValue, &lang);
	  }
     }
   XMLTextToDocument ();
   elType.ElTypeNum = MathML_EL_TEXT_UNIT; 
   elType.ElSSchema = GetMathMLSSchema (doc);
   elText = TtaNewElement (doc, elType);
   SetElemLineNumber (elText);
   XMLInsertElement (elText);
   TtaSetTextContent (elText, entityValue, lang, doc);
   TtaSetAccessRight (elText, ReadOnly, doc);
   attrType.AttrSSchema = GetMathMLSSchema (doc);
   attrType.AttrTypeNum = MathML_ATTR_EntityName;
   attr = TtaNewAttribute (attrType);
   TtaAttachAttribute (elText, attr, doc);
   len = ustrlen (entityName);
   if (len > MAX_ENTITY_LENGTH -3)
     len = MAX_ENTITY_LENGTH -3;
   buffer[0] = '&';
   ustrncpy (&buffer[1], entityName, len);
   buffer[len+1] = ';';
   buffer[len+2] = EOS;
   TtaSetAttributeText (attr, buffer, elText, doc);
}

/*----------------------------------------------------------------------
  CheckTextElement  Put the content of input buffer into the document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void	CheckTextElement (Element *el, Document doc)
#else
static void     CheckTextElement (el, doc)
Element *el;
Document doc;

#endif
{
   ElementType parentType, elType;
   Element     parent, new;
   int         len;
   Language    lang;
   CHAR_T      alphabet;
   CHAR_T      text[4];

   len = TtaGetTextLength (*el);
   if (len == 1)
      {
      len = 2;
      TtaGiveTextContent (*el, text, &len, &lang);
      alphabet = TtaGetAlphabet (lang);
      parent = TtaGetParent (*el);
      if (text[0] != WC_EOS)
	  {
	    parentType = TtaGetElementType (parent);
	    elType = parentType;
	    if (parentType.ElTypeNum == MathML_EL_MF &&
		(text[0] == TEXT('(') ||
		 text[0] == TEXT(')') ||
		 text[0] == TEXT('[') ||
		 text[0] == TEXT(']') ||
		 text[0] == TEXT('{') ||
		 text[0] == TEXT('}')))
	       /* Transform the text element into a Thot SYMBOL */
	       elType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
	    else if (parentType.ElTypeNum == MathML_EL_MF &&
		     text[0] == TEXT('|'))
	       /* Transform the text element into a Thot GRAPHIC */
	       {
	       elType.ElTypeNum = MathML_EL_GRAPHICS_UNIT;
	       text[0] = TEXT('v');
	       }
	    else
	       /* a TEXT element is OK */
	       elType.ElTypeNum = MathML_EL_TEXT_UNIT;
	    if (elType.ElTypeNum != MathML_EL_TEXT_UNIT)
	       {
	       new = TtaNewElement (doc, elType);
	       TtaInsertSibling (new, *el, FALSE, doc);
	       TtaDeleteTree (*el, doc);
	       *el = new;
	       TtaSetGraphicsShape (new, (char)text[0], doc);
	       }
	  }
      }
}

/*----------------------------------------------------------------------
  ElementNeedsPlaceholder
  returns TRUE if element el needs a sibling placeholder.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool     ElementNeedsPlaceholder (Element el)
#else
ThotBool     ElementNeedsPlaceholder (el)
Element el;
 
#endif
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
#ifdef __STDC__
static void	CreatePlaceholders (Element el, Document doc)
#else
static void	CreatePlaceholders (el, doc)
   Element	el;
   Document	doc;
#endif
{
   Element	sibling, prev, constr, child;
   Attribute	attr;
   ElementType	elType;
   AttributeType	attrType;
   ThotBool	create;

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
	    TtaSetAttributeValue (attr, MathML_ATTR_IntPlaceholder_VAL_yes_, constr, doc);
	    }
	 create = TRUE;
	 }
      prev = sibling;
      TtaNextSibling (&sibling);
      }
   if (prev != NULL && create)
      {
	elType = TtaGetElementType (prev);
	/* don't insert a placeholder after the last element if it's a MF
	   or a SEP */
	if (elType.ElTypeNum == MathML_EL_MF ||
	    elType.ElTypeNum == MathML_EL_SEP)
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
	if (create)
	   {
           elType.ElTypeNum = MathML_EL_Construct;
	   constr = TtaNewElement (doc, elType);
	   TtaInsertSibling (constr, prev, FALSE, doc);
	   attrType.AttrSSchema = elType.ElSSchema;
	   attrType.AttrTypeNum = MathML_ATTR_IntPlaceholder;
	   attr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (constr, attr, doc);
	   TtaSetAttributeValue (attr, MathML_ATTR_IntPlaceholder_VAL_yes_, constr, doc);
	   } 
      }
}

/*----------------------------------------------------------------------
  NextNotSepOrComment
  Return the next sibling of element el that is not a SEP element
  nor an XMLcomment element.
  Return el itself if it's not a SEP or a comment.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void	NextNotSepOrComment (Element* el, Element* prev)
#else
static void	NextNotSepOrComment (el, prev)
   Element	*el;
#endif
{
   ElementType	elType;

   if (*el == NULL)
      return;
   elType = TtaGetElementType (*el);
   while (*el != NULL && (elType.ElTypeNum == MathML_EL_SEP ||
			  elType.ElTypeNum == MathML_EL_XMLcomment))
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
  Create an element of that type.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void	CheckMathSubExpressions (Element el, int type1, int type2, int type3, Document doc)
#else
static void	CheckMathSubExpressions (el, type1, type2, type3, doc)
   Element	el;
   int		type1;
   int		type2;
   int		type3;
   Document	doc;
#endif
{
  Element	child, new, prev;
  ElementType	elType, childType;

  elType.ElSSchema = GetMathMLSSchema (doc);
  child = TtaGetFirstChild (el);
  prev = NULL;
  NextNotSepOrComment (&child, &prev);
  if (child != NULL && type1 != 0)
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
      if (type2 != 0)
	{
	  prev = child;
	  TtaNextSibling (&child);
	  NextNotSepOrComment (&child, &prev);
	  if (child != NULL)
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
	      if (type3 != 0)
		{
		  prev = child;
		  TtaNextSibling (&child);
		  NextNotSepOrComment (&child, &prev);
		  if (child != NULL)
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
			}
		    }
		}
	    }
        }
    }
}


/*----------------------------------------------------------------------
   SetSingleIntHorizStretchAttr

   Put a IntHorizStretch attribute on element el if it contains only
   a MO element that is a stretchable symbol.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void SetSingleIntHorizStretchAttr (Element el, Document doc, Element* selEl)
#else /* __STDC__*/
void SetSingleIntHorizStretchAttr (el, doc, selEl)
  Element	el;
  Document	doc;
  Element*	selEl;
#endif /* __STDC__*/
{
  Element	child, sibling, textEl, symbolEl;
  ElementType	elType;
  Attribute	attr;
  AttributeType	attrType;
  int		    len;
  Language	    lang;
  CHAR_T		alphabet;
  UCHAR_T       text[2];
  unsigned char c;

  if (el == NULL)
     return;
  child = TtaGetFirstChild (el);
  if (child != NULL)
     {
     elType = TtaGetElementType (child);
     if (elType.ElTypeNum == MathML_EL_MO)
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
	      {
	      len = TtaGetTextLength (textEl);
	      if (len == 1)
		{
		len = 2;
		TtaGiveTextContent (textEl, text, &len, &lang);
		alphabet = TtaGetAlphabet (lang);
		if (len == 1)
		   if (alphabet == 'G')
		     /* a single Symbol character */
		     if ((int)text[0] == 172 || (int)text[0] == 174)
			/* horizontal arrow */
			{
			c = EOS;
			/* attach a IntHorizStretch attribute */
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
			if ((int)text[0] == 172)
			   c = '<';
			if ((int)text[0] == 174)
			   c = '>';
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
#ifdef __STDC__
static void SetIntHorizStretchAttr (Element el, Document doc)
#else /* __STDC__*/
static void SetIntHorizStretchAttr (el, doc)
  Element	el;
  Document	doc;
#endif /* __STDC__*/
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
   stretchable symbol.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void SetIntVertStretchAttr (Element el, Document doc, int base, Element* selEl)
#else /* __STDC__*/
void SetIntVertStretchAttr (el, doc, base, selEl)
  Element	el;
  Document	doc;
  int		base;
  Element*	selEl;
#endif /* __STDC__*/
{
  Element	child, sibling, textEl, symbolEl, parent, operator;
  ElementType	elType;
  Attribute	attr;
  AttributeType	attrType;
  int		    len;
  Language	    lang;
  CHAR_T		alphabet;
  UCHAR_T       text[2];
  unsigned char c;

  if (el == NULL)
     return;
  operator = NULL;
  if (base == 0)
     /* it's a MO */
     {
     parent = TtaGetParent (el);
     if (parent != NULL)
	{
	elType = TtaGetElementType (parent);
	if (elType.ElTypeNum != MathML_EL_Base &&
	    elType.ElTypeNum != MathML_EL_UnderOverBase &&
	    elType.ElTypeNum != MathML_EL_MSUBSUP &&
	    elType.ElTypeNum != MathML_EL_MSUB &&
	    elType.ElTypeNum != MathML_EL_MSUP &&
	    elType.ElTypeNum != MathML_EL_MUNDEROVER &&
	    elType.ElTypeNum != MathML_EL_MUNDER &&
	    elType.ElTypeNum != MathML_EL_MUNDEROVER)
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
        if (elType.ElTypeNum == base)
	   /* the first child is a Base or UnderOverBase */
           {
	   child = TtaGetFirstChild (child);
	   if (child != NULL)
	      {
	      elType = TtaGetElementType (child);
              if (elType.ElTypeNum == MathML_EL_MO)
	         /* its first child is a MO */
                 {
                 sibling = child;
                 TtaNextSibling (&sibling);
	         if (sibling == NULL)
	            /* there is no other child */
	            operator = child;
		 }
	      }
	   }
	}
     }
  if (operator != NULL)
     {
	   textEl = TtaGetFirstChild (operator);
	   if (textEl != NULL)
	      {
	      elType = TtaGetElementType (textEl);
	      if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
	         {
	         len = TtaGetTextLength (textEl);
	         if (len == 1)
		   {
		   len = 2;
		   TtaGiveTextContent (textEl, text, &len, &lang); 
		   alphabet = TtaGetAlphabet (lang);
		   if (len == 1)
		     if (alphabet == 'G')
		       /* a single Symbol character */
		       if ((int)text[0] == 242)
			 /* Integral */
			 {
			 /* attach a IntVertStretch attribute */
			 attrType.AttrSSchema = elType.ElSSchema;
			 attrType.AttrTypeNum = MathML_ATTR_IntVertStretch;
			 attr = TtaNewAttribute (attrType);
			 TtaAttachAttribute (el, attr, doc);
			 TtaSetAttributeValue (attr, MathML_ATTR_IntVertStretch_VAL_yes_, el, doc);
			 /* replace the TEXT element by a Thot SYMBOL element*/
			 elType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
			 symbolEl = TtaNewElement (doc, elType);
			 TtaInsertSibling (symbolEl, textEl, FALSE, doc);
			 if (selEl != NULL)
			   if (*selEl == textEl)
			      *selEl = symbolEl;
			 TtaDeleteTree (textEl, doc);
			 c = 'i';
			 TtaSetGraphicsShape (symbolEl, c, doc);
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
#ifdef __STDC__
static void SetIntPlaceholderAttr (Element el, Document doc)
#else /* __STDC__*/
static void SetIntPlaceholderAttr (el, doc)
  Element	el;
  Document	doc;
#endif /* __STDC__*/
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
#ifdef __STDC__
static void BuildMultiscript (Element elMMULTISCRIPT, Document doc)
#else /* __STDC__*/
static void BuildMultiscript (elMMULTISCRIPT, doc)
  Element	elMMULTISCRIPT;
  Document	doc;
#endif /* __STDC__*/
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
#ifdef __STDC__
static void CreateWrapper (Element el, int wrapperType, Document doc)
#else /* __STDC__*/
static void CreateWrapper (el, wrapperType, doc)
Element el;
int wrapperType;
Document doc;
#endif /* __STDC__*/
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
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void CheckMTable (Element elMTABLE, Document doc)
#else /* __STDC__*/
void CheckMTable (elMTABLE, doc)
  Element	elMTABLE;
  Document	doc;
#endif /* __STDC__*/
{
  ElementType	elType;
  Element	MTableHead, MTableBody, row, nextRow, el, prevRow, cell,
		nextCell, newMTD, firstColHead;
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
  elType.ElSSchema = MathMLSSchema;
  elType.ElTypeNum = MathML_EL_MTable_body;
  MTableBody = TtaNewElement (doc, elType);
  TtaInsertSibling (MTableBody, MTableHead, FALSE, doc);

  /* move all children of element MTABLE into the new MTable_body element
     and wrap each non-MTR element with a MTR */
  prevRow = NULL;
  while (row)
    {
    nextRow = row;
    TtaNextSibling (&nextRow);
    elType = TtaGetElementType (row);
    TtaRemoveTree (row, doc);
    if (TtaSameSSchemas (elType.ElSSchema, MathMLSSchema) &&
	(elType.ElTypeNum == MathML_EL_XMLcomment ||
	 elType.ElTypeNum == MathML_EL_MTR))
       {
       if (prevRow == NULL)
	  TtaInsertFirstChild (&row, MTableBody, doc);
       else
	  TtaInsertSibling (row, prevRow, FALSE, doc);
       prevRow = row;
       if (elType.ElTypeNum == MathML_EL_MTR)
          cell = TtaGetFirstChild (row);
       else
	  cell = NULL;
       }
    else
       /* this child is not a MTR nor a comment, create a MTR element */
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
  CheckAllRows (elMTABLE, doc);
}

/*----------------------------------------------------------------------
   SetMcharContent
   Set the content of the mchar element according to the value of
   its name attribute
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void SetMcharContent (Element el, Document doc)
#else /* __STDC__*/
void SetMcharContent (el, doc)
  Element	el;
  Document	doc;
#endif /* __STDC__*/
{
  ElementType	elType;
  AttributeType	attrType;
  Attribute	attr;
  Element       leaf;
  int           length;
  CHAR_T        name[MAX_ENTITY_LENGTH];
  UCHAR_T	value[MAX_ENTITY_LENGTH];	
  CHAR_T	alphabet;
  Language	lang;

  if (el)
     {
     /* get the name attribute */
     elType = TtaGetElementType (el);
     attrType.AttrSSchema = elType.ElSSchema;
     attrType.AttrTypeNum = MathML_ATTR_name;
     attr = TtaGetAttribute (el, attrType);
     if (attr)
        {
	leaf = TtaGetFirstChild (el);
	if (!leaf)
	  /* there is no text leaf. Create one */
	  {
	  elType.ElTypeNum = MathML_EL_TEXT_UNIT;
	  leaf = TtaNewElement (doc, elType);
	  TtaInsertFirstChild (&leaf, el, doc);
	  }
        length = MAX_ENTITY_LENGTH - 1;
        TtaGiveTextAttributeValue (attr, name, &length);
	MapMathMLEntity (name, value, MAX_ENTITY_LENGTH - 1, &alphabet);
	if (alphabet == EOS)
	   /* unknown name */
	   {
	   /* by default display a question mark */
	   value[0] = '?';
	   value[1] = EOS;
	   lang = TtaGetLanguageIdFromAlphabet('L');
	   }
	else
	   {
	   lang = TtaGetLanguageIdFromAlphabet(alphabet);
	   }
        SetElemLineNumber (leaf);
	TtaSetTextContent (leaf, value, lang, doc);
	TtaSetAccessRight (leaf, ReadOnly, doc);
        }
     }
}

/*----------------------------------------------------------------------
   SetFontstyleAttr
   The content of a MI element has been created or modified.
   Create or change attribute IntFontstyle for that element accordingly.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void SetFontstyleAttr (Element el, Document doc)
#else /* __STDC__*/
void SetFontstyleAttr (el, doc)
  Element	el;
  Document	doc;
#endif /* __STDC__*/
{
  ElementType	elType;
  AttributeType	attrType;
  Attribute	attr, IntAttr;
  Element       textEl;
  int		len;
  STRING        value;
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
	      DifferentialD. */
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
		  value = TtaAllocString (len+1);
		  TtaGiveTextAttributeValue (attr, value, &len);
		  if (ustrcmp (value, TEXT("&ImaginaryI;")) == 0 ||
		      ustrcmp (value, TEXT("&ExponentialE;")) == 0 ||
		      ustrcmp (value, TEXT("&DifferentialD;")) == 0)
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
#ifdef __STDC__
void SetIntAddSpaceAttr (Element el, Document doc)
#else /* __STDC__*/
void SetIntAddSpaceAttr (el, doc)
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
  UCHAR_T    	text[BUFLEN];
  Language	lang;
  CHAR_T		alphabet;

  textEl = TtaGetFirstChild (el);
  if (textEl != NULL)
     {
     /* search the IntAddSpace attribute */
     elType = TtaGetElementType (el);
     attrType.AttrSSchema = elType.ElSSchema;
     attrType.AttrTypeNum = MathML_ATTR_IntAddSpace;
     attr = TtaGetAttribute (el, attrType);
     if (attr == NULL)
	{
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (el, attr, doc);
	}
     val = MathML_ATTR_IntAddSpace_VAL_nospace;
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
		   val = MathML_ATTR_IntAddSpace_VAL_nospace;
		else
		   {
		   elType = TtaGetElementType (previous);
		   if (elType.ElTypeNum == MathML_EL_MO)
		      /* after an operator => unary operator */
		      val = MathML_ATTR_IntAddSpace_VAL_nospace;
		   else
		      /* binary operator */
		      val = MathML_ATTR_IntAddSpace_VAL_both;
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
	         val = MathML_ATTR_IntAddSpace_VAL_both;
	     else if (text[0] == ',' ||
		      text[0] == ';')
	         val = MathML_ATTR_IntAddSpace_VAL_spaceafter;
	     }
	   else if (alphabet == 'G')
	     /* Symbol character set */
	     if ((int)text[0] == 163 || /* less or equal */
		 (int)text[0] == 177 ||	/* plus or minus */
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
		val = MathML_ATTR_IntAddSpace_VAL_both;
	}
     TtaSetAttributeValue (attr, val, el, doc);
     }
}


/*----------------------------------------------------------------------
   ChangeTypeOfElement
   Change the type of element elem into newTypeNum
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void ChangeTypeOfElement (Element elem, Document doc, int newTypeNum)
#else /* __STDC__*/
void ChangeTypeOfElement (elem, doc, newTypeNum)
     Element elem;
     Document doc;
     int newTypeNum;
#endif /* __STDC__*/
 
{
     Element    prev, next, parent;

     parent = NULL;
     prev = elem;
     TtaPreviousSibling (&prev);
     if (prev == NULL)
        {
        next = elem;
        TtaNextSibling (&next);
        if (next == NULL)
           parent = TtaGetParent (elem);
        }
     TtaRemoveTree (elem, doc);
     ChangeElementType (elem, newTypeNum);
     if (prev != NULL)
        TtaInsertSibling (elem, prev, FALSE, doc);
     else if (next != NULL)
        TtaInsertSibling (elem, next, TRUE, doc);
     else
        TtaInsertFirstChild (&elem, parent, doc);
}


/*----------------------------------------------------------------------
   CheckFence
   If el is a MO element that contains a single fence character,
   transform the MO into a MF and the character into a Thot symbol.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      CheckFence (Element el, Document doc)
#else
void      CheckFence (el, doc)
Element                 el;
Document		doc;

#endif
{
   ElementType	 elType;
   Element	     content;
   AttributeType attrType;
   Attribute	 attr, attrStretchy;
   int           len, val;
   Language	     lang;
   CHAR_T		 alphabet;
   UCHAR_T       text[2];
   unsigned char c;

   elType = TtaGetElementType (el);
   if (elType.ElTypeNum == MathML_EL_MO)
      {
      content = TtaGetFirstChild (el);
      if (content != NULL)
	{
	elType = TtaGetElementType (content);
	if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
	   {
	   len = TtaGetTextLength (content);
	   if (len == 1)
	      {
	      len = 2;
	      TtaGiveTextContent (content, text, &len, &lang);
	      alphabet = TtaGetAlphabet (lang);
	      if (len == 1)
		if (alphabet == 'L')
		   /* a single character */
		   if (text[0] == TEXT('(') || text[0] == TEXT(')') ||
		       text[0] == TEXT('[') || text[0] == TEXT(']') ||
		       text[0] == TEXT('{') || text[0] == TEXT('}') ||
		       text[0] == TEXT('|'))
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
		         /* attach a IntVertStretch attribute to the MF element */
		         attrType.AttrTypeNum = MathML_ATTR_IntVertStretch;
		         attr = TtaNewAttribute (attrType);
		         TtaAttachAttribute (el, attr, doc);
		         TtaSetAttributeValue (attr, MathML_ATTR_IntVertStretch_VAL_yes_, el, doc);
			 }
		      /* create a new content for the MF element */
		      if (text[0] == TEXT('|'))
			 {
			 elType.ElTypeNum = MathML_EL_GRAPHICS_UNIT;
			 c = 'v';
			 }
		      else
			 {
		         elType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
		         c = (char) text[0];
			 }
		      content = TtaNewElement (doc, elType);
		      TtaInsertFirstChild (&content, el, doc);
		      TtaSetGraphicsShape (content, c, doc);
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
#ifdef __STDC__
void      CreateFencedSeparators (Element fencedExpression, Document doc, ThotBool record)
#else
void      CreateFencedSeparators (fencedExpression, doc, record)
Element		fencedExpression;
Document	doc;
ThotBool        record;

#endif
{
   ElementType	 elType;
   Element	 child, separator, leaf, next, prev, mfenced;
   AttributeType attrType;
   Attribute     attr;
   int		 length, sep, i;
   Language	 lang;
   CHAR_T	 text[32], sepValue[4];

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
   TransformMFENCED
   Transform the content of a MFENCED element: create elements
   OpeningFence, FencedExpression, ClosingFence and FencedSeparator.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void      TransformMFENCED (Element el, Document doc)
#else
static void      TransformMFENCED (el, doc)
Element		el;
Document	doc;

#endif
{
   ElementType	 elType;
   Element	 child, fencedExpression, leaf, fence, next, prev,
		 firstChild;
   AttributeType attrType;
   Attribute     attr;
   int           length;
   CHAR_T        text[32];
   char          c;

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
      c = '(';
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = MathML_ATTR_open;
      attr = TtaGetAttribute (el, attrType);
      if (attr != NULL)
	{
        length = 7;
        TtaGiveTextAttributeValue (attr, text, &length);
	c = (char)text[0];
	}
      elType.ElTypeNum = MathML_EL_OpeningFence;
      fence = TtaNewElement (doc, elType);
      TtaInsertSibling (fence, fencedExpression, TRUE, doc);
      elType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
      leaf = TtaNewElement (doc, elType);
      TtaInsertFirstChild (&leaf, fence, doc);
      TtaSetGraphicsShape (leaf, c, doc);

      /* create the ClosingFence element according to close attribute */
      c = ')';
      attrType.AttrTypeNum = MathML_ATTR_close;
      attr = TtaGetAttribute (el, attrType);
      if (attr != NULL)
	{
        length = 7;
        TtaGiveTextAttributeValue (attr, text, &length);
	c = (char) text[0];
	}
      elType.ElTypeNum = MathML_EL_ClosingFence;
      fence = TtaNewElement (doc, elType);
      TtaInsertSibling (fence, fencedExpression, FALSE, doc);
      elType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
      leaf = TtaNewElement (doc, elType);
      TtaInsertFirstChild (&leaf, fence, doc);
      TtaSetGraphicsShape (leaf, c, doc);
      }
}

/*----------------------------------------------------------------------
   MathMLElementComplete
   Check the Thot structure of the MathML element el.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      MathMLElementComplete (Element el, Document doc)
#else
void      MathMLElementComplete (el, doc)
Element		el;
Document	doc;

#endif
{
   ElementType		elType, parentType;
   Element		child, parent, new, prev, next;
   AttributeType	attrType;
   Attribute		attr;
   SSchema       MathMLSSchema;

   elType = TtaGetElementType (el);
   MathMLSSchema = GetMathMLSSchema (doc);

   if (elType.ElSSchema != MathMLSSchema)
     /* this is not a MathML element. It's the HTML element <math>, or
	any other element containing a MathML expression */
     {
     if (TtaGetFirstChild (el) == NULL && !TtaIsLeaf (elType))
	/* this element is empty. Create a MathML element as it's child */
	{
	elType.ElSSchema = MathMLSSchema;
	elType.ElTypeNum = MathML_EL_MathML;
	new = TtaNewElement (doc, elType);
	TtaInsertFirstChild (&new, el, doc);
	/* Create a placeholder within the MathML element */
        elType.ElTypeNum = MathML_EL_Construct;
	child = TtaNewElement (doc, elType);
	TtaInsertFirstChild (&child, new, doc);
	attrType.AttrSSchema = elType.ElSSchema;
	attrType.AttrTypeNum = MathML_ATTR_IntPlaceholder;
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (child, attr, doc);
	TtaSetAttributeValue (attr, MathML_ATTR_IntPlaceholder_VAL_yes_,
			      child, doc);
	}
     }
   else
     {
     switch (elType.ElTypeNum)
       {
       case MathML_EL_TEXT_UNIT:
	  CheckTextElement (&el, doc);
	  break;
       case MathML_EL_MI:
	  SetFontstyleAttr (el, doc);
	  break;
       case MathML_EL_MO:
	  SetIntAddSpaceAttr (el, doc);
	  SetIntVertStretchAttr (el, doc, 0, NULL);
	  break;
       case MathML_EL_MROW:
	  /* end of MROW */
	  /*if the first and the last child are MO containing a fence character
	     transform the MO into a MF and the character into a Thot SYMBOL */
	  child = TtaGetFirstChild (el);
	  if (child != NULL)
	    {
	    CheckFence (child, doc);
	    child = TtaGetLastChild (el);
	    if (child != NULL)
	      CheckFence (child, doc);
	    /* Create placeholders within the MROW */
            CreatePlaceholders (TtaGetFirstChild (el), doc);
	    }
	  break;
       case MathML_EL_MFRAC:
       case MathML_EL_BevelledMFRAC:
	  /* end of a fraction. Create a Numerator and a Denominator */
	  CheckMathSubExpressions (el, MathML_EL_Numerator,
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
	  CheckMathSubExpressions (el, MathML_EL_RootBase, MathML_EL_Index,
				   0, doc);
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
	  CheckMathSubExpressions (el, MathML_EL_Base, MathML_EL_Subscript,
				   0, doc);
	  SetIntVertStretchAttr (el, doc, MathML_EL_Base, NULL);
	  break;
       case MathML_EL_MSUP:
	  /* end of a MSUP. Create Base and Superscript */
	  CheckMathSubExpressions (el, MathML_EL_Base, MathML_EL_Superscript,
				   0, doc);
	  SetIntVertStretchAttr (el, doc, MathML_EL_Base, NULL);
	  break;
       case MathML_EL_MSUBSUP:
	  /* end of a MSUBSUP. Create Base, Subscript, and Superscript */
	  CheckMathSubExpressions (el, MathML_EL_Base, MathML_EL_Subscript,
				   MathML_EL_Superscript, doc);
	  SetIntVertStretchAttr (el, doc, MathML_EL_Base, NULL);
	  break;
       case MathML_EL_MUNDER:
	  /* end of a MUNDER. Create UnderOverBase, and Underscript */
	  CheckMathSubExpressions (el, MathML_EL_UnderOverBase,
				   MathML_EL_Underscript, 0, doc);
	  SetIntHorizStretchAttr (el, doc);
	  SetIntVertStretchAttr (el, doc, MathML_EL_UnderOverBase, NULL);
	  break;
       case MathML_EL_MOVER:
	  /* end of a MOVER. Create UnderOverBase, and Overscript */
	  CheckMathSubExpressions (el, MathML_EL_UnderOverBase,
				   MathML_EL_Overscript, 0, doc);
	  SetIntHorizStretchAttr (el, doc);
	  SetIntVertStretchAttr (el, doc, MathML_EL_UnderOverBase, NULL);
	  break;
       case MathML_EL_MUNDEROVER:
	  /* end of a MUNDEROVER. Create UnderOverBase, Underscript, and
	     Overscript */
	  CheckMathSubExpressions (el, MathML_EL_UnderOverBase,
			  MathML_EL_Underscript, MathML_EL_Overscript, doc);
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
	  CheckMTable (el, doc);
	  break;
       case MathML_EL_MTD:
	  /* Create placeholders within the table cell */
          CreatePlaceholders (TtaGetFirstChild (el), doc);
	  break;
       case MathML_EL_MCHAR:
	 /* set the content of the mchar element according to the value of
	    its name attribute */
	  SetMcharContent (el, doc);
	  break;
       case MathML_EL_MACTION:
	  /* Create placeholders within the MACTION element */
          CreatePlaceholders (TtaGetFirstChild (el), doc);
	  break;
       default:
	  break;
       }
     parent = TtaGetParent (el);
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

/*----------------------------------------------------------------------
 SetFontfamily
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void SetFontfamily (Document doc, Element el, STRING value)
#else /* __STDC__*/
void SetFontfamily (doc, el, value)
  Document doc;
  Element el;
  STRING value;
#endif /* __STDC__*/
{
#define buflen 50
  CHAR_T           css_command[buflen+20];
 
  usprintf (css_command, TEXT("font-family: %s"), value);
  ParseHTMLSpecificStyle (el, css_command, doc, FALSE);
}

/*----------------------------------------------------------------------
 SetFontsize
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void SetFontsize (Document doc, Element el, STRING value)
#else /* __STDC__*/
void SetFontsize (doc, el, value)
  Document doc;
  Element el;
  STRING value;
#endif /* __STDC__*/
{
#define buflen 50
  CHAR_T           css_command[buflen+20];
 
  usprintf (css_command, TEXT("font-size: %s"), value);
  ParseHTMLSpecificStyle (el, css_command, doc, FALSE);
}

/*----------------------------------------------------------------------
   MathMLAttributeComplete
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      MathMLAttributeComplete (Attribute attr, Element el, Document doc)
#else
void      MathMLAttributeComplete (attr, el, doc)
Attribute	attr;
Element		el;
Document	doc;

#endif
{
   AttributeType     attrType;
   int		     attrKind;
   ElementType       elType;
#define buflen 50
   STRING            value;
   int               val, length;
 
   TtaGiveAttributeType (attr, &attrType, &attrKind);
   if (attrType.AttrTypeNum == MathML_ATTR_bevelled)
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
   else if (attrType.AttrTypeNum == MathML_ATTR_color ||
       attrType.AttrTypeNum == MathML_ATTR_background_ ||
       attrType.AttrTypeNum == MathML_ATTR_fontsize ||
       attrType.AttrTypeNum == MathML_ATTR_fontfamily)
      {
      length = TtaGetTextAttributeLength (attr);
      if (length >= buflen)
         length = buflen - 1;
      if (length > 0)
	 {
	   value = TtaAllocString (buflen);
	   value[0] = EOS;
	   TtaGiveTextAttributeValue (attr, value, &length);
	   switch (attrType.AttrTypeNum)
	     {
	     case MathML_ATTR_color:
               HTMLSetForegroundColor (doc, el, value);
	       break;
	     case MathML_ATTR_background_:
               HTMLSetBackgroundColor (doc, el, value);
	       break;
	     case MathML_ATTR_fontsize:
	       SetFontsize (doc, el, value);
	       break;
	     case MathML_ATTR_fontfamily:
	       SetFontfamily (doc, el, value);
	       break;
	     }
	   TtaFreeMemory (value);
	 }
      }
}

/*----------------------------------------------------------------------
   MathMLGetDTDName
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      MathMLGetDTDName (STRING DTDname, STRING elementName)
#else
void      MathMLGetDTDName (DTDname, elementName)
STRING DTDname;
STRING elementName;

#endif
{
   /* no other DTD allowed within MathML elements */
   ustrcpy (DTDname, TEXT(""));
}

/* end of module */
