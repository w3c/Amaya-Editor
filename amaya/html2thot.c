/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * html2thot parses a HTML file and builds the corresponding abstract tree
 * for a Thot document of type HTML.
 *
 * Author: V. Quint
 *         I. Vatton (W3C/INRIA): XML extension and Unicode
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "fetchHTMLname.h"
#include "parser.h"
#include "zlib.h"

#include "AHTURLTools_f.h"
#include "css_f.h"
#include "EDITstyle_f.h"
#include "fetchHTMLname_f.h"
#include "fetchXMLname_f.h"
#include "html2thot_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLimage_f.h"
#include "HTMLtable_f.h"
#include "HTMLsave_f.h"
#include "init_f.h"
#include "styleparser_f.h"
#include "UIcss_f.h"
#include "XHTMLbuilder_f.h"
#include "Xml2thot_f.h"

#ifdef ANNOTATIONS
#include "annotlib.h"
#include "ANNOTtools_f.h"
#endif /* ANNOTATIONS */

/* tables defined in XHTMLbuilder.c */
extern AttrValueMapping XhtmlAttrValueMappingTable[]; 
extern XmlEntity      XhtmlEntityTable[];

typedef struct _UnicodeFallbackEntry
{
  int	unicodeVal;	/* the Unicode code */
  int	EightbitCode;   /* the corresponding glyph to be used from
                         the ISO Latin-1 or Symbol character set.
                         if 0 < EightbitCode < 255, it's the Symbol code for the correct glyph
                         if 1000 < EightbitCode < 1256, it's the ISO Latin-1 code + 1000 of an
                         approaching glyph
                         if 2000 < EightbitCode < 2256, it's the Symbol code + 2000 of an
                         approaching glyph */
}
UnicodeFallbackEntry;

UnicodeFallbackEntry	UnicodeFallbackTable[] =
  {
    /* This table MUST be ordered according to the first field of each
       entry (Unicode code) */

    /* OElig    */ {338, 1079}, /* latin capital ligature OE, U+0152 ISOlat2 */
    /* oelig    */ {339, 1111}, /* latin small ligature oe, U+0153 ISOlat2 */
    /* Scaron   */ {352, 1083}, /* latin capital letter S with caron, U+0160 ISOlat2 */
    /* scaron   */ {353, 1115}, /* latin small letter s with caron, U+0161 ISOlat2 */
    /* Yuml     */ {376, 1089}, /* latin capital letter Y with diaeresis, U+0178 ISOlat2 */
    /* fnof     */ {402, 166},  /* latin small f with hook = function = florin, U+0192 ISOtech */
    /* epsilon  */ {603, 101}, /* greek small letter epsilon, U+03B5 ISOgrk3 */
    /* circ     */ {710, 2217}, /* modifier letter circumflex accent, U+02C6 ISOpub */
    /* tilde    */ {732, 1126}, /* small tilde, U+02DC ISOdia */
    /* hat      */ {770, 1094}, /* small tilde, U+02DC ISOdia */
    /* UnderBar */ {818, 45}, /* U+0332 */
    /* Alpha    */ {913, 65}, /* greek capital letter alpha, U+0391 */
    /* Beta     */ {914, 66}, /* greek capital letter beta, U+0392 */
    /* Gamma    */ {915, 71}, /* greek capital letter gamma, U+0393 ISOgrk3 */
    /* Delta    */ {916, 68}, /* greek capital letter delta, U+0394 ISOgrk3 */
    /* Epsilon  */ {917, 69}, /* greek capital letter epsilon, U+0395 */
    /* Zeta     */ {918, 90}, /* greek capital letter zeta, U+0396 */
    /* Eta      */ {919, 72}, /* greek capital letter eta, U+0397 */
    /* Theta    */ {920, 81}, /* greek capital letter theta, U+0398 ISOgrk3 */
    /* Iota     */ {921, 73}, /* greek capital letter iota, U+0399 */
    /* Kappa    */ {922, 75}, /* greek capital letter kappa, U+039A */
    /* Lambda   */ {923, 76}, /* greek capital letter lambda, U+039B ISOgrk3 */
    /* Mu       */ {924, 77}, /* greek capital letter mu, U+039C */
    /* Nu       */ {925, 78}, /* greek capital letter nu, U+039D */
    /* Xi       */ {926, 88}, /* greek capital letter xi, U+039E ISOgrk3 */
    /* Omicron  */ {927, 79}, /* greek capital letter omicron, U+039F */
    /* Pi       */ {928, 80}, /* greek capital letter pi, U+03A0 ISOgrk3 */
    /* Rho      */ {929, 82}, /* greek capital letter rho, U+03A1 */
    /* Sigma    */ {931, 83}, /* greek capital letter sigma, U+03A3 ISOgrk3 */
    /* Tau      */ {932, 84}, /* greek capital letter tau, U+03A4 */
    /* Upsilon  */ {933, 85}, /* greek capital letter upsilon, U+03A5 ISOgrk3 */
    /* Phi      */ {934, 70}, /* greek capital letter phi, U+03A6 ISOgrk3 */
    /* Chi      */ {935, 67}, /* greek capital letter chi, U+03A7 */
    /* Psi      */ {936, 89}, /* greek capital letter psi, U+03A8 ISOgrk3 */
    /* Omega    */ {937, 87}, /* greek capital letter omega, U+03A9 ISOgrk3 */
    /* alpha    */ {945, 97}, /* greek small letter alpha, U+03B1 ISOgrk3 */
    /* beta     */ {946, 98}, /* greek small letter beta, U+03B2 ISOgrk3 */
    /* gamma    */ {947, 103}, /* greek small letter gamma, U+03B3 ISOgrk3 */
    /* delta    */ {948, 100}, /* greek small letter delta, U+03B4 ISOgrk3 */
    /* epsilon  */ {949, 101}, /* greek small letter epsilon, U+03B5 ISOgrk3 */
    /* zeta     */ {950, 122}, /* greek small letter zeta, U+03B6 ISOgrk3 */
    /* eta      */ {951, 104}, /* greek small letter eta, U+03B7 ISOgrk3 */
    /* theta    */ {952, 113}, /* greek small letter theta, U+03B8 ISOgrk3 */
    /* iota     */ {953, 105}, /* greek small letter iota, U+03B9 ISOgrk3 */
    /* kappa    */ {954, 107}, /* greek small letter kappa, U+03BA ISOgrk3 */
    /* lambda   */ {955, 108}, /* greek small letter lambda, U+03BB ISOgrk3 */
    /* mu       */ {956, 109}, /* greek small letter mu, U+03BC ISOgrk3 */
    /* nu       */ {957, 110}, /* greek small letter nu, U+03BD ISOgrk3 */
    /* xi       */ {958, 120}, /* greek small letter xi, U+03BE ISOgrk3 */
    /* omicron  */ {959, 111}, /* greek small letter omicron, U+03BF NEW */
    /* pi       */ {960, 112}, /* greek small letter pi, U+03C0 ISOgrk3 */
    /* rho      */ {961, 114}, /* greek small letter rho, U+03C1 ISOgrk3 */
    /* sigmaf   */ {962, 86},  /* greek small letter final sigma, U+03C2 ISOgrk3 */
    /* sigma    */ {963, 115}, /* greek small letter sigma, U+03C3 ISOgrk3 */
    /* tau      */ {964, 116}, /* greek small letter tau, U+03C4 ISOgrk3 */
    /* upsilon  */ {965, 117}, /* greek small letter upsilon, U+03C5 ISOgrk3 */
    /* phi      */ {966, 106}, /* greek small letter phi, U+03C6 ISOgrk3 */
    /* chi      */ {967, 99},  /* greek small letter chi, U+03C7 ISOgrk3 */
    /* psi      */ {968, 121}, /* greek small letter psi, U+03C8 ISOgrk3 */
    /* omega    */ {969, 119}, /* greek small letter omega, U+03C9 ISOgrk3 */
    /* thetasym */ {977, 74},  /* greek small letter theta symbol, U+03D1 NEW */
    /* upsih    */ {978, 161}, /* greek upsilon with hook symbol, U+03D2 NEW */
    /* phiv     */ {981, 102}, /* greek U+03D5 ISOgrk3 */
    /* piv      */ {982, 118}, /* greek pi symbol, U+03D6 ISOgrk3 */
    /* ensp     */ {8194, 1130}, /* en space, U+2002 ISOpub */
    /* emsp     */ {8195, 1160}, /* em space, U+2003 ISOpub */
    /* thinsp   */ {8201, 1129}, /* thin space, U+2009 ISOpub */
    /* zwnj     */ {8204, 1063}, /* zero width non-joiner, U+200C NEW RFC 2070 */
    /* zwj      */ {8205, 1063}, /* zero width joiner, U+200D NEW RFC 2070 */
    /* lrm      */ {8206, 1063}, /* left-to-right mark, U+200E NEW RFC 2070 */
    /* rlm      */ {8207, 1063}, /* right-to-left mark, U+200F NEW RFC 2070 */
    /* ndash    */ {8211, 2045}, /* en dash, U+2013 ISOpub */
    /* mdash    */ {8212, 2190}, /* em dash, U+2014 ISOpub */
    /* horbar   */ {8213, 190},  /* U+2015 */
    /* Verbar   */ {8214, 189},  /* U+2016 */
    /* lsquo    */ {8216, 1096}, /* left single quotation mark, U+2018 ISOnum */
    /* rsquo    */ {8217, 1039}, /* right single quotation mark, U+2019 ISOnum */
    /* sbquo    */ {8218, 1044}, /* single low-9 quotation mark, U+201A NEW */
    /* ldquo    */ {8220, 1096}, /* left double quotation mark, U+201C ISOnum */
    /* rdquo    */ {8221, 1039}, /* right double quotation mark, U+201D ISOnum */
    /* bdquo    */ {8222, 1044}, /* double low-9 quotation mark, U+201E NEW */
    /* dagger   */ {8224, 2042}, /* dagger, U+2020 ISOpub */
    /* Dagger   */ {8225, 2042}, /* double dagger, U+2021 ISOpub */
    /* bull     */ {8226, 183},  /* bullet = black small circle,  U+2022 ISOpub  */
    /* hellip   */ {8230, 188},  /* horizontal ellipsis = three dot leader,  U+2026 ISOpub  */
    /* lre      */ {8234, 1063}, /* left-to-right embed, U+202A NEW RFC 2070 */
    /* rle      */ {8235, 1063}, /* right-to-left embed, U+202B NEW RFC 2070 */
    /* pdf      */ {8236, 1063}, /* pop directional format, U+202C NEW RFC 2070 */
    /* lro      */ {8237, 1063}, /* left-to-right override, U+202D NEW RFC 2070 */
    /* rlo      */ {8238, 1063}, /* right-to-left override, U+202E NEW RFC 2070 */
    /* permil   */ {8240, 2037}, /* per mille sign, U+2030 ISOtech */
    /* prime    */ {8242, 162},  /* prime = minutes = feet, U+2032 ISOtech */
    /* Prime    */ {8243, 178},  /* double prime = seconds = inches,  U+2033 ISOtech */
    /* lsaquo   */ {8249, 1060}, /* single left-pointing angle quotation mark, U+2039 ISO proposed */
    /* rsaquo   */ {8250, 1062}, /* single right-pointing angle quotation mark, U+203A ISO proposed */
    /* oline    */ {8254, 1175}, /* overline = spacing overscore,  U+203E NEW */
    /* frasl    */ {8260, 164},  /* fraction slash, U+2044 NEW */
    /*ApplyFunction*/ {8289, 1129}, /* thin space, U+2009 ISOpub */
    /*InvisibleTimes*/ {8290, 1129}, /* thin space, U+2009 ISOpub */
    /*InvisibleComa*/ {8291, 1129}, /* thin space, U+2009 ISOpub */
    /* euro     */ {8364, 2206}, /* euro sign, U+20AC NEW */
    /*TripleDot */ {8411, 188},  /* tdot, U+20DB ISOtech */
    /* image    */ {8465, 193},  /* blackletter capital I = imaginary part,  U+2111 ISOamso */
    /* copysf   */ {8471, 211}, /* U+2117 */
    /* weierp   */ {8472, 195}, /* script capital P = power set  = Weierstrass p, U+2118 ISOamso */
    /* real     */ {8476, 194}, /* blackletter capital R = real part symbol,  U+211C ISOamso */
    /* trade    */ {8482, 212}, /* trade mark sign, U+2122 ISOnum */
    /* alefsym  */ {8501, 192}, /* alef symbol = first transfinite cardinal,  U+2135 NEW */
    /*DifferentialD*/{8518, 1100}, /* U+2146 */
    /*ExponentialE*/{8519, 1101},/* */
    /*ImaginaryI*/ {8520, 1105},/* */
    /* larr     */ {8592, 172}, /* leftwards arrow, U+2190 ISOnum */
    /* uarr     */ {8593, 173}, /* upwards arrow, U+2191 ISOnum*/
    /* rarr     */ {8594, 174}, /* rightwards arrow, U+2192 ISOnum */
    /* darr     */ {8595, 175}, /* downwards arrow, U+2193 ISOnum */
    /* harr     */ {8596, 171}, /* left right arrow, U+2194 ISOamsa */
    /* crarr    */ {8629, 191}, /* downwards arrow with corner leftwards  = carriage return, U+21B5 NEW */
    /* lrarr    */ {8646, 171}, /* U+21C6 */
    /* lArr     */ {8656, 220}, /* leftwards double arrow, U+21D0 ISOtech */
    /* uArr     */ {8657, 221}, /* upwards double arrow, U+21D1 ISOamsa */
    /* rArr     */ {8658, 222}, /* rightwards double arrow,  U+21D2 ISOtech */
    /* dArr     */ {8659, 223}, /* downwards double arrow, U+21D3 ISOamsa */
    /* hArr     */ {8660, 219}, /* left right double arrow,  U+21D4 ISOamsa */
    /* forall   */ {8704, 34},  /* for all, U+2200 ISOtech */
    /* part     */ {8706, 182}, /* partial differential, U+2202 ISOtech  */
    /* exist    */ {8707, 36},  /* there exists, U+2203 ISOtech */
    /* empty    */ {8709, 198}, /* empty set = null set = diameter,  U+2205 ISOamso */
    /* nabla    */ {8711, 209}, /* nabla = backward difference,  U+2207 ISOtech */
    /* isin     */ {8712, 206}, /* element of, U+2208 ISOtech */
    /* notin    */ {8713, 207}, /* not an element of, U+2209 ISOtech */
    /* ni       */ {8715, 39},  /* contains as member, U+220B ISOtech */
    /* prod     */ {8719, 213}, /* n-ary product = product sign,  U+220F ISOamsb */
    /* sum      */ {8721, 229}, /* n-ary sumation, U+2211 ISOamsb */
    /* minus    */ {8722, 45},  /* minus sign, U+2212 ISOtech */
    /* Backslash*/ {8726, 1092},/* U+8726 */
    /* lowast   */ {8727, 42},  /* asterisk operator, U+2217 ISOtech */
    /* radic    */ {8730, 214}, /* square root = radical sign,  U+221A ISOtech */
    /* prop     */ {8733, 181}, /* proportional to, U+221D ISOtech */
    /* infin    */ {8734, 165}, /* infinity, U+221E ISOtech */
    /* ang      */ {8736, 208}, /* angle, U+2220 ISOamso */
    /* VerticalBar*/ {8739, 1124}, /*  */
    /* parallel */ {8741, 1124}, /* parallel, U+2225 ISOtech */
    /* and      */ {8743, 217}, /* logical and = wedge, U+2227 ISOtech */
    /* or       */ {8744, 218}, /* logical or = vee, U+2228 ISOtech */
    /* cap      */ {8745, 199}, /* intersection = cap, U+2229 ISOtech */
    /* cup      */ {8746, 200}, /* union = cup, U+222A ISOtech */
    /* int      */ {8747, 242}, /* integral, U+222B ISOtech */
    /* there4   */ {8756, 92},  /* therefore, U+2234 ISOtech */
    /* Colon    */ {8759, 58},  /* Colon, U+2237 */
    /* sim      */ {8764, 126}, /* tilde operator = varies with = similar to,  U+223C ISOtech */
    /*EqualTilde*/ {8770, 64},  /* U+2242 ISOamsr */
    /* cong     */ {8773, 64},  /* approximately equal to, U+2245 ISOtech */
    /* asymp    */ {8776, 187}, /* almost equal to = asymptotic to,  U+2248 ISOamsr */
    /* ne       */ {8800, 185}, /* not equal to, U+2260 ISOtech */
    /* equiv    */ {8801, 186}, /* identical to, U+2261 ISOtech */
    /* le       */ {8804, 163}, /* less-than or equal to, U+2264 ISOtech */
    /* ge       */ {8805, 179}, /* greater-than or equal to,  U+2265 ISOtech */
    /* sub      */ {8834, 204}, /* subset of, U+2282 ISOtech */
    /* sup      */ {8835, 201}, /* superset of, U+2283 ISOtech */
    /* nsub     */ {8836, 203}, /* not a subset of, U+2284 ISOamsn */
    /* sube     */ {8838, 205}, /* subset of or equal to, U+2286 ISOtech */
    /* supe     */ {8839, 202}, /* superset of or equal to,  U+2287 ISOtech */
    /* subne    */ {8842, 203}, /* U+228A */
    /* oplus    */ {8853, 197}, /* circled plus = direct sum,  U+2295 ISOamsb */
    /* otimes   */ {8855, 196}, /* circled times = vector product,  U+2297 ISOamsb */
    /* DownTee  */ {8868, 94},  /* U+22A4 ISOtech */
    /* perp     */ {8869, 94},  /* up tack = orthogonal to = perpendicular,  U+22A5 ISOtech */
    /* Vee      */ {8897, 218},  /* U+22C1 ISOamsb */
    /*Intersection*/ {8898, 199},  /* U+22C2 ISOamsb */
    /*Intersection*/ {8899, 200},  /* U+22C3 ISOamsb */
    /* Diamond  */ {8900, 168}, /* diamond operator, U+22C4 ISOamsb */
    /* sdot     */ {8901, 215}, /* dot operator, U+22C5 ISOamsb */
    /* star     */ {8902, 1042},/* */
    /* Subset   */ {8912, 204}, /* U+22D0 */
    /* Cap      */ {8914, 199}, /* U+22D2 */
    /* Cup      */ {8915, 199}, /* U+22D3 */
    /* lceil    */ {8968, 233}, /* left ceiling = apl upstile,  U+2308 ISOamsc  */
    /* rceil    */ {8969, 249}, /* right ceiling, U+2309 ISOamsc  */
    /* lfloor   */ {8970, 235}, /* left floor = apl downstile,  U+230A ISOamsc  */
    /* rfloor   */ {8971, 251}, /* right floor, U+230B ISOamsc  */
    /* lang     */ {9001, 225}, /* left-pointing angle bracket = bra,  U+2329 ISOtech */
    /* rang     */ {9002, 241}, /* right-pointing angle bracket = ket,  U+232A ISOtech */
    /* dtri     */ {9663, 209}, /* lozenge, U+25BF */
    /* loz      */ {9674, 224}, /* lozenge, U+25CA ISOpub */
    /* spades   */ {9824, 170}, /* black spade suit, U+2660 ISOpub */
    /* clubs    */ {9827, 167}, /* black club suit = shamrock,  U+2663 ISOpub */
    /* hearts   */ {9829, 169}, /* black heart suit = valentine,  U+2665 ISOpub */
    /* diams    */ {9830, 168}, /* black diamond suit, U+2666 ISOpub */
    /* lang     */ {0x27E8, 225},/* U+27E8 Mathematical left angle bracket */
    /* rang     */ {0x27E9, 241},/* U+27E9 Mathematical right angle bracket */
    /* And      */ {10835, 217}, /* U+2A53 */
    /* Or       */ {10836, 218}, /* U+2A54 */
    /* Equal    */ {10869, 1061},/*  */
    /* Not      */ {10988, 216}, /* U+2AEC */
    /* OverBrace*/ {65079, 132}, /* U+FE37 */
    /*UnderBrace*/ {65080, 133}, /* U+FE38 */

    /* THE END  */ {0, 0}	    /* last entry (required) */
  };

typedef struct _ElemToBeChecked *PtrElemToBeChecked;
typedef struct _ElemToBeChecked
{
  Element               Elem;	/* the element to be checked */
  PtrElemToBeChecked    nextElemToBeChecked;
}
ElemToBeChecked;

/* empty elements */
static int          EmptyElement[] =
  {
    HTML_EL_AREA,
    HTML_EL_BASE,
    HTML_EL_BaseFont,
    HTML_EL_BR,
    HTML_EL_COL,
    HTML_EL_FRAME,
    HTML_EL_Horizontal_Rule,
    HTML_EL_IMG,
    HTML_EL_Input,
    HTML_EL_ISINDEX,
    HTML_EL_LINK,
    HTML_EL_META,
    HTML_EL_Parameter,
    HTML_EL_PICTURE_UNIT,
    0};

/* character level elements */
static int          CharLevelElement[] =
  {
    HTML_EL_TEXT_UNIT, HTML_EL_PICTURE_UNIT, HTML_EL_SYMBOL_UNIT,
    HTML_EL_Anchor,
    HTML_EL_Teletype_text, HTML_EL_Italic_text, HTML_EL_Bold_text,
    HTML_EL_Underlined_text, HTML_EL_Struck_text, HTML_EL_Big_text,
    HTML_EL_Small_text,
    HTML_EL_Emphasis, HTML_EL_Strong, HTML_EL_Def, HTML_EL_Code, HTML_EL_Sample,
    HTML_EL_Keyboard, HTML_EL_Variable_, HTML_EL_Cite, HTML_EL_ABBR,
    HTML_EL_ACRONYM,
    HTML_EL_Font_, HTML_EL_Quotation, HTML_EL_Subscript, HTML_EL_Superscript,
    HTML_EL_Span, HTML_EL_BDO, HTML_EL_ins, HTML_EL_del,
    HTML_EL_IMG, HTML_EL_Input,
    HTML_EL_Option, HTML_EL_OptGroup, HTML_EL_Option_Menu,
    HTML_EL_Text_Input, HTML_EL_Password_Input, HTML_EL_File_Input,
    HTML_EL_Checkbox_Input, HTML_EL_Radio_Input, HTML_EL_Submit_Input,
    HTML_EL_Reset_Input, HTML_EL_Hidden_Input, HTML_EL_Inserted_Text,
    HTML_EL_Button_Input, HTML_EL_BUTTON_,
    HTML_EL_LABEL,
    HTML_EL_BR, HTML_EL_ruby,
    HTML_EL_Object, HTML_EL_Basic_Elem,
    0};

/* block level elements, i.e. elements having a Line rule in the presentation
   schema fo the main view */
static int          BlockLevelElement[] =
  {
    HTML_EL_H1, HTML_EL_H2, HTML_EL_H3, HTML_EL_H4, HTML_EL_H5, HTML_EL_H6,
    HTML_EL_Paragraph, HTML_EL_Pseudo_paragraph, HTML_EL_Text_Area,
    HTML_EL_Term, HTML_EL_Address, HTML_EL_LEGEND, HTML_EL_CAPTION,
    HTML_EL_INS, HTML_EL_DEL, HTML_EL_Division,
    0};

/* start tags that imply the end of a current element */
/* any tag of each line implies the end of the current element if the type of
   that element is in the same line */
typedef char        oneLine[100];
static oneLine      EquivEndingElem[] =
  {
    "dt dd li option",
    "h1 h2 h3 h4 h5 h6",
    "address pre listing xmp",
    ""
  };
/* acording the HTML DTD, HR should be added to the 2nd line above, as it */
/* is not allowed within a H1, H2, H3, etc. But we should tolerate that case */
/* because many documents contain rules in headings... */

/* start tags that imply the end of current element */
static oneLine      StartTagEndingElem[] =
  {
    "form closes form p p* hr h1 h2 h3 h4 h5 h6 dl ul ol menu dir address pre listing xmp head",
    "head closes p p*",
    "title closes p p*",
    "body closes head style script title p p*",
    "li closes p p* h1 h2 h3 h4 h5 h6 dl address pre listing xmp head",
    "hr closes p p* head",
    "h1 closes p p* head",
    "h2 closes p p* head",
    "h3 closes p p* head",
    "h4 closes p p* head",
    "h5 closes p p* head",
    "h6 closes p p* head",
    "dir closes p p* head",
    "address closes p p* head ul",
    "pre closes p p* head ul",
    "listing closes p p* head",
    "xmp closes p p* head",
    "blockquote closes p p* head",
    "dl closes p p* dt menu dir address pre listing xmp head",
    "dt closes p p* menu dir address pre listing xmp head",
    "dd closes p p* menu dir address pre listing xmp head",
    "ul closes p p* head ol menu dir address pre listing xmp",
    "ol closes p p* head ul",
    "menu closes p p* head ul",
    "p closes p p* head h1 h2 h3 h4 h5 h6",
    "p* closes p p* head",
    "div closes p p* head",
    "noscript closes p p* head",
    "center closes font b i p p* head",
    "a closes a",
    "caption closes p p*",
    "colgroup closes caption colgroup col p p*",
    "col closes caption col p p*",
    "table closes p p* head h1 h2 h3 h4 h5 h6 pre listing xmp a",
    "th closes th td",
    "td closes th td",
    "tr closes th td tr caption col colgroup",
    "thead closes caption col colgroup",
    "tfoot closes th td tr caption col colgroup thead tbody",
    "tbody closes th td tr caption col colgroup thead tfoot tbody",
    "optgroup closes option",
    "fieldset closes legend p p* head h1 h2 h3 h4 h5 h6 pre listing xmp a",
    ""
  };


typedef int         State;	/* a state of the automaton */

extern int               HTML_ENTRIES;
static PtrClosedElement *FirstClosedElem;

/* ---------------------- static variables ---------------------- */
/* parser stack */
#define MaxStack 200		/* maximum stack height */
static int          GINumberStack[MaxStack]; /* entry of pHTMLGIMapping */
static Element      ElementStack[MaxStack];  /* element in the Thot abstract
                                                tree */
static int          ThotLevel[MaxStack];     /* level of element in the Thot
                                                tree */
static Language	    LanguageStack[MaxStack]; /* element language */
static int          StackLevel = 0;	     /* first free element on the
                                            stack */
/* information about the input file */
#define INPUT_FILE_BUFFER_SIZE 2000
#define PREV_READ_CHARS 30
static char         FileBuffer[INPUT_FILE_BUFFER_SIZE+1];
static char         PreviousRead[PREV_READ_CHARS+1];
static char        *WorkBuffer = FileBuffer;
static int	        LastCharInWorkBuffer = 0; /* last char. in the buffer */
static int	        LastCharInPreviousRead = 0;
static int          CurrentBufChar = 0;           /* current character read */
static int          StartOfTagIndx = 0;           /* last "<" read */
static int          StartOfRead = 0;
static char	        PreviousBufChar = EOS;    /* previous character read */
static char        *InputText = NULL;
static gzFile       stream = 0;
static int          NumberOfLinesRead = 0;/* number of lines read in the
                                             file */
static int          NumberOfCharRead = 0; /* number of characters read in the
                                             current line */
static ThotBool     EmptyLine = TRUE;	  /* no printable character encountered
                                           yet in the current line */
static ThotBool     StartOfFile = TRUE;	  /* no printable character encountered
                                             yet in the file */
static ThotBool     AfterTagPRE = FALSE;  /* <PRE> has just been read */
static char*        docURL = NULL;	  /* path or URL of the document */
static char        *docURL2 = NULL;       /* save the docURL for some cases of parsing errors */

/* Static variables used for the call to the XML parser */
static ThotBool     NotToReadFile = FALSE;
static int          PreviousNumberOfLinesRead = 0;
static int          PreviousNumberOfCharRead = 0;

/* Boolean that indicates the end of a HTML file */
/* It is a static variable because it is used in parameter */
/* for the call of the new XML parser (EndOfStartGI) */
static ThotBool     EndOfHtmlFile;

/* input buffer */
#define MaxBufferLength 1000
#define AllmostFullBuffer 700
#define MaxMsgLength 300	/* maximum size of error messages */
static  unsigned char     inputBuffer[MaxBufferLength];
static int          LgBuffer = 0;	  /* actual length of text in input
                                       buffer */
static int	    BufferLineNumber = 0; /* line number in the source file of
                                         the beginning of the text
                                         contained in the buffer */

/* information about the Thot document under construction */
/* global data used by the HTML parser */
static ParserData   HTMLcontext = {0, ISO_8859_1, 0, NULL, 0,
                                   FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};

static SSchema      DocumentSSchema = NULL;  /* the HTML structure schema */
static Element      rootElement = NULL;	  /* root element of the document */
static int          lastElemEntry = 0;	  /* index in the pHTMLGIMapping of the
                                             element being created */
static Attribute    lastAttribute = NULL; /* last attribute created */
static Attribute    lastAttrElement = NULL;/* element with which the last
                                              attribute has been associated */
static AttributeMapping *lastAttrEntry = NULL; /* entry in the AttributeMappingTable
                                                  of the attribute being created */
static ThotBool     UnknownAttr = FALSE;  /* the last attribute encountered is
                                             invalid */
static ThotBool     ReadingAnAttrValue = FALSE;
static ThotBool     TruncatedAttrValue = FALSE;
static char        *BufferAttrValue = NULL;
static int          LgBufferAttrValue = 0;
static Element      CommentText = NULL;	  /* TEXT element of the current
                                             Comment element */
static Element      ASPText = NULL;	     /* TEXT element of the current
                                            ASP element */
static Element      PIText = NULL;	     /* TEXT element of the current
                                            ASP element */
static ThotBool     UnknownTag = FALSE;	  /* the last start tag encountered is
                                             invalid */
static ThotBool     HTMLrootClosed = FALSE;
static char        *HTMLrootClosingTag = NULL;

static PtrElemToBeChecked FirstElemToBeChecked = NULL;
static PtrElemToBeChecked LastElemToBeChecked = NULL;

/* automaton */
static State        currentState;	  /* current state of the automaton */
static State        returnState;	  /* return state from subautomaton */
static ThotBool     NormalTransition;
static ThotBool     CharProcessed;


/* information about an entity being read */
static char         EntityName[MaxEntityLength];/* name of entity being read */
static int          LgEntityName = 0;	  /* length of entity name read so
                                           far */
static int          EntityTableEntry = 0; /* entry of the entity table that
                                             matches the entity read so far */
static int          CharRank = 0;	  /* rank of the last matching
                                       character in that entry */
/* second char of an UTF-8 string */
static unsigned char SecondByte[6] = {EOS, EOS, EOS, EOS, EOS, EOS};
static void         ProcessStartGI (const char* GIname);
static void         EndOfAttrValue (char c);

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static const char *StrCaseStr (const char *str1, const char *str2)
{
  char        c;
  const char *ptr;
  int         len;

  if (str1 == NULL || str2 == NULL)
    return NULL;
  c = *str2;
  len = strlen ((char *)str2);
  ptr = str1;
  while (*ptr != EOS)
    {
      if (tolower(*ptr) == c && !strncasecmp ((char *)str2, ptr, len))
        return ptr;
      else
        ptr++;
    }
  return NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
char *SkipSep (char *ptr)
{
  while (*ptr == SPACE || *ptr == ',')
    ptr++;
  return (ptr);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
char *SkipInt (char *ptr)
{
  while (*ptr != EOS && *ptr != SPACE && *ptr != ',')
    ptr++;
  return (ptr);
}

/*----------------------------------------------------------------------
  ParseAreaCoords 
  Computes x, y, width and height of the box from the coords attribute value.
  ----------------------------------------------------------------------*/
void ParseAreaCoords (Element element, Document document)
{
  ElementType      elType;
  AttributeType    attrType;
  Attribute        attrCoords, attrX, attrY;
  Attribute        attrW, attrH, attrShape;
  char            *ptr3, *text;
  int              x1, y1, x2, y2;
  int              length, shape, r;

  /* Is it an AREA element */
  elType = TtaGetElementType (element);
  if (elType.ElTypeNum != HTML_EL_AREA)
    return;

  /* Search the coords attribute */
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = HTML_ATTR_coords;
  attrCoords = TtaGetAttribute (element, attrType);
  if (attrCoords == NULL)
    return;

  /* Search the shape attribute */
  attrType.AttrTypeNum = HTML_ATTR_shape;
  attrShape = TtaGetAttribute (element, attrType);
  if (attrShape == NULL)
    /* no shape attribute. Create one with value rectangle */
    {
      attrShape = TtaNewAttribute (attrType);
      TtaAttachAttribute (element, attrShape, document);
      shape = HTML_ATTR_shape_VAL_rectangle;
      TtaSetAttributeValue (attrShape, shape, element, document);
    }
  else
    shape = TtaGetAttributeValue (attrShape);
  length = TtaGetTextAttributeLength (attrCoords);
  text = (char*)TtaGetMemory (length + 1);
  TtaGiveTextAttributeValue (attrCoords, text, &length);

  if (shape == HTML_ATTR_shape_VAL_rectangle ||
      shape == HTML_ATTR_shape_VAL_circle)
    {
      /* Search the x_coord attribute */
      attrType.AttrTypeNum = HTML_ATTR_x_coord;
      attrX = TtaGetAttribute (element, attrType);
      if (attrX == NULL)
        {
          /* create it */
          attrX = TtaNewAttribute (attrType);
          TtaAttachAttribute (element, attrX, document);
        }
      /* Search the y_coord attribute */
      attrType.AttrTypeNum = HTML_ATTR_y_coord;
      attrY = TtaGetAttribute (element, attrType);
      if (attrY == NULL)
        {
          /* create it */
          attrY = TtaNewAttribute (attrType);
          TtaAttachAttribute (element, attrY, document);
        }
      /* Search the width attribute */
      attrType.AttrTypeNum = HTML_ATTR_IntWidthPxl;
      attrW = TtaGetAttribute (element, attrType);
      if (attrW == NULL)
        {
          /* create it */
          attrW = TtaNewAttribute (attrType);
          TtaAttachAttribute (element, attrW, document);
        }
      /* Search the height attribute */
      attrType.AttrTypeNum = HTML_ATTR_IntHeightPxl;
      attrH = TtaGetAttribute (element, attrType);
      if (attrH == NULL)
        {
          /* create it */
          attrH = TtaNewAttribute (attrType);
          TtaAttachAttribute (element, attrH, document);
        }
      if (shape == HTML_ATTR_shape_VAL_rectangle)
        {
          x1 = x2 = y1 = y2 = 0;
          ptr3 = text;
          if (ptr3)
            sscanf (ptr3, "%d", &x1);
          ptr3 = SkipInt (ptr3);
          ptr3 = SkipSep (ptr3);
          if (ptr3)
            sscanf (ptr3, "%d", &y1);
          ptr3 = SkipInt (ptr3);
          ptr3 = SkipSep (ptr3);
          if (ptr3)
            sscanf (ptr3, "%d", &x2);
          ptr3 = SkipInt (ptr3);
          ptr3 = SkipSep (ptr3);
          sscanf (ptr3, "%d", &y2);
          TtaSetAttributeValue (attrX, x1, element, document);
          TtaSetAttributeValue (attrY, y1, element, document);
          TtaSetAttributeValue (attrW, x2 - x1, element, document);
          TtaSetAttributeValue (attrH, y2 - y1, element, document);
        }
      else
        {
          x1 = y1 = r = 0;
          ptr3 = text;
          if (ptr3)
            sscanf (ptr3, "%d", &x1);
          ptr3 = SkipInt (ptr3);
          ptr3 = SkipSep (ptr3);
          if (ptr3)
            sscanf (ptr3, "%d", &y1);
          ptr3 = SkipInt (ptr3);
          ptr3 = SkipSep (ptr3);
          if (ptr3)
            sscanf (ptr3, "%d", &r);
          TtaSetAttributeValue (attrX, x1 - r, element, document);
          TtaSetAttributeValue (attrY, y1 - r, element, document);
          TtaSetAttributeValue (attrW, 2 * r, element, document);
          TtaSetAttributeValue (attrH, 2 * r, element, document);
        }
    }
  else if (shape == HTML_ATTR_shape_VAL_polygon)
    {
      element = TtaGetFirstChild (element);
      length = TtaGetPolylineLength (element);
      /* remove previous points */
      while (length > 1)
        {
          TtaDeletePointInPolyline (element, length, document);
          length--;
        }
      length = 1;
      ptr3 = text;
      /* add new points */
      while (*ptr3 != EOS)
        {
          x1 = y1 = 0;
          sscanf (ptr3, "%d", &x1);
          ptr3 = SkipInt (ptr3);
          ptr3 = SkipSep (ptr3);
          if (ptr3)
            sscanf (ptr3, "%d", &y1);
          ptr3 = SkipInt (ptr3);
          ptr3 = SkipSep (ptr3);
          TtaAddPointInPolyline (element, length, UnPixel, x1, y1,document,
				 FALSE);
          length++;
        }
    }
  TtaFreeMemory (text);
}

/*----------------------------------------------------------------------
  SetLanguagInHTMLStack
  Sets the value of the language.
  ----------------------------------------------------------------------*/
void  SetLanguagInHTMLStack (Language lang)
{
  LanguageStack[StackLevel - 1] = lang;
}

/*----------------------------------------------------------------------
  IsHtmlParsingCSS 
  Returns the value of ParsingCSS boolean.
  ----------------------------------------------------------------------*/
ThotBool  IsHtmlParsingCSS ()
{
  return HTMLcontext.parsingCSS;
}

/*----------------------------------------------------------------------
  SetHtmlParsingCSS 
  Sets the value of ParsingCSS boolean.
  ----------------------------------------------------------------------*/
void  SetHtmlParsingCSS (ThotBool value)
{
  HTMLcontext.parsingCSS = value;
}

/*----------------------------------------------------------------------
  SetHtmlParsingTextArea
  Sets the value of ParsingTextArea boolean.
  ----------------------------------------------------------------------*/
void  SetHtmlParsingTextArea (ThotBool value)
{
  HTMLcontext.parsingTextArea = value;
}

/*----------------------------------------------------------------------
  SetHtmlParsingScript
  Sets the value of ParsingScript boolean.
  ----------------------------------------------------------------------*/
void  SetHtmlParsingScript (ThotBool value)
{
  HTMLcontext.parsingScript = value;
}

/*----------------------------------------------------------------------
  SetHtmlElemLineNumber
  Assigns the current line number
  ----------------------------------------------------------------------*/
void SetHtmlElemLineNumber (Element el)
{
  TtaSetElementLineNumber (el, NumberOfLinesRead);
}

/*----------------------------------------------------------------------
  IsWithinHtmlTable 
  Returns the value of WithinTable integer.
  ----------------------------------------------------------------------*/
int  IsWithinHtmlTable ()
{
  return HTMLcontext.withinTable;
}

/*----------------------------------------------------------------------
  copyCEstring    create a copy of the string of elements pointed
  by first and return a pointer on the first
  element of the copy.
  ----------------------------------------------------------------------*/
static PtrClosedElement copyCEstring (PtrClosedElement first)
{
  PtrClosedElement     ret, cur, next, prev;
 
  ret = NULL;
  cur = first;
  prev = NULL;
  while (cur != NULL)
    {
      next = (PtrClosedElement) TtaGetMemory (sizeof (ClosedElement));
      next->nextClosedElem = NULL;
      next->tagNum = cur->tagNum;
      if (ret == NULL)
        ret = next;
      else
        prev->nextClosedElem = next;
      prev = next;
      cur = cur->nextClosedElem;
    }
  return ret;
}

/*----------------------------------------------------------------------
  InitMapping     intialise the list of the elements closed by
  each start tag.
  ----------------------------------------------------------------------*/
void InitMapping (void)
{
  int                 line;
  int                 entry;
  int                 ptr;
  int                 i;
  typeName            name;
  PtrClosedElement    newCE, lastCE, firstCE, curCE;
  SSchema	       schema;

  /* building the table */
  FirstClosedElem = (PtrClosedElement *)TtaGetMemory (HTML_ENTRIES * sizeof(PtrClosedElement));
  for (entry = 0; entry < HTML_ENTRIES; entry++)
    FirstClosedElem[entry] = NULL;

  /* read table EquivEndingElem */
  line = 0;
  do
    /* read one line of EquivEndingElem */
    {
      ptr = 0;
      lastCE = NULL;
      firstCE = NULL; 
      do
        {
          /* read one identifier */
          i = 0;
          while (EquivEndingElem[line][ptr] != SPACE &&
                 EquivEndingElem[line][ptr] != EOS)
            name[i++] = EquivEndingElem[line][ptr++];
          name[i] = EOS;
          ptr++;
          if (i > 0)
            /* a identifier has been read */
            {
              schema = DocumentSSchema;
              entry = MapGI ((char *)name, &schema, HTMLcontext.doc);
#ifdef DEBUG
              if (entry < 0)
                fprintf (stderr, "error in EquivEndingElem: tag %s unknown in line\n%s\n", name, EquivEndingElem[line]);
              else
#endif
                {
                  newCE = (PtrClosedElement) TtaGetMemory (sizeof (ClosedElement));
                  newCE->nextClosedElem = NULL;
                  newCE->tagNum = entry;
                  if (firstCE == NULL)
                    firstCE = newCE;
                  else
                    lastCE->nextClosedElem = newCE;
                  lastCE = newCE;
                }
            }
        }
      while (EquivEndingElem[line][ptr] != EOS);

      /* one line has been read */
      curCE = firstCE;
      while (curCE != NULL)
        {
          if (curCE->nextClosedElem == NULL)
            newCE = firstCE;
          else
            newCE = copyCEstring (firstCE);
          if (FirstClosedElem[curCE->tagNum] == NULL)
            FirstClosedElem[curCE->tagNum] = newCE;
          else
            {
              lastCE = FirstClosedElem[curCE->tagNum];
              while (lastCE->nextClosedElem != NULL)
                lastCE = lastCE->nextClosedElem;
              lastCE->nextClosedElem = newCE;
            }
          curCE = curCE->nextClosedElem;
        }

      line++;
    }
  while (strcmp (EquivEndingElem[line], "") != 0);

  /* read table StartTagEndingElem */
  line = 0;
  do
    /* read one line of StartTagEndingElem */
    {
      ptr = 0;
      i = 0;
      /* read the first tag name of the line */
      while (StartTagEndingElem[line][ptr] != SPACE &&
             StartTagEndingElem[line][ptr] != EOS)
        name[i++] = StartTagEndingElem[line][ptr++];
      name[i] = EOS;
      i = 0;
      ptr++;
      schema = DocumentSSchema;
      entry = MapGI ((char *)name, &schema, HTMLcontext.doc);
#ifdef DEBUG
      if (entry < 0)
        fprintf (stderr, "error in StartTagEndingElem: tag %s unknown in line\n%s\n", name, StartTagEndingElem[line]);
#endif
      /* read the keyword "closes" */
      while (StartTagEndingElem[line][ptr] != SPACE &&
             StartTagEndingElem[line][ptr] != EOS)
        name[i++] = StartTagEndingElem[line][ptr++];
      name[i] = EOS;
      i = 0;
      ptr++;
#ifdef DEBUG
      if (strcmp (name, "closes") != 0)
        fprintf (stderr, "error in StartTagEndingElem: \"%s\" instead of \"closes\" in line\n%s\n", name, StartTagEndingElem[line]);
#endif
      lastCE = FirstClosedElem[entry];
      if (lastCE != NULL)
        while (lastCE->nextClosedElem != NULL)
          lastCE = lastCE->nextClosedElem;
      do
        {
          while (StartTagEndingElem[line][ptr] != SPACE &&
                 StartTagEndingElem[line][ptr] != EOS)
            name[i++] = StartTagEndingElem[line][ptr++];
          name[i] = EOS;
          ptr++;
          if (i > 0)
            {
              i = 0;
              newCE = (PtrClosedElement) TtaGetMemory (sizeof (ClosedElement));
              newCE->nextClosedElem = NULL;
              schema = DocumentSSchema;
              newCE->tagNum = MapGI ((char *)name, &schema, HTMLcontext.doc);
#ifdef DEBUG
              if (newCE->tagNum < 0)
                fprintf (stderr, "error in StartTagEndingElem: tag %s unknown in line\n%s\n", name, StartTagEndingElem[line]);
#endif
              if (lastCE == NULL)
                FirstClosedElem[entry] = newCE;
              else
                lastCE->nextClosedElem = newCE;
              lastCE = newCE;
            }
        }
      while (StartTagEndingElem[line][ptr] != EOS);
      line++;
    }
  while (strcmp (StartTagEndingElem[line], "") != 0);
}

/*----------------------------------------------------------------------
  Within  checks if an element of type ThotType is in the stack.
  ----------------------------------------------------------------------*/
static ThotBool Within (int ThotType, SSchema ThotSSchema)
{
  ThotBool         ret;
  int              i;
  ElementType      elType;

  ret = FALSE;
  i = StackLevel - 1;
  while (i >= 0 && !ret)
    {
      if (ElementStack[i] != NULL)
        {
          elType = TtaGetElementType (ElementStack[i]);
          if (elType.ElTypeNum == ThotType &&
              elType.ElSSchema == ThotSSchema)
            ret = TRUE;
        }
      i--;
    }
  return ret;
}

/*----------------------------------------------------------------------
  HTMLParseError  print the error message msg on stderr.
  If lineNumber = 0, print the current line number in the source file,
  otherwise print the line number provided.
  ----------------------------------------------------------------------*/
void HTMLParseError (Document doc, const char* msg, int lineNumber)
{
  if (IgnoreErrors)
    return;

  HTMLErrorsFound = TRUE;
  if (!ErrFile)
    if (OpenParsingErrors (doc) == FALSE)
      return;
   
  if (doc == HTMLcontext.doc)
    {
      /* the error message is related to the document being parsed */
      if (docURL != NULL)
        {
          if (!XMLErrorsFound)
            fprintf (ErrFile, "\n*** Errors/warnings in %s\n", docURL);
          TtaFreeMemory (docURL);
          docURL = NULL;
        }
      else
        {
          if (CSSErrorsFound && docURL2)
            {
              fprintf (ErrFile, "\n*** Errors/warnings in %s\n", docURL2);
              TtaFreeMemory (docURL2);
              docURL2 = NULL;
            }
        }

      if (lineNumber <= 0)
        /* print the line number and character number before the message */
        fprintf (ErrFile, "@   line %d, char %d: %s\n", NumberOfLinesRead,
                 NumberOfCharRead, msg);
      else
        fprintf (ErrFile, "@   line %d, char 0: %s\n", lineNumber, msg);
    }
  else
    /* print only the error message */
    fprintf (ErrFile, "%s\n", msg);
}

/*----------------------------------------------------------------------
  CloseBuffer     close the input buffer.
  ----------------------------------------------------------------------*/
static void CloseBuffer ()
{
  inputBuffer[LgBuffer] = EOS;
}

/*----------------------------------------------------------------------
  InitBuffer      initialize the input buffer.
  ----------------------------------------------------------------------*/
static void InitBuffer ()
{
  LgBuffer = 0;
}

static ThotBool InsertElement (Element * el);

/*----------------------------------------------------------------------
  InsertSibling   return TRUE if the new element must be inserted
  in the Thot document as a sibling of lastElement;
  return FALSE it it must be inserted as a child.
  ----------------------------------------------------------------------*/
static ThotBool InsertSibling ()
{
  if (StackLevel == 0)
    return FALSE;
  else if (HTMLcontext.lastElementClosed ||
           TtaIsLeaf (TtaGetElementType (HTMLcontext.lastElement)) ||
           (GINumberStack[StackLevel - 1] >= 0 &&
            pHTMLGIMapping[GINumberStack[StackLevel - 1]].XMLcontents == 'E'))
    return TRUE;
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  IsEmptyElement return TRUE if element el is defined as an empty element.
  ----------------------------------------------------------------------*/
static ThotBool IsEmptyElement (Element el)
{
  ElementType         elType;
  int                 i;
  ThotBool            ret;

  ret = FALSE;
  elType = TtaGetElementType (el);
  if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") != 0)
    return ret;
  i = 0;
  while (EmptyElement[i] > 0 && EmptyElement[i] != elType.ElTypeNum)
    i++;
  if (EmptyElement[i] == elType.ElTypeNum)
    ret = TRUE;
  return ret;
}

/*----------------------------------------------------------------------
  IsCharacterLevelType return TRUE if element type is a
  character level element, FALSE if not.
  ----------------------------------------------------------------------*/
ThotBool IsCharacterLevelType (ElementType elType)
{
  int              i;
  ThotBool         ret;

  ret = FALSE;
  if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") != 0)
    return ret;
  i = 0;
  while (CharLevelElement[i] > 0 &&
         CharLevelElement[i] != elType.ElTypeNum)
    i++;
  if (CharLevelElement[i] == elType.ElTypeNum)
    ret = TRUE;
  return ret;
}

/*----------------------------------------------------------------------
  IsCharacterLevelElement return TRUE if element el is a
  character level element, FALSE if not.
  ----------------------------------------------------------------------*/
ThotBool IsCharacterLevelElement (Element el)
{
  ElementType      elType;

  elType = TtaGetElementType (el);
  return IsCharacterLevelType (elType);
}

/*----------------------------------------------------------------------
  IsBlockElementType  return TRUE if element type is a block element.
  Same as IsBlockElement but just with the element type.
  ----------------------------------------------------------------------*/
ThotBool IsBlockElementType (ElementType elType)
{
  int           i;
  ThotBool      ret;

  ret = FALSE;
  if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") != 0)
    return ret;
  i = 0;
  while (BlockLevelElement[i] > 0 &&
         BlockLevelElement[i] != elType.ElTypeNum)
    i++;
  if (BlockLevelElement[i] == elType.ElTypeNum)
    ret = TRUE;
  return ret;
}

/*----------------------------------------------------------------------
  IsBlockElement  return TRUE if element el is a block element.
  ----------------------------------------------------------------------*/
ThotBool IsBlockElement (Element el)
{
  ElementType   elType;

  elType = TtaGetElementType (el);
  return IsBlockElementType (elType);
}

 
/*----------------------------------------------------------------------
  TextToDocument  Put the content of input buffer in the document.
  ----------------------------------------------------------------------*/
static void TextToDocument ()
{
  ElementType      elType;
  Element          elText, parent;
  int              i;
  ThotBool         ignoreLeadingSpaces;
  ThotBool         insSibling, ok;

  CloseBuffer ();
  if (HTMLcontext.lastElement)
    {
      i = 0;
      insSibling = InsertSibling ();
      ignoreLeadingSpaces = IsLeadingSpaceUseless (HTMLcontext.lastElement,
                                                   HTMLcontext.doc, insSibling, FALSE);
      if (ignoreLeadingSpaces &&
          !Within (HTML_EL_Preformatted, DocumentSSchema) &&
          !Within (HTML_EL_STYLE_, DocumentSSchema) &&
          !Within (HTML_EL_SCRIPT_, DocumentSSchema))
        /* suppress leading spaces */
        while (inputBuffer[i] <= SPACE && inputBuffer[i] != EOS)
          i++;
      if (inputBuffer[i] != EOS)
        {
          elType = TtaGetElementType (HTMLcontext.lastElement);
          if (elType.ElTypeNum == HTML_EL_TEXT_UNIT && HTMLcontext.mergeText)
            TtaAppendTextContent (HTMLcontext.lastElement, (unsigned char *)&(inputBuffer[i]),
                                  HTMLcontext.doc);
          else
            {
              if (inputBuffer[i] == SPACE && LgBuffer == 1)
                {
                  // avoid to generate an empty pseudo paragraph
                  ok = FALSE;
                  if (InsertSibling ())
                    parent = TtaGetParent (HTMLcontext.lastElement);
                  else
                    parent = HTMLcontext.lastElement;
                  if (parent)
                    {
                      elType = TtaGetElementType (parent);
                      if (IsCharacterLevelElement (parent) ||
                          !XhtmlCannotContainText (elType))
                        ok = TRUE; // generate the TEXT element
                    }
                }
              else
                ok = TRUE;
              if (ok)
                {
                  /* create a TEXT element */
                  elType.ElSSchema = DocumentSSchema;
                  elType.ElTypeNum = HTML_EL_TEXT_UNIT;
                  elText = TtaNewElement (HTMLcontext.doc, elType);
                  TtaSetElementLineNumber (elText, BufferLineNumber);
                  InsertElement (&elText);
                  HTMLcontext.lastElementClosed = TRUE;
                  HTMLcontext.mergeText = TRUE;
                  /* put the content of the input buffer into the TEXT element */
                  if (elText)
                    TtaSetTextContent (elText, (unsigned char *)&(inputBuffer[i]),
                                       HTMLcontext.language, HTMLcontext.doc);
                }
            }
        }
    }
  InitBuffer ();
}


/*----------------------------------------------------------------------
  StartOfTag      Beginning of a HTML tag (start or end tag).
  Put the preceding text into the Thot document.
  ----------------------------------------------------------------------*/
static void StartOfTag (char c)
{
  if (LgBuffer > 0)
    TextToDocument ();
  HTMLcontext.mergeText = FALSE;
  StartOfTagIndx = CurrentBufChar - 1;
  PreviousNumberOfCharRead = NumberOfCharRead - 1;
  /* Is there an EOL or CR character inside tag ?? */
  PreviousNumberOfLinesRead = NumberOfLinesRead;

}

/*----------------------------------------------------------------------
  PutInBuffer     put character c in the input buffer.
  ----------------------------------------------------------------------*/
static void PutInBuffer (unsigned char c)
{
  int               len;

  /* put the character into the buffer if it is not an ignored char. */
  if ((int) c == TAB)		/* HT */
    len = 8;			/* HT = 8 spaces */
  else
    len = 1;
  if (c != EOS)
    {
      if (LgBuffer + len >= AllmostFullBuffer &&
          // simplete text or cdata
          (currentState == 0 || currentState == 24))
        TextToDocument ();
      if (LgBuffer + len >= MaxBufferLength)
        {
          if (currentState == 0)
            TextToDocument ();
          else if (currentState == 6)
            {
              TruncatedAttrValue = TRUE;
              EndOfAttrValue (c);
              TruncatedAttrValue = FALSE;
            }
          else
            HTMLParseError (HTMLcontext.doc, "Buffer overflow", 0);
          LgBuffer = 0;
        }

      if (LgBuffer == 0)
        BufferLineNumber = NumberOfLinesRead;

      if (len == 1)
        inputBuffer[LgBuffer++] = c;
      else
        /* HT */
        do
          {
            inputBuffer[LgBuffer++] = SPACE;
            len--;
          }
        while (len > 0);
    }
}


/*----------------------------------------------------------------------
  BlockInCharLevelElem
  Element el is a block-level element. If its parent is a character-level
  element, add a record in the list of block-level elements to be
  checked when the document is complete.
  ----------------------------------------------------------------------*/
void BlockInCharLevelElem (Element el)
{
  PtrElemToBeChecked  nextElTBC, elTBC;
  Element             parent;
  ElementType         elType, parentType;

  parent = TtaGetParent (el);
  elType = TtaGetElementType (el);
  /* a <div> within a <button> is allowed */
  if (elType.ElTypeNum == HTML_EL_Division)
    {
      parentType = TtaGetElementType (parent);
      if (parentType.ElTypeNum == HTML_EL_BUTTON_)
	return;
    }
  if (LastElemToBeChecked != NULL)
    {
      nextElTBC = FirstElemToBeChecked;
      while (nextElTBC != NULL)
        {
          if (nextElTBC->Elem == el)
            /* this element is already in the queue */
            return;
          else
            nextElTBC = nextElTBC->nextElemToBeChecked;
        }
    }   
  if (parent != NULL)
    if (IsCharacterLevelElement (parent))
      {
        elTBC = (PtrElemToBeChecked) TtaGetMemory(sizeof(ElemToBeChecked));
        elTBC->Elem = el;
        elTBC->nextElemToBeChecked = NULL;
        if (LastElemToBeChecked == NULL)
          FirstElemToBeChecked = elTBC;
        else
          LastElemToBeChecked->nextElemToBeChecked = elTBC;
        LastElemToBeChecked = elTBC;
      }
}

/*----------------------------------------------------------------------
  CheckSurrounding

  inserts an element Pseudo_paragraph in the abstract tree of the Thot
  document if el is a leaf and is not allowed to be a child of element parent.
  Return TRUE if element *el has been inserted in the tree.
  ----------------------------------------------------------------------*/
static ThotBool CheckSurrounding (Element * el, Element parent)
{
  ElementType      parentType, newElType, elType, ancestorType;
  Element          newEl, ancestor, prev, prevprev;
  ThotBool         ret;

  if (parent == NULL)
    return(FALSE);
  ret = FALSE;
  elType = TtaGetElementType (*el);
  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT || elType.ElTypeNum == HTML_EL_BR
      || elType.ElTypeNum == HTML_EL_PICTURE_UNIT
      || elType.ElTypeNum == HTML_EL_IFRAME
      || elType.ElTypeNum == HTML_EL_IMG
      || elType.ElTypeNum == HTML_EL_Input
      || elType.ElTypeNum == HTML_EL_Text_Area
      || IsCharacterLevelElement (*el))
    {
      /* the element to be inserted is a character string */
      /* Search the ancestor that is not a character level element */
      ancestor = parent;
      while (ancestor != NULL && 
             (IsCharacterLevelElement (ancestor) ||
              !strcmp (TtaGetSSchemaName (TtaGetElementType(ancestor).ElSSchema), "Template")))
        ancestor = TtaGetParent (ancestor);
      if (ancestor != NULL)
        {
          ancestorType = TtaGetElementType (ancestor);
          if (XhtmlCannotContainText (ancestorType) &&
              !Within (HTML_EL_Option_Menu, DocumentSSchema))
            /* Element ancestor cannot contain text directly. Create a */
            /* Pseudo_paragraph element as the parent of the text element */
            {
              newElType.ElSSchema = DocumentSSchema;
              newElType.ElTypeNum = HTML_EL_Pseudo_paragraph;
              newEl = TtaNewElement (HTMLcontext.doc, newElType);
              TtaSetElementLineNumber (newEl, NumberOfLinesRead);
              /* insert the new Pseudo_paragraph element */
              InsertElement (&newEl);
              if (newEl != NULL)
                {
                  /* insert the Text element in the tree */
                  TtaInsertFirstChild (el, newEl, HTMLcontext.doc);
                  BlockInCharLevelElem (newEl);
                  ret = TRUE;

                  /* if previous siblings of the new Pseudo_paragraph element
                     are character level elements, move them within the new
                     Pseudo_paragraph element */
                  prev = newEl;
                  TtaPreviousSibling (&prev);
                  while (prev != NULL)
                    {
                      if (!IsCharacterLevelElement (prev))
                        prev = NULL;
                      else
                        {
                          prevprev = prev;  TtaPreviousSibling (&prevprev);
                          TtaRemoveTree (prev, HTMLcontext.doc);
                          TtaInsertFirstChild (&prev, newEl, HTMLcontext.doc);
                          prev = prevprev;
                        }
                    }
                }
     	      }
        }
    }
  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
      (elType.ElTypeNum != HTML_EL_Inserted_Text &&
       IsCharacterLevelElement (*el)))
    /* it is a character level element */
    {
      parentType = TtaGetElementType (parent);
      if (parentType.ElTypeNum == HTML_EL_Text_Area)
        /* A basic element cannot be a child of a Text_Area */
        /* create a Inserted_Text element as a child of Text_Area */
        {
          newElType.ElSSchema = DocumentSSchema;
          newElType.ElTypeNum = HTML_EL_Inserted_Text;
          newEl = TtaNewElement (HTMLcontext.doc, newElType);
          TtaSetElementLineNumber (newEl, NumberOfLinesRead);
          InsertElement (&newEl);
          if (newEl != NULL)
            {
              TtaInsertFirstChild (el, newEl, HTMLcontext.doc);
              ret = TRUE;
            }
        }
    }
  return ret;
}


/*----------------------------------------------------------------------
  InsertElement   inserts element el in the abstract tree of the
  Thot document, at the current position.
  ----------------------------------------------------------------------*/
static ThotBool InsertElement (Element *el)
{
  ThotBool         ret;
  Element          parent;

  if (InsertSibling ())
    {
      if (HTMLcontext.lastElement == NULL)
        parent = NULL;
      else
        parent = TtaGetParent (HTMLcontext.lastElement);
      if (!CheckSurrounding (el, parent))
        {
          if (parent != NULL)
            TtaInsertSibling (*el, HTMLcontext.lastElement, FALSE, HTMLcontext.doc);
          else
            {
              TtaDeleteTree (*el, HTMLcontext.doc);
              *el = NULL;
            }
        }
      ret = TRUE;
    }
  else
    {
      if (!CheckSurrounding (el, HTMLcontext.lastElement))
        TtaInsertFirstChild (el, HTMLcontext.lastElement, HTMLcontext.doc);
      ret = FALSE;
    }
  if (*el != NULL)
    {
      HTMLcontext.lastElement = *el;
      HTMLcontext.lastElementClosed = FALSE;
    }
  return ret;
}

/*----------------------------------------------------------------------
  ProcessOptionElement
  If multiple is FALSE, remove the SELECTED attribute from the
  option element, except if it's element el.
  If parsing is TRUE, associate a DefaultSelected attribute with
  element option if it has a SELECTED attribute.
  ----------------------------------------------------------------------*/
static void ProcessOptionElement (Element option, Element el,
                                  Document doc, ThotBool multiple,
                                  ThotBool parsing)
{
  ElementType	   elType;
  AttributeType   attrType;
  Attribute	   attr;

  elType = TtaGetElementType (option);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = HTML_ATTR_Selected;
  if (!multiple && option != el)
    {
      /* Search the SELECTED attribute */
      attr = TtaGetAttribute (option, attrType);
      /* remove it if it exists */
      if (attr)
        TtaRemoveAttribute (option, attr, doc);
    }
  if (parsing)
    {
      attr = TtaGetAttribute (option, attrType);
      if (attr != NULL)
        {
          attrType.AttrTypeNum = HTML_ATTR_DefaultSelected;
          attr = TtaGetAttribute (option, attrType);
          if (!attr)
            {
              /* create the DefaultSelected attribute */
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (option, attr, doc);
              TtaSetAttributeValue (attr, HTML_ATTR_DefaultSelected_VAL_Yes_,
                                    option, doc);
            }
        }
    }
}

/*----------------------------------------------------------------------
  OnlyOneOptionSelected
  If the option menu el is a single-choice menu, check that only
  one option has an attribute Selected.
  If there is no option element with an attribute Selected, put an
  attribute ShowMe on the first option.
  If parsing is TRUE, associate an attribute DefaultSelected with
  each option having an attribute Selected.
  ----------------------------------------------------------------------*/
void OnlyOneOptionSelected (Element el, Document doc, ThotBool parsing)
{
  ElementType      elType, opType;
  Element          option, menu, child, firstOption;
  AttributeType    attrType, attrshowMeType;
  Attribute        attr, showMeAttr;
  ThotBool         multiple;

  if (el == NULL)
    return;

  menu = NULL;
  attr = NULL;
  firstOption = NULL;
  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = HTML_ATTR_Selected;
  attrshowMeType.AttrSSchema = elType.ElSSchema;
  attrshowMeType.AttrTypeNum = HTML_ATTR_ShowMe;
  if (elType.ElTypeNum == HTML_EL_Option_Menu)
    {
      /* it's a menu (SELECT) */
      menu = el;
      /* search the first OPTION element having an attribute SELECTED */
      option = TtaGetFirstChild (el);
      while (option && !attr)
        {
          elType = TtaGetElementType (option);
          if (elType.ElTypeNum == HTML_EL_Option)
            {
              attr = TtaGetAttribute (option, attrType);
              if (!firstOption)
                firstOption = option;
            }
          else if (elType.ElTypeNum == HTML_EL_OptGroup)
            {
              child = TtaGetFirstChild (option);
              while (child && !attr)
                {
                  elType = TtaGetElementType (child);
                  if (elType.ElTypeNum == HTML_EL_Option)
                    {
                      attr = TtaGetAttribute (child, attrType);
                      if (!firstOption)
                        firstOption = child;
                    }
                  if (attr)
                    option = child;
                  else
                    TtaNextSibling (&child);
                }
            }
          if (!attr)
            TtaNextSibling (&option);
        }
      el = option;
    }
  else
    {
      option = NULL;
      do
        {
          if (elType.ElTypeNum == HTML_EL_Option_Menu)
            menu = el;
          else
            {
              if (elType.ElTypeNum == HTML_EL_Option)
                option = el;
              el = TtaGetParent (el);
              if (el)
                elType = TtaGetElementType (el);
            }
        }
      while (el && !menu);
      el = option;
    }

  if (el)
    {
      if (menu)
        {
          /* Remove the SELECTED attribute from other options in the menu */
          /* if it's not a multiple-choices menu. */
          /* When parsing the HTML file, associate a DefaultSelected */
          /* attribute with each element having a SELECTED attribute */
          attrType.AttrTypeNum = HTML_ATTR_Multiple;
          multiple = (TtaGetAttribute (menu, attrType) != NULL);
          if (parsing || !multiple)
            {
              option = TtaGetFirstChild (menu);
              opType = TtaGetElementType (menu);
              opType.ElTypeNum = HTML_EL_Option;
              while (option)
                {
                  elType = TtaGetElementType (option);
                  if (elType.ElTypeNum == HTML_EL_Option)
                    {
                      ProcessOptionElement (option, el, doc, multiple,
                                            parsing);
                      if (!firstOption)
                        firstOption = option;
                    }
                  else 
                    {
                      child = TtaSearchTypedElement (opType, SearchInTree, option);
                      while (child)
                        {
                          ProcessOptionElement (child, el, doc, multiple, parsing);
                          if (!firstOption)
                            firstOption = option;
                          // look for the next option
                          child = TtaSearchTypedElementInTree (opType, SearchForward, option, child);
                        }
                    }
                  TtaNextSibling (&option);
                }
            }
        }

      /* set this option SELECTED */
      attrType.AttrTypeNum = HTML_ATTR_Selected;
      attr = TtaGetAttribute (el, attrType);
      if (attr == NULL)
        {
          /* create the SELECTED attribute */
          attr = TtaNewAttribute (attrType);
          TtaSetAttributeValue (attr, HTML_ATTR_Selected_VAL_Yes_, el,doc);
          TtaAttachAttribute (el, attr, doc);
        }
    }

  if (firstOption)
    {
      showMeAttr = TtaGetAttribute (firstOption, attrshowMeType);
      if (attr)
        /* there is at least one option element with a selected attribute.
           Remove the ShowMe attribute from the first option element */
        {
          if (showMeAttr)
            TtaRemoveAttribute (firstOption, showMeAttr, doc);
        }
      else
        /* there is no option element with a selected attribute. Put
           an attribute ShowMe on the first option element to display it
           in the main view */
        {
          if (!showMeAttr)
            {
              showMeAttr = TtaNewAttribute (attrType);
              TtaSetAttributeValue (showMeAttr, HTML_ATTR_ShowMe_VAL_Yes_,
                                    firstOption, doc);
              TtaAttachAttribute (firstOption, showMeAttr, doc);
            }
        }
    }
}

/*----------------------------------------------------------------------
  LastLeafInElement
  return the last leaf element in element el.
  ----------------------------------------------------------------------*/
static Element LastLeafInElement (Element el)
{
  Element          child, lastLeaf;

  child = el;
  lastLeaf = NULL;
  while (child != NULL)
    {
      child = TtaGetLastChild (child);
      if (child != NULL)
        lastLeaf = child;
    }
  return lastLeaf;
}

/*----------------------------------------------------------------------
  CheckIconLink
  The element is a HTML link.
  Check element attributes and load the style sheet if needed.
  ----------------------------------------------------------------------*/
void CheckIconLink (Element el, Document doc, SSchema schema)
{
  Attribute           attr;
  AttributeType       attrType;
  char               *buff, *ptr;
  int                 length;

  /* A LINK element is complete.
     If it is a link to an icon, add the icon to the page
  */
  attrType.AttrSSchema = schema;
  attrType.AttrTypeNum = HTML_ATTR_REL;
  attr = TtaGetAttribute (el, attrType);
  if (attr)
    {
      /* get a buffer for the attribute value */
      length = TtaGetTextAttributeLength (attr);
      buff = (char*)TtaGetMemory (length + 1);
      TtaGiveTextAttributeValue (attr, buff, &length);
      ptr = strstr (buff, "icon");
      if (ptr == NULL)
        ptr = strstr (buff, "ICON");
      if (ptr &&
          DocumentMeta[doc] && DocumentMeta[doc]->method != CE_MAKEBOOK &&
          DocumentMeta[doc]->link_icon == NULL)
        DocumentMeta[doc]->link_icon = el;
      TtaFreeMemory (buff);
    }
}

/*----------------------------------------------------------------------
  CheckCSSLink
  The element is a HTML link.
  Check element attributes and load the style sheet if needed.
  ----------------------------------------------------------------------*/
void CheckCSSLink (Element el, Document doc, SSchema schema)
{
  Attribute           attr;
  AttributeType       attrType;
  CSSmedia            media;
  char               *utf8path, *buff;
  int                 length;

  /* A LINK element is complete.
     If it is a link to a style sheet, load that style sheet.
  */
  if (IsCSSLink (el, doc))
    {
      /* it's a link to a style sheet */
      /* get the media specification */
      attrType.AttrSSchema = schema;
      attrType.AttrTypeNum = HTML_ATTR_media;
      attr = TtaGetAttribute (el, attrType);
      if (attr)
        {
          length = TtaGetTextAttributeLength (attr);
          buff = (char*)TtaGetMemory (length + 1);
          TtaGiveTextAttributeValue (attr, buff, &length);
          media = CheckMediaCSS (buff);
          TtaFreeMemory (buff);
        }
      else
        media = CSS_ALL;

      /* Load that style sheet */
      attrType.AttrTypeNum = HTML_ATTR_HREF_;
      attr = TtaGetAttribute (el, attrType);
      if (attr &&
          DocumentMeta[doc] && 
          DocumentMeta[doc]->method != CE_MAKEBOOK)
        {
          length = TtaGetTextAttributeLength (attr);
          utf8path = (char*)TtaGetMemory (length + 1);
          TtaGiveTextAttributeValue (attr, utf8path, &length);
          /* load the stylesheet file found here ! */
          buff = (char *)TtaConvertMbsToByte ((unsigned char *)utf8path,
                                              TtaGetDefaultCharset ());
          if (buff)
            {
              LoadStyleSheet (buff, doc, el, NULL, NULL, media, FALSE);
              TtaFreeMemory (buff);
              UpdateStyleList (doc, 1);
            }
          TtaFreeMemory (utf8path);
        }
    }
}

/*----------------------------------------------------------------------
  RemoveEndingSpaces
  If element el is a block-level element, remove all spaces contained
  at the end of that element.
  Return TRUE if spaces have been removed.
  ----------------------------------------------------------------------*/
static ThotBool RemoveEndingSpaces (Element el)
{
  int              length;
  ElementType      elType;
  Element          lastLeaf;
  ThotBool         endingSpacesDeleted;

  endingSpacesDeleted = FALSE;
  elType = TtaGetElementType (el);
  if (!TtaIsLeaf (elType))
    /* it's a block element. */
    {
      /* Search the last leaf in the element's tree */
      lastLeaf = LastLeafInElement (el);
      if (elType.ElTypeNum == HTML_EL_Preformatted)
        el = NULL;
      else
        {
          // check if the element is within a preformatted
          elType.ElTypeNum = HTML_EL_Preformatted;
          el = TtaGetTypedAncestor (el, elType);
        }
      if (el == NULL && lastLeaf)
        {
          elType = TtaGetElementType (lastLeaf);
          if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
            /* the las leaf is a TEXT element */
            {
              length = TtaGetTextLength (lastLeaf);
              if (length > 0)
                TtaRemoveFinalSpaces (lastLeaf, HTMLcontext.doc, TRUE);
            }
        }
      endingSpacesDeleted = TRUE;
    }
  return endingSpacesDeleted;
}

/*----------------------------------------------------------------------
  CloseElement
  End of HTML element defined in entry entry of pHTMLGIMapping.
  Terminate all corresponding Thot elements.
  If start < 0, an explicit end tag has been encountered in the HTML file,
  else the end of element is implied by the beginning of an element
  described by entry start of pHTMLGIMapping.
  ----------------------------------------------------------------------*/
static ThotBool     CloseElement (int entry, int start, ThotBool onStartTag)
{
  int               i;
  ElementType       elType, parentType;
  Element           el, parent;
  ThotBool          ret, stop, spacesDeleted;
  int               error;

  ret = FALSE;
  /* the closed HTML element corresponds to a Thot element. */
  stop = FALSE;
  /* type of the element to be closed */
  elType.ElSSchema = DocumentSSchema;
  elType.ElTypeNum = pHTMLGIMapping[entry].ThotType;
  if (StackLevel > 0)
    {
      el = HTMLcontext.lastElement;
      if (HTMLcontext.lastElementClosed)
        el = TtaGetParent (el);
      i = StackLevel - 1;
      if (start < 0)
        /* Explicit close */
        {
          /* If we meet the end tag of a form, font or center
             looks for that element in the stack, but not at
             a higher level as a table element */
          if (!onStartTag &&
              (!strcmp (pHTMLGIMapping[entry].XMLname, "form") ||
               !strcmp (pHTMLGIMapping[entry].XMLname, "font") ||
               !strcmp (pHTMLGIMapping[entry].XMLname, "center")))
            while (i > 0 && entry != GINumberStack[i] && !stop)
              if (!strcmp (pHTMLGIMapping[GINumberStack[i]].XMLname, "tbody") ||
                  !strcmp (pHTMLGIMapping[GINumberStack[i]].XMLname, "tr")    ||
                  !strcmp (pHTMLGIMapping[GINumberStack[i]].XMLname, "th")    ||
                  !strcmp (pHTMLGIMapping[GINumberStack[i]].XMLname, "td"))
                {
                  /* ignore this end tag */
                  ret = FALSE;
                  stop = TRUE;
                  i = -1;
                }
              else
                i--;
          else
            /* looks in the stack for the element to be closed */
            while (i >= 0 && entry != GINumberStack[i])
              i--;
        }
      else
        /* Implicit close */
        {
          /* If the element to be closed is a list item (or
             equivalent), looks for that element in the
             stack, but not at a higher level as the list (or
             equivalent) element */
          if (!strcmp (pHTMLGIMapping[start].XMLname, "li"))
            {
              while (i > 0 && entry != GINumberStack[i] && !stop)
                if (!strcmp (pHTMLGIMapping[GINumberStack[i]].XMLname, "ol")  ||
                    !strcmp (pHTMLGIMapping[GINumberStack[i]].XMLname, "ul")  ||
                    !strcmp (pHTMLGIMapping[GINumberStack[i]].XMLname, "dir") ||
                    !strcmp (pHTMLGIMapping[GINumberStack[i]].XMLname, "menu"))
                  stop = TRUE;
                else
                  i--;
            }
          else if (!strcmp (pHTMLGIMapping[start].XMLname, "option"))
            {
              while (i > 0 && entry != GINumberStack[i] && !stop)
                if (!strcmp (pHTMLGIMapping[GINumberStack[i]].XMLname, "select"))
                  stop = TRUE;
                else
                  i--;
            }
          else if (!strcmp (pHTMLGIMapping[start].XMLname, "dd") ||
                   !strcmp (pHTMLGIMapping[start].XMLname, "dt"))
            {
              while (i > 0 && entry != GINumberStack[i] && !stop)
                if (!strcmp (pHTMLGIMapping[GINumberStack[i]].XMLname, "dl"))
                  stop = TRUE;
                else
                  i--;
            }
          else if (!strcmp (pHTMLGIMapping[start].XMLname, "tr") ||
                   !strcmp (pHTMLGIMapping[start].XMLname, "td") ||
                   !strcmp (pHTMLGIMapping[start].XMLname, "th"))
            {
              while (i > 0 && entry != GINumberStack[i] && !stop)
                if (!strcmp (pHTMLGIMapping[GINumberStack[i]].XMLname, "table"))
                  stop = TRUE;
                else
                  i--;
            }
        }

      if (i >= 0 && entry == GINumberStack[i])
        /* element found in the stack */
        {
          /* This element and its whole subtree are closed */
          StackLevel = i;
          HTMLcontext.lastElement = ElementStack[i];
          HTMLcontext.lastElementClosed = TRUE;
          ret = TRUE;
        }
      else if (!stop)
        /* element not found in the stack */
        if (start >= 0 && HTMLcontext.lastElement != NULL)
          {
            /* implicit close. Check the parent of current element */
            if (InsertSibling ())
              parent = TtaGetParent (HTMLcontext.lastElement);
            else
              parent = HTMLcontext.lastElement;
            if (parent != NULL)
              {
                parentType = TtaGetElementType (parent);
                if (elType.ElTypeNum == parentType.ElTypeNum)
                  {
                    HTMLcontext.lastElement = parent;
                    HTMLcontext.lastElementClosed = TRUE;
                    ret = TRUE;
                  }
                else if (TtaIsLeaf (TtaGetElementType (HTMLcontext.lastElement)))
                  {
                    parent = TtaGetParent (parent);
                    if (parent != NULL)
                      {
                        parentType = TtaGetElementType (parent);
                        if (elType.ElTypeNum == parentType.ElTypeNum)
                          {
                            HTMLcontext.lastElement = parent;
                            HTMLcontext.lastElementClosed = TRUE;
                            ret = TRUE;
                          }
                      }
                  }
              }
          }

      if (ret)
        /* successful close */
        {
          /* remove closed elements from the stack */
          while (i > 0)
            if (ElementStack[i] == HTMLcontext.lastElement)
              {
                StackLevel = i;
                i = 0;
              }
            else
              {
                if (TtaIsAncestor (ElementStack[i], HTMLcontext.lastElement))
                  StackLevel = i;
                i--;
              }
          if (StackLevel > 0)
            HTMLcontext.language = LanguageStack[StackLevel - 1];

          /* complete all closed elements */
          if (el != HTMLcontext.lastElement)
            if (!TtaIsAncestor(el, HTMLcontext.lastElement))
              el = NULL;
          spacesDeleted = FALSE;
          while (el != NULL)
            {
              XhtmlElementComplete (&HTMLcontext, el, &error);
              elType = TtaGetElementType (el);
              if (elType.ElTypeNum == HTML_EL_Table_)
                HTMLcontext.withinTable--;
              if (!spacesDeleted)
                /* If the element closed is a block-element, remove */
                /* spaces contained at the end of that element */
                spacesDeleted = RemoveEndingSpaces (el);
              if (el == HTMLcontext.lastElement)
                el = NULL;
              else
                el = TtaGetParent (el);
            }
        }
    }
   
  return ret;
}


/*----------------------------------------------------------------------
  MapAttrValue    search in AttrValueMappingTable the entry for
  the attribute thotAttr and its value attrVal. Returns the corresponding
  Thot value.
  ----------------------------------------------------------------------*/
int           MapAttrValue (int thotAttr, char* attrVal)
{
  int        i, value;

  value = -1;
  i = 0;
  while (XhtmlAttrValueMappingTable[i].ThotAttr != thotAttr &&
         XhtmlAttrValueMappingTable[i].ThotAttr != 0)
    i++;
  if (XhtmlAttrValueMappingTable[i].ThotAttr == thotAttr)
    do
      if (attrVal[1] == EOS && (thotAttr == HTML_ATTR_NumberStyle ||
                                thotAttr == HTML_ATTR_ItemStyle))
        /* attributes NumberStyle (which is always 1 character long) */
        /* and ItemStyle (only when its length is 1) are */
        /* case sensistive. Compare their exact value */
        if (attrVal[0] == XhtmlAttrValueMappingTable[i].XMLattrValue[0])
          value = XhtmlAttrValueMappingTable[i].ThotAttrValue;
        else
          i++;
      else
        /* for other attributes, uppercase and lowercase are */
        /* equivalent */
        if (!strcasecmp ((char *)XhtmlAttrValueMappingTable[i].XMLattrValue, (char *)attrVal))
          value = XhtmlAttrValueMappingTable[i].ThotAttrValue;
        else
          i++;
    while (value < 0 && XhtmlAttrValueMappingTable[i].ThotAttr == thotAttr);
  return value;
}

/*----------------------------------------------------------------------
  StopParsing 
  Stops the document parsing when an unrecoverable error is found
  ----------------------------------------------------------------------*/
static void StopParsing (Document doc)
{
  NormalTransition = FALSE;
  HTMLrootClosed = TRUE;
  CurrentBufChar = 0;
}

/*----------------------------------------------------------------------
  InsertInvalidEl
  create an Invalid_element element or a Unknown element.
  badposition indicate whether the element type is unknown (FALSE) or the
  tag position is incorrect (TRUE).
  ----------------------------------------------------------------------*/
static void InsertInvalidEl (char* content, ThotBool badposition)
{
  ElementType       elType;
  Element           elInv, elText;

  elType.ElSSchema = DocumentSSchema;
  if (badposition)
    elType.ElTypeNum = HTML_EL_Invalid_element;
  else
    elType.ElTypeNum = HTML_EL_Unknown_namespace;
  elInv = TtaNewElement (HTMLcontext.doc, elType);
  TtaSetElementLineNumber (elInv, NumberOfLinesRead);
  InsertElement (&elInv);
  if (elInv)
    {
      HTMLcontext.lastElementClosed = TRUE;
      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
      elText = TtaNewElement (HTMLcontext.doc, elType);
      TtaSetElementLineNumber (elText, NumberOfLinesRead);
      TtaInsertFirstChild (&elText, elInv, HTMLcontext.doc);
      TtaSetTextContent (elText, (unsigned char *)content, HTMLcontext.language, HTMLcontext.doc);
      InitBuffer ();
      if (!UnknownTag)
        /* close the end tag */
        TtaAppendTextContent (elText, (unsigned char *)">", HTMLcontext.doc);
      if (badposition)
        TtaSetAccessRight (elInv, ReadOnly, HTMLcontext.doc);
    }
}

/*----------------------------------------------------------------------
  EndOfStartTag   a ">" has been read. It indicates the end
  of a start tag.
  ----------------------------------------------------------------------*/
static void EndOfStartTag (char c)
{
  Element             elText;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  char               *text;
  int                 length, error;

  if (UnknownTag)
    {
      if (HTMLcontext.lastElement)
        {
          CloseBuffer ();
          elType = TtaGetElementType (HTMLcontext.lastElement);
          if (elType.ElTypeNum == HTML_EL_Invalid_element ||
              elType.ElTypeNum == HTML_EL_Unknown_namespace)
            {
              elText = TtaGetLastChild (HTMLcontext.lastElement);
              if (LgBuffer > 0)
                TtaAppendTextContent (elText, (unsigned char *)inputBuffer,
                                      HTMLcontext.doc);
              TtaAppendTextContent (elText, (unsigned char *)">",
                                    HTMLcontext.doc);
            }
          InitBuffer ();
        }
      UnknownTag = FALSE;
    }
  if (HTMLcontext.lastElement && lastElemEntry != -1)
    {
      if (!strcmp (pHTMLGIMapping[lastElemEntry].XMLname, "pre") ||
          !strcmp (pHTMLGIMapping[lastElemEntry].XMLname, "style") ||
          !strcmp (pHTMLGIMapping[lastElemEntry].XMLname, "script"))
        /* a <PRE>, <STYLE> or <SCRIPT> tag has been read */
        AfterTagPRE = TRUE;
      else if (!strcmp (pHTMLGIMapping[lastElemEntry].XMLname, "table"))
        /* <TABLE> has been read */
        HTMLcontext.withinTable++;
      else if (pHTMLGIMapping[lastElemEntry].XMLcontents == 'E')
        /* this is an empty element. Do not expect an end tag */
        {
          CloseElement (lastElemEntry, -1, TRUE);
          XhtmlElementComplete (&HTMLcontext, HTMLcontext.lastElement, &error);
        }

      /* if it's an AREA element, computes its position and size */
      ParseAreaCoords (HTMLcontext.lastElement, HTMLcontext.doc);
      /* if it's a STYLE element in CSS notation, activate the CSS */
      /* parser for parsing the element content */
      elType = TtaGetElementType (HTMLcontext.lastElement);
      if (elType.ElTypeNum == HTML_EL_STYLE_)
        {
          /* Search the Notation attribute */
          attrType.AttrSSchema = elType.ElSSchema;
          attrType.AttrTypeNum = HTML_ATTR_Notation;
          attr = TtaGetAttribute (HTMLcontext.lastElement, attrType);
          if (attr == NULL)
            /* No Notation attribute. Assume CSS by default */
            HTMLcontext.parsingCSS = TRUE;
          else
            /* the STYLE element has a Notation attribute */
            /* get its value */
            {
              length = TtaGetTextAttributeLength (attr);
              text = (char*)TtaGetMemory (length + 1);
              TtaGiveTextAttributeValue (attr, text, &length);
              if (!strcasecmp ((char *)text, "text/css"))
                HTMLcontext.parsingCSS = TRUE;
              TtaFreeMemory (text);
            }
        }
      else if (elType.ElTypeNum == HTML_EL_Text_Area)
        /* we have to read the content as a simple text unit */
        HTMLcontext.parsingTextArea = TRUE;
      else if (elType.ElTypeNum == HTML_EL_SCRIPT_)
        /* we have to read the content as a simple text unit */
        HTMLcontext.parsingScript = TRUE;
    }

  if (c == '<')
    {
      HTMLParseError (HTMLcontext.doc, "Syntax error", 0);
      StartOfTag (c);
    }
}


/*----------------------------------------------------------------------
  ContextOK       returns TRUE if the element at position entry
  in the mapping table is allowed to occur in the
  current structural context.
  ----------------------------------------------------------------------*/
static ThotBool     ContextOK (int entry)
{
  ThotBool         ok;
  int		    saveLastElemEntry;

  if (StackLevel == 0 || GINumberStack[StackLevel - 1] < 0)
    return TRUE;
  else
    {
      ok = TRUE;
      if (!strcmp (pHTMLGIMapping[GINumberStack[StackLevel - 1]].XMLname, "tr") &&
          strcmp (pHTMLGIMapping[entry].XMLname, "th") &&
          strcmp (pHTMLGIMapping[entry].XMLname, "td"))
        /* only TH and TD elements are allowed as children of a TR element */
        ok = FALSE;
      if (ok &&
          !strcmp (pHTMLGIMapping[GINumberStack[StackLevel - 1]].XMLname, "table") &&
          strcmp (pHTMLGIMapping[entry].XMLname, "caption")  &&
          strcmp (pHTMLGIMapping[entry].XMLname, "thead")    &&
          strcmp (pHTMLGIMapping[entry].XMLname, "tfoot")    &&
          strcmp (pHTMLGIMapping[entry].XMLname, "tbody")    &&
          strcmp (pHTMLGIMapping[entry].XMLname, "colgroup") &&
          strcmp (pHTMLGIMapping[entry].XMLname, "col")      &&
          strcmp (pHTMLGIMapping[entry].XMLname, "tr"))
        {
          /* only CAPTION, THEAD, TFOOT, TBODY, COLGROUP, COL and TR are */
          /* allowed as children of a TABLE element */
          if (!strcmp (pHTMLGIMapping[entry].XMLname, "td") ||
              !strcmp (pHTMLGIMapping[entry].XMLname, "th"))
            /* Table cell within a table, without a tr. Assume tr */
            {
              /* save the last last identifier read from the input file */
              saveLastElemEntry = lastElemEntry;
              /* simulate a <TR> tag */
              ProcessStartGI ("tr");
              /* restore the last tag that has actually been read */
              lastElemEntry = saveLastElemEntry;
            }
          else
            ok = FALSE;
        }
      if (ok &&
          (!strcmp (pHTMLGIMapping[entry].XMLname, "caption") ||
           !strcmp (pHTMLGIMapping[entry].XMLname, "thead") ||
           !strcmp (pHTMLGIMapping[entry].XMLname, "tfoot") ||
           !strcmp (pHTMLGIMapping[entry].XMLname, "tbody") ||
           !strcmp (pHTMLGIMapping[entry].XMLname, "colgroup")) &&
          strcmp (pHTMLGIMapping[GINumberStack[StackLevel - 1]].XMLname, "table"))
        /* CAPTION, THEAD, TFOOT, TBODY, COLGROUP are allowed only as
           children of a TABLE element */
        ok = FALSE;
      if (ok &&
          (!strcmp (pHTMLGIMapping[GINumberStack[StackLevel - 1]].XMLname, "thead") ||
           !strcmp (pHTMLGIMapping[GINumberStack[StackLevel - 1]].XMLname, "tfoot") ||
           !strcmp (pHTMLGIMapping[GINumberStack[StackLevel - 1]].XMLname, "tbody")) &&
          strcmp (pHTMLGIMapping[entry].XMLname, "tr"))
        /* only TR is allowed as a child of a THEAD, TFOOT or TBODY element */
        {
          if (!strcmp (pHTMLGIMapping[entry].XMLname, "td") ||
              !strcmp (pHTMLGIMapping[entry].XMLname, "th"))
            /* Table cell within a thead, tfoot or tbody without a tr. */
            /* Assume tr */
            {
              /* save the last last identifier read from the input file */
              saveLastElemEntry = lastElemEntry;
              /* simulate a <tr> tag */
              ProcessStartGI ("tr");
              /* restore the last tag that has actually been read */
              lastElemEntry = saveLastElemEntry;
            }
          else
            ok = FALSE;
        }
      if (ok)
        /* refuse HEAD within HEAD */
        if (strcmp (pHTMLGIMapping[entry].XMLname, "head") == 0)
          if (Within (HTML_EL_HEAD, DocumentSSchema))
            ok = FALSE;
      if (ok)
        /* refuse STYLE within STYLE */
        if (strcmp (pHTMLGIMapping[entry].XMLname, "style") == 0)
          if (Within (HTML_EL_STYLE_, DocumentSSchema))
            ok = FALSE;
      return ok;
    }
}

/*----------------------------------------------------------------------
  SpecialImplicitEnd
  ----------------------------------------------------------------------*/
static void SpecialImplicitEnd (int entry)
{
  ElementType      elType;

  /* if current element is DD, Hn closes that DD only when there is */
  /* no enclosing DL */
  if (pHTMLGIMapping[entry].XMLname[0] == 'H' &&
      pHTMLGIMapping[entry].XMLname[1] >= '1' &&
      pHTMLGIMapping[entry].XMLname[1] <= '6' &&
      pHTMLGIMapping[entry].XMLname[2] == EOS)
    /* the new element is a Hn */
    if (StackLevel > 1)
      if (ElementStack[StackLevel - 1] != NULL)
        {
          elType = TtaGetElementType (ElementStack[StackLevel - 1]);
          if (elType.ElTypeNum == HTML_EL_Definition)
            /* the current element is a DD */
            {
              elType = TtaGetElementType (ElementStack[StackLevel - 2]);
              if (elType.ElTypeNum != HTML_EL_Definition_List)
                /* DD in not within a DL. Close the DD element */
                CloseElement (GINumberStack[StackLevel - 1], entry, FALSE);
            }
        }
}

/*----------------------------------------------------------------------
  ProcessStartGI  An HTML GI has been read in a start tag.
  Create the corresponding Thot thing (element, attribute,
  or character), according to the mapping table.
  ----------------------------------------------------------------------*/
static void ProcessStartGI (const char* GIname)
{
  ElementType         elType;
  Element             el;
  int                 entry, i;
  char                msgBuffer[MaxMsgLength];
  PtrClosedElement    pClose;
  ThotBool            sameLevel, removed, error;
  SSchema	      schema;

  /* ignore tag <P> within PRE */
  if (Within (HTML_EL_Preformatted, DocumentSSchema))
    if (strcasecmp ((char *)GIname, "p") == 0)
      return;

  /* search the HTML element name in the mapping table */
  schema = DocumentSSchema;
  entry = MapGI ((char *)GIname, &schema, HTMLcontext.doc);
  lastElemEntry = entry;

  if (entry < 0)
    /* not found in the HTML DTD */
    {
      /* check if it's the math or svg tag with a namespace prefix */
      /* So, look for a colon in the element name */
      for (i = 0; GIname[i] != ':' && GIname[i] != EOS; i++);
      if (GIname[i] == ':' &&
          (strcasecmp ((char *)&GIname[i+1], "math") == 0 ||
           strcasecmp ((char *)&GIname[i+1], "xmlgraphics") == 0 ||
           strcasecmp ((char *)&GIname[i+1], "svg") == 0))
        /* it's a math or svg tag with a namespace prefix. OK */
        {
          entry = MapGI ((char *)&GIname[i+1], &schema, HTMLcontext.doc);
          lastElemEntry = entry;
        }
      else
        /* unknown tag */
        {
          UnknownTag = TRUE;
          if (DocumentMeta[HTMLcontext.doc] &&
              DocumentMeta[HTMLcontext.doc]->xmlformat)
            {
              snprintf (msgBuffer, MaxMsgLength,
			"Invalid tag <%s> (removed when saving)", GIname);
              HTMLParseError (HTMLcontext.doc, msgBuffer, 0);
              removed = TRUE;
            }
          else
            {
              snprintf (msgBuffer, MaxMsgLength, "Warning - unknown tag <%s>",
			GIname);
              HTMLParseError (HTMLcontext.doc, msgBuffer, 0);
              removed = FALSE;
            }
          /* create an Invalid_element */
          snprintf (msgBuffer, MaxMsgLength, "<%s", GIname);
          InsertInvalidEl (msgBuffer, removed);
        }
    }

  if (entry >= 0)
    {
      if (TtaGetDocumentProfile(HTMLcontext.doc) != L_Other &&
          !(pHTMLGIMapping[entry].Level &
            TtaGetDocumentProfile(HTMLcontext.doc))) 
        {
          /* Invalid element for the document profile */
          /* don't process that element */
          snprintf (msgBuffer, MaxMsgLength,
                   "Invalid element <%s> for the document profile", GIname);
          HTMLParseError (HTMLcontext.doc, msgBuffer, 0);
          XMLErrorsFoundInProfile = TRUE;
          UnknownTag = TRUE;
        }
      else
        {
          if (HTMLcontext.withinTable == 0 &&
              (!strcmp (pHTMLGIMapping[entry].XMLname, "td") ||
               !strcmp (pHTMLGIMapping[entry].XMLname, "th")))
            {
              sprintf (msgBuffer, "Tags <table>, <tbody> and <tr> added");
              HTMLParseError (HTMLcontext.doc, msgBuffer, 0);
              /* generate mandatory parent elements */ 
              ProcessStartGI ("table");
              HTMLcontext.withinTable = 1;
              ProcessStartGI ("tr");
            }
          else if (HTMLcontext.withinTable == 0 &&
                   !strcmp (pHTMLGIMapping[entry].XMLname, "tr"))
            {
              /* generate mandatory parent elements */ 
              sprintf (msgBuffer, "Tags <table> and <tbody> added");
              HTMLParseError (HTMLcontext.doc, msgBuffer, 0);
              ProcessStartGI ("table");
            }
          /* does this start tag also imply the end tag of some current elements?*/
          pClose = FirstClosedElem[entry];
          while (pClose != NULL)
            {
              CloseElement (pClose->tagNum, entry, TRUE);
              pClose = pClose->nextClosedElem;
            }
          /* process some special cases... */
          SpecialImplicitEnd (entry);
          error = !ContextOK (entry);
          if (error)
            /* element not allowed in the current structural context */
            {
              /* send an error message */
              snprintf (msgBuffer, MaxMsgLength,
                       "Tag <%s> is not allowed here (removed when saving)",
                       GIname);
              HTMLParseError (HTMLcontext.doc, msgBuffer, 0);
              /* if it's a <script> tag, process it normally to avoid its
                 content to be considered as plain text */
              if (!strcmp (pHTMLGIMapping[entry].XMLname, "script"))
                error = FALSE;
            }
          if (error)
            {
              UnknownTag = TRUE;
              /* create an Invalid_element */
              snprintf (msgBuffer, MaxMsgLength, "<%s", GIname);
              InsertInvalidEl (msgBuffer, TRUE);
            }
          else
            {
              el = NULL;
              sameLevel = TRUE;
              if (pHTMLGIMapping[entry].ThotType > 0)
                {
                  /* create a Thot element */
                  elType.ElSSchema = DocumentSSchema;
                  elType.ElTypeNum = pHTMLGIMapping[entry].ThotType;
                  if (pHTMLGIMapping[entry].XMLcontents == 'E')
                    /* empty HTML element. Create all children specified */
                    /* in the Thot structure schema */
                    el = TtaNewTree (HTMLcontext.doc, elType, "");
                  else
                    /* the HTML element may have children. Create only */
                    /* the corresponding Thot element, without any child */
                    el = TtaNewElement (HTMLcontext.doc, elType);
                  TtaSetElementLineNumber (el, NumberOfLinesRead);
                  sameLevel = InsertElement (&el);
                  if (el != NULL)
                    {
                      if (pHTMLGIMapping[entry].XMLcontents == 'E')
                        HTMLcontext.lastElementClosed = TRUE;
                      if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
                        /* an empty Text element has been created. The */
                        /* following character data must go to that elem. */
                        HTMLcontext.mergeText = TRUE;
                    }
                }
              if (pHTMLGIMapping[entry].XMLcontents != 'E')
                {
                  if (StackLevel >= MaxStack - 1)
                    HTMLParseError (HTMLcontext.doc, "Too many nested elements", 0);
                  else
                    {
                      ElementStack[StackLevel] = el;
                      if (sameLevel)
                        ThotLevel[StackLevel] = ThotLevel[StackLevel - 1];
                      else
                        ThotLevel[StackLevel] = ThotLevel[StackLevel - 1] + 1;
                      LanguageStack[StackLevel] = HTMLcontext.language;
                      GINumberStack[StackLevel++] = entry;
                    }
                }
            }
        }
    }
}


/*----------------------------------------------------------------------
  EndOfStartGI    An HTML GI has been read in a start tag.
  ----------------------------------------------------------------------*/
static void     EndOfStartGI (char c)
{
  char        schemaName[20];
  char        theGI[MaxMsgLength];
  char        *tagName;
  int	      i;

  if (HTMLcontext.parsingTextArea || HTMLcontext.parsingScript)
    /* We are parsing the contents of a TEXTAREA or SCRIPT element. If a start
       tag appears, consider it as plain text */
    {
      /* next state is state 0, not the state computed by the automaton */
      NormalTransition = FALSE;
      currentState = 0; 
      /* put a '<' and the tagname (GI) in the input buffer */
      for (i = LgBuffer; i > 0; i--)
        inputBuffer[i] = inputBuffer[i - 1];
      LgBuffer++;
      inputBuffer[0] = '<';
      inputBuffer[LgBuffer] = EOS;
      /* copy the input buffer in the document */
      TextToDocument ();
    }
  else
    {
      /* if the last character in the GI is a '/', ignore it.  This is to
         accept the XML syntax for empty elements, for instance <br/> */
      if (LgBuffer > 0 && inputBuffer[LgBuffer-1] == '/')
        LgBuffer--;
      CloseBuffer ();
      strncpy ((char *)theGI, (char *)inputBuffer, MaxMsgLength - 1);
      theGI[MaxMsgLength - 1] = EOS;

      InitBuffer ();
      if (HTMLcontext.lastElementClosed &&
          HTMLcontext.lastElement == rootElement)
        /* an element after the tag </html>, ignore it */
        {
          HTMLParseError (HTMLcontext.doc, "Element after tag </html>. Ignored", 0);
          return;
        }

      /* if it's a "math" or "svg" tag, it may have a namespace name */
      tagName = theGI;
      for (i = 0; theGI[i] != ':' && theGI[i] != EOS; i++);
      if (theGI[i] == ':' &&
          (strcasecmp ((char *)&theGI[i+1], "math") == 0 ||
           strcasecmp ((char *)&theGI[i+1], "svg") == 0))
        /* it's a math or svg tag with a namespace prefix. ignore the prefix */
	tagName = &theGI[i+1];

      if (!strcmp (tagName, "math") || !strcmp (tagName, "svg"))
        /* a <math> or <svg> tag has been read */
        {
          /* get back to the beginning of the tag in the input buffer */
          /* "NotToReadFile" boolean means that we get back in the */
          /* previous input buffer */	
          /* That case happens when the "<" and ">" characters for that */
          /* tag have not been read in the same input buffer */
          if (StartOfTagIndx <= 0 ||
              (StartOfTagIndx > CurrentBufChar && CurrentBufChar != 0))
            {
              NumberOfCharRead = PreviousNumberOfCharRead;
              NumberOfLinesRead = PreviousNumberOfLinesRead;
              NotToReadFile = TRUE;
              if (StartOfTagIndx < 0)
                CurrentBufChar = LastCharInPreviousRead;
              else
                CurrentBufChar = StartOfTagIndx;
            }
          else
            CurrentBufChar = StartOfTagIndx;
	  
          if (!strcmp ((char *)tagName, (char *)"math"))
            strcpy ((char *)schemaName, (char *)"MathML");
          else
            strcpy ((char *)schemaName, (char *)"SVG");
          /* Parse the corresponding element with the XML parser */
          if (!ParseIncludedXml ((FILE *)stream, &WorkBuffer, INPUT_FILE_BUFFER_SIZE,
                                 &EndOfHtmlFile, &NotToReadFile,
                                 PreviousRead, &LastCharInWorkBuffer,
                                 InputText, &CurrentBufChar,
                                 &NumberOfLinesRead, &NumberOfCharRead,
                                 schemaName, HTMLcontext.doc,
                                 &HTMLcontext.lastElement,
                                 &HTMLcontext.lastElementClosed,
                                 HTMLcontext.language))
            StopParsing (HTMLcontext.doc);   /* the XML parser raised an error */
          /* the whole element has been read by the XML parser */
          /* reset the automaton state */
          NormalTransition = FALSE;
          currentState = 0;
          CharProcessed = TRUE;
        }
      else
        ProcessStartGI (tagName);
    }
}

/*----------------------------------------------------------------------
  EndOfStartGIandTag      a ">" has been read. It indicates the
  end of a GI and the end of a start tag.
  ----------------------------------------------------------------------*/
static void EndOfStartGIandTag (char c)
{
  EndOfStartGI (c);
  EndOfStartTag (c);
  if (c == '<')
    {
      HTMLParseError (HTMLcontext.doc, "Syntax error", 0);
      StartOfTag (c);
    }
}

/*----------------------------------------------------------------------
  StartCData  a new CDATA element (<![CDATA[)
  ----------------------------------------------------------------------*/
static void StartCData (char c)
{
  ElementType         elType;
  Element             el, child;

  CloseBuffer ();
  if (!strcasecmp ((char *)inputBuffer, "cdata"))
    {
      elType.ElSSchema = DocumentSSchema;
      elType.ElTypeNum = HTML_EL_CDATA;
      el = TtaNewElement (HTMLcontext.doc, elType);
      TtaSetElementLineNumber (el, NumberOfLinesRead);
      InsertElement (&el);
      elType.ElTypeNum = HTML_EL_CDATA_line;
      child = TtaNewTree (HTMLcontext.doc, elType, "");
      TtaSetElementLineNumber (child, NumberOfLinesRead);
      TtaInsertFirstChild (&child, el, HTMLcontext.doc);
      HTMLcontext.lastElement = TtaGetFirstChild (child);
      /* clear the input buffer */
      InitBuffer ();
    }
}

/*----------------------------------------------------------------------
  CloseCDataLine closes a CDATA line.
  ----------------------------------------------------------------------*/
static void CloseCDataLine (char c)
{

  CloseBuffer ();
  if (LgBuffer > 0 && inputBuffer[LgBuffer-1] == EOL)
    {
      LgBuffer--;
      inputBuffer[LgBuffer] = EOS;
    }
  if (LgBuffer > 0 && inputBuffer[LgBuffer-1] == ']')
    {
      LgBuffer--;
      inputBuffer[LgBuffer] = EOS;
    }
  if (LgBuffer > 0 && inputBuffer[LgBuffer-1] == ']')
    {
      LgBuffer--;
      inputBuffer[LgBuffer] = EOS;
    }
  /* copy the input buffer into the document */
  if (LgBuffer)
    TtaAppendTextContent (HTMLcontext.lastElement, (unsigned char *)inputBuffer,
                          HTMLcontext.doc);
  /* clear the input buffer */
  InitBuffer ();
}

/*----------------------------------------------------------------------
  EndOfCDataLine closes a CDATA line.
  ----------------------------------------------------------------------*/
static void EndOfCDataLine (char c)
{
  ElementType         elType;
  Element             el, child;

  CloseCDataLine (c);
  /* start a new CDATA line */
  elType.ElSSchema = DocumentSSchema;
  elType.ElTypeNum = HTML_EL_CDATA_line;
  child = TtaNewTree (HTMLcontext.doc, elType, "");
  TtaSetElementLineNumber (child, NumberOfLinesRead);
  elType = TtaGetElementType (HTMLcontext.lastElement);
  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
    el = TtaGetParent (HTMLcontext.lastElement);
  else
    el = HTMLcontext.lastElement;
  TtaInsertSibling (child, el, FALSE, HTMLcontext.doc);
  HTMLcontext.lastElement = TtaGetFirstChild (child);
}

/*----------------------------------------------------------------------
  EndOfCdata closes a CDATA.
  ----------------------------------------------------------------------*/
static void EndOfCData (char c)
{
  ElementType         elType;

  CloseCDataLine (c);
  HTMLcontext.lastElementClosed = TRUE;
  elType.ElSSchema = DocumentSSchema;
  elType.ElTypeNum = HTML_EL_CDATA;
  HTMLcontext.lastElement = TtaGetTypedAncestor (HTMLcontext.lastElement, elType);
}

/*----------------------------------------------------------------------
  EndOfEndTag     An end tag has been read in the HTML file.
  Terminate all corresponding Thot elements.
  ----------------------------------------------------------------------*/
static void EndOfEndTag (char c)
{
  SSchema	   schema;
  char            msgBuffer[MaxMsgLength];
  int             entry;
  int             i, profile;
  ThotBool        ok, removed;

  CloseBuffer ();
  if ((HTMLcontext.parsingTextArea &&
       strcasecmp ((char *)inputBuffer, "textarea")) ||
      (HTMLcontext.parsingScript &&
       strcasecmp ((char *)inputBuffer, "script")))
    /* We are parsing the contents of a textarea or script element. The end
       tag is not the one closing the current textarea or script, consider it
       as plain text */
    {
      /* next state is state 0, not the state computed by the automaton */
      NormalTransition = FALSE;
      currentState = 0;
      /* put "</" and the tag name in the input buffer */
      for (i = LgBuffer; i > 0; i--)
        inputBuffer[i + 1] = inputBuffer[i - 1];
      LgBuffer += 2;
      inputBuffer[0] = '<';
      inputBuffer[1] = '/';
      inputBuffer[LgBuffer] = EOS;
      /* copy the input buffer into the document */
      TextToDocument ();
    }
  else
    {
      /* is it the end of the current HTML fragment ? */
      ok = FALSE;
      if (HTMLrootClosingTag != EOS)
        {
          /* look for a colon in the element name (namespaces) and ignore the
             prefix if there is one */
          for (i = 0; i < LgBuffer && inputBuffer[i] != ':'; i++);
          if (inputBuffer[i] == ':')
            i++;
          else
            i = 0;
          if (strcasecmp ((char *)&inputBuffer[i], (char *)HTMLrootClosingTag) == 0)
            {
              HTMLrootClosed = TRUE;
              ok = TRUE;
            }
        }

      //if (!strcasecmp ((char *)inputBuffer, "font"))
      //             printf ("font element\n");
      profile = TtaGetDocumentProfile(HTMLcontext.doc);
      if (!ok)
        {
          /* search the HTML tag in the mapping table */
          schema = DocumentSSchema;
          entry = MapGI ((char *)inputBuffer, &schema, HTMLcontext.doc);
          if (entry < 0)
            {
              if (strlen ((char *)inputBuffer) > MaxMsgLength - 20)
                inputBuffer[MaxMsgLength - 20] = EOS;
              if (DocumentMeta[HTMLcontext.doc] &&
                  DocumentMeta[HTMLcontext.doc]->xmlformat)
                {
                  snprintf (msgBuffer, MaxMsgLength,
			    "Invalid tag <%s> (removed when saving)", inputBuffer);
                  HTMLParseError (HTMLcontext.doc, msgBuffer, 0);
                  removed = TRUE;
                }
              else
                {
                  snprintf (msgBuffer, MaxMsgLength,
			    "Warning - unknown tag </%s>", inputBuffer);
                  HTMLParseError (HTMLcontext.doc, msgBuffer, 0);
                  removed = FALSE;
                }
              /* create an Invalid_element */
              snprintf (msgBuffer, MaxMsgLength, "</%s", inputBuffer);
              InsertInvalidEl (msgBuffer, removed);
            }
          else if (entry >= 0 &&
                   profile != L_Other &&
                   !(pHTMLGIMapping[entry].Level & profile)) 
            {
              /* Invalid element for the document profile */
              if (strlen ((char *)inputBuffer) > MaxMsgLength - 20)
                inputBuffer[MaxMsgLength - 20] = EOS;
              snprintf (msgBuffer, MaxMsgLength,
			"Invalid end element <%s> for the document profile",
			inputBuffer);
              HTMLParseError (HTMLcontext.doc, msgBuffer, 0);
              XMLErrorsFoundInProfile = TRUE;
            }
          else if (!CloseElement (entry, -1, FALSE))
            /* the end tag does not close any current element */
            {
              if (DocumentMeta[HTMLcontext.doc] &&
                  DocumentMeta[HTMLcontext.doc]->xmlformat)
                {
                  snprintf (msgBuffer, MaxMsgLength,
			    "Invalid end tag <%s>", inputBuffer);
                  HTMLParseError (HTMLcontext.doc, msgBuffer, 0);
                }
              else
                {
                  /* try to recover */
                  if ((inputBuffer[0] == 'H' || inputBuffer[0] == 'h') &&
                      inputBuffer[1] >= '1' && inputBuffer[1] <= '6' &&
                      inputBuffer[2] == EOS)
                    /* the end tag is </Hn>. Consider all Hn as equivalent. */
                    /* </H3> is considered as an end tag for <H2>, for instance */
                    {
                      strcpy ((char *)msgBuffer, (char *)inputBuffer);
                      msgBuffer[1] = '1';
                      i = 1;
                      do
                        {
                          schema = DocumentSSchema;
                          entry = MapGI ((char *)msgBuffer, &schema, HTMLcontext.doc);
                          ok = CloseElement (entry, -1, FALSE);
                          msgBuffer[1]++;
                          i++;
                        }
                      while (i <= 6 && !ok);
                    }
                  if (!ok &&
                      (!strcasecmp ((char *)inputBuffer, "ol")   ||
                       !strcasecmp ((char *)inputBuffer, "ul")   ||
                       !strcasecmp ((char *)inputBuffer, "menu") ||
                       !strcasecmp ((char *)inputBuffer, "dir")))
                    /* the end tag is supposed to close a list */
                    /* try to close another type of list */
                    {
                      ok = TRUE;
                      schema = DocumentSSchema;
                      if (!CloseElement (MapGI ((char *)"ol", &schema, HTMLcontext.doc), -1, FALSE) &&
                          !CloseElement (MapGI ((char *)"ul", &schema, HTMLcontext.doc), -1, FALSE) &&
                          !CloseElement (MapGI ((char *)"menu", &schema, HTMLcontext.doc), -1, FALSE) &&
                          !CloseElement (MapGI ((char *)"dir", &schema, HTMLcontext.doc), -1, FALSE))
                        ok = FALSE;
                    }
                  if (!ok)
                    /* unrecoverable error. Create an Invalid_element */
                    {
                      snprintf (msgBuffer, MaxMsgLength, "</%s", inputBuffer);
                      InsertInvalidEl (msgBuffer, TRUE);
                      /* print an error message... */
                      snprintf (msgBuffer, MaxMsgLength,
				"Invalid end tag </%s> (removed when saving)",
                               inputBuffer);
                    }
                  else
                    /* print an error message... */
                    snprintf (msgBuffer, MaxMsgLength,
			      "Warning - unexpected end tag </%s>",
			      inputBuffer);
                  HTMLParseError (HTMLcontext.doc, msgBuffer, 0);
                }
            }
        }
      InitBuffer ();
    }

  if (c == '<')
    {
      HTMLParseError (HTMLcontext.doc, "Syntax error", 0);
      StartOfTag (c);
    }
}

/*----------------------------------------------------------------------
  EndOfAttrName   A HTML attribute has been read. Create the
  corresponding Thot attribute.
  ----------------------------------------------------------------------*/
static void EndOfAttrName (char c)
{
  AttributeMapping*   tableEntry;
  AttributeType       attrType;
  Element             elText;
  ElementType         elType;
  Attribute           attr;
  SSchema             schema;
  char                translation;
  char                msgBuffer[MaxMsgLength];
  ThotBool            highEnoughLevel;

  CloseBuffer ();
  if (UnknownTag && HTMLcontext.lastElement)
    {
      elType = TtaGetElementType (HTMLcontext.lastElement);
      if (elType.ElTypeNum == HTML_EL_Invalid_element ||
          elType.ElTypeNum == HTML_EL_Unknown_namespace)
        {
          elText = TtaGetLastChild (HTMLcontext.lastElement);
          TtaAppendTextContent (elText, (unsigned char *)" ", HTMLcontext.doc);
          TtaAppendTextContent (elText, (unsigned char *)inputBuffer,
                                HTMLcontext.doc);
        }
      InitBuffer ();
      lastAttrEntry = NULL;
      return;
    }

  /* if a single '/' or '?' has been read instead of an attribute name, ignore
     that character.  This is to accept the XML syntax for empty elements or
     processing instructions, such as <img src="SomeUrl" /> or
     <?xml version="1.0"?>  */
  if (LgBuffer == 1 &&
      (inputBuffer[0] == '/' || inputBuffer[0] == '?'))
    {
      InitBuffer ();
      return;
    }

  highEnoughLevel = TRUE;
  /* inputBuffer contains the attribute name */
  /* get the corresponding Thot attribute */
  if (UnknownTag)
    /* ignore attributes of unknown tags */
    tableEntry = NULL;
  else
    tableEntry = MapAttr ((char *)inputBuffer, &schema,
                          lastElemEntry, &highEnoughLevel, HTMLcontext.doc);

  if (tableEntry)
    /* this is a known attribute. Can it be associated with the current
       element ? */
    {
      /* reject attribute height on a table */
      if (tableEntry->ThotAttribute == HTML_ATTR_Height_)
        {
          elType = TtaGetElementType (HTMLcontext.lastElement);
          if (elType.ElTypeNum == HTML_EL_Table_)
            tableEntry = NULL;
        }
      else if (tableEntry->ThotAttribute == HTML_ATTR_xmlid)
        {
          snprintf (msgBuffer, MaxMsgLength,
		   "Invalid attribute \"%s\"(removed when saving)",
		   inputBuffer);
          HTMLParseError (HTMLcontext.doc, msgBuffer, 0);
          /* attach an Invalid_attribute to the current element */
          tableEntry = &pHTMLAttributeMapping[0];
          schema = DocumentSSchema;
          UnknownAttr = TRUE;
        }
    }
   
  if (!tableEntry)
    {
      if (highEnoughLevel)
        {
          /* this attribute is not in the HTML mapping table */
          if (strcasecmp ((char *)inputBuffer, "xmlns") == 0 ||
              strncasecmp ((char *)inputBuffer, "xmlns:", 6) == 0)
            /* this is a namespace declaration */
            {
              lastAttrEntry = NULL;
              /**** register this namespace ****/;
            }
          //else if (strcasecmp ((char *)inputBuffer, "xml:lang") == 0)
          /* attribute xml:lang is not considered as invalid, but it is
             ignored */
          //  lastAttrEntry = NULL;
          else
            {
              snprintf (msgBuffer, MaxMsgLength,
			"Invalid attribute \"%s\"(removed when saving)",
			inputBuffer);
              HTMLParseError (HTMLcontext.doc, msgBuffer, 0);
              /* attach an Invalid_attribute to the current element */
              tableEntry = &pHTMLAttributeMapping[0];
              schema = DocumentSSchema;
              UnknownAttr = TRUE;
            }
        }
      else
        {
          /* attribute invalid for the document profile */
          snprintf (msgBuffer, MaxMsgLength,
                   "Invalid attribute \"%s\" for the document profile",
                   inputBuffer);
          HTMLParseError (HTMLcontext.doc, msgBuffer, 0);
          XMLErrorsFoundInProfile = TRUE;
          UnknownAttr = TRUE;
          lastAttrEntry = NULL;
        }
    }
  else
    UnknownAttr = FALSE;

  if (tableEntry != NULL && HTMLcontext.lastElement != NULL &&
      (!HTMLcontext.lastElementClosed ||
       (HTMLcontext.lastElement != rootElement)))
    {
      lastAttrEntry = tableEntry;
      translation = lastAttrEntry->AttrOrContent;
      switch (translation)
        {
        case 'C':	/* Content */
          /* Nothing to do yet: wait for attribute value */
          break;
        case 'A':
          /* create an attribute for current element */
          attrType.AttrSSchema = schema;
          attrType.AttrTypeNum = tableEntry->ThotAttribute;
          CreateHTMLAttribute (HTMLcontext.lastElement, attrType, (char *)inputBuffer, 
                               (ThotBool)(tableEntry == &pHTMLAttributeMapping[0]),
                               HTMLcontext.doc, &lastAttribute, &lastAttrElement);
          if (attrType.AttrTypeNum == HTML_ATTR_HREF_)
            {
              elType = TtaGetElementType (HTMLcontext.lastElement);
              if (elType.ElTypeNum == HTML_EL_Anchor)
                /* attribute HREF for element Anchor */
                /* create attribute PseudoClass = link */
                {
                  attrType.AttrTypeNum = HTML_ATTR_PseudoClass;
                  attr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (HTMLcontext.lastElement, attr,
                                      HTMLcontext.doc);
                  TtaSetAttributeText (attr, "link",
                                       HTMLcontext.lastElement,
                                       HTMLcontext.doc);
                }
            }
          else if (attrType.AttrTypeNum == HTML_ATTR_Checked)
            {
              /* create Default-Checked attribute */
              attrType.AttrSSchema = DocumentSSchema;
              attrType.AttrTypeNum = HTML_ATTR_DefaultChecked;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (HTMLcontext.lastElement, attr,
                                  HTMLcontext.doc);
              TtaSetAttributeValue (attr, HTML_ATTR_DefaultChecked_VAL_Yes_,
                                    HTMLcontext.lastElement, HTMLcontext.doc);
            }
          else if (attrType.AttrTypeNum == HTML_ATTR_Selected)
            {
              /* create Default-Selected attribute */
              attrType.AttrSSchema = DocumentSSchema;
              attrType.AttrTypeNum = HTML_ATTR_DefaultSelected;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (HTMLcontext.lastElement, attr,
                                  HTMLcontext.doc);
              TtaSetAttributeValue (attr, HTML_ATTR_DefaultSelected_VAL_Yes_,
                                    HTMLcontext.lastElement, HTMLcontext.doc);
            }
          break;
        case SPACE:
          /* nothing to do */
          break;
        default:
          break;
        }
    }
  InitBuffer ();
}


/*----------------------------------------------------------------------
  EndOfAttrNameAndTag     A ">" has been read. It indicates the
  end of an attribute name and the end of a start tag.
  ----------------------------------------------------------------------*/
static void         EndOfAttrNameAndTag (char c)
{
  EndOfAttrName (c);
  EndOfStartTag (c);
  if (c == '<')
    {
      HTMLParseError (HTMLcontext.doc, "Syntax error", 0);
      StartOfTag (c);
    }
}

/*----------------------------------------------------------------------
  StartOfQuotedAttrValue
  A quote (or double quote) starting an attribute value has been read.
  ----------------------------------------------------------------------*/
static void StartOfQuotedAttrValue (char c)
{
  ReadingAnAttrValue = TRUE;
  if (UnknownAttr)
    /* this is the value of an unknown attribute. keep the quote */
    /* in the input buffer for copying it in the current */
    /* Invalid_attribute */
    PutInBuffer (c);
}

/*----------------------------------------------------------------------
  StartOfUnquotedAttrValue
  The first character of an unquoted attribute value has been read.
  ----------------------------------------------------------------------*/
static void         StartOfUnquotedAttrValue (char c)
{
  ReadingAnAttrValue = TRUE;
  PutInBuffer (c);
}

static ThotBool   isAttrValueTruncated;
/*----------------------------------------------------------------------
  EndOfAttrValue
  An attribute value has been read from the HTML file.
  Put that value in the current Thot attribute.
  ----------------------------------------------------------------------*/
static void         EndOfAttrValue (char c)
{
  Element             elText;
  ElementType         elType;
  char               *newBufferAttrValue;
  int                 lg;

  if (TruncatedAttrValue)
    {
      isAttrValueTruncated = TRUE;
      if (BufferAttrValue == NULL)
        {
          lg = 2 * MaxBufferLength;
          BufferAttrValue = (char*)TtaGetMemory (lg + 1);
          strcpy ((char *)BufferAttrValue, (char *)inputBuffer);
          LgBufferAttrValue = lg;
        }
      else
        {
          LgBufferAttrValue += MaxBufferLength;
          newBufferAttrValue = (char*)TtaGetMemory (LgBufferAttrValue + 1);
          strcpy ((char *)newBufferAttrValue, (char *)BufferAttrValue);
          strcat ((char *)newBufferAttrValue, (char *)inputBuffer);
          TtaFreeMemory (BufferAttrValue);
          BufferAttrValue = newBufferAttrValue;
        }
    }
  else
    {
      ReadingAnAttrValue = FALSE;
      if (UnknownAttr)
        /* this is the end of value of an invalid attribute. Keep the */
        /* quote character that ends the value for copying it into the */
        /* Invalid_attribute. */
        if (c == '\'' || c == '\"')
          PutInBuffer (c);
      CloseBuffer ();
      /* inputBuffer contains the attribute value */
      
      if (UnknownTag && HTMLcontext.lastElement)
        {
          elType = TtaGetElementType (HTMLcontext.lastElement);
          if (elType.ElTypeNum == HTML_EL_Invalid_element ||
              elType.ElTypeNum == HTML_EL_Unknown_namespace)
            {
              elText = TtaGetLastChild (HTMLcontext.lastElement);
              TtaAppendTextContent (elText, (unsigned char *)"=",
                                    HTMLcontext.doc);
              TtaAppendTextContent (elText, (unsigned char *)inputBuffer,
                                    HTMLcontext.doc);
            }
          InitBuffer ();
          lastAttrEntry = NULL;
          return;
        }
      if (lastAttrEntry == NULL)
        {
          InitBuffer ();
          return;
        }
      
      if (HTMLcontext.lastElementClosed &&
          (HTMLcontext.lastElement == rootElement))
        {
          /* an attribute after the tag </html>, ignore it */
        }
      else
        {
          if (isAttrValueTruncated)
            {
              strcat ((char *)BufferAttrValue, (char *)inputBuffer);
              EndOfHTMLAttributeValue (BufferAttrValue, lastAttrEntry,
                                       lastAttribute, lastAttrElement,
                                       UnknownAttr, &HTMLcontext,
                                       FALSE/*HTML parser*/);
              TtaFreeMemory (BufferAttrValue);
              BufferAttrValue = NULL;
              LgBufferAttrValue = 0;
              isAttrValueTruncated = FALSE;
            }
          else
            {
              EndOfHTMLAttributeValue ((char *)inputBuffer, lastAttrEntry, lastAttribute,
                                       lastAttrElement, UnknownAttr, &HTMLcontext,
                                       FALSE/*HTML parser*/);
            }
        }
      
      InitBuffer ();
    }
}

/*----------------------------------------------------------------------
  EndOfAttrValueAndTag    A ">" has been read. It indicates the
  end of an attribute value and the end of a start tag.
  ----------------------------------------------------------------------*/
static void EndOfAttrValueAndTag (char c)
{
  EndOfAttrValue (c);
  EndOfStartTag (c);
}

/*----------------------------------------------------------------------
  StartOfEntity   A character '&' has been encountered in the text.
  ----------------------------------------------------------------------*/
static void StartOfEntity (char c)
{
  LgEntityName = 0;
  EntityTableEntry = 0;
  CharRank = 0;
}

/*----------------------------------------------------------------------
  GetFallbackCharacter
  Parameter lang gives the language of the enclosing element.
  Returns the fallback string and the language.
  ----------------------------------------------------------------------*/
void GetFallbackCharacter (int code, unsigned char *fallback, Language *lang)
{  
  unsigned char *ptr;
  int	         i;

  fallback[0] = EOS;
  fallback[1] = EOS;
  fallback[2] = EOS;
  /* get the UTF-8 string of the unicode character */
  ptr = fallback;
  i = TtaWCToMBstring ((wchar_t) code, &ptr);
  fallback[i] = EOS;
}


#ifdef LC
/*----------------------------------------------------------------------
  PutAmpersandInDoc
  Put an '&' character in the document tree with an attribute
  IntEntity.
  ----------------------------------------------------------------------*/
static void PutAmpersandInDoc ()
{
  ElementType      elType;
  Element          elText;
  AttributeType    attrType;
  Attribute        attr;

  TextToDocument ();
  /* create a TEXT element for '&'*/
  elType.ElSSchema = DocumentSSchema;
  elType.ElTypeNum = HTML_EL_TEXT_UNIT;
  elText = TtaNewElement (HTMLcontext.doc, elType);
  TtaSetElementLineNumber (elText, NumberOfLinesRead);
  InsertElement (&elText);
  HTMLcontext.lastElementClosed = TRUE;
  HTMLcontext.mergeText = FALSE;
  TtaSetTextContent (elText, (unsigned char *)"&", HTMLcontext.language, HTMLcontext.doc);
  attrType.AttrSSchema = DocumentSSchema;
  attrType.AttrTypeNum = HTML_ATTR_IntEntity;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (elText, attr, HTMLcontext.doc);
  TtaSetAttributeValue (attr, HTML_ATTR_IntEntity_VAL_Yes_, elText, HTMLcontext.doc);
}
#endif /* LC */

/*----------------------------------------------------------------------
  EndOfEntity     End of a HTML entity. Search that entity in the
  entity table and put the corresponding character in the input buffer.
  ----------------------------------------------------------------------*/
static void EndOfEntity (unsigned char c)
{
  unsigned char fallback[7], *ptr;
  int           len;
  int           i;
  unsigned char msgBuffer[MaxMsgLength];

  EntityName[LgEntityName] = EOS;
  if (XhtmlEntityTable[EntityTableEntry].charName[CharRank] == EOS)
    {
      /* the entity read matches the current entry of entity table */
      if (XhtmlEntityTable[EntityTableEntry].charCode > 127)
        {
          /* generate the UTF-8 string */
          ptr = fallback;
          len = TtaWCToMBstring ((wchar_t) (XhtmlEntityTable[EntityTableEntry].charCode), &ptr);
          for (i = 0; i < len; i++)
            PutInBuffer (fallback[i]);
        }
      else
        PutInBuffer ((char)XhtmlEntityTable[EntityTableEntry].charCode);
    }
  else
    {
      /* entity not in the table. Print an error message */
      PutInBuffer ('&');
      for (i = 0; i < LgEntityName; i++)
        PutInBuffer (EntityName[i]);
      PutInBuffer (';');
      /* print an error message */
      sprintf ((char *)msgBuffer, "Unknown entity");
      HTMLParseError (HTMLcontext.doc, (char *)msgBuffer, 0);
    }
  LgEntityName = 0;
}

/*----------------------------------------------------------------------
  EntityChar      A character belonging to a HTML entity has been
  read.
  ----------------------------------------------------------------------*/
static void EntityChar (unsigned char c)
{
  unsigned char fallback[7], *ptr;
  int           len;
  unsigned char msgBuffer[MaxMsgLength];
  int           i;
  ThotBool      OK, done, stop;

  done = FALSE;
  if (XhtmlEntityTable[EntityTableEntry].charName[CharRank] == EOS)
    /* the entity name read so far matches the current entry of */
    /* entity table */
    /* does it also match the next entry? */
    {
      OK = FALSE;
      i = EntityTableEntry+1;
      stop = FALSE;
      do
        {
          if (strncmp (EntityName, XhtmlEntityTable[i].charName, LgEntityName) != 0)
            stop = TRUE;
          else if (XhtmlEntityTable[i].charName[CharRank] < c)
            i++;
          else
            {
              stop = TRUE;
              if (XhtmlEntityTable[i].charName[CharRank] == c)
                OK = TRUE;
            }
        }
      while (!stop);     
      if (!OK &&
          (c == SPACE || c == EOL || c == TAB || c == __CR__))
        {
          /* If we are not reading an attribute value, assume that semicolon is
             missing and put the corresponding char in the document content */
          EntityName[LgEntityName] = EOS;
          if (XhtmlEntityTable[EntityTableEntry].charCode > 127)
            {
              /* generate the UTF-8 string */
              ptr = fallback;
              len = TtaWCToMBstring ((wchar_t) (XhtmlEntityTable[EntityTableEntry].charCode), &ptr);
              for (i = 0; i < len; i++)
                PutInBuffer (fallback[i]);
            }
          else
            PutInBuffer ((char)(XhtmlEntityTable[EntityTableEntry].charCode));
          if (c != SPACE)
            /* print an error message */
            HTMLParseError (HTMLcontext.doc, "Missing semicolon", 0);
          /* next state is the return state from the entity subautomaton, not
             the state computed by the automaton. In addition the character read
             has not been processed yet */
          NormalTransition = FALSE;
          currentState = returnState;
          /* end of entity */
          LgEntityName = 0;
          done = TRUE;
        }
    }
  
  if (!done)
    {
      while (XhtmlEntityTable[EntityTableEntry].charName[CharRank] < c
             && XhtmlEntityTable[EntityTableEntry].charCode != 0)
        EntityTableEntry++;
      if (XhtmlEntityTable[EntityTableEntry].charName[CharRank] != c)
        OK = FALSE;
      else
        {
          if (LgEntityName > 0 &&
              strncmp (EntityName,
                       XhtmlEntityTable[EntityTableEntry].charName,
                       LgEntityName) != 0)
            OK = FALSE;
          else
            {
              OK = TRUE;
              CharRank++;
              if (LgEntityName < MaxEntityLength - 1)
                EntityName[LgEntityName++] = c;
            }
        }
      if (!OK)
        {
          /* the entity name read so far is not in the table */
          /* invalid entity */
          /* put the entity name in the buffer */
          PutInBuffer ('&');
          for (i = 0; i < LgEntityName; i++)
            PutInBuffer (EntityName[i]);
          /* print an error message only if it's not the first character
             after '&' or if it is a letter */
          if (LgEntityName > 0 ||
              ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))
            {
              /* print an error message */
              EntityName[LgEntityName++] = c;
              EntityName[LgEntityName++] = EOS;
              sprintf ((char *)msgBuffer, "Unknown entity");
              HTMLParseError (HTMLcontext.doc, (char *)msgBuffer, 0);
            }
          /* next state is the return state from the entity subautomaton,
             not the state computed by the automaton.
             In addition the character read has not been processed yet */
          NormalTransition = FALSE;
          currentState = returnState;
          /* end of entity */
          LgEntityName = 0;
        }
    }
}

/*----------------------------------------------------------------------
  EndOfDecEntity  End of a decimal entity. Convert the
  string read into a number and put the character
  having that code in the input buffer.
  ----------------------------------------------------------------------*/
static void EndOfDecEntity (unsigned char c)
{
  unsigned char fallback[7], *ptr;
  int           len;
  int           code;
  int           i;

  EntityName[LgEntityName] = EOS;
  sscanf (EntityName, "%d", &code);
  if (code > 127)
    {
      /* generate the UTF-8 string */
      ptr = fallback;
      len = TtaWCToMBstring ((wchar_t) code, &ptr);
      for (i = 0; i < len; i++)
        PutInBuffer (fallback[i]);
    }
  else
    PutInBuffer ((char) code);
  LgEntityName = 0;
}

/*----------------------------------------------------------------------
  DecEntityChar   A character belonging to a decimal entity has been read.
  Put that character in the entity buffer.
  ----------------------------------------------------------------------*/
static void DecEntityChar (unsigned char c)
{
  int		i;

  if (LgEntityName < MaxEntityLength - 1)
    {
      /* the entity buffer is not full */
      if (c >= '0' && c <= '9')
        /* the character is a decimal digit */
        EntityName[LgEntityName++] = c;
      else
        /* not a decimal digit. assume end of entity */
        {
          if (c == '<')
            /* accept start of tag as an end of entity */
            EndOfDecEntity (c);
          else
            {
              PutInBuffer ('&');
              PutInBuffer ('#');
              for (i = 0; i < LgEntityName; i++)
                PutInBuffer (EntityName[i]);
              LgEntityName = 0;
              /* error message */
              HTMLParseError (HTMLcontext.doc, "Invalid decimal entity", 0);
            }
          /* next state is state 0, not the state computed by the automaton */
          /* and the character read has not been processed yet */
          NormalTransition = FALSE;
          currentState = 0;
        }
    }
}

/*----------------------------------------------------------------------
  EndOfHexEntity  End of an hexadecimal entity. Convert the
  string read into a number and put the character
  having that code in the input buffer.
  ----------------------------------------------------------------------*/
static void EndOfHexEntity (unsigned char c)
{
  unsigned char fallback[7], *ptr;
  int           len;
  int           code;
  int           i;

  EntityName[LgEntityName] = EOS;
  sscanf (EntityName, "%x", &code);
  if (code > 127)
    {
      /* generate the UTF-8 string */
      ptr = fallback;
      len = TtaWCToMBstring ((wchar_t) code, &ptr);
      for (i = 0; i < len; i++)
        PutInBuffer (fallback[i]);
    }
  else
    PutInBuffer ((char) code);
  LgEntityName = 0;
}

/*----------------------------------------------------------------------
  HexEntityChar   A character belonging to an hexadecimal entity has been
  read. Put that character in the entity buffer.
  ----------------------------------------------------------------------*/
static void     HexEntityChar (char c)
{
  int		i;

  if (LgEntityName < MaxEntityLength - 1)
    {
      /* the entity buffer is not full */
      if ((c >= '0' && c <= '9') ||
          (c >= 'a' && c <= 'f') ||
          (c >= 'A' && c <= 'F'))
        /* the character is a valid hexadecimal digit */
        EntityName[LgEntityName++] = c;
      else
        {
          /* not an hexadecimal digit. Assume end of entity */
          if (c == '<')
            /* accept start of tag as the end of the entity */
            EndOfHexEntity (c);
          else
            /* error */
            {
              PutInBuffer ('&');
              PutInBuffer ('#');
              PutInBuffer ('x');
              for (i = 0; i < LgEntityName; i++)
                PutInBuffer (EntityName[i]);
              LgEntityName = 0;
              /* error message */
              HTMLParseError (HTMLcontext.doc, "Invalid hexadecimal entity", 0);
            }
          /* next state is state 0, not the state computed by the automaton */
          /* and the character read has not been processed yet */
          NormalTransition = FALSE;
          currentState = 0;
        }
    }
}

/*----------------------------------------------------------------------
  EndOfDocument   End of the HTML file. Terminate the Thot
  document
  ----------------------------------------------------------------------*/
static void         EndOfDocument ()
{
  if (LgBuffer > 0)
    TextToDocument ();
}

/*----------------------------------------------------------------------
  PutLess put '<' in the input buffer
  ----------------------------------------------------------------------*/
static void         PutLess (char c)
{
  PutInBuffer ('<');
}

/*----------------------------------------------------------------------
  PutAmpersandSpace       put '& ' in the input buffer.
  ----------------------------------------------------------------------*/
static void         PutAmpersandSpace (char c)
{
  PutInBuffer ('&');
  PutInBuffer (SPACE);
}

/*----------------------------------------------------------------------
  PutLessAndSpace put '<' and the space read in the input buffer.
  ----------------------------------------------------------------------*/
static void         PutLessAndSpace (char c)
{
  PutInBuffer ('<');
  PutInBuffer (c);
}


/*----------------------------------------------------------------------
  StartOfComment  Beginning of a HTML comment.
  ----------------------------------------------------------------------*/
static void StartOfComment (char c)
{
  ElementType      elType;
  Element          elComment, elCommentLine;

  /* create a Thot element Comment */
  elType.ElSSchema = DocumentSSchema;
  elType.ElTypeNum = HTML_EL_Comment_;
  elComment = TtaNewElement (HTMLcontext.doc, elType);
  TtaSetElementLineNumber (elComment, NumberOfLinesRead);
  InsertElement (&elComment);
  /* create a Comment_line element as the first child of */
  /* element Comment */
  if (elComment != NULL)
    {
      elType.ElTypeNum = HTML_EL_Comment_line;
      elCommentLine = TtaNewElement (HTMLcontext.doc, elType);
      TtaSetElementLineNumber (elCommentLine, NumberOfLinesRead);
      TtaInsertFirstChild (&elCommentLine, elComment, HTMLcontext.doc);
      /* create a TEXT element as the first child of element Comment_line */
      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
      CommentText = TtaNewElement (HTMLcontext.doc, elType);
      TtaSetElementLineNumber (CommentText, NumberOfLinesRead);
      TtaInsertFirstChild (&CommentText, elCommentLine, HTMLcontext.doc);
      TtaSetTextContent (CommentText, (unsigned char *)"", HTMLcontext.language,
                         HTMLcontext.doc);
    }
  InitBuffer ();
}

/*----------------------------------------------------------------------
  PutInComment    put character c in the current HTML comment.
  ----------------------------------------------------------------------*/
static void PutInComment (unsigned char c)
{
  ElementType       elType;
  Element           elCommentLine, prevElCommentLine;
  
  if (c != EOS)
    {
      if (!HTMLcontext.parsingCSS && ((int) c == EOL || (int) c == __CR__))
        /* new line in a comment */
        {
          /* put the content of the inputBuffer into the current */
          /* Comment_line element */
          CloseBuffer ();
          TtaAppendTextContent (CommentText, (unsigned char *)inputBuffer, HTMLcontext.doc);
          InitBuffer ();
          /* create a new Comment_line element */
          elType.ElSSchema = DocumentSSchema;
          elType.ElTypeNum = HTML_EL_Comment_line;
          elCommentLine = TtaNewElement (HTMLcontext.doc, elType);
          TtaSetElementLineNumber (elCommentLine, NumberOfLinesRead);
          /* inserts the new Comment_line element after the previous one */
          prevElCommentLine = TtaGetParent (CommentText);
          TtaInsertSibling (elCommentLine, prevElCommentLine, FALSE, HTMLcontext.doc);
          /* create a TEXT element as the first child of the new element
             Comment_line */
          elType.ElTypeNum = HTML_EL_TEXT_UNIT;
          CommentText = TtaNewElement (HTMLcontext.doc, elType);
          TtaSetElementLineNumber (CommentText, NumberOfLinesRead);
          TtaInsertFirstChild (&CommentText, elCommentLine, HTMLcontext.doc);
          TtaSetTextContent (CommentText, (unsigned char *)"", HTMLcontext.language, HTMLcontext.doc);
        }
      else
        {
          if (LgBuffer >= MaxBufferLength - 1)
            {
              CloseBuffer ();
              TtaAppendTextContent (CommentText, (unsigned char *)inputBuffer,
                                    HTMLcontext.doc);
              InitBuffer ();
            }
          inputBuffer[LgBuffer++] = c;
        }
    }
}

/*----------------------------------------------------------------------
  EndOfComment    End of a HTML comment.
  ----------------------------------------------------------------------*/
static void EndOfComment (char c)
{
  if (LgBuffer > 0)
    {
      CloseBuffer ();
      if (CommentText != NULL)
        TtaAppendTextContent (CommentText, (unsigned char *)inputBuffer,
                              HTMLcontext.doc);
    }
  CommentText = NULL;
  HTMLcontext.lastElementClosed = TRUE;
  InitBuffer ();
}

/*----------------------------------------------------------------------
  StartOfASP  Beginning of a HTML ASP
  ----------------------------------------------------------------------*/
static void StartOfASP (char c)
{
  ElementType      elType;
  Element          elASP, elASPLine;

  /* create a Thot element ASP */
  elType.ElSSchema = DocumentSSchema;
  elType.ElTypeNum = HTML_EL_ASP_element;
  elASP = TtaNewElement (HTMLcontext.doc, elType);
  TtaSetElementLineNumber (elASP, NumberOfLinesRead);
  InsertElement (&elASP);
  /* create a ASP_line element as the first child of */
  /* element ASP */
  if (elASP != NULL)
    {
      elType.ElTypeNum = HTML_EL_ASP_line;
      elASPLine = TtaNewElement (HTMLcontext.doc, elType);
      TtaSetElementLineNumber (elASPLine, NumberOfLinesRead);
      TtaInsertFirstChild (&elASPLine, elASP, HTMLcontext.doc);
      /* create a TEXT element as the first child of element ASP_line */
      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
      ASPText = TtaNewElement (HTMLcontext.doc, elType);
      TtaSetElementLineNumber (ASPText, NumberOfLinesRead);
      TtaInsertFirstChild (&ASPText, elASPLine, HTMLcontext.doc);
      TtaSetTextContent (ASPText, (unsigned char *)"", HTMLcontext.language,
                         HTMLcontext.doc);
    }
  InitBuffer ();
}

/*----------------------------------------------------------------------
  PutInASP    put character c in the current HTML ASP
  ----------------------------------------------------------------------*/
static void PutInASP (unsigned char c)
{
  ElementType       elType;
  Element           elASPLine, prevElASPLine;
  
  if (c != EOS)
    {
      if (!HTMLcontext.parsingCSS && ((int) c == EOL || (int) c == __CR__))
        /* new line in a ASP */
        {
          /* put the content of the inputBuffer into the current */
          /* ASP_line element */
          CloseBuffer ();
          TtaAppendTextContent (ASPText, (unsigned char *)inputBuffer, HTMLcontext.doc);
          InitBuffer ();
          /* create a new ASP_line element */
          elType.ElSSchema = DocumentSSchema;
          elType.ElTypeNum = HTML_EL_ASP_line;
          elASPLine = TtaNewElement (HTMLcontext.doc, elType);
          TtaSetElementLineNumber (elASPLine, NumberOfLinesRead);
          /* inserts the new ASP_line element after the previous one */
          prevElASPLine = TtaGetParent (ASPText);
          TtaInsertSibling (elASPLine, prevElASPLine, FALSE, HTMLcontext.doc);
          /* create a TEXT element as the first child of the new element
             ASP_line */
          elType.ElTypeNum = HTML_EL_TEXT_UNIT;
          ASPText = TtaNewElement (HTMLcontext.doc, elType);
          TtaSetElementLineNumber (ASPText, NumberOfLinesRead);
          TtaInsertFirstChild (&ASPText, elASPLine, HTMLcontext.doc);
          TtaSetTextContent (ASPText, (unsigned char *)"", HTMLcontext.language, HTMLcontext.doc);
        }
      else
        {
          if (LgBuffer >= MaxBufferLength - 1)
            {
              CloseBuffer ();
              TtaAppendTextContent (ASPText, (unsigned char *)inputBuffer,
                                    HTMLcontext.doc);
              InitBuffer ();
            }
          inputBuffer[LgBuffer++] = c;
        }
    }
}

/*----------------------------------------------------------------------
  EndOfASP    End of a HTML ASP
  ----------------------------------------------------------------------*/
static void EndOfASP (char c)
{
  if (LgBuffer > 0)
    {
      CloseBuffer ();
      if (ASPText != NULL)
        TtaAppendTextContent (ASPText, (unsigned char *)inputBuffer,
                              HTMLcontext.doc);
    }
  ASPText = NULL;
  HTMLcontext.lastElementClosed = TRUE;
  InitBuffer ();
}

/*----------------------------------------------------------------------
  PutDash put a dash character in the current comment.
  ----------------------------------------------------------------------*/
static void PutDash (char c)
{
  PutInComment ('-');
  PutInComment (c);
}

/*----------------------------------------------------------------------
  PutDashDash     put 2 dash characters in the current comment.
  ----------------------------------------------------------------------*/
static void         PutDashDash (char c)
{
  PutInComment ('-');
  PutInComment ('-');
  PutInComment (c);
}

/*----------------------------------------------------------------------
  PutQuestionMark put a question mark in the current PI.
  ----------------------------------------------------------------------*/
static void PutQuestionMark (char c)
{
  PutInBuffer ('?');
  PutInBuffer (c);
}

/*----------------------------------------------------------------------
  EndOfDoctypeDecl	A Doctype declaration has been read
  ----------------------------------------------------------------------*/
static void EndOfDoctypeDecl (char c)
{
  int	          i, j;  
  Element         docEl, text, doctype, prev, doctypeLine;
  ElementType     elType;
  unsigned char  *buffer;

  CloseBuffer ();
  buffer = (unsigned char*)TtaGetMemory (strlen ((char *)inputBuffer) + 20);
  strcpy ((char *)buffer, (char *)"<!DOCTYPE ");
  j = strlen ((char *)buffer);
  /* process the Doctype declaration available in inputBuffer */
  if (!strncasecmp ((char *)inputBuffer, "doctype", 7))
    {
      for (i = 7; inputBuffer[i] <= SPACE && inputBuffer[i] != EOS; i++);
      if (!strncasecmp ((char *)&inputBuffer[i], "HTML", 4))
        /* it's a HTML document */
        {
          docEl = TtaGetMainRoot (HTMLcontext.doc);
          elType = TtaGetElementType (docEl);
          /* Create a DOCTYPE element */
          elType.ElTypeNum = HTML_EL_DOCTYPE;
          doctype = TtaNewElement (HTMLcontext.doc, elType);
          TtaSetElementLineNumber (doctype, NumberOfLinesRead);
          InsertElement (&doctype);
          /* Make the DOCTYPE element read-only */
          TtaSetAccessRight (doctype, ReadOnly, HTMLcontext.doc);
          HTMLcontext.lastElement = doctype;
          HTMLcontext.lastElementClosed = TRUE;
          /* Create a DOCTYPE_line element as first child */
          elType.ElTypeNum = HTML_EL_DOCTYPE_line;
          doctypeLine = TtaNewElement (HTMLcontext.doc, elType);
          TtaSetElementLineNumber (doctypeLine, NumberOfLinesRead);
          TtaInsertFirstChild (&doctypeLine, doctype, HTMLcontext.doc);
          /* Look for line breaks in the input buffer  and create */
          /* as many DOCTYPE_line elements as needed */
          while (inputBuffer[i] != EOS)
            {
              if (inputBuffer[i] != EOL && inputBuffer[i] != __CR__)
                buffer[j++] = inputBuffer[i];
              else
                {
                  buffer[j] = EOS;
                  j = 0;
                  elType.ElTypeNum = 1;
                  text = TtaNewElement (HTMLcontext.doc, elType);
                  if (text != NULL)
                    {
                      TtaSetElementLineNumber (text, NumberOfLinesRead);
                      /* get the position of the Doctype text */
                      TtaInsertFirstChild (&text, doctypeLine, HTMLcontext.doc);

                      /* We use the Latin_Script language to avoid the spell_checker */
                      /* the spell_chekcer to check the doctype */
                      TtaSetTextContent (text, (unsigned char *)buffer, Latin_Script, HTMLcontext.doc);
                    }
                  /* Create a new DOCTYPE_line element */
                  elType.ElTypeNum = HTML_EL_DOCTYPE_line;
                  prev = doctypeLine;
                  doctypeLine = TtaNewElement (HTMLcontext.doc, elType);
                  if (doctypeLine != NULL)
                    {
                      TtaSetElementLineNumber (doctypeLine, NumberOfLinesRead);
                      TtaInsertSibling (doctypeLine, prev, FALSE, HTMLcontext.doc);
                    }		   
                }
              i++;
            }
          buffer [j++] = '>';
          buffer [j] = EOS;
          elType.ElTypeNum = 1;
          text = TtaNewElement (HTMLcontext.doc, elType);
          if (text)
            {
              TtaSetElementLineNumber (text, NumberOfLinesRead);
              /* get the position of the Doctype text */
              TtaInsertFirstChild (&text, doctypeLine, HTMLcontext.doc);
              /* We use the Latin_Script language to avoid the spell_chekcer */
              /* the spell_chekcer to check the doctype */
              TtaSetTextContent (text, (unsigned char *)buffer, Latin_Script, HTMLcontext.doc);
            }
        }
    }
   
  InitBuffer ();
  TtaFreeMemory (buffer);
}


/*----------------------------------------------------------------------
  StartOfPI  Beginning of a HTML comment.
  ----------------------------------------------------------------------*/
static void         StartOfPI (char c)
{
  ElementType      elType;
  Element          elPI, elPILine;

  /* create a Thot element PI */
  elType.ElSSchema = DocumentSSchema;
  elType.ElTypeNum = HTML_EL_XMLPI;
  elPI = TtaNewElement (HTMLcontext.doc, elType);
  TtaSetElementLineNumber (elPI, NumberOfLinesRead);
  InsertElement (&elPI);
  /* create a PI_line element as the first child of */
  /* element PI */
  if (elPI != NULL)
    {
      elType.ElTypeNum = HTML_EL_PI_line;
      elPILine = TtaNewElement (HTMLcontext.doc, elType);
      TtaSetElementLineNumber (elPILine, NumberOfLinesRead);
      TtaInsertFirstChild (&elPILine, elPI, HTMLcontext.doc);
      /* create a TEXT element as the first child of element PI_line */
      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
      PIText = TtaNewElement (HTMLcontext.doc, elType);
      TtaSetElementLineNumber (PIText, NumberOfLinesRead);
      TtaInsertFirstChild (&PIText, elPILine, HTMLcontext.doc);
      TtaSetTextContent (PIText, (unsigned char *)"", HTMLcontext.language,
                         HTMLcontext.doc);
    }
  InitBuffer ();
}

/*----------------------------------------------------------------------
  PutInPI    put character c in the current HTML comment.
  ----------------------------------------------------------------------*/
static void         PutInPI (unsigned char c)
{
  ElementType       elType;
  Element           elPILine, prevElPILine;
  
  if (c != EOS)
    {
      if (!HTMLcontext.parsingCSS && ((int) c == EOL || (int) c == __CR__))
        /* new line in a comment */
        {
          /* put the content of the inputBuffer into the current */
          /* PI_line element */
          CloseBuffer ();
          TtaAppendTextContent (PIText, (unsigned char *)inputBuffer, HTMLcontext.doc);
          InitBuffer ();
          /* create a new PI_line element */
          elType.ElSSchema = DocumentSSchema;
          elType.ElTypeNum = HTML_EL_PI_line;
          elPILine = TtaNewElement (HTMLcontext.doc, elType);
          TtaSetElementLineNumber (elPILine, NumberOfLinesRead);
          /* inserts the new PI_line element after the previous one */
          prevElPILine = TtaGetParent (PIText);
          TtaInsertSibling (elPILine, prevElPILine, FALSE, HTMLcontext.doc);
          /* create a TEXT element as the first child of the new element
             PI_line */
          elType.ElTypeNum = HTML_EL_TEXT_UNIT;
          PIText = TtaNewElement (HTMLcontext.doc, elType);
          TtaSetElementLineNumber (PIText, NumberOfLinesRead);
          TtaInsertFirstChild (&PIText, elPILine, HTMLcontext.doc);
          TtaSetTextContent (PIText, (unsigned char *)"", HTMLcontext.language, HTMLcontext.doc);
        }
      else
        {
          if (LgBuffer >= MaxBufferLength - 1)
            {
              CloseBuffer ();
              TtaAppendTextContent (PIText, (unsigned char *)inputBuffer,
                                    HTMLcontext.doc);
              InitBuffer ();
            }
          inputBuffer[LgBuffer++] = c;
        }
    }
}

/*----------------------------------------------------------------------
  EndOfPI    End of a HTML PI
  ----------------------------------------------------------------------*/
static void         EndOfPI (char c)
{
  if (LgBuffer > 0)
    {
      CloseBuffer ();
      if (PIText != NULL)
        TtaAppendTextContent (PIText, (unsigned char *)inputBuffer,
                              HTMLcontext.doc);
    }
  PIText = NULL;
  HTMLcontext.lastElementClosed = TRUE;
  InitBuffer ();
}


/*----------------------------------------------------------------------
  Do_nothing does nothing.
  ----------------------------------------------------------------------*/
static void Do_nothing (char c)
{
}

/* some type definitions for the automaton */
typedef struct _Transition *PtrTransition;
typedef struct _Transition
{				/* a transition of the automaton in
				   "executable" form */
  unsigned char       trigger;	/* the imput character that triggers
                                   the transition */
  Proc                action;	/* the procedure to be called when
                                 the transition occurs */
  State               newState;	/* the new state of the automaton
                                   after the transition */
  PtrTransition       nextTransition;	/* next transition from the same
                                         state */
} 
Transition;

typedef struct _StateDescr
{				/* a state of the automaton */
  State               automatonState;	/* the state */
  PtrTransition       firstTransition;	/* first transition from that state */
}
StateDescr;

/* the automaton that drives the HTML parser */
#define MaxState 40
static StateDescr        automaton[MaxState];

typedef struct _sourceTransition
{				/* a transition of the automaton in
				   "source" form */
  State               initState;	/* initial state of transition */
  char                trigger;	/* the imput character that triggers
                                   the transition */
  Proc                transitionAction;	/* the procedure to be called when
                                           the transition occurs */
  State               newState;	/* final state of the transition */
}
sourceTransition;

/* the automaton in "source" form */
static sourceTransition sourceAutomaton[] =
  {
    /*
      state, trigger, action, new state
    */
    /* state 0: reading character data */
    {0, '<', (Proc) StartOfTag, 1},
    {0, '&', (Proc) StartOfEntity, -30},		/* call subautomaton 30 */
    {0, '*', (Proc) PutInBuffer, 0},	/*  * = any other character */
    /* state 1: '<' has been read */
    {1, '%', (Proc) StartOfASP, 35},
    {1, '/', (Proc) Do_nothing, 3},
    {1, '!', (Proc) Do_nothing, 10},
    {1, '?', (Proc) StartOfPI, 20},
    {1, '<', (Proc) Do_nothing, 18},
    {1, 'S', (Proc) PutLessAndSpace, 0},		/*   S = Space */
    {1, '*', (Proc) PutInBuffer, 2},
    /* state 2: reading a start tag */
    {2, '>', (Proc) EndOfStartGIandTag, 0},
    {2, '<', (Proc) EndOfStartGIandTag, 1},     /* Error: tag not closed */
    {2, '&', (Proc) StartOfEntity, -30},		/* call subautomaton 30 */
    {2, 'S', (Proc) EndOfStartGI, 16},	/*   S = Space */
    {2, '*', (Proc) PutInBuffer, 2},
    /* state 3: reading an end tag */
    {3, '>', (Proc) EndOfEndTag, 0},
    {3, '<', (Proc) EndOfEndTag, 1},             /* Error: tag not closed */
    {3, '&', (Proc) StartOfEntity, -30},		/* call subautomaton 30 */
    {3, 'S', (Proc) Do_nothing, 3},
    {3, '*', (Proc) PutInBuffer, 3},
    /* state 4: reading an attribute name */
    {4, '=', (Proc) EndOfAttrName, 5},
    {4, 'S', (Proc) EndOfAttrName, 17},
    {4, '&', (Proc) StartOfEntity, -30},		/* call subautomaton 30 */
    {4, '>', (Proc) EndOfAttrNameAndTag, 0},
    {4, '<', (Proc) EndOfAttrNameAndTag, 1},     /* Error: tag not closed */
    {4, '*', (Proc) PutInBuffer, 4},
    /* state 5: expecting an attribute value */
    {5, '\"', (Proc) StartOfQuotedAttrValue, 6},
    {5, '\'', (Proc) StartOfQuotedAttrValue, 9},
    {5, 'S', (Proc) Do_nothing, 5},
    {5, '>', (Proc) EndOfStartTag, 0},
    {5, '*', (Proc) StartOfUnquotedAttrValue, 7},
    /* state 6: reading an attribute value between double quotes */
    {6, '\"', (Proc) EndOfAttrValue, 8},
    {6, '&', (Proc) StartOfEntity, -30},		/* call subautomaton 30... */
    {6, '*', (Proc) PutInBuffer, 6},
    /* state 7: reading an attribute value without delimiting quotes */
    {7, '>', (Proc) EndOfAttrValueAndTag, 0},
    {7, 'S', (Proc) EndOfAttrValue, 16},
    {7, '&', (Proc) StartOfEntity, -30},		/* call subautomaton 30 */
    {7, '*', (Proc) PutInBuffer, 7},
    /* state 8: end of attribute value */
    {8, '>', (Proc) EndOfStartTag, 0},
    {8, '<', (Proc) EndOfStartTag, 1},     /* Error: tag not closed */
    {8, 'S', (Proc) Do_nothing, 16},
    {8, '*', (Proc) PutInBuffer, 4},
    /* state 9: reading an attribute value between simple quotes */
    {9, '\'', (Proc) EndOfAttrValue, 8},
    {9, '&', (Proc) StartOfEntity, -30},		/* call subautomaton 30 */
    {9, '*', (Proc) PutInBuffer, 9},
    /* state 10: "<!" has been read */
    {10, '-', (Proc) Do_nothing, 11},
    {10, '[', (Proc) Do_nothing, 23},		/* call subautomaton 23 */
    {10, 'S', (Proc) Do_nothing, 10},
    {10, '>', (Proc) Do_nothing, 0},            /* weird empty comment <!> */
    {10, '*', (Proc) PutInBuffer, 15},
    /* state 11: "<!-" has been read. Probably a comment */
    {11, '-', (Proc) StartOfComment, 12},
    {11, '*', (Proc) PutInBuffer, 15},	/* incorrect comment, expect */
    /* a closing '>' */
    /* state 12: reading a comment */
    {12, '-', (Proc) Do_nothing, 13},
    {12, '*', (Proc) PutInComment, 12},
    /* state 13: a dash "-" has been read within a comment */
    {13, '-', (Proc) Do_nothing, 14},
    {13, '*', (Proc) PutDash, 12},
    /* state 14: a double dash "--" has been read within a comment */
    {14, 'S', (Proc) Do_nothing, 14},
    {14, '>', (Proc) EndOfComment, 0},
    {14, '-', (Proc) PutInComment, 14},
    {14, '*', (Proc) PutDashDash, 12},
    /* state 15: '<!' has been read. It may be a doctype declaration */
    {15, '>', (Proc) EndOfDoctypeDecl, 0},
    {15, '*', (Proc) PutInBuffer, 15},
    /* state 16: expecting an attribute name or an end of start tag */
    {16, 'S', (Proc) Do_nothing, 16},
    {16, '>', (Proc) EndOfStartTag, 0},
    {16, '<', (Proc) EndOfStartTag, 1},     /* Error: tag not closed */
    {16, '*', (Proc) PutInBuffer, 4},
    /* state 17: expecting '=' after an attribute name */
    {17, 'S', (Proc) Do_nothing, 17},
    {17, '=', (Proc) Do_nothing, 5},
    {17, '>', (Proc) EndOfStartTag, 0},
    {17, '*', (Proc) PutInBuffer, 4},
    /* state 18: '<' has been read */
    {18, '!', (Proc) Do_nothing, 19},
    {18, '*', (Proc) Do_nothing, 0},
    /* state 19: '<!' has been read */
    {19, '>', (Proc) PutLess, 0},
    {19, '*', (Proc) Do_nothing, 0},
    /* state 20: "<?" has been read; beginning of a Processing Instruction */
    {20, '?', (Proc) Do_nothing, 21},
    {20, '*', (Proc) PutInPI, 20},
    /* state 21: reading the end of Processing Instruction? */
    {21, '>', (Proc) EndOfPI, 0},
    {21, '*', (Proc) PutQuestionMark, 20},
    /* state 22: a question mark has been read in a Processing Instruction */
    {22, '>', (Proc) EndOfPI, 0},
    {22, '*', (Proc) PutQuestionMark, 20},
    /* state 23: "<![*" has been read, wait for CDATA */
    {23, '[', (Proc) StartCData, 24},
    {23, 'C', (Proc) PutInBuffer, 23},
    {23, 'D', (Proc) PutInBuffer, 23},
    {23, 'A', (Proc) PutInBuffer, 23},
    {23, 'T', (Proc) PutInBuffer, 23},
    {23, ']', (Proc) PutInBuffer, -1},
    {23, '*', (Proc) PutInBuffer, 15},
    /* state 24: "<![CDATA[" has been read: read its contents */
    {24, ']', (Proc) PutInBuffer, 25},
    {24, '\n', (Proc) EndOfCDataLine, 24},
    {24, '*', (Proc) PutInBuffer, 24},
    /* state 25: "]" has been read: check the second "]" */
    {25, ']', (Proc) PutInBuffer, 26},
    {25, '*', (Proc) PutInBuffer, 24},
    /* state 26: "]]" has been read: check the end of CDATA */
    {26, ']', (Proc) PutInBuffer, 26},
    {26, '>', (Proc) EndOfCData, 0},
    {26, '*', (Proc) PutInBuffer, 24},

    /* sub automaton for reading entities in various contexts */
    /* state -1 means "return to calling state" */
    /* state 30: a '&' has been read */
    {30, '#', (Proc) Do_nothing, 32},
    {30, 'S', (Proc) PutAmpersandSpace, -1},	/* return to calling state */
    {30, '*', (Proc) EntityChar, 31},
    /* state 31: reading a string entity */
    {31, ';', (Proc) EndOfEntity, -1},	/* return to calling state */
    {31, '*', (Proc) EntityChar, 31},
    /* state 32: "&#" has been read: reading a numerical entity */
    {32, 'x', (Proc) Do_nothing, 34},
    {32, 'X', (Proc) Do_nothing, 34},
    {32, '*', (Proc) DecEntityChar, 33},
    /* state 33: "&#x" has been read: reading a decimal value */
    {33, ';', (Proc) EndOfDecEntity, -1},	/* return to calling state */
    {33, '*', (Proc) DecEntityChar, 33},
    /* state 34: "&#x" has been read: reading an hexadecimal value */
    {34, ';', (Proc) EndOfHexEntity, -1},	/* return to calling state */
    {34, '*', (Proc) HexEntityChar, 34},
    /* state 35: reading a ASP */
    {35, '%', (Proc) PutInASP, 36},
    {35, '*', (Proc) PutInASP, 35},
    /* state 36: reading a ASP */
    {36, '>', (Proc) EndOfASP, 0},
    {36, '*', (Proc) PutInASP, 35},

    /* state 1000: fake state. End of automaton table */
    /* the next line must be the last one in the automaton declaration */
    {1000, '*', (Proc) Do_nothing, 1000}
  };

/*----------------------------------------------------------------------
  InitAutomaton   read the "source" form of the automaton and
  build the "executable" form.
  ----------------------------------------------------------------------*/
void InitAutomaton (void)
{
  int                 entry;
  State               theState;
  State               curState;
  PtrTransition       trans;
  PtrTransition       prevTrans;

  for (entry = 0; entry < MaxState; entry++)
    automaton[entry].firstTransition = NULL;
  entry = 0;
  curState = 1000;
  prevTrans = NULL;
  do
    {
      theState = sourceAutomaton[entry].initState;
      if (theState < 1000)
        {
          trans = (PtrTransition) TtaGetMemory (sizeof (Transition));
          trans->nextTransition = NULL;
          trans->trigger = sourceAutomaton[entry].trigger;
          trans->action = sourceAutomaton[entry].transitionAction;
          trans->newState = sourceAutomaton[entry].newState;
          if (trans->trigger == 'S')		/* any spacing character */
            trans->trigger = SPACE;
          if (trans->trigger == '*')		/* any character */
            trans->trigger = EOS;
          if (theState != curState)
            {
              automaton[theState].automatonState = theState;
              automaton[theState].firstTransition = trans;
              curState = theState;
            }
          else
            prevTrans->nextTransition = trans;
          prevTrans = trans;
          entry++;
        }
    }
  while (theState < 1000);
}

/*----------------------------------------------------------------------
  FreeHTMLParser
  Frees all ressources associated with the HTML parser.
  ----------------------------------------------------------------------*/
void FreeHTMLParser (void)
{
  PtrTransition       trans, nextTrans;
  PtrClosedElement    pClose, nextClose;
  int		       entry;

  /* free the internal representation of the automaton */
  for (entry = 0; entry < MaxState; entry++)
    {
      trans = automaton[entry].firstTransition;
      while (trans != NULL)
        {
          nextTrans = trans->nextTransition;
          TtaFreeMemory (trans);
          trans = nextTrans;
        }
    }

  /* free descriptors of elements closed by a start tag */
  for (entry = 0; pHTMLGIMapping[entry].XMLname[0] != EOS; entry++)
    {
      pClose = FirstClosedElem[entry];
      while (pClose != NULL)
        {
          nextClose = pClose->nextClosedElem;
          TtaFreeMemory (pClose);
          pClose = nextClose;
        }
    }
  TtaFreeMemory (FirstClosedElem);
  FirstClosedElem = NULL;
}



/*----------------------------------------------------------------------
  GetNextHTMLbuffer returns the next buffer to be parsed and update
  global variables.
  ----------------------------------------------------------------------*/
void GetNextHTMLbuffer (FILE *infile, ThotBool *endOfFile,
                        char **buff, int *lastchar)
{
  CHARSET      charset =  TtaGetDocumentCharset (HTMLcontext.doc);
  int          res;

  // copy last treated characters
  if (StartOfTagIndx > 0)
    {
      strncpy (PreviousRead, &WorkBuffer[StartOfTagIndx], PREV_READ_CHARS);
      LastCharInPreviousRead = LastCharInWorkBuffer - StartOfTagIndx;
      StartOfTagIndx = 0;
    }
  else
    {
      PreviousRead[0] = EOS;
      LastCharInPreviousRead = 0;
    }

  *buff = NULL;
  // free previous translation buffer
  if (WorkBuffer !=  FileBuffer)
    {
      TtaFreeMemory (WorkBuffer);
      WorkBuffer = FileBuffer;
    }
  // need to read a new set of characters
  LastCharInWorkBuffer = gzread (infile, &FileBuffer[StartOfRead],
                                 INPUT_FILE_BUFFER_SIZE - StartOfRead);
  // add previous read characters not managed yet
  LastCharInWorkBuffer += StartOfRead;
  StartOfRead = 0;
  if (LastCharInWorkBuffer <= 0)
    {
      /* error or end of file */
      *endOfFile = TRUE;
      LastCharInWorkBuffer = 0;
    }
  else
    {
      FileBuffer[LastCharInWorkBuffer] = EOS;
      LastCharInWorkBuffer--;
      if (charset == ISO_8859_2   || charset == ISO_8859_3   ||
          charset == ISO_8859_4   || charset == ISO_8859_5   ||
          charset == ISO_8859_6   || charset == ISO_8859_7   ||
          charset == ISO_8859_8   || charset == ISO_8859_9   ||
          charset == ISO_8859_15  || charset == KOI8_R       ||
          charset == WINDOWS_1250 || charset == WINDOWS_1251 ||
          charset == WINDOWS_1252 || charset == WINDOWS_1253 ||
          charset == WINDOWS_1254 || charset == WINDOWS_1255 ||
          charset == WINDOWS_1256 || charset == WINDOWS_1257 ||
          charset == ISO_2022_JP  || charset == EUC_JP       ||
          charset == SHIFT_JIS    || charset == GB_2312)
        {
          /* convert the original stream into UTF-8 */
          *buff = (char *)TtaConvertByteToMbsWithCheck ((const unsigned char *)FileBuffer,
                                                        charset, &res);
          HTMLcontext.encoding = UTF_8;
          if (*buff)
            {
              WorkBuffer = *buff;
              if (res > 0 && res < INPUT_FILE_BUFFER_SIZE)
                {
                  // keep last characters for the next read
                  StartOfRead = INPUT_FILE_BUFFER_SIZE - res;
                  strcpy (FileBuffer, &FileBuffer[res]);
                }
              LastCharInWorkBuffer = strlen (*buff) - 1;
            }
        }
    }
  *lastchar = LastCharInWorkBuffer;
}


/*----------------------------------------------------------------------
  GetNextChar returns the next character in the imput file or buffer,
  whatever it is.
  ----------------------------------------------------------------------*/
static char GetNextChar (FILE *infile, char* buffer, int *index,
                         ThotBool *endOfFile)
{
  wchar_t        wcharRead = EOS;
  unsigned char  charRead;
  unsigned char  fallback[8];
  unsigned char *ptr;
  char          *buff;
  int            res = 0;

  charRead = EOS;
  *endOfFile = FALSE;

  if (buffer)
    {
      /* read from a buffer */
      if (SecondByte[0] != EOS)
        {
          /* return the second UTF-8 byte */
          charRead = SecondByte[0];
          /* shift */
          strncpy ((char *)SecondByte, (char *)&SecondByte[1], 4);
        }
      else
        {
          charRead = buffer[(*index)++];
          if (charRead == EOS)
            *endOfFile = TRUE;
          else
            {
              if (HTMLcontext.encoding != UTF_8)
                {
                  /* translate ISO-latin characters into a UTF-8 string */
                  ptr = fallback;
                  wcharRead = TtaGetWCFromChar (charRead, HTMLcontext.encoding);
                  res = TtaWCToMBstring (wcharRead, &ptr);
                  /* handle the first character */
                  charRead = fallback[0];
                  if (res > 1)
                    {
                      /* store the second UTF-8 byte */
                      res--;
                      strncpy ((char *)SecondByte, (char *)&fallback[1], res);
                      SecondByte[res] = EOS;
                    }
                }
            }
        }
    }
  else if (infile == NULL)
    *endOfFile = TRUE;
  else
    {
      /* read from a file */
      if (*index == 0 && SecondByte[0] == EOS)
        {
          if (NotToReadFile)
            NotToReadFile = FALSE;
          else
            {
              // read next characters
              GetNextHTMLbuffer (infile, endOfFile, &buff, &res);
              if (*endOfFile)
                /* error or end of file */
                charRead = EOS;
            }
        }
	
      if (NotToReadFile)
        {
          charRead = PreviousRead[(*index)++];
          if (*index > LastCharInPreviousRead)
            *index = 0;
        }
      else if (*endOfFile == FALSE)
        {
          if (SecondByte[0] != EOS)
            {
              /* return the second UTF-8 byte */
              charRead = SecondByte[0];
              /* shift */
              strncpy ((char *)SecondByte, (char *)&SecondByte[1], 4);
            }
          else
            {
              charRead = WorkBuffer[(*index)++];
              if (charRead == EOS)
                *endOfFile = TRUE;
              else
                {
                  if (HTMLcontext.encoding == UTF_8)
                    {
                      /* Search for an UTF-8 BOM character (EF BB BF) */
                      /* Laurent Carcone 7/11/2002 */
                      if (*index == 1 && LastCharInWorkBuffer > 2 &&
                          (unsigned char) WorkBuffer[0] == 0xEF &&
                          (unsigned char) WorkBuffer[1] == 0xBB &&
                          (unsigned char) WorkBuffer[2] == 0xBF &&
                          PreviousRead[0] == EOS)
                        {
                          charRead = WorkBuffer[(*index)++];
                          charRead = WorkBuffer[(*index)++];
                          charRead = WorkBuffer[(*index)++];
                        }
                    }
                  else
                    {
                      /* translate the ISO-latin-1 character into a UTF-8 string */
                      ptr = fallback;
                      fallback[1] = EOS;
                      wcharRead = TtaGetWCFromChar (charRead, HTMLcontext.encoding);
                      res = TtaWCToMBstring (wcharRead, &ptr);
                      /* handle the first character */
                      charRead = fallback[0];
                      if (res > 1)
                        {
                          /* store the second UTF-8 byte */
                          res--;
                          strncpy ((char *)SecondByte, (char *)&fallback[1], res);
                          SecondByte[res] = EOS;
                        }
                    }
                }
            }
	  
          if (*index > LastCharInWorkBuffer)
            *index = 0;
        }
    }
  return charRead;
}

/*----------------------------------------------------------------------
  SetElemLineNumber
  assigns the current line number (number of latest line read from the
  input file) to element el.
  ----------------------------------------------------------------------*/
void SetElemLineNumber (Element el)
{
  TtaSetElementLineNumber (el, NumberOfLinesRead);
}

/*----------------------------------------------------------------------
  GetNextInputChar returns the next non-null character in the input
  file or buffer.
  ----------------------------------------------------------------------*/
char GetNextInputChar (FILE *infile, int *index, ThotBool *endOfFile)
{
  char    charRead;
  static  ThotBool beg_pair;

  charRead = EOS;
  *endOfFile = FALSE;
  if (PreviousBufChar != EOS)
    {
      charRead = PreviousBufChar;
      PreviousBufChar = EOS;
    }
  else 
    {
      charRead = GetNextChar (infile, InputText, index, endOfFile);
      if (InputText == NULL)
        NumberOfCharRead++;
      /* skip null characters*/
      while (charRead == EOS && !*endOfFile)
        {
          charRead = GetNextChar (infile, InputText, index, endOfFile);
          if (InputText == NULL)
            NumberOfCharRead++;
        }
    }
  if (*endOfFile == FALSE)
    {
      if ((int) charRead == __CR__)
        /* CR has been read */
        {
          /* Read next character */
          charRead = GetNextChar (infile, InputText, index, endOfFile);
          if ((int) charRead != EOL && (int) charRead != __CR__)
            /* next character is not LF. Store next character and return LF */
            {
              PreviousBufChar = charRead;
              charRead = EOL;
            }
        }
      /* update the counters of characters and lines read */
      if ((int) charRead == EOL || (int) charRead == __CR__)
        {
          if ((int) charRead == __CR__)
            {
              beg_pair = TRUE;
              if (InputText == NULL)
                NumberOfLinesRead++;
              NumberOfCharRead = 0;
            }
          else
            {
              if (!beg_pair)
                {
                  if (InputText == NULL)
                    NumberOfLinesRead++;
                  NumberOfCharRead = 0;
                }
              else
                beg_pair = FALSE;
            }
        }
      else
        beg_pair = FALSE;
    }
  return charRead;
}

/*----------------------------------------------------------------------
  HTMLparse       
  Parse either the HTML file infile or the text buffer HTMLbuf and
  build the equivalent Thot abstract tree.
  One parameter should be NULL.
  ----------------------------------------------------------------------*/
static void HTMLparse (FILE * infile, char* HTMLbuf)
{
  unsigned char         charRead;
  PtrTransition         trans;
  ThotBool              match;
  
  currentState = 0;
  if (HTMLbuf != NULL || infile != NULL)
    {
      InputText = HTMLbuf;
      EndOfHtmlFile = FALSE;
    }
  charRead = EOS;
  HTMLrootClosed = FALSE;
  
  /* read the HTML file sequentially */
  do
    {
      /* read one character from the source if the last character */
      /* read has been processed */
      if (charRead == EOS)
        charRead = GetNextInputChar (infile, &CurrentBufChar, &EndOfHtmlFile);
      if (charRead != EOS)
        {
          /* Check the character read */
          /* Consider LF and FF as the end of an input line. */
          /* Replace end of line by space, except in preformatted text. */
          /* Replace HT by space, except in preformatted text. */
          /* Ignore spaces at the beginning and at the end of input lines */
          /* Ignore non printable characters except HT, LF, FF. */
          if ((int) charRead == EOL || (int) charRead == __CR__)
            /* LF = end of input line */
            {
              /* don't replace end of line by space in a doctype declaration */
              if (currentState != 12 && currentState != 15 &&
                  currentState != 20 && currentState != 21 &&
                  currentState != 24 && currentState != 35)
                {
                  /* don't change characters in comments */
                  if (currentState != 0)
                    /* not within a text element */
                    {
                      if (currentState == 6 || currentState == 9)
                        /* within an attribute value between quotes */
                        if (lastAttrEntry != NULL &&
                            !strcmp (lastAttrEntry->XMLattribute, "src"))
                          /* value of an SRC attribute */
                          /* consider new line as an empty char*/
                          charRead = EOS;
                      if (charRead != EOS)
                        {
                          /* Replace new line by a space, except if an entity is
                             being read */
                          if (currentState == 30 &&
                              Within (HTML_EL_Preformatted, DocumentSSchema) &&
                              !Within (HTML_EL_Option_Menu, DocumentSSchema))
                            charRead = EOL; /* new line character */
                          else
                            charRead = SPACE;
                        }
                    }
                  else if ((Within (HTML_EL_Preformatted, DocumentSSchema) &&
                            !Within (HTML_EL_Option_Menu, DocumentSSchema)) ||
                           Within (HTML_EL_Text_Area, DocumentSSchema) ||
                           Within (HTML_EL_SCRIPT_, DocumentSSchema) ||
                           Within (HTML_EL_STYLE_, DocumentSSchema))
                    /* new line in a text element */
                    {
                      /* within preformatted text */
                      if (AfterTagPRE)
                        /* ignore NL after a <PRE> tag */
                        charRead = EOS;
                      else
                        /* generate a new line character */
                        charRead = EOL;
                    }
                  else
                    /* new line in ordinary text */
                    {
                      /* suppress all spaces preceding the end of line */
                      while (LgBuffer > 0 &&
                             inputBuffer[LgBuffer - 1] == SPACE)
                        LgBuffer--;
                      /* new line is equivalent to space */
                      charRead = SPACE;
                      if (LgBuffer > 0)
                        TextToDocument ();
                    }
                }
              /* beginning of a new input line */
              EmptyLine = TRUE;
            }
          else
            /* it's not an end of line */
            {
              if ((int) charRead == TAB)
                /* HT = Horizontal tabulation */
                {
                  if (currentState != 0)
                    /* not in a text element. Replace HT by space */
                    charRead = SPACE;
                  else
                    /* in a text element. Replace HT by space except in */
                    /* preformatted text */
                    if (!Within (HTML_EL_Preformatted, DocumentSSchema) &&
                        !Within (HTML_EL_STYLE_, DocumentSSchema) &&
                        !Within (HTML_EL_SCRIPT_, DocumentSSchema))
                      charRead = SPACE;
                }
              if (charRead == SPACE)
                /* space character */
                {
                  if (currentState == 12 || currentState == 35 ||
                      currentState == 20 || currentState == 21 ||
                      (currentState == 0 &&
                       !Within (HTML_EL_Preformatted, DocumentSSchema) &&
                       !Within (HTML_EL_STYLE_, DocumentSSchema) &&
                       !Within (HTML_EL_SCRIPT_, DocumentSSchema) &&
                       !Within (HTML_EL_Text_Area, DocumentSSchema)))
                    {
                      if (EmptyLine)
                        /* ignore spaces at the beginning of an input line */
                        charRead = EOS;
                      else if (LgBuffer > 0 && inputBuffer[LgBuffer-1] == SPACE)
                        /* ignore multiple spaces */
                        charRead = EOS;
                    }
                }
              else
                /* it's a printable character. Keep it as it is and */
                /* stop ignoring spaces */
                {
                  EmptyLine = FALSE;
                  StartOfFile = FALSE;
                }
            }
          AfterTagPRE = FALSE;
          
          if (charRead != EOS)
            /* a valid character has been read */
            {
              /* first transition of the automaton for the current state */
              trans = automaton[currentState].firstTransition;
              /* search a transition triggered by the character read */
              while (trans != NULL && !HTMLrootClosed)
                {
                  match = FALSE;
                  if (charRead == trans->trigger)
                    /* the char is the trigger */
                    match = TRUE;
                  else if (trans->trigger == EOS)
                    /* any char is a trigger */
                    match = TRUE;
                  else if (trans->trigger == SPACE)
                    /* any space is a trigger */
                    if ((int) charRead == TAB ||
                        (int) charRead == EOL ||
                        (int) charRead == 12)
                      /* a delimiter has been read */
                      match = TRUE;
                  if (match)
                    /* transition found. Activate the transition */
                    {
                      NormalTransition = TRUE;
                      /* call the procedure associated with the transition */
                      CharProcessed = FALSE;
                      if (trans->action != NULL)
                        (*((Proc1)trans->action)) ((void *)(int)charRead);
                      if (NormalTransition || CharProcessed)
                        /* the input character has been processed */
                        charRead = EOS;
                      
                      if (NormalTransition)
                        {
                          /* the procedure associated with the transition has not */
                          /* changed state explicitely */
                          /* change current automaton state */
                          if (trans->newState >= 0)
                            currentState = trans->newState;
                          else if (trans->newState == -1)
                            /* return form subautomaton */
                            currentState = returnState;
                          else
                            /* calling a subautomaton */
                            {
                              returnState = currentState;
                              currentState = -trans->newState;
                            }
                        }
                      /* done */
                      trans = NULL;
                    }
                  else
                    /* access next transition from the same state */
                    {
                      trans = trans->nextTransition;
                      /* an exception: when reading the value of an HREF attribute,
                         SGML entities (&xxx;) should not be interpreted */
                      if (trans == NULL)
                        charRead = EOS;
                    }
                }
            }
        }
    }
  while (!EndOfHtmlFile && !HTMLrootClosed);
  /* end of HTML file */
  
  if (!HTMLrootClosed)
    EndOfDocument ();
  HTMLrootClosingTag = NULL;
  HTMLrootClosed = FALSE;
}

/*----------------------------------------------------------------------
  GetANewText generates a new text element within a line.
  ----------------------------------------------------------------------*/
static Element GetANewText (Element el, ElementType elType, Document doc)
{
  Element             elLeaf;

  if (LgBuffer)
    {
      inputBuffer[LgBuffer] = EOS;
      TtaAppendTextContent (el, (unsigned char *)inputBuffer, doc);
      LgBuffer = 0;
      /* Create a new text leaf */
      elType.ElTypeNum = TextFile_EL_TEXT_UNIT;
      elLeaf = TtaNewElement (doc, elType);
      TtaSetElementLineNumber (elLeaf, NumberOfLinesRead);
      TtaInsertSibling (elLeaf, el,  FALSE, doc);
      el = elLeaf;
      HTMLcontext.lastElement = el;
    }
  return el;
}

/*----------------------------------------------------------------------
  ReadTextFile
  read plain text file into a TextFile document.
  input text comes from either the infile file or the text
  buffer textbuf. One parameter should be NULL.
  ----------------------------------------------------------------------*/
static void ReadTextFile (FILE *infile, char *textbuf, Document doc,
                          const char *pathURL)
{
  Element             parent, el, prev;
  Element             elLeaf;
  ElementType         elType;
  AttributeType       attrType;
  Attribute	      attr;
  unsigned char       charRead;
  int		      val;
  ThotBool            endOfTextFile, color_source;
  ThotBool            withinMarkup = FALSE;
  ThotBool            withinQuote = FALSE, withinString = FALSE;
  ThotBool            withinComment = FALSE;

  InputText = textbuf;
  LgBuffer = 0;
  endOfTextFile = FALSE;
  NumberOfCharRead = 0;
  NumberOfLinesRead = 1; 
  CurrentBufChar = 0;
  TtaGetEnvBoolean ("COLOR_SOURCE", &color_source);

#ifdef ANNOTATIONS
  if (DocumentTypes[doc] == docAnnot)
    /* we search the start of HTML document in the annotation struct */
    parent = ANNOT_GetHTMLRoot (doc, TRUE);
  else
#endif /* ANNOTATIONS */
    parent = TtaGetRootElement (doc);    /* the root element */

  elType = TtaGetElementType (parent);
  el = TtaGetFirstChild (parent);    /* first child of the root element */
  if (el == NULL)
    {
      /* insert the Document_URL element */
      elType.ElTypeNum = TextFile_EL_Document_URL;
      prev = TtaNewTree (doc, elType, "");
      TtaInsertFirstChild (&prev, parent, doc);
      /* prevent the user from editing this element */
      TtaSetAccessRight (prev, ReadOnly, doc);
      if (pathURL != NULL && prev != NULL)
        {
          el = TtaGetFirstChild (prev);
          TtaSetTextContent (el, (unsigned char *)pathURL,
                             HTMLcontext.language, doc);
        }
      /* insert the BODY element */
      elType.ElTypeNum = TextFile_EL_BODY;
      el = TtaNewElement (doc, elType);
      TtaSetElementLineNumber (el, NumberOfLinesRead);      
      if (prev != NULL)
        TtaInsertSibling (el, prev,  FALSE, doc);
      else
        TtaInsertFirstChild (&el, parent, doc);
      parent = el;
    }
  prev = el = NULL;
  /* initialize the context */
  HTMLcontext.encoding = TtaGetDocumentCharset (doc);
  HTMLcontext.lastElement = NULL;
  HTMLcontext.lastElementClosed = False;
  HTMLcontext.doc = doc;
  HTMLcontext.mergeText = FALSE;
  HTMLcontext.language = TtaGetDefaultLanguage ();
  attrType.AttrSSchema = TtaGetSSchema ("TextFile", doc);
  /* initialize input buffer */
  charRead = GetNextInputChar (infile, &CurrentBufChar, &endOfTextFile);
  /* read the text file sequentially */
  while (!endOfTextFile)
    {
      if (el == NULL)
        {
          /* create a new line */
          elType.ElTypeNum = TextFile_EL_Line_;
          el = TtaNewTree (doc, elType, "");
          TtaSetElementLineNumber (el, NumberOfLinesRead);      
          if (prev != NULL)
            /* new line after the previous */
            TtaInsertSibling (el, prev,  FALSE, doc);
          else
            /* first line */
            TtaInsertFirstChild (&el, parent, doc);
          prev = el;
          /* get the text element */
          el = TtaGetFirstChild (el);
          TtaSetElementLineNumber (el, NumberOfLinesRead);      
          HTMLcontext.lastElement = el;
        }
      else if (HTMLcontext.lastElement && HTMLcontext.lastElement != el)
        {
          /* one or more symbols were inserted */
          elType.ElTypeNum = TextFile_EL_TEXT_UNIT;
          el = TtaNewElement (doc, elType);
          TtaSetElementLineNumber (el, NumberOfLinesRead);      
          TtaInsertSibling (el, HTMLcontext.lastElement,  FALSE, doc);
          HTMLcontext.lastElement = el;
        }

      /* Check the character read */
      /* Consider LF and FF as the end of an input line. */
      if ((int) charRead == EOL || (int) charRead == 0)
        {
          /* LF = end of line */
          inputBuffer[LgBuffer] = EOS;
          if (LgBuffer > 0)
            {
              TtaAppendTextContent (el, (unsigned char *)inputBuffer, doc);
              LgBuffer = 0;
              attrType.AttrTypeNum = 0;
              if (withinMarkup)
                {
                  /* attach the markup attribute */
                  attrType.AttrTypeNum = TextFile_ATTR_IsMarkup;
                  val = TextFile_ATTR_IsMarkup_VAL_Yes_;
                }
              else if (withinComment)
                {
                  /* attach the markup attribute */
                  attrType.AttrTypeNum = TextFile_ATTR_IsComment;
                  val = TextFile_ATTR_IsComment_VAL_Yes_;
                }
              else if (withinString)
                {
                  /* attach the markup attribute */
                  attrType.AttrTypeNum = TextFile_ATTR_IsString;
                  val = TextFile_ATTR_IsString_VAL_Yes_;
                }
              if (withinMarkup || withinComment || withinString)
                {
                  attr = TtaGetAttribute (el, attrType);
                  if (attr == NULL)
                    {
                      attr = TtaNewAttribute (attrType);
                      TtaAttachAttribute (el, attr, doc);
                      TtaSetAttributeValue (attr, val, el, doc);
                    }
                }
            }
          el = NULL; /* generate a new line */
          charRead = EOS;
        }

      if (charRead != EOS)
        {
          /* a valid character has been read */
          if (!color_source)
            {
              if (LgBuffer + 1 >= AllmostFullBuffer)
                {
                  /* store the current buffer contents and continue */
                  inputBuffer[LgBuffer] = EOS;
                  TtaAppendTextContent (el, (unsigned char *)inputBuffer, doc);
                  LgBuffer = 0;
                  inputBuffer[LgBuffer++] = charRead;
                }
              else
                inputBuffer[LgBuffer++] = charRead;
            }
          else if (charRead == '@' && DocumentTypes[doc] == docLog && LgBuffer == 0)
            {
              attrType.AttrTypeNum = TextFile_ATTR_IsLink;
              val = TextFile_ATTR_IsLink_VAL_Yes_;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (el, attr, doc);
              TtaSetAttributeValue (attr, val, el, doc);
            }
          else if (withinMarkup &&
                   DocumentTypes[doc] != docCSS &&
                   DocumentTypes[doc] != docLog &&
                   DocumentTypes[doc] != docText &&
                   charRead == '-' &&
                   !withinString &&
                   LgBuffer > 2 &&
                   inputBuffer[LgBuffer-1] == '-' &&
                   inputBuffer[LgBuffer-2] == '!' &&
                   inputBuffer[LgBuffer-3] == '<')
            {
              /* Start a XML comment */
              withinMarkup = FALSE;
              withinComment = TRUE;
              /* add the current character */
              inputBuffer[LgBuffer++] = charRead;
            }
          else if (withinComment &&
                   DocumentTypes[doc] != docCSS &&
                   DocumentTypes[doc] != docLog &&
                   DocumentTypes[doc] != docText &&
                   charRead == '>' &&
                   !withinString &&
                   LgBuffer > 1 &&
                   inputBuffer[LgBuffer-1] == '-' &&
                   inputBuffer[LgBuffer-2] == '-')
            {
              /* End a XML comment */
              withinComment = FALSE;
              /* add the current character */
              inputBuffer[LgBuffer++] = charRead;
              /* attach the markup attribute */
              attrType.AttrTypeNum = TextFile_ATTR_IsComment;
              attr = TtaGetAttribute (el, attrType);
              if (attr == NULL)
                {
                  attr = TtaNewAttribute (attrType);
                  val = TextFile_ATTR_IsComment_VAL_Yes_;
                  TtaAttachAttribute (el, attr, doc);
                  TtaSetAttributeValue (attr, val, el, doc);
                }
              /* generate a new IsString element */
              el = GetANewText (el, elType, doc);
            }
          else if (!withinQuote && !withinComment &&
                   DocumentTypes[doc] != docCSS &&
                   DocumentTypes[doc] != docLog &&
                   DocumentTypes[doc] != docText &&
                   ((charRead == '"' && withinString) ||
                    (charRead == '"' && !withinString &&
                     LgBuffer > 0 && inputBuffer[LgBuffer-1] == '=') ||
                    (LgBuffer == 0 && withinString)))
            {
              /* Start/end a string */
              if (charRead == '"')
                withinString = !withinString;
              if (withinString)
                {
                  if (withinMarkup)
                    {
                      /* attach the markup attribute */
                      attrType.AttrTypeNum = TextFile_ATTR_IsMarkup;
                      attr = TtaGetAttribute (el, attrType);
                      if (attr == NULL)
                        {
                          attr = TtaNewAttribute (attrType);
                          val = TextFile_ATTR_IsMarkup_VAL_Yes_;
                          TtaAttachAttribute (el, attr, doc);
                          TtaSetAttributeValue (attr, val, el, doc);
                        }
                    }
                  /* generate a new IsString element */
                  el = GetANewText (el, elType, doc);
                  attrType.AttrTypeNum = TextFile_ATTR_IsString;
                  attr = TtaGetAttribute (el, attrType);
                  if (attr == NULL)
                    {
                      attr = TtaNewAttribute (attrType);
                      val = TextFile_ATTR_IsString_VAL_Yes_;
                      TtaAttachAttribute (el, attr, doc);
                      TtaSetAttributeValue (attr, val, el, doc);
                    }
                  /* add the current character */
                  inputBuffer[LgBuffer++] = charRead;
                }
              else
                {
                  /* add the current character */
                  inputBuffer[LgBuffer++] = charRead;
                  /* close the IsString element */
                  el = GetANewText (el, elType, doc);
                }
            }
          else if (!withinString && !withinComment &&
                   DocumentTypes[doc] != docCSS &&
                   DocumentTypes[doc] != docLog &&
                   DocumentTypes[doc] != docText &&
                   ((charRead == '\'' && withinQuote) ||
                    (charRead == '\'' && !withinQuote &&
                     LgBuffer > 0 && inputBuffer[LgBuffer-1] == '=') ||
                    (LgBuffer == 0 && withinQuote)))
            {
              if (charRead == '\'')
                withinQuote = !withinQuote;
              if (withinQuote)
                {
                  /* generate a new IsString element */
                  el = GetANewText (el, elType, doc);
                  attrType.AttrTypeNum = TextFile_ATTR_IsString;
                  attr = TtaGetAttribute (el, attrType);
                  if (attr == NULL)
                    {
                      attr = TtaNewAttribute (attrType);
                      val = TextFile_ATTR_IsString_VAL_Yes_;
                      TtaAttachAttribute (el, attr, doc);
                      TtaSetAttributeValue (attr, val, el, doc);
                    }
                  /* add the current character */
                  inputBuffer[LgBuffer++] = charRead;
                }
              else
                {
                  /* add the current character */
                  inputBuffer[LgBuffer++] = charRead;
                  /* close the IsString element */
                  el = GetANewText (el, elType, doc);
                }
            }
          else if (!withinString && !withinQuote &&
                   !withinComment &&
                   DocumentTypes[doc] != docCSS &&
                   DocumentTypes[doc] != docLog &&
                   DocumentTypes[doc] != docText &&
                   (charRead == '<' ||
                    (charRead == '>' && withinMarkup)))
            {
              if (charRead == '<')
                {
                  /* Start a markup */
                  withinMarkup = TRUE;
                  /* close the previous element */
                  el = GetANewText (el, elType, doc);
                  /* add the current character */
                  inputBuffer[LgBuffer++] = charRead;
                }
              else
                {
                  /* End a markup */
                  withinMarkup = FALSE;
                  /* add the current character */
                  inputBuffer[LgBuffer++] = charRead;
                  attrType.AttrTypeNum = TextFile_ATTR_IsMarkup;
                  attr = TtaGetAttribute (el, attrType);
                  if (attr == NULL)
                    {
                      attr = TtaNewAttribute (attrType);
                      val = TextFile_ATTR_IsMarkup_VAL_Yes_;
                      TtaAttachAttribute (el, attr, doc);
                      TtaSetAttributeValue (attr, val, el, doc);
                    }
                  el = GetANewText (el, elType, doc);
                }
            }
          else if ((DocumentTypes[doc] == docCSS ||
                    DocumentTypes[doc] == docText) &&
                   (charRead == '*' || charRead == '/' ||
                    (LgBuffer == 0 && withinComment)))
            {
              if (!withinComment && charRead == '*' &&
                  LgBuffer > 0 && inputBuffer[LgBuffer-1] == '/')
                {
                  /* open a comment */
                  withinComment = !withinComment;
                  /* close previous element */
                  inputBuffer[0] = EOS;
                  el = GetANewText (el, elType, doc);
                  /* add the current character */
                  inputBuffer[LgBuffer++] = '/';
                }
              else if ((withinComment && charRead == '*') ||
                       (!withinComment && charRead == '/'))
                {
                  /* flush the current buffer */
                  inputBuffer[LgBuffer] = EOS;
                  TtaAppendTextContent (el, (unsigned char *)inputBuffer, doc);
                  LgBuffer = 0;
                }
              if (withinComment)
                {
                  attrType.AttrTypeNum = TextFile_ATTR_IsComment;
                  attr = TtaGetAttribute (el, attrType);
                  if (attr == NULL)
                    {
                      attr = TtaNewAttribute (attrType);
                      val = TextFile_ATTR_IsComment_VAL_Yes_;
                      TtaAttachAttribute (el, attr, doc);
                      TtaSetAttributeValue (attr, val, el, doc);
                    }
                }
              /* add the current character */
              inputBuffer[LgBuffer++] = charRead;
              if (withinComment && charRead == '/' &&
                  LgBuffer == 2 && inputBuffer[0] == '*')
                {
                  /* close a comment */
                  withinComment = !withinComment;
                  el = GetANewText (el, elType, doc);
                }
            }
          else if (LgBuffer + 1 >= AllmostFullBuffer)
            {
              /* store the current buffer contents and continue */
              inputBuffer[LgBuffer] = EOS;
              TtaAppendTextContent (el, (unsigned char *)inputBuffer, doc);
              LgBuffer = 0;
              inputBuffer[LgBuffer++] = charRead;
            }
          else
            inputBuffer[LgBuffer++] = charRead;

          if (el != NULL)
            {
              /* test if last created element is a Symbol */
              elType = TtaGetElementType (el);
              if (elType.ElTypeNum != TextFile_EL_TEXT_UNIT)
                {
                  /* Create a new text leaf */
                  elType.ElTypeNum = TextFile_EL_TEXT_UNIT;
                  elLeaf = TtaNewElement (doc, elType);
                  TtaSetElementLineNumber (elLeaf, NumberOfLinesRead);
                  TtaInsertSibling (elLeaf, el,  FALSE, doc);
                  el = elLeaf;
                }
            }
        }

      /* read next character from the source */
      charRead = GetNextInputChar (infile, &CurrentBufChar, &endOfTextFile);
    }
  /* close the document */
  if (LgBuffer != 0)
    {
      inputBuffer[LgBuffer] = EOS;
      TtaAppendTextContent (el, (unsigned char *)inputBuffer, doc);
    }
  if (DocumentTypes[doc] != docSource)
    // clean up the list of css files
    UpdateStyleList (doc, 1);
}

/*----------------------------------------------------------------------
  CheckDocHeader parses the loaded file to detect if it includes:
  - an XML declaration (returns xmlDec = TRUE)
  - a doctype (returns docType = TRUE)
  Other returns:
  The indicator isXML
  The document type transitional, XHTML 1.1, basic, other (docProfile)
  The charset value if the XML declaration gives an encoding or
  UNDEFINED_CHARSET.
  The type of the doc'ument (given by the first element name)
  A boolean that indicates if an XML DTD is supported by Amaya
  ----------------------------------------------------------------------*/
void CheckDocHeader (char *fileName, ThotBool *xmlDec, ThotBool *docType,
                     ThotBool *isXML, ThotBool *useMath, ThotBool *isknown,
                     int *docProfile, CHARSET *charset, char *charsetname,
                     DocumentType *thotType, int *extraProfile)
{
  gzFile      stream;
  char       *ptr, *beg, *end, *ptrns, *prefix;
  char       *buffer = FileBuffer;
  int         res, i, j, k, pref_lg = 0;
  ThotBool    endOfSniffedFile, beginning;
  ThotBool    found;

  *xmlDec = FALSE;
  *docType = FALSE;
  *isXML = FALSE;
  *isknown = FALSE;
  *useMath = FALSE;
  *docProfile = L_Other;
  *charset = UNDEFINED_CHARSET;
  *thotType = docText;
  *extraProfile = L_NoExtraProfile;
  CurrentNameSpace[0] = EOS;
  stream = TtaGZOpen (fileName);
  if (stream != 0)
    {
      InputText = NULL;
      LgBuffer = 0;
      endOfSniffedFile = FALSE;
      beginning = TRUE;
      while (!endOfSniffedFile)
        {
          res = gzread (stream, buffer, INPUT_FILE_BUFFER_SIZE);
          if (res < 0)
            {
              TtaGZClose (stream);
              return;
            }
          if (res >= 5)
            buffer[res] = EOS;
          /* check if the file contains "<?xml ..." */
          i = 0;
          prefix = NULL;
          endOfSniffedFile = (res < INPUT_FILE_BUFFER_SIZE);
          found = TRUE;
          while (found)
            {
              if (beginning)
                {
                  /* looks for the first tag */
                  while (i < res &&
                         (buffer[i] == SPACE  ||
                          buffer[i] == EOL    ||
                          buffer[i] == TAB    ||
                          buffer[i] == __CR__ ||
                          (unsigned char) buffer[i] == 0xEF ||
                          (unsigned char) buffer[i] == 0xBB ||
                          (unsigned char) buffer[i] == 0xBF ))
                    i++;
                  if (buffer[i] == '<')
                    found = TRUE;
                  else
                    found = FALSE;
                }
              else
                {
                  /* looks for the next tag */
                  found = FALSE;
                  while (!found && i < res)
                    if (buffer[i] == '<')
                      found = TRUE;
                    else
                      i++;
                }
              /* if the declaration is present it's the first element */
              if (found)
                {
                  if (beginning && !strncmp (&buffer[i], "<?xml ", 6))
                    {
                      /* we've found <?xml */
                      i += 6;
                      *xmlDec = TRUE;
                      *isXML = TRUE;
#ifdef XML_GENERIC
                      *thotType = docXml;
#endif /* XML_GENERIC */
                      end = strstr (&buffer[i], "?>");
                      /* check whether there is an encoding */
                      ptr = strstr (&buffer[i], "encoding");
                      if (ptr && ptr < end)
                        {
                          beg = strstr (ptr, "\"");
                          if (beg && beg < end)
                            end = strstr (&beg[1], "\"");
                          else
                            {
                              beg = strstr (ptr, "\'");
                              if (beg && beg < end)
                                end = strstr (&beg[1], "\'");
                            }
                          if (end && beg && end != beg)
                            {
                              /* get the document charset */
                              k = 0; j = 1;
                              while (&beg[j] != end && k < MAX_LENGTH)
                                charsetname[k++] = beg[j++];
                              charsetname[k] = EOS;
                              *charset = TtaGetCharset (charsetname);
                            }
                        }
                    }
                  else if (!strncasecmp ((char *)&buffer[i], "<!DOCTYPE", 9))
                    {
                      /* the doctype is found */
                      i += 9;
                      *docType = TRUE;
                      /* it's not necessary to continue */
                      found = FALSE;
                      endOfSniffedFile = TRUE;
                      end = strstr (&buffer[i], ">");
                      /* check the current DOCTYPE */
                      ptr = strstr (&buffer[i], "HTML");
                      if (!ptr || (ptr && ptr > end))
                        ptr = strstr (&buffer[i], "html");
                      if (ptr && ptr < end)
                        {
                          *thotType = docHTML;
                          *docProfile = L_Transitional;
                          ptr = strstr (&buffer[i], "XHTML");
                          if (!ptr || (ptr && ptr > end))
                            ptr = strstr (&buffer[i], "xhtml");
                          if (ptr && ptr < end)
                            {
                              /* XHTML has been found */
                              /* Does Amaya support this doctype */
                              *isXML = TRUE;
                              *thotType = docXml;
                              ptr = strstr (&buffer[i], "Basic 1.0");
                              if (!ptr || (ptr && ptr > end))
                                ptr = strstr (&buffer[i], "basic 1.0");
                              if (ptr && ptr < end)
                                {
                                  *thotType = docHTML;
                                  *isknown = TRUE;
                                  *docProfile = L_Basic;
                                }
                              else
                                {
                                  ptr = strstr (&buffer[i], "XHTML 1.0");
                                  if (!ptr || (ptr && ptr > end))
                                    ptr = strstr (&buffer[i], "xhtml 1.0");
                                  if (!ptr || (ptr && ptr > end))
                                    ptr = strstr (&buffer[i], "XHTML 1.1");
                                  if (!ptr || (ptr && ptr > end))
                                    ptr = strstr (&buffer[i], "xhtml 1.1");
                                  if (ptr && ptr < end)
                                    {
                                      /* A supported XHTML doctype has been found */
                                      *thotType = docHTML;
                                      *isknown = TRUE;
                                      ptr = strstr (&buffer[i], "Strict");
                                      if (!ptr || (ptr && ptr > end))
                                        ptr = strstr (&buffer[i], "strict");
                                      if (ptr && ptr < end)
                                        *docProfile = L_Strict;
                                      else
                                        {
                                          ptr = strstr (&buffer[i], "Transitional");
                                          if (!ptr || (ptr && ptr > end))
                                            ptr = strstr (&buffer[i], "transitional");
                                          if (ptr && ptr < end)
                                            *docProfile = L_Transitional;
                                          else
                                            {
                                              ptr = strstr (&buffer[i], "1.1");
                                              if (ptr && ptr < end)
                                                {
                                                  ptr = strstr (&buffer[i], "svg:svg");
                                                  if (ptr && ptr < end)
                                                    {
                                                      *thotType = docSVG;
                                                      *isXML = TRUE;
                                                      *isknown = TRUE;
                                                      *docProfile = L_SVG;
                                                      *useMath = TRUE;
                                                    }
                                                  else
                                                    {
                                                      *docProfile = L_Xhtml11;
                                                      ptr = strstr (&buffer[i], "plus MathML");
                                                      if (ptr && ptr < end)
                                                        *useMath = TRUE;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                  else
                                    {
                                      ptr = strstr (&buffer[i], "xhtml-math11");
                                      if (!ptr || (ptr && ptr > end))
                                        ptr = strstr (&buffer[i], "XHTML-MATH11");
                                      if (ptr && ptr < end)
                                        {
                                          *thotType = docHTML;
                                          *isknown = TRUE;
                                          *docProfile = L_Xhtml11;
                                        }
                                      else
                                        {
                                          ptr = strstr (&buffer[i], "+RDFa");
                                          if (!ptr || (ptr && ptr > end))
                                            ptr = strstr (&buffer[i], "+rdfa");
                                          if (ptr && ptr < end)
                                            {
                                              *thotType = docHTML;
                                              *isknown = TRUE;
                                              *docProfile = L_Xhtml11;
                                              *extraProfile = L_RDFa;
                                            }
                                        }
                                    }
                                }
                            }
                          else
                            {
                              ptr = strstr (&buffer[i], "Strict");
                              if (!ptr || (ptr && ptr > end))
                                ptr = strstr (&buffer[i], "strict");
                              if (ptr && ptr < end)
                                *docProfile = L_Strict;
                              else
                                {
                                  ptr = strstr (&buffer[i], "Transitional");
                                  if (!ptr || (ptr && ptr > end))
                                    ptr = strstr (&buffer[i], "transitional");
                                  if (ptr && ptr < end)
                                    *docProfile = L_Transitional;
                                }
                            }
                        }
                      else
                        {
                          /* Look for svg tag */
                          ptr = strstr (&buffer[i], "SVG");
                          if (!ptr || (ptr && ptr > end))
                            ptr = strstr (&buffer[i], "svg");
                          if (ptr && ptr < end)
                            {
                              *thotType = docSVG;
                              *isXML = TRUE;
                              *isknown = TRUE;
                              *docProfile = L_SVG;
                            }
                          else
                            {
                              /* Look for math tag */
                              ptr = strstr (&buffer[i], "MATH");
                              if (!ptr || (ptr && ptr > end))
                                ptr = strstr (&buffer[i], "math");
                              if (ptr && ptr < end)
                                {
                                  *isXML = TRUE;
                                  *isknown = TRUE;
                                  *thotType = docMath;
                                  *docProfile = L_MathML;
                                }
                            }
                        }
                    }
                  else if (!strncmp (&buffer[i], "<!", 2) ||
                           !strncmp (&buffer[i], "<?", 2))
                    {
                      /* it's a comment or a PI */
                      if (!strncmp (&buffer[i], "<!", 2))
                        {
                          /* look for the end of the comment */
                          found = FALSE;
                          while (!found && i < res-2)
                            if (!strncmp (&buffer[i], "-->", 3))
                              found = TRUE;
                            else
                              i++;
                        }
                      else
                        {
                          /* look for the end of the PI */
                          found = FALSE;
                          while (!found && i < res-1)
                            if (!strncmp (&buffer[i], "?>", 2))
                              found = TRUE;
                            else
                              i++;
                        }
                      if (!found)
                        /* the end of the comment or PI can't be found */
                        /* it's not necessary to continue */
                        endOfSniffedFile = TRUE;			
                    }
                  else if (buffer[i] == '<')
                    {
                      /* it's most probably a start tag. Is there a
                         namespace prefix? */
                      i++;
                      j = i;
                      while (j < res &&
                             (buffer[j] != SPACE  &&
                              buffer[j] != EOL    &&
                              buffer[j] != TAB    &&
                              buffer[j] != __CR__ &&
                              buffer[j] != ':'))
                        j++;
                      if (buffer[j] == ':')
                        {
                        /* there is a prefix, skip it */
                          prefix = &buffer[i];
                          pref_lg = j - i;
                          i = j + 1;
                        }
                      if (!strncasecmp ((char *)&buffer[i], "html", 4))
                        {
                          /* the html tag is found */
                          i += 4;
                          /* it's not necessary to continue */
                          found = FALSE;
                          endOfSniffedFile = TRUE;
                          end = strstr (&buffer[i], ">");
                          ptrns = strstr (&buffer[i], "xmlns");
                          if (ptrns)
                          while (ptrns && ptrns < end)
                            {
                              *isXML = TRUE;
                              ptrns += 5;
                              if (*ptrns != ':' ||
                                  (prefix && !strncmp (&ptrns[1], (const char*)prefix, pref_lg)))
                                {
                                  ptr = strstr (ptrns,  XHTML_URI);
                                  if (ptr && ptr < end)
                                    {
                                      /* The xhtml namespace declaration is found */
                                      *thotType = docHTML;
                                      *isknown = TRUE;
                                      *docProfile = L_Transitional;
                                    }
                                  ptrns = NULL;
                                }
                              else
                                ptrns = strstr (ptrns, "xmlns");
                            }
                          else
                            /* No namespace, we consider the document as an html one */
                            *thotType = docHTML;
                        }
                      else if (!strncasecmp ((char *)&buffer[i], "svg", 3))
                        {
                          /* the svg tag is found */
                          i += 3;
                          /* it's not necessary to continue */
                          found = FALSE;
                          endOfSniffedFile = TRUE;
                          /* We consider the document as a svg one */
                          *thotType = docSVG;
                          *docProfile = L_SVG;
                          end = strstr (&buffer[i], ">");
                          ptrns = strstr (&buffer[i], "xmlns");
                          while (ptrns && ptrns < end)
                            {
                              *isXML = TRUE;
                              ptrns += 5;
                              if (*ptrns != ':' ||
                                  (prefix && !strncmp (&ptrns[1], (const char*)prefix, pref_lg)))
                                {
                                  ptr = strstr (ptrns, "svg");
                                  if (ptr && ptr < end)
                                    {
                                      /* The svg namespace declaration is found */
                                      *isknown = TRUE;
                                    }
                                  ptrns = NULL;
                                }
                              else
                                ptrns = strstr (ptrns, "xmlns");
                            }
                        }
                      else if (!strncasecmp ((char *)&buffer[i], "math", 4))
                        {
                          /* the math tag is found */
                          i += 4;
                          /* it's not necessary to continue */
                          found = FALSE;
                          endOfSniffedFile = TRUE;
                          /* We consider the document as a mathml one */
                          *thotType = docMath;
                          *docProfile = L_MathML;
                          end = strstr (&buffer[i], ">");
                          ptrns = strstr (&buffer[i], "xmlns");
                          while (ptrns && ptrns < end)
                            {
                              *isXML = TRUE;
                              ptrns += 5;
                              if (*ptrns != ':' ||
                                  (prefix && !strncmp (&ptrns[1], (const char*)prefix, pref_lg)))
                                {
                                  ptr = strstr (ptrns, "MathML");
                                  if (ptr && ptr < end)
                                    /* The MathML namespace declaration is found */
                                    *isknown = TRUE;
                                  ptrns = NULL;
                                }
                              else
                                 ptrns = strstr (ptrns, "xmlns");
                            }
                        }
                      else if (!strncasecmp ((char *)&buffer[i], "library", 7))
                        {
                          /* the library tag is found */
                          i += 7;
                          /* it's not necessary to continue */
                          found = FALSE;
                          endOfSniffedFile = TRUE;
                          /* We consider the document as a mathml one */
                          *thotType = docTemplate;
                          end = strstr (&buffer[i], ">");
                          ptrns = strstr (&buffer[i], "xmlns");
                          while (ptrns && ptrns < end)
                            {
                              *isXML = TRUE;
                              ptrns += 5;
                              if (*ptrns != ':' ||
                                  (prefix && !strncmp (&ptrns[1], (const char*)prefix, pref_lg)))
                                {
                                  ptr = strstr (ptrns, "xtiger");
                                  if (ptr && ptr < end)
                                    /* The xtiger namespace declaration is found */
                                    *isknown = TRUE;
                                  ptrns = NULL;
                                }
                              else
                                 ptrns = strstr (ptrns, "xmlns");
                            }
                        }
                      else
                        {
                          /* it's not necessary to continue */
                          found = FALSE;
                          endOfSniffedFile = TRUE;			  
                          /* We consider the document as a xml one */
                          end = strstr (&buffer[i], ">");
                          ptrns = strstr (&buffer[i], "xmlns");
                          while (ptrns && ptrns < end)
                            {
                              *thotType = docXml;
                              ptrns += 5;
                              if (*ptrns != ':' ||
                                  (prefix && !strncmp (&ptrns[1], (const char*)prefix, pref_lg)))
                                {
                                  // copy the namespace
                                  while (ptrns != end && *ptrns != '"')
                                    ptrns++;
                                  ptr = &ptrns[1];
                                  j = 0;
                                  while (ptr != end && *ptr != *ptrns && j < NAME_LENGTH)
                                    {
                                      CurrentNameSpace[j++] = *ptr;
                                      ptr++;
                                    }
                                  ptrns = NULL;
                                }
                              else
                                 ptrns = strstr (ptrns, "xmlns");
                            }
                          if (CurrentNameSpace[0] == EOS)
                            {
                              // copy the root name
                              j = 0;
                              ptr = &buffer[i];
                              while (ptr != end && *ptr != SPACE && j < NAME_LENGTH)
                                {
                                  CurrentNameSpace[j++] = *ptr;
                                  ptr++;
                                }
                              
                            }
                        }
                    }
                  else
                    {
                      /* it's not a comment nor a PI nor a start tag */
                      /* stop sniffing */
                      found = FALSE;
                      endOfSniffedFile = TRUE;
                    }
                }
              else
                /* it's not necessary to continue */
                endOfSniffedFile = TRUE;
              /* we're no longer parsing the beginning of the file */
              beginning = FALSE;
            }
        }
      TtaGZClose (stream);
    }

  // allow RDFa for XHTML documents without doctype
  if ((*docProfile == L_Xhtml11 || *docProfile == L_Transitional) &&
      *isXML && !(*docType))
    *extraProfile = L_RDFa;
}

/*----------------------------------------------------------------------
  CheckCharsetInMeta
  Parses the loaded file to detect if it includes a charset value 
  in a META element 
  ----------------------------------------------------------------------*/
void CheckCharsetInMeta (char *fileName, CHARSET *charset, char *charsetname)
{
  gzFile     stream;
  char      *ptr, *end, *end2, *meta, *endmeta, *content, *body, *http;
  char      *buffer = FileBuffer;
  int        res, i, j, k;
  ThotBool   endOfSniffedFile;

  *charset = UNDEFINED_CHARSET;
  stream = TtaGZOpen (fileName);
  if (stream != 0)
    {
      InputText = NULL;
      LgBuffer = 0;
      endOfSniffedFile = FALSE;
      while (!endOfSniffedFile)
        {
          res = gzread (stream, buffer, INPUT_FILE_BUFFER_SIZE);
          if (res < 0)
            {
              TtaGZClose (stream);
              return;
            }
          if (res >= 5)
            buffer[res] = EOS;
          i = 0;
          endOfSniffedFile = (res < INPUT_FILE_BUFFER_SIZE);
	  
          /* looks for the first <meta> element */
          meta = (char*)StrCaseStr (&buffer[i], "<meta");
          if (meta)
            {
              endmeta = strstr (meta, ">");
              /* looks for the first "http-equiv" declaration */
              http = (char*)StrCaseStr (meta, "http-equiv");
              if (http && http > endmeta)
                {
                  while (endmeta && http > endmeta)
                    {
                      meta =  (char*)StrCaseStr (endmeta,  "<meta");
                      if (meta)
                        endmeta = strstr (meta, ">");
                      else
                        {
                          endmeta = NULL;
                          http = NULL;
                        }
                    }
		  
                }
              if (http)
                {
                  /* looks for the "Content-Type" declaration */
                  content = (char*)StrCaseStr (meta, "content-type");
                  if (content)
                    {
                      /* check whether there is a charset */
                      ptr = (char*)StrCaseStr (meta, "charset");
                      if (ptr)
                        {
                          endOfSniffedFile = TRUE;
                          end = NULL;
                          ptr = strstr (ptr, "=");
                          if (ptr)
                            {
                              end2 = strstr (&ptr[1], ">");
                              if (end2)
                                {
                                  end = strstr (&ptr[1], "\"");
                                  if (!end || (end && end > end2))
                                    {
                                      end = strstr (&ptr[1], "\'");
                                      if (end && end > end2)
                                        end = NULL;
                                    }
                                }
                            }
                          if (end && end != ptr)
                            {
                              /* get the document charset */
                              k = 0; j = 1;
                              while (&ptr[j] != end && k < MAX_LENGTH)
                                charsetname[k++] = ptr[j++];
                              charsetname[k] = EOS;
                              *charset = TtaGetCharset (charsetname);
                            }
                        }
                    }
                }
            }
          /* looks for the <body> element */
          if (!endOfSniffedFile)
            {
              body = (char*)StrCaseStr (&buffer[i], "<body");
              if (body)
                endOfSniffedFile = TRUE;
            }
        }
      TtaGZClose (stream);
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void CheckHeadElements (Element el, Element *elHead,
                               Element *elBody, Document doc)
{
  Element           nextEl, rootEl, lastChild;
  ElementType       elType;

  /* check all children of the given element */
  el = TtaGetFirstChild (el);
  lastChild = NULL;
  while (el != NULL)
    {
      nextEl = el;
      TtaNextSibling (&nextEl);
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == HTML_EL_BODY &&
          elType.ElSSchema == DocumentSSchema && *elBody == NULL)
        *elBody = el;
      else if ((elType.ElTypeNum == HTML_EL_TITLE ||
                elType.ElTypeNum == HTML_EL_ISINDEX ||
                elType.ElTypeNum == HTML_EL_BASE ||
                elType.ElTypeNum == HTML_EL_STYLE_ ||
                elType.ElTypeNum == HTML_EL_META ||
                elType.ElTypeNum == HTML_EL_LINK) &&
               elType.ElSSchema == DocumentSSchema)
        /* this element should be a child of HEAD */
        {
          /* create the HEAD element if it does not exist */
          if (*elHead == NULL)
            {
#ifdef ANNOTATIONS
              if (DocumentTypes[doc] == docAnnot)
                rootEl = ANNOT_GetHTMLRoot (doc, TRUE); 
              else
#endif /* ANNOTATIONS */
                rootEl = TtaGetRootElement (doc);
              elType.ElTypeNum = HTML_EL_HEAD;
              *elHead = TtaNewElement (doc, elType);
              TtaInsertFirstChild (elHead, rootEl, doc);
            }
          else
            {
              elType.ElTypeNum = HTML_EL_TITLE;
              lastChild = TtaSearchTypedElement (elType, SearchInTree, *elHead);
              if (!lastChild)
                lastChild = TtaGetFirstChild (*elHead);
            }
          /* move the element as the last child of the HEAD element */
          TtaRemoveTree (el, doc);
          if (lastChild)
            TtaInsertSibling (el, lastChild, FALSE, doc);
          else
            TtaInsertFirstChild (&el, *elHead, doc);
          lastChild = el;
        }
      /* get next child of the root */
      el = nextEl;
    }
  /* is there a TITLE element in the HEAD ? */
  if (*elHead != NULL)
    {
      elType = TtaGetElementType (*elHead);
      elType.ElTypeNum = HTML_EL_TITLE;
      if (!TtaSearchTypedElement (elType, SearchInTree, *elHead))
        {
          /* create the title */
          lastChild = TtaNewTree (doc, elType, "");
          TtaInsertFirstChild (&lastChild, *elHead, doc);
        }
    }
}


/*----------------------------------------------------------------------
  EncloseCharLevelElem
  create a copy of element charEl for all descendants of el which are not
  block level elements.
  ----------------------------------------------------------------------*/
static void EncloseCharLevelElem (Element el, Element charEl,
                                  Document doc, ThotBool *done)
{
  Element      child, next, copy, prev, elem;
  ElementType	elType;

  if (IsEmptyElement (el))
    return;
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == HTML_EL_Table_head ||
      elType.ElTypeNum == HTML_EL_C_Head ||
      elType.ElTypeNum == HTML_EL_Horizontal_Rule)
    /* cannot insert any element into a Table_head or Horizontal_Rule */
    return;
  child = TtaGetFirstChild (el);
  if (child == NULL)
    {
      copy = TtaCopyTree (charEl, doc, doc, el);
      TtaInsertFirstChild (&copy, el, doc);
    }
  else
    {
      prev = NULL;
      do
        {
          next = child;
          TtaNextSibling (&next);
          elem = child;
          if (!IsCharacterLevelElement (elem))
            /* create copies of element parent for all descendants of elem */
            {
              EncloseCharLevelElem (elem, charEl, doc, done);
              prev = NULL;
              if (*done)
                next = NULL;
            }
          else
            /* enclose elem in a copy of charEl */
            {
              if (prev != NULL)
                {
                  TtaRemoveTree (elem, doc);
                  TtaInsertSibling (elem, prev, FALSE, doc);
                }
              else
                {
                  elType = TtaGetElementType (charEl);
                  copy = TtaCopyTree (charEl, doc, doc, el);
                  TtaInsertSibling (copy, elem, TRUE, doc);
                  TtaRemoveTree (elem, doc);
                  TtaInsertFirstChild (&elem, copy, doc);
                  /* do it only once for an Anchor */
                  if (elType.ElTypeNum == HTML_EL_Anchor)
                    *done = TRUE;
                }
              prev = elem;
            }
          child = next;
        }
      while (child != NULL);
    }
}


/*----------------------------------------------------------------------
  MergeElements
  merge element old into element element el.
  ----------------------------------------------------------------------*/
static void  MergeElements (Element old, Element el, Document doc)
{
  Element    elem, next, prev, sibling;

  elem = TtaGetFirstChild (old);
  sibling = TtaGetFirstChild (el);
  prev = NULL;
  while (elem != NULL)
    {
      next = elem;
      TtaNextSibling (&next);
      TtaRemoveTree (elem, doc);
      if (prev != NULL)
        TtaInsertSibling (elem, prev, FALSE, doc);
      else
        if (sibling == NULL)
          TtaInsertFirstChild (&elem, el, doc);
        else
          TtaInsertSibling (elem, sibling, TRUE, doc);
      prev = elem;
      elem = next;
    }
  TtaDeleteTree (old, doc);
}

/*----------------------------------------------------------------------
  MergePseudoParagraph
  if element el is a pseudo-paragraph and its neighbours elements are also
  pseudo paragraphs, merge these elements into a single pseudo-paragraph.
  ----------------------------------------------------------------------*/
static void MergePseudoParagraph (Element el, Document doc)
{
  Element	prev, next;
  ElementType	elType;

  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
    {
      prev = el;
      TtaPreviousSibling (&prev);
      if (prev != NULL)
        {
          elType = TtaGetElementType (prev);
          if (elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
            /* previous sibling is a pseud-paragraph too */
            MergeElements (prev, el, doc);
        }
      next = el;
      TtaNextSibling (&next);
      if (next != NULL)
        {
          elType = TtaGetElementType (next);
          if (elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
            /* next sibling is a pseud-paragraph too */
            MergeElements (el, next, doc);
        }
    }
}

/*----------------------------------------------------------------------
  CheckBlocksInCharElem
  handle character-level elements which contain block-level elements
  ----------------------------------------------------------------------*/
void CheckBlocksInCharElem (Document doc)
{
  Element             el, parent, child, first, last, next, copy;
  Element             newparent, elem, prev, firstNotCharElem;
  PtrElemToBeChecked  elTBC, nextElTBC, TBC;
  ElementType	      elType, parentType;
  ThotBool            done;
  
  /* check all block-level elements whose parent 
     was a character-level element */
  elTBC = FirstElemToBeChecked;
  while (elTBC)
    {
      el = elTBC->Elem;
      while (el != NULL)
        {
          parent = TtaGetParent (el);
          if (parent == NULL)
            el = NULL;
          else if (!IsCharacterLevelElement (parent))
            {
              MergePseudoParagraph (el, doc);
              el = NULL;
            }
          else
            {
              parentType = TtaGetElementType (parent);
              elType = TtaGetElementType (el);
              firstNotCharElem = NULL;
              if (elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
                {
                  first = TtaGetFirstChild (el);
                  child = first;
                  last = NULL;
                  /* move the pseudo paragraph as sibling of parent*/
                  TtaRemoveTree (el, doc);
                  TtaInsertSibling (el, parent, TRUE, doc);
                  /* move all children of element el as children of parent */
                  do
                    {
                      next = child;
                      TtaNextSibling (&next);
                      /* register the next element to be checked */
                      if (firstNotCharElem == NULL &&
                          !IsCharacterLevelElement (child))
                        firstNotCharElem = child;		    
                      TtaRemoveTree (child, doc);
                      if (child == first)
                        TtaInsertFirstChild (&child, parent, doc);
                      else
                        /* Modif LC 21/06/01 */
                        /* Insert the element 'child' after the element
                           'last', not before */
                        TtaInsertSibling (child, last, FALSE, doc);
                      last = child;
                      child = next;
                    }
                  while (child != NULL);
                  elType.ElTypeNum = HTML_EL_Pseudo_paragraph;
                  if (TtaGetTypedAncestor (parent, elType))
                    {
                      /* there is already an ancestor of type pseudo paragraph.
                         Delete this one */
                      TtaDeleteTree (el, doc);
                      el = NULL;
                    }
                  else
                    /* move parent as a child of the pseudo paragraph */
                    {
                      TtaRemoveTree (parent, doc);
                      TtaInsertFirstChild (&parent, el, doc);
                    }
                }
              else
                {
                  /* move all children of element parent as siblings of el */
                  first = TtaGetFirstChild (parent);
                  child = first;
                  do
                    {
                      next = child;
                      TtaNextSibling (&next);
                      TtaRemoveTree (child, doc);
                      TtaInsertSibling (child, parent, TRUE, doc);
                      last = child;
                      child = next;
                    }
                  while (child != NULL);
                  /* copy the character-level element for all elements that
                     have been moved */
                  newparent = TtaGetParent (parent);
                  elem = first;
                  prev = NULL;
                  do
                    {
                      /* if the character level element (parent) is an anchor, don't
                         repeat it several times */
                      if (elem == last || parentType.ElTypeNum == HTML_EL_Anchor)
                        next = NULL;
                      else
                        {
                          next = elem;
                          TtaNextSibling (&next);
                        }
                      elType = TtaGetElementType (elem);
                      if (elType.ElTypeNum != HTML_EL_Comment_ &&
                          !IsCharacterLevelElement (elem))
                        /* This is not a character level element */
                        /* create a copy of parent for all decendants of child */
                        {
                          done = FALSE;
                          EncloseCharLevelElem (elem, parent, doc, &done);
                          if (done)
                            next = NULL;
                          prev = NULL;
                          /* register the next element to be checked */
                          if (firstNotCharElem == NULL)
                            firstNotCharElem = elem;		    
                        }
                      else
                        /* this is a character level element */
                        /* enclose elem in a copy of parent element */
                        {
                          if (prev != NULL)
                            {
                              TtaRemoveTree (elem, doc);
                              TtaInsertSibling (elem, prev, FALSE, doc);
                            }
                          else
                            {
                              copy = TtaCopyTree (parent, doc, doc, newparent);
                              TtaInsertSibling (copy, elem, TRUE, doc);
                              TtaRemoveTree (elem, doc);
                              TtaInsertFirstChild (&elem, copy, doc);
                            }
                          prev = elem;
                        }
                      elem = next;
                    }
                  while (elem != NULL);
                  /* delete the old character-level element */
                  TtaDeleteTree (parent, doc);
                }

              /* if, among the elements that have just been moved, there are
                 pseudo-paragraphs which are now children of a block element,
                 remove these pseudo-paragraphs */
              elem = firstNotCharElem;
              if (firstNotCharElem)
                {
                  parent = TtaGetParent (firstNotCharElem);
                  if (parent != NULL && !IsBlockElement (parent))
                    elem = NULL;
                }
              while (elem != NULL)
                {
                  if (elem == last)
                    next = NULL;
                  else
                    {
                      next = elem;
                      TtaNextSibling (&next);
                    }
                  elType = TtaGetElementType (elem);
                  if (elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
                    {
                      child = TtaGetFirstChild (elem);
                      do
                        {
                          next = child;
                          TtaNextSibling (&next);
                          TtaRemoveTree (child, doc);
                          TtaInsertSibling (child, elem, TRUE, doc);
                          child = next;
                        }
                      while (child != NULL);
                      if (elem == el)
                        el = NULL;

                      /* if this element is in the queue, remove it from the queue */
                      TBC = elTBC->nextElemToBeChecked;
                      while (TBC != NULL)
                        {
                          if (TBC->Elem == elem)
                            TBC->Elem = NULL;
                          TBC = TBC->nextElemToBeChecked;
                        }
                      TtaDeleteTree (elem, doc);
                    }
                  elem = next;	     
                }
              /* if el is a Pseudo-paragraph, merge it with its next or previous
                 siblings if they also are Pseudo-paragraphs */
              if (el != NULL)
                MergePseudoParagraph (el, doc);
            }
        }
      nextElTBC = elTBC->nextElemToBeChecked;
      TtaFreeMemory (elTBC);
      elTBC = nextElTBC;
    }
  FirstElemToBeChecked = NULL;
  LastElemToBeChecked = NULL;
}

/*----------------------------------------------------------------------
  ParentOfType
  Return the parent element of element el if it is an HTML element of type
  typeNum. Ignore elements from the Templates namespace.
  ----------------------------------------------------------------------*/
static Element ParentOfType (Element el, int typeNum)
{
  ElementType	elType;
  Element     parent;

  parent = TtaGetParent (el);
  elType = TtaGetElementType (parent);
#ifdef TEMPLATES
  /* if the parent is a Template element, skip it */
  while (strcmp(TtaGetSSchemaName(elType.ElSSchema),"Template") == 0)
    {
      parent = TtaGetParent (parent);
      elType = TtaGetElementType (parent);
    }
#endif /* TEMPLATES */
  if (elType.ElTypeNum != typeNum)
    parent = NULL;
  else if (strcmp(TtaGetSSchemaName(elType.ElSSchema),"HTML") != 0)
    parent = NULL;
  return parent;
}

/*----------------------------------------------------------------------
  CheckAbstractTree
  Check the Thot abstract tree and create the missing elements.
  The parameter isXTiger is TRUE when parsing a XTiger template.
  ----------------------------------------------------------------------*/
void CheckAbstractTree (Document doc, ThotBool isXTiger)
{
  ElementType	elType, newElType, headElType;
  Element	elRoot, glossary, list, elText, previous;
  Element	el, elHead, elBody, elFrameset, elNoframes, nextEl, newEl;
  Element	prevEl, lastChild, firstTerm, lastTerm, termList, child;
  Element	parent, firstDef, lastDef, defList, firstEntry, lastEntry;
  ThotBool	ok, moved;
  SSchema       htmlSSchema;

  /* the root HTML element only accepts elements HEAD, BODY, FRAMESET
     Comment and PI as children */
  elHead = NULL;
  elBody = NULL;
  elFrameset = NULL;
  elNoframes = NULL;

  htmlSSchema = TtaGetSSchema ("HTML", doc);
#ifdef ANNOTATIONS
  if (DocumentTypes[doc] == docAnnot)
    /* we search the start of HTML document in the annotation struct */
    elRoot = ANNOT_GetHTMLRoot (doc, FALSE);
  else
#endif /* ANNOTATIONS */
    elRoot = TtaGetRootElement (doc);
  if (!elRoot)
    /* there is no <html> element! Create one */
    {
      /* create a <html> element */
      elType.ElSSchema = htmlSSchema;
      elType.ElTypeNum = HTML_EL_HTML;
      elRoot = TtaNewElement (doc, elType);
      /* insert it as the first child of the Document node */
      el = TtaGetMainRoot (doc);
      TtaInsertFirstChild (&elRoot, el, doc);
      /* move all other children of the Document node within this
         new <html> element */
      el = elRoot;
      TtaNextSibling (&el);
      lastChild = NULL;
      while (el)
        {
          nextEl = el;
          TtaNextSibling (&nextEl);
          TtaRemoveTree (el, doc);
          if (!lastChild)
            TtaInsertFirstChild (&el, elRoot, doc);
          else
            TtaInsertSibling (el, lastChild, FALSE, doc);
          lastChild = el;
          el = nextEl;
        }
    }
  el = TtaGetFirstChild (elRoot);
  if (el != NULL)
    {
      elType = TtaGetElementType (el);
      /* skip Comments, PI and Invalid_elements */
      while (el != NULL && elType.ElSSchema == htmlSSchema &&
             (elType.ElTypeNum == HTML_EL_Comment_ ||
              elType.ElTypeNum == HTML_EL_Invalid_element ||
              elType.ElTypeNum == HTML_EL_XMLPI))
        {
          TtaNextSibling (&el);
          if (el != NULL)
            elType = TtaGetElementType (el);
        }
      if (elType.ElTypeNum == HTML_EL_HTML && elType.ElSSchema == htmlSSchema)
        /* that's the HTML root element */
        {
          elRoot = el;
          /* check its children elements */
          el = TtaGetFirstChild (elRoot);
          elType = TtaGetElementType (el);
          /* skip Comments, PI and Invalid_elements */
          while (el != NULL && elType.ElSSchema == htmlSSchema &&
                 (elType.ElTypeNum == HTML_EL_Comment_ ||
                  elType.ElTypeNum == HTML_EL_Invalid_element ||
                  elType.ElTypeNum == HTML_EL_XMLPI))
            {
              TtaNextSibling (&el);
              if (el != NULL)
                elType = TtaGetElementType (el);
            }
        }
      if (elType.ElTypeNum == HTML_EL_HEAD && elType.ElSSchema == htmlSSchema)
        /* the first child of the root element is HEAD */
        {
          elHead = el;
          TtaNextSibling (&el);
          if (el != NULL)
            elType = TtaGetElementType (el);
        }
      else
        {
          elType.ElSSchema = htmlSSchema;
          elType.ElTypeNum = HTML_EL_HEAD;
          elHead = TtaSearchTypedElement (elType, SearchForward, elRoot);
          if (elHead != NULL)
            /* an element HEAD has been found */
            {
              /* move the HEAD element before the current element */
              TtaRemoveTree (elHead, doc);
              TtaInsertSibling (elHead, el, TRUE, doc);
            }
        }
      /* skip Comments, PI and Invalid_elements */
      while (el != NULL &&  elType.ElSSchema == htmlSSchema &&
             (elType.ElTypeNum == HTML_EL_Comment_ ||
              elType.ElTypeNum == HTML_EL_Invalid_element ||
              elType.ElTypeNum == HTML_EL_XMLPI))
        {
          TtaNextSibling (&el);
          if (el != NULL)
            elType = TtaGetElementType (el);
        }
      if (el != NULL)
        {
          if (elType.ElTypeNum == HTML_EL_HTML &&
              elType.ElSSchema == htmlSSchema)
            elRoot = el;
          else if (elType.ElTypeNum == HTML_EL_BODY &&
                   elType.ElSSchema == htmlSSchema)
            /* this child of the root element is BODY */
            elBody = el;
        }
	
      /* check all children of the root element */
      CheckHeadElements (elRoot, &elHead, &elBody, doc);
      if (elBody != NULL)
        CheckHeadElements (elBody, &elHead, &elBody, doc);

      if (elHead == NULL)
        /* there is no HEAD element. Create one */
        {
          newElType.ElSSchema = htmlSSchema;
          newElType.ElTypeNum = HTML_EL_HEAD;
          elHead = TtaNewTree (doc, newElType, "");
          TtaInsertFirstChild (&elHead, elRoot, doc);
        }

      if (elHead != NULL)
        {
          headElType = TtaGetElementType (elHead);
          /* create a Document_URL element as the first child of HEAD */
          newElType.ElSSchema = htmlSSchema;
          newElType.ElTypeNum = HTML_EL_Document_URL;
          el = TtaGetFirstChild (elHead);
          if (el != NULL)
            {
              elType = TtaGetElementType (el);
              if (elType.ElTypeNum == newElType.ElTypeNum &&
                  elType.ElSSchema == newElType.ElSSchema)
                /* element Document_URL already exists */
                elText = TtaGetFirstChild (el);
              else
                el = NULL;
            }
          if (el == NULL)
            /* there is no Document_URL element */
            {
              el = TtaNewElement (doc, newElType);
              TtaInsertFirstChild (&el, elHead, doc);
              newElType.ElTypeNum = HTML_EL_TEXT_UNIT;
              elText = TtaNewElement (doc, newElType);
              TtaInsertFirstChild (&elText, el, doc);
            }
          TtaSetAccessRight (el, ReadOnly, doc);
          if (DocumentURLs[doc] != NULL && elText != NULL)
            TtaSetTextContent (elText,(unsigned char *) DocumentURLs[doc], HTMLcontext.language, doc);
          /* check all chidren of the HEAD Element, except the first one */
          /* which is Document_URL */
          TtaNextSibling (&el);
          lastChild = NULL;
          while (el != NULL)
            {
              nextEl = el;
              TtaNextSibling (&nextEl);
              elType = TtaGetElementType (el);
              /* is this element allowed in the HEAD? */
              if (TtaGetRankInAggregate (elType, headElType) <= 0)
                /* this element is not a valid component of aggregate HEAD */
#ifdef TEMPLATES
                /* if it is a Template element, accept it */
                if (strcmp(TtaGetSSchemaName(elType.ElSSchema),"Template") != 0)
#endif /* TEMPLATES */
                  /* It may be an SGML inclusion, let's check */
                  if (!TtaCanInsertFirstChild (elType, elHead, doc))
                    /* this element cannot be a child of HEAD, move it to
                       the BODY */
                    {
                      /* create the BODY element if it does not exist */
                      if (elBody == NULL)
                        {
                          newElType.ElSSchema = htmlSSchema;
                          newElType.ElTypeNum = HTML_EL_BODY;
                          elBody = TtaNewElement (doc, newElType);
                          TtaInsertSibling (elBody, elHead, FALSE, doc);
                        }
                      /* move the current element into the BODY element */
                      TtaRemoveTree (el, doc);
                      if (lastChild == NULL)
                        TtaInsertFirstChild (&el, elBody, doc);
                      else
                        TtaInsertSibling (el, lastChild, FALSE, doc);
                      lastChild = el;
                    }
              el = nextEl;
            }
        }
      /* check the children of the root */
      lastChild = NULL;
      el = TtaGetFirstChild (elRoot);
      previous = elHead;
      moved = FALSE;
      while (el != NULL)
        {
          nextEl = el;
          TtaNextSibling (&nextEl);
          elType = TtaGetElementType (el);
          if (elType.ElTypeNum == HTML_EL_BODY &&
              elType.ElSSchema == htmlSSchema)
            /* stop */
            nextEl = NULL;
          else if (elType.ElTypeNum == HTML_EL_FRAMESET &&
                   elType.ElSSchema == htmlSSchema)
            {
              if (elFrameset == NULL)
                elFrameset = el;
            }
          else if (elType.ElTypeNum == HTML_EL_NOFRAMES &&
                   elType.ElSSchema == htmlSSchema)
            {
              if (elNoframes == NULL)
                elNoframes = el;
            }
          else if (!moved && elType.ElSSchema == htmlSSchema &&
                   (elType.ElTypeNum == HTML_EL_Invalid_element ||
                    elType.ElTypeNum == HTML_EL_Comment_ ||
                    elType.ElTypeNum == HTML_EL_XMLPI))
            /* don't move Comments, PI and Invalid_elements if the previous
               element has not been moved */
            previous = el;
          else if (elType.ElTypeNum == HTML_EL_HEAD &&
                   elType.ElSSchema == htmlSSchema)
            previous = el;
          else if (elType.ElTypeNum != HTML_EL_FRAMESET ||
                   elType.ElSSchema != htmlSSchema)
            /* this element should be a child of BODY */
            {
              /* create the BODY element if it does not exist */
              if (elBody == NULL)
                {
                  newElType.ElSSchema = htmlSSchema;
                  newElType.ElTypeNum = HTML_EL_BODY;
                  elBody = TtaNewElement (doc, newElType);
                  if (previous == NULL)
                    TtaInsertFirstChild (&elBody, elRoot, doc);
                  else
                    TtaInsertSibling (elBody, previous, FALSE, doc);
                }
              /* move the current element into the BODY element */
              TtaRemoveTree (el, doc);
              if (lastChild == NULL)
                TtaInsertFirstChild (&el, elBody, doc);
              else
                TtaInsertSibling (el, lastChild, FALSE, doc);
              lastChild = el;
              moved = TRUE;
            }
          /* get next child of the root */
          el = nextEl;
        }
      if (elFrameset && elNoframes)
        if (!TtaIsAncestor(elNoframes, elFrameset))
          /* moves the NOFRAMES element within the FRAMESET element */
          {
            el = TtaGetFirstChild(elFrameset);
            previous = NULL;
            while (el)
              {
                previous = el;
                TtaNextSibling (&el);
              }
            TtaRemoveTree (elNoframes, doc);
            if (previous == NULL)
              TtaInsertFirstChild (&elNoframes, elFrameset, doc);
            else
              TtaInsertSibling (elNoframes, previous, FALSE, doc);
          }

      /* handle character-level elements which contain block-level elements*/
      CheckBlocksInCharElem (doc);

      /* create a Term_List element for each sequence of Term elements */
      el = TtaGetFirstChild (elRoot);
      /* search all Term elements in the document */
      while (el != NULL)
        {
          elType.ElSSchema = htmlSSchema;
          elType.ElTypeNum = HTML_EL_Term;
          el = TtaSearchTypedElement (elType, SearchForward, el);
          if (el != NULL)
            /* an element Term has been found */
            {
              /* search all immediate Term siblings, ignoring
                 Comments, PI and Invalid_elements */
              firstTerm = el;
              do
                {
                  lastTerm = el;
                  TtaNextSibling (&el);
                  if (el == NULL)
                    elType.ElTypeNum = 0;
                  else
                    elType = TtaGetElementType (el);
                }
              while (elType.ElSSchema == htmlSSchema &&
                     (elType.ElTypeNum == HTML_EL_Term ||
                      elType.ElTypeNum == HTML_EL_Invalid_element ||
                      elType.ElTypeNum == HTML_EL_Comment_ ||
                      elType.ElTypeNum == HTML_EL_XMLPI));
              termList = ParentOfType (firstTerm, HTML_EL_Term_List);
              if (!termList)
                {
                  /* create a Term_List element before the first
                     Term element */
                  newElType.ElSSchema = htmlSSchema;
                  newElType.ElTypeNum = HTML_EL_Term_List;
                  termList = TtaNewElement (doc, newElType);
                  TtaInsertSibling (termList, firstTerm, TRUE, doc);
                  /* move the Term elements as children of the new Term_List */
                  nextEl = firstTerm;
                  TtaNextSibling (&nextEl);
                  TtaRemoveTree (firstTerm, doc);
                  TtaInsertFirstChild (&firstTerm, termList, doc);
                  if (lastTerm != firstTerm)
                    {
                      prevEl = firstTerm;
                      do
                        {
                          child = nextEl;
                          TtaNextSibling (&nextEl);
                          TtaRemoveTree (child, doc);
                          TtaInsertSibling (child, prevEl, FALSE, doc);
                          prevEl = child;
                        }
                      while (nextEl != NULL && child != lastTerm);
                    }
                }
              if (!ParentOfType (termList, HTML_EL_Definition_Item))
                {
                  /* Create a Definition_Item element surrounding */
                  /* the Term_List element */
                  newElType.ElSSchema = htmlSSchema;
                  newElType.ElTypeNum = HTML_EL_Definition_Item;
                  newEl = TtaNewElement (doc, newElType);
                  TtaInsertSibling (newEl, termList, TRUE, doc);
                  TtaRemoveTree (termList, doc);
                  TtaInsertFirstChild (&termList, newEl, doc);
                }
              if (el != NULL)
                {
                  elType = TtaGetElementType (el);
                  if (elType.ElTypeNum == HTML_EL_Definition &&
                      elType.ElSSchema == htmlSSchema)
                    /* the following element is a definition */
                    {
                      /* search all Definition siblings, ignoring
                         Comments, PI and Invalid_elements */
                      firstDef = el;
                      do
                        {
                          lastDef = el;
                          TtaNextSibling (&el);
                          if (el == NULL)
                            elType.ElTypeNum = 0;
                          else
                            elType = TtaGetElementType (el);
                        }
                      while (elType.ElSSchema == htmlSSchema &&
                             (elType.ElTypeNum == HTML_EL_Definition ||
                              elType.ElTypeNum == HTML_EL_Invalid_element ||
                              elType.ElTypeNum == HTML_EL_Comment_ ||
                              elType.ElTypeNum == HTML_EL_XMLPI));
                      if (!ParentOfType (firstDef, HTML_EL_Definitions))
                        {
                          /* create a Definitions element after the
                             Term_List element */
                          newElType.ElSSchema = htmlSSchema;
                          newElType.ElTypeNum = HTML_EL_Definitions;
                          defList = TtaNewElement (doc, newElType);
                          TtaInsertSibling (defList, termList, FALSE, doc);
                          /* move the Definitions elements as children of the
                             new Definitions element */
                          nextEl = firstDef;
                          TtaNextSibling (&nextEl);
                          TtaRemoveTree (firstDef, doc);
                          TtaInsertFirstChild (&firstDef, defList, doc);
                          if (lastDef != firstDef)
                            {
                              prevEl = firstDef;
                              do
                                {
                                  child = nextEl;
                                  TtaNextSibling (&nextEl);
                                  TtaRemoveTree (child, doc);
                                  TtaInsertSibling (child, prevEl, FALSE, doc);
                                  prevEl = child;
                                }
                              while (nextEl != NULL && child != lastDef);
                            }
                        }
                    }
                }
              /* starting element for the next search of a Term element */
              el = lastTerm;
            }
        }

      /* search all Definition elements without a Definitions parent */
      el = TtaGetFirstChild (elRoot);
      if (el != NULL)
        {
          /* search all Definition elements in the document */
          while (el != NULL)
            {
              elType.ElSSchema = htmlSSchema;
              elType.ElTypeNum = HTML_EL_Definition;
              el = TtaSearchTypedElement (elType, SearchForward, el);
              if (el != NULL)
                /* an element Definition has been found */
                {
                  if (!ParentOfType (el, HTML_EL_Definitions))
                    /* this Definition is not within a Definitions
                       element */
                    {
                      /* search all Definition siblings, ignoring Comments,
                         PIs and Invalid_elements */
                      firstDef = el;
                      do
                        {
                          lastDef = el;
                          TtaNextSibling (&el);
                          if (el == NULL)
                            elType.ElTypeNum = 0;
                          else
                            elType = TtaGetElementType (el);
                        }
                      while (elType.ElSSchema == htmlSSchema &&
                             (elType.ElTypeNum == HTML_EL_Definition ||
                              elType.ElTypeNum == HTML_EL_Invalid_element ||
                              elType.ElTypeNum == HTML_EL_Comment_ ||
                              elType.ElTypeNum == HTML_EL_XMLPI));
                      /* create a Definitions element */
                      newElType.ElSSchema = htmlSSchema;
                      newElType.ElTypeNum = HTML_EL_Definitions;
                      defList = TtaNewElement (doc, newElType);
                      TtaInsertSibling (defList, firstDef, TRUE, doc);
                      TtaRemoveTree (firstDef, doc);
                      TtaInsertFirstChild (&firstDef, defList, doc);
                      if (!ParentOfType (defList, HTML_EL_Definition_Item))
                        /* this Definition is not within a Definition_Item
                           element */
                        {
                          /* create a Definition_Item */
                          newElType.ElTypeNum = HTML_EL_Definition_Item;
                          newEl = TtaNewElement (doc, newElType);
                          TtaInsertSibling (newEl, defList, TRUE, doc);
                          TtaRemoveTree (defList, doc);
                          TtaInsertFirstChild (&defList, newEl, doc);
                        }
                      /* move the Definitions elements as children of the
                         Definitions element */
                      nextEl = firstDef;
                      TtaNextSibling (&nextEl);
                      if (lastDef != firstDef)
                        {
                          prevEl = firstDef;
                          do
                            {
                              child = nextEl;
                              TtaNextSibling (&nextEl);
                              TtaRemoveTree (child, doc);
                              TtaInsertSibling (child, prevEl, FALSE, doc);
                              prevEl = child;
                            }
                          while (nextEl != NULL && child != lastDef);
                        }
                    }
                }
            }
        }

      if (!isXTiger)
        {
          /* create a surrounding element Definition_List for each sequence */
          /* of elements Definition_Item which are not in a Definition_List */
          el = TtaGetFirstChild (elRoot);
          if (el != NULL)
            {
              /* search all elements Definition_Item in the document */
              while (el != NULL)
                {
                  elType.ElSSchema = htmlSSchema;
                  elType.ElTypeNum = HTML_EL_Definition_Item;
                  el = TtaSearchTypedElement (elType, SearchForward, el);
                  if (el != NULL)
                    /* an element Definition_Item has been found */
                    {
                      if (!ParentOfType (el, HTML_EL_Definition_List))
                        /* this Definition_Item is not a child of a Definition_List*/
                        {
                          /* search all immediate Definition_Item siblings */
                          firstEntry = el;
                          do
                            {
                              lastEntry = el;
                              TtaNextSibling (&el);
                              if (el == NULL)
                                elType.ElTypeNum = 0;
                              else
                                elType = TtaGetElementType (el);
                            }
                          while (elType.ElSSchema == htmlSSchema &&
                                 (elType.ElTypeNum == HTML_EL_Definition_Item ||
                                  elType.ElTypeNum == HTML_EL_Invalid_element ||
                                  elType.ElTypeNum == HTML_EL_Comment_ ||
                                  elType.ElTypeNum == HTML_EL_XMLPI));
                          /* create a Definition_List element before the */
                          /* first Definition_Item element */
                          newElType.ElSSchema = htmlSSchema;
                          newElType.ElTypeNum = HTML_EL_Definition_List;
                          glossary = TtaNewElement (doc, newElType);
                          TtaInsertSibling (glossary, firstEntry, TRUE, doc);
                          /* move the Definition_Item elements as children */
                          /* of the new Definition_List element */
                          nextEl = firstEntry;
                          TtaNextSibling (&nextEl);
                          TtaRemoveTree (firstEntry, doc);
                          TtaInsertFirstChild (&firstEntry, glossary, doc);
                          if (lastEntry != firstEntry)
                            {
                              prevEl = firstEntry;
                              do
                                {
                                  child = nextEl;
                                  TtaNextSibling (&nextEl);
                                  TtaRemoveTree (child, doc);
                                  TtaInsertSibling (child, prevEl, FALSE, doc);
                                  prevEl = child;
                                }
                              while (nextEl != NULL && child != lastEntry);
                            }
                          /* starting element for the next search of a */
                          /* Definition_Item */
                          el = lastEntry;
                        }
                    }
                }
            }

          /* create a surrounding element Unnumbered_List for each sequence */
          /* of elements List_Item which are not in a Unnumbered_List, a */
          /* Numbered_List, a Menu, or a Directory */
          el = TtaGetFirstChild (elRoot);
          if (el != NULL)
            {
              /* search all elements List_Item in the document */
              do
                {
                  elType.ElSSchema = htmlSSchema;
                  elType.ElTypeNum = HTML_EL_List_Item;
                  el = TtaSearchTypedElement (elType, SearchForward, el);
                  if (el != NULL)
                    /* an element List_Item has been found */
                    {
                      parent = TtaGetParent (el);
                      elType = TtaGetElementType (parent);
#ifdef TEMPLATES
                      /* if its parent is a Template element, skip it */
                      while (strcmp(TtaGetSSchemaName(elType.ElSSchema),"Template") == 0)
                        {
                          parent = TtaGetParent (parent);
                          elType = TtaGetElementType (parent);
                        }
#endif /* TEMPLATES */
                      if (elType.ElSSchema != htmlSSchema ||
                          (elType.ElTypeNum != HTML_EL_Unnumbered_List &&
                           elType.ElTypeNum != HTML_EL_Numbered_List &&
                           elType.ElTypeNum != HTML_EL_Menu &&
                           elType.ElTypeNum != HTML_EL_Directory))
                        /* this List_Item is not within a list */
                        {
                          /* search all immediate List_Item siblings */
                          firstEntry = el;
                          do
                            {
                              lastEntry = el;
                              TtaNextSibling (&el);
                              if (el == NULL)
                                elType.ElTypeNum = 0;
                              else
                                elType = TtaGetElementType (el);
                            }
                          while (elType.ElSSchema == htmlSSchema &&
                                 (elType.ElTypeNum == HTML_EL_List_Item ||
                                  elType.ElTypeNum == HTML_EL_Invalid_element ||
                                  elType.ElTypeNum == HTML_EL_Comment_ ||
                                  elType.ElTypeNum == HTML_EL_XMLPI));
                          /* create a HTML_EL_Unnumbered_List element before
                             the first List_Item element */
                          newElType.ElSSchema = htmlSSchema;
                          newElType.ElTypeNum = HTML_EL_Unnumbered_List;
                          list = TtaNewElement (doc, newElType);
                          TtaInsertSibling (list, firstEntry, TRUE, doc);
                          /* move the List_Item elements as children of */
                          /* the new HTML_EL_Unnumbered_List element */
                          nextEl = firstEntry;
                          TtaNextSibling (&nextEl);
                          TtaRemoveTree (firstEntry, doc);
                          TtaInsertFirstChild (&firstEntry, list, doc);
                          if (lastEntry != firstEntry)
                            {
                              prevEl = firstEntry;
                              do
                                {
                                  child = nextEl;
                                  TtaNextSibling (&nextEl);
                                  TtaRemoveTree (child, doc);
                                  TtaInsertSibling (child, prevEl, FALSE, doc);
                                  prevEl = child;
                                }
                              while (nextEl != NULL && child != lastEntry);
                            }
                          /* starting element for the next search of a
                             List_Item */
                          el = lastEntry;
                        }
                    }
                }
              while (el);
            }
        }

      /* merge sibling Text elements with same attributes */
      el = elRoot;
      elType.ElSSchema = htmlSSchema;
      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
      /* search all TEXT elements in the document */
      while (el != NULL)
        {
          /* search the next TEXT element in the abstract tree */
          el = TtaSearchTypedElement (elType, SearchForward, el);
          if (el != NULL)
            /* a Text element has been found. Try to merge it with its */
            /* following siblings */
            do
              ok = TtaMergeText (el, doc);
            while (ok);
        }

#ifdef IV
      /* checks all MAP elements. If they are within a Block element, */
      /* move them up in the structure */
      el = elRoot;
      elType.ElSSchema = htmlSSchema;
      elType.ElTypeNum = HTML_EL_map;
      /* search all MAP elements in the document */
      while (el)
        {
          /* search the next MAP element in the abstract tree */
          el = TtaSearchTypedElement (elType, SearchForward, el);
          if (el != NULL)
            /* a MAP element has been found. */
            {
              parent = TtaGetParent(el);
#ifdef TEMPLATES
              /* if its parent is a Template element, skip it */
              elType = TtaGetElementType (parent);
              while (strcmp(TtaGetSSchemaName(elType.ElSSchema),"Template") == 0)
                {
                  parent = TtaGetParent (parent);
                  elType = TtaGetElementType (parent);
                }
#endif /* TEMPLATES */
              if (IsBlockElement (parent))
                /* its parent is a block element */
                {
                  TtaRemoveTree (el, doc);
                  TtaInsertSibling (el, parent, TRUE, doc);
                }
            }
        }
#endif /* IV */

      /* If element BODY is empty, create an empty element as a placeholder*/
      if (elBody)
        {
          el = TtaGetFirstChild (elBody);
          elType = TtaGetElementType (el);
          while (el && TtaHasNotElementException(TtaGetElementType (el)))
            TtaNextSibling (&el);
          if (el == NULL)
            {
              newElType.ElSSchema = htmlSSchema;
              newElType.ElTypeNum = HTML_EL_Element;
              newEl = TtaNewElement (doc, newElType);
              TtaInsertFirstChild (&newEl, elBody, doc);
            }
        }
      /* add additional checking here */
    }
}

/*----------------------------------------------------------------------
  InitializeHTMLParser
  initializes variables and stack for parsing file
  the parser will insert the thot tree after or as a child
  of last elem, in the document doc.
  If last elem is NULL or doc=0, the parser doesn't initialize
  the stack
  ----------------------------------------------------------------------*/
static void InitializeHTMLParser (Element lastelem, ThotBool isclosed,
                                  Document doc)
{
  char         tag[20];
  Element      elem;
  int          i;
  SSchema      schema;

  StackLevel = 1;
  HTMLcontext.language = TtaGetDefaultLanguage ();
  HTMLcontext.parsingTextArea = FALSE;
  HTMLcontext.parsingScript = FALSE;
  HTMLcontext.parsingCSS = FALSE;
  if (lastelem != NULL && doc != 0)
    {
      /* initialize the stack with ancestors of lastelem */
      HTMLcontext.doc = doc;
      DocumentSSchema = TtaGetDocumentSSchema (HTMLcontext.doc);
#ifdef ANNOTATIONS
      if (DocumentTypes[doc] == docAnnot)
        rootElement = ANNOT_GetHTMLRoot (doc, TRUE); 
      else
#endif /* ANNOTATIONS */
        rootElement = TtaGetMainRoot (HTMLcontext.doc);
      if (isclosed)
        elem = TtaGetParent (lastelem);
      else
        elem = lastelem;
      while (elem != NULL && elem != rootElement && StackLevel < MaxStack-2)
        {
          strcpy ((char *)tag, (char *)GetXMLElementName (TtaGetElementType (elem), doc));
          if (strcmp (tag, "???"))
            {
              for (i = StackLevel; i > 0; i--)
                {
                  GINumberStack[i + 1] = GINumberStack[i];
                  ElementStack[i + 1] = ElementStack[i];
                  LanguageStack[i + 1] = LanguageStack[i];
                  ThotLevel[i + 1] = ThotLevel[i] + 1;
                }
              schema = DocumentSSchema;
              GINumberStack[1] = MapGI ((char *)tag, &schema, HTMLcontext.doc);
              ElementStack[1] = elem;
              ThotLevel[1] = 1;
              LanguageStack[1] = HTMLcontext.language;
              StackLevel++;
            }
          elem = TtaGetParent (elem);
        }
      HTMLcontext.lastElement = lastelem;
      HTMLcontext.lastElementClosed = isclosed;
    }
  else
    {
      HTMLcontext.lastElement = rootElement;
      HTMLcontext.lastElementClosed = FALSE;
    }
  NumberOfCharRead = 0;
  NumberOfLinesRead = 1;
  /* input file is supposed to be HTML */
  GINumberStack[0] = -1;
  ElementStack[0] = rootElement;
  ThotLevel[0] = 1;
  lastElemEntry = 0;
  /* initialize input buffer */
  EmptyLine = TRUE;
  StartOfFile = TRUE;
  inputBuffer[0] = EOS;
  LgBuffer = 0;
  lastAttribute = NULL;
  lastAttrElement = NULL;
  lastAttrEntry = NULL;
  UnknownAttr = FALSE;
  ReadingAnAttrValue = FALSE;
  LgEntityName = 0;
  EntityTableEntry = 0;
  CharRank = 0;
  HTMLcontext.mergeText = FALSE;
  AfterTagPRE = FALSE;
  HTMLcontext.parsingCSS = FALSE;
  CurrentBufChar = 0;
}

/*----------------------------------------------------------------------
  ParseIncludedHTML
  ----------------------------------------------------------------------*/
void ParseIncludedHTML (Element elem, char *closingTag)
{
  Element    oldLastElement;
  ThotBool   oldLastElementClosed;
  int	      oldLastElemEntry;
   

  oldLastElement = HTMLcontext.lastElement;
  HTMLcontext.lastElement = elem;
  oldLastElementClosed = HTMLcontext.lastElementClosed;
  HTMLcontext.lastElementClosed = FALSE;
  oldLastElemEntry = lastElemEntry;
  lastElemEntry = 0;
  HTMLrootClosingTag = closingTag;
  /* TODO: the XML parser must call that function with two new parameters:
     the current infile and current index */
  HTMLparse ((FILE*)stream, NULL);

  HTMLcontext.lastElement = oldLastElement;
  HTMLcontext.lastElementClosed = oldLastElementClosed;
  lastElemEntry = oldLastElemEntry;
}


/*----------------------------------------------------------------------
  ParseSubTree called by tranformation.
  ----------------------------------------------------------------------*/
void ParseSubTree (char* HTMLbuf, Element lastelem, Language language,
                   ThotBool isclosed, Document doc)
{
  ElementType  elType;
  char        *schemaName;

  docURL = NULL;
  docURL2 = NULL;
  elType = TtaGetElementType (lastelem);
  schemaName = TtaGetSSchemaName(elType.ElSSchema);
  if (strcmp (schemaName, "HTML") == 0)
    /* parse an HTML subtree */
    {
      InitializeHTMLParser (lastelem, isclosed, doc);
      /* Initialize the language context with the 'lastelem' language*/
      HTMLcontext.language = language;
      /* transformation files are alway encoded in UTF_8 */
      HTMLcontext.encoding = UTF_8;
      /* We set number line with 0 when we are parsing a sub-tree */
      NumberOfLinesRead = 0;
      HTMLparse (NULL, HTMLbuf);
      /* Handle character-level elements which contain block-level elements */
      TtaSetStructureChecking (FALSE, doc);
      CheckBlocksInCharElem (doc);
      TtaSetStructureChecking (TRUE, doc);
    }
  else
    {
      InputText = HTMLbuf; 
      /* InputText = HTMLbuf; */
      CurrentBufChar = 0;
      if (!ParseXmlBuffer (InputText, lastelem, isclosed, doc, language, NULL))
        StopParsing (doc);
    }
}

/*-------------------------------------------------------------------------------
  ParseExternalHTMLDoc
  Parse an external HTML document called from an other document
  ------------------------------------------------------------------------------*/
void ParseExternalHTMLDoc (Document doc, FILE * infile, CHARSET charset, char *extDocURL)
{
  Element         el, oldel;
  int             error;

  /* Context initialization */
  HTMLcontext.doc = doc;
  FirstElemToBeChecked = NULL;
  LastElemToBeChecked = NULL;
  HTMLcontext.lastElement = NULL;
  HTMLcontext.lastElementClosed = FALSE;
  lastElemEntry = 0;
  lastAttribute = NULL;
  lastAttrElement = NULL;
  lastAttrEntry = NULL;
  UnknownAttr = FALSE;
  ReadingAnAttrValue = FALSE;
  CommentText = NULL;
  UnknownTag = FALSE;
  HTMLcontext.mergeText = FALSE;
  LgEntityName = 0;
  EntityTableEntry = 0;
  CharRank = 0;
  HTMLcontext.encoding = TtaGetDocumentCharset (doc);
  HTMLcontext.withinTable = 0;
  LastCharInWorkBuffer = 0;
  FileBuffer[0] = EOS;
  HTMLcontext.language = TtaGetDefaultLanguage ();
  DocumentSSchema = TtaGetDocumentSSchema (doc);

  rootElement = TtaGetMainRoot (doc);
  /* delete all element except the root element and its parent document
     element */
  el = TtaGetFirstChild (rootElement);
  while (el != NULL)
    {
      oldel = el;
      TtaNextSibling (&el);
      TtaDeleteTree (oldel, doc);
    }

  docURL = (char*)TtaGetMemory (strlen ((char *)extDocURL) + 1);
  strcpy ((char *)docURL, (char *)extDocURL);

  /* Check if it's a valid encoding */
  if (DocumentMeta[doc]->charset)
    {
      charset = TtaGetCharset (DocumentMeta[doc]->charset);
      if (charset != UTF_8        && charset != ISO_8859_1   &&
          charset != ISO_8859_2   && charset != ISO_8859_3   &&
          charset != ISO_8859_4   && charset != ISO_8859_5   &&
          charset != ISO_8859_6   && charset != ISO_8859_7   &&
          charset != ISO_8859_8   && charset != ISO_8859_9   &&
          charset != ISO_8859_15  && charset != KOI8_R       &&
          charset != WINDOWS_1250 && charset != WINDOWS_1251 &&
          charset != WINDOWS_1252 && charset != WINDOWS_1253 &&
          charset != WINDOWS_1254 && charset != WINDOWS_1255 &&
          charset != WINDOWS_1256 && charset != WINDOWS_1257 &&
          charset != US_ASCII     && charset != SHIFT_JIS    &&
          charset != ISO_2022_JP  && charset != EUC_JP       &&
          charset != SHIFT_JIS    && charset != GB_2312)
        HTMLParseError (doc,
                        TtaGetMessage (AMAYA, AM_UNKNOWN_ENCODING), 0);
    }
  
  /* parse the input file and build the external document */
  /* initialize parsing environment */
  InitializeHTMLParser (NULL, FALSE, 0);
  HTMLparse (infile, NULL);
  /* completes all unclosed elements */
  el = HTMLcontext.lastElement;
  while (el != NULL)
    {
      XhtmlElementComplete (&HTMLcontext, el, &error);
      el = TtaGetParent (el);
    }
 
  if (docURL)
    {
      TtaFreeMemory (docURL);
      docURL = NULL;
    }
  DocumentSSchema = NULL;
  HTMLcontext.doc = 0;
  CleanUpParsingErrors ();
  return;
}

/*-------------------------------------------------------------------------------
  ClearHTMLParser 
  Clear all parser variables
  ------------------------------------------------------------------------------*/
void ClearHTMLParser ()
{
  PtrElemToBeChecked  elTBC;

  /* clean up the list of ElemToBeChecked */
  elTBC = FirstElemToBeChecked;
  while (FirstElemToBeChecked)
    {
      LastElemToBeChecked = FirstElemToBeChecked->nextElemToBeChecked;
      TtaFreeMemory (FirstElemToBeChecked);
      FirstElemToBeChecked = LastElemToBeChecked;
    }
  FirstElemToBeChecked = NULL;
  LastElemToBeChecked = NULL;
  lastElemEntry = 0;
  lastAttribute = NULL;
  lastAttrElement = NULL;
  lastAttrEntry = NULL;
  UnknownAttr = FALSE;
  ReadingAnAttrValue = FALSE;
  CommentText = NULL;
  UnknownTag = FALSE;
  LastCharInWorkBuffer = 0;
  FileBuffer[0] = EOS;
  LgEntityName = 0;
  EntityTableEntry = 0;
  CharRank = 0;
}

/*-------------------------------------------------------------------------------
  StartParser 
  Loads the file Directory/htmlFileName for displaying the document documentName.
  The parameter pathURL gives the original (local or distant)
  path or URL of the html document.
  ------------------------------------------------------------------------------*/
void StartParser (Document doc, char *fileName,
                  const char *documentName, char* documentDirectory,
                  const char *pathURL, ThotBool plainText, ThotBool external_doc)
{
  DisplayMode     dispMode;
  CHARSET         charset;
  Element         el, oldel, root;
  AttributeType   attrType;
  Attribute       attr;
  char           *s;
  char            tempname[MAX_LENGTH];
  char            temppath[MAX_LENGTH];
  ThotBool        isHTML;
  int             error;

  HTMLcontext.doc = doc;
  FirstElemToBeChecked = NULL;
  LastElemToBeChecked = NULL;
  HTMLcontext.lastElement = NULL;
  HTMLcontext.lastElementClosed = FALSE;
  lastElemEntry = 0;
  lastAttribute = NULL;
  lastAttrElement = NULL;
  lastAttrEntry = NULL;
  UnknownAttr = FALSE;
  ReadingAnAttrValue = FALSE;
  CommentText = NULL;
  UnknownTag = FALSE;
  HTMLcontext.mergeText = FALSE;
  HTMLcontext.withinTable = 0;
  LastCharInWorkBuffer = 0;
  FileBuffer[0] = EOS;
  LgEntityName = 0;
  EntityTableEntry = 0;
  CharRank = 0;

  HTMLcontext.encoding = TtaGetDocumentCharset (doc);
  stream = TtaGZOpen (fileName);
  if (stream != 0)
    {
      if (documentName[0] == EOS && !TtaCheckDirectory (documentDirectory))
        {
          strcpy ((char *)documentName, (char *)documentDirectory);
          documentDirectory[0] = EOS;
          s = TtaGetEnvString ("PWD");
          /* set path on current directory */
          if (s != NULL)
            strcpy ((char *)documentDirectory, (char *)s);
          else
            documentDirectory[0] = EOS;
        }
      TtaAppendDocumentPath (documentDirectory);

      /* Set document URL */
      if (DocumentURLs[doc])
        {
          docURL = (char*)TtaGetMemory (strlen ((char *)DocumentURLs[doc]) + 1);
          strcpy ((char *)docURL, (char *)DocumentURLs[doc]);
        }
      else
        {
          docURL = (char*)TtaGetMemory (strlen ((char *)pathURL) + 1);
          strcpy ((char *)docURL, (char *)pathURL);
        }
      /* Set document URL2 */
      if (docURL)
        {
          docURL2 = (char *)TtaGetMemory (strlen ((char *)docURL) + 1);
          strcpy ((char *)docURL2, (char *)docURL);
        }


      /* do not check the Thot abstract tree against the structure */
      /* schema while building the Thot document. */
      TtaSetStructureChecking (FALSE, doc);
      /* set the notification mode for the new document */
      TtaSetNotificationMode (doc, 1);
      HTMLcontext.language = TtaGetDefaultLanguage ();
#ifdef ANNOTATIONS
      if (DocumentTypes[doc] == docAnnot)
        {
          /* get the schema associated to the annotation body */
          DocumentSSchema = ANNOT_GetBodySSchema (doc);
          attrType.AttrSSchema = DocumentSSchema;
        }
      else
#endif /* ANNOTATIONS */
        DocumentSSchema = TtaGetDocumentSSchema (doc);

      /* is the current document a HTML document */
      isHTML = (strcmp (TtaGetSSchemaName (DocumentSSchema), "HTML") == 0);
      if (plainText)
        {
#ifdef ANNOTATIONS
          if (DocumentTypes[doc] == docAnnot)
            rootElement = ANNOT_GetHTMLRoot (doc, TRUE); 
          else
#endif /* ANNOTATIONS */
            rootElement = TtaGetRootElement (doc);

          if (DocumentTypes[doc] == docSource || DocumentTypes[doc] == docCSS)
            {
              /* add the attribute Source */
              attrType.AttrSSchema = DocumentSSchema;
              attrType.AttrTypeNum = TextFile_ATTR_Source;
              attr = TtaGetAttribute (rootElement, attrType);
              if (attr == 0)
                {
                  attr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (rootElement, attr, doc);
                }
            }
	  
          /* add the default attribute PrintURL */
          attrType.AttrSSchema = DocumentSSchema;
          attrType.AttrTypeNum = TextFile_ATTR_PrintURL;
          attr = TtaGetAttribute (rootElement, attrType);
          if (attr == 0)
            {
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (rootElement, attr, doc);
            }
        }
      else
        {
          /* Check if it's a valid encoding */
          if (DocumentMeta[doc]->charset)
            {
              charset = TtaGetCharset (DocumentMeta[doc]->charset);
              if (charset != UTF_8        && charset != ISO_8859_1   &&
                  charset != ISO_8859_2   && charset != ISO_8859_3   &&
                  charset != ISO_8859_4   && charset != ISO_8859_5   &&
                  charset != ISO_8859_6   && charset != ISO_8859_7   &&
                  charset != ISO_8859_8   && charset != ISO_8859_9   &&
                  charset != ISO_8859_15  && charset != KOI8_R       &&
                  charset != WINDOWS_1250 && charset != WINDOWS_1251 &&
                  charset != WINDOWS_1252 && charset != WINDOWS_1253 &&
                  charset != WINDOWS_1254 && charset != WINDOWS_1255 &&
                  charset != WINDOWS_1256 && charset != WINDOWS_1257 &&
                  charset != US_ASCII     && charset != SHIFT_JIS    &&
                  charset != ISO_2022_JP  && charset != EUC_JP       &&
                  charset != SHIFT_JIS    && charset != GB_2312)
                HTMLParseError (doc,
                                TtaGetMessage (AMAYA, AM_UNKNOWN_ENCODING), 0);
            }
          if (!isHTML)
            {
              /* change the document type */
              TtaFreeView (doc, 1);
              doc = TtaNewDocument ("HTML", documentName);
              if (TtaGetScreenDepth () > 1)
                TtaSetPSchema (doc, "HTMLP");
              else
                TtaSetPSchema (doc, "HTMLPBW");
              DocumentSSchema = TtaGetDocumentSSchema (doc);
              /* set attribute dir on the Document element. */
              root = TtaGetMainRoot (doc);
              if (root)
                {
                  attrType.AttrSSchema = DocumentSSchema;
                  attrType.AttrTypeNum = HTML_ATTR_dir;
                  attr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (root, attr, doc);
                  TtaSetAttributeValue (attr, HTML_ATTR_dir_VAL_ltr_, root,
                                        doc);
                }
              isHTML = TRUE;
            }

#ifdef ANNOTATIONS
          if (DocumentTypes[doc] == docAnnot)
            rootElement = ANNOT_GetHTMLRoot (doc, FALSE); 
          else
#endif /* ANNOTATIONS */
            rootElement = TtaGetMainRoot (doc);
          /* add the default attribute PrintURL */
          attrType.AttrSSchema = DocumentSSchema;
          attrType.AttrTypeNum = HTML_ATTR_PrintURL;
          attr = TtaGetAttribute (rootElement, attrType);
          if (!attr)
            {
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (rootElement, attr, doc);
            }
          if (MapAreas[doc])
            ChangeAttrOnRoot (doc, HTML_ATTR_ShowAreas);
        }

      dispMode = TtaGetDisplayMode (doc);
      if (dispMode != NoComputedDisplay)
        TtaSetDisplayMode (doc, NoComputedDisplay);
      /* delete all element except the root element and its parent document
         element */
      el = TtaGetFirstChild (rootElement);
      while (el != NULL)
        {
          oldel = el;
          TtaNextSibling (&el);
          TtaDeleteTree (oldel, doc);
        }

      /* save the path or URL of the document */
      TtaExtractName (pathURL, temppath, tempname);
      TtaSetDocumentDirectory (doc, temppath);
      // change the type of the root element if needed
      if (isHTML)
        TtaUpdateRootElementType (rootElement, "HTML", doc);

      /* parse the input file and build the Thot document */
      if (plainText)
        ReadTextFile ((FILE*)stream, NULL, doc, pathURL);
      else
        {
          /* initialize parsing environment */
          InitializeHTMLParser (NULL, FALSE, 0);
          HTMLparse ((FILE*)stream, NULL);
          /* completes all unclosed elements */
          el = HTMLcontext.lastElement;
          while (el != NULL)
            {
              XhtmlElementComplete (&HTMLcontext, el, &error);
              el = TtaGetParent (el);
            }
          /* check the Thot abstract tree */
          CheckAbstractTree (HTMLcontext.doc, IsXTiger (documentName));
          // now load the user style sheet
          if (!external_doc)
            {
              LoadUserStyleSheet (doc);
              UpdateStyleList (doc, 1);
            }
        }

      TtaGZClose (stream);
      if (docURL)
        {
          TtaFreeMemory (docURL);
          docURL = NULL;
        }
      if (docURL2)
        {
          TtaFreeMemory (docURL2);
          docURL2 = NULL;
        }

      if (!external_doc)
        TtaSetDisplayMode (doc, dispMode);

      /* check the Thot abstract tree against the structure schema. */
      TtaSetStructureChecking (TRUE, doc);

      DocumentSSchema = NULL;
    }

  TtaSetDocumentUnmodified (doc);
  HTMLcontext.doc = 0;
}

/* end of module */
