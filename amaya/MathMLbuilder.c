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

typedef UCHAR_T  MathEntityName[30];
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
#if defined (_I18N_) || defined (__JIS__)
    {L"Agr", 65, L'G'},
    {L"And", 217, L'G'},
    {L"ApplyFunction", 32, L'L'},  /* render as white space */
    {L"Backslash", 92, L'L'},
    {L"Bgr", 66, L'G'},
    {L"Cap", 199, L'G'},
    {L"CenterDot", 215, L'G'},
    {L"CirclePlus", 197, L'G'},
    {L"CircleTimes", 196, L'G'},
    {L"Colon", 58, L'G'},
    {L"Congruent", 64, L'G'},
    {L"Cup", 200, L'G'},
    {L"Delta", 68, L'G'},
    {L"Diamond", 168, L'G'},
    {L"DoubleDownArrow", 223, L'G'},
    {L"DoubleLeftArrow", 220, L'G'},
    {L"DoubleLeftRightArrow", 219, L'G'},
    {L"DoubleRightArrow", 222, L'G'},
    {L"DoubleUpArrow", 221, L'G'},
    {L"DownArrow", 175, L'G'},
    {L"DownTee", 94, L'G'},
    {L"EEgr", 72, L'G'},
    {L"Egr", 69, L'G'},
    {L"Element", 206, L'G'},
    {L"Equal", 61, L'L'},
    {L"EqualTilde", 64, L'G'},
    {L"Exists", 36, L'G'},
    {L"ForAll", 34, L'G'},
    {L"Gamma", 71, L'G'},
    {L"GreaterEqual", 179, L'G'},
    {L"Igr", 73, L'G'},
    {L"Integral", 242, L'G'},
    {L"Intersection", 199, L'G'},
    {L"InvisibleTimes", 0, SPACE},
    {L"Kgr", 75, L'G'},
    {L"KHgr", 67, L'G'},
    {L"Lambda", 76, L'G'},
    {L"LeftArrow", 172, L'G'},
    {L"LeftRightArrow", 171, L'G'},
    {L"Mgr", 77, L'G'},
    {L"Ngr", 78, L'G'},
    {L"NonBreakingSpace", 160, L'L'},
    {L"Not", 216, L'G'},
    {L"NotElement", 207, L'G'},
    {L"NotEqual", 185, L'G'},
    {L"NotSubset", 203, L'G'},
    {L"Ogr", 79, L'G'},
    {L"Omega", 87, L'G'},
    {L"Or", 218, L'G'},
    {L"PI", 213, L'G'},
    {L"PartialD", 182, L'G'},
    {L"Phi", 70, L'G'},
    {L"Pi", 80, L'G'},
    {L"PlusMinus", 177, L'G'},
    {L"Product", 213, L'G'},
    {L"Proportional", 181, L'G'},
    {L"Psi", 89, L'G'},
    {L"Rgr", 82, L'G'},
    {L"RightArrow", 174, L'G'},
    {L"Sigma", 83, L'G'},
    {L"Sol", 164, L'G'},
    {L"Star", 42, L'L'},
    {L"Subset", 204, L'G'},
    {L"SubsetEqual", 205, L'G'},
    {L"SuchThat", 39, L'G'},
    {L"Sum", 229, L'G'},
    {L"Superset", 201, L'G'},
    {L"SupersetEqual", 202, L'G'},
    {L"Tgr", 84, L'G'},
    {L"Therefore", 92, L'G'},
    {L"Theta", 81, L'G'},
    {L"ThickSpace", 32, L'L'},
    {L"Tilde", 126, L'L'},
    {L"TripleDot", 188, L'G'},
    {L"Union", 200, L'G'},
    {L"UpArrow", 173, L'G'},
    {L"Upsi", 85, L'G'},
    {L"Upsi1", 161, L'G'},
    {L"Vee", 218, L'G'},
    {L"Verbar", 189, L'G'},
    {L"VerticalBar", 124, L'L'},
    {L"Xi", 88, L'G'},
    {L"Zgr", 90, L'G'},
    {L"af", 32, L'L'},             /* render as white space */
    {L"aleph", 192, L'G'},
    {L"alpha", 97, L'G'},
    {L"and", 217, L'G'},
    {L"angle", 208, L'G'},
    {L"ap", 187, L'G'},
    {L"beta", 98, L'G'},
    {L"bottom", 94, L'G'},
    {L"bull", 183, L'G'},
    {L"cap", 199, L'G'},
    {L"chi", 99, L'G'},
    {L"clubs", 167, L'G'},
    {L"cong", 64, L'G'},
    {L"copysf", 211, L'G'},
    {L"copyssf", 227, L'G'},
    {L"cr", 191, L'G'},
    {L"cup", 200, L'G'},
    {L"darr", 175, L'G'},
    {L"dArr", 223, L'G'},
    {L"dd", 100, L'L'},
    {L"deg", 176, L'G'},
    {L"delta", 100, L'G'},
    {L"diams", 168, L'G'},
    {L"divide", 184, L'G'},
    {L"dtri", 209, L'G'},
    {L"ee", 101, L'L'},
    {L"empty", 198, L'G'},
    {L"emsp", 32, L'G'},
    {L"epsiv", 101, L'G'},
    {L"equiv", 186, L'G'},
    {L"eta", 104, L'G'},
    {L"exist", 36, L'G'},
    {L"florin", 166, L'G'},
    {L"forall", 34, L'G'},
    {L"gamma", 103, L'G'},
    {L"ge", 179, L'G'},
    {L"gt", 62, L'L'},
    {L"hearts", 169, L'G'},
    {L"horbar", 190, L'G'},
    {L"ifraktur", 193, L'G'},
    {L"infin", 165, L'G'},
    {L"int", 242, L'G'},
    {L"iota", 105, L'G'},
    {L"isin", 206, L'G'},
    {L"it", 0, SPACE},
    {L"kappa", 107, L'G'},
    {L"lambda", 108, L'G'},
    {L"lang", 225, L'G'},
    {L"larr", 172, L'G'},
    {L"lArr", 220, L'G'},
    {L"le", 163, L'G'},
    {L"lowbar", 95, L'G'},
    {L"loz", 224, L'G'},
    {L"lrarr", 171, L'G'},
    {L"lrArr", 219, L'G'},
    {L"lsqb", 91, L'G'},
    {L"lt", 60, L'L'},
    {L"middot", 215, L'G'},
    {L"mldr", 188, L'G'},
    {L"mu", 109, L'G'},
    {L"ne", 185, L'G'},
    {L"not", 216, L'G'},
    {L"notin", 207, L'G'},
    {L"nu", 110, L'G'},
    {L"ogr", 111, L'G'},
    {L"omega", 119, L'G'},
    {L"oplus", 197, L'G'},
    {L"or", 218, L'G'},
    {L"otimes", 196, L'G'},
    {L"part", 182, L'G'},
    {L"phi", 102, L'G'},
    {L"phiv", 106, L'G'},
    {L"pi", 112, L'G'},
    {L"piv", 118, L'G'},
    {L"prop", 181, L'G'},
    {L"psi", 121, L'G'},
    {L"radic", 214, L'G'},
    {L"rarr", 174, L'G'},
    {L"rArr", 222, L'G'},
    {L"rdquo", 178, L'G'},
    {L"regsf", 210, L'G'},
    {L"regssf", 226, L'G'},
    {L"rfraktur", 194, L'G'},
    {L"rho", 114, L'G'},
    {L"rsqb", 93, L'G'},
    {L"sigma", 115, L'G'},
    {L"sigmav", 86, L'G'},
    {L"spades", 170, L'G'},
    {L"sub", 204, L'G'},
    {L"sube", 205, L'G'},
    {L"subne", 203, L'G'},
    {L"sum", 229, L'G'},
    {L"sup", 201, L'G'},
    {L"supe", 202, L'G'},
    {L"tau", 116, L'G'},
    {L"there4", 92, L'G'},
    {L"theta", 113, L'G'},
    {L"thetav", 74, L'G'},
    {L"thickspace", 32, L'L'},
    {L"times", 180, L'G'},
    {L"trade", 212, L'G'},
    {L"tradesf", 212, L'G'},
    {L"tradessf", 228, L'G'},
    {L"uarr", 173, L'G'},
    {L"uArr", 221, L'G'},
    {L"upsi", 117, L'G'},
    {L"vee", 218, L'G'},
    {L"weierp", 195, L'G'},
    {L"xi", 120, L'G'},
    {L"zeta", 122, L'G'},
    {L"zzzz", -1, SPACE}		/* this last entry is required */

#else
   {"Agr", 65, 'G'},
   {"And", 217, 'G'},
   {"ApplyFunction", 32, 'L'},  /* render as white space */
   {"Backslash", 92, 'L'},
   {"Bgr", 66, 'G'},
   {"Cap", 199, 'G'},
   {"CenterDot", 215, 'G'},
   {"CirclePlus", 197, 'G'},
   {"CircleTimes", 196, 'G'},
   {"Colon", 58, 'G'},
   {"Congruent", 64, 'G'},
   {"Cup", 200, 'G'},
   {"Delta", 68, 'G'},
   {"Diamond", 168, 'G'},
   {"DoubleDownArrow", 223, 'G'},
   {"DoubleLeftArrow", 220, 'G'},
   {"DoubleLeftRightArrow", 219, 'G'},
   {"DoubleRightArrow", 222, 'G'},
   {"DoubleUpArrow", 221, 'G'},
   {"DownArrow", 175, 'G'},
   {"DownTee", 94, 'G'},
   {"EEgr", 72, 'G'},
   {"Egr", 69, 'G'},
   {"Element", 206, 'G'},
   {"Equal", 61, 'L'},
   {"EqualTilde", 64, 'G'},
   {"Exists", 36, 'G'},
   {"ForAll", 34, 'G'},
   {"Gamma", 71, 'G'},
   {"GreaterEqual", 179, 'G'},
   {"Igr", 73, 'G'},
   {"Integral", 242, 'G'},
   {"Intersection", 199, 'G'},
   {"InvisibleTimes", 0, SPACE},
   {"Kgr", 75, 'G'},
   {"KHgr", 67, 'G'},
   {"Lambda", 76, 'G'},
   {"LeftArrow", 172, 'G'},
   {"LeftRightArrow", 171, 'G'},
   {"Mgr", 77, 'G'},
   {"Ngr", 78, 'G'},
   {"NonBreakingSpace", 160, 'L'},
   {"Not", 216, 'G'},
   {"NotElement", 207, 'G'},
   {"NotEqual", 185, 'G'},
   {"NotSubset", 203, 'G'},
   {"Ogr", 79, 'G'},
   {"Omega", 87, 'G'},
   {"Or", 218, 'G'},
   {"PI", 213, 'G'},
   {"PartialD", 182, 'G'},
   {"Phi", 70, 'G'},
   {"Pi", 80, 'G'},
   {"PlusMinus", 177, 'G'},
   {"Product", 213, 'G'},
   {"Proportional", 181, 'G'},
   {"Psi", 89, 'G'},
   {"Rgr", 82, 'G'},
   {"RightArrow", 174, 'G'},
   {"Sigma", 83, 'G'},
   {"Sol", 164, 'G'},
   {"Star", 42, 'L'},
   {"Subset", 204, 'G'},
   {"SubsetEqual", 205, 'G'},
   {"SuchThat", 39, 'G'},
   {"Sum", 229, 'G'},
   {"Superset", 201, 'G'},
   {"SupersetEqual", 202, 'G'},
   {"Tgr", 84, 'G'},
   {"Therefore", 92, 'G'},
   {"Theta", 81, 'G'},
   {"Tilde", 126, 'L'},
   {"TripleDot", 188, 'G'},
   {"Union", 200, 'G'},
   {"UpArrow", 173, 'G'},
   {"Upsi", 85, 'G'},
   {"Upsi1", 161, 'G'},
   {"Vee", 218, 'G'},
   {"Verbar", 189, 'G'},
   {"VerticalBar", 124, 'L'},
   {"Xi", 88, 'G'},
   {"Zgr", 90, 'G'},
   {"af", 32, 'L'},             /* render as white space */
   {"aleph", 192, 'G'},
   {"alpha", 97, 'G'},
   {"and", 217, 'G'},
   {"angle", 208, 'G'},
   {"ap", 187, 'G'},
   {"beta", 98, 'G'},
   {"bottom", 94, 'G'},
   {"bull", 183, 'G'},
   {"cap", 199, 'G'},
   {"chi", 99, 'G'},
   {"clubs", 167, 'G'},
   {"cong", 64, 'G'},
   {"copysf", 211, 'G'},
   {"copyssf", 227, 'G'},
   {"cr", 191, 'G'},
   {"cup", 200, 'G'},
   {"darr", 175, 'G'},
   {"dArr", 223, 'G'},
   {"dd", 100, 'L'},
   {"deg", 176, 'G'},
   {"delta", 100, 'G'},
   {"diams", 168, 'G'},
   {"divide", 184, 'G'},
   {"dtri", 209, 'G'},
   {"ee", 101, 'L'},
   {"empty", 198, 'G'},
   {"emsp", 32, 'G'},
   {"epsiv", 101, 'G'},
   {"equiv", 186, 'G'},
   {"eta", 104, 'G'},
   {"exist", 36, 'G'},
   {"florin", 166, 'G'},
   {"forall", 34, 'G'},
   {"gamma", 103, 'G'},
   {"ge", 179, 'G'},
   {"gt", 62, 'L'},
   {"hearts", 169, 'G'},
   {"horbar", 190, 'G'},
   {"ifraktur", 193, 'G'},
   {"infin", 165, 'G'},
   {"int", 242, 'G'},
   {"iota", 105, 'G'},
   {"isin", 206, 'G'},
   {"it", 0, SPACE},
   {"kappa", 107, 'G'},
   {"lambda", 108, 'G'},
   {"lang", 225, 'G'},
   {"larr", 172, 'G'},
   {"lArr", 220, 'G'},
   {"le", 163, 'G'},
   {"lowbar", 95, 'G'},
   {"loz", 224, 'G'},
   {"lrarr", 171, 'G'},
   {"lrArr", 219, 'G'},
   {"lsqb", 91, 'G'},
   {"lt", 60, 'L'},
   {"middot", 215, 'G'},
   {"mldr", 188, 'G'},
   {"mu", 109, 'G'},
   {"ne", 185, 'G'},
   {"not", 216, 'G'},
   {"notin", 207, 'G'},
   {"nu", 110, 'G'},
   {"ogr", 111, 'G'},
   {"omega", 119, 'G'},
   {"oplus", 197, 'G'},
   {"or", 218, 'G'},
   {"otimes", 196, 'G'},
   {"part", 182, 'G'},
   {"phi", 102, 'G'},
   {"phiv", 106, 'G'},
   {"pi", 112, 'G'},
   {"piv", 118, 'G'},
   {"prop", 181, 'G'},
   {"psi", 121, 'G'},
   {"radic", 214, 'G'},
   {"rarr", 174, 'G'},
   {"rArr", 222, 'G'},
   {"rdquo", 178, 'G'},
   {"regsf", 210, 'G'},
   {"regssf", 226, 'G'},
   {"rfraktur", 194, 'G'},
   {"rho", 114, 'G'},
   {"rsqb", 93, 'G'},
   {"sigma", 115, 'G'},
   {"sigmav", 86, 'G'},
   {"spades", 170, 'G'},
   {"sub", 204, 'G'},
   {"sube", 205, 'G'},
   {"subne", 203, 'G'},
   {"sum", 229, 'G'},
   {"sup", 201, 'G'},
   {"supe", 202, 'G'},
   {"tau", 116, 'G'},
   {"there4", 92, 'G'},
   {"theta", 113, 'G'},
   {"thetav", 74, 'G'},
   {"thickspace", 32, 'L'},
   {"times", 180, 'G'},
   {"trade", 212, 'G'},
   {"tradesf", 212, 'G'},
   {"tradessf", 228, 'G'},
   {"uarr", 173, 'G'},
   {"uArr", 221, 'G'},
   {"upsi", 117, 'G'},
   {"vee", 218, 'G'},
   {"weierp", 195, 'G'},
   {"xi", 120, 'G'},
   {"zeta", 122, 'G'},
   {"zzzz", -1, SPACE}		/* this last entry is required */

#endif
};

static AttributeMapping MathMLAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   /* The rest of this table MUST be in alphabetical order */
   {TEXT ("unknown_attr"), _EMPTYSTR_, 'A', MathML_ATTR_Invalid_attribute},
   {TEXT ("ZZGHOST"), _EMPTYSTR_, 'A', MathML_ATTR_Ghost_restruct},
   {TEXT ("accent"), TEXT (""), 'A', MathML_ATTR_accent},
   {TEXT ("accentunder"), TEXT (""), 'A', MathML_ATTR_accentunder},
   {TEXT ("actiontype"), TEXT (""), 'A', MathML_ATTR_actiontype},
   {TEXT ("align"), TEXT (""), 'A', MathML_ATTR_align},
   {TEXT ("alignmentscope"), TEXT (""), 'A', MathML_ATTR_alignmentscope},
   {TEXT ("background"), TEXT (""), 'A', MathML_ATTR_background_},
   {TEXT ("class"), TEXT (""), 'A', MathML_ATTR_class},
   {TEXT ("close"), TEXT (""), 'A', MathML_ATTR_close},
   {TEXT ("columnalign"), TEXT (""), 'A', MathML_ATTR_columnalign},
   {TEXT ("columnlines"), TEXT (""), 'A', MathML_ATTR_columnlines},
   {TEXT ("columnspacing"), TEXT (""), 'A', MathML_ATTR_columnspacing},
   {TEXT ("columnspan"), TEXT (""), 'A', MathML_ATTR_columnspan},
   {TEXT ("color"), TEXT (""), 'A', MathML_ATTR_color},
   {TEXT ("depth"), TEXT (""), 'A', MathML_ATTR_depth_},
   {TEXT ("displaystyle"), TEXT (""), 'A', MathML_ATTR_displaystyle},
   {TEXT ("edge"), TEXT (""), 'A', MathML_ATTR_edge},
   {TEXT ("equalcolumns"), TEXT (""), 'A', MathML_ATTR_equalcolumns},
   {TEXT ("equalrows"), TEXT (""), 'A', MathML_ATTR_equalrows},
   {TEXT ("fence"), TEXT (""), 'A', MathML_ATTR_fence},
   {TEXT ("fontfamily"), TEXT (""), 'A', MathML_ATTR_fontfamily},
   {TEXT ("fontstyle"), TEXT (""), 'A', MathML_ATTR_fontstyle},
   {TEXT ("fontsize"), TEXT (""), 'A', MathML_ATTR_fontsize},
   {TEXT ("fontweight"), TEXT (""), 'A', MathML_ATTR_fontweight},
   {TEXT ("form"), TEXT (""), 'A', MathML_ATTR_form},
   {TEXT ("frame"), TEXT (""), 'A', MathML_ATTR_frame},
   {TEXT ("framespacing"), TEXT (""), 'A', MathML_ATTR_framespacing},
   {TEXT ("groupalign"), TEXT (""), 'A', MathML_ATTR_groupalign},
   {TEXT ("height"), TEXT (""), 'A', MathML_ATTR_height_},
   {TEXT ("id"), TEXT (""), 'A', MathML_ATTR_id},
   {TEXT ("largeop"), TEXT (""), 'A', MathML_ATTR_largeop},
   {TEXT ("linethickness"), TEXT (""), 'A', MathML_ATTR_linethickness},
   {TEXT ("link"), TEXT (""), 'A', MathML_ATTR_link},
   {TEXT ("lquote"), TEXT (""), 'A', MathML_ATTR_lquote},
   {TEXT ("lspace"), TEXT (""), 'A', MathML_ATTR_lspace},
   {TEXT ("maxsize"), TEXT (""), 'A', MathML_ATTR_maxsize},
   {TEXT ("minsize"), TEXT (""), 'A', MathML_ATTR_minsize},
   {TEXT ("movablelimits"), TEXT (""), 'A', MathML_ATTR_movablelimits},
   {TEXT ("open"), TEXT (""), 'A', MathML_ATTR_open},
   {TEXT ("other"), TEXT (""), 'A', MathML_ATTR_other},
   {TEXT ("rowalign"), TEXT (""), 'A', MathML_ATTR_rowalign},
   {TEXT ("rowlines"), TEXT (""), 'A', MathML_ATTR_rowlines},
   {TEXT ("rowspacing"), TEXT (""), 'A', MathML_ATTR_rowspacing},
   {TEXT ("rowspan"), TEXT (""), 'A', MathML_ATTR_rowspan_},
   {TEXT ("rquote"), TEXT (""), 'A', MathML_ATTR_rquote},
   {TEXT ("rspace"), TEXT (""), 'A', MathML_ATTR_rspace},
   {TEXT ("scriptlevel"), TEXT (""), 'A', MathML_ATTR_scriptlevel},
   {TEXT ("scriptminsize"), TEXT (""), 'A', MathML_ATTR_scriptminsize},
   {TEXT ("scriptsizemultiplier"), TEXT (""), 'A', MathML_ATTR_scriptsizemultiplier},
   {TEXT ("selection"), TEXT (""), 'A', MathML_ATTR_selection},
   {TEXT ("separator"), TEXT (""), 'A', MathML_ATTR_separator},
   {TEXT ("separators"), TEXT (""), 'A', MathML_ATTR_separators},
   {TEXT ("stretchy"), TEXT (""), 'A', MathML_ATTR_stretchy},
   {TEXT ("style"), TEXT (""), 'A', MathML_ATTR_style_},
   {TEXT ("subscriptshift"), TEXT (""), 'A', MathML_ATTR_subscriptshift},
   {TEXT ("superscriptshift"), TEXT (""), 'A', MathML_ATTR_superscriptshift},
   {TEXT ("symmetric"), TEXT (""), 'A', MathML_ATTR_symmetric},
   {TEXT ("width"), TEXT (""), 'A', MathML_ATTR_width_},

    {TEXT (""), TEXT (""), EOS, 0}		/* Last entry. Mandatory */
};

/* mapping table of attribute values */

static AttrValueMapping MathMLAttrValueMappingTable[] =
{
   {MathML_ATTR_accent, TEXT ("true"), MathML_ATTR_accent_VAL_true},
   {MathML_ATTR_accent, TEXT ("false"), MathML_ATTR_accent_VAL_false},
   {MathML_ATTR_accentunder, TEXT ("true"), MathML_ATTR_accentunder_VAL_true},
   {MathML_ATTR_accentunder, TEXT ("false"), MathML_ATTR_accentunder_VAL_false},
   {MathML_ATTR_displaystyle, TEXT ("true"), MathML_ATTR_displaystyle_VAL_true},
   {MathML_ATTR_displaystyle, TEXT ("false"), MathML_ATTR_displaystyle_VAL_false},
   {MathML_ATTR_edge, TEXT ("left"), MathML_ATTR_edge_VAL_left_},
   {MathML_ATTR_edge, TEXT ("right"), MathML_ATTR_edge_VAL_right_},
   {MathML_ATTR_fence, TEXT ("true"), MathML_ATTR_fence_VAL_true},
   {MathML_ATTR_fence, TEXT ("false"), MathML_ATTR_fence_VAL_false},
   {MathML_ATTR_fontstyle, TEXT ("italic"), MathML_ATTR_fontstyle_VAL_italic},
   {MathML_ATTR_fontstyle, TEXT ("normal"), MathML_ATTR_fontstyle_VAL_normal_},
   /*******  some missing attributes here: fontweight, form, frame, etc...*/
   {MathML_ATTR_link, TEXT ("document"), MathML_ATTR_link_VAL_document},
   {MathML_ATTR_link, TEXT ("extended"), MathML_ATTR_link_VAL_extended},
   {MathML_ATTR_link, TEXT ("group"), MathML_ATTR_link_VAL_group},
   {MathML_ATTR_link, TEXT ("locator"), MathML_ATTR_link_VAL_locator},
   {MathML_ATTR_link, TEXT ("simple"), MathML_ATTR_link_VAL_simple},

   {0, TEXT (""), 0}			/* Last entry. Mandatory */
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
void          MapMathMLAttribute (STRING Attr, AttributeType *attrType, STRING elementName, Document doc)
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
   int		 len;
#define MAX_ENTITY_LENGTH 80
   CHAR_T	 buffer[MAX_ENTITY_LENGTH];

   if (lang < 0)
     /* unknown entity */
     {
       entityValue[0] = '?';
       entityValue[1] = EOS;
       lang = TtaGetLanguageIdFromAlphabet('L');
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
   ElementType	parentType, elType;
   Element	parent, new;
   int		len;
   Language	lang;
   CHAR_T		alphabet;
   CHAR_T		text[4];

   len = TtaGetTextLength (*el);
   if (len == 1)
      {
      len = 2;
      TtaGiveTextContent (*el, text, &len, &lang);
      alphabet = TtaGetAlphabet (lang);
      parent = TtaGetParent (*el);
      if (text[0] != EOS)
	  {
	    parentType = TtaGetElementType (parent);
	    elType = parentType;
	    if (parentType.ElTypeNum == MathML_EL_MF &&
		(text[0] == '(' ||
		 text[0] == ')' ||
		 text[0] == '[' ||
		 text[0] == ']' ||
		 text[0] == '{' ||
		 text[0] == '}'))
	       /* Transform the text element into a Thot SYMBOL */
	       elType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
	    else if (parentType.ElTypeNum == MathML_EL_MF &&
		     text[0] == '|')
	       /* Transform the text element into a Thot GRAPHIC */
	       {
	       elType.ElTypeNum = MathML_EL_GRAPHICS_UNIT;
	       text[0] = 'v';
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
	       TtaSetGraphicsShape (new, text[0], doc);
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
  if (elType.ElTypeNum == MathML_EL_MROW ||
      elType.ElTypeNum == MathML_EL_MF ||
      elType.ElTypeNum == MathML_EL_MFENCED ||
      elType.ElTypeNum == MathML_EL_MACTION ||
      elType.ElTypeNum == MathML_EL_MROOT ||
      elType.ElTypeNum == MathML_EL_MSQRT ||
      elType.ElTypeNum == MathML_EL_MFRAC ||
      elType.ElTypeNum == MathML_EL_MSUBSUP ||
      elType.ElTypeNum == MathML_EL_MSUB ||
      elType.ElTypeNum == MathML_EL_MSUP ||
      elType.ElTypeNum == MathML_EL_MUNDER ||
      elType.ElTypeNum == MathML_EL_MOVER ||
      elType.ElTypeNum == MathML_EL_MUNDEROVER ||
      elType.ElTypeNum == MathML_EL_MMULTISCRIPTS ||
      elType.ElTypeNum == MathML_EL_MTABLE)
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
  int		len;
  Language	lang;
  CHAR_T		alphabet;
  UCHAR_T	        text[2], c;

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
  int		len;
  Language	lang;
  CHAR_T		alphabet;
  UCHAR_T	        text[2], c;

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
	      pair = TtaNewTree (doc, elTypePair, _EMPTYSTR_);
	      TtaInsertFirstChild (&pair, group, doc);
	      SetIntPlaceholderAttr (pair, doc);
	      prevPair = pair;
	      pair = TtaNewTree (doc, elTypePair, _EMPTYSTR_);
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
		  group = TtaNewTree (doc, elTypeGroup, _EMPTYSTR_);
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
	      pair = TtaNewTree (doc, elTypePair, _EMPTYSTR_);
	      TtaInsertFirstChild (&pair, group, doc);
	      SetIntPlaceholderAttr (pair, doc);
	      prevPair = pair;
	      pair = TtaNewTree (doc, elTypePair, _EMPTYSTR_);
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
      elem = TtaNewTree (doc, elTypeGroup, _EMPTYSTR_);
      if (group == NULL)
	group = base;
      TtaInsertSibling (elem, group, TRUE, doc);
      SetIntPlaceholderAttr (elem, doc);
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
		nextCell, newMTD, firstColHead, child, prevChild, nextChild,
		wrapper;
  SSchema	MathMLSSchema;

  MathMLSSchema = GetMathMLSSchema (doc);
  row = TtaGetFirstChild (elMTABLE);

  /* create a MTable_head as the first child of element MTABLE */
  elType.ElSSchema = MathMLSSchema;
  elType.ElTypeNum = MathML_EL_MTable_head;
  MTableHead = TtaNewElement (doc, elType);
  TtaInsertFirstChild (&MTableHead, elMTABLE, doc);
  elType.ElTypeNum = MathML_EL_MColumn_head;
  firstColHead = TtaNewTree (doc, elType, _EMPTYSTR_);
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
	 {
	 child = TtaGetFirstChild (cell);
	 elType.ElSSchema = MathMLSSchema;
	 elType.ElTypeNum = MathML_EL_CellWrapper;
	 wrapper = TtaNewElement (doc, elType);
	 TtaInsertFirstChild (&wrapper, cell, doc);
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
      cell = nextCell;
      }
    row = nextRow;
    }
  CheckAllRows (elMTABLE, doc);
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
  int		len;

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
	      if it exists */
	   {
	   if (IntAttr != NULL)
	      TtaRemoveAttribute (el, IntAttr, doc);
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
   ElementType	elType;
   Element	content;
   AttributeType attrType;
   Attribute	attr;
   int		len;
   Language	lang;
   CHAR_T		alphabet;
   UCHAR_T	text[2], c;

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
		   if (text[0] == '(' || text[0] == ')' ||
		       text[0] == '[' || text[0] == ']' ||
		       text[0] == '{' || text[0] == '}' ||
		       text[0] == '|' )
		      {
		      /* remove the content of the MO element */
		      TtaDeleteTree (content, doc);
		      /* change the MO element into a MF element */
		      ChangeTypeOfElement (el, doc, MathML_EL_MF);
		      /* attach a IntVertStretch attribute to the MF element */
		      attrType.AttrSSchema = elType.ElSSchema;
		      attrType.AttrTypeNum = MathML_ATTR_IntVertStretch;
		      attr = TtaNewAttribute (attrType);
		      TtaAttachAttribute (el, attr, doc);
		      TtaSetAttributeValue (attr, MathML_ATTR_IntVertStretch_VAL_yes_, el, doc);
		      /* create a new content for the MF element */
		      if (text[0] == '|')
			 {
			 elType.ElTypeNum = MathML_EL_GRAPHICS_UNIT;
			 c = 'v';
			 }
		      else
			 {
		         elType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
		         c = text[0];
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
   CHAR_T		 text[32], sepValue[4];

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
void      TransformMFENCED (Element el, Document doc)
#else
void      TransformMFENCED (el, doc)
Element		el;
Document	doc;

#endif
{
   ElementType	 elType;
   Element	 child, fencedExpression, leaf, fence, next, prev,
		 firstChild;
   AttributeType attrType;
   Attribute     attr;
   int		 length;
   CHAR_T		 text[32], c;

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
	c = text[0];
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
	c = text[0];
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
       case MathML_EL_MROOT:
	  /* end of a Root. Create a RootBase and an Index */
	  CheckMathSubExpressions (el, MathML_EL_RootBase, MathML_EL_Index,
				   0, doc);
	  break;
       case MathML_EL_MSQRT:
	  /* end od a Square Root. Create a RootBase */
	  CheckMathSubExpressions (el, MathML_EL_RootBase, 0, 0, doc);
	  break;
       case MathML_EL_MFRAC:
	  /* end of a fraction. Create a Numerator and a Denominator */
	  CheckMathSubExpressions (el, MathML_EL_Numerator,
				 MathML_EL_Denominator, 0, doc);
	  break;
       case MathML_EL_MFENCED:
	  TransformMFENCED (el, doc);
	  break;
       case MathML_EL_MSUBSUP:
	  /* end of a MSUBSUP. Create Base, Subscript, and Superscript */
	  CheckMathSubExpressions (el, MathML_EL_Base, MathML_EL_Subscript,
				   MathML_EL_Superscript, doc);
	  SetIntVertStretchAttr (el, doc, MathML_EL_Base, NULL);
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
       case MathML_EL_MUNDEROVER:
	  /* end of a MUNDEROVER. Create UnderOverBase, Underscript, and
	     Overscript */
	  CheckMathSubExpressions (el, MathML_EL_UnderOverBase,
			  MathML_EL_Underscript, MathML_EL_Overscript, doc);
	  SetIntHorizStretchAttr (el, doc);
	  SetIntVertStretchAttr (el, doc, MathML_EL_UnderOverBase, NULL);
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
#define buflen 50
   STRING            value;
   int               length;
 
   TtaGiveAttributeType (attr, &attrType, &attrKind);
   if (attrType.AttrTypeNum == MathML_ATTR_color ||
       attrType.AttrTypeNum == MathML_ATTR_background_ ||
       attrType.AttrTypeNum == MathML_ATTR_fontsize ||
       attrType.AttrTypeNum == MathML_ATTR_fontfamily)
      {
      length = TtaGetTextAttributeLength (attr);
      if (length >= buflen)
         length = buflen - 1;
      if (length > 0)
	 {
	   value = (STRING) TtaGetMemory (sizeof (CHAR_T) * buflen);
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
   ustrcpy (DTDname, _EMPTYSTR_);
}

/* end of module */
