/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Everything directly linked to the CSS syntax should now hopefully
 * be contained in this module.
 *
 * Author: D. Veillard
 *
 */

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"

#include "css_f.h"
#include "html2thot_f.h"
#include "HTMLstyle_f.h"
#include "HTMLpresentation_f.h"
#include "UIcss_f.h"
#include "HTMLimage_f.h"

extern boolean      NonPPresentChanged;

/* CSSLEVEL2 adding new features to the standard */
/* DEBUG_STYLES verbose output of style actions */
/* DEBUG_CLASS_INTERF verbose output on class interface actions */

#ifdef _WINDOWS
#ifndef __GNUC__
int                 strncasecmp (char *s1, char *s2, size_t n)
{
   if (n == 0)
      return 0;

   while (n-- != 0 && toupper (*s1) == toupper (*s2))
     {
	if (n == 0 || *s1 == EOS || *s2 == EOS)
	   break;
	s1++;
	s2++;
     }

   return toupper (*(unsigned char *) s1) - toupper (*(unsigned char *) s2);
}

#endif /* ! __GNUC__ */
#endif /* _WINDOWS */


#define MAX_DEEP 10
#include "HTMLstyleColor.h"

/************************************************************************
 *									*  
 *			PARSER FRONT-END				*                                  
 *									*  
 ************************************************************************/

/*----------------------------------------------------------------------
   CSSparser :  is the front-end function called by the HTML parser
        when detecting a <STYLE TYPE="text/css"> indicating it's the
        beginning of a CSS fragment. readfunc is a function used to
        read one character at a time from the input stream.
  
        The CSS parser has to handle <!-- ... --> constructs used to
        prevent prehistoric browser from displaying the CSS as a text
        content. It will stop on any sequence "<x" where x is different
        from ! and will return x as to the caller. Theorically x should
        be equal to / for the </STYLE> end of style marker but who knows !
  ----------------------------------------------------------------------*/

#define CSS_CHECK_BUFFER					\
{								\
    if (index >= (buffer_size - 2)) {				\
        char *new =(char *)TtaRealloc(buffer, buffer_size * 2); \
	if (new == NULL) return(EOS);				\
	buffer_size *= 2;					\
	buffer = new;						\
    }}

#ifdef __STDC__
char                CSSparser (AmayaReadChar readfunc, Document doc)
#else
char                CSSparser (readfunc, doc)
AmayaReadChar       readfunc;
Document            doc;

#endif
{
   char               *buffer = NULL;
   int                 buffer_size = 2000;
   int                 index = 0;
   char                cour = readfunc ();

   buffer = TtaGetMemory (buffer_size);
   if (buffer == NULL)
      return (EOS);

   while (cour != EOS)
     {
	switch (cour)
	      {
		 case '<':
		    cour = readfunc ();
		    if (cour != '!')
		      {

			 /*
			  * Ok we consider this as a closing tag !
			  */
			 if (index > 0)
			   {
			      CSS_CHECK_BUFFER
				 buffer[index++] = EOS;
			      ParseHTMLStyleHeader (NULL, buffer, doc, TRUE);
			      index = 0;
			   }
			 TtaFreeMemory (buffer);
			 return (cour);
		      }
		    cour = readfunc ();
		    continue;
		 case '-':
		    cour = readfunc ();
		    if (cour != '-')
		      {
			 CSS_CHECK_BUFFER
			    buffer[index++] = '-';
			 continue;
		      }
		    cour = readfunc ();
		    if (cour != '>')
		      {
			 CSS_CHECK_BUFFER
			    buffer[index++] = '-';
			 CSS_CHECK_BUFFER
			    buffer[index++] = '-';
			 continue;
		      }
		    cour = readfunc ();
		    continue;
	      }
	CSS_CHECK_BUFFER
	   buffer[index++] = cour;
	cour = readfunc ();
     }
   if (index > 0)
     {
	/* give this piece of CSS to the parser */
	CSS_CHECK_BUFFER
	   buffer[index++] = EOS;
	ParseHTMLStyleHeader (NULL, buffer, doc, TRUE);
     }
   TtaFreeMemory (buffer);
   return (cour);
}

/************************************************************************
 *									*  
 *			  PARSING DEFINITIONS    			*
 *									*  
 ************************************************************************/

/*
 * This flag is used to switch the parser to a destructive mode where
 * instead of adding the corresponding style, the rule are deleted.
 * Manipulate with care !!!
 */

static boolean         HTMLStyleParserDestructiveMode = FALSE;

/*
 * A HTML3StyleValueParser is a function used to parse  the
 * description substring associated to a given style attribute
 * e.g. : "red" for a color attribute or "12pt bold helvetica"
 * for a font attribute.
 */

#ifdef __STDC__
typedef char       *(*HTML3StyleValueParser)
                    (PresentationTarget target,
		     PresentationContext context, char *attrstr);

#else
typedef char       *(*HTML3StyleValueParser) ();

#endif


/*
 *	Macro's used to generate Parser routines signatures.
 *      These heavily rely on the token-pasting mechanism provided by
 *      the C preprocessor. The string a##b is replaced by the string
 *      "ab", but this is done after the macro is expanded.
 *      This mecanism allows to avoid a lot of typing, errors and keep
 *      the code compact at the price of a loss of readability.
 *      On old fashionned preprocessor (pre-Ansi) the token pasting was
 *      a side effect of the preprocessor implementation on empty
 *      comments. In this case we use a+slash+star+star+slash+b to
 *      produce the same string "ab".
 */

#if (defined(__STDC__) && !defined(UNIXCPP)) || defined(ANSICPP) || defined(WWW_MSWINDOWS)
#define VALUEPARSER(name)						\
static char        *ParseCSS##name (PresentationTarget target,	\
			PresentationContext context, char *attrstr);
#else
#define VALUEPARSER(name)						\
static char        *ParseCSS/**/name();
#endif

VALUEPARSER(FontFamily)
VALUEPARSER(FontStyle)
VALUEPARSER(FontVariant)
VALUEPARSER(FontWeight)
VALUEPARSER(FontSize)
VALUEPARSER(Font)

VALUEPARSER(Foreground)
VALUEPARSER(BackgroundColor)
VALUEPARSER(BackgroundImage)
VALUEPARSER(BackgroundRepeat)
VALUEPARSER(BackgroundAttachment)
VALUEPARSER(BackgroundPosition)
VALUEPARSER(Background)

VALUEPARSER(WordSpacing)
VALUEPARSER(LetterSpacing)
VALUEPARSER(TextDecoration)
VALUEPARSER(VerticalAlign)
VALUEPARSER(TextTransform)
VALUEPARSER(TextAlign)
VALUEPARSER(TextIndent)
VALUEPARSER(LineSpacing)

VALUEPARSER(MarginTop)
VALUEPARSER(MarginRight)
VALUEPARSER(MarginBottom)
VALUEPARSER(MarginLeft)
VALUEPARSER(Margin)

VALUEPARSER(PaddingTop)
VALUEPARSER(PaddingRight)
VALUEPARSER(PaddingBottom)
VALUEPARSER(PaddingLeft)
VALUEPARSER(Padding)

VALUEPARSER(BorderTopWidth)
VALUEPARSER(BorderRightWidth)
VALUEPARSER(BorderBottomWidth)
VALUEPARSER(BorderLeftWidth)
VALUEPARSER(BorderWidth)
VALUEPARSER(BorderColor)
VALUEPARSER(BorderStyle)
VALUEPARSER(BorderTop)
VALUEPARSER(BorderRight)
VALUEPARSER(BorderBottom)
VALUEPARSER(BorderLeft)
VALUEPARSER(Border)

VALUEPARSER(Width)
VALUEPARSER(Height)
VALUEPARSER(Float)
VALUEPARSER(Clear)

VALUEPARSER(Display)
VALUEPARSER(WhiteSpace)

VALUEPARSER(ListStyleType)
VALUEPARSER(ListStyleImage)
VALUEPARSER(ListStylePosition)
VALUEPARSER(ListStyle)

/* Sorry, not in CSS but so useful ! */
VALUEPARSER(Magnification)
VALUEPARSER(Test)

/*
 * Description of the set of HTML3 Style Attributes supported.
 */

typedef struct HTML3StyleAttribute
  {
     char               *name;
     HTML3StyleValueParser parsing_function;
  }
HTML3StyleAttribute;

/*
 * NOTE : Long attribute name MUST be placed before shortened ones !
 *        e.g. "FONT-SIZE" must be placed before "FONT"
 */

static HTML3StyleAttribute HTML3StyleAttributes[] =
{
   {"font-family", ParseCSSFontFamily},
   {"font-style", ParseCSSFontStyle},
   {"font-variant", ParseCSSFontVariant},
   {"font-weight", ParseCSSFontWeight},
   {"font-size", ParseCSSFontSize},
   {"font", ParseCSSFont},

   {"color", ParseCSSForeground},
   {"background-color", ParseCSSBackgroundColor},
   {"background-image", ParseCSSBackgroundImage},
   {"background-repeat", ParseCSSBackgroundRepeat},
   {"background-attachment", ParseCSSBackgroundAttachment},
   {"background-position", ParseCSSBackgroundPosition},
   {"background", ParseCSSBackground},

   {"word-spacing", ParseCSSWordSpacing},
   {"letter-spacing", ParseCSSLetterSpacing},
   {"text-decoration", ParseCSSTextDecoration},
   {"vertical-align", ParseCSSVerticalAlign},
   {"text-transform", ParseCSSTextTransform},
   {"text-align", ParseCSSTextAlign},
   {"text-indent", ParseCSSTextIndent},
   {"line-height", ParseCSSLineSpacing},

   {"margin-top", ParseCSSMarginTop},
   {"margin-right", ParseCSSMarginRight},
   {"margin-bottom", ParseCSSMarginBottom},
   {"margin-left", ParseCSSMarginLeft},
   {"margin", ParseCSSMargin},

   {"padding-top", ParseCSSPaddingTop},
   {"padding-right", ParseCSSPaddingRight},
   {"padding-bottom", ParseCSSPaddingBottom},
   {"padding-left", ParseCSSPaddingLeft},
   {"padding", ParseCSSPadding},

   {"border-top-width", ParseCSSBorderTopWidth},
   {"border-right-width", ParseCSSBorderRightWidth},
   {"border-bottom-width", ParseCSSBorderBottomWidth},
   {"border-left-width", ParseCSSBorderLeftWidth},
   {"border-width", ParseCSSBorderWidth},
   {"border-color", ParseCSSBorderColor},
   {"border-style", ParseCSSBorderStyle},
   {"border-top", ParseCSSBorderTop},
   {"border-right", ParseCSSBorderRight},
   {"border-bottom", ParseCSSBorderBottom},
   {"border-left", ParseCSSBorderLeft},
   {"border", ParseCSSBorder},

   {"width", ParseCSSWidth},
   {"height", ParseCSSHeight},
   {"float", ParseCSSFloat},
   {"clear", ParseCSSClear},

   {"display", ParseCSSDisplay},
   {"white-space", ParseCSSWhiteSpace},

   {"list-style-type", ParseCSSListStyleType},
   {"list-style-image", ParseCSSListStyleImage},
   {"list-style-position", ParseCSSListStylePosition},
   {"list-style", ParseCSSListStyle},

   /* Extra's wrt. CSS 1.0 */

   {"magnification", ParseCSSMagnification},
   {"test", ParseCSSTest},
};

#define NB_HTML3STYLEATTRIBUTE (sizeof(HTML3StyleAttributes) / \
                                sizeof(HTML3StyleAttributes[0]))

/*
 * A few macro needed to help building the parser
 */

#define ERR -1000000
/*
   #define SKIP_BLANK(ptr) \
   { while (((*(ptr)) == SPACE) || ((*(ptr)) == '\b') || \
   ((*(ptr)) == EOL) || ((*(ptr)) == '\r')) ptr++; } */
#define SKIP_WORD(ptr) { while (isalnum(*ptr)) ptr++; }
#define SKIP_PROPERTY(ptr) { while ((*ptr) && (*ptr != ';') && \
                                    (*ptr != '}') && (*ptr != ',')) ptr++; }
#define SKIP_INT(ptr) { while (isdigit(*ptr)) ptr++; }
#define SKIP_FLOAT(ptr) { while (isdigit(*ptr)) ptr++; \
      if (*ptr == '.') do ptr++; while (isdigit(*ptr)); }
#define IS_SEPARATOR(ptr) ((*ptr == ',') || (*ptr == ';'))
#define IS_BLANK(ptr) (((*(ptr)) == SPACE) || ((*(ptr)) == '\b') || \
              ((*(ptr)) == EOL) || ((*(ptr)) == '\r'))
#define START_DESCR(ptr) (*ptr == ':')
#define IS_WORD(ptr,word) (!strncmp((ptr),(word),strlen(word)))
#define IS_CASE_WORD(ptr,word) (!strncasecmp((ptr),(word),strlen(word)))
#define IS_NUM(ptr) \
      ((isdigit(*ptr)) || (*ptr == '.') || (*ptr == '+') || (*ptr == '-'))
#define IS_INT(ptr) (isdigit(*ptr))
#define READ_I(ptr,i) { int l; l = sscanf(ptr,"%d",&i); \
        if (l <= 0) i = ERR; else SKIP_INT(ptr); }
#define READ_IOF(ptr,i,f) { int d; READ_I(ptr,i); \
        if (*ptr == '.') { ptr++; READ_I(ptr,d); f }


#ifdef AMAYA_DEBUG
#define TODO { fprintf(stderr, "code incomplete file %s line %d\n",\
                       __FILE__,__LINE__); };
#define MSG(msg) fprintf(stderr, msg)
#else
	/*static char        *last_message = NULL; */

#define TODO
	/*#define MSG(msg) last_message = msg */
static char        *last_message = NULL;

#define MSG(msg) last_message = msg
#endif


/************************************************************************
 *									*  
 * 			 UNITS CONVERSION FUNCTIONS			*
 *									*  
 ************************************************************************/

/********** UNUSED *************
#ifdef __STDC__
static int is_float(const char *ptr)
#else
static int is_float(ptr)
    char *ptr;
#endif
{
     if ((*ptr == '+') || (*ptr == '-')) ptr++;
     if (!((isdigit(*ptr)) || (*ptr == '.'))) return(0);
     while (isdigit(*ptr)) ptr++;
     if (*ptr == '.') return(1);
     return(0);
}
#ifdef __STDC__
static int is_int(const char *ptr)
#else
static int is_int(ptr)
    char *ptr;
#endif
{
     if ((*ptr == '+') || (*ptr == '-')) ptr++;
     if (isdigit(*ptr)) return(1);
     return(0);
}
 ********** UNUSED *************/

#ifdef __STDC__
static unsigned int hexa_val (char c)
#else
static unsigned int hexa_val (c)
char                c;

#endif
{
   if ((c >= '0') && (c <= '9'))
      return (c - '0');
   if ((c >= 'a') && (c <= 'f'))
      return (c - 'a' + 10);
   if ((c >= 'A') && (c <= 'F'))
      return (c - 'A' + 10);
   return (0);
}

/*----------------------------------------------------------------------
   ParseHTML3Unit :                                                  
   parse a CSS Unit substring and returns the corresponding      
   value and its unit.                                           
  ----------------------------------------------------------------------*/

#define UNIT_INVALID	0
#define UNIT_POINT	1
#define UNIT_EM		2
#define UNIT_PIXEL	3
struct unit_def
{
   char               *sign;
   int                 unit;
};
static struct unit_def HTML3UnitNames[] =
{
   {"pt", DRIVERP_UNIT_PT},
   {"pc", DRIVERP_UNIT_PC},
   {"in", DRIVERP_UNIT_IN},
   {"cm", DRIVERP_UNIT_CM},
   {"mm", DRIVERP_UNIT_MM},
   {"em", DRIVERP_UNIT_EM},
   {"px", DRIVERP_UNIT_PX},
   {"ex", DRIVERP_UNIT_XHEIGHT},
   {"%", DRIVERP_UNIT_PERCENT},
};

#define NB_UNITS (sizeof(HTML3UnitNames) / sizeof(struct unit_def))

#ifdef __STDC__
static char        *ParseHTML3Unit (char *attrstr, PresentationValue * pval)
#else
static char        *ParseHTML3Unit (attrstr, pval)
char               *attrstr;
PresentationValue  *pval;

#endif
{
   int                 val = 0;
   int                 minus = 0;
   int                 real = 0;
   int                 valid = 0;
   int                 f = 0;
   int                 uni;

   pval->typed_data.unit = DRIVERP_UNIT_REL;

   SKIP_BLANK (attrstr);
   if (*attrstr == '-')
     {
	minus = 1;
	attrstr++;
	SKIP_BLANK (attrstr);
     }
   if (*attrstr == '+')
     {
	attrstr++;
	SKIP_BLANK (attrstr);
     }
   while ((*attrstr >= '0') && (*attrstr <= '9'))
     {
	val *= 10;
	val += *attrstr - '0';
	attrstr++;
	valid = 1;
     }
   if (*attrstr == '.')
     {
	real = 1;
	f = val;
	val = 0;
	attrstr++;
	/* keep only 3 digits */
	if ((*attrstr >= '0') && (*attrstr <= '9'))
	  {
	     val = (*attrstr - '0') * 100;
	     attrstr++;
	     if ((*attrstr >= '0') && (*attrstr <= '9'))
	       {
		  val += (*attrstr - '0') * 10;
		  attrstr++;
		  if ((*attrstr >= '0') && (*attrstr <= '9'))
		    {
		       val += *attrstr - '0';
		       attrstr++;
		    }
	       }
	     while ((*attrstr >= '0') && (*attrstr <= '9'))
	       {
		  attrstr++;
	       }
	     valid = 1;
	  }
     }
   if (!valid)
     {
	SKIP_WORD (attrstr);
	pval->typed_data.unit = DRIVERP_UNIT_INVALID;
	pval->typed_data.value = 0;
	return (attrstr);
     }
   SKIP_BLANK (attrstr);
   for (uni = 0; uni < NB_UNITS; uni++)
     {
#ifdef WWW_WINDOWS
	if (!_strnicmp (HTML3UnitNames[uni].sign, attrstr,
			strlen (HTML3UnitNames[uni].sign)))
#else  /* WWW_WINDOWS */
	if (!strncasecmp (HTML3UnitNames[uni].sign, attrstr,
			  strlen (HTML3UnitNames[uni].sign)))
#endif /* !WWW_WINDOWS */
	  {
	     pval->typed_data.unit = HTML3UnitNames[uni].unit;
	     if (real)
	       {
		  DRIVERP_UNIT_SET_FLOAT (pval->typed_data.unit);
		  if (minus)
		     pval->typed_data.value = -(f * 1000 + val);
		  else
		     pval->typed_data.value = f * 1000 + val;
	       }
	     else
	       {
		  if (minus)
		     pval->typed_data.value = -val;
		  else
		     pval->typed_data.value = val;
	       }
	     return (attrstr + strlen (HTML3UnitNames[uni].sign));
	  }
     }

   /*
    * not in the list of predefined units.
    */
   pval->typed_data.unit = DRIVERP_UNIT_REL;
   if (minus)
      pval->typed_data.value = -val;
   else
      pval->typed_data.value = val;
   return (attrstr);
}

/************************************************************************
 *									*  
 *			PARSING FUNCTIONS				*
 *									*  
 ************************************************************************/

/*----------------------------------------------------------------------
   ParseHTMLURL :                                                    
   parse a CSS URL construct, returning the string for the URL   
   this string need to be freed.                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseHTMLURL (char *attrstr, char **url)
#else
static char        *ParseHTMLURL (attrstr, url)
char               *attrstr;
char              **url;
#endif
{
  char                sauve;
  char               *base;
  if (((attrstr[0] == 'u') || (attrstr[0] = 'U')) &&
      ((attrstr[0] == 'r') || (attrstr[0] = 'R')) &&
      ((attrstr[0] == 'l') || (attrstr[0] = 'L')))
    {
      attrstr += 3;
      SKIP_BLANK (attrstr);
      if (*attrstr == '(')
	{
	  attrstr++;
	  SKIP_BLANK (attrstr);
	  base = attrstr;
	  while ((*attrstr != EOS) && (!IS_BLANK (attrstr)) &&
		 (*attrstr != ')'))
	    attrstr++;
      
	  if (url != NULL)
	    {
	      sauve = *attrstr;
	      *attrstr = EOS;
	      *url = TtaStrdup (base);
	      *attrstr = sauve;
	    }
	  attrstr++;
	  return (attrstr);
	}
    }

  if (url != NULL)
    *url = NULL;  
  return (attrstr);
}

/*----------------------------------------------------------------------
   GetHTML3StyleAttrIndex : returns the index of the current         
   attribute type in the HTML3StyleAttributes array             
   return NULL if not found                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *GetHTML3StyleAttrIndex (char *attrstr, int *index)
#else
static char        *GetHTML3StyleAttrIndex (attrstr, index)
char               *attrstr;
int                *index;
#endif
{
   int                 i;

   SKIP_BLANK (attrstr);

   for (i = 0; i < NB_HTML3STYLEATTRIBUTE; i++)
      if (IS_WORD (attrstr, HTML3StyleAttributes[i].name))
	{
	   *index = i;
	   return (attrstr + strlen (HTML3StyleAttributes[i].name));
	}
   return (NULL);
}

/*----------------------------------------------------------------------
   GetHTML3Name : return a string corresponding to the CSS name of   
   an element                                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char               *GetHTML3Name (Element elem, Document doc)
#else
char               *GetHTML3Name (elem, doc)
Element             elem;
Document            doc;
#endif
{
   char               *res = GITagName (elem);

   /* some kind of filtering is probably needed !!! */
   if (res == NULL)
      return ("unknown");
   return (res);
}

/*----------------------------------------------------------------------
   GetHTML3Names : return the list of strings corresponding to the   
   CSS names of an element                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int             GetHTML3Names (Element elem, Document doc, char **lst, int max)
#else
int             GetHTML3Names (elem, doc, lst, max)
Element         elem;
Document        doc;
char          **lst;
int             max;
#endif
{
   char        *res;
   int          deep;
   Element      father = elem;

   for (deep = 0; deep < max;)
     {
	elem = father;
	if (elem == NULL)
	   break;
	father = TtaGetParent (elem);

	res = GITagName (elem);

	if (res == NULL)
	   continue;
	if (!strcmp (res, "BODY"))
	   break;
	if (!strcmp (res, "HTML"))
	   break;

	/* store this level in the array */
	lst[deep] = res;
	deep++;
     }
   return (deep);
}

/************************************************************************
 *									*  
 *	TRANSLATING FROM PRESENTATION DRIVER VALUES TO CSS STRING	*
 *									*  
 ************************************************************************/

/*
 * PresentationSettingsToCSS :  translate a PresentationSetting to the
 *      equivalent CSS string, and add it to the buffer given as the
 *      argument. It is used when extracting the CSS string from actual
 *      presentation.
 *
 * All the possible values returned by the presentation drivers are
 * described in thotlib/include/presentdriver.h (PresentationType enum).
 */

#ifdef __STDC__
void         PresentationSettingsToCSS (PresentationSetting settings,
                         char *buffer, int len)
#else
void         PresentationSettingsToCSS (settings, buffer, len)
PresentationSetting settings;
char               *param;

#endif
{
   unsigned short      red, green, blue;
   LoadedImageDesc    *imgInfo;
   int                 add_unit = 0;
   int                 real = 0;
   float               fval = 0;
   int                 unit, i;

   buffer[0] = EOS;
   if (len < 40) return;

   unit = settings->value.typed_data.unit;
   if (DRIVERP_UNIT_IS_FLOAT (unit))
     {
	DRIVERP_UNIT_UNSET_FLOAT (unit);
	real = 1;
	fval = (float) settings->value.typed_data.value;
	fval /= 1000;
     }

   switch (settings->type)
	 {
	    case DRIVERP_NONE:
	       break;
	    case DRIVERP_FOREGROUND_COLOR:
	       TtaGiveThotRGB (settings->value.typed_data.value, &red, &green, &blue);
	       sprintf (buffer, "color : #%02X%02X%02X", red, green, blue);
	       break;
	    case DRIVERP_BACKGROUND_COLOR:
	       TtaGiveThotRGB (settings->value.typed_data.value, &red, &green, &blue);
	       sprintf (buffer, "background-color : #%02X%02X%02X", red, green, blue);
	       break;
	    case DRIVERP_FONT_SIZE:
	       if (unit == DRIVERP_UNIT_REL)
		  switch (settings->value.typed_data.value)
			{
			   case 1:
			      strcpy (buffer, "font-size : xx-small");
			      break;
			   case 2:
			      strcpy (buffer, "font-size : x-small");
			      break;
			   case 3:
			      strcpy (buffer, "font-size : small");
			      break;
			   case 4:
			      strcpy (buffer, "font-size : medium");
			      break;
			   case 5:
			      strcpy (buffer, "font-size : large");
			      break;
			   case 6:
			      strcpy (buffer, "font-size : x-large");
			      break;
			   default:
			      if (settings->value.typed_data.value > 6)
				 strcpy (buffer, "font-size : xx-large");
			      break;
			}
	       else
		 {
		    if (real)
		       sprintf (buffer, "font-size : %g", fval);
		    else
		       sprintf (buffer, "font-size : %d", settings->value.typed_data.value);
		    add_unit = 1;
		 }
	       break;
	    case DRIVERP_FONT_STYLE:
	       switch (settings->value.typed_data.value)
		     {
			case DRIVERP_FONT_BOLD:
			   strcpy (buffer, "font-weight : bold");
			   break;
			case DRIVERP_FONT_ROMAN:
			   strcpy (buffer, "font-style : normal");
			   break;
			case DRIVERP_FONT_ITALICS:
			   strcpy (buffer, "font-style : italic");
			   break;
			case DRIVERP_FONT_BOLDITALICS:
			   strcpy (buffer, "font-weight : bold, font-style : italic");
			   break;
			case DRIVERP_FONT_OBLIQUE:
			   strcpy (buffer, "font-style : oblique");
			   break;
			case DRIVERP_FONT_BOLDOBLIQUE:
			   strcpy (buffer, "font-weight : bold, font-style : oblique");
			   break;
		     }
	       break;
	    case DRIVERP_FONT_FAMILY:
	       switch (settings->value.typed_data.value)
		     {
			case DRIVERP_FONT_HELVETICA:
			   strcpy (buffer, "font-family : helvetica");
			   break;
			case DRIVERP_FONT_TIMES:
			   strcpy (buffer, "font-family : times");
			   break;
			case DRIVERP_FONT_COURIER:
			   strcpy (buffer, "font-family : courrier");
			   break;
		     }
	       break;
	    case DRIVERP_TEXT_UNDERLINING:
	       switch (settings->value.typed_data.value)
		     {
			case DRIVERP_UNDERLINE:
			   strcpy (buffer, "text-decoration : underline");
			   break;
			case DRIVERP_OVERLINE:
			   strcpy (buffer, "text-decoration : overline");
			   break;
			case DRIVERP_CROSSOUT:
			   strcpy (buffer, "text-decoration : line-through");
			   break;
		     }
	       break;
	    case DRIVERP_ALIGNMENT:
	       switch (settings->value.typed_data.value)
		     {
                        case DRIVERP_ADJUSTLEFT:
                           strcpy (buffer, "text-align : left");
                           break;
                        case DRIVERP_ADJUSTRIGHT:
                           strcpy (buffer, "text-align : right");
                           break;
                        case DRIVERP_ADJUSTCENTERED:
                           strcpy (buffer, "text-align : center");
                           break;
                        case DRIVERP_ADJUSTLEFTWITHDOTS:
                           strcpy (buffer, "text-align : left");
                           break;
		     }
	       break;
	    case DRIVERP_LINE_SPACING:
	       if (real)
		  sprintf (buffer, "line-height : %g", fval);
	       else
		  sprintf (buffer, "line-height : %d", settings->value.typed_data.value);
	       add_unit = 1;
	       break;
	    case DRIVERP_INDENT:
	       if (real)
		  sprintf (buffer, "text-indent : %g", fval);
	       else
		  sprintf (buffer, "text-indent : %d", settings->value.typed_data.value);
	       add_unit = 1;
	       break;
	    case DRIVERP_JUSTIFICATION:
	       if (settings->value.typed_data.value == DRIVERP_JUSTIFIED)
		  sprintf (buffer, "text-align: justify");
	       break;
	    case DRIVERP_HYPHENATION:
#if 0
	       /* not yet in CSS */
	       if (settings->value.typed_data.value == DRIVERP_JUSTIFIED)
		  sprintf (buffer, "text-align: justify");
#endif
	       break;
	    case DRIVERP_FILL_PATTERN:
	       break;
	    case DRIVERP_VERTICAL_POSITION:
	       if (real)
		  sprintf (buffer, "marging-top : %g", fval);
	       else
		  sprintf (buffer, "marging-top : %d", settings->value.typed_data.value);
	       add_unit = 1;
	       break;
	    case DRIVERP_HORIZONTAL_POSITION:
	       if (real)
		  sprintf (buffer, "margin-left : %g", fval);
	       else
		  sprintf (buffer, "margin-left : %d", settings->value.typed_data.value);
	       add_unit = 1;
	       break;
	    case DRIVERP_HEIGHT:
	       if (real)
		  sprintf (buffer, "height : %g", fval);
	       else
		  sprintf (buffer, "height : %d", settings->value.typed_data.value);
	       add_unit = 1;
	       break;
	    case DRIVERP_RELATIVE_HEIGHT:
	       break;
	    case DRIVERP_WIDTH:
	       if (real)
		  sprintf (buffer, "width : %g", fval);
	       else
		  sprintf (buffer, "width : %d", settings->value.typed_data.value);
	       add_unit = 1;
	       break;
	       break;
	    case DRIVERP_RELATIVE_WIDTH:
	       break;
	    case DRIVERP_IN_LINE:
	       if (settings->value.typed_data.value == DRIVERP_INLINE)
		  strcpy (buffer, "display: inline");
	       else if (settings->value.typed_data.value == DRIVERP_NOTINLINE)
		  strcpy (buffer, "display: block");
	       break;
	    case DRIVERP_SHOW:
	       break;
	    case DRIVERP_BOX:
	       break;
	    case DRIVERP_SHOWBOX:
	       break;
	    case DRIVERP_BGIMAGE:
	       if (settings->value.pointer != NULL)
		 {
		   imgInfo = SearchLoadedImage((char *)settings->value.pointer,
		                               0);
		   if (imgInfo != NULL)
		       sprintf (buffer, "background-image: url(%s)",
				(char *) imgInfo->originalName);
		   else
		       sprintf (buffer, "background-image: url(file:/%s)",
		                (char *)settings->value.pointer);
		 }
	       else
		   sprintf (buffer, "background-image: none");
	       break;
	    case DRIVERP_PICTUREMODE:
	       switch (settings->value.typed_data.value) {
	           case DRIVERP_REALSIZE:
		       sprintf (buffer, "background-repeat: no-repeat");
		       break;
	           case DRIVERP_REPEAT:
		       sprintf (buffer, "background-repeat: repeat");
		       break;
	           case DRIVERP_VREPEAT:
		       sprintf (buffer, "background-repeat: repeat-y");
		       break;
	           case DRIVERP_HREPEAT:
		       sprintf (buffer, "background-repeat: repeat-x");
		       break;
	       }
	       break;
	 }
   if (add_unit)
     {
	/*
	 * add the unit string to the CSS string.
	 */
	for (i = 0; i < NB_UNITS; i++)
	  {
	     if (HTML3UnitNames[i].unit == unit)
	       {
		  strcat (buffer, HTML3UnitNames[i].sign);
		  break;
	       }
	  }
     }
}

/************************************************************************
 *									*  
 *	THESE FUNCTIONS ARE USED TO MAINTAIN THE CSS ATTRIBUTE		*
 *	COHERENCY WRT. THE ACTUAL INTERNAL PRESENTATION VALUES		*
 *									*  
 ************************************************************************/

/*----------------------------------------------------------------------
   UpdateStyleDelete : attribute Style will be deleted.            
   remove the existing style presentation.                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateStyleDelete (NotifyAttribute * event)
#else
void                UpdateStyleDelete (event)
NotifyAttribute    *event;
#endif
{
   PresentationTarget  target;
   SpecificContextBlock block;
   PresentationContext context;
   PresentationValue   unused;
   
   unused.data = 0;

   /*
    * remove all the presentation specific rules applied to the element.
    */
   context = (PresentationContext) & block;
   target = (PresentationTarget) event->element;
   block.drv = &SpecificStrategy;
   block.doc = event->document;
   block.schema = TtaGetDocumentSSchema (event->document);
   if (context->drv->CleanPresentation != NULL)
      context->drv->CleanPresentation (target, context, unused);
}

/*----------------------------------------------------------------------
   UpdateStylePost : attribute Style has been updated or created.  
   reflect the new style presentation.                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateStylePost (NotifyAttribute * event)
#else
void                UpdateStylePost (event)
NotifyAttribute    *event;
#endif
{
   Element             el;
   Document            doc;
   Attribute           at;
   AttributeType       atType;
   char               *style = NULL;
   int                 len;

   el = event->element;
   doc = event->document;

   /*
    * First remove all the presentation specific rules applied to the element.
    */
   UpdateStyleDelete (event);

   len = TtaGetTextAttributeLength (event->attribute);
   if ((len < 0) || (len > 10000))
      return;
   if (len == 0)
     {
	/*
	 * suppress the Style Attribute.
	 */
	atType.AttrSSchema = TtaGetDocumentSSchema (doc);
	atType.AttrTypeNum = HTML_ATTR_Style_;

	at = TtaGetAttribute (el, atType);
	if (at != NULL)
	  {
	     TtaRemoveAttribute (el, at, doc);
	     DeleteSpanIfNoAttr (el, doc);
	  }
     }
   else
     {
	/*
	 * parse and apply the new style content.
	 */
	style = (char *) TtaGetMemory (len + 2);
	if (style == NULL)
	   return;
	TtaGiveTextAttributeValue (event->attribute, style, &len);
	style[len] = EOS;

	ParseHTMLSpecificStyle (el, style, doc);
	TtaFreeMemory(style);
     }
}

/*----------------------------------------------------------------------
   SpecificSettingsToCSS :  Callback for ApplyAllSpecificSettings,
       enrich the CSS string.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         SpecificSettingsToCSS (SpecificTarget target,
     SpecificContext ctxt,PresentationSetting settings, void *param)
#else
static void         SpecificSettingsToCSS (settings, param)
PresentationSetting settings;
void               *param;

#endif
{
   char               *css_rules = param;
   char                string[150];

   string[0] = EOS;

   PresentationSettingsToCSS(settings, &string[0], sizeof(string));

   if ((string[0] != EOS) && (*css_rules != EOS))
      strcat (css_rules, "; ");
   if (string[0] != EOS)
      strcat (css_rules, string);
}

/*----------------------------------------------------------------------
   GetHTML3StyleString : return a string corresponding to the CSS    
   description of the presentation attribute applied to a       
   element.
   For stupid reasons, if the target element is HTML or BODY,
   one returns the concatenation of both element style strings.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetHTML3StyleString (Element elem, Document doc, char *buf, int *len)
#else
void                GetHTML3StyleString (elem, doc, buf, len)
Element             elem;
Document            doc;
char               *buf;
int                *len;
#endif
{
   SpecificContext      ctxt;
   ElementType          elType;

   if ((buf == NULL) || (len == NULL) || (*len <= 0))
      return;

   /*
    * this will transform all the Specific Settings associated to
    * the element to one CSS string.
    */
   buf[0] = EOS;
   ctxt = GetSpecificContext(doc);
   ApplyAllSpecificSettings (elem, ctxt, SpecificSettingsToCSS, &buf[0]);
   FreeSpecificContext(ctxt);

   *len = strlen (buf);

   /*
    * BODY / HTML elements specific handling.
    */
   elType = TtaGetElementType(elem);

   if (elType.ElTypeNum == HTML_EL_HTML) {
       elType.ElTypeNum = HTML_EL_BODY;
       elem = TtaSearchTypedElement(elType, SearchForward, elem);
       if (!elem) return;
       if (*len > 0)
           strcat(buf,";");
       *len = strlen (buf);
       ctxt = GetSpecificContext(doc);
       ApplyAllSpecificSettings (elem, ctxt, SpecificSettingsToCSS, &buf[*len]);
       FreeSpecificContext(ctxt);
       *len = strlen (buf);
   } else if (elType.ElTypeNum == HTML_EL_BODY) {
       elem = TtaGetParent(elem);
       if (!elem) return;
       if (*len > 0)
           strcat(buf,";");
       *len = strlen (buf);
       ctxt = GetSpecificContext(doc);
       ApplyAllSpecificSettings (elem, ctxt, SpecificSettingsToCSS, &buf[*len]);
       FreeSpecificContext(ctxt);
       *len = strlen (buf);
   }
}

/************************************************************************
 *									*  
 *	CORE OF THE CSS PARSER : THESE TAKE THE CSS STRINGS 		*
 *	PRODUCE THE CORECT DRIVER CONTEXT, AND DO THE PARSING		*
 *									*  
 ************************************************************************/

/*----------------------------------------------------------------------
   ParseHTMLStyleDecl : parse an HTML3 Style string                        
   we expect the input string describing the style to be of the  
   form : ATTRIBUTE : DESCRIPTION [ , ATTIBUTE : DESCRIPTION ] * 
   but tolerate incorrect or incomplete input                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ParseHTMLStyleDecl (PresentationTarget target, PresentationContext context, char *attrstr)
#else
void                ParseHTMLStyleDecl (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   int                 styleno;
   char               *new;
   PresentationValue   unused;

   while (*attrstr != 0)
     {
	SKIP_BLANK (attrstr);
	/* look for the type of attribute */
	new = GetHTML3StyleAttrIndex (attrstr, &styleno);
	if (!new)
	  {
	     attrstr++;
	     SKIP_WORD (attrstr);
	     SKIP_BLANK (attrstr);
	     continue;
	  }
	/*
	 * update index and skip the ":" indicator if present
	 */
	attrstr = new;
	SKIP_BLANK (attrstr);
	if (START_DESCR (attrstr))
	  {
	     attrstr++;
	     SKIP_BLANK (attrstr);
	  }
	/*
	 * try to parse the attribute associated to this attribute.
	 */
	if (HTML3StyleAttributes[styleno].parsing_function != NULL)
	   new = HTML3StyleAttributes[styleno].
	      parsing_function (target, context, attrstr);

	/*
	 * Update the rendering.
	 */
	if (context->drv->UpdatePresentation != NULL)
	   context->drv->UpdatePresentation (target, context, unused);

	/*
	 * update index and skip the ";" separator if present
	 */
	attrstr = new;
	SKIP_BLANK (attrstr);
	if (IS_SEPARATOR (attrstr))
	  {
	     attrstr++;
	     SKIP_BLANK (attrstr);
	  }
     }

}

/*----------------------------------------------------------------------
   ParseHTMLSpecificStyle : parse and apply an HTML3 Style string. 
   This function must be called only to in the context of        
   specific style applying to an element, we will use the        
   specific presentation driver to reflect the new presentation  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ParseHTMLSpecificStyle (Element elem, char *attrstr, Document doc)
#else
void                ParseHTMLSpecificStyle (elem, attrstr, doc)
Element             elem;
char               *attrstr;
Document            doc;

#endif
{
   PresentationTarget  target;
   SpecificContext     context;
   PresentationValue   unused;
   ElementType         elType;
   Element             el;

   unused.data = 0;
#ifdef DEBUG_STYLES
   fprintf (stderr, "ParseHTMLSpecificStyle(%s,%s,%d)\n",
	    GetHTML3Name (elem, doc), attrstr, doc);
#endif
   /* 
    * A rule applying to BODY is really meant to address HTML.
    */
   elType = TtaGetElementType(elem);
   if (elType.ElTypeNum == HTML_EL_BODY || elType.ElTypeNum == HTML_EL_HEAD)
     {
       elType.ElTypeNum = HTML_EL_HTML;

       el = TtaGetMainRoot (doc);
       /*el = TtaSearchTypedElement (elType, SearchInTree, el);*/
       if (el != NULL)
	 elem = el;
     }
	   
   /*
    * create the context of the Specific presentation driver.
    */
   context = GetSpecificContext(doc);
   if (context == NULL)
     return;
   target = (PresentationTarget) elem;
   if (HTMLStyleParserDestructiveMode)
     context->destroy = 1;

   /* Call the parsor. */
   ParseHTMLStyleDecl (target, (PresentationContext) context, attrstr);

   /* free the context */
   FreeSpecificContext(context);
}

/*----------------------------------------------------------------------
   ParseHTMLGenericSelector : Create a generic context for a given 
   selector string. If the selector is made of multiple comma- 
   separated selector items, it parses them one at a time and  
   return the end of the selector string to be handled or NULL 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char               *ParseHTMLGenericSelector (char *selector, char *attrstr,
			   GenericContext ctxt, Document doc, PSchema gPres)
#else
char               *ParseHTMLGenericSelector (selector, attrstr, ctxt, doc, gPres)
char               *selector;
char               *attrstr;
GenericContext      ctxt;
Document            doc;
PSchema             gPres;
#endif
{
  PresentationTarget  target = (PresentationTarget) gPres;
  char                sel[150];
  char                class[150];
  char                pseudoclass[150];
  char                id[150];
  char                attrelemname[150];
  char               *deb = &sel[0];
  char               *elem = &sel[0];
  char               *cur = &sel[0];
  int                 type, attr, attrval;
  char               *ancestors[MAX_ANCESTORS];
  int                 i, j;
  PresentationValue   unused;

  unused.data = 0;
  for (i = 0; i < MAX_ANCESTORS; i++)
    {
      ancestors[i] = NULL;
      ctxt->ancestors[i] = 0;
      ctxt->ancestors_nb[i] = 0;
    }
  
  /*
   * first format the first selector item, uniformizing blanks.
   */
  SKIP_BLANK (selector);
  sel[0] = 0;
  class[0] = 0;
  pseudoclass[0] = 0;
  id[0] = 0;
  attrelemname[0] = 0;
  while (1)
    {
      /* put one word in the sel buffer */
      while ((*selector != 0) && (*selector != ',') &&
	     (*selector != '.') && (*selector != ':') &&
	     (*selector != '#') && (!IS_BLANK (selector)))
	*cur++ = *selector++;
      *cur++ = 0;
      
      if ((*selector == ':') || (*selector == '.') || (*selector == '#'))
	{
	  /* keep the name as attrelemname, it's not an ancestor */
	  strcpy (attrelemname, elem);
	  elem = "";
	}
      else
	elem = deb;
      deb = cur;
      
      /* store elem in the list if the string is non-empty */
      if (*elem != 0)
	{
	  for (i = MAX_ANCESTORS - 1; i > 0; i--)
	    ancestors[i] = ancestors[i - 1];
	  ancestors[0] = elem;
	}
      /* why did we stop ? */
      if (*selector == 0)
	/* end of the selector */
	break;
      else if (*selector == ',')
	{
	  /* end of the current selector */
	  selector++;
	  break;
	}
      else if (*selector == '.')
	{
	  /* read the class id : only one allowed by selector */
	  class[0] = 0;
	  cur = &class[0];
	  selector++;
	  while ((*selector != 0) && (*selector != ',') &&
		 (*selector != '.') && (*selector != ':') &&
		 (!IS_BLANK (selector)))
	    *cur++ = *selector++;
	  *cur++ = 0;
	  cur = deb;
	}
      else if (*selector == ':')
	{
	  /* read the pseudoclass id : only one allowed by selector */
	  pseudoclass[0] = 0;
	  cur = &pseudoclass[0];
	  selector++;
	  while ((*selector != 0) && (*selector != ',') &&
		 (*selector != '.') && (*selector != ':') &&
		 (!IS_BLANK (selector)))
	    *cur++ = *selector++;
	  *cur++ = 0;
	  cur = deb;
	}
      else if (*selector == '#')
	{
	  /* read the id : only one allowed by selector */
	  id[0] = 0;
	  cur = &id[0];
	  selector++;
	  while ((*selector != 0) && (*selector != ',') &&
		 (*selector != '.') && (*selector != ':') &&
		 (!IS_BLANK (selector)))
	    *cur++ = *selector++;
	  *cur++ = 0;
	  cur = deb;
	}
      else if (IS_BLANK (selector))
	SKIP_BLANK (selector);
    }

  elem = ancestors[0];
  if ((elem == NULL) || (*elem == 0))
    elem = &class[0];
  if (*elem == 0)
    elem = &pseudoclass[0];
  if (*elem == 0)
    elem = &id[0];
  if (*elem == 0)
    return (selector);

   /*
    * set up the context block.
    */
  ctxt->box = 0;
  if (class[0] != 0)
    {
      ctxt->class = &class[0];
      ctxt->classattr = HTML_ATTR_Class;
    }
  else if (pseudoclass[0] != 0)
    {
      ctxt->class = &pseudoclass[0];
      ctxt->classattr = HTML_ATTR_PseudoClass;
    }
  else if (id[0] != 0)
    {
      ctxt->class = &id[0];
      ctxt->classattr = HTML_ATTR_ID;
    }
  else
    {
      ctxt->class = NULL;
      ctxt->classattr = 0;
    }
  
  ctxt->type = ctxt->attr = ctxt->attrval = ctxt->attrelem = 0;
  if (attrelemname[0] != EOS) {
    GIType (attrelemname, &ctxt->attrelem);
    if (ctxt->attrelem == HTML_EL_BODY) ctxt->attrelem = HTML_EL_HTML;
  }
  
  GIType (elem, &ctxt->type);
  if (ctxt->type == HTML_EL_BODY) ctxt->type = HTML_EL_HTML;
  if ((ctxt->type == 0) && (ctxt->attr == 0) &&
      (ctxt->attrval == 0) && (ctxt->classattr == 0))
    {
      ctxt->class = elem;
      ctxt->classattr = HTML_ATTR_Class;
    }
  
  if (ctxt->class != NULL)
    i = 0;
  else
    i = 1;
  for (; i < MAX_ANCESTORS; i++)
    {
      if (ancestors[i] == NULL)
	break;
      type = attr = attrval = 0;
      GIType (ancestors[i], &type);
      if (type == HTML_EL_BODY) type = HTML_EL_HTML;
      if (type == 0)
	continue;
      for (j = 0; j < MAX_ANCESTORS; j++)
	{
	  if (ctxt->ancestors[j] == 0)
	    {
	      ctxt->ancestors[j] = type;
	      ctxt->ancestors_nb[j] = 0;
	      break;
	    }
	  if (ctxt->ancestors[j] == type)
	    {
	      ctxt->ancestors_nb[j]++;
	      break;
	    }
	}
    }

  if (attrstr)
      ParseHTMLStyleDecl (target, (PresentationContext) ctxt, attrstr);
  return (selector);
}

/*----------------------------------------------------------------------
   ParseHTMLGenericStyle : parse and apply an HTML3 Style string.  
   This function must be called only to in the context of        
   a generic style applying to class of element. The generic     
   presentation driver is used to reflect the new presentation.  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ParseHTMLGenericStyle (char *selector, char *attrstr, Document doc, PSchema gPres)
#else
void                ParseHTMLGenericStyle (selector, attrstr, doc, gPres)
char               *selector;
char               *attrstr;
Document            doc;
PSchema             gPres;
#endif
{
   GenericContext      ctxt;

#ifdef DEBUG_STYLES
   fprintf (stderr, "ParseHTMLGenericStyle(%s,%s,%d)\n",
	    selector, attrstr, doc);
#endif

   ctxt = GetGenericContext (doc);
   if (ctxt == NULL)
      return;

   if (HTMLStyleParserDestructiveMode) ctxt->destroy = 1;

   while ((selector != NULL) && (*selector != 0))
      selector = ParseHTMLGenericSelector (selector, attrstr, ctxt,
					   doc, gPres);
   FreeGenericContext (ctxt);

#ifdef DEBUG_STYLES
   fprintf (stderr, "ParseHTMLGenericStyle(%s,%s,%d) done\n",
	    selector, attrstr, doc);
#endif
}

/************************************************************************
 *									*  
 *	PARSING FUNCTIONS FOR EACH CSS ATTRIBUTE SUPPORTED		*
 *									*  
 ************************************************************************/

/*----------------------------------------------------------------------
   ParseCSSTest : For testing purposes only !!!             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSTest (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSTest (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{

   SKIP_WORD (attrstr);
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSBorderTopWidth : parse an HTML3 BorderTopWidth
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorderTopWidth (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSBorderTopWidth (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSBorderTopWidth ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSBorderRightWidth : parse an HTML3 BorderRightWidth
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorderRightWidth (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSBorderRightWidth (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSBorderRightWidth ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSBorderBottomWidth : parse an HTML3 BorderBottomWidth
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorderBottomWidth (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSBorderBottomWidth (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSBorderBottomWidth ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSBorderLeftWidth : parse an HTML3 BorderLeftWidth
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorderLeftWidth (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSBorderLeftWidth (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSBorderLeftWidth ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSBorderWidth : parse an HTML3 BorderWidth
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorderWidth (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSBorderWidth (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSBorderWidth ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSBorderTop : parse an HTML3 BorderTop
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorderTop (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSBorderTop (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSBorderTop ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSBorderRight : parse an HTML3 BorderRight
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorderRight (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSBorderRight (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSBorderRight ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSBorderBottom : parse an HTML3 BorderBottom
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorderBottom (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSBorderBottom (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSBorderBottom ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSBorderLeft : parse an HTML3 BorderLeft
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorderLeft (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSBorderLeft (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSBorderLeft ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSBorderColor : parse an HTML3 border-color        
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorderColor (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSBorderColor (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSBorderColor ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSBorderStyle : parse an HTML3 border-style        
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorderStyle (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSBorderStyle (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSBorderStyle ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSBorder : parse an HTML3 border        
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorder (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSBorder (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSBorder ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSClear : parse an HTML3 clear attribute string    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSClear (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSClear (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSClear ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSDisplay : parse an HTML3 display attribute string        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSDisplay (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSDisplay (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   PresentationValue   pval;

   SKIP_BLANK (attrstr);
   if (IS_WORD (attrstr, "block"))
     {
	pval.typed_data.unit = DRIVERP_UNIT_REL;
	pval.typed_data.value = DRIVERP_NOTINLINE;
	if (context->drv->SetInLine)
	   context->drv->SetInLine (target, context, pval);
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "inline"))
     {
	pval.typed_data.unit = DRIVERP_UNIT_REL;
	pval.typed_data.value = DRIVERP_INLINE;
	if (context->drv->SetInLine)
	   context->drv->SetInLine (target, context, pval);
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "none"))
     {
	pval.typed_data.unit = DRIVERP_UNIT_REL;
	pval.typed_data.value = DRIVERP_HIDE;
	if (context->drv->SetShow)
	   context->drv->SetShow (target, context, pval);
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "list-item"))
     {
	MSG ("list-item display value unsupported\n");
	SKIP_WORD (attrstr);
     }
   else
     {
	fprintf (stderr, "invalid display value %s\n", attrstr);
	return (attrstr);
     }
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSFloat : parse an HTML3 float attribute string    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSFloat (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSFloat (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSFloat ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSLetterSpacing : parse an HTML3 letter-spacing    
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSLetterSpacing (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSLetterSpacing (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSLetterSpacing ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSListStyleType : parse an HTML3 list-style-type
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSListStyleType (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSListStyleType (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSListStyleType ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSListStyleImage : parse an HTML3 list-style-image
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSListStyleImage (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSListStyleImage (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSListStyleImage ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSListStylePosition : parse an HTML3 list-style-position
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSListStylePosition (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSListStylePosition (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSListStylePosition ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSListStyle : parse an HTML3 list-style            
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSListStyle (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSListStyle (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSListStyle ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSMagnification : parse an HTML3 magnification     
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSMagnification (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSMagnification (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   PresentationValue   pval;

   SKIP_BLANK (attrstr);
   attrstr = ParseHTML3Unit (attrstr, &pval);
   if ((pval.typed_data.unit == DRIVERP_UNIT_REL) && (pval.typed_data.value >= -10) &&
       (pval.typed_data.value <= 10))
     {
	if (context->drv == &GenericStrategy)
	  {
	     GenericContext      block = (GenericContext) context;

	     if ((block->type == HTML_EL_HTML) ||
		 (block->type == HTML_EL_BODY) ||
		 (block->type == HTML_EL_HEAD))
	       {
		  CSSSetMagnification (block->doc, (PSchema) target, pval.typed_data.value);
		  return (attrstr);
	       }
	  }
     }
   else
     {
	MSG ("invalid magnification value\n");
     }
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSMarginLeft : parse an HTML3 margin-left          
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSMarginLeft (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSMarginLeft (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSMarginLeft ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSMarginRight : parse an HTML3 margin-right        
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSMarginRight (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSMarginRight (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSMarginRight ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSMargin : parse an HTML3 margin attribute string. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSMargin (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSMargin (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSMargin ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSPaddingTop : parse an HTML3 PaddingTop
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSPaddingTop (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSPaddingTop (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSPaddingTop ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSPaddingRight : parse an HTML3 PaddingRight
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSPaddingRight (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSPaddingRight (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSPaddingRight ");
   TODO
      return (attrstr);
}


/*----------------------------------------------------------------------
   ParseCSSPaddingBottom : parse an HTML3 PaddingBottom
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSPaddingBottom (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSPaddingBottom (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSPaddingBottom ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSPaddingLeft : parse an HTML3 PaddingLeft
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSPaddingLeft (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSPaddingLeft (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSPaddingLeft ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSPadding : parse an HTML3 padding attribute string. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSPadding (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSPadding (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSPadding ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSTextAlign : parse an HTML3 text-align            
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSTextAlign (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSTextAlign (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   PresentationValue   align;
   PresentationValue   justify;

   align.typed_data.value = 0;
   align.typed_data.unit = 1;
   justify.typed_data.value = 0;
   justify.typed_data.unit = 1;

   SKIP_BLANK (attrstr);
   if (IS_WORD (attrstr, "left"))
     {
	align.typed_data.value = AdjustLeft;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "right"))
     {
	align.typed_data.value = AdjustRight;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "center"))
     {
	align.typed_data.value = Centered;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "justify"))
     {
	justify.typed_data.value = Justified;
	SKIP_WORD (attrstr);
     }
   else
     {
	fprintf (stderr, "invalid align value\n");
	return (attrstr);
     }

   /*
    * install the new presentation.
    */
   if (align.typed_data.value)
     {
	if (context->drv->SetAlignment)
	   context->drv->SetAlignment (target, context, align);
     }
   if (justify.typed_data.value)
     {
	if (context->drv->SetJustification)
	   context->drv->SetJustification (target, context, justify);
	if (context->drv->SetHyphenation)
	   context->drv->SetHyphenation (target, context, justify);
     }
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSTextIndent : parse an HTML3 text-indent          
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSTextIndent (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSTextIndent (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   PresentationValue   pval;

   SKIP_BLANK (attrstr);
   attrstr = ParseHTML3Unit (attrstr, &pval);
   if (pval.typed_data.unit == DRIVERP_UNIT_INVALID)
     {
	fprintf (stderr, "invalid font size\n");
	return (attrstr);
     }
   /*
    * install the attribute
    */
   if (context->drv->SetIndent != NULL)
     {
	context->drv->SetIndent (target, context, pval);
     }
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSTextTransform : parse an HTML3 text-transform    
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSTextTransform (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSTextTransform (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSTextTransform ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSVerticalAlign : parse an HTML3 vertical-align    
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSVerticalAlign (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSVerticalAlign (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSVerticalAlign ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSWhiteSpace : parse an HTML3 white-space          
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSWhiteSpace (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSWhiteSpace (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   SKIP_BLANK (attrstr);
   if (IS_WORD (attrstr, "normal"))
     {
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "pre"))
     {
	MSG ("pre white-space setting unsupported\n");
	SKIP_WORD (attrstr);
     }
   else
     {
	fprintf (stderr, "invalid white-space value %s\n", attrstr);
	return (attrstr);
     }
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSWordSpacing : parse an HTML3 word-spacing        
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSWordSpacing (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSWordSpacing (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSWordSpacing ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSFont : parse an HTML3 font attribute string      
   we expect the input string describing the attribute to be     
   !!!!!!                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSFont (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSFont (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSFont ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSFontSize : parse an HTML3 font size attr string  
   we expect the input string describing the attribute to be     
   xx-small, x-small, small, medium, large, x-large, xx-large      
   or an absolute size, or an imcrement relative to the parent     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSFontSize (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSFontSize (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   PresentationValue   pval;

   SKIP_BLANK (attrstr);
   if (IS_WORD (attrstr, "xx-small"))
     {
	pval.typed_data.unit = DRIVERP_UNIT_REL;
	pval.typed_data.value = 1;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "x-small"))
     {
	pval.typed_data.unit = DRIVERP_UNIT_REL;
	pval.typed_data.value = 2;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "small"))
     {
	pval.typed_data.unit = DRIVERP_UNIT_REL;
	pval.typed_data.value = 3;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "medium"))
     {
	pval.typed_data.unit = DRIVERP_UNIT_REL;
	pval.typed_data.value = 4;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "large"))
     {
	pval.typed_data.unit = DRIVERP_UNIT_REL;
	pval.typed_data.value = 5;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "x-large"))
     {
	pval.typed_data.unit = DRIVERP_UNIT_REL;
	pval.typed_data.value = 6;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "xx-large"))
     {
	pval.typed_data.unit = DRIVERP_UNIT_REL;
	pval.typed_data.value = 8;
	SKIP_WORD (attrstr);
     }
   else
     {
	attrstr = ParseHTML3Unit (attrstr, &pval);
	if (pval.typed_data.unit == DRIVERP_UNIT_INVALID)
	  {
	     fprintf (stderr, "invalid font size\n");
	     return (attrstr);
	  }
     }

   /*
    * install the attribute
    */
   if (context->drv->SetFontSize)
      context->drv->SetFontSize (target, context, pval);
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSFontFamily : parse an HTML3 font family string   
   we expect the input string describing the attribute to be     
   a common generic font style name                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSFontFamily (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSFontFamily (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   PresentationValue   font;

   font.typed_data.value = 0;
   font.typed_data.unit = 1;
   SKIP_BLANK (attrstr);
   if (IS_CASE_WORD (attrstr, "times"))
     {
	font.typed_data.value = DRIVERP_FONT_TIMES;
	SKIP_PROPERTY (attrstr);
     }
   else if (IS_CASE_WORD (attrstr, "serif"))
     {
	font.typed_data.value = DRIVERP_FONT_TIMES;
	SKIP_PROPERTY (attrstr);
     }
   else if (IS_CASE_WORD (attrstr, "helvetica"))
     {
	font.typed_data.value = DRIVERP_FONT_HELVETICA;
	SKIP_PROPERTY (attrstr);
     }
   else if (IS_CASE_WORD (attrstr, "sans-serif"))
     {
	font.typed_data.value = DRIVERP_FONT_HELVETICA;
	SKIP_PROPERTY (attrstr);
     }
   else if (IS_CASE_WORD (attrstr, "courier"))
     {
	font.typed_data.value = DRIVERP_FONT_COURIER;
	SKIP_PROPERTY (attrstr);
     }
   else if (IS_CASE_WORD (attrstr, "monospace"))
     {
	font.typed_data.value = DRIVERP_FONT_COURIER;
	SKIP_PROPERTY (attrstr);
     }
   else
     {
	/* !!!!!! manque cursive et fantasy !!!!!!!! */
	MSG ("invalid font familly\n");
	SKIP_PROPERTY (attrstr);
	return (attrstr);
     }

   /*
    * install the new presentation.
    */
   if (context->drv->SetFontFamily)
      context->drv->SetFontFamily (target, context, font);
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSFontWeight : parse an HTML3 font weight string   
   we expect the input string describing the attribute to be     
   extra-light, light, demi-light, medium, demi-bold, bold, extra-bold
   or a number encoding for the previous values                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSFontWeight (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSFontWeight (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   PresentationValue   weight;
   int                 val;

   weight.typed_data.value = 0;
   weight.typed_data.unit = 1;
   SKIP_BLANK (attrstr);
   if (IS_WORD (attrstr, "extra-light"))
     {
	weight.typed_data.value = -3;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "light"))
     {
	weight.typed_data.value = -2;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "demi-light"))
     {
	weight.typed_data.value = -1;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "medium"))
     {
	weight.typed_data.value = 0;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "extra-bold"))
     {
	weight.typed_data.value = +3;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "bold"))
     {
	weight.typed_data.value = +2;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "demi-bold"))
     {
	weight.typed_data.value = +1;
	SKIP_WORD (attrstr);
     }
   else if (sscanf (attrstr, "%d", &val) > 0)
     {
	if ((val < -3) || (val > 3))
	  {
	     fprintf (stderr, "invalid font weight %d\n", val);
	     weight.typed_data.value = 0;
	  }
	else
	   weight.typed_data.value = val;
	SKIP_INT (attrstr);
     }
   else
     {
	fprintf (stderr, "invalid font weight\n");
	return (attrstr);
     }
   /*
    * Here we have to reduce since font weight is not well supported
    * by the Thot presentation API.
    */
   switch (weight.typed_data.value)
	 {
	    case 3:
	    case 2:
	    case 1:
	       weight.typed_data.value = DRIVERP_FONT_BOLD;
	       break;
	    case -3:
	    case -2:
	    case -1:
	       weight.typed_data.value = DRIVERP_FONT_ITALICS;
	       break;
	 }

   /*
    * install the new presentation.
    */
   if (context->drv->SetFontStyle)
      context->drv->SetFontStyle (target, context, weight);
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSFontVariant : parse an HTML3 font variant string     
   we expect the input string describing the attribute to be     
   normal or small-caps
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSFontVariant (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSFontVariant (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   PresentationValue   style;

   style.typed_data.value = 0;
   style.typed_data.unit = 1;
   SKIP_BLANK (attrstr);
   if (IS_WORD (attrstr, "small-caps"))
     {
        /*
	 * Not supported yet, so we use bold for rendering
	 */
	style.typed_data.value = DRIVERP_FONT_BOLD;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "normal"))
     {
	style.typed_data.value = DRIVERP_FONT_ROMAN;
	SKIP_WORD (attrstr);
     }
   else
     {
	MSG ("invalid font variant\n");
	return (attrstr);
     }

   /*
    * install the new presentation.
    */
   if (style.typed_data.value != 0)
     {
	PresentationValue   previous_style;

	if ((context->drv->GetFontStyle) &&
	    (!context->drv->GetFontStyle (target, context, &previous_style)))
	  {
	     if (previous_style.typed_data.value == DRIVERP_FONT_BOLD)
	       {
		  if (style.typed_data.value == DRIVERP_FONT_ITALICS)
		     style.typed_data.value = DRIVERP_FONT_BOLDITALICS;
		  if (style.typed_data.value == DRIVERP_FONT_OBLIQUE)
		     style.typed_data.value = DRIVERP_FONT_BOLDOBLIQUE;
	       }
	     if (context->drv->SetFontStyle)
		context->drv->SetFontStyle (target, context, style);
	  }
	else
	  {
	     if (context->drv->SetFontStyle)
		context->drv->SetFontStyle (target, context, style);
	  }
     }
   return (attrstr);
}


/*----------------------------------------------------------------------
   ParseCSSFontStyle : parse an HTML3 font style string     
   we expect the input string describing the attribute to be     
   italic, oblique or normal                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSFontStyle (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSFontStyle (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   PresentationValue   style;
   PresentationValue   size;

   style.typed_data.value = 0;
   style.typed_data.unit = 1;
   size.typed_data.value = 0;
   size.typed_data.unit = 1;
   SKIP_BLANK (attrstr);
   if (IS_WORD (attrstr, "italic"))
     {
	style.typed_data.value = DRIVERP_FONT_ITALICS;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "oblique"))
     {
	style.typed_data.value = DRIVERP_FONT_OBLIQUE;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "normal"))
     {
	style.typed_data.value = DRIVERP_FONT_ROMAN;
	SKIP_WORD (attrstr);
     }
   else
     {
	MSG ("invalid font style\n");
	return (attrstr);
     }

   /*
    * install the new presentation.
    */
   if (style.typed_data.value != 0)
     {
	PresentationValue   previous_style;

	if ((context->drv->GetFontStyle) &&
	    (!context->drv->GetFontStyle (target, context, &previous_style)))
	  {
	     if (previous_style.typed_data.value == DRIVERP_FONT_BOLD)
	       {
		  if (style.typed_data.value == DRIVERP_FONT_ITALICS)
		     style.typed_data.value = DRIVERP_FONT_BOLDITALICS;
		  if (style.typed_data.value == DRIVERP_FONT_OBLIQUE)
		     style.typed_data.value = DRIVERP_FONT_BOLDOBLIQUE;
	       }
	     if (context->drv->SetFontStyle)
		context->drv->SetFontStyle (target, context, style);
	  }
	else
	  {
	     if (context->drv->SetFontStyle)
		context->drv->SetFontStyle (target, context, style);
	  }
     }
   if (size.typed_data.value != 0)
     {
	PresentationValue   previous_size;

	if ((context->drv->GetFontSize) &&
	    (!context->drv->GetFontSize (target, context, &previous_size)))
	  {
	     /* !!!!!!!!!!!!!!!!!!!!!!!! Unite + relatif !!!!!!!!!!!!!!!! */
	     size.typed_data.value += previous_size.typed_data.value;
	     if (context->drv->SetFontSize)
		context->drv->SetFontSize (target, context, size);
	  }
	else
	  {
	     size.typed_data.value = 10;
	     if (context->drv->SetFontSize)
		context->drv->SetFontSize (target, context, size);
	  }
     }
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSLineSpacing : parse an HTML3 font leading string 
   we expect the input string describing the attribute to be     
   value% or value                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSLineSpacing (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSLineSpacing (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   PresentationValue   lead;

   attrstr = ParseHTML3Unit (attrstr, &lead);
   if (lead.typed_data.unit == DRIVERP_UNIT_INVALID)
     {
	MSG ("invalid font leading\n");
	return (attrstr);
     }
   /*
    * install the new presentation.
    */
   if (context->drv->SetLineSpacing)
      context->drv->SetLineSpacing (target, context, lead);
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSTextDecoration : parse an HTML3 text decor string   
   we expect the input string describing the attribute to be     
   underline, overline, line-through, box, shadowbox, box3d,       
   cartouche, blink or none                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSTextDecoration (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSTextDecoration (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   PresentationValue   decor;

   decor.typed_data.value = 0;
   decor.typed_data.unit = 1;
   SKIP_BLANK (attrstr);
   if (IS_WORD (attrstr, "underline"))
     {
	decor.typed_data.value = Underline;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "overline"))
     {
	decor.typed_data.value = Overline;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "line-through"))
     {
	decor.typed_data.value = CrossOut;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "box"))
     {
	MSG ("the box text-decoration attribute is not yet supported\n");
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "boxshadow"))
     {
	MSG ("the boxshadow text-decoration attribute is not yet supported\n");
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "box3d"))
     {
	MSG ("the box3d text-decoration attribute is not yet supported\n");
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "cartouche"))
     {
	MSG ("the cartouche text-decoration attribute is not yet supported\n");
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "blink"))
     {
	MSG ("the blink text-decoration attribute will not be supported\n");
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "none"))
     {
	decor.typed_data.value = NoUnderline;
	SKIP_WORD (attrstr);
     }
   else
     {
	fprintf (stderr, "invalid text decoration\n");
	return (attrstr);
     }

   /*
    * install the new presentation.
    */
   if (decor.typed_data.value)
     {
	if (context->drv->SetTextUnderlining)
	   context->drv->SetTextUnderlining (target, context, decor);
     }
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSColor : parse an HTML3 color attribute string    
   we expect the input string describing the attribute to be     
   either a color name, a 3 tuple or an hexadecimal encoding.    
   The color used will be approximed from the current color      
   table                                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSColor (char *attrstr, PresentationValue * val)
#else
static char        *ParseCSSColor (attrstr, val)
char               *attrstr;
PresentationValue  *val;
#endif
{
   char                colname[100];
   unsigned short      redval = (unsigned short) -1;
   unsigned short      greenval = 0;	/* composant of each RGB       */
   unsigned short      blueval = 0;	/* default to red if unknown ! */
   int                 i;
   int                 best = 0;	/* best color in list found */

   SKIP_BLANK (attrstr);

   val->typed_data.unit = DRIVERP_UNIT_INVALID;
   val->typed_data.value = 0;

   /*
    * first parse the attribute string
    * NOTE : this can't lookup for color name in
    *        cause  we try first to lokup color name from digits
    *        [0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f]
    */
   if ((*attrstr == '#') ||
       (isxdigit (attrstr[0]) && isxdigit (attrstr[1]) &&
	isxdigit (attrstr[2])))
     {

	if (*attrstr == '#')
	   attrstr++;

	/*
	 * we expect an hexa encoding like F00 or FF0000.
	 */
	if ((!isxdigit (attrstr[0])) || (!isxdigit (attrstr[1])) ||
	    (!isxdigit (attrstr[2])))
	  {
	     fprintf (stderr, "Invalid color encoding %s\n", attrstr - 1);
	     goto failed;
	  }
	else if (!isxdigit (attrstr[3]))
	  {
	     /*
	      * encoded as on 3 digits #F0F 
	      */
	     redval = hexa_val (attrstr[0]) * 16 + hexa_val (attrstr[0]);
	     greenval = hexa_val (attrstr[1]) * 16 + hexa_val (attrstr[1]);
	     blueval = hexa_val (attrstr[2]) * 16 + hexa_val (attrstr[2]);
	  }
	else
	  {
	     if ((!isxdigit (attrstr[4])) || (!isxdigit (attrstr[5])))
	       {
		  fprintf (stderr, "Invalid color encoding %s\n", attrstr - 1);
	       }
	     else
	       {
		  /*
		   * encoded as on 3 digits #FF00FF 
		   */
		  redval = hexa_val (attrstr[0]) * 16 +
		     hexa_val (attrstr[1]);
		  greenval = hexa_val (attrstr[2]) * 16 +
		     hexa_val (attrstr[3]);
		  blueval = hexa_val (attrstr[4]) * 16 +
		     hexa_val (attrstr[5]);
	       }
	  }
	goto found_RGB;
     }
   else if (IS_CASE_WORD (attrstr, "url"))
     {
	char               *url;

	/*
	 * we don't currently support URL just parse it to skip it.
	 */
	attrstr = ParseHTMLURL (attrstr, &url);
	TtaFreeMemory (url);
	return (attrstr);
     }
   else if (isalpha (*attrstr))
     {

	/*
	 * we expect a color name like "red", store it in colname.
	 */
	for (i = 0; i < sizeof (colname) - 1; i++)
	  {
	     if (!(isalnum (attrstr[i])))
	       {
		  attrstr += i;
		  break;
	       }
	     colname[i] = attrstr[i];
	  }
	colname[i] = 0;

	/*
	 * Lookup the color name in our own color name database
	 */
	for (i = 0; i < NBCOLORNAME; i++)
	   if (!strcasecmp (ColornameTable[i].name, colname))
	     {
		redval = ColornameTable[i].red;
		greenval = ColornameTable[i].green;
		blueval = ColornameTable[i].blue;
		goto found_RGB;
	     }
	/*
	 * Lookup the color name in Thot color name database
	 */
	TtaGiveRGB (colname, &redval, &greenval, &blueval);
	goto found_RGB;
     }
   else if ((isdigit (*attrstr)) || (*attrstr == '.'))
     {
	/*
	 * we expect a color defined by it's three components.
	 * like "255 0 0" or "1.0 0.0 0.0"
	 TODO
	 */
     }
 failed:
   val->typed_data.unit = DRIVERP_UNIT_INVALID;
   val->typed_data.value = 0;
   return (attrstr);

 found_RGB:
   best = TtaGetThotColor (redval, greenval, blueval);
   val->typed_data.value = best;
   val->typed_data.unit = DRIVERP_UNIT_REL;

   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSWidth : parse an HTML3 width attribute           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSWidth (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSWidth (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{

   SKIP_BLANK (attrstr);

   /*
    * first parse the attribute string
    */
   if (!strcasecmp (attrstr, "auto"))
     {
	SKIP_WORD (attrstr);
	MSG ("ParseCSSWidth : auto ");
	TODO;
	return (attrstr);
     }
   /*
    * install the new presentation.
    mainview = TtaGetViewFromName(doc, "Document_View");
    TtaGiveBoxSize(elem, doc, mainview, UnPoint, &width, &height);
    new_height = height;
    TtaChangeBoxSize(elem doc, mainview, 0, new_height - height, UnPoint);
    */
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSMarginTop : parse an HTML3 margin-top attribute  
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseCSSMarginTop (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSMarginTop (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   PresentationValue   margin;

   SKIP_BLANK (attrstr);

   /*
    * first parse the attribute string
    */
   attrstr = ParseHTML3Unit (attrstr, &margin);
   if (margin.typed_data.unit == DRIVERP_UNIT_INVALID)
     {
	MSG ("invalid margin top\n");
	return (attrstr);
     }
   if (context->drv->SetVPos)
      context->drv->SetVPos (target, context, margin);
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSMarginBottom : parse an HTML3 margin-bottom      
   attribute                                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSMarginBottom (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSMarginBottom (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   PresentationValue   margin;

   SKIP_BLANK (attrstr);

   /*
    * first parse the attribute string
    */
   attrstr = ParseHTML3Unit (attrstr, &margin);
   if (margin.typed_data.unit == DRIVERP_UNIT_INVALID)
     {
	MSG ("invalid margin top\n");
	return (attrstr);
     }
   if (context->drv->SetVPos)
      context->drv->SetVPos (target, context, margin);
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSHeight : parse an HTML3 height attribute                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSHeight (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSHeight (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   SKIP_BLANK (attrstr);

   /*
    * first parse the attribute string
    */
   if (!strcasecmp (attrstr, "auto"))
     {
	SKIP_WORD (attrstr);
	MSG ("ParseCSSHeight : auto ");
	TODO;
	return (attrstr);
     }
   /*
    * read the value, and if necessary convert to point size
    attrstr = ParseHTML3Unit(attrstr, &new_height, &unit);
    */
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSForeground : parse an HTML3 foreground attribute 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSForeground (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSForeground (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   PresentationValue   best;

   attrstr = ParseCSSColor (attrstr, &best);

   if (best.typed_data.unit == DRIVERP_UNIT_INVALID)
     {
	return (attrstr);
     }
   /*
    * install the new presentation.
    */
   if (context->drv->SetForegroundColor)
      context->drv->SetForegroundColor (target, context, best);
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSBackgroundColor : parse an HTML3 background color attribute 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBackgroundColor (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSBackgroundColor (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   ElementType	         elType;
   PresentationValue     best;
   GenericContext        gblock;
   SpecificContextBlock *sblock;
   SpecificTarget        elem;
   boolean               setColor;

   best.typed_data.unit = DRIVERP_UNIT_INVALID;
   setColor = TRUE;
   if (IS_CASE_WORD (attrstr, "transparent"))
     {
       best.typed_data.value = DRIVERP_PATTERN_NONE;
       best.typed_data.unit = DRIVERP_UNIT_REL;
       if (context->drv->SetFillPattern)
	 context->drv->SetFillPattern (target, context, best);
       return (attrstr);
     }

   attrstr = ParseCSSColor (attrstr, &best);
   if (best.typed_data.unit == DRIVERP_UNIT_INVALID)
     setColor = FALSE;

   if (setColor)
     {
       /*
	* if the background is set on the HTML or BODY element,
	* set the background color for the full window.
	*/
       if (context->drv == &GenericStrategy)
	 {
	   gblock = (GenericContext) context;
	   if ((gblock->type == HTML_EL_HTML)
	       || (gblock->type == HTML_EL_BODY)
	       || (gblock->type == HTML_EL_HEAD))
	     {
	       if (setColor)
		 {
		   CSSSetBackground (gblock->doc, (PSchema) target, best.typed_data.value);
		   setColor = FALSE;
		 }
	     }
	 }
       else if (context->drv == &SpecificStrategy)
	 {
	   sblock = (SpecificContextBlock *) context;
	   elem = (SpecificTarget) target;
	   elType = TtaGetElementType (elem);
	   if ((elType.ElTypeNum == HTML_EL_HTML)
	       || (elType.ElTypeNum == HTML_EL_BODY)
	       || (elType.ElTypeNum == HTML_EL_HEAD))
	     {
	       if (setColor)
		 {
		   TtaSetViewBackgroundColor (sblock->doc, 1, best.typed_data.value);
		   setColor = FALSE;
		 }
	     }
	 }

       if (setColor)
	 {
	   /* install the new presentation. */
	   if (context->drv->SetBackgroundColor)
	     context->drv->SetBackgroundColor (target, context, best);
	   /* thot specificity : need to set fill pattern for background color */
	   best.typed_data.value = DRIVERP_PATTERN_BACKGROUND;
	   best.typed_data.unit = DRIVERP_UNIT_REL;
	   if (context->drv->SetFillPattern)
	     context->drv->SetFillPattern (target, context, best);
	   best.typed_data.value = 1;
	   best.typed_data.unit = DRIVERP_UNIT_REL;
	   if (context->drv->SetShowBox)
	     context->drv->SetShowBox (target, context, best);
	 }
     }

   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSBackgroundImageCallback : Callback called asynchronously by
   FetchImage when a background image has been fetched.
  ----------------------------------------------------------------------*/

typedef struct _BackgroundImageCallbackBlock {
    PresentationTarget target;
    union {
	PresentationContextBlock blk;
	SpecificContextBlock specific;
	GenericContextBlock generic;
    } context;
} BackgroundImageCallbackBlock, *BackgroundImageCallbackPtr;

#ifdef __STDC__
void ParseCSSBackgroundImageCallback (Document doc, Element el, char *file,
                                      void *extra)
#else
void ParseCSSBackgroundImageCallback (doc, el, file, extra)
Document doc;
Element el;
char *file;
void *extra;
#endif
{
   BackgroundImageCallbackPtr callblock = (BackgroundImageCallbackPtr) extra;
   PresentationTarget  target;
   PresentationContext context;
   PresentationValue   image;
   PresentationValue   repeat;
   PresentationValue   unused, value;

   if (callblock == NULL)
     return;
   target = callblock->target;
   context = &callblock->context.blk;

   /*
    * Ok the image was fetched, finish the background-image handling.
    */
   image.pointer = file;
   if (context->drv->SetBgImage)
     context->drv->SetBgImage (target, context, image);

   /*
    * If there is no default repeat mode, enforce a V-Repeat
    */
   if (context->drv->GetPictureMode && context->drv->SetPictureMode)
     {
       if (context->drv->GetPictureMode(target, context, &repeat) < 0)
         {
	   repeat.typed_data.value = DRIVERP_REPEAT;
	   repeat.typed_data.unit = DRIVERP_UNIT_REL;
	   context->drv->SetPictureMode (target, context, repeat);
	 }
     }

   /*
    * If there is no default repeat mode, enforce a V-Repeat
    */
   if (context->drv->SetShowBox)
     {
       value.typed_data.value = 1;
       value.typed_data.unit = DRIVERP_UNIT_REL;
       context->drv->SetShowBox (target, context, value);
     }

   /*
    * Update the Document header if this is a generic rule
    */
   if (context->drv == &GenericStrategy)
       RebuildHTMLStyleHeader(doc);

   /*
    * Update the rendering.
    */
   if (context->drv->UpdatePresentation != NULL)
      context->drv->UpdatePresentation (target, context, unused);

   TtaFreeMemory(callblock);
}

/*----------------------------------------------------------------------
   ParseCSSBackgroundImage : parse an HTML3 BackgroundImage
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBackgroundImage (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSBackgroundImage (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   Element               el;
   GenericContext        gblock;
   SpecificContextBlock *sblock;
   char                 *url;
   BackgroundImageCallbackPtr callblock;
   PresentationValue     image, value;
   char                 *no_bg_image;

       url = NULL;
       if (! IS_CASE_WORD (attrstr, "url"))
	 return (attrstr);
       
       attrstr = ParseHTMLURL (attrstr, &url);

   if (context->destroy == 1)
     {
       /* remove the background image PRule */
       image.pointer = NULL;
       if (context->drv->SetBgImage)
	 context->drv->SetBgImage (target, context, image);
       if (context->drv->GetFillPattern)
	 {
	   if (context->drv->GetFillPattern (target, context, &value) < 0)
	     /* there is no FillPattern rule -> remove ShowBox rule */
	     if (context->drv->SetShowBox)
	       {
		 value.typed_data.value = 1;
		 value.typed_data.unit = DRIVERP_UNIT_REL;
		 context->drv->SetShowBox (target, context, value);
	       }
	 }
     }
   else
     {   
       if (url)
	 {
	   no_bg_image = TtaGetEnvString("NO_BG_IMAGES");
	   if ((no_bg_image != NULL) &&
	       ((!(strcasecmp(no_bg_image,"yes"))) ||
		(!(strcasecmp(no_bg_image,"true")))))
	     return (attrstr);
	   
	   /*
	    * if the background is set on the HTML or BODY element,
	    * set the background color for the full window.
	    */
	   if (context->drv == &GenericStrategy)
	     {
	       gblock = (GenericContext) context;
	       callblock = (BackgroundImageCallbackPtr)
		 TtaGetMemory(sizeof(BackgroundImageCallbackBlock));
	       if (callblock != NULL) {
		 callblock->target = target;
		 memcpy(&callblock->context.generic, gblock,
			sizeof(GenericContextBlock));
		 
		 /* fetch and display background image of element */
		 el = TtaGetMainRoot (gblock->doc);
		 
		 FetchImage (gblock->doc, el, url, 0,
			     ParseCSSBackgroundImageCallback,
			     callblock);
	       }
	     }
	   else if (context->drv == &SpecificStrategy)
	     {
	       sblock = (SpecificContextBlock *) context;
	       callblock = (BackgroundImageCallbackPtr)
		 TtaGetMemory(sizeof(BackgroundImageCallbackBlock));
	       if (callblock != NULL) {
		 callblock->target = target;
		 memcpy(&callblock->context.specific, sblock,
			sizeof(SpecificContextBlock));
		 
		 /* fetch and display background image of element */
		 el = TtaGetMainRoot (sblock->doc);
		 
		 FetchImage (sblock->doc, el, url, 0,
			     ParseCSSBackgroundImageCallback,
			     callblock);
	       }
	     }
	 }
       
       if (url)
	 TtaFreeMemory (url);
     }
       return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSBackgroundRepeat : parse an HTML3 BackgroundRepeat
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBackgroundRepeat (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSBackgroundRepeat (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   PresentationValue   repeat;

   repeat.typed_data.value = 0;
   repeat.typed_data.unit = 1;
   SKIP_BLANK (attrstr);
   if (IS_WORD (attrstr, "no-repeat"))
     {
	repeat.typed_data.value = DRIVERP_SCALE;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "repeat-y"))
     {
	repeat.typed_data.value = DRIVERP_VREPEAT;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "repeat-x"))
     {
	repeat.typed_data.value = DRIVERP_HREPEAT;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "repeat"))
     {
	repeat.typed_data.value = DRIVERP_REPEAT;
	SKIP_WORD (attrstr);
     }
   else
     {
	fprintf (stderr, "invalid repeat\n");
	return (attrstr);
     }

   /*
    * install the new presentation.
    */
   if (context->drv->SetPictureMode)
       context->drv->SetPictureMode (target, context, repeat);
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSBackgroundAttachment : parse an HTML3 BackgroundAttachment
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBackgroundAttachment (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSBackgroundAttachment (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSBackgroundAttachment ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSBackgroundPosition : parse an HTML3 BackgroundPosition
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBackgroundPosition (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSBackgroundPosition (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   MSG ("ParseCSSBackgroundPosition ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseCSSBackground : parse an HTML3 background attribute 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBackground (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseCSSBackground (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;
#endif
{
   ElementType	         elType;
   Element               el;
   PresentationValue     best;
   GenericContext        gblock;
   SpecificContextBlock *sblock;
   SpecificTarget        elem;
   char                 *url;
   char                 *no_bg_image;
   boolean               setColor;
   BackgroundImageCallbackPtr callblock;

   url = NULL;
   best.typed_data.unit = DRIVERP_UNIT_INVALID;
   setColor = TRUE;
   if (IS_CASE_WORD (attrstr, "url"))
     /*
      * we don't currently support URL just parse it to skip it.
      */
     attrstr = ParseHTMLURL (attrstr, &url);

   no_bg_image = TtaGetEnvString("NO_BG_IMAGES");
   if ((no_bg_image != NULL) &&
       ((!(strcasecmp(no_bg_image,"yes"))) ||
	(!(strcasecmp(no_bg_image,"true"))))) {
       if (url) TtaFreeMemory (url);
       url = NULL;
   }
       

   attrstr = ParseCSSColor (attrstr, &best);
   if (best.typed_data.unit == DRIVERP_UNIT_INVALID)
     setColor = FALSE;

   if (url || setColor)
     {
       /*
	* if the background is set on the HTML or BODY element,
	* set the background color for the full window.
	*/
       if (context->drv == &GenericStrategy)
	 {
	   gblock = (GenericContext) context;
	   if ((gblock->type == HTML_EL_HTML)
	       || (gblock->type == HTML_EL_BODY)
	       || (gblock->type == HTML_EL_HEAD))
	     {
	       if (setColor)
		 {
		   CSSSetBackground (gblock->doc, (PSchema) target,
		                     best.typed_data.value);
		   setColor = FALSE;
		 }
	       if (url)
		 {
		   callblock = (BackgroundImageCallbackPtr)
		       TtaGetMemory(sizeof(BackgroundImageCallbackBlock));
		   if (callblock != NULL) {
		       callblock->target = target;
		       memcpy(&callblock->context.generic, gblock,
		              sizeof(GenericContextBlock));

		       /* fetch and display background image of element */
		       el = TtaGetMainRoot (gblock->doc);

		       FetchImage (gblock->doc, el, url, 0,
		                   ParseCSSBackgroundImageCallback,
				   callblock);
		   }
		 }
	     }
	 }
       else if (context->drv == &SpecificStrategy)
	 {
	   sblock = (SpecificContextBlock *) context;
	   elem = (SpecificTarget) target;
	   elType = TtaGetElementType (elem);
	   if ((elType.ElTypeNum == HTML_EL_HTML)
	       || (elType.ElTypeNum == HTML_EL_BODY)
	       || (elType.ElTypeNum == HTML_EL_HEAD))
	     {
	       if (setColor)
		 {
		   TtaSetViewBackgroundColor (sblock->doc, 1,
		                              best.typed_data.value);
		   setColor = FALSE;
		 }
	       if (url)
		 {
		   callblock = (BackgroundImageCallbackPtr)
		       TtaGetMemory(sizeof(BackgroundImageCallbackBlock));
		   if (callblock != NULL) {
		       callblock->target = target;
		       memcpy(&callblock->context.specific, sblock,
		              sizeof(SpecificContextBlock));

		       /* fetch and display background image of element */
		       el = TtaGetMainRoot (sblock->doc);

		       FetchImage (sblock->doc, el, url, 0,
		                   ParseCSSBackgroundImageCallback,
				   callblock);
		   }
		 }
	     }
	 }

       if (setColor)
	 {
	   /* install the new presentation. */
	   if (context->drv->SetBackgroundColor)
	     context->drv->SetBackgroundColor (target, context, best);
	   /* thot specificity : need to set fill pattern for background color */
	   best.typed_data.value = DRIVERP_PATTERN_BACKGROUND;
	   best.typed_data.unit = DRIVERP_UNIT_REL;
	   if (context->drv->SetFillPattern)
	     context->drv->SetFillPattern (target, context, best);
	   best.typed_data.value = 1;
	   best.typed_data.unit = DRIVERP_UNIT_REL;
	   if (context->drv->SetShowBox)
	     context->drv->SetShowBox (target, context, best);
	 }
     }

   if (url)
     TtaFreeMemory (url);
   return (attrstr);
}

/************************************************************************
 *									*  
 *	PARSING FUNCTIONS FOR HEADER STYLE DECLARATIONS			*
 *									*  
 ************************************************************************/

/*----------------------------------------------------------------------
   ParseHTMLStyleDeclaration : parse one HTML style declaration    
   stored in the header of a HTML document                       
   We expect the style string to be of the form :                   
   [                                                                
   e.g: pinky, awful { color: pink, font-family: helvetica }        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ParseHTMLStyleDeclaration (Element elem, char *attrstr, Document doc, PSchema gPres)
#else
void                ParseHTMLStyleDeclaration (elem, attrstr, doc, gPres)
Element             elem;
char               *attrstr;
Document            doc;
PSchema             gPres;
#endif
{
   char               *decl_end;
   char               *sel_end;
   char               *sel;
   char                sauve1;
   char                sauve2;

   /*
    * separate the selectors string.
    */
   decl_end = attrstr;
   while ((*decl_end != 0) && (*decl_end != '{'))
      decl_end++;
   if (*decl_end == 0)
     {
	fprintf (stderr, "Invalid STYLE declaration header : %s\n", attrstr);
	return;
     }
   /*
    * verify and clean the selector string.
    */
   sel_end = decl_end - 1;
   while (((*(sel_end)) == SPACE) || ((*(sel_end)) == '\b') ||
	  ((*(sel_end)) == EOL) || ((*(sel_end)) == '\r'))
      sel_end--;
   sel_end++;
   sauve1 = *sel_end;
   *sel_end = 0;
   sel = attrstr;

   /*
    * now, deal with the content ...
    */
   decl_end++;
   attrstr = decl_end;
   while ((*decl_end != 0) && (*decl_end != '}'))
      decl_end++;
   if (*decl_end == 0)
     {
	fprintf (stderr, "Invalid STYLE declaration : %s\n", attrstr);
	return;
     }
   sauve2 = *decl_end;
   *decl_end = 0;

   /*
    * parse the style attribute string and install the corresponding
    * presentation attributes on the new element
    */
   ParseHTMLGenericStyle (sel, attrstr, doc, gPres);

   /* restore the string to its original form ! */
   *sel_end = sauve1;
   *decl_end = sauve2;

}

/*----------------------------------------------------------------------
   ParseHTMLClass : parse an HTML3 Class string                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ParseHTMLClass (Element elem, char *attrstr, Document doc)
#else
void                ParseHTMLClass (elem, attrstr, doc)
Element             elem;
char               *attrstr;
Document            doc;
#endif
{
   ElementType         elType;
   Element             el;
   Attribute           at;
   AttributeType       atType;
   PRule               rule, new;
   char                selector[101];
   int                 len;
   Element             best = NULL;
   int                 score = 0;
   int                 res;

   elType.ElSSchema = TtaGetDocumentSSchema (doc);
   elType.ElTypeNum = HTML_EL_StyleRule;
   el = TtaSearchTypedElement (elType, SearchInTree, TtaGetMainRoot (doc));

   /*
    * browse the style definitions, looking for the given class name.
    */
   if (el == NULL)
     {
#ifdef DEBUG_STYLES
	fprintf (stderr, "Invalid class \"%s\" : no class found\n", attrstr);
#endif
	return;
     }
   while (el != NULL)
     {
	atType.AttrSSchema = TtaGetDocumentSSchema (doc);
	atType.AttrTypeNum = HTML_ATTR_Selector;

	at = TtaGetAttribute (el, atType);
	if (at)
	  {
	     len = 100;
	     TtaGiveTextAttributeValue (at, selector, &len);
	     selector[len + 1] = 0;
	     res = EvaluateClassSelector (elem, attrstr, selector, doc);
	     if (res > score)
	       {
		  best = el;
		  score = res;
	       }
	  }
	else
	  {
	     MSG ("Invalid StyleRule\n");
	  }

	/* get next StyleRule */
	TtaNextSibling (&el);
     }

   /*
    * apply all presentation specific attributes found.
    */
   rule = NULL;
   do
     {
	TtaNextPRule (best, &rule);
	if (rule)
	  {
	     new = TtaCopyPRule (rule);
	     TtaAttachPRule (elem, new, doc);
	  }
     }
   while (rule != NULL);
}

/*----------------------------------------------------------------------
   ParseHTMLStyleSheet : Parse an HTML Style Sheet fragment        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ParseHTMLStyleSheet (char *fragment, Document doc, PSchema gPres)
#else
void                ParseHTMLStyleSheet (fragment, doc, gPres)
char               *fragment;
Document            doc;
PSchema             gPres;
#endif
{
   /* these static variables should pertain to a context block */
   static int          SelecIndex = 0;
   static char         SelecBuffer[1000];
   static int          StyleIndex = 0;
   static char         StyleBuffer[3000];
   static char         in_comment = 0;
   static char         in_selec = 1;
   static char         in_style = 0;
   static char         last;

   if (fragment == NULL)
     {
	/* force the interpetation of the end of the Buffers */
	StyleBuffer[StyleIndex++] = 0;
	SelecBuffer[SelecIndex++] = 0;
	ParseHTMLGenericStyle (SelecBuffer, StyleBuffer,
			       doc, gPres);

	StyleIndex = 0;
	SelecIndex = 0;
	in_selec = 1;
	in_style = 0;
	return;
     }
   while ((*fragment != 0) && (SelecIndex < 1000) &&
	  (StyleIndex < 3000))
     {
	/* comments in external CSS are coded the C way */
	if (in_comment != 0)
	  {
	     if ((last == '*') && (*fragment == '/'))
	       {
		  in_comment = 0;
		  fragment++;
		  continue;
	       }
	     last = *fragment++;
	     continue;
	  }			/* else */
	if (last == '/')
	  {
	     if (*fragment == '*')
	       {
		  /* start of a comment */
		  in_comment = 1;
		  fragment++;
		  continue;
	       }
	     else
	       {
		  /* No comment here, put the slash at the correct location */
		  if (in_selec)
		     SelecBuffer[SelecIndex++] = '/';
		  else if (in_style)
		     StyleBuffer[StyleIndex++] = '/';
	       }
	  }
	last = *fragment++;
	switch (last)
	      {
		 case '/':
		    /* treated on next char to deal with comments */
		    break;
		 case EOL:
		 case TAB:
		 case '\r':
		    if (in_selec)
		       SelecBuffer[SelecIndex++] = SPACE;
		    else if (in_style)
		       StyleBuffer[StyleIndex++] = SPACE;
		    break;
		 case '{':
		    if (in_selec)
		      {
			 SelecBuffer[SelecIndex++] = 0;
			 in_selec = 0;
			 in_style = 1;
		      }
		    break;
		 case '}':
		    /* this is an end one element style specification, parse it */
		    if (in_style)
		      {
			 StyleBuffer[StyleIndex++] = 0;
			 ParseHTMLGenericStyle (SelecBuffer, StyleBuffer,
						doc, gPres);

			 StyleIndex = 0;
			 SelecIndex = 0;
			 in_selec = 1;
			 in_style = 0;
		      }
		    break;
		 default:
		    if (in_selec)
		       SelecBuffer[SelecIndex++] = last;
		    else if (in_style)
		       StyleBuffer[StyleIndex++] = last;
		    break;
	      }
     }
   if ((SelecIndex >= 1000) || (StyleIndex >= 3000))
     {
	/* something went havoc, reset the parser */
	MSG ("ParseHTMLStyleSheet : parser error, resetting\n");
	StyleIndex = 0;
	SelecIndex = 0;
	in_selec = 1;
	in_style = 0;
	in_comment = 0;
     }

   if (NonPPresentChanged)
     ApplyExtraPresentation (doc);
}

/************************************************************************
 *									*  
 *	EVALUATION FUNCTIONS / CASCADING AND OVERLOADING		*
 *									*  
 ************************************************************************/

/*----------------------------------------------------------------------
   EvaluateClassContext : gives a score for an element in a tree   
   in function of a selector. Three argument enter in the          
   evaluation process :                                            
   - the class name associated to the element                    
   - the selector string associated to the rule                  
   - the element and it's place in the tree                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 EvaluateClassContext (Element elem, char *class, char *selector, Document doc)
#else
int                 EvaluateClassContext (elem, class, selector, doc)
Element             elem;
char               *class;
char               *selector;
Document            doc;
#endif
{
   Element             father;
   char               *elHtmlName;
   char               *end_str;
   char               *sel = selector;
   int                 result = 0;
   char               *names[MAX_DEEP];

#ifdef DEBUG_STYLES
   int                 i;
   int                 deep;

#endif

   elHtmlName = GetHTML3Name (elem, doc);
   GetHTML3Names (elem, doc, &names[0], MAX_DEEP);

#ifdef DEBUG_STYLES
   fprintf (stderr, "EvaluateClassContext(%s,%s,%s,%d)\n",
	    elHtmlName, class, selector, doc);
   fprintf (stderr, "Context name : ");
   for (i = 0; i < deep; i++)
      fprintf (stderr, "%s ", names[i]);
   fprintf (stderr, "\n");
#endif

   /*
    * look for a selector (ELEM)
    */
   SKIP_BLANK (selector);
   if (*selector == '(')
     {
	for (end_str = selector; *end_str; end_str++)
	   if (*end_str == ')')
	      break;
	if (*end_str != ')')
	  {
	     fprintf (stderr, "Unmatched '(' in selector \"%s\"\n", sel);
	  }
	else
	  {
	     /*
	      * separate the father name, and evaluate it.
	      */
	     *end_str = 0;
	     father = TtaGetParent (elem);
	     result = EvaluateClassContext (father, class, selector + 1, doc);
	     *end_str = ')';

	     if (result)
	       {
		  /*
		   * verify that the end of the string match the current element.
		   */
		  if (EvaluateClassContext (elem, class, end_str + 1, doc))
		     result *= 10;
		  else
		     result = 0;
	       }
	  }
     }
   if (!result)
     {
	if (!strcasecmp (class, elHtmlName))
	   result = 1000;
	else if (!strcasecmp (class, selector))
	   result = 100;
     }
#ifdef DEBUG_STYLES
   fprintf (stderr, "EvaluateClassContext(%s,%s,%s,%d) : %d\n",
	    elHtmlName, class, sel, doc, result);
#endif
   return (result);
}

/*----------------------------------------------------------------------
   EvaluateClassSelector : gives a score for an element in a tree  
   in function of a selector. Three argument enter in the          
   evaluation process :                                            
   - the class name associated to the element                    
   - the selector string associated to the rule                  
   - the element and it's place in the tree                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 EvaluateClassSelector (Element elem, char *class, char *selector, Document doc)
#else
int                 EvaluateClassSelector (elem, class, selector, doc)
Element             elem;
char               *class;
char               *selector;
Document            doc;
#endif
{
   int                 l = strlen (class);
   int                 L = strlen (selector);
   int                 val = 0;

   val = EvaluateClassContext (elem, class, selector, doc);
   if (val)
      return (val);

   if (L < l)
      return (0);

   /*
    * first approximation based on substrings .... :-( !!!!!!!!!!
    */
   while (*selector != 0)
      if ((*selector == *class) && (!strncmp (class, selector, l)))
	 return (val = ((l * 1000) / L));
      else
	 selector++;

   return (val);
}

/************************************************************************
 *									*  
 *		LIBRARY FUNCTIONS of GENERAL INTERREST			*
 *									*  
 ************************************************************************/

/*----------------------------------------------------------------------
   CreateWWWElement : find or create an element of a given type in   
   the document structure.                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Element             CreateWWWElement (Document doc, int type)
#else
Element             CreateWWWElement (doc, type)
Document            doc;
int                 type;
#endif
{
   ElementType         elType;
   Element             el;

   /*
    * try to locate the element.
    */
   elType.ElSSchema = TtaGetDocumentSSchema (doc);
   elType.ElTypeNum = type;
   el = TtaSearchTypedElement (elType, SearchInTree, TtaGetMainRoot (doc));
   if (el)
      return (el);

   /*
    * not found, create it !
    */
   return (CreateNewWWWElement (doc, type));
}

/*----------------------------------------------------------------------
   CreateNewWWWElement : create a new element of a given type in     
   the document structure.                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Element             CreateNewWWWElement (Document doc, int type)
#else
Element             CreateNewWWWElement (doc, type)
Document            doc;
int                 type;
#endif
{
   Element             father = NULL;
   ElementType         elType;
   Element             el;

   /*
    * find the father, create it if necessary.
    */
   switch (type)
	 {
	    case HTML_EL_HEAD:
	       father = TtaGetMainRoot (doc);
	       break;
	    case HTML_EL_Styles:
	       father = CreateWWWElement (doc, HTML_EL_HEAD);
	       break;
	    case HTML_EL_StyleRule:
	       father = CreateWWWElement (doc, HTML_EL_Styles);
	       break;
	    case HTML_EL_Links:
	       father = CreateWWWElement (doc, HTML_EL_HEAD);
	       break;
	    case HTML_EL_LINK:
	       father = CreateWWWElement (doc, HTML_EL_Links);
	       break;
	    default:
#ifdef AMAYA_DEBUG
	       fprintf (stderr, "Don't know how to create father of type %d\n", type);
#endif
	       return (NULL);
	 }
   if (!father)
     {
#ifdef AMAYA_DEBUG
	fprintf (stderr, "Unable to get/create father of type %d\n", type);
#endif
	return (NULL);
     }
   /*
    * create a new element and prune it in the structure.
    */
   elType.ElSSchema = TtaGetDocumentSSchema (doc);
   elType.ElTypeNum = type;
   if (type == HTML_EL_LINK)
      el = TtaNewTree (doc, elType, "");
   else
      el = TtaNewElement (doc, elType);
   if (!el)
     {
#ifdef AMAYA_DEBUG
	fprintf (stderr, "Unable to create type %d\n", type);
#endif
	return (NULL);
     }
   /*
    * pre processing : eg create mandatory attributes ...
    */
   switch (type)
	 {
	    case HTML_EL_LINK:
	       {
		  Attribute           at;
		  AttributeType       atType;

		  atType.AttrSSchema = TtaGetDocumentSSchema (doc);
		  atType.AttrTypeNum = HTML_ATTR_HREF_;
		  at = TtaNewAttribute (atType);
		  if (at)
		    {
		       TtaAttachAttribute (el, at, doc);
		    }
		  break;
	       }
	    default:
	       break;
	 }
   TtaInsertFirstChild (&el, father, doc);
   /*
    * post processing : eg create child ...
    */
   switch (type)
	 {
	/******
        case HTML_EL_StyleRule: {
	    Element         contenu;

	    elType.ElSSchema = TtaGetDocumentSSchema(doc);
	    elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	    contenu = TtaNewElement(doc, elType);
	    TtaInsertFirstChild(&contenu, el, doc);
            break;
        }
	 *****/
	    default:
	       break;
	 }
   return (el);
}

/*----------------------------------------------------------------------
   IsImplicitClassName : return wether the Class name is an        
   implicit one, eg "H1" or "H2 EM" meaning it's a GI name       
   or an HTML context name.                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 IsImplicitClassName (char *class, Document doc)
#else
int                 IsImplicitClassName (class, doc)
char               *class;
Document            doc;
#endif
{
   char                name[200];
   char               *cur = &name[0], *first, save;

   /* make a local copy */
   strncpy (name, class, 199);
   name[199] = 0;

   /* loop looking if each word is a GI */
   while (*cur != 0)
     {
	first = cur;
	SKIP_WORD (cur);
	save = *cur;
	*cur = 0;
	if (MapGI (first) == -1)
	  {
	     return (0);
	  }
	*cur = save;
	SKIP_BLANK (cur);
     }
#ifdef DEBUG_STYLES
   fprintf (stderr, "IsImplicitClassName(%s)\n", class);
#endif
   return (1);
}

/*----------------------------------------------------------------------
   RemoveStyleRule : removes the corresponding class.              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RemoveStyleRule (Element elClass, Document doc)
#else
void                RemoveStyleRule (elem, doc)
Element             elem;
Document            doc;
#endif
{
   Attribute           at;
   AttributeType       atType;
   int                 len;
   char                name[100];

   if (elClass == NULL)
      return;

   /*
    * get the name of the class.
    */
   atType.AttrSSchema = TtaGetDocumentSSchema (doc);
   atType.AttrTypeNum = HTML_ATTR_Selector;

   at = TtaGetAttribute (elClass, atType);
   if (at)
     {
	len = 100;
	TtaGiveTextAttributeValue (at, name, &len);
     }
   else
     {
#ifdef AMAYA_DEBUG
	fprintf (stderr, "RemoveStyleRule(%d), invalid element\n", doc);
#endif
	return;
     }
#ifdef DEBUG_STYLES
   fprintf (stderr, "RemoveStyleRule(%d,%s)\n", doc, name);
#endif

   /*
    * Remove the element fom the StyleRule List.
    */
   TtaRemoveTree (elClass, doc);
}

/*----------------------------------------------------------------------
   RemoveStyle : clean all the presentation attributes of          
   a given element.                                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RemoveStyle (Element elem, Document doc)
#else
void                RemoveStyle (elem, doc)
Element             elem;
Document            doc;
#endif
{
   Attribute           at;
   AttributeType       atType;
   PresentationTarget  target;
   SpecificContextBlock block;
   PresentationContext context;
   PresentationValue   unused;

   unused.data = 0;
#ifdef DEBUG_STYLES
   char               *elHtmlName;

#endif

   if (elem == NULL)
      return;

#ifdef DEBUG_STYLES
   elHtmlName = GetHTML3Name (elem, doc);
   fprintf (stderr, "RemoveStyle(%s,%d)\n", elHtmlName, doc);
#endif

   /*
    * remove any Class or ImplicitClass associated to the element.
    */
   atType.AttrSSchema = TtaGetDocumentSSchema (doc);
   atType.AttrTypeNum = HTML_ATTR_Class;

   at = TtaGetAttribute (elem, atType);
   if (at != NULL)
     {
	TtaRemoveAttribute (elem, at, doc);
	DeleteSpanIfNoAttr (elem, doc);
     }
   atType.AttrSSchema = TtaGetDocumentSSchema (doc);
   atType.AttrTypeNum = HTML_ATTR_Style_;

   at = TtaGetAttribute (elem, atType);
   if (at != NULL)
     {
	TtaRemoveAttribute (elem, at, doc);
	DeleteSpanIfNoAttr (elem, doc);
     }
   /*
    * remove all the presentation specific rules applied to the element.
    */
   context = (PresentationContext) & block;
   target = (PresentationTarget) elem;
   block.drv = &SpecificStrategy;
   block.doc = doc;
   block.schema = TtaGetDocumentSSchema (doc);
   if (context->drv->CleanPresentation != NULL)
      context->drv->CleanPresentation (target, context, unused);
}

/*----------------------------------------------------------------------
   SetHTMLStyleParserDestructiveMode :                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetHTMLStyleParserDestructiveMode (boolean mode)
#else
void                SetHTMLStyleParserDestructiveMode (mode)
boolean                mode;
#endif
{
#ifdef DEBUG_CSS
   if (mode != HTMLStyleParserDestructiveMode)
     {
	if (mode)
	   fprintf (stderr, "Switching HTML Style parser to destroy mode\n");
	else
	   fprintf (stderr, "Switching HTML Style back to normal mode\n");
     }
#endif

   HTMLStyleParserDestructiveMode = mode;
}

/************************************************************************
 *									*  
 *  Functions Needed for support of HTML 3.2 : translate to CSS equiv   *
 *									*  
 ************************************************************************/

/*----------------------------------------------------------------------
   HTMLSetBackgroundColor :
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLSetBackgroundColor (Document doc, Element elem, char *color)
#else
void                HTMLSetBackgroundColor (doc, elem, color)
Document            doc;
Element             elem;
char               *color;
#endif
{
   char                css_command[100];

   sprintf (css_command, "background: %s", color);
   ParseHTMLSpecificStyle (elem, css_command, doc);
}

/*----------------------------------------------------------------------
   HTMLSetBackgroundImage :
   repeat = repeat value
   image = url of background image
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLSetBackgroundImage (Document doc, Element elem, int repeat, char *image)
#else
void                HTMLSetBackgroundImage (doc, elem, repeat, image)
Document            doc;
Element             elem;
int                 repeat;
char               *image;
#endif
{
   char                css_command[100];

   if (image[0] != DIR_SEP)
     sprintf (css_command, "background-image: url(%s); background-repeat: ", image);
   else
     sprintf (css_command, "background-image: url(file:/%s); background-repeat: ", image);
   if (repeat == DRIVERP_REPEAT)
     strcat (css_command, "repeat");
   else if (repeat == DRIVERP_HREPEAT)
     strcat (css_command, "repeat-x");
   else if (repeat == DRIVERP_VREPEAT)
     strcat (css_command, "repeat-y");
   else
     strcat (css_command, "no-repeat");
   ParseHTMLSpecificStyle (elem, css_command, doc);
}

/*----------------------------------------------------------------------
   HTMLSetForegroundColor :                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLSetForegroundColor (Document doc, Element elem, char *color)
#else
void                HTMLSetForegroundColor (doc, elem, color)
Document            doc;
Element             elem;
char               *color;
#endif
{
   char                css_command[100];

   sprintf (css_command, "color: %s", color);
   ParseHTMLSpecificStyle (elem, css_command, doc);
}

/*----------------------------------------------------------------------
   HTMLResetBackgroundColor :                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLResetBackgroundColor (Document doc, Element elem)
#else
void                HTMLResetBackgroundColor (doc, elem)
Document            doc;
Element             elem;
#endif
{
   char                css_command[100];

   sprintf (css_command, "background: xx"       );
   SetHTMLStyleParserDestructiveMode (TRUE);
   ParseHTMLSpecificStyle (elem, css_command, doc);
   SetHTMLStyleParserDestructiveMode (FALSE);
}

/*----------------------------------------------------------------------
   HTMLResetBackgroundImage :                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLResetBackgroundImage (Document doc, Element elem)
#else
void                HTMLResetBackgroundImage (doc, elem)
Document            doc;
Element             elem;
#endif
{
   char                css_command[1000];

   sprintf (css_command, "background-image: url(xx); background-repeat: repeat");
   SetHTMLStyleParserDestructiveMode (TRUE);
   ParseHTMLSpecificStyle (elem, css_command, doc);
   SetHTMLStyleParserDestructiveMode (FALSE);
}

/*----------------------------------------------------------------------
   HTMLResetForegroundColor :                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLResetForegroundColor (Document doc, Element elem)
#else
void                HTMLResetForegroundColor (doc, elem)
Document            doc;
Element             elem;
#endif
{
   char                css_command[100];

   sprintf (css_command, "color: xx");
   SetHTMLStyleParserDestructiveMode (TRUE);
   ParseHTMLSpecificStyle (elem, css_command, doc);
   SetHTMLStyleParserDestructiveMode (FALSE);
}

/*----------------------------------------------------------------------
   HTMLSetAlinkColor :                                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLSetAlinkColor (Document doc, char *color)
#else
void                HTMLSetAlinkColor (doc, color)
Document            doc;
char               *color;
#endif
{
   char                css_command[100];

   sprintf (css_command, "A:link { color : %s }", color);
   ParseHTMLStyleHeader (NULL, css_command, doc, TRUE);
}

/*----------------------------------------------------------------------
   HTMLSetAactiveColor :                                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLSetAactiveColor (Document doc, char *color)
#else
void                HTMLSetAactiveColor (doc, color)
Document            doc;
char               *color;
#endif
{
   char                css_command[100];

   sprintf (css_command, "A:active { color : %s }", color);
   ParseHTMLStyleHeader (NULL, css_command, doc, TRUE);
}

/*----------------------------------------------------------------------
   HTMLSetAvisitedColor :                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLSetAvisitedColor (Document doc, char *color)
#else
void                HTMLSetAvisitedColor (doc, color)
Document            doc;
char               *color;
#endif
{
   char                css_command[100];

   sprintf (css_command, "A:visited { color : %s }", color);
   ParseHTMLStyleHeader (NULL, css_command, doc, TRUE);
}

/*----------------------------------------------------------------------
   HTMLResetAlinkColor :                                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLResetAlinkColor (Document doc)
#else
void                HTMLResetAlinkColor (doc)
Document            doc;
#endif
{
   char                css_command[100];

   sprintf (css_command, "A:link { color : red }");
   SetHTMLStyleParserDestructiveMode (TRUE);
   ParseHTMLStyleHeader (NULL, css_command, doc, TRUE);
   SetHTMLStyleParserDestructiveMode (FALSE);
}

/*----------------------------------------------------------------------
   HTMLResetAactiveColor :                                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLResetAactiveColor (Document doc)
#else
void                HTMLResetAactiveColor (doc)
Document            doc;
#endif
{
   char                css_command[100];

   sprintf (css_command, "A:active { color : red }");
   SetHTMLStyleParserDestructiveMode (TRUE);
   ParseHTMLStyleHeader (NULL, css_command, doc, TRUE);
   SetHTMLStyleParserDestructiveMode (FALSE);
}

/*----------------------------------------------------------------------
   HTMLResetAvisitedColor :                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLResetAvisitedColor (Document doc)
#else
void                HTMLResetAvisitedColor (doc)
Document            doc;
#endif
{
   char                css_command[100];

   sprintf (css_command, "A:visited { color : red }");
   SetHTMLStyleParserDestructiveMode (TRUE);
   ParseHTMLStyleHeader (NULL, css_command, doc, TRUE);
   SetHTMLStyleParserDestructiveMode (FALSE);
}
