/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * html2thot parses a HTML file and builds the corresponding abstract tree
 * for a Thot document of type HTML.
 *
 * Author: V. Quint
 *         R. Guetari (W3C/INRIA): Unicode version 
 */

/* Compiling this module with -DSTANDALONE generates the main program of  */
/* a converter which reads a HTML file and creates a Thot .PIV file.      */
/* Without this option, it creates a function StartParser that parses a   */
/* file and creates the internal representation of a Thot document.       */

#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "fetchHTMLname.h"
#include "zlib.h"
#include "parser.h"
#include "css_f.h"
#include "fetchHTMLname_f.h"
#include "fetchXMLname_f.h"
#include "html2thot_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLimage_f.h"
#include "HTMLtable_f.h"
#include "HTMLimage_f.h"
#include "styleparser_f.h"
#include "templates_f.h"
#include "XMLparser_f.h"
#include "UIcss_f.h"

#ifdef ANNOTATIONS
#include "annotlib.h"
#endif /* ANNOTATIONS */

#ifdef EXPAT
#include "xmlparse.h"
#endif /* EXPAT */


typedef CHAR_T entityName[10];
typedef struct _CharEntityEntry
  {			 /* a SGML entity representing an ISO-Latin1 char */
     entityName          charName;	/* entity name */
     int                 charCode;	/* decimal code of ISO-Latin1 char */
  }
CharEntityEntry;


CharEntityEntry        CharEntityTable2[] =
{
   /* This table MUST be in alphabetical order */
{TEXT("AElig"), 198},		/* latin capital letter AE = latin capital ligature AE, U+00C6 ISOlat1 */
{TEXT("Aacute"), 193},	/* latin capital letter A with acute, U+00C1 ISOlat1 */
{TEXT("Acirc"), 194},		/* latin capital letter A with circumflex, U+00C2 ISOlat1 */
{TEXT("Agrave"), 192},	/* latin capital letter A with grave = latin capital letter A grave, U+00C0 ISOlat1 */
{TEXT("Alpha"), 913},		/* greek capital letter alpha, U+0391 */
{TEXT("Aring"), 197},		/* latin capital letter A with ring above = latin capital letter A ring, U+00C5 ISOlat1 */
{TEXT("Atilde"), 195},	/* latin capital letter A with tilde, U+00C3 ISOlat1 */
{TEXT("Auml"), 196},		/* latin capital letter A with diaeresis, U+00C4 ISOlat1 */
{TEXT("Beta"), 914},		/* greek capital letter beta, U+0392 */
{TEXT("Ccedil"), 199},	/* latin capital letter C with cedilla, U+00C7 ISOlat1 */
{TEXT("Chi"), 935},		/* greek capital letter chi, U+03A7 */
{TEXT("Dagger"), 8225},	/* double dagger, U+2021 ISOpub */
{TEXT("Delta"), 916},		/* greek capital letter delta, U+0394 ISOgrk3 */
{TEXT("ETH"), 208},		/* latin capital letter ETH, U+00D0 ISOlat1 */
{TEXT("Eacute"), 201},	/* latin capital letter E with acute, U+00C9 ISOlat1 */
{TEXT("Ecirc"), 202},		/* latin capital letter E with circumflex, U+00CA ISOlat1 */
{TEXT("Egrave"), 200},	/* latin capital letter E with grave, U+00C8 ISOlat1 */
{TEXT("Epsilon"), 917},	/* greek capital letter epsilon, U+0395 */
{TEXT("Eta"), 919},		/* greek capital letter eta, U+0397 */
{TEXT("Euml"), 203},		/* latin capital letter E with diaeresis, U+00CB ISOlat1 */
{TEXT("Gamma"), 915},		/* greek capital letter gamma, U+0393 ISOgrk3 */
{TEXT("Iacute"), 205},	/* latin capital letter I with acute, U+00CD ISOlat1 */
{TEXT("Icirc"), 206},		/* latin capital letter I with circumflex, U+00CE ISOlat1 */
{TEXT("Igrave"), 204},	/* latin capital letter I with grave, U+00CC ISOlat1 */
{TEXT("Iota"), 921},		/* greek capital letter iota, U+0399 */
{TEXT("Iuml"), 207},		/* latin capital letter I with diaeresis, U+00CF ISOlat1 */
{TEXT("Kappa"), 922},		/* greek capital letter kappa, U+039A */
{TEXT("Lambda"), 923},	/* greek capital letter lambda, U+039B ISOgrk3 */
{TEXT("Mu"), 924},		/* greek capital letter mu, U+039C */
{TEXT("Ntilde"), 209},	/* latin capital letter N with tilde, U+00D1 ISOlat1 */
{TEXT("Nu"), 925},		/* greek capital letter nu, U+039D */
{TEXT("OElig"), 338},		/* latin capital ligature OE, U+0152 ISOlat2 */
{TEXT("Oacute"), 211},	/* latin capital letter O with acute, U+00D3 ISOlat1 */
{TEXT("Ocirc"), 212},		/* latin capital letter O with circumflex, U+00D4 ISOlat1 */
{TEXT("Ograve"), 210},	/* latin capital letter O with grave, U+00D2 ISOlat1 */
{TEXT("Omega"), 937},		/* greek capital letter omega, U+03A9 ISOgrk3 */
{TEXT("Omicron"), 927},	/* greek capital letter omicron, U+039F */
{TEXT("Oslash"), 216},	/* latin capital letter O with stroke = latin capital letter O slash, U+00D8 ISOlat1 */
{TEXT("Otilde"), 213},	/* latin capital letter O with tilde, U+00D5 ISOlat1 */
{TEXT("Ouml"), 214},		/* latin capital letter O with diaeresis, U+00D6 ISOlat1 */
{TEXT("Phi"), 934},		/* greek capital letter phi, U+03A6 ISOgrk3 */
{TEXT("Pi"), 928},		/* greek capital letter pi, U+03A0 ISOgrk3 */
{TEXT("Prime"), 8243},	/* double prime = seconds = inches, U+2033 ISOtech */
{TEXT("Psi"), 936},		/* greek capital letter psi, U+03A8 ISOgrk3 */
{TEXT("Rho"), 929},		/* greek capital letter rho, U+03A1 */
{TEXT("Scaron"), 352},	/* latin capital letter S with caron, U+0160 ISOlat2 */
{TEXT("Sigma"), 931},		/* greek capital letter sigma, U+03A3 ISOgrk3 */
{TEXT("THORN"), 222},		/* latin capital letter THORN, U+00DE ISOlat1 */
{TEXT("Tau"), 932},		/* greek capital letter tau, U+03A4 */
{TEXT("Theta"), 920},		/* greek capital letter theta, U+0398 ISOgrk3 */
{TEXT("Uacute"), 218},	/* latin capital letter U with acute, U+00DA ISOlat1 */
{TEXT("Ucirc"), 219},		/* latin capital letter U with circumflex, U+00DB ISOlat1 */
{TEXT("Ugrave"), 217},	/* latin capital letter U with grave, U+00D9 ISOlat1 */
{TEXT("Upsilon"), 933},	/* greek capital letter upsilon, U+03A5 ISOgrk3 */
{TEXT("Uuml"), 220},		/* latin capital letter U with diaeresis, U+00DC ISOlat1 */
{TEXT("Xi"), 926},		/* greek capital letter xi, U+039E ISOgrk3 */
{TEXT("Yacute"), 221},	/* latin capital letter Y with acute, U+00DD ISOlat1 */
{TEXT("Yuml"), 376},		/* latin capital letter Y with diaeresis, U+0178 ISOlat2 */
{TEXT("Zeta"), 918},		/* greek capital letter zeta, U+0396 */
{TEXT("aacute"), 225},	/* latin small letter a with acute, U+00E1 ISOlat1 */
{TEXT("acirc"), 226},		/* latin small letter a with circumflex, U+00E2 ISOlat1 */
{TEXT("acute"), 180},		/* acute accent = spacing acute, U+00B4 ISOdia */
{TEXT("aelig"), 230},		/* latin small letter ae = latin small ligature ae, U+00E6 ISOlat1 */
{TEXT("agrave"), 224},	/* latin small letter a with grave = latin small letter a grave, U+00E0 ISOlat1 */
{TEXT("alefsym"), 8501},	/* alef symbol = first transfinite cardinal, U+2135 NEW */
{TEXT("alpha"), 945},		/* greek small letter alpha, U+03B1 ISOgrk3 */
{TEXT("amp"), 38},		/* ampersand, U+0026 ISOnum */
{TEXT("and"), 8743},		/* logical and = wedge, U+2227 ISOtech */
{TEXT("ang"), 8736},		/* angle, U+2220 ISOamso */
{TEXT("aring"), 229},		/* latin small letter a with ring above = latin small letter a ring, U+00E5 ISOlat1 */
{TEXT("asymp"), 8776},	/* almost equal to = asymptotic to, U+2248 ISOamsr */
{TEXT("atilde"), 227},	/* latin small letter a with tilde, U+00E3 ISOlat1 */
{TEXT("auml"), 228},		/* latin small letter a with diaeresis, U+00E4 ISOlat1 */
{TEXT("bdquo"), 8222},	/* double low-9 quotation mark, U+201E NEW */
{TEXT("beta"), 946},		/* greek small letter beta, U+03B2 ISOgrk3 */
{TEXT("brvbar"), 166},	/* broken bar = broken vertical bar, U+00A6 ISOnum */
{TEXT("bull"), 8226},		/* bullet = black small circle, U+2022 ISOpub */
{TEXT("cap"), 8745},		/* intersection = cap, U+2229 ISOtech */
{TEXT("ccedil"), 231},	/* latin small letter c with cedilla, U+00E7 ISOlat1 */
{TEXT("cedil"), 184},		/* cedilla = spacing cedilla, U+00B8 ISOdia */
{TEXT("cent"), 162},		/* cent sign, U+00A2 ISOnum */
{TEXT("chi"), 967},		/* greek small letter chi, U+03C7 ISOgrk3 */
{TEXT("circ"), 710},		/* modifier letter circumflex accent, U+02C6 ISOpub */
{TEXT("clubs"), 9827},	/* black club suit = shamrock, U+2663 ISOpub */
{TEXT("cong"), 8773},		/* approximately equal to, U+2245 ISOtech */
{TEXT("copy"), 169},		/* copyright sign, U+00A9 ISOnum */
{TEXT("crarr"), 8629},	/* downwards arrow with corner leftwards = carriage return, U+21B5 NEW */
{TEXT("cup"), 8746},		/* union = cup, U+222A ISOtech */
{TEXT("curren"), 164},	/* currency sign, U+00A4 ISOnum */
{TEXT("dArr"), 8659},		/* downwards double arrow, U+21D3 ISOamsa */
{TEXT("dagger"), 8224},	/* dagger, U+2020 ISOpub */
{TEXT("darr"), 8595},		/* downwards arrow, U+2193 ISOnum */
{TEXT("deg"), 176},		/* degree sign, U+00B0 ISOnum */
{TEXT("delta"), 948},		/* greek small letter delta, U+03B4 ISOgrk3 */
{TEXT("diams"), 9830},	/* black diamond suit, U+2666 ISOpub */
{TEXT("divide"), 247},	/* division sign, U+00F7 ISOnum */
{TEXT("eacute"), 233},	/* latin small letter e with acute, U+00E9 ISOlat1 */
{TEXT("ecirc"), 234},		/* latin small letter e with circumflex, U+00EA ISOlat1 */
{TEXT("egrave"), 232},	/* latin small letter e with grave, U+00E8 ISOlat1 */
{TEXT("empty"), 8709},	/* empty set = null set = diameter, U+2205 ISOamso */
{TEXT("emsp"), 8195},		/* em space, U+2003 ISOpub */
{TEXT("ensp"), 8194},		/* en space, U+2002 ISOpub */
{TEXT("epsilon"), 949},	/* greek small letter epsilon, U+03B5 ISOgrk3 */
{TEXT("equiv"), 8801},	/* identical to, U+2261 ISOtech */
{TEXT("eta"), 951},		/* greek small letter eta, U+03B7 ISOgrk3 */
{TEXT("eth"), 240},		/* latin small letter eth, U+00F0 ISOlat1 */
{TEXT("euml"), 235},		/* latin small letter e with diaeresis, U+00EB ISOlat1 */
{TEXT("euro"), 8364},		/* euro sign, U+20AC NEW */
{TEXT("exist"), 8707},	/* there exists, U+2203 ISOtech */
{TEXT("fnof"), 402},		/* latin small f with hook = function = florin, U+0192 ISOtech */
{TEXT("forall"), 8704},	/* for all, U+2200 ISOtech */
{TEXT("frac12"), 189},	/* vulgar fraction one half = fraction one half, U+00BD ISOnum */
{TEXT("frac14"), 188},	/* vulgar fraction one quarter = fraction one quarter, U+00BC ISOnum */
{TEXT("frac34"), 190},	/* vulgar fraction three quarters = fraction three quarters, U+00BE ISOnum */
{TEXT("frasl"), 8260},	/* fraction slash, U+2044 NEW */
{TEXT("gamma"), 947},		/* greek small letter gamma, U+03B3 ISOgrk3 */
{TEXT("ge"), 8805},		/* greater-than or equal to, U+2265 ISOtech */
{TEXT("gt"), 62},		/* greater-than sign, U+003E ISOnum */
{TEXT("hArr"), 8660},		/* left right double arrow, U+21D4 ISOamsa */
{TEXT("harr"), 8596},		/* left right arrow, U+2194 ISOamsa */
{TEXT("hearts"), 9829},	/* black heart suit = valentine, U+2665 ISOpub */
{TEXT("hellip"), 8230},	/* horizontal ellipsis = three dot leader, U+2026 ISOpub */
{TEXT("hyphen"), 173},	/* hyphen = discretionary hyphen, U+00AD ISOnum */
{TEXT("iacute"), 237},	/* latin small letter i with acute, U+00ED ISOlat1 */
{TEXT("icirc"), 238},		/* latin small letter i with circumflex, U+00EE ISOlat1 */
{TEXT("iexcl"), 161},		/* inverted exclamation mark, U+00A1 ISOnum */
{TEXT("igrave"), 236},	/* latin small letter i with grave, U+00EC ISOlat1 */
{TEXT("image"), 8465},	/* blackletter capital I = imaginary part, U+2111 ISOamso */
{TEXT("infin"), 8734},	/* infinity, U+221E ISOtech */
{TEXT("int"), 8747},		/* integral, U+222B ISOtech */
{TEXT("iota"), 953},		/* greek small letter iota, U+03B9 ISOgrk3 */
{TEXT("iquest"), 191},	/* inverted question mark = turned question mark, U+00BF ISOnum */
{TEXT("isin"), 8712},		/* element of, U+2208 ISOtech */
{TEXT("iuml"), 239},		/* latin small letter i with diaeresis, U+00EF ISOlat1 */
{TEXT("kappa"), 954},		/* greek small letter kappa, U+03BA ISOgrk3 */
{TEXT("lArr"), 8656},		/* leftwards double arrow, U+21D0 ISOtech */
{TEXT("lambda"), 955},	/* greek small letter lambda, U+03BB ISOgrk3 */
{TEXT("lang"), 9001},		/* left-pointing angle bracket = bra, U+2329 ISOtech */
{TEXT("laquo"), 171},		/* left-pointing double angle quotation mark = left pointing guillemet, U+00AB ISOnum */
{TEXT("larr"), 8592},		/* leftwards arrow, U+2190 ISOnum */
{TEXT("lceil"), 8968},	/* left ceiling = apl upstile, U+2308 ISOamsc */
{TEXT("ldquo"), 8220},	/* left double quotation mark, U+201C ISOnum */
{TEXT("le"), 8804},		/* less-than or equal to, U+2264 ISOtech */
{TEXT("lfloor"), 8970},	/* left floor = apl downstile, U+230A ISOamsc */
{TEXT("lowast"), 8727},	/* asterisk operator, U+2217 ISOtech */
{TEXT("loz"), 9674},		/* lozenge, U+25CA ISOpub */
{TEXT("lrm"), 8206},		/* left-to-right mark, U+200E NEW RFC 2070 */
{TEXT("lsaquo"), 8249},	/* single left-pointing angle quotation mark, U+2039 ISO proposed */
{TEXT("lsquo"), 8216},	/* left single quotation mark, U+2018 ISOnum */
{TEXT("lt"), 60},		/* less-than sign, U+003C ISOnum */
{TEXT("macr"), 175},		/* macron = spacing macron = overline = APL overbar, U+00AF ISOdia */
{TEXT("mdash"), 8212},	/* em dash, U+2014 ISOpub */
{TEXT("micro"), 181},		/* micro sign, U+00B5 ISOnum */
{TEXT("middot"), 183},	/* middle dot = Georgian comma = Greek middle dot, U+00B7 ISOnum */
{TEXT("minus"), 8722},	/* minus sign, U+2212 ISOtech */
{TEXT("mu"), 956},		/* greek small letter mu, U+03BC ISOgrk3 */
{TEXT("nabla"), 8711},	/* nabla = backward difference, U+2207 ISOtech */
{TEXT("nbsp"), 160},		/* no-break space = non-breaking space, U+00A0 ISOnum */
{TEXT("ndash"), 8211},	/* en dash, U+2013 ISOpub */
{TEXT("ne"), 8800},		/* not equal to, U+2260 ISOtech */
{TEXT("ni"), 8715},		/* contains as member, U+220B ISOtech */
{TEXT("not"), 172},		/* not sign, U+00AC ISOnum */
{TEXT("notin"), 8713},	/* not an element of, U+2209 ISOtech */
{TEXT("nsub"), 8836},		/* not a subset of, U+2284 ISOamsn */
{TEXT("ntilde"), 241},	/* latin small letter n with tilde, U+00F1 ISOlat1 */
{TEXT("nu"), 957},		/* greek small letter nu, U+03BD ISOgrk3 */
{TEXT("oacute"), 243},	/* latin small letter o with acute, U+00F3 ISOlat1 */
{TEXT("ocirc"), 244},		/* latin small letter o with circumflex, U+00F4 ISOlat1 */
{TEXT("oelig"), 339},		/* latin small ligature oe, U+0153 ISOlat2 */
{TEXT("ograve"), 242},	/* latin small letter o with grave, U+00F2 ISOlat1 */
{TEXT("oline"), 8254},	/* overline = spacing overscore, U+203E NEW */
{TEXT("omega"), 969},		/* greek small letter omega, U+03C9 ISOgrk3 */
{TEXT("omicron"), 959},	/* greek small letter omicron, U+03BF NEW */
{TEXT("oplus"), 8853},	/* circled plus = direct sum, U+2295 ISOamsb */
{TEXT("or"), 8744},		/* logical or = vee, U+2228 ISOtech */
{TEXT("ordf"), 170},		/* feminine ordinal indicator, U+00AA ISOnum */
{TEXT("ordm"), 186},		/* masculine ordinal indicator, U+00BA ISOnum */
{TEXT("oslash"), 248},	/* latin small letter o with stroke, = latin small letter o slash, U+00F8 ISOlat1 */
{TEXT("otilde"), 245},	/* latin small letter o with tilde, U+00F5 ISOlat1 */
{TEXT("otimes"), 8855},	/* circled times = vector product, U+2297 ISOamsb */
{TEXT("ouml"), 246},		/* latin small letter o with diaeresis, U+00F6 ISOlat1 */
{TEXT("para"), 182},		/* pilcrow sign = paragraph sign, U+00B6 ISOnum */
{TEXT("part"), 8706},		/* partial differential, U+2202 ISOtech */
{TEXT("permil"), 8240},	/* per mille sign, U+2030 ISOtech */
{TEXT("perp"), 8869},		/* up tack = orthogonal to = perpendicular, U+22A5 ISOtech */
{TEXT("phi"), 966},		/* greek small letter phi, U+03C6 ISOgrk3 */
{TEXT("pi"), 960},		/* greek small letter pi, U+03C0 ISOgrk3 */
{TEXT("piv"), 982},		/* greek pi symbol, U+03D6 ISOgrk3 */
{TEXT("plusmn"), 177},	/* plus-minus sign = plus-or-minus sign, U+00B1 ISOnum */
{TEXT("pound"), 163},		/* pound sign, U+00A3 ISOnum */
{TEXT("prime"), 8242},	/* prime = minutes = feet, U+2032 ISOtech */
{TEXT("prod"), 8719},		/* n-ary product = product sign, U+220F ISOamsb */
{TEXT("prop"), 8733},		/* proportional to, U+221D ISOtech */
{TEXT("psi"), 968},		/* greek small letter psi, U+03C8 ISOgrk3 */
{TEXT("quot"), 34},		/* quotation mark = APL quote, U+0022 ISOnum */
{TEXT("rArr"), 8658},		/* rightwards double arrow, U+21D2 ISOtech */
{TEXT("radic"), 8730},	/* square root = radical sign, U+221A ISOtech */
{TEXT("rang"), 9002},		/* right-pointing angle bracket = ket, U+232A ISOtech */
{TEXT("raquo"), 187},		/* right-pointing double angle quotation mark = right pointing guillemet, U+00BB ISOnum */
{TEXT("rarr"), 8594},		/* rightwards arrow, U+2192 ISOnum */
{TEXT("rceil"), 8969},	/* right ceiling, U+2309 ISOamsc */
{TEXT("rdquo"), 8221},	/* right double quotation mark, U+201D ISOnum */
{TEXT("real"), 8476},		/* blackletter capital R = real part symbol, U+211C ISOamso */
{TEXT("reg"), 174},		/* registered sign = registered trade mark sign, U+00AE ISOnum */
{TEXT("rfloor"), 8971},	/* right floor, U+230B ISOamsc */
{TEXT("rho"), 961},		/* greek small letter rho, U+03C1 ISOgrk3 */
{TEXT("rlm"), 8207},		/* right-to-left mark, U+200F NEW RFC 2070 */
{TEXT("rsaquo"), 8250},	/* single right-pointing angle quotation mark, U+203A ISO proposed */
{TEXT("rsquo"), 8217},	/* right single quotation mark, U+2019 ISOnum */
{TEXT("sbquo"), 8218},	/* single low-9 quotation mark, U+201A NEW */
{TEXT("scaron"), 353},	/* latin small letter s with caron, U+0161 ISOlat2 */
{TEXT("sdot"), 8901},		/* dot operator, U+22C5 ISOamsb */
{TEXT("sect"), 167},		/* section sign, U+00A7 ISOnum */
{TEXT("shy"), 173},		/* soft hyphen = discretionary hyphen, U+00AD ISOnum */
{TEXT("sigma"), 963},		/* greek small letter sigma, U+03C3 ISOgrk3 */
{TEXT("sigmaf"), 962},	/* greek small letter final sigma, U+03C2 ISOgrk3 */
{TEXT("sim"), 8764},		/* tilde operator = varies with = similar to, U+223C ISOtech */
{TEXT("spades"), 9824},	/* black spade suit, U+2660 ISOpub */
{TEXT("sub"), 8834},		/* subset of, U+2282 ISOtech */
{TEXT("sube"), 8838},		/* subset of or equal to, U+2286 ISOtech */
{TEXT("sum"), 8721},		/* n-ary sumation, U+2211 ISOamsb */
{TEXT("sup"), 8835},		/* superset of, U+2283 ISOtech */
{TEXT("sup1"), 185},		/* superscript one = superscript digit one, U+00B9 ISOnum */
{TEXT("sup2"), 178},		/* superscript two = superscript digit two = squared, U+00B2 ISOnum */
{TEXT("sup3"), 179},		/* superscript three = superscript digit three = cubed, U+00B3 ISOnum */
{TEXT("supe"), 8839},		/* superset of or equal to, U+2287 ISOtech */
{TEXT("szlig"), 223},		/* latin small letter sharp s = ess-zed, U+00DF ISOlat1 */
{TEXT("tau"), 964},		/* greek small letter tau, U+03C4 ISOgrk3 */
{TEXT("there4"), 8756},	/* therefore, U+2234 ISOtech */
{TEXT("theta"), 952},		/* greek small letter theta, U+03B8 ISOgrk3 */
{TEXT("thetasym"), 977},	/* greek small letter theta symbol, U+03D1 NEW */
{TEXT("thinsp"), 8201},	/* thin space, U+2009 ISOpub */
{TEXT("thorn"), 254},		/* latin small letter thorn with, U+00FE ISOlat1 */
{TEXT("tilde"), 732},		/* small tilde, U+02DC ISOdia */
{TEXT("times"), 215},		/* multiplication sign, U+00D7 ISOnum */
{TEXT("trade"), 8482},	/* trade mark sign, U+2122 ISOnum */
{TEXT("uArr"), 8657},		/* upwards double arrow, U+21D1 ISOamsa */
{TEXT("uacute"), 250},	/* latin small letter u with acute, U+00FA ISOlat1 */
{TEXT("uarr"), 8593},		/* upwards arrow, U+2191 ISOnum*/
{TEXT("ucirc"), 251},		/* latin small letter u with circumflex, U+00FB ISOlat1 */
{TEXT("ugrave"), 249},	/* latin small letter u with grave, U+00F9 ISOlat1 */
{TEXT("uml"), 168},		/* diaeresis = spacing diaeresis, U+00A8 ISOdia */
{TEXT("upsih"), 978},		/* greek upsilon with hook symbol, U+03D2 NEW */
{TEXT("upsilon"), 965},	/* greek small letter upsilon, U+03C5 ISOgrk3 */
{TEXT("uuml"), 252},		/* latin small letter u with diaeresis, U+00FC ISOlat1 */
{TEXT("weierp"), 8472},	/* script capital P = power set = Weierstrass p, U+2118 ISOamso */
{TEXT("xi"), 958},		/* greek small letter xi, U+03BE ISOgrk3 */
{TEXT("yacute"), 253},	/* latin small letter y with acute, U+00FD ISOlat1 */
{TEXT("yen"), 165},		/* yen sign = yuan sign, U+00A5 ISOnum */
{TEXT("yuml"), 255},		/* latin small letter y with diaeresis, U+00FF ISOlat1 */
{TEXT("zeta"), 950},		/* greek small letter zeta, U+03B6 ISOgrk3 */
{TEXT("zwj"), 8205},		/* zero width joiner, U+200D NEW RFC 2070 */
{TEXT("zwnj"), 8204},		/* zero width non-joiner, U+200C NEW RFC 2070 */

{TEXT("zzzz"), 0}			/* this last entry is required */
};

typedef struct _ElemToBeChecked *PtrElemToBeChecked;
typedef struct _ElemToBeChecked
  {
     Element               Elem;	/* the element to be checked */
     PtrElemToBeChecked    nextElemToBeChecked;
  }
ElemToBeChecked;


/* elements that cannot contain text as immediate children.
   When some text is present in the HTML file it must be surrounded
   by a Thot Paragraph (or Pseudo_paragraph) element */
static int          NoTextChild[] =
{
   HTML_EL_HTML, HTML_EL_HEAD, HTML_EL_BODY,
   HTML_EL_Definition_List, HTML_EL_Block_Quote, HTML_EL_Directory,
   HTML_EL_Form, HTML_EL_Menu, HTML_EL_FIELDSET,
   HTML_EL_Numbered_List, HTML_EL_Option_Menu,
   HTML_EL_Unnumbered_List, HTML_EL_Definition, HTML_EL_List_Item,
   HTML_EL_MAP, HTML_EL_Applet,
   HTML_EL_Object, HTML_EL_IFRAME, HTML_EL_NOFRAMES,
   HTML_EL_Division, HTML_EL_Center, HTML_EL_NOSCRIPT,
   HTML_EL_Data_cell, HTML_EL_Heading_cell,
#ifdef GRAPHML
   HTML_EL_XMLGraphics,
#endif
   0};

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
   HTML_EL_TEXT_UNIT, HTML_EL_PICTURE_UNIT,
   HTML_EL_Anchor,
   HTML_EL_Teletype_text, HTML_EL_Italic_text, HTML_EL_Bold_text,
   HTML_EL_Underlined_text, HTML_EL_Struck_text, HTML_EL_Big_text,
   HTML_EL_Small_text,
   HTML_EL_Emphasis, HTML_EL_Strong, HTML_EL_Def, HTML_EL_Code, HTML_EL_Sample,
   HTML_EL_Keyboard, HTML_EL_Variable, HTML_EL_Cite, HTML_EL_ABBR,
   HTML_EL_ACRONYM,
   HTML_EL_Font_, HTML_EL_Quotation, HTML_EL_Subscript, HTML_EL_Superscript,
   HTML_EL_Span, HTML_EL_BDO, HTML_EL_INS, HTML_EL_DEL,
   HTML_EL_Math,
   HTML_EL_Input,
   HTML_EL_Option, HTML_EL_OptGroup, HTML_EL_Option_Menu,
   HTML_EL_Text_Input, HTML_EL_Password_Input, HTML_EL_File_Input,
   HTML_EL_Text_With_Frame, HTML_EL_Inserted_Text, HTML_EL_Text_Area,
   HTML_EL_Button_Input, HTML_EL_BUTTON,
   HTML_EL_LABEL,
   HTML_EL_BR,
   0};

/* block level elements */
static int          BlockLevelElement[] =
{
   HTML_EL_Paragraph, HTML_EL_Pseudo_paragraph,
   HTML_EL_H1, HTML_EL_H2, HTML_EL_H3, HTML_EL_H4, HTML_EL_H5, HTML_EL_H6,
   HTML_EL_TITLE, HTML_EL_Term, HTML_EL_CAPTION, HTML_EL_LEGEND,
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


/* mapping table of HTML attribute values */

static AttrValueMapping HTMLAttrValueMappingTable[] =
{
   {HTML_ATTR_dir, TEXT("ltr"), HTML_ATTR_dir_VAL_ltr},
   {HTML_ATTR_dir, TEXT("rtl"), HTML_ATTR_dir_VAL_rtl},

   {HTML_ATTR_TextAlign, TEXT("left"), HTML_ATTR_TextAlign_VAL_left_},
   {HTML_ATTR_TextAlign, TEXT("center"), HTML_ATTR_TextAlign_VAL_center_},
   {HTML_ATTR_TextAlign, TEXT("right"), HTML_ATTR_TextAlign_VAL_right_},
   {HTML_ATTR_TextAlign, TEXT("justify"), HTML_ATTR_TextAlign_VAL_justify_},

   {HTML_ATTR_Align, TEXT("left"), HTML_ATTR_Align_VAL_left_},
   {HTML_ATTR_Align, TEXT("center"), HTML_ATTR_Align_VAL_center_},
   {HTML_ATTR_Align, TEXT("right"), HTML_ATTR_Align_VAL_right_},

   {HTML_ATTR_LAlign, TEXT("top"), HTML_ATTR_LAlign_VAL_Top_},
   {HTML_ATTR_LAlign, TEXT("bottom"), HTML_ATTR_LAlign_VAL_Bottom_},
   {HTML_ATTR_LAlign, TEXT("left"), HTML_ATTR_LAlign_VAL_Left_},
   {HTML_ATTR_LAlign, TEXT("right"), HTML_ATTR_LAlign_VAL_Right_},

   {HTML_ATTR_Clear, TEXT("left"), HTML_ATTR_Clear_VAL_Left_},
   {HTML_ATTR_Clear, TEXT("right"), HTML_ATTR_Clear_VAL_Right_},
   {HTML_ATTR_Clear, TEXT("all"), HTML_ATTR_Clear_VAL_All_},
   {HTML_ATTR_Clear, TEXT("none"), HTML_ATTR_Clear_VAL_None_},

   {HTML_ATTR_NumberStyle, TEXT("1"), HTML_ATTR_NumberStyle_VAL_Arabic_},
   {HTML_ATTR_NumberStyle, TEXT("a"), HTML_ATTR_NumberStyle_VAL_LowerAlpha},
   {HTML_ATTR_NumberStyle, TEXT("A"), HTML_ATTR_NumberStyle_VAL_UpperAlpha},
   {HTML_ATTR_NumberStyle, TEXT("i"), HTML_ATTR_NumberStyle_VAL_LowerRoman},
   {HTML_ATTR_NumberStyle, TEXT("I"), HTML_ATTR_NumberStyle_VAL_UpperRoman},

   {HTML_ATTR_BulletStyle, TEXT("disc"), HTML_ATTR_BulletStyle_VAL_disc},
   {HTML_ATTR_BulletStyle, TEXT("square"), HTML_ATTR_BulletStyle_VAL_square},
   {HTML_ATTR_BulletStyle, TEXT("circle"), HTML_ATTR_BulletStyle_VAL_circle},

   {HTML_ATTR_ItemStyle, TEXT("1"), HTML_ATTR_ItemStyle_VAL_Arabic_},
   {HTML_ATTR_ItemStyle, TEXT("a"), HTML_ATTR_ItemStyle_VAL_LowerAlpha},
   {HTML_ATTR_ItemStyle, TEXT("A"), HTML_ATTR_ItemStyle_VAL_UpperAlpha},
   {HTML_ATTR_ItemStyle, TEXT("i"), HTML_ATTR_ItemStyle_VAL_LowerRoman},
   {HTML_ATTR_ItemStyle, TEXT("I"), HTML_ATTR_ItemStyle_VAL_UpperRoman},
   {HTML_ATTR_ItemStyle, TEXT("disc"), HTML_ATTR_ItemStyle_VAL_disc},
   {HTML_ATTR_ItemStyle, TEXT("square"), HTML_ATTR_ItemStyle_VAL_square},
   {HTML_ATTR_ItemStyle, TEXT("circle"), HTML_ATTR_ItemStyle_VAL_circle},

   {HTML_ATTR_Button_type, TEXT("button"), HTML_ATTR_Button_type_VAL_button},
   {HTML_ATTR_Button_type, TEXT("submit"), HTML_ATTR_Button_type_VAL_submit},
   {HTML_ATTR_Button_type, TEXT("reset"), HTML_ATTR_Button_type_VAL_reset},

   {HTML_ATTR_mode, TEXT("display"), HTML_ATTR_mode_VAL_display},
   {HTML_ATTR_mode, TEXT("inline"), HTML_ATTR_mode_VAL_inline_math},

   {HTML_ATTR_frame, TEXT("void"), HTML_ATTR_frame_VAL_void},
   {HTML_ATTR_frame, TEXT("above"), HTML_ATTR_frame_VAL_above},
   {HTML_ATTR_frame, TEXT("below"), HTML_ATTR_frame_VAL_below},
   {HTML_ATTR_frame, TEXT("hsides"), HTML_ATTR_frame_VAL_hsides},
   {HTML_ATTR_frame, TEXT("lhs"), HTML_ATTR_frame_VAL_lhs},
   {HTML_ATTR_frame, TEXT("rhs"), HTML_ATTR_frame_VAL_rhs},
   {HTML_ATTR_frame, TEXT("vsides"), HTML_ATTR_frame_VAL_vsides},
   {HTML_ATTR_frame, TEXT("box"), HTML_ATTR_frame_VAL_box},
   {HTML_ATTR_frame, TEXT("border"), HTML_ATTR_frame_VAL_border},

   {HTML_ATTR_frameborder, TEXT("0"), HTML_ATTR_frameborder_VAL_Border0},
   {HTML_ATTR_frameborder, TEXT("1"), HTML_ATTR_frameborder_VAL_Border1},

   {HTML_ATTR_scrolling, TEXT("yes"), HTML_ATTR_scrolling_VAL_Yes_},
   {HTML_ATTR_scrolling, TEXT("no"), HTML_ATTR_scrolling_VAL_No_},
   {HTML_ATTR_scrolling, TEXT("auto"), HTML_ATTR_scrolling_VAL_auto_},

   {HTML_ATTR_rules_, TEXT("none"), HTML_ATTR_rules__VAL_none_},
   {HTML_ATTR_rules_, TEXT("groups"), HTML_ATTR_rules__VAL_groups},
   {HTML_ATTR_rules_, TEXT("rows"), HTML_ATTR_rules__VAL_rows},
   {HTML_ATTR_rules_, TEXT("cols"), HTML_ATTR_rules__VAL_cols},
   {HTML_ATTR_rules_, TEXT("all"), HTML_ATTR_rules__VAL_all},

   {HTML_ATTR_Cell_align, TEXT("left"), HTML_ATTR_Cell_align_VAL_Cell_left},
   {HTML_ATTR_Cell_align, TEXT("center"), HTML_ATTR_Cell_align_VAL_Cell_center},
   {HTML_ATTR_Cell_align, TEXT("right"), HTML_ATTR_Cell_align_VAL_Cell_right},
   {HTML_ATTR_Cell_align, TEXT("justify"), HTML_ATTR_Cell_align_VAL_Cell_justify},
   {HTML_ATTR_Cell_align, TEXT("char"), HTML_ATTR_Cell_align_VAL_Cell_char},

   {HTML_ATTR_Alignment, TEXT("top"), HTML_ATTR_Alignment_VAL_Top_},
   {HTML_ATTR_Alignment, TEXT("middle"), HTML_ATTR_Alignment_VAL_Middle_},
   {HTML_ATTR_Alignment, TEXT("bottom"), HTML_ATTR_Alignment_VAL_Bottom_},
   {HTML_ATTR_Alignment, TEXT("left"), HTML_ATTR_Alignment_VAL_Left_},
   {HTML_ATTR_Alignment, TEXT("right"), HTML_ATTR_Alignment_VAL_Right_},

   {HTML_ATTR_METHOD, TEXT("get"), HTML_ATTR_METHOD_VAL_Get_},
   {HTML_ATTR_METHOD, TEXT("post"), HTML_ATTR_METHOD_VAL_Post_},

   {HTML_ATTR_Position, TEXT("top"), HTML_ATTR_Position_VAL_Position_top},
   {HTML_ATTR_Position, TEXT("bottom"), HTML_ATTR_Position_VAL_Position_bottom},
   {HTML_ATTR_Position, TEXT("left"), HTML_ATTR_Position_VAL_Position_left},
   {HTML_ATTR_Position, TEXT("right"), HTML_ATTR_Position_VAL_Position_right},

   {HTML_ATTR_Row_valign, TEXT("top"), HTML_ATTR_Row_valign_VAL_Row_top},
   {HTML_ATTR_Row_valign, TEXT("middle"), HTML_ATTR_Row_valign_VAL_Row_middle},
   {HTML_ATTR_Row_valign, TEXT("bottom"), HTML_ATTR_Row_valign_VAL_Row_bottom},
   {HTML_ATTR_Row_valign, TEXT("baseline"), HTML_ATTR_Row_valign_VAL_Row_baseline},

   {HTML_ATTR_Cell_valign, TEXT("top"), HTML_ATTR_Cell_valign_VAL_Cell_top},
   {HTML_ATTR_Cell_valign, TEXT("middle"), HTML_ATTR_Cell_valign_VAL_Cell_middle},
   {HTML_ATTR_Cell_valign, TEXT("bottom"), HTML_ATTR_Cell_valign_VAL_Cell_bottom},
   {HTML_ATTR_Cell_valign, TEXT("baseline"), HTML_ATTR_Cell_valign_VAL_Cell_baseline},

   {HTML_ATTR_shape, TEXT("rect"), HTML_ATTR_shape_VAL_rectangle},
   {HTML_ATTR_shape, TEXT("circle"), HTML_ATTR_shape_VAL_circle},
   {HTML_ATTR_shape, TEXT("poly"), HTML_ATTR_shape_VAL_polygon},

   {HTML_ATTR_valuetype, TEXT("data"), HTML_ATTR_valuetype_VAL_data_},
   {HTML_ATTR_valuetype, TEXT("ref"), HTML_ATTR_valuetype_VAL_ref},
   {HTML_ATTR_valuetype, TEXT("object"), HTML_ATTR_valuetype_VAL_object_},

/* HTML attribute TYPE generates a Thot element */
   {DummyAttribute, TEXT("button"), HTML_EL_Button_Input},
   {DummyAttribute, TEXT("checkbox"), HTML_EL_Checkbox_Input},
   {DummyAttribute, TEXT("file"), HTML_EL_File_Input},
   {DummyAttribute, TEXT("hidden"), HTML_EL_Hidden_Input},
   {DummyAttribute, TEXT("image"), HTML_EL_PICTURE_UNIT},
   {DummyAttribute, TEXT("password"), HTML_EL_Password_Input},
   {DummyAttribute, TEXT("radio"), HTML_EL_Radio_Input},
   {DummyAttribute, TEXT("reset"), HTML_EL_Reset_Input},
   {DummyAttribute, TEXT("submit"), HTML_EL_Submit_Input},
   {DummyAttribute, TEXT("text"), HTML_EL_Text_Input},

/* The following declarations allow the parser to accept boolean attributes */
/* written "checked=CHECKED"), for instance */
   {HTML_ATTR_ISMAP, TEXT("ismap"), HTML_ATTR_ISMAP_VAL_Yes_},
   {HTML_ATTR_nohref, TEXT("nohref"), HTML_ATTR_nohref_VAL_Yes_},
   {HTML_ATTR_COMPACT, TEXT("compact"), HTML_ATTR_COMPACT_VAL_Yes_},
   {HTML_ATTR_Multiple, TEXT("multiple"), HTML_ATTR_Multiple_VAL_Yes_},
   {HTML_ATTR_Selected, TEXT("selected"), HTML_ATTR_Selected_VAL_Yes_},
   {HTML_ATTR_Checked, TEXT("checked"), HTML_ATTR_Checked_VAL_Yes_},
   {HTML_ATTR_No_wrap, TEXT("nowrap"), HTML_ATTR_No_wrap_VAL_no_wrap},
   {HTML_ATTR_NoShade, TEXT("noshade"), HTML_ATTR_NoShade_VAL_NoShade_},
   {HTML_ATTR_declare, TEXT("declare"), HTML_ATTR_declare_VAL_Yes_},
   {HTML_ATTR_defer, TEXT("defer"), HTML_ATTR_defer_VAL_Yes_},
   {HTML_ATTR_disabled, TEXT("disabled"), HTML_ATTR_disabled_VAL_Yes_},
   {HTML_ATTR_readonly, TEXT("readonly"), HTML_ATTR_readonly_VAL_Yes_},
   {HTML_ATTR_no_resize, TEXT("noresize"), HTML_ATTR_no_resize_VAL_Yes_},
   {0, TEXT(""), 0}			/* Last entry. Mandatory */
};


/* ---------------------- static variables ---------------------- */

#ifdef EXPAT
/* Expat perser identifier */
static XML_Parser  parser;
#endif /* EXPAT */

/* parser stack */
	        /* maximum stack height */
#define MaxStack   200
                /* entry of GIMappingTable */
static int         GINumberStack[MaxStack];
                /* element in the Thot abstract tree */
static Element     ElementStack[MaxStack];
                /* level of element in the Thot tree */
static int         ThotLevel[MaxStack];
                /* element language */
static Language	   LanguageStack[MaxStack];
                /* first free element on the stack */
static int         StackLevel = 0;

/* information about the input file */
#define INPUT_FILE_BUFFER_SIZE 2000
static char         FileBuffer[INPUT_FILE_BUFFER_SIZE+1];
#ifdef _I18N_
static char         FileBufferA[INPUT_FILE_BUFFER_SIZE+1];
#endif

                /* last char. in the buffer */
static int	   LastCharInFileBuffer = 0;
                /* current character read */
static int         CurrentBufChar;
                /* previous character read */
static CHAR_T	   PreviousBufChar = EOS;
static char*       InputText;
static gzFile      stream = 0;
                /* number of lines read in the file */
static int         NumberOfLinesRead = 0;
                /* number of characters read in the current line */
static int         NumberOfCharRead = 0;
                /* no printable character encountered yet 
		   in the current line */
static ThotBool    EmptyLine = TRUE;
                /* no printable character encountered yet
		   in the file */
static ThotBool    StartOfFile = TRUE;
                /* <PRE> has just been read */
static ThotBool    AfterTagPRE = FALSE;
                /* reading the content of a STYLE element */
static ThotBool    ParsingCSS = FALSE;
                /* reading the content of a text area element */
static ThotBool    ParsingTextArea = FALSE;
                /* <TABLE> has been read */
static int         WithinTable = 0;
                /* path or URL of the document */
static CHAR_T*     docURL = NULL;

/* input buffer */
#define MaxBufferLength   1000
#define AllmostFullBuffer  700
static  UCHAR_T    inputBuffer[MaxBufferLength];
                /* actual length of text in input buffer */
static int         LgBuffer = 0;
                /* line number in the source file of the beginning
		   of the text contained in the buffer */
static int	   BufferLineNumber = 0;

/* information about the Thot document under construction */
                /* the Thot document */
static Document    theDocument = 0;
                /* language used in the document */
static Language    currentLanguage;
                /* the HTML structure schema */
static SSchema     DocumentSSchema = NULL;
                /* root element of the document */
static Element     rootElement;
                /* last element created */
static Element     lastElement = NULL;
                /* last element is complete */
static ThotBool    lastElementClosed = FALSE;
                /* index in the GIMappingTable of the
		   element being created */
static int         lastElemEntry = 0;
                /* last attribute created */
static Attribute   lastAttribute = NULL;
                /* element with which the last
		   attribute has been associated */
static Attribute   lastAttrElement = NULL;
                /* entry in the AttributeMappingTable
		   of the attribute being created */
static AttributeMapping* lastAttrEntry = NULL;
                /* the last attribute encountered is invalid */
static ThotBool    UnknownAttr = FALSE;
static ThotBool    ReadingAnAttrValue = FALSE;
                /* TEXT element of the current Comment element */
static Element     CommentText = NULL;
                /* the last start tag encountered is invalid */
static ThotBool    UnknownTag = FALSE;
                /* character data should be catenated
		   with the last Text element */
static ThotBool    MergeText = FALSE;
static ThotBool    HTMLrootClosed = FALSE;
static CHAR_T*     HTMLrootClosingTag = NULL;
static PtrElemToBeChecked FirstElemToBeChecked = NULL;
static PtrElemToBeChecked LastElemToBeChecked = NULL;

/* information about an entity being read */
                /* maximum size entity */
#define MaxEntityLength 50
                /* name of entity being read */
static CHAR_T      EntityName[MaxEntityLength];
                /* length of entity name read so far */
static int         LgEntityName = 0;
                /* entry of the entity table that
		   matches the entity read so far */
static int         EntityTableEntry = 0;
                /* rank of the last matching
		   character in that entry */
static int         CharRank = 0;

                /* maximum size of error messages */
#define MaxMsgLength 200

#ifdef __STDC__
static void         ProcessStartGI (CHAR_T* GIname);
#else
static void         ProcessStartGI ();
#endif

static FILE*    ErrFile = (FILE*) 0;
static CHAR_T   ErrFileName [80];

extern CHARSET  CharEncoding;
extern ThotBool charset_undefined;

/*----------------------------------------------------------------------
  ParseCharset: Parses the element HTTP-EQUIV and looks for the charset 
  value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void ParseCharset (Element el) 
#else  /* !__STDC__ */
static void ParseCharset (el) 
Element el;
#endif /* !__STDC__ */
{
 
   int length;
   CHAR_T *text, *text2, *ptrText, *str;
   CHAR_T  charsetname[MAX_LENGTH];
   int     pos, index = 0;
   AttributeType attrType;
   Attribute attr;
   Element root;

   if (!charset_undefined)
 	  return;

  attrType.AttrSSchema = DocumentSSchema;
  attrType.AttrTypeNum = HTML_ATTR_http_equiv;
  attr = TtaGetAttribute (el, attrType);
  if (attr != NULL) {
     /* There is a HTTP-EQUIV attribute */
     length = TtaGetTextAttributeLength (attr);
     if (length > 0) {
        text = TtaAllocString (length + 1);
        TtaGiveTextAttributeValue (attr, text, &length);
        if (!ustrcasecmp (text, TEXT("content-type"))) {
           attrType.AttrTypeNum = HTML_ATTR_meta_content;
           attr = TtaGetAttribute (el, attrType);
           if (attr != NULL) {
              length = TtaGetTextAttributeLength (attr);
              if (length > 0) {
                 text2 = TtaAllocString (length + 1);
                 TtaGiveTextAttributeValue (attr, text2, &length);
                 ptrText = text2;
                 while (*ptrText) {
                       *ptrText = utolower (*ptrText);
                       ptrText++;
				 }

                 str = ustrstr (text2, TEXT("charset="));
    
                 if (str) {
 			       pos = str - text2 + 8;

                   while (text2[pos] != WC_SPACE && text2[pos] != WC_TAB && text2[pos] != WC_EOS)
                         charsetname[index++] = text2[pos++];
                   charsetname[index] = WC_EOS;
                   CharEncoding = TtaGetCharset (charsetname);

                   if (CharEncoding == UNDEFINED_CHARSET)
                      CharEncoding = UTF_8;
                   else {
                        /* copy the charset to the document's metadata info */
                        root = TtaGetMainRoot (theDocument);
                        attrType.AttrTypeNum = HTML_ATTR_Charset;
                        attr = TtaGetAttribute (root, attrType);
                        if (!attr)
                           /* the root element does not have a Charset attribute.
                           Create one */
						{
                          attr = TtaNewAttribute (attrType);
                          TtaAttachAttribute (root, attr, theDocument);
						}
                        TtaSetAttributeText (attr, charsetname, root, theDocument);
				   }
                   charset_undefined = FALSE;
				 }
                 TtaFreeMemory (text2);
			  }       
		   } 
		}
        TtaFreeMemory (text);
	 }
  }
}


/*----------------------------------------------------------------------
   copyCEstring    create a copy of the string of elements pointed
   by first and return a pointer on the first
   element of the copy.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrClosedElement copyCEstring (PtrClosedElement first)
#else
static PtrClosedElement copyCEstring (first)
PtrClosedElement    first;

#endif
{
   PtrClosedElement    ret, cur, next, prev;

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
   Within  checks if an element of type ThotType is in the stack.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     Within (int ThotType, SSchema ThotSSchema)
#else
static ThotBool     Within (ThotType, ThotSSchema)
int                 ThotType;
SSchema		    ThotSSchema;

#endif
{
   ThotBool            ret;
   int                 i;
   ElementType         elType;

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
   CloseBuffer     close the input buffer.
  ----------------------------------------------------------------------*/
static void         CloseBuffer ()
{
   inputBuffer[LgBuffer] = WC_EOS;
}

/*----------------------------------------------------------------------
   InitBuffer      initialize the input buffer.
  ----------------------------------------------------------------------*/
static void         InitBuffer ()
{
   LgBuffer = 0;
}

#ifdef __STDC__
static ThotBool     InsertElement (Element * el);

#else
static ThotBool     InsertElement ();

#endif

/*----------------------------------------------------------------------
   InsertSibling   return TRUE if the new element must be inserted
   in the Thot document as a sibling of lastElement;
   return FALSE it it must be inserted as a child.
  ----------------------------------------------------------------------*/
static ThotBool     InsertSibling ()
{
   if (StackLevel == 0)
      return FALSE;
   else if (lastElementClosed ||
	    TtaIsLeaf (TtaGetElementType (lastElement)) ||
	    (GINumberStack[StackLevel - 1] >= 0 &&
	     pHTMLGIMapping[GINumberStack[StackLevel - 1]].htmlContents == 'E'))
      return TRUE;
   else
      return FALSE;
}

/*----------------------------------------------------------------------
   IsEmptyElement return TRUE if element el is defined as an empty element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool        IsEmptyElement (Element el)
#else
static ThotBool        IsEmptyElement (el)
Element             el;

#endif
{
   ElementType         elType;
   int                 i;
   ThotBool            ret;

   ret = FALSE;
   elType = TtaGetElementType (el);
   i = 0;
   while (EmptyElement[i] > 0 && EmptyElement[i] != elType.ElTypeNum)
      i++;
   if (EmptyElement[i] == elType.ElTypeNum)
      ret = TRUE;
   return ret;
}

/*----------------------------------------------------------------------
   IsBlockElement  return TRUE if element el is a block element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     IsBlockElement (Element el)
#else
static ThotBool     IsBlockElement (el)
Element             el;

#endif
{
   ElementType         elType;
   int                 i;
   ThotBool            ret;

   ret = FALSE;
   elType = TtaGetElementType (el);
   i = 0;
   while (BlockLevelElement[i] > 0 &&
	  BlockLevelElement[i] != elType.ElTypeNum)
      i++;
   if (BlockLevelElement[i] == elType.ElTypeNum)
      ret = TRUE;
   return ret;
}

/*----------------------------------------------------------------------
   CannotContainText return TRUE if element el is a block element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     CannotContainText (ElementType elType)
#else
static ThotBool     CannotContainText (elType)
ElementType         elType;

#endif
{
   int                 i;
   ThotBool            ret;

   if (ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")))
      /* not an HTML element */
      ret = TRUE;
   else
      {
      ret = FALSE;
      i = 0;
      while (NoTextChild[i] > 0 && NoTextChild[i] != elType.ElTypeNum)
         i++;
      if (NoTextChild[i] == elType.ElTypeNum)
         ret = TRUE;
      }
   return ret;
}

/*----------------------------------------------------------------------
   TextToDocument  Put the content of input buffer in the document.
  ----------------------------------------------------------------------*/
static void         TextToDocument ()
{
   ElementType         elType;
   Element             elText, parent, ancestor, prev;
   int                 i;
   ThotBool            ignoreLeadingSpaces;

   CloseBuffer ();
   if (lastElement != NULL)
     {
	i = 0;
	if (InsertSibling ())
	   /* There is a previous sibling (lastElement) for the new Text
	      element */
	  {
	     parent = TtaGetParent (lastElement);
	     if (parent == NULL)
		parent = lastElement;
	     elType = TtaGetElementType (parent);
	     if (IsCharacterLevelElement (lastElement) &&
		 elType.ElTypeNum != HTML_EL_Option_Menu &&
		 elType.ElTypeNum != HTML_EL_OptGroup)
		{
	        ignoreLeadingSpaces = FALSE;
	        elType = TtaGetElementType (lastElement);
	        if (elType.ElTypeNum == HTML_EL_BR)
		   ignoreLeadingSpaces = TRUE;
		}
	     else
	        ignoreLeadingSpaces = TRUE;
	  }
	else
	   /* the new Text element should be the first child of the latest
	      element encountered */
	  {
	     parent = lastElement;
	     ignoreLeadingSpaces = TRUE;
	     elType = TtaGetElementType (lastElement);
	     if (elType.ElTypeNum != HTML_EL_Option_Menu &&
		 elType.ElTypeNum != HTML_EL_OptGroup)
		{
	        ancestor = parent;
	        while (ignoreLeadingSpaces && IsCharacterLevelElement (ancestor))
		   {
		   prev = ancestor;
		   TtaPreviousSibling (&prev);
		   if (prev == NULL)
		      ancestor = TtaGetParent (ancestor);
		   else
		      ignoreLeadingSpaces = FALSE;
		   }
		}
	  }
	if (ignoreLeadingSpaces)
	   if (!Within (HTML_EL_Preformatted, DocumentSSchema) &&
	       !Within (HTML_EL_STYLE_, DocumentSSchema) &&
	       !Within (HTML_EL_SCRIPT, DocumentSSchema))
	      /* suppress leading spaces */
	      while (inputBuffer[i] <= WC_SPACE && inputBuffer[i] != WC_EOS)
		 i++;
	if (inputBuffer[i] != WC_EOS)
	  {
	     elType = TtaGetElementType (lastElement);
	     if (elType.ElTypeNum == HTML_EL_TEXT_UNIT && MergeText)
		TtaAppendTextContent (lastElement, &(inputBuffer[i]), theDocument);
	     else
	       {
		  /* create a TEXT element */
		  elType.ElSSchema = DocumentSSchema;
		  elType.ElTypeNum = HTML_EL_TEXT_UNIT;
		  elText = TtaNewElement (theDocument, elType);
		  TtaSetElementLineNumber (elText, BufferLineNumber);
		  InsertElement (&elText);
		  lastElementClosed = TRUE;
		  MergeText = TRUE;
		  /* put the content of the input buffer into the TEXT element */
		  if (elText != NULL)
		     TtaSetTextContent (elText, &(inputBuffer[i]), currentLanguage,
					theDocument);
	       }
	  }
     }
   InitBuffer ();
}


/*----------------------------------------------------------------------
   PutInBuffer     put character c in the input buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutInBuffer (UCHAR_T c)
#else
static void         PutInBuffer (c)
UCHAR_T             c;
#endif
{
  int                 len;

  /* put the character into the buffer if it is not an ignored char. */
  if ((int) c == WC_TAB)		/* HT */
    len = 8;			/* HT = 8 spaces */
  else
    len = 1;
  if (c != WC_EOS)
    {
      /* if (LgBuffer + len >= AllmostFullBuffer && currentState == 0) */
      if (LgBuffer + len >= AllmostFullBuffer)
	TextToDocument ();
      if (LgBuffer + len >= MaxBufferLength)
	{
	  ParseHTMLError (theDocument, TEXT("Panic: buffer overflow"));
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
	    inputBuffer[LgBuffer++] = WC_SPACE;
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
#ifdef __STDC__
static void BlockInCharLevelElem (Element el)
#else
static void BlockInCharLevelElem (el)
Element             el;

#endif
{
   PtrElemToBeChecked  elTBC;
   Element             parent;

   if (LastElemToBeChecked != NULL)
      if (LastElemToBeChecked->Elem == el)
	 /* this element is already in the queue */
	 return;

   parent = TtaGetParent (el);
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
#ifdef __STDC__
static ThotBool     CheckSurrounding (Element * el, Element parent)
#else
static ThotBool     CheckSurrounding (el, parent)
Element             *el;
Element             parent;

#endif
{
   ElementType         parentType, newElType, elType;
   Element             newEl, ancestor, prev, prevprev;
   ThotBool	       ret;

   if (parent == NULL)
      return(FALSE);
   ret = FALSE;
   elType = TtaGetElementType (*el);
   if (elType.ElTypeNum == HTML_EL_TEXT_UNIT || elType.ElTypeNum == HTML_EL_BR
       || elType.ElTypeNum == HTML_EL_PICTURE_UNIT
       || elType.ElTypeNum == HTML_EL_Input
       || elType.ElTypeNum == HTML_EL_Text_Area)
     {
	/* the element to be inserted is a character string */
	/* Search the ancestor that is not a character level element */
	ancestor = parent;
	while (ancestor != NULL && IsCharacterLevelElement (ancestor))
	   ancestor = TtaGetParent (ancestor);
	if (ancestor != NULL)
	  {
	   elType = TtaGetElementType (ancestor);
	   if (CannotContainText (elType) &&
	       !Within (HTML_EL_Option_Menu, DocumentSSchema))
	      /* Element ancestor cannot contain text directly. Create a */
	      /* Pseudo_paragraph element as the parent of the text element */
	      {
	      newElType.ElSSchema = DocumentSSchema;
	      newElType.ElTypeNum = HTML_EL_Pseudo_paragraph;
	      newEl = TtaNewElement (theDocument, newElType);
	      TtaSetElementLineNumber (newEl, NumberOfLinesRead);
	      /* insert the new Pseudo_paragraph element */
	      InsertElement (&newEl);
	      if (newEl != NULL)
	        {
	          /* insert the Text element in the tree */
	          TtaInsertFirstChild (el, newEl, theDocument);
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
			TtaRemoveTree (prev, theDocument);
			TtaInsertFirstChild (&prev, newEl, theDocument);
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
	     newEl = TtaNewElement (theDocument, newElType);
	     TtaSetElementLineNumber (newEl, NumberOfLinesRead);
	     InsertElement (&newEl);
	     if (newEl != NULL)
	       {
	         TtaInsertFirstChild (el, newEl, theDocument);
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
#ifdef __STDC__
static ThotBool     InsertElement (Element * el)
#else
static ThotBool     InsertElement (el)
Element            *el;

#endif
{
   ThotBool            ret;
   Element             parent;

   if (InsertSibling ())
     {
	if (lastElement == NULL)
	   parent = NULL;
	else
	   parent = TtaGetParent (lastElement);
	if (!CheckSurrounding (el, parent))
	  if (parent != NULL)
	    TtaInsertSibling (*el, lastElement, FALSE, theDocument);
	  else
	    {
	       TtaDeleteTree (*el, theDocument);
	       *el = NULL;
	    }
	ret = TRUE;
     }
   else
     {
	if (!CheckSurrounding (el, lastElement))
	  TtaInsertFirstChild (el, lastElement, theDocument);
	ret = FALSE;
     }
   if (*el != NULL)
     {
	lastElement = *el;
	lastElementClosed = FALSE;
     }
   return ret;
}

/*----------------------------------------------------------------------
   CreateAttr      create an attribute of type attrType for the
   element el.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CreateAttr (Element el, AttributeType attrType, CHAR_T* text, ThotBool invalid)
#else
static void         CreateAttr (el, attrType, text, invalid)
Element             el;
AttributeType       attrType;
CHAR_T*             text;
ThotBool            invalid;

#endif
{
   int                 attrKind;
   int                 length;
   CHAR_T*             buffer;
   Attribute           attr, oldAttr;

   if (attrType.AttrTypeNum != 0)
     {
	oldAttr = TtaGetAttribute (el, attrType);
	if (oldAttr != NULL)
	   /* this attribute already exists */
	   attr = oldAttr;
	else
	   /* create a new attribute and attach it to the element */
	  {
	     attr = TtaNewAttribute (attrType);
	     TtaAttachAttribute (el, attr, theDocument);
	  }
	lastAttribute = attr;
	lastAttrElement = el;
	TtaGiveAttributeType (attr, &attrType, &attrKind);
	if (attrKind == 0)	/* enumerate */
	   TtaSetAttributeValue (attr, 1, el, theDocument);
	/* attribute BORDER without any value (ThotBool attribute) is */
	/* considered as BORDER=1 */
	if (attrType.AttrTypeNum == HTML_ATTR_Border)
	   TtaSetAttributeValue (attr, 1, el, theDocument);
	if (invalid)
	   /* Copy the name of the invalid attribute as the content */
	   /* of the Invalid_attribute attribute. */
	  {
	     length = ustrlen (text) + 2;
	     length += TtaGetTextAttributeLength (attr);
	     buffer = TtaAllocString (length + 1);
	     TtaGiveTextAttributeValue (attr, buffer, &length);
	     ustrcat (buffer, TEXT(" "));
	     ustrcat (buffer, text);
	     TtaSetAttributeText (attr, buffer, el, theDocument);
	     TtaFreeMemory (buffer);
	  }
     }
}

/*----------------------------------------------------------------------
   	ProcessOptionElement
	If multiple is FALSE, remove the SELECTED attribute from the
	option element, except if it's element el.
	If parsing is TRUE, associate a DefaultSelected attribute with
        element option if it has a SELECTED attribute.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void        ProcessOptionElement (Element option, Element el, Document doc, ThotBool multiple, ThotBool parsing)
#else  /* __STDC__ */
static void        ProcessOptionElement (option, el, doc, multiple, parsing)
Element             option;
Element		    el;
Document            doc;
ThotBool		    multiple;
ThotBool            parsing;

#endif /* __STDC__ */
{
   ElementType		elType;
   AttributeType	attrType;
   Attribute		attr;

   elType = TtaGetElementType (option);
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_Selected;
   if (!multiple && option != el)
      {
      /* Search the SELECTED attribute */
      attr = TtaGetAttribute (option, attrType);
      /* remove it if it exists */
      if (attr != NULL)
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
   LastLeafInElement
   return the last leaf element in element el.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element      LastLeafInElement (Element el)
#else
static Element      LastLeafInElement (el)
Element             el;

#endif
{
   Element             child, lastLeaf;

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
   ElementComplete
   Element el is complete. Check its attributes and its contents.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ElementComplete (Element el)
#else
static void         ElementComplete (el)
Element             el;
#endif
{
   ElementType         elType, newElType, childType;
   Element             constElem, child, desc, leaf, prev, next, last,
		       elFrames, lastFrame, lastChild;
   Attribute           attr;
   AttributeType       attrType;
   Language            lang;
   STRING              text;
   CHAR_T              lastChar[2];
   STRING              name1;
#ifdef STANDALONE
   STRING              imageName, name2;
#endif
   int                 length;

   elType = TtaGetElementType (el);
   /* is this a block-level element in a character-level element? */
   if (!IsCharacterLevelElement (el) && elType.ElTypeNum != HTML_EL_Comment_)
      BlockInCharLevelElem (el);

   newElType.ElSSchema = elType.ElSSchema;
   switch (elType.ElTypeNum)
    {
    case HTML_EL_Object:	/*  it's an object */
       /* create Object_Content */
       child = TtaGetFirstChild (el);
       if (child != NULL)
	 elType = TtaGetElementType (child);
	 
       /* is it the PICTURE element ? */
       if (child == NULL || elType.ElTypeNum != HTML_EL_PICTURE_UNIT)
	 {
	   desc = child;
	   /* create the PICTURE element */
	   elType.ElTypeNum = HTML_EL_PICTURE_UNIT;
	   child = TtaNewTree (theDocument, elType, "");
	   if (desc == NULL)
	     TtaInsertFirstChild (&child, el, theDocument);
	   else
	     TtaInsertSibling (child, desc, TRUE, theDocument);
	 }
       /* copy attribute data into SRC attribute of Object_Image */
       attrType.AttrSSchema = DocumentSSchema;
       attrType.AttrTypeNum = HTML_ATTR_data;
       attr = TtaGetAttribute (el, attrType);
       if (attr != NULL)
	 {
	    length = TtaGetTextAttributeLength (attr);
	    if (length > 0)
	      {
		name1 = TtaAllocString (length + 1);
		TtaGiveTextAttributeValue (attr, name1, &length);
		attrType.AttrTypeNum = HTML_ATTR_SRC;
		attr = TtaGetAttribute (child, attrType);
		if (attr == NULL)
		  {
		    attr = TtaNewAttribute (attrType);
		    TtaAttachAttribute (child, attr, theDocument);
		  }
		TtaSetAttributeText (attr, name1, child, theDocument);
		TtaFreeMemory (name1);
	      }
	 }
       /* is the Object_Content element already created ? */
       desc = child;
       TtaNextSibling(&desc);
       if (desc != NULL)
	 elType = TtaGetElementType (desc);
	 
       /* is it the Object_Content element ? */
       if (desc == NULL || elType.ElTypeNum != HTML_EL_Object_Content)
	 {
	   /* create Object_Content */
	   elType.ElTypeNum = HTML_EL_Object_Content;
	   desc = TtaNewTree (theDocument, elType, "");
	   TtaInsertSibling (desc, child, FALSE, theDocument);
	   /* move previous existing children into Object_Content */
	   child = TtaGetLastChild(el);
	   while (child != desc)
	     {
	       TtaRemoveTree (child, theDocument);
	       TtaInsertFirstChild (&child, desc, theDocument);
	       child = TtaGetLastChild(el);
	     }
	 }
	break;

    case HTML_EL_Unnumbered_List:
    case HTML_EL_Numbered_List:
    case HTML_EL_Menu:
    case HTML_EL_Directory:
	/* It's a List element. It should only have List_Item children.
	   If it has List element chidren, move these List elements
	   within their previous List_Item sibling.  This is to fix
           a bug in document generated by Mozilla. */
	prev = NULL;
	next = NULL;
	child = TtaGetFirstChild (el);
	while (child != NULL)
	   {
	   next = child;
	   TtaNextSibling (&next);
	   elType = TtaGetElementType (child);
	   if (elType.ElTypeNum == HTML_EL_Unnumbered_List ||
	       elType.ElTypeNum == HTML_EL_Numbered_List ||
	       elType.ElTypeNum == HTML_EL_Menu ||
	       elType.ElTypeNum == HTML_EL_Directory)
	     /* this list element is a child of another list element */
	     if (prev)
		{
		elType = TtaGetElementType (prev);
		if (elType.ElTypeNum == HTML_EL_List_Item)
		   {
		   /* get the last child of the previous List_Item */
		   desc = TtaGetFirstChild (prev);
		   last = NULL;
		   while (desc)
		      {
		      last = desc;
		      TtaNextSibling (&desc);
		      }
		   /* move the list element after the last child of the
		      previous List_Item */
		   TtaRemoveTree (child, theDocument);
		   if (last)
		      TtaInsertSibling (child, last, FALSE, theDocument);
		   else
		      TtaInsertFirstChild (&child, prev, theDocument);
		   child = prev;
		   }
	        }
	   prev = child;
	   child = next;
	   }
	break;

    case HTML_EL_FRAMESET:
	/* The FRAMESET element is now complete.  Gather all its FRAMESET
	   and FRAME children and wrap them up in a Frames element */
	elFrames = NULL; lastFrame = NULL;
	lastChild = NULL;
	child = TtaGetFirstChild (el);
	while (child != NULL)
	   {
	   next = child;
	   TtaNextSibling (&next);
	   elType = TtaGetElementType (child);
	   if (elType.ElTypeNum == HTML_EL_FRAMESET ||
	       elType.ElTypeNum == HTML_EL_FRAME ||
	       elType.ElTypeNum == HTML_EL_Comment_)
	       {
	       /* create the Frames element if it does not exist */
	       if (elFrames == NULL)
		 {
		    newElType.ElSSchema = DocumentSSchema;
		    newElType.ElTypeNum = HTML_EL_Frames;
		    elFrames = TtaNewElement (theDocument, newElType);
		    TtaSetElementLineNumber (elFrames, NumberOfLinesRead);
		    TtaInsertSibling (elFrames, child, TRUE, theDocument);
		 }
	       /* move the element as the last child of the Frames element */
	       TtaRemoveTree (child, theDocument);
	       if (lastFrame == NULL)
		  TtaInsertFirstChild (&child, elFrames, theDocument);
	       else
		  TtaInsertSibling (child, lastFrame, FALSE, theDocument);
	       lastFrame = child;
	       }
	   child = next;
           }
	break;

    case HTML_EL_Input:	/* it's an INPUT without any TYPE attribute */
	/* Create a child of type Text_Input */
	elType.ElTypeNum = HTML_EL_Text_Input;
	child = TtaNewTree (theDocument, elType, "");
	TtaSetElementLineNumber (child, NumberOfLinesRead);
	TtaInsertFirstChild (&child, el, theDocument);
	/* now, process it like a Text_Input element */
    case HTML_EL_Text_Input:
    case HTML_EL_Password_Input:
    case HTML_EL_File_Input:
      /* get element Inserted_Text */
      child = TtaGetFirstChild (el);
      if (child != NULL)
	{
	  attrType.AttrSSchema = DocumentSSchema;
	  attrType.AttrTypeNum = HTML_ATTR_Value_;
	  attr = TtaGetAttribute (el, attrType);
	  if (attr != NULL)
	    {
	      /* copy the value of attribute "value" into the first text
		 leaf of element */
	      length = TtaGetTextAttributeLength (attr);
	      if (length > 0)
		{
		  /* get the text leaf */
		  leaf = TtaGetFirstChild (child);
		  if (leaf != NULL)
		    {
		      childType = TtaGetElementType (leaf);
		      if (childType.ElTypeNum == HTML_EL_TEXT_UNIT)
			{
			  /* copy attribute value into the text leaf */
			  text = TtaAllocString (length + 1);
			  TtaGiveTextAttributeValue (attr, text, &length);
			  TtaSetTextContent (leaf, text, currentLanguage,
					     theDocument);
			  TtaFreeMemory (text);
			}
		    }
		}
	    }
	}
      break;

	case HTML_EL_META:
         ParseCharset (el);
         break;

    case HTML_EL_STYLE_:	/* it's a STYLE element */
    case HTML_EL_Preformatted:	/* it's a PRE */
    case HTML_EL_SCRIPT:	/* it's a SCRIPT element */
       /* if the last line of the Preformatted is empty, remove it */
       leaf = LastLeafInElement (el);
       if (leaf != NULL)
	  {
	    elType = TtaGetElementType (leaf);
	    if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	      /* the last leaf is a TEXT element */
	      {
		length = TtaGetTextLength (leaf);
		if (length > 0)
		  {
		     TtaGiveSubString (leaf, lastChar, length, 1);
		     if (lastChar[0] == EOL)
			/* last character is new line, delete it */
			{
		        if (length == 1)
		          /* empty TEXT element */
		          TtaDeleteTree (leaf, theDocument);
		        else
		          /* remove the last character */
		          TtaDeleteTextContent (leaf, length, 1,
					        theDocument);
			}
		  }
	      }
	  }
       if (ParsingCSS)
	 {
#ifndef STANDALONE
	   text = GetStyleContents (el);
	   if (text)
	     {
	       ReadCSSRules (theDocument, NULL, text, FALSE);
	       TtaFreeMemory (text);
	     }
#endif /* !STANDALONE */
	   ParsingCSS = FALSE;
	 }
	/* and continue as if it were a Preformatted or a Script */
       break;

    case HTML_EL_Text_Area:	/* it's a Text_Area */
      ParsingTextArea = FALSE;
       child = TtaGetFirstChild (el);
       if (child == NULL)
	  /* it's an empty Text_Area */
	  /* insert a Inserted_Text element in the element */
	 {
	   newElType.ElTypeNum = HTML_EL_Inserted_Text;
	   child = TtaNewTree (theDocument, newElType, "");
	   TtaInsertFirstChild (&child, el, theDocument);
	 }
       else
	 {
	   /* save the text into Default_Value attribute */
	   attrType.AttrSSchema = DocumentSSchema;
	   attrType.AttrTypeNum = HTML_ATTR_Default_Value;
	   if (TtaGetAttribute (el, attrType) == NULL)
	     /* attribute Default_Value is missing */
	     {
	       attr = TtaNewAttribute (attrType);
	       TtaAttachAttribute (el, attr, theDocument);
	       desc = TtaGetFirstChild (child);
	       length = TtaGetTextLength (desc) + 1;
	       text = TtaAllocString (length);
	       TtaGiveTextContent (desc, text, &length, &lang);
	       TtaSetAttributeText (attr, text, el, theDocument);
	       TtaFreeMemory (text);
	     }
	 }
       /* insert a Frame element */
       newElType.ElTypeNum = HTML_EL_Frame;
       constElem = TtaNewTree (theDocument, newElType, "");
       TtaInsertSibling (constElem, child, FALSE, theDocument);
       break;

    case HTML_EL_Radio_Input:
    case HTML_EL_Checkbox_Input:
       /* put an attribute Checked if it is missing */
       attrType.AttrSSchema = DocumentSSchema;
       attrType.AttrTypeNum = HTML_ATTR_Checked;
       if (TtaGetAttribute (el, attrType) == NULL)
	  /* attribute Checked is missing */
	 {
	    attr = TtaNewAttribute (attrType);
	    TtaAttachAttribute (el, attr, theDocument);
	    TtaSetAttributeValue (attr, HTML_ATTR_Checked_VAL_No_, el,
				  theDocument);
	 }
       break;

    case HTML_EL_Option_Menu:
       /* Check that at least one option has a SELECTED attribute */
       OnlyOneOptionSelected (el, theDocument, TRUE);
       break;
    case HTML_EL_PICTURE_UNIT:
#ifdef STANDALONE
       /* copy value of attribute SRC into the content of the element */
       attrType.AttrSSchema = DocumentSSchema;
       attrType.AttrTypeNum = HTML_ATTR_SRC;
       attr = TtaGetAttribute (el, attrType);
       if (attr != NULL)
	 {
	    length = TtaGetTextAttributeLength (attr);
	    name1 = TtaAllocString (length + 1);
	    name2 = TtaAllocString (length + 1);
	    imageName = TtaAllocString (length + 1);
	    TtaGiveTextAttributeValue (attr, name1, &length);
	    /* extract image name from full name */
	    TtaExtractName (name1, name2, imageName);
	    if (ustrlen (imageName) == 0)
	       /* full names ends with ''/ */
	       TtaExtractName (name2, name1, imageName);
	    if (ustrlen (imageName) != 0)
	       TtaSetTextContent (el, imageName, currentLanguage, theDocument);
	    TtaFreeMemory (name1);
	    TtaFreeMemory (name2);
	    TtaFreeMemory (imageName);
	 }
#endif /* STANDALONE */
       break;

#ifndef STANDALONE
    case HTML_EL_LINK:
       CheckCSSLink (el, theDocument, DocumentSSchema);
       break;
#endif /* STANDALONE */

    case HTML_EL_Data_cell:
    case HTML_EL_Heading_cell:
      /* insert a pseudo paragraph into empty cells */
       child = TtaGetFirstChild (el);
       if (child == NULL)
	 {
	   elType.ElTypeNum = HTML_EL_Pseudo_paragraph;
	   child = TtaNewTree (theDocument, elType, "");
	   if (child != NULL)
	     TtaInsertFirstChild (&child, el, theDocument);
	 }

#ifndef STANDALONE
       /* detect whether we're parsing a whole table or just a cell */
       if (WithinTable == 0)
	 NewCell (el, theDocument, FALSE);
#endif /* STANDALONE */
       break;

    case HTML_EL_Table:
#ifndef STANDALONE
       CheckTable (el, theDocument);
#endif
       WithinTable--;
       break;

#ifndef STANDALONE
    case HTML_EL_TITLE:
       /* show the TITLE in the main window */
       UpdateTitle (el, theDocument);
       break;
#endif

    default:
       break;
    }
}

/*----------------------------------------------------------------------
   RemoveEndingSpaces
   If element el is a block-level element, remove all spaces contained
   at the end of that element.
   Return TRUE if spaces have been removed.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     RemoveEndingSpaces (Element el)
#else
static ThotBool     RemoveEndingSpaces (el)
Element el;

#endif
{
   int                 length, nbspaces;
   ElementType         elType;
   Element             lastLeaf;
   CHAR_T                lastChar[2];
   ThotBool            endingSpacesDeleted;

   endingSpacesDeleted = FALSE;
   if (IsBlockElement (el))
      /* it's a block element. */
      {
	   /* Search the last leaf in the element's tree */
	   lastLeaf = LastLeafInElement (el);
	   if (lastLeaf != NULL)
	     {
	       elType = TtaGetElementType (lastLeaf);
	       if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
		 /* the las leaf is a TEXT element */
		 {
		   length = TtaGetTextLength (lastLeaf);
		   if (length > 0)
		     {
		       /* count ending spaces */
		       nbspaces = 0;
		       do
			 {
			   TtaGiveSubString (lastLeaf, lastChar, length,
					     1);
			   if (lastChar[0] == SPACE)
			     {
			       length--;
			       nbspaces++;
			     }
			 }
		       while (lastChar[0] == SPACE && length > 0);
		       if (nbspaces > 0)
			 if (length == 0)
			   /* empty TEXT element */
			   TtaDeleteTree (lastLeaf, theDocument);
			 else
			   /* remove the ending spaces */
			   TtaDeleteTextContent (lastLeaf, length + 1,
						 nbspaces, theDocument);
		     }
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
#ifdef __STDC__
static ThotBool     CloseElement (int entry, int start, ThotBool onStartTag)
#else
static ThotBool     CloseElement (entry, start, onStartTag)
int                 entry;
int                 start;
ThotBool            onStartTag;
#endif
{
   int                 i;
   ElementType         elType, parentType;
   Element             el, parent;
   ThotBool            ret, stop, spacesDeleted;

   ret = FALSE;
   /* the closed HTML element corresponds to a Thot element. */
   stop = FALSE;
   /* type of the element to be closed */
   elType.ElSSchema = DocumentSSchema;
   elType.ElTypeNum = pHTMLGIMapping[entry].ThotType;
   if (StackLevel > 0)
     {
       el = lastElement;
       if (lastElementClosed)
	  el = TtaGetParent (el);
       i = StackLevel - 1;
       if (start < 0)
	 /* Explicit close */
	 {
	   /* If we meet the end tag of a form, font or center
	      looks for that element in the stack, but not at
	      a higher level as a table element */
	   if (!onStartTag &&
	       (!ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("form")) ||
            !ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("font")) ||
            !ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("center"))))
	     while (i > 0 && entry != GINumberStack[i] && !stop)
	       if (!ustrcmp (pHTMLGIMapping[GINumberStack[i]].htmlGI, TEXT("tbody")) ||
               !ustrcmp (pHTMLGIMapping[GINumberStack[i]].htmlGI, TEXT("tr"))    ||
               !ustrcmp (pHTMLGIMapping[GINumberStack[i]].htmlGI, TEXT("th"))    ||
               !ustrcmp (pHTMLGIMapping[GINumberStack[i]].htmlGI, TEXT("td")))
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
	   if (!ustrcmp (pHTMLGIMapping[start].htmlGI, TEXT("li")))
	     while (i > 0 && entry != GINumberStack[i] && !stop)
	       if (!ustrcmp (pHTMLGIMapping[GINumberStack[i]].htmlGI, TEXT("ol"))  ||
               !ustrcmp (pHTMLGIMapping[GINumberStack[i]].htmlGI, TEXT("ul"))  ||
               !ustrcmp (pHTMLGIMapping[GINumberStack[i]].htmlGI, TEXT("dir")) ||
               !ustrcmp (pHTMLGIMapping[GINumberStack[i]].htmlGI, TEXT("menu")))
		 stop = TRUE;
	       else
		 i--;
	   else if (!ustrcmp (pHTMLGIMapping[start].htmlGI, TEXT("option")))
	     while (i > 0 && entry != GINumberStack[i] && !stop)
	       if (!ustrcmp (pHTMLGIMapping[GINumberStack[i]].htmlGI, TEXT("select")))
		 stop = TRUE;
	       else
		 i--;
	   else if (!ustrcmp (pHTMLGIMapping[start].htmlGI, TEXT("dd")) ||
                !ustrcmp (pHTMLGIMapping[start].htmlGI, TEXT("dt")))
	     while (i > 0 && entry != GINumberStack[i] && !stop)
	       if (!ustrcmp (pHTMLGIMapping[GINumberStack[i]].htmlGI, TEXT("dl")))
		 stop = TRUE;
	       else
		 i--;
	   else if (!ustrcmp (pHTMLGIMapping[start].htmlGI, TEXT("tr")) ||
                !ustrcmp (pHTMLGIMapping[start].htmlGI, TEXT("td")) ||
                !ustrcmp (pHTMLGIMapping[start].htmlGI, TEXT("th")))
	     while (i > 0 && entry != GINumberStack[i] && !stop)
	       if (!ustrcmp (pHTMLGIMapping[GINumberStack[i]].htmlGI, TEXT("table")))
		 stop = TRUE;
	       else
		 i--;
	 }

       if (i >= 0 && entry == GINumberStack[i])
	 /* element found in the stack */
	 {
	   /* This element and its whole subtree are closed */
	   StackLevel = i;
	   lastElement = ElementStack[i];
	   lastElementClosed = TRUE;
	   ret = TRUE;
	 }
       else if (!stop)
	 /* element not found in the stack */
	 if (start >= 0 && lastElement != NULL)
	   {
	     /* implicit close. Check the parent of current element */
	     if (InsertSibling ())
	       parent = TtaGetParent (lastElement);
	     else
	       parent = lastElement;
	     if (parent != NULL)
	       {
	         parentType = TtaGetElementType (parent);
	         if (elType.ElTypeNum == parentType.ElTypeNum)
	           {
	             lastElement = parent;
	             lastElementClosed = TRUE;
	             ret = TRUE;
	           }
	         else if (TtaIsLeaf (TtaGetElementType (lastElement)))
	           {
	             parent = TtaGetParent (parent);
	             if (parent != NULL)
	               {
	                 parentType = TtaGetElementType (parent);
	                 if (elType.ElTypeNum == parentType.ElTypeNum)
	                   {
	                     lastElement = parent;
	                     lastElementClosed = TRUE;
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
	     if (ElementStack[i] == lastElement)
	       {
		 StackLevel = i;
		 i = 0;
	       }
	     else
	       {
		 if (TtaIsAncestor (ElementStack[i], lastElement))
	           StackLevel = i;
	         i--;
	       }
	   if (StackLevel > 0)
	     currentLanguage = LanguageStack[StackLevel - 1];

	   /* complete all closed elements */
	   if (el != lastElement)
	     if (!TtaIsAncestor(el, lastElement))
	       el = NULL;
	   spacesDeleted = FALSE;
	   while (el != NULL)
	     {
	       ElementComplete (el);
	       if (!spacesDeleted)
	          /* If the element closed is a block-element, remove */
	          /* spaces contained at the end of that element */
	          spacesDeleted = RemoveEndingSpaces (el);
	       if (el == lastElement)
		 el = NULL;
	       else
		 el = TtaGetParent (el);
	     }
	 }
     }
   
   return ret;
}


/*----------------------------------------------------------------------
   TypeAttrValue   Value val has been read for the HTML attribute
   TYPE. Create a child for the current Thot
   element INPUT accordingly.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         TypeAttrValue (CHAR_T* val)
#else
static void         TypeAttrValue (val)
CHAR_T*             val;

#endif
{
  ElementType         elType;
  Element             newChild;
  AttributeType       attrType;
  Attribute           attr;
  CHAR_T              msgBuffer[MaxMsgLength];
  int                 value;

  value = MapAttrValue (DummyAttribute, val);
  if (value < 0)
    {
      if (ustrlen (val) > MaxMsgLength - 40)
         val[MaxMsgLength - 40] = WC_EOS;
      usprintf (msgBuffer, TEXT("Unknown attribute value \"type = %s\""), val);
      ParseHTMLError (theDocument, msgBuffer);
      attrType.AttrSSchema = DocumentSSchema;
      attrType.AttrTypeNum = pHTMLAttributeMapping[0].ThotAttribute;
      usprintf (msgBuffer, TEXT("type=%s"), val);
      CreateAttr (lastElement, attrType, msgBuffer, TRUE);
    }
  else
    {
      elType = TtaGetElementType (lastElement);
      if (elType.ElTypeNum != HTML_EL_Input)
	{
        if (ustrlen (val) > MaxMsgLength - 40)
	   val[MaxMsgLength - 40] = WC_EOS;
	usprintf (msgBuffer, TEXT("Duplicate attribute \"type = %s\""), val);
	}
      else
	{
	  elType.ElSSchema = DocumentSSchema;
	  elType.ElTypeNum = value;
	  newChild = TtaNewTree (theDocument, elType, "");
	  TtaSetElementLineNumber (newChild, NumberOfLinesRead);
	  TtaInsertFirstChild (&newChild, lastElement, theDocument);
	  if (value == HTML_EL_PICTURE_UNIT)
	    {
	      /* add the attribute IsInput to input pictures */
	      attrType.AttrSSchema = elType.ElSSchema;
	      attrType.AttrTypeNum = HTML_ATTR_IsInput;
	      attr = TtaNewAttribute (attrType);
	      TtaAttachAttribute (newChild, attr, theDocument);
	    }
	}
    }
}


/*----------------------------------------------------------------------
   EndOfStartTag   a ">" has been read. It indicates the end
   of a start tag.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfStartTag (CHAR_T c)
#else
static void         EndOfStartTag (c)
CHAR_T                c;

#endif
{
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  int                 length;
  STRING              text;
  ThotBool	      math;

  UnknownTag = FALSE;
  if ((lastElement != NULL) && (lastElemEntry != -1))
    {
      math = FALSE;
      if (!ustrcmp (pHTMLGIMapping[lastElemEntry].htmlGI, TEXT("math")))
	/* a <math> tag has been read */
	math = TRUE;
      else if (!ustrcmp (pHTMLGIMapping[lastElemEntry].htmlGI, TEXT("mathdisp")))
	/* a <mathdisp> tag has been read.  add an attribute "mode=display"
	   (for compatibility with old MathML version WD-math-970704 */
	{
	  math = TRUE;
	  elType = TtaGetElementType (lastElement);
	  attrType.AttrSSchema = elType.ElSSchema;
	  attrType.AttrTypeNum = HTML_ATTR_mode;
	  attr = TtaGetAttribute (lastElement, attrType);
	  if (attr == NULL)
	    /* create a new attribute and attach it to the element */
	    {
	      attr = TtaNewAttribute (attrType);
	      TtaAttachAttribute (lastElement, attr, theDocument);
	    }
	  TtaSetAttributeValue (attr, HTML_ATTR_mode_VAL_display,
				lastElement, theDocument);
	}
      if (math)
	{
#ifndef STANDALONE
	  /* Parse the MathML structure */
	  XMLparse (stream, &CurrentBufChar, TEXT("MathML"), theDocument, lastElement, FALSE,
		    currentLanguage, pHTMLGIMapping[lastElemEntry].htmlGI);
#endif /* STANDALONE */
	  /* when returning from the XML parser, the end tag has already
	     been read */
	  (void) CloseElement (lastElemEntry, -1, FALSE);
	}
      else if (!ustrcmp (pHTMLGIMapping[lastElemEntry].htmlGI, TEXT("xmlgraphics")))
	/* a <XMLGRAPHICS> tag has been read */
        {
	  /* Parse the GraphML structure */
#ifndef STANDALONE
	  XMLparse (stream, &CurrentBufChar, TEXT("GraphML"), theDocument, lastElement, FALSE,
		    currentLanguage, pHTMLGIMapping[lastElemEntry].htmlGI);
#endif /* STANDALONE */
	  /* when returning from the XML parser, the end tag has already
	     been read */
	  (void) CloseElement (lastElemEntry, -1, FALSE);	   
	}
      else if (!ustrcmp (pHTMLGIMapping[lastElemEntry].htmlGI, TEXT("pre"))   ||
               !ustrcmp (pHTMLGIMapping[lastElemEntry].htmlGI, TEXT("style")) ||
               !ustrcmp (pHTMLGIMapping[lastElemEntry].htmlGI, TEXT("script")))
	/* a <PRE>, <STYLE> or <SCRIPT> tag has been read */
	AfterTagPRE = TRUE;
      else if (!ustrcmp (pHTMLGIMapping[lastElemEntry].htmlGI, TEXT("table")))
	/* <TABLE> has been read */
	WithinTable++;
      else if (pHTMLGIMapping[lastElemEntry].htmlContents == 'E')
	/* this is an empty element. Do not expect an end tag */
	{
	  CloseElement (lastElemEntry, -1, TRUE);
	  ElementComplete (lastElement);
	}
      
      /* if it's a LI element, creates its IntItemStyle attribute
	 according to surrounding elements */
      SetAttrIntItemStyle (lastElement, theDocument);
      /* if it's an AREA element, computes its position and size */
      ParseAreaCoords (lastElement, theDocument);
      /* if it's a STYLE element in CSS notation, activate the CSS */
      /* parser for parsing the element content */
      elType = TtaGetElementType (lastElement);
      if (elType.ElTypeNum == HTML_EL_STYLE_)
	{
	  /* Search the Notation attribute */
	  attrType.AttrSSchema = elType.ElSSchema;
	  attrType.AttrTypeNum = HTML_ATTR_Notation;
	  attr = TtaGetAttribute (lastElement, attrType);
	  if (attr == NULL)
	    /* No Notation attribute. Assume CSS by default */
	    ParsingCSS = TRUE;
	  else
	    /* the STYLE element has a Notation attribute */
	    /* get its value */
	    {
	      length = TtaGetTextAttributeLength (attr);
	      text = TtaAllocString (length + 1);
	      TtaGiveTextAttributeValue (attr, text, &length);
	      if (!ustrcasecmp (text, TEXT("text/css")))
		ParsingCSS = TRUE;
	      TtaFreeMemory (text);
	    }
	}
      else if (elType.ElTypeNum == HTML_EL_Text_Area)
	{
	  /* we have to read the content as a simple text unit */
	  ParsingTextArea = TRUE;
	}
      
    }
}


/*----------------------------------------------------------------------
   ContextOK       returns TRUE if the element at position entry
   in the mapping table is allowed to occur in the
   current structural context.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     ContextOK (int entry)
#else
static ThotBool     ContextOK (entry)
int                 entry;

#endif
{
   ThotBool            ok;
   int		       saveLastElemEntry;

   if (StackLevel == 0 || GINumberStack[StackLevel - 1] < 0)
     return TRUE;
   else
     {
       ok = TRUE;
       /* only TH and TD elements are allowed as children of a TR element */
       if (!ustrcmp (pHTMLGIMapping[GINumberStack[StackLevel - 1]].htmlGI, TEXT("tr")))
	 if (ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("th")) &&
	     ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("td")))
	   ok = FALSE;
       if (ok)
	 /* only CAPTION, THEAD, TFOOT, TBODY, COLGROUP, COL and TR are */
	 /* allowed as children of a TABLE element */
	 if (!ustrcmp (pHTMLGIMapping[GINumberStack[StackLevel - 1]].htmlGI, TEXT("table")))
	   if (ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("caption"))  &&
	       ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("thead"))    &&
	       ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("tfoot"))    &&
	       ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("tbody"))    &&
	       ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("colgroup")) &&
	       ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("col"))      &&
	       ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("tr")))
	     if (!ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("td")) ||
             !ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("th")))
	       /* Table cell within a table, without a tr. Assume tr */
	       {
		/* save the last last GI read from the input file */
		saveLastElemEntry = lastElemEntry;
		/* simulate a <TR> tag */
	        ProcessStartGI (TEXT("tr"));
		/* restore the last tag that has actually been read */
		lastElemEntry = saveLastElemEntry;
	       }
	     else
	       ok = FALSE;
       if (ok)
	 /* CAPTION, THEAD, TFOOT, TBODY, COLGROUP are allowed only as
	    children of a TABLE element */
	 if (ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("caption"))  == 0 ||
	     ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("thead"))    == 0 ||
	     ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("tfoot"))    == 0 ||
	     ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("tbody"))    == 0 ||
	     ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("colgroup")) == 0)
	   if (ustrcmp (pHTMLGIMapping[GINumberStack[StackLevel - 1]].htmlGI, TEXT("table")) != 0)
	      ok = FALSE;
       if (ok)
	 /* only TR is allowed as a child of a THEAD, TFOOT or TBODY element */
	 if (!ustrcmp (pHTMLGIMapping[GINumberStack[StackLevel - 1]].htmlGI, TEXT("thead")) ||
	     !ustrcmp (pHTMLGIMapping[GINumberStack[StackLevel - 1]].htmlGI, TEXT("tfoot")) ||
	     !ustrcmp (pHTMLGIMapping[GINumberStack[StackLevel - 1]].htmlGI, TEXT("tbody")))
	   if (ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("tr")))
	     if (!ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("td")) ||
             !ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("th")))
	       /* Table cell within a thead, tfoot or tbody without a tr. */
	       /* Assume tr */
	       {
		/* save the last last GI read from the input file */
		saveLastElemEntry = lastElemEntry;
		/* simulate a <tr> tag */
	        ProcessStartGI (TEXT("tr"));
		/* restore the last tag that has actually been read */
		lastElemEntry = saveLastElemEntry;
	       }
	     else
	       ok = FALSE;
       if (ok)
	 /* refuse BODY within BODY */
	 if (ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("body")) == 0)
	   if (Within (HTML_EL_BODY, DocumentSSchema))
	     ok = FALSE;
       if (ok)
	 /* refuse HEAD within HEAD */
	 if (ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("head")) == 0)
	   if (Within (HTML_EL_HEAD, DocumentSSchema))
	     ok = FALSE;
       if (ok)
	 /* refuse STYLE within STYLE */
	 if (ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("style")) == 0)
	   if (Within (HTML_EL_STYLE_, DocumentSSchema))
	     ok = FALSE;
       return ok;
     }
}

/*----------------------------------------------------------------------
   SpecialImplicitEnd
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SpecialImplicitEnd (int entry)
#else
static void         SpecialImplicitEnd (entry)
int                 entry;

#endif
{
   ElementType         elType;

   /* if current element is DD, Hn closes that DD only when there is */
   /* no enclosing DL */
   if (pHTMLGIMapping[entry].htmlGI[0] == TEXT('H') &&
       pHTMLGIMapping[entry].htmlGI[1] >= TEXT('1') &&
       pHTMLGIMapping[entry].htmlGI[1] <= TEXT('6') &&
       pHTMLGIMapping[entry].htmlGI[2] == WC_EOS)
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
   InsertInvalidEl
   create an element Invalid_element with the indicated content.
   position indicate whether the element type is unknown (FALSE) or the
   tag position is incorrect (TRUE).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InsertInvalidEl (CHAR_T* content, ThotBool position)
#else
static void         InsertInvalidEl (content, position)
CHAR_T*             content;
ThotBool		    position;

#endif
{
   ElementType         elType;
   AttributeType       attrType;
   Element             elInv, elText;
   Attribute	       attr;

   elType.ElSSchema = DocumentSSchema;
   elType.ElTypeNum = HTML_EL_Invalid_element;
   elInv = TtaNewElement (theDocument, elType);
   TtaSetElementLineNumber (elInv, NumberOfLinesRead);
   InsertElement (&elInv);
   if (elInv != NULL)
     {
	lastElementClosed = TRUE;
	elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	elText = TtaNewElement (theDocument, elType);
	TtaSetElementLineNumber (elText, NumberOfLinesRead);
	TtaInsertFirstChild (&elText, elInv, theDocument);
	TtaSetTextContent (elText, content, currentLanguage, theDocument);
	TtaSetAccessRight (elText, ReadOnly, theDocument);
	attrType.AttrSSchema = DocumentSSchema;
	attrType.AttrTypeNum = HTML_ATTR_Error_type;
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (elInv, attr, theDocument);
	if (position)
	   TtaSetAttributeValue (attr, HTML_ATTR_Error_type_VAL_BadPosition,
				 elInv, theDocument);
	else
	   TtaSetAttributeValue (attr, HTML_ATTR_Error_type_VAL_UnknownTag,
				 elInv, theDocument);
     }
}

/*----------------------------------------------------------------------
   ProcessStartGI  An HTML GI has been read in a start tag.
   Create the corresponding Thot thing (element, attribute,
   or character), according to the mapping table.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ProcessStartGI (CHAR_T* GIname)
#else
static void         ProcessStartGI (GIname)
CHAR_T*             GIname;

#endif
{
  ElementType         elType;
  Element             el;
  int                 entry, i;
  CHAR_T              msgBuffer[MaxMsgLength];
  PtrClosedElement    pClose;
  ThotBool            sameLevel;
  SSchema	      schema;

  /* ignore tag <P> within PRE */
  if (Within (HTML_EL_Preformatted, DocumentSSchema))
    if (ustrcasecmp (GIname, TEXT("p")) == 0)
      return;

  /* search the HTML element name in the mapping table */
  schema = DocumentSSchema;
  entry = MapGI (GIname, &schema, theDocument);
  lastElemEntry = entry;
  if (entry < 0)
    /* not found in the HTML DTD */
    {
      /* check if it's the math or svg tag with a namespace prefix */
      /* So, look for a colon in the element name */
      for (i = 0; GIname[i] != TEXT(':') && GIname[i] != WC_EOS; i++);
      if (GIname[i] == TEXT(':') &&
	      (ustrcasecmp (&GIname[i+1], TEXT("math")) == 0 ||
	       ustrcasecmp (&GIname[i+1], TEXT("xmlgraphics")) == 0))
	/* it's a math or svg tag with a namespace prefix. OK */
	{
         entry = MapGI (&GIname[i+1], &schema, theDocument);
	 lastElemEntry = entry;
	}
      else
	/* unknown tag */
	{
	  if (ustrlen (GIname) > MaxMsgLength - 20)
	    GIname[MaxMsgLength - 20] = WC_EOS;
	  usprintf (msgBuffer, TEXT("Unknown tag <%s>"), GIname);
	  ParseHTMLError (theDocument, msgBuffer);
	  UnknownTag = TRUE;
	  /* create an Invalid_element */
	  usprintf (msgBuffer, TEXT("<%s"), GIname);
	  InsertInvalidEl (msgBuffer, FALSE);
	}
    }
  if (entry >= 0)
    {
      /* does this start tag also imply the end tag of some current elements? */
      pClose = pHTMLGIMapping[entry].firstClosedElem;
      while (pClose != NULL)
	{
	  CloseElement (pClose->tagNum, entry, TRUE);
	  pClose = pClose->nextClosedElem;
	}
      /* process some special cases... */
      SpecialImplicitEnd (entry);
      if (!ContextOK (entry))
	/* element not allowed in the current structural context */
	{
	  usprintf (msgBuffer, TEXT("Tag <%s> is not allowed here"), GIname);
	  ParseHTMLError (theDocument, msgBuffer);
	  UnknownTag = TRUE;
	  /* create an Invalid_element */
	  usprintf (msgBuffer, TEXT("<%s"), GIname);
	  InsertInvalidEl (msgBuffer, TRUE);
	}
      else
	  {
	    el = NULL;
	    sameLevel = TRUE;
	    if (pHTMLGIMapping[entry].ThotType > 0)
	      {
		if (pHTMLGIMapping[entry].ThotType == HTML_EL_HTML)
		  /* the corresponding Thot element is the root of the
		     abstract tree, which has been created at initialization */
		  el = rootElement;
		else
		  /* create a Thot element */
		  {
		    elType.ElSSchema = DocumentSSchema;
		    elType.ElTypeNum = pHTMLGIMapping[entry].ThotType;
		    if (pHTMLGIMapping[entry].htmlContents == 'E')
		      /* empty HTML element. Create all children specified */
		      /* in the Thot structure schema */
		      el = TtaNewTree (theDocument, elType, "");
		    else
		      /* the HTML element may have children. Create only */
		      /* the corresponding Thot element, without any child */
		      el = TtaNewElement (theDocument, elType);
		    TtaSetElementLineNumber (el, NumberOfLinesRead);
		    sameLevel = InsertElement (&el);
		    if (el != NULL)
		      {
			if (pHTMLGIMapping[entry].htmlContents == 'E')
			  lastElementClosed = TRUE;
			if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
			  /* an empty Text element has been created. The */
			  /* following character data must go to that elem. */
			  MergeText = TRUE;
		      }
		  }
	      }
	    if (pHTMLGIMapping[entry].htmlContents != 'E')
	      {
		ElementStack[StackLevel] = el;
		if (sameLevel)
		  ThotLevel[StackLevel] = ThotLevel[StackLevel - 1];
		else
		  ThotLevel[StackLevel] = ThotLevel[StackLevel - 1] + 1;
	        LanguageStack[StackLevel] = currentLanguage;
		GINumberStack[StackLevel++] = entry;
	      }
	  }
     }
}


/*----------------------------------------------------------------------
   PutInContent    put the string ChrString in the leaf of
   current element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element      PutInContent (STRING ChrString)
#else
static Element      PutInContent (ChrString)
STRING              ChrString;

#endif
{
   Element             el, child;
   ElementType         elType;
   int                 length;

   el = NULL;
   if (lastElement != NULL)
     {
	/* search first leaf of current element */
	el = lastElement;
	do
	  {
	     child = TtaGetFirstChild (el);
	     if (child != NULL)
		el = child;
	  }
	while (child != NULL);
	elType = TtaGetElementType (el);
	length = 0;
	if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	   length = TtaGetTextLength (el);
	if (length == 0)
	   TtaSetTextContent (el, ChrString, currentLanguage, theDocument);
	else
	   TtaAppendTextContent (el, ChrString, theDocument);
     }
   return el;
}

/*----------------------------------------------------------------------
   EndOfAttrName   
   A XHTML attribute has been read. Create the 
   corresponding Thot attribute.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfAttrName (CHAR_T *attrName)
#else
static void         EndOfAttrName (attrName)
CHAR_T         *attrName;

#endif
{
   AttributeMapping*   tableEntry;
   AttributeType       attrType;
   ElementType         elType;
   Element             child;
   Attribute           attr;
   SSchema             schema;
   CHAR_T              translation;
   CHAR_T              msgBuffer[MaxMsgLength];

   /* if a single '/' or '?' has been read instead of an attribute name, ignore
      that character.  This is to accept the XML syntax for empty elements or
      processing instructions, such as <img src="SomeUrl" /> or
      <?xml version="1.0"?>  */

   /* A FAIRE */

   /* get the corresponding Thot attribute */
   if (UnknownTag)
      /* ignore attributes of unknown tags */
      tableEntry = NULL;
   else
      tableEntry = MapAttr (attrName, &schema,
			    lastElemEntry, theDocument);

   if (tableEntry == NULL)
      /* this attribute is not in the HTML mapping table */
     {
	if (ustrcasecmp (attrName, TEXT("xmlns")) == 0 ||
	    ustrncasecmp (attrName, TEXT("xmlattrNamens:"), 6) == 0)
	  /* this is a namespace declaration */
	  {
	    lastAttrEntry = NULL;
	    /**** register this namespace ****/;
	  }
	else
	  if (ustrcasecmp (attrName, TEXT("xml:lang")) == 0)
	    /* attribute xml:lang is not considered as invalid, but it is
	       ignored */
	    lastAttrEntry = NULL;
	  else
	    {
	      if (ustrlen (attrName) > MaxMsgLength - 30)
		  inputBuffer[MaxMsgLength - 30] = WC_EOS;
	      usprintf (msgBuffer,
			TEXT("Unknown attribute \"%s\""),
			attrName);
	      ParseHTMLError (theDocument, msgBuffer);
	      /* attach an Invalid_attribute to the current element */
	      tableEntry = &pHTMLAttributeMapping[0];
	      schema = DocumentSSchema;
	      UnknownAttr = TRUE;
	    }
     }
   else
        UnknownAttr = FALSE;


   if (tableEntry != NULL &&
       lastElement != NULL &&
       (!lastElementClosed || (lastElement != rootElement)))
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
	   CreateAttr (lastElement, attrType, attrName, 
		       (ThotBool)(tableEntry == &pHTMLAttributeMapping[0]));
	   if (attrType.AttrTypeNum == HTML_ATTR_HREF_)
	     {
	       elType = TtaGetElementType (lastElement);
	       if (elType.ElTypeNum == HTML_EL_Anchor)
		 /* attribute HREF for element Anchor */
		 /* create attribute PseudoClass = link */
		 {
		   attrType.AttrTypeNum = HTML_ATTR_PseudoClass;
		   attr = TtaNewAttribute (attrType);
		   TtaAttachAttribute (lastElement, attr, theDocument);
		   TtaSetAttributeText (attr, TEXT("link"),
					lastElement, theDocument);
		 }
	     }
	   else if (attrType.AttrTypeNum == HTML_ATTR_Checked)
	     {
	       /* create Default-Checked attribute */
	       child = TtaGetFirstChild (lastElement);
	       if (child != NULL)
		 {
		   attrType.AttrSSchema = DocumentSSchema;
		   attrType.AttrTypeNum = HTML_ATTR_DefaultChecked;
		   attr = TtaNewAttribute (attrType);
		   TtaAttachAttribute (child, attr, theDocument);
		   TtaSetAttributeValue (attr, HTML_ATTR_DefaultChecked_VAL_Yes_,
					 child, theDocument);
		 }
	     }
	   else 
	     if (attrType.AttrTypeNum == HTML_ATTR_Selected)
	       {
		 /* create Default-Selected attribute */
		 attrType.AttrSSchema = DocumentSSchema;
		 attrType.AttrTypeNum = HTML_ATTR_DefaultSelected;
		 attr = TtaNewAttribute (attrType);
		 TtaAttachAttribute (lastElement, attr, theDocument);
		 TtaSetAttributeValue (attr, HTML_ATTR_DefaultSelected_VAL_Yes_,
				       lastElement, theDocument);
	       }
	   break;

	 case SPACE:
	   /* nothing to do */
	   break;
	   
	 default:
	   break;
	 }
     }
}


/*----------------------------------------------------------------------
   EndOfAttrValue
   An attribute value has been read from the HTML file.
   Put that value in the current Thot attribute.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfAttrValue (CHAR_T *attrValue)
#else
static void         EndOfAttrValue (attrValue)
CHAR_T     *attrValue;

#endif
{
   AttributeType       attrType, attrType1;
   Attribute	       attr;
   ElementType	       elType;
   Element             child;
   Language            lang;
   CHAR_T              translation;
   char                shape;
   STRING              buffer;
   STRING              attrName;
   int                 val;
   int                 length;
   int                 attrKind;
   ThotBool            done;
   CHAR_T              msgBuffer[MaxMsgLength];

   ReadingAnAttrValue = FALSE;

   if (UnknownAttr)
       /* this is the end of value of an invalid attribute. Keep the */
       /* quote character that ends the value for copying it into the */
       /* Invalid_attribute. */
     {
       /* What to do in thos case ? */
     }

   if (lastAttrEntry == NULL)
     {
       return;
     }

   done = FALSE;
   if (lastElementClosed && (lastElement == rootElement))
      /* an attribute after the tag </html>, ignore it */
      done = TRUE;

   else
      /* treatments of some particular HTML attributes */
      if (!ustrcmp (lastAttrEntry->XMLattribute, TEXT("style")))
	{
#       ifndef STANDALONE
	  TtaSetAttributeText (lastAttribute, attrValue,
			       lastAttrElement, theDocument);
	  ParseHTMLSpecificStyle (lastElement, attrValue,
				  theDocument, FALSE);
#       endif
	  done = TRUE;
	}
#  ifndef STANDALONE
      else
	{
	  if (!ustrcmp (lastAttrEntry->XMLattribute, TEXT("link")))
	      HTMLSetAlinkColor (theDocument, attrValue);
	  else
	    if (!ustrcmp (lastAttrEntry->XMLattribute, TEXT("alink")))
	        HTMLSetAactiveColor (theDocument, attrValue);
	    else
	      if (!ustrcmp (lastAttrEntry->XMLattribute, TEXT("vlink")))
	          HTMLSetAvisitedColor (theDocument, attrValue);
	}
#  endif

   if (!done)
     {
       val = 0;
       translation = lastAttrEntry->AttrOrContent;

       switch (translation)
	 {
	 case 'C':	/* Content */
	   child = PutInContent (attrValue);
	   if (child != NULL)
	       TtaAppendTextContent (child, TEXT("\" "), theDocument);
	   break;

	 case 'A':
	   if (lastAttribute != NULL)
	     {
	       TtaGiveAttributeType (lastAttribute, &attrType, &attrKind);
	       switch (attrKind)
		 {
		 case 0:	/* enumerate */
		   val = MapAttrValue (lastAttrEntry->ThotAttribute, attrValue);
		   if (val < 0)
		     {
		       TtaGiveAttributeType (lastAttribute, &attrType, &attrKind);
		       attrName = TtaGetAttributeName (attrType);
		       if (ustrlen (attrValue) > MaxMsgLength - 30)
			   inputBuffer[MaxMsgLength - 30] = WC_EOS;
		       usprintf (msgBuffer,
				 TEXT("Unknown attribute value \"%s = %s\""),
				 attrName, attrValue);
		       ParseHTMLError (theDocument, msgBuffer);
		       /* remove the attribute and replace it by an */
		       /* Invalid_attribute */
		       TtaRemoveAttribute (lastAttrElement,
					   lastAttribute, theDocument);
		       attrType.AttrSSchema = DocumentSSchema;
		       attrType.AttrTypeNum = pHTMLAttributeMapping[0].ThotAttribute;
		       usprintf (msgBuffer, TEXT("%s=%s"), attrName, attrValue);
		       CreateAttr (lastAttrElement, attrType, msgBuffer, TRUE);
		     }
		   else
		       TtaSetAttributeValue (lastAttribute, val,
					     lastAttrElement, theDocument);
		   break;
		 case 1:	/* integer */
		   if (attrType.AttrTypeNum == HTML_ATTR_Border &&
		       !ustrcasecmp (inputBuffer, TEXT("border")))
		     {
		       /* border="border" for a table */
		       val = 1;
		       TtaSetAttributeValue (lastAttribute, val,
					     lastAttrElement, theDocument);
		     }
		   else
		     if (usscanf (inputBuffer, TEXT("%d"), &val))
		       TtaSetAttributeValue (lastAttribute, val,
					     lastAttrElement, theDocument);
		     else
		       {
			 TtaRemoveAttribute (lastAttrElement,
					     lastAttribute, theDocument);
			 usprintf (msgBuffer,
				   TEXT("Invalid attribute value \"%s\""),
				   attrValue);
			 ParseHTMLError (theDocument, msgBuffer);
		       }
		   break;
		 case 2:	/* text */
		   if (!UnknownAttr)
		     {
		       TtaSetAttributeText (lastAttribute, attrValue,
					    lastAttrElement, theDocument);
		       if (attrType.AttrTypeNum == HTML_ATTR_Langue)
			 {
			   /* it's a LANG attribute value */
			   lang = TtaGetLanguageIdFromName (attrValue);
			   if (lang == 0)
			     {
			       usprintf (msgBuffer,
					 TEXT("Unknown language: %s"),
					 attrValue);
			       ParseHTMLError (theDocument, msgBuffer);
			     }
			   else
			     {
			       /* change current language */
			       currentLanguage = lang;
			       LanguageStack[StackLevel - 1] = currentLanguage;
			     }
			   if (!TtaGetParent (lastAttrElement))
			     /* it's a LANG attribute on the root element */
			     /* set the RealLang attribute */
			     {
			       attrType1.AttrSSchema = DocumentSSchema;
			       attrType1.AttrTypeNum = HTML_ATTR_RealLang;
			       attr = TtaNewAttribute (attrType1);
			       TtaAttachAttribute (lastAttrElement,
						   attr, theDocument);
			       TtaSetAttributeValue (attr,
						     HTML_ATTR_RealLang_VAL_Yes_,
						     lastAttrElement, theDocument);
			     }
			 }
		     }
		   else
		     /* this is the content of an invalid attribute */
		     /* append it to the current Invalid_attribute */
		     {
		       length = ustrlen (attrValue) + 2;
		       length += TtaGetTextAttributeLength (lastAttribute);
		       buffer = TtaAllocString (length + 1);
		       TtaGiveTextAttributeValue (lastAttribute,
						  buffer, &length);
		       ustrcat (buffer, TEXT("="));
		       ustrcat (buffer, attrValue);
		       TtaSetAttributeText (lastAttribute, buffer,
					    lastAttrElement, theDocument);
		       TtaFreeMemory (buffer);
		     }
		   break;
		 case 3:	/* reference */
		   break;
		 }
	     }
	   break;
	   
	 case SPACE:
	   TypeAttrValue (attrValue);
	   break;

	 default:
	   break;
	 }

       if (lastAttrEntry->ThotAttribute == HTML_ATTR_Width__)
	 /* HTML attribute "width" for a table or a hr */
	 /* create the corresponding attribute IntWidthPercent or */
	 /* IntWidthPxl */
	 CreateAttrWidthPercentPxl (attrValue,
				    lastAttrElement, theDocument, -1);
       else
	 if (!ustrcmp (lastAttrEntry->XMLattribute, TEXT("size")))
	   {
	     TtaGiveAttributeType (lastAttribute, &attrType, &attrKind);
	     if (attrType.AttrTypeNum == HTML_ATTR_Font_size)
	         CreateAttrIntSize (inputBuffer,
				    lastAttrElement, theDocument);
	   }
	 else
	   if (!ustrcmp (lastAttrEntry->XMLattribute, TEXT("shape")))
	     {
	       child = TtaGetFirstChild (lastAttrElement);
	       if (child != NULL)
		 {
		   switch (val)
		     {
		     case HTML_ATTR_shape_VAL_rectangle:
		       shape = 'R';
		       break;
		     case HTML_ATTR_shape_VAL_circle:
		       shape = 'a';
		       break;
		     case HTML_ATTR_shape_VAL_polygon:
		       shape = 'p';
		       break;
		     default:
		       shape = SPACE;
		       break;
		     }
		   TtaSetGraphicsShape (child, shape, theDocument);
		 }
	     }
	   else
	     if (!ustrcmp (lastAttrEntry->XMLattribute, TEXT("value")))
	       {
		 elType = TtaGetElementType (lastAttrElement);
		 if (elType.ElTypeNum == HTML_EL_Text_Input ||
		     elType.ElTypeNum == HTML_EL_Password_Input ||
		     elType.ElTypeNum == HTML_EL_File_Input ||
		     elType.ElTypeNum == HTML_EL_Input)
		   /* create a Default_Value attribute with the same content */
		   {
		     attrType1.AttrSSchema = attrType.AttrSSchema;
		     attrType1.AttrTypeNum = HTML_ATTR_Default_Value;
		     attr = TtaNewAttribute (attrType1);
		     TtaAttachAttribute (lastAttrElement, attr, theDocument);
		     TtaSetAttributeText (attr, attrValue,
					  lastAttrElement, theDocument);
		   }
	       }
#ifndef STANDALONE
       /* Some HTML attributes are equivalent to a CSS property:      */
       /*      background     ->                   background         */
       /*      bgcolor        ->                   background         */
       /*      text           ->                   color              */
       /*      color          ->                   color              */
	     else
	       if (!ustrcmp (lastAttrEntry->XMLattribute, TEXT("background")))
		 {
		   if (ustrlen (attrValue) > MaxMsgLength - 30)
		       inputBuffer[MaxMsgLength - 30] = WC_EOS;
		   usprintf (msgBuffer,
			     TEXT("background: url(%s)"), inputBuffer);
		   ParseHTMLSpecificStyle (lastElement, msgBuffer,
					   theDocument, FALSE);
		 }
	       else
		 if (!ustrcmp (lastAttrEntry->XMLattribute, TEXT("bgcolor")))
		     HTMLSetBackgroundColor (theDocument,
					     lastElement, attrValue);
		 else
		   if (!ustrcmp (lastAttrEntry->XMLattribute, TEXT("text")) ||
		       !ustrcmp (lastAttrEntry->XMLattribute, TEXT("color")))
		     HTMLSetForegroundColor (theDocument,
					     lastElement, attrValue);
#endif /* !STANDALONE */
     }
}


/*----------------------------------------------------------------------
   StartOfEntity   A character '&' has been encountered in the text.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         StartOfEntity (CHAR_T c)
#else
static void         StartOfEntity (c)
CHAR_T                c;

#endif
{
   LgEntityName = 0;
   EntityTableEntry = 0;
   CharRank = 0;
}


/*----------------------------------------------------------------------
   PutNonISOlatin1Char     put a Unicode character in the input buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutNonISOlatin1Char (int code, STRING prefix)
#else
static void         PutNonISOlatin1Char (code, prefix)
int                 code;
STRING              prefix;
#endif
{
   Language	 lang, l;
   ElementType	 elType;
   Element	 elText;
   AttributeType attrType;
   Attribute	 attr;
   CHAR_T	 buffer[MaxEntityLength+10];

   if (ReadingAnAttrValue)
     /* this entity belongs to an attribute value */
     {
       /* Thot can't mix different languages in the same attribute value */
       /* just discard that character */
       ;
     }
   else
     /* this entity belongs to the element contents */
     {
       /* put the current content of the input buffer into the document */
       TextToDocument ();
       MergeText = FALSE;
       /* create a new text leaf */
       elType.ElSSchema = DocumentSSchema;
       elType.ElTypeNum = HTML_EL_TEXT_UNIT;
       elText = TtaNewElement (theDocument, elType);
       TtaSetElementLineNumber (elText, NumberOfLinesRead);
       InsertElement (&elText);
       lastElementClosed = TRUE;
       /* try to find a fallback character */
       l = currentLanguage;
       GetFallbackCharacter (code, buffer, &lang);
       /* put that fallback character in the new text leaf */
       TtaSetTextContent (elText, buffer, lang, theDocument);
       currentLanguage = l;
       /* make that text leaf read-only */
       TtaSetAccessRight (elText, ReadOnly, theDocument);
       /* associate an attribute EntityName with the new text leaf */
       attrType.AttrSSchema = DocumentSSchema;
       attrType.AttrTypeNum = HTML_ATTR_EntityName;
       attr = TtaNewAttribute (attrType);
       TtaAttachAttribute (elText, attr, theDocument);
       buffer[0] = '&';
       ustrcpy (&buffer[1], prefix);
       ustrcat (buffer, EntityName);
       ustrcat (buffer, TEXT(";"));
       TtaSetAttributeText (attr, buffer, elText, theDocument);
       MergeText = FALSE;
     }
}

/*----------------------------------------------------------------------
   PutAmpersandInDoc
   Put an '&' character in the document tree with an attribute
   IntEntity.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutAmpersandInDoc ()
#else
static void         PutAmpersandInDoc ()
#endif
{
   ElementType         elType;
   Element             elText;
   AttributeType       attrType;
   Attribute           attr;

   TextToDocument ();
   /* create a TEXT element for '&'*/
   elType.ElSSchema = DocumentSSchema;
   elType.ElTypeNum = HTML_EL_TEXT_UNIT;
   elText = TtaNewElement (theDocument, elType);
   TtaSetElementLineNumber (elText, NumberOfLinesRead);
   InsertElement (&elText);
   lastElementClosed = TRUE;
   MergeText = FALSE;
   TtaSetTextContent (elText, TEXT("&"), currentLanguage, theDocument);
   attrType.AttrSSchema = DocumentSSchema;
   attrType.AttrTypeNum = HTML_ATTR_IntEntity;
   attr = TtaNewAttribute (attrType);
   TtaAttachAttribute (elText, attr, theDocument);
   TtaSetAttributeValue (attr, HTML_ATTR_IntEntity_VAL_Yes_, elText, theDocument);
}

/*----------------------------------------------------------------------
   EndOfEntity     End of a HTML entity. Search that entity in the
   entity table and put the corresponding character in the input buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfEntity (CHAR_T c)
#else
static void         EndOfEntity (c)
CHAR_T                c;

#endif
{
   int                 i;
   CHAR_T              msgBuffer[MaxMsgLength];

   EntityName[LgEntityName] = WC_EOS;
   if (CharEntityTable2[EntityTableEntry].charName[CharRank] == EOS)
      /* the entity read matches the current entry of entity table */
      if (CharEntityTable2[EntityTableEntry].charCode > 255)
	 PutNonISOlatin1Char (CharEntityTable2[EntityTableEntry].charCode, TEXT(""));
      else
	 PutInBuffer ((CHAR_T)CharEntityTable2[EntityTableEntry].charCode); 
   else
      /* entity not in the table. Print an error message */
     {
       if (ReadingAnAttrValue)
	  PutInBuffer ('&');
       else
          PutAmpersandInDoc ();
       for (i = 0; i < LgEntityName; i++)
	  PutInBuffer (EntityName[i]);
       PutInBuffer (';');
       /* print an error message */
       usprintf (msgBuffer, TEXT("Invalid entity \"&%s;\""), EntityName);
       ParseHTMLError (theDocument, msgBuffer);
     }
   LgEntityName = 0;
}

/*----------------------------------------------------------------------
   EntityChar      A character belonging to a HTML entity has been
   read.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EntityChar (unsigned char c)
#else
static void         EntityChar (c)
unsigned char       c;

#endif
{
   int                 i;
   CHAR_T              msgBuffer[MaxMsgLength];
   ThotBool	       OK, done, stop;

   done = FALSE;
   if (CharEntityTable2[EntityTableEntry].charName[CharRank] == EOS)
      /* the entity name read so far matches the current entry of */
      /* entity table */
      /* does it also match the next entry? */
     {
     OK = FALSE;
     i = EntityTableEntry+1;
     stop = FALSE;
     do
	{
	if (ustrncmp (EntityName, CharEntityTable2[i].charName, LgEntityName) != 0)
	   stop = TRUE;
	else
	   if (CharEntityTable2[i].charName[CharRank] < c)
	      i++;
	   else
	      {
	      stop = TRUE;
	      if (CharEntityTable2[i].charName[CharRank] == c)
		 OK = TRUE;
	      }
	}
     while (!stop);     
     if (!OK && !ReadingAnAttrValue)
        {
	/* If we are not reading an attribute value, assume that semicolon is
	   missing and put the corresponding char in the document content */
	EntityName[LgEntityName] = WC_EOS;
	if (CharEntityTable2[EntityTableEntry].charCode > 255)
	   PutNonISOlatin1Char (CharEntityTable2[EntityTableEntry].charCode, TEXT (""));
	else
	   PutInBuffer ((char)(CharEntityTable2[EntityTableEntry].charCode));
	if (c != SPACE)
	   /* print an error message */
	   ParseHTMLError (theDocument, TEXT("Missing semicolon"));
	/* end of entity */
	LgEntityName = 0;
	done = TRUE;
        }
     }
   if (!done)
     {
	while (CharEntityTable2[EntityTableEntry].charName[CharRank] < c
	       && CharEntityTable2[EntityTableEntry].charCode != 0)
	   EntityTableEntry++;
	if (CharEntityTable2[EntityTableEntry].charName[CharRank] != c)
	  OK = FALSE;
	else
	  if (LgEntityName > 0 &&
	      ustrncmp (EntityName,
			CharEntityTable2[EntityTableEntry].charName,
			LgEntityName) != 0)
	     OK = FALSE;
	  else
	     {
	       OK = TRUE;
	       CharRank++;
	       if (LgEntityName < MaxEntityLength - 1)
		  EntityName[LgEntityName++] = c;
	     }
	if (!OK)
	  {
	     /* the entity name read so far is not in the table */
	     /* invalid entity */
	     /* put the entity name in the buffer */
	     if (ReadingAnAttrValue)
	        PutInBuffer ('&');
	     else
	        PutAmpersandInDoc ();
	     for (i = 0; i < LgEntityName; i++)
		PutInBuffer (EntityName[i]);
	     /* print an error message only if it's not the first character
		after '&' or if it is a letter */
	     if (LgEntityName > 0 ||
		 ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))
		{
	        /* print an error message */
	        EntityName[LgEntityName++] = c;
	        EntityName[LgEntityName++] = WC_EOS;
	        usprintf (msgBuffer, TEXT("Invalid entity \"&%s\""), EntityName);
	        ParseHTMLError (theDocument, msgBuffer);
		}
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
#ifdef __STDC__
static void         EndOfDecEntity (CHAR_T c)
#else
static void         EndOfDecEntity (c)
CHAR_T                c;

#endif
{
   int                 code;

   EntityName[LgEntityName] = WC_EOS;
   usscanf (EntityName, TEXT("%d"), &code);
   if (code > 255)
      PutNonISOlatin1Char (code, TEXT ("#"));
   else
      PutInBuffer ((char)code);
   LgEntityName = 0;
}

/*----------------------------------------------------------------------
   DecEntityChar   A character belonging to a decimal entity has been read.
   Put that character in the entity buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DecEntityChar (char c)
#else
static void         DecEntityChar (c)
char                c;

#endif
{
   int		i;

   if (LgEntityName < MaxEntityLength - 1)
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
	    ParseHTMLError (theDocument, TEXT("Invalid decimal entity"));
	    }
	 }
}

/*----------------------------------------------------------------------
   EndOfHexEntity  End of an hexadecimal entity. Convert the
   string read into a number and put the character
   having that code in the input buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfHexEntity (CHAR_T c)
#else
static void         EndOfHexEntity (c)
CHAR_T                c;

#endif
{
   int                 code;

   EntityName[LgEntityName] = WC_EOS;
   usscanf (EntityName, TEXT("%x"), &code);
   if (code > 255)
      PutNonISOlatin1Char (code, TEXT ("#x"));
   else
      PutInBuffer ((char) code);
   LgEntityName = 0;
}

/*----------------------------------------------------------------------
   HexEntityChar   A character belonging to an hexadecimal entity has been
   read. Put that character in the entity buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         HexEntityChar (char c)
#else
static void         HexEntityChar (c)
char                c;

#endif
{
   int		i;

   if (LgEntityName < MaxEntityLength - 1)
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
	    ParseHTMLError (theDocument, TEXT("Invalid hexadecimal entity"));
	    }
	 }
}


/*----------------------------------------------------------------------
   PutLess put '<' in the input buffer
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutLess (CHAR_T c)
#else
static void         PutLess (c)
CHAR_T                c;

#endif
{
   PutInBuffer ('<');
}

/*----------------------------------------------------------------------
   PutAmpersandSpace       put '& ' in the input buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutAmpersandSpace (CHAR_T c)
#else
static void         PutAmpersandSpace (c)
CHAR_T                c;

#endif
{
   PutInBuffer ('&');
   PutInBuffer (SPACE);
}

/*----------------------------------------------------------------------
   PutLessAndSpace put '<' and the space read in the input buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutLessAndSpace (char c)
#else
static void         PutLessAndSpace (c)
char                c;

#endif
{
   PutInBuffer ('<');
   PutInBuffer (c);
}


/*----------------------------------------------------------------------
   PutInComment    put character c in the current HTML comment.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutInComment (UCHAR_T c)
#else
static void         PutInComment (c)
UCHAR_T             c;

#endif
{
   ElementType         elType;
   Element             elCommentLine, prevElCommentLine;

   if (c != WC_EOS)
      if (!ParsingCSS && ((int) c == WC_EOL || (int) c == WC_CR))
	 /* new line in a comment */
	{
	   /* put the content of the inputBuffer into the current */
	   /* Comment_line element */
	   CloseBuffer ();
	   TtaAppendTextContent (CommentText, inputBuffer, theDocument);
	   InitBuffer ();
	   /* create a new Comment_line element */
	   elType.ElSSchema = DocumentSSchema;
	   elType.ElTypeNum = HTML_EL_Comment_line;
	   elCommentLine = TtaNewElement (theDocument, elType);
	   TtaSetElementLineNumber (elCommentLine, NumberOfLinesRead);
	   /* inserts the new Comment_line element after the previous one */
	   prevElCommentLine = TtaGetParent (CommentText);
	   TtaInsertSibling (elCommentLine, prevElCommentLine, FALSE, theDocument);
	   /* create a TEXT element as the first child of the new element
	      Comment_line */
	   elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	   CommentText = TtaNewElement (theDocument, elType);
	   TtaSetElementLineNumber (CommentText, NumberOfLinesRead);
	   TtaInsertFirstChild (&CommentText, elCommentLine, theDocument);
	   TtaSetTextContent (CommentText, TEXT(""), currentLanguage, theDocument);
	}
      else
	{
	   if (LgBuffer >= MaxBufferLength - 1)
	     {
		CloseBuffer ();
		TtaAppendTextContent (CommentText, inputBuffer, theDocument);
		InitBuffer ();
	     }
	   inputBuffer[LgBuffer++] = c;
	}
}


/*----------------------------------------------------------------------
   EndOfDoctypeDecl	A Doctype declaration has been read
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfDoctypeDecl (char c)
#else
static void         EndOfDoctypeDecl (c)
char                c;
 
#endif
{
   int		i;

   CloseBuffer ();
   /* process the Doctype declaration available in inputBuffer */
   if (!ustrcasecmp (inputBuffer, TEXT("DOCTYPE")))
      {
      for (i = 7; inputBuffer[i] <= WC_SPACE && inputBuffer[i] != WC_EOS; i++);
      if (!ustrcasecmp (&inputBuffer[i], TEXT("HTML")))
	 /* it's a HTML document */
	 {
         /***** TO DO *****/;
	 }
      }
   InitBuffer ();
}


#ifdef EXPAT
/* Handlers associated with Amaya */
/*----------------------------------------------------------------------
   Hndl_NotStandalone
   Handler that is called if the document is not "standalone".
   This happens when there is an external subset or a reference
   to a parameter entity, but does not have standalone set to "yes" 
   in an XML declaration.
   If this handler returns 0, then the parser will throw an
   XML_ERROR_NOT_STANDALONE error.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int     Hndl_NotStandalone (void *userData)
#else  /* __STDC__ */
static int     Hndl_NotStandalone (userData)
void            *userData; 
#endif  /* __STDC__ */

{
  printf ("\n Hndl_NotStandalone\n");
}


/*----------------------------------------------------------------------
   Hndl_Notation
   Handler that receives notation declarations.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_Notation (void *userData,
			       const XML_Char *notationName,
			       const XML_Char *base,
			       const XML_Char *systemId,
			       const XML_Char *publicId)
#else  /* __STDC__ */
static void     Hndl_Notation (userData,
			       notationName,
			       base,
			       systemId,
			       publicId)
void            *userData; 
const XML_Char  *notationName;
const XML_Char  *base;
const XML_Char  *systemId;
const XML_Char  *publicId;
#endif  /* __STDC__ */

{
  printf ("\n Hndl_Notation\n");
}


/*----------------------------------------------------------------------
   Hndl_UnparsedEntity
   Handler that receives declarations of unparsed entities.
   These are entity declarations that have a notation (NDATA) field:
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_UnparsedEntity (void *userData,
				     const XML_Char *entityName,
				     const XML_Char *base,
				     const XML_Char *systemId,
				     const XML_Char *publicId,
				     const XML_Char *notationName)
#else  /* __STDC__ */
static void     Hndl_UnparsedEntity (userData,
				     entityName,
				     base,
				     systemId,
				     publicId,
				     notationName)
void            *userData; 
const XML_Char  *entityName;
const XML_Char  *base;
const XML_Char  *systemId;
const XML_Char  *publicId;
const XML_Char  *notationName;
#endif  /* __STDC__ */

{
  printf ("\n Hndl_UnparsedEntity\n");
}


/*----------------------------------------------------------------------
   Hndl_EndNameSpace
   Handler for the end of namespace declarations
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_EndNameSpace (void *userData,
				   const XML_Char *prefix)
#else  /* __STDC__ */
static void     Hndl_EndNameSpace (userData,
				   prefix)
void            *userData; 
const XML_Char  *prefix;
#endif  /* __STDC__ */

{
  printf ("\n Hndl_EndNameSpace\n");
}

/*----------------------------------------------------------------------
   Hndl_StartNameSpace
   Handler for the start of namespace declarations
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_StartNameSpace (void *userData,
				     const XML_Char *prefix,
				     const XML_Char *uri)
#else  /* __STDC__ */
static void     Hndl_StartNameSpace (userData,
				     prefix,
				     uri)
void            *userData; 
const XML_Char  *prefix;
const XML_Char  *uri;
#endif  /* __STDC__ */

{
  printf ("\n Hndl_StartNameSpace\n");
}


/*----------------------------------------------------------------------
   Hndl_UnknownEncoding
   Handler to deal with encodings other than the built in
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int     Hndl_UnknownEncoding (void *encodingData,
				     const XML_Char *name,
				     XML_Encoding *info)
#else  /* __STDC__ */
static int     Hndl_UnknownEncoding (userData,
				     data,
				     length)
void            *encodingData; 
const XML_Char  *name;
XML_Encoding    *info
#endif  /* __STDC__ */

{
  printf ("\n Hndl_UnknownEncoding\n");
}


/*----------------------------------------------------------------------
   Hndl_ExternalEntityRef
   Handler for external entity references.
   his handler is also called for processing an external DTD subset.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int     Hndl_ExternalEntityRef (void *userData,
					const XML_Char *context,
					const XML_Char *base,
					const XML_Char *systemId,
					const XML_Char *publicId)
#else  /* __STDC__ */
static int     Hndl_ExternalEntityRef (userData,
					context,
					base,
					systemId,
					publicId)
void            *userData; 
const XML_Char  *context;
const XML_Char  *base;
const XML_Char  *systemId;
const XML_Char  *publicId;
#endif  /* __STDC__ */

{
  printf ("\n Hndl_ExternalEntityRef\n");
}


/*----------------------------------------------------------------------
   Hndl_DefaultExpand
   Default handler with expansion of internal entity references
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_DefaultExpand (void *userData,
				    const XML_Char *data,
				    int length)
#else  /* __STDC__ */
static void     Hndl_DefaultExpand (userData,
				    data,
				    length)
void            *userData; 
const XML_Char  *data;
int              length;
#endif  /* __STDC__ */

{
  int  i;

  printf ("\n Hndl_DefaultExpand");

  for (i=0; i<length; i++)
    {
      printf ("%c", data[i]);
    }
  printf ("\n");
}


/*----------------------------------------------------------------------
   Hndl_Default
   Handler for any characters in the document which wouldn't
   otherwise be handled.
   This includes both data for which no handlers can be set
   (like some kinds of DTD declarations) and data which could be
   reported but which currently has no handler set.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_Default (void *userData,
			      const XML_Char *data,
			      int length)
#else  /* __STDC__ */
static void     Hndl_Default (userData,
			      data,
			      length)
void            *userData; 
const XML_Char  *data;
int              length;
#endif  /* __STDC__ */

{
  int  i;

  printf ("\n Hndl_Default\n");

  for (i=0; i<length; i++)
    {
      printf ("%c", data[i]);
    }
  printf ("\n");
}


/*----------------------------------------------------------------------
   Hndl_StartCdata
   Handlers that get called at the beginning of a CDATA section
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_StartCdata (void *userData)
#else  /* __STDC__ */
static void     Hndl_StartCdata (userData)
void            *userData; 
#endif  /* __STDC__ */

{
  printf ("\n Hndl_StartCdata\n");
}


/*----------------------------------------------------------------------
   Hndl_EndCdata
   Handlers that get called at the end of a CDATA section
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_EndCdata (void *userData)
#else  /* __STDC__ */
static void     Hndl_EndCdata (userData)
void            *userData; 
#endif  /* __STDC__ */

{
  printf ("\n Hndl_EndCdata\n");
}


/*----------------------------------------------------------------------
   Hndl_PI
   Handler for processing instructions.
   The target is the first word in the processing instruction.
   The pidata is the rest of the characters in it after skipping
   all whitespace after the initial word.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_PI (void *userData,
			 const XML_Char *target,
			 const XML_Char *pidata)
#else  /* __STDC__ */
static void     Hndl_PI (userData,
			 target,
			 pidata)
void            *userData; 
const XML_Char  *target;
const XML_Char  *pidata;
#endif  /* __STDC__ */

{
  printf ("\n Hndl_PI %s\n", pidata);
  /* No treatment in Amaya for PI */
}


/*----------------------------------------------------------------------
   Hndl_CharacterData
   Handler for the text
   The string the handler receives is NOT zero terminated.
   We have to use the length argument to deal with the end of the string.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_CharacterData (void *userData,
				    const XML_Char *data,
				    int length)
#else  /* __STDC__ */
static void     Hndl_CharacterData (userData,
				    data,
				    length)
void            *userData; 
const XML_Char  *data;
int              length;
#endif  /* __STDC__ */

{
   ElementType   elType;
   Element       elText, parent, ancestor, prev;
   int           i;
   ThotBool      ignoreLeadingSpaces;
   STRING        buffer;
   STRING        bufferwc;

   /* ----- */
   printf ("\n Hndl_CharacterData avant transformation \n");
   for (i=0; i<length; i++)
     {
       printf ("%c", data[i]);
     }
   /* ----- */

   buffer = TtaAllocString (length + 1);

   for (i = 0; i < length; i++)
        buffer[i] = data[i];
   buffer[length] = WC_EOS;


   bufferwc = TtaAllocString ((2*length) + 1);
   TtaMBS2WCS (&buffer, &bufferwc, UTF_8);
   printf ("\n Hndl_CharacterData apres transformation\n %s \n", bufferwc);

   if (lastElement != NULL)
     {
	i = 0;
	if (InsertSibling ())
	   /* There is a previous sibling (lastElement) 
	      for the new Text element */
	  {
	     parent = TtaGetParent (lastElement);
	     if (parent == NULL)
		 parent = lastElement;
	     elType = TtaGetElementType (parent);
	     if (IsCharacterLevelElement (lastElement) &&
		 elType.ElTypeNum != HTML_EL_Option_Menu &&
		 elType.ElTypeNum != HTML_EL_OptGroup)
	       {
		 ignoreLeadingSpaces = FALSE;
		 elType = TtaGetElementType (lastElement);
		 if (elType.ElTypeNum == HTML_EL_BR)
		     ignoreLeadingSpaces = TRUE;
	       }
	     else
	         ignoreLeadingSpaces = TRUE;
	  }
	else
	   /* the new Text element should be the first child of the latest
	      element encountered */
	  {
	    parent = lastElement;
	    ignoreLeadingSpaces = TRUE;
	    elType = TtaGetElementType (lastElement);
	    if (elType.ElTypeNum != HTML_EL_Option_Menu &&
		elType.ElTypeNum != HTML_EL_OptGroup)
	      {
	        ancestor = parent;
	        while (ignoreLeadingSpaces &&
		       IsCharacterLevelElement (ancestor))
		  {
		    prev = ancestor;
		    TtaPreviousSibling (&prev);
		    if (prev == NULL)
		        ancestor = TtaGetParent (ancestor);
		    else
		        ignoreLeadingSpaces = FALSE;
		  }
	      }
	  }

	if (ignoreLeadingSpaces)
	    if (!Within (HTML_EL_Preformatted, DocumentSSchema) &&
		!Within (HTML_EL_STYLE_, DocumentSSchema) &&
		!Within (HTML_EL_SCRIPT, DocumentSSchema))
	        /* suppress leading spaces */
	        while (bufferwc[i] <= WC_SPACE && bufferwc[i] != WC_EOS)
		  i++;

	if (bufferwc[i] != WC_EOS)
	  {
	    elType = TtaGetElementType (lastElement);
	    if (elType.ElTypeNum == HTML_EL_TEXT_UNIT && MergeText)
	      {
		TtaAppendTextContent (lastElement,
				      &(bufferwc[i]),
				      theDocument);
	      }
	    else
	      {
		/* create a TEXT element */
		elType.ElSSchema = DocumentSSchema;
		elType.ElTypeNum = HTML_EL_TEXT_UNIT;
		elText = TtaNewElement (theDocument, elType);
		TtaSetElementLineNumber (elText, BufferLineNumber);
		InsertElement (&elText);
		lastElementClosed = TRUE;
		MergeText = TRUE;
		/* put the content of the input buffer into the TEXT element */
		if (elText != NULL)
		  {
		    TtaSetTextContent (elText,
				       &(bufferwc[i]),
				       currentLanguage,
				       theDocument);
		  }
	      }
	  }
     }
  TtaFreeMemory (buffer);
  TtaFreeMemory (bufferwc);
}


/*----------------------------------------------------------------------
   Hndl_Comment
   Handler for comments
   The data is all text inside the comment delimiters
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_Comment (void *userData,
			      const XML_Char *data)
#else  /* __STDC__ */
static void     Hndl_Comment (userData,
			      data)
void            *userData; 
const XML_Char  *data;
#endif  /* __STDC__ */

{
   ElementType   elType;
   Element       elComment, elCommentLine, child, lastChild;


   printf ("\n Hndl_Comment %s\n", data);

   /* ATTENTION : problene des sauts de ligne a gerer */
   /* Voir PutInComment plus haut */

   /* create a Thot element Comment */
   elType.ElSSchema = DocumentSSchema;
   elType.ElTypeNum = HTML_EL_Comment_;
   elComment = TtaNewElement (theDocument, elType);
   TtaSetElementLineNumber (elComment, NumberOfLinesRead);

   /* Faut-il garder ce test avec EXPAT ? */
   if (lastElementClosed && (lastElement == rootElement))
      /* a comment after the tag </html> */
      /* insert it as the last child of the root element */
     {
	child = TtaGetFirstChild (rootElement);
	lastChild = NULL;
	while (child != NULL)
	  {
	     lastChild = child;
	     TtaNextSibling (&child);
	  }
	TtaInsertSibling (elComment, lastChild, FALSE, theDocument);
     }
   else
     {
       InsertElement (&elComment);
     }
   lastElementClosed = TRUE;

   /* create a Comment_line element as the first child */
   /* of element Comment */
   if (elComment != NULL)
     {
	elType.ElTypeNum = HTML_EL_Comment_line;
	elCommentLine = TtaNewElement (theDocument, elType);
	TtaSetElementLineNumber (elCommentLine, NumberOfLinesRead);
	TtaInsertFirstChild (&elCommentLine, elComment, theDocument);

	/* create a TEXT element as the first child of element Comment_line */
	elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	CommentText = TtaNewElement (theDocument, elType);
	TtaSetElementLineNumber (CommentText, NumberOfLinesRead);
	TtaInsertFirstChild (&CommentText, elCommentLine, theDocument);
	TtaSetTextContent (CommentText, (CHAR_T*) data,
			   currentLanguage, theDocument);
     }
}

/*----------------------------------------------------------------------
   Hndl_StartElement
   Handler for start tags
   Attributes are passed as a pointer to a vector of char pointers
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_StartElement (void *userData,
				   const XML_Char *name,
				   const XML_Char **attlist)
#else  /* __STDC__ */
static void     Hndl_StartElement (userData,
				   name,
				   attlist)
void            *userData; 
const XML_Char  *name;
const XML_Char **attlist;
#endif  /* __STDC__ */

{
   int           nbatts;
   CHAR_T       *buffer;

  printf ("\n Hndl_StartElement %s\n", name);

  if (ParsingTextArea)
    {
      /* We are parsing the contents of a TEXTAREA element. If a start
	 tag appears, consider it as plain text */
    }
  else
    {
      /* if the last character in the GI is a '/', ignore it.  This is to
         accept the XML syntax for empty elements, for instance <br/> */
      
      if (lastElementClosed && (lastElement == rootElement))
	/* an element after the tag </html>, ignore it */
	{
	  ParseHTMLError (theDocument, TEXT("Element after tag </html>. Ignored"));
	  return;
	}
      ProcessStartGI ((CHAR_T*) name);
    }

  /* Treatment of the attributes */
  nbatts = XML_GetSpecifiedAttributeCount (parser);

  while (*attlist != NULL)
    {
      /* creation of the attribut into the Thot tree */
      buffer = TtaGetMemory ((strlen (*attlist)) + 1);
      strcpy (buffer, *attlist);
      printf (" name : %s ", buffer);
      EndOfAttrName (buffer);
      TtaFreeMemory (buffer);

      /* Filling of the attribute value */
      attlist++;
      if (*attlist != NULL)
	{
	  buffer = TtaGetMemory ((strlen (*attlist)) + 1);
	  strcpy (buffer, *attlist);
	  printf (" value=%s \n", buffer);
	  EndOfAttrValue (buffer);
	  TtaFreeMemory (buffer);
	}
      attlist++;
    }

}

/*----------------------------------------------------------------------
   Hndl_EndElement
   Handler for end tags
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     Hndl_EndElement (void *userData,
				 const XML_Char *name)
#else  /* __STDC__ */
static void     Hndl_EndElement (userData,
				 name)
void            *userData; 
const XML_Char  *name
#endif  /* __STDC__ */

{
   SSchema	schema;
   CHAR_T       msgBuffer[MaxMsgLength];
   int          entry;
   int          i;
   ThotBool     ok;


  printf ("\n Hndl_EndElement %s\n", name);

   if (ParsingTextArea)
     {
       if (ustrcasecmp (inputBuffer, TEXT("textarea")) != 0)
         /* We are parsing the contents of a textarea element. The end
	    tag is not the one closing the current textarea, consider it
	    as plain text */
	 {
	   return;
         }
     }

   /* is it the end of the current HTML fragment ? */
   ok = FALSE;
   if (HTMLrootClosingTag != EOS)
     {
       /* look for a colon in the element name (namespaces) and ignore the
	  prefix if there is one */
       for (i = 0; i < LgBuffer && inputBuffer[i] != TEXT(':'); i++);
       if (inputBuffer[i] == TEXT(':'))
	 i++;
       else
	 i = 0;
       if (ustrcasecmp (&inputBuffer[i], HTMLrootClosingTag) == 0)
	 {
	   HTMLrootClosed = TRUE;
	   ok = TRUE;
	 }
     }
   
   if (!ok)
     {
       /* search the HTML tag in the mapping table */
       schema = DocumentSSchema;
       entry = MapGI ((CHAR_T*) name, &schema, theDocument);
       if (entry < 0)
	 {
	   if (ustrlen (inputBuffer) > MaxMsgLength - 20)
	       inputBuffer[MaxMsgLength - 20] = WC_EOS;
	   usprintf (msgBuffer, TEXT("Unknown tag </%s>"), (CHAR_T*) name);
	   ParseHTMLError (theDocument, msgBuffer);
	 }
       else
	 {
	   if (!CloseElement (entry, -1, FALSE))
	     /* the end tag does not close any current element */
	     {
	       usprintf (msgBuffer,
			 TEXT("Unexpected end tag </%s>"),
			 (CHAR_T*) name);
	       ParseHTMLError (theDocument, msgBuffer);
	     }
	 }
     }
}

#endif /* EXPAT */


/*----------------------------------------------------------------------
   FreeHTMLParser
   Frees all ressources associated with the HTML parser.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeXHTMLParser (void)
#else
void                FreeXHTMLParser ()
#endif
{
   PtrClosedElement    pClose, nextClose;
   int		       entry;


   /* free descriptors of elements closed by a start tag */
   for (entry = 0; pHTMLGIMapping[entry].htmlGI[0] != WC_EOS; entry++)
      {
	pClose = pHTMLGIMapping[entry].firstClosedElem;
	while (pClose != NULL)
	  {
	    nextClose = pClose->nextClosedElem;
	    TtaFreeMemory (pClose);
	    pClose = nextClose;
	  }
      }
}


/*----------------------------------------------------------------------
   HTMLparse       parse either the HTML file infile or the text
   buffer HTMLbuf and build the equivalent Thot
   abstract tree.
   One parameter should be NULL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void        XHTMLparse (XML_Parser *parser,
			       FILE *infile,
			       char *HTMLbuf)
#else
static void        XHTMLparse (parser,
			       infile,
			       HTMLbuf)
XML_Parser  *parser;
FILE        *infile;
char        *HTMLbuf;

#endif
{
   UCHAR_T      charRead; 
   ThotBool     match;
   ThotBool     endOfFile = FALSE;

#define	 COPY_BUFFER_SIZE	1024
  char          bufferRead[COPY_BUFFER_SIZE];
  int           res;


   if (HTMLbuf != NULL || infile != NULL)
     {
       InputText = HTMLbuf;
       endOfFile = FALSE;
     }

   HTMLrootClosed = FALSE;


  /* read the XML file */

  while (!endOfFile && !HTMLrootClosed)
    {
      res = gzread (infile, bufferRead, COPY_BUFFER_SIZE);

      if (res < COPY_BUFFER_SIZE)
	  endOfFile = TRUE;
      
      if (!XML_Parse (parser, bufferRead,
		      res, endOfFile))
	{
	  printf("Parse error at line %d:\n%s\n",
		 XML_GetCurrentLineNumber (parser),
		 XML_ErrorString (XML_GetErrorCode (parser)));
	}
    }
  

  HTMLrootClosingTag = NULL;
  HTMLrootClosed = FALSE;

  if (ErrFile)
    {
      fclose (ErrFile);
      ErrFile = (FILE*) 0;
    } 
}

#ifdef EXPAT
/*----------------------------------------------------------------------
   FreeExpatParser
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         FreeExpatParser ()
#else  /* __STDC__ */
static void         FreeExpatParser ()
#endif  /* __STDC__ */

{  
  XML_ParserFree (parser);
}


/*----------------------------------------------------------------------
   InitializeExpatParser
   Specific initialization for expat
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InitializeExpatParser ()
#else  /* __STDC__ */
static void         InitializeExpatParser ()
#endif  /* __STDC__ */

{  
  /* Construct a new parser */
  parser = XML_ParserCreate (NULL);
  /*
    XML_SetEncoding (parser, "iso-8859-1");
  */
  
  /* Define the user data pointer that gets passed to handlers */  
  /*
  XML_SetUserData (parser, (void*) doc);
  */
  
  /* Set handlers for start and end tags */
  XML_SetElementHandler (parser,
			 Hndl_StartElement,
			 Hndl_EndElement);
  
  /* Set a text handler */
  XML_SetCharacterDataHandler (parser,
			       Hndl_CharacterData);
  
  /* Set a handler for comments */
  XML_SetCommentHandler (parser,
			 Hndl_Comment);
  
  /* Set a handler for processing instructions */
  XML_SetProcessingInstructionHandler (parser,
				       Hndl_PI);
  
  /* Set handlers that get called at the beginning 
     and end of a CDATA section */
  XML_SetCdataSectionHandler (parser,
			      Hndl_StartCdata,
			      Hndl_EndCdata);
  
  /* Set default handler */
  XML_SetDefaultHandler (parser,
			 Hndl_Default);
  
  /* Set a default handler with expansion */
  /*  of internal entity references */
  /*
    XML_SetDefaultHandlerExpand (parser,
    Hndl_DefaultExpand);
  */
  
  /* Set an external entity reference handler */
  XML_SetExternalEntityRefHandler (parser,
				   Hndl_ExternalEntityRef);
  
  /* Set a handler to deal with encodings other than the built in */
  XML_SetUnknownEncodingHandler (parser,
				 Hndl_UnknownEncoding, 0);
  
  /* Set handlers for namespace declarations */
  XML_SetNamespaceDeclHandler (parser,
			       Hndl_StartNameSpace,
			       Hndl_EndNameSpace);
  
  /* Set a handler that receives declarations of unparsed entities */
  XML_SetUnparsedEntityDeclHandler (parser,
				    Hndl_UnparsedEntity);
  
  /* Set a handler for notation declarations */
  XML_SetNotationDeclHandler (parser,
			      Hndl_Notation);
  
  /* Set a handler for no 'standalone' document */
  XML_SetNotStandaloneHandler (parser,
			       Hndl_NotStandalone);
}
#endif /* EXPAT */


/*----------------------------------------------------------------------
   InitializeHTMLParser
   initializes variables and stack for parsing file
   the parser will insert the thot tree after or as a child
   of last elem, in the document doc.
   If last elem is NULL or doc=0, the parser doesn't initialize
   the stack
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InitializeXHTMLParser (Element  lastelem,
					   ThotBool isclosed,
					   Document doc)
#else  /* __STDC__ */
static void         InitializeXHTMLParser (lastelem,
					   isclosed,
					   doc)
Element             lastelem;
ThotBool            isclosed;
Document            doc;
#endif  /* __STDC__ */
{
   CHAR_T           tag[20];
   Element          elem;
   int              i;
   SSchema          schema;

   StackLevel = 1;
   currentLanguage = TtaGetDefaultLanguage ();

   if (lastelem != NULL && doc != 0)
     {
	/* initialize the stack with ancestors of lastelem */
	theDocument = doc;
	DocumentSSchema = TtaGetDocumentSSchema (theDocument);
	rootElement = TtaGetMainRoot (theDocument);
	if (isclosed)
	   elem = TtaGetParent (lastelem);
	else
	   elem = lastelem;

	while (elem != NULL && elem != rootElement)
	  {
	     ustrcpy (tag, GITagNameByType (TtaGetElementType (elem)));
	     if (ustrcmp (tag, TEXT("???")))
	       {
		  for (i = StackLevel; i > 0; i--)
		    {
		       GINumberStack[i + 1] = GINumberStack[i];
		       ElementStack[i + 1] = ElementStack[i];
		       LanguageStack[i + 1] = LanguageStack[i];
		       ThotLevel[i + 1] = ThotLevel[i] + 1;
		    }
		  schema = DocumentSSchema;
		  GINumberStack[1] = MapGI (tag, &schema, theDocument);
		  ElementStack[1] = elem;
		  ThotLevel[1] = 1;
		  LanguageStack[1] = currentLanguage;
		  StackLevel++;
	       }
	     elem = TtaGetParent (elem);
	  }
	lastElement = lastelem;
	lastElementClosed = isclosed;
     }
   else
     {
	lastElement = rootElement;
	lastElementClosed = FALSE;
     }

   NumberOfCharRead = 0;
   NumberOfLinesRead = 1;

   /* input file is supposed to be XHTML */
   GINumberStack[0] = -1;
   ElementStack[0] = rootElement;
   ThotLevel[0] = 1;
   lastElemEntry = 0;

   /* initialize input buffer */
   EmptyLine = TRUE;
   StartOfFile = TRUE;
   inputBuffer[0] = WC_EOS;
   LgBuffer = 0;
   lastAttribute = NULL;
   lastAttrElement = NULL;
   lastAttrEntry = NULL;
   UnknownAttr = FALSE;
   ReadingAnAttrValue = FALSE;
   LgEntityName = 0;
   EntityTableEntry = 0;
   CharRank = 0;
   MergeText = FALSE;
   AfterTagPRE = FALSE;
   ParsingCSS = FALSE;
   CurrentBufChar = 0;
}


#ifdef STANDALONE
/*----------------------------------------------------------------------
   main program
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 main (int argc, char **argv)
#else
int                 main (argc, argv)
int                 argc;
char              **argv;
#endif
{
  Element             el, oldel;
  Document            doc;
  FILE               *infile;
  STRING              pathURL = NULL;
  CHAR_T              htmlFileName[200];
  CHAR_T              pivotFileName[200];
  CHAR_T              documentDirectory[200];
  CHAR_T              documentName[200];
  int                 returnCode;
  ThotBool	      plainText;

  /* check the number of arguments in command line */
  returnCode = 0;
  if (argc != 3)
    /* command line is not OK */
    {
      fprintf (stderr, "Usage: html2thot html_file Thot_file\n");
      returnCode = 1;
    }
  else
    {
      TtaInitializeAppRegistry (argv[0]);
      /* get the input file name from the command line */
      argv++;
      ustrcpy (htmlFileName, *argv);
      /*  open the input file */
      infile = fopen (htmlFileName, "r");
      if (infile == 0)
	{
	  fprintf (stderr, "Cannot open %s\n", htmlFileName);
	  returnCode = 2;
	}
      else
	{
	  /* input file OK. Get the output file name from the command line */
	  argv++;
	  ustrcpy (pivotFileName, *argv);
	  /* the file to be parsed is supposed to be HTML */
	  plainText = FALSE;
	  /* initialize mapping table */
	  InitMapping ();
	  /* initialize the Thot toolkit */
	  TtaInitialize ("HTMLThot");
	  /* extract directory and file name from second argument */
	  TtaExtractName (pivotFileName, documentDirectory, documentName);
	  if (documentName[0] == EOS && !TtaCheckDirectory (documentDirectory))
	    {
	      ustrcpy (documentName, documentDirectory);
	      documentDirectory[0] = EOS;
	    }
	  TtaSetDocumentPath (documentDirectory);
	  docURL = htmlFileName;
	  /* create a Thot document of type HTML */
	  doc = TtaNewDocument (TEXT("HTML"), pivotFileName);
	  theDocument = doc;
	  if (doc == 0)
	    {
	      fprintf (stderr, "Cannot create file %s.PIV\n", pivotFileName);
	      returnCode = 3;
	    }
	  else
	    /* set the notification mode for the new document */
	    TtaSetNotificationMode (doc, 1);
	  stream = gzopen (pivotFileName, "r");
#else  /* STANDALONE */
/*----------------------------------------------------------------------
   StartXHTMLParser loads the file Directory/htmlFileName for
   displaying the document documentName.
   The parameter pathURL gives the original (local or
   distant) path or URL of the html document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void       StartXHTMLParser (Document doc,
			     CHAR_T*  htmlFileName,
			     CHAR_T*  documentName,
			     CHAR_T*  documentDirectory,
			     CHAR_T*  pathURL,
			     ThotBool plainText)
#else
void        StartXHTMLParser (doc,
			      htmlFileName,
			      documentName,
			      documentDirectory,
			      pathURL,
			      plainText)
Document    doc;
CHAR_T*     htmlFileName;
CHAR_T*     documentName;
CHAR_T*     documentDirectory;
CHAR_T*     pathURL;
ThotBool    plainText;
#endif

{
  Element         el, oldel;
  AttributeType   attrType;
  Attribute       attr;
  CHAR_T*         s;
  CHAR_T          tempname[MAX_LENGTH];
  CHAR_T          temppath[MAX_LENGTH];
  int             length;
  ThotBool        isHTML;
  char            www_file_name[MAX_LENGTH];

  theDocument = doc;
  FirstElemToBeChecked = NULL;
  LastElemToBeChecked = NULL;
  lastElement = NULL;
  lastElementClosed = FALSE;
  lastElemEntry = 0;
  lastAttribute = NULL;
  lastAttrElement = NULL;
  lastAttrEntry = NULL;
  UnknownAttr = FALSE;
  ReadingAnAttrValue = FALSE;
  CommentText = NULL;
  UnknownTag = FALSE;
  MergeText = FALSE;
  LgEntityName = 0;
  EntityTableEntry = 0;
  CharRank = 0;

  wc2iso_strcpy (www_file_name, htmlFileName);
  stream = gzopen (www_file_name, "r");


  if (stream != 0)
    {
      FileBuffer[0] = EOS;
      WithinTable = 0;
      if (documentName[0] == WC_EOS && !TtaCheckDirectory (documentDirectory))
	{
	  ustrcpy (documentName, documentDirectory);
	  documentDirectory[0] = WC_EOS;
	  s = TtaGetEnvString ("PWD");
	  /* set path on current directory */
	  if (s != NULL)
	    ustrcpy (documentDirectory, s);
	  else
	    documentDirectory[0] = WC_EOS;
	}
      TtaAppendDocumentPath (documentDirectory);

      /* create a Thot document of type HTML */
#endif /* STANDALONE */
      /* the Thot document has been successfully created */
      {
#ifndef STANDALONE
	length = ustrlen (pathURL);
	if (ustrcmp (pathURL, htmlFileName) == 0)
	  {
	    docURL = TtaAllocString (length + 1);
	    ustrcpy (docURL, pathURL);
	  }
	else
	  {
	    length += ustrlen (htmlFileName) + 20;
	    docURL = TtaAllocString (length+1);
	    usprintf (docURL, TEXT("%s temp file: %s"), pathURL, htmlFileName);
	  }
#endif /* STANDALONE */

	/* do not check the Thot abstract tree against the structure */
	/* schema while building the Thot document. */
	TtaSetStructureChecking (0, doc);
	/* set the notification mode for the new document */
	TtaSetNotificationMode (doc, 1);
	currentLanguage = TtaGetDefaultLanguage ();
#ifndef STANDALONE
	DocumentSSchema = TtaGetDocumentSSchema (doc);

	/* is the current document a HTML document */
	isHTML = (ustrcmp (TtaGetSSchemaName (DocumentSSchema),
			   TEXT("HTML")) == 0);	
	if (!isHTML)
	  {
	    /* change the document type */
	    TtaFreeView (doc, 1);
	    doc = TtaNewDocument (TEXT("HTML"), documentName);
	    if (TtaGetScreenDepth () > 1)
	        TtaSetPSchema (doc, TEXT("HTMLP"));
	    else
	        TtaSetPSchema (doc, TEXT("HTMLPBW"));
	    DocumentSSchema = TtaGetDocumentSSchema (doc);
	    isHTML = TRUE;
	  }
	
	LoadUserStyleSheet (doc);
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
	
	TtaSetDisplayMode (doc, NoComputedDisplay);

	/* delete all element except the root element */
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
	/* disable auto save */
	TtaSetDocumentBackUpInterval (doc, 0);
#endif /* STANDALONE */

	/* initialize parsing environment */
	InitializeXHTMLParser (NULL, FALSE, 0);

#ifdef EXPAT
	/* Specific initialization for expat */
	InitializeExpatParser ();
#endif /* EXPAT */
	
	/* parse the input file and build the Thot document */
	XHTMLparse (parser, stream, NULL);

	/* completes all unclosed elements */
	el = lastElement;
	while (el != NULL)
	  {
	    ElementComplete (el);
	    el = TtaGetParent (el);
	  }

	/* check the Thot abstract tree */
	CheckAbstractTree (pathURL);

	gzclose (stream);
	
#ifdef STANDALONE
	/* save and close the Thot document */
	TtaSaveDocument (doc, pivotFileName);
	TtaCloseDocument (doc);
#else  /* STANDALONE */
	TtaFreeMemory (docURL);
	/* an HTML document could be a template */
	if (!plainText)
	    OpenTemplateDocument (doc);
	TtaSetDisplayMode (doc, DisplayImmediately);
#endif /* STANDALONE */

#ifdef EXPAT
	FreeExpatParser ();
#endif /* EXPAT */

	/* check the Thot abstract tree against the structure schema. */
	TtaSetStructureChecking (1, doc);
	DocumentSSchema = NULL;
      }
    }

#ifdef STANDALONE
  /* quit the Thot toolkit */
  TtaQuit ();
}
#else  /* STANDALONE */
   TtaSetDocumentUnmodified (doc);
   theDocument = 0;
#endif /* STANDALONE */
}

/* end of module */

