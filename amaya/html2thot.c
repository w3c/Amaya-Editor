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

typedef UCHAR_T entityName[10];
typedef struct _CharEntityEntry
  {			 /* a SGML entity representing an ISO-Latin1 char */
     entityName          charName;	/* entity name */
     int                 charCode;	/* decimal code of ISO-Latin1 char */
  }
CharEntityEntry;


CharEntityEntry        CharEntityTable[] =
{
#if defined(_I18N_) || defined(__JIS__)
{L"AElig", 198},		/* latin capital letter AE = latin capital ligature AE, U+00C6 ISOlat1 */
{L"Aacute", 193},	/* latin capital letter A with acute, U+00C1 ISOlat1 */
{L"Acirc", 194},		/* latin capital letter A with circumflex, U+00C2 ISOlat1 */
{L"Agrave", 192},	/* latin capital letter A with grave = latin capital letter A grave, U+00C0 ISOlat1 */
{L"Alpha", 913},		/* greek capital letter alpha, U+0391 */
{L"Aring", 197},		/* latin capital letter A with ring above = latin capital letter A ring, U+00C5 ISOlat1 */
{L"Atilde", 195},	/* latin capital letter A with tilde, U+00C3 ISOlat1 */
{L"Auml", 196},		/* latin capital letter A with diaeresis, U+00C4 ISOlat1 */
{L"Beta", 914},		/* greek capital letter beta, U+0392 */
{L"Ccedil", 199},	/* latin capital letter C with cedilla, U+00C7 ISOlat1 */
{L"Chi", 935},		/* greek capital letter chi, U+03A7 */
{L"Dagger", 8225},	/* double dagger, U+2021 ISOpub */
{L"Delta", 916},		/* greek capital letter delta, U+0394 ISOgrk3 */
{L"ETH", 208},		/* latin capital letter ETH, U+00D0 ISOlat1 */
{L"Eacute", 201},	/* latin capital letter E with acute, U+00C9 ISOlat1 */
{L"Ecirc", 202},		/* latin capital letter E with circumflex, U+00CA ISOlat1 */
{L"Egrave", 200},	/* latin capital letter E with grave, U+00C8 ISOlat1 */
{L"Epsilon", 917},	/* greek capital letter epsilon, U+0395 */
{L"Eta", 919},		/* greek capital letter eta, U+0397 */
{L"Euml", 203},		/* latin capital letter E with diaeresis, U+00CB ISOlat1 */
{L"Gamma", 915},		/* greek capital letter gamma, U+0393 ISOgrk3 */
{L"Iacute", 205},	/* latin capital letter I with acute, U+00CD ISOlat1 */
{L"Icirc", 206},		/* latin capital letter I with circumflex, U+00CE ISOlat1 */
{L"Igrave", 204},	/* latin capital letter I with grave, U+00CC ISOlat1 */
{L"Iota", 921},		/* greek capital letter iota, U+0399 */
{L"Iuml", 207},		/* latin capital letter I with diaeresis, U+00CF ISOlat1 */
{L"Kappa", 922},		/* greek capital letter kappa, U+039A */
{L"Lambda", 923},	/* greek capital letter lambda, U+039B ISOgrk3 */
{L"Mu", 924},		/* greek capital letter mu, U+039C */
{L"Ntilde", 209},	/* latin capital letter N with tilde, U+00D1 ISOlat1 */
{L"Nu", 925},		/* greek capital letter nu, U+039D */
{L"OElig", 338},		/* latin capital ligature OE, U+0152 ISOlat2 */
{L"Oacute", 211},	/* latin capital letter O with acute, U+00D3 ISOlat1 */
{L"Ocirc", 212},		/* latin capital letter O with circumflex, U+00D4 ISOlat1 */
{L"Ograve", 210},	/* latin capital letter O with grave, U+00D2 ISOlat1 */
{L"Omega", 937},		/* greek capital letter omega, U+03A9 ISOgrk3 */
{L"Omicron", 927},	/* greek capital letter omicron, U+039F */
{L"Oslash", 216},	/* latin capital letter O with stroke = latin capital letter O slash, U+00D8 ISOlat1 */
{L"Otilde", 213},	/* latin capital letter O with tilde, U+00D5 ISOlat1 */
{L"Ouml", 214},		/* latin capital letter O with diaeresis, U+00D6 ISOlat1 */
{L"Phi", 934},		/* greek capital letter phi, U+03A6 ISOgrk3 */
{L"Pi", 928},		/* greek capital letter pi, U+03A0 ISOgrk3 */
{L"Prime", 8243},	/* double prime = seconds = inches, U+2033 ISOtech */
{L"Psi", 936},		/* greek capital letter psi, U+03A8 ISOgrk3 */
{L"Rho", 929},		/* greek capital letter rho, U+03A1 */
{L"Scaron", 352},	/* latin capital letter S with caron, U+0160 ISOlat2 */
{L"Sigma", 931},		/* greek capital letter sigma, U+03A3 ISOgrk3 */
{L"THORN", 222},		/* latin capital letter THORN, U+00DE ISOlat1 */
{L"Tau", 932},		/* greek capital letter tau, U+03A4 */
{L"Theta", 920},		/* greek capital letter theta, U+0398 ISOgrk3 */
{L"Uacute", 218},	/* latin capital letter U with acute, U+00DA ISOlat1 */
{L"Ucirc", 219},		/* latin capital letter U with circumflex, U+00DB ISOlat1 */
{L"Ugrave", 217},	/* latin capital letter U with grave, U+00D9 ISOlat1 */
{L"Upsilon", 933},	/* greek capital letter upsilon, U+03A5 ISOgrk3 */
{L"Uuml", 220},		/* latin capital letter U with diaeresis, U+00DC ISOlat1 */
{L"Xi", 926},		/* greek capital letter xi, U+039E ISOgrk3 */
{L"Yacute", 221},	/* latin capital letter Y with acute, U+00DD ISOlat1 */
{L"Yuml", 376},		/* latin capital letter Y with diaeresis, U+0178 ISOlat2 */
{L"Zeta", 918},		/* greek capital letter zeta, U+0396 */
{L"aacute", 225},	/* latin small letter a with acute, U+00E1 ISOlat1 */
{L"acirc", 226},		/* latin small letter a with circumflex, U+00E2 ISOlat1 */
{L"acute", 180},		/* acute accent = spacing acute, U+00B4 ISOdia */
{L"aelig", 230},		/* latin small letter ae = latin small ligature ae, U+00E6 ISOlat1 */
{L"agrave", 224},	/* latin small letter a with grave = latin small letter a grave, U+00E0 ISOlat1 */
{L"alefsym", 8501},	/* alef symbol = first transfinite cardinal, U+2135 NEW */
{L"alpha", 945},		/* greek small letter alpha, U+03B1 ISOgrk3 */
{L"amp", 38},		/* ampersand, U+0026 ISOnum */
{L"and", 8743},		/* logical and = wedge, U+2227 ISOtech */
{L"ang", 8736},		/* angle, U+2220 ISOamso */
{L"aring", 229},		/* latin small letter a with ring above = latin small letter a ring, U+00E5 ISOlat1 */
{L"asymp", 8776},	/* almost equal to = asymptotic to, U+2248 ISOamsr */
{L"atilde", 227},	/* latin small letter a with tilde, U+00E3 ISOlat1 */
{L"auml", 228},		/* latin small letter a with diaeresis, U+00E4 ISOlat1 */
{L"bdquo", 8222},	/* double low-9 quotation mark, U+201E NEW */
{L"beta", 946},		/* greek small letter beta, U+03B2 ISOgrk3 */
{L"brvbar", 166},	/* broken bar = broken vertical bar, U+00A6 ISOnum */
{L"bull", 8226},		/* bullet = black small circle, U+2022 ISOpub */
{L"cap", 8745},		/* intersection = cap, U+2229 ISOtech */
{L"ccedil", 231},	/* latin small letter c with cedilla, U+00E7 ISOlat1 */
{L"cedil", 184},		/* cedilla = spacing cedilla, U+00B8 ISOdia */
{L"cent", 162},		/* cent sign, U+00A2 ISOnum */
{L"chi", 967},		/* greek small letter chi, U+03C7 ISOgrk3 */
{L"circ", 710},		/* modifier letter circumflex accent, U+02C6 ISOpub */
{L"clubs", 9827},	/* black club suit = shamrock, U+2663 ISOpub */
{L"cong", 8773},		/* approximately equal to, U+2245 ISOtech */
{L"copy", 169},		/* copyright sign, U+00A9 ISOnum */
{L"crarr", 8629},	/* downwards arrow with corner leftwards = carriage return, U+21B5 NEW */
{L"cup", 8746},		/* union = cup, U+222A ISOtech */
{L"curren", 164},	/* currency sign, U+00A4 ISOnum */
{L"dArr", 8659},		/* downwards double arrow, U+21D3 ISOamsa */
{L"dagger", 8224},	/* dagger, U+2020 ISOpub */
{L"darr", 8595},		/* downwards arrow, U+2193 ISOnum */
{L"deg", 176},		/* degree sign, U+00B0 ISOnum */
{L"delta", 948},		/* greek small letter delta, U+03B4 ISOgrk3 */
{L"diams", 9830},	/* black diamond suit, U+2666 ISOpub */
{L"divide", 247},	/* division sign, U+00F7 ISOnum */
{L"eacute", 233},	/* latin small letter e with acute, U+00E9 ISOlat1 */
{L"ecirc", 234},		/* latin small letter e with circumflex, U+00EA ISOlat1 */
{L"egrave", 232},	/* latin small letter e with grave, U+00E8 ISOlat1 */
{L"empty", 8709},	/* empty set = null set = diameter, U+2205 ISOamso */
{L"emsp", 8195},		/* em space, U+2003 ISOpub */
{L"ensp", 8194},		/* en space, U+2002 ISOpub */
{L"epsilon", 949},	/* greek small letter epsilon, U+03B5 ISOgrk3 */
{L"equiv", 8801},	/* identical to, U+2261 ISOtech */
{L"eta", 951},		/* greek small letter eta, U+03B7 ISOgrk3 */
{L"eth", 240},		/* latin small letter eth, U+00F0 ISOlat1 */
{L"euml", 235},		/* latin small letter e with diaeresis, U+00EB ISOlat1 */
{L"euro", 8364},		/* euro sign, U+20AC NEW */
{L"exist", 8707},	/* there exists, U+2203 ISOtech */
{L"fnof", 402},		/* latin small f with hook = function = florin, U+0192 ISOtech */
{L"forall", 8704},	/* for all, U+2200 ISOtech */
{L"frac12", 189},	/* vulgar fraction one half = fraction one half, U+00BD ISOnum */
{L"frac14", 188},	/* vulgar fraction one quarter = fraction one quarter, U+00BC ISOnum */
{L"frac34", 190},	/* vulgar fraction three quarters = fraction three quarters, U+00BE ISOnum */
{L"frasl", 8260},	/* fraction slash, U+2044 NEW */
{L"gamma", 947},		/* greek small letter gamma, U+03B3 ISOgrk3 */
{L"ge", 8805},		/* greater-than or equal to, U+2265 ISOtech */
{L"gt", 62},		/* greater-than sign, U+003E ISOnum */
{L"hArr", 8660},		/* left right double arrow, U+21D4 ISOamsa */
{L"harr", 8596},		/* left right arrow, U+2194 ISOamsa */
{L"hearts", 9829},	/* black heart suit = valentine, U+2665 ISOpub */
{L"hellip", 8230},	/* horizontal ellipsis = three dot leader, U+2026 ISOpub */
{L"hyphen", 173},	/* hyphen = discretionary hyphen, U+00AD ISOnum */
{L"iacute", 237},	/* latin small letter i with acute, U+00ED ISOlat1 */
{L"icirc", 238},		/* latin small letter i with circumflex, U+00EE ISOlat1 */
{L"iexcl", 161},		/* inverted exclamation mark, U+00A1 ISOnum */
{L"igrave", 236},	/* latin small letter i with grave, U+00EC ISOlat1 */
{L"image", 8465},	/* blackletter capital I = imaginary part, U+2111 ISOamso */
{L"infin", 8734},	/* infinity, U+221E ISOtech */
{L"int", 8747},		/* integral, U+222B ISOtech */
{L"iota", 953},		/* greek small letter iota, U+03B9 ISOgrk3 */
{L"iquest", 191},	/* inverted question mark = turned question mark, U+00BF ISOnum */
{L"isin", 8712},		/* element of, U+2208 ISOtech */
{L"iuml", 239},		/* latin small letter i with diaeresis, U+00EF ISOlat1 */
{L"kappa", 954},		/* greek small letter kappa, U+03BA ISOgrk3 */
{L"lArr", 8656},		/* leftwards double arrow, U+21D0 ISOtech */
{L"lambda", 955},	/* greek small letter lambda, U+03BB ISOgrk3 */
{L"lang", 9001},		/* left-pointing angle bracket = bra, U+2329 ISOtech */
{L"laquo", 171},		/* left-pointing double angle quotation mark = left pointing guillemet, U+00AB ISOnum */
{L"larr", 8592},		/* leftwards arrow, U+2190 ISOnum */
{L"lceil", 8968},	/* left ceiling = apl upstile, U+2308 ISOamsc */
{L"ldquo", 8220},	/* left double quotation mark, U+201C ISOnum */
{L"le", 8804},		/* less-than or equal to, U+2264 ISOtech */
{L"lfloor", 8970},	/* left floor = apl downstile, U+230A ISOamsc */
{L"lowast", 8727},	/* asterisk operator, U+2217 ISOtech */
{L"loz", 9674},		/* lozenge, U+25CA ISOpub */
{L"lrm", 8206},		/* left-to-right mark, U+200E NEW RFC 2070 */
{L"lsaquo", 8249},	/* single left-pointing angle quotation mark, U+2039 ISO proposed */
{L"lsquo", 8216},	/* left single quotation mark, U+2018 ISOnum */
{L"lt", 60},		/* less-than sign, U+003C ISOnum */
{L"macr", 175},		/* macron = spacing macron = overline = APL overbar, U+00AF ISOdia */
{L"mdash", 8212},	/* em dash, U+2014 ISOpub */
{L"micro", 181},		/* micro sign, U+00B5 ISOnum */
{L"middot", 183},	/* middle dot = Georgian comma = Greek middle dot, U+00B7 ISOnum */
{L"minus", 8722},	/* minus sign, U+2212 ISOtech */
{L"mu", 956},		/* greek small letter mu, U+03BC ISOgrk3 */
{L"nabla", 8711},	/* nabla = backward difference, U+2207 ISOtech */
{L"nbsp", 160},		/* no-break space = non-breaking space, U+00A0 ISOnum */
{L"ndash", 8211},	/* en dash, U+2013 ISOpub */
{L"ne", 8800},		/* not equal to, U+2260 ISOtech */
{L"ni", 8715},		/* contains as member, U+220B ISOtech */
{L"not", 172},		/* not sign, U+00AC ISOnum */
{L"notin", 8713},	/* not an element of, U+2209 ISOtech */
{L"nsub", 8836},		/* not a subset of, U+2284 ISOamsn */
{L"ntilde", 241},	/* latin small letter n with tilde, U+00F1 ISOlat1 */
{L"nu", 957},		/* greek small letter nu, U+03BD ISOgrk3 */
{L"oacute", 243},	/* latin small letter o with acute, U+00F3 ISOlat1 */
{L"ocirc", 244},		/* latin small letter o with circumflex, U+00F4 ISOlat1 */
{L"oelig", 339},		/* latin small ligature oe, U+0153 ISOlat2 */
{L"ograve", 242},	/* latin small letter o with grave, U+00F2 ISOlat1 */
{L"oline", 8254},	/* overline = spacing overscore, U+203E NEW */
{L"omega", 969},		/* greek small letter omega, U+03C9 ISOgrk3 */
{L"omicron", 959},	/* greek small letter omicron, U+03BF NEW */
{L"oplus", 8853},	/* circled plus = direct sum, U+2295 ISOamsb */
{L"or", 8744},		/* logical or = vee, U+2228 ISOtech */
{L"ordf", 170},		/* feminine ordinal indicator, U+00AA ISOnum */
{L"ordm", 186},		/* masculine ordinal indicator, U+00BA ISOnum */
{L"oslash", 248},	/* latin small letter o with stroke, = latin small letter o slash, U+00F8 ISOlat1 */
{L"otilde", 245},	/* latin small letter o with tilde, U+00F5 ISOlat1 */
{L"otimes", 8855},	/* circled times = vector product, U+2297 ISOamsb */
{L"ouml", 246},		/* latin small letter o with diaeresis, U+00F6 ISOlat1 */
{L"para", 182},		/* pilcrow sign = paragraph sign, U+00B6 ISOnum */
{L"part", 8706},		/* partial differential, U+2202 ISOtech */
{L"permil", 8240},	/* per mille sign, U+2030 ISOtech */
{L"perp", 8869},		/* up tack = orthogonal to = perpendicular, U+22A5 ISOtech */
{L"phi", 966},		/* greek small letter phi, U+03C6 ISOgrk3 */
{L"pi", 960},		/* greek small letter pi, U+03C0 ISOgrk3 */
{L"piv", 982},		/* greek pi symbol, U+03D6 ISOgrk3 */
{L"plusmn", 177},	/* plus-minus sign = plus-or-minus sign, U+00B1 ISOnum */
{L"pound", 163},		/* pound sign, U+00A3 ISOnum */
{L"prime", 8242},	/* prime = minutes = feet, U+2032 ISOtech */
{L"prod", 8719},		/* n-ary product = product sign, U+220F ISOamsb */
{L"prop", 8733},		/* proportional to, U+221D ISOtech */
{L"psi", 968},		/* greek small letter psi, U+03C8 ISOgrk3 */
{L"quot", 34},		/* quotation mark = APL quote, U+0022 ISOnum */
{L"rArr", 8658},		/* rightwards double arrow, U+21D2 ISOtech */
{L"radic", 8730},	/* square root = radical sign, U+221A ISOtech */
{L"rang", 9002},		/* right-pointing angle bracket = ket, U+232A ISOtech */
{L"raquo", 187},		/* right-pointing double angle quotation mark = right pointing guillemet, U+00BB ISOnum */
{L"rarr", 8594},		/* rightwards arrow, U+2192 ISOnum */
{L"rceil", 8969},	/* right ceiling, U+2309 ISOamsc */
{L"rdquo", 8221},	/* right double quotation mark, U+201D ISOnum */
{L"real", 8476},		/* blackletter capital R = real part symbol, U+211C ISOamso */
{L"reg", 174},		/* registered sign = registered trade mark sign, U+00AE ISOnum */
{L"rfloor", 8971},	/* right floor, U+230B ISOamsc */
{L"rho", 961},		/* greek small letter rho, U+03C1 ISOgrk3 */
{L"rlm", 8207},		/* right-to-left mark, U+200F NEW RFC 2070 */
{L"rsaquo", 8250},	/* single right-pointing angle quotation mark, U+203A ISO proposed */
{L"rsquo", 8217},	/* right single quotation mark, U+2019 ISOnum */
{L"sbquo", 8218},	/* single low-9 quotation mark, U+201A NEW */
{L"scaron", 353},	/* latin small letter s with caron, U+0161 ISOlat2 */
{L"sdot", 8901},		/* dot operator, U+22C5 ISOamsb */
{L"sect", 167},		/* section sign, U+00A7 ISOnum */
{L"shy", 173},		/* soft hyphen = discretionary hyphen, U+00AD ISOnum */
{L"sigma", 963},		/* greek small letter sigma, U+03C3 ISOgrk3 */
{L"sigmaf", 962},	/* greek small letter final sigma, U+03C2 ISOgrk3 */
{L"sim", 8764},		/* tilde operator = varies with = similar to, U+223C ISOtech */
{L"spades", 9824},	/* black spade suit, U+2660 ISOpub */
{L"sub", 8834},		/* subset of, U+2282 ISOtech */
{L"sube", 8838},		/* subset of or equal to, U+2286 ISOtech */
{L"sum", 8721},		/* n-ary sumation, U+2211 ISOamsb */
{L"sup", 8835},		/* superset of, U+2283 ISOtech */
{L"sup1", 185},		/* superscript one = superscript digit one, U+00B9 ISOnum */
{L"sup2", 178},		/* superscript two = superscript digit two = squared, U+00B2 ISOnum */
{L"sup3", 179},		/* superscript three = superscript digit three = cubed, U+00B3 ISOnum */
{L"supe", 8839},		/* superset of or equal to, U+2287 ISOtech */
{L"szlig", 223},		/* latin small letter sharp s = ess-zed, U+00DF ISOlat1 */
{L"tau", 964},		/* greek small letter tau, U+03C4 ISOgrk3 */
{L"there4", 8756},	/* therefore, U+2234 ISOtech */
{L"theta", 952},		/* greek small letter theta, U+03B8 ISOgrk3 */
{L"thetasym", 977},	/* greek small letter theta symbol, U+03D1 NEW */
{L"thinsp", 8201},	/* thin space, U+2009 ISOpub */
{L"thorn", 254},		/* latin small letter thorn with, U+00FE ISOlat1 */
{L"tilde", 732},		/* small tilde, U+02DC ISOdia */
{L"times", 215},		/* multiplication sign, U+00D7 ISOnum */
{L"trade", 8482},	/* trade mark sign, U+2122 ISOnum */
{L"uArr", 8657},		/* upwards double arrow, U+21D1 ISOamsa */
{L"uacute", 250},	/* latin small letter u with acute, U+00FA ISOlat1 */
{L"uarr", 8593},		/* upwards arrow, U+2191 ISOnum*/
{L"ucirc", 251},		/* latin small letter u with circumflex, U+00FB ISOlat1 */
{L"ugrave", 249},	/* latin small letter u with grave, U+00F9 ISOlat1 */
{L"uml", 168},		/* diaeresis = spacing diaeresis, U+00A8 ISOdia */
{L"upsih", 978},		/* greek upsilon with hook symbol, U+03D2 NEW */
{L"upsilon", 965},	/* greek small letter upsilon, U+03C5 ISOgrk3 */
{L"uuml", 252},		/* latin small letter u with diaeresis, U+00FC ISOlat1 */
{L"weierp", 8472},	/* script capital P = power set = Weierstrass p, U+2118 ISOamso */
{L"xi", 958},		/* greek small letter xi, U+03BE ISOgrk3 */
{L"yacute", 253},	/* latin small letter y with acute, U+00FD ISOlat1 */
{L"yen", 165},		/* yen sign = yuan sign, U+00A5 ISOnum */
{L"yuml", 255},		/* latin small letter y with diaeresis, U+00FF ISOlat1 */
{L"zeta", 950},		/* greek small letter zeta, U+03B6 ISOgrk3 */
{L"zwj", 8205},		/* zero width joiner, U+200D NEW RFC 2070 */
{L"zwnj", 8204},		/* zero width non-joiner, U+200C NEW RFC 2070 */

{L"zzzz", 0}			/* this last entry is required */
#else /* !defined(_I18N_) && defined(__JIS__) */
   /* This table MUST be in alphabetical order */
{"AElig", 198},		/* latin capital letter AE = latin capital ligature AE, U+00C6 ISOlat1 */
{"Aacute", 193},	/* latin capital letter A with acute, U+00C1 ISOlat1 */
{"Acirc", 194},		/* latin capital letter A with circumflex, U+00C2 ISOlat1 */
{"Agrave", 192},	/* latin capital letter A with grave = latin capital letter A grave, U+00C0 ISOlat1 */
{"Alpha", 913},		/* greek capital letter alpha, U+0391 */
{"Aring", 197},		/* latin capital letter A with ring above = latin capital letter A ring, U+00C5 ISOlat1 */
{"Atilde", 195},	/* latin capital letter A with tilde, U+00C3 ISOlat1 */
{"Auml", 196},		/* latin capital letter A with diaeresis, U+00C4 ISOlat1 */
{"Beta", 914},		/* greek capital letter beta, U+0392 */
{"Ccedil", 199},	/* latin capital letter C with cedilla, U+00C7 ISOlat1 */
{"Chi", 935},		/* greek capital letter chi, U+03A7 */
{"Dagger", 8225},	/* double dagger, U+2021 ISOpub */
{"Delta", 916},		/* greek capital letter delta, U+0394 ISOgrk3 */
{"ETH", 208},		/* latin capital letter ETH, U+00D0 ISOlat1 */
{"Eacute", 201},	/* latin capital letter E with acute, U+00C9 ISOlat1 */
{"Ecirc", 202},		/* latin capital letter E with circumflex, U+00CA ISOlat1 */
{"Egrave", 200},	/* latin capital letter E with grave, U+00C8 ISOlat1 */
{"Epsilon", 917},	/* greek capital letter epsilon, U+0395 */
{"Eta", 919},		/* greek capital letter eta, U+0397 */
{"Euml", 203},		/* latin capital letter E with diaeresis, U+00CB ISOlat1 */
{"Gamma", 915},		/* greek capital letter gamma, U+0393 ISOgrk3 */
{"Iacute", 205},	/* latin capital letter I with acute, U+00CD ISOlat1 */
{"Icirc", 206},		/* latin capital letter I with circumflex, U+00CE ISOlat1 */
{"Igrave", 204},	/* latin capital letter I with grave, U+00CC ISOlat1 */
{"Iota", 921},		/* greek capital letter iota, U+0399 */
{"Iuml", 207},		/* latin capital letter I with diaeresis, U+00CF ISOlat1 */
{"Kappa", 922},		/* greek capital letter kappa, U+039A */
{"Lambda", 923},	/* greek capital letter lambda, U+039B ISOgrk3 */
{"Mu", 924},		/* greek capital letter mu, U+039C */
{"Ntilde", 209},	/* latin capital letter N with tilde, U+00D1 ISOlat1 */
{"Nu", 925},		/* greek capital letter nu, U+039D */
{"OElig", 338},		/* latin capital ligature OE, U+0152 ISOlat2 */
{"Oacute", 211},	/* latin capital letter O with acute, U+00D3 ISOlat1 */
{"Ocirc", 212},		/* latin capital letter O with circumflex, U+00D4 ISOlat1 */
{"Ograve", 210},	/* latin capital letter O with grave, U+00D2 ISOlat1 */
{"Omega", 937},		/* greek capital letter omega, U+03A9 ISOgrk3 */
{"Omicron", 927},	/* greek capital letter omicron, U+039F */
{"Oslash", 216},	/* latin capital letter O with stroke = latin capital letter O slash, U+00D8 ISOlat1 */
{"Otilde", 213},	/* latin capital letter O with tilde, U+00D5 ISOlat1 */
{"Ouml", 214},		/* latin capital letter O with diaeresis, U+00D6 ISOlat1 */
{"Phi", 934},		/* greek capital letter phi, U+03A6 ISOgrk3 */
{"Pi", 928},		/* greek capital letter pi, U+03A0 ISOgrk3 */
{"Prime", 8243},	/* double prime = seconds = inches, U+2033 ISOtech */
{"Psi", 936},		/* greek capital letter psi, U+03A8 ISOgrk3 */
{"Rho", 929},		/* greek capital letter rho, U+03A1 */
{"Scaron", 352},	/* latin capital letter S with caron, U+0160 ISOlat2 */
{"Sigma", 931},		/* greek capital letter sigma, U+03A3 ISOgrk3 */
{"THORN", 222},		/* latin capital letter THORN, U+00DE ISOlat1 */
{"Tau", 932},		/* greek capital letter tau, U+03A4 */
{"Theta", 920},		/* greek capital letter theta, U+0398 ISOgrk3 */
{"Uacute", 218},	/* latin capital letter U with acute, U+00DA ISOlat1 */
{"Ucirc", 219},		/* latin capital letter U with circumflex, U+00DB ISOlat1 */
{"Ugrave", 217},	/* latin capital letter U with grave, U+00D9 ISOlat1 */
{"Upsilon", 933},	/* greek capital letter upsilon, U+03A5 ISOgrk3 */
{"Uuml", 220},		/* latin capital letter U with diaeresis, U+00DC ISOlat1 */
{"Xi", 926},		/* greek capital letter xi, U+039E ISOgrk3 */
{"Yacute", 221},	/* latin capital letter Y with acute, U+00DD ISOlat1 */
{"Yuml", 376},		/* latin capital letter Y with diaeresis, U+0178 ISOlat2 */
{"Zeta", 918},		/* greek capital letter zeta, U+0396 */
{"aacute", 225},	/* latin small letter a with acute, U+00E1 ISOlat1 */
{"acirc", 226},		/* latin small letter a with circumflex, U+00E2 ISOlat1 */
{"acute", 180},		/* acute accent = spacing acute, U+00B4 ISOdia */
{"aelig", 230},		/* latin small letter ae = latin small ligature ae, U+00E6 ISOlat1 */
{"agrave", 224},	/* latin small letter a with grave = latin small letter a grave, U+00E0 ISOlat1 */
{"alefsym", 8501},	/* alef symbol = first transfinite cardinal, U+2135 NEW */
{"alpha", 945},		/* greek small letter alpha, U+03B1 ISOgrk3 */
{"amp", 38},		/* ampersand, U+0026 ISOnum */
{"and", 8743},		/* logical and = wedge, U+2227 ISOtech */
{"ang", 8736},		/* angle, U+2220 ISOamso */
{"aring", 229},		/* latin small letter a with ring above = latin small letter a ring, U+00E5 ISOlat1 */
{"asymp", 8776},	/* almost equal to = asymptotic to, U+2248 ISOamsr */
{"atilde", 227},	/* latin small letter a with tilde, U+00E3 ISOlat1 */
{"auml", 228},		/* latin small letter a with diaeresis, U+00E4 ISOlat1 */
{"bdquo", 8222},	/* double low-9 quotation mark, U+201E NEW */
{"beta", 946},		/* greek small letter beta, U+03B2 ISOgrk3 */
{"brvbar", 166},	/* broken bar = broken vertical bar, U+00A6 ISOnum */
{"bull", 8226},		/* bullet = black small circle, U+2022 ISOpub */
{"cap", 8745},		/* intersection = cap, U+2229 ISOtech */
{"ccedil", 231},	/* latin small letter c with cedilla, U+00E7 ISOlat1 */
{"cedil", 184},		/* cedilla = spacing cedilla, U+00B8 ISOdia */
{"cent", 162},		/* cent sign, U+00A2 ISOnum */
{"chi", 967},		/* greek small letter chi, U+03C7 ISOgrk3 */
{"circ", 710},		/* modifier letter circumflex accent, U+02C6 ISOpub */
{"clubs", 9827},	/* black club suit = shamrock, U+2663 ISOpub */
{"cong", 8773},		/* approximately equal to, U+2245 ISOtech */
{"copy", 169},		/* copyright sign, U+00A9 ISOnum */
{"crarr", 8629},	/* downwards arrow with corner leftwards = carriage return, U+21B5 NEW */
{"cup", 8746},		/* union = cup, U+222A ISOtech */
{"curren", 164},	/* currency sign, U+00A4 ISOnum */
{"dArr", 8659},		/* downwards double arrow, U+21D3 ISOamsa */
{"dagger", 8224},	/* dagger, U+2020 ISOpub */
{"darr", 8595},		/* downwards arrow, U+2193 ISOnum */
{"deg", 176},		/* degree sign, U+00B0 ISOnum */
{"delta", 948},		/* greek small letter delta, U+03B4 ISOgrk3 */
{"diams", 9830},	/* black diamond suit, U+2666 ISOpub */
{"divide", 247},	/* division sign, U+00F7 ISOnum */
{"eacute", 233},	/* latin small letter e with acute, U+00E9 ISOlat1 */
{"ecirc", 234},		/* latin small letter e with circumflex, U+00EA ISOlat1 */
{"egrave", 232},	/* latin small letter e with grave, U+00E8 ISOlat1 */
{"empty", 8709},	/* empty set = null set = diameter, U+2205 ISOamso */
{"emsp", 8195},		/* em space, U+2003 ISOpub */
{"ensp", 8194},		/* en space, U+2002 ISOpub */
{"epsilon", 949},	/* greek small letter epsilon, U+03B5 ISOgrk3 */
{"equiv", 8801},	/* identical to, U+2261 ISOtech */
{"eta", 951},		/* greek small letter eta, U+03B7 ISOgrk3 */
{"eth", 240},		/* latin small letter eth, U+00F0 ISOlat1 */
{"euml", 235},		/* latin small letter e with diaeresis, U+00EB ISOlat1 */
{"euro", 8364},		/* euro sign, U+20AC NEW */
{"exist", 8707},	/* there exists, U+2203 ISOtech */
{"fnof", 402},		/* latin small f with hook = function = florin, U+0192 ISOtech */
{"forall", 8704},	/* for all, U+2200 ISOtech */
{"frac12", 189},	/* vulgar fraction one half = fraction one half, U+00BD ISOnum */
{"frac14", 188},	/* vulgar fraction one quarter = fraction one quarter, U+00BC ISOnum */
{"frac34", 190},	/* vulgar fraction three quarters = fraction three quarters, U+00BE ISOnum */
{"frasl", 8260},	/* fraction slash, U+2044 NEW */
{"gamma", 947},		/* greek small letter gamma, U+03B3 ISOgrk3 */
{"ge", 8805},		/* greater-than or equal to, U+2265 ISOtech */
{"gt", 62},		/* greater-than sign, U+003E ISOnum */
{"hArr", 8660},		/* left right double arrow, U+21D4 ISOamsa */
{"harr", 8596},		/* left right arrow, U+2194 ISOamsa */
{"hearts", 9829},	/* black heart suit = valentine, U+2665 ISOpub */
{"hellip", 8230},	/* horizontal ellipsis = three dot leader, U+2026 ISOpub */
{"hyphen", 173},	/* hyphen = discretionary hyphen, U+00AD ISOnum */
{"iacute", 237},	/* latin small letter i with acute, U+00ED ISOlat1 */
{"icirc", 238},		/* latin small letter i with circumflex, U+00EE ISOlat1 */
{"iexcl", 161},		/* inverted exclamation mark, U+00A1 ISOnum */
{"igrave", 236},	/* latin small letter i with grave, U+00EC ISOlat1 */
{"image", 8465},	/* blackletter capital I = imaginary part, U+2111 ISOamso */
{"infin", 8734},	/* infinity, U+221E ISOtech */
{"int", 8747},		/* integral, U+222B ISOtech */
{"iota", 953},		/* greek small letter iota, U+03B9 ISOgrk3 */
{"iquest", 191},	/* inverted question mark = turned question mark, U+00BF ISOnum */
{"isin", 8712},		/* element of, U+2208 ISOtech */
{"iuml", 239},		/* latin small letter i with diaeresis, U+00EF ISOlat1 */
{"kappa", 954},		/* greek small letter kappa, U+03BA ISOgrk3 */
{"lArr", 8656},		/* leftwards double arrow, U+21D0 ISOtech */
{"lambda", 955},	/* greek small letter lambda, U+03BB ISOgrk3 */
{"lang", 9001},		/* left-pointing angle bracket = bra, U+2329 ISOtech */
{"laquo", 171},		/* left-pointing double angle quotation mark = left pointing guillemet, U+00AB ISOnum */
{"larr", 8592},		/* leftwards arrow, U+2190 ISOnum */
{"lceil", 8968},	/* left ceiling = apl upstile, U+2308 ISOamsc */
{"ldquo", 8220},	/* left double quotation mark, U+201C ISOnum */
{"le", 8804},		/* less-than or equal to, U+2264 ISOtech */
{"lfloor", 8970},	/* left floor = apl downstile, U+230A ISOamsc */
{"lowast", 8727},	/* asterisk operator, U+2217 ISOtech */
{"loz", 9674},		/* lozenge, U+25CA ISOpub */
{"lrm", 8206},		/* left-to-right mark, U+200E NEW RFC 2070 */
{"lsaquo", 8249},	/* single left-pointing angle quotation mark, U+2039 ISO proposed */
{"lsquo", 8216},	/* left single quotation mark, U+2018 ISOnum */
{"lt", 60},		/* less-than sign, U+003C ISOnum */
{"macr", 175},		/* macron = spacing macron = overline = APL overbar, U+00AF ISOdia */
{"mdash", 8212},	/* em dash, U+2014 ISOpub */
{"micro", 181},		/* micro sign, U+00B5 ISOnum */
{"middot", 183},	/* middle dot = Georgian comma = Greek middle dot, U+00B7 ISOnum */
{"minus", 8722},	/* minus sign, U+2212 ISOtech */
{"mu", 956},		/* greek small letter mu, U+03BC ISOgrk3 */
{"nabla", 8711},	/* nabla = backward difference, U+2207 ISOtech */
{"nbsp", 160},		/* no-break space = non-breaking space, U+00A0 ISOnum */
{"ndash", 8211},	/* en dash, U+2013 ISOpub */
{"ne", 8800},		/* not equal to, U+2260 ISOtech */
{"ni", 8715},		/* contains as member, U+220B ISOtech */
{"not", 172},		/* not sign, U+00AC ISOnum */
{"notin", 8713},	/* not an element of, U+2209 ISOtech */
{"nsub", 8836},		/* not a subset of, U+2284 ISOamsn */
{"ntilde", 241},	/* latin small letter n with tilde, U+00F1 ISOlat1 */
{"nu", 957},		/* greek small letter nu, U+03BD ISOgrk3 */
{"oacute", 243},	/* latin small letter o with acute, U+00F3 ISOlat1 */
{"ocirc", 244},		/* latin small letter o with circumflex, U+00F4 ISOlat1 */
{"oelig", 339},		/* latin small ligature oe, U+0153 ISOlat2 */
{"ograve", 242},	/* latin small letter o with grave, U+00F2 ISOlat1 */
{"oline", 8254},	/* overline = spacing overscore, U+203E NEW */
{"omega", 969},		/* greek small letter omega, U+03C9 ISOgrk3 */
{"omicron", 959},	/* greek small letter omicron, U+03BF NEW */
{"oplus", 8853},	/* circled plus = direct sum, U+2295 ISOamsb */
{"or", 8744},		/* logical or = vee, U+2228 ISOtech */
{"ordf", 170},		/* feminine ordinal indicator, U+00AA ISOnum */
{"ordm", 186},		/* masculine ordinal indicator, U+00BA ISOnum */
{"oslash", 248},	/* latin small letter o with stroke, = latin small letter o slash, U+00F8 ISOlat1 */
{"otilde", 245},	/* latin small letter o with tilde, U+00F5 ISOlat1 */
{"otimes", 8855},	/* circled times = vector product, U+2297 ISOamsb */
{"ouml", 246},		/* latin small letter o with diaeresis, U+00F6 ISOlat1 */
{"para", 182},		/* pilcrow sign = paragraph sign, U+00B6 ISOnum */
{"part", 8706},		/* partial differential, U+2202 ISOtech */
{"permil", 8240},	/* per mille sign, U+2030 ISOtech */
{"perp", 8869},		/* up tack = orthogonal to = perpendicular, U+22A5 ISOtech */
{"phi", 966},		/* greek small letter phi, U+03C6 ISOgrk3 */
{"pi", 960},		/* greek small letter pi, U+03C0 ISOgrk3 */
{"piv", 982},		/* greek pi symbol, U+03D6 ISOgrk3 */
{"plusmn", 177},	/* plus-minus sign = plus-or-minus sign, U+00B1 ISOnum */
{"pound", 163},		/* pound sign, U+00A3 ISOnum */
{"prime", 8242},	/* prime = minutes = feet, U+2032 ISOtech */
{"prod", 8719},		/* n-ary product = product sign, U+220F ISOamsb */
{"prop", 8733},		/* proportional to, U+221D ISOtech */
{"psi", 968},		/* greek small letter psi, U+03C8 ISOgrk3 */
{"quot", 34},		/* quotation mark = APL quote, U+0022 ISOnum */
{"rArr", 8658},		/* rightwards double arrow, U+21D2 ISOtech */
{"radic", 8730},	/* square root = radical sign, U+221A ISOtech */
{"rang", 9002},		/* right-pointing angle bracket = ket, U+232A ISOtech */
{"raquo", 187},		/* right-pointing double angle quotation mark = right pointing guillemet, U+00BB ISOnum */
{"rarr", 8594},		/* rightwards arrow, U+2192 ISOnum */
{"rceil", 8969},	/* right ceiling, U+2309 ISOamsc */
{"rdquo", 8221},	/* right double quotation mark, U+201D ISOnum */
{"real", 8476},		/* blackletter capital R = real part symbol, U+211C ISOamso */
{"reg", 174},		/* registered sign = registered trade mark sign, U+00AE ISOnum */
{"rfloor", 8971},	/* right floor, U+230B ISOamsc */
{"rho", 961},		/* greek small letter rho, U+03C1 ISOgrk3 */
{"rlm", 8207},		/* right-to-left mark, U+200F NEW RFC 2070 */
{"rsaquo", 8250},	/* single right-pointing angle quotation mark, U+203A ISO proposed */
{"rsquo", 8217},	/* right single quotation mark, U+2019 ISOnum */
{"sbquo", 8218},	/* single low-9 quotation mark, U+201A NEW */
{"scaron", 353},	/* latin small letter s with caron, U+0161 ISOlat2 */
{"sdot", 8901},		/* dot operator, U+22C5 ISOamsb */
{"sect", 167},		/* section sign, U+00A7 ISOnum */
{"shy", 173},		/* soft hyphen = discretionary hyphen, U+00AD ISOnum */
{"sigma", 963},		/* greek small letter sigma, U+03C3 ISOgrk3 */
{"sigmaf", 962},	/* greek small letter final sigma, U+03C2 ISOgrk3 */
{"sim", 8764},		/* tilde operator = varies with = similar to, U+223C ISOtech */
{"spades", 9824},	/* black spade suit, U+2660 ISOpub */
{"sub", 8834},		/* subset of, U+2282 ISOtech */
{"sube", 8838},		/* subset of or equal to, U+2286 ISOtech */
{"sum", 8721},		/* n-ary sumation, U+2211 ISOamsb */
{"sup", 8835},		/* superset of, U+2283 ISOtech */
{"sup1", 185},		/* superscript one = superscript digit one, U+00B9 ISOnum */
{"sup2", 178},		/* superscript two = superscript digit two = squared, U+00B2 ISOnum */
{"sup3", 179},		/* superscript three = superscript digit three = cubed, U+00B3 ISOnum */
{"supe", 8839},		/* superset of or equal to, U+2287 ISOtech */
{"szlig", 223},		/* latin small letter sharp s = ess-zed, U+00DF ISOlat1 */
{"tau", 964},		/* greek small letter tau, U+03C4 ISOgrk3 */
{"there4", 8756},	/* therefore, U+2234 ISOtech */
{"theta", 952},		/* greek small letter theta, U+03B8 ISOgrk3 */
{"thetasym", 977},	/* greek small letter theta symbol, U+03D1 NEW */
{"thinsp", 8201},	/* thin space, U+2009 ISOpub */
{"thorn", 254},		/* latin small letter thorn with, U+00FE ISOlat1 */
{"tilde", 732},		/* small tilde, U+02DC ISOdia */
{"times", 215},		/* multiplication sign, U+00D7 ISOnum */
{"trade", 8482},	/* trade mark sign, U+2122 ISOnum */
{"uArr", 8657},		/* upwards double arrow, U+21D1 ISOamsa */
{"uacute", 250},	/* latin small letter u with acute, U+00FA ISOlat1 */
{"uarr", 8593},		/* upwards arrow, U+2191 ISOnum*/
{"ucirc", 251},		/* latin small letter u with circumflex, U+00FB ISOlat1 */
{"ugrave", 249},	/* latin small letter u with grave, U+00F9 ISOlat1 */
{"uml", 168},		/* diaeresis = spacing diaeresis, U+00A8 ISOdia */
{"upsih", 978},		/* greek upsilon with hook symbol, U+03D2 NEW */
{"upsilon", 965},	/* greek small letter upsilon, U+03C5 ISOgrk3 */
{"uuml", 252},		/* latin small letter u with diaeresis, U+00FC ISOlat1 */
{"weierp", 8472},	/* script capital P = power set = Weierstrass p, U+2118 ISOamso */
{"xi", 958},		/* greek small letter xi, U+03BE ISOgrk3 */
{"yacute", 253},	/* latin small letter y with acute, U+00FD ISOlat1 */
{"yen", 165},		/* yen sign = yuan sign, U+00A5 ISOnum */
{"yuml", 255},		/* latin small letter y with diaeresis, U+00FF ISOlat1 */
{"zeta", 950},		/* greek small letter zeta, U+03B6 ISOgrk3 */
{"zwj", 8205},		/* zero width joiner, U+200D NEW RFC 2070 */
{"zwnj", 8204},		/* zero width non-joiner, U+200C NEW RFC 2070 */

{"zzzz", 0}			/* this last entry is required */
#endif /* defined(_I18N_) || defined(__JIS__) */
};

typedef struct _UnicodeFallbackEntry
  {
     int	unicodeVal;	/* the Unicode code */
     int	EightbitCode;   /* the corresponding glyph to be used from the ISO Latin-1 or Symbol
				   character set.
   if 0 < EightbitCode < 255, it's the Symbol code for the correct glyph
   if 1000 < EightbitCode < 1256, it's the ISO Latin-1 code + 1000 of an approaching glyph
   if 2000 < EightbitCode < 2256, it's the Symbol code + 2000 of an approaching glyph	*/
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
/* fnof     */ {402, 166}, /* latin small f with hook = function = florin, U+0192 ISOtech */
/* circ     */ {710, 2217}, /* modifier letter circumflex accent, U+02C6 ISOpub */
/* tilde    */ {732, 1126}, /* small tilde, U+02DC ISOdia */
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
/* sigmaf   */ {962, 86}, /* greek small letter final sigma, U+03C2 ISOgrk3 */
/* sigma    */ {963, 115}, /* greek small letter sigma, U+03C3 ISOgrk3 */
/* tau      */ {964, 116}, /* greek small letter tau, U+03C4 ISOgrk3 */
/* upsilon  */ {965, 117}, /* greek small letter upsilon, U+03C5 ISOgrk3 */
/* phi      */ {966, 102}, /* greek small letter phi, U+03C6 ISOgrk3 */
/* chi      */ {967, 99}, /* greek small letter chi, U+03C7 ISOgrk3 */
/* psi      */ {968, 121}, /* greek small letter psi, U+03C8 ISOgrk3 */
/* omega    */ {969, 119}, /* greek small letter omega, U+03C9 ISOgrk3 */
/* thetasym */ {977, 74}, /* greek small letter theta symbol, U+03D1 NEW */
/* upsih    */ {978, 161}, /* greek upsilon with hook symbol, U+03D2 NEW */
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
/* lsquo    */ {8216, 1096}, /* left single quotation mark, U+2018 ISOnum */
/* rsquo    */ {8217, 1039}, /* right single quotation mark, U+2019 ISOnum */
/* sbquo    */ {8218, 1044}, /* single low-9 quotation mark, U+201A NEW */
/* ldquo    */ {8220, 1096}, /* left double quotation mark, U+201C ISOnum */
/* rdquo    */ {8221, 1039}, /* right double quotation mark, U+201D ISOnum */
/* bdquo    */ {8222, 1044}, /* double low-9 quotation mark, U+201E NEW */
/* dagger   */ {8224, 2042}, /* dagger, U+2020 ISOpub */
/* Dagger   */ {8225, 2042}, /* double dagger, U+2021 ISOpub */
/* bull     */ {8226, 183}, /* bullet = black small circle,  U+2022 ISOpub  */
/* hellip   */ {8230, 188}, /* horizontal ellipsis = three dot leader,  U+2026 ISOpub  */
/* permil   */ {8240, 2037}, /* per mille sign, U+2030 ISOtech */
/* prime    */ {8242, 162}, /* prime = minutes = feet, U+2032 ISOtech */
/* Prime    */ {8243, 178}, /* double prime = seconds = inches,  U+2033 ISOtech */
/* lsaquo   */ {8249, 1060}, /* single left-pointing angle quotation mark, U+2039 ISO proposed */
/* rsaquo   */ {8250, 1062}, /* single right-pointing angle quotation mark, U+203A ISO proposed */
/* oline    */ {8254, 1175}, /* overline = spacing overscore,  U+203E NEW */
/* frasl    */ {8260, 164}, /* fraction slash, U+2044 NEW */
/* euro     */ {8364, 2206}, /* euro sign, U+20AC NEW */
/* image    */ {8465, 193}, /* blackletter capital I = imaginary part,  U+2111 ISOamso */
/* weierp   */ {8472, 195}, /* script capital P = power set  = Weierstrass p, U+2118 ISOamso */
/* real     */ {8476, 194}, /* blackletter capital R = real part symbol,  U+211C ISOamso */
/* trade    */ {8482, 212}, /* trade mark sign, U+2122 ISOnum */
/* alefsym  */ {8501, 192}, /* alef symbol = first transfinite cardinal,  U+2135 NEW */
/* larr     */ {8592, 172}, /* leftwards arrow, U+2190 ISOnum */
/* uarr     */ {8593, 173}, /* upwards arrow, U+2191 ISOnum*/
/* rarr     */ {8594, 174}, /* rightwards arrow, U+2192 ISOnum */
/* darr     */ {8595, 175}, /* downwards arrow, U+2193 ISOnum */
/* harr     */ {8596, 171}, /* left right arrow, U+2194 ISOamsa */
/* crarr    */ {8629, 191}, /* downwards arrow with corner leftwards  = carriage return, U+21B5 NEW */
/* lArr     */ {8656, 220}, /* leftwards double arrow, U+21D0 ISOtech */
/* uArr     */ {8657, 221}, /* upwards double arrow, U+21D1 ISOamsa */
/* rArr     */ {8658, 222}, /* rightwards double arrow,  U+21D2 ISOtech */
/* dArr     */ {8659, 223}, /* downwards double arrow, U+21D3 ISOamsa */
/* hArr     */ {8660, 219}, /* left right double arrow,  U+21D4 ISOamsa */
/* forall   */ {8704, 34}, /* for all, U+2200 ISOtech */
/* part     */ {8706, 182}, /* partial differential, U+2202 ISOtech  */
/* exist    */ {8707, 36}, /* there exists, U+2203 ISOtech */
/* empty    */ {8709, 198}, /* empty set = null set = diameter,  U+2205 ISOamso */
/* nabla    */ {8711, 209}, /* nabla = backward difference,  U+2207 ISOtech */
/* isin     */ {8712, 206}, /* element of, U+2208 ISOtech */
/* notin    */ {8713, 207}, /* not an element of, U+2209 ISOtech */
/* ni       */ {8715, 39}, /* contains as member, U+220B ISOtech */
/* prod     */ {8719, 213}, /* n-ary product = product sign,  U+220F ISOamsb */
/* sum      */ {8721, 229}, /* n-ary sumation, U+2211 ISOamsb */
/* minus    */ {8722, 45}, /* minus sign, U+2212 ISOtech */
/* lowast   */ {8727, 42}, /* asterisk operator, U+2217 ISOtech */
/* radic    */ {8730, 214}, /* square root = radical sign,  U+221A ISOtech */
/* prop     */ {8733, 181}, /* proportional to, U+221D ISOtech */
/* infin    */ {8734, 165}, /* infinity, U+221E ISOtech */
/* ang      */ {8736, 208}, /* angle, U+2220 ISOamso */
/* and      */ {8743, 217}, /* logical and = wedge, U+2227 ISOtech */
/* or       */ {8744, 218}, /* logical or = vee, U+2228 ISOtech */
/* cap      */ {8745, 199}, /* intersection = cap, U+2229 ISOtech */
/* cup      */ {8746, 200}, /* union = cup, U+222A ISOtech */
/* int      */ {8747, 242}, /* integral, U+222B ISOtech */
/* there4   */ {8756, 92}, /* therefore, U+2234 ISOtech */
/* sim      */ {8764, 126}, /* tilde operator = varies with = similar to,  U+223C ISOtech */
/* cong     */ {8773, 64}, /* approximately equal to, U+2245 ISOtech */
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
/* oplus    */ {8853, 197}, /* circled plus = direct sum,  U+2295 ISOamsb */
/* otimes   */ {8855, 196}, /* circled times = vector product,  U+2297 ISOamsb */
/* perp     */ {8869, 94}, /* up tack = orthogonal to = perpendicular,  U+22A5 ISOtech */
/* sdot     */ {8901, 215}, /* dot operator, U+22C5 ISOamsb */
/* lceil    */ {8968, 233}, /* left ceiling = apl upstile,  U+2308 ISOamsc  */
/* rceil    */ {8969, 249}, /* right ceiling, U+2309 ISOamsc  */
/* lfloor   */ {8970, 235}, /* left floor = apl downstile,  U+230A ISOamsc  */
/* rfloor   */ {8971, 251}, /* right floor, U+230B ISOamsc  */
/* lang     */ {9001, 225}, /* left-pointing angle bracket = bra,  U+2329 ISOtech */
/* rang     */ {9002, 241}, /* right-pointing angle bracket = ket,  U+232A ISOtech */
/* loz      */ {9674, 224}, /* lozenge, U+25CA ISOpub */
/* spades   */ {9824, 170}, /* black spade suit, U+2660 ISOpub */
/* clubs    */ {9827, 167}, /* black club suit = shamrock,  U+2663 ISOpub */
/* hearts   */ {9829, 169}, /* black heart suit = valentine,  U+2665 ISOpub */
/* diams    */ {9830, 168}, /* black diamond suit, U+2666 ISOpub */

/* THE END  */ {0, 0}		/* last entry (required) */
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
typedef CHAR_T        oneLine[100];
static oneLine      EquivEndingElem[] =
{
#  if defined(_I18N_) || defined(__JIS__)
   L"DT DD LI OPTION",
   L"H1 H2 H3 H4 H5 H6",
   L"OL MENU DIR ADDRESS PRE LISTING XMP",
   L""
#  else /* !defined(_I18N_) && !defined(__JIS__) */
   "DT DD LI OPTION",
   "H1 H2 H3 H4 H5 H6",
   "OL MENU DIR ADDRESS PRE LISTING XMP",
   ""
#  endif /* defined(_I18N_) || defined(__JIS__) */
};
/* acording the HTML DTD, HR should be added to the 2nd line above, as it */
/* is not allowed within a H1, H2, H3, etc. But we should tolerate that case */
/* because many documents contain rules in headings... */

/* start tags that imply the end of current element */
static oneLine      StartTagEndingElem[] =
{
#  if defined(_I18N_) || defined(__JIS__)
   L"FORM closes FORM P P* HR H1 H2 H3 H4 H5 H6 DL UL OL MENU DIR ADDRESS PRE LISTING XMP HEAD",
   L"HEAD closes P P*",
   L"TITLE closes P P*",
   L"BODY closes HEAD STYLE LINK TITLE P P*",
   L"LI closes P P* H1 H2 H3 H4 H5 H6 DL ADDRESS PRE LISTING XMP HEAD",
   L"HR closes P P* HEAD",
   L"H1 closes P P* HEAD",
   L"H2 closes P P* HEAD",
   L"H3 closes P P* HEAD",
   L"H4 closes P P* HEAD",
   L"H5 closes P P* HEAD",
   L"H6 closes P P* HEAD",
   L"DIR closes P P* HEAD",
   L"ADDRESS closes P P* HEAD UL",
   L"PRE closes P P* HEAD UL",
   L"LISTING closes P P* HEAD",
   L"XMP closes P P* HEAD",
   L"BLOCKQUOTE closes P P* HEAD",
   L"DL closes P P* DT MENU DIR ADDRESS PRE LISTING XMP HEAD",
   L"DT closes P P* MENU DIR ADDRESS PRE LISTING XMP HEAD",
   L"DD closes P P* MENU DIR ADDRESS PRE LISTING XMP HEAD",
   L"UL closes P P* HEAD OL MENU DIR ADDRESS PRE LISTING XMP",
   L"OL closes P P* HEAD UL",
   L"MENU closes P P* HEAD UL",
   L"P closes P P* HEAD H1 H2 H3 H4 H5 H6",
   L"P* closes P P* HEAD",
   L"DIV closes P P* HEAD",
   L"NOSCRIPT closes P P* HEAD",
   L"CENTER closes FONT B I P P* HEAD",
   L"A closes A",
   L"CAPTION closes P P*",
   L"COLGROUP closes CAPTION COLGROUP COL P P*",
   L"COL closes CAPTION COL P P*",
   L"TABLE closes P P* HEAD H1 H2 H3 H4 H5 H6 PRE LISTING XMP A",
   L"TH closes TH TD",
   L"TD closes TH TD",
   L"TR closes TH TD TR CAPTION COL COLGROUP",
   L"THEAD closes CAPTION COL COLGROUP",
   L"TFOOT closes TH TD TR CAPTION COL COLGROUP THEAD TBODY",
   L"TBODY closes TH TD TR CAPTION COL COLGROUP THEAD TFOOT TBODY",
   L"OPTGROUP closes OPTION",
   L"FIELDSET closes LEGEND P P* HEAD H1 H2 H3 H4 H5 H6 PRE LISTING XMP A",
   L""
#  else /* !defined(_I18N_) && !defined(__JIS__) */
   "FORM closes FORM P P* HR H1 H2 H3 H4 H5 H6 DL UL OL MENU DIR ADDRESS PRE LISTING XMP HEAD",
   "HEAD closes P P*",
   "TITLE closes P P*",
   "BODY closes HEAD STYLE LINK TITLE P P*",
   "LI closes P P* H1 H2 H3 H4 H5 H6 DL ADDRESS PRE LISTING XMP HEAD",
   "HR closes P P* HEAD",
   "H1 closes P P* HEAD",
   "H2 closes P P* HEAD",
   "H3 closes P P* HEAD",
   "H4 closes P P* HEAD",
   "H5 closes P P* HEAD",
   "H6 closes P P* HEAD",
   "DIR closes P P* HEAD",
   "ADDRESS closes P P* HEAD UL",
   "PRE closes P P* HEAD UL",
   "LISTING closes P P* HEAD",
   "XMP closes P P* HEAD",
   "BLOCKQUOTE closes P P* HEAD",
   "DL closes P P* DT MENU DIR ADDRESS PRE LISTING XMP HEAD",
   "DT closes P P* MENU DIR ADDRESS PRE LISTING XMP HEAD",
   "DD closes P P* MENU DIR ADDRESS PRE LISTING XMP HEAD",
   "UL closes P P* HEAD OL MENU DIR ADDRESS PRE LISTING XMP",
   "OL closes P P* HEAD UL",
   "MENU closes P P* HEAD UL",
   "P closes P P* HEAD H1 H2 H3 H4 H5 H6",
   "P* closes P P* HEAD",
   "DIV closes P P* HEAD",
   "NOSCRIPT closes P P* HEAD",
   "CENTER closes FONT B I P P* HEAD",
   "A closes A",
   "CAPTION closes P P*",
   "COLGROUP closes CAPTION COLGROUP COL P P*",
   "COL closes CAPTION COL P P*",
   "TABLE closes P P* HEAD H1 H2 H3 H4 H5 H6 PRE LISTING XMP A",
   "TH closes TH TD",
   "TD closes TH TD",
   "TR closes TH TD TR CAPTION COL COLGROUP",
   "THEAD closes CAPTION COL COLGROUP",
   "TFOOT closes TH TD TR CAPTION COL COLGROUP THEAD TBODY",
   "TBODY closes TH TD TR CAPTION COL COLGROUP THEAD TFOOT TBODY",
   "OPTGROUP closes OPTION",
   "FIELDSET closes LEGEND P P* HEAD H1 H2 H3 H4 H5 H6 PRE LISTING XMP A",
   ""
#endif /* defined(_I18N_) || defined(__JIS__) */
};


/* mapping table of HTML attribute values */

static AttrValueMapping HTMLAttrValueMappingTable[] =
{
   {HTML_ATTR_dir, TEXT("LTR"), HTML_ATTR_dir_VAL_ltr},
   {HTML_ATTR_dir, TEXT("RTL"), HTML_ATTR_dir_VAL_rtl},

   {HTML_ATTR_TextAlign, TEXT("LEFT"), HTML_ATTR_TextAlign_VAL_left_},
   {HTML_ATTR_TextAlign, TEXT("CENTER"), HTML_ATTR_TextAlign_VAL_center_},
   {HTML_ATTR_TextAlign, TEXT("RIGHT"), HTML_ATTR_TextAlign_VAL_right_},
   {HTML_ATTR_TextAlign, TEXT("JUSTIFY"), HTML_ATTR_TextAlign_VAL_justify_},

   {HTML_ATTR_Align, TEXT("LEFT"), HTML_ATTR_Align_VAL_left_},
   {HTML_ATTR_Align, TEXT("CENTER"), HTML_ATTR_Align_VAL_center_},
   {HTML_ATTR_Align, TEXT("RIGHT"), HTML_ATTR_Align_VAL_right_},

   {HTML_ATTR_LAlign, TEXT("TOP"), HTML_ATTR_LAlign_VAL_Top_},
   {HTML_ATTR_LAlign, TEXT("BOTTOM"), HTML_ATTR_LAlign_VAL_Bottom_},
   {HTML_ATTR_LAlign, TEXT("LEFT"), HTML_ATTR_LAlign_VAL_Left_},
   {HTML_ATTR_LAlign, TEXT("RIGHT"), HTML_ATTR_LAlign_VAL_Right_},

   {HTML_ATTR_Clear, TEXT("LEFT"), HTML_ATTR_Clear_VAL_Left_},
   {HTML_ATTR_Clear, TEXT("RIGHT"), HTML_ATTR_Clear_VAL_Right_},
   {HTML_ATTR_Clear, TEXT("ALL"), HTML_ATTR_Clear_VAL_All_},
   {HTML_ATTR_Clear, TEXT("NONE"), HTML_ATTR_Clear_VAL_None},

   {HTML_ATTR_NumberStyle, TEXT("1"), HTML_ATTR_NumberStyle_VAL_Arabic_},
   {HTML_ATTR_NumberStyle, TEXT("a"), HTML_ATTR_NumberStyle_VAL_LowerAlpha},
   {HTML_ATTR_NumberStyle, TEXT("A"), HTML_ATTR_NumberStyle_VAL_UpperAlpha},
   {HTML_ATTR_NumberStyle, TEXT("i"), HTML_ATTR_NumberStyle_VAL_LowerRoman},
   {HTML_ATTR_NumberStyle, TEXT("I"), HTML_ATTR_NumberStyle_VAL_UpperRoman},

   {HTML_ATTR_BulletStyle, TEXT("DISC"), HTML_ATTR_BulletStyle_VAL_disc},
   {HTML_ATTR_BulletStyle, TEXT("SQUARE"), HTML_ATTR_BulletStyle_VAL_square},
   {HTML_ATTR_BulletStyle, TEXT("CIRCLE"), HTML_ATTR_BulletStyle_VAL_circle},

   {HTML_ATTR_ItemStyle, TEXT("1"), HTML_ATTR_ItemStyle_VAL_Arabic_},
   {HTML_ATTR_ItemStyle, TEXT("a"), HTML_ATTR_ItemStyle_VAL_LowerAlpha},
   {HTML_ATTR_ItemStyle, TEXT("A"), HTML_ATTR_ItemStyle_VAL_UpperAlpha},
   {HTML_ATTR_ItemStyle, TEXT("i"), HTML_ATTR_ItemStyle_VAL_LowerRoman},
   {HTML_ATTR_ItemStyle, TEXT("I"), HTML_ATTR_ItemStyle_VAL_UpperRoman},
   {HTML_ATTR_ItemStyle, TEXT("DISC"), HTML_ATTR_ItemStyle_VAL_disc},
   {HTML_ATTR_ItemStyle, TEXT("SQUARE"), HTML_ATTR_ItemStyle_VAL_square},
   {HTML_ATTR_ItemStyle, TEXT("CIRCLE"), HTML_ATTR_ItemStyle_VAL_circle},

   {HTML_ATTR_Button_type, TEXT("BUTTON"), HTML_ATTR_Button_type_VAL_button},
   {HTML_ATTR_Button_type, TEXT("SUBMIT"), HTML_ATTR_Button_type_VAL_submit},
   {HTML_ATTR_Button_type, TEXT("RESET"), HTML_ATTR_Button_type_VAL_reset},

   {HTML_ATTR_mode, TEXT("DISPLAY"), HTML_ATTR_mode_VAL_display},
   {HTML_ATTR_mode, TEXT("INLINE"), HTML_ATTR_mode_VAL_inline_math},

   {HTML_ATTR_frame, TEXT("VOID"), HTML_ATTR_frame_VAL_void},
   {HTML_ATTR_frame, TEXT("ABOVE"), HTML_ATTR_frame_VAL_above},
   {HTML_ATTR_frame, TEXT("BELOW"), HTML_ATTR_frame_VAL_below},
   {HTML_ATTR_frame, TEXT("HSIDES"), HTML_ATTR_frame_VAL_hsides},
   {HTML_ATTR_frame, TEXT("LHS"), HTML_ATTR_frame_VAL_lhs},
   {HTML_ATTR_frame, TEXT("RHS"), HTML_ATTR_frame_VAL_rhs},
   {HTML_ATTR_frame, TEXT("VSIDES"), HTML_ATTR_frame_VAL_vsides},
   {HTML_ATTR_frame, TEXT("BOX"), HTML_ATTR_frame_VAL_box},
   {HTML_ATTR_frame, TEXT("BORDER"), HTML_ATTR_frame_VAL_border},

   {HTML_ATTR_frameborder, TEXT("0"), HTML_ATTR_frameborder_VAL_Border0},
   {HTML_ATTR_frameborder, TEXT("1"), HTML_ATTR_frameborder_VAL_Border1},

   {HTML_ATTR_scrolling, TEXT("YES"), HTML_ATTR_scrolling_VAL_Yes_},
   {HTML_ATTR_scrolling, TEXT("NO"), HTML_ATTR_scrolling_VAL_No_},
   {HTML_ATTR_scrolling, TEXT("AUTO"), HTML_ATTR_scrolling_VAL_auto},

   {HTML_ATTR_rules_, TEXT("NONE"), HTML_ATTR_rules__VAL_none},
   {HTML_ATTR_rules_, TEXT("GROUPS"), HTML_ATTR_rules__VAL_groups},
   {HTML_ATTR_rules_, TEXT("ROWS"), HTML_ATTR_rules__VAL_rows},
   {HTML_ATTR_rules_, TEXT("COLS"), HTML_ATTR_rules__VAL_cols},
   {HTML_ATTR_rules_, TEXT("ALL"), HTML_ATTR_rules__VAL_all},

   {HTML_ATTR_Cell_align, TEXT("LEFT"), HTML_ATTR_Cell_align_VAL_Cell_left},
   {HTML_ATTR_Cell_align, TEXT("CENTER"), HTML_ATTR_Cell_align_VAL_Cell_center},
   {HTML_ATTR_Cell_align, TEXT("RIGHT"), HTML_ATTR_Cell_align_VAL_Cell_right},
   {HTML_ATTR_Cell_align, TEXT("JUSTIFY"), HTML_ATTR_Cell_align_VAL_Cell_justify},
   {HTML_ATTR_Cell_align, TEXT("CHAR"), HTML_ATTR_Cell_align_VAL_Cell_char},

   {HTML_ATTR_Alignment, TEXT("TOP"), HTML_ATTR_Alignment_VAL_Top_},
   {HTML_ATTR_Alignment, TEXT("MIDDLE"), HTML_ATTR_Alignment_VAL_Middle_},
   {HTML_ATTR_Alignment, TEXT("BOTTOM"), HTML_ATTR_Alignment_VAL_Bottom_},
   {HTML_ATTR_Alignment, TEXT("LEFT"), HTML_ATTR_Alignment_VAL_Left_},
   {HTML_ATTR_Alignment, TEXT("RIGHT"), HTML_ATTR_Alignment_VAL_Right_},

   {HTML_ATTR_METHOD, TEXT("GET"), HTML_ATTR_METHOD_VAL_Get_},
   {HTML_ATTR_METHOD, TEXT("POST"), HTML_ATTR_METHOD_VAL_Post_},

   {HTML_ATTR_Position, TEXT("TOP"), HTML_ATTR_Position_VAL_Position_top},
   {HTML_ATTR_Position, TEXT("BOTTOM"), HTML_ATTR_Position_VAL_Position_bottom},
   {HTML_ATTR_Position, TEXT("LEFT"), HTML_ATTR_Position_VAL_Position_left},
   {HTML_ATTR_Position, TEXT("RIGHT"), HTML_ATTR_Position_VAL_Position_right},

   {HTML_ATTR_Row_valign, TEXT("TOP"), HTML_ATTR_Row_valign_VAL_Row_top},
   {HTML_ATTR_Row_valign, TEXT("MIDDLE"), HTML_ATTR_Row_valign_VAL_Row_middle},
   {HTML_ATTR_Row_valign, TEXT("BOTTOM"), HTML_ATTR_Row_valign_VAL_Row_bottom},
   {HTML_ATTR_Row_valign, TEXT("BASELINE"), HTML_ATTR_Row_valign_VAL_Row_baseline},

   {HTML_ATTR_Cell_valign, TEXT("TOP"), HTML_ATTR_Cell_valign_VAL_Cell_top},
   {HTML_ATTR_Cell_valign, TEXT("MIDDLE"), HTML_ATTR_Cell_valign_VAL_Cell_middle},
   {HTML_ATTR_Cell_valign, TEXT("BOTTOM"), HTML_ATTR_Cell_valign_VAL_Cell_bottom},
   {HTML_ATTR_Cell_valign, TEXT("BASELINE"), HTML_ATTR_Cell_valign_VAL_Cell_baseline},

   {HTML_ATTR_shape, TEXT("RECT"), HTML_ATTR_shape_VAL_rectangle},
   {HTML_ATTR_shape, TEXT("CIRCLE"), HTML_ATTR_shape_VAL_circle},
   {HTML_ATTR_shape, TEXT("POLY"), HTML_ATTR_shape_VAL_polygon},

   {HTML_ATTR_valuetype, TEXT("DATA"), HTML_ATTR_valuetype_VAL_data_},
   {HTML_ATTR_valuetype, TEXT("REF"), HTML_ATTR_valuetype_VAL_ref},
   {HTML_ATTR_valuetype, TEXT("OBJECT"), HTML_ATTR_valuetype_VAL_object_},

/* HTML attribute TYPE generates a Thot element */
   {DummyAttribute, TEXT("BUTTON"), HTML_EL_Button_Input},
   {DummyAttribute, TEXT("CHECKBOX"), HTML_EL_Checkbox_Input},
   {DummyAttribute, TEXT("FILE"), HTML_EL_File_Input},
   {DummyAttribute, TEXT("HIDDEN"), HTML_EL_Hidden_Input},
   {DummyAttribute, TEXT("IMAGE"), HTML_EL_PICTURE_UNIT},
   {DummyAttribute, TEXT("PASSWORD"), HTML_EL_Password_Input},
   {DummyAttribute, TEXT("RADIO"), HTML_EL_Radio_Input},
   {DummyAttribute, TEXT("RESET"), HTML_EL_Reset_Input},
   {DummyAttribute, TEXT("SUBMIT"), HTML_EL_Submit_Input},
   {DummyAttribute, TEXT("TEXT"), HTML_EL_Text_Input},

/* The following declarations allow the parser to accept boolean attributes */
/* written "checked=CHECKED"), for instance */
   {HTML_ATTR_ISMAP, TEXT("ISMAP"), HTML_ATTR_ISMAP_VAL_Yes_},
   {HTML_ATTR_nohref, TEXT("NOHREF"), HTML_ATTR_nohref_VAL_Yes_},
   {HTML_ATTR_COMPACT, TEXT("COMPACT"), HTML_ATTR_COMPACT_VAL_Yes_},
   {HTML_ATTR_Multiple, TEXT("MULTIPLE"), HTML_ATTR_Multiple_VAL_Yes_},
   {HTML_ATTR_Selected, TEXT("SELECTED"), HTML_ATTR_Selected_VAL_Yes_},
   {HTML_ATTR_Checked, TEXT("CHECKED"), HTML_ATTR_Checked_VAL_Yes_},
   {HTML_ATTR_No_wrap, TEXT("NOWRAP"), HTML_ATTR_No_wrap_VAL_no_wrap},
   {HTML_ATTR_NoShade, TEXT("NOSHADE"), HTML_ATTR_NoShade_VAL_NoShade_},
   {HTML_ATTR_declare, TEXT("DECLARE"), HTML_ATTR_declare_VAL_Yes_},
   {HTML_ATTR_defer, TEXT("DEFER"), HTML_ATTR_defer_VAL_Yes_},
   {HTML_ATTR_disabled, TEXT("DISABLED"), HTML_ATTR_disabled_VAL_Yes_},
   {HTML_ATTR_readonly, TEXT("READONLY"), HTML_ATTR_readonly_VAL_Yes_},
   {HTML_ATTR_no_resize, TEXT("NORESIZE"), HTML_ATTR_no_resize_VAL_Yes_},
   {0, TEXT(""), 0}			/* Last entry. Mandatory */
};

typedef int         State;	/* a state of the automaton */

/* ---------------------- static variables ---------------------- */
/* parser stack */
#define MaxStack 200		/* maximum stack height */
static int          GINumberStack[MaxStack]; /* entry of GIMappingTable */
static Element      ElementStack[MaxStack];  /* element in the Thot abstract
						tree */
static int          ThotLevel[MaxStack];     /* level of element in the Thot
						tree */
static Language	    LanguageStack[MaxStack]; /* element language */
static int          StackLevel = 0;	     /* first free element on the
						stack */
/* information about the input file */
#define INPUT_FILE_BUFFER_SIZE 2000
static CHAR_T	    FileBuffer[INPUT_FILE_BUFFER_SIZE+1];
#ifdef _I18N_
static char         FileBufferA[INPUT_FILE_BUFFER_SIZE+1];
#endif
static int	    LastCharInFileBuffer = 0; /* last char. in the buffer */
static int          CurrentBufChar;           /* current character read */
static CHAR_T	    PreviousBufChar = EOS;    /* previous character read */
static STRING       InputText;
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
static ThotBool     ParsingCSS = FALSE;	  /* reading the content of a STYLE
					     element */
static ThotBool     ParsingTextArea = FALSE; /* reading the content of a text area
					     element */
static int          WithinTable = 0;      /* <TABLE> has been read */
static STRING       docURL = NULL;	  /* path or URL of the document */

/* input buffer */
#define MaxBufferLength 1000
#define AllmostFullBuffer 700
static UCHAR_T      inputBuffer[MaxBufferLength];
static int          LgBuffer = 0;	  /* actual length of text in input
					     buffer */
static int	    BufferLineNumber = 0; /* line number in the source file of
					     the beginning of the text
					     contained in the buffer */

/* information about the Thot document under construction */
static Document     theDocument = 0;	  /* the Thot document */
static Language     currentLanguage;	  /* language used in the document */
static SSchema      DocumentSSchema = NULL;  /* the HTML structure schema */
static Element      rootElement;	  /* root element of the document */
static Element      lastElement = NULL;	  /* last element created */
static ThotBool     lastElementClosed = FALSE;/* last element is complete */
static int          lastElemEntry = 0;	  /* index in the GIMappingTable of the
					     element being created */
static Attribute    lastAttribute = NULL; /* last attribute created */
static Attribute    lastAttrElement = NULL;/* element with which the last
					     attribute has been associated */
static AttributeMapping* lastAttrEntry = NULL; /* entry in the AttributeMappingTable
					     of the attribute being created */
static ThotBool     UnknownAttr = FALSE;  /* the last attribute encountered is
					     invalid */
static ThotBool     ReadingAnAttrValue = FALSE;
static Element      CommentText = NULL;	  /* TEXT element of the current
					     Comment element */
static ThotBool     UnknownTag = FALSE;	  /* the last start tag encountered is
					     invalid */
static ThotBool     MergeText = FALSE;	  /* character data should be catenated
					     with the last Text element */
static ThotBool     HTMLrootClosed = FALSE;
static STRING       HTMLrootClosingTag = NULL;

static PtrElemToBeChecked FirstElemToBeChecked = NULL;
static PtrElemToBeChecked LastElemToBeChecked = NULL;

/* automaton */
static State        currentState;	  /* current state of the automaton */
static State        returnState;	  /* return state from subautomaton */
static ThotBool     NormalTransition;

/* information about an entity being read */
#define MaxEntityLength 50
static CHAR_T       EntityName[MaxEntityLength];/* name of entity being read */
static int          LgEntityName = 0;	  /* length of entity name read so
					     far */
static int          EntityTableEntry = 0; /* entry of the entity table that
					     matches the entity read so far */
static int          CharRank = 0;	  /* rank of the last matching
					     character in that entry */

#define MaxMsgLength 200	/* maximum size of error messages */

#ifdef __STDC__
static void         ProcessStartGI (STRING GIname);
#else
static void         ProcessStartGI ();
#endif

static FILE*   ErrFile = (FILE*) 0;
static CHAR_T    ErrFileName [80];


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING       SkipSep (STRING ptr)
#else
STRING       SkipSep (ptr)
STRING              ptr;
#endif
{
  while (*ptr == SPACE || *ptr == ',')
    ptr++;
  return (ptr);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING       SkipInt (STRING ptr)
#else
STRING       SkipInt (ptr)
STRING              ptr;
#endif
{
  while (*ptr != EOS && *ptr != SPACE && *ptr != ',')
    ptr++;
  return (ptr);
}

/*----------------------------------------------------------------------
   ParseAreaCoords computes x, y, width and height of the box from
   the coords attribute value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ParseAreaCoords (Element element, Document document)
#else
void                ParseAreaCoords (element, document)
Element             element;
Document            document;
#endif
{
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attrCoords, attrX, attrY;
   Attribute           attrW, attrH, attrShape;
   STRING              ptr3, text;
   int                 x1, y1, x2, y2;
   int                 length, shape, r;

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
   text = TtaAllocString (length + 1);
   TtaGiveTextAttributeValue (attrCoords, text, &length);

   if (shape == HTML_ATTR_shape_VAL_rectangle || shape == HTML_ATTR_shape_VAL_circle)
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
	attrType.AttrTypeNum = HTML_ATTR_height_;
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
	       usscanf (ptr3, TEXT("%d"), &x1);
	     ptr3 = SkipInt (ptr3);
	     ptr3 = SkipSep (ptr3);
	     if (ptr3)
	       usscanf (ptr3, TEXT("%d"), &y1);
	     ptr3 = SkipInt (ptr3);
	     ptr3 = SkipSep (ptr3);
	     if (ptr3)
	       usscanf (ptr3, TEXT("%d"), &x2);
	     ptr3 = SkipInt (ptr3);
	     ptr3 = SkipSep (ptr3);
	     usscanf (ptr3, TEXT("%d"), &y2);
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
	       usscanf (ptr3, TEXT("%d"), &x1);
	     ptr3 = SkipInt (ptr3);
	     ptr3 = SkipSep (ptr3);
	     if (ptr3)
	       usscanf (ptr3, TEXT("%d"), &y1);
	     ptr3 = SkipInt (ptr3);
	     ptr3 = SkipSep (ptr3);
	     if (ptr3)
	       usscanf (ptr3, TEXT("%d"), &r);
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
	     usscanf (ptr3, TEXT("%d"), &x1);
	     ptr3 = SkipInt (ptr3);
	     ptr3 = SkipSep (ptr3);
	     if (ptr3)
	       usscanf (ptr3, TEXT("%d"), &y1);
	     ptr3 = SkipInt (ptr3);
	     ptr3 = SkipSep (ptr3);
	     TtaAddPointInPolyline (element, length, UnPixel, x1, y1, document);
	     length++;
	  }
     }
   TtaFreeMemory (text);
}

/*----------------------------------------------------------------------
   GITagNameByType search in the mapping tables the name for a given type
  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING              GITagNameByType (ElementType elType)
#else
STRING              GITagNameByType (elType)
ElementType elType;

#endif
{
  int		i;
  STRING	buffer;

  if (elType.ElTypeNum > 0)
    {
      i = 0;
      if (ustrcmp (TEXT("HTML"), TtaGetSSchemaName (elType.ElSSchema)) == 0)
	do
	  {
	    if (pHTMLGIMapping[i].ThotType == elType.ElTypeNum &&
		ustrcmp (pHTMLGIMapping[i].htmlGI, TEXT("LISTING")))	/* use PRE */
	      return (STRING) pHTMLGIMapping[i].htmlGI;
	    i++;
	  }
	while (pHTMLGIMapping[i].htmlGI[0] != EOS);
#ifndef STANDALONE
      else
	{
	  GetXMLElementName (elType, &buffer);
	  return buffer;
	}
#endif
    }
  return TEXT("???");
}

/*----------------------------------------------------------------------
   GITagName search in GIMappingTable the name for a given element
  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING              GITagName (Element elem)
#else
STRING              GITagName (elem)
Element             elem;

#endif
{
   ElementType         elType;

   elType = TtaGetElementType (elem);
   return (GITagNameByType (elType));
}

/*----------------------------------------------------------------------
   MapThotAttr     search in AttributeMappingTable the entry for
   the attribute of name Attr and returns the Thot Attribute
   corresponding to the rank of that entry.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 MapThotAttr (STRING Attr, STRING tag)
#else
int                 MapThotAttr (Attr, tag)
STRING              Attr;
STRING              tag;

#endif
{
   int                 thotAttr;
   SSchema	       schema;
   AttributeMapping*   tableEntry;

   thotAttr = -1;
   lastElemEntry = -1;
   schema = DocumentSSchema;
   if (tag[0] != EOS)
   lastElemEntry = MapGI (tag, &schema, theDocument);
   if (lastElemEntry >= 0 || tag[0] == EOS)
     {
	tableEntry = MapAttr (Attr, &schema, lastElemEntry, theDocument);
	if (tableEntry != NULL)
	   thotAttr = tableEntry->ThotAttribute;
     }
   return thotAttr;
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
   InitMapping     intialise the list of the elements closed by
   each start tag.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitMapping (void)
#else
void                InitMapping ()
#endif
{
   int                 line;
   int                 entry;
   int                 ptr;
   int                 i;
   GI                  name;
   PtrClosedElement    newCE, lastCE, firstCE, curCE;
   SSchema	       schema;

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
	     /* read one GI */
	     i = 0;
	     while (EquivEndingElem[line][ptr] != SPACE && EquivEndingElem[line][ptr] != EOS)
		name[i++] = EquivEndingElem[line][ptr++];
	     name[i] = EOS;
	     ptr++;
	     if (i > 0)
		/* a GI has been read */
	       {
		  schema = DocumentSSchema;
		  entry = MapGI (name, &schema, theDocument);
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
	     if (pHTMLGIMapping[curCE->tagNum].firstClosedElem == NULL)
		pHTMLGIMapping[curCE->tagNum].firstClosedElem = newCE;
	     else
	       {
		  lastCE = pHTMLGIMapping[curCE->tagNum].firstClosedElem;
		  while (lastCE->nextClosedElem != NULL)
		     lastCE = lastCE->nextClosedElem;
		  lastCE->nextClosedElem = newCE;
	       }
	     curCE = curCE->nextClosedElem;
	  }

	line++;
     }
   while (ustrcmp (EquivEndingElem[line], _EMPTYSTR_) != 0);

   /* read table StartTagEndingElem */
   line = 0;
   do
      /* read one line of StartTagEndingElem */
     {
	ptr = 0;
	i = 0;
	/* read the first tag name of the line */
	while (StartTagEndingElem[line][ptr] != SPACE && StartTagEndingElem[line][ptr] != EOS)
	   name[i++] = StartTagEndingElem[line][ptr++];
	name[i] = EOS;
	i = 0;
	ptr++;
	schema = DocumentSSchema;
	entry = MapGI (name, &schema, theDocument);
#ifdef DEBUG
	if (entry < 0)
	   fprintf (stderr, "error in StartTagEndingElem: tag %s unknown in line\n%s\n", name, StartTagEndingElem[line]);
#endif
	/* read the keyword "closes" */
	while (StartTagEndingElem[line][ptr] != SPACE && StartTagEndingElem[line][ptr] != EOS)
	   name[i++] = StartTagEndingElem[line][ptr++];
	name[i] = EOS;
	i = 0;
	ptr++;
#ifdef DEBUG
	if (ustrcmp (name, TEXT("closes")) != 0)
	   fprintf (stderr, "error in StartTagEndingElem: \"%s\" instead of \"closes\" in line\n%s\n", name, StartTagEndingElem[line]);
#endif
	lastCE = pHTMLGIMapping[entry].firstClosedElem;
	if (lastCE != NULL)
	   while (lastCE->nextClosedElem != NULL)
	      lastCE = lastCE->nextClosedElem;
	do
	  {
	     while (StartTagEndingElem[line][ptr] != SPACE && StartTagEndingElem[line][ptr] != EOS)
		name[i++] = StartTagEndingElem[line][ptr++];
	     name[i] = EOS;
	     ptr++;
	     if (i > 0)
	       {
		  i = 0;
		  newCE = (PtrClosedElement) TtaGetMemory (sizeof (ClosedElement));
		  newCE->nextClosedElem = NULL;
		  schema = DocumentSSchema;
		  newCE->tagNum = MapGI (name, &schema, theDocument);
#ifdef DEBUG
		  if (newCE->tagNum < 0)
		     fprintf (stderr, "error in StartTagEndingElem: tag %s unknown in line\n%s\n", name, StartTagEndingElem[line]);
#endif
		  if (lastCE == NULL)
		     pHTMLGIMapping[entry].firstClosedElem = newCE;
		  else
		     lastCE->nextClosedElem = newCE;
		  lastCE = newCE;
	       }
	  }
	while (StartTagEndingElem[line][ptr] != EOS);
	line++;
     }
   while (ustrcmp (StartTagEndingElem[line], _EMPTYSTR_) != 0);
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
   ParseHTMLError  print the error message msg on stderr.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ParseHTMLError (Document doc, USTRING msg)
#else
void                ParseHTMLError (doc, msg)
Document            doc;
USTRING             msg;

#endif
{
   HTMLErrorsFound = TRUE;
   if (!ErrFile)
      {
      usprintf (ErrFileName, TEXT("%s%c%d%cHTML.ERR"), TempFileDirectory,
		DIR_SEP, doc, DIR_SEP);
      if ((ErrFile = ufopen (ErrFileName, _WriteMODE_)) == NULL)
         return;
      }

   if (doc == theDocument)
      /* the error message is related to the document being parsed */
      {
      if (docURL != NULL)
	 {
	 fprintf (ErrFile, "*** Errors in %s\n", docURL);
#ifndef STANDALONE
	 TtaFreeMemory (docURL);
#endif /* STANDALONE */
	 docURL = NULL;
	 }
      /* print the line number and character number before the message */
      fprintf (ErrFile, "   line %d, char %d: %s\n", NumberOfLinesRead,
	       NumberOfCharRead, msg);
      }
   else
      /* print only the error message */
      fprintf (ErrFile, "%s\n", msg);
}

/*----------------------------------------------------------------------
   CloseBuffer     close the input buffer.
  ----------------------------------------------------------------------*/
static void         CloseBuffer ()
{
   inputBuffer[LgBuffer] = EOS;
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
   IsCharacterLevelElement return TRUE if element el is a
   character level element, FALSE if not.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            IsCharacterLevelElement (Element el)
#else
ThotBool            IsCharacterLevelElement (el)
Element             el;

#endif
{
   ElementType         elType;
   int                 i;
   AttributeType       attrType;
   Attribute	       attr;
   ThotBool            ret;

   ret = FALSE;
   elType = TtaGetElementType (el);
   i = 0;
   while (CharLevelElement[i] > 0 &&
	  CharLevelElement[i] != elType.ElTypeNum)
      i++;
   if (CharLevelElement[i] == elType.ElTypeNum)
      {
      ret = TRUE;
      /* a Math element is a block element if it has an attribute mode=display */
      if (elType.ElTypeNum == HTML_EL_Math)
	{
	attrType.AttrSSchema = elType.ElSSchema;
	attrType.AttrTypeNum = HTML_ATTR_mode;
	attr = TtaGetAttribute (el, attrType);
	if (attr)
	   if (TtaGetAttributeValue (attr) == HTML_ATTR_mode_VAL_display)
	      ret = FALSE;
	}
      }
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

   if (ustrcmp (TtaGetSSchemaName(elType.ElSSchema), TEXT("HTML")))
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
	      while (inputBuffer[i] <= SPACE && inputBuffer[i] != EOS)
		 i++;
	if (inputBuffer[i] != EOS)
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
   StartOfTag      Beginning of a HTML tag (start or end tag).
   Put the preceding text into the Thot document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         StartOfTag (CHAR_T c)
#else
static void         StartOfTag (c)
CHAR_T                c;

#endif
{
   if (LgBuffer > 0)
      TextToDocument ();
   MergeText = FALSE;
}

/*----------------------------------------------------------------------
   PutInBuffer     put character c in the input buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutInBuffer (UCHAR_T c)
#else
static void         PutInBuffer (c)
UCHAR_T               c;
#endif
{
  int                 len;

  /* put the character into the buffer if it is not an ignored char. */
  if ((int) c == TAB)		/* HT */
    len = 8;			/* HT = 8 spaces */
  else
    len = 1;
  if (c != EOS)
    {
      if (LgBuffer + len >= AllmostFullBuffer && currentState == 0)
	TextToDocument ();
      if (LgBuffer + len >= MaxBufferLength)
	{
	  if (currentState == 0)
	    TextToDocument ();
	  else
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
static void         CreateAttr (Element el, AttributeType attrType, STRING text, ThotBool invalid)
#else
static void         CreateAttr (el, attrType, text, invalid)
Element             el;
AttributeType       attrType;
STRING              text;
ThotBool            invalid;

#endif
{
   int                 attrKind;
   int                 length;
   STRING              buffer;
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
   	OnlyOneOptionSelected
	If the option menu is a single-choice menu, check that only
	one option has an attribute Selected.
	Check that at least one option has an attribute Selected.
	If parsing is TRUE, associate an attribute DefaultSelected with
	each option having an attribute Selected.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                OnlyOneOptionSelected (Element el, Document doc, ThotBool parsing)
#else  /* __STDC__ */
void                OnlyOneOptionSelected (el, doc, parsing)
Element             el;
Document            doc;
ThotBool            parsing;

#endif /* __STDC__ */
{
   ElementType         elType;
   Element             option, menu, firstOption, child;
   AttributeType       attrType;
   Attribute           attr;
   ThotBool	       multiple;

   if (el == NULL)
      return;

   menu = NULL;
   elType = TtaGetElementType (el);

   if (elType.ElTypeNum == HTML_EL_Option_Menu)
     {
	/* it's a menu (SELECT) */
	menu = el;
	/* search the first OPTION element having an attribute SELECTED */
	attr = NULL;
	attrType.AttrSSchema = elType.ElSSchema;
	attrType.AttrTypeNum = HTML_ATTR_Selected;
	option = TtaGetFirstChild (el);
	firstOption = NULL;
	while (option && !attr)
	  {
	     elType = TtaGetElementType (option);
	     if (elType.ElTypeNum == HTML_EL_Option)
		{
		if (!firstOption)
		    firstOption = option;
		attr = TtaGetAttribute (option, attrType);
		}
	     else if (elType.ElTypeNum == HTML_EL_OptGroup)
		{
		child = TtaGetFirstChild (option);
		while (child && !attr)
		   {
		   elType = TtaGetElementType (child);
		   if (elType.ElTypeNum == HTML_EL_Option)
		      {
		      if (!firstOption)
		         firstOption = child;
		      attr = TtaGetAttribute (child, attrType);
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
	if (option)
	  el = option;
	else
	  el = firstOption;
     }
   else
     {
     menu = NULL;
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
	/* set this option SELECTED */
	attrType.AttrSSchema = elType.ElSSchema;
	attrType.AttrTypeNum = HTML_ATTR_Selected;
	attr = TtaGetAttribute (el, attrType);
	if (attr == NULL)
	  {
	     /* create the SELECTED attribute */
	     attr = TtaNewAttribute (attrType);
	     TtaAttachAttribute (el, attr, doc);
	     TtaSetAttributeValue (attr, HTML_ATTR_Selected_VAL_Yes_, el, doc);
	  }

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
	        while (option)
	          {
		  elType = TtaGetElementType (option);
		  if (elType.ElTypeNum == HTML_EL_Option)
		     ProcessOptionElement (option, el, doc, multiple, parsing);
	          else if (elType.ElTypeNum == HTML_EL_OptGroup)
		     {
		     child = TtaGetFirstChild (option);
		     while (child)
		        {
		        elType = TtaGetElementType (child);
		        if (elType.ElTypeNum == HTML_EL_Option)
			   ProcessOptionElement (child, el, doc, multiple, parsing);
		        TtaNextSibling (&child);
		        }
		     }
		  TtaNextSibling (&option);
	          }
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
   STRING              name1, name2;
#ifdef STANDALONE
   STRING              imageName;
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
	   child = TtaNewTree (theDocument, elType, _EMPTYSTR_);
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
	   desc = TtaNewTree (theDocument, elType, _EMPTYSTR_);
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
	child = TtaNewTree (theDocument, elType, _EMPTYSTR_);
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
	   child = TtaNewTree (theDocument, newElType, _EMPTYSTR_);
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
       constElem = TtaNewTree (theDocument, newElType, _EMPTYSTR_);
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
       /* A LINK element is complete. If it is a link to a style sheet, */
       /* load that style sheet. */
       attrType.AttrSSchema = DocumentSSchema;
       attrType.AttrTypeNum = HTML_ATTR_REL;
       attr = TtaGetAttribute (el, attrType);
       if (attr != NULL)
	  /* there is an attribute REL */
	 {
	    length = TtaGetTextAttributeLength (attr);
	    name1 = TtaAllocString (length + 1);
	    TtaGiveTextAttributeValue (attr, name1, &length);
	    if (!ustrcasecmp (name1, TEXT("STYLESHEET")) || !ustrcasecmp (name1, TEXT("STYLE")))
	      {
		 /* it's a link to a style sheet. Load that style sheet */
		 attrType.AttrSSchema = DocumentSSchema;
		 attrType.AttrTypeNum = HTML_ATTR_HREF_;
		 attr = TtaGetAttribute (el, attrType);
		 if (attr != NULL)
		   {
		      length = TtaGetTextAttributeLength (attr);
		      name2 = TtaAllocString (length + 1);
		      TtaGiveTextAttributeValue (attr, name2, &length);
		      /* load the stylesheet file found here ! */
		      LoadStyleSheet (name2, theDocument, el, NULL);
		      TtaFreeMemory (name2);
		   }
	      }		/* other kind of Links ... */
	    TtaFreeMemory (name1);
	 }
       break;
#endif /* STANDALONE */

    case HTML_EL_Data_cell:
    case HTML_EL_Heading_cell:
      /* insert a pseudo paragraph into empty cells */
       child = TtaGetFirstChild (el);
       if (child == NULL)
	 {
	   elType.ElTypeNum = HTML_EL_Pseudo_paragraph;
	   child = TtaNewTree (theDocument, elType, _EMPTYSTR_);
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
	   /* If we meet the end tag of a FORM, FONT or CENTER
	      looks for that element in the stack, but not at
	      a higher level as a table element */
	   if (!onStartTag &&
	       (!ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("FORM")) ||
		!ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("FONT")) ||
		!ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("CENTER"))))
	     while (i > 0 && entry != GINumberStack[i] && !stop)
	       if (!ustrcmp (pHTMLGIMapping[GINumberStack[i]].htmlGI, TEXT("TBODY")) ||
		   !ustrcmp (pHTMLGIMapping[GINumberStack[i]].htmlGI, TEXT("TR")) ||
		   !ustrcmp (pHTMLGIMapping[GINumberStack[i]].htmlGI, TEXT("TH")) ||
		   !ustrcmp (pHTMLGIMapping[GINumberStack[i]].htmlGI, TEXT("TD")))
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
	   if (!ustrcmp (pHTMLGIMapping[start].htmlGI, TEXT("LI")))
	     while (i > 0 && entry != GINumberStack[i] && !stop)
	       if (!ustrcmp (pHTMLGIMapping[GINumberStack[i]].htmlGI, TEXT("OL")) ||
		   !ustrcmp (pHTMLGIMapping[GINumberStack[i]].htmlGI, TEXT("UL")) ||
		   !ustrcmp (pHTMLGIMapping[GINumberStack[i]].htmlGI, TEXT("DIR")) ||
		   !ustrcmp (pHTMLGIMapping[GINumberStack[i]].htmlGI, TEXT("MENU")))
		 stop = TRUE;
	       else
		 i--;
	   else if (!ustrcmp (pHTMLGIMapping[start].htmlGI, TEXT("OPTION")))
	     while (i > 0 && entry != GINumberStack[i] && !stop)
	       if (!ustrcmp (pHTMLGIMapping[GINumberStack[i]].htmlGI, TEXT("SELECT")))
		 stop = TRUE;
	       else
		 i--;
	   else if (!ustrcmp (pHTMLGIMapping[start].htmlGI, TEXT("DD")) ||
		    !ustrcmp (pHTMLGIMapping[start].htmlGI, TEXT("DT")))
	     while (i > 0 && entry != GINumberStack[i] && !stop)
	       if (!ustrcmp (pHTMLGIMapping[GINumberStack[i]].htmlGI, TEXT("DL")))
		 stop = TRUE;
	       else
		 i--;
	   else if (!ustrcmp (pHTMLGIMapping[start].htmlGI, TEXT("TR")) ||
		    !ustrcmp (pHTMLGIMapping[start].htmlGI, TEXT("TD")) ||
		    !ustrcmp (pHTMLGIMapping[start].htmlGI, TEXT("TH")))
	     while (i > 0 && entry != GINumberStack[i] && !stop)
	       if (!ustrcmp (pHTMLGIMapping[GINumberStack[i]].htmlGI, TEXT("TABLE")))
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
   MapAttrValue    search in AttrValueMappingTable the entry for
   the attribute ThotAtt and its value AttrVal. Returns the corresponding
   Thot value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 MapAttrValue (int ThotAtt, STRING AttrVal)
#else
int                 MapAttrValue (ThotAtt, AttrVal)
int                 ThotAtt;
STRING              AttrVal;

#endif
{
   int                 i, value;

   value = -1;
   i = 0;
   while (HTMLAttrValueMappingTable[i].ThotAttr != ThotAtt &&
	  HTMLAttrValueMappingTable[i].ThotAttr != 0)
      i++;
   if (HTMLAttrValueMappingTable[i].ThotAttr == ThotAtt)
      do
	 if (AttrVal[1] == EOS && (ThotAtt == HTML_ATTR_NumberStyle ||
				   ThotAtt == HTML_ATTR_ItemStyle))
	    /* attributes NumberStyle (which is always 1 character long) */
	    /* and ItemStyle (only when its length is 1) are */
	    /* case sensistive. Compare their exact value */
	    if (AttrVal[0] == HTMLAttrValueMappingTable[i].XMLattrValue[0])
	       value = HTMLAttrValueMappingTable[i].ThotAttrValue;
	    else
	       i++;
	 else
	    /* for other attributes, uppercase and lowercase are */
	    /* equivalent */
	    if (!ustrcasecmp (HTMLAttrValueMappingTable[i].XMLattrValue, AttrVal))
	       value = HTMLAttrValueMappingTable[i].ThotAttrValue;
	    else
	       i++;
      while (value < 0 && HTMLAttrValueMappingTable[i].ThotAttr != 0);
   return value;
}

/*----------------------------------------------------------------------
   TypeAttrValue   Value val has been read for the HTML attribute
   TYPE. Create a child for the current Thot
   element INPUT accordingly.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         TypeAttrValue (STRING val)
#else
static void         TypeAttrValue (val)
STRING              val;

#endif
{
  ElementType         elType;
  Element             newChild;
  AttributeType       attrType;
  Attribute           attr;
  UCHAR_T             msgBuffer[MaxMsgLength];
  int                 value;

  value = MapAttrValue (DummyAttribute, val);
  if (value < 0)
    {
      if (ustrlen (val) > MaxMsgLength - 40)
	 val[MaxMsgLength - 40] = EOS;
      usprintf (msgBuffer, TEXT("Unknown attribute value \"TYPE = %s\""), val);
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
	   val[MaxMsgLength - 40] = EOS;
	usprintf (msgBuffer, TEXT("Duplicate attribute \"TYPE = %s\""), val);
	}
      else
	{
	  elType.ElSSchema = DocumentSSchema;
	  elType.ElTypeNum = value;
	  newChild = TtaNewTree (theDocument, elType, _EMPTYSTR_);
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
   SetAttrIntItemStyle     Create or update attribute IntItemStyle
   of List_Item element el according to its surrounding elements.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetAttrIntItemStyle (Element el, Document doc)
#else
void                SetAttrIntItemStyle (el, doc)
Element             el;
Document            doc;

#endif
{
   ElementType         elType, ancestorType;
   AttributeType       attrType;
   Attribute           attrItem, attrList;
   int                 nbUL, attrVal, val;
   Element             ancestor, parent, sibling;
   ThotBool            orderedList;

   elType = TtaGetElementType (el);
   if (elType.ElTypeNum == HTML_EL_List_Item)
      /* It's a List_Item. Create an attribute IntItemStyle according to */
      /* the surrounding Unnumbered_List and Numbered_List elements */
     {
	attrVal = HTML_ATTR_IntItemStyle_VAL_disc;
	/* is there an ItemStyle attribute on the list item or on its */
	/* previous siblings */
	attrType.AttrSSchema = elType.ElSSchema;
	attrType.AttrTypeNum = HTML_ATTR_ItemStyle;
	attrList = NULL;
	sibling = el;
	while (sibling != NULL && attrList == NULL)
	  {
	     attrList = TtaGetAttribute (sibling, attrType);
	     TtaPreviousSibling (&sibling);
	  }
	if (attrList != NULL)
	   /* there is an ItemStyle attribute on the list item */
	   /* The internal attribute takes the same value */
	   attrVal = TtaGetAttributeValue (attrList);
	else
	  {
	     orderedList = FALSE;
	     nbUL = 0;
	     parent = TtaGetParent (el);
	     ancestor = parent;
	     while (ancestor != NULL)
	       {
		  ancestorType = TtaGetElementType (ancestor);
		  if (ancestorType.ElTypeNum == HTML_EL_Unnumbered_List)
		     nbUL++;
		  else if (ancestorType.ElTypeNum == HTML_EL_Numbered_List)
		     if (nbUL == 0)
		       {
			  orderedList = TRUE;
			  ancestor = NULL;
		       }
		  if (ancestor != NULL)
		     ancestor = TtaGetParent (ancestor);
	       }
	     if (orderedList || nbUL > 0)
	       {
		  if (orderedList)
		    {
		       attrType.AttrTypeNum = HTML_ATTR_NumberStyle;
		       attrList = TtaGetAttribute (parent, attrType);
		       if (attrList == NULL)
			  attrVal = HTML_ATTR_IntItemStyle_VAL_Arabic_;
		       else
			 {
			    val = TtaGetAttributeValue (attrList);
			    switch (val)
				  {
				     case HTML_ATTR_NumberStyle_VAL_Arabic_:
					attrVal = HTML_ATTR_IntItemStyle_VAL_Arabic_;
					break;
				     case HTML_ATTR_NumberStyle_VAL_LowerAlpha:
					attrVal = HTML_ATTR_IntItemStyle_VAL_LowerAlpha;
					break;
				     case HTML_ATTR_NumberStyle_VAL_UpperAlpha:
					attrVal = HTML_ATTR_IntItemStyle_VAL_UpperAlpha;
					break;
				     case HTML_ATTR_NumberStyle_VAL_LowerRoman:
					attrVal = HTML_ATTR_IntItemStyle_VAL_LowerRoman;
					break;
				     case HTML_ATTR_NumberStyle_VAL_UpperRoman:
					attrVal = HTML_ATTR_IntItemStyle_VAL_UpperRoman;
					break;
				     default:
					attrVal = 1;
					break;
				  }
			 }
		    }
		  else
		     /* unnumbered list */
		    {
		       attrType.AttrTypeNum = HTML_ATTR_BulletStyle;
		       attrList = TtaGetAttribute (parent, attrType);
		       if (attrList == NULL)
			 {
			    switch (nbUL)
				  {
				     case 1:
					attrVal = HTML_ATTR_IntItemStyle_VAL_disc;
					break;
				     case 2:
					attrVal = HTML_ATTR_IntItemStyle_VAL_circle;
					break;
				     default:
					attrVal = HTML_ATTR_IntItemStyle_VAL_square;
					break;
				  }
			 }
		       else
			 {
			    val = TtaGetAttributeValue (attrList);
			    switch (val)
				  {
				     case HTML_ATTR_BulletStyle_VAL_disc:
					attrVal = HTML_ATTR_IntItemStyle_VAL_disc;
					break;
				     case HTML_ATTR_BulletStyle_VAL_circle:
					attrVal = HTML_ATTR_IntItemStyle_VAL_circle;
					break;
				     default:
					attrVal = HTML_ATTR_IntItemStyle_VAL_square;
					break;
				  }
			 }
		    }
	       }
	  }
	attrType.AttrSSchema = elType.ElSSchema;
	attrType.AttrTypeNum = HTML_ATTR_IntItemStyle;
	attrItem = TtaGetAttribute (el, attrType);
	if (attrItem == NULL)
	   /* create a new attribute and attach it to the element */
	  {
	     attrItem = TtaNewAttribute (attrType);
	     TtaAttachAttribute (el, attrItem, doc);
	  }
	TtaSetAttributeValue (attrItem, attrVal, el, doc);
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
      if (!ustrcmp (pHTMLGIMapping[lastElemEntry].htmlGI, TEXT("MATH")))
	/* a <MATH> tag has been read */
	math = TRUE;
      else if (!ustrcmp (pHTMLGIMapping[lastElemEntry].htmlGI, TEXT("MATHDISP")))
	/* a <MATHDISP> tag has been read.  add an attribute "mode=display"
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
      else if (!ustrcmp (pHTMLGIMapping[lastElemEntry].htmlGI, TEXT("XMLGRAPHICS")))
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
      else if (!ustrcmp (pHTMLGIMapping[lastElemEntry].htmlGI, TEXT("PRE")) ||
	       !ustrcmp (pHTMLGIMapping[lastElemEntry].htmlGI, TEXT("STYLE")) ||
	       !ustrcmp (pHTMLGIMapping[lastElemEntry].htmlGI, TEXT("SCRIPT")) )
	/* a <PRE>, <STYLE> or <SCRIPT> tag has been read */
	AfterTagPRE = TRUE;
      else if (!ustrcmp (pHTMLGIMapping[lastElemEntry].htmlGI, TEXT("TABLE")))
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
       if (!ustrcmp (pHTMLGIMapping[GINumberStack[StackLevel - 1]].htmlGI, TEXT("TR")))
	 if (ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("TH")) &&
	     ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("TD")))
	   ok = FALSE;
       if (ok)
	 /* only CAPTION, THEAD, TFOOT, TBODY, COLGROUP, COL and TR are */
	 /* allowed as children of a TABLE element */
	 if (!ustrcmp (pHTMLGIMapping[GINumberStack[StackLevel - 1]].htmlGI, TEXT("TABLE")))
	   if (ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("CAPTION")) &&
	       ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("THEAD")) &&
	       ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("TFOOT")) &&
	       ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("TBODY")) &&
	       ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("COLGROUP")) &&
	       ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("COL")) &&
	       ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("TR")))
	     if (!ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("TD")) ||
		 !ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("TH")))
	       /* Table cell within a TABLE, without a TR. Assume TR */
	       {
		/* save the last last GI read from the input file */
		saveLastElemEntry = lastElemEntry;
		/* simulate a <TR> tag */
	        ProcessStartGI (TEXT("TR"));
		/* restore the last tag that has actually been read */
		lastElemEntry = saveLastElemEntry;
	       }
	     else
	       ok = FALSE;
       if (ok)
	 /* CAPTION, THEAD, TFOOT, TBODY, COLGROUP are allowed only as
	    children of a TABLE element */
	 if (ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("CAPTION")) == 0 ||
	     ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("THEAD")) == 0 ||
	     ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("TFOOT")) == 0 ||
	     ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("TBODY")) == 0 ||
	     ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("COLGROUP")) == 0)
	   if (ustrcmp (pHTMLGIMapping[GINumberStack[StackLevel - 1]].htmlGI, TEXT("TABLE")) != 0)
	      ok = FALSE;
       if (ok)
	 /* only TR is allowed as a child of a THEAD, TFOOT or TBODY element */
	 if (!ustrcmp (pHTMLGIMapping[GINumberStack[StackLevel - 1]].htmlGI, TEXT("THEAD")) ||
	     !ustrcmp (pHTMLGIMapping[GINumberStack[StackLevel - 1]].htmlGI, TEXT("TFOOT")) ||
	     !ustrcmp (pHTMLGIMapping[GINumberStack[StackLevel - 1]].htmlGI, TEXT("TBODY")))
	   if (ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("TR")))
	     if (!ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("TD")) ||
		 !ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("TH")))
	       /* Table cell within a THEAD, TFOOT or TBODY without a TR. */
	       /* Assume TR */
	       {
		/* save the last last GI read from the input file */
		saveLastElemEntry = lastElemEntry;
		/* simulate a <TR> tag */
	        ProcessStartGI (TEXT("TR"));
		/* restore the last tag that has actually been read */
		lastElemEntry = saveLastElemEntry;
	       }
	     else
	       ok = FALSE;
       if (ok)
	 /* refuse BODY within BODY */
	 if (ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("BODY")) == 0)
	   if (Within (HTML_EL_BODY, DocumentSSchema))
	     ok = FALSE;
       if (ok)
	 /* refuse HEAD within HEAD */
	 if (ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("HEAD")) == 0)
	   if (Within (HTML_EL_HEAD, DocumentSSchema))
	     ok = FALSE;
       if (ok)
	 /* refuse STYLE within STYLE */
	 if (ustrcmp (pHTMLGIMapping[entry].htmlGI, TEXT("STYLE")) == 0)
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
   if (pHTMLGIMapping[entry].htmlGI[0] == 'H' &&
       pHTMLGIMapping[entry].htmlGI[1] >= '1' &&
       pHTMLGIMapping[entry].htmlGI[1] <= '6' &&
       pHTMLGIMapping[entry].htmlGI[2] == EOS)
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
static void         InsertInvalidEl (STRING content, ThotBool position)
#else
static void         InsertInvalidEl (content, position)
STRING              content;
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
static void         ProcessStartGI (STRING GIname)
#else
static void         ProcessStartGI (GIname)
STRING              GIname;

#endif
{
  ElementType         elType;
  Element             el;
  int                 entry, i;
  UCHAR_T             msgBuffer[MaxMsgLength];
  PtrClosedElement    pClose;
  ThotBool            sameLevel;
  SSchema	      schema;

  /* ignore tag <P> within PRE */
  if (Within (HTML_EL_Preformatted, DocumentSSchema))
    if (ustrcasecmp (GIname, TEXT("P")) == 0)
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
      for (i = 0; GIname[i] != ':' && GIname[i] != EOS; i++);
      if (GIname[i] == ':' &&
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
	    GIname[MaxMsgLength - 20] = EOS;
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
		      el = TtaNewTree (theDocument, elType, _EMPTYSTR_);
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
   EndOfStartGI    An HTML GI has been read in a start tag.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfStartGI (CHAR_T c)
#else
static void         EndOfStartGI (c)
CHAR_T                c;
#endif
{
   CHAR_T                theGI[MaxMsgLength];
   int			 i;

   if (ParsingTextArea)
      /* We are parsing the contents of a TEXTAREA element. If a start
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
      ustrncpy (theGI, inputBuffer, MaxMsgLength - 1);
      theGI[MaxMsgLength - 1] = EOS;
      InitBuffer ();
      if (lastElementClosed && (lastElement == rootElement))
         /* an element after the tag </html>, ignore it */
         {
         ParseHTMLError (theDocument, TEXT("Element after tag </html>. Ignored"));
         return;
         }
      ProcessStartGI (theGI);
      }
}

/*----------------------------------------------------------------------
   EndOfStartGIandTag      a ">" has been read. It indicates the
   end of a GI and the end of a start tag.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfStartGIandTag (CHAR_T c)
#else
static void         EndOfStartGIandTag (c)
CHAR_T                c;

#endif
{
   EndOfStartGI (c);
   EndOfStartTag (c);
}

/*----------------------------------------------------------------------
   EndOfEndTag     An end tag has been read in the HTML file.
   Terminate all corresponding Thot elements.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfEndTag (CHAR_T c)
#else
static void         EndOfEndTag (c)
CHAR_T                c;

#endif
{
   SSchema	       schema;
   UCHAR_T	       msgBuffer[MaxMsgLength];
   int                 entry;
   int                 i;
   ThotBool            ok;

   CloseBuffer ();

   if (ParsingTextArea)
      if (ustrcasecmp (inputBuffer, TEXT("TEXTAREA")) != 0)
         /* We are parsing the contents of a TEXTAREA element. The end
	    tag is not the one closing the current TEXTAREA, consider it
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
         return;
         }

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
      entry = MapGI (inputBuffer, &schema, theDocument);
      if (entry < 0)
        {
        if (ustrlen (inputBuffer) > MaxMsgLength - 20)
	   inputBuffer[MaxMsgLength - 20] = EOS;
	usprintf (msgBuffer, TEXT("Unknown tag </%s>"), inputBuffer);
	ParseHTMLError (theDocument, msgBuffer);
	/* create an Invalid_element */
	usprintf (msgBuffer, TEXT("</%s"), inputBuffer);
	InsertInvalidEl (msgBuffer, FALSE);
        }
      else if (!CloseElement (entry, -1, FALSE))
        /* the end tag does not close any current element */
        {
	/* print an error message... */
	usprintf (msgBuffer, TEXT("Unexpected end tag </%s>"), inputBuffer);
	ParseHTMLError (theDocument, msgBuffer);
	/* ... and try to recover */
	if ((inputBuffer[0] == 'H' || inputBuffer[0] == 'h') &&
	    inputBuffer[1] >= '1' && inputBuffer[1] <= '6' &&
	    inputBuffer[2] == EOS)
	   /* the end tag is </Hn>. Consider all Hn as equivalent. */
	   /* </H3> is considered as an end tag for <H2>, for instance */
	  {
	     ustrcpy (msgBuffer, inputBuffer);
	     msgBuffer[1] = '1';
	     i = 1;
	     do
	       {
		  schema = DocumentSSchema;
		  entry = MapGI (msgBuffer, &schema, theDocument);
		  ok = CloseElement (entry, -1, FALSE);
		  msgBuffer[1]++;
		  i++;
	       }
	     while (i <= 6 && !ok);
	  }
	if (!ok &&
	    (!ustrcasecmp (inputBuffer, TEXT("OL")) ||
	     !ustrcasecmp (inputBuffer, TEXT("UL")) ||
	     !ustrcasecmp (inputBuffer, TEXT("MENU")) ||
	     !ustrcasecmp (inputBuffer, TEXT("DIR"))))
	  /* the end tag is supposed to close a list */
	  /* try to close another type of list */
	  {
	    ok = TRUE;
	    schema = DocumentSSchema;
	    if (!CloseElement (MapGI (TEXT("OL"), &schema, theDocument), -1, FALSE) &&
		!CloseElement (MapGI (TEXT("UL"), &schema, theDocument), -1, FALSE) &&
		!CloseElement (MapGI (TEXT("MENU"), &schema, theDocument), -1, FALSE) &&
		!CloseElement (MapGI (TEXT("DIR"), &schema, theDocument), -1, FALSE))
	      ok = FALSE;
	  }
	if (!ok)
	  /* unrecoverable error. Create an Invalid_element */
	  {
            if (ustrlen (inputBuffer) > MaxMsgLength - 10)
	       inputBuffer[MaxMsgLength - 10] = EOS;
	    usprintf (msgBuffer, TEXT("</%s"), inputBuffer);
	    InsertInvalidEl (msgBuffer, TRUE);
	  }
        }
      }
   InitBuffer ();
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
   EndOfAttrName   A HTML attribute has been read. Create the
   corresponding Thot attribute.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfAttrName (CHAR_T c)
#else
static void         EndOfAttrName (c)
CHAR_T                c;

#endif
{
   AttributeMapping*   tableEntry;
   AttributeType       attrType;
   ElementType         elType;
   Element             child;
   Attribute           attr;
   SSchema	       schema;
   CHAR_T              translation;
   UCHAR_T             msgBuffer[MaxMsgLength];

   CloseBuffer ();
   /* if a single '/' or '?' has been read instead of an attribute name, ignore
      that character.  This is to accept the XML syntax for empty elements or
      processing instructions, such as <img src="SomeUrl" /> or
      <?xml version="1.0"?>  */
   if (LgBuffer == 1 && (inputBuffer[0] == '/' || inputBuffer[0] == '?'))
      {
      InitBuffer ();
      return;
      }
   /* inputBuffer contains the attribute name */
   /* get the corresponding Thot attribute */
   if (UnknownTag)
      /* ignore attributes of unknown tags */
      tableEntry = NULL;
   else
      tableEntry = MapAttr (inputBuffer, &schema, lastElemEntry, theDocument);

   if (tableEntry == NULL)
      /* this attribute is not in the HTML mapping table */
     {
	if (ustrcasecmp (inputBuffer, TEXT("xmlns")) == 0 ||
	    ustrncasecmp (inputBuffer, TEXT("xmlns:"), 6) == 0)
	   /* this is a namespace declaration */
	   {
	   lastAttrEntry = NULL;
	   /**** register this namespace ****/;
	   }
	else if (ustrcasecmp (inputBuffer, TEXT("xml:lang")) == 0)
	   /* attribute xml:lang is not considered as invalid, but it is
	      ignored */
	   lastAttrEntry = NULL;
	else
	   {
           if (ustrlen (inputBuffer) > MaxMsgLength - 30)
	      inputBuffer[MaxMsgLength - 30] = EOS;
	   usprintf (msgBuffer, TEXT("Unknown attribute \"%s\""), inputBuffer);
	   ParseHTMLError (theDocument, msgBuffer);
	   /* attach an Invalid_attribute to the current element */
	   tableEntry = &pHTMLAttributeMapping[0];
	   schema = DocumentSSchema;
	   UnknownAttr = TRUE;
	   }
     }
   else
      UnknownAttr = FALSE;
   if (tableEntry != NULL && lastElement != NULL &&
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
		    CreateAttr (lastElement, attrType, inputBuffer,
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
			      TtaSetAttributeText (attr, TEXT("link"), lastElement, theDocument);
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
			      TtaSetAttributeValue (attr, HTML_ATTR_DefaultChecked_VAL_Yes_, child, theDocument);
			   }
		      }
		    else if (attrType.AttrTypeNum == HTML_ATTR_Selected)
		      {
			 /* create Default-Selected attribute */
			 attrType.AttrSSchema = DocumentSSchema;
			 attrType.AttrTypeNum = HTML_ATTR_DefaultSelected;
			 attr = TtaNewAttribute (attrType);
			 TtaAttachAttribute (lastElement, attr, theDocument);
			 TtaSetAttributeValue (attr, HTML_ATTR_DefaultSelected_VAL_Yes_, lastElement, theDocument);
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
#ifdef __STDC__
static void         EndOfAttrNameAndTag (CHAR_T c)
#else
static void         EndOfAttrNameAndTag (c)
CHAR_T                c;

#endif
{
   EndOfAttrName (c);
   EndOfStartTag (c);
}

/*----------------------------------------------------------------------
   StartOfQuotedAttrValue
   A quote (or double quote) starting an attribute value has been read.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         StartOfQuotedAttrValue (CHAR_T c)
#else
static void         StartOfQuotedAttrValue (c)
CHAR_T                c;

#endif
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
#ifdef __STDC__
static void         StartOfUnquotedAttrValue (CHAR_T c)
#else
static void         StartOfUnquotedAttrValue (c)
CHAR_T                c;

#endif
{
   ReadingAnAttrValue = TRUE;
   PutInBuffer (c);
}

/*----------------------------------------------------------------------
   CreateAttrWidthPercentPxl
   an HTML attribute "width" has been created for a Table of a HR.
   Create the corresponding attribute IntWidthPercent or
   IntWidthPxl.
   oldWidth is -1 or the old image width.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateAttrWidthPercentPxl (STRING buffer, Element el, Document doc, int oldWidth)
#else
void                CreateAttrWidthPercentPxl (buffer, el, doc, oldWidth)
STRING              buffer;
Element             el;
Document            doc;
int                 oldWidth;
#endif
{
  AttributeType      attrTypePxl, attrTypePercent;
  Attribute          attrOld, attrNew;
  int                length, val;
  UCHAR_T            msgBuffer[MaxMsgLength];
#ifndef STANDALONE
  ElementType	     elType;
  int                w, h;
  ThotBool           isImage;

  elType = TtaGetElementType (el);
  isImage = (elType.ElTypeNum == HTML_EL_PICTURE_UNIT ||
	     elType.ElTypeNum == HTML_EL_Data_cell ||
	     elType.ElTypeNum == HTML_EL_Heading_cell);
#endif

  /* remove trailing spaces */
  length = ustrlen (buffer) - 1;
  while (length > 0 && buffer[length] <= SPACE)
    length--;
  attrTypePxl.AttrSSchema = TtaGetDocumentSSchema (doc);
  attrTypePercent.AttrSSchema = TtaGetDocumentSSchema (doc);
  attrTypePxl.AttrTypeNum = HTML_ATTR_IntWidthPxl;
  attrTypePercent.AttrTypeNum = HTML_ATTR_IntWidthPercent;
  /* is the last character a '%' ? */
  if (buffer[length] == '%')
    {
      /* remove IntWidthPxl */
      attrOld = TtaGetAttribute (el, attrTypePxl);
      /* update IntWidthPercent */
      attrNew = TtaGetAttribute (el, attrTypePercent);
      if (attrNew == NULL)
	{
	  attrNew = TtaNewAttribute (attrTypePercent);
	  TtaAttachAttribute (el, attrNew, doc);
	}
#ifndef STANDALONE
      else if (isImage && oldWidth == -1)
	{
	  if (attrOld == NULL)
	    oldWidth = TtaGetAttributeValue (attrNew);
	  else
	    oldWidth = TtaGetAttributeValue (attrOld);
	}
#endif
    }
  else
    {
      /* remove IntWidthPercent */
      attrOld = TtaGetAttribute (el, attrTypePercent);
      /* update IntWidthPxl */
      attrNew = TtaGetAttribute (el, attrTypePxl);
      if (attrNew == NULL)
	{
	  attrNew = TtaNewAttribute (attrTypePxl);
	  TtaAttachAttribute (el, attrNew, doc);
	}
#ifndef STANDALONE
      else if (isImage && oldWidth == -1)
	{
	  TtaGiveBoxSize (el, doc, 1, UnPixel, &w, &h);
	  if (attrOld == NULL)
	    oldWidth = w * TtaGetAttributeValue (attrNew) / 100;
	  else
	    oldWidth = w * TtaGetAttributeValue (attrOld) / 100;	  
	}
#endif
    }

  if (attrOld != NULL)
    TtaRemoveAttribute (el, attrOld, doc);
  if (usscanf (buffer, TEXT("%d"), &val))
    TtaSetAttributeValue (attrNew, val, el, doc);
  else
    /* its not a number. Delete attribute and send an error message */
    {
    TtaRemoveAttribute (el, attrNew, doc);
    if (ustrlen (buffer) > MaxMsgLength - 30)
        buffer[MaxMsgLength - 30] = EOS;
    usprintf (msgBuffer, TEXT("Invalid attribute value \"%s\""), buffer);
    ParseHTMLError (doc, msgBuffer);
    }
#ifndef STANDALONE
  if (isImage)
    UpdateImageMap (el, doc, oldWidth, -1);
#endif
}

/*----------------------------------------------------------------------
   an HTML attribute "size" has been created for a Font element.
   Create the corresponding internal attribute.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateAttrIntSize (STRING buffer, Element el, Document doc)
#else
void                CreateAttrIntSize (buffer, el, doc)
STRING              buffer;
Element             el;
Document            doc;

#endif
{
   AttributeType       attrType;
   int                 val, ind, factor, delta;
   Attribute           attr;
   UCHAR_T             msgBuffer[MaxMsgLength];

   /* is the first character a '+' or a '-' ? */
   ind = 0;
   factor = 1;
   delta = 0;
   if (buffer[0] == '+')
     {
	attrType.AttrTypeNum = HTML_ATTR_IntSizeIncr;
	ind++;
	factor = 2;
     }
   else if (buffer[0] == '-')
     {
	attrType.AttrTypeNum = HTML_ATTR_IntSizeDecr;
	ind++;
	factor = 2;
     }
   else
     {
	attrType.AttrTypeNum = HTML_ATTR_IntSizeRel;
	delta = 1;
     }
   attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
   attr = TtaGetAttribute (el, attrType);
   if (usscanf (&buffer[ind], TEXT("%d"), &val))
      {
      val = val * factor + delta;
      if (attr == NULL)
        {
        /* this attribute doesn't exist, create it */
        attr = TtaNewAttribute (attrType);
        TtaAttachAttribute (el, attr, doc);
        }
      TtaSetAttributeValue (attr, val, el, doc);
      }
   else
      /* its not a number. Delete attribute and send an error message */
      {
      if (attr)
         TtaRemoveAttribute (el, attr, doc);
      if (ustrlen (buffer) > MaxMsgLength - 30)
         buffer[MaxMsgLength - 30] = EOS;
      usprintf (msgBuffer, TEXT("Invalid attribute value \"%s\""), buffer);
      ParseHTMLError (doc, msgBuffer);
      }
}

/*----------------------------------------------------------------------
   EndOfAttrValue
   An attribute value has been read from the HTML file.
   Put that value in the current Thot attribute.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfAttrValue (CHAR_T c)
#else
static void         EndOfAttrValue (c)
CHAR_T                c;

#endif
{
   AttributeType       attrType, attrType1;
   Attribute	       attr;
   ElementType	       elType;
   Element             child;
   Language	       lang;
   CHAR_T              translation;
   CHAR_T              shape;
   STRING              buffer;
   STRING              attrName;
   int                 val;
   int                 length;
   int                 attrKind;
   ThotBool            done;
   UCHAR_T             msgBuffer[MaxMsgLength];

   ReadingAnAttrValue = FALSE;
   if (UnknownAttr)
      /* this is the end of value of an invalid attribute. Keep the */
      /* quote character that ends the value for copying it into the */
      /* Invalid_attribute. */
      if (c == '\'' || c == '\"')
	 PutInBuffer (c);
   CloseBuffer ();
   /* inputBuffer contains the attribute value */

   if (lastAttrEntry == NULL)
      {
      InitBuffer ();
      return;
      }
   done = FALSE;
   if (lastElementClosed && (lastElement == rootElement))
      /* an attribute after the tag </html>, ignore it */
      done = TRUE;
   /* treatments of some particular HTML attributes */
   else if (!ustrcmp (lastAttrEntry->XMLattribute, TEXT("STYLE")))
     {
#ifndef STANDALONE
	TtaSetAttributeText (lastAttribute, inputBuffer, lastAttrElement,
			     theDocument);
	ParseHTMLSpecificStyle (lastElement, inputBuffer, theDocument, FALSE);
#endif
	done = TRUE;
     }
#ifndef STANDALONE
   else if (!ustrcmp (lastAttrEntry->XMLattribute, TEXT("LINK")))
      HTMLSetAlinkColor (theDocument, inputBuffer);
   else if (!ustrcmp (lastAttrEntry->XMLattribute, TEXT("ALINK")))
      HTMLSetAactiveColor (theDocument, inputBuffer);
   else if (!ustrcmp (lastAttrEntry->XMLattribute, TEXT("VLINK")))
      HTMLSetAvisitedColor (theDocument, inputBuffer);
#endif

   if (!done)
     {
     val = 0;
     translation = lastAttrEntry->AttrOrContent;
     switch (translation)
	{
	 case 'C':	/* Content */
	    child = PutInContent (inputBuffer);
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
		     val = MapAttrValue (lastAttrEntry->ThotAttribute,
					 inputBuffer);
		     if (val < 0)
		        {
			TtaGiveAttributeType (lastAttribute, &attrType,
					      &attrKind);
			attrName = TtaGetAttributeName (attrType);
                        if (ustrlen (inputBuffer) > MaxMsgLength - 30)
                           inputBuffer[MaxMsgLength - 30] = EOS;
			usprintf (msgBuffer, TEXT("Unknown attribute value \"%s = %s\""),
				 attrName, inputBuffer);
			ParseHTMLError (theDocument, msgBuffer);
			/* remove the attribute and replace it by an */
			/* Invalid_attribute */
			TtaRemoveAttribute (lastAttrElement, lastAttribute,
					    theDocument);
			attrType.AttrSSchema = DocumentSSchema;
			attrType.AttrTypeNum = pHTMLAttributeMapping[0].ThotAttribute;
			usprintf (msgBuffer, TEXT("%s=%s"), attrName, inputBuffer);
			CreateAttr (lastAttrElement, attrType, msgBuffer, TRUE);
		        }
		     else
			TtaSetAttributeValue (lastAttribute, val,
					      lastAttrElement, theDocument);
		     break;
		  case 1:	/* integer */
		     if (attrType.AttrTypeNum == HTML_ATTR_Border &&
			 !ustrcasecmp (inputBuffer, TEXT("border")) )
			/* BORDER="BORDER" for a TABLE */
			{
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
			   TtaRemoveAttribute (lastAttrElement, lastAttribute,
					       theDocument);
			   usprintf (msgBuffer, TEXT("Invalid attribute value \"%s\""), inputBuffer);
			   ParseHTMLError (theDocument, msgBuffer);
			   }
		     break;
		  case 2:	/* text */
		     if (!UnknownAttr)
			 {
			 TtaSetAttributeText (lastAttribute, inputBuffer,
			      lastAttrElement, theDocument);
			 if (attrType.AttrTypeNum == HTML_ATTR_Langue)
			   /* it's a LANG attribute value */
			   {
			   lang = TtaGetLanguageIdFromName (inputBuffer);
			   if (lang == 0)
			      {
			      usprintf (msgBuffer, TEXT("Unknown language: %s"),
				       inputBuffer);
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
			      TtaAttachAttribute (lastAttrElement, attr, theDocument);
			      TtaSetAttributeValue (attr, HTML_ATTR_RealLang_VAL_Yes_, lastAttrElement, theDocument);
			      }
			   }
			 }
		     else
			 /* this is the content of an invalid attribute */
			 /* append it to the current Invalid_attribute */
		         {
			 length = ustrlen (inputBuffer) + 2;
			 length += TtaGetTextAttributeLength (lastAttribute);
			 buffer = TtaAllocString (length + 1);
			 TtaGiveTextAttributeValue (lastAttribute, buffer,
						    &length);
			 ustrcat (buffer, TEXT("="));
			 ustrcat (buffer, inputBuffer);
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
	    TypeAttrValue (inputBuffer);
	    break;
	 default:
	    break;
	}

     if (lastAttrEntry->ThotAttribute == HTML_ATTR_Width__)
        /* HTML attribute "width" for a Table or a HR */
        /* create the corresponding attribute IntWidthPercent or */
        /* IntWidthPxl */
        CreateAttrWidthPercentPxl (inputBuffer, lastAttrElement, theDocument, -1);

     else if (!ustrcmp (lastAttrEntry->XMLattribute, TEXT("SIZE")))
       {
       TtaGiveAttributeType (lastAttribute, &attrType, &attrKind);
       if (attrType.AttrTypeNum == HTML_ATTR_Font_size)
	  CreateAttrIntSize (inputBuffer, lastAttrElement, theDocument);
       }
     else if (!ustrcmp (lastAttrEntry->XMLattribute, TEXT("SHAPE")))
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
     else if (!ustrcmp (lastAttrEntry->XMLattribute, TEXT("VALUE")))
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
     	 TtaSetAttributeText (attr, inputBuffer, lastAttrElement, theDocument);
     	 }
       }
#ifndef STANDALONE
     /* Some HTML attributes are equivalent to a CSS property:      */
     /*      background     ->                   background         */
     /*      bgcolor        ->                   background         */
     /*      text           ->                   color              */
     /*      color          ->                   color              */
     else if (!ustrcmp (lastAttrEntry->XMLattribute, TEXT("BACKGROUND")))
        {
        if (ustrlen (inputBuffer) > MaxMsgLength - 30)
            inputBuffer[MaxMsgLength - 30] = EOS;
        usprintf (msgBuffer, TEXT("background: url(%s)"), inputBuffer);
        ParseHTMLSpecificStyle (lastElement, msgBuffer, theDocument, FALSE);
        }
     else if (!ustrcmp (lastAttrEntry->XMLattribute, TEXT("BGCOLOR")))
        HTMLSetBackgroundColor (theDocument, lastElement, inputBuffer);
     else if (!ustrcmp (lastAttrEntry->XMLattribute, TEXT("TEXT")) ||
	      !ustrcmp (lastAttrEntry->XMLattribute, TEXT("COLOR")))
        HTMLSetForegroundColor (theDocument, lastElement, inputBuffer);
#endif /* !STANDALONE */
     }
   InitBuffer ();
}

/*----------------------------------------------------------------------
   EndOfAttrValueAndTag    A ">" has been read. It indicates the
   end of an attribute value and the end of a start tag.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfAttrValueAndTag (CHAR_T c)
#else
static void         EndOfAttrValueAndTag (c)
CHAR_T                c;

#endif
{
   EndOfAttrValue (c);
   EndOfStartTag (c);
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
static void         PutNonISOlatin1Char (int code)
#else
static void         PutNonISOlatin1Char (code)
int                 code;

#endif
{
   int		i, c = 0;
   Language	lang, l;

   /* look for that code in the fallback table */
   for (i = 0; UnicodeFallbackTable[i].unicodeVal < code &&
	       UnicodeFallbackTable[i].unicodeVal > 0;  i++);
   if (UnicodeFallbackTable[i].unicodeVal == code)
      {
      if (UnicodeFallbackTable[i].EightbitCode < 255)
	 {
	 /* Symbol character */
	 lang = TtaGetLanguageIdFromAlphabet('G');
	 c = UnicodeFallbackTable[i].EightbitCode;
	 }
      else if (UnicodeFallbackTable[i].EightbitCode < 2000)
	 {
	 /* ISO latin-1 fallback */
	 lang = TtaGetLanguageIdFromAlphabet('L');
	 c = UnicodeFallbackTable[i].EightbitCode - 1000;
	 }
      else
	 {
	 /* Symbol fallback */
	 lang = TtaGetLanguageIdFromAlphabet('G');
	 c = UnicodeFallbackTable[i].EightbitCode - 2000;
	 }
      }
   else
     lang = TtaGetLanguageIdFromAlphabet('L');

   if (lang == currentLanguage)
      PutInBuffer ((CHAR_T) c);
   else
      {
      TextToDocument ();
      MergeText = FALSE;
      l = currentLanguage;
      currentLanguage = lang;
      PutInBuffer ((CHAR_T) c);
      TextToDocument ();
      MergeText = FALSE;
      currentLanguage = l;
      }

   /* some special cases: insert a second character */
   /* this pair of characters should not be saved as is in the output file.
      The original entity should be saved instead */

   if (code == 338)		/* OE ligature */
      PutInBuffer ('E');
   else if (code == 339)	/* oe ligature */
      PutInBuffer ('e');
   else if (code == 8195)	/* em space, U+2003 ISOpub */
      PutInBuffer ('\240');
   else if (code == 8220)	/* left double quotation mark */
      PutInBuffer ('\140');
   else if (code == 8221)	/* right double quotation mark */
      PutInBuffer ('\47');
   else if (code == 8222)	/* double low-9 quotation mark */
      PutInBuffer (',');
   else if (code == 8240)	/* per mille sign */
      PutInBuffer ('\260');
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
   UCHAR_T             msgBuffer[MaxMsgLength];

   EntityName[LgEntityName] = EOS;
   if (CharEntityTable[EntityTableEntry].charName[CharRank] == EOS)
      /* the entity read matches the current entry of entity table */
      if (CharEntityTable[EntityTableEntry].charCode > 255)
	 PutNonISOlatin1Char (CharEntityTable[EntityTableEntry].charCode);
      else
	 PutInBuffer ((CHAR_T) (CharEntityTable[EntityTableEntry].charCode));
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
static void         EntityChar (UCHAR_T c)
#else
static void         EntityChar (c)
UCHAR_T       c;

#endif
{
   int                 i;
   UCHAR_T             msgBuffer[MaxMsgLength];
   ThotBool	       OK, done, stop;

   done = FALSE;
   if (CharEntityTable[EntityTableEntry].charName[CharRank] == EOS)
      /* the entity name read so far matches the current entry of */
      /* entity table */
      /* does it also match the next entry? */
     {
     OK = FALSE;
     i = EntityTableEntry+1;
     stop = FALSE;
     do
	{
	if (ustrncmp (EntityName, CharEntityTable[i].charName, LgEntityName) != 0)
	   stop = TRUE;
	else
	   if (CharEntityTable[i].charName[CharRank] < c)
	      i++;
	   else
	      {
	      stop = TRUE;
	      if (CharEntityTable[i].charName[CharRank] == c)
		 OK = TRUE;
	      }
	}
     while (!stop);     
     if (!OK)
        {
	/* assume that semicolon is missing and put the corresponding char */
	if (CharEntityTable[EntityTableEntry].charCode > 255)
	   PutNonISOlatin1Char (CharEntityTable[EntityTableEntry].charCode);
	else
	   PutInBuffer ((CHAR_T) (CharEntityTable[EntityTableEntry].charCode));
	if (c != SPACE)
	   /* print an error message */
	   ParseHTMLError (theDocument, TEXT("Missing semicolon"));
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
	while (CharEntityTable[EntityTableEntry].charName[CharRank] < c
	       && CharEntityTable[EntityTableEntry].charCode != 0)
	   EntityTableEntry++;
	if (CharEntityTable[EntityTableEntry].charName[CharRank] != c)
	  OK = FALSE;
	else
	  if (LgEntityName > 0 &&
	      ustrncmp (EntityName, CharEntityTable[EntityTableEntry].charName,
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
	        EntityName[LgEntityName++] = EOS;
	        usprintf (msgBuffer, TEXT("Invalid entity \"&%s\""), EntityName);
	        ParseHTMLError (theDocument, msgBuffer);
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
#ifdef __STDC__
static void         EndOfDecEntity (CHAR_T c)
#else
static void         EndOfDecEntity (c)
CHAR_T                c;

#endif
{
   int                 code;

   EntityName[LgEntityName] = EOS;
   usscanf (EntityName, TEXT("%d"), &code);
   if (code > 255)
      PutNonISOlatin1Char (code);
   else
      PutInBuffer ((CHAR_T) code);
   LgEntityName = 0;
}

/*----------------------------------------------------------------------
   DecEntityChar   A character belonging to a decimal entity has been read.
   Put that character in the entity buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DecEntityChar (CHAR_T c)
#else
static void         DecEntityChar (c)
CHAR_T                c;

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
	 /* next state is state 0, not the state computed by the automaton */
	 /* and the character read has not been processed yet */
	 NormalTransition = FALSE;
	 currentState = 0;
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

   EntityName[LgEntityName] = EOS;
   usscanf (EntityName, TEXT("%x"), &code);
   if (code > 255)
      PutNonISOlatin1Char (code);
   else
      PutInBuffer ((CHAR_T) code);
   LgEntityName = 0;
}

/*----------------------------------------------------------------------
   HexEntityChar   A character belonging to an hexadecimal entity has been
   read. Put that character in the entity buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         HexEntityChar (CHAR_T c)
#else
static void         HexEntityChar (c)
CHAR_T                c;

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
	 /* next state is state 0, not the state computed by the automaton */
	 /* and the character read has not been processed yet */
	 NormalTransition = FALSE;
	 currentState = 0;
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
static void         PutLessAndSpace (CHAR_T c)
#else
static void         PutLessAndSpace (c)
CHAR_T                c;

#endif
{
   PutInBuffer ('<');
   PutInBuffer (c);
}


/*----------------------------------------------------------------------
   StartOfComment  Beginning of a HTML comment.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         StartOfComment (CHAR_T c)
#else
static void         StartOfComment (c)
CHAR_T                c;

#endif
{
   ElementType         elType;
   Element             elComment, elCommentLine, child, lastChild;

   /* create a Thot element Comment */
   elType.ElSSchema = DocumentSSchema;
   elType.ElTypeNum = HTML_EL_Comment_;
   elComment = TtaNewElement (theDocument, elType);
   TtaSetElementLineNumber (elComment, NumberOfLinesRead);
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
      InsertElement (&elComment);
   lastElementClosed = TRUE;
   /* create a Comment_line element as the first child of */
   /* element Comment */
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
	TtaSetTextContent (CommentText, _EMPTYSTR_, currentLanguage, theDocument);
     }
   InitBuffer ();
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

   if (c != EOS)
      if (!ParsingCSS && ((int) c == EOL || (int) c == __CR__))
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
	   TtaInsertSibling (elCommentLine, prevElCommentLine, FALSE,
			     theDocument);
	   /* create a TEXT element as the first child of the new element
	      Comment_line */
	   elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	   CommentText = TtaNewElement (theDocument, elType);
	   TtaSetElementLineNumber (CommentText, NumberOfLinesRead);
	   TtaInsertFirstChild (&CommentText, elCommentLine, theDocument);
	   TtaSetTextContent (CommentText, _EMPTYSTR_, currentLanguage, theDocument);
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
   EndOfComment    End of a HTML comment.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfComment (CHAR_T c)
#else
static void         EndOfComment (c)
CHAR_T                c;

#endif
{
   if (LgBuffer > 0)
     {
	CloseBuffer ();
	if (CommentText != NULL)
	   TtaAppendTextContent (CommentText, inputBuffer, theDocument);
     }
   CommentText = NULL;
   InitBuffer ();
}

/*----------------------------------------------------------------------
   PutDash put a dash character in the current comment.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutDash (CHAR_T c)
#else
static void         PutDash (c)
CHAR_T                c;

#endif
{
   PutInComment ('-');
   PutInComment (c);
}

/*----------------------------------------------------------------------
   PutDashDash     put 2 dash characters in the current comment.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutDashDash (CHAR_T c)
#else
static void         PutDashDash (c)
CHAR_T                c;

#endif
{
   PutInComment ('-');
   PutInComment ('-');
   PutInComment (c);
}

/*----------------------------------------------------------------------
   PutQuestionMark put a question mark in the current PI.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutQuestionMark (CHAR_T c)
#else
static void         PutQuestionMark (c)
CHAR_T                c;

#endif
{
   PutInBuffer ('?');
   PutInBuffer (c);
}

/*----------------------------------------------------------------------
   EndOfDoctypeDecl	A Doctype declaration has been read
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfDoctypeDecl (CHAR_T c)
#else
static void         EndOfDoctypeDecl (c)
CHAR_T                c;
 
#endif
{
   int		i;

   CloseBuffer ();
   /* process the Doctype declaration available in inputBuffer */
   if (!ustrcasecmp (inputBuffer, TEXT("DOCTYPE")))
      {
      for (i = 7; inputBuffer[i] <= SPACE && inputBuffer[i] != EOS; i++);
      if (!ustrcasecmp (&inputBuffer[i], TEXT("HTML")))
	 /* it's a HTML document */
	 {
         /***** TO DO *****/;
	 }
      }
   InitBuffer ();
}


/*----------------------------------------------------------------------
   EndOfPI	A Processing Instruction has been read
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfPI (CHAR_T c)
#else
static void         EndOfPI (c)
CHAR_T                c;
 
#endif
{
   CloseBuffer ();
   /* process the Processing Instruction available in inputBuffer */
   /* printf ("PI: %s\n", inputBuffer); */
   InitBuffer ();
}


/*----------------------------------------------------------------------
   Do_nothing      Do nothing.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         Do_nothing (CHAR_T c)
#else
static void         Do_nothing (c)
CHAR_T                c;

#endif
{
}

/* some type definitions for the automaton */

typedef struct _Transition *PtrTransition;

typedef struct _Transition
  {				/* a transition of the automaton in
				   "executable" form */
     UCHAR_T       trigger;	/* the imput character that triggers
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
     CHAR_T                trigger;	/* the imput character that triggers
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
   {1, '/', (Proc) Do_nothing, 3},
   {1, '!', (Proc) Do_nothing, 10},
   {1, '?', (Proc) Do_nothing, 20},
   {1, '<', (Proc) Do_nothing, 18},
   {1, 'S', (Proc) PutLessAndSpace, 0},		/*   S = Space */
   {1, '*', (Proc) PutInBuffer, 2},
/* state 2: reading a start tag */
   {2, '>', (Proc) EndOfStartGIandTag, 0},
   {2, '&', (Proc) StartOfEntity, -30},		/* call subautomaton 30 */
   {2, 'S', (Proc) EndOfStartGI, 16},	/*   S = Space */
   {2, '*', (Proc) PutInBuffer, 2},
/* state 3: reading an end tag */
   {3, '>', (Proc) EndOfEndTag, 0},
   {3, '&', (Proc) StartOfEntity, -30},		/* call subautomaton 30 */
   {3, 'S', (Proc) Do_nothing, 3},
   {3, '*', (Proc) PutInBuffer, 3},
/* state 4: reading an attribute name */
   {4, '=', (Proc) EndOfAttrName, 5},
   {4, 'S', (Proc) EndOfAttrName, 17},
   {4, '&', (Proc) StartOfEntity, -30},		/* call subautomaton 30 */
   {4, '>', (Proc) EndOfAttrNameAndTag, 0},
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
   {8, 'S', (Proc) Do_nothing, 16},
   {8, '*', (Proc) PutInBuffer, 4},
/* state 9: reading an attribute value between simple quotes */
   {9, '\'', (Proc) EndOfAttrValue, 8},
   {9, '&', (Proc) StartOfEntity, -30},		/* call subautomaton 30 */
   {9, '*', (Proc) PutInBuffer, 9},
/* state 10: "<!" has been read */
   {10, '-', (Proc) Do_nothing, 11},
   {10, 'S', (Proc) Do_nothing, 10},
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
   {20, 'S', (Proc) Do_nothing, 20},
   {20, '?', (Proc) Do_nothing, 22},
   {20, '*', (Proc) PutInBuffer, 21},
/* state 21: reading a Processing Instruction */
   {21, '?', (Proc) Do_nothing, 22},
   {21, '*', (Proc) PutInBuffer, 21},
/* state 22: a question mark has been read in a Processing Instruction */
   {22, '>', (Proc) EndOfPI, 0},
   {22, '*', (Proc) PutQuestionMark, 21},

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

/* state 1000: fake state. End of automaton table */
/* the next line must be the last one in the automaton declaration */
   {1000, '*', (Proc) Do_nothing, 1000}
};

/*----------------------------------------------------------------------
   InitAutomaton   read the "source" form of the automaton and
   build the "executable" form.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitAutomaton (void)
#else
void                InitAutomaton ()
#endif
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
#ifdef __STDC__
void                FreeHTMLParser (void)
#else
void                FreeHTMLParser ()
#endif
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
   for (entry = 0; pHTMLGIMapping[entry].htmlGI[0] != EOS; entry++)
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
   GetNextChar     returns the next character in the imput file or buffer,
   whatever it is.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T     GetNextChar (FILE *infile, STRING buffer, int *index, ThotBool *endOfFile)
#else
static CHAR_T     GetNextChar (infile, buffer, index, endOfFile)
FILE             *infile;
STRING            buffer;
int              *index;
ThotBool         *endOfFile;
#endif
{
  CHAR_T	  charRead;
  int		  res;

  charRead = EOS;
  *endOfFile = FALSE;
  if (buffer != NULL)
    {
      charRead = buffer[(*index)++];
      *endOfFile = (charRead == EOS);
    }
  else if (infile == NULL)
    *endOfFile = TRUE;
  else
    {
      if (*index == 0)
	{
#        ifdef _I18N_
	  res = gzread (infile, FileBufferA, INPUT_FILE_BUFFER_SIZE);
#        ifdef _WINDOWS
	  if (!MultiByteToWideChar (1256, 0, FileBufferA, sizeof(FileBufferA), FileBuffer, sizeof (FileBufferA)))
            WinErrorBox (NULL, "GetNextChar");
	  /* Ubuffer = ISO2WideChar (FileBufferA); */
#        else  /* _WINDOWS */
	  mbstowcs (FileBuffer, FileBufferA, sizeof (FileBufferA));
#        endif /* _WINDOWS */
#        else  /* !_I18N_ */
	  res = gzread (infile, FileBuffer, INPUT_FILE_BUFFER_SIZE);
#        endif /* _I18N_ */
	  /* ustrcpy (FileBuffer, Ubuffer); */
	  if (res <= 0)
            /* error or end of file */
            {
	      *endOfFile = TRUE;
	      charRead = EOS;
	      LastCharInFileBuffer = 0;
            }
	  else
            {
	      /* FileBuffer[res] = EOS; $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	      LastCharInFileBuffer = res - 1;
            }
	}
      if (*endOfFile == FALSE)
	{
	  charRead = FileBuffer[(*index)++];
	  /* charRead = WC_Buffer[(*index)++]; */
	  if (*index > LastCharInFileBuffer)
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
#ifdef __STDC__
void            SetElemLineNumber (Element el)
#else
void            SetElemLineNumber (el)
Element		el;
#endif
{
  TtaSetElementLineNumber (el, NumberOfLinesRead);
}

/*----------------------------------------------------------------------
   GetNextInputChar        returns the next non-null character in the
   input file or buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T          GetNextInputChar (FILE *infile, int *index, ThotBool *endOfFile)
#else
CHAR_T          GetNextInputChar (infile, index, endOfFile)
FILE           *infile;
int            *index;
ThotBool       *endOfFile;
#endif
{
  CHAR_T		charRead;

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
      NumberOfCharRead++;
      /* skip null characters*/
      while (charRead == EOS && !*endOfFile)
	{
	charRead = GetNextChar (infile, InputText, index, endOfFile);
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
	  if ((int) charRead != EOL)
	    /* next character is not LF. Store next character and return LF */
	    {
	      PreviousBufChar = charRead;
	      charRead = EOL;
	    }
	}
      /* update the counters of characters and lines read */
      if ((int) charRead == EOL || (int) charRead == __CR__)
	/* new line in HTML file */
	{
	  NumberOfLinesRead++;
	  NumberOfCharRead = 0;
	}
      else
	NumberOfCharRead++;
    }
  return charRead;
}

/*----------------------------------------------------------------------
   HTMLparse       parse either the HTML file infile or the text
   buffer HTMLbuf and build the equivalent Thot
   abstract tree.
   One parameter should be NULL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void        HTMLparse (FILE * infile, STRING HTMLbuf)
#else
static void        HTMLparse (infile, HTMLbuf)
FILE               *infile;
STRING              HTMLbuf;

#endif
{
   UCHAR_T             charRead;
   ThotBool            match;
   PtrTransition       trans;
   ThotBool            endOfFile;

   currentState = 0;
   if (HTMLbuf != NULL || infile != NULL)
      {
      InputText = HTMLbuf;
      endOfFile = FALSE;
      }
   charRead = EOS;
   HTMLrootClosed = FALSE;

   /* read the HTML file sequentially */
   do
     {
	/* read one character from the source if the last character */
	/* read has been processed */
	if (charRead == EOS)
	  charRead = GetNextInputChar (infile, &CurrentBufChar, &endOfFile);
	if (charRead != EOS)
	  {
	     /* Check the character read */
	     /* Consider LF and FF as the end of an input line. */
	     /* Replace end of line by space, except in preformatted text. */
	     /* Replace HT by space, except in preformatted text. */
	     /* Ignore spaces at the beginning and at the end of input lines */
	     /* Ignore non printable characters except HT, LF, FF. */
	     if ((int) charRead == EOL)
		/* LF = end of input line */
	       {
		if (currentState != 12)
		   /* don't change characters in comments */
		   if (currentState != 0)
		      /* not within a text element */
		      {
		      if (currentState == 6 || currentState == 9)
			/* within an attribute value between quotes */
			if (lastAttrEntry != NULL &&
			    !ustrcmp (lastAttrEntry->XMLattribute, TEXT("SRC")))
			   /* value of an SRC attribute */
			   /* consider new line as an empty char*/
			   charRead = EOS;
		      if (charRead != EOS)
		         /* Replace new line by a space, except if an entity is
			    being read */
			 if (currentState == 30 &&
			     Within (HTML_EL_Preformatted, DocumentSSchema) &&
	                     !Within (HTML_EL_Option_Menu, DocumentSSchema))
			   charRead = EOL; /* new line character */
			 else
		           charRead = SPACE;
		      }
		   else
		      /* new line in a text element */
		      if ((Within (HTML_EL_Preformatted, DocumentSSchema) &&
			      !Within (HTML_EL_Option_Menu, DocumentSSchema)) ||
			   Within (HTML_EL_Text_Area, DocumentSSchema) ||
			   Within (HTML_EL_SCRIPT, DocumentSSchema) ||
			   Within (HTML_EL_STYLE_, DocumentSSchema))
			/* within preformatted text */
			if (AfterTagPRE)
			   /* ignore NL after a <PRE> tag */
			   charRead = EOS;
			else
			   /* generate a new line character */
			   charRead = EOL;
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
			      !Within (HTML_EL_SCRIPT, DocumentSSchema))
			     charRead = SPACE;
		    }
		  if (charRead == SPACE)
		     /* space character */
		    {
		       if (currentState == 12 ||
			   (currentState == 0 &&
				!Within (HTML_EL_Preformatted, DocumentSSchema) &&
				!Within (HTML_EL_STYLE_, DocumentSSchema) &&
				!Within (HTML_EL_SCRIPT, DocumentSSchema)))
			  /* reading text in a comment or in an element
			     that is not preformatted text */
			  /* ignore spaces at the beginning of an input line */
			  if (EmptyLine)
			     charRead = EOS;
		    }
#         ifndef _I18N_
		  else if ((charRead < SPACE || (int) charRead >= 254 ||
			    ((int) charRead >= 127 && (int) charRead <= 159))
			   && (int) charRead != TAB)
		     /* it's not a printable character, ignore it */
		     charRead = EOS;
#         endif /* _I18N_ */
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
			  if ((int) charRead == TAB || (int) charRead == EOL ||
			      (int) charRead == 12)
			     /* a delimiter has been read */
			     match = TRUE;
		       if (match)
			  /* transition found. Activate the transition */
			 {
			    NormalTransition = TRUE;

			    /* Special case: '<' within a SCRIPT element */
			    if (currentState == 1)
			       /* the previous character was '<' in a text */
			       if (trans->newState == 2)
				  /* the current character is not '/', '!', '<'
				     or space */
				  if (Within (HTML_EL_SCRIPT, DocumentSSchema))
				     /* we are within a SCRIPT element */
				     {
				     /* put '<' and the character read in the
					text buffer */
				     PutInBuffer ('<');
				     PutInBuffer (charRead);
				     charRead = EOS;
				     /* and return to state 0: reading text */
				     currentState = 0;
				     NormalTransition = FALSE;
				     }

			    /* call the procedure associated with the transition */
			    if (trans->action != NULL)
			       (*(trans->action)) (charRead);
			    if (NormalTransition)
			      {
				 /* the input character has been processed */
				 charRead = EOS;
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
   while (!endOfFile && !HTMLrootClosed);
   /* end of HTML file */

   if (!HTMLrootClosed)
      EndOfDocument ();
   HTMLrootClosingTag = NULL;
   HTMLrootClosed = FALSE;
   if (ErrFile) {
      fclose (ErrFile);
      ErrFile = (FILE*) 0;
   } 
}


/*----------------------------------------------------------------------
   ReadTextFile
   read plain text file into a PRE element.
   input text comes from either the infile file or the text
   buffer textbuf. One parameter should be NULL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void        ReadTextFile (FILE *infile, STRING textbuf, Document doc, STRING pathURL)
#else
static void        ReadTextFile (infile, textbuf, doc, pathURL)
FILE              *infile;
STRING	           textbuf;
Document           doc;
STRING	           pathURL;
#endif
{
  Element             parent, el, prev;
  ElementType         elType;
  UCHAR_T             charRead;
  ThotBool            endOfFile;

  InputText = textbuf;
  LgBuffer = 0;
  endOfFile = FALSE;
  NumberOfCharRead = 0;
  NumberOfLinesRead = 1;

  /* initialize input buffer */
  charRead = GetNextInputChar (infile, &CurrentBufChar, &endOfFile);
  parent = TtaGetMainRoot (doc);
  elType = TtaGetElementType (parent);
  el = TtaGetLastChild (parent);
  if (el == NULL)
    {
      /* insert the Document_URL element */
      elType.ElTypeNum = TextFile_EL_Document_URL;
      prev = TtaNewTree (doc, elType, _EMPTYSTR_);
      TtaInsertFirstChild (&prev, parent, doc);
      if (pathURL != NULL && prev != NULL)
        {
          el = TtaGetFirstChild (prev);
          TtaSetTextContent (el, pathURL, currentLanguage, doc);
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

  /* read the text file sequentially */
  while (!endOfFile)
    {
      if (el == NULL)
	{
	  /* create a new line */
	  elType.ElTypeNum = TextFile_EL_Line_;
	  el = TtaNewTree (doc, elType, _EMPTYSTR_);
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
	}

      /* Check the character read */
      /* Consider LF and FF as the end of an input line. */
      if ((int) charRead == EOL || (int) charRead == 0)
	{
	  /* LF = end of line */
	  inputBuffer[LgBuffer] = EOS;
	  if (LgBuffer != 0)
	    TtaAppendTextContent (el, inputBuffer, doc);
	  LgBuffer = 0;
	  el = NULL;
	  charRead = EOS;
	}
      else if ((charRead < SPACE || (int) charRead >= 254 ||
		((int) charRead >= 127 && (int) charRead <= 159))
	       && (int) charRead != TAB)
	/* it's not an end of line */
	/* Ignore non printable characters except HT, LF, FF. */
	/* it's not a printable character, ignore it */
	charRead = EOS;
      if (charRead != EOS)
	{
	  /* a valid character has been read */
	  if (LgBuffer + 1 >= AllmostFullBuffer)
	    {
	      /* store the current buffer contents and continue */
	      inputBuffer[LgBuffer] = EOS;
	      TtaAppendTextContent (el, inputBuffer, doc);
	      LgBuffer = 0;
	    }
	  inputBuffer[LgBuffer++] = charRead;
	}

      /* read next character from the source */
      charRead = GetNextInputChar (infile, &CurrentBufChar, &endOfFile);
    }
  /* close the document */
  if (LgBuffer != 0)
    {
      inputBuffer[LgBuffer] = EOS;
      TtaAppendTextContent (el, inputBuffer, doc);
    }
}

/*----------------------------------------------------------------------
  IsXHTMLDocType parses the XML file to detect if it's XHML document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            IsXHTMLDocType (STRING fileName)
#else
ThotBool            IsXHTMLDocType (fileName)
STRING              fileName;
#endif
{
  gzFile              stream;
  int                 res, i;
  ThotBool            endOfFile, isXHTML;

  isXHTML = FALSE;
  stream = gzopen (WideChar2ISO (fileName), "r");
  if (stream != 0)
    {
      InputText = NULL;
      LgBuffer = 0;
      endOfFile = FALSE;
      while (!endOfFile)
	{
	  res = gzread (stream, FileBuffer, INPUT_FILE_BUFFER_SIZE);
	  if (res <= 0)
	    endOfFile = TRUE;
	  else
	    FileBuffer[res] = EOS;
	  /* check if we are reading a file with "<html ...>" */
	  i = 0;
	  while (!endOfFile && i < res)
	    {
	      if (ustrncasecmp(&FileBuffer[i], TEXT("<html"), 5))
		i++;
	      else
		{
		  /* we've found <html  */
		  i += 5;
		  /* stop the research */
		  endOfFile = TRUE;
		  if (FileBuffer[i] == SPACE ||
			 FileBuffer[i] == BSPACE ||
			 FileBuffer[i] == EOL ||
			 FileBuffer[i] == TAB ||
			 FileBuffer[i] == __CR__ ||
			 FileBuffer[i] == '>')
		     isXHTML = TRUE;
		}
	    }
	}
      gzclose (stream);
    }
  return (isXHTML);
}


/*----------------------------------------------------------------------
  ContentIsXML parses the HTML file to detect if it's XHML document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            ContentIsXML (STRING fileName)
#else
ThotBool            ContentIsXML (fileName)
STRING              fileName;
#endif
{
  gzFile              stream;
  int                 res, i;
  ThotBool            endOfFile, isXHTML;

  isXHTML = FALSE;
  stream = gzopen (WideChar2ISO (fileName), "r");
  if (stream != 0)
    {
      InputText = NULL;
      LgBuffer = 0;
      endOfFile = FALSE;
      while (!endOfFile)
	{
	  res = gzread (stream, FileBuffer, INPUT_FILE_BUFFER_SIZE);
	  if (res <= 0)
	    endOfFile = TRUE;
	  else
	    FileBuffer[res] = EOS;
	  /* check if the file contains "<?xml ..." */
	  i = 0;
	  while (!endOfFile && i < res)
	    {
	      if (ustrncasecmp(&FileBuffer[i], TEXT("<?xml"), 5))
		i++;
	      else
		{
		  /* we've found <?xml */
		  i += 5;
		  /* stop the research */
		  endOfFile = TRUE;
		  if (FileBuffer[i] == SPACE ||
			 FileBuffer[i] == BSPACE ||
			 FileBuffer[i] == EOL ||
			 FileBuffer[i] == TAB ||
			 FileBuffer[i] == __CR__)
		     isXHTML = TRUE;
		}
	    }
	}
      gzclose (stream);
    }
  return (isXHTML);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CheckHeadElements (Element el, Element *elHead, Element *elBody, Document doc)
#else
static void         CheckHeadElements (el, elHead, elBody, doc)
Element             el;
Element            *elHead;
Element            *elBody, 
Document            doc;
#endif
{
  Element           nextEl, rootElement, lastChild;
  ElementType       elType;

  /* check all children of the given element */
  el = TtaGetFirstChild (el);
  lastChild = NULL;
  while (el != NULL)
    {
      nextEl = el;
      TtaNextSibling (&nextEl);
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == HTML_EL_BODY && *elBody == NULL)
	*elBody = el;
      else if (elType.ElTypeNum == HTML_EL_TITLE ||
	       elType.ElTypeNum == HTML_EL_ISINDEX ||
	       elType.ElTypeNum == HTML_EL_BASE ||
	       elType.ElTypeNum == HTML_EL_STYLE_ ||
	       elType.ElTypeNum == HTML_EL_META ||
	       elType.ElTypeNum == HTML_EL_LINK)
	/* this element should be a child of HEAD */
	{
	  /* create the HEAD element if it does not exist */
	  if (*elHead == NULL)
	    {
	      rootElement = TtaGetMainRoot (doc);
	      elType.ElTypeNum = HTML_EL_HEAD;
	      *elHead = TtaNewElement (doc, elType);
	      TtaInsertFirstChild (elHead, rootElement, doc);
	      /* create also the title */
	      elType.ElTypeNum = HTML_EL_TITLE;
	      lastChild = TtaNewTree (doc, elType, _EMPTYSTR_);
	      TtaInsertFirstChild (&lastChild, *elHead, doc);
	    }
	  else
	    {
	      elType.ElTypeNum = HTML_EL_TITLE;
	      lastChild = TtaSearchTypedElement (elType, SearchForward, *elHead);
	    }
	  /* move the element as the last child of the HEAD element */
	  TtaRemoveTree (el, doc);
	  TtaInsertSibling (el, lastChild, FALSE, doc);
	  lastChild = el;
	}
      /* get next child of the root */
      el = nextEl;
    }
}


/*----------------------------------------------------------------------
  EncloseCharLevelElem
  create a copy of element charEl for all descendants of el which are not
  block level elements.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EncloseCharLevelElem (Element el, Element charEl, Document doc)
#else
static void         EncloseCharLevelElem (el, charEl, doc)
Element	el;
Element charEl;
Document doc;

#endif
{
   Element      child, next, copy, prev, elem;
   ElementType	elType;

   if (IsEmptyElement (el))
      return;
   elType = TtaGetElementType (el);
   if (elType.ElTypeNum == HTML_EL_Table_head ||
       elType.ElTypeNum == HTML_EL_C_Foot ||
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
	       EncloseCharLevelElem (elem, charEl, doc);
	       prev = NULL;
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
		   copy = TtaCopyTree (charEl, doc, doc, el);
		   TtaInsertSibling (copy, elem, TRUE, doc);
		   TtaRemoveTree (elem, doc);
		   TtaInsertFirstChild (&elem, copy, doc);
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
#ifdef __STDC__
static void MergeElements (Element old, Element el, Document doc)
#else
static void MergeElements (old, el, doc)
Element old;
Element	el;
Document doc;

#endif
{
  Element	elem, next, prev, sibling;

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
#ifdef __STDC__
static void MergePseudoParagraph (Element el, Document doc)
#else
static void MergePseudoParagraph (el, doc)
Element	el;
Document doc;

#endif
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
#ifdef __STDC__
static void         CheckBlocksInCharElem (Document doc)
#else
static void         CheckBlocksInCharElem (doc)
Document            doc;

#endif
{
   Element             el, parent, child, first, last, next, copy, newparent,
		       elem, prev, firstNotCharElem;
   PtrElemToBeChecked  elTBC, nextElTBC, TBC;
   ElementType	       elType, parentType;

   /* check all block-level elements whose parent was a character-level
      element */
   elTBC = FirstElemToBeChecked;
   while (elTBC != NULL)
     {
     el = elTBC->Elem;
     while (el != NULL)
       {
       parent = TtaGetParent (el);
       if (parent == NULL)
	 el = NULL;
       else
	 if (!IsCharacterLevelElement (parent))
	   {
	   MergePseudoParagraph (el, doc);
	   el = NULL;
	   }
	 else
	   {
	   /* move all children of element parent as siblings of this element*/
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
	   /* copy the character-level element for all elements that have been
	      moved */
	   newparent = TtaGetParent (parent);
	   elem = first;
	   prev = NULL;
	   firstNotCharElem = NULL;
	   parentType = TtaGetElementType (parent);
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
	     if (!IsCharacterLevelElement (elem) &&
		 elType.ElTypeNum != HTML_EL_Comment_)
	       /* This is not a character level element */
	       /* create copies of element parent for all decendants of child*/
	       {
	       EncloseCharLevelElem (elem, parent, doc);
	       prev = NULL;
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

	   /* if, among the elements that have just been moved, there are
	      pseudo-paragraphs which are now children of a block element,
	      remove these pseudo-paragraphs */
	   elem = firstNotCharElem;
	   parent = TtaGetParent (firstNotCharElem);
	   if (parent != NULL)
	      if (!IsBlockElement (parent))
	         elem = NULL;
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
   CheckAbstractTree       Check the Thot abstract tree and create
   the missing elements.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CheckAbstractTree (STRING pathURL)
#else
void                CheckAbstractTree (pathURL)
STRING              pathURL;

#endif
{
   ElementType	elType, newElType, headElType;
   Element	el, elHead, elBody, elFrameset, elNoframes, nextEl, newEl,
		prevEl, lastChild, firstTerm, lastTerm, termList, child,
		parent, firstEntry, lastEntry, glossary, list, elText,
		previous;
   ThotBool	ok, moved;

   /* the root element only accepts elements HEAD, BODY, FRAMESET and Comment*/
   /* as children */
   elHead = NULL;
   elBody = NULL;
   elFrameset = NULL;
   elNoframes = NULL;
   el = TtaGetFirstChild (rootElement);
   if (el != NULL)
     {
	elType = TtaGetElementType (el);
	/* skip Comments and Invalid_elements */
	while (el != NULL && (elType.ElTypeNum == HTML_EL_Comment_ ||
			      elType.ElTypeNum == HTML_EL_Invalid_element))
	  {
	     TtaNextSibling (&el);
	     if (el != NULL)
		elType = TtaGetElementType (el);
	  }
	if (elType.ElTypeNum == HTML_EL_HEAD)
	   /* the first child of the root element is HEAD */
	  {
	     elHead = el;
	     TtaNextSibling (&el);
	     if (el != NULL)
		elType = TtaGetElementType (el);
	  }
	else
	  {
	     elType.ElTypeNum = HTML_EL_HEAD;
	     elHead = TtaSearchTypedElement (elType, SearchForward, rootElement);
	     if (elHead != NULL)
		/* an element HEAD has been found */
	       {
		  /* move the HEAD element before the current element */
		  TtaRemoveTree (elHead, theDocument);
		  TtaInsertSibling (elHead, el, TRUE, theDocument);
	       }
	  }
	/* skip Comments and Invalid_elements */
	while (el != NULL && (elType.ElTypeNum == HTML_EL_Comment_ ||
			      elType.ElTypeNum == HTML_EL_Invalid_element))
	  {
	     TtaNextSibling (&el);
	     if (el != NULL)
		elType = TtaGetElementType (el);
	  }
	if (el != NULL)
	  {
	     if (elType.ElTypeNum == HTML_EL_BODY)
		/* this child of the root element is BODY */
		elBody = el;
	  }
	/* check all children of the root element */
	CheckHeadElements (rootElement, &elHead, &elBody, theDocument);
	if (elBody != NULL)
	  CheckHeadElements (elBody, &elHead, &elBody, theDocument);

	if (elHead == NULL)
	   /* there is no HEAD element. Create one */
	  {
	     newElType.ElSSchema = DocumentSSchema;
	     newElType.ElTypeNum = HTML_EL_HEAD;
	     elHead = TtaNewTree (theDocument, newElType, _EMPTYSTR_);
	     TtaInsertFirstChild (&elHead, rootElement, theDocument);
	  }

	if (elHead != NULL)
	  {
	     headElType = TtaGetElementType (elHead);
	     /* create a Document_URL element as the first child of HEAD */
	     newElType.ElSSchema = DocumentSSchema;
	     newElType.ElTypeNum = HTML_EL_Document_URL;
	     el = TtaGetFirstChild (elHead);
	     if (el != NULL)
	       {
		  elType = TtaGetElementType (el);
		  if (elType.ElTypeNum == newElType.ElTypeNum)
		     /* element Document_URL already exists */
		     elText = TtaGetFirstChild (el);
		  else
		     el = NULL;
	       }
	     if (el == NULL)
		/* there is no Document_URL element */
	       {
		  el = TtaNewElement (theDocument, newElType);
		  TtaInsertFirstChild (&el, elHead, theDocument);
		  TtaSetAccessRight (el, ReadOnly, theDocument);
		  newElType.ElTypeNum = HTML_EL_TEXT_UNIT;
		  elText = TtaNewElement (theDocument, newElType);
		  TtaInsertFirstChild (&elText, el, theDocument);
	       }
	     if (pathURL != NULL && elText != NULL)
		TtaSetTextContent (elText, pathURL, currentLanguage, theDocument);
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
		    /* this element is not a valid component of aggregate
		       HEAD. It may be an SGML inclusion, let's check */
		    if (!TtaCanInsertFirstChild (elType, elHead, theDocument))
		      /* this element cannot be a child of HEAD, move it to
			 the BODY */
		      {
		        /* create the BODY element if it does not exist */
		        if (elBody == NULL)
		          {
			    newElType.ElSSchema = DocumentSSchema;
			    newElType.ElTypeNum = HTML_EL_BODY;
			    elBody = TtaNewElement (theDocument, newElType);
			    TtaInsertSibling (elBody, elHead, FALSE, theDocument);
		          }
		        /* move the current element into the BODY element */
		        TtaRemoveTree (el, theDocument);
		        if (lastChild == NULL)
			  TtaInsertFirstChild (&el, elBody, theDocument);
		        else
			  TtaInsertSibling (el, lastChild, FALSE, theDocument);
		        lastChild = el;
		      }
		  el = nextEl;
	       }
	  }
	/* check the children of the root */
	lastChild = NULL;
	el = TtaGetFirstChild (rootElement);
	previous = elHead;
	moved = FALSE;
	while (el != NULL)
	  {
	     nextEl = el;
	     TtaNextSibling (&nextEl);
	     elType = TtaGetElementType (el);
	     if (elType.ElTypeNum == HTML_EL_BODY)
		/* stop */
		nextEl = NULL;
	     else if (elType.ElTypeNum == HTML_EL_FRAMESET)
		{
		if (elFrameset == NULL)
		   elFrameset = el;
		}
	     else if (elType.ElTypeNum == HTML_EL_NOFRAMES)
		{
		if (elNoframes == NULL)
		   elNoframes = el;
		}
	     else if (!moved && (elType.ElTypeNum == HTML_EL_Invalid_element ||
				 elType.ElTypeNum == HTML_EL_Comment_))
		/* don't move Comments and Invalid_elements if the previous
		   element has not been moved */
		previous = el;
	     else if (elType.ElTypeNum != HTML_EL_HEAD &&
		      elType.ElTypeNum != HTML_EL_FRAMESET)
		/* this element should be a child of BODY */
	       {
		  /* create the BODY element if it does not exist */
		  if (elBody == NULL)
		    {
		       newElType.ElSSchema = DocumentSSchema;
		       newElType.ElTypeNum = HTML_EL_BODY;
		       elBody = TtaNewElement (theDocument, newElType);
		       if (previous == NULL)
			  TtaInsertFirstChild (&elBody, rootElement, theDocument);
		       else
			  TtaInsertSibling (elBody, previous, FALSE, theDocument);
		    }
		  /* move the current element into the BODY element */
		  TtaRemoveTree (el, theDocument);
		  if (lastChild == NULL)
		     TtaInsertFirstChild (&el, elBody, theDocument);
		  else
		     TtaInsertSibling (el, lastChild, FALSE, theDocument);
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
	     TtaRemoveTree (elNoframes, theDocument);
	     if (previous == NULL)
		TtaInsertFirstChild (&elNoframes, elFrameset, theDocument);
	     else
		TtaInsertSibling (elNoframes, previous, FALSE, theDocument);
	     }

	/* handle character-level elements which contain block-level elements*/
	CheckBlocksInCharElem (theDocument);
	/* create an element Term_List for each sequence of elements Term */
	el = TtaGetFirstChild (rootElement);
	if (el != NULL)
	  {
	     elType = TtaGetElementType (el);
	     elType.ElTypeNum = HTML_EL_Term;
	     /* search all elements Term in the document */
	     while (el != NULL)
	       {
		  elType.ElTypeNum = HTML_EL_Term;
		  el = TtaSearchTypedElement (elType, SearchForward, el);
		  if (el != NULL)
		     /* an element Term has been found */
		    {
		       /* search all immediate Term siblings, ignoring Comments */
		       /* and Invalid_elements */
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
		       while (elType.ElTypeNum == HTML_EL_Term ||
			      elType.ElTypeNum == HTML_EL_Invalid_element ||
			      elType.ElTypeNum == HTML_EL_Comment_);
		       termList = TtaGetParent (firstTerm);
		       elType = TtaGetElementType (termList);
		       if (elType.ElTypeNum != HTML_EL_Term_List)
			 {
			    /* create a Term_List element before the first Term element */
			    newElType.ElSSchema = DocumentSSchema;
			    newElType.ElTypeNum = HTML_EL_Term_List;
			    termList = TtaNewElement (theDocument, newElType);
			    TtaInsertSibling (termList, firstTerm, TRUE, theDocument);
			    /* move the Term elements as children of the new Term_List */
			    nextEl = firstTerm;
			    TtaNextSibling (&nextEl);
			    TtaRemoveTree (firstTerm, theDocument);
			    TtaInsertFirstChild (&firstTerm, termList, theDocument);
			    if (lastTerm != firstTerm)
			      {
				 prevEl = firstTerm;
				 do
				   {
				      child = nextEl;
				      TtaNextSibling (&nextEl);
				      TtaRemoveTree (child, theDocument);
				      TtaInsertSibling (child, prevEl, FALSE, theDocument);
				      prevEl = child;
				   }
				 while (nextEl != NULL && child != lastTerm);
			      }
			 }
		       parent = TtaGetParent (termList);
		       elType = TtaGetElementType (parent);
		       if (elType.ElTypeNum != HTML_EL_Definition_Item)
			 {
			    /* Create a Definition_Item element surrounding the */
			    /* Term_List element */
			    newElType.ElSSchema = DocumentSSchema;
			    newElType.ElTypeNum = HTML_EL_Definition_Item;
			    newEl = TtaNewElement (theDocument, newElType);
			    TtaInsertSibling (newEl, termList, TRUE, theDocument);
			    TtaRemoveTree (termList, theDocument);
			    TtaInsertFirstChild (&termList, newEl, theDocument);
			 }
		       if (el != NULL)
			 {
			    elType = TtaGetElementType (el);
			    if (elType.ElTypeNum == HTML_EL_Definition)
			       /* the element following the new Term_List element is */
			       /* a Definition. Move it as a sibling of the Term_List */
			      {
				 TtaRemoveTree (el, theDocument);
				 TtaInsertSibling (el, termList, FALSE, theDocument);
			      }
			    else
			      {
				 /* the element following the Term_List element is not */
				 /* a Definition. Create a Definition element surrounding */
				 /* that element */
				 newElType.ElSSchema = DocumentSSchema;
				 newElType.ElTypeNum = HTML_EL_Definition;
				 newEl = TtaNewElement (theDocument, newElType);
				 TtaInsertSibling (newEl, termList, FALSE, theDocument);
				 nextEl = el;
				 TtaNextSibling (&nextEl);
				 elType = TtaGetElementType (el);
				 TtaRemoveTree (el, theDocument);
				 TtaInsertFirstChild (&el, newEl, theDocument);
				 while (nextEl != NULL &&
					(elType.ElTypeNum == HTML_EL_Invalid_element ||
				      elType.ElTypeNum == HTML_EL_Comment_))
				   {
				      previous = el;
				      el = nextEl;
				      TtaNextSibling (&nextEl);
				      elType = TtaGetElementType (el);
				      TtaRemoveTree (el, theDocument);
				      TtaInsertSibling (el, previous, FALSE, theDocument);
				   }
			      }
			 }
		       /* starting element for the next search of a Term element */
		       el = lastTerm;
		    }
	       }
	  }
	/* search all Definition elements without a Definition_Item parent */
	el = TtaGetFirstChild (rootElement);
	if (el != NULL)
	  {
	     elType = TtaGetElementType (el);
	     /* search all elements Definition in the document */
	     while (el != NULL)
	       {
		  elType.ElTypeNum = HTML_EL_Definition;
		  el = TtaSearchTypedElement (elType, SearchForward, el);
		  if (el != NULL)
		     /* an element Definition has been found */
		    {
		       parent = TtaGetParent (el);
		       elType = TtaGetElementType (parent);
		       if (elType.ElTypeNum != HTML_EL_Definition_Item)
			  /* this Definition is not within a Definition_Item */
			 {
			    /* create a Definition_Item */
			    newElType.ElSSchema = DocumentSSchema;
			    newElType.ElTypeNum = HTML_EL_Definition_Item;
			    newEl = TtaNewElement (theDocument, newElType);
			    TtaInsertSibling (newEl, el, TRUE, theDocument);
			    TtaRemoveTree (el, theDocument);
			    TtaInsertFirstChild (&el, newEl, theDocument);
			 }
		    }
	       }
	  }
	/* create a surrounding element Definition_List for each sequence */
	/* of elements Definition_Item which are not in a Definition_List */
	el = TtaGetFirstChild (rootElement);
	if (el != NULL)
	  {
	     elType = TtaGetElementType (el);
	     /* search all elements Definition_Item in the document */
	     while (el != NULL)
	       {
		  elType.ElTypeNum = HTML_EL_Definition_Item;
		  el = TtaSearchTypedElement (elType, SearchForward, el);
		  if (el != NULL)
		     /* an element Definition_Item has been found */
		    {
		       parent = TtaGetParent (el);
		       elType = TtaGetElementType (parent);
		       if (elType.ElTypeNum != HTML_EL_Definition_List)
			  /* this Definition_Item is not within a Definition_List */
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
			    while (elType.ElTypeNum == HTML_EL_Definition_Item ||
			      elType.ElTypeNum == HTML_EL_Invalid_element ||
				   elType.ElTypeNum == HTML_EL_Comment_);
			    /* create a Definition_List element before the */
			    /* first Definition_Item element */
			    newElType.ElSSchema = DocumentSSchema;
			    newElType.ElTypeNum = HTML_EL_Definition_List;
			    glossary = TtaNewElement (theDocument, newElType);
			    TtaInsertSibling (glossary, firstEntry, TRUE,
					      theDocument);
			    /* move the Definition_Item elements as children */
			    /* of the new Definition_List element */
			    nextEl = firstEntry;
			    TtaNextSibling (&nextEl);
			    TtaRemoveTree (firstEntry, theDocument);
			    TtaInsertFirstChild (&firstEntry, glossary,
						 theDocument);
			    if (lastEntry != firstEntry)
			      {
				 prevEl = firstEntry;
				 do
				   {
				      child = nextEl;
				      TtaNextSibling (&nextEl);
				      TtaRemoveTree (child, theDocument);
				      TtaInsertSibling (child, prevEl, FALSE,
							theDocument);
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
	el = TtaGetFirstChild (rootElement);
	if (el != NULL)
	  {
	     elType = TtaGetElementType (el);
	     /* search all elements List_Item in the document */
	     do
	       {
		  elType.ElTypeNum = HTML_EL_List_Item;
		  el = TtaSearchTypedElement (elType, SearchForward, el);
		  if (el != NULL)
		     /* an element List_Item has been found */
		    {
		       parent = TtaGetParent (el);
		       elType = TtaGetElementType (parent);
		       if (elType.ElTypeNum != HTML_EL_Unnumbered_List &&
			   elType.ElTypeNum != HTML_EL_Numbered_List &&
			   elType.ElTypeNum != HTML_EL_Menu &&
			   elType.ElTypeNum != HTML_EL_Directory)
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
			    while (elType.ElTypeNum == HTML_EL_List_Item ||
			         elType.ElTypeNum == HTML_EL_Invalid_element ||
				 elType.ElTypeNum == HTML_EL_Comment_);
			    /* create a HTML_EL_Unnumbered_List element before
			       the first List_Item element */
			    newElType.ElSSchema = DocumentSSchema;
			    newElType.ElTypeNum = HTML_EL_Unnumbered_List;
			    list = TtaNewElement (theDocument, newElType);
			    TtaInsertSibling (list, firstEntry, TRUE,
					      theDocument);
			    /* move the List_Item elements as children of */
			    /* the new HTML_EL_Unnumbered_List element */
			    nextEl = firstEntry;
			    TtaNextSibling (&nextEl);
			    TtaRemoveTree (firstEntry, theDocument);
			    TtaInsertFirstChild (&firstEntry, list,
						 theDocument);
			    if (lastEntry != firstEntry)
			      {
				 prevEl = firstEntry;
				 do
				   {
				      child = nextEl;
				      TtaNextSibling (&nextEl);
				      TtaRemoveTree (child, theDocument);
				      TtaInsertSibling (child, prevEl, FALSE,
							theDocument);
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
	/* merge sibling Text elements with same attributes */
	el = rootElement;
	elType = TtaGetElementType (el);
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
		   ok = TtaMergeText (el, theDocument);
		while (ok);
	  }

	/* checks all MAP elements. If they are within a Block element, */
	/* move them up in the structure */
	el = rootElement;
	elType = TtaGetElementType (el);
	elType.ElTypeNum = HTML_EL_MAP;
	/* search all MAP elements in the document */
	while (el != NULL)
	  {
	     /* search the next MAP element in the abstract tree */
	     el = TtaSearchTypedElement (elType, SearchForward, el);
	     if (el != NULL)
		/* a MAP element has been found. */
		{
		parent = TtaGetParent(el);
		if (IsBlockElement (parent))
		  /* its parent is a block element */
		  {
		  TtaRemoveTree (el, theDocument);
		  TtaInsertSibling (el, parent, TRUE, theDocument);
		  }
		}
	  }

	/* If element BODY is empty, create an empty element as a placeholder*/
        if (elBody != NULL)
	  if (TtaGetFirstChild (elBody) == NULL)
	     {
	     newElType.ElSSchema = DocumentSSchema;
	     newElType.ElTypeNum = HTML_EL_Element;
	     newEl = TtaNewElement (theDocument, newElType);
	     TtaInsertFirstChild (&newEl, elBody, theDocument);
	     }

	/* add other checks here */
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
#ifdef __STDC__
static void         InitializeHTMLParser (Element lastelem, ThotBool isclosed, Document doc)
#else  /* __STDC__ */
static void         InitializeHTMLParser (lastelem, isclosed, doc)
Element             lastelem;
ThotBool            isclosed;
Document            doc;
#endif  /* __STDC__ */
{
   CHAR_T                tag[20];
   Element             elem;
   int                 i;
   SSchema	       schema;

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
   MergeText = FALSE;
   AfterTagPRE = FALSE;
   ParsingCSS = FALSE;
   CurrentBufChar = 0;
}

/*----------------------------------------------------------------------
   ParseIncludedHTML
   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void       ParseIncludedHTML (Element elem, STRING closingTag)
#else
void       ParseIncludedHTML (elem, closingTag)
Element		elem;
STRING	        closingTag;

#endif
{
   Element	oldLastElement;
   ThotBool	oldLastElementClosed;
   int		oldLastElemEntry;
   

   oldLastElement = lastElement;
   lastElement = elem;
   oldLastElementClosed = lastElementClosed;
   lastElementClosed = FALSE;
   oldLastElemEntry = lastElemEntry;
   lastElemEntry = 0;
   HTMLrootClosingTag = closingTag;
   /* TODO: the XML parser must call that function with two new parameters:
      the current infile and current index */
   HTMLparse (stream, NULL);

   lastElement = oldLastElement;
   lastElementClosed = oldLastElementClosed;
   lastElemEntry = oldLastElemEntry;
}


/*----------------------------------------------------------------------
   ParseSubTree
   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void       ParseSubTree (STRING HTMLbuf, Element lastelem, ThotBool isclosed, Document doc)
#else
void       ParseSubTree (HTMLbuf, lastelem, isclosed, doc)
STRING	   HTMLbuf;
Element		lastelem;
ThotBool		isclosed;
Document	doc;

#endif
{
   ElementType	elType;
   STRING	schemaName;

   docURL = NULL;
   elType = TtaGetElementType (lastelem);
   schemaName = TtaGetSSchemaName(elType.ElSSchema);
   if (ustrcmp (schemaName, TEXT("HTML")) == 0)
     /* parse an HTML subtree */
     {
       InitializeHTMLParser (lastelem, isclosed, doc);
       HTMLparse (NULL, HTMLbuf);
     }
   else
     /* parse an XML subtree */
     {
       InputText = HTMLbuf;
       CurrentBufChar = 0;
#ifndef STANDALONE
       XMLparse (NULL, &CurrentBufChar, schemaName, doc, lastelem, isclosed,
		 TtaGetDefaultLanguage(), NULL);
#endif
      }
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
	  /* initialize automaton for the HTML parser */
	  InitAutomaton ();
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
	  doc = TtaNewDocument ("HTML", pivotFileName);
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
   StartParser loads the file Directory/htmlFileName for
   displaying the document documentName.
   The parameter pathURL gives the original (local or
   distant) path or URL of the html document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                StartParser (Document doc, STRING htmlFileName, STRING documentName, STRING documentDirectory, STRING pathURL, ThotBool plainText)
#else
void                StartParser (doc, htmlFileName, documentName, documentDirectory, pathURL, plainText)
Document            doc;
STRING              htmlFileName;
STRING              documentName;
STRING              documentDirectory;
STRING              pathURL;
ThotBool	    plainText;
#endif
{
  Element             el, oldel;
  AttributeType       attrType;
  Attribute           attr;
  STRING              s;
  CHAR_T              tempname[MAX_LENGTH];
  CHAR_T              temppath[MAX_LENGTH];
  int		      length;
  ThotBool            isHTML;

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
  stream = gzopen (WideChar2ISO (htmlFileName), "r");
  if (stream != 0)
    {
      FileBuffer[0] = EOS;
      WithinTable = 0;
      if (documentName[0] == EOS && !TtaCheckDirectory (documentDirectory))
	{
	  ustrcpy (documentName, documentDirectory);
	  documentDirectory[0] = EOS;
	  s = TtaGetEnvString ("PWD");
	  /* set path on current directory */
	  if (s != NULL)
	    ustrcpy (documentDirectory, s);
	  else
	    documentDirectory[0] = EOS;
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
	    docURL = TtaAllocString (length+1);
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
	isHTML = (ustrcmp(TtaGetSSchemaName (DocumentSSchema), TEXT("HTML")) == 0);
	if (plainText)
	  {
	    if (isHTML)
	      {
		/* change the document type */
		TtaFreeView (doc, 1);
		doc = TtaNewDocument (TEXT("TextFile"), documentName);
		TtaSetPSchema (doc, TEXT("TextFileP"));
		DocumentSSchema = TtaGetDocumentSSchema (doc);
		isHTML = FALSE;
	      }
	    
	    /* default ATTR_PrintURL */
	    attrType.AttrSSchema = DocumentSSchema;
	    attrType.AttrTypeNum = TextFile_ATTR_PrintURL;
	  }
	else
	  {
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
	    /* default ATTR_PrintURL */
	    attrType.AttrSSchema = DocumentSSchema;
	    attrType.AttrTypeNum = HTML_ATTR_PrintURL;
	  }
	
	TtaSetDisplayMode (doc, NoComputedDisplay);
	/* add the default attribute HTML_ATTR_PrintURL */
	rootElement = TtaGetMainRoot (doc);
	attr = TtaGetAttribute (rootElement, attrType);
	if (attr == 0)
	  {
	    attr = TtaNewAttribute (attrType);
	    TtaAttachAttribute (rootElement, attr, doc);
	  }
	
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
	/* parse the input file and build the Thot document */
	if (plainText)
	  ReadTextFile (stream, NULL, doc, pathURL);
	else
	  {
	    /* initialize parsing environment */
	    InitializeHTMLParser (NULL, FALSE, 0);
	    HTMLparse (stream, NULL);
	    /* completes all unclosed elements */
	    el = lastElement;
	    while (el != NULL)
	      {
		ElementComplete (el);
		el = TtaGetParent (el);
	      }
	    /* check the Thot abstract tree */
	    CheckAbstractTree (pathURL);
	  }
	gzclose (stream);
#ifdef STANDALONE
	/* save and close the Thot document */
	TtaSaveDocument (doc, pivotFileName);
	TtaCloseDocument (doc);
#else  /* STANDALONE */
        #ifdef AMAYA_JAVA
	handleLinkHeaders (doc);
        #endif /* AMAYA_JAVA */
	TtaFreeMemory (docURL);
	/* an HTML document could be a template */
	if (!plainText)
	  OpenTemplateDocument (doc);
	TtaSetDisplayMode (doc, DisplayImmediately);
#endif /* STANDALONE */
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
