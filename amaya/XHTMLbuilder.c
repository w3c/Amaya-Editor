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
 * Authors: L. Carcone
 *          V. Quint 
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "parser.h"
#include "HTML.h"

#include "css_f.h"
#include "fetchXMLname_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLimage_f.h"
#include "HTMLtable_f.h"
#include "HTMLimage_f.h"
#include "UIcss_f.h"
#include "styleparser_f.h"
#include "XHTMLbuilder_f.h"
#include "Xml2thot_f.h"

/* maximum length of a Thot structure schema name */
#define MAX_SS_NAME_LENGTH 32

XhtmlEntity        XhtmlEntityTable[] =
{
/* This table MUST be in alphabetical order */
{TEXT("AElig"), 198},	/* latin capital letter AE = */ 
                        /* latin capital ligature AE, U+00C6 ISOlat1 */
{TEXT("Aacute"), 193},	/* latin capital letter A with acute, U+00C1 ISOlat1 */
{TEXT("Acirc"), 194},	/* latin capital letter A with circumflex, U+00C2 ISOlat1 */
{TEXT("Agrave"), 192},	/* latin capital letter A with grave = */
                        /* latin capital letter A grave, U+00C0 ISOlat1 */
{TEXT("Alpha"), 913},	/* greek capital letter alpha, U+0391 */
{TEXT("Aring"), 197},	/* latin capital letter A with ring above = */ 
                        /* latin capital letter A ring, U+00C5 ISOlat1 */
{TEXT("Atilde"), 195},	/* latin capital letter A with tilde, U+00C3 ISOlat1 */
{TEXT("Auml"), 196},	/* latin capital letter A with diaeresis, U+00C4 ISOlat1 */
{TEXT("Beta"), 914},	/* greek capital letter beta, U+0392 */
{TEXT("Ccedil"), 199},	/* latin capital letter C with cedilla, U+00C7 ISOlat1 */
{TEXT("Chi"), 935},	/* greek capital letter chi, U+03A7 */
{TEXT("Dagger"), 8225},	/* double dagger, U+2021 ISOpub */
{TEXT("Delta"), 916},	/* greek capital letter delta, U+0394 ISOgrk3 */
{TEXT("ETH"), 208},	/* latin capital letter ETH, U+00D0 ISOlat1 */
{TEXT("Eacute"), 201},	/* latin capital letter E with acute, U+00C9 ISOlat1 */
{TEXT("Ecirc"), 202},	/* latin capital letter E with circumflex, U+00CA ISOlat1 */
{TEXT("Egrave"), 200},	/* latin capital letter E with grave, U+00C8 ISOlat1 */
{TEXT("Epsilon"), 917},	/* greek capital letter epsilon, U+0395 */
{TEXT("Eta"), 919},	/* greek capital letter eta, U+0397 */
{TEXT("Euml"), 203},	/* latin capital letter E with diaeresis, U+00CB ISOlat1 */
{TEXT("Gamma"), 915},	/* greek capital letter gamma, U+0393 ISOgrk3 */
{TEXT("Iacute"), 205},	/* latin capital letter I with acute, U+00CD ISOlat1 */
{TEXT("Icirc"), 206},	/* latin capital letter I with circumflex, U+00CE ISOlat1 */
{TEXT("Igrave"), 204},	/* latin capital letter I with grave, U+00CC ISOlat1 */
{TEXT("Iota"), 921},	/* greek capital letter iota, U+0399 */
{TEXT("Iuml"), 207},	/* latin capital letter I with diaeresis, U+00CF ISOlat1 */
{TEXT("Kappa"), 922},	/* greek capital letter kappa, U+039A */
{TEXT("Lambda"), 923},	/* greek capital letter lambda, U+039B ISOgrk3 */
{TEXT("Mu"), 924},	/* greek capital letter mu, U+039C */
{TEXT("Ntilde"), 209},	/* latin capital letter N with tilde, U+00D1 ISOlat1 */
{TEXT("Nu"), 925},	/* greek capital letter nu, U+039D */
{TEXT("OElig"), 338},	/* latin capital ligature OE, U+0152 ISOlat2 */
{TEXT("Oacute"), 211},	/* latin capital letter O with acute, U+00D3 ISOlat1 */
{TEXT("Ocirc"), 212},  	/* latin capital letter O with circumflex, U+00D4 ISOlat1 */
{TEXT("Ograve"), 210},	/* latin capital letter O with grave, U+00D2 ISOlat1 */
{TEXT("Omega"), 937},  	/* greek capital letter omega, U+03A9 ISOgrk3 */
{TEXT("Omicron"), 927},	/* greek capital letter omicron, U+039F */
{TEXT("Oslash"), 216},	/* latin capital letter O with stroke = */ 
                        /* latin capital letter O slash, U+00D8 ISOlat1 */
{TEXT("Otilde"), 213},	/* latin capital letter O with tilde, U+00D5 ISOlat1 */
{TEXT("Ouml"), 214},   	/* latin capital letter O with diaeresis, U+00D6 ISOlat1 */
{TEXT("Phi"), 934},    	/* greek capital letter phi, U+03A6 ISOgrk3 */
{TEXT("Pi"), 928},     	/* greek capital letter pi, U+03A0 ISOgrk3 */
{TEXT("Prime"), 8243},	/* double prime = seconds = inches, U+2033 ISOtech */
{TEXT("Psi"), 936},    	/* greek capital letter psi, U+03A8 ISOgrk3 */
{TEXT("Rho"), 929},    	/* greek capital letter rho, U+03A1 */
{TEXT("Scaron"), 352},	/* latin capital letter S with caron, U+0160 ISOlat2 */
{TEXT("Sigma"), 931},  	/* greek capital letter sigma, U+03A3 ISOgrk3 */
{TEXT("THORN"), 222},  	/* latin capital letter THORN, U+00DE ISOlat1 */
{TEXT("Tau"), 932},    	/* greek capital letter tau, U+03A4 */
{TEXT("Theta"), 920},  	/* greek capital letter theta, U+0398 ISOgrk3 */
{TEXT("Uacute"), 218},	/* latin capital letter U with acute, U+00DA ISOlat1 */
{TEXT("Ucirc"), 219},  	/* latin capital letter U with circumflex, U+00DB ISOlat1 */
{TEXT("Ugrave"), 217},	/* latin capital letter U with grave, U+00D9 ISOlat1 */
{TEXT("Upsilon"), 933},	/* greek capital letter upsilon, U+03A5 ISOgrk3 */
{TEXT("Uuml"), 220},   	/* latin capital letter U with diaeresis, U+00DC ISOlat1 */
{TEXT("Xi"), 926},     	/* greek capital letter xi, U+039E ISOgrk3 */
{TEXT("Yacute"), 221},	/* latin capital letter Y with acute, U+00DD ISOlat1 */
{TEXT("Yuml"), 376},   	/* latin capital letter Y with diaeresis, U+0178 ISOlat2 */
{TEXT("Zeta"), 918},   	/* greek capital letter zeta, U+0396 */
{TEXT("aacute"), 225},	/* latin small letter a with acute, U+00E1 ISOlat1 */
{TEXT("acirc"), 226},  	/* latin small letter a with circumflex, U+00E2 ISOlat1 */
{TEXT("acute"), 180},  	/* acute accent = spacing acute, U+00B4 ISOdia */
{TEXT("aelig"), 230},  	/* latin small letter ae = */
                        /* latin small ligature ae, U+00E6 ISOlat1 */
{TEXT("agrave"), 224},	/* latin small letter a with grave = */
                        /* latin small letter a grave, U+00E0 ISOlat1 */
{TEXT("alefsym"), 8501},/* alef symbol = first transfinite cardinal, U+2135 NEW */
{TEXT("alpha"), 945},  	/* greek small letter alpha, U+03B1 ISOgrk3 */
{TEXT("amp"), 38},     	/* ampersand, U+0026 ISOnum */
{TEXT("and"), 8743},   	/* logical and = wedge, U+2227 ISOtech */
{TEXT("ang"), 8736},   	/* angle, U+2220 ISOamso */
{TEXT("aring"), 229},  	/* latin small letter a with ring above = */
                        /* latin small letter a ring, U+00E5 ISOlat1 */
{TEXT("asymp"), 8776},	/* almost equal to = asymptotic to, U+2248 ISOamsr */
{TEXT("atilde"), 227},	/* latin small letter a with tilde, U+00E3 ISOlat1 */
{TEXT("auml"), 228},   	/* latin small letter a with diaeresis, U+00E4 ISOlat1 */
{TEXT("bdquo"), 8222},	/* double low-9 quotation mark, U+201E NEW */
{TEXT("beta"), 946},   	/* greek small letter beta, U+03B2 ISOgrk3 */
{TEXT("brvbar"), 166},	/* broken bar = broken vertical bar, U+00A6 ISOnum */
{TEXT("bull"), 8226},  	/* bullet = black small circle, U+2022 ISOpub */
{TEXT("cap"), 8745},   	/* intersection = cap, U+2229 ISOtech */
{TEXT("ccedil"), 231},	/* latin small letter c with cedilla, U+00E7 ISOlat1 */
{TEXT("cedil"), 184},  	/* cedilla = spacing cedilla, U+00B8 ISOdia */
{TEXT("cent"), 162},   	/* cent sign, U+00A2 ISOnum */
{TEXT("chi"), 967},    	/* greek small letter chi, U+03C7 ISOgrk3 */
{TEXT("circ"), 710},   	/* modifier letter circumflex accent, U+02C6 ISOpub */
{TEXT("clubs"), 9827},	/* black club suit = shamrock, U+2663 ISOpub */
{TEXT("cong"), 8773},  	/* approximately equal to, U+2245 ISOtech */
{TEXT("copy"), 169},   	/* copyright sign, U+00A9 ISOnum */
{TEXT("crarr"), 8629},	/* downwards arrow with corner leftwards = */
                        /* carriage return, U+21B5 NEW */
{TEXT("cup"), 8746},   	/* union = cup, U+222A ISOtech */
{TEXT("curren"), 164},	/* currency sign, U+00A4 ISOnum */
{TEXT("dArr"), 8659},  	/* downwards double arrow, U+21D3 ISOamsa */
{TEXT("dagger"), 8224},	/* dagger, U+2020 ISOpub */
{TEXT("darr"), 8595},  	/* downwards arrow, U+2193 ISOnum */
{TEXT("deg"), 176},    	/* degree sign, U+00B0 ISOnum */
{TEXT("delta"), 948},  	/* greek small letter delta, U+03B4 ISOgrk3 */
{TEXT("diams"), 9830},	/* black diamond suit, U+2666 ISOpub */
{TEXT("divide"), 247},	/* division sign, U+00F7 ISOnum */
{TEXT("eacute"), 233},	/* latin small letter e with acute, U+00E9 ISOlat1 */
{TEXT("ecirc"), 234},  	/* latin small letter e with circumflex, U+00EA ISOlat1 */
{TEXT("egrave"), 232},	/* latin small letter e with grave, U+00E8 ISOlat1 */
{TEXT("empty"), 8709},	/* empty set = null set = diameter, U+2205 ISOamso */
{TEXT("emsp"), 8195},  	/* em space, U+2003 ISOpub */
{TEXT("ensp"), 8194},  	/* en space, U+2002 ISOpub */
{TEXT("epsilon"), 949},	/* greek small letter epsilon, U+03B5 ISOgrk3 */
{TEXT("equiv"), 8801},	/* identical to, U+2261 ISOtech */
{TEXT("eta"), 951},    	/* greek small letter eta, U+03B7 ISOgrk3 */
{TEXT("eth"), 240},    	/* latin small letter eth, U+00F0 ISOlat1 */
{TEXT("euml"), 235},   	/* latin small letter e with diaeresis, U+00EB ISOlat1 */
{TEXT("euro"), 8364},  	/* euro sign, U+20AC NEW */
{TEXT("exist"), 8707},  /* there exists, U+2203 ISOtech */
{TEXT("fnof"), 402},   	/* latin small f with hook = function = */
                        /* florin, U+0192 ISOtech */
{TEXT("forall"), 8704},	/* for all, U+2200 ISOtech */
{TEXT("frac12"), 189},	/* vulgar fraction one half = */
                        /*fraction one half, U+00BD ISOnum */
{TEXT("frac14"), 188},	/* vulgar fraction one quarter = */
                        /* fraction one quarter, U+00BC ISOnum */
{TEXT("frac34"), 190},	/* vulgar fraction three quarters = */
                        /* fraction three quarters, U+00BE ISOnum */
{TEXT("frasl"), 8260},	/* fraction slash, U+2044 NEW */
{TEXT("gamma"), 947},  	/* greek small letter gamma, U+03B3 ISOgrk3 */
{TEXT("ge"), 8805},    	/* greater-than or equal to, U+2265 ISOtech */
{TEXT("gt"), 62},      	/* greater-than sign, U+003E ISOnum */
{TEXT("hArr"), 8660},  	/* left right double arrow, U+21D4 ISOamsa */
{TEXT("harr"), 8596},  	/* left right arrow, U+2194 ISOamsa */
{TEXT("hearts"), 9829},	/* black heart suit = valentine, U+2665 ISOpub */
{TEXT("hellip"), 8230},	/* horizontal ellipsis = three dot leader, U+2026 ISOpub */
{TEXT("hyphen"), 173},	/* hyphen = discretionary hyphen, U+00AD ISOnum */
{TEXT("iacute"), 237},	/* latin small letter i with acute, U+00ED ISOlat1 */
{TEXT("icirc"), 238},  	/* latin small letter i with circumflex, U+00EE ISOlat1 */
{TEXT("iexcl"), 161},  	/* inverted exclamation mark, U+00A1 ISOnum */
{TEXT("igrave"), 236},	/* latin small letter i with grave, U+00EC ISOlat1 */
{TEXT("image"), 8465},	/* blackletter capital I = imaginary part, U+2111 ISOamso */
{TEXT("infin"), 8734},	/* infinity, U+221E ISOtech */
{TEXT("int"), 8747},   	/* integral, U+222B ISOtech */
{TEXT("iota"), 953},   	/* greek small letter iota, U+03B9 ISOgrk3 */
{TEXT("iquest"), 191},	/* inverted question mark = */
                        /* turned question mark, U+00BF ISOnum */
{TEXT("isin"), 8712},  	/* element of, U+2208 ISOtech */
{TEXT("iuml"), 239},   	/* latin small letter i with diaeresis, U+00EF ISOlat1 */
{TEXT("kappa"), 954},  	/* greek small letter kappa, U+03BA ISOgrk3 */
{TEXT("lArr"), 8656},  	/* leftwards double arrow, U+21D0 ISOtech */
{TEXT("lambda"), 955},	/* greek small letter lambda, U+03BB ISOgrk3 */
{TEXT("lang"), 9001},  	/* left-pointing angle bracket = bra, U+2329 ISOtech */
{TEXT("laquo"), 171},  	/* left-pointing double angle quotation mark = */
                        /* left pointing guillemet, U+00AB ISOnum */
{TEXT("larr"), 8592},  	/* leftwards arrow, U+2190 ISOnum */
{TEXT("lceil"), 8968},	/* left ceiling = apl upstile, U+2308 ISOamsc */
{TEXT("ldquo"), 8220},	/* left double quotation mark, U+201C ISOnum */
{TEXT("le"), 8804},    	/* less-than or equal to, U+2264 ISOtech */
{TEXT("lfloor"), 8970},	/* left floor = apl downstile, U+230A ISOamsc */
{TEXT("lowast"), 8727},	/* asterisk operator, U+2217 ISOtech */
{TEXT("loz"), 9674},   	/* lozenge, U+25CA ISOpub */
{TEXT("lrm"), 8206},   	/* left-to-right mark, U+200E NEW RFC 2070 */
{TEXT("lsaquo"), 8249},	/* single left-pointing angle quotation mark, */
                        /* U+2039 ISO proposed */
{TEXT("lsquo"), 8216},	/* left single quotation mark, U+2018 ISOnum */
{TEXT("lt"), 60},      	/* less-than sign, U+003C ISOnum */
{TEXT("macr"), 175},   	/* macron = spacing macron = overline = APL overbar, */
                        /* U+00AF ISOdia */
{TEXT("mdash"), 8212},	/* em dash, U+2014 ISOpub */
{TEXT("micro"), 181},  	/* micro sign, U+00B5 ISOnum */
{TEXT("middot"), 183},	/* middle dot = Georgian comma = */
                        /* Greek middle dot, U+00B7 ISOnum */
{TEXT("minus"), 8722},	/* minus sign, U+2212 ISOtech */
{TEXT("mu"), 956},     	/* greek small letter mu, U+03BC ISOgrk3 */
{TEXT("nabla"), 8711},	/* nabla = backward difference, U+2207 ISOtech */
{TEXT("nbsp"), 160},   	/* no-break space = non-breaking space, U+00A0 ISOnum */
{TEXT("ndash"), 8211},	/* en dash, U+2013 ISOpub */
{TEXT("ne"), 8800},    	/* not equal to, U+2260 ISOtech */
{TEXT("ni"), 8715},    	/* contains as member, U+220B ISOtech */
{TEXT("not"), 172},    	/* not sign, U+00AC ISOnum */
{TEXT("notin"), 8713},	/* not an element of, U+2209 ISOtech */
{TEXT("nsub"), 8836},  	/* not a subset of, U+2284 ISOamsn */
{TEXT("ntilde"), 241},	/* latin small letter n with tilde, U+00F1 ISOlat1 */
{TEXT("nu"), 957},     	/* greek small letter nu, U+03BD ISOgrk3 */
{TEXT("oacute"), 243},	/* latin small letter o with acute, U+00F3 ISOlat1 */
{TEXT("ocirc"), 244},  	/* latin small letter o with circumflex, U+00F4 ISOlat1 */
{TEXT("oelig"), 339},  	/* latin small ligature oe, U+0153 ISOlat2 */
{TEXT("ograve"), 242},	/* latin small letter o with grave, U+00F2 ISOlat1 */
{TEXT("oline"), 8254},	/* overline = spacing overscore, U+203E NEW */
{TEXT("omega"), 969},  	/* greek small letter omega, U+03C9 ISOgrk3 */
{TEXT("omicron"), 959},	/* greek small letter omicron, U+03BF NEW */
{TEXT("oplus"), 8853},	/* circled plus = direct sum, U+2295 ISOamsb */
{TEXT("or"), 8744},    	/* logical or = vee, U+2228 ISOtech */
{TEXT("ordf"), 170},   	/* feminine ordinal indicator, U+00AA ISOnum */
{TEXT("ordm"), 186},   	/* masculine ordinal indicator, U+00BA ISOnum */
{TEXT("oslash"), 248},	/* latin small letter o with stroke, = */
                        /* latin small letter o slash, U+00F8 ISOlat1 */
{TEXT("otilde"), 245},	/* latin small letter o with tilde, U+00F5 ISOlat1 */
{TEXT("otimes"), 8855},	/* circled times = vector product, U+2297 ISOamsb */
{TEXT("ouml"), 246},   	/* latin small letter o with diaeresis, U+00F6 ISOlat1 */
{TEXT("para"), 182},   	/* pilcrow sign = paragraph sign, U+00B6 ISOnum */
{TEXT("part"), 8706},  	/* partial differential, U+2202 ISOtech */
{TEXT("permil"), 8240},	/* per mille sign, U+2030 ISOtech */
{TEXT("perp"), 8869},  	/* up tack = orthogonal to = perpendicular, U+22A5 ISOtech */
{TEXT("phi"), 966},    	/* greek small letter phi, U+03C6 ISOgrk3 */
{TEXT("pi"), 960},     	/* greek small letter pi, U+03C0 ISOgrk3 */
{TEXT("piv"), 982},    	/* greek pi symbol, U+03D6 ISOgrk3 */
{TEXT("plusmn"), 177},	/* plus-minus sign = plus-or-minus sign, U+00B1 ISOnum */
{TEXT("pound"), 163},  	/* pound sign, U+00A3 ISOnum */
{TEXT("prime"), 8242},	/* prime = minutes = feet, U+2032 ISOtech */
{TEXT("prod"), 8719},  	/* n-ary product = product sign, U+220F ISOamsb */
{TEXT("prop"), 8733},  	/* proportional to, U+221D ISOtech */
{TEXT("psi"), 968},    	/* greek small letter psi, U+03C8 ISOgrk3 */
{TEXT("quot"), 34},    	/* quotation mark = APL quote, U+0022 ISOnum */
{TEXT("rArr"), 8658},  	/* rightwards double arrow, U+21D2 ISOtech */
{TEXT("radic"), 8730},	/* square root = radical sign, U+221A ISOtech */
{TEXT("rang"), 9002},  	/* right-pointing angle bracket = ket, U+232A ISOtech */
{TEXT("raquo"), 187},  	/* right-pointing double angle quotation mark = */
                        /* right pointing guillemet, U+00BB ISOnum */
{TEXT("rarr"), 8594},  	/* rightwards arrow, U+2192 ISOnum */
{TEXT("rceil"), 8969},	/* right ceiling, U+2309 ISOamsc */
{TEXT("rdquo"), 8221},	/* right double quotation mark, U+201D ISOnum */
{TEXT("real"), 8476},  	/* blackletter capital R = real part symbol, U+211C ISOamso */
{TEXT("reg"), 174},    	/* registered sign = registered trade mark sign, */
                        /* U+00AE ISOnum */
{TEXT("rfloor"), 8971},	/* right floor, U+230B ISOamsc */
{TEXT("rho"), 961},     /* greek small letter rho, U+03C1 ISOgrk3 */
{TEXT("rlm"), 8207},   	/* right-to-left mark, U+200F NEW RFC 2070 */
{TEXT("rsaquo"), 8250},	/* single right-pointing angle quotation mark, */
                        /* U+203A ISO proposed */
{TEXT("rsquo"), 8217},	/* right single quotation mark, U+2019 ISOnum */
{TEXT("sbquo"), 8218},	/* single low-9 quotation mark, U+201A NEW */
{TEXT("scaron"), 353},	/* latin small letter s with caron, U+0161 ISOlat2 */
{TEXT("sdot"), 8901},  	/* dot operator, U+22C5 ISOamsb */
{TEXT("sect"), 167},   	/* section sign, U+00A7 ISOnum */
{TEXT("shy"), 173},    	/* soft hyphen = discretionary hyphen, U+00AD ISOnum */
{TEXT("sigma"), 963},  	/* greek small letter sigma, U+03C3 ISOgrk3 */
{TEXT("sigmaf"), 962},	/* greek small letter final sigma, U+03C2 ISOgrk3 */
{TEXT("sim"), 8764},   	/* tilde operator = varies with = similar to, U+223C ISOtech */
{TEXT("spades"), 9824},	/* black spade suit, U+2660 ISOpub */
{TEXT("sub"), 8834},   	/* subset of, U+2282 ISOtech */
{TEXT("sube"), 8838},  	/* subset of or equal to, U+2286 ISOtech */
{TEXT("sum"), 8721},   	/* n-ary sumation, U+2211 ISOamsb */
{TEXT("sup"), 8835},   	/* superset of, U+2283 ISOtech */
{TEXT("sup1"), 185},   	/* superscript one = superscript digit one, U+00B9 ISOnum */
{TEXT("sup2"), 178},   	/* superscript two = superscript digit two = squared, */
                        /* U+00B2 ISOnum */
{TEXT("sup3"), 179},   	/* superscript three = superscript digit three = cubed, */
                        /* U+00B3 ISOnum */
{TEXT("supe"), 8839},  	/* superset of or equal to, U+2287 ISOtech */
{TEXT("szlig"), 223},  	/* latin small letter sharp s = ess-zed, U+00DF ISOlat1 */
{TEXT("tau"), 964},    	/* greek small letter tau, U+03C4 ISOgrk3 */
{TEXT("there4"), 8756},	/* therefore, U+2234 ISOtech */
{TEXT("theta"), 952},  	/* greek small letter theta, U+03B8 ISOgrk3 */
{TEXT("thetasym"), 977},/* greek small letter theta symbol, U+03D1 NEW */
{TEXT("thinsp"), 8201},	/* thin space, U+2009 ISOpub */
{TEXT("thorn"), 254},  	/* latin small letter thorn with, U+00FE ISOlat1 */
{TEXT("tilde"), 732},  	/* small tilde, U+02DC ISOdia */
{TEXT("times"), 215},  	/* multiplication sign, U+00D7 ISOnum */
{TEXT("trade"), 8482},	/* trade mark sign, U+2122 ISOnum */
{TEXT("uArr"), 8657},  	/* upwards double arrow, U+21D1 ISOamsa */
{TEXT("uacute"), 250},	/* latin small letter u with acute, U+00FA ISOlat1 */
{TEXT("uarr"), 8593},  	/* upwards arrow, U+2191 ISOnum*/
{TEXT("ucirc"), 251},  	/* latin small letter u with circumflex, U+00FB ISOlat1 */
{TEXT("ugrave"), 249},	/* latin small letter u with grave, U+00F9 ISOlat1 */
{TEXT("uml"), 168},    	/* diaeresis = spacing diaeresis, U+00A8 ISOdia */
{TEXT("upsih"), 978},  	/* greek upsilon with hook symbol, U+03D2 NEW */
{TEXT("upsilon"), 965},	/* greek small letter upsilon, U+03C5 ISOgrk3 */
{TEXT("uuml"), 252},   	/* latin small letter u with diaeresis, U+00FC ISOlat1 */
{TEXT("weierp"), 8472}, /* script capital P = power set = Weierstrass p, */
                        /* U+2118 ISOamso */
{TEXT("xi"), 958},     	/* greek small letter xi, U+03BE ISOgrk3 */
{TEXT("yacute"), 253},	/* latin small letter y with acute, U+00FD ISOlat1 */
{TEXT("yen"), 165},    	/* yen sign = yuan sign, U+00A5 ISOnum */
{TEXT("yuml"), 255},   	/* latin small letter y with diaeresis, U+00FF ISOlat1 */
{TEXT("zeta"), 950},   	/* greek small letter zeta, U+03B6 ISOgrk3 */
{TEXT("zwj"), 8205},   	/* zero width joiner, U+200D NEW RFC 2070 */
{TEXT("zwnj"), 8204},  	/* zero width non-joiner, U+200C NEW RFC 2070 */
{TEXT("zzzz"), 0}			/* this last entry is required */
};

/* tables defined in fetchHTMLname.c */
extern AttributeMapping XhtmlAttributeMappingTable[];

/* Mapping table of HTML attribute values */
AttrValueMapping XhtmlAttrValueMappingTable[] =
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

   /* XML attribute xml:space */
   {HTML_ATTR_xml_space, TEXT("default"), HTML_ATTR_xml_space_VAL_xml_space_default},
   {HTML_ATTR_xml_space, TEXT("preserve"), HTML_ATTR_xml_space_VAL_xml_space_preserve},

   {0, TEXT(""), 0}			/* Last entry. Mandatory */
};


/*----------------------------------------------------------------------
  ParseCharset:
  Parses the element HTTP-EQUIV and looks for the charset value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void     ParseCharset (Element el, Document doc) 
#else  /* !__STDC__ */
void     ParseCharset (el, doc) 
Element  el;
Document doc;
#endif /* !__STDC__ */
{
   AttributeType attrType;
   Attribute     attr;
   SSchema       docSSchema;
   CHARSET       charset;
   CHAR_T       *text, *text2, *ptrText, *str;
   CHAR_T        charsetname[MAX_LENGTH];
   int           length;
   int           pos, index = 0;

   charset = TtaGetDocumentCharset (doc);
   if (charset != UNDEFINED_CHARSET)
     /* the charset was already defined by the http header */
     return;

   docSSchema = TtaGetDocumentSSchema (doc);
   attrType.AttrSSchema = docSSchema;
   attrType.AttrTypeNum = HTML_ATTR_http_equiv;
   attr = TtaGetAttribute (el, attrType);
   if (attr != NULL)
     {
       /* There is a HTTP-EQUIV attribute */
       length = TtaGetTextAttributeLength (attr);
       if (length > 0)
	 {
	   text = TtaAllocString (length + 1);
	   TtaGiveTextAttributeValue (attr, text, &length);
	   if (!ustrcasecmp (text, TEXT("content-type")))
	     {
	       attrType.AttrTypeNum = HTML_ATTR_meta_content;
	       attr = TtaGetAttribute (el, attrType);
	       if (attr != NULL)
		 {
		   length = TtaGetTextAttributeLength (attr);
		   if (length > 0)
		     {
		       text2 = TtaAllocString (length + 1);
		       TtaGiveTextAttributeValue (attr, text2, &length);
		       ptrText = text2;
		       while (*ptrText)
			 {
			   *ptrText = utolower (*ptrText);
			   ptrText++;
			 }
		       
		       str = ustrstr (text2, TEXT("charset="));
		       if (str)
			 {
			   pos = str - text2 + 8;
			   while (text2[pos] != WC_SPACE &&
				  text2[pos] != WC_TAB && text2[pos] != WC_EOS)
			     charsetname[index++] = text2[pos++];
			   charsetname[index] = WC_EOS;
			   charset = TtaGetCharset (charsetname);
			   if (charset != UNDEFINED_CHARSET)
			     TtaSetDocumentCharset (doc, charset);
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
  XhtmlElementComplete
  Complete Xhtml elements.
  Check its attributes and its contents.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void       XhtmlElementComplete (Element el, Document doc, int *error)
#else
void       XhtmlElementComplete (el, doc, error)
Element    el;
Document   doc;
int       *error;
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
   int                 length;
   SSchema             docSSchema;

   *error = 0;
   docSSchema = TtaGetDocumentSSchema (doc);

   elType = TtaGetElementType (el);
   /* is this a block-level element in a character-level element? */
   if (!IsXMLElementInline (el) &&
       elType.ElTypeNum != HTML_EL_Comment_)
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
	   child = TtaNewTree (doc, elType, "");
	   if (desc == NULL)
	       TtaInsertFirstChild (&child, el, doc);
	   else
	       TtaInsertSibling (child, desc, TRUE, doc);
	 }

       /* copy attribute data into SRC attribute of Object_Image */
       attrType.AttrSSchema = docSSchema;
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
		   TtaAttachAttribute (child, attr, doc);
		 }
	       TtaSetAttributeText (attr, name1, child, doc);
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
	   desc = TtaNewTree (doc, elType, "");
	   TtaInsertSibling (desc, child, FALSE, doc);
	   /* move previous existing children into Object_Content */
	   child = TtaGetLastChild(el);
	   while (child != desc)
	     {
	       TtaRemoveTree (child, doc);
	       TtaInsertFirstChild (&child, desc, doc);
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
		     TtaRemoveTree (child, doc);
		     if (last)
		       TtaInsertSibling (child, last, FALSE, doc);
		     else
		       TtaInsertFirstChild (&child, prev, doc);
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
		   newElType.ElSSchema = docSSchema;
		   newElType.ElTypeNum = HTML_EL_Frames;
		   elFrames = TtaNewElement (doc, newElType);
		   XmlSetElemLineNumber (elFrames);
		   TtaInsertSibling (elFrames, child, TRUE, doc);
		 }
	       /* move the element as the last child of the Frames element */
	       TtaRemoveTree (child, doc);
	       if (lastFrame == NULL)
		 TtaInsertFirstChild (&child, elFrames, doc);
	       else
		 TtaInsertSibling (child, lastFrame, FALSE, doc);
	       lastFrame = child;
	     }
	   child = next;
	 }
       break;
       
     case HTML_EL_Input:	/* it's an INPUT without any TYPE attribute */
       /* Create a child of type Text_Input */
       elType.ElTypeNum = HTML_EL_Text_Input;
       child = TtaNewTree (doc, elType, "");
       XmlSetElemLineNumber (child);
       TtaInsertFirstChild (&child, el, doc);
       /* now, process it like a Text_Input element */

     case HTML_EL_Text_Input:
     case HTML_EL_Password_Input:
     case HTML_EL_File_Input:
       /* get element Inserted_Text */
       child = TtaGetFirstChild (el);
       if (child != NULL)
	 {
	   attrType.AttrSSchema = docSSchema;
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
			   TtaSetTextContent (leaf, text, 
					      TtaGetDefaultLanguage (), doc);
			   TtaFreeMemory (text);
			 }
		     }
		 }
	     }
	 }
       break;
       
     case HTML_EL_META:
       ParseCharset (el, doc);
       break;

     case HTML_EL_STYLE_:	/* it's a STYLE element */
     case HTML_EL_SCRIPT:	/* it's a SCRIPT element */
     case HTML_EL_Preformatted:	/* it's a PRE */
       /* if the last line of the Preformatted is empty, remove it */
       leaf = XmlLastLeafInElement (el);
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
			 TtaDeleteTree (leaf, doc);
		       else
			 /* remove the last character */
			 TtaDeleteTextContent (leaf, length, 1, doc);
		     }
		 }
	     }
	 }
       if (IsParsingCSS ())
	 {
	   text = GetStyleContents (el);
	   if (text)
	     {
	       ReadCSSRules (doc, NULL, text, FALSE);
	       TtaFreeMemory (text);
	     }
	   SetParsingCSS (FALSE);
	 }
       /* and continue as if it were a Preformatted or a Script */
       break;
       
     case HTML_EL_Text_Area:	/* it's a Text_Area */
       SetParsingTextArea (FALSE);
       child = TtaGetFirstChild (el);
       if (child == NULL)
	 /* it's an empty Text_Area */
	 /* insert a Inserted_Text element in the element */
	 {
	   newElType.ElTypeNum = HTML_EL_Inserted_Text;
	   child = TtaNewTree (doc, newElType, "");
	   TtaInsertFirstChild (&child, el, doc);
	 }
       else
	 {
	   /* save the text into Default_Value attribute */
	   attrType.AttrSSchema = docSSchema;
	   attrType.AttrTypeNum = HTML_ATTR_Default_Value;
	   if (TtaGetAttribute (el, attrType) == NULL)
	     /* attribute Default_Value is missing */
	     {
	       attr = TtaNewAttribute (attrType);
	       TtaAttachAttribute (el, attr, doc);
	       desc = TtaGetFirstChild (child);
	       length = TtaGetTextLength (desc) + 1;
	       text = TtaAllocString (length);
	       TtaGiveTextContent (desc, text, &length, &lang);
	       TtaSetAttributeText (attr, text, el, doc);
	       TtaFreeMemory (text);
	     }
	 }
       /* insert a Frame element */
       newElType.ElTypeNum = HTML_EL_Frame;
       constElem = TtaNewTree (doc, newElType, "");
       TtaInsertSibling (constElem, child, FALSE, doc);
       break;
       
     case HTML_EL_Radio_Input:
     case HTML_EL_Checkbox_Input:
       /* put an attribute Checked if it is missing */
       attrType.AttrSSchema = docSSchema;
       attrType.AttrTypeNum = HTML_ATTR_Checked;
       if (TtaGetAttribute (el, attrType) == NULL)
	 /* attribute Checked is missing */
	 {
	   attr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (el, attr, doc);
	   TtaSetAttributeValue (attr, HTML_ATTR_Checked_VAL_No_, el, doc);
	 }
       break;
       
     case HTML_EL_Option_Menu:
       /* Check that at least one option has a SELECTED attribute */
       OnlyOneOptionSelected (el, doc, TRUE);
       break;

     case HTML_EL_PICTURE_UNIT:
       break;
       
     case HTML_EL_LINK:
       CheckCSSLink (el, doc, docSSchema);
       break;
       
     case HTML_EL_Data_cell:
     case HTML_EL_Heading_cell:
       /* insert a pseudo paragraph into empty cells */
       child = TtaGetFirstChild (el);
       if (child == NULL)
	 {
	   elType.ElTypeNum = HTML_EL_Pseudo_paragraph;
	   child = TtaNewTree (doc, elType, "");
	   if (child != NULL)
	       TtaInsertFirstChild (&child, el, doc);
	 }
       
       /* detect whether we're parsing a whole table or just a cell */
       if (IsWithinTable ())
	   NewCell (el, doc, FALSE);
       break;
       
     case HTML_EL_Table:
       CheckTable (el, doc);
       SubWithinTable ();
       break;
       
     case HTML_EL_TITLE:
       /* show the TITLE in the main window */
       UpdateTitle (el, doc);
       break;
       
     default:
       break;
     }
}

/*----------------------------------------------------------------------
   MapHTMLAttributeValue
   Search in the Attribute Value Mapping Table the entry for the attribute
   ThotAtt and its value AttrVal. Returns the corresponding Thot value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void            MapHTMLAttributeValue (CHAR_T* AttrVal,
				       AttributeType attrType,
				       int* value)
#else
void            MapHTMLAttributeValue (AttrVal,
				       attrType,
				       value)
CHAR_T*         AttrVal;
AttributeType   attrType;
int*            value;
#endif
{
   int       i;

   *value = 0;
   i = 0;

   while (XhtmlAttrValueMappingTable[i].ThotAttr != attrType.AttrTypeNum &&
	  XhtmlAttrValueMappingTable[i].ThotAttr != 0)
       i++;

   if (XhtmlAttrValueMappingTable[i].ThotAttr == attrType.AttrTypeNum)
     {
       do
           if (!ustrcmp (XhtmlAttrValueMappingTable[i].XMLattrValue, AttrVal))
	       *value = XhtmlAttrValueMappingTable[i].ThotAttrValue;
	   else 
	       i++;
       while (*value <= 0 &&
	      XhtmlAttrValueMappingTable[i].ThotAttr != 0);
     }
}

/*---------------------------------------------------------------------------
   XhtmlMapEntity
   Search that entity in the entity table and return the corresponding value.
  ---------------------------------------------------------------------------*/
#ifdef __STDC__
void	XhtmlMapEntity (STRING entityName, int *entityValue, STRING alphabet)
#else
void	XhtmlMapEntity (entityName, entityValue, alphabet)
STRING  entityName;
int    *entityValue;
STRING  alphabet;
#endif
{
  int            i;
  ThotBool       found;

  found = FALSE;
  for (i = 0; XhtmlEntityTable[i].charCode >= 0 && ! found; i++)
     found = !ustrcmp (XhtmlEntityTable[i].charName, entityName);

  if (found)
    {
      /* entity found */
      i--;
      *entityValue = XhtmlEntityTable[i].charCode;
      *alphabet = 'L';
    }
  else
    *alphabet = EOS;
}

/*----------------------------------------------------------------------
  PutNonISOlatin1Char     
  Put a Unicode character in the input buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void    PutNonISOlatin1Char (int code, STRING prefix, STRING entityName, ParserData *context)
#else
static void    PutNonISOlatin1Char (code, prefix, entityName, context)
int            code;
STRING         prefix;
STRING         entityName;
ParserData    *context;
#endif
{
   Language	 lang, l;
   ElementType	 elType;
   Element	 elText;
   AttributeType attrType;
   Attribute	 attr;
   CHAR_T	 buffer[MaxEntityLength+10];

   if (context->readingAnAttrValue)
     /* this entity belongs to an attribute value */
     {
       /* Thot can't mix different languages in the same attribute value */
       /* just discard that character */
       ;
     }
   else
     /* this entity belongs to the element contents */
     {
       /* create a new text leaf */
       elType.ElSSchema = TtaGetDocumentSSchema (context->doc);
       elType.ElTypeNum = HTML_EL_TEXT_UNIT;
       elText = TtaNewElement (context->doc, elType);
       XmlSetElemLineNumber (elText);
       XhtmlInsertElement (&elText);
       context->lastElement = elText;
       context->lastElementClosed = FALSE;
       context->lastElementClosed = TRUE;

       /* try to find a fallback character */
       l = context->language;
       GetFallbackCharacter (code, buffer, &lang);

       /* put that fallback character in the new text leaf */
       TtaSetTextContent (elText, buffer, lang, context->doc);
       context->language = l;

       /* make that text leaf read-only */
       TtaSetAccessRight (elText, ReadOnly, context->doc);

       /* associate an attribute EntityName with the new text leaf */
       attrType.AttrSSchema = TtaGetDocumentSSchema (context->doc);
       attrType.AttrTypeNum = HTML_ATTR_EntityName;
       attr = TtaNewAttribute (attrType);
       TtaAttachAttribute (elText, attr, context->doc);
       ustrcpy (buffer, prefix);
       ustrcat (buffer, entityName);
       TtaSetAttributeText (attr, buffer, elText, context->doc);
       context->mergeText = FALSE;
     }
}

/*----------------------------------------------------------------------
   XhtmlEntityCreated
   A XTHML entity has been created by the XML parser.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void        XhtmlEntityCreated (int entityVal, Language lang, STRING entityName, ParserData *context)
#else
void        XhtmlEntityCreated (entityVal, lang, entityName, context)
int         entityVal;
Language    lang;
STRING      entityName;
ParserData *context;
#endif
{ 
  CHAR_T	 buffer[2];

  if (lang < 0)
    PutInXmlElement (entityName);
  else
    {
#ifdef LC
      printf (" \n code=%d", entityVal);
#endif /* LC */
      if (entityVal < 255)
	{
	  buffer[0] = TEXT(entityVal);
	  buffer[1] = WC_EOS;
	  PutInXmlElement (buffer);
	}
      else
	PutNonISOlatin1Char (entityVal, TEXT(""), entityName, context);
    }
}
