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
/* Without this option, it creates a function StartHTMLParser that parses */
/* a HTML file and creates the internal representation of a Thot document. */

/* Amaya includes  */

#ifdef STANDALONE
/* Includes for STANDALONE version */
#include <stdio.h>
#include "HTML.h"
#define THOT_EXPORT
#include "amaya.h"

#else /* !STANDALONE */
#define HANDLE_COMPRESSED_FILES
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"

#ifdef HANDLE_COMPRESSED_FILES
#include "zlib.h"
#endif

#include "language.h"
#include "css_f.h"
#include "html2thot_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLimage_f.h"
#include "HTMLstyle_f.h"
#include "HTMLtable_f.h"
#ifdef MATHML
#include "Mathedit_f.h"
#endif /* MATHML */
#endif /* STANDALONE */

typedef unsigned char entityName[10];
typedef struct _ISOlat1entry
  {			 /* a SGML entity representing an ISO-Latin1 char */
     entityName          charName;	/* entity name */
     int                 charCode;	/* decimal code of ISO-Latin1 char */
  }
ISOlat1entry;


ISOlat1entry        ISOlat1table[] =
{
   /* This table MUST be in alphabetical order */
   {"AElig", 198},
   {"Aacute", 193},
   {"Acirc", 194},
   {"Agrave", 192},
   {"Aring", 197},
   {"Atilde", 195},
   {"Auml", 196},
   {"Ccedil", 199},
   {"ETH", 208},
   {"Eacute", 201},
   {"Ecirc", 202},
   {"Egrave", 200},
   {"Euml", 203},
   {"Iacute", 205},
   {"Icirc", 206},
   {"Igrave", 204},
   {"Iuml", 207},
   {"Ntilde", 209},
   {"Oacute", 211},
   {"Ocirc", 212},
   {"Ograve", 210},
   {"Oslash", 216},
   {"Otilde", 213},
   {"Ouml", 214},
   {"THORN", 222},
   {"Uacute", 218},
   {"Ucirc", 219},
   {"Ugrave", 217},
   {"Uuml", 220},
   {"Yacute", 221},
   {"aacute", 225},
   {"acirc", 226},
   {"acute", 180},
   {"aelig", 230},
   {"agrave", 224},
   {"amp", 38},
   {"aring", 229},
   {"atilde", 227},
   {"auml", 228},
   {"brvbar", 166},
   {"ccedil", 231},
   {"cedil", 184},
   {"cent", 162},
   {"copy", 169},
   {"curren", 164},
   {"deg", 176},
   {"divide", 247},
   {"eacute", 233},
   {"ecirc", 234},
   {"egrave", 232},
   {"eth", 240},
   {"euml", 235},
   {"frac12", 189},
   {"frac14", 188},
   {"frac34", 190},
   {"gt", 62},			/* Numeric and Special Graphic Entity */
   {"hyphen", 173},
   {"iacute", 237},
   {"icirc", 238},
   {"iexcl", 161},
   {"igrave", 236},
   {"iquest", 191},
   {"iuml", 239},
   {"laquo", 171},		/* added by VQ */
   {"lt", 60},			/* Numeric and Special Graphic Entity */
   {"macr", 175},
   {"micro", 181},
   {"middot", 183},
   {"nbsp", 160},		/* Non breaking space */
   {"not", 172},
   {"ntilde", 241},
   {"oacute", 243},
   {"ocirc", 244},
   {"ograve", 242},
   {"ordf", 170},
   {"ordm", 186},
   {"oslash", 248},
   {"otilde", 245},
   {"ouml", 246},
   {"para", 182},
   {"plusmn", 177},
   {"pound", 163},
   {"quot", 34},		/* Numeric and Special Graphic Entity */
   {"raquo", 187},		/* added by VQ */
   {"reg", 174},
   {"sect", 167},
   {"shy", 45},			/* Soft hyphen */
   {"sup1", 185},
   {"sup2", 178},
   {"sup3", 179},
   {"szlig", 223},
   {"thorn", 254},
   {"times", 215},
   {"uacute", 250},
   {"ucirc", 251},
   {"ugrave", 249},
   {"uml", 168},
   {"uuml", 252},
   {"yacute", 253},
   {"yen", 165},
   {"yuml", 255},
   {"zzzz", 0}			/* this last entry is required */
};

typedef struct _ElemToBeChecked *PtrElemToBeChecked;

typedef struct _ElemToBeChecked
  {
     Element               Elem;	 /* the element to be checked */
     PtrElemToBeChecked    nextElemToBeChecked;
  }
ElemToBeChecked;

typedef struct _ClosedElement *PtrClosedElement;

typedef struct _ClosedElement
  {				/* an element closed by a start tag */
     int                 tagNum;	 /* rank (in GIMappingTable) of closed
					    element */
     PtrClosedElement    nextClosedElem; /* next element closed by the same
					    start tag */
  }
ClosedElement;

#define MaxGIlength 14
typedef unsigned char GI[MaxGIlength];

typedef struct _GIMapping
  {				/* mapping of a HTML element */
     GI                  htmlGI;	/* name of the HTML element */
     char                htmlContents;	/* info about the contents of the HTML element:
					   'E'=empty,  space=some contents */
     int                 ThotType;	/* type of the Thot element or attribute */
     PtrClosedElement    firstClosedElem;	/* first element closed by the start
						   tag htmlGI */
  }
GIMapping;

typedef struct _AttributeMapping
  {				/* mapping of a HTML attribute */
     char                htmlAttribute[12];	/* name of HTML attribute */
     GI                  htmlElement;	/* name of HTML GI */
     char                AttrOrContent;		/* info about the corresponding Thot
						   thing: 'A'=Attribute, 'C'=Content
						   SPACE= Nothing */
     int                 ThotAttribute;		/* Thot attribute */
  }
AttributeMapping;

typedef struct _AttrValueMapping
  {				/* mapping of a HTML attribute value */
     int                 ThotAttr;	/* corresponding Thot attribute */
     char                htmlAttrValue[20];	/* HTML value */
     int                 ThotAttrValue;		/* corresponding value of the Thot
						   attribute */
  }
AttrValueMapping;

#ifdef MATHML
#include "MathML.h"

typedef unsigned char MathEntityName[20];
typedef struct _MathEntity
  {			 /* a Math entity representing an operator char */
     MathEntityName      MentityName;	/* entity name */
     int                 charCode;	/* decimal code of char */
     char		 alphabet;	/* 'L' = ISO-Latin-1, 'G' = Symbol */
  }
MathEntity;

MathEntity        MathEntityTable[] =
{
   /* This table MUST be in alphabetical order */
   /* This table contains characters from the Symbol font plus some
      specific MathML entities */
   {"Agr", 65, 'G'},
   {"ApplyFunction", 32, 'L'},  /* render as white space */
   {"Bgr", 66, 'G'},
   {"Cap", 199, 'G'},
   {"Cup", 200, 'G'},
   {"Delta", 68, 'G'},
   {"EEgr", 72, 'G'},
   {"Egr", 69, 'G'},
   {"Gamma", 71, 'G'},
   {"Igr", 73, 'G'},
   {"Integral", 242, 'G'},
   {"InvisibleTimes", 0, ' '},
   {"Kgr", 75, 'G'},
   {"KHgr", 67, 'G'},
   {"Lambda", 76, 'G'},
   {"LeftArrow", 172, 'G'},
   {"Mgr", 77, 'G'},
   {"Ngr", 78, 'G'},
   {"Ogr", 79, 'G'},
   {"Omega", 87, 'G'},
   {"PI", 213, 'G'},
   {"Phi", 70, 'G'},
   {"Pi", 80, 'G'},
   {"PlusMinus", 177, 'G'},
   {"Psi", 89, 'G'},
   {"Rgr", 82, 'G'},
   {"RightArrow", 174, 'G'},
   {"Sigma", 83, 'G'},
   {"Sol", 164, 'G'},
   {"Sum", 229, 'G'},
   {"Tgr", 84, 'G'},
   {"Theta", 81, 'G'},
   {"Upsi", 85, 'G'},
   {"Upsi1", 161, 'G'},
   {"Verbar", 189, 'G'},
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
   {"ifraktur", 225, 'G'},
   {"infin", 165, 'G'},
   {"int", 242, 'G'},
   {"iota", 105, 'G'},
   {"isin", 206, 'G'},
   {"it", 242, 'G'},
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
   {"lt", 62, 'L'},
   {"middot", 215, 'G'},
   {"mldr", 188, 'G'},
   {"mu", 109, 'G'},
   {"ne", 185, 'G'},
   {"not", 216, 'G'},
   {"notin", 207, 'G'},
   {"nu", 107, 'G'},
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
   {"weierp", 195, 'G'},
   {"xi", 120, 'G'},
   {"zeta", 122, 'G'},
   {"zzzz", -1, ' '}		/* this last entry is required */
};

/* mapping table of MathML elements */

static GIMapping    MathMLGIMappingTable[] =
{
   /* This table MUST be in alphabetical order */
   {"MERROR", SPACE, MathML_EL_MERROR, NULL},
   {"MF", SPACE, MathML_EL_MF, NULL},  /* for compatibility with an old
				          version of MathML: WD-math-970704 */
   {"MFENCED", SPACE, MathML_EL_MFENCED, NULL},
   {"MFRAC", SPACE, MathML_EL_MFRAC, NULL},
   {"MI", SPACE, MathML_EL_MI, NULL},
   {"MMULTISCRIPTS", SPACE, MathML_EL_MMULTISCRIPTS, NULL},
   {"MN", SPACE, MathML_EL_MN, NULL},
   {"MO", SPACE, MathML_EL_MO, NULL},
   {"MOVER", SPACE, MathML_EL_MOVER, NULL},
   {"MPADDED", SPACE, MathML_EL_MPADDED, NULL},
   {"MPHANTOM", SPACE, MathML_EL_MPHANTOM, NULL},
   {"MPRESCRIPTS", SPACE, MathML_EL_PrescriptPairs, NULL},
   {"MROOT", SPACE, MathML_EL_MROOT, NULL},
   {"MROW", SPACE, MathML_EL_MROW, NULL},
   {"MS", SPACE, MathML_EL_MS, NULL},
   {"MSPACE", 'E', MathML_EL_MSPACE, NULL},
   {"MSQRT", SPACE, MathML_EL_MSQRT, NULL},
   {"MSTYLE", SPACE, MathML_EL_MSTYLE, NULL},
   {"MSUB", SPACE, MathML_EL_MSUB, NULL},
   {"MSUBSUP", SPACE, MathML_EL_MSUBSUP, NULL},
   {"MSUP", SPACE, MathML_EL_MSUP, NULL},
   {"MTEXT", SPACE, MathML_EL_MTEXT, NULL},
   {"MUNDER", SPACE, MathML_EL_MUNDER, NULL},
   {"MUNDEROVER", SPACE, MathML_EL_MUNDEROVER, NULL},
   {"NONE", SPACE, MathML_EL_Construct, NULL},
   {"SEP", 'E', MathML_EL_SEP, NULL},
   {"", SPACE, 0, NULL}	/* Last entry. Mandatory */
};

static AttributeMapping MathMLAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   /* The rest of this table MUST be in alphabetical order */
   {"unknown_attr", "", 'A', MathML_ATTR_Invalid_attribute},
   {"CLOSE", "MFENCED", 'A', MathML_ATTR_close},
   {"FENCE", "MO", 'A', MathML_ATTR_fence},
   {"FONTSLANT", "MI", 'A', MathML_ATTR_fontslant},
   {"OPEN", "MFENCED", 'A', MathML_ATTR_open},
   {"SEPARATORS", "MFENCED", 'A', MathML_ATTR_separators},

   {"ZZGHOST", "", 'A', MathML_ATTR_Ghost_restruct},
   {"", "", EOS, 0}		/* Last entry. Mandatory */
};

/* mapping table of HTML attribute values */

static AttrValueMapping MathMLAttrValueMappingTable[] =
{
   {MathML_ATTR_fence, "TRUE", MathML_ATTR_fence_VAL_true},
   {MathML_ATTR_fence, "FALSE", MathML_ATTR_fence_VAL_false},
   {MathML_ATTR_fontslant, "ITALIC", MathML_ATTR_fontslant_VAL_italic},
   {MathML_ATTR_fontslant, "PLAIN", MathML_ATTR_fontslant_VAL_plain},

   {0, "", 0}			/* Last entry. Mandatory */
};

static boolean WithinMathML;
static boolean ReadingHTMLentity;
static SSchema MathMLSSchema;

#endif	/* MATHML */

/* mapping table of HTML elements */

static GIMapping    HTMLGIMappingTable[] =
{
   /* This table MUST be in alphabetical order */
   {"A", SPACE, HTML_EL_Anchor, NULL},
   {"ADDRESS", SPACE, HTML_EL_Address, NULL},
   {"APPLET", SPACE, HTML_EL_Applet, NULL},
   {"AREA", 'E', HTML_EL_AREA, NULL},
   {"B", SPACE, HTML_EL_Bold_text, NULL},
   {"BASE", 'E', HTML_EL_BASE, NULL},
   {"BASEFONT", 'E', HTML_EL_BaseFont, NULL},
   {"BIG", SPACE, HTML_EL_Big_text, NULL},
   {"BLOCKQUOTE", SPACE, HTML_EL_Block_Quote, NULL},
   {"BODY", SPACE, HTML_EL_BODY, NULL},
   {"BR", 'E', HTML_EL_BR, NULL},
   {"C", SPACE, HTML_EL_TEXT_UNIT, NULL},
   {"CAPTION", SPACE, HTML_EL_CAPTION, NULL},
   {"CENTER", SPACE, HTML_EL_Center, NULL},
   {"CITE", SPACE, HTML_EL_Cite, NULL},
   {"CODE", SPACE, HTML_EL_Code, NULL},
   {"DD", SPACE, HTML_EL_Definition, NULL},
   {"DFN", SPACE, HTML_EL_Def, NULL},
   {"DIR", SPACE, HTML_EL_Directory, NULL},
   {"DIV", SPACE, HTML_EL_Division, NULL},
   {"DL", SPACE, HTML_EL_Definition_List, NULL},
   {"DT", SPACE, HTML_EL_Term, NULL},
   {"EM", SPACE, HTML_EL_Emphasis, NULL},
   {"FONT", SPACE, HTML_EL_Font_, NULL},
   {"FORM", SPACE, HTML_EL_Form, NULL},
   {"H1", SPACE, HTML_EL_H1, NULL},
   {"H2", SPACE, HTML_EL_H2, NULL},
   {"H3", SPACE, HTML_EL_H3, NULL},
   {"H4", SPACE, HTML_EL_H4, NULL},
   {"H5", SPACE, HTML_EL_H5, NULL},
   {"H6", SPACE, HTML_EL_H6, NULL},
   {"HEAD", SPACE, HTML_EL_HEAD, NULL},
   {"HR", 'E', HTML_EL_Horizontal_Rule, NULL},
   {"HTML", SPACE, HTML_EL_HTML, NULL},
   {"I", SPACE, HTML_EL_Italic_text, NULL},
   {"IMG", 'E', HTML_EL_PICTURE_UNIT, NULL},
   {"INPUT", 'E', HTML_EL_Input, NULL},
   {"ISINDEX", 'E', HTML_EL_ISINDEX, NULL},
   {"KBD", SPACE, HTML_EL_Keyboard, NULL},
   {"LI", SPACE, HTML_EL_List_Item, NULL},
   {"LINK", 'E', HTML_EL_LINK, NULL},
   {"LISTING", SPACE, HTML_EL_Preformatted, NULL},	/*converted to PRE */
   {"MAP", SPACE, HTML_EL_MAP, NULL},
#ifdef MATHML
   {"MATH", SPACE, HTML_EL_Math, NULL},
   {"MATHDISP", SPACE, HTML_EL_MathDisp, NULL},  /* for compatibility with an
				     old version of MathML: WD-math-970704 */
#endif
   {"MENU", SPACE, HTML_EL_Menu, NULL},
   {"META", 'E', HTML_EL_META, NULL},
#ifdef COUGAR
   {"OBJECT", SPACE, HTML_EL_Object, NULL},
#endif
   {"OL", SPACE, HTML_EL_Numbered_List, NULL},
   {"OPTION", SPACE, HTML_EL_Option, NULL},
   {"P", SPACE, HTML_EL_Paragraph, NULL},
   {"P*", SPACE, HTML_EL_Pseudo_paragraph, NULL},
   {"PARAM", 'E', HTML_EL_Parameter, NULL},
   {"PRE", SPACE, HTML_EL_Preformatted, NULL},
   {"SAMP", SPACE, HTML_EL_Sample, NULL},
   {"SCRIPT", SPACE, HTML_EL_SCRIPT, NULL},
   {"SELECT", SPACE, HTML_EL_Option_Menu, NULL},
   {"SMALL", SPACE, HTML_EL_Small_text, NULL},
#ifdef COUGAR
   {"SPAN", SPACE, HTML_EL_Span, NULL},
#endif
   {"STRIKE", SPACE, HTML_EL_Struck_text, NULL},
   {"STRONG", SPACE, HTML_EL_Strong, NULL},
   {"STYLE", SPACE, HTML_EL_Styles, NULL},
   {"SUB", SPACE, HTML_EL_Subscript, NULL},
   {"SUP", SPACE, HTML_EL_Superscript, NULL},
   {"TABLE", SPACE, HTML_EL_Table, NULL},
   {"TBODY", SPACE, HTML_EL_tbody, NULL},
   {"TD", SPACE, HTML_EL_Data_cell, NULL},
   {"TEXTAREA", SPACE, HTML_EL_Text_Area, NULL},
   {"TFOOT", SPACE, HTML_EL_tfoot, NULL},
   {"TH", SPACE, HTML_EL_Heading_cell, NULL},
   {"THEAD", SPACE, HTML_EL_thead, NULL},
   {"TITLE", SPACE, HTML_EL_TITLE, NULL},
   {"TR", SPACE, HTML_EL_Table_row, NULL},
   {"TT", SPACE, HTML_EL_Teletype_text, NULL},
   {"U", SPACE, HTML_EL_Underlined_text, NULL},
   {"UL", SPACE, HTML_EL_Unnumbered_List, NULL},
   {"VAR", SPACE, HTML_EL_Variable, NULL},
   {"XMP", SPACE, HTML_EL_Preformatted, NULL},	/* converted to PRE */
   {"", SPACE, 0, NULL}	/* Last entry. Mandatory */
};

/* elements that cannot contain text as immediate children.
   When some text is present in the HTML file it must be surrounded
   by a Thot Paragraph (or Pseudo_paragraph) element */
static int          NoTextChild[] =
{
   HTML_EL_HTML, HTML_EL_HEAD, HTML_EL_BODY,
   HTML_EL_Definition_List, HTML_EL_Block_Quote, HTML_EL_Directory,
   HTML_EL_Form, HTML_EL_Menu, HTML_EL_Numbered_List, HTML_EL_Option_Menu,
   HTML_EL_Unnumbered_List, HTML_EL_Definition, HTML_EL_List_Item,
   HTML_EL_MAP, HTML_EL_Applet,
#ifdef COUGAR
   HTML_EL_Object,
#endif
   HTML_EL_Division, HTML_EL_Center,
   HTML_EL_Table, HTML_EL_thead, HTML_EL_tbody, HTML_EL_tfoot, HTML_EL_Table_row,
   HTML_EL_Data_cell, HTML_EL_Heading_cell,
   0};

/* epmty elements */
static int          EmptyElement[] =
{
   HTML_EL_AREA,
   HTML_EL_BASE,
   HTML_EL_BaseFont,
   HTML_EL_BR,
   HTML_EL_Horizontal_Rule,
   HTML_EL_PICTURE_UNIT,
   HTML_EL_Input,
   HTML_EL_ISINDEX,
   HTML_EL_LINK,
   HTML_EL_META,
   HTML_EL_Parameter,
   0};

/* character level elements */
static int          CharLevelElement[] =
{
   HTML_EL_TEXT_UNIT, HTML_EL_PICTURE_UNIT,
   HTML_EL_Anchor,
   HTML_EL_Italic_text, HTML_EL_Bold_text, HTML_EL_Teletype_text,
   HTML_EL_Underlined_text, HTML_EL_Struck_text, HTML_EL_Big_text,
   HTML_EL_Small_text, HTML_EL_Subscript, HTML_EL_Superscript,
   HTML_EL_Font_,
   HTML_EL_Emphasis, HTML_EL_Strong, HTML_EL_Def, HTML_EL_Code, HTML_EL_Sample,
   HTML_EL_Keyboard, HTML_EL_Variable, HTML_EL_Cite,
#ifdef COUGAR
   HTML_EL_Span,
#endif
   HTML_EL_Input,
   HTML_EL_Option, HTML_EL_Option_Menu,
   HTML_EL_Toggle_Item, HTML_EL_Toggle_Menu,
   HTML_EL_Radio_Item, HTML_EL_Radio_Menu,
   HTML_EL_Text_Input_Line, HTML_EL_Text_Input,
   HTML_EL_Text_With_Frame, HTML_EL_Inserted_Text,
   HTML_EL_BR,
   0};

/* block level elements */
static int          BlockLevelElement[] =
{
   HTML_EL_Paragraph, HTML_EL_Pseudo_paragraph,
   HTML_EL_H1, HTML_EL_H2, HTML_EL_H3, HTML_EL_H4, HTML_EL_H5, HTML_EL_H6,
   HTML_EL_TITLE, HTML_EL_Term, HTML_EL_CAPTION,
   0};

/* start tags that imply the end of a current element */
/* any tag of each line implies the end of the current element if the type of
   that element is in the same line */
typedef char        oneLine[100];
static oneLine      EquivEndingElem[] =
{
   "DT DD LI OPTION",
   "H1 H2 H3 H4 H5 H6",
   "UL OL MENU DIR ADDRESS PRE LISTING XMP",
   ""};
/* acording the HTML DTD, HR should be added to the 2nd line above, as it */
/* is not allowed within a H1, H2, H3, etc. But we should tolerate that case */
/* because many documents contain rules in headings... */

/* start tags that imply the end of current element */
static oneLine      StartTagEndingElem[] =
{
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
   "ADDRESS closes P P* HEAD",
   "PRE closes P P* HEAD",
   "LISTING closes P P* HEAD",
   "XMP closes P P* HEAD",
   "BLOCKQUOTE closes P P* HEAD",
   "DL closes P P* DT MENU DIR ADDRESS PRE LISTING XMP HEAD",
   "DT closes P P* MENU DIR ADDRESS PRE LISTING XMP HEAD",
   "DD closes P P* MENU DIR ADDRESS PRE LISTING XMP HEAD",
   "UL closes P P* HEAD",
   "OL closes P P* HEAD",
   "MENU closes P P* HEAD",
   "P closes P P* HEAD H1 H2 H3 H4 H5 H6",
   "P* closes P P* HEAD",
   "DIV closes P P* HEAD",
   "CENTER closes FONT B I P P* HEAD",
   "A closes A",
   "CAPTION closes P P*",
   "TABLE closes P P* HEAD H1 H2 H3 H4 H5 H6 PRE LISTING XMP A",
   "TH closes TH TD",
   "TD closes TH TD",
   "TR closes TH TD TR CAPTION",
   "THEAD closes CAPTION",
   "TFOOT closes TH TD TR CAPTION THEAD TBODY",
   "TBODY closes TH TD TR CAPTION THEAD TFOOT TBODY",
   ""};

/* mapping table of HTML attributes */

#define DummyAttribute 500
static AttributeMapping HTMLAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   /* The rest of this table MUST be in alphabetical order */
   {"unknown_attr", "", 'A', HTML_ATTR_Invalid_attribute},
   {"ACTION", "", 'A', HTML_ATTR_Script_URL},
   {"ALIGN", "APPLET", 'A', HTML_ATTR_Alignment},
   {"ALIGN", "CAPTION", 'A', HTML_ATTR_Position},
   {"ALIGN", "DIV", 'A', HTML_ATTR_Align},
   {"ALIGN", "H1", 'A', HTML_ATTR_Align},
   {"ALIGN", "H2", 'A', HTML_ATTR_Align},
   {"ALIGN", "H3", 'A', HTML_ATTR_Align},
   {"ALIGN", "H4", 'A', HTML_ATTR_Align},
   {"ALIGN", "H5", 'A', HTML_ATTR_Align},
   {"ALIGN", "H6", 'A', HTML_ATTR_Align},
   {"ALIGN", "HR", 'A', HTML_ATTR_Align},
   {"ALIGN", "IMG", 'A', HTML_ATTR_Alignment},
#ifdef COUGAR
   {"ALIGN", "OBJECT", 'A', HTML_ATTR_Alignment},
#endif
   {"ALIGN", "P", 'A', HTML_ATTR_Align},
   {"ALIGN", "TABLE", 'A', HTML_ATTR_Align},
   {"ALIGN", "TD", 'A', HTML_ATTR_Cell_align},
   {"ALIGN", "TH", 'A', HTML_ATTR_Cell_align},
   {"ALIGN", "TR", 'A', HTML_ATTR_Row_align},
   {"ALINK", "", 'A', HTML_ATTR_ActiveLinkColor},
   {"ALT", "", 'A', HTML_ATTR_ALT},
   {"BACKGROUND", "", 'A', HTML_ATTR_background_},
   {"BGCOLOR", "", 'A', HTML_ATTR_BackgroundColor},
   {"BORDER", "IMG", 'A', HTML_ATTR_Img_border},
#ifdef COUGAR
   {"BORDER", "OBJECT", 'A', HTML_ATTR_Img_border},
#endif
   {"BORDER", "TABLE", 'A', HTML_ATTR_Border},
   {"CELLSPACING", "", 'A', HTML_ATTR_cellspacing},
   {"CELLPADDING", "", 'A', HTML_ATTR_cellpadding},
   {"CHECKED", "", 'A', HTML_ATTR_Checked},
   {"CLASS", "", 'A', HTML_ATTR_Class},
#ifdef COUGAR
   {"CLASSID", "", 'A', HTML_ATTR_classid},
#endif
   {"CLEAR", "BR", 'A', HTML_ATTR_Clear},
   {"CODE", "", 'A', HTML_ATTR_code},
   {"CODEBASE", "", 'A', HTML_ATTR_codebase},
#ifdef COUGAR
   {"CODETYPE", "", 'A', HTML_ATTR_codetype},
#endif
   {"COLOR", "", 'A', HTML_ATTR_color},
   {"COLS", "", 'A', HTML_ATTR_Columns},
   {"COLSPAN", "", 'A', HTML_ATTR_colspan_},
   {"COMPACT", "", 'A', HTML_ATTR_COMPACT},
   {"CONTENT", "", 'A', HTML_ATTR_meta_content},
   {"COORDS", "", 'A', HTML_ATTR_coords},
#ifdef COUGAR
   {"DATA", "", 'A', HTML_ATTR_data},
#endif
   {"ENCTYPE", "", 'A', HTML_ATTR_ENCTYPE},
   {"HEIGHT", "APPLET", 'A', HTML_ATTR_Height_},
   {"HEIGHT", "IMG", 'A', HTML_ATTR_Height_},
#ifdef COUGAR
   {"HEIGHT", "OBJECT", 'A', HTML_ATTR_Height_},
#endif
   {"HEIGHT", "TD", 'A', HTML_ATTR_Cell_height},
   {"HEIGHT", "TH", 'A', HTML_ATTR_Cell_height},
   {"HREF", "", 'A', HTML_ATTR_HREF_},
   {"HSPACE", "", 'A', HTML_ATTR_hspace},
   {"HTTP-EQUIV", "", 'A', HTML_ATTR_http_equiv},
#ifdef COUGAR
   {"ID", "", 'A', HTML_ATTR_ID},
#endif
   {"ISMAP", "", 'A', HTML_ATTR_ISMAP},
#ifdef COUGAR
   {"LANG", "", 'A', HTML_ATTR_Langue},
#endif
   {"LINK", "", 'A', HTML_ATTR_LinkColor},
   {"MAXLENGTH", "", 'A', HTML_ATTR_MaxLength},
   {"METHOD", "", 'A', HTML_ATTR_METHOD},
#ifdef MATHML
   {"MODE", "", 'A', HTML_ATTR_mode},
#endif
   {"MULTIPLE", "", 'A', HTML_ATTR_Multiple},
   {"N", "", 'C', 0},
   {"NAME", "APPLET", 'A', HTML_ATTR_applet_name},
   {"NAME", "META", 'A', HTML_ATTR_meta_name},
   {"NAME", "PARAM", 'A', HTML_ATTR_Param_name},
   {"NAME", "", 'A', HTML_ATTR_NAME},
   {"NOHREF", "", 'A', HTML_ATTR_nohref},
   {"NOSHADE", "", 'A', HTML_ATTR_NoShade},
   {"NOWRAP", "", 'A', HTML_ATTR_No_wrap},
   {"PROMPT", "", 'A', HTML_ATTR_Prompt},
   {"REL", "", 'A', HTML_ATTR_REL},
   {"REV", "", 'A', HTML_ATTR_REV},
   {"ROWS", "", 'A', HTML_ATTR_Rows},
   {"ROWSPAN", "", 'A', HTML_ATTR_rowspan_},
   {"SELECTED", "", 'A', HTML_ATTR_Selected},
   {"SHAPE", "", 'A', HTML_ATTR_shape},
   {"SIZE", "BASEFONT", 'A', HTML_ATTR_BaseFontSize},
   {"SIZE", "FONT", 'A', HTML_ATTR_Font_size},
   {"SIZE", "HR", 'A', HTML_ATTR_Size_},
   {"SIZE", "INPUT", 'A', HTML_ATTR_Area_Size},
   {"SIZE", "SELECT", 'A', HTML_ATTR_MenuSize},
   {"SRC", "", 'A', HTML_ATTR_SRC},
   {"START", "", 'A', HTML_ATTR_Start},
   {"STYLE", "", 'A', HTML_ATTR_Style_},
   {"TEXT", "", 'A', HTML_ATTR_TextColor},
   {"TITLE", "", 'A', HTML_ATTR_Title},
   {"TYPE", "LI", 'A', HTML_ATTR_ItemStyle},
#ifdef COUGAR
   {"TYPE", "LINK", 'A', HTML_ATTR_Link_type},
   {"TYPE", "OBJECT", 'A', HTML_ATTR_Object_type},
#endif
   {"TYPE", "OL", 'A', HTML_ATTR_NumberStyle},
   {"TYPE", "STYLE", 'A', HTML_ATTR_Notation},
   {"TYPE", "UL", 'A', HTML_ATTR_BulletStyle},
   {"TYPE", "", SPACE, DummyAttribute},
   {"USEMAP", "", 'A', HTML_ATTR_USEMAP},
   {"VALIGN", "TD", 'A', HTML_ATTR_Cell_valign},
   {"VALIGN", "TH", 'A', HTML_ATTR_Cell_valign},
   {"VALIGN", "TR", 'A', HTML_ATTR_Row_valign},
   {"VALUE", "LI", 'A', HTML_ATTR_ItemValue},
   {"VALUE", "PARAM", 'A', HTML_ATTR_Param_value},
   {"VALUE", "", 'A', HTML_ATTR_Value_},
   {"VERSION", "", 'A', 0},
   {"VLINK", "", 'A', HTML_ATTR_VisitedLinkColor},
   {"VSPACE", "", 'A', HTML_ATTR_vspace},
   {"WIDTH", "APPLET", 'A', HTML_ATTR_Width__},
   {"WIDTH", "HR", 'A', HTML_ATTR_Width__},
   {"WIDTH", "IMG", 'A', HTML_ATTR_Width__},
#ifdef COUGAR
   {"WIDTH", "OBJECT", 'A', HTML_ATTR_Width__},
#endif
   {"WIDTH", "PRE", 'A', HTML_ATTR_Width__},
   {"WIDTH", "TABLE", 'A', HTML_ATTR_Width__},
   {"WIDTH", "TD", 'A', HTML_ATTR_Width__},
   {"WIDTH", "TH", 'A', HTML_ATTR_Width__},
   {"ZZGHOST", "", 'A', HTML_ATTR_Ghost_restruct},
   {"", "", EOS, 0}		/* Last entry. Mandatory */
};

/* mapping table of HTML attribute values */

static AttrValueMapping HTMLAttrValueMappingTable[] =
{
   {HTML_ATTR_Align, "LEFT", HTML_ATTR_Align_VAL_left_},
   {HTML_ATTR_Align, "CENTER", HTML_ATTR_Align_VAL_center_},
   {HTML_ATTR_Align, "RIGHT", HTML_ATTR_Align_VAL_right_},

   {HTML_ATTR_Clear, "LEFT", HTML_ATTR_Clear_VAL_Left_},
   {HTML_ATTR_Clear, "RIGHT", HTML_ATTR_Clear_VAL_Right_},
   {HTML_ATTR_Clear, "ALL", HTML_ATTR_Clear_VAL_All_},
   {HTML_ATTR_Clear, "NONE", HTML_ATTR_Clear_VAL_None},

   {HTML_ATTR_NumberStyle, "1", HTML_ATTR_NumberStyle_VAL_Arabic_},
   {HTML_ATTR_NumberStyle, "a", HTML_ATTR_NumberStyle_VAL_LowerAlpha},
   {HTML_ATTR_NumberStyle, "A", HTML_ATTR_NumberStyle_VAL_UpperAlpha},
   {HTML_ATTR_NumberStyle, "i", HTML_ATTR_NumberStyle_VAL_LowerRoman},
   {HTML_ATTR_NumberStyle, "I", HTML_ATTR_NumberStyle_VAL_UpperRoman},

   {HTML_ATTR_BulletStyle, "DISC", HTML_ATTR_BulletStyle_VAL_disc},
   {HTML_ATTR_BulletStyle, "SQUARE", HTML_ATTR_BulletStyle_VAL_square},
   {HTML_ATTR_BulletStyle, "CIRCLE", HTML_ATTR_BulletStyle_VAL_circle},

   {HTML_ATTR_ItemStyle, "1", HTML_ATTR_ItemStyle_VAL_Arabic_},
   {HTML_ATTR_ItemStyle, "a", HTML_ATTR_ItemStyle_VAL_LowerAlpha},
   {HTML_ATTR_ItemStyle, "A", HTML_ATTR_ItemStyle_VAL_UpperAlpha},
   {HTML_ATTR_ItemStyle, "i", HTML_ATTR_ItemStyle_VAL_LowerRoman},
   {HTML_ATTR_ItemStyle, "I", HTML_ATTR_ItemStyle_VAL_UpperRoman},
   {HTML_ATTR_ItemStyle, "DISC", HTML_ATTR_ItemStyle_VAL_disc},
   {HTML_ATTR_ItemStyle, "SQUARE", HTML_ATTR_ItemStyle_VAL_square},
   {HTML_ATTR_ItemStyle, "CIRCLE", HTML_ATTR_ItemStyle_VAL_circle},

#ifdef MATHML
   {HTML_ATTR_mode, "DISPLAY", HTML_ATTR_mode_VAL_display},
   {HTML_ATTR_mode, "INLINE", HTML_ATTR_mode_VAL_inline_math},
#endif

   {HTML_ATTR_Row_align, "LEFT", HTML_ATTR_Row_align_VAL_Row_left},
   {HTML_ATTR_Row_align, "CENTER", HTML_ATTR_Row_align_VAL_Row_center},
   {HTML_ATTR_Row_align, "RIGHT", HTML_ATTR_Row_align_VAL_Row_right},

   {HTML_ATTR_Cell_align, "LEFT", HTML_ATTR_Cell_align_VAL_Cell_left},
   {HTML_ATTR_Cell_align, "CENTER", HTML_ATTR_Cell_align_VAL_Cell_center},
   {HTML_ATTR_Cell_align, "RIGHT", HTML_ATTR_Cell_align_VAL_Cell_right},

   {HTML_ATTR_Alignment, "TOP", HTML_ATTR_Alignment_VAL_Top_},
   {HTML_ATTR_Alignment, "MIDDLE", HTML_ATTR_Alignment_VAL_Middle_},
   {HTML_ATTR_Alignment, "BOTTOM", HTML_ATTR_Alignment_VAL_Bottom_},
   {HTML_ATTR_Alignment, "LEFT", HTML_ATTR_Alignment_VAL_Left_},
   {HTML_ATTR_Alignment, "RIGHT", HTML_ATTR_Alignment_VAL_Right_},

   {HTML_ATTR_METHOD, "GET", HTML_ATTR_METHOD_VAL_Get_},
   {HTML_ATTR_METHOD, "POST", HTML_ATTR_METHOD_VAL_Post_},

   {HTML_ATTR_Position, "TOP", HTML_ATTR_Position_VAL_Position_top},
   {HTML_ATTR_Position, "BOTTOM", HTML_ATTR_Position_VAL_Position_bottom},

   {HTML_ATTR_Row_valign, "TOP", HTML_ATTR_Row_valign_VAL_Row_top},
   {HTML_ATTR_Row_valign, "MIDDLE", HTML_ATTR_Row_valign_VAL_Row_middle},
   {HTML_ATTR_Row_valign, "BOTTOM", HTML_ATTR_Row_valign_VAL_Row_bottom},

   {HTML_ATTR_Cell_valign, "TOP", HTML_ATTR_Cell_valign_VAL_Cell_top},
   {HTML_ATTR_Cell_valign, "MIDDLE", HTML_ATTR_Cell_valign_VAL_Cell_middle},
   {HTML_ATTR_Cell_valign, "BOTTOM", HTML_ATTR_Cell_valign_VAL_Cell_bottom},

   {HTML_ATTR_shape, "RECT", HTML_ATTR_shape_VAL_rectangle},
   {HTML_ATTR_shape, "CIRCLE", HTML_ATTR_shape_VAL_circle},
   {HTML_ATTR_shape, "POLY", HTML_ATTR_shape_VAL_polygon},

/* HTML attribute TYPE generates a Thot element */
   {DummyAttribute, "CHECKBOX", HTML_EL_Checkbox_Input},
   {DummyAttribute, "HIDDEN", HTML_EL_Hidden_Input},
   {DummyAttribute, "FILE", HTML_EL_File_Input},
   {DummyAttribute, "IMAGE", HTML_EL_PICTURE_UNIT},
   {DummyAttribute, "PASSWORD", HTML_EL_Password_Input},
   {DummyAttribute, "RADIO", HTML_EL_Radio_Input},
   {DummyAttribute, "RESET", HTML_EL_Reset_Input},
   {DummyAttribute, "SUBMIT", HTML_EL_Submit_Input},
   {DummyAttribute, "TEXT", HTML_EL_Text_Input},

/* The following declarations allow the parser to accept boolean attributes */
/* written "checked=CHECKED", for instance */
   {HTML_ATTR_ISMAP, "ISMAP", HTML_ATTR_ISMAP_VAL_Yes_},
   {HTML_ATTR_nohref, "NOHREF", HTML_ATTR_nohref_VAL_Yes_},
   {HTML_ATTR_COMPACT, "COMPACT", HTML_ATTR_COMPACT_VAL_Yes_},
   {HTML_ATTR_Multiple, "MULTIPLE", HTML_ATTR_Multiple_VAL_Yes_},
   {HTML_ATTR_Selected, "SELECTED", HTML_ATTR_Selected_VAL_Yes_},
   {HTML_ATTR_Checked, "CHECKED", HTML_ATTR_Checked_VAL_Yes_},
   {HTML_ATTR_No_wrap, "NOWRAP", HTML_ATTR_No_wrap_VAL_no_wrap},
   {HTML_ATTR_NoShade, "NOSHADE", HTML_ATTR_NoShade_VAL_NoShade_},
   {0, "", 0}			/* Last entry. Mandatory */
};

typedef int         State;	/* a state of the automaton */

/********************** static variables ***********************/

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
static char        *InputText = NULL;
static FILE        *InputFile = NULL;
static int          curChar = 0;
static int          numberOfLinesRead = 0;/* number of lines read in the
					     file */
static int          numberOfCharRead = 0; /* number of characters read in the
					     current line */
static boolean      EmptyLine = TRUE;	  /* no printable character encountered
					     yet in the current line */
static boolean      StartOfFile = TRUE;	  /* no printable character encountered
					     yet in the file */
static boolean      AfterTagPRE = FALSE;  /* <PRE> has just been read */
static boolean      ParsingCSS = FALSE;	  /* reading the content of a STYLE
					     element */
static int          WithinTable = 0;  /* <TABLE> has been read */
static char	    prevChar = EOS;	  /* last character read */
static char*	    docURL = NULL;	  /* path or URL of the document */

/* input buffer */
#define MaxBufferLength 1000
#define AllmostFullBuffer 700
static unsigned char inputBuffer[MaxBufferLength];
static int          LgBuffer = 0;	  /* actual length of text in input
					     buffer */

/* information about the Thot document under construction */
static Document     theDocument = 0;	  /* the Thot document */
static Language     currentLanguage;	  /* language used in the document */
static SSchema      HTMLSSchema = NULL;	  /* the HTML structure schema */
static Element      rootElement;	  /* root element of the document */
static Element      lastElement = NULL;	  /* last element created */
static boolean      lastElementClosed = FALSE;/* last element is complete */
static int          lastElemEntry = 0;	  /* index in the GIMappingTable of the
					     element being created */
static Attribute    lastAttribute = NULL; /* last attribute created */
static Attribute    lastAttrElement = NULL;/* element with which the last
					     attribute has been associated */
static AttributeMapping* lastAttrEntry = NULL;  /* entry in the AttributeMappingTable
					     of the attribute being created */
static boolean      IgnoreAttr = FALSE;	  /* the last attribute encountered is
					     invalid */
static Element      CommentText = NULL;	  /* TEXT element of the current
					     Comment element */
static boolean      UnknownTag = FALSE;	  /* the last start tag encountered is
					     invalid */
static boolean      ReadingHREF = FALSE;  /* reading the value of a HREF
					     attribute */
static boolean      MergeText = FALSE;	  /* character data should be catenated
					     with the last Text element */

static PtrElemToBeChecked FirstElemToBeChecked = NULL;
static PtrElemToBeChecked LastElemToBeChecked = NULL;

/* automaton */
static State        currentState;	  /* current state of the automaton */
static State        returnState;	  /* return state from subautomaton */
static boolean      NormalTransition;

/* information about an entity being read */
#define MaxEntityLength 50
static char         EntityName[MaxEntityLength];/* name of entity being read */
static int          LgEntityName = 0;	  /* length of entity name read so
					     far */
static int          EntityTableEntry = 0; /* entry of the entity table that
					     matches the entity read so far */
static int          CharRank = 0;	  /* rank of the last matching
					     character in that entry */

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *SkipSep (char *ptr)
#else
static char        *SkipSep (ptr)
char               *ptr;
#endif
{
  while (*ptr == SPACE || *ptr == ',')
    ptr++;
  return (ptr);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *SkipInt (char *ptr)
#else
static char        *SkipInt (ptr)
char               *ptr;
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
   char               *ptr3, *text;
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
   text = TtaGetMemory (length + 1);
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
	     TtaAddPointInPolyline (element, length, UnPixel, x1, y1, document);
	     length++;
	  }
     }
   TtaFreeMemory (text);
}


/*----------------------------------------------------------------------
   MapGI
   search in the mapping tables the entry for the element of
   name GI and returns the rank of that entry.
   When returning, schema contains the Thot SSchema that defines that element,
   Returns -1 and schema = NULL if not found.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 MapGI (char *gi, SSchema *schema)
#else
int                 MapGI (gi, schema)
char               *gi;
SSchema		   *schema;

#endif
{
   int                 i;
   int                 entry;

   entry = -1;
   *schema = NULL;
   i = 0;
   do
         if (!strcasecmp (HTMLGIMappingTable[i].htmlGI, gi))
            {
	    entry = i;
/******
	    if (HTMLSSchema == NULL)
	       *schema = TtaGetSSchema ("HTML", doc);
	    else
*****/
	       *schema = HTMLSSchema;
	    }
         else
	    i++;
   while (entry < 0 && HTMLGIMappingTable[i].htmlGI[0] != EOS);
#ifdef MATHML
   if (entry < 0)
      {
      /* tag not found in HTMLGIMappingTable */
      /* search in MathMLGIMappingTable */
      i = 0;
      do
         if (!strcasecmp (MathMLGIMappingTable[i].htmlGI, gi))
	    {
	    entry = i;
/*****
	    if (MathMLSSchema == NULL)
	       *schema = TtaGetSSchema ("MathML", doc);
	    else
******/
	       *schema = MathMLSSchema;
	    }
         else
	    i++;
      while (entry < 0 && MathMLGIMappingTable[i].htmlGI[0] != EOS);
      }
#endif /* MATHML */
   return entry;
}

/*----------------------------------------------------------------------
   GIType  search in mapping tables the Element type associated with
   a given GI Name. If not found returns zero.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GIType (char *gi, ElementType *elType, Document doc)
#else
void                GIType (gi, elType, doc)
char               *gi;
ElementType        *elType;
Document	    doc;
#endif
{
   int                 i;

   elType->ElSSchema = NULL;
   elType->ElTypeNum = 0;
   i = 0;
   do
     {
	if (!strcasecmp (HTMLGIMappingTable[i].htmlGI, gi))
	  {

	    if (HTMLSSchema == NULL && ! (doc == (Document) 0))
	       elType->ElSSchema = TtaGetSSchema ("HTML", doc);
	    else

	       elType->ElSSchema = HTMLSSchema;
	    elType->ElTypeNum = HTMLGIMappingTable[i].ThotType;
	    return;
	  }
	i++;
     }
   while (HTMLGIMappingTable[i].htmlGI[0] != EOS);
#ifdef MATHML
   i = 0;
   do
     {
	if (!strcasecmp (MathMLGIMappingTable[i].htmlGI, gi))
	  {

	    if (MathMLSSchema == NULL && doc != 0)
	       {
	       elType->ElSSchema = TtaGetSSchema ("MathML", doc);
	       /* the document does not contain any MathML element.
		  Associate the MathML Thot schema with the document */
	       if (elType->ElSSchema == NULL)
		  elType->ElSSchema = TtaNewNature (TtaGetDocumentSSchema(doc),
						    "MathML", "MathMLP");
	       }
	    else

	       elType->ElSSchema = MathMLSSchema;
	    elType->ElTypeNum = MathMLGIMappingTable[i].ThotType;
	    return;
	  }
	i++;
     }
   while (MathMLGIMappingTable[i].htmlGI[0] != EOS);
#endif /* MATHML */
}

/*----------------------------------------------------------------------
   GITagNameByType search in the mapping tables the name for a given type
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char               *GITagNameByType (ElementType elType)
#else
char               *GITagNameByType (elType)
ElementType elType;

#endif
{
   int                 i;

   if (elType.ElTypeNum > 0)
     {
	i = 0;
	if (strcmp ("HTML", TtaGetSSchemaName (elType.ElSSchema)) == 0)
	  do
	    {
	     if (HTMLGIMappingTable[i].ThotType == elType.ElTypeNum &&
		 strcmp (HTMLGIMappingTable[i].htmlGI, "LISTING"))	/* use PRE */
		return (char *) HTMLGIMappingTable[i].htmlGI;
	     i++;
	    }
	  while (HTMLGIMappingTable[i].htmlGI[0] != EOS);
#ifdef MATHML
	else if (strcmp ("MathML", TtaGetSSchemaName (elType.ElSSchema)) == 0)
	  do
	    {
	     if (MathMLGIMappingTable[i].ThotType == elType.ElTypeNum)
		return (char *) MathMLGIMappingTable[i].htmlGI;
	     i++;
	    }
	  while (MathMLGIMappingTable[i].htmlGI[0] != EOS);	  
#endif /* MATHML */
     }
   return "???";
}

/*----------------------------------------------------------------------
   GITagName search in GIMappingTable the name for a given element
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char               *GITagName (Element elem)
#else
char               *GITagName (elem)
Element             elem;

#endif
{
   ElementType         elType;

   elType = TtaGetElementType (elem);
   return (GITagNameByType (elType));
}

/*----------------------------------------------------------------------
   MapAttr search in all AttributeMappingTables the entry for the
   attribute of name Attr and returns a pointer to that entry,
   as well as the corresponding Thot SSchema
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static AttributeMapping*          MapAttr (char *Attr, SSchema *schema)
#else
static AttributeMapping*          MapAttr (Attr, schema)
char               *Attr;
SSchema            *schema;

#endif
{
   int                 i;
   int                 entry;

   entry = -1;
   *schema = NULL;
   i = 0;
   do
      if (!strcasecmp (HTMLAttributeMappingTable[i].htmlAttribute, Attr))
	 if (HTMLAttributeMappingTable[i].htmlElement[0] == EOS)
	       {
	       entry = i;
	       *schema = HTMLSSchema;
	       }
	 else if (!strcasecmp (HTMLAttributeMappingTable[i].htmlElement,
			          HTMLGIMappingTable[lastElemEntry].htmlGI))
	       {
	       entry = i;
	       *schema = HTMLSSchema;
	       }
	 else
	       i++;
      else
	 i++;
   while (entry < 0 && HTMLAttributeMappingTable[i].AttrOrContent != EOS);
   if (entry >= 0)
      return (&HTMLAttributeMappingTable[entry]);
#ifdef MATHML
   i = 0;
   do
      if (!strcasecmp (MathMLAttributeMappingTable[i].htmlAttribute, Attr))
	 if (MathMLAttributeMappingTable[i].htmlElement[0] == EOS)
	       {
	       entry = i;
	       *schema = MathMLSSchema;
	       }
	 else if (!strcasecmp (MathMLAttributeMappingTable[i].htmlElement,
			          MathMLGIMappingTable[lastElemEntry].htmlGI))
	       {
	       entry = i;
	       *schema = MathMLSSchema;
	       }
	 else
	       i++;
      else
	 i++;
   while (entry < 0 && MathMLAttributeMappingTable[i].AttrOrContent != EOS);
   if (entry >= 0)
       return (&MathMLAttributeMappingTable[entry]);
#endif
   return NULL;
}

/*----------------------------------------------------------------------
   MapThotAttr     search in AttributeMappingTable the entry for
   the attribute of name Attr and returns the Thot Attribute
   corresponding to the rank of that entry.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 MapThotAttr (char *Attr, char *tag)
#else
int                 MapThotAttr (Attr, tag)
char               *Attr;
char               *tag;

#endif
{
   int                 thotAttr;
   SSchema	       schema;
   AttributeMapping*   tableEntry;

   thotAttr = -1;
   lastElemEntry = MapGI (tag, &schema);
   if (lastElemEntry != -1)
     {
	tableEntry = MapAttr (Attr, &schema);
	if (tableEntry != NULL)
	   thotAttr = tableEntry->ThotAttribute;
     }
   return thotAttr;
}

/*----------------------------------------------------------------------
   MapAttrValue    search in AttrValueMappingTable the entry for
   the attribute ThotAtt and its value AttrVal. Returns the corresponding
   Thot value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 MapAttrValue (int ThotAtt, char *AttrVal)
#else
int                 MapAttrValue (ThotAtt, AttrVal)
int                 ThotAtt;
char               *AttrVal;

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
	    if (AttrVal[0] == HTMLAttrValueMappingTable[i].htmlAttrValue[0])
	       value = HTMLAttrValueMappingTable[i].ThotAttrValue;
	    else
	       i++;
	 else
	    /* for other attributes, uppercase and lowercase are */
	    /* equivalent */
	    if (!strcasecmp (HTMLAttrValueMappingTable[i].htmlAttrValue, AttrVal))
	       value = HTMLAttrValueMappingTable[i].ThotAttrValue;
	    else
	       i++;
      while (value < 0 && HTMLAttrValueMappingTable[i].ThotAttr != 0);
#ifdef MATHML
   if (value < 0)
      {
      i = 0;
      while (MathMLAttrValueMappingTable[i].ThotAttr != ThotAtt &&
	     MathMLAttrValueMappingTable[i].ThotAttr != 0)
         i++;
      if (MathMLAttrValueMappingTable[i].ThotAttr == ThotAtt)
         do
	    if (!strcasecmp (MathMLAttrValueMappingTable[i].htmlAttrValue, AttrVal))
	       value = MathMLAttrValueMappingTable[i].ThotAttrValue;
	    else
	       i++;
         while (value < 0 && MathMLAttrValueMappingTable[i].ThotAttr != 0);
      }
#endif
   return value;
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
		  entry = MapGI (name, &schema);
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
	     if (HTMLGIMappingTable[curCE->tagNum].firstClosedElem == NULL)
		HTMLGIMappingTable[curCE->tagNum].firstClosedElem = newCE;
	     else
	       {
		  lastCE = HTMLGIMappingTable[curCE->tagNum].firstClosedElem;
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
	while (StartTagEndingElem[line][ptr] != SPACE && StartTagEndingElem[line][ptr] != EOS)
	   name[i++] = StartTagEndingElem[line][ptr++];
	name[i] = EOS;
	i = 0;
	ptr++;
	entry = MapGI (name, &schema);
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
	if (strcmp (name, "closes") != 0)
	   fprintf (stderr, "error in StartTagEndingElem: \"%s\" instead of \"closes\" in line\n%s\n", name, StartTagEndingElem[line]);
#endif
	lastCE = HTMLGIMappingTable[entry].firstClosedElem;
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
		  newCE->tagNum = MapGI (name, &schema);
#ifdef DEBUG
		  if (newCE->tagNum < 0)
		     fprintf (stderr, "error in StartTagEndingElem: tag %s unknown in line\n%s\n", name, StartTagEndingElem[line]);
#endif
		  if (lastCE == NULL)
		     HTMLGIMappingTable[entry].firstClosedElem = newCE;
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
#ifdef __STDC__
static boolean      Within (int ThotType, SSchema ThotSSchema)
#else
static boolean      Within (ThotType, ThotSSchema)
int                 ThotType;
SSchema		    ThotSSchema;

#endif
{
   boolean             ret;
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
void                ParseHTMLError (Document doc, unsigned char *msg)
#else
void                ParseHTMLError (doc, msg)
Document            doc;
unsigned char      *msg;

#endif
{
   if (doc == theDocument)
      /* the error message is related to the document being parsed */
      {
      if (docURL != NULL)
	 {
	 fprintf (stderr, "*** Errors in %s\n", docURL);
#ifndef STANDALONE
	 TtaFreeMemory (docURL);
#endif /* STANDALONE */
	 docURL = NULL;
	 }
      /* print the line number and character number before the message */
      fprintf (stderr, "   line %d, char %d: %s\n", numberOfLinesRead,
	       numberOfCharRead, msg);
      }
   else
      /* print only the error message */
      fprintf (stderr, "%s\n", msg);
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
static boolean      InsertElement (Element * el);

#else
static boolean      InsertElement ();

#endif

/*----------------------------------------------------------------------
   InsertSibling   return TRUE if the new element must be inserted
   in the Thot document as a sibling of lastElement;
   return FALSE it it must be inserted as a child.
  ----------------------------------------------------------------------*/
static boolean      InsertSibling ()
{
   if (StackLevel == 0)
      return FALSE;
   else if (lastElementClosed ||
	    TtaIsLeaf (TtaGetElementType (lastElement)) ||
	    (GINumberStack[StackLevel - 1] >= 0 &&
	     HTMLGIMappingTable[GINumberStack[StackLevel - 1]].htmlContents == 'E'))
      return TRUE;
   else
      return FALSE;
}

/*----------------------------------------------------------------------
   IsEmptyElement return TRUE if element el is defined as an empty element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean         IsEmptyElement (Element el)
#else
static boolean         IsEmptyElement (el)
Element             el;

#endif
{
   ElementType         elType;
   int                 i;
   boolean             ret;

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
boolean             IsCharacterLevelElement (Element el)
#else
boolean             IsCharacterLevelElement (el)
Element             el;

#endif
{
   ElementType         elType;
   int                 i;
   boolean             ret;

   ret = FALSE;
   elType = TtaGetElementType (el);
   i = 0;
   while (CharLevelElement[i] > 0 &&
	  CharLevelElement[i] != elType.ElTypeNum)
      i++;
   if (CharLevelElement[i] == elType.ElTypeNum)
      ret = TRUE;
   return ret;
}

/*----------------------------------------------------------------------
   IsBlockElement  return TRUE if element el is a block element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      IsBlockElement (Element el)
#else
static boolean      IsBlockElement (el)
Element             el;

#endif
{
   ElementType         elType;
   int                 i;
   boolean             ret;

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
static boolean      CannotContainText (ElementType elType)
#else
static boolean      CannotContainText (elType)
ElementType         elType;

#endif
{
   int                 i;
   boolean             ret;

   ret = FALSE;
   i = 0;
   while (NoTextChild[i] > 0 && NoTextChild[i] != elType.ElTypeNum)
      i++;
   if (NoTextChild[i] == elType.ElTypeNum)
      ret = TRUE;
   return ret;
}

#ifdef MATHML

/*----------------------------------------------------------------------
   MathInsertSibling   return TRUE if the new element must be inserted
   in the Thot document as a sibling of lastElement;
   return FALSE it it must be inserted as a child.
  ----------------------------------------------------------------------*/
static boolean      MathInsertSibling ()
{
   if (StackLevel == 0)
      return FALSE;
   else
      if (lastElementClosed || TtaIsLeaf (TtaGetElementType (lastElement)))
         return TRUE;
   else
      return FALSE;
}

/*----------------------------------------------------------------------
   InsertMathElement   inserts element el in the abstract tree of the
   Thot document, at the current position.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      InsertMathElement (Element * el)
#else
static boolean      InsertMathElement (el)
Element            *el;

#endif
{
   boolean             ret;
   Element             parent;

   if (MathInsertSibling ())
     {
	if (lastElement == NULL)
	   parent = NULL;
	else
	   parent = TtaGetParent (lastElement);
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
   TextToMath  Put the content of input buffer into the document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         TextToMath (char alphabet)
#else
static void         TextToMath (alphabet)
char	alphabet;

#endif
{
   ElementType         elType, parentType;
   Element             elContent, parent;
   int                 i, firstChar, lastChar;
   Language	       lang;

   CloseBuffer ();
   if (lastElement != NULL)
     {
	if (MathInsertSibling ())
	   /* There is a previous sibling (lastElement) for the new Text
	      element */
	  {
	     parent = TtaGetParent (lastElement);
	     if (parent == NULL)
		parent = lastElement;
	  }
	else
	   /* the new Text element should be the first child of the latest
	      element encountered */
	   parent = lastElement;
	/* suppress leading spaces */
	for (i = 0; inputBuffer[i] <= SPACE && inputBuffer[i] != EOS; i++);
	firstChar = i;
	/* suppress trailing spaces */
	lastChar = firstChar;
	for (i = firstChar; inputBuffer[i] != EOS; i++)
	   if (inputBuffer[i] > SPACE)
	      lastChar = i;
	inputBuffer[lastChar+1] = EOS;

	if (inputBuffer[firstChar] != EOS)
	  {
	    parentType = TtaGetElementType (parent);
	    if (parentType.ElTypeNum == MathML_EL_MF &&
		firstChar == lastChar &&
		(inputBuffer[firstChar] == '(' ||
		 inputBuffer[firstChar] == ')' ||
		 inputBuffer[firstChar] == '[' ||
		 inputBuffer[firstChar] == ']' ||
		 inputBuffer[firstChar] == '{' ||
		 inputBuffer[firstChar] == '}'))
	       /* create a Thot SYMBOL */
	       elType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
	    else if (parentType.ElTypeNum == MathML_EL_MF &&
		     firstChar == lastChar &&
		     inputBuffer[firstChar] == '|')
	       /* create a Thot GRAPHIC */
	       {
	       elType.ElTypeNum = MathML_EL_GRAPHICS_UNIT;
	       inputBuffer[firstChar] = 'v';
	       }
	    else
	       /* create a TEXT element */
	       elType.ElTypeNum = MathML_EL_TEXT_UNIT;
	    elType.ElSSchema = MathMLSSchema;
	    elContent = TtaNewElement (theDocument, elType);
	    InsertMathElement (&elContent);
	    lastElementClosed = TRUE;
	    MergeText = FALSE;
	    if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
	       {
	       /* put the content of the input buffer into the TEXT element */
	       lang = TtaGetLanguageIdFromAlphabet(alphabet);
	       TtaSetTextContent (elContent, &(inputBuffer[firstChar]), lang, theDocument);
	       }
	    else
	       TtaSetGraphicsShape (elContent, inputBuffer[firstChar], theDocument);
	  }
     }
   InitBuffer ();
}

/*----------------------------------------------------------------------
  ElementNeedsPlaceholder
  returns TRUE if element el needs a sibling placeholder.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean      ElementNeedsPlaceholder (Element el)
#else
boolean      ElementNeedsPlaceholder (el)
Element el;
 
#endif
{
  ElementType   elType;
  Element	child, parent;
  boolean	ret;
 
  ret = FALSE;
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == MathML_EL_MROW ||
      elType.ElTypeNum == MathML_EL_MF ||
      elType.ElTypeNum == MathML_EL_MFENCED ||
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
   boolean	create;

   elType.ElSSchema = MathMLSSchema;
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
	    attrType.AttrTypeNum = MathML_ATTR_placeholder;
	    attr = TtaNewAttribute (attrType);
	    TtaAttachAttribute (constr, attr, doc);
	    TtaSetAttributeValue (attr, MathML_ATTR_placeholder_VAL_yes_, constr, doc);
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
	   attrType.AttrTypeNum = MathML_ATTR_placeholder;
	   attr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (constr, attr, doc);
	   TtaSetAttributeValue (attr, MathML_ATTR_placeholder_VAL_yes_, constr, doc);
	   } 
      }
}
#endif /* MATHML */

/*----------------------------------------------------------------------
   TextToDocument  Put the content of input buffer in the document.
  ----------------------------------------------------------------------*/
static void         TextToDocument ()
{
   ElementType         elType;
   Element             elText, parent, ancestor, prev;
   int                 i;
   boolean             ignoreLeadingSpaces;

#ifdef MATHML
   if (WithinMathML)
      {
      TextToMath('L');
      return;
      }
#endif /* MATHML */
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
	     if (IsCharacterLevelElement (lastElement))
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
	     if (elType.ElTypeNum != HTML_EL_Option_Menu)
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
	elType = TtaGetElementType (parent);
	if (elType.ElTypeNum == HTML_EL_Styles && elType.ElSSchema == HTMLSSchema)
	  {
#ifndef STANDALONE
	     ParseHTMLStyleHeader (parent, inputBuffer, theDocument, FALSE);
#endif
	     InitBuffer ();
	     return;
	  }
	if (ignoreLeadingSpaces)
	   if (!Within (HTML_EL_Preformatted, HTMLSSchema))
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
		  elType.ElSSchema = HTMLSSchema;
		  elType.ElTypeNum = HTML_EL_TEXT_UNIT;
		  elText = TtaNewElement (theDocument, elType);
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
static void         StartOfTag (char c)
#else
static void         StartOfTag (c)
char                c;

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
static void         PutInBuffer (unsigned char c)
#else
static void         PutInBuffer (c)
unsigned char       c;

#endif
{
   int                 len;

   /* put the character into the buffer if it is not an ignored char. */
   if ((int) c == 9)		/* HT */
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
		ParseHTMLError (theDocument, "Panic: buffer overflow");
	     LgBuffer = 0;
	  }
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
static boolean      CheckSurrounding (Element * el, Element parent)
#else
static boolean      CheckSurrounding (el, parent)
Element             *el;
Element             parent;

#endif
{
   ElementType         parentType, newElType, elType;
   Element             newEl, ancestor, prev, prevprev;
   boolean	       ret;

   if (parent == NULL)
      return(FALSE);
   ret = FALSE;
   elType = TtaGetElementType (*el);
   if (elType.ElTypeNum == HTML_EL_TEXT_UNIT || elType.ElTypeNum == HTML_EL_BR
       || elType.ElTypeNum == HTML_EL_PICTURE_UNIT
       || elType.ElTypeNum == HTML_EL_Input)
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
	       !Within (HTML_EL_Option_Menu, HTMLSSchema))
	      /* Element ancestor cannot contain text directly. Create a */
	      /* Pseudo_paragraph element as the parent of the text element */
	      {
	      newElType.ElSSchema = HTMLSSchema;
	      newElType.ElTypeNum = HTML_EL_Pseudo_paragraph;
	      newEl = TtaNewElement (theDocument, newElType);
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
	     newElType.ElSSchema = HTMLSSchema;
	     newElType.ElTypeNum = HTML_EL_Inserted_Text;
	     newEl = TtaNewElement (theDocument, newElType);
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
static boolean      InsertElement (Element * el)
#else
static boolean      InsertElement (el)
Element            *el;

#endif
{
   boolean             ret;
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
static void         CreateAttr (Element el, AttributeType attrType, char *text, boolean invalid)
#else
static void         CreateAttr (el, attrType, text, invalid)
Element             el;
AttributeType       attrType;
char               *text;
boolean             invalid;

#endif
{
   int                 attrKind;
   int                 length;
   char               *buffer;
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
	/* attribute BORDER without any value (boolean attribute) is */
	/* considered as BORDER=1 */
	if (attrType.AttrTypeNum == HTML_ATTR_Border)
	   TtaSetAttributeValue (attr, 1, el, theDocument);
	if (invalid)
	   /* Copy the name of the invalid attribute as the content */
	   /* of the Invalid_attribute attribute. */
	  {
	     length = strlen (text) + 2;
	     length += TtaGetTextAttributeLength (attr);
	     buffer = TtaGetMemory (length + 1);
	     TtaGiveTextAttributeValue (attr, buffer, &length);
	     strcat (buffer, " ");
	     strcat (buffer, text);
	     TtaSetAttributeText (attr, buffer, el, theDocument);
	     TtaFreeMemory (buffer);
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
void                OnlyOneOptionSelected (Element el, Document doc, boolean parsing)
#else  /* __STDC__ */
void                OnlyOneOptionSelected (el, doc, parsing)
Element             el;
Document            doc;
boolean             parsing;

#endif /* __STDC__ */
{
   ElementType         elType;
   Element             option, menu;
   AttributeType       attrType;
   Attribute           attr;

   if (el == NULL)
      return;

   elType = TtaGetElementType (el);

   if (elType.ElTypeNum == HTML_EL_Option_Menu)
     {
	/* it's an menu (SELECT) */
	/* search the first OPTION element having an attribute SELECTED */
	attr = NULL;
	attrType.AttrSSchema = elType.ElSSchema;
	attrType.AttrTypeNum = HTML_ATTR_Selected;
	option = TtaGetFirstChild (el);
	while (option != NULL && attr == NULL)
	  {
	     elType = TtaGetElementType (option);
	     if (elType.ElTypeNum == HTML_EL_Option)
		attr = TtaGetAttribute (option, attrType);
	     if (attr == NULL)
		TtaNextSibling (&option);
	  }
	if (option != NULL)
	  el = option;
	else
	  {
	  option = TtaGetFirstChild (el);
	  elType = TtaGetElementType (option);
	  while (option != NULL && elType.ElTypeNum != HTML_EL_Option)
	     {
	     TtaNextSibling (&option);
	     if (option != NULL)
		elType = TtaGetElementType (option);
	     }
	  if (elType.ElTypeNum == HTML_EL_Option)
	     el = option;
	  }
     }
   else
      while (elType.ElTypeNum != HTML_EL_BODY &&
	     elType.ElTypeNum != HTML_EL_Option && el != NULL)
	{
	   el = TtaGetParent (el);
	   elType = TtaGetElementType (el);
	}

   if (elType.ElTypeNum == HTML_EL_Option && el != NULL)
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

	/* remove the SELECTED attribute from other options in the menu */
	/* if it's not a multiple-choices menu */
	menu = TtaGetParent (el);
	if (menu != NULL)
	  {
	     attrType.AttrTypeNum = HTML_ATTR_Multiple;
	     attr = TtaGetAttribute (menu, attrType);
	     if (attr == NULL)
		{
		attrType.AttrTypeNum = HTML_ATTR_Selected;
	        option = TtaGetFirstChild (menu);
	        while (option != NULL)
	          {
		  if (option != el)
		    {
		       /* Search the SELECTED attribute */
		       attr = TtaGetAttribute (option, attrType);
		       /* remove it if it exists */
		       if (attr != NULL)
			  TtaRemoveAttribute (option, attr, doc);
		    }
		  TtaNextSibling (&option);
	          }
		}
	     if (parsing)
		/* when parsing the HTML file, associate a DefaultSelected
		   attribute with each element having a SELECTED attribute */
		{
		  option = TtaGetFirstChild (menu);
		  while (option != NULL)
		     {
		     attrType.AttrTypeNum = HTML_ATTR_Selected;
		     attr = TtaGetAttribute (option, attrType);
		     if (attr != NULL)
			{
			attrType.AttrTypeNum = HTML_ATTR_DefaultSelected;
			attr = TtaGetAttribute (option, attrType);
			if (attr == NULL)
			   {
			   /* create the DefaultSelected attribute */
			   attr = TtaNewAttribute (attrType);
			   TtaAttachAttribute (option, attr, doc);
			   TtaSetAttributeValue (attr, HTML_ATTR_DefaultSelected_VAL_Yes_, option, doc);
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
   Element             constElem, child, desc, leaf;
#ifdef MATHML
   Element	       added, prev, next, firstChild;
#endif
   Attribute           attr;
   AttributeType       attrType;
   char               *text;
   char                lastChar[2];
   Language            lang;
#ifdef STANDALONE
   char               *name1, *name2;
   char               *imageName;
#else
   char               *name1, *name2;
#endif
   int                 length;

    elType = TtaGetElementType (el);
  /* is this an block-level element in a character-level element? */
   if (!IsCharacterLevelElement (el) && elType.ElTypeNum != HTML_EL_Comment_)
      BlockInCharLevelElem (el);

   newElType.ElSSchema = elType.ElSSchema;
   switch (elType.ElTypeNum)
	 {
#ifdef COUGAR
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
	       attrType.AttrSSchema = HTMLSSchema;
	       attrType.AttrTypeNum = HTML_ATTR_data;
	       attr = TtaGetAttribute (el, attrType);
	       if (attr != NULL)
		 {
		    length = TtaGetTextAttributeLength (attr);
		    if (length > 0)
		      {
			name1 = TtaGetMemory (length + 1);
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
#endif /* COUGAR */
#ifdef MATHML
	    case HTML_EL_Math:
	    case HTML_EL_MathDisp:
		/*  it's a Math element. Create a MathML element */
	        firstChild = TtaGetFirstChild(el);
	        if (firstChild != NULL)
		  {
		  childType = TtaGetElementType (firstChild);
		  if (childType.ElSSchema != MathMLSSchema  ||
		      childType.ElTypeNum != MathML_EL_MathML)
		     {
		     elType.ElSSchema = MathMLSSchema;
		     elType.ElTypeNum = MathML_EL_MathML;
		     added = TtaNewElement (theDocument, elType);
		     TtaInsertSibling (added, firstChild, TRUE, theDocument);
		     next = firstChild;
		     TtaNextSibling (&next);
		     TtaRemoveTree (firstChild, theDocument);
		     TtaInsertFirstChild (&firstChild, added, theDocument);
		     prev = firstChild;
		     while (next != NULL)
		        {
		        child = next;
		        TtaNextSibling (&next);
		        TtaRemoveTree (child, theDocument);
		        TtaInsertSibling (child, prev, FALSE, theDocument);
		        prev = child;
		        }
		     /* Create placeholders within the MathML element */
		     CreatePlaceholders (firstChild, theDocument);
		     }
		  }
		break;
#endif /* MATHML */
	    case HTML_EL_Input:	/*  it's an INPUT without TYPE attribute */
		/* Create a child of type Text_Input */
		elType.ElTypeNum = HTML_EL_Text_Input;
		child = TtaNewTree (theDocument, elType, "");
		TtaInsertFirstChild (&child, el, theDocument);
	    case HTML_EL_Text_Input:
		attrType.AttrSSchema = HTMLSSchema;
		attrType.AttrTypeNum = HTML_ATTR_Value_;
		attr = TtaGetAttribute (el, attrType);
		if (attr != NULL)
		   {
		   /* copy the value of attribute "value" into the first text
		      leaf of element */
		   length = TtaGetTextAttributeLength (attr);
		   if (length > 0)
		     {
		        text = TtaGetMemory (length + 1);
		        TtaGiveTextAttributeValue (attr, text, &length);
		        leaf = TtaGetFirstChild (el);
			if (leaf != NULL)
			  {
			  childType = TtaGetElementType (leaf);
			  if (childType.ElTypeNum == HTML_EL_TEXT_UNIT)
			    TtaSetTextContent (leaf, text, currentLanguage,
					       theDocument);
			  }
			TtaFreeMemory (text);
		     }
		   }
	       break;

            case HTML_EL_Preformatted:          /* it's a preformatted */
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
			     if (lastChar[0] == (char) 138)
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
	       break;

	    case HTML_EL_Text_Area:	/* it's a Text_Area */
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
		    attrType.AttrSSchema = HTMLSSchema;
		    attrType.AttrTypeNum = HTML_ATTR_Default_Value;
		    if (TtaGetAttribute (el, attrType) == NULL)
		       /* attribute Default_Value is missing */
		      {
			 attr = TtaNewAttribute (attrType);
			 TtaAttachAttribute (el, attr, theDocument);
			 desc = TtaGetFirstChild (child);
			 length = TtaGetTextLength (desc) + 1;
			 text = TtaGetMemory (length);
			 TtaGiveTextContent (desc, text, &length, &lang);
			 TtaSetAttributeText (attr, text, el, theDocument);
			 TtaFreeMemory (text);
		      }
		 }
	       /* insert constant C_Empty */
	       newElType.ElTypeNum = HTML_EL_Frame;
	       constElem = TtaNewTree (theDocument, newElType, "");
	       TtaInsertSibling (constElem, child, FALSE, theDocument);
	       break;

	    case HTML_EL_Radio_Input:
	    case HTML_EL_Checkbox_Input:
	       /* put an attribute Checked if it is missing */
	       attrType.AttrSSchema = HTMLSSchema;
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
	       attrType.AttrSSchema = HTMLSSchema;
	       attrType.AttrTypeNum = HTML_ATTR_SRC;
	       attr = TtaGetAttribute (el, attrType);
	       if (attr != NULL)
		 {
		    length = TtaGetTextAttributeLength (attr);
		    name1 = TtaGetMemory (length + 1);
		    name2 = TtaGetMemory (length + 1);
		    imageName = TtaGetMemory (length + 1);
		    TtaGiveTextAttributeValue (attr, name1, &length);
		    /* extract image name from full name */
		    TtaExtractName (name1, name2, imageName);
		    if (strlen (imageName) == 0)
		       /* full names ends with ''/ */
		       TtaExtractName (name2, name1, imageName);
		    if (strlen (imageName) != 0)
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
	       attrType.AttrSSchema = HTMLSSchema;
	       attrType.AttrTypeNum = HTML_ATTR_REL;
	       attr = TtaGetAttribute (el, attrType);
	       if (attr != NULL)
		  /* there is an attribute REL */
		 {
		    length = TtaGetTextAttributeLength (attr);
		    name1 = TtaGetMemory (length + 1);
		    TtaGiveTextAttributeValue (attr, name1, &length);
		    if ((!strcasecmp (name1, "STYLESHEET")) || (!strcasecmp (name1, "STYLE")))
		      {
			 /* it's a link to a style sheet. Load that style sheet */
			 attrType.AttrSSchema = HTMLSSchema;
			 attrType.AttrTypeNum = HTML_ATTR_HREF_;
			 attr = TtaGetAttribute (el, attrType);
			 if (attr != NULL)
			   {
			      length = TtaGetTextAttributeLength (attr);
			      name2 = TtaGetMemory (length + 1);
			      TtaGiveTextAttributeValue (attr, name2, &length);
			      /* load the stylesheet file found here ! */
			      LoadHTMLStyleSheet (name2, theDocument);
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
		   child = TtaNewTree (theDocument, elType, "");
		   if (child != NULL)
		     TtaInsertFirstChild (&child, el, theDocument);
		 }

#ifndef STANDALONE
	       if (WithinTable == 0)
		 NewCell (el, theDocument);
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
static boolean      RemoveEndingSpaces (Element el)
#else
static boolean      RemoveEndingSpaces (el)
Element el;

#endif
{
   int                 length, nbspaces;
   ElementType         elType;
   Element             lastLeaf;
   char                lastChar[2];
   boolean             endingSpacesDeleted;

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

#ifdef MATHML

/*----------------------------------------------------------------------
  NextNotSep
  Return the next sibling of element el that is not a SEP element
  Return el itself if it's not a SEP
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void	NextNotSep (Element* el, Element* prev)
#else
static void	NextNotSep (el, prev)
   Element	*el;
#endif
{
   ElementType	elType;

   if (*el == NULL)
      return;
   elType = TtaGetElementType (*el);
   while (*el != NULL && elType.ElTypeNum == MathML_EL_SEP)
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
static void	CheckMathSubExpressions (Element el, int type1, int type2, int type3)
#else
static void	CheckMathSubExpressions (el, type1, type2, type3)
   Element	el;
   int		type1;
   int		type2;
   int		type3;
#endif
{
  Element	child, added, prev;
  ElementType	elType, childType;

  elType.ElSSchema = MathMLSSchema;
  child = TtaGetFirstChild (el);
  prev = NULL;
  NextNotSep (&child, &prev);
  if (child != NULL && type1 != 0)
    {
      elType.ElTypeNum = type1;
      childType = TtaGetElementType (child);
      if (TtaSameTypes (childType, elType) == 0)
	{
	  TtaRemoveTree (child, theDocument);	
	  added = TtaNewElement (theDocument, elType);
	  if (prev == NULL)
	    TtaInsertFirstChild (&added, el, theDocument);
	  else
	    TtaInsertSibling (added, prev, FALSE, theDocument);
	  TtaInsertFirstChild (&child, added, theDocument);
	  CreatePlaceholders (child, theDocument);
	  child = added;
	}
      if (type2 != 0)
	{
	  prev = child;
	  TtaNextSibling (&child);
	  NextNotSep (&child, &prev);
	  if (child != NULL)
	    {
	      elType.ElTypeNum = type2;
	      childType = TtaGetElementType (child);
	      if (TtaSameTypes (childType, elType) == 0)
		{
		  TtaRemoveTree (child, theDocument);
		  added = TtaNewElement (theDocument, elType);
		  TtaInsertSibling (added, prev, FALSE, theDocument);
		  TtaInsertFirstChild (&child, added, theDocument);
		  CreatePlaceholders (child, theDocument);
		  child = added;
		}
	      if (type3 != 0)
		{
		  prev = child;
		  TtaNextSibling (&child);
		  NextNotSep (&child, &prev);
		  if (child != NULL)
		    {
		      elType.ElTypeNum = type3;
		      childType = TtaGetElementType (child);
		      if (TtaSameTypes (childType, elType) == 0)
			{
			  TtaRemoveTree (child, theDocument);
			  added = TtaNewElement (theDocument, elType);
			  TtaInsertSibling (added, prev, FALSE, theDocument);
			  TtaInsertFirstChild (&child, added, theDocument);
			  CreatePlaceholders (child, theDocument);
			}
		    }
		}
	    }
        }
    }
}


/*----------------------------------------------------------------------
   SetSingleHorizStretchAttr

   Put a horizstretch attribute on element el if it contains only
   a MO element that is a stretchable symbol.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void SetSingleHorizStretchAttr (Element el, Document doc, Element* selEl)
#else /* __STDC__*/
void SetSingleHorizStretchAttr (el, doc, selEl)
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
  char		alphabet;
  unsigned char	text[2], c;

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
			/* attach a horizstretch attribute */
			attrType.AttrSSchema = elType.ElSSchema;
			attrType.AttrTypeNum = MathML_ATTR_horizstretch;
			attr = TtaNewAttribute (attrType);
			TtaAttachAttribute (el, attr, doc);
			TtaSetAttributeValue (attr, MathML_ATTR_horizstretch_VAL_yes_, el, doc);
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
			TtaSetGraphicsShape (symbolEl, c, doc);
			}
		}
	      }
	   }
	}
     }
}

/*----------------------------------------------------------------------
   SetHorizStretchAttr

   Put a horizstretch attribute on all children of element el which
   contain only a MO element that is a stretchable symbol.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void SetHorizStretchAttr (Element el, Document doc)
#else /* __STDC__*/
static void SetHorizStretchAttr (el, doc)
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
     SetSingleHorizStretchAttr (child, doc, NULL);
     TtaNextSibling (&child);
     }
}

/*----------------------------------------------------------------------
   SetVertStretchAttr

   Put a vertstretch attribute on element el if its base element
   (Base for a MSUBSUP, MSUP or MSUB; UnderOverBase for a MUNDEROVER,
   a MUNDER of a MOVER) contains only a MO element that is a vertically
   stretchable symbol.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void SetVertStretchAttr (Element el, Document doc, int base, Element* selEl)
#else /* __STDC__*/
void SetVertStretchAttr (el, doc, base, selEl)
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
  char		alphabet;
  unsigned char	text[2], c;

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
			/* attach a vertstretch attribute */
			attrType.AttrSSchema = elType.ElSSchema;
			attrType.AttrTypeNum = MathML_ATTR_vertstretch;
			attr = TtaNewAttribute (attrType);
			TtaAttachAttribute (el, attr, doc);
			TtaSetAttributeValue (attr, MathML_ATTR_vertstretch_VAL_yes_, el, doc);
			/* replace the TEXT element by a Thot SYMBOL element */
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

/*----------------------------------------------------------------------
   SetPlaceholderAttr

   Put a placeholder attribute on all Construct elements in the
   subtree of root el.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void SetPlaceholderAttr (Element el, Document doc)
#else /* __STDC__*/
static void SetPlaceholderAttr (el, doc)
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
      elType.ElSSchema == MathMLSSchema)
     {
     attrType.AttrSSchema = elType.ElSSchema;
     attrType.AttrTypeNum = MathML_ATTR_placeholder;
     attr = TtaNewAttribute (attrType);
     TtaAttachAttribute (el, attr, doc);
     TtaSetAttributeValue (attr, MathML_ATTR_placeholder_VAL_yes_, el, doc);
     }
  else
     {
     child = TtaGetFirstChild (el);
     while (child != NULL)
        {
        SetPlaceholderAttr (child, doc);
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
static void BuildMultiscript (Element elMMULTISCRIPT)
#else /* __STDC__*/
static void BuildMultiscript (elMMULTISCRIPT)
  Element	elMMULTISCRIPT;
#endif /* __STDC__*/
{
  Element	elem, base, next, group, pair, script, prevPair, prevScript;
  ElementType	elType, elTypeGroup, elTypePair, elTypeScript;

  base = NULL;
  group = NULL;
  prevPair = NULL;
  prevScript = NULL;

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
      TtaRemoveTree (elem, theDocument);

      if (base == NULL)
	/* the current element is the first child of the MMULTISCRIPT
	   element */
	{
	  /* Create a MultiscriptBase element as the first child of
	     MMULTISCRIPT and move the current element as the first child
	     of the MultiscriptBase element */
	  elTypeGroup.ElTypeNum = MathML_EL_MultiscriptBase;
	  base = TtaNewElement (theDocument, elTypeGroup);
	  TtaInsertFirstChild (&base, elMMULTISCRIPT, theDocument);
	  TtaInsertFirstChild (&elem, base, theDocument);
	}
      else
	/* the current element is a subscript or a superscript */
	{
	  if (group == NULL)
	    /* there is no PostscriptPairs element. Create one */
	    {
	      elTypeGroup.ElTypeNum = MathML_EL_PostscriptPairs;
	      group = TtaNewElement (theDocument, elTypeGroup);
	      TtaInsertSibling (group, base, FALSE, theDocument);
	      elTypePair.ElTypeNum = MathML_EL_PostscriptPair;
	      /* create a first and a last PostscriptPair as placeholders */
	      pair = TtaNewTree (theDocument, elTypePair, "");
	      TtaInsertFirstChild (&pair, group, theDocument);
	      SetPlaceholderAttr (pair, theDocument);
	      prevPair = pair;
	      pair = TtaNewTree (theDocument, elTypePair, "");
	      TtaInsertSibling (pair, prevPair, FALSE, theDocument);
	      SetPlaceholderAttr (pair, theDocument);
	      prevScript = NULL;
	    }
	  if (prevScript == NULL)
	    /* the current element is the first subscript or superscript
	       in a pair */
	    {
	      /* create a PostscriptPair or PrescriptPair element */
	      pair = TtaNewElement (theDocument, elTypePair);
	      if (prevPair == NULL)
		TtaInsertFirstChild (&pair, group, theDocument);
	      else
		TtaInsertSibling (pair, prevPair, FALSE, theDocument);
	      prevPair = pair;
	      /* create a MSubscript element */
	      elTypeScript.ElTypeNum = MathML_EL_MSubscript;
	      script = TtaNewElement (theDocument, elTypeScript);
	      TtaInsertFirstChild (&script, pair, theDocument);
	      prevScript = script;	  
	    }
	  else
	    /* the current element is a superscript in a pair */
	    {
	      /* create a MSuperscript element */
	      elTypeScript.ElTypeNum = MathML_EL_MSuperscript;
	      script = TtaNewElement (theDocument, elTypeScript);
	      /* insert it as a sibling of the previous MSubscript element */
	      TtaInsertSibling (script, prevScript, FALSE, theDocument);
	      prevScript = NULL;	  
	    }
	  /* insert the current element as a child of the new MSuperscript or
	     MSubscript element */
	  TtaInsertFirstChild (&elem, script, theDocument);
	  SetPlaceholderAttr (elem, theDocument);
	}

      CreatePlaceholders (elem, theDocument);

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
		  group = TtaNewTree (theDocument, elTypeGroup, "");
		  TtaInsertSibling (group, elem, TRUE, theDocument);
		  SetPlaceholderAttr (group, theDocument);
		}
	      /* the following elements will be interpreted as sub- superscripts
		 in PrescriptPair elements, wich will be children of this
		 PrescriptPairs element */
	      elTypeGroup.ElTypeNum = MathML_EL_PrescriptPairs;
	      elTypePair.ElTypeNum = MathML_EL_PrescriptPair;
	      group = elem;
	      /* create a first and a last PostscriptPair as placeholders */
	      pair = TtaNewTree (theDocument, elTypePair, "");
	      TtaInsertFirstChild (&pair, group, theDocument);
	      SetPlaceholderAttr (pair, theDocument);
	      prevPair = pair;
	      pair = TtaNewTree (theDocument, elTypePair, "");
	      TtaInsertSibling (pair, prevPair, FALSE, theDocument);
	      SetPlaceholderAttr (pair, theDocument);
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
      elem = TtaNewTree (theDocument, elTypeGroup, "");
      if (group == NULL)
	group = base;
      TtaInsertSibling (elem, group, TRUE, theDocument);
      SetPlaceholderAttr (elem, theDocument);
    }
}


/*----------------------------------------------------------------------
   SetFontslantAttr
   The content of a MI element has been created or modified.
   Create or change attribute IntFontslant for that element accordingly.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void SetFontslantAttr (Element el, Document doc)
#else /* __STDC__*/
void SetFontslantAttr (el, doc)
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
     /* search the fontslant attribute */
     elType = TtaGetElementType (el);
     attrType.AttrSSchema = elType.ElSSchema;
     attrType.AttrTypeNum = MathML_ATTR_fontslant;
     attr = TtaGetAttribute (el, attrType);
     attrType.AttrTypeNum = MathML_ATTR_IntFontslant;
     IntAttr = TtaGetAttribute (el, attrType);
     if (attr != NULL)
	/* there is a fontslant attribute. Remove the corresponding
	   internal attribute that is not needed */
	{
	if (IntAttr != NULL)
	TtaRemoveAttribute (el, IntAttr, doc);
	}
     else
	/* there is no fontslant attribute. Create an internal attribute
	   IntFontslant with a value that depends on the content of the MI */
	{
        /* get content length */
        len = TtaGetElementVolume (el);
        if (len > 1)
           /* put an attribute IntFontslant = IntPlain */
	   {
	   if (IntAttr == NULL)
	      {
	      IntAttr = TtaNewAttribute (attrType);
	      TtaAttachAttribute (el, IntAttr, doc);
	      }
	   TtaSetAttributeValue (IntAttr, MathML_ATTR_IntFontslant_VAL_IntPlain,
				 el, doc);
	   }
        else
	   /* MI contains a single character. Remove attribute IntFontslant
	      if it exists */
	   {
	   if (IntAttr != NULL)
	      TtaRemoveAttribute (el, IntAttr, doc);
	   }
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
		val = MathML_ATTR_addspace_VAL_both;
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
   char		alphabet;
   unsigned char	text[2], c;

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
		      /* attach a vertstretch attribute to the MF element */
		      attrType.AttrSSchema = elType.ElSSchema;
		      attrType.AttrTypeNum = MathML_ATTR_vertstretch;
		      attr = TtaNewAttribute (attrType);
		      TtaAttachAttribute (el, attr, doc);
		      TtaSetAttributeValue (attr, MathML_ATTR_vertstretch_VAL_yes_, el, doc);
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
void      CreateFencedSeparators (Element fencedExpression, Document doc)
#else
void      CreateFencedSeparators (fencedExpression, doc)
Element		fencedExpression;
Document	doc;

#endif
{
   ElementType	 elType;
   Element	 child, separator, leaf, next, prev, mfenced;
   AttributeType attrType;
   Attribute     attr;
   int		 length, sep, i;
   char		 text[32], sepValue[4];

   /* get the separators attribute */
   mfenced = TtaGetParent (fencedExpression);
   elType = TtaGetElementType (fencedExpression);
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = MathML_ATTR_separators;
   text[0] = ',';	/* default value is  sparators=","  */
   text[1] = '\0';
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
   while (text[sep] <= ' ' && text[sep] != '\0')
      sep++;
   /* if attribute separators is empty or contains only spaces, do not
      insert any separator element */
   if (text[sep] != '\0')
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
           sepValue[1] = ' ';
           sepValue[2] = '\0';
           TtaSetTextContent (leaf, sepValue, currentLanguage, doc);
	   /* is there a following non-space character in separators? */
	   i = sep + 1;
	   while (text[i] <= ' ' && text[i] != '\0')
	      i++;
           if (text[i] > ' ' && text[i] != '\0')
              sep = i;
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
void      TransformMFENCED (Element el)
#else
void      TransformMFENCED (el)
Element                 el;

#endif
{
   ElementType	 elType;
   Element	 child, fencedExpression, leaf, fence, next, prev,
		 firstChild;
   AttributeType attrType;
   Attribute     attr;
   int		 length;
   char		 text[32], c;

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
		TtaDeleteTree (child, theDocument);
	    child = next;
	    }
	 /* create FencedSeparator elements in the FencedExpression */
	 CreateFencedSeparators (fencedExpression, theDocument);
	 }
      }
   else
      /* this MFENCED element must be transformed */
      {
      /* create a FencedExpression element as a child of the MFENCED elem. */
      elType = TtaGetElementType (el);
      elType.ElTypeNum = MathML_EL_FencedExpression;
      fencedExpression = TtaNewElement (theDocument, elType);
      TtaInsertFirstChild (&fencedExpression, el, theDocument);
      if (child == NULL)
	/* empty MFENCED element */
	{
        elType.ElTypeNum = MathML_EL_Construct;
	child = TtaNewElement (theDocument, elType);
	TtaInsertFirstChild (&child, fencedExpression, theDocument);
	SetPlaceholderAttr (child, theDocument);
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
	  TtaRemoveTree (child, theDocument);
	  if (prev == NULL)
	    {
	    TtaInsertFirstChild (&child, fencedExpression, theDocument);
	    firstChild = child;
	    }
	  else
	    TtaInsertSibling (child, prev, FALSE, theDocument);
	  prev = child;
	  child = next;
	  }

	/* create FencedSeparator elements in the FencedExpression */
	CreateFencedSeparators (fencedExpression, theDocument);

        /* Create placeholders within the FencedExpression element */
        CreatePlaceholders (firstChild, theDocument);
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
      fence = TtaNewElement (theDocument, elType);
      TtaInsertSibling (fence, fencedExpression, TRUE, theDocument);
      elType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
      leaf = TtaNewElement (theDocument, elType);
      TtaInsertFirstChild (&leaf, fence, theDocument);
      TtaSetGraphicsShape (leaf, c, theDocument);

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
      fence = TtaNewElement (theDocument, elType);
      TtaInsertSibling (fence, fencedExpression, FALSE, theDocument);
      elType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
      leaf = TtaNewElement (theDocument, elType);
      TtaInsertFirstChild (&leaf, fence, theDocument);
      TtaSetGraphicsShape (leaf, c, theDocument);
      }
}

/*----------------------------------------------------------------------
   CheckMathElement
   Check the Thot structure of the MathML element el.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      CheckMathElement (Element el)
#else
void      CheckMathElement (el)
Element                 el;

#endif
{
   ElementType	elType;
   Element	child;

   elType = TtaGetElementType (el);
   
   switch (elType.ElTypeNum)
     {
     case MathML_EL_MI:
	SetFontslantAttr (el, theDocument);
	break;
     case MathML_EL_MO:
	SetAddspaceAttr (el, theDocument);
	SetVertStretchAttr (el, theDocument, 0, NULL);
	break;
     case MathML_EL_MROOT:
	/* end of a Root. Create a RootBase and an Index */
	CheckMathSubExpressions (el, MathML_EL_RootBase, MathML_EL_Index, 0);
	break;
     case MathML_EL_MSQRT:
	/* end od a Square Root. Create a RootBase */
	CheckMathSubExpressions (el, MathML_EL_RootBase, 0, 0);
	break;
     case MathML_EL_MFRAC:
	/* end of a fraction. Create a Numerator and a Denominator */
	CheckMathSubExpressions (el, MathML_EL_Numerator,
				 MathML_EL_Denominator, 0);
	break;
     case MathML_EL_MFENCED:
	TransformMFENCED (el);
	break;
     case MathML_EL_MSUBSUP:
	/* end of a MSUBSUP. Create Base, Subscript, and Superscript */
	CheckMathSubExpressions (el, MathML_EL_Base, MathML_EL_Subscript,
				 MathML_EL_Superscript);
	SetVertStretchAttr (el, theDocument, MathML_EL_Base, NULL);
	break;
     case MathML_EL_MSUB:
	/* end of a MSUB. Create Base and Subscript */
	CheckMathSubExpressions (el, MathML_EL_Base, MathML_EL_Subscript, 0);
	SetVertStretchAttr (el, theDocument, MathML_EL_Base, NULL);
	break;
     case MathML_EL_MSUP:
	/* end of a MSUP. Create Base and Superscript */
	CheckMathSubExpressions (el, MathML_EL_Base, MathML_EL_Superscript, 0);
	SetVertStretchAttr (el, theDocument, MathML_EL_Base, NULL);
	break;
     case MathML_EL_MUNDEROVER:
	/* end of a MUNDEROVER. Create UnderOverBase, Underscript, and
	   Overscript */
	CheckMathSubExpressions (el, MathML_EL_UnderOverBase,
				 MathML_EL_Underscript, MathML_EL_Overscript);
	SetHorizStretchAttr (el, theDocument);
	SetVertStretchAttr (el, theDocument, MathML_EL_UnderOverBase, NULL);
	break;
     case MathML_EL_MUNDER:
	/* end of a MUNDER. Create UnderOverBase, and Underscript */
	CheckMathSubExpressions (el, MathML_EL_UnderOverBase,
				 MathML_EL_Underscript, 0);
	SetHorizStretchAttr (el, theDocument);
	SetVertStretchAttr (el, theDocument, MathML_EL_UnderOverBase, NULL);
	break;
     case MathML_EL_MOVER:
	/* end of a MOVER. Create UnderOverBase, and Overscript */
	CheckMathSubExpressions (el, MathML_EL_UnderOverBase,
				 MathML_EL_Overscript, 0);
	SetHorizStretchAttr (el, theDocument);
	SetVertStretchAttr (el, theDocument, MathML_EL_UnderOverBase, NULL);
	break;
     case MathML_EL_MMULTISCRIPTS:
	/* end of a MMULTISCRIPTS. Create all elements defined in the
	   MathML S schema */
	BuildMultiscript (el);
	break;
     case MathML_EL_MROW:
	/* end of MROW */
	/* if the first and the last child are MO containing a fence character
	   transform the MO into a MF and the character into a Thot SYMBOL */
	child = TtaGetFirstChild (el);
	if (child != NULL)
	   {
	   CheckFence (child, theDocument);
	   child = TtaGetLastChild (el);
	   if (child != NULL)
	      CheckFence (child, theDocument);
	   /* Create placeholders within the MROW */
           CreatePlaceholders (TtaGetFirstChild (el), theDocument);
	   }
	break;
     default:
	break;
     }
}

/*----------------------------------------------------------------------
   CloseMathElement
   End of MathML element defined in entry entry of MappingTable.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      CloseMathElement (int entry)
#else
static boolean      CloseMathElement (entry)
int                 entry;

#endif
{
   int                 i;
   ElementType         elType;
   Element             el;
   boolean             ret;

   ret = FALSE;
   /* the closed HTML element corresponds to a Thot element. */
   /* type of the element to be closed */
   elType.ElSSchema = MathMLSSchema;
   elType.ElTypeNum = MathMLGIMappingTable[entry].ThotType;
   if (StackLevel > 0)
     {
       el = lastElement;
       i = StackLevel - 1;
	 {
	   /* looks in the stack for the element to be closed */
	   while (i >= 0 && entry != GINumberStack[i])
	     i--;
	 }
       if (i >= 0 && entry == GINumberStack[i] &&
	   TtaGetElementType(ElementStack[i]).ElSSchema == MathMLSSchema)
	 /* element found in the stack */
	 {
	   /* This element and its whole subtree are closed */
	   StackLevel = i;
	   lastElement = ElementStack[i];
	   lastElementClosed = TRUE;
	   ret = TRUE;
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
	       i--;
	   if (StackLevel > 0)
	     currentLanguage = LanguageStack[StackLevel - 1];

	   /* check the Thot structure of the complete element */
	   CheckMathElement (lastElement);
	 }
     }
   return ret;
}

/*----------------------------------------------------------------------
   ChangeToMathML
  ----------------------------------------------------------------------*/

static void ChangeToMathML ()
{
   WithinMathML = TRUE;
   if (MathMLSSchema == NULL)
      MathMLSSchema = TtaNewNature(HTMLSSchema, "MathML", "MathMLP");
}

#endif /* MATHML */

/*----------------------------------------------------------------------
   CloseElement
   End of HTML element defined in entry entry of HTMLGIMappingTable.
   Terminate all corresponding Thot elements.
   If start <0, an explicit end tag has been
   encountered in the HTML file, else the end
   of element is implied by the beginning of
   an element described by entry start of
   HTMLGIMappingTable.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      CloseElement (int entry, int start)
#else
static boolean      CloseElement (entry, start)
int                 entry;
int                 start;

#endif
{
   int                 i;
   ElementType         elType, parentType;
   Element             el, parent;
   boolean             ret, stop, spacesDeleted;

   ret = FALSE;
   /* the closed HTML element corresponds to a Thot element. */
   stop = FALSE;
   /* type of the element to be closed */
   elType.ElSSchema = HTMLSSchema;
   elType.ElTypeNum = HTMLGIMappingTable[entry].ThotType;
   if (StackLevel > 0)
     {
       el = lastElement;
       if (lastElementClosed)
	  el = TtaGetParent (el);
       i = StackLevel - 1;
       if (start < 0)
	 /* Explicit close */
	 {
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
	   if (!strcmp (HTMLGIMappingTable[start].htmlGI, "LI"))
	     while (i > 0 && entry != GINumberStack[i] && !stop)
	       if (!strcmp (HTMLGIMappingTable[GINumberStack[i]].htmlGI, "OL") ||
		   !strcmp (HTMLGIMappingTable[GINumberStack[i]].htmlGI, "UL") ||
		   !strcmp (HTMLGIMappingTable[GINumberStack[i]].htmlGI, "DIR") ||
		   !strcmp (HTMLGIMappingTable[GINumberStack[i]].htmlGI, "MENU"))
		 stop = TRUE;
	       else
		 i--;
	   else if (!strcmp (HTMLGIMappingTable[start].htmlGI, "OPTION"))
	     while (i > 0 && entry != GINumberStack[i] && !stop)
	       if (!strcmp (HTMLGIMappingTable[GINumberStack[i]].htmlGI, "SELECT"))
		 stop = TRUE;
	       else
		 i--;
	   else if (!strcmp (HTMLGIMappingTable[start].htmlGI, "DD") ||
		    !strcmp (HTMLGIMappingTable[start].htmlGI, "DT"))
	     while (i > 0 && entry != GINumberStack[i] && !stop)
	       if (!strcmp (HTMLGIMappingTable[GINumberStack[i]].htmlGI, "DL"))
		 stop = TRUE;
	       else
		 i--;
	   else if (!strcmp (HTMLGIMappingTable[start].htmlGI, "TR") ||
		    !strcmp (HTMLGIMappingTable[start].htmlGI, "TD") ||
		    !strcmp (HTMLGIMappingTable[start].htmlGI, "TH"))
	     while (i > 0 && entry != GINumberStack[i] && !stop)
	       if (!strcmp (HTMLGIMappingTable[GINumberStack[i]].htmlGI, "TABLE"))
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
static void         TypeAttrValue (char *val)
#else
static void         TypeAttrValue (val)
char               *val;

#endif
{
   int                 value;
   unsigned char       msgBuffer[MaxBufferLength];
   ElementType         elType;
   Element             newChild;
   AttributeType       attrType;

   value = MapAttrValue (DummyAttribute, val);
   if (value < 0)
     {
	sprintf (msgBuffer, "Unknown attribute value \"TYPE = %s\"", val);
	ParseHTMLError (theDocument, msgBuffer);
	attrType.AttrSSchema = HTMLSSchema;
	attrType.AttrTypeNum = HTMLAttributeMappingTable[0].ThotAttribute;
	sprintf (msgBuffer, "type=%s", val);
	CreateAttr (lastElement, attrType, msgBuffer, TRUE);
     }
   else
     {
	elType = TtaGetElementType (lastElement);
	if (elType.ElTypeNum != HTML_EL_Input)
	  sprintf (msgBuffer, "Duplicate attribute \"TYPE = %s\"", val);
	else
	  {
	     elType.ElSSchema = HTMLSSchema;
	     elType.ElTypeNum = value;
	     newChild = TtaNewTree (theDocument, elType, "");
	     TtaInsertFirstChild (&newChild, lastElement, theDocument);
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
   boolean             orderedList;

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
   EndOfStartTag   a ">" or a "/" (XML) has been read. It indicates the end
   of a start tag.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfStartTag (char c)
#else
static void         EndOfStartTag (c)
char                c;

#endif
{
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;
   int                 length;
   char               *text;

   UnknownTag = FALSE;
#ifdef MATHML
   if (WithinMathML)
     {
     if (c == '/')
	/* empty element */
	CloseMathElement (lastElemEntry);
     }
   else
#endif /* MATHML */
   if ((lastElement != NULL) && (lastElemEntry != -1))
     {
#ifdef MATHML
	if (!strcmp (HTMLGIMappingTable[lastElemEntry].htmlGI, "MATH"))
	   /* <MATH> has been read */
	   ChangeToMathML ();
	else if (!strcmp (HTMLGIMappingTable[lastElemEntry].htmlGI, "MATHDISP"))
	   /* <MATHDISP> has been read.  add an attribute "mode=display"
	      (for compatibility with old MathML version WD-math-970704 */
	   {
	   ChangeToMathML ();
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
	else 
#endif /* MATHML */
	if (!strcmp (HTMLGIMappingTable[lastElemEntry].htmlGI, "PRE"))
	   /* <PRE> has been read */
	   AfterTagPRE = TRUE;
	else if (!strcmp (HTMLGIMappingTable[lastElemEntry].htmlGI, "TABLE"))
	   /* <TABLE> has been read */
	   WithinTable++;
	else if (HTMLGIMappingTable[lastElemEntry].htmlContents == 'E' ||
	    c == '/')
	   /* this is an empty element. Do not expect an end tag */
	   {
	     CloseElement (lastElemEntry, -1);
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
	if (elType.ElTypeNum == HTML_EL_Styles)
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
		  text = TtaGetMemory (length + 1);
		  TtaGiveTextAttributeValue (attr, text, &length);
		  if (!strcasecmp (text, "text/css"))
		     ParsingCSS = TRUE;
		  TtaFreeMemory (text);
	       }
	  }
     }
}

#ifdef __STDC__
static void         ProcessStartGI (char *GIname);

#else
static void         ProcessStartGI ();

#endif

/*----------------------------------------------------------------------
   ContextOK       returns TRUE if the element at position entry
   in the mapping table is allowed to occur in the
   current structural context.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      ContextOK (int entry)
#else
static boolean      ContextOK (entry)
int                 entry;

#endif
{
   boolean             ok;
   int		       saveLastElemEntry;

   if (StackLevel == 0 || GINumberStack[StackLevel - 1] < 0)
     return TRUE;
   else
     {
       ok = TRUE;
       /* only TH and TD elements are allowed as children of a TR element */
       if (!strcmp (HTMLGIMappingTable[GINumberStack[StackLevel - 1]].htmlGI, "TR"))
	 if (strcmp (HTMLGIMappingTable[entry].htmlGI, "TH") &&
	     strcmp (HTMLGIMappingTable[entry].htmlGI, "TD"))
	   ok = FALSE;
       if (ok)
	 /* only CAPTION, THEAD, TFOOT, TBODY, COLGROUP, COL and TR are */
	 /* allowed as children of a TABLE element */
	 if (!strcmp (HTMLGIMappingTable[GINumberStack[StackLevel - 1]].htmlGI, "TABLE"))
	   if (strcmp (HTMLGIMappingTable[entry].htmlGI, "CAPTION") &&
	       strcmp (HTMLGIMappingTable[entry].htmlGI, "THEAD") &&
	       strcmp (HTMLGIMappingTable[entry].htmlGI, "TFOOT") &&
	       strcmp (HTMLGIMappingTable[entry].htmlGI, "TBODY") &&
	       strcmp (HTMLGIMappingTable[entry].htmlGI, "COLGROUP") &&
	       strcmp (HTMLGIMappingTable[entry].htmlGI, "COL") &&
	       strcmp (HTMLGIMappingTable[entry].htmlGI, "TR"))
	     if (!strcmp (HTMLGIMappingTable[entry].htmlGI, "TD") ||
		 !strcmp (HTMLGIMappingTable[entry].htmlGI, "TH"))
	       /* Table cell within a TABLE, without a TR. Assume TR */
	       {
		/* save the last last GI read from the input file */
		saveLastElemEntry = lastElemEntry;
		/* simulate a <TR> tag */
	        ProcessStartGI ("TR");
		/* restore the last tag that has actually been read */
		lastElemEntry = saveLastElemEntry;
	       }
	     else
	       ok = FALSE;
       if (ok)
	 /* only TR is allowed as a child of a THEAD, TFOOT or TBODY element */
	 if (!strcmp (HTMLGIMappingTable[GINumberStack[StackLevel - 1]].htmlGI, "THEAD") ||
	     !strcmp (HTMLGIMappingTable[GINumberStack[StackLevel - 1]].htmlGI, "TFOOT") ||
	     !strcmp (HTMLGIMappingTable[GINumberStack[StackLevel - 1]].htmlGI, "TBODY"))
	   if (strcmp (HTMLGIMappingTable[entry].htmlGI, "TR"))
	     if (!strcmp (HTMLGIMappingTable[entry].htmlGI, "TD") ||
		 !strcmp (HTMLGIMappingTable[entry].htmlGI, "TH"))
	       /* Table cell within a THEAD, TFOOT or TBODY without a TR. */
	       /* Assume TR */
	       {
		/* save the last last GI read from the input file */
		saveLastElemEntry = lastElemEntry;
		/* simulate a <TR> tag */
	        ProcessStartGI ("TR");
		/* restore the last tag that has actually been read */
		lastElemEntry = saveLastElemEntry;
	       }
	     else
	       ok = FALSE;
       if (ok)
	 /* refuse BODY within BODY */
	 if (strcmp (HTMLGIMappingTable[entry].htmlGI, "BODY") == 0)
	   if (Within (HTML_EL_BODY, HTMLSSchema))
	     ok = FALSE;
       if (ok)
	 /* refuse HEAD within HEAD */
	 if (strcmp (HTMLGIMappingTable[entry].htmlGI, "HEAD") == 0)
	   if (Within (HTML_EL_HEAD, HTMLSSchema))
	     ok = FALSE;
       if (ok)
	 /* refuse STYLE within STYLE */
	 if (strcmp (HTMLGIMappingTable[entry].htmlGI, "STYLE") == 0)
	   if (Within (HTML_EL_Styles, HTMLSSchema))
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
   if (HTMLGIMappingTable[entry].htmlGI[0] == 'H' &&
       HTMLGIMappingTable[entry].htmlGI[1] >= '1' &&
       HTMLGIMappingTable[entry].htmlGI[1] <= '6' &&
       HTMLGIMappingTable[entry].htmlGI[2] == EOS)
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
		      CloseElement (GINumberStack[StackLevel - 1], entry);
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
static void         InsertInvalidEl (char *content, boolean position)
#else
static void         InsertInvalidEl (content, position)
char               *content;
boolean		    position;

#endif
{
   ElementType         elType;
   AttributeType       attrType;
   Element             elInv, elText;
   Attribute	       attr;

   elType.ElSSchema = HTMLSSchema;
   elType.ElTypeNum = HTML_EL_Invalid_element;
   elInv = TtaNewElement (theDocument, elType);
   InsertElement (&elInv);
   if (elInv != NULL)
     {
	lastElementClosed = TRUE;
	elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	elText = TtaNewElement (theDocument, elType);
	TtaInsertFirstChild (&elText, elInv, theDocument);
	TtaSetTextContent (elText, content, currentLanguage, theDocument);
	TtaSetAccessRight (elText, ReadOnly, theDocument);
	attrType.AttrSSchema = HTMLSSchema;
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

#ifdef MATHML

/*----------------------------------------------------------------------
   MapMathGI   search in MathMLGIMappingTable the entry for the element of
   name GI and returns the rank of that entry.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 MapMathGI (char *gi)
#else
int                 MapMathGI (gi)
char               *gi;

#endif
{
   int                 i;
   int                 entry;

   entry = -1;
   i = 0;
   do
      if (!strcasecmp (MathMLGIMappingTable[i].htmlGI, gi))
	   entry = i;
      else
	   i++;
   while (entry < 0 && MathMLGIMappingTable[i].htmlGI[0] != EOS);
   return entry;
}


/*----------------------------------------------------------------------
   ProcessStartMathGI  A MathML GI has been read in a start tag.
   Create the corresponding Thot element according to the mapping table.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ProcessStartMathGI (char *GIname)
#else
static void         ProcessStartMathGI (GIname)
char               *GIname;

#endif
{
  ElementType         elType;
  Element             el;
  int                 entry;
  char                msgBuffer[MaxBufferLength];
  boolean             sameLevel;

  /* search the MathML element name in the mapping table */
  entry = MapMathGI (GIname);
  lastElemEntry = entry;
  if (entry < 0)
    /* not found */
    {
      sprintf (msgBuffer, "Unknown tag <%s>", GIname);
      ParseHTMLError (theDocument, msgBuffer);
      UnknownTag = TRUE;
    }
  else
    {
	    el = NULL;
	    sameLevel = TRUE;
	    if (MathMLGIMappingTable[entry].ThotType > 0)
	      {
		  /* create a Thot element */
		    elType.ElSSchema = MathMLSSchema;
		    elType.ElTypeNum = MathMLGIMappingTable[entry].ThotType;
		    if (MathMLGIMappingTable[entry].htmlContents == 'E')
		      /* empty HTML element. Create all children specified */
		      /* in the Thot structure schema */
		      el = TtaNewTree (theDocument, elType, "");
		    else
		      /* the HTML element may have children. Create only */
		      /* the corresponding Thot element, without any child */
		      el = TtaNewElement (theDocument, elType);
		    sameLevel = InsertMathElement (&el);
		    if (el != NULL)
		      {
			if (MathMLGIMappingTable[entry].htmlContents == 'E')
			  lastElementClosed = TRUE;
			if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
			  /* an empty Text element has been created. The */
			  /* following character data must go to that elem. */
			  MergeText = TRUE;
		      }
	      }
	    if (MathMLGIMappingTable[entry].htmlContents != 'E')
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
#endif	/* MATHML */

/*----------------------------------------------------------------------
   ProcessStartGI  An HTML GI has been read in a start tag.
   Create the corresponding Thot thing (element, attribute,
   or character), according to the mapping table.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ProcessStartGI (char *GIname)
#else
static void         ProcessStartGI (GIname)
char               *GIname;

#endif
{
  ElementType         elType;
  Element             el;
  int                 entry;
  char                msgBuffer[MaxBufferLength];
  PtrClosedElement    pClose;
  boolean             sameLevel;
  SSchema	      schema;

  /* ignore tag <P> within PRE */
  if (Within (HTML_EL_Preformatted, HTMLSSchema))
    if (strcasecmp (GIname, "P") == 0)
      return;
  /* search the HTML element name in the mapping table */
  entry = MapGI (GIname, &schema);
  lastElemEntry = entry;
  if (entry < 0)
    /* not found */
    {
      sprintf (msgBuffer, "Unknown tag <%s>", GIname);
      ParseHTMLError (theDocument, msgBuffer);
      UnknownTag = TRUE;
      /* create an Invalid_element */
      sprintf (msgBuffer, "<%s", GIname);
      InsertInvalidEl (msgBuffer, FALSE);
    }
  else
    {
      /* does this start tag also imply the end tag of some current elements? */
      pClose = HTMLGIMappingTable[entry].firstClosedElem;
      while (pClose != NULL)
	{
	  CloseElement (pClose->tagNum, entry);
	  pClose = pClose->nextClosedElem;
	}
      /* process some special cases... */
      SpecialImplicitEnd (entry);
      if (!ContextOK (entry))
	/* element not allowed in the current structural context */
	{
	  sprintf (msgBuffer, "Tag <%s> is not allowed here", GIname);
	  ParseHTMLError (theDocument, msgBuffer);
	  UnknownTag = TRUE;
	  /* create an Invalid_element */
	  sprintf (msgBuffer, "<%s", GIname);
	  InsertInvalidEl (msgBuffer, TRUE);
	}
      else
	  {
	    el = NULL;
	    sameLevel = TRUE;
	    if (HTMLGIMappingTable[entry].ThotType > 0)
	      {
		if (HTMLGIMappingTable[entry].ThotType == HTML_EL_HTML)
		  /* the corresponding Thot element is the root of the
		     abstract tree, which has been created at initialization */
		  el = rootElement;
		else
		  /* create a Thot element */
		  {
		    elType.ElSSchema = HTMLSSchema;
		    elType.ElTypeNum = HTMLGIMappingTable[entry].ThotType;
		    if (HTMLGIMappingTable[entry].htmlContents == 'E')
		      /* empty HTML element. Create all children specified */
		      /* in the Thot structure schema */
		      el = TtaNewTree (theDocument, elType, "");
		    else
		      /* the HTML element may have children. Create only */
		      /* the corresponding Thot element, without any child */
		      el = TtaNewElement (theDocument, elType);
		    sameLevel = InsertElement (&el);
		    if (el != NULL)
		      {
			if (HTMLGIMappingTable[entry].htmlContents == 'E')
			  lastElementClosed = TRUE;
			if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
			  /* an empty Text element has been created. The */
			  /* following character data must go to that elem. */
			  MergeText = TRUE;
		      }
		  }
	      }
	    if (HTMLGIMappingTable[entry].htmlContents != 'E')
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
static void         EndOfStartGI (char c)
#else
static void         EndOfStartGI (c)
char                c;
#endif
{
   char                theGI[MaxBufferLength];

   CloseBuffer ();
   strncpy (theGI, inputBuffer, MaxBufferLength - 1);
   theGI[MaxBufferLength - 1] = EOS;
   InitBuffer ();
   if (lastElementClosed && (lastElement == rootElement))
      /* an element after the tag </html>, ignore it */
      {
      ParseHTMLError (theDocument, "Element after tag </html>. Ignored");
      return;
      }
#ifdef MATHML
   if (WithinMathML)
      ProcessStartMathGI (theGI);
   else
#endif /* MATHML */
      ProcessStartGI (theGI);
}

/*----------------------------------------------------------------------
   EndOfStartGIandTag      a ">" has been read. It indicates the
   end of a GI and the end of a start tag.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfStartGIandTag (char c)
#else
static void         EndOfStartGIandTag (c)
char                c;

#endif
{
   EndOfStartGI (c);
   EndOfStartTag (c);
}

#ifdef MATHML

/*----------------------------------------------------------------------
   EndOfEndMathTag     An end tag has been read in the HTML file.
   Terminate all corresponding Thot elements.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfEndMathTag ()
#else
static void         EndOfEndMathTag ()

#endif
{
   int                 entry;
   char                msgBuffer[MaxBufferLength];

   /* seach the HTML tag in the mapping table */
   entry = MapMathGI (inputBuffer);
   if (entry < 0)
     {
	sprintf (msgBuffer, "Unknown tag </%s>", inputBuffer);
	ParseHTMLError (theDocument, msgBuffer);
     }
   else if (!CloseMathElement (entry))
      /* the end tag does not close any current element */
     {
	/* print an error message... */
	sprintf (msgBuffer, "Unexpected end tag </%s>", inputBuffer);
	ParseHTMLError (theDocument, msgBuffer);
     }
   InitBuffer ();
}
#endif /* MATHML */

/*----------------------------------------------------------------------
   EndOfEndTag     An end tag has been read in the HTML file.
   Terminate all corresponding Thot elements.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfEndTag (char c)
#else
static void         EndOfEndTag (c)
char                c;

#endif
{
   SSchema	       schema;
   char                msgBuffer[MaxBufferLength];
   int                 entry;
   int                 i;
   boolean             ok;

   CloseBuffer ();
#ifdef MATHML
   if (WithinMathML)
	{
	if (strcasecmp(inputBuffer, "MATH") == 0 ||
	    strcasecmp(inputBuffer, "MATHDISP") == 0)
	   WithinMathML = FALSE;
	else
	   {
	   EndOfEndMathTag ();
	   return;
	   }
	}
#endif /* MATHML */
   /* seach the HTML tag in the mapping table */
   entry = MapGI (inputBuffer, &schema);
   if (entry < 0)
     {
	sprintf (msgBuffer, "Unknown tag </%s>", inputBuffer);
	ParseHTMLError (theDocument, msgBuffer);
	/* create an Invalid_element */
	sprintf (msgBuffer, "</%s", inputBuffer);
	InsertInvalidEl (msgBuffer, FALSE);
     }
   else if (!CloseElement (entry, -1))
      /* the end tag does not close any current element */
     {
	/* print an error message... */
	sprintf (msgBuffer, "Unexpected end tag </%s>", inputBuffer);
	ParseHTMLError (theDocument, msgBuffer);
	/* ... and try to recover */
	ok = FALSE;
	if ((inputBuffer[0] == 'H' || inputBuffer[0] == 'h') &&
	    inputBuffer[1] >= '1' && inputBuffer[1] <= '6' &&
	    inputBuffer[2] == EOS)
	   /* the end tag is </Hn>. Consider all Hn as equivalent. */
	   /* </H3> is considered as an end tag for <H2>, for instance */
	  {
	     strcpy (msgBuffer, inputBuffer);
	     msgBuffer[1] = '1';
	     i = 1;
	     do
	       {
		  entry = MapGI (msgBuffer, &schema);
		  ok = CloseElement (entry, -1);
		  msgBuffer[1]++;
		  i++;
	       }
	     while (i <= 6 && !ok);
	  }
	if (!ok)
	   if (!strcasecmp (inputBuffer, "OL") ||
	       !strcasecmp (inputBuffer, "UL") ||
	       !strcasecmp (inputBuffer, "MENU") ||
	       !strcasecmp (inputBuffer, "DIR"))
	      /* the end tag is supposed to close a list */
	      /* try to close another type of list */
	     {
		ok = TRUE;
		if (!CloseElement (MapGI ("OL", &schema), -1))
		   if (!CloseElement (MapGI ("UL", &schema), -1))
		      if (!CloseElement (MapGI ("MENU", &schema), -1))
			 if (!CloseElement (MapGI ("DIR", &schema), -1))
			    ok = FALSE;
	     }
	if (!ok)
	   /* unrecoverable error. Create an Invalid_element */
	  {
	     sprintf (msgBuffer, "</%s", inputBuffer);
	     InsertInvalidEl (msgBuffer, TRUE);
	  }
     }
   InitBuffer ();
}


/*----------------------------------------------------------------------
   PutInContent    put the string ChrString in the leaf of
   current element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element      PutInContent (char *ChrString)
#else
static Element      PutInContent (ChrString)
char               *ChrString;

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
static void         EndOfAttrName (char c)
#else
static void         EndOfAttrName (c)
char                c;

#endif
{
   AttributeMapping*   tableEntry;
   AttributeType       attrType;
   ElementType         elType;
   Element             child;
   Attribute           attr;
   SSchema	       schema;
   char                translation;
   char                msgBuffer[MaxBufferLength];

   CloseBuffer ();
   /* inputBuffer contains the attribute name */
   /* get the corresponding Thot attribute */
   if (UnknownTag)
      /* ignore attributes of unknown tags */
      tableEntry = NULL;
   else
      tableEntry = MapAttr (inputBuffer, &schema);
   if (tableEntry == NULL)
      /* the attribute is not in the table */
     {
	sprintf (msgBuffer, "Unknown attribute \"%s\"", inputBuffer);
	ParseHTMLError (theDocument, msgBuffer);
	/* attach an Invalid_attribute to the current element */
	tableEntry = &HTMLAttributeMappingTable[0];
	schema = HTMLSSchema;
	IgnoreAttr = TRUE;
     }
   else
      IgnoreAttr = FALSE;
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
				tableEntry == &HTMLAttributeMappingTable[0]);
		    ReadingHREF = (attrType.AttrTypeNum == HTML_ATTR_HREF_);
		    if (ReadingHREF)
		      {
			 elType = TtaGetElementType (lastElement);
			 if (elType.ElTypeNum == HTML_EL_Anchor)
			    /* attribute HREF for element Anchor */
			    /* create attribute PseudoClass = link */
			   {
			      attrType.AttrTypeNum = HTML_ATTR_PseudoClass;
			      attr = TtaNewAttribute (attrType);
			      TtaAttachAttribute (lastElement, attr, theDocument);
			      TtaSetAttributeText (attr, "link", lastElement, theDocument);
			   }
		      }
		    else if (attrType.AttrTypeNum == HTML_ATTR_Checked)
		      {
			 /* create Default-Checked attribute */
			 child = TtaGetFirstChild (lastElement);
			 if (child != NULL)
			   {
			      attrType.AttrSSchema = HTMLSSchema;
			      attrType.AttrTypeNum = HTML_ATTR_DefaultChecked;
			      attr = TtaNewAttribute (attrType);
			      TtaAttachAttribute (child, attr, theDocument);
			      TtaSetAttributeValue (attr, HTML_ATTR_DefaultChecked_VAL_Yes_, child, theDocument);
			   }
		      }
		    else if (attrType.AttrTypeNum == HTML_ATTR_Selected)
		      {
			 /* create Default-Selected attribute */
			 attrType.AttrSSchema = HTMLSSchema;
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
   EndOfAttrNameAndTag     A ">" or a "/" (XML) has been read. It indicates the
   end of an attribute name and the end of a start tag.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfAttrNameAndTag (char c)
#else
static void         EndOfAttrNameAndTag (c)
char                c;

#endif
{
   EndOfAttrName (c);
   EndOfStartTag (c);
}

/*----------------------------------------------------------------------
   StartOfAttrValue        A quote (or double quote) starting an
   attribute value has been read.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         StartOfAttrValue (char c)
#else
static void         StartOfAttrValue (c)
char                c;

#endif
{
   if (IgnoreAttr)
      /* this is the value of an unknown attribute. keep the quote */
      /* in the input buffer for copying it in the current */
      /* Invalid_attribute */
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
void                CreateAttrWidthPercentPxl (char *buffer, Element el, Document doc, int oldWidth)
#else
void                CreateAttrWidthPercentPxl (buffer, el, doc, oldWidth)
char               *buffer;
Element             el;
Document            doc;
int                 oldWidth;
#endif
{
  AttributeType      attrTypePxl, attrTypePercent;
  Attribute          attrOld, attrNew;
  int                length, val;
#ifndef STANDALONE
  ElementType	     elType;
  int                w, h;
  boolean            isImage;

  elType = TtaGetElementType (el);
  isImage = (elType.ElTypeNum == HTML_EL_PICTURE_UNIT);
#endif

  /* is the last character a '%' ? */
  length = strlen (buffer) - 1;
  while (length > 0 && buffer[length] <= SPACE)
    length--;
  attrTypePxl.AttrSSchema = TtaGetDocumentSSchema (doc);
  attrTypePercent.AttrSSchema = TtaGetDocumentSSchema (doc);
  attrTypePxl.AttrTypeNum = HTML_ATTR_IntWidthPxl;
  attrTypePercent.AttrTypeNum = HTML_ATTR_IntWidthPercent;
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
  sscanf (buffer, "%d", &val);
  TtaSetAttributeValue (attrNew, val, el, doc);
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
void                CreateAttrIntSize (char *buffer, Element el, Document doc)
#else
void                CreateAttrIntSize (buffer, el, doc)
char               *buffer;
Element             el;
Document            doc;

#endif
{
   AttributeType       attrType;
   int                 val, ind, factor, delta;
   Attribute           attr;

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
   if (attr == NULL)
     {
       /* this attribute doesn't exist, create it */
       attr = TtaNewAttribute (attrType);
       TtaAttachAttribute (el, attr, doc);
     }
   sscanf (&buffer[ind], "%d", &val);
   val = val * factor + delta;
   TtaSetAttributeValue (attr, val, el, doc);
}

/*----------------------------------------------------------------------
   EndOfAttrValue
   An attribute value has been read from the HTML file.
   Put that value in the current Thot attribute.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfAttrValue (char c)
#else
static void         EndOfAttrValue (c)
char                c;

#endif
{
   AttributeType       attrType;
   Element             child;
   Language	       lang;
   char                translation;
   char                shape;
   char                msgBuffer[MaxBufferLength];
   char               *buffer;
   char               *attrName;
   int                 val;
   int                 length;
   int                 attrKind;
   boolean             done;

   if (IgnoreAttr)
      /* this is the end of value of an invalid attribute. Keep the */
      /* quote character that ends the value for copying it into the */
      /* Invalid_attribute. */
      if (c == '\'' || c == '\"')
	 PutInBuffer (c);
   CloseBuffer ();
   /* inputBuffer contains the attribute value */
   done = FALSE;
   if (lastElementClosed && (lastElement == rootElement))
      /* an attribute after the tag </html>, ignore it */
      done = TRUE;
   /* treatments of some particular HTML attributes */
   else if (!strcmp (lastAttrEntry->htmlAttribute, "STYLE"))
     {
#ifndef STANDALONE
	TtaSetAttributeText (lastAttribute, inputBuffer, lastAttrElement,
			     theDocument);
	ParseHTMLSpecificStyle (lastElement, (char *) inputBuffer, theDocument);
#endif
	done = TRUE;
     }
   else if (!strcmp (lastAttrEntry->htmlAttribute, "CLASS"))
     {
#ifndef STANDALONE
	TtaSetAttributeText (lastAttribute, inputBuffer, lastAttrElement,
			     theDocument);
	ParseHTMLClass (lastElement, (char *) inputBuffer, theDocument);
#endif
	done = TRUE;
     }
#ifndef STANDALONE
   else if (!strcmp (lastAttrEntry->htmlAttribute, "LINK"))
      HTMLSetAlinkColor (theDocument, (char *) inputBuffer);
   else if (!strcmp (lastAttrEntry->htmlAttribute, "ALINK"))
      HTMLSetAactiveColor (theDocument, (char *) inputBuffer);
   else if (!strcmp (lastAttrEntry->htmlAttribute, "VLINK"))
      HTMLSetAvisitedColor (theDocument, (char *) inputBuffer);
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
		       TtaAppendTextContent (child, "\" ", theDocument);
		    break;
		 case 'A':
		    if (lastAttribute != NULL)
		      {
			 TtaGiveAttributeType (lastAttribute, &attrType, &attrKind);
			 switch (attrKind)
			       {
				  case 0:	/* enumerate */
				     val = MapAttrValue (lastAttrEntry->ThotAttribute, inputBuffer);
				     if (val < 0)
				       {
					  TtaGiveAttributeType (lastAttribute, &attrType, &attrKind);
					  attrName = TtaGetAttributeName (attrType);
					  sprintf (msgBuffer, "Unknown attribute value \"%s = %s\"",
						   attrName, inputBuffer);
					  ParseHTMLError (theDocument, msgBuffer);
					  /* remove the attribute and replace it by an */
					  /* Invalid_attribute */
					  TtaRemoveAttribute (lastAttrElement, lastAttribute,
							      theDocument);
					  attrType.AttrSSchema = HTMLSSchema;
					  attrType.AttrTypeNum = HTMLAttributeMappingTable[0].ThotAttribute;
					  sprintf (msgBuffer, "%s=%s", attrName, inputBuffer);
					  CreateAttr (lastAttrElement, attrType, msgBuffer, TRUE);
				       }
				     else
					  TtaSetAttributeValue (lastAttribute,
					    val, lastAttrElement, theDocument);
				     break;
				  case 1:	/* integer */
				    if (attrType.AttrTypeNum == HTML_ATTR_Border &&
					!strcasecmp (inputBuffer, "border") )
					/* BORDER="BORDER" for a TABLE */
					val = 1;
				    else
				        sscanf (inputBuffer, "%d", &val);
				     TtaSetAttributeValue (lastAttribute, val, lastAttrElement,
							   theDocument);
				     break;
				  case 2:	/* text */
				     if (!IgnoreAttr)
					{
					TtaSetAttributeText (lastAttribute, inputBuffer,
					      lastAttrElement, theDocument);
					if (attrType.AttrTypeNum == HTML_ATTR_Langue)
					   /* it's a LANG attribute value */
					   {
					   lang = TtaGetLanguageIdFromName (inputBuffer);
					   if (lang == 0)
					      {
					      sprintf (msgBuffer, "Unknown language: %s", inputBuffer);
					      ParseHTMLError (theDocument, msgBuffer);
					      }
					   else
					      {
					      /* change current language */
					      currentLanguage = lang;
					      LanguageStack[StackLevel - 1] = currentLanguage;
					      }
					   }
					}
				     else
					/* this is the content of an invalid attribute */
					/* append it to the current Invalid_attribute */
				       {
					  length = strlen (inputBuffer) + 2;
					  length += TtaGetTextAttributeLength (lastAttribute);
					  buffer = TtaGetMemory (length + 1);
					  TtaGiveTextAttributeValue (lastAttribute, buffer, &length);
					  strcat (buffer, "=");
					  strcat (buffer, inputBuffer);
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

	else if (!strcmp (lastAttrEntry->htmlAttribute, "SIZE"))
	  {
	     TtaGiveAttributeType (lastAttribute, &attrType, &attrKind);
	     if (attrType.AttrTypeNum == HTML_ATTR_Font_size)
		CreateAttrIntSize (inputBuffer, lastAttrElement, theDocument);
	  }
	else if (!strcmp (lastAttrEntry->htmlAttribute, "SHAPE"))
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
			      shape = 'c';
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
#ifndef STANDALONE
	/* Some HTML attributes are equivalent to a CSS property:      */
	/*      background     ->                   background         */
	/*      bgcolor        ->                   background         */
	/*      text           ->                   color              */
	/*      color          ->                   color              */
	else if (!strcmp (lastAttrEntry->htmlAttribute, "BACKGROUND"))
	  {
	     sprintf (msgBuffer, "background: url(%s)", inputBuffer);
	     ParseHTMLSpecificStyle (lastElement, msgBuffer, theDocument);
	  }
	else if (!strcmp (lastAttrEntry->htmlAttribute, "BGCOLOR"))
	   HTMLSetBackgroundColor (theDocument, lastElement, inputBuffer);
	else if (!strcmp (lastAttrEntry->htmlAttribute, "TEXT") ||
		 !strcmp (lastAttrEntry->htmlAttribute, "COLOR"))
	   HTMLSetForegroundColor (theDocument, lastElement, inputBuffer);
#endif /* !STANDALONE */
     }
   InitBuffer ();
}

/*----------------------------------------------------------------------
   EndOfAttrValueAndTag    A ">" or "/" (XML) has been read. It indicates the
   end of an attribute value and the end of a start tag.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfAttrValueAndTag (char c)
#else
static void         EndOfAttrValueAndTag (c)
char                c;

#endif
{
   EndOfAttrValue (c);
   EndOfStartTag (c);
}


#ifdef MATHML

/*----------------------------------------------------------------------
   PutMathChar
   Put a Math character in the document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutMathChar (unsigned char c, char alphabet)
#else
static void         PutMathChar (c, alphabet)
unsigned char       c;
char		    alphabet
#endif
{
   ElementType	 elType;
   Element	 elText;
   AttributeType attrType;
   Attribute	 attr;
   Language	 lang;
   char		 buffer[MaxEntityLength+2];

   if (alphabet == 'L')
      PutInBuffer (c);
   else
      {
      TextToMath ('L');
      PutInBuffer (c);
      TextToMath (alphabet);
      }
   if (c == '\0' || c == ' ' ||
       ((int)c) == 129 ||	/* thin space */
       ((int)c) == 130 ||	/* en space */
       ((int)c) == 160)		/* sticky space */
      /* null character or space */
      if (LgEntityName > 0)
        /* this character comes from an entity */
        /* create a text element containing the entity name with an
           attribute entity */
	{
	if (alphabet == 'L')
	   TextToMath ('L');
	buffer[0] = '&';
	strncpy (&buffer[1], EntityName, LgEntityName);
	buffer[LgEntityName+1] = ';';
	buffer[LgEntityName+2] = '\0';
	elType.ElTypeNum = MathML_EL_TEXT_UNIT;
	elType.ElSSchema = MathMLSSchema;
	elText = TtaNewElement (theDocument, elType);
	InsertMathElement (&elText);
	lastElementClosed = TRUE;
	MergeText = FALSE;
	lang = TtaGetLanguageIdFromAlphabet('L');
	TtaSetTextContent (elText, buffer, lang, theDocument);
	attrType.AttrSSchema = MathMLSSchema;
	attrType.AttrTypeNum = MathML_ATTR_entity;
        attr = TtaNewAttribute (attrType);
        TtaAttachAttribute (elText, attr, theDocument);
	TtaSetAttributeValue (attr, MathML_ATTR_entity_VAL_yes_, elText,
			      theDocument);
	}
}

/*----------------------------------------------------------------------
   EndOfMathEntity     End of a Math entity. Search that entity in the
   entity table and put the corresponding character in the input buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfMathEntity (char c)
#else
static void         EndOfMathEntity (c)
char                c;

#endif
{
   int                 i;
   char                msgBuffer[MaxBufferLength];

   EntityName[LgEntityName] = EOS;
   if (MathEntityTable[EntityTableEntry].MentityName[CharRank] == EOS)
      /* the entity read matches the current entry of entity table */
        PutMathChar ((unsigned char) (MathEntityTable[EntityTableEntry].charCode),
		     MathEntityTable[EntityTableEntry].alphabet);
   else
      /* entity not in the table. Print an error message */
     {
	PutInBuffer ('&');
	for (i = 0; i < LgEntityName; i++)
	   PutInBuffer (EntityName[i]);
	PutInBuffer (';');
	/* print an error message */
	sprintf (msgBuffer, "Invalid entity \"&%s;\"", EntityName);
	ParseHTMLError (theDocument, msgBuffer);
     }
   LgEntityName = 0;
   ReadingHTMLentity = FALSE;
}

/*----------------------------------------------------------------------
   MathEntityChar      A character belonging to a Math entity has been
   read.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         MathEntityChar (unsigned char c)
#else
static void         MathEntityChar (c)
unsigned char       c;

#endif
{
   int                 i;
   char               msgBuffer[MaxBufferLength];
   boolean	       OK;

   if (MathEntityTable[EntityTableEntry].MentityName[CharRank] == EOS)
      /* the entity name read so far matches the current entry of */
      /* entity table */
     {
	/* assume that semicolon is missing and put the corresponding char */
	EntityName[LgEntityName] = EOS;
	PutMathChar ((unsigned char) (MathEntityTable[EntityTableEntry].charCode),
		     MathEntityTable[EntityTableEntry].alphabet);
	if (c != SPACE)
	   /* print an error message */
	   ParseHTMLError (theDocument, "Missing semicolon");
	/* next state is the return state from the entity subautomaton, not
	   the state computed by the automaton. In addition the character read
	   has not been processed yet */
	NormalTransition = FALSE;
	currentState = returnState;
	/* end of entity */
	LgEntityName = 0;
     }
   else
     {
	while (MathEntityTable[EntityTableEntry].MentityName[CharRank] < c
	       && MathEntityTable[EntityTableEntry].charCode >= 0)
	   EntityTableEntry++;
	if (MathEntityTable[EntityTableEntry].MentityName[CharRank] != c)
	  OK = FALSE;
	else
	  if (LgEntityName > 0 &&
	      strncmp (EntityName, MathEntityTable[EntityTableEntry].MentityName,
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
	  /* the entity name read so far is not in the table */
	  {
	  /* is this entity in the HTML entities table? */
	  EntityTableEntry = 0;
	  CharRank = 0;
	  while (CharRank < LgEntityName &&
		 ISOlat1table[EntityTableEntry].charCode != 0)
	    {
	    while (ISOlat1table[EntityTableEntry].charName[CharRank] <
					EntityName[CharRank] &&
		   ISOlat1table[EntityTableEntry].charCode != 0)
	       EntityTableEntry++;
	    CharRank++;
	    }
	  if (ISOlat1table[EntityTableEntry].charCode != 0)
	    {
	    while (ISOlat1table[EntityTableEntry].charName[CharRank] < c &&
		   ISOlat1table[EntityTableEntry].charCode != 0)
	       EntityTableEntry++;
	    if (ISOlat1table[EntityTableEntry].charName[CharRank] == c)
	       if (LgEntityName == 0 ||
	          strncmp (EntityName, ISOlat1table[EntityTableEntry].charName,
		         LgEntityName) == 0)
	         {
	         OK = TRUE;
	         CharRank++;
	         if (LgEntityName < MaxEntityLength - 1)
		    EntityName[LgEntityName++] = c;
	         ReadingHTMLentity = TRUE;
	         }
	     }
	  if (!OK)
	     {
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
	        sprintf (msgBuffer, "Invalid entity \"&%s\"", EntityName);
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
}
#endif  /* MATHML */

/*----------------------------------------------------------------------
   StartOfEntity   A character '&' has been encountered in text.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         StartOfEntity (char c)
#else
static void         StartOfEntity (c)
char                c;

#endif
{
   LgEntityName = 0;
   EntityTableEntry = 0;
   CharRank = 0;
#ifdef MATHML
   ReadingHTMLentity = FALSE;
#endif
}

/*----------------------------------------------------------------------
   EndOfEntity     End of a HTML entity. Search that entity in the
   entity table and put the corresponding character in the input buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfEntity (char c)
#else
static void         EndOfEntity (c)
char                c;

#endif
{
   int                 i;
   char                msgBuffer[MaxBufferLength];

#ifdef MATHML
   if (WithinMathML && !ReadingHTMLentity)
      {
      EndOfMathEntity (c);
      return;
      }
#endif  /* MATHML */
   EntityName[LgEntityName] = EOS;
   if (ISOlat1table[EntityTableEntry].charName[CharRank] == EOS)
      /* the entity read matches the current entry of entity table */
      PutInBuffer ((char) (ISOlat1table[EntityTableEntry].charCode));
   else
      /* entity not in the table. Print an error message */
     {
	PutInBuffer ('&');
	for (i = 0; i < LgEntityName; i++)
	   PutInBuffer (EntityName[i]);
	PutInBuffer (';');
	/* print an error message */
	sprintf (msgBuffer, "Invalid entity \"&%s;\"", EntityName);
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
   char                msgBuffer[MaxBufferLength];
   boolean	       OK;

#ifdef MATHML
   if (WithinMathML && !ReadingHTMLentity)
      {
      MathEntityChar (c);
      return;
      }
#endif  /* MATHML */
   if (ISOlat1table[EntityTableEntry].charName[CharRank] == EOS)
      /* the entity name read so far matches the current entry of */
      /* entity table */
     {
	/* assume that semicolon is missing and put the corresponding char */
	PutInBuffer ((char) (ISOlat1table[EntityTableEntry].charCode));
	if (c != SPACE)
	   /* print an error message */
	   ParseHTMLError (theDocument, "Missing semicolon");
	/* next state is the return state from the entity subautomaton, not
	   the state computed by the automaton. In addition the character read
	   has not been processed yet */
	NormalTransition = FALSE;
	currentState = returnState;
	/* end of entity */
	LgEntityName = 0;
     }
   else
     {
	while (ISOlat1table[EntityTableEntry].charName[CharRank] < c
	       && ISOlat1table[EntityTableEntry].charCode != 0)
	   EntityTableEntry++;
	if (ISOlat1table[EntityTableEntry].charName[CharRank] != c)
	  OK = FALSE;
	else
	  if (LgEntityName > 0 &&
	      strncmp (EntityName, ISOlat1table[EntityTableEntry].charName,
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
	        sprintf (msgBuffer, "Invalid entity \"&%s\"", EntityName);
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
   EndOfNumEntity  End of a numerical entity. Convert the
   string read into a number and put the character
   having that code in the input buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfNumEntity (char c)
#else
static void         EndOfNumEntity (c)
char                c;

#endif
{
   int                 code;

   EntityName[LgEntityName] = EOS;
   sscanf (EntityName, "%d", &code);
   PutInBuffer ((char) code);
   LgEntityName = 0;
}

/*----------------------------------------------------------------------
   NumEntityChar   A character belonging to a HTML numerical
   entity has been read. Put that character in
   the entity buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         NumEntityChar (char c)
#else
static void         NumEntityChar (c)
char                c;

#endif
{
   if (LgEntityName < MaxEntityLength - 1)
      /* the entity buffer is not full */
      if (c >= '0' && c <= '9')
	 /* the character is a decimal digit */
	 EntityName[LgEntityName++] = c;
      else
	 /* not a decimal digit. assume end of entity */
	{
	   EndOfNumEntity (c);
	   /* next state is state 0, not the state computed by the automaton */
	   /* and the character read has not been processed yet */
	   NormalTransition = FALSE;
	   currentState = 0;
	   if (c != SPACE)
	      /* error message */
	      ParseHTMLError (theDocument, "Missing semicolon");
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
static void         PutLess (char c)
#else
static void         PutLess (c)
char                c;

#endif
{
   PutInBuffer ('<');
}

/*----------------------------------------------------------------------
   PutAmpersandSpace       put '& ' in the input buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutAmpersandSpace (char c)
#else
static void         PutAmpersandSpace (c)
char                c;

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
   StartOfComment  Beginning of a HTML comment.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         StartOfComment (char c)
#else
static void         StartOfComment (c)
char                c;

#endif
{
   ElementType         elType;
   Element             elComment, elCommentLine, child, lastChild;

   /* create a Thot element Comment */
   elType.ElSSchema = HTMLSSchema;
   elType.ElTypeNum = HTML_EL_Comment_;
   elComment = TtaNewElement (theDocument, elType);
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
	TtaInsertFirstChild (&elCommentLine, elComment, theDocument);
	/* create a TEXT element as the first child of element Comment_line */
	elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	CommentText = TtaNewElement (theDocument, elType);
	TtaInsertFirstChild (&CommentText, elCommentLine, theDocument);
	TtaSetTextContent (CommentText, "", currentLanguage, theDocument);
     }
   InitBuffer ();
}

/*----------------------------------------------------------------------
   PutInComment    put character c in the current HTML comment.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutInComment (unsigned char c)
#else
static void         PutInComment (c)
unsigned char       c;

#endif
{
   ElementType         elType;
   Element             elCommentLine, prevElCommentLine;

   if (c != EOS)
      if ((int) c == 10 || (int) c == 13)
	 /* new line in a comment */
	{
	   /* put the content of the inputBuffer into the current */
	   /* Comment_line element */
	   CloseBuffer ();
	   TtaAppendTextContent (CommentText, inputBuffer, theDocument);
	   InitBuffer ();
	   /* create a new Comment_line element */
	   elType.ElSSchema = HTMLSSchema;
	   elType.ElTypeNum = HTML_EL_Comment_line;
	   elCommentLine = TtaNewElement (theDocument, elType);
	   /* inserts the new Comment_line element after the previous one */
	   prevElCommentLine = TtaGetParent (CommentText);
	   TtaInsertSibling (elCommentLine, prevElCommentLine, FALSE,
			     theDocument);
	   /* create a TEXT element as the first child of the new element
	      Comment_line */
	   elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	   CommentText = TtaNewElement (theDocument, elType);
	   TtaInsertFirstChild (&CommentText, elCommentLine, theDocument);
	   TtaSetTextContent (CommentText, "", currentLanguage, theDocument);
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
static void         EndOfComment (char c)
#else
static void         EndOfComment (c)
char                c;

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
static void         PutDash (char c)
#else
static void         PutDash (c)
char                c;

#endif
{
   PutInComment ('-');
   PutInComment (c);
}

/*----------------------------------------------------------------------
   PutDashDash     put 2 dash characters in the current comment.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutDashDash (char c)
#else
static void         PutDashDash (c)
char                c;

#endif
{
   PutInComment ('-');
   PutInComment ('-');
   PutInComment (c);
}

/*----------------------------------------------------------------------
   XMLerror	The character following '/' in a start tag is not '>'.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         XMLerror (char c)
#else
static void         XMLerror (c)
char                c;

#endif
{
   ParseHTMLError (theDocument, "Invalid XML syntax");
}

/*----------------------------------------------------------------------
   Do_nothing      Do nothing.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         Do_nothing (char c)
#else
static void         Do_nothing (c)
char                c;

#endif
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
#define MaxState 30
StateDescr          automaton[MaxState];

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
   {0, '&', (Proc) StartOfEntity, -20},		/* call subautomaton 20 */
   {0, '*', (Proc) PutInBuffer, 0},	/*  * = any other character */
/* state 1: '<' has been read */
   {1, '/', (Proc) Do_nothing, 3},
   {1, '!', (Proc) Do_nothing, 10},
   {1, '<', (Proc) Do_nothing, 18},
   {1, 'S', (Proc) PutLessAndSpace, 0},		/*   S = Space */
   {1, '*', (Proc) PutInBuffer, 2},
/* state 2: reading a start tag */
   {2, '/', (Proc) EndOfStartGIandTag, 23},		/* XML */
   {2, '>', (Proc) EndOfStartGIandTag, 0},
   {2, '&', (Proc) StartOfEntity, -20},		/* call subautomaton 20 */
   {2, 'S', (Proc) EndOfStartGI, 16},	/*   S = Space */
   {2, '*', (Proc) PutInBuffer, 2},
/* state 3: reading an end tag */
   {3, '>', (Proc) EndOfEndTag, 0},
   {3, '&', (Proc) StartOfEntity, -20},		/* call subautomaton 20 */
   {3, 'S', (Proc) Do_nothing, 3},
   {3, '*', (Proc) PutInBuffer, 3},
/* state 4: reading an attribute name */
   {4, '=', (Proc) EndOfAttrName, 5},
   {4, 'S', (Proc) EndOfAttrName, 17},
   {4, '&', (Proc) StartOfEntity, -20},		/* call subautomaton 20 */
   {4, '/', (Proc) EndOfAttrNameAndTag, 23},	/* XML */
   {4, '>', (Proc) EndOfAttrNameAndTag, 0},
   {4, '*', (Proc) PutInBuffer, 4},
/* state 5: begin of attribute value */
   {5, '\"', (Proc) StartOfAttrValue, 6},
   {5, '\'', (Proc) StartOfAttrValue, 9},
   {5, 'S', (Proc) Do_nothing, 5},
   {5, '>', (Proc) EndOfStartTag, 0},
   {5, '*', (Proc) PutInBuffer, 7},
/* state 6: reading an attribute value between double quotes */
   {6, '\"', (Proc) EndOfAttrValue, 8},
   {6, '&', (Proc) StartOfEntity, -20},		/* call subautomaton 20... */
   {6, '&', (Proc) PutInBuffer, 6},	/* ...except for HREF */
   {6, '*', (Proc) PutInBuffer, 6},
/* state 7: reading an attribute value without delimiting quotes */
   {7, '>', (Proc) EndOfAttrValueAndTag, 0},
   {7, 'S', (Proc) EndOfAttrValue, 16},
   {7, '&', (Proc) StartOfEntity, -20},		/* call subautomaton 20 */
   {7, '*', (Proc) PutInBuffer, 7},
/* state 8: end of attribute value */
   {8, '/', (Proc) EndOfStartTag, 23},		/* XML */
   {8, '>', (Proc) EndOfStartTag, 0},
   {8, 'S', (Proc) Do_nothing, 16},
   {8, '*', (Proc) PutInBuffer, 4},
/* state 9: reading an attribute value between simple quotes */
   {9, '\'', (Proc) EndOfAttrValue, 8},
   {9, '&', (Proc) StartOfEntity, -20},		/* call subautomaton 20 */
   {9, '*', (Proc) PutInBuffer, 9},
/* state 10: "<!" has been read */
   {10, '-', (Proc) Do_nothing, 11},
   {10, 'S', (Proc) Do_nothing, 10},
   {10, '*', (Proc) Do_nothing, 15},
/* state 11: "<!-" has been read. Probably a comment */
   {11, '-', (Proc) StartOfComment, 12},
   {11, '*', (Proc) Do_nothing, 15},	/* incorrect comment, expect */
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
/* state 15: reading the prologue "<!doctype HTML public..." */
   {15, '>', (Proc) Do_nothing, 0},
   {15, '*', (Proc) Do_nothing, 15},
/* state 16: expecting an attribute name or an end of start tag */
   {16, 'S', (Proc) Do_nothing, 16},
   {16, '/', (Proc) EndOfStartTag, 23},		/* XML */
   {16, '>', (Proc) EndOfStartTag, 0},
   {16, '*', (Proc) PutInBuffer, 4},
/* state 17: expecting '=' after an attribute name */
   {17, 'S', (Proc) Do_nothing, 17},
   {17, '=', (Proc) Do_nothing, 5},
   {17, '/', (Proc) EndOfStartTag, 23},		/* XML */
   {17, '>', (Proc) EndOfStartTag, 0},
   {17, '*', (Proc) PutInBuffer, 4},
/* state 18: '<' has been read */
   {18, '!', (Proc) Do_nothing, 19},
   {18, '*', (Proc) Do_nothing, 0},
/* state 19: '<!' has been read */
   {19, '>', (Proc) PutLess, 0},
   {19, '*', (Proc) Do_nothing, 0},

/* sub automaton for reading entities in various contexts */
/* state -1 means "return to calling state" */
/* state 20: a '&' has been read */
   {20, '#', (Proc) Do_nothing, 22},
   {20, 'S', (Proc) PutAmpersandSpace, -1},	/* return to calling state */
   {20, '*', (Proc) EntityChar, 21},
/* state 21: reading an name entity */
   {21, ';', (Proc) EndOfEntity, -1},	/* return to calling state */
   {21, '*', (Proc) EntityChar, 21},
/* state 22: reading a numerical entity */
   {22, ';', (Proc) EndOfNumEntity, -1},	/* return to calling state */
   {22, '*', (Proc) NumEntityChar, 22},

/* state 23: a '/' has been read within a start tag. Expect a '>' which */
/* indicates the end of the start tag for an empty element (XML) */
   {23, '>', (Proc) Do_nothing, 0},		/* XML */
   {23, '*', (Proc) XMLerror, 24},		/* XML */
/* state 24: ignore all characters until the next '>' */
   {24, '>', (Proc) Do_nothing, 0},		/* XML */
   {24, '*', (Proc) Do_nothing, 24},		/* XML */

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
   for (entry = 0; HTMLGIMappingTable[entry].htmlGI[0] != EOS; entry++)
      {
      pClose = HTMLGIMappingTable[entry].firstClosedElem;
      while (pClose != NULL)
	 {
	 nextClose = pClose->nextClosedElem;
	 TtaFreeMemory (pClose);
	 pClose = nextClose;
	 }
      }
}

/*----------------------------------------------------------------------
   GetNextInputChar        returns the next character in the imput
   file or buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char                GetNextInputChar (void)
#else
char                GetNextInputChar ()
#endif
{
   char                charRead;

   charRead = EOS;
   if (prevChar != EOS)
      {
      charRead = prevChar;
      prevChar = EOS;
      }
   else
      if (InputFile != NULL)
         charRead = getc (InputFile);
      else
         charRead = InputText[curChar++];
   if ((InputFile != NULL && !feof (InputFile) && !ferror (InputFile))
       || (InputText != NULL && charRead != EOS))
      {
      if ((int) charRead == 13)
	 /* CR has been read */
         {
	   /* Read next character */
	   if (InputFile != NULL)
	      charRead = getc (InputFile);
	   else
	      charRead = InputText[curChar++];
	   if ((int) charRead != 10)
	      /* next character is not LF. Store next character
		 and return LF */
	      {
	      prevChar = charRead;
	      charRead = (char) 10;
	      }
	 }
      /* update the counters of characters and lines read */
      if ((int) charRead == 10 || (int) charRead == 13)
	 /* new line in HTML file */
	{
	   numberOfLinesRead++;
	   numberOfCharRead = 0;
	}
      else
	 numberOfCharRead++;
      }
   return charRead;
}

/*----------------------------------------------------------------------
   HTMLread       parse either the HTML file infile or the text
   buffer HTMLbuf and build the equivalent Thot
   abstract tree.
   One parameter should be NULL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         HTMLread (FILE * infile, char *HTMLbuf)
#else
static void         HTMLread (infile, HTMLbuf)
FILE               *infile;
char               *HTMLbuf;

#endif
{
   unsigned char       charRead;
   boolean             match;
   PtrTransition       trans;
   boolean             endBuffer;

   if (infile != NULL)
     {
       currentState = 0;
       numberOfCharRead = 0;
       numberOfLinesRead = 1;
     }

   InputText = HTMLbuf;
   InputFile = infile;
   endBuffer = FALSE;
   if (ParsingCSS)
     charRead = SPACE;
   else
     charRead = EOS;
   /* read the HTML file sequentially */
   do
     {
	/* read one character from the source if the last character */
	/* read has been processed */
	if (charRead == EOS)
	  {
	     charRead = GetNextInputChar ();
	     if (infile == NULL)
		endBuffer = (charRead == EOS);
	  }
	if (charRead != EOS)
	  {
	     /* Check the character read */
	     /* Consider LF and FF as the end of an input line. */
	     /* Replace end of line by space, except in preformatted text. */
	     /* Replace HT by space, except in preformatted text. */
	     /* Ignore spaces at the beginning and at the end of input lines */
	     /* Ignore non printable characters except HT, LF, FF. */
	     if ((int) charRead == 10)
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
			    !strcmp (lastAttrEntry->htmlAttribute, "SRC"))
			   /* value of an SRC attribute */
			   /* consider new line as an empty char*/
			   charRead = EOS;
		      if (charRead != EOS)
		         /* Replace new line by a space, except if an entity is
			    being read */
			 if (currentState == 20 &&
			     Within (HTML_EL_Preformatted, HTMLSSchema) &&
                             !Within (HTML_EL_Option_Menu, HTMLSSchema))
			   charRead = (unsigned char) 138; /* Thot new line */
			 else
		           charRead = SPACE;
		      }
		   else
		      /* new line in a text element */
		      if (Within (HTML_EL_Preformatted, HTMLSSchema) &&
			  !Within (HTML_EL_Option_Menu, HTMLSSchema))
			/* within preformatted text */
			if (AfterTagPRE)
			   /* ignore NL after a <PRE> tag */
			   charRead = '\0';
			else
			   /* generate a Thot new line character */
			   charRead = (unsigned char) 138;
		      else
			/* new line in ordinary text */
		        {
			  /* suppress all spaces preceding the end of line */
			  while (LgBuffer > 0 && inputBuffer[LgBuffer - 1] == SPACE)
			     LgBuffer--;
			  /* new line is equivalent to space */
			  charRead = SPACE;
			  if (LgBuffer > 0)
			     TextToDocument ();
		        }
		/* beginning of a new input line */
		EmptyLine = TRUE;
	       }
	     else if (!ParsingCSS)
		/* it's not an end of line */
	       {
		  if ((int) charRead == 9)
		     /* HT = Horizontal tabulation */
		    {
		       if (currentState != 0)
			  /* not in a text element. Replace HT by space */
			  charRead = SPACE;
		       else
			  /* in a text element. Replace HT by space except in */
			  /* preformatted text */
		          if (!Within (HTML_EL_Preformatted, HTMLSSchema))
			     charRead = SPACE;
		    }
		  if (charRead == SPACE)
		     /* space character */
		    {
		       if (currentState == 0)
			  /* in a text element */
			  if (!Within (HTML_EL_Preformatted, HTMLSSchema))
			     /* not in preformatted text */
			     /* ignore spaces at the beginning of an input line */
			     if (EmptyLine)
				charRead = EOS;
		    }
		  else if ((charRead < SPACE || (int) charRead >= 254 ||
			    ((int) charRead >= 127 && (int) charRead <= 159))
			   && (int) charRead != 9 && (int) charRead != 138)
		     /* it's not a printable character, ignore it */
		     charRead = EOS;
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
		  while (trans != NULL)
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
			  if ((int) charRead == 9 || (int) charRead == 10 ||
			      (int) charRead == 12)
			     /* a delimiter has been read */
			     match = TRUE;
		       if (match || ParsingCSS)
			  /* transition found. Activate the transition */
			 {
			    /* call the procedure associated with the transition */
			    NormalTransition = !ParsingCSS;
			    if (NormalTransition && trans->action != NULL)
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
			    if (ParsingCSS)
			      {
#ifndef STANDALONE
				 charRead = CSSparser (theDocument, &ParsingCSS);
				 /* when returning from the CSS parser, a '<' has been
				    read by the CSS parser and the following character,
				    which is in charRead */
#endif /* !STANDALONE */
				 if (!ParsingCSS)
				   currentState = 1;
			      }
			 }
		       else
			  /* access next transition from the same state */
			 {
			    trans = trans->nextTransition;
			    /* an exception: when reading the value of an HREF attribute,
			       SGML entities (&xxx;) should not be interpreted */
			    if (trans != NULL)
			       if (currentState == 6 && trans->trigger == '&' && ReadingHREF)
			          trans = trans->nextTransition;
			    if (trans == NULL)
			       charRead = EOS;
			 }
		    }
	       }
	  }
     }
   while ((infile != NULL && !feof (infile) && !ferror (infile)) ||
	  (HTMLbuf != NULL && !endBuffer));
   if (infile != NULL)
     /* end of HTML file */
     EndOfDocument ();
}


/*----------------------------------------------------------------------
   HTMLparse       parse either the HTML file infile or the text
   buffer HTMLbuf and build the equivalent Thot
   abstract tree.
   One parameter should be NULL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         HTMLparse (FILE * infile, char *HTMLbuf)
#else
void         HTMLparse (infile, HTMLbuf)
FILE               *infile;
char               *HTMLbuf;

#endif
{
  currentState = 0;
  numberOfCharRead = 0;
  numberOfLinesRead = 1;
  HTMLread (infile, HTMLbuf);
  if (infile == NULL)
    /* end of HTML file */
    EndOfDocument ();
}

/*----------------------------------------------------------------------
   ReadTextFile
   read plain text file into a PRE element.
   input text comes from either the infile file or the text
   buffer textbuf. One parameter should be NULL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ReadTextFile (FILE * infile, char *textbuf, boolean beginning)
#else
static void         ReadTextFile (infile, textbuf, beginning)
FILE               *infile;
char		   *textbuf;
boolean             beginning;
#endif
{
   unsigned char       charRead;
   boolean             endBuffer;

   if (infile != NULL)
     {
       numberOfCharRead = 0;
       numberOfLinesRead = 1;
     }

   InputText = textbuf;
   InputFile = infile;
   endBuffer = FALSE;
   if (beginning)
     {
       /* create a PRE element by simulating a PRE tag */
       StartOfTag (SPACE);
       ProcessStartGI ("PRE");
     }

   /* read the text file sequentially */
   do
     {
	/* read one character from the source */
	charRead = GetNextInputChar ();
	if (infile == NULL)
	   endBuffer = (charRead == EOS);
	if (charRead != EOS)
	  {
	     /* Check the character read */
	     /* Consider LF and FF as the end of an input line. */
	     if ((int) charRead == 10)
		/* LF = end of line */
		/* generate a Thot new line character */
		charRead = (unsigned char) 138;
	     else
		/* it's not an end of line */
	        /* Ignore non printable characters except HT, LF, FF. */
		if ((charRead < SPACE || (int) charRead >= 254 ||
		    	((int) charRead >= 127 && (int) charRead <= 159))
			   && (int) charRead != 9 && (int) charRead != 138)
		     /* it's not a printable character, ignore it */
		     charRead = EOS;
	     if (charRead != EOS)
		/* a valid character has been read */
		PutInBuffer (charRead);
	  }
     }
   while ((infile != NULL && !feof (infile) && !ferror (infile)) ||
	  (textbuf != NULL && !endBuffer));
   if (infile != NULL)
     /* end of HTML file */
     EndOfDocument ();
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
	       elType.ElTypeNum == HTML_EL_Styles ||
	       elType.ElTypeNum == HTML_EL_Scripts ||
	       elType.ElTypeNum == HTML_EL_SCRIPT ||
	       elType.ElTypeNum == HTML_EL_Metas ||
	       elType.ElTypeNum == HTML_EL_META ||
	       elType.ElTypeNum == HTML_EL_Links ||
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
	    }
	  /* move the element as the last child of the HEAD element */
	  TtaRemoveTree (el, doc);
	  if (lastChild == NULL)
	    TtaInsertFirstChild (&el, *elHead, doc);
	  else
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
   Element          child, next, copy, prev, elem;

   if (IsEmptyElement (el))
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
void                CheckAbstractTree (char *pathURL)
#else
void                CheckAbstractTree (pathURL)
char               *pathURL;

#endif
{
   ElementType         elType, newElType, headElType;
   Element             el, elHead, elBody, nextEl, newEl, prevEl, lastChild,
                       firstTerm, lastTerm, termList, child, parent, firstEntry,
                       lastEntry, glossary, list, elText, previous, elLinks,
                       lastLink, elMetas, lastMeta, elScripts, lastScript;
   boolean             ok, moved;

   /* the root element only accepts elements HEAD, BODY and Comment as */
   /* children */
   elHead = NULL;
   elBody = NULL;
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
	     newElType.ElSSchema = HTMLSSchema;
	     newElType.ElTypeNum = HTML_EL_HEAD;
	     elHead = TtaNewTree (theDocument, newElType, "");
	     TtaInsertFirstChild (&elHead, rootElement, theDocument);
	  }

	if (elHead != NULL)
	  {
	     headElType = TtaGetElementType (elHead);
	     /* create a Document_URL element as the first child of HEAD */
	     newElType.ElSSchema = HTMLSSchema;
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
	     /* move all Link elements as children of a Links element, */
	     /* all Meta elements as children of a Metas element, */
	     /* and all Script elements as children of a Scripts element */
	     TtaNextSibling (&el);
	     elLinks = NULL;
	     lastLink = NULL;
	     elMetas = NULL;
	     lastMeta = NULL;
	     elScripts = NULL;
	     lastScript = NULL;
	     lastChild = NULL;
	     while (el != NULL)
	       {
		  nextEl = el;
		  TtaNextSibling (&nextEl);
		  elType = TtaGetElementType (el);
		  if (elType.ElTypeNum == HTML_EL_LINK)
		    {
		       /* create the Links element if it does not exist */
		       if (elLinks == NULL)
			 {
			    newElType.ElSSchema = HTMLSSchema;
			    newElType.ElTypeNum = HTML_EL_Links;
			    elLinks = TtaNewElement (theDocument, newElType);
			    TtaInsertSibling (elLinks, el, TRUE, theDocument);
			 }
		       /* move the element as the last child of the Links element */
		       TtaRemoveTree (el, theDocument);
		       if (lastLink == NULL)
			  TtaInsertFirstChild (&el, elLinks, theDocument);
		       else
			  TtaInsertSibling (el, lastLink, FALSE, theDocument);
		       lastLink = el;
		    }
		  else if (elType.ElTypeNum == HTML_EL_META)
		    {
		       /* create the Metas element if it does not exist */
		       if (elMetas == NULL)
			 {
			    newElType.ElSSchema = HTMLSSchema;
			    newElType.ElTypeNum = HTML_EL_Metas;
			    elMetas = TtaNewElement (theDocument, newElType);
			    TtaInsertSibling (elMetas, el, TRUE, theDocument);
			 }
		       /* move the element as the last child of the Metas element */
		       TtaRemoveTree (el, theDocument);
		       if (lastMeta == NULL)
			  TtaInsertFirstChild (&el, elMetas, theDocument);
		       else
			  TtaInsertSibling (el, lastMeta, FALSE, theDocument);
		       lastMeta = el;
		    }
		  else if (elType.ElTypeNum == HTML_EL_SCRIPT)
		    {
		       /* create the Scripts element if it does not exist */
		       if (elScripts == NULL)
			 {
			    newElType.ElSSchema = HTMLSSchema;
			    newElType.ElTypeNum = HTML_EL_Scripts;
			    elScripts = TtaNewElement (theDocument, newElType);
			    TtaInsertSibling (elScripts, el, TRUE, theDocument);
			 }
		       /* move the element as the last child of the Scripts element */
		       TtaRemoveTree (el, theDocument);
		       if (lastScript == NULL)
			  TtaInsertFirstChild (&el, elScripts, theDocument);
		       else
			  TtaInsertSibling (el, lastScript, FALSE, theDocument);
		       lastScript = el;
		    }
		  else
		    /* is this element allowed in the HEAD? */
		    {
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
		             newElType.ElSSchema = HTMLSSchema;
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
		    }
		  el = nextEl;
	       }
	  }
	/* check the children of the root, except HEAD */
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
	     else if (!moved && (elType.ElTypeNum == HTML_EL_Invalid_element ||
				 elType.ElTypeNum == HTML_EL_Comment_))
		/* don't move Comments and Invalid_elements if the previous
		   element has not been moved */
		previous = el;
	     else if (elType.ElTypeNum != HTML_EL_HEAD)
		/* this element should be a child of BODY */
	       {
		  /* create the BODY element if it does not exist */
		  if (elBody == NULL)
		    {
		       newElType.ElSSchema = HTMLSSchema;
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
			    newElType.ElSSchema = HTMLSSchema;
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
			    newElType.ElSSchema = HTMLSSchema;
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
				 newElType.ElSSchema = HTMLSSchema;
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
			    newElType.ElSSchema = HTMLSSchema;
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
			    /* create a Definition_List element before the first */
			    /* Definition_Item element */
			    newElType.ElSSchema = HTMLSSchema;
			    newElType.ElTypeNum = HTML_EL_Definition_List;
			    glossary = TtaNewElement (theDocument, newElType);
			    TtaInsertSibling (glossary, firstEntry, TRUE, theDocument);
			    /* move the Definition_Item elements as children of the */
			    /* new Definition_List element */
			    nextEl = firstEntry;
			    TtaNextSibling (&nextEl);
			    TtaRemoveTree (firstEntry, theDocument);
			    TtaInsertFirstChild (&firstEntry, glossary, theDocument);
			    if (lastEntry != firstEntry)
			      {
				 prevEl = firstEntry;
				 do
				   {
				      child = nextEl;
				      TtaNextSibling (&nextEl);
				      TtaRemoveTree (child, theDocument);
				      TtaInsertSibling (child, prevEl, FALSE, theDocument);
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
	     while (el != NULL)
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
			    /* create a HTML_EL_Unnumbered_List element before the */
			    /* first List_Item element */
			    newElType.ElSSchema = HTMLSSchema;
			    newElType.ElTypeNum = HTML_EL_Unnumbered_List;
			    list = TtaNewElement (theDocument, newElType);
			    TtaInsertSibling (list, firstEntry, TRUE, theDocument);
			    /* move the List_Item elements as children of the new */
			    /* HTML_EL_Unnumbered_List element */
			    nextEl = firstEntry;
			    TtaNextSibling (&nextEl);
			    TtaRemoveTree (firstEntry, theDocument);
			    TtaInsertFirstChild (&firstEntry, list, theDocument);
			    if (lastEntry != firstEntry)
			      {
				 prevEl = firstEntry;
				 do
				   {
				      child = nextEl;
				      TtaNextSibling (&nextEl);
				      TtaRemoveTree (child, theDocument);
				      TtaInsertSibling (child, prevEl, FALSE, theDocument);
				      prevEl = child;
				   }
				 while (nextEl != NULL && child != lastEntry);
			      }
			    /* starting element for the next search of a List_Item */
			    el = lastEntry;
			 }
		    }
	       }
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
	     newElType.ElSSchema = HTMLSSchema;
	     newElType.ElTypeNum = HTML_EL_Element;
	     newEl = TtaNewElement (theDocument, newElType);
	     TtaInsertFirstChild (&newEl, elBody, theDocument);
	     }

	/* add other checks here */
     }
}


/*----------------------------------------------------------------------
   InitializeParser
   initializes variables and stack for parsing file
   the parser will insert the thot tree after or as a child
   of last elem, in the document doc.
   If last elem is NULL or doc=0, the parser doesn't initialize
   the stack
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitializeParser (Element lastelem, boolean isclosed, Document doc)
#else  /* __STDC__ */
void                InitializeParser (lastelem, isclosed, doc)
Element             lastelem;
boolean             isclosed;
Document            doc;
#endif  /* __STDC__ */
{
   char                tag[20];
   Element             elem;
   ElementType	       elType;
   int                 i;
   SSchema	       schema;

   StackLevel = 1;
   currentLanguage = TtaGetDefaultLanguage ();
   if (lastelem != NULL && doc != 0)
     {
	/* initialize the stack with ancestors of lastelem */
	theDocument = doc;
	HTMLSSchema = TtaGetDocumentSSchema (theDocument);
	elType = TtaGetElementType (lastelem);
#   ifdef MATHML
	if (strcmp ("MathML", TtaGetSSchemaName (elType.ElSSchema)) == 0)
	  {
	    MathMLSSchema = elType.ElSSchema;
	    WithinMathML = TRUE;
	  } 
	else if (strcmp ("MATH", TtaGetElementTypeName (elType)) == 0 ||
		 strcmp ("MATHDISP", TtaGetElementTypeName (elType)) == 0 ||
		 strcmp ("Math", TtaGetElementTypeName (elType)) == 0 ||
		 strcmp ("MathDisp", TtaGetElementTypeName (elType)) == 0 )
	  {
	    MathMLSSchema =  TtaGetSSchema("MathML", theDocument);
	    WithinMathML = TRUE;
	    if (TtaGetFirstChild (lastelem) == NULL)
	      {
		elType.ElSSchema = MathMLSSchema;
		TtaGiveTypeFromName (&elType, "MathML");
		elem = TtaNewElement (theDocument, elType);
		TtaInsertFirstChild (&elem, lastelem, theDocument);
		lastelem = elem;
	      }
	  }
	else 
	  WithinMathML = FALSE;
#endif /* MATHML */
	rootElement = TtaGetMainRoot (theDocument);
	if (isclosed)
	   elem = TtaGetParent (lastelem);
	else
	   elem = lastelem;
	while (elem != NULL && elem != rootElement)
	  {
	     strcpy (tag, GITagNameByType (TtaGetElementType (elem)));
	     if (strcmp (tag, "???"))
	       {
		  for (i = StackLevel; i > 0; i--)
		    {
		       GINumberStack[i + 1] = GINumberStack[i];
		       ElementStack[i + 1] = ElementStack[i];
		       LanguageStack[i + 1] = LanguageStack[i];
		       ThotLevel[i + 1] = ThotLevel[i] + 1;
		    }
		  GINumberStack[1] = MapGI (tag, &schema);
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
   IgnoreAttr = FALSE;
   LgEntityName = 0;
   EntityTableEntry = 0;
   CharRank = 0;
   MergeText = FALSE;
   AfterTagPRE = FALSE;
   ParsingCSS = FALSE;
   curChar = 0;
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
   FILE               *infile;
   char                htmlFileName[200];
   char                pivotFileName[200];
   char                documentDirectory[200];
   char                documentName[200];
   Element             el, oldel;
   int                 returnCode;
   boolean	       PlainText;

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
	strcpy (htmlFileName, *argv);
	/*  open the input file */
	infile = fopen (htmlFileName, "r");
	if (infile == 0)
	   /* error */
	  {
	     fprintf (stderr, "Cannot open %s\n", htmlFileName);
	     returnCode = 2;
	  }
	else
	  {
	     /* input file OK. Get the output file name from the command line */
	     argv++;
	     strcpy (pivotFileName, *argv);
	     /* the file to be parsed is supposed to be HTML */
	     PlainText = FALSE;
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
		  strcpy (documentName, documentDirectory);
		  documentDirectory[0] = EOS;
	       }
	     TtaSetDocumentPath (documentDirectory);
	     docURL = &htmlFileName;
	     /* create a Thot document of type HTML */
	     theDocument = TtaNewDocument ("HTML", pivotFileName);
	     if (theDocument == 0)
	       {
		  fprintf (stderr, "Cannot create file %s.PIV\n", pivotFileName);
		  returnCode = 3;
	       }
	     else
		/* set the notification mode for the new document */
		TtaSetNotificationMode (theDocument, 1);

	     if (returnCode != 0)
	       {
		  /* problem occured */
	       }
#else  /* STANDALONE */
/*----------------------------------------------------------------------
   StartHTMLParser loads the file Directory/htmlFileName for
   displaying the document documentName.
   The parameter pathURL gives the original (local or
   distant) path or URL of the html document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                StartHTMLParser (Document doc, char *htmlFileName, char *documentName, char *documentDirectory, char *pathURL, boolean PlainText)
#else
void                StartHTMLParser (doc, htmlFileName, documentName, documentDirectory, pathURL, PlainText)
Document            doc;
char               *htmlFileName;
char               *documentName;
char               *documentDirectory;
char               *pathURL;
boolean	            PlainText;

#endif
{
   FILE               *infile;
   Element             el, oldel;
   char               *s;
   char                tempname[MAX_LENGTH];
   char                temppath[MAX_LENGTH];
   char               *buffer = NULL;
#ifdef HANDLE_COMPRESSED_FILES
   gzFile              stream = NULL;
   int                 bufsize = 2000;
   int                 res, c, diff, nbNul;
#endif
   int		       length;
   boolean             beginning = TRUE;

   theDocument = doc;
   FirstElemToBeChecked = NULL;
   LastElemToBeChecked = NULL;
   lastElement = NULL;
   lastElementClosed = FALSE;
   lastElemEntry = 0;
   lastAttribute = NULL;
   lastAttrElement = NULL;
   lastAttrEntry = NULL;
   IgnoreAttr = FALSE;
   CommentText = NULL;
   UnknownTag = FALSE;
   ReadingHREF = FALSE;
   MergeText = FALSE;
   LgEntityName = 0;
   EntityTableEntry = 0;
   CharRank = 0;
   infile = fopen (htmlFileName, "r");
   if (infile != 0)
     {
#ifdef HANDLE_COMPRESSED_FILES
       stream = gzopen (htmlFileName, "r");
       if (stream != NULL)
	 {
	   buffer = TtaGetMemory (bufsize + 1);
	   buffer[bufsize] = EOS;
	   if (buffer != NULL)
	     res = gzread (stream, buffer, bufsize);
	   if (res < 0)
	     {
	       TtaFreeMemory (buffer);
	       buffer = NULL;
	     }
	   else if (res == 0)
	       /* end of file */
	       buffer[0] = EOS;
	 }
#endif
	WithinTable = 0;
	if (documentName[0] == EOS && !TtaCheckDirectory (documentDirectory))
	  {
	     strcpy (documentName, documentDirectory);
	     documentDirectory[0] = EOS;
	     s = (char *) TtaGetEnvString ("PWD");
	     /* set path on current directory */
	     if (s != NULL)
		strcpy (documentDirectory, s);
	     else
		documentDirectory[0] = EOS;
	  }
	TtaAppendDocumentPath (documentDirectory);
	/* create a Thot document of type HTML */
#endif /* STANDALONE */
	/* the Thot document has been successfully created */
	{
#ifndef STANDALONE
	   length = strlen (pathURL);
	   if (strcmp (pathURL, htmlFileName) == 0)
	      {
	      docURL = TtaGetMemory (length+1);
	      strcpy (docURL, pathURL);
	      }
	   else
	      {
	      length += strlen (htmlFileName) + 20;
	      docURL = TtaGetMemory (length+1);
	      sprintf (docURL, "%s temp file: %s", pathURL, htmlFileName);
	      }
#endif /* STANDALONE */
	   /* do not allow the user to edit the document while parsing */
	   /**** TtaSetDocumentAccessMode(theDocument, 0);  ****/
	   /* do not check the Thot abstract tree against the structure */
	   /* schema while building the Thot document. */
	   TtaSetStructureChecking (0, theDocument);
	   /* set the notification mode for the new document */
	   TtaSetNotificationMode (theDocument, 1);
	   currentLanguage = TtaGetDefaultLanguage ();
	   HTMLSSchema = TtaGetDocumentSSchema (theDocument);
	   rootElement = TtaGetMainRoot (theDocument);
#ifndef STANDALONE
#ifndef INCR_DISPLAY
	   TtaSetDisplayMode (theDocument, NoComputedDisplay);
#endif /* INCR_DISPLAY */
	   /* delete all element except the root element */
	   el = TtaGetFirstChild (rootElement);
	   while (el != NULL)
	     {
		oldel = el;
		TtaNextSibling (&el);
		TtaDeleteTree (oldel, theDocument);
	     }
	   /* save the path or URL of the document */
	   TtaExtractName (pathURL, temppath, tempname);
	   TtaSetDocumentDirectory (doc, temppath);
	   /* disable auto save */
	   TtaSetDocumentBackUpInterval (doc, 0);
#endif /* STANDALONE */
	   /* initialize parsing environment */
	   InitializeParser (NULL, FALSE, 0);
#ifdef MATHML
	   WithinMathML = FALSE;
	   ReadingHTMLentity = FALSE;
	   MathMLSSchema = NULL;
#endif
	   /* parse the input file and build the Thot document */
           if (buffer != NULL)
	     {
	       currentState = 0;
	       numberOfCharRead = 0;
	       numberOfLinesRead = 1;
	       while (res > 0)
		 {
		   /* remove null characters */
		   c = 0;
		   while (c < res)
		     {
		       if (buffer[c] == EOS)
			 {
			   nbNul = 0;
			   while (buffer[c+nbNul] == EOS)
			     nbNul++;
			   res-= nbNul;
			   diff = res - c;
			   if (diff > 0)
			     strncpy (&buffer[c], &buffer[c+nbNul], diff);
			 }
		       else
			 c++;
		     }
		   /* complete the buffer */
		   for (c = res; c < bufsize; c++)
		     buffer[c] = EOS;
		   if (beginning && buffer != NULL &&
		       !strncmp (buffer, "<!DOCTYPE HTML", 14))
		     PlainText = FALSE;
		   curChar = 0;
		   if (PlainText)
		     ReadTextFile (NULL, buffer, beginning);
		   else
		     HTMLread (NULL, buffer);
		   if (res >= bufsize)
		     {
		       /* continue */
		       res = gzread (stream, buffer, bufsize);
		       beginning = FALSE;
		     }
		   else
		     res = 0;
		 }
	       TtaFreeMemory (buffer);
	       /* end of HTML file */
	       EndOfDocument ();
	     }
	   else if (PlainText)
	     ReadTextFile (infile, NULL, beginning);
	   else
	     HTMLread (infile, NULL);
#ifdef HANDLE_COMPRESSED_FILES
	   if (stream != NULL)
	     gzclose (stream);
#endif
	   /* completes all unclosed elements */
	   el = lastElement;
	   while (el != NULL)
	     {
		ElementComplete (el);
		el = TtaGetParent (el);
	     }
#ifdef STANDALONE
	   /* check the Thot abstract tree */
	   CheckAbstractTree (NULL);
	   /* save and close the Thot document */
	   TtaSaveDocument (theDocument, pivotFileName);
	   TtaCloseDocument (theDocument);
#else  /* STANDALONE */
	   /* check the Thot abstract tree */
	   CheckAbstractTree (pathURL);
#ifdef AMAYA_JAVA
           handleLinkHeaders (theDocument);
#endif /* AMAYA_JAVA */
	   if (TtaIsViewOpened (theDocument, 1))
	       ApplyFinalStyle (theDocument);
	   TtaFreeMemory (docURL);
#ifndef INCR_DISPLAY
	   TtaSetDisplayMode (theDocument, DisplayImmediately);
#endif /* INCR_DISPLAY */
#endif /* STANDALONE */
	   /* check the Thot abstract tree against the structure schema. */
	   TtaSetStructureChecking (1, theDocument);
	   /* allow the user to edit the document */
	   /***** TtaSetDocumentAccessMode(theDocument, 1); ****/
#      ifdef MATHML 
	   MathMLSSchema = NULL;
#      endif /* MATHML */
	   HTMLSSchema = NULL;
	}
	/* close the HTML file */
	fclose (infile);
     }
#ifdef STANDALONE
   /* quit the Thot toolkit */
   TtaQuit ();
}
#else  /* STANDALONE */
   TtaSetDocumentUnmodified (theDocument);
   theDocument = 0;
#endif /* STANDALONE */
}

/* end of module */
