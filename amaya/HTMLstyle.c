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
 * Authors: D. Veillard and I. Vatton
 *
 */

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"

#include "AHTURLTools_f.h"
#include "HTMLpresentation_f.h"
#include "HTMLstyle_f.h"
#include "HTMLimage_f.h"
#include "UIcss_f.h"
#include "css_f.h"
#include "html2thot_f.h"

#define MAX_BUFFER_LENGTH 200
#define MAX_CSS_LENGTH 2000
static char         CSSbuffer[MAX_CSS_LENGTH + 1];
static int          CSSindex = 0;
static int          CSScomment = MAX_CSS_LENGTH;
static boolean      HTMLcomment = FALSE;
extern boolean      NonPPresentChanged;

/* CSSLEVEL2 adding new features to the standard */
/* DEBUG_STYLES verbose output of style actions */
/* DEBUG_CLASS_INTERF verbose output on class interface actions */

#define MAX_DEEP 10
#include "HTMLstyleColor.h"



/*----------------------------------------------------------------------
   CSSparser :  is the front-end function called by the HTML parser
   when detecting a <STYLE TYPE="text/css"> indicating it's the
   beginning of a CSS fragment. readfunc is a function used to
   read one character at a time from the input stream.
  
   The CSS parser has to handle <!-- ... --> constructs used to
   prevent prehistoric browser from displaying the CSS as a text
   content. It will stop on any sequence "<x" where x is different
   from ! and will return x as to the caller. Theorically x should
   be equal to / for the </STYLE> end of style. In this case CSSparsing 
   will be FALSE.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char                CSSparser (Document doc, boolean *CSSparsing)
#else
char                CSSparser (doc, CSSparsing)
AmayaReadChar       readfunc;
Document            doc;
boolean            *CSSparsing;
#endif
{
  char                c;
  boolean             toParse;

  *CSSparsing = TRUE;
  toParse = FALSE;
  c = SPACE;
  while (CSSindex < MAX_CSS_LENGTH && c != EOS && *CSSparsing)
    {
      c = GetNextInputChar ();
      CSSbuffer[CSSindex] = c;
      switch (c)
	{
	case '*':
	  if (CSSindex > 0 && CSSbuffer[CSSindex - 1] == '/')
	    /* start a comment */
	    CSScomment = CSSindex - 1;
	  break;
	case '/':
	  if (CSSindex > 0 && CSSbuffer[CSSindex - 1] == '*' && CSScomment != MAX_CSS_LENGTH)
	    {
	      /* close a comment */
	      CSSindex = CSScomment - 1; /* incremented later */
	      CSScomment = MAX_CSS_LENGTH;
	    }
	  else if (CSSindex > 0 && CSSbuffer[CSSindex - 1] ==  '<')
	    {
	      /* this is the closing tag ! */
	      *CSSparsing = FALSE;
	      CSSindex -= 2; /* remove </ from the CSS string */
	    }	    
	  break;
	case '<':
	  c = GetNextInputChar ();
	  if (c == '!')
	    {
	      if (CSSindex > 0)
		/* Ok we consider this as a closing tag ! */
		*CSSparsing = FALSE;
	      else
		{
		  /* CSS within an HTML comment */
		  HTMLcomment = TRUE;
		  CSSindex++;
		  CSSbuffer[CSSindex] = c;
		}
	    }
	  else if (c == '/')
	    {
	      CSSindex--;
	      /* Ok we consider this as a closing tag ! */
	      *CSSparsing = FALSE;
	    }
	  else if (c == EOS)
	    CSSindex++;
	  break;
	case '-':
	  if (CSSindex > 0 && CSSbuffer[CSSindex - 1] == '-' && HTMLcomment)
	    /* CSS within an HTML comment */
	    CSSindex = - 1; /* incremented later */
	  break;
	case '>':
	  if (HTMLcomment)
	    {
	      CSSindex--;
	      toParse = TRUE;
	    }
	  break;
	case '}':
	  toParse = TRUE;
	  break;
	}
      if (c != EOS)
	CSSindex++;
      if  (CSSindex >= MAX_CSS_LENGTH || !*CSSparsing || toParse)
	{
	  CSSbuffer[CSSindex] = EOS;
	  /* parse a not empty string */
	  if (CSSindex > 0)
	    ParseHTMLStyleHeader (NULL, CSSbuffer, doc, TRUE);
	  toParse = FALSE;
	  CSSindex = 0;
	}
    }
  return (c);
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
 * A HTMLStyleValueParser is a function used to parse  the
 * description substring associated to a given style attribute
 * e.g. : "red" for a color attribute or "12pt bold helvetica"
 * for a font attribute.
 */
#ifdef __STDC__
typedef char       *(*HTMLStyleValueParser)
                    (PresentationTarget target,
		     PresentationContext context, char *cssRule);
#else
typedef char       *(*HTMLStyleValueParser) ();
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
			PresentationContext context, char *cssRule);
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
 * Description of the set of CSS Style Attributes supported.
 */

typedef struct HTMLStyleAttribute
  {
     char               *name;
     HTMLStyleValueParser parsing_function;
  }
HTMLStyleAttribute;

/*
 * NOTE : Long attribute name MUST be placed before shortened ones !
 *        e.g. "FONT-SIZE" must be placed before "FONT"
 */

static HTMLStyleAttribute HTMLStyleAttributes[] =
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

#define NB_CSSSTYLEATTRIBUTE (sizeof(HTMLStyleAttributes) / \
                                sizeof(HTMLStyleAttributes[0]))

/*
 * A few macro needed to help building the parser
 */

#define ERR -1000000
#ifdef AMAYA_DEBUG
#define TODO { fprintf(stderr, "code incomplete file %s line %d\n",\
                       __FILE__,__LINE__); };
#define MSG(msg) fprintf(stderr, msg)
#else /* AMAYA_DEBUG */
#define TODO
#define MSG(msg)
#endif


/************************************************************************
 *									*  
 * 			 UNITS CONVERSION FUNCTIONS			*
 *									*  
 ************************************************************************/

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

#define UNIT_INVALID	0
#define UNIT_POINT	1
#define UNIT_EM		2
#define UNIT_PIXEL	3
struct unit_def
{
   char               *sign;
   int                 unit;
};
static struct unit_def CSSUnitNames[] =
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

#define NB_UNITS (sizeof(CSSUnitNames) / sizeof(struct unit_def))
/*----------------------------------------------------------------------
   SkipBlanks:                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char        *SkipBlanks (char *ptr)
#else
char        *SkipBlanks (ptr)
char               *ptr;
#endif
{
  while (*ptr == SPACE || *ptr == '\b' ||  *ptr == '\n' || *ptr == '\r')
    ptr++;
  return (ptr);
}
/*----------------------------------------------------------------------
   IsBlank:                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean     IsBlank (char *ptr)
#else
boolean     IsBlank (ptr)
char               *ptr;
#endif
{
  if (*ptr == SPACE || *ptr == '\b' ||  *ptr == '\n' || *ptr == '\r')
    return (TRUE);
  else
    return (FALSE);
}

/*----------------------------------------------------------------------
   SkipWord:                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *SkipWord (char *ptr)
#else
static char        *SkipWord (ptr)
char               *ptr;
#endif
{
  while (isalnum(*ptr) || *ptr == '-' || *ptr == '%')
    ptr++;
  return (ptr);
}
/*----------------------------------------------------------------------
   SkipProperty:                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char        *SkipProperty (char *ptr)
#else
char        *SkipProperty (ptr)
char               *ptr;
#endif
{
  while (*ptr != EOS && *ptr != ';' && *ptr != '}' && *ptr != ',')
    ptr++;
  return (ptr);
}

/*----------------------------------------------------------------------
   ParseCSSUnit :                                                  
   parse a CSS Unit substring and returns the corresponding      
   value and its unit.                                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSUnit (char *cssRule, PresentationValue * pval)
#else
static char        *ParseCSSUnit (cssRule, pval)
char               *cssRule;
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
  cssRule = SkipBlanks (cssRule);
  if (*cssRule == '-')
    {
      minus = 1;
      cssRule++;
      cssRule = SkipBlanks (cssRule);
    }

  if (*cssRule == '+')
    {
      cssRule++;
      cssRule = SkipBlanks (cssRule);
    }

  while ((*cssRule >= '0') && (*cssRule <= '9'))
    {
      val *= 10;
      val += *cssRule - '0';
      cssRule++;
      valid = 1;
    }

   if (*cssRule == '.')
     {
       real = 1;
       f = val;
       val = 0;
       cssRule++;
       /* keep only 3 digits */
       if ((*cssRule >= '0') && (*cssRule <= '9'))
	 {
	   val = (*cssRule - '0') * 100;
	   cssRule++;
	   if ((*cssRule >= '0') && (*cssRule <= '9'))
	     {
	       val += (*cssRule - '0') * 10;
	       cssRule++;
	       if ((*cssRule >= '0') && (*cssRule <= '9'))
		 {
		   val += *cssRule - '0';
		   cssRule++;
		 }
	     }

	   while ((*cssRule >= '0') && (*cssRule <= '9'))
	     cssRule++;
	   valid = 1;
	 }
     }

   if (!valid)
     {
       cssRule = SkipWord (cssRule);
       pval->typed_data.unit = DRIVERP_UNIT_INVALID;
       pval->typed_data.value = 0;
       return (cssRule);
     }

   cssRule = SkipBlanks (cssRule);
   for (uni = 0; uni < NB_UNITS; uni++)
     {
#ifdef WWW_WINDOWS
       if (!_strnicmp (CSSUnitNames[uni].sign, cssRule,
		       strlen (CSSUnitNames[uni].sign)))
#else  /* WWW_WINDOWS */
	 if (!strncasecmp (CSSUnitNames[uni].sign, cssRule,
			   strlen (CSSUnitNames[uni].sign)))
#endif /* !WWW_WINDOWS */
	   {
	     pval->typed_data.unit = CSSUnitNames[uni].unit;
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
	     return (cssRule + strlen (CSSUnitNames[uni].sign));
	   }
     }

   /*
    * not in the list of predefined units.
    */
   pval->typed_data.unit = DRIVERP_UNIT_REL;
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
   return (cssRule);
}

/************************************************************************
 *									*  
 *			PARSING FUNCTIONS				*
 *									*  
 ************************************************************************/


/*----------------------------------------------------------------------
   GetHTMLStyleAttrIndex : returns the index of the current         
   attribute type in the HTMLStyleAttributes array             
   return NULL if not found                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *GetHTMLStyleAttrIndex (char *cssRule, int *index)
#else
static char        *GetHTMLStyleAttrIndex (cssRule, index)
char               *cssRule;
int                *index;
#endif
{
  int                 i;
  
  cssRule = SkipBlanks (cssRule);
  for (i = 0; i < NB_CSSSTYLEATTRIBUTE; i++)
    if (!strncmp (cssRule, HTMLStyleAttributes[i].name, strlen(HTMLStyleAttributes[i].name)))
      {
	*index = i;
	return (cssRule + strlen (HTMLStyleAttributes[i].name));
      }
  return (NULL);
}

/*----------------------------------------------------------------------
   GetCSSName : return a string corresponding to the CSS name of   
   an element                                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char               *GetCSSName (Element el, Document doc)
#else
char               *GetCSSName (el, doc)
Element             el;
Document            doc;
#endif
{
  char               *res = GITagName (el);

  /* some kind of filtering is probably needed !!! */
  if (res == NULL)
    return ("unknown");
  return (res);
}

/*----------------------------------------------------------------------
   GetCSSNames : return the list of strings corresponding to the   
   CSS names of an element                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int      GetCSSNames (Element el, Document doc, char **lst, int max)
#else
static int      GetCSSNames (el, doc, lst, max)
Element         el;
Document        doc;
char          **lst;
int             max;
#endif
{
   char        *res;
   int          deep;
   Element      father = el;

   for (deep = 0; deep < max;)
     {
	el = father;
	if (el == NULL)
	   break;
	father = TtaGetParent (el);

	res = GITagName (el);

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

/*----------------------------------------------------------------------
 PresentationSettingsToCSS :  translate a PresentationSetting to the
     equivalent CSS string, and add it to the buffer given as the
      argument. It is used when extracting the CSS string from actual
      presentation.
 
  All the possible values returned by the presentation drivers are
  described in thotlib/include/presentdriver.h (PresentationType enum).
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void   PresentationSettingsToCSS (PresentationSetting settings, char *buffer, int len)
#else
void   PresentationSettingsToCSS (settings, buffer, len)
PresentationSetting  settings;
char                *param;
int                  len
#endif
{
  unsigned short      red, green, blue;
  int                 add_unit = 0;
  int                 real = 0;
  float               fval = 0;
  int                 unit, i;

  buffer[0] = EOS;
  if (len < 40)
    return;

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
	{
	  if (real)
	    {
	      sprintf (buffer, "font-size : %g", fval);
	      add_unit = 1;
	    }
	  else
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
	      case 7:
	      case 8:
	      case 9:
	      case 10:
	      case 11:
	      case 12:
		strcpy (buffer, "font-size : xx-large");
		break;
	      }
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
	  strcpy (buffer, "font-family : courier");
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
	sprintf (buffer, "background-image: url(%s)", (char *)settings->value.pointer);
      else
	sprintf (buffer, "background-image: none");
      break;
    case DRIVERP_PICTUREMODE:
      switch (settings->value.typed_data.value)
	{
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
      /* add the unit string to the CSS string */
      for (i = 0; i < NB_UNITS; i++)
	{
	  if (CSSUnitNames[i].unit == unit)
	    {
	      strcat (buffer, CSSUnitNames[i].sign);
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
   SpecificSettingsToCSS :  Callback for ApplyAllSpecificSettings,
       enrich the CSS string.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void  SpecificSettingsToCSS (SpecificTarget target, SpecificContext ctxt, PresentationSetting settings, void *param)
#else
static void  SpecificSettingsToCSS (SpecificTarget target, SpecificContext ctxt, PresentationSetting settings, void *param)
SpecificTarget      target;
SpecificContext     ctxt;
PresentationSetting settings;
void               *param;
#endif
{
  LoadedImageDesc    *imgInfo;
  char               *css_rules = param;
  char                string[150];
  char               *ptr;

  string[0] = EOS;
  if (settings->type == DRIVERP_BGIMAGE)
    {
      /* transform absolute URL into relative URL */
      imgInfo = SearchLoadedImage((char *)settings->value.pointer, 0);
      if (imgInfo != NULL)
	ptr = MakeRelativeURL (imgInfo->originalName, DocumentURLs[ctxt->doc]);
      else
	ptr = MakeRelativeURL ((char *)settings->value.pointer, DocumentURLs[ctxt->doc]);
      settings->value.pointer = ptr;
      PresentationSettingsToCSS(settings, &string[0], sizeof(string));
      TtaFreeMemory (ptr);
    }
  else
    PresentationSettingsToCSS(settings, &string[0], sizeof(string));

  if ((string[0] != EOS) && (*css_rules != EOS))
    strcat (css_rules, "; ");
  if (string[0] != EOS)
    strcat (css_rules, string);
}

/*----------------------------------------------------------------------
   GetHTMLStyleString : return a string corresponding to the CSS    
   description of the presentation attribute applied to a       
   element.
   For stupid reasons, if the target element is HTML or BODY,
   one returns the concatenation of both element style strings.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetHTMLStyleString (Element el, Document doc, char *buf, int *len)
#else
void                GetHTMLStyleString (el, doc, buf, len)
Element             el;
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
  ctxt = GetSpecificContext (doc);
  ApplyAllSpecificSettings (el, ctxt, SpecificSettingsToCSS, &buf[0]);
  FreeSpecificContext (ctxt);
  *len = strlen (buf);

  /*
   * BODY / HTML elements specific handling.
   */
  elType = TtaGetElementType (el);
  if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
    {
      if (elType.ElTypeNum == HTML_EL_HTML)
	{
	  elType.ElTypeNum = HTML_EL_BODY;
	  el = TtaSearchTypedElement(elType, SearchForward, el);
	  if (!el)
	    return;
	  if (*len > 0)
	    strcat(buf,";");
	  *len = strlen (buf);
	  ctxt = GetSpecificContext (doc);
	  ApplyAllSpecificSettings (el, ctxt, SpecificSettingsToCSS, &buf[*len]);
	  FreeSpecificContext (ctxt);
	  *len = strlen (buf);
	}
      else if (elType.ElTypeNum == HTML_EL_BODY)
	{
	  el = TtaGetParent (el);
	  if (!el)
	    return;
	  if (*len > 0)
	    strcat(buf,";");
	  *len = strlen (buf);
	  ctxt = GetSpecificContext (doc);
	  ApplyAllSpecificSettings (el, ctxt, SpecificSettingsToCSS, &buf[*len]);
	  FreeSpecificContext (ctxt);
	  *len = strlen (buf);
	}
    }
}

/************************************************************************
 *									*  
 *	CORE OF THE CSS PARSER : THESE TAKE THE CSS STRINGS 		*
 *	PRODUCE THE CORECT DRIVER CONTEXT, AND DO THE PARSING		*
 *									*  
 ************************************************************************/


/*----------------------------------------------------------------------
   ParseCSSRule : parse a CSS Style string                        
   we expect the input string describing the style to be of the  
   form : ATTRIBUTE : DESCRIPTION [ , ATTIBUTE : DESCRIPTION ] * 
   but tolerate incorrect or incomplete input                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ParseCSSRule (PresentationTarget target, PresentationContext context, char *cssRule)
#else
void                ParseCSSRule (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   PresentationValue   unused;
   int                 styleno;
   char               *p;

   while (*cssRule != 0)
     {
	cssRule = SkipBlanks (cssRule);
	/* look for the type of attribute */
	p = GetHTMLStyleAttrIndex (cssRule, &styleno);
	if (!p)
	  {
	     cssRule++;
	     cssRule = SkipWord (cssRule);
	     cssRule = SkipBlanks (cssRule);
	     continue;
	  }
	/* update index and skip the ":" indicator if present */
	cssRule = p;
	cssRule = SkipBlanks (cssRule);
	if (*cssRule == ':')
	  {
	     cssRule++;
	     cssRule = SkipBlanks (cssRule);
	  }
	/* try to parse the attribute associated to this attribute */
	if (HTMLStyleAttributes[styleno].parsing_function != NULL)
	   p = HTMLStyleAttributes[styleno].
	      parsing_function (target, context, cssRule);

	/* Update the rendering */
	if (context->drv->UpdatePresentation != NULL)
	   context->drv->UpdatePresentation (target, context, unused);

	/* update index and skip the ";" separator if present */
	cssRule = p;
	cssRule = SkipBlanks (cssRule);
	if (*cssRule == ',' || *cssRule == ';')
	  {
	     cssRule++;
	     cssRule = SkipBlanks (cssRule);
	  }
     }

}

/*----------------------------------------------------------------------
   ParseHTMLSpecificStyle : parse and apply a CSS Style string. 
   This function must be called only to in the context of        
   specific style applying to an element, we will use the        
   specific presentation driver to reflect the new presentation  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ParseHTMLSpecificStyle (Element elem, char *cssRule, Document doc)
#else
void                ParseHTMLSpecificStyle (elem, cssRule, doc)
Element             elem;
char               *cssRule;
Document            doc;

#endif
{
   PresentationTarget  target;
   SpecificContext     context;
   /*PresentationValue   unused;*/
   ElementType         elType;
   Element             el;

   /*unused.data = 0;*/
#ifdef DEBUG_STYLES
   fprintf (stderr, "ParseHTMLSpecificStyle(%s,%s,%d)\n",
	    GetCSSName (elem, doc), cssRule, doc);
#endif
   /* 
    * A rule applying to BODY is really meant to address HTML.
    */
   elType = TtaGetElementType(elem);
  if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0 &&
      (elType.ElTypeNum == HTML_EL_BODY || elType.ElTypeNum == HTML_EL_HEAD))
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

   /* Call the parser */
   ParseCSSRule (target, (PresentationContext) context, cssRule);

   /* free the context */
   FreeSpecificContext(context);
}

/*----------------------------------------------------------------------
   ParseHTMLGenericSelector : Create a generic context for a given 
   selector string. If the selector is made of multiple comma- 
   separated selector items, it parses them one at a time and  
   return the end of the selector string to be handled or NULL 

   Need to add multi-DTD support here !!!!
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char    *ParseHTMLGenericSelector (char *selector, char *cssRule,
			   GenericContext ctxt, Document doc, CSSInfoPtr css)
#else
static char    *ParseHTMLGenericSelector (selector, cssRule, ctxt, doc, css)
char           *selector;
char           *cssRule;
GenericContext  ctxt;
Document        doc;
CSSInfoPtr      css;
#endif
{
  ElementType         elType;
  PresentationTarget  target;
  PSchema             pSchema, prev;
  char                sel[150];
  char                class[150];
  char                pseudoclass[150];
  char                id[150];
  char                attrelemname[150];
  char               *deb;
  char               *elem;
  char               *cur;
  char               *ancestors[MAX_ANCESTORS];
  int                 i, j;

  target = (PresentationTarget) css->pschema;
  sel[0] = EOS;
  class[0] = EOS;
  pseudoclass[0] = EOS;
  id[0] = EOS;
  attrelemname[0] = EOS;
  deb = cur = elem = &sel[0];
  for (i = 0; i < MAX_ANCESTORS; i++)
    {
      ancestors[i] = NULL;
      ctxt->ancestors[i] = 0;
      ctxt->ancestors_nb[i] = 0;
    }

  /*
   * first format the first selector item, uniformizing blanks.
   */
  selector = SkipBlanks (selector);
  while (1)
    {
      /* put one word in the sel buffer */
      while ((*selector != EOS) && (*selector != ',') &&
	     (*selector != '.') && (*selector != ':') &&
	     (*selector != '#') && (!IsBlank (selector)))
	*cur++ = *selector++;
      *cur++ = EOS;
      
      if ((*selector == ':') || (*selector == '.') || (*selector == '#'))
	{
	  /* keep the name as attrelemname, it's not an ancestor */
	  strcpy (attrelemname, elem);
	  elem = "";
	}
      else
	elem = deb;
      deb = cur;
      
      if (*selector == '.')
	{
	  /* read the class id : only one allowed by selector */
	  class[0] = EOS;
	  cur = class;
	  selector++;
	  while ((*selector != EOS) && (*selector != ',') &&
		 (*selector != '.') && (*selector != ':') &&
		 (!IsBlank (selector)))
	    *cur++ = *selector++;
	  *cur++ = EOS;
	  cur = deb;
	}
      else if (*selector == ':')
	{
	  /* read the pseudoclass id : only one allowed by selector */
	  pseudoclass[0] = EOS;
	  cur = pseudoclass;
	  selector++;
	  while ((*selector != EOS) && (*selector != ',') &&
		 (*selector != '.') && (*selector != ':') &&
		 (!IsBlank (selector)))
	    *cur++ = *selector++;
	  *cur++ = EOS;
	  cur = deb;
	}
      else if (*selector == '#')
	{
	  /* read the id : only one allowed by selector */
	  id[0] = EOS;
	  cur = &id[0];
	  selector++;
	  while ((*selector != EOS) && (*selector != ',') &&
		 (*selector != '.') && (*selector != ':') &&
		 (!IsBlank (selector)))
	    *cur++ = *selector++;
	  *cur++ = EOS;
	  cur = deb;
	}
      else if (IsBlank (selector))
	{
	  for (i = MAX_ANCESTORS - 1; i > 0; i--)
	    ancestors[i] = ancestors[i - 1];
	  selector = SkipBlanks (selector);
	  /* don't take class and pseudoclass into account for ancestors */
	  class[0] = EOS;
	  pseudoclass[0] = EOS;
	  id[0] = EOS;
	}

      /* store elem in the list if the string is non-empty */
      if (*elem != EOS)
	ancestors[0] = elem;

      /* why did we stop ? */
      if (*selector == EOS)
	/* end of the selector */
	break;
      else if (*selector == ',')
	{
	  /* end of the current selector */
	  selector++;
	  break;
	}
    }

   /* Now set up the context block */
  ctxt->box = 0;
  elem = ancestors[0];
  if ((elem == NULL) || (*elem == EOS))
    {
      if (class[0] != EOS)
	elem = &class[0];
      else if (pseudoclass[0]  != EOS)
	elem = &pseudoclass[0];
      else if (id[0]  != EOS)
	elem = &id[0];
      else
	return (selector);
    }

  if (class[0] != EOS)
    {
      ctxt->class = class;
      ctxt->classattr = HTML_ATTR_Class;
    }
  else if (pseudoclass[0] != EOS)
    {
      ctxt->class = pseudoclass;
      ctxt->classattr = HTML_ATTR_PseudoClass;
    }
  else if (id[0] != EOS)
    {
      ctxt->class = id;
      ctxt->classattr = HTML_ATTR_ID;
    }
  else
    {
      ctxt->class = NULL;
      ctxt->classattr = 0;
    }

  ctxt->type = ctxt->attr = ctxt->attrval = ctxt->attrelem = 0;
  if (attrelemname[0] != EOS)
    {
      GIType (attrelemname, &elType, doc);
      ctxt->attrelem = elType.ElTypeNum;
      if (ctxt->attrelem == HTML_EL_BODY && strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
	ctxt->attrelem = HTML_EL_HTML;
    }
  
  GIType (elem, &elType, doc);
  ctxt->type = elType.ElTypeNum;
  ctxt->schema = elType.ElSSchema;
  if (elType.ElSSchema == NULL)
    ctxt->schema = TtaGetDocumentSSchema (doc);
  else if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
    {
      /* it's not an HTML element */
      if (css->mschema == NULL)
	{
	  /* create a new extension schema for MathML elements */
	  css->mschema = TtaNewPSchema ();
	  pSchema = TtaGetFirstPSchema (doc, elType.ElSSchema);
	  prev = NULL;
	  while (pSchema != NULL)
	    {
	      prev = pSchema;
	      TtaNextPSchema (&pSchema, doc, NULL);
	    }
	  TtaAddPSchema (css->mschema, prev, TRUE, doc, elType.ElSSchema);
	}
      target = (PresentationTarget) css->mschema;
    }
  else if (ctxt->type == HTML_EL_BODY)
     ctxt->type = HTML_EL_HTML;

  if (ctxt->type == 0 && ctxt->attr == 0 &&
      ctxt->attrval == 0 && ctxt->classattr == 0)
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
      GIType (ancestors[i], &elType, doc);
      if (elType.ElTypeNum == HTML_EL_BODY && !strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
	elType.ElTypeNum = HTML_EL_HTML;
      if (elType.ElTypeNum == 0)
	continue;
      for (j = 0; j < MAX_ANCESTORS; j++)
	{
	  if (ctxt->ancestors[j] == 0)
	    {
	      ctxt->ancestors[j] = elType.ElTypeNum;
	      ctxt->ancestors_nb[j] = 0;
	      break;
	    }
	  if (ctxt->ancestors[j] == elType.ElTypeNum)
	    {
	      ctxt->ancestors_nb[j]++;
	      break;
	    }
	}
    }

  if (cssRule)
      ParseCSSRule (target, (PresentationContext) ctxt, cssRule);
  return (selector);
}

/*----------------------------------------------------------------------
   ParseCSSGenericStyle : parse and apply a CSS Style string.  
   This function must be called only to in the context of        
   a generic style applying to class of element. The generic     
   presentation driver is used to reflect the new presentation.  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         ParseCSSGenericStyle (char *selector, char *cssRule, Document doc, CSSInfoPtr css)
#else
void         ParseCSSGenericStyle (selector, cssRule, doc, css)
char        *selector;
char        *cssRule;
Document     doc;
CSSInfoPtr   css;
#endif
{
  GenericContext      ctxt;

#ifdef DEBUG_STYLES
  fprintf (stderr, "ParseCSSGenericStyle(%s,%s,%d)\n", selector, cssRule, doc);
#endif
  ctxt = GetGenericContext (doc);
  if (ctxt == NULL)
    return;

  if (HTMLStyleParserDestructiveMode)
    ctxt->destroy = 1;

  while ((selector != NULL) && (*selector != EOS))
    selector = ParseHTMLGenericSelector (selector, cssRule, ctxt, doc, css);
  FreeGenericContext (ctxt);

#ifdef DEBUG_STYLES
  fprintf (stderr, "ParseCSSGenericStyle(%s,%s,%d) done\n", selector, cssRule, doc);
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
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSTest (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{

   cssRule = SkipWord (cssRule);
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderTopWidth : parse a CSS BorderTopWidth
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorderTopWidth (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSBorderTopWidth (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSBorderTopWidth ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderRightWidth : parse a CSS BorderRightWidth
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorderRightWidth (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSBorderRightWidth (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSBorderRightWidth ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderBottomWidth : parse a CSS BorderBottomWidth
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorderBottomWidth (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSBorderBottomWidth (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSBorderBottomWidth ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderLeftWidth : parse a CSS BorderLeftWidth
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorderLeftWidth (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSBorderLeftWidth (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSBorderLeftWidth ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderWidth : parse a CSS BorderWidth
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorderWidth (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSBorderWidth (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSBorderWidth ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderTop : parse a CSS BorderTop
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorderTop (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSBorderTop (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSBorderTop ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderRight : parse a CSS BorderRight
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorderRight (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSBorderRight (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSBorderRight ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderBottom : parse a CSS BorderBottom
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorderBottom (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSBorderBottom (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSBorderBottom ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderLeft : parse a CSS BorderLeft
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorderLeft (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSBorderLeft (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSBorderLeft ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderColor : parse a CSS border-color        
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorderColor (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSBorderColor (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSBorderColor ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderStyle : parse a CSS border-style        
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorderStyle (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSBorderStyle (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSBorderStyle ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorder : parse a CSS border        
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBorder (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSBorder (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSBorder ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSClear : parse a CSS clear attribute string    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSClear (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSClear (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSClear ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSDisplay : parse a CSS display attribute string        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSDisplay (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSDisplay (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   PresentationValue   pval;

   cssRule = SkipBlanks (cssRule);
   if (!strncasecmp (cssRule, "block", 5))
     {
	pval.typed_data.unit = DRIVERP_UNIT_REL;
	pval.typed_data.value = DRIVERP_NOTINLINE;
	if (context->drv->SetInLine)
	   context->drv->SetInLine (target, context, pval);
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "inline", 6))
     {
	pval.typed_data.unit = DRIVERP_UNIT_REL;
	pval.typed_data.value = DRIVERP_INLINE;
	if (context->drv->SetInLine)
	   context->drv->SetInLine (target, context, pval);
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "none", 4))
     {
	pval.typed_data.unit = DRIVERP_UNIT_REL;
	pval.typed_data.value = DRIVERP_HIDE;
	if (context->drv->SetShow)
	   context->drv->SetShow (target, context, pval);
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "list-item", 9))
     {
	MSG ("list-item display value unsupported\n");
	cssRule = SkipWord (cssRule);
     }
   else
     {
	fprintf (stderr, "invalid display value %s\n", cssRule);
	return (cssRule);
     }
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSFloat : parse a CSS float attribute string    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSFloat (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSFloat (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSFloat ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSLetterSpacing : parse a CSS letter-spacing    
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSLetterSpacing (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSLetterSpacing (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSLetterSpacing ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSListStyleType : parse a CSS list-style-type
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSListStyleType (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSListStyleType (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSListStyleType ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSListStyleImage : parse a CSS list-style-image
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSListStyleImage (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSListStyleImage (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSListStyleImage ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSListStylePosition : parse a CSS list-style-position
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSListStylePosition (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSListStylePosition (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSListStylePosition ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSListStyle : parse a CSS list-style            
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSListStyle (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSListStyle (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSListStyle ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSMagnification : parse a CSS magnification     
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSMagnification (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSMagnification (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
  PresentationValue   pval;
  GenericContext      block;

  cssRule = SkipBlanks (cssRule);
  cssRule = ParseCSSUnit (cssRule, &pval);
  if ((pval.typed_data.unit == DRIVERP_UNIT_REL) && (pval.typed_data.value >= -10) &&
      (pval.typed_data.value <= 10))
    {
      if (context->drv == &GenericStrategy)
	{
	  block = (GenericContext) context;
	  if ((block->type == HTML_EL_HTML) ||
	      (block->type == HTML_EL_BODY) ||
	      (block->type == HTML_EL_HEAD))
	    {
	      CSSSetMagnification (block->doc, (PSchema) target, pval.typed_data.value);
	      return (cssRule);
	    }
	}
    }
  else
    MSG ("invalid magnification value\n");
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSMarginLeft : parse a CSS margin-left          
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSMarginLeft (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSMarginLeft (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSMarginLeft ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSMarginRight : parse a CSS margin-right        
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSMarginRight (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSMarginRight (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSMarginRight ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSMargin : parse a CSS margin attribute string. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSMargin (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSMargin (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSMargin ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSPaddingTop : parse a CSS PaddingTop
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSPaddingTop (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSPaddingTop (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSPaddingTop ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSPaddingRight : parse a CSS PaddingRight
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSPaddingRight (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSPaddingRight (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSPaddingRight ");
   TODO
      return (cssRule);
}


/*----------------------------------------------------------------------
   ParseCSSPaddingBottom : parse a CSS PaddingBottom
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSPaddingBottom (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSPaddingBottom (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSPaddingBottom ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSPaddingLeft : parse a CSS PaddingLeft
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSPaddingLeft (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSPaddingLeft (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSPaddingLeft ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSPadding : parse a CSS padding attribute string. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSPadding (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSPadding (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSPadding ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSTextAlign : parse a CSS text-align            
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSTextAlign (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSTextAlign (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   PresentationValue   align;
   PresentationValue   justify;

   align.typed_data.value = 0;
   align.typed_data.unit = 1;
   justify.typed_data.value = 0;
   justify.typed_data.unit = 1;

   cssRule = SkipBlanks (cssRule);
   if (!strncasecmp (cssRule, "left", 4))
     {
	align.typed_data.value = AdjustLeft;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "right", 5))
     {
	align.typed_data.value = AdjustRight;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "center", 6))
     {
	align.typed_data.value = Centered;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "justify", 7))
     {
	justify.typed_data.value = Justified;
	cssRule = SkipWord (cssRule);
     }
   else
     {
	fprintf (stderr, "invalid align value\n");
	return (cssRule);
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
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSTextIndent : parse a CSS text-indent          
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSTextIndent (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSTextIndent (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   PresentationValue   pval;

   cssRule = SkipBlanks (cssRule);
   cssRule = ParseCSSUnit (cssRule, &pval);
   if (pval.typed_data.unit == DRIVERP_UNIT_INVALID)
     {
	fprintf (stderr, "invalid font size\n");
	return (cssRule);
     }
   /*
    * install the attribute
    */
   if (context->drv->SetIndent != NULL)
     {
	context->drv->SetIndent (target, context, pval);
     }
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSTextTransform : parse a CSS text-transform    
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSTextTransform (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSTextTransform (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSTextTransform ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSVerticalAlign : parse a CSS vertical-align    
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSVerticalAlign (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSVerticalAlign (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSVerticalAlign ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSWhiteSpace : parse a CSS white-space          
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSWhiteSpace (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSWhiteSpace (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   cssRule = SkipBlanks (cssRule);
   if (!strncasecmp (cssRule, "normal", 6))
     {
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "pre", 3))
     {
	MSG ("pre white-space setting unsupported\n");
	cssRule = SkipWord (cssRule);
     }
   else
     {
	fprintf (stderr, "invalid white-space value %s\n", cssRule);
	return (cssRule);
     }
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSWordSpacing : parse a CSS word-spacing        
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSWordSpacing (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSWordSpacing (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSWordSpacing ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSFont : parse a CSS font attribute string      
   we expect the input string describing the attribute to be     
   !!!!!!                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSFont (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSFont (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   MSG ("ParseCSSFont ");
   TODO
      return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSFontSize : parse a CSS font size attr string  
   we expect the input string describing the attribute to be     
   xx-small, x-small, small, medium, large, x-large, xx-large      
   or an absolute size, or an imcrement relative to the parent     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSFontSize (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSFontSize (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   PresentationValue   pval;

   cssRule = SkipBlanks (cssRule);
   if (!strncasecmp (cssRule, "xx-small", 8))
     {
	pval.typed_data.unit = DRIVERP_UNIT_REL;
	pval.typed_data.value = 1;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "x-small", 7))
     {
	pval.typed_data.unit = DRIVERP_UNIT_REL;
	pval.typed_data.value = 2;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "small", 5))
     {
	pval.typed_data.unit = DRIVERP_UNIT_REL;
	pval.typed_data.value = 3;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "medium", 6))
     {
	pval.typed_data.unit = DRIVERP_UNIT_REL;
	pval.typed_data.value = 4;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "large", 5))
     {
	pval.typed_data.unit = DRIVERP_UNIT_REL;
	pval.typed_data.value = 5;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "x-large", 7))
     {
	pval.typed_data.unit = DRIVERP_UNIT_REL;
	pval.typed_data.value = 6;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "xx-large", 8))
     {
	pval.typed_data.unit = DRIVERP_UNIT_REL;
	pval.typed_data.value = 8;
	cssRule = SkipWord (cssRule);
     }
   else
     {
	cssRule = ParseCSSUnit (cssRule, &pval);
	if (pval.typed_data.unit == DRIVERP_UNIT_INVALID)
	  {
	     fprintf (stderr, "invalid font size\n");
	     return (cssRule);
	  }
     }

   /*
    * install the attribute
    */
   if (context->drv->SetFontSize)
      context->drv->SetFontSize (target, context, pval);
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSFontFamily : parse a CSS font family string   
   we expect the input string describing the attribute to be     
   a common generic font style name                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSFontFamily (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSFontFamily (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   PresentationValue   font;
   unsigned char       msgBuffer[MAX_BUFFER_LENGTH];

   font.typed_data.value = 0;
   font.typed_data.unit = 1;
   cssRule = SkipBlanks (cssRule);
   if (!strncasecmp (cssRule, "times", strlen("times")))
     {
	font.typed_data.value = DRIVERP_FONT_TIMES;
	cssRule = SkipProperty (cssRule);
     }
   else if (!strncasecmp (cssRule, "serif", strlen("serif")))
     {
	font.typed_data.value = DRIVERP_FONT_TIMES;
	cssRule = SkipProperty (cssRule);
     }
   else if (!strncasecmp (cssRule, "helvetica", strlen("helvetica")))
     {
	font.typed_data.value = DRIVERP_FONT_HELVETICA;
	cssRule = SkipProperty (cssRule);
     }
   else if (!strncasecmp (cssRule, "sans-serif", strlen("sans")))
     {
	font.typed_data.value = DRIVERP_FONT_HELVETICA;
	cssRule = SkipProperty (cssRule);
     }
   else if (!strncasecmp (cssRule, "courier", strlen("courier")))
     {
	font.typed_data.value = DRIVERP_FONT_COURIER;
	cssRule = SkipProperty (cssRule);
     }
   else if (!strncasecmp (cssRule, "monospace", strlen("monospace")))
     {
	font.typed_data.value = DRIVERP_FONT_COURIER;
	cssRule = SkipProperty (cssRule);
     }
   else
     {
	/* !!!!!! many font families are missing !!!!!!!! */
	sprintf (msgBuffer, "unknown font family: %s\n", cssRule);
	MSG (msgBuffer);
	cssRule = SkipProperty (cssRule);
	return (cssRule);
     }

   /*
    * install the new presentation.
    */
   if (context->drv->SetFontFamily)
      context->drv->SetFontFamily (target, context, font);
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSFontWeight : parse a CSS font weight string   
   we expect the input string describing the attribute to be     
   extra-light, light, demi-light, medium, demi-bold, bold, extra-bold
   or a number encoding for the previous values                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSFontWeight (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSFontWeight (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   PresentationValue   weight;
   int                 val;

   weight.typed_data.value = 0;
   weight.typed_data.unit = 1;
   cssRule = SkipBlanks (cssRule);
   if (!strncasecmp (cssRule, "extra-light", strlen ("extra-light")))
     {
	weight.typed_data.value = -3;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "light", strlen ("light")))
     {
	weight.typed_data.value = -2;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "demi-light", strlen ("demi-light")))
     {
	weight.typed_data.value = -1;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "medium", strlen ("medium")))
     {
	weight.typed_data.value = 0;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "extra-bold", strlen ("extra-bold")))
     {
	weight.typed_data.value = +3;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "bold", strlen ("bold")))
     {
	weight.typed_data.value = +2;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "demi-bold", strlen ("demi-bold")))
     {
	weight.typed_data.value = +1;
	cssRule = SkipWord (cssRule);
     }
   else if (sscanf (cssRule, "%d", &val) > 0)
     {
	if ((val < -3) || (val > 3))
	  {
	     fprintf (stderr, "invalid font weight %d\n", val);
	     weight.typed_data.value = 0;
	  }
	else
	   weight.typed_data.value = val;
	while (isdigit(*cssRule))
	  cssRule++;
     }
   else
     {
	fprintf (stderr, "invalid font weight\n");
	return (cssRule);
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
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSFontVariant : parse a CSS font variant string     
   we expect the input string describing the attribute to be     
   normal or small-caps
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSFontVariant (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSFontVariant (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   PresentationValue   style;

   style.typed_data.value = 0;
   style.typed_data.unit = 1;
   cssRule = SkipBlanks (cssRule);
   if (!strncasecmp (cssRule, "small-caps", strlen ("small-caps")))
     {
        /*
	 * Not supported yet, so we use bold for rendering
	 */
	style.typed_data.value = DRIVERP_FONT_BOLD;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "normal", strlen ("normal")))
     {
	style.typed_data.value = DRIVERP_FONT_ROMAN;
	cssRule = SkipWord (cssRule);
     }
   else
     {
	MSG ("invalid font variant\n");
	return (cssRule);
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
   return (cssRule);
}


/*----------------------------------------------------------------------
   ParseCSSFontStyle : parse a CSS font style string     
   we expect the input string describing the attribute to be     
   italic, oblique or normal                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSFontStyle (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSFontStyle (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   PresentationValue   style;
   PresentationValue   size;

   style.typed_data.value = 0;
   style.typed_data.unit = 1;
   size.typed_data.value = 0;
   size.typed_data.unit = 1;
   cssRule = SkipBlanks (cssRule);
   if (!strncasecmp (cssRule, "italic", 6))
     {
	style.typed_data.value = DRIVERP_FONT_ITALICS;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "oblique", 7))
     {
	style.typed_data.value = DRIVERP_FONT_OBLIQUE;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "normal", 6))
     {
	style.typed_data.value = DRIVERP_FONT_ROMAN;
	cssRule = SkipWord (cssRule);
     }
   else
     {
	MSG ("invalid font style\n");
	return (cssRule);
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
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSLineSpacing : parse a CSS font leading string 
   we expect the input string describing the attribute to be     
   value% or value                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSLineSpacing (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSLineSpacing (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   PresentationValue   lead;

   cssRule = ParseCSSUnit (cssRule, &lead);
   if (lead.typed_data.unit == DRIVERP_UNIT_INVALID)
     {
	MSG ("invalid line spacing\n");
	return (cssRule);
     }
   /*
    * install the new presentation.
    */
   if (context->drv->SetLineSpacing)
      context->drv->SetLineSpacing (target, context, lead);
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSTextDecoration : parse a CSS text decor string   
   we expect the input string describing the attribute to be     
   underline, overline, line-through, box, shadowbox, box3d,       
   cartouche, blink or none                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSTextDecoration (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSTextDecoration (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   PresentationValue   decor;

   decor.typed_data.value = 0;
   decor.typed_data.unit = 1;
   cssRule = SkipBlanks (cssRule);
   if (!strncasecmp (cssRule, "underline", strlen ("underline")))
     {
	decor.typed_data.value = Underline;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "overline", strlen ("overline")))
     {
	decor.typed_data.value = Overline;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "line-through", strlen ("line-through")))
     {
	decor.typed_data.value = CrossOut;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "box", strlen ("box")))
     {
	MSG ("the box text-decoration attribute is not yet supported\n");
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "boxshadow", strlen ("boxshadow")))
     {
	MSG ("the boxshadow text-decoration attribute is not yet supported\n");
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "box3d", strlen ("box3d")))
     {
	MSG ("the box3d text-decoration attribute is not yet supported\n");
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "cartouche", strlen ("cartouche")))
     {
	MSG ("the cartouche text-decoration attribute is not yet supported\n");
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "blink", strlen ("blink")))
     {
	MSG ("the blink text-decoration attribute will not be supported\n");
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "none", strlen ("none")))
     {
	decor.typed_data.value = NoUnderline;
	cssRule = SkipWord (cssRule);
     }
   else
     {
	fprintf (stderr, "invalid text decoration\n");
	return (cssRule);
     }

   /*
    * install the new presentation.
    */
   if (decor.typed_data.value)
     {
	if (context->drv->SetTextUnderlining)
	   context->drv->SetTextUnderlining (target, context, decor);
     }
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSColor : parse a CSS color attribute string    
   we expect the input string describing the attribute to be     
   either a color name, a 3 tuple or an hexadecimal encoding.    
   The color used will be approximed from the current color      
   table                                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSColor (char *cssRule, PresentationValue * val)
#else
static char        *ParseCSSColor (cssRule, val)
char               *cssRule;
PresentationValue  *val;
#endif
{
  char                colname[100];
  unsigned short      redval = (unsigned short) -1;
  unsigned short      greenval = 0;	/* composant of each RGB       */
  unsigned short      blueval = 0;	/* default to red if unknown ! */
  int                 i, len;
  int                 best = 0;	/* best color in list found */
  boolean             failed;

  cssRule = SkipBlanks (cssRule);
  val->typed_data.unit = DRIVERP_UNIT_INVALID;
  val->typed_data.value = 0;
  failed = TRUE;
  /*
   * first parse the attribute string
   * NOTE : this can't lookup for color name in
   *        cause  we try first to lokup color name from digits
   *        [0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f]
   */
  if ((*cssRule == '#') ||
      (isxdigit (cssRule[0]) && isxdigit (cssRule[1]) &&
       isxdigit (cssRule[2])))
    {
      if (*cssRule == '#')
	cssRule++;
      failed = FALSE;
      /* we expect an hexa encoding like F00 or FF0000 */
      if ((!isxdigit (cssRule[0])) || (!isxdigit (cssRule[1])) ||
	  (!isxdigit (cssRule[2])))
	{
	  fprintf (stderr, "Invalid color encoding %s\n", cssRule - 1);
	  failed = TRUE;
	}
      else if (!isxdigit (cssRule[3]))
	{
	  /* encoded as on 3 digits #F0F  */
	  redval = hexa_val (cssRule[0]) * 16 + hexa_val (cssRule[0]);
	  greenval = hexa_val (cssRule[1]) * 16 + hexa_val (cssRule[1]);
	  blueval = hexa_val (cssRule[2]) * 16 + hexa_val (cssRule[2]);
	}
      else if ((!isxdigit (cssRule[4])) || (!isxdigit (cssRule[5])))
	fprintf (stderr, "Invalid color encoding %s\n", cssRule - 1);
      else
	{
	  /* encoded as on 3 digits #FF00FF */
	  redval = hexa_val (cssRule[0]) * 16 + hexa_val (cssRule[1]);
	  greenval = hexa_val (cssRule[2]) * 16 + hexa_val (cssRule[3]);
	  blueval = hexa_val (cssRule[4]) * 16 + hexa_val (cssRule[5]);
	}
    }
  else if (isalpha (*cssRule))
    {
      /* we expect a color name like "red", store it in colname */
      len = sizeof (colname) - 1;
      for (i = 0; i < len; i++)
	{
	  if (!isalnum (cssRule[i]))
	    {
	      cssRule += i;
	      break;
	    }
	  colname[i] = cssRule[i];
	}
      colname[i] = EOS;
      
      /* Lookup the color name in our own color name database */
      for (i = 0; i < NBCOLORNAME; i++)
	if (!strcasecmp (ColornameTable[i].name, colname))
	  {
	    redval = ColornameTable[i].red;
	    greenval = ColornameTable[i].green;
	    blueval = ColornameTable[i].blue;
	    failed = FALSE;
	    i = NBCOLORNAME;
	  }
      /* Lookup the color name in Thot color name database */
      if (failed)
	{
	  TtaGiveRGB (colname, &redval, &greenval, &blueval);
	  failed = FALSE;
	}
    }
  else if (isdigit (*cssRule) || *cssRule == '.')
    {
      /*
       * we expect a color defined by it's three components.
       * like "255 0 0" or "1.0 0.0 0.0"
       TODO
       */
    }
  
  if (failed)
    {
      val->typed_data.unit = DRIVERP_UNIT_INVALID;
      val->typed_data.value = 0;
    }
  else
    {
      best = TtaGetThotColor (redval, greenval, blueval);
      val->typed_data.value = best;
      val->typed_data.unit = DRIVERP_UNIT_REL;
    }
 return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSWidth : parse a CSS width attribute           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSWidth (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSWidth (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{

   cssRule = SkipBlanks (cssRule);

   /*
    * first parse the attribute string
    */
   if (!strcasecmp (cssRule, "auto"))
     {
	cssRule = SkipWord (cssRule);
	MSG ("ParseCSSWidth : auto ");
	TODO;
	return (cssRule);
     }
   /*
    * install the new presentation.
    mainview = TtaGetViewFromName(doc, "Document_View");
    TtaGiveBoxSize(el, doc, mainview, UnPoint, &width, &height);
    new_height = height;
    TtaChangeBoxSize(el doc, mainview, 0, new_height - height, UnPoint);
    */
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSMarginTop : parse a CSS margin-top attribute  
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseCSSMarginTop (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSMarginTop (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;

#endif
{
   PresentationValue   margin;

   cssRule = SkipBlanks (cssRule);

   /*
    * first parse the attribute string
    */
   cssRule = ParseCSSUnit (cssRule, &margin);
   if (margin.typed_data.unit == DRIVERP_UNIT_INVALID)
     {
	MSG ("invalid margin top\n");
	return (cssRule);
     }
   if (context->drv->SetVPos)
      context->drv->SetVPos (target, context, margin);
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSMarginBottom : parse a CSS margin-bottom      
   attribute                                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSMarginBottom (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSMarginBottom (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   PresentationValue   margin;

   cssRule = SkipBlanks (cssRule);

   /*
    * first parse the attribute string
    */
   cssRule = ParseCSSUnit (cssRule, &margin);
   if (margin.typed_data.unit == DRIVERP_UNIT_INVALID)
     {
	MSG ("invalid margin top\n");
	return (cssRule);
     }
   if (context->drv->SetVPos)
      context->drv->SetVPos (target, context, margin);
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSHeight : parse a CSS height attribute                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSHeight (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSHeight (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   cssRule = SkipBlanks (cssRule);

   /*
    * first parse the attribute string
    */
   if (!strcasecmp (cssRule, "auto"))
     {
	cssRule = SkipWord (cssRule);
	MSG ("ParseCSSHeight : auto ");
	TODO;
	return (cssRule);
     }
   /*
    * read the value, and if necessary convert to point size
    cssRule = ParseCSSUnit(cssRule, &new_height, &unit);
    */
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSForeground : parse a CSS foreground attribute 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSForeground (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSForeground (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   PresentationValue   best;

   cssRule = ParseCSSColor (cssRule, &best);

   if (best.typed_data.unit == DRIVERP_UNIT_INVALID)
     {
	return (cssRule);
     }
   /*
    * install the new presentation.
    */
   if (context->drv->SetForegroundColor)
      context->drv->SetForegroundColor (target, context, best);
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBackgroundColor : parse a CSS background color attribute 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBackgroundColor (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSBackgroundColor (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   PresentationValue     best;

   best.typed_data.unit = DRIVERP_UNIT_INVALID;
   if (!strncasecmp (cssRule, "transparent", strlen("transparent")))
     {
       best.typed_data.value = DRIVERP_PATTERN_NONE;
       best.typed_data.unit = DRIVERP_UNIT_REL;
       if (context->drv->SetFillPattern)
	 context->drv->SetFillPattern (target, context, best);
     }
   else
     {
       cssRule = ParseCSSColor (cssRule, &best);
       if (best.typed_data.unit != DRIVERP_UNIT_INVALID)
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
   cssRule = SkipWord (cssRule);
   return (cssRule);
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

   TtaFreeMemory (callblock);
}


/*----------------------------------------------------------------------
   UpdateCSSBackgroundImage searches a CSS BackgroundImage url within
   the styleString and make it relative to the newpath.
   oldpath = old document path
   newpath = new document path
   imgpath = new image directory
   If the image is not moved, the imgpath has to be NULL else the new
   image url is obtained by concatenation of imgpath and the image name.
   Returns NULL or a new allocated styleString.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char               *UpdateCSSBackgroundImage (char *oldpath, char *newpath, char *imgpath, char *styleString)
#else
char               *UpdateCSSBackgroundImage (oldpath, newpath, imgpath, styleString)
char               *oldpath;
char               *newpath;
char               *imgpath;
char               *styleString;
#endif
{
  char               *b, *e, *ptr;
  char                old_url[MAX_LENGTH];
  char                tempname[MAX_LENGTH];
  char                imgname[MAX_LENGTH];
  char               *new_url;
  int                 len;

  ptr = NULL;
  b = strstr (styleString, "background-image");
  if (b != NULL)
    {
      b = SkipWord (b);
      /* we need to compare this url with the new doc path */
      b = SkipBlanks (b);
      if (*b != ':')
	return (ptr);
      b++;
      b = SkipBlanks (b);
      if (strncasecmp (b, "url", 3))
	return (ptr);
      b = SkipWord (b);
      b = SkipBlanks (b);
      if (*b != '(')
	return (ptr);
      b++;
      /* search the url end */
      e = b;
      while (*e != EOS && *e != ')')
	e++;
      if (*e == EOS)
	return (ptr);
      len = (int)(e - b);
      strncpy (old_url, b, len);
      old_url[len] = EOS;
      /* get the old full image name */
      NormalizeURL (old_url, 0, tempname, imgname, oldpath);
      /* build the new full image name */
      if (imgpath != NULL)
	NormalizeURL (imgname, 0, tempname, imgname, imgpath);
      new_url = MakeRelativeURL (tempname, newpath);
      /* generate the new style string */
      len = - len + strlen (styleString) + strlen (new_url) + 1;
      ptr = (char *) TtaGetMemory (len);
      len = (int)(b - styleString);
      strncpy (ptr, styleString, len);
      strcpy (&ptr[len], new_url);
      strcat (&ptr[len], e);
      TtaFreeMemory (new_url);
    }
  return (ptr);
}


/*----------------------------------------------------------------------
   GetCSSBackgroundURL searches a CSS BackgroundImage url within
   the styleString.
   Returns NULL or a new allocated url string.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char               *GetCSSBackgroundURL (char *styleString)
#else
char               *GetCSSBackgroundURL (styleString)
char               *styleString;
#endif
{
  char               *b, *e, *ptr;
  int                 len;

  ptr = NULL;
  b = strstr (styleString, "background-image");
  if (b != NULL)
    {
      b = SkipWord (b);
      /* we need to compare this url with the new doc path */
      b = SkipBlanks (b);
      if (*b != ':')
	return (ptr);
      b++;
      b = SkipBlanks (b);
      if (strncasecmp (b, "url", 3))
	return (ptr);
      b = SkipWord (b);
      b = SkipBlanks (b);
      if (*b != '(')
	return (ptr);
      b++;
      /* search the url end */
      e = b;
      while (*e != EOS && *e != ')')
	e++;
      if (*e == EOS)
	return (ptr);
      len = (int)(e - b);
      ptr = (char *) TtaGetMemory (len+1);
      strncpy (ptr, b, len);
      ptr[len] = EOS;
    }
  return (ptr);
}


/*----------------------------------------------------------------------
   ParseCSSBackgroundImage : parse a CSS BackgroundImage
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBackgroundImage (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSBackgroundImage (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
  Element              el;
  ElementType          elType;
  GenericContext        gblock;
  SpecificContextBlock *sblock;
  BackgroundImageCallbackPtr callblock;
  PresentationValue     image, value;
  char                 *url;
  char                 *no_bg_image;
  char                  sauve;
  char                 *base;

  url = NULL;
  cssRule = SkipBlanks (cssRule);
  if (!strncasecmp (cssRule, "url", 3))
    {  
      cssRule += 3;
      cssRule = SkipBlanks (cssRule);
      if (*cssRule == '(')
	{
	  cssRule++;
	  cssRule = SkipBlanks (cssRule);
	  base = cssRule;
#     ifdef _WINDOWS
	  while (*cssRule != EOS && *cssRule != ')')
#     else /* !_WINDOWS */
	  while (*cssRule != EOS && !IsBlank (cssRule) && *cssRule != ')')
#     endif /* _WINDOWS */
	    cssRule++;

	  sauve = *cssRule;
	  *cssRule = EOS;
	  url = TtaStrdup (base);
	  *cssRule = sauve;
	}
      cssRule++;

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
      else if (url)
	{
	  no_bg_image = TtaGetEnvString("NO_BG_IMAGES");
	  if (no_bg_image == NULL ||
	      (strcasecmp(no_bg_image,"yes") && strcasecmp(no_bg_image,"true")))
	    {	       
	      /*
	       * if the background is set on the HTML or BODY element,
	       * set the background color for the full window.
	       */
	      callblock = (BackgroundImageCallbackPtr) TtaGetMemory(sizeof(BackgroundImageCallbackBlock));
	      if (callblock != NULL)
		{
		  callblock->target = target;
		  if (context->drv == &GenericStrategy)
		    {
		      gblock = (GenericContext) context;
		      memcpy (&callblock->context.generic, gblock,
			      sizeof(GenericContextBlock));
		      el = TtaGetMainRoot (context->doc);
		    }
		  else if (context->drv == &SpecificStrategy)
		    {
		      sblock = (SpecificContextBlock *) context;
		      memcpy (&callblock->context.specific, sblock,
			      sizeof(SpecificContextBlock));
		      el = (SpecificTarget) target;
		      elType = TtaGetElementType (el);
		      if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0 &&
			  (elType.ElTypeNum == HTML_EL_HTML
			   || elType.ElTypeNum == HTML_EL_BODY
			   || elType.ElTypeNum == HTML_EL_HEAD))
			el = TtaGetMainRoot (context->doc);
		    }
		  /* fetch and display background image of element */
		  FetchImage (context->doc, el, url, 0,
			      ParseCSSBackgroundImageCallback, callblock);
		}
	    }
		
	  if (url)
	    TtaFreeMemory (url);
	}
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBackgroundRepeat : parse a CSS BackgroundRepeat
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBackgroundRepeat (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSBackgroundRepeat (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
  PresentationValue   repeat;

  repeat.typed_data.value = 0;
  repeat.typed_data.unit = 1;
  cssRule = SkipBlanks (cssRule);
  if (!strncasecmp (cssRule, "no-repeat", 9))
    {
      if (context->drv->GetPictureMode(target, context, &repeat) < 0)
	repeat.typed_data.value = DRIVERP_SCALE;
    }
  else if (!strncasecmp (cssRule, "repeat-y", 8))
    repeat.typed_data.value = DRIVERP_VREPEAT;
  else if (!strncasecmp (cssRule, "repeat-x", 8))
    repeat.typed_data.value = DRIVERP_HREPEAT;
  else if (!strncasecmp (cssRule, "repeat", 6))
    repeat.typed_data.value = DRIVERP_REPEAT;
  else
    return (cssRule);

   /* install the new presentation */
   if (context->drv->SetPictureMode)
       context->drv->SetPictureMode (target, context, repeat);
   cssRule = SkipWord (cssRule);
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBackgroundAttachment : parse a CSS BackgroundAttachment
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBackgroundAttachment (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSBackgroundAttachment (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
   cssRule = SkipBlanks (cssRule);
   if (!strncasecmp (cssRule, "scroll", 6))
     cssRule = SkipWord (cssRule);
   else if (!strncasecmp (cssRule, "fixed", 5))
     cssRule = SkipWord (cssRule);
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBackgroundPosition : parse a CSS BackgroundPosition
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBackgroundPosition (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSBackgroundPosition (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
  PresentationValue   repeat;
  boolean           ok;

   cssRule = SkipBlanks (cssRule);
   ok = TRUE;
   if (!strncasecmp (cssRule, "left", 4))
     cssRule = SkipWord (cssRule);
   else if (!strncasecmp (cssRule, "right", 5))
     cssRule = SkipWord (cssRule);
   else if (!strncasecmp (cssRule, "center", 6))
     cssRule = SkipWord (cssRule);
   else if (!strncasecmp (cssRule, "top", 3))
     cssRule = SkipWord (cssRule);
   else if (!strncasecmp (cssRule, "bottom", 6))
     cssRule = SkipWord (cssRule);
   else if (isdigit (*cssRule))
     cssRule = SkipWord (cssRule);
   else
     ok = FALSE;

   if (ok && context->drv->SetPictureMode)
     {
       /* force realsize for the background image */
       repeat.typed_data.value = DRIVERP_REALSIZE;
       repeat.typed_data.unit = 1;
       context->drv->SetPictureMode (target, context, repeat);
     }

   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBackground : parse a CSS background attribute 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char        *ParseCSSBackground (PresentationTarget target,
				 PresentationContext context, char *cssRule)
#else
static char        *ParseCSSBackground (target, context, cssRule)
PresentationTarget  target;
PresentationContext context;
char               *cssRule;
#endif
{
  cssRule = SkipBlanks (cssRule);
  while (*cssRule != ';' && *cssRule != EOS && *cssRule != ',')
    {
      /* perhaps a Backgroud Image */
      if (!strncasecmp (cssRule, "url", 3))
	cssRule = ParseCSSBackgroundImage (target, context, cssRule);
      /* perhaps a Background Attachment */
      else if (!strncasecmp (cssRule, "scroll", 6) ||
	       !strncasecmp (cssRule, "fixed", 5))
	cssRule = ParseCSSBackgroundAttachment (target, context, cssRule);
      /* perhaps a Background Repeat */
      else if (!strncasecmp (cssRule, "no-repeat", 9) ||
	       !strncasecmp (cssRule, "repeat-y", 8) ||
	       !strncasecmp (cssRule, "repeat-x", 8) ||
	       !strncasecmp (cssRule, "repeat", 6))
	cssRule = ParseCSSBackgroundRepeat (target, context, cssRule);
      /* perhaps a Background Position */
      else if (!strncasecmp (cssRule, "left", 4) ||
	       !strncasecmp (cssRule, "right", 5) ||
	       !strncasecmp (cssRule, "center", 6) ||
	       !strncasecmp (cssRule, "top", 3) ||
	       !strncasecmp (cssRule, "bottom", 6) ||
	       isdigit (*cssRule))
	cssRule = ParseCSSBackgroundPosition (target, context, cssRule);
      /* perhaps a Background Color */
      else
	cssRule = ParseCSSBackgroundColor (target, context, cssRule);
      cssRule = SkipBlanks (cssRule);
    }
   return (cssRule);
}

/************************************************************************
 *									*  
 *	PARSING FUNCTIONS FOR HEADER STYLE DECLARATIONS			*
 *									*  
 ************************************************************************/

/*----------------------------------------------------------------------
   ParseStyleDeclaration : parse one HTML style declaration    
   stored in the header of a HTML document                       
   We expect the style string to be of the form :                   
   [                                                                
   e.g: pinky, awful { color: pink, font-family: helvetica }        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ParseStyleDeclaration (Element el, char *cssRule, Document doc, CSSInfoPtr css)
#else
void                ParseStyleDeclaration (el, cssRule, doc, css)
Element             el;
char               *cssRule;
Document            doc;
CSSInfoPtr          css;
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
   decl_end = cssRule;
   while ((*decl_end != 0) && (*decl_end != '{'))
      decl_end++;
   if (*decl_end == 0)
     {
	fprintf (stderr, "Invalid STYLE declaration header : %s\n", cssRule);
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
   sel = cssRule;

   /*
    * now, deal with the content ...
    */
   decl_end++;
   cssRule = decl_end;
   while ((*decl_end != 0) && (*decl_end != '}'))
      decl_end++;
   if (*decl_end == 0)
     {
	fprintf (stderr, "Invalid STYLE declaration : %s\n", cssRule);
	return;
     }
   sauve2 = *decl_end;
   *decl_end = 0;

   /*
    * parse the style attribute string and install the corresponding
    * presentation attributes on the new element
    */
   ParseCSSGenericStyle (sel, cssRule, doc, css);

   /* restore the string to its original form ! */
   *sel_end = sauve1;
   *decl_end = sauve2;

}

/*----------------------------------------------------------------------
   ParseHTMLClass : parse a CSS Class string                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ParseHTMLClass (Element elem, char *cssRule, Document doc)
#else
void                ParseHTMLClass (elem, cssRule, doc)
Element             elem;
char               *cssRule;
Document            doc;
#endif
{
   ElementType         elType;
   Element             el;
   Attribute           attr;
   AttributeType       attrType;
   PRule               rule, added;
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
	fprintf (stderr, "Invalid class \"%s\" : no class found\n", cssRule);
#endif
	return;
     }
   while (el != NULL)
     {
       attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
       attrType.AttrTypeNum = HTML_ATTR_Selector;
       attr = TtaGetAttribute (el, attrType);
       if (attr)
	 {
	   len = 100;
	   TtaGiveTextAttributeValue (attr, selector, &len);
	   selector[len + 1] = 0;
	   res = EvaluateClassSelector (elem, cssRule, selector, doc);
	   if (res > score)
	     {
	       best = el;
	       score = res;
	     }
	 }
       else
	 MSG ("Invalid StyleRule\n");

       /* get next StyleRule */
       TtaNextSibling (&el);
     }

   /* apply all presentation specific attributes found */
   rule = NULL;
   if (best != NULL)
     do
       {
	 TtaNextPRule (best, &rule);
	 if (rule)
	   {
	     added = TtaCopyPRule (rule);
	     TtaAttachPRule (elem, added, doc);
	   }
       }
     while (rule != NULL);
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
int                 EvaluateClassContext (Element el, char *class, char *selector, Document doc)
#else
int                 EvaluateClassContext (el, class, selector, doc)
Element             el;
char               *class;
char               *selector;
Document            doc;
#endif
{
  Element             father;
  char               *elHtmlName;
  char               *end_str;
  char               *sel = selector;
  char               *names[MAX_DEEP];
  int                 result = 0;
#ifdef DEBUG_STYLES
  int                 i;
  int                 deep;
#endif

  elHtmlName = GetCSSName (el, doc);
  GetCSSNames (el, doc, &names[0], MAX_DEEP);

#ifdef DEBUG_STYLES
  fprintf (stderr, "EvaluateClassContext(%s,%s,%s,%d)\n", elHtmlName, class, selector, doc);
  fprintf (stderr, "Context name : ");
  for (i = 0; i < deep; i++)
    fprintf (stderr, "%s ", names[i]);
  fprintf (stderr, "\n");
#endif

  /*
   * look for a selector (ELEM)
   */
  selector = SkipBlanks (selector);
  if (*selector == '(')
    {
      for (end_str = selector; *end_str; end_str++)
	if (*end_str == ')')
	  break;
      if (*end_str != ')')
	fprintf (stderr, "Unmatched '(' in selector \"%s\"\n", sel);
      else
	{
	  /*
	   * separate the father name, and evaluate it.
	   */
	  *end_str = 0;
	  father = TtaGetParent (el);
	  result = EvaluateClassContext (father, class, selector + 1, doc);
	  *end_str = ')';
	  
	  if (result)
	    {
	      /*
	       * verify that the end of the string match the current element.
	       */
	      if (EvaluateClassContext (el, class, end_str + 1, doc))
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
  fprintf (stderr, "EvaluateClassContext(%s,%s,%s,%d) : %d\n", elHtmlName, class, sel, doc, result);
#endif
  return (result);
}

/*----------------------------------------------------------------------
   EvaluateClassSelector : gives a score for an element in a tree  
   in function of a selector. Three arguments enter in the          
   evaluation process:                                            
   - the class name associated to the element                    
   - the selector string associated to the rule                  
   - the element and it's place in the tree                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 EvaluateClassSelector (Element el, char *class, char *selector, Document doc)
#else
int                 EvaluateClassSelector (el, class, selector, doc)
Element             el;
char               *class;
char               *selector;
Document            doc;
#endif
{
   int                 l = strlen (class);
   int                 L = strlen (selector);
   int                 val = 0;

   val = EvaluateClassContext (el, class, selector, doc);
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
   else
     /* not found, create it ! */
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
   Element          father = NULL;
   Element          el;
   ElementType      elType;

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
   char             name[200];
   char            *cur = &name[0], *first, save;
   SSchema	    schema;

   /* make a local copy */
   strncpy (name, class, 199);
   name[199] = 0;

   /* loop looking if each word is a GI */
   while (*cur != 0)
     {
	first = cur;
	cur = SkipWord (cur);
	save = *cur;
	*cur = 0;
	schema = NULL;
	if (MapGI (first, &schema) == -1)
	  {
	     return (0);
	  }
	*cur = save;
	cur = SkipBlanks (cur);
     }
#ifdef DEBUG_STYLES
   fprintf (stderr, "IsImplicitClassName(%s)\n", class);
#endif
   return (1);
}

/*----------------------------------------------------------------------
   SetHTMLStyleParserDestructiveMode :                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void             SetHTMLStyleParserDestructiveMode (boolean mode)
#else
void             SetHTMLStyleParserDestructiveMode (mode)
boolean          mode;
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
void                HTMLSetBackgroundColor (Document doc, Element el, char *color)
#else
void                HTMLSetBackgroundColor (doc, el, color)
Document            doc;
Element             el;
char               *color;
#endif
{
   char             css_command[100];

   sprintf (css_command, "background: %s", color);
   ParseHTMLSpecificStyle (el, css_command, doc);
}

/*----------------------------------------------------------------------
   HTMLSetBackgroundImage :
   repeat = repeat value
   image = url of background image
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLSetBackgroundImage (Document doc, Element el, int repeat, char *image)
#else
void                HTMLSetBackgroundImage (doc, el, repeat, image)
Document            doc;
Element             el;
int                 repeat;
char               *image;
#endif
{
   char                css_command[100];

   sprintf (css_command, "background-image: url(%s); background-repeat: ", image);
   if (repeat == DRIVERP_REPEAT)
     strcat (css_command, "repeat");
   else if (repeat == DRIVERP_HREPEAT)
     strcat (css_command, "repeat-x");
   else if (repeat == DRIVERP_VREPEAT)
     strcat (css_command, "repeat-y");
   else
     strcat (css_command, "no-repeat");
   ParseHTMLSpecificStyle (el, css_command, doc);
}

/*----------------------------------------------------------------------
   HTMLSetForegroundColor :                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLSetForegroundColor (Document doc, Element el, char *color)
#else
void                HTMLSetForegroundColor (doc, el, color)
Document            doc;
Element             el;
char               *color;
#endif
{
   char             css_command[100];

   sprintf (css_command, "color: %s", color);
   ParseHTMLSpecificStyle (el, css_command, doc);
}

/*----------------------------------------------------------------------
   HTMLResetBackgroundColor :                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLResetBackgroundColor (Document doc, Element el)
#else
void                HTMLResetBackgroundColor (doc, el)
Document            doc;
Element             el;
#endif
{
   char             css_command[100];

   sprintf (css_command, "background: xx");
   SetHTMLStyleParserDestructiveMode (TRUE);
   ParseHTMLSpecificStyle (el, css_command, doc);
   SetHTMLStyleParserDestructiveMode (FALSE);
}

/*----------------------------------------------------------------------
   HTMLResetBackgroundImage :                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLResetBackgroundImage (Document doc, Element el)
#else
void                HTMLResetBackgroundImage (doc, el)
Document            doc;
Element             el;
#endif
{
   char             css_command[1000];

   sprintf (css_command, "background-image: url(xx); background-repeat: repeat");
   SetHTMLStyleParserDestructiveMode (TRUE);
   ParseHTMLSpecificStyle (el, css_command, doc);
   SetHTMLStyleParserDestructiveMode (FALSE);
}

/*----------------------------------------------------------------------
   HTMLResetForegroundColor :                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLResetForegroundColor (Document doc, Element el)
#else
void                HTMLResetForegroundColor (doc, el)
Document            doc;
Element             el;
#endif
{
   char             css_command[100];

   sprintf (css_command, "color: xx");
   SetHTMLStyleParserDestructiveMode (TRUE);
   ParseHTMLSpecificStyle (el, css_command, doc);
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


