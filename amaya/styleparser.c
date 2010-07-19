/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2010
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Everything directly related to the CSS syntax should now hopefully
 * be contained in this module.
 *
 * Author: I. Vatton
 *
 */

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "fetchHTMLname.h"
#include "SVG.h"
#include "XML.h"
#include "document.h"

typedef struct _CSSImageCallbackBlock
{
  Element                el;
  PSchema                tsch;
  CSSInfoPtr             css;
  PresentationContext    ctxt;
  unsigned int           ruleType;
}
CSSImageCallbackBlock, *CSSImageCallbackPtr;

#include "AHTURLTools_f.h"
#include "HTMLpresentation_f.h"
#include "HTMLimage_f.h"
#include "HTMLtable_f.h"
#include "UIcss_f.h"
#include "css_f.h"
#include "fetchHTMLname_f.h"
#include "fetchXMLname_f.h"
#include "html2thot_f.h"
#include "init_f.h"
#include "styleparser_f.h"
#include "SVGbuilder_f.h"
#include "wxdialogapi_f.h"

#define MAX_BUFFER_LENGTH 200
/*
 * A PropertyParser is a function used to parse  the
 * description substring associated to a given style attribute
 * e.g.: "red" for a color attribute or "12pt bold helvetica"
 * for a font attribute.
 */
typedef char *(*PropertyParser) (Element element,
                                 PSchema tsch,
                                 PresentationContext context,
                                 char *cssRule,
                                 CSSInfoPtr css,
                                 ThotBool isHTML);

/* Description of the set of CSS properties supported */
typedef struct CSSProperty
{
  const char          *name;
  PropertyParser       parsing_function;
}
CSSProperty;

static int           LineNumber = -1; /* The line where the error occurs */
static int           NewLineSkipped = 0;
static int           RedisplayImages = 0; /* number of BG images loading */
static int           Style_parsing = 0; /* > 0 when parsing a set of CSS rules */
static char         *ImportantPos = NULL;
static ThotBool      RedisplayBGImage = FALSE; /* TRUE when a BG image is inserted */
static ThotBool      DoApply = TRUE;
static ThotBool      All_sides = FALSE; // TRUE when "boder valus must be displayed
static char          CSSbuffer[MAX_CSS_LENGTH + 1];


/*----------------------------------------------------------------------
  SkipWord:                                                  
  ----------------------------------------------------------------------*/
static char *SkipWord (char *ptr)
{
  while (isalnum((int)*ptr) || *ptr == '-' || *ptr == '#' || *ptr == '%' || *ptr == '.')
    ptr++;
  return (ptr);
}

/*----------------------------------------------------------------------
  SkipBlanksAndComments:                                                  
  ----------------------------------------------------------------------*/
char *SkipBlanksAndComments (char *ptr)
{
  /* skip spaces */
  while (*ptr == SPACE ||
         *ptr == BSPACE ||
         *ptr == EOL ||
         *ptr == TAB ||
         *ptr == __CR__)
    {
      if (*ptr == EOL)
        /* increment the number of newline skipped */
        NewLineSkipped++;
      ptr++;
    }
  while (ptr[0] == '/' && ptr[1] == '*')
    {
      /* look for the end of the comment */
      ptr = &ptr[2];
      while (ptr[0] != EOS && (ptr[0] != '*' || ptr[1] != '/'))
        ptr++;
      if (ptr[0] != EOS)
        ptr = &ptr[2];
      /* skip spaces */
      while (*ptr == SPACE || *ptr == BSPACE || *ptr == EOL ||
             *ptr == TAB || *ptr == __CR__)
        {
          if (*ptr == EOL)
            /* increment the number of newline skipped */
            NewLineSkipped++;
          ptr++;
        }
    }
  return (ptr);
}


/*----------------------------------------------------------------------
  Number of values
  ----------------------------------------------------------------------*/
static int NumberOfValues (char *ptr)
{
  int n = 0;
  while (*ptr != EOS && *ptr != ';' &&  *ptr != '}' &&  *ptr != ',')
    {
      ptr = SkipBlanksAndComments (ptr);
      n++;
      ptr = SkipWord (ptr);
    }
  return n;
}

/*----------------------------------------------------------------------
  SkipQuotedString
  ----------------------------------------------------------------------*/
static char *SkipQuotedString (char *ptr, char quote)
{
  ThotBool	stop;

  stop = FALSE;
  while (!stop)
    {
      if (*ptr == quote)
        {
          ptr++;
          stop = TRUE;
        }
      else if (*ptr == EOS)
        stop = TRUE;
      else if (*ptr == '\\')
        /* escape character */
        {
          ptr++;
          if ((*ptr >= '0' && *ptr <= '9') || (*ptr >= 'A' && *ptr <= 'F') ||
              (*ptr >= 'a' && *ptr <= 'f'))
            {
              ptr++;
              if ((*ptr >= '0' && *ptr <= '9') || (*ptr >= 'A' && *ptr <= 'F') ||
                  (*ptr >= 'a' && *ptr <= 'f'))
                ptr++;
            }
          else
            ptr++;
        }
      else
        ptr++;
    }
  return (ptr);
}

/*----------------------------------------------------------------------
  CSSPrintError
  print the error message msg on stderr.
  When the line is 0 ask expat the current line number
  ----------------------------------------------------------------------*/
static void CSSPrintError (const char *msg, const char *value)
{
  if (!IgnoreErrors && !DoDialog && !TtaIsPrinting () && ParsedDoc > 0)
    {
      if (!ErrFile)
        {
          if (OpenParsingErrors (ParsedDoc) == FALSE)
            return;
        }

      /* check if a CSS error file shoulb be updated too */
      if (ParsedCSS > 0 && !CSSErrFile)
        OpenParsingErrors (ParsedCSS);

      if (Error_DocURL)
        {
          fprintf (ErrFile, "\n*** Errors/warnings in %s\n", Error_DocURL);
          /* set to NULL as long as the CSS file doesn't change */
          Error_DocURL = NULL;
        }
      CSSErrorsFound = TRUE;
      if (LineNumber < 0)
        {
          if (value)
            fprintf (ErrFile, "  In style attribute, %s \"%s\"\n", msg, value);
          else
            fprintf (ErrFile, "  In style attribute, %s\n", msg);
        }
      else
        {
          if (value)
            fprintf (ErrFile, "@  line %d: %s \"%s\"\n",
                     LineNumber+NewLineSkipped, msg, value);
          else
            fprintf (ErrFile, "@  line %d: %s\n", LineNumber+NewLineSkipped,
                     msg);
          if (CSSErrFile)
            {
              if (value)
                fprintf (CSSErrFile, "@  line %d: %s \"%s\"\n",
                         LineNumber+NewLineSkipped, msg, value);
              else
                fprintf (CSSErrFile, "@  line %d: %s\n",
                         LineNumber+NewLineSkipped, msg);
            }
        }
    }
}

/*----------------------------------------------------------------------
  CSSParseError
  print the error message msg on stderr.
  ----------------------------------------------------------------------*/
static void CSSParseError (const char *msg, const char *value, char *endvalue)
{
  char        c = EOS;

  if (endvalue)
    {
      /* close the string here */
      c = *endvalue;
      *endvalue = EOS;
    }
  CSSPrintError (msg, value);
  if (endvalue)
    *endvalue = c;
}

/*----------------------------------------------------------------------
  SkipString move to the end of the string
  ----------------------------------------------------------------------*/
static char *SkipString (char *ptr)
{
  char        c = *ptr;

  ptr++;
  while (*ptr != EOS &&
         (*ptr != c || (*ptr == c && ptr[-1] == '\\')))
    ptr++;
  return ptr;
}

/*----------------------------------------------------------------------
  CSSCheckEndValue
  print an error message if another character is found
  ----------------------------------------------------------------------*/
static char *CSSCheckEndValue (char *cssRule, char *endvalue, const char *msg)
{
  char        c = EOS;
  if (*endvalue != EOS && *endvalue != SPACE && *endvalue != '/' &&
      *endvalue != ';' && *endvalue != '}' && *endvalue != EOL && 
      *endvalue != TAB && *endvalue !=  __CR__)
    {
      while (*endvalue != EOS && *endvalue != SPACE && *endvalue != '/' &&
             *endvalue != ';' && *endvalue != '}' && *endvalue != EOL &&
             *endvalue != TAB && *endvalue !=  __CR__)
        {
          if (*endvalue == '"' || *endvalue == '\'')
            endvalue = SkipString (endvalue);
          if (*endvalue != EOS)
            endvalue++;
        }
      /* close the string here */
      c = *endvalue;
      *endvalue = EOS;
      CSSPrintError (msg, cssRule);
      *endvalue = c;
    }
  return endvalue;
}


/*----------------------------------------------------------------------
  SkipProperty skips a property and display and error message
  ----------------------------------------------------------------------*/
static char *SkipProperty (char *ptr, ThotBool reportError)
{
  char       *deb;
  char        c;
  ThotBool    warn;

  // check if Amaya should report CSS warnings
  TtaGetEnvBoolean ("CSS_WARN", &warn);
  if (!warn)
    reportError = FALSE;
  deb = ptr;
  while (*ptr != EOS && *ptr != ';' && *ptr != '}')
    {
      if (*ptr == '"' || *ptr == '\'')
        ptr = SkipString (ptr);
      if (*ptr != EOS)
        ptr++;
    }
  /* print the skipped property */
  c = *ptr;
  if (*ptr != EOS)
    *ptr = EOS;
  if (DoDialog)
    DisplayStyleValue ("", deb, ptr);
  else if (reportError && *deb != EOS &&
           strncasecmp (deb, "azimuth", 7) &&
           strncasecmp (deb, "border-collapse", 15) &&
           strncasecmp (deb, "border-spacing", 14) &&
           strncasecmp (deb, "caption-side", 12) &&
           strncasecmp (deb, "clip", 4) &&
           strncasecmp (deb, "cue-after", 9) &&
           strncasecmp (deb, "cue-before", 10) &&
           strncasecmp (deb, "cue", 3) &&
           strncasecmp (deb, "cursor", 6) &&
           strncasecmp (deb, "elevation", 9) &&
           strncasecmp (deb, "empty-cells", 11) &&
           strncasecmp (deb, "font-strech", 11) &&
           strncasecmp (deb, "letter-spacing", 14) &&
           strncasecmp (deb, "marker-offset", 12) &&
           strncasecmp (deb, "orphans", 7) &&
           strncasecmp (deb, "outline-color", 13) &&
           strncasecmp (deb, "outline-style", 13) &&
           strncasecmp (deb, "outline-width", 13) &&
           strncasecmp (deb, "outline", 7) &&
           strncasecmp (deb, "overflow", 8) &&
           strncasecmp (deb, "pause-after", 11) &&
           strncasecmp (deb, "pause-before", 12) &&
           strncasecmp (deb, "pause", 5) &&
           strncasecmp (deb, "quotes", 6) &&
           strncasecmp (deb, "richness", 8) &&
           strncasecmp (deb, "speech-rate", 11) &&
           strncasecmp (deb, "speak-header", 12) &&
           strncasecmp (deb, "speak-punctuation", 17) &&
           strncasecmp (deb, "speak-numeral", 13) &&
           strncasecmp (deb, "speak", 5) &&
           strncasecmp (deb, "pitch-range", 11) &&
           strncasecmp (deb, "pitch", 5) &&
           strncasecmp (deb, "stress", 6) &&
           strncasecmp (deb, "table-layout", 12) &&
           strncasecmp (deb, "text-shadow", 11) &&
           strncasecmp (deb, "voice-family", 12) &&
           strncasecmp (deb, "volume", 6) &&
           strncasecmp (deb, "widows", 6))
    CSSPrintError ("CSS property ignored:", deb);
  if (c != EOS)
    *ptr = c;
  return (ptr);
}

/*----------------------------------------------------------------------
  SkipValue
  skips the value and display an error message if msg is not NULL
  ----------------------------------------------------------------------*/
static char *SkipValue (const char *msg, char *ptr)
{
  char       *deb;
  char        c;

  deb = ptr;
  while (*ptr != EOS && *ptr != ';' && *ptr != '}' && *ptr != '\n')
    {
      if (*ptr == '"' || *ptr == '\'')
        ptr = SkipString (ptr);
      if (*ptr != EOS)
        ptr++;
    }
  /* print the skipped property */
  c = *ptr;
  if (c != EOS)
    *ptr = EOS;
  if (msg && *deb != EOS && *deb != ',')
    CSSPrintError (msg, deb);
  if (c != EOS)
    *ptr = c;
  return (ptr);
}

/*----------------------------------------------------------------------
  ParseNumber:                                                  
  parse a number and returns the corresponding value.
  ----------------------------------------------------------------------*/
char *ParseNumber (char *cssRule, PresentationValue *pval)
{
  int                 val = 0;
  int                 minus = 0;
  int                 valid = 0;
  int                 f = 0;
  ThotBool            real = FALSE;

  pval->typed_data.unit = UNIT_REL;
  pval->typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  if (*cssRule == '-')
    {
      minus = 1;
      cssRule++;
      cssRule = SkipBlanksAndComments (cssRule);
    }
  else if (*cssRule == '+')
    {
      cssRule++;
      cssRule = SkipBlanksAndComments (cssRule);
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
      real = TRUE;
      f = val;
      val = 0;
      cssRule++;
      /* keep only 3 digits */
      if (*cssRule >= '0' && *cssRule <= '9')
        {
          val = (*cssRule - '0') * 100;
          cssRule++;
          if (*cssRule >= '0' && *cssRule <= '9')
            {
              val += (*cssRule - '0') * 10;
              cssRule++;
              if ((*cssRule >= '0') && (*cssRule <= '9'))
                {
                  val += *cssRule - '0';
                  cssRule++;
                }
            }

          while (*cssRule >= '0' && *cssRule <= '9')
            cssRule++;
          valid = 1;
        }
    }

  if (!valid)
    {
      pval->typed_data.unit = UNIT_INVALID;
      pval->typed_data.value = 0;
    }
  else
    {
      pval->typed_data.real = real;
      if (real)
        {
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
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSUnit a number followed by a CSS Unit and returns the corresponding      
  value and its unit.                                           
  ----------------------------------------------------------------------*/
char *ParseCSSUnit (char *cssRule, PresentationValue *pval)
{
  char               *p;
  unsigned int        uni;

  pval->typed_data.unit = UNIT_REL;
  cssRule = ParseNumber (cssRule, pval);
  if (pval->typed_data.unit == UNIT_INVALID)
    /* it does not start with a valid number */
    cssRule = SkipWord (cssRule);
  else
    {
      /* is there a space after the number? */
      p = cssRule;
      cssRule = SkipBlanksAndComments (cssRule);
      if (p == cssRule)
        /* no space */
        p = NULL;
      else
        /* a space is here. restore the pointer */
        cssRule = p;
      uni = 0;
      while (CSSUnitNames[uni].sign)
        {
          if (!strncasecmp (CSSUnitNames[uni].sign, cssRule,
                            strlen (CSSUnitNames[uni].sign)))
            /* this is a correct unit */
            {
              pval->typed_data.unit = CSSUnitNames[uni].unit;
              if (p)
                /* there was a space before the unit. Syntax error */
                pval->typed_data.unit = UNIT_INVALID;
              return (cssRule + strlen (CSSUnitNames[uni].sign));
            }
          else
            uni++;
        }
      /* not in the list of accepted units */
      pval->typed_data.unit = UNIT_BOX;
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseClampedUnit:                                                  
  parse a CSS Unit substring and returns the corresponding value and unit.
  [0,1]
  ----------------------------------------------------------------------*/
char *ParseClampedUnit (char *cssRule, PresentationValue *pval)
{
  char           *p;

  p = cssRule;
  cssRule = ParseNumber (cssRule, pval);
  if (*cssRule != EOS && *cssRule != SPACE && *cssRule != ';' && *cssRule != '}')
    /* the value contains something after the number. Invalid */
    {
      cssRule++;
      pval->typed_data.unit = UNIT_REL;
      if (pval->typed_data.value > 100)
        pval->typed_data.value = 1000;
      else
        pval->typed_data.value *= 10;
      CSSParseError ("Invalid value", p, cssRule);
    }
  else
    /* it's a number. Check its value */
    {
      pval->typed_data.unit = UNIT_REL;
      if (pval->typed_data.real)
        pval->typed_data.real = FALSE;
      else if (pval->typed_data.value > 1)
        {
          pval->typed_data.value = 1000;
          CSSParseError ("Invalid value", p, cssRule);
        }
      else if (pval->typed_data.value < 0)
        {
          pval->typed_data.value = 0;
          CSSParseError ("Invalid value", p, cssRule);
        }
      else
        pval->typed_data.value *= 1000;
    }
  pval->data = pval->typed_data.value;
  return (cssRule);
}


/*----------------------------------------------------------------------
  ParseABorderValue                                       
  ----------------------------------------------------------------------*/
static char *ParseABorderValue (char *cssRule, PresentationValue *border)
{
  char             *ptr = cssRule;

  /* first parse the attribute string */
  border->typed_data.value = 0;
  border->typed_data.unit = UNIT_INVALID;
  border->typed_data.real = FALSE;
  if (!strncasecmp (cssRule, "thin", 4))
    {
      border->typed_data.unit = UNIT_PX;
      border->typed_data.value = 1;
      cssRule += 4;
    }
  else if (!strncasecmp (cssRule, "medium", 6))
    {
      border->typed_data.unit = UNIT_PX;
      border->typed_data.value = 3;
      cssRule += 6;
    }
  else if (!strncasecmp (cssRule, "thick", 5))
    {
      border->typed_data.unit = UNIT_PX;
      border->typed_data.value = 5;
      cssRule += 5;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      border->typed_data.unit = VALUE_INHERIT;
      cssRule += 7;
    }
  else if (isdigit (*cssRule) || *cssRule == '.')
    {
      cssRule = ParseCSSUnit (cssRule, border);
      if (border->typed_data.value == 0 &&
          border->typed_data.unit != UNIT_INVALID)
        border->typed_data.unit = UNIT_PX;
      else if (border->typed_data.unit == UNIT_INVALID ||
               border->typed_data.unit == UNIT_BOX ||
               border->typed_data.unit == UNIT_PERCENT)
        {
          border->typed_data.unit = UNIT_INVALID;
          border->typed_data.value = 0;
          CSSParseError ("Invalid border-width value", ptr, cssRule);
        }
    }
  return (cssRule);
}


/*----------------------------------------------------------------------
  ParseBorderStyle                                      
  ----------------------------------------------------------------------*/
static char *ParseBorderStyle (char *cssRule, PresentationValue *border)
{
  char             *ptr = cssRule;

  /* first parse the attribute string */
  border->typed_data.value = 0;
  border->typed_data.unit = UNIT_PX;
  border->typed_data.real = FALSE;
  if (!strncasecmp (cssRule, "none", 4))
    {
      border->typed_data.value = BorderStyleNone;
      cssRule += 4;
    }
  else if (!strncasecmp (cssRule, "hidden", 6))
    {
      border->typed_data.value = BorderStyleHidden;
      cssRule += 6;
    }
  else if (!strncasecmp (cssRule, "dotted", 6))
    {
      cssRule += 6;
      border->typed_data.value = BorderStyleDotted;
    }
  else if (!strncasecmp (cssRule, "dashed", 6))
    {
      border->typed_data.value = BorderStyleDashed;
      cssRule += 6;
    }
  else if (!strncasecmp (cssRule, "solid", 5))
    {
      border->typed_data.value = BorderStyleSolid;
      cssRule += 5;
    }
  else if (!strncasecmp (cssRule, "double", 6))
    {
      border->typed_data.value = BorderStyleDouble;
      cssRule += 6;
    }
  else if (!strncasecmp (cssRule, "groove", 6))
    {
      border->typed_data.value = BorderStyleGroove;
      cssRule += 6;
    }
  else if (!strncasecmp (cssRule, "ridge", 5))
    {
      border->typed_data.value = BorderStyleRidge;
      cssRule += 5;
    }
  else if (!strncasecmp (cssRule, "inset", 5))
    {
      border->typed_data.value = BorderStyleInset;
      cssRule += 5;
    }
  else if (!strncasecmp (cssRule, "outset", 6))
    {
      border->typed_data.value = BorderStyleOutset;
      cssRule += 6;
    }
  else
    {
      /* invalid style */
      border->typed_data.unit = UNIT_INVALID;
      return (cssRule);
    }
  if (border->typed_data.value != 0)
    cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid border-style value");
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSColor: parse a CSS color attribute string    
  we expect the input string describing the attribute to be     
  either a color name, a 3 tuple or an hexadecimal encoding.    
  The color used will be approximed from the current color      
  table                                                         
  ----------------------------------------------------------------------*/
static char *ParseCSSColor (char *cssRule, PresentationValue * val)
{
  char               *ptr;
  unsigned short      redval = (unsigned short) -1;
  unsigned short      greenval = 0;	/* composant of each RGB       */
  unsigned short      blueval = 0;	/* default to red if unknown ! */
  int                 best = 0;	/* best color in list found */
  ThotBool            warn;

  cssRule = SkipBlanksAndComments (cssRule);
  val->typed_data.unit = UNIT_INVALID;
  val->typed_data.real = FALSE;
  val->typed_data.value = 0;
  ptr = TtaGiveRGB (cssRule, &redval, &greenval, &blueval);
  if (!strncasecmp (cssRule, "InactiveCaptionText", 19))
    {
      val->typed_data.unit = VALUE_INHERIT;
      cssRule += 19;
    }
  else if (!strncasecmp (cssRule, "ThreeDLightShadow", 17))
    {
      val->typed_data.unit = VALUE_INHERIT;
      cssRule += 17;
    }
  else if (!strncasecmp (cssRule, "ThreeDDarkShadow", 16))
    {
      val->typed_data.unit = VALUE_INHERIT;
      cssRule += 16;
    }
  else if (!strncasecmp (cssRule, "ButtonHighlight", 15) ||
           !strncasecmp (cssRule, "InactiveCaption", 15) ||
           !strncasecmp (cssRule, "ThreeDHighlight", 15))
    {
      val->typed_data.unit = VALUE_INHERIT;
      cssRule += 15;
    }
  else if (!strncasecmp (cssRule, "InactiveBorder", 14) ||
           !strncasecmp (cssRule, "InfoBackground", 14))
    {
      val->typed_data.unit = VALUE_INHERIT;
      cssRule += 14;
    }
  else if (!strncasecmp (cssRule, "ActiveCaption", 13) ||
           !strncasecmp (cssRule, "HighlightText", 13))
    {
      val->typed_data.unit = VALUE_INHERIT;
      cssRule += 13;
    }
  else if (!strncasecmp (cssRule, "ActiveBorder", 12) ||
           !strncasecmp (cssRule, "AppWorkspace", 12) ||
           !strncasecmp (cssRule, "ButtonShadow", 12) ||
           !strncasecmp (cssRule, "ThreeDShadow", 12))
    {
      val->typed_data.unit = VALUE_INHERIT;
      cssRule += 12;
    }
  else if (!strncasecmp (cssRule, "CaptionText", 11) ||
           !strncasecmp (cssRule, "WindowFrame", 11))
    {
      val->typed_data.unit = VALUE_INHERIT;
      cssRule += 11;
    }
  else if (!strncasecmp (cssRule, "Background", 10) ||
           !strncasecmp (cssRule, "ButtonFace", 10) ||
           !strncasecmp (cssRule, "ButtonText", 10) ||
           !strncasecmp (cssRule, "ThreeDFace", 10) ||
           !strncasecmp (cssRule, "WindowText", 10))
    {
      val->typed_data.unit = VALUE_INHERIT;
      cssRule += 10;
    }
  else if (!strncasecmp (cssRule, "Highlight", 9) ||
           !strncasecmp (cssRule, "Scrollbar", 9))
    {
      val->typed_data.unit = VALUE_INHERIT;
      cssRule += 9;
    }
  else if (!strncasecmp (cssRule, "GrayText", 8) ||
           !strncasecmp (cssRule, "InfoText", 8) ||
           !strncasecmp (cssRule, "MenuText", 8))
    {
      val->typed_data.unit = VALUE_INHERIT;
      cssRule += 8;
    }
  else if (!strncasecmp (cssRule, "Window", 6))
    {
      val->typed_data.unit = VALUE_INHERIT;
      cssRule += 6;
    }
  else if (!strncasecmp (cssRule, "Menu", 5))
    {
      val->typed_data.unit = VALUE_INHERIT;
      cssRule += 5;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      val->typed_data.unit = VALUE_INHERIT;
      cssRule += 7;
    }
  else if (!strncasecmp (cssRule, "hsl", 3))
    {
      val->typed_data.unit = VALUE_INHERIT;
      // check if Amaya should report CSS warnings
      TtaGetEnvBoolean ("CSS_WARN", &warn);
      if (warn)
	cssRule = SkipValue ("Warning: CSS3 value not supported", cssRule);
      else
	cssRule = SkipValue (NULL, cssRule);
    }
  else if (!strncasecmp (cssRule, "rgba", 4))
    {
      val->typed_data.unit = VALUE_INHERIT;
      // check if Amaya should report CSS warnings
      TtaGetEnvBoolean ("CSS_WARN", &warn);
      if (warn)
	cssRule = SkipValue ("Warning: CSS3 value not supported", cssRule);
      else
	cssRule = SkipValue (NULL, cssRule);
    }
  if (ptr == cssRule)
    {
      cssRule = SkipValue ("Invalid color value", cssRule);
      val->typed_data.value = 0;
      val->typed_data.unit = UNIT_INVALID;
    }
  else if (val->typed_data.unit != VALUE_INHERIT)
    {
      best = TtaGetThotColor (redval, greenval, blueval);
      val->typed_data.value = best;
      val->typed_data.unit = UNIT_REL;
      cssRule = ptr;
    }
  val->typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
  CheckImportantRule updates the field important of the context and
  the line number.
  ----------------------------------------------------------------------*/
static void CheckImportantRule (char *cssRule, PresentationContext context)
{
  PresentationContextBlock dummyctxt;

  if (context == NULL)
    /* no context provided */
    context = &dummyctxt;

  cssRule = SkipBlanksAndComments (cssRule);
  while (*cssRule != EOS && *cssRule != '!' && *cssRule != ';')
    cssRule++;
  if (*cssRule != '!')
    context->important = FALSE;
  else
    {
      cssRule++;
      ImportantPos = cssRule;
      cssRule = SkipBlanksAndComments (cssRule);
      if (!strncasecmp (cssRule, "important", 9))
        {
          ImportantPos[-1] = EOS;
          context->important = TRUE;
        }
      else
        {
          ImportantPos = NULL;
          context->important = FALSE;
        }
    }
}

/*----------------------------------------------------------------------
  SkipImportantRule skips important markup
  ----------------------------------------------------------------------*/
static char *SkipImportantRule (char *cssRule)
{
  if (ImportantPos)
    {
      ImportantPos[-1] = '!';
      cssRule = ImportantPos;
      cssRule = SkipBlanksAndComments (cssRule);
      cssRule += 9;
      ImportantPos = NULL;
    }
  cssRule = SkipBlanksAndComments (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBorderTopWidth: parse a CSS BorderTopWidth
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSBorderTopWidth (Element element, PSchema tsch,
                                     PresentationContext context, 
                                     char *cssRule, CSSInfoPtr css,
                                     ThotBool isHTML)
{
  PresentationValue   border;
  char               *start_value = cssRule;
  
  cssRule = SkipBlanksAndComments (cssRule);
  cssRule = ParseABorderValue (cssRule, &border);
  if (border.typed_data.unit != UNIT_INVALID)
    {
      if (DoDialog)
        {
          if (All_sides)
            DisplayStyleValue ("border-width", start_value, cssRule);
          else
            DisplayStyleValue ("border-top-width", start_value, cssRule);
        }
      else if (DoApply)
        TtaSetStylePresentation (PRBorderTopWidth, element, tsch, context, border);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBorderBottomWidth: parse a CSS BorderBottomWidth
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSBorderBottomWidth (Element element, PSchema tsch,
                                        PresentationContext context,
                                        char *cssRule, CSSInfoPtr css,
                                        ThotBool isHTML)
{
  PresentationValue   border;
  char               *start_value = cssRule;
  
  cssRule = SkipBlanksAndComments (cssRule);
  /* first parse the attribute string */
  cssRule = ParseABorderValue (cssRule, &border);
  if (border.typed_data.unit != UNIT_INVALID)
    {
      if (DoDialog)
        DisplayStyleValue ("border-bottom-width", start_value, cssRule);
      else if (DoApply)
        TtaSetStylePresentation (PRBorderBottomWidth, element, tsch, context, border);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBorderLeftWidth: parse a CSS BorderLeftWidth
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSBorderLeftWidth (Element element, PSchema tsch,
                                      PresentationContext context,
                                      char *cssRule, CSSInfoPtr css,
                                      ThotBool isHTML)
{
  PresentationValue   border;
  char               *start_value = cssRule;
  
  cssRule = SkipBlanksAndComments (cssRule);
  /* first parse the attribute string */
  cssRule = ParseABorderValue (cssRule, &border);
  if (border.typed_data.unit != UNIT_INVALID)
    {
      if (DoDialog)
        DisplayStyleValue ("border-left-width", start_value, cssRule);
      else if (DoApply)
        TtaSetStylePresentation (PRBorderLeftWidth, element, tsch, context, border);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBorderRightWidth: parse a CSS BorderRightWidth
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSBorderRightWidth (Element element, PSchema tsch,
                                       PresentationContext context,
                                       char *cssRule, CSSInfoPtr css,
                                       ThotBool isHTML)
{
  PresentationValue   border;
  char               *start_value = cssRule;
  
  cssRule = SkipBlanksAndComments (cssRule);
  /* first parse the attribute string */
  cssRule = ParseABorderValue (cssRule, &border);
  if (border.typed_data.unit != UNIT_INVALID && DoApply)
    {
      if (DoDialog)
        DisplayStyleValue ("border-right-width", start_value, cssRule);
      else if (DoApply)
        TtaSetStylePresentation (PRBorderRightWidth, element, tsch, context, border);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBorderWidth: parse a CSS BorderWidth
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSBorderWidth (Element element, PSchema tsch,
                                  PresentationContext context,
                                  char *cssRule, CSSInfoPtr css,
                                  ThotBool isHTML)
{
  char *ptrT, *ptrR, *ptrB, *ptrL;
  int   skippedNL, n;

  ptrT = SkipBlanksAndComments (cssRule);
  if (DoDialog)
    n = NumberOfValues (ptrT);
  if (DoDialog && n < 2)
    {
      // check if the border dialog must be updated
      All_sides = TRUE;
     ptrR = ParseCSSBorderTopWidth (element, tsch, context, ptrT, css, isHTML);
      All_sides = FALSE;
    }
  else
    {
      /* First parse Border-Top */
      ptrR = ParseCSSBorderTopWidth (element, tsch, context, ptrT, css, isHTML);
      if (ptrR == ptrT)
	{
	  // invalid value
          cssRule = SkipValue ("Invalid border-width value", ptrT);
	  return (cssRule);
	}
      ptrR = SkipBlanksAndComments (ptrR);
      if (*ptrR == ';' || *ptrR == '}' || *ptrR == EOS || *ptrR == ',')
        {
          skippedNL = NewLineSkipped;
          cssRule = ptrR;
          /* apply the Border-Top to all */
          ptrR = ParseCSSBorderRightWidth (element, tsch, context, ptrT, css, isHTML);
          NewLineSkipped = skippedNL;
          ptrR = ParseCSSBorderBottomWidth (element, tsch, context, ptrT, css, isHTML);
          NewLineSkipped = skippedNL;
          ptrR = ParseCSSBorderLeftWidth (element, tsch, context, ptrT, css, isHTML);
        }
      else
        {
          /* parse Border-Right */
          ptrB = ParseCSSBorderRightWidth (element, tsch, context, ptrR, css, isHTML);
          ptrB = SkipBlanksAndComments (ptrB);
          if (*ptrB == ';' || *ptrB == '}' || *ptrB == EOS || *ptrB == ',')
            {
              skippedNL = NewLineSkipped;
              cssRule = ptrB;
              /* apply the Border-Top to Border-Bottom */
              ptrB = ParseCSSBorderBottomWidth (element, tsch, context, ptrT, css, isHTML);
              NewLineSkipped = skippedNL;
              /* apply the Border-Right to Border-Left */
              ptrB = ParseCSSBorderLeftWidth (element, tsch, context, ptrR, css, isHTML);
            }
          else
            {
              /* parse Border-Bottom */
              ptrL = ParseCSSBorderBottomWidth (element, tsch, context, ptrB, css, isHTML);
              ptrL = SkipBlanksAndComments (ptrL);
              if (*ptrL == ';' || *ptrL == '}' || *ptrL == EOS || *ptrL == ',')
                {
                  cssRule = ptrL;
                  /* apply the Border-Right to Border-Left */
                  ptrL = ParseCSSBorderLeftWidth (element, tsch, context, ptrR, css, isHTML);
                }
              else
                /* parse Border-Left */
                cssRule = ParseCSSBorderLeftWidth (element, tsch, context, ptrL, css, isHTML);
              cssRule = SkipBlanksAndComments (cssRule);
            }
        }
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBorderColorTop: parse a CSS BorderColorTop
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSBorderColorTop (Element element, PSchema tsch,
                                     PresentationContext context,
                                     char *cssRule, CSSInfoPtr css,
                                     ThotBool isHTML)
{
  PresentationValue   best;
  char               *start_value = cssRule;

  if (!strncasecmp (cssRule, "transparent", 11))
    {
      best.typed_data.value = -2;  /* -2 means transparent */
      best.typed_data.unit = UNIT_REL;
      cssRule = SkipWord (cssRule);
    }
  else
    cssRule = ParseCSSColor (cssRule, &best);
  if (best.typed_data.unit != UNIT_INVALID)
    {
      if (DoDialog)
        {
          if (All_sides)
            DisplayStyleValue ("border-color", start_value, cssRule);
          else
            DisplayStyleValue ("border-top-color", start_value, cssRule);
        }
      else if (DoApply)
        /* install the new presentation */
        TtaSetStylePresentation (PRBorderTopColor, element, tsch, context, best);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBorderColorLeft: parse a CSS BorderColorLeft
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSBorderColorLeft (Element element, PSchema tsch,
                                      PresentationContext context,
                                      char *cssRule, CSSInfoPtr css,
                                      ThotBool isHTML)
{
  PresentationValue   best;
  char               *start_value = cssRule;
  
  if (!strncasecmp (cssRule, "transparent", 11))
    {
      best.typed_data.value = -2;  /* -2 means transparent */
      best.typed_data.unit = UNIT_REL;
      cssRule = SkipWord (cssRule);
    }
  else
    cssRule = ParseCSSColor (cssRule, &best);
  if (best.typed_data.unit != UNIT_INVALID && DoApply)
    {
      if (DoDialog)
        DisplayStyleValue ("border-left-color", start_value, cssRule);
      else if (DoApply)
        /* install the new presentation */
        TtaSetStylePresentation (PRBorderLeftColor, element, tsch, context, best);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBorderColorBottom: parse a CSS BorderColorBottom
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSBorderColorBottom (Element element, PSchema tsch,
                                        PresentationContext context,
                                        char *cssRule, CSSInfoPtr css,
                                        ThotBool isHTML)
{
  PresentationValue   best;
  char               *start_value = cssRule;

  if (!strncasecmp (cssRule, "transparent", 11))
    {
      best.typed_data.value = -2;  /* -2 means transparent */
      best.typed_data.unit = UNIT_REL;
      cssRule = SkipWord (cssRule);
    }
  else
    cssRule = ParseCSSColor (cssRule, &best);
  if (best.typed_data.unit != UNIT_INVALID && DoApply)
    {
      if (DoDialog)
        DisplayStyleValue ("border-bottom-color", start_value, cssRule);
      else if (DoApply)
        /* install the new presentation */
        TtaSetStylePresentation (PRBorderBottomColor, element, tsch, context, best);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBorderColorRight: parse a CSS BorderColorRight
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSBorderColorRight (Element element, PSchema tsch,
                                       PresentationContext context,
                                       char *cssRule, CSSInfoPtr css,
                                       ThotBool isHTML)
{
  PresentationValue   best;
  char               *start_value = cssRule;

  if (!strncasecmp (cssRule, "transparent", 11))
    {
      best.typed_data.value = -2;  /* -2 means transparent */
      best.typed_data.unit = UNIT_REL;
      cssRule = SkipWord (cssRule);
    }
  else
    cssRule = ParseCSSColor (cssRule, &best);
  if (best.typed_data.unit != UNIT_INVALID && DoApply)
    {
      if (DoDialog)
        DisplayStyleValue ("border-right-color", start_value, cssRule);
      else if (DoApply)
        TtaSetStylePresentation (PRBorderRightColor, element, tsch, context, best);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBorderColor: parse a CSS border-color        
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSBorderColor (Element element, PSchema tsch,
                                  PresentationContext context,
                                  char *cssRule, CSSInfoPtr css,
                                  ThotBool isHTML)
{
  char *ptrT, *ptrR, *ptrB, *ptrL;
  int   skippedNL, n;

  ptrT = SkipBlanksAndComments (cssRule);
  if (DoDialog)
    n = NumberOfValues (ptrT);
  if (DoDialog && n < 2)
    {
      // check if the border dialog must be updated
      All_sides = TRUE;
     ptrR = ParseCSSBorderColorTop (element, tsch, context, ptrT, css, isHTML);
      All_sides = FALSE;
    }
  else
    {
      /* First parse Border-Top */
      ptrR = ParseCSSBorderColorTop (element, tsch, context, ptrT, css, isHTML);
      ptrR = SkipBlanksAndComments (ptrR);
      if (*ptrR == ';' || *ptrR == '}' || *ptrR == EOS || *ptrR == ',')
        {
          skippedNL = NewLineSkipped;
          cssRule = ptrR;
          /* apply the Border-Top to all */
          ptrR = ParseCSSBorderColorRight (element, tsch, context, ptrT, css, isHTML);
          NewLineSkipped = skippedNL;
          ptrR = ParseCSSBorderColorBottom (element, tsch, context, ptrT, css, isHTML);
          NewLineSkipped = skippedNL;
          ptrR = ParseCSSBorderColorLeft (element, tsch, context, ptrT, css, isHTML);
        }
      else
        {
          /* parse Border-Right */
          ptrB = ParseCSSBorderColorRight (element, tsch, context, ptrR, css, isHTML);
          ptrB = SkipBlanksAndComments (ptrB);
          if (*ptrB == ';' || *ptrB == '}' || *ptrB == EOS || *ptrB == ',')
            {
              skippedNL = NewLineSkipped;
              cssRule = ptrB;
              /* apply the Border-Top to Border-Bottom */
              ptrB = ParseCSSBorderColorBottom (element, tsch, context, ptrT, css, isHTML);
              NewLineSkipped = skippedNL;
              /* apply the Border-Right to Border-Left */
              ptrB = ParseCSSBorderColorLeft (element, tsch, context, ptrR, css, isHTML);
            }
          else
            {
              skippedNL = NewLineSkipped;
              /* parse Border-Bottom */
              ptrL = ParseCSSBorderColorBottom (element, tsch, context, ptrB, css, isHTML);
              NewLineSkipped = skippedNL;
              ptrL = SkipBlanksAndComments (ptrL);
              if (*ptrL == ';' || *ptrL == '}' || *ptrL == EOS || *ptrL == ',')
                {
                  cssRule = ptrL;
                  /* apply the Border-Right to Border-Left */
                  ptrL = ParseCSSBorderColorLeft (element, tsch, context, ptrR, css, isHTML);
                }
              else
                /* parse Border-Left */
                cssRule = ParseCSSBorderColorLeft (element, tsch, context, ptrL, css, isHTML);
              cssRule = SkipBlanksAndComments (cssRule);
            }
        }
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBorderStyleTop: parse a CSS BorderStyleTop
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSBorderStyleTop (Element element, PSchema tsch,
                                     PresentationContext context,
                                     char *cssRule, CSSInfoPtr css,
                                     ThotBool isHTML)
{
  PresentationValue   border;
  char               *start_value;
  
  cssRule = SkipBlanksAndComments (cssRule);
  start_value = cssRule;
  cssRule = ParseBorderStyle (cssRule, &border);
  if (border.typed_data.unit != UNIT_INVALID)
    {
      if (DoDialog)
        {
          if (All_sides)
            DisplayStyleValue ("border-style", start_value, cssRule);
          else
            DisplayStyleValue ("border-top-style", start_value, cssRule);
        }
      else if (DoApply)
        TtaSetStylePresentation (PRBorderTopStyle, element, tsch, context, border);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBorderStyleLeft: parse a CSS BorderStyleLeft
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSBorderStyleLeft (Element element, PSchema tsch,
                                      PresentationContext context,
                                      char *cssRule, CSSInfoPtr css,
                                      ThotBool isHTML)
{
  PresentationValue   border;
  char               *start_value;
  
  cssRule = SkipBlanksAndComments (cssRule);
  start_value = cssRule;
  cssRule = ParseBorderStyle (cssRule, &border);
  if (border.typed_data.unit != UNIT_INVALID)
    {
      if (DoDialog)
        DisplayStyleValue ("border-left-style", start_value, cssRule);
      else if (DoApply)
        TtaSetStylePresentation (PRBorderLeftStyle, element, tsch, context, border);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBorderStyleBottom: parse a CSS BorderStyleBottom
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSBorderStyleBottom (Element element, PSchema tsch,
                                        PresentationContext context,
                                        char *cssRule, CSSInfoPtr css,
                                        ThotBool isHTML)
{
  PresentationValue   border;
  char               *start_value;
  
  cssRule = SkipBlanksAndComments (cssRule);
  start_value = cssRule;
  cssRule = ParseBorderStyle (cssRule, &border);
  if (border.typed_data.unit != UNIT_INVALID)
    {
      if (DoDialog)
        DisplayStyleValue ("border-bottom-style", start_value, cssRule);
      else if (DoApply)
        TtaSetStylePresentation (PRBorderBottomStyle, element, tsch, context, border);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBorderStyleRight: parse a CSS BorderStyleRight
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSBorderStyleRight (Element element, PSchema tsch,
                                       PresentationContext context,
                                       char *cssRule, CSSInfoPtr css,
                                       ThotBool isHTML)
{
  PresentationValue   border;
  char               *start_value;
  
  cssRule = SkipBlanksAndComments (cssRule);
  start_value = cssRule;
  cssRule = ParseBorderStyle (cssRule, &border);
  if (border.typed_data.unit != UNIT_INVALID && DoApply)
    {
      if (DoDialog)
        DisplayStyleValue ("border-right-style", start_value, cssRule);
      else if (DoApply)
        TtaSetStylePresentation (PRBorderRightStyle, element, tsch, context, border);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBorderStyle: parse a CSS border-style attribute string.
  ----------------------------------------------------------------------*/
static char *ParseCSSBorderStyle (Element element, PSchema tsch,
                                  PresentationContext context,
                                  char *cssRule, CSSInfoPtr css,
                                  ThotBool isHTML)
{
  char *ptrT, *ptrR, *ptrB, *ptrL;
  int   skippedNL, n;

  ptrT = SkipBlanksAndComments (cssRule);
  if (DoDialog)
    n = NumberOfValues (ptrT);
  if (DoDialog && n < 2)
    {
      // check if the border dialog must be updated
      All_sides = TRUE;
     ptrR =  ParseCSSBorderStyleTop(element, tsch, context, ptrT, css, isHTML);
      All_sides = FALSE;
    }
  else
    {
      /* First parse Border-Top */
      ptrR = ParseCSSBorderStyleTop (element, tsch, context, ptrT, css, isHTML);
      ptrR = SkipBlanksAndComments (ptrR);
      if (*ptrR == ';' || *ptrR == '}' || *ptrR == EOS || *ptrR == ',')
        {
          skippedNL = NewLineSkipped;
          cssRule = ptrR;
          /* apply the Border-Top to all */
          ptrR = ParseCSSBorderStyleRight (element, tsch, context, ptrT, css, isHTML);
          NewLineSkipped = skippedNL;
          ptrR = ParseCSSBorderStyleBottom (element, tsch, context, ptrT, css, isHTML);
          NewLineSkipped = skippedNL;
          ptrR = ParseCSSBorderStyleLeft (element, tsch, context, ptrT, css, isHTML);
        }
      else
        {
          /* parse Border-Right */
          ptrB = ParseCSSBorderStyleRight (element, tsch, context, ptrR, css, isHTML);
          ptrB = SkipBlanksAndComments (ptrB);
          if (*ptrB == ';' || *ptrR == '}' || *ptrB == EOS || *ptrB == ',')
            {
              skippedNL = NewLineSkipped;
              cssRule = ptrB;
              /* apply the Border-Top to Border-Bottom */
              ptrB = ParseCSSBorderStyleBottom (element, tsch, context, ptrT, css, isHTML);
              NewLineSkipped = skippedNL;
              /* apply the Border-Right to Border-Left */
              ptrB = ParseCSSBorderStyleLeft (element, tsch, context, ptrR, css, isHTML);
            }
          else
            {
              /* parse Border-Bottom */
              ptrL = ParseCSSBorderStyleBottom (element, tsch, context, ptrB, css, isHTML);
              ptrL = SkipBlanksAndComments (ptrL);
              if (*ptrL == ';' || *ptrR == '}' || *ptrL == EOS || *ptrL == ',')
                {
                  cssRule = ptrL;
                  /* apply the Border-Right to Border-Left */
                  ptrL = ParseCSSBorderStyleLeft (element, tsch, context, ptrR, css, isHTML);
                }
              else
                /* parse Border-Left */
                cssRule = ParseCSSBorderStyleLeft (element, tsch, context, ptrL, css, isHTML);
              cssRule = SkipBlanksAndComments (cssRule);
            }
        }
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBorderTop: parse a CSS BorderTop
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSBorderTop (Element element, PSchema tsch,
                                PresentationContext context, char *cssRule,
                                CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   best;
  char               *ptr;
  ThotBool            style, width, color;

  cssRule = SkipBlanksAndComments (cssRule);
  /* register given values */
  if (!strncmp (cssRule, "none", 4))
    style = width = color = TRUE;
  else
    style = width = color = FALSE;
  while (*cssRule != ';' && *cssRule != '}' && *cssRule != EOS && *cssRule != ',')
    {
      ptr = cssRule;
      cssRule = ParseCSSBorderStyleTop (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
        {
          cssRule = ParseCSSBorderTopWidth (element, tsch, context, cssRule, css, isHTML);
          if (ptr == cssRule)
            {
              cssRule = ParseCSSBorderColorTop (element, tsch, context, cssRule, css, isHTML);
              if (ptr != cssRule)
                color = TRUE;
            }
          else
            width = TRUE;
          if (ptr == cssRule)
            {
              /* rule not found */
              cssRule = SkipValue ("Invalid border value", cssRule);
              return (cssRule);
            }
        }
      else
        style = TRUE;
      cssRule = SkipBlanksAndComments (cssRule);
    }

  if (!width)
    ParseCSSBorderTopWidth (element, tsch, context, (char*)"medium", css, isHTML);
  if (!style)
    ParseCSSBorderStyleTop (element, tsch, context, (char*)"none", css, isHTML);
  if (!color && DoApply)
    {
      /* get the box color */
      best.typed_data.value = -1;
      best.typed_data.unit = UNIT_REL; 
      best.typed_data.real = FALSE;
      TtaSetStylePresentation (PRBorderTopColor, element, tsch, context, best);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBorderLeft: parse a CSS BorderLeft
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSBorderLeft (Element element, PSchema tsch,
                                 PresentationContext context, char *cssRule,
                                 CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   best;
  char               *ptr;
  ThotBool            style, width, color;

  cssRule = SkipBlanksAndComments (cssRule);
  /* register given values */
  if (!strncmp (cssRule, "none", 4))
    style = width = color = TRUE;
  else
    style = width = color = FALSE;
  while (*cssRule != ';' && *cssRule != '}' && *cssRule != EOS && *cssRule != ',')
    {
      ptr = cssRule;
      cssRule = ParseCSSBorderStyleLeft (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
        {
          cssRule = ParseCSSBorderLeftWidth (element, tsch, context, cssRule, css, isHTML);
          if (ptr == cssRule)
            {
              cssRule = ParseCSSBorderColorLeft (element, tsch, context, cssRule, css, isHTML);
              if (ptr != cssRule)
                color = TRUE;
            }
          else
            width = TRUE;
          if (ptr == cssRule)
            {
              /* rule not found */
              cssRule = SkipValue ("Invalid border value", cssRule);
              return (cssRule);
            }
        }
      else
        style = TRUE;
      cssRule = SkipBlanksAndComments (cssRule);
    }

  if (!width)
    ParseCSSBorderLeftWidth (element, tsch, context, (char*)"medium", css, isHTML);
  if (!style)
    ParseCSSBorderStyleLeft (element, tsch, context, (char*)"none", css, isHTML);
  if (!color && DoApply)
    {
      /* get the box color */
      best.typed_data.value = -1;
      best.typed_data.unit = UNIT_REL;
      best.typed_data.real = FALSE;
      TtaSetStylePresentation (PRBorderLeftColor, element, tsch, context, best);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBorderBottom: parse a CSS BorderBottom
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSBorderBottom (Element element, PSchema tsch,
                                   PresentationContext context, char *cssRule,
                                   CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   best;
  char               *ptr;
  ThotBool            style, width, color;

  cssRule = SkipBlanksAndComments (cssRule);
  /* register given values */
  if (!strncmp (cssRule, "none", 4))
    style = width = color = TRUE;
  else
    style = width = color = FALSE;
  while (*cssRule != ';' && *cssRule != '}' && *cssRule != EOS && *cssRule != ',')
    {
      ptr = cssRule;
      cssRule = ParseCSSBorderStyleBottom (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
        {
          cssRule = ParseCSSBorderBottomWidth (element, tsch, context, cssRule, css, isHTML);
          if (ptr == cssRule)
            {
              cssRule = ParseCSSBorderColorBottom (element, tsch, context, cssRule, css, isHTML);
              if (ptr != cssRule)
                color = TRUE;
            }
          else
            width = TRUE;
          if (ptr == cssRule)
            {
              /* rule not found */
              cssRule = SkipValue ("Invalid border value", cssRule);
              return (cssRule);
            }
        }
      else
        style = TRUE;
      cssRule = SkipBlanksAndComments (cssRule);
    }

  if (!width)
    ParseCSSBorderBottomWidth (element, tsch, context, (char*)"medium", css, isHTML);
  if (!style)
    ParseCSSBorderStyleBottom (element, tsch, context, (char*)"none", css, isHTML);
  if (!color && DoApply)
    {
      /* get the box color */
      best.typed_data.value = -1;
      best.typed_data.unit = UNIT_REL;
      best.typed_data.real = FALSE;
      TtaSetStylePresentation (PRBorderBottomColor, element, tsch, context, best);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBorderRight: parse a CSS BorderRight
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSBorderRight (Element element, PSchema tsch,
                                  PresentationContext context, char *cssRule,
                                  CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   best;
  char               *ptr;
  ThotBool            style, width, color;

  cssRule = SkipBlanksAndComments (cssRule);
  /* register given values */
  if (!strncmp (cssRule, "none", 4))
    style = width = color = TRUE;
  else
    style = width = color = FALSE;
  while (*cssRule != ';' && *cssRule != '}' && *cssRule != EOS && *cssRule != ',')
    {
      ptr = cssRule;
      cssRule = ParseCSSBorderStyleRight (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
        {
          cssRule = ParseCSSBorderRightWidth (element, tsch, context, cssRule, css, isHTML);
          if (ptr == cssRule)
            {
              cssRule = ParseCSSBorderColorRight (element, tsch, context, cssRule, css, isHTML);
              if (ptr != cssRule)
                color = TRUE;
            }
          else
            width = TRUE;
          if (ptr == cssRule)
            {
              /* rule not found */
              cssRule = SkipValue ("Invalid border value", cssRule);
              return (cssRule);
            }
        }
      else
        style = TRUE;
      cssRule = SkipBlanksAndComments (cssRule);
    }

  if (!width)
    ParseCSSBorderRightWidth (element, tsch, context, (char*)"medium", css, isHTML);
  if (!style)
    ParseCSSBorderStyleRight (element, tsch, context, (char*)"none", css, isHTML);
  if (!color && DoApply)
    {
      /* get the box color */
      best.typed_data.value = -1;
      best.typed_data.unit = UNIT_REL;
      best.typed_data.real = FALSE;
      TtaSetStylePresentation (PRBorderRightColor, element, tsch, context, best);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBorder: parse a CSS border        
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSBorder (Element element, PSchema tsch,
                             PresentationContext context, char *cssRule,
                             CSSInfoPtr css, ThotBool isHTML)
{
  char *ptrT, *ptrR;
  int   skippedNL, n;

  ptrT = SkipBlanksAndComments (cssRule);
  if (DoDialog)
    n = NumberOfValues (ptrT);
  if (DoDialog && n < 4)
    {
      // check if the border dialog must be updated
      All_sides = TRUE;
     ptrR = ParseCSSBorderTop (element, tsch, context, ptrT, css, isHTML);
      All_sides = FALSE;
    }
  else
    {
      /* First parse Border-Top */
      ptrR = ParseCSSBorderTop (element, tsch, context, ptrT, css, isHTML);
      ptrR = SkipBlanksAndComments (ptrR);
      if (*ptrR == ';' || *ptrR == '}' || *ptrR == EOS || *ptrR == ',')
        {
          skippedNL = NewLineSkipped;
          cssRule = ptrR;
          /* apply the Border-Top to all */
          ptrR = ParseCSSBorderRight (element, tsch, context, ptrT, css, isHTML);
          NewLineSkipped = skippedNL;
          ptrR = ParseCSSBorderBottom (element, tsch, context, ptrT, css, isHTML);
          NewLineSkipped = skippedNL;
          ptrR = ParseCSSBorderLeft (element, tsch, context, ptrT, css, isHTML);
        }
    }
  return (cssRule);
}


/*----------------------------------------------------------------------
  ParseCSSFloat: parse a CSS float attribute string    
  ----------------------------------------------------------------------*/
static char *ParseCSSFloat (Element element, PSchema tsch,
                            PresentationContext context, char *cssRule,
                            CSSInfoPtr css, ThotBool isHTML)
{
  DisplayMode         dispMode;
  PresentationValue   pval;
  char               *ptr = cssRule;
  ThotBool            warn;

  // check if Amaya should report CSS warnings
  TtaGetEnvBoolean ("CSS_WARN", &warn);
  pval.typed_data.value = 0;
  pval.typed_data.unit = UNIT_BOX;
  pval.typed_data.real = FALSE;
  if (!strncasecmp (cssRule, "inherit", 7))
    {
      pval.typed_data.unit = VALUE_INHERIT;
      cssRule += 7;
    }
  if (!strncasecmp (cssRule, "none", 4))
    {
      pval.typed_data.value = FloatNone;
      cssRule += 4;
    }
  else if (!strncasecmp (cssRule, "left", 4))
    {
      pval.typed_data.value = FloatLeft;
      cssRule += 4;
    }
  else if (!strncasecmp (cssRule, "right", 5))
    {
      pval.typed_data.value = FloatRight;
      cssRule += 5;
    }

  if (pval.typed_data.value == 0 && pval.typed_data.unit != VALUE_INHERIT)
    {
      if (!strncasecmp (cssRule, "top", 3) ||
          !strncasecmp (cssRule, "bottom", 6) ||
          !strncasecmp (cssRule, "inside", 6) ||
          !strncasecmp (cssRule, "outside", 7) ||
          !strncasecmp (cssRule, "start", 5) ||
          !strncasecmp (cssRule, "end", 3))
	{
	  if (warn)
	    cssRule = SkipValue ("Warning: CSS3 value not supported", cssRule);
	  else
	    cssRule = SkipValue (NULL, cssRule);
	}
      else
        cssRule = SkipValue ("Invalid float value", cssRule);
    }
  else
    {
      if (DoDialog)
        DisplayStyleValue ("float", ptr, cssRule);
      else if (DoApply)
        {
          dispMode = TtaGetDisplayMode (context->doc);
          if (dispMode != NoComputedDisplay)
            {
              /* force a redisplay of the whole document */
              TtaSetDisplayMode (context->doc, NoComputedDisplay);
#ifdef AMAYA_DEBUG
              /*printf ("Force NoComputedDisplay doc=%d\n", context->doc);*/
#endif /* AMAYA_DEBUG */
            }
          TtaSetStylePresentation (PRFloat, element, tsch, context, pval);
        }
      cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid float value");
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSClear: parse a CSS clear rule 
  ----------------------------------------------------------------------*/
static char *ParseCSSClear (Element element, PSchema tsch,
                            PresentationContext context, char *cssRule,
                            CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   pval;
  char               *start_value = cssRule;

  pval.typed_data.value = 0;
  pval.typed_data.unit = UNIT_BOX;
  pval.typed_data.real = FALSE;
  if (!strncasecmp (cssRule, "inherit", 7))
    pval.typed_data.unit = VALUE_INHERIT;
  if (!strncasecmp (cssRule, "none", 4))
    pval.typed_data.value = ClearNone;
  else if (!strncasecmp (cssRule, "left", 4))
    pval.typed_data.value = ClearLeft;
  else if (!strncasecmp (cssRule, "right", 5))
    pval.typed_data.value = ClearRight;
  else if (!strncasecmp (cssRule, "both", 4))
    pval.typed_data.value = ClearBoth;

  if (pval.typed_data.value == 0 && pval.typed_data.unit != VALUE_INHERIT)
    {
      cssRule = SkipValue ("Invalid clear value", cssRule);
      cssRule = SkipValue (NULL, cssRule);
    }
  else
    {
      cssRule = SkipValue (NULL, cssRule);
      if (DoDialog)
        DisplayStyleValue ("clear", start_value, cssRule);
      else if (DoApply)
        TtaSetStylePresentation (PRClear, element, tsch, context, pval);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSVisibility: parse a CSS visibility attribute string        
  ----------------------------------------------------------------------*/
static char *ParseCSSVisibility(Element element, PSchema tsch,
                                PresentationContext context, char *cssRule,
                                CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   pval;
  char               *ptr;

  pval.typed_data.unit = UNIT_REL;
  pval.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  if (!strncasecmp (cssRule, "hidden", 6))
    {
      cssRule += 6;
      pval.typed_data.value = VsHidden;
    }
  else if (!strncasecmp (cssRule, "visible", 7))
    {
      cssRule += 7;
      pval.typed_data.value = VsVisible;
    }
  else if (!strncasecmp (cssRule, "collapse", 8))
    {
      cssRule += 8;
      pval.typed_data.value = VsCollapse;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      cssRule += 7;
      pval.typed_data.value = VsInherit;
    }
  else
    {
      cssRule = SkipValue ("Invalid visibility value", cssRule);
      return (cssRule);
    }
  if (DoDialog)
    DisplayStyleValue ("visibility", ptr, cssRule);
  else if (DoApply)
    TtaSetStylePresentation (PRVis, element, tsch, context, pval);
  cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid visibility value");
  return (cssRule);
}


/*----------------------------------------------------------------------
  ParseCSSDisplay: parse a CSS display attribute string        
  ----------------------------------------------------------------------*/
static char *ParseCSSDisplay (Element element, PSchema tsch,
                              PresentationContext context, char *cssRule,
                              CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   pval;
  char               *ptr;
  ThotBool            warn;

  pval.typed_data.unit = UNIT_REL;
  pval.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  if (!strncasecmp (cssRule, "none", 4))
    {
      cssRule += 4;
      pval.typed_data.value = DisplayNone;
    }
  else if (!strncasecmp (cssRule, "block", 5))
    {
      cssRule += 5;
      pval.typed_data.value = Block;
    }
  else if (!strncasecmp (cssRule, "inline-block", 12))
    {
      cssRule += 12;
      pval.typed_data.value = InlineBlock;
    }
  else if (!strncasecmp (cssRule, "inline", 6))
    {
      cssRule += 6;
      pval.typed_data.value = Inline;
    }
  else if (!strncasecmp (cssRule, "list-item", 9))
    {
      cssRule += 9;
      pval.typed_data.value = ListItem;
    }
  else if (!strncasecmp (cssRule, "run-in", 6))
    {
      cssRule += 6;
      pval.typed_data.value = RunIn;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      cssRule += 7;
      pval.typed_data.unit = VALUE_INHERIT;
    }
  else
    {
      TtaGetEnvBoolean ("CSS_WARN", &warn);
      if (warn &&
	  strncasecmp (cssRule, "table-row-group", 15) &&
          strncasecmp (cssRule, "table-column-group", 18) &&
          strncasecmp (cssRule, "table-header-group", 5) &&
          strncasecmp (cssRule, "table-footer-group", 6) &&
          strncasecmp (cssRule, "table-row", 9) &&
          strncasecmp (cssRule, "table-column", 12) &&
          strncasecmp (cssRule, "table-cell", 10) &&
          strncasecmp (cssRule, "table-caption", 13) &&
          strncasecmp (cssRule, "inline-table", 12) &&
          strncasecmp (cssRule, "table", 5))
        cssRule = SkipValue ("Display value not supported", cssRule);
      else
        cssRule = SkipWord (cssRule);
      return (cssRule);
    }

  if (DoDialog)
    DisplayStyleValue ("display", ptr, cssRule);
  else if (DoApply)
    TtaSetStylePresentation (PRDisplay, element, tsch, context, pval);
  cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid display value");
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSLetterSpacing: parse a CSS letter-spacing    
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSLetterSpacing (Element element, PSchema tsch,
                                    PresentationContext context, char *cssRule,
                                    CSSInfoPtr css, ThotBool isHTML)
{
  char               *start_value = cssRule;

  cssRule = SkipValue (NULL, cssRule);
  if (DoDialog)
    DisplayStyleValue ("letter-spacing", start_value, cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCounterStyle: parse a CSS counter style.
  ----------------------------------------------------------------------*/
static char *ParseCounterStyle (char *cssRule, PresentationValue *pval,
				char *start_value)
{
  (*pval).typed_data.unit = UNIT_REL;
  (*pval).typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  start_value = cssRule;
  if (!strncasecmp (cssRule, "disc", 4))
    {
      cssRule += 4;
      (*pval).typed_data.value = Disc;
    }
  else if (!strncasecmp (cssRule, "circle", 6))
    {
      cssRule += 6;
      (*pval).typed_data.value = Circle;
    }
  else if (!strncasecmp (cssRule, "square", 6))
    {
      cssRule += 6;
      (*pval).typed_data.value = Square;
    }
  else if (!strncasecmp (cssRule, "decimal", 7))
    {
      cssRule += 7;
      (*pval).typed_data.value = Decimal;
    }
  else if (!strncasecmp (cssRule, "decimal-leading-zero", 20))
    {
      cssRule += 20;
      (*pval).typed_data.value = DecimalLeadingZero;
    }
  else if (!strncasecmp (cssRule, "lower-roman", 11))
    {
      cssRule += 11;
      (*pval).typed_data.value = LowerRoman;
    }
  else if (!strncasecmp (cssRule, "upper-roman", 11))
    {
      cssRule += 11;
      (*pval).typed_data.value = UpperRoman;
    }
  else if (!strncasecmp (cssRule, "lower-greek", 11))
    {
      cssRule += 11;
      (*pval).typed_data.value = LowerGreek;
    }
  else if (!strncasecmp (cssRule, "upper-greek", 11))
    {
      cssRule += 11;
      (*pval).typed_data.value = UpperGreek;
    }
  else if (!strncasecmp (cssRule, "lower-latin", 11))
    {
      cssRule += 11;
      (*pval).typed_data.value = LowerLatin;
    }
  else if (!strncasecmp (cssRule, "upper-latin", 11))
    {
      cssRule += 11;
      (*pval).typed_data.value = UpperLatin;
    }
  else if (!strncasecmp (cssRule, "armenian", 8))
    {
      cssRule += 8;
      (*pval).typed_data.value = Decimal;
    }
  else if (!strncasecmp (cssRule, "georgian", 8))
    {
      cssRule += 8;
      (*pval).typed_data.value = Decimal;
    }
  else if (!strncasecmp (cssRule, "lower-alpha", 11))
    {
      cssRule += 11;
      (*pval).typed_data.value = LowerLatin;
    }
  else if (!strncasecmp (cssRule, "upper-alpha", 11))
    {
      cssRule += 11;
      (*pval).typed_data.value = UpperLatin;
    }
  else if (!strncasecmp (cssRule, "none", 4))
    {
      cssRule += 4;
      (*pval).typed_data.value = ListStyleTypeNone;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      cssRule += 7;
      (*pval).typed_data.unit = VALUE_INHERIT;
    }
  else
    cssRule = SkipValue ("Invalid list-style-type value", cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseACSSListStyleType: parse a CSS list-style-type
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseACSSListStyleType (Element element, PSchema tsch,
                                     PresentationContext context, char *cssRule,
                                     CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   pval;
  char               *start_value;

  cssRule = ParseCounterStyle (cssRule, &pval, start_value);
  if (DoDialog)
    DisplayStyleValue ("list-style-type", start_value, cssRule);
  else if (DoApply)
    TtaSetStylePresentation (PRListStyleType, element, tsch, context, pval);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSListStyleType: parse a CSS list-style-type
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSListStyleType (Element element, PSchema tsch,
                                    PresentationContext context, char *cssRule,
                                    CSSInfoPtr css, ThotBool isHTML)
{
  char               *ptr = cssRule;
  cssRule = ParseACSSListStyleType (element, tsch, context, cssRule, css,
                                    isHTML);
  cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid list-style-type value");
  return cssRule;
}

/*----------------------------------------------------------------------
  ParseCSSUrl: parse an URL
  Parse the url content (don't start with "url")
  Return the next pointer in the CSS string
  If a correct URL is found, it's returned in url (this string must
  be freed)
  ----------------------------------------------------------------------*/
static char *ParseCSSUrl (char *cssRule, char **url)
{
  char                       saved;
  char                      *base, *ptr;

  cssRule = SkipBlanksAndComments (cssRule);
  saved = *cssRule;
  if (*cssRule == '(')
    {
      cssRule++;
      cssRule = SkipBlanksAndComments (cssRule);
      /*** Escaped quotes are not handled. See function SkipQuotedString */
      if (*cssRule == '"')
        {
          cssRule++;
          base = cssRule;
          while (*cssRule != EOS && *cssRule != '"')
            cssRule++;
        }
      else if (*cssRule == '\'')
        {
          cssRule++;
          base = cssRule;
          while (*cssRule != EOS && *cssRule != '\'')
            cssRule++;
        }
      else
        {
          base = cssRule;
          while (*cssRule != EOS && *cssRule != ')')
            cssRule++;
        }
      /* keep the current position */
      ptr = cssRule;
      if (saved == '(')
        {
          /* remove extra spaces */
          if (cssRule[-1] == SPACE)
            {
              *cssRule = SPACE;
              cssRule--;
              while (cssRule[-1] == SPACE)
                cssRule--;
            }
        }
      saved = *cssRule;
      *cssRule = EOS;
      *url = TtaStrdup (base);
      *cssRule = saved;
      if (saved == '"' || saved == '\'')
        /* we need to skip the quote character and possible spaces */
        {
          cssRule++;
          cssRule = SkipBlanksAndComments (cssRule);
        }
      else
        cssRule = ptr;
    }
  cssRule++;
  return cssRule;
}

/*----------------------------------------------------------------------
  ParseCSSImageCallback: Callback called asynchronously by
  FetchImage when a CSS image (background-image or list-style-image)
  has been fetched.
  ----------------------------------------------------------------------*/
void ParseCSSImageCallback (Document doc, Element element, char *file,
                            void *extra, ThotBool isnew)
{
  DisplayMode                dispMode = DisplayImmediately;
  CSSImageCallbackPtr        callblock;
  Element                    el;
  PSchema                    tsch;
  Document                   redisplaydoc;
  PInfoPtr                   pInfo = NULL;
  CSSInfoPtr                 css;
  PresentationContext        ctxt;
  PresentationValue          image;
  PresentationValue          value;
  ThotBool                   found;

  callblock = (CSSImageCallbackPtr) extra;
  if (callblock == NULL)
    return;

  css = callblock->css;
  el = callblock->el;
  tsch = callblock->tsch;
  ctxt = callblock->ctxt;
  redisplaydoc = ctxt->doc;
  if (doc == 0 && !isnew)
    /* apply to the current document only */
    doc = redisplaydoc;
  if (doc)
    redisplaydoc = doc;
  else
    {
      /* check if the CSS still exists */
      css = CSSList;
      while (css && css != callblock->css)
        css = css->NextCSS;
      if (css == NULL)
        // the presentation schema doesn't exist anymore
        tsch = NULL;
    }

  if (tsch && css && ctxt && redisplaydoc)
    {
      // check if the presentation schema is still there
      pInfo = css->infos[redisplaydoc];
      if (pInfo == NULL && DocumentURLs[redisplaydoc] == NULL)
        {
          // the redisplaydoc was probably an object
          while (redisplaydoc > 0 && pInfo == 0)
            pInfo = css->infos[--redisplaydoc];
          if (redisplaydoc)
            ctxt->doc = redisplaydoc;
        }

      found = FALSE;
      while (!found && pInfo)
        {
          found = (pInfo->PiSchemas && tsch == pInfo->PiSchemas->PiPSchema);
          pInfo = pInfo->PiNext;
        }
      if (!found)
        tsch = NULL;
   }

  if (el || tsch)
    {
      /* Ok the image was fetched */
      image.typed_data.unit = UNIT_REL;
      image.typed_data.real = FALSE;
      image.pointer = file;
      TtaSetStylePresentation (callblock->ruleType, el, tsch, ctxt, image);
      
      if (callblock->ruleType == PRBackgroundPicture)
        /* enforce the showbox */
        {
          value.typed_data.value = 1;
          value.typed_data.unit = UNIT_REL;
          value.typed_data.real = FALSE;
          TtaSetStylePresentation (PRShowBox, el, tsch, ctxt, value);
        }
      /* check if the context can be freed */
      ctxt->uses -= 1;
      if (ctxt->uses == 0)
        /* no other image loading */
        TtaFreeMemory (ctxt);
    }

  TtaFreeMemory (callblock);
  if (css)
    RedisplayImages--;
  if (redisplaydoc &&
      /* check if all background images are now loaded */
      (css == NULL || (pInfo && Style_parsing == 0 && RedisplayImages == 0)))
    {
      /* don't manage a document used by make book */
      if (DocumentMeta[redisplaydoc] == NULL ||
          DocumentMeta[redisplaydoc]->method != CE_MAKEBOOK)
        {
          /* Change the Display Mode to take into account the new
             presentation */
          dispMode = TtaGetDisplayMode (redisplaydoc);
          /* force the redisplay of this box */
          if (dispMode == DisplayImmediately)
            TtaSetDisplayMode (redisplaydoc, NoComputedDisplay);
          TtaSetDisplayMode (redisplaydoc, dispMode);
        }
      RedisplayBGImage = FALSE;
    }
  else
    RedisplayBGImage = TRUE;
}

/*----------------------------------------------------------------------
  SetCSSImage fetch the image referred by a background-image or a
  list-style-image property.
  ----------------------------------------------------------------------*/
static char *SetCSSImage (Element element, PSchema tsch,
                          PresentationContext ctxt, char *cssRule,
                          CSSInfoPtr css, unsigned int ruleType)
{
  CSSImageCallbackPtr        callblock;
  Element                    el;
  PresentationValue          image;
  char                      *url, *ptr;
  char                      *bg_image;
  char                       tempname[MAX_LENGTH];
  char                       imgname[MAX_LENGTH];

  if (element)
    el = element;
  else
    /* default element for FetchImage */
    el = TtaGetMainRoot (ctxt->doc);
  url = NULL;
  image.typed_data.real = FALSE;
  cssRule = ParseCSSUrl (cssRule, &url);
  if (strlen (url) > MAX_LENGTH / 4)
    url[MAX_LENGTH / 4] = EOS;
  ptr = cssRule;
  if (ctxt->destroy)
    {
      /* remove the background image PRule */
      image.pointer = NULL;
      TtaSetStylePresentation (ruleType, element, tsch, ctxt, image);
    }
  else if (url)
    {
      if (css && css->url)
        /* the image concerns a CSS file */
        NormalizeURL (url, 0, tempname, imgname, css->url);
      else
        /* the image concerns a style element */
        NormalizeURL (url, ctxt->doc, tempname, imgname, NULL);
      if (DoDialog)
        {
          if (ruleType == PRBackgroundPicture)
            DisplayStyleValue ("background-image", tempname, &tempname[MAX_LENGTH-1]);
          else if (ruleType == PRListStyleImage)
            DisplayStyleValue ("list-style-image", tempname, &tempname[MAX_LENGTH-1]);
          else if (ruleType == PRContentURL)
            DisplayStyleValue ("", ptr, cssRule);
        }
      else if (DoApply)
        {
          bg_image = TtaGetEnvString ("ENABLE_BG_IMAGES");
          if (bg_image == NULL || !strcasecmp (bg_image, "yes"))
            /* background images are enabled */
            {
              callblock = (CSSImageCallbackPtr) TtaGetMemory (sizeof (CSSImageCallbackBlock));
              if (callblock)
                {
                  callblock->el = element;
                  callblock->tsch = tsch;
                  callblock->css = css;
                  callblock->ctxt = ctxt;
                  callblock->ruleType = ruleType;
                  /* new use of the context */
                  ctxt->uses += 1;
                  /* check if the image url is related to an external CSS */
                  if (css)
                    {
                      /* fetch and display background image of element */
                      if (FetchImage (0, el, tempname, AMAYA_LOAD_IMAGE,
                                      ParseCSSImageCallback, callblock))
                        RedisplayImages++;
                    }
                  else
                    FetchImage (ctxt->doc, el, url, AMAYA_LOAD_IMAGE,
                                ParseCSSImageCallback, callblock);
                }
            }
        }
      TtaFreeMemory (url);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseACSSListStyleImage: parse a CSS list-style-image
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseACSSListStyleImage (Element element, PSchema tsch,
                                      PresentationContext ctxt,
                                      char *cssRule, CSSInfoPtr css,
                                      ThotBool isHTML)
{
  char               *url;
  char               *start_value;
  PresentationValue   pval;

  pval.typed_data.unit = UNIT_REL;
  pval.typed_data.real = FALSE;
  url = NULL;
  cssRule = SkipBlanksAndComments (cssRule);
  start_value = cssRule;
  if (!strncasecmp (cssRule, "none", 4))
    {
      cssRule += 4;
      pval.typed_data.value = 0;
      if (DoDialog)
        DisplayStyleValue ("list-style-image", start_value, cssRule);
      else if (DoApply)
        TtaSetStylePresentation (PRListStyleImage, element, tsch, ctxt, pval);
    }
  else if (!strncasecmp (cssRule, "url", 3))
    {  
      cssRule += 3;
      cssRule = SetCSSImage (element, tsch, ctxt, cssRule, css,
                             PRListStyleImage);
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      cssRule += 7;
      pval.typed_data.unit = VALUE_INHERIT;
      if (DoDialog)
        DisplayStyleValue ("list-style-image", start_value, cssRule);
      else if (DoApply)
        TtaSetStylePresentation (PRListStyleImage, element, tsch, ctxt, pval);
    }
  else
      cssRule = SkipValue ("Invalid list-style-image value", cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSListStyleImage: parse a CSS list-style-image
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSListStyleImage (Element element, PSchema tsch,
                                     PresentationContext ctxt,
                                     char *cssRule, CSSInfoPtr css,
                                     ThotBool isHTML)
{
  char               *ptr = cssRule;
  cssRule = ParseACSSListStyleImage (element, tsch, ctxt, cssRule, css,
                                     isHTML);
  cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid list-style-image value");
  return cssRule;
}

/*----------------------------------------------------------------------
  ParseACSSListStylePosition: parse a CSS list-style-position
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseACSSListStylePosition (Element element, PSchema tsch,
                                         PresentationContext context,
                                         char *cssRule, CSSInfoPtr css,
                                         ThotBool isHTML)
{
  PresentationValue   pval;
  char               *start_value;

  pval.typed_data.unit = UNIT_REL;
  pval.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  start_value = cssRule;
  if (!strncasecmp (cssRule, "inside", 6))
    {
      pval.typed_data.value = Inside;
      cssRule += 6;
    }
  else if (!strncasecmp (cssRule, "outside", 7))
    {
      pval.typed_data.value = Outside;
      cssRule += 7;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      pval.typed_data.unit = VALUE_INHERIT;
      cssRule += 7;
    }
  else
    {
      cssRule = SkipValue ("Invalid list-style-position value", cssRule);
      return (cssRule);
    }

  if (DoDialog)
    DisplayStyleValue ("list-style-position", start_value, cssRule);
  else if (DoApply)
    TtaSetStylePresentation (PRListStylePosition, element, tsch, context, pval);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSListStylePosition: parse a CSS list-style-position
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSListStylePosition (Element element, PSchema tsch,
                                        PresentationContext context,
                                        char *cssRule, CSSInfoPtr css,
                                        ThotBool isHTML)
{
  char               *ptr = cssRule;
  cssRule = ParseACSSListStylePosition (element, tsch, context, cssRule, css,
                                        isHTML);
  cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid list-style-position value");
  return cssRule;
}

/*----------------------------------------------------------------------
  ParseCSSListStyle: parse a CSS list-style value string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSListStyle (Element element, PSchema tsch,
                                PresentationContext ctxt, char *cssRule,
                                CSSInfoPtr css, ThotBool isHTML)
{
  char               *ptr = cssRule;
  int                 skippedNL;

  cssRule = SkipBlanksAndComments (cssRule);
  while (*cssRule != ';' && *cssRule != '}' && *cssRule != EOS && *cssRule != ',')
    {
      skippedNL = NewLineSkipped;
      /* perhaps a list-style-image */
      if (!strncasecmp (cssRule, "url", 3))
        cssRule = ParseACSSListStyleImage (element, tsch, ctxt, cssRule, css,
                                           isHTML);
      /* perhaps a list-style-position */
      else if (!strncasecmp (cssRule, "inside", 6) ||
               !strncasecmp (cssRule, "outside", 7))
        cssRule = ParseACSSListStylePosition (element, tsch, ctxt, cssRule,
                                              css, isHTML);
      /* perhaps a list-style-type */
      else if (!strncasecmp (cssRule, "disc", 4) ||
               !strncasecmp (cssRule, "circle", 6) ||
               !strncasecmp (cssRule, "square", 6) ||
               !strncasecmp (cssRule, "decimal", 7) ||
               !strncasecmp (cssRule, "decimal-leading-zero", 20) ||
               !strncasecmp (cssRule, "lower-roman", 11) ||
               !strncasecmp (cssRule, "upper-roman", 11) ||
               !strncasecmp (cssRule, "lower-greek", 11) ||
               !strncasecmp (cssRule, "lower-latin", 11) ||
               !strncasecmp (cssRule, "lower-alpha", 11) ||
               !strncasecmp (cssRule, "upper-latin", 11) ||
               !strncasecmp (cssRule, "upper-alpha", 11) ||
               !strncasecmp (cssRule, "armenian", 8) ||
               !strncasecmp (cssRule, "georgian", 8) ||
               !strncasecmp (cssRule, "none", 4) ||
               !strncasecmp (cssRule, "inherit", 7))
        cssRule = ParseACSSListStyleType (element, tsch, ctxt, cssRule, css,
                                          isHTML);
      else
        {
          NewLineSkipped = skippedNL;
          /* rule not found */
          cssRule = SkipProperty (cssRule, FALSE);
        }
      cssRule = SkipBlanksAndComments (cssRule);
    }
  cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid list-style value");
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSTextAlign: parse a CSS text-align            
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSTextAlign (Element element, PSchema tsch,
                                PresentationContext context, char *cssRule,
                                CSSInfoPtr css, ThotBool isHTML)
{
  char               *ptr;
  PresentationValue   align;

  align.typed_data.value = 0;
  align.typed_data.unit = UNIT_REL;
  align.typed_data.real = FALSE;

  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  if (!strncasecmp (cssRule, "left", 4))
    {
      align.typed_data.value = AdjustLeft;
      cssRule += 4;
    }
  else if (!strncasecmp (cssRule, "right", 5))
    {
      align.typed_data.value = AdjustRight;
      cssRule += 5;
    }
  else if (!strncasecmp (cssRule, "center", 6))
    {
      align.typed_data.value = Centered;
      cssRule += 6;
    }
  else if (!strncasecmp (cssRule, "justify", 7))
    {
      align.typed_data.value = Justify;
      cssRule += 7;
    }
  else
    {
      cssRule = SkipValue ("Invalid text-align value", cssRule);
      return (cssRule);
    }

  /*
   * install the new presentation.
   */
  if (align.typed_data.value)
    {
      if (DoDialog)
        DisplayStyleValue ("text-align", ptr, cssRule);
      else if (DoApply)
        TtaSetStylePresentation (PRAdjust, element, tsch, context, align);
    }
  cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid text-align value");
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSTextAnchor: parse a CSS text-anchor property (SVG property)
  We use the Thot Adjust PRule to represent the text-anchor property
  for CSS 1.0, as Adjust is not used otherwise in this context.
  ----------------------------------------------------------------------*/
static char *ParseCSSTextAnchor (Element element, PSchema tsch,
                                 PresentationContext context, char *cssRule,
                                 CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   align;
  char               *ptr;

  align.typed_data.value = 0;
  align.typed_data.unit = UNIT_REL;
  align.typed_data.real = FALSE;

  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  if (!strncasecmp (cssRule, "start", 5))
    {
      align.typed_data.value = AdjustLeft;
      cssRule += 5;
    }
  else if (!strncasecmp (cssRule, "middle", 6))
    {
      align.typed_data.value = Centered;
      cssRule += 6;
    }
  else if (!strncasecmp (cssRule, "end", 3))
    {
      align.typed_data.value = AdjustRight;
      cssRule += 3;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      align.typed_data.unit = VALUE_INHERIT;
      cssRule += 7;
    }
  else
    {
      cssRule = SkipValue ("Invalid text-anchor value", cssRule);
      return (cssRule);
    }

  /*
   * install the new presentation.
   */
  if (align.typed_data.value || align.typed_data.unit == VALUE_INHERIT)
    {
      if (DoDialog)
        DisplayStyleValue ("text-anchor", ptr, cssRule);
      else if (DoApply)
        TtaSetStylePresentation (PRAdjust, element, tsch, context, align);
    }
  cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid text-anchor value");
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSDirection: parse a CSS direction property
  ----------------------------------------------------------------------*/
static char *ParseCSSDirection (Element element, PSchema tsch,
                                PresentationContext context, char *cssRule,
                                CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   direction;
  char               *ptr;

  direction.typed_data.value = 0;
  direction.typed_data.unit = UNIT_REL;
  direction.typed_data.real = FALSE;

  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  if (!strncasecmp (cssRule, "ltr", 3))
    {
      direction.typed_data.value = LeftToRight;
      cssRule += 3;
    }
  else if (!strncasecmp (cssRule, "rtl", 3))
    {
      direction.typed_data.value = RightToLeft;
      cssRule += 3;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      direction.typed_data.unit = VALUE_INHERIT;
      cssRule += 7;
    }
  else
    {
      cssRule = SkipValue ("Invalid direction value", cssRule);
      return (cssRule);
    }

  /*
   * install the new presentation.
   */
  if (direction.typed_data.value || direction.typed_data.unit == VALUE_INHERIT)
    {
      if (DoDialog)
        DisplayStyleValue ("direction", ptr, cssRule);
      else if (DoApply)
        TtaSetStylePresentation (PRDirection, element, tsch, context, direction);
    }
  cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid direction value");
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSUnicodeBidi: parse a CSS unicode-bidi property
  ----------------------------------------------------------------------*/
static char *ParseCSSUnicodeBidi (Element element, PSchema tsch,
                                  PresentationContext context, char *cssRule,
                                  CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   bidi;
  char               *ptr;

  bidi.typed_data.value = 0;
  bidi.typed_data.unit = UNIT_REL;
  bidi.typed_data.real = FALSE;

  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  if (!strncasecmp (cssRule, "normal", 6))
    {
      bidi.typed_data.value = Normal;
      cssRule += 6;
    }
  else if (!strncasecmp (cssRule, "embed", 5))
    {
      bidi.typed_data.value = Embed;
      cssRule += 5;
    }
  else if (!strncasecmp (cssRule, "bidi-override", 13))
    {
      bidi.typed_data.value = Override;
      cssRule += 13;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      bidi.typed_data.unit = VALUE_INHERIT;
      cssRule += 7;
    }
  else
    {
      cssRule = SkipValue ("Invalid unicode-bidi value", cssRule);
      return (cssRule);
    }

  /*
   * install the new presentation.
   */
  if (bidi.typed_data.value || bidi.typed_data.unit == VALUE_INHERIT)
    {
      if (DoDialog)
        DisplayStyleValue ("unicode-bidi", ptr, cssRule);
      else if (DoApply)
        TtaSetStylePresentation (PRUnicodeBidi, element, tsch, context, bidi);
    }
  cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid unicode-bidi value");
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSTextIndent: parse a CSS text-indent
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSTextIndent (Element element, PSchema tsch,
                                 PresentationContext context, char *cssRule,
                                 CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   pval;
  char               *ptr;

  pval.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  cssRule = ParseCSSUnit (cssRule, &pval);
  if (pval.typed_data.value == 0 &&
      pval.typed_data.unit != UNIT_INVALID)
    pval.typed_data.unit = UNIT_PX;
  else if (pval.typed_data.unit == UNIT_INVALID ||
           pval.typed_data.unit == UNIT_BOX)
    {
      CSSParseError ("Invalid text-indent value", ptr, cssRule);
      return (cssRule);
    }
  /* install the attribute */
  if (DoDialog)
    DisplayStyleValue ("text-indent", ptr, cssRule);
  else if (DoApply)
    TtaSetStylePresentation (PRIndent, element, tsch, context, pval);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSTextTransform: parse a CSS text-transform    
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSTextTransform (Element element, PSchema tsch,
                                    PresentationContext context, char *cssRule,
                                    CSSInfoPtr css, ThotBool isHTML)
{
  char               *ptr;

  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  cssRule = SkipValue (NULL, cssRule);
  if (DoDialog)
    DisplayStyleValue ("text-transform", ptr, cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSVerticalAlign: parse a CSS vertical-align    
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSVerticalAlign (Element element, PSchema tsch,
                                    PresentationContext context, char *cssRule,
                                    CSSInfoPtr css, ThotBool isHTML)
{
  char                 *ptr;
  PresentationValue    pval;

  pval.typed_data.unit = UNIT_REL;
  pval.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  if (!strncasecmp (cssRule, "baseline", 8))
    {
      pval.typed_data.value = 0;
      cssRule += 8;
    }
  else if (!strncasecmp (cssRule, "sub", 3))
    {
      pval.typed_data.value = -3;
      cssRule += 3;
    }
  else if (!strncasecmp (cssRule, "super", 5))
    {
      pval.typed_data.value = 4;
      cssRule += 5;
    }
  else if (!strncasecmp (cssRule, "top", 3))
    {
      pval.typed_data.unit = UNIT_INVALID;      /* Not supported yet */
      pval.typed_data.value = 0;
      cssRule += 3;
    }
  else if (!strncasecmp (cssRule, "text-top", 8))
    {
      pval.typed_data.unit = UNIT_INVALID;      /* Not supported yet */
      pval.typed_data.value = 0;
      cssRule += 8;
    }
  else if (!strncasecmp (cssRule, "middle", 6))
    {
      pval.typed_data.unit = UNIT_INVALID;      /* Not supported yet */
      pval.typed_data.value = 0;
      cssRule += 6;
    }
  else if (!strncasecmp (cssRule, "bottom", 6))
    {
      pval.typed_data.unit = UNIT_INVALID;      /* Not supported yet */
      pval.typed_data.value = 0;
      cssRule += 6;
    }
  else if (!strncasecmp (cssRule, "text-bottom", 11))
    {
      pval.typed_data.unit = UNIT_INVALID;      /* Not supported yet */
      pval.typed_data.value = 0;
      cssRule += 11;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      pval.typed_data.unit = VALUE_INHERIT;
      pval.typed_data.value = 0;
      cssRule +=7;
    }
  else
    {
      /* parse <percentage> or <length> */
      cssRule = ParseCSSUnit (cssRule, &pval);
      if (pval.typed_data.unit == UNIT_INVALID)
        {
          pval.typed_data.value = 0;
          CSSParseError ("Invalid vertical-align value", ptr, cssRule);
          return (cssRule);
        }
      else if (pval.typed_data.value == 0)
        pval.typed_data.unit = UNIT_PX;
      else if (pval.typed_data.unit == UNIT_BOX)
        pval.typed_data.unit = UNIT_EM;
      else if (pval.typed_data.unit == UNIT_PERCENT)
        /* it's a percentage */
        {
          /* convert it into a relative size */
          pval.typed_data.unit = UNIT_REL;
          pval.typed_data.value /= 10;
        }
    }

  if (pval.typed_data.unit != UNIT_INVALID)
    {
      if (DoDialog)
        DisplayStyleValue ("vertical-align", ptr, cssRule);
      else if (DoApply)
        TtaSetStylePresentation (PRHorizRef, element, tsch, context, pval);
    }
  cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid vertical-align value");
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSWhiteSpace: parse a CSS white-space          
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSWhiteSpace (Element element, PSchema tsch,
                                 PresentationContext context, char *cssRule,
                                 CSSInfoPtr css, ThotBool isHTML)
{
  char *ptr;

  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  if (!strncasecmp (cssRule, "normal", 6))
    cssRule += 6;
  else if (!strncasecmp (cssRule, "pre-wrap", 8))
    cssRule += 8;
  else if (!strncasecmp (cssRule, "pre-line", 8))
    cssRule += 8;
  else if (!strncasecmp (cssRule, "pre", 3))
    cssRule += 3;
  else if (!strncasecmp (cssRule, "nowrap", 6))
    cssRule += 6;
  else if (!strncasecmp (cssRule, "inherit", 7))
    cssRule += 7;
  else
    cssRule = SkipValue ("Invalid white-space value", cssRule);

  if (ptr != cssRule && DoDialog)
    DisplayStyleValue ("white-space", ptr, cssRule);
  cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid white-space value");
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSWordSpacing: parse a CSS word-spacing        
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSWordSpacing (Element element, PSchema tsch,
                                  PresentationContext context, char *cssRule,
                                  CSSInfoPtr css, ThotBool isHTML)
{
  char *ptr;

  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  cssRule = SkipValue (NULL, cssRule);
  if (DoDialog)
    DisplayStyleValue ("word-spacing", ptr, cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSLineHeight: parse a CSS line-height property
  ----------------------------------------------------------------------*/
static char *ParseCSSLineHeight (Element element, PSchema tsch,
                                 PresentationContext context, char *cssRule,
                                 CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   pval;
  char               *ptr;

  pval.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  if (!strncasecmp (cssRule, "normal", 6))
    {
      pval.typed_data.unit = UNIT_REL;
      pval.typed_data.real = TRUE;
      pval.typed_data.value = 1100;
      cssRule += 6;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      pval.typed_data.unit = VALUE_INHERIT;
      cssRule += 7;
    }
  else
    cssRule = ParseCSSUnit (cssRule, &pval);

  if (pval.typed_data.unit == UNIT_INVALID)
    CSSParseError ("Invalid line-height value", ptr, cssRule);
  else if (DoDialog)
    DisplayStyleValue ("line-height", ptr, cssRule);
  else if (DoApply)
    {
      /* install the new presentation */
      if (pval.typed_data.unit == UNIT_BOX)
        pval.typed_data.unit = UNIT_EM;
      TtaSetStylePresentation (PRLineSpacing, element, tsch, context, pval);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSFontSizeAdjust: parse a CSS fontsizeAdjust attr string  
  we expect the input string describing the attribute to be     
  xx-small, x-small, small, medium, large, x-large, xx-large      
  or an absolute size, or an imcrement relative to the parent     
  ----------------------------------------------------------------------*/
static char *ParseCSSFontSizeAdjust (Element element, PSchema tsch,
                                     PresentationContext context, char *cssRule,
                                     CSSInfoPtr css, ThotBool isHTML)
{

  cssRule = SkipBlanksAndComments (cssRule);
  cssRule = SkipProperty (cssRule, FALSE);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseACSSFontSize: parse a CSS font size attr string  
  we expect the input string describing the attribute to be     
  xx-small, x-small, small, medium, large, x-large, xx-large      
  or an absolute size, or an imcrement relative to the parent.
  The parameter check is TRUE if the rule is just checked.
  ----------------------------------------------------------------------*/
static char *ParseACSSFontSize (Element element, PSchema tsch,
                                PresentationContext context, char *cssRule,
                                CSSInfoPtr css, ThotBool isHTML, ThotBool check)
{
  ElementType         elType;
  PresentationValue   pval;
  char               *ptr = NULL, *ptr1 = NULL, *ptr2 = NULL;
  char               *start_value;
  ThotBool	          real, error, linespace = FALSE;

  error = FALSE;
  pval.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  start_value = cssRule;
  /* look for a '/' within the current cssRule */
  ptr1 = strchr (cssRule, ';');
  ptr = strchr (cssRule, '/');
  if (ptr && (ptr1 == NULL || ptr < ptr1))
    {
      /* keep the line spacing rule */
      linespace = TRUE;
      ptr[0] = EOS;
    }
  else
    ptr = NULL;
  ptr1 = cssRule;
  /* relative size */
  if (!strncasecmp (cssRule, "larger", 6))
    {
      pval.typed_data.unit = UNIT_PERCENT;
      pval.typed_data.value = 130;
      cssRule += 6;
    }
  else if (!strncasecmp (cssRule, "smaller", 7))
    {
      pval.typed_data.unit = UNIT_PERCENT;
      pval.typed_data.value = 80;
      cssRule += 7;
    }
  /* absolute size */
  else if (!strncasecmp (cssRule, "xx-small", 8))
    {
      pval.typed_data.unit = UNIT_PT;
      pval.typed_data.value = 6;
      cssRule += 8;
    }
  else if (!strncasecmp (cssRule, "x-small", 7))
    {
      pval.typed_data.unit = UNIT_PT;
      pval.typed_data.value = 8;
      cssRule += 7;
    }
  else if (!strncasecmp (cssRule, "small", 5))
    {
      pval.typed_data.unit = UNIT_PT;
      pval.typed_data.value = 10;
      cssRule += 5;
    }
  else if (!strncasecmp (cssRule, "medium", 6))
    {
      pval.typed_data.unit = UNIT_PT;
      pval.typed_data.value = 12;
      cssRule += 6;
    }
  else if (!strncasecmp (cssRule, "large", 5))
    {
      pval.typed_data.unit = UNIT_PT;
      pval.typed_data.value = 13;
      cssRule += 5;
    }
  else if (!strncasecmp (cssRule, "x-large", 7))
    {
      pval.typed_data.unit = UNIT_PT;
      pval.typed_data.value = 14;
      cssRule += 7;
    }
  else if (!strncasecmp (cssRule, "xx-large", 8))
    {
      pval.typed_data.unit = UNIT_PT;
      pval.typed_data.value = 16;
      cssRule += 8;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      pval.typed_data.unit = VALUE_INHERIT;
      pval.typed_data.value = 0;
      cssRule += 7;
    }
  /* length or percentage */
  else if (!isdigit (*cssRule) && *cssRule != '.')
    {
      if (!check)
        cssRule = SkipValue ("Invalid font-size value", cssRule);
      error = TRUE;
    }
  else
    {       
      cssRule = ParseCSSUnit (cssRule, &pval);
      if (pval.typed_data.unit == UNIT_BOX)
        /* no unit specified */
        {
          elType = TtaGetElementType(element);
          if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "SVG"))
            /* we are working for an SVG element. No unit means pixels */
            pval.typed_data.unit = UNIT_PX;
        }
      if (pval.typed_data.unit == UNIT_INVALID ||
          (pval.typed_data.value != 0 && pval.typed_data.unit == UNIT_BOX) ||
          pval.typed_data.value < 0)
        /* not a valid value */
        {
          if (!check)
            {
              ptr2 = SkipWord (cssRule);
              CSSParseError ("Invalid font-size value", ptr1, ptr2);
            }
          error = TRUE;
        }
      else if (pval.typed_data.unit == UNIT_REL && pval.typed_data.value > 0)
        /* CSS relative sizes have to be higher than Thot ones */
        pval.typed_data.value += 1;
      else 
        {
          real = pval.typed_data.real;
          if (pval.typed_data.unit == UNIT_EM)
            {
              if (real)
                {
                  pval.typed_data.value /= 10;
                  pval.typed_data.real = FALSE;
                  real = FALSE;
                }
              else
                pval.typed_data.value *= 100;
              pval.typed_data.unit = UNIT_PERCENT;
            }
          else if (pval.typed_data.unit == UNIT_XHEIGHT)
            {
              /* a font size expressed in ex is converted into a percentage.
                 For example, "3ex" is converted into "180%", supposing
                 that 1ex is approximately 0.6 times the height of the
                 current font */
              if (real)
                {
                  pval.typed_data.value *= 6;
                  pval.typed_data.value /= 100;
                  pval.typed_data.real = FALSE;
                  real = FALSE;
                }
              else
                pval.typed_data.value *= 60;
              pval.typed_data.unit = UNIT_PERCENT;
            }
        }
    }

  /* install the presentation style */
  if (!check && !error)
    {
      if (DoDialog)
        DisplayStyleValue ("font-size", start_value, cssRule);
      else if (DoApply)
        TtaSetStylePresentation (PRSize, element, tsch, context, pval);
    }
  if (!check && ptr)
    cssRule = ParseCSSLineHeight (element, tsch, context, &ptr[1], css, isHTML);
  if (linespace)
    *ptr = '/';

  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSFontSize: parse a CSS font size attr string  
  we expect the input string describing the attribute to be     
  xx-small, x-small, small, medium, large, x-large, xx-large      
  or an absolute size, or an imcrement relative to the parent     
  ----------------------------------------------------------------------*/
static char *ParseCSSFontSize (Element element, PSchema tsch,
                               PresentationContext context, char *cssRule,
                               CSSInfoPtr css, ThotBool isHTML)
{
  char               *ptr = cssRule;
  cssRule = ParseACSSFontSize (element, tsch, context, cssRule, css, isHTML, FALSE);
  cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid font-size value");
  return cssRule;
}

/*----------------------------------------------------------------------
  ParseACSSFontFamily: parse a CSS font family string   
  we expect the input string describing the attribute to be     
  a common generic font style name                                
  ----------------------------------------------------------------------*/
static char *ParseACSSFontFamily (Element element, PSchema tsch,
                                  PresentationContext context, char *cssRule,
                                  CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   font;
  char                quoteChar, *p;

  font.typed_data.value = 0;
  font.typed_data.unit = UNIT_REL;
  font.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  if (*cssRule == '"' || *cssRule == '\'')
    {
      quoteChar = *cssRule;
      cssRule++;
    }
  else
    quoteChar = EOS;

  if (!strncasecmp (cssRule, "inherit", 7) && quoteChar == EOS)
    {
      font.typed_data.unit = VALUE_INHERIT;
      cssRule += 7;
    }
  else if (!strncasecmp (cssRule, "times", 5) &&
           (quoteChar == EOS || quoteChar == cssRule[5]))
    {
      font.typed_data.value = FontTimes;
      cssRule += 5;
    }
  else if (!strncasecmp (cssRule, "serif", 5) &&
           (quoteChar == EOS || quoteChar == cssRule[5]))
    {
      font.typed_data.value = FontTimes;
      cssRule += 5;
      if (quoteChar != EOS)
        cssRule++;
    }
  else if (!strncasecmp (cssRule, "helvetica", 9) &&
           (quoteChar == EOS || quoteChar == cssRule[9]))
    {
      font.typed_data.value = FontHelvetica;
      cssRule += 9;
      if (quoteChar != EOS)
        cssRule++;
    }
  else if (!strncasecmp (cssRule, "verdana", 7) &&
           (quoteChar == EOS || quoteChar == cssRule[7]))
    {
      font.typed_data.value = FontHelvetica;
      cssRule += 7;
      if (quoteChar != EOS)
        cssRule++;
    }
  else if (!strncasecmp (cssRule, "sans-serif", 10) &&
           (quoteChar == EOS || quoteChar == cssRule[10]))
    {
      font.typed_data.value = FontHelvetica;
      cssRule += 10;
      if (quoteChar != EOS)
        cssRule++;
    }
  else if (!strncasecmp (cssRule, "courier new", 11) &&
           (quoteChar == EOS || quoteChar == cssRule[11]))
    {
      font.typed_data.value = FontCourier;
      cssRule += 11;
      if (quoteChar != EOS)
        cssRule++;
    }
  else if (!strncasecmp (cssRule, "courier", 7) &&
           (quoteChar == EOS || quoteChar == cssRule[7]))
    {
      font.typed_data.value = FontCourier;
      cssRule += 7;
      if (quoteChar != EOS)
        cssRule++;
    }
  else if (!strncasecmp (cssRule, "monospace", 9) &&
           (quoteChar == EOS || quoteChar == cssRule[9]))
    {
      font.typed_data.value = FontCourier;
      cssRule += 9;
      if (quoteChar != EOS)
        cssRule++;
    }
  else
    /* unknown font name.  Skip it */
    {
      p = cssRule;
      if (quoteChar != EOS)
        cssRule = SkipQuotedString (cssRule, quoteChar);
      else
        /* unquoted font name. The name may contain spaces */
        {
          cssRule = SkipWord (cssRule);
          while (*cssRule == SPACE || *cssRule == BSPACE || *cssRule == EOL ||
              *cssRule == TAB || *cssRule == __CR__)
            {
              cssRule = SkipBlanksAndComments (cssRule);
              if (*cssRule != ','  && *cssRule != ';'  && *cssRule != '}' &&
                  *cssRule != EOS)
                cssRule = SkipWord (cssRule);
            }
        }
      while (p == cssRule &&
             *cssRule != ','  && *cssRule != ';'  && *cssRule != '}' && *cssRule != EOS)
        {
          cssRule++;
          p = cssRule;
          cssRule = SkipWord (cssRule);
        }
      cssRule = SkipBlanksAndComments (cssRule);
      if (*cssRule == ',')
        {
          /* recursive call to ParseCSSFontFamily */
          cssRule++;
          cssRule = ParseACSSFontFamily (element, tsch, context, cssRule, css, isHTML);
          return (cssRule);
        }
    }

  /* skip other values */
  cssRule = SkipBlanksAndComments (cssRule);
  while (*cssRule == ',')
    {
      cssRule++;
      cssRule = SkipValue (NULL, cssRule);
      cssRule = SkipBlanksAndComments (cssRule);
    }

  if (font.typed_data.value != 0 || font.typed_data.unit == VALUE_INHERIT)
    {
      if (!DoDialog && DoApply)
        /* install the new presentation */
        TtaSetStylePresentation (PRFont, element, tsch, context, font);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSFontFamily: parse a CSS font family string   
  we expect the input string describing the attribute to be     
  a common generic font style name                                
  ----------------------------------------------------------------------*/
static char *ParseCSSFontFamily (Element element, PSchema tsch,
                                 PresentationContext context, char *cssRule,
                                 CSSInfoPtr css, ThotBool isHTML)
{
  char               *start_value;

  cssRule = SkipBlanksAndComments (cssRule);
  start_value = cssRule;
  cssRule = ParseACSSFontFamily (element, tsch, context, cssRule, css, isHTML);
  /* skip extra values */
  while (cssRule && *cssRule != ';' && *cssRule != '}' && *cssRule != EOS)
    cssRule++;
  if (DoDialog)
    DisplayStyleValue ("font-family", start_value, cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseACSSFontWeight: parse a CSS font weight string   
  we expect the input string describing the attribute to be     
  normal, bold, bolder, lighter, 100, 200, 300, ... 900, inherit.
  ----------------------------------------------------------------------*/
static char *ParseACSSFontWeight (Element element, PSchema tsch,
                                  PresentationContext context, char *cssRule,
                                  CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   weight;
  char               *ptr;

  weight.typed_data.value = 0;
  weight.typed_data.unit = UNIT_REL;
  weight.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  if (isdigit (*cssRule) && *cssRule != '0' &&
      cssRule[1] == '0' && cssRule[2] == '0' &&
      (cssRule[3] == EOS || cssRule[3] == SPACE || cssRule[3] == '/' ||
       cssRule[3] == ';' || cssRule[3] == '}' || cssRule[3] == EOL || 
       cssRule[3] == TAB || cssRule[3] ==  __CR__))
    {
      if (!strncasecmp (cssRule, "100", 3))
        {
          weight.typed_data.value = -4;
          cssRule = SkipWord (cssRule);
        }
      else if (!strncasecmp (cssRule, "200", 3))
        {
          weight.typed_data.value = -3;
          cssRule = SkipWord (cssRule);
        }
      else if (!strncasecmp (cssRule, "300", 3))
        {
          weight.typed_data.value = -2;
          cssRule = SkipWord (cssRule);
        }
      else if (!strncasecmp (cssRule, "400", 3))
        {
          weight.typed_data.value = -1;
          cssRule = SkipWord (cssRule);
        }
      else if (!strncasecmp (cssRule, "500", 3))
        {
          weight.typed_data.value = 0;
          cssRule = SkipWord (cssRule);
        }
      else if (!strncasecmp (cssRule, "600", 3))
        {
          weight.typed_data.value = +1;
          cssRule = SkipWord (cssRule);
        }
      else if (!strncasecmp (cssRule, "700", 3))
        {
          weight.typed_data.value = +2;
          cssRule = SkipWord (cssRule);
        }
      else if (!strncasecmp (cssRule, "800", 3))
        {
          weight.typed_data.value = +3;
          cssRule = SkipWord (cssRule);
        }
      else if (!strncasecmp (cssRule, "900", 3))
        {
          weight.typed_data.value = +4;
          cssRule = SkipWord (cssRule);
        }
    }
  else if (!strncasecmp (cssRule, "normal", 6))
    {
      weight.typed_data.value = 0;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "bold", 4))
    {
      weight.typed_data.value = +3;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      weight.typed_data.unit = VALUE_INHERIT;
      cssRule += 7;
    }
  else if (!strncasecmp (cssRule, "bolder", 6))
    {
      weight.typed_data.value = +2;
      cssRule = SkipWord (cssRule);
    }

  else if (!strncasecmp (cssRule, "lighter", 7))
    {
      weight.typed_data.value = -1;
      cssRule = SkipWord (cssRule);
    }
  else
    return (cssRule);

  /*
   * Here we have to reduce since only two font weight values are supported
   * by the Thot presentation API.
   */
  if (weight.typed_data.unit != VALUE_INHERIT)
    {
      if (weight.typed_data.value > 0)
        weight.typed_data.value = WeightBold;
      else
        weight.typed_data.value = WeightNormal;
    }

  /* install the new presentation */
  if (cssRule != ptr && DoDialog)
    DisplayStyleValue ("font-weight", ptr, cssRule);
  else if (DoApply)
    TtaSetStylePresentation (PRWeight, element, tsch, context, weight);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSFontWeight: parse a CSS font weight string   
  we expect the input string describing the attribute to be     
  normal, bold, bolder, lighter, 100, 200, 300, ... 900, inherit.
  ----------------------------------------------------------------------*/
static char *ParseCSSFontWeight (Element element, PSchema tsch,
                                 PresentationContext context, char *cssRule,
                                 CSSInfoPtr css, ThotBool isHTML)
{
  char           *ptr;
  
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  cssRule = ParseACSSFontWeight (element, tsch, context, cssRule, css, isHTML);
  if (ptr == cssRule)
    cssRule = SkipValue ("Invalid font-weight value", cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseACSSFontVariant: parse a CSS font variant string     
  we expect the input string describing the attribute to be     
  normal or small-caps
  ----------------------------------------------------------------------*/
static char *ParseACSSFontVariant (Element element, PSchema tsch,
                                   PresentationContext context, char *cssRule,
                                   CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   style;
  char               *ptr;

  style.typed_data.value = 0;
  style.typed_data.unit = UNIT_REL;
  style.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  if (!strncasecmp (cssRule, "small-caps", 10))
    {
      style.typed_data.value = VariantSmallCaps;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "normal", 6))
    {
      style.typed_data.value = VariantNormal;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      style.typed_data.unit = VALUE_INHERIT;
      cssRule = SkipWord (cssRule);
    }
  else
    /* invalid font-variant */
    return (cssRule);

  if (style.typed_data.value != 0 || style.typed_data.unit == VALUE_INHERIT)
    {
      if (DoDialog)
        DisplayStyleValue ("font-variant", ptr, cssRule);
      else if (DoApply)
        TtaSetStylePresentation (PRVariant, element, tsch, context, style);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSFontVariant: parse a CSS font variant string     
  we expect the input string describing the attribute to be     
  normal or small-caps
  ----------------------------------------------------------------------*/
static char *ParseCSSFontVariant (Element element, PSchema tsch,
                                  PresentationContext context, char *cssRule,
                                  CSSInfoPtr css, ThotBool isHTML)
{
  char           *ptr;
  
  ptr = cssRule;
  cssRule = ParseACSSFontVariant (element, tsch, context, cssRule, css, isHTML);
  if (ptr == cssRule)
    cssRule = SkipValue ("Invalid font-variant value", cssRule);
  return (cssRule);
}


/*----------------------------------------------------------------------
  ParseACSSFontStyle: parse a CSS font style string     
  we expect the input string describing the attribute to be     
  normal, italic, oblique or inherit                         
  ----------------------------------------------------------------------*/
static char *ParseACSSFontStyle (Element element, PSchema tsch,
                                 PresentationContext context, char *cssRule,
                                 CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   style;
  PresentationValue   size;
  PresentationValue   previous_size;
  char               *ptr;

  style.typed_data.value = 0;
  style.typed_data.unit = UNIT_REL;
  style.typed_data.real = FALSE;
  size.typed_data.value = 0;
  size.typed_data.unit = UNIT_REL;
  size.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  if (!strncasecmp (cssRule, "italic", 6))
    {
      style.typed_data.value = StyleItalics;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "oblique", 7))
    {
      style.typed_data.value = StyleOblique;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "normal", 6))
    {
      style.typed_data.value = StyleRoman;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      style.typed_data.unit = VALUE_INHERIT;
      cssRule = SkipWord (cssRule);
    }
  else
    /* invalid font style */
    return (cssRule);

  /*
   * install the new presentation.
   */
  if (style.typed_data.value != 0 || style.typed_data.unit == VALUE_INHERIT)
    {
      if (DoDialog)
        DisplayStyleValue ("font-style", ptr, cssRule);
      else if (DoApply)
        TtaSetStylePresentation (PRStyle, element, tsch, context, style);
    }
  if (size.typed_data.value != 0)
    {
      if (DoDialog)
        DisplayStyleValue ("font-style", ptr, cssRule);
      else if (DoApply)
        {
          if (!TtaGetStylePresentation (PRSize, element, tsch, context, &previous_size))
            {
              /* !!!!!!!!!!!!!!!!!!!!!!!! Unit + relative !!!!!!!!!!!!!!!! */
              size.typed_data.value += previous_size.typed_data.value;
              TtaSetStylePresentation (PRSize, element, tsch, context, size);
            }
          else
            {
              size.typed_data.value = 10;
              TtaSetStylePresentation (PRSize, element, tsch, context, size);
            }
        }
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSFontStyle: parse a CSS font style string     
  we expect the input string describing the attribute to be     
  italic, oblique or normal                         
  ----------------------------------------------------------------------*/
static char *ParseCSSFontStyle (Element element, PSchema tsch,
                                PresentationContext context, char *cssRule,
                                CSSInfoPtr css, ThotBool isHTML)
{
  char           *ptr;
  
  ptr = cssRule;
  cssRule = ParseACSSFontStyle (element, tsch, context, cssRule, css, isHTML);
  if (ptr == cssRule)
    cssRule = SkipValue ("Invalid font-style value", cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSFont: parse a CSS font attribute string
  we expect the input string describing the attribute to be
  !!!!!!                                  
  ----------------------------------------------------------------------*/
static char *ParseCSSFont (Element element, PSchema tsch,
                           PresentationContext context, char *cssRule,
                           CSSInfoPtr css, ThotBool isHTML)
{
  char           *ptr, *p;
  char           *start_value;
  int             skippedNL;
  ThotBool        variant = FALSE, style = FALSE, weight = FALSE, found; 

  cssRule = SkipBlanksAndComments (cssRule);
  if (!strncasecmp (cssRule, "caption", 7))
    cssRule += 7;
  else if (!strncasecmp (cssRule, "icon", 4))
    cssRule += 4;
  else if (!strncasecmp (cssRule, "menu", 4))
    cssRule += 4;
  else if (!strncasecmp (cssRule, "message-box", 11))
    cssRule += 11;
  else if (!strncasecmp (cssRule, "small-caption", 13))
    cssRule += 13;
  else if (!strncasecmp (cssRule, "status-bar", 10))
    cssRule += 10;
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      ParseACSSFontStyle (element, tsch, context, cssRule, css, isHTML);
      ParseACSSFontVariant (element, tsch, context, cssRule, css, isHTML);
      ParseACSSFontWeight (element, tsch, context, cssRule, css, isHTML);
      ParseACSSFontSize (element, tsch, context, cssRule, css, isHTML, FALSE);
      cssRule = SkipBlanksAndComments (cssRule);
      start_value = cssRule;
      ParseACSSFontFamily (element, tsch, context, cssRule, css, isHTML);
      cssRule += 7;
      if (DoDialog)
        DisplayStyleValue ("font-family", start_value, cssRule);
    }
  else
    {
      ptr = NULL;
      p = cssRule;
      while (*cssRule != ';' && *cssRule != '}' && *cssRule != EOS && p == cssRule)
        {
          found = FALSE;
          /* style, variant, weight can appear in any order */
          ptr = cssRule;
          skippedNL = NewLineSkipped;
          cssRule = ParseACSSFontStyle (element, tsch, context, cssRule, css, isHTML);
          if (ptr != cssRule)
            {
              skippedNL = NewLineSkipped;
              found = TRUE;
              style = TRUE;
            }
          else
            NewLineSkipped = skippedNL;
          ptr = cssRule;
          cssRule = ParseACSSFontVariant (element, tsch, context, cssRule, css, isHTML);
          if (ptr != cssRule)
            {
              skippedNL = NewLineSkipped;
              found = TRUE;
              variant = TRUE;
            }
          else
            NewLineSkipped = skippedNL;
          ptr = cssRule;
          cssRule = ParseACSSFontWeight (element, tsch, context, cssRule, css, isHTML);
          if (ptr != cssRule)
            {
              skippedNL = NewLineSkipped;
              found = TRUE;
              weight = TRUE;
            }
          else
            NewLineSkipped = skippedNL;
          cssRule = SkipBlanksAndComments (cssRule);
          p = ParseACSSFontSize (element, tsch, context, cssRule, css, isHTML, TRUE);
          NewLineSkipped = skippedNL;
          if (!found)
            /* break the loop when the current value was not parsed */
            p = cssRule + 1;
        }
      ptr = cssRule;
      /* set default variant, style, weight */
      if (!variant)
        ParseACSSFontVariant (element, tsch, context, (char*)"normal", css, isHTML);
      if (!style)
        ParseACSSFontStyle (element, tsch, context, (char*)"normal", css, isHTML);
      if (!weight)
        ParseACSSFontWeight (element, tsch, context, (char*)"normal", css, isHTML);
      /* now parse the font size and the font family */
      if (*cssRule != ';' && *cssRule != '}' && *cssRule != EOS)
        cssRule = ParseACSSFontSize (element, tsch, context, cssRule, css, isHTML, FALSE);
      if (*cssRule != ';' && *cssRule != '}' && *cssRule != EOS)
        {
          cssRule = SkipBlanksAndComments (cssRule);
          start_value = cssRule;
          cssRule = ParseACSSFontFamily (element, tsch, context, cssRule, css, isHTML);
          if (DoDialog)
            DisplayStyleValue ("font-family", start_value, cssRule);
        }
      if (ptr == cssRule)
        cssRule = SkipValue ("Invalid font value", cssRule);
    }
  cssRule = SkipBlanksAndComments (cssRule);
  if (*cssRule != ';' && *cssRule != '}' && *cssRule != EOS)
    cssRule = SkipValue ("Invalid font value", cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSTextDecoration: parse a CSS text-decoration value.
  We expect the input string to be none, inherit or a combination of
  underline, overline, line-through, and blink.
  ----------------------------------------------------------------------*/
static char *ParseCSSTextDecoration (Element element, PSchema tsch,
                                     PresentationContext context, char *cssRule,
                                     CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   decor;
  char               *ptr;
  ThotBool            ok;

  decor.typed_data.value = 0;
  decor.typed_data.unit = UNIT_REL;
  decor.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  ok = TRUE;
  if (!strncasecmp (cssRule, "none", 4))
    {
      decor.typed_data.value = NoUnderline;
      cssRule += 4;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      decor.typed_data.unit = VALUE_INHERIT;
      cssRule += 7;
    }
  else
    {
      do
        {
          if (!strncasecmp (cssRule, "underline", 9))
            {
              decor.typed_data.value = Underline;
              cssRule += 9;
            }
          else if (!strncasecmp (cssRule, "overline", 8))
            {
              decor.typed_data.value = Overline;
              cssRule += 8;
            }
          else if (!strncasecmp (cssRule, "line-through", 12))
            {
              decor.typed_data.value = CrossOut;
              cssRule += 12;
            }
          else if (!strncasecmp (cssRule, "blink", 5))
            {
              /* the blink text-decoration attribute is not supported */
              cssRule += 5;
            }
          else
            ok = FALSE;
          if (ok)
            {
              cssRule = SkipBlanksAndComments (cssRule);
            }
        }
      while (ok && (*cssRule != ';' && *cssRule != '}' && *cssRule != EOS));
    }
  if (!ok)
    {
      cssRule = SkipValue ("Invalid text-decoration value", cssRule);
      return (cssRule);
    }

  /*
   * install the new presentation.
   */
  if (decor.typed_data.value || decor.typed_data.unit == VALUE_INHERIT)
    {
      if (DoDialog)
        DisplayStyleValue ("text-decoration", ptr, cssRule);
      else if (DoApply)
        TtaSetStylePresentation (PRUnderline, element, tsch, context, decor);
    }
  cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid text-decoration value");
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSHeight: parse a CSS height attribute
  ----------------------------------------------------------------------*/
static char *ParseCSSHeight (Element element, PSchema tsch,
                             PresentationContext context, char *cssRule,
                             CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   val;
  char               *ptr;

  val.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      val.typed_data.unit = VALUE_AUTO;
      val.typed_data.value = 0;
      cssRule += 4;
      cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid height value");
    }
  else
    cssRule = ParseCSSUnit (cssRule, &val);

  if (val.typed_data.unit == UNIT_INVALID ||
      (val.typed_data.value != 0 &&
       val.typed_data.unit == UNIT_BOX))
    CSSParseError ("height value", ptr, cssRule);
  else if (DoDialog)
    DisplayStyleValue ("height", ptr, cssRule);
  else if (DoApply)
    /* install the new presentation */
    TtaSetStylePresentation (PRHeight, element, tsch, context, val);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSMaxHeight: parse a CSS height attribute
  ----------------------------------------------------------------------*/
static char *ParseCSSMaxHeight (Element element, PSchema tsch,
                             PresentationContext context, char *cssRule,
                             CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   val;
  char               *ptr;

  val.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      val.typed_data.unit = VALUE_AUTO;
      val.typed_data.value = 0;
      cssRule += 4;
      cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid height value");
    }
  else
    cssRule = ParseCSSUnit (cssRule, &val);

  if (val.typed_data.unit == UNIT_INVALID ||
      (val.typed_data.value != 0 &&
       val.typed_data.unit == UNIT_BOX))
    CSSParseError ("height value", ptr, cssRule);
  else if (DoDialog)
    DisplayStyleValue ("max-height", ptr, cssRule);
  /*else if (DoApply)
    install the new presentation
    TtaSetStylePresentation (PRHeight, element, tsch, context, val)*/;
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSMinHeight: parse a CSS height attribute
  ----------------------------------------------------------------------*/
static char *ParseCSSMinHeight (Element element, PSchema tsch,
                             PresentationContext context, char *cssRule,
                             CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   val;
  char               *ptr;

  val.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      val.typed_data.unit = VALUE_AUTO;
      val.typed_data.value = 0;
      cssRule += 4;
      cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid height value");
    }
  else
    cssRule = ParseCSSUnit (cssRule, &val);

  if (val.typed_data.unit == UNIT_INVALID ||
      (val.typed_data.value != 0 &&
       val.typed_data.unit == UNIT_BOX))
    CSSParseError ("height value", ptr, cssRule);
  else if (DoDialog)
    DisplayStyleValue ("min-height", ptr, cssRule);
  /*else if (DoApply)*/
    /* install the new presentation */
    /*TtaSetStylePresentation (PRHeight, element, tsch, context, val)*/;
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSWidth: parse a CSS width attribute
  ----------------------------------------------------------------------*/
static char *ParseCSSWidth (Element element, PSchema tsch,
                            PresentationContext context,
                            char *cssRule, CSSInfoPtr css,
                            ThotBool isHTML)
{
  PresentationValue   val;
  char               *ptr;

  val.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      val.typed_data.unit = VALUE_AUTO;
      val.typed_data.value = 0;
      cssRule += 4;
      cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid width value");
    }
  else
    cssRule = ParseCSSUnit (cssRule, &val);
  if (val.typed_data.unit == UNIT_INVALID ||
      (val.typed_data.value != 0 &&
       val.typed_data.unit == UNIT_BOX))
    CSSParseError ("Invalid width value", ptr, cssRule);
  else if (DoDialog)
    DisplayStyleValue ("width", ptr, cssRule);
  else if (DoApply)
    /* install the new presentation */
    TtaSetStylePresentation (PRWidth, element, tsch, context, val);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSMaxWidth: parse a CSS width attribute
  ----------------------------------------------------------------------*/
static char *ParseCSSMaxWidth (Element element, PSchema tsch,
                               PresentationContext context,
                               char *cssRule, CSSInfoPtr css,
                               ThotBool isHTML)
{
  PresentationValue   val;
  char               *ptr;

  val.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      val.typed_data.unit = VALUE_AUTO;
      val.typed_data.value = 0;
      cssRule += 4;
      cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid width value");
    }
  else
    cssRule = ParseCSSUnit (cssRule, &val);
  if (val.typed_data.unit == UNIT_INVALID ||
      (val.typed_data.value != 0 &&
       val.typed_data.unit == UNIT_BOX))
      CSSParseError ("Invalid width value", ptr, cssRule);
  else if (DoDialog)
    DisplayStyleValue ("max-width", ptr, cssRule);
  /*else if (DoApply)*/
    /* install the new presentation */
    /*TtaSetStylePresentation (PRWidth, element, tsch, context, val)*/;
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSMinWidth: parse a CSS width attribute
  ----------------------------------------------------------------------*/
static char *ParseCSSMinWidth (Element element, PSchema tsch,
                               PresentationContext context,
                               char *cssRule, CSSInfoPtr css,
                               ThotBool isHTML)
{
  PresentationValue   val;
  char               *ptr;

  val.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      val.typed_data.unit = VALUE_AUTO;
      val.typed_data.value = 0;
      cssRule += 4;
      cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid width value");
    }
  else
    cssRule = ParseCSSUnit (cssRule, &val);
  if (val.typed_data.unit == UNIT_INVALID ||
      (val.typed_data.value != 0 &&
       val.typed_data.unit == UNIT_BOX))
    CSSParseError ("Invalid width value", ptr, cssRule);
  else if (DoDialog)
    DisplayStyleValue ("min-width", ptr, cssRule);
  /*else if (DoApply)*/
    /* install the new presentation */
    /*TtaSetStylePresentation (PRWidth, element, tsch, context, val)*/;
  return (cssRule);
}

/*----------------------------------------------------------------------
  GetEmMarginValue returns the em value 
  ----------------------------------------------------------------------*/
int GetEmValue (char *data, Element el, Document doc)
{
  PresentationValue   val;
  char               *ptr;
  int                 value;

  val.typed_data.real = FALSE;
  ptr = SkipBlanksAndComments (data);
  if (!strncasecmp (data, "auto", 4))
    value = TtaGetPixelValue (0, VALUE_AUTO, el, doc);
  else
    {
      ptr = ParseCSSUnit (data, &val);
      value = TtaGetPixelValue (val.typed_data.value, val.typed_data.unit,
                             el, doc);
    }
  return TtaGetLogicalValue (value, UNIT_EM, el, doc);
}

/*----------------------------------------------------------------------
  ParseACSSMarginTop: parse a CSS margin-top attribute
  ----------------------------------------------------------------------*/
static char *ParseACSSMarginTop (Element element, PSchema tsch,
                                 PresentationContext context,
                                 char *cssRule, CSSInfoPtr css,
                                 ThotBool isHTML)
{
  PresentationValue   margin;
  char               *ptr;
  
  margin.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      margin.typed_data.unit = VALUE_AUTO;
      margin.typed_data.value = 0;
      cssRule += 4;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      margin.typed_data.unit = VALUE_AUTO;
      margin.typed_data.value = 0;
      cssRule += 7;
    }
  else
    cssRule = ParseCSSUnit (cssRule, &margin);

  if (margin.typed_data.unit == UNIT_INVALID ||
      (margin.typed_data.value != 0 &&
       margin.typed_data.unit == UNIT_BOX))
    CSSParseError ("Invalid margin-top value", ptr, cssRule);
  else if (DoDialog)
    {
      if (All_sides)
        DisplayStyleValue ("margin", ptr, cssRule);
      else
        DisplayStyleValue ("margin-top", ptr, cssRule);
    }
  else if (DoApply)
    TtaSetStylePresentation (PRMarginTop, element, tsch, context, margin);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSMarginTop: parse a CSS margin-top attribute
  ----------------------------------------------------------------------*/
static char *ParseCSSMarginTop (Element element, PSchema tsch,
                                PresentationContext context,
                                char *cssRule, CSSInfoPtr css,
                                ThotBool isHTML)
{
  char *ptr = cssRule;

  cssRule = ParseACSSMarginTop (element, tsch, context, ptr, css, isHTML);
  cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid margin-top value");
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseACSSMarginBottom: parse a CSS margin-bottom attribute
  ----------------------------------------------------------------------*/
static char *ParseACSSMarginBottom (Element element, PSchema tsch,
                                    PresentationContext context,
                                    char *cssRule, CSSInfoPtr css,
                                    ThotBool isHTML)
{
  PresentationValue   margin;
  char               *ptr;
  
  margin.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      margin.typed_data.unit = VALUE_AUTO;
      margin.typed_data.value = 0;
      cssRule += 4;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      margin.typed_data.unit = VALUE_AUTO;
      margin.typed_data.value = 0;
      cssRule += 7;
    }
  else
    cssRule = ParseCSSUnit (cssRule, &margin);

  if (margin.typed_data.unit == UNIT_INVALID ||
      (margin.typed_data.value != 0 &&
       margin.typed_data.unit == UNIT_BOX))
    CSSParseError ("Invalid margin-bottom value", ptr, cssRule);
  else if (DoDialog)
    DisplayStyleValue ("margin-bottom", ptr, cssRule);
  else if (DoApply)
    TtaSetStylePresentation (PRMarginBottom, element, tsch, context, margin);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSMarginBottom: parse a CSS margin-bottom attribute
  ----------------------------------------------------------------------*/
static char *ParseCSSMarginBottom (Element element, PSchema tsch,
                                   PresentationContext context,
                                   char *cssRule, CSSInfoPtr css,
                                   ThotBool isHTML)
{
  char *ptr = cssRule;

  cssRule = ParseACSSMarginBottom (element, tsch, context, ptr, css, isHTML);
  cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid margin-bottom value");
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseACSSMarginLeft: parse a CSS margin-left attribute string
  ----------------------------------------------------------------------*/
static char *ParseACSSMarginLeft (Element element, PSchema tsch,
                                  PresentationContext context,
                                  char *cssRule, CSSInfoPtr css,
                                  ThotBool isHTML)
{
  PresentationValue   margin;
  char               *ptr;
  
  margin.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      margin.typed_data.unit = VALUE_AUTO;
      margin.typed_data.value = 0;
      cssRule += 4;
    }
  else
    cssRule = ParseCSSUnit (cssRule, &margin);

  if (margin.typed_data.unit == UNIT_INVALID ||
      (margin.typed_data.value != 0 &&
       margin.typed_data.unit == UNIT_BOX))
    CSSParseError ("Invalid margin-left value", ptr, cssRule);
  else if (DoDialog)
    DisplayStyleValue ("margin-left", ptr, cssRule);
  else if (DoApply && margin.typed_data.unit != UNIT_INVALID && DoApply)
    TtaSetStylePresentation (PRMarginLeft, element, tsch, context, margin);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSMarginBottom: parse a CSS margin-bottom attribute
  ----------------------------------------------------------------------*/
static char *ParseCSSMarginLeft (Element element, PSchema tsch,
                                 PresentationContext context,
                                 char *cssRule, CSSInfoPtr css,
                                 ThotBool isHTML)
{
  char *ptr = cssRule;

  cssRule = ParseACSSMarginLeft (element, tsch, context, ptr, css, isHTML);
  cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid margin-left value");
  return (cssRule);
}


/*----------------------------------------------------------------------
  ParseACSSMarginRight: parse a CSS margin-right attribute string
  ----------------------------------------------------------------------*/
static char *ParseACSSMarginRight (Element element, PSchema tsch,
                                   PresentationContext context,
                                   char *cssRule, CSSInfoPtr css,
                                   ThotBool isHTML)
{
  PresentationValue   margin;
  char               *ptr;
  
  margin.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      margin.typed_data.unit = VALUE_AUTO;
      margin.typed_data.value = 0;
      cssRule += 4;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      margin.typed_data.unit = VALUE_AUTO;
      margin.typed_data.value = 0;
      cssRule += 7;
    }
  else
    cssRule = ParseCSSUnit (cssRule, &margin);

  if (margin.typed_data.unit == UNIT_INVALID ||
      (margin.typed_data.value != 0 &&
       margin.typed_data.unit == UNIT_BOX))
    CSSParseError ("Invalid margin-right value", ptr, cssRule);
  else if (DoDialog)
    DisplayStyleValue ("margin-right", ptr, cssRule);
  else if (DoApply)
    TtaSetStylePresentation (PRMarginRight, element, tsch, context, margin);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSMarginRight: parse a CSS margin-right attribute string
  ----------------------------------------------------------------------*/
static char *ParseCSSMarginRight (Element element, PSchema tsch,
                                  PresentationContext context,
                                  char *cssRule, CSSInfoPtr css,
                                  ThotBool isHTML)
{
  char *ptr = cssRule;

  cssRule = ParseACSSMarginRight (element, tsch, context, ptr, css, isHTML);
  cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid margin-right value");
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSMargin: parse a CSS margin attribute string
  ----------------------------------------------------------------------*/
static char *ParseCSSMargin (Element element, PSchema tsch,
                             PresentationContext context,
                             char *cssRule, CSSInfoPtr css,
                             ThotBool isHTML)
{
  char *ptrT, *ptrR, *ptrB, *ptrL;
  int   skippedNL, n;

  ptrT = SkipBlanksAndComments (cssRule);
  if (DoDialog)
    n = NumberOfValues (ptrT);
  if (DoDialog && n < 2)
    {
      // check if the margin dialog must be updated
      All_sides = TRUE;
      ptrR = ParseACSSMarginTop (element, tsch, context, ptrT, css, isHTML);
      All_sides = FALSE;
    }
  else
    {
      /* First parse Margin-Top */
      ptrR = ParseACSSMarginTop (element, tsch, context, ptrT, css, isHTML);
      ptrR = SkipBlanksAndComments (ptrR);
      if (*ptrR == ';' || *ptrR == '}' || *ptrR == EOS || *ptrR == ',')
        {
          skippedNL = NewLineSkipped;
          cssRule = ptrR;
          /* apply the Margin-Top to all */
          ptrR = ParseACSSMarginRight (element, tsch, context, ptrT, css, isHTML);
          NewLineSkipped = skippedNL;
          ptrR = ParseACSSMarginBottom (element, tsch, context, ptrT, css, isHTML);
          NewLineSkipped = skippedNL;
          ptrR = ParseACSSMarginLeft (element, tsch, context, ptrT, css, isHTML);
        }
      else
        {
          /* parse Margin-Right */
          ptrB = ParseACSSMarginRight (element, tsch, context, ptrR, css, isHTML);
          ptrB = SkipBlanksAndComments (ptrB);
          if (*ptrB == ';' || *ptrB == '}' || *ptrB == EOS || *ptrB == ',')
            {
              skippedNL = NewLineSkipped;
              cssRule = ptrB;
              /* apply the Margin-Top to Margin-Bottom */
              ptrB = ParseACSSMarginBottom (element, tsch, context, ptrT, css, isHTML);
              NewLineSkipped = skippedNL;
              /* apply the Margin-Right to Margin-Left */
              ptrB = ParseACSSMarginLeft (element, tsch, context, ptrR, css, isHTML);
            }
          else
            {
              /* parse Margin-Bottom */
              ptrL = ParseACSSMarginBottom (element, tsch, context, ptrB, css, isHTML);
              ptrL = SkipBlanksAndComments (ptrL);
              if (*ptrL == ';' || *ptrL == '}' || *ptrL == EOS || *ptrL == ',')
                {
                  cssRule = ptrL;
                  /* apply the Margin-Right to Margin-Left */
                  ptrL = ParseACSSMarginLeft (element, tsch, context, ptrR, css, isHTML);
                }
              else
                /* parse Margin-Left */
                cssRule = ParseACSSMarginLeft (element, tsch, context, ptrL, css, isHTML);
              cssRule = SkipBlanksAndComments (cssRule);
            }
        }
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSOpacity: parse a CSS opacity property
  ----------------------------------------------------------------------*/
static char *ParseCSSOpacity (Element element, PSchema tsch,
                              PresentationContext context, char *cssRule,
                              CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue     opacity;

  opacity.typed_data.unit = UNIT_INVALID;
  opacity.typed_data.real = FALSE;
  if (!strncasecmp (cssRule, "inherit", 7))
    {
      opacity.typed_data.unit = VALUE_INHERIT;
      cssRule += 7;
    }
  else
    cssRule = ParseClampedUnit (cssRule, &opacity);
  if (DoApply)
    /* install the new presentation. */
    TtaSetStylePresentation (PROpacity, element, tsch, context, opacity);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSPaddingTop: parse a CSS PaddingTop attribute string
  ----------------------------------------------------------------------*/
static char *ParseCSSPaddingTop (Element element, PSchema tsch,
                                 PresentationContext context,
                                 char *cssRule, CSSInfoPtr css,
                                 ThotBool isHTML)
{
  PresentationValue   padding;
  char               *ptr;
 
  padding.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  cssRule = ParseCSSUnit (cssRule, &padding);

  if (padding.typed_data.unit == UNIT_INVALID ||
      (padding.typed_data.value != 0 &&
       padding.typed_data.unit == UNIT_BOX))
    CSSParseError ("Invalid padding-top value", ptr, cssRule);
  else if (DoDialog)
    {
      if (All_sides)
        DisplayStyleValue ("padding", ptr, cssRule);
      else
        DisplayStyleValue ("padding-top", ptr, cssRule);
    }
  else if (DoApply)
    TtaSetStylePresentation (PRPaddingTop, element, tsch, context, padding);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSPaddingBottom: parse a CSS PaddingBottom attribute string
  ----------------------------------------------------------------------*/
static char *ParseCSSPaddingBottom (Element element, PSchema tsch,
                                    PresentationContext context,
                                    char *cssRule, CSSInfoPtr css,
                                    ThotBool isHTML)
{
  PresentationValue   padding;
  char               *ptr;
  
  padding.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  cssRule = ParseCSSUnit (cssRule, &padding);
  if (padding.typed_data.value == 0 && padding.typed_data.unit != UNIT_INVALID)
    padding.typed_data.unit = UNIT_EM;

  if (padding.typed_data.unit == UNIT_INVALID ||
      (padding.typed_data.value != 0 &&
       padding.typed_data.unit == UNIT_BOX))
    CSSParseError ("Invalid padding-bottom value", ptr, cssRule);
  else if (DoDialog)
    DisplayStyleValue ("padding-bottom", ptr, cssRule);
  else if (DoApply)
    TtaSetStylePresentation (PRPaddingBottom, element, tsch, context, padding);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSPaddingLeft: parse a CSS PaddingLeft attribute string.
  ----------------------------------------------------------------------*/
static char *ParseCSSPaddingLeft (Element element, PSchema tsch,
                                  PresentationContext context,
                                  char *cssRule, CSSInfoPtr css,
                                  ThotBool isHTML)
{
  PresentationValue   padding;
  char               *ptr;
  
  padding.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  cssRule = ParseCSSUnit (cssRule, &padding);
  if (padding.typed_data.value == 0 && padding.typed_data.unit != UNIT_INVALID)
    padding.typed_data.unit = UNIT_EM;

  if (padding.typed_data.unit == UNIT_INVALID ||
      (padding.typed_data.value != 0 &&
       padding.typed_data.unit == UNIT_BOX))
    {
      CSSParseError ("Invalid padding-left value", ptr, cssRule);
      padding.typed_data.value = 0;
    }
  else if (DoDialog)
    DisplayStyleValue ("padding-left", ptr, cssRule);
  else if (DoApply)
    TtaSetStylePresentation (PRPaddingLeft, element, tsch, context, padding);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSPaddingRight: parse a CSS PaddingRight attribute string.
  ----------------------------------------------------------------------*/
static char *ParseCSSPaddingRight (Element element, PSchema tsch,
                                   PresentationContext context,
                                   char *cssRule, CSSInfoPtr css,
                                   ThotBool isHTML)
{
  PresentationValue   padding;
  char               *ptr;
  
  padding.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  cssRule = ParseCSSUnit (cssRule, &padding);
  if (padding.typed_data.value == 0 && padding.typed_data.unit != UNIT_INVALID)
    padding.typed_data.unit = UNIT_EM;

  if (padding.typed_data.unit == UNIT_INVALID ||
      (padding.typed_data.value != 0 &&
       padding.typed_data.unit == UNIT_BOX))
    CSSParseError ("Invalid padding-right value", ptr, cssRule);
  else if (DoDialog)
    DisplayStyleValue ("padding-right", ptr, cssRule);
  else if (DoApply)
    TtaSetStylePresentation (PRPaddingRight, element, tsch, context, padding);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSPadding: parse a CSS padding attribute string. 
  ----------------------------------------------------------------------*/
static char *ParseCSSPadding (Element element, PSchema tsch,
                              PresentationContext context,
                              char *cssRule, CSSInfoPtr css,
                              ThotBool isHTML)
{
  char *ptrT, *ptrR, *ptrB, *ptrL;
  int   skippedNL, n;

  ptrT = SkipBlanksAndComments (cssRule);
  if (DoDialog)
    n = NumberOfValues (ptrT);
  if (DoDialog && n < 2)
    {
      // check if the padding dialog must be updated
      All_sides = TRUE;
      ptrR = ParseCSSPaddingTop (element, tsch, context, ptrT, css, isHTML);
      All_sides = FALSE;
    }
  else
    {
      /* First parse Padding-Top */
      ptrR = ParseCSSPaddingTop (element, tsch, context, ptrT, css, isHTML);
      ptrR = SkipBlanksAndComments (ptrR);
      if (*ptrR == ';' || *ptrR == EOS || *ptrR == ',')
        {
          skippedNL = NewLineSkipped;
          cssRule = ptrR;
          /* apply the Padding-Top to all */
          ptrR = ParseCSSPaddingRight (element, tsch, context, ptrT, css, isHTML);
          NewLineSkipped = skippedNL;
          ptrR = ParseCSSPaddingBottom (element, tsch, context, ptrT, css, isHTML);
          NewLineSkipped = skippedNL;
          ptrR = ParseCSSPaddingLeft (element, tsch, context, ptrT, css, isHTML);
        }
      else
        {
          /* parse Padding-Right */
          ptrB = ParseCSSPaddingRight (element, tsch, context, ptrR, css, isHTML);
          ptrB = SkipBlanksAndComments (ptrB);
          if (*ptrB == ';' || *ptrB == EOS || *ptrB == ',')
            {
              skippedNL = NewLineSkipped;
              cssRule = ptrB;
              /* apply the Padding-Top to Padding-Bottom */
              ptrB = ParseCSSPaddingBottom (element, tsch, context, ptrT, css, isHTML);
              NewLineSkipped = skippedNL;
              /* apply the Padding-Right to Padding-Left */
              ptrB = ParseCSSPaddingLeft (element, tsch, context, ptrR, css, isHTML);
            }
          else
            {
              /* parse Padding-Bottom */
              ptrL = ParseCSSPaddingBottom (element, tsch, context, ptrB, css, isHTML);
              ptrL = SkipBlanksAndComments (ptrL);
              if (*ptrL == ';' || *ptrL == EOS || *ptrL == ',')
                {
                  cssRule = ptrL;
                  /* apply the Padding-Right to Padding-Left */
                  ptrL = ParseCSSPaddingLeft (element, tsch, context, ptrR, css, isHTML);
                }
              else
                /* parse Padding-Left */
                cssRule = ParseCSSPaddingLeft (element, tsch, context, ptrL, css, isHTML);
              cssRule = SkipBlanksAndComments (cssRule);
            }
        }
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSForeground: parse a CSS foreground attribute 
  ----------------------------------------------------------------------*/
static char *ParseCSSForeground (Element element, PSchema tsch,
                                 PresentationContext context,
                                 char *cssRule,
                                 CSSInfoPtr css, ThotBool isHTML)
{
  ElementType         elType;
  PresentationValue   best;
  char               *p;

  best.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  p = cssRule;
  cssRule = ParseCSSColor (cssRule, &best);
  if (best.typed_data.unit != UNIT_INVALID)
    {
      if (*cssRule != EOS && *cssRule !=';')
        {
          cssRule = SkipProperty (cssRule, FALSE);
          CSSParseError ("Invalid color value", p, cssRule);
        }
      else if (DoDialog)
        DisplayStyleValue ("color", p, cssRule);
      else if (DoApply)
        /* install the new presentation */
	{
          elType = TtaGetElementType(element);
          if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "SVG"))
            /* we are working for an SVG element */
	    TtaSetStylePresentation (PRColor, element, tsch, context, best);
	  else
	    TtaSetStylePresentation (PRForeground, element, tsch, context,best);
	}
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBackgroundColor: parse a CSS background color attribute 
  ----------------------------------------------------------------------*/
static char *ParseCSSBackgroundColor (Element element, PSchema tsch,
                                      PresentationContext context,
                                      char *cssRule,
                                      CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue     best;
  char                 *ptr;

  best.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  best.typed_data.unit = UNIT_INVALID;
  best.typed_data.real = FALSE;
  if (!strncasecmp (cssRule, "transparent", 11))
    {
      best.typed_data.value = PATTERN_NONE;
      best.typed_data.unit = UNIT_REL;
      cssRule = SkipWord (cssRule);
      if (DoApply)
        TtaSetStylePresentation (PRFillPattern, element, tsch, context, best);
    }
  else
    {
      cssRule = ParseCSSColor (cssRule, &best);
      if (best.typed_data.unit != UNIT_INVALID)
        {
          if (DoDialog)
            DisplayStyleValue ("background-color", ptr, cssRule);
          else if (DoApply)
            {
              /* install the new presentation. */
              TtaSetStylePresentation (PRBackground, element, tsch, context, best);
              /* thot specifics: need to set fill pattern for background color */
              best.typed_data.value = PATTERN_BACKGROUND;
              best.typed_data.unit = UNIT_REL;
              TtaSetStylePresentation (PRFillPattern, element, tsch, context, best);
              best.typed_data.value = 1;
              best.typed_data.unit = UNIT_REL;
              TtaSetStylePresentation (PRShowBox, element, tsch, context, best);
            }
        }
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseSVGStroke: parse a SVG stroke property
  ----------------------------------------------------------------------*/
static char *ParseSVGStroke (Element element, PSchema tsch,
                             PresentationContext context, char *cssRule,
                             CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue     best, color;
  char                  *url;

  best.typed_data.unit = UNIT_INVALID;
  best.typed_data.real = FALSE;
  if (!strncasecmp (cssRule, "none", 4))
    {
      best.typed_data.value = -2;  /* -2 means transparent */
      best.typed_data.unit = UNIT_REL;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      best.typed_data.unit = VALUE_INHERIT;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "currentColor", 12))
    {
      best.typed_data.unit = VALUE_CURRENT;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "url", 3))
    {  
      cssRule += 3;
      cssRule = ParseCSSUrl (cssRule, &url);
      /* **** do something with the url ***** */;
      TtaFreeMemory (url);
      /* a color property may follow the url */
      cssRule = SkipBlanksAndComments (cssRule);
      if (*cssRule != ';' && *cssRule != '}' && *cssRule != EOS &&
	  *cssRule != ',')
	/* we expect a color property here */
	{
	  cssRule = ParseCSSColor (cssRule, &color);
	  /* ***** do something with the color we have just parsed */
	}
    }
  else
    cssRule = ParseCSSColor (cssRule, &best);

  if (best.typed_data.unit != UNIT_INVALID && DoApply)
    /* install the new presentation */
    TtaSetStylePresentation (PRForeground, element, tsch, context, best);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseSVGFill: parse a SVG fill property
  ----------------------------------------------------------------------*/
static char *ParseSVGFill (Element element, PSchema tsch,
                           PresentationContext context, char *cssRule,
                           CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue     fill, color;
  char                  *url;
  int                   pattern;

  url = NULL;
  fill.typed_data.unit = UNIT_INVALID;
  fill.typed_data.real = FALSE;
  pattern = PATTERN_BACKGROUND;
  if (!strncasecmp (cssRule, "none", 4))
    {
      pattern = PATTERN_NONE;
      fill.typed_data.value = -2;
      fill.typed_data.unit = UNIT_REL;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "currentColor", 12))
    {
      fill.typed_data.unit = VALUE_CURRENT;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "url", 3))
    {  
      cssRule += 3;
      cssRule = ParseCSSUrl (cssRule, &url);
      fill.typed_data.unit = VALUE_URL;
      fill.pointer = url;
      /* a color property may follow the url */
      cssRule = SkipBlanksAndComments (cssRule);
      if (*cssRule != ';' && *cssRule != '}' && *cssRule != EOS &&
	  *cssRule != ',')
	/* we expect a color property here */
	{
	  cssRule = ParseCSSColor (cssRule, &color);
	  /* @@@@@@ do something with the color we have just parsed */
	}
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      fill.typed_data.unit = VALUE_INHERIT;
      cssRule = SkipWord (cssRule);
    }
  else
    cssRule = ParseCSSColor (cssRule, &fill);

  if (fill.typed_data.unit != UNIT_INVALID && DoApply)
    {
      /* install the new presentation. */
      TtaSetStylePresentation (PRBackground, element, tsch, context, fill);
      if (url)
	TtaFreeMemory (url);
      /* thot specifics: need to set fill pattern for background color */
      fill.typed_data.value = pattern;
      fill.typed_data.unit = UNIT_REL;
      TtaSetStylePresentation (PRFillPattern, element, tsch, context, fill);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseSVGStopColor: parse a SVG stop-color property
  ----------------------------------------------------------------------*/
static char *ParseSVGStopColor (Element element, PSchema tsch,
				PresentationContext context, char *cssRule,
				CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue     best;

  best.typed_data.unit = UNIT_INVALID;
  best.typed_data.real = FALSE;
  if (!strncasecmp (cssRule, "inherit", 7))
    {
      best.typed_data.unit = VALUE_INHERIT;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "currentColor", 12))
    {
      best.typed_data.unit = VALUE_CURRENT;
      cssRule = SkipWord (cssRule);
    }
  else
    cssRule = ParseCSSColor (cssRule, &best);

  if (best.typed_data.unit != UNIT_INVALID && DoApply)
    /* install the new presentation */
    TtaSetStylePresentation (PRStopColor, element, tsch, context, best);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSColor: parse a CSS color attribute string    
  we expect the input string describing the attribute to be     
  either a color name, a 3 tuple or an hexadecimal encoding.    
  The color used will be approximed from the current color      
  table                                                         
  ----------------------------------------------------------------------*/
static char *ParseSVGMarkerValue (char *cssRule, PresentationValue *val, char **url)
{
  *url = NULL;
  val->typed_data.unit = UNIT_INVALID;
  val->typed_data.real = FALSE;
  if (!strncasecmp (cssRule, "none", 4))
    {
      val->typed_data.unit = UNIT_REL;
      val->typed_data.value = 0;
      cssRule += 4;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      val->typed_data.unit = VALUE_INHERIT;
      cssRule += 7;
    }
  else if (!strncasecmp (cssRule, "url", 3))
    {  
      cssRule += 3;
      cssRule = ParseCSSUrl (cssRule, url);
      val->typed_data.unit = VALUE_URL;
      val->pointer = *url;
    }
  return cssRule;
}

/*----------------------------------------------------------------------
  ParseSVGMarker: parse a SVG marker property
  ----------------------------------------------------------------------*/
static char *ParseSVGMarker (Element element, PSchema tsch,
			     PresentationContext context, char *cssRule,
			     CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue     marker;
  char                  *url;

  url = NULL;
  cssRule = ParseSVGMarkerValue (cssRule, &marker, &url);
  if (marker.typed_data.unit != UNIT_INVALID && DoApply)
    /* install the new presentation. */
    TtaSetStylePresentation (PRMarker, element, tsch, context, marker);
  if (url)
    TtaFreeMemory (url);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseSVGMarkerEnd: parse a SVG marker-end property
  ----------------------------------------------------------------------*/
static char *ParseSVGMarkerEnd (Element element, PSchema tsch,
				PresentationContext context, char *cssRule,
				CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue     marker;
  char                  *url;

  url = NULL;
  cssRule = ParseSVGMarkerValue (cssRule, &marker, &url);
  if (marker.typed_data.unit != UNIT_INVALID && DoApply)
    /* install the new presentation. */
    TtaSetStylePresentation (PRMarkerEnd, element, tsch, context, marker);
  if (url)
    TtaFreeMemory (url);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseSVGMarkerMid: parse a SVG marker-mid property
  ----------------------------------------------------------------------*/
static char *ParseSVGMarkerMid (Element element, PSchema tsch,
				PresentationContext context, char *cssRule,
				CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue     marker;
  char                  *url;

  url = NULL;
  cssRule = ParseSVGMarkerValue (cssRule, &marker, &url);
  if (marker.typed_data.unit != UNIT_INVALID && DoApply)
    /* install the new presentation. */
    TtaSetStylePresentation (PRMarkerMid, element, tsch, context, marker);
  if (url)
    TtaFreeMemory (url);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseSVGMarkerStart: parse a SVG marker-start property
  ----------------------------------------------------------------------*/
static char *ParseSVGMarkerStart (Element element, PSchema tsch,
				  PresentationContext context, char *cssRule,
				  CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue     marker;
  char                  *url;

  url = NULL;
  cssRule = ParseSVGMarkerValue (cssRule, &marker, &url);
  if (marker.typed_data.unit != UNIT_INVALID && DoApply)
    /* install the new presentation. */
    TtaSetStylePresentation (PRMarkerStart, element, tsch, context, marker);
  if (url)
    TtaFreeMemory (url);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseSVGStopOpacity: parse a SVG opacity property
  ----------------------------------------------------------------------*/
static char *ParseSVGStopOpacity (Element element, PSchema tsch,
				  PresentationContext context, char *cssRule,
				  CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue     opacity;

  opacity.typed_data.unit = UNIT_INVALID;
  opacity.typed_data.real = FALSE;
  if (!strncasecmp (cssRule, "inherit", 7))
    {
      opacity.typed_data.unit = VALUE_INHERIT;
      cssRule += 7;
    }
  else
    cssRule = ParseClampedUnit (cssRule, &opacity);
  if (opacity.typed_data.unit != UNIT_INVALID && DoApply)
    /* install the new presentation. */
    TtaSetStylePresentation (PRStopOpacity, element, tsch, context, opacity);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseSVGOpacity: parse a SVG opacity property
  ----------------------------------------------------------------------*/
static char *ParseSVGOpacity (Element element, PSchema tsch,
                              PresentationContext context, char *cssRule,
                              CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue     opacity;

  opacity.typed_data.unit = UNIT_INVALID;
  opacity.typed_data.real = FALSE;
  if (!strncasecmp (cssRule, "inherit", 7))
    {
      opacity.typed_data.unit = VALUE_INHERIT;
      cssRule += 7;
    }
  else
    cssRule = ParseClampedUnit (cssRule, &opacity);
  if (opacity.typed_data.unit != UNIT_INVALID && DoApply)
    /* install the new presentation. */
    TtaSetStylePresentation (PROpacity, element, tsch, context, opacity);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseSVGStrokeOpacity: parse a SVG stroke-opacity property
  ----------------------------------------------------------------------*/
static char *ParseSVGStrokeOpacity (Element element, PSchema tsch,
                                    PresentationContext context, char *cssRule,
                                    CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue     opacity;

  opacity.typed_data.unit = UNIT_INVALID;
  opacity.typed_data.real = FALSE;
  if (!strncasecmp (cssRule, "inherit", 7))
    {
      opacity.typed_data.unit = VALUE_INHERIT;
      cssRule += 7;
    }
  else
    cssRule = ParseClampedUnit (cssRule, &opacity);
  if (opacity.typed_data.unit != UNIT_INVALID && DoApply)
    /* install the new presentation. */
    TtaSetStylePresentation (PRStrokeOpacity, element, tsch, context, opacity);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseSVGFillOpacity: parse a SVG fill-opacityl property
  ----------------------------------------------------------------------*/
static char *ParseSVGFillOpacity (Element element, PSchema tsch,
                                  PresentationContext context, char *cssRule,
                                  CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue     opacity;

  opacity.typed_data.unit = UNIT_INVALID;
  opacity.typed_data.real = FALSE;
  if (!strncasecmp (cssRule, "inherit", 7))
    {
      opacity.typed_data.unit = VALUE_INHERIT;
      cssRule += 7;
    }
  else
    cssRule = ParseClampedUnit (cssRule, &opacity);
  if (opacity.typed_data.unit != UNIT_INVALID && DoApply)
    /* install the new presentation. */
    TtaSetStylePresentation (PRFillOpacity, element, tsch, context, opacity);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseSVGFillRule: parse a SVG fill-rule property
  ----------------------------------------------------------------------*/
static char *ParseSVGFillRule (Element element, PSchema tsch,
                               PresentationContext context, char *cssRule,
                               CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue     best;

  best.typed_data.unit = UNIT_INVALID;
  best.typed_data.real = FALSE;
  if (!strncasecmp (cssRule, "inherit", 7))
    {
      best.typed_data.unit = VALUE_INHERIT;
      best.typed_data.value = 0;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "nonzero", 7))
    {
      best.typed_data.unit = VALUE_AUTO;
      best.typed_data.value = NonZero;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "evenodd", 7))
    {
      best.typed_data.unit = VALUE_AUTO;
      best.typed_data.value = EvenOdd;
      cssRule = SkipWord (cssRule);
    }

  if (DoApply)
    /* install the new presentation. */
    TtaSetStylePresentation (PRFillRule, element, tsch, context, best);
  return (cssRule);
}

/*----------------------------------------------------------------------
  GetCSSBackgroundURL searches a CSS BackgroundImage url within
  the cssRule.
  Returns NULL or a new allocated url string.
  ----------------------------------------------------------------------*/
char *GetCSSBackgroundURL (char *cssRule)
{
  char            *b, *url;

  url = NULL;
  b = strstr (cssRule, "url");
  if (b)
    b = ParseCSSUrl (b, &url);
  return (url);
}

/*----------------------------------------------------------------------
  ParseCSSContent: parse the value of property "content"
  ----------------------------------------------------------------------*/
static char *ParseCSSContent (Element element, PSchema tsch,
                              PresentationContext ctxt, char *cssRule,
                              CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   value, pval;
  char                *last, *start, quoteChar, savedChar;
  int                 length, val, l;
  char               *buffer, *p;
  char               *start_value;
  wchar_t             wc;
  ThotBool            repeat, done;

  value.typed_data.unit = UNIT_REL;
  value.typed_data.real = FALSE;
  value.typed_data.value = 0;
  if (!DoDialog && DoApply)
    TtaSetStylePresentation (PRContent, element, tsch, ctxt, value);
  cssRule = SkipBlanksAndComments (cssRule);
  start_value = cssRule;
  repeat = TRUE;
  while (repeat)
    {
      p = cssRule;
      if (!strncasecmp (cssRule, "normal", 6))
        /* The pseudo-element is not generated */
        {
          /* @@@@@@ */
          cssRule += 6;
          repeat = FALSE;
        }
      else if (!strncasecmp (cssRule, "none", 4))
        /* The pseudo-element is not generated */
        {
          /* @@@@@@ */
          cssRule += 4;
          repeat = FALSE;
        }
      else if (*cssRule == '"' || *cssRule == '\'')
        /* It's a string */
        {
          quoteChar = *cssRule;
          /* how long is the string? */
          last = SkipString (cssRule);
          length = last - cssRule;
          /* get a buffer to store the string */
          buffer = (char *)TtaGetMemory (3 * length);
          p = buffer; /* beginning of the string */
          cssRule++;

          if (ctxt->cssURL)
	    /* it's an external style sheet. Assume it is encoded in the
	       default encoding (iso-latin-1), but we should use the actual
	       encoding of the file @@@@ */
	    l = TtaGetNextWCFromString (&wc, (unsigned char **) &cssRule,
					ISO_8859_1);
	  else
	    /* it's the content of a <style> element. It is encoded in UTF-8 */
	    l = TtaGetNextWCFromString (&wc, (unsigned char **) &cssRule,
					UTF_8);
          while (wc != EOS && wc != quoteChar && l > 0)
            {
	      done = FALSE;
              if (wc == '\\')
                {
                  cssRule++; /* skip the backslash */
                  if ((*cssRule >= '0' && *cssRule <= '9') ||
                      (*cssRule >= 'A' && *cssRule <= 'F') ||
                      (*cssRule >= 'a' && *cssRule <= 'f'))
                    {
                      start = cssRule; /* first hex digit after the backslash*/
                      cssRule++;
                      while ((*cssRule >= '0' && *cssRule <= '9') ||
                             (*cssRule >= 'A' && *cssRule <= 'F') ||
                             (*cssRule >= 'a' && *cssRule <= 'f'))
                        cssRule++;
                      savedChar = *cssRule;
                      *cssRule = EOS;
                      sscanf (start, "%x", &val);
                      TtaWCToMBstring ((wchar_t) val, (unsigned char **) &p);
                      *cssRule = savedChar;
		      wc = savedChar;
		      done = TRUE;
                    }
                }
              if (!done)
                {
		  TtaWCToMBstring (wc, (unsigned char **) &p);
                  cssRule+= l;
		  if (ctxt->cssURL)
		    /* it's an external style sheet. Assume it is encoded in
		       the default encoding (iso-latin-1), but we should use
		       the actual encoding of the file @@@@ */
		    l = TtaGetNextWCFromString (&wc, (unsigned char **)&cssRule,
						ISO_8859_1);
		  else
		    /* it's the content of a <style> element. It is encoded
		       in UTF-8 */
		    l = TtaGetNextWCFromString (&wc, (unsigned char **)&cssRule,
						UTF_8);
                }
            }
          *p = EOS;
          if (DoDialog)
            {
              DisplayStyleValue ("", start_value, p);
              start_value = p;
            }
          else if (*cssRule != quoteChar)
            cssRule = SkipProperty (cssRule, FALSE);
          else
            {
              *cssRule = EOS;
              value.typed_data.unit = UNIT_REL;
              value.typed_data.real = FALSE;
              value.pointer = buffer;
              if (DoApply)
                TtaSetStylePresentation (PRContentString, element, tsch, ctxt,
                                         value);
              *cssRule = quoteChar;
              cssRule++;
            }
          TtaFreeMemory (buffer);
        }
      else if (!strncasecmp (cssRule, "url", 3))
        {  
          cssRule += 3;
          cssRule = SetCSSImage (element, tsch, ctxt, cssRule, css,
                                 PRContentURL);
          if (DoDialog)
            {
              DisplayStyleValue ("", start_value, p);
              start_value = p;
            }
        }
      else if (!strncasecmp (cssRule, "counter", 7))
        {
          value.pointer = NULL;
          cssRule += 7;
	  cssRule = SkipBlanksAndComments (cssRule);
	  if (*cssRule == '(')
	    {
	      cssRule++;
	      cssRule = SkipBlanksAndComments (cssRule);
	      start = cssRule;
	      while (*cssRule != EOS && *cssRule != ')' && *cssRule != ',')
		cssRule++;
	      if (*cssRule != ')' && *cssRule != ',')
		cssRule = start;
	      else
		{
                  /* remove extra spaces */
		  last = cssRule;
                  while (last[-1] == SPACE || last[-1] == TAB)
		    last--;
                  savedChar = *last;
                  *last = EOS;
                  value.pointer = start;
                  if (DoDialog)
                    {
                      DisplayStyleValue ("", start_value, p);
                      start_value = p;
                    }
                  else if (DoApply)
                    TtaSetStylePresentation (PRContentCounter, element, tsch,
                                             ctxt, value);
                  *last = savedChar;
		  if (*cssRule == ',')
		    /* parse the counter style */
		    {
		      cssRule++;
		      cssRule = ParseCounterStyle (cssRule, &pval, start_value);
		      cssRule = SkipBlanksAndComments (cssRule);
		      if (*cssRule == ')')
			{
			  cssRule++;
			  TtaSetStylePresentation (PRContentCounterStyle,
						   element, tsch, ctxt, pval);
			}
		    }
		}
	    }
          if (value.pointer == NULL)
            {
              CSSParseError ("Invalid content value", (char*) p, cssRule);
              if (DoDialog)
                {
                  DisplayStyleValue ("", start_value, p);
                  start_value = p;
                }
              else
                cssRule = SkipProperty (cssRule, FALSE);
            }
          cssRule++;
        }
      else if (!strncasecmp (cssRule, "counters", 8))
        {
          cssRule += 8;
          /* @@@@@@ */
          if (DoDialog)
            {
              DisplayStyleValue ("", start_value, p);
              start_value = p;
            }
          else
	    {
	      cssRule = SkipProperty (cssRule, FALSE);
	    }
        }
      else if (!strncasecmp (cssRule, "attr", 4))
        {
          value.pointer = NULL;
          cssRule += 4;
          cssRule = SkipBlanksAndComments (cssRule);
          if (*cssRule == '(')
            {
              cssRule++;
              cssRule = SkipBlanksAndComments (cssRule);
              start = cssRule;
              while (*cssRule != EOS && *cssRule != ')')
                cssRule++;
              if (*cssRule != ')')
                cssRule = start;
              else
                {
                  last = cssRule;
                  /* remove extra spaces */
		  while (last[-1] == SPACE || last[-1] == TAB)
		    last--;
                  savedChar = *last;
                  *last = EOS;
                  value.typed_data.unit = UNIT_REL;
                  value.typed_data.real = FALSE;
                  value.pointer = start;
                  if (DoDialog)
                    {
                      DisplayStyleValue ("", start_value, p);
                      start_value = p;
                    }
                  else if (DoApply)
                    TtaSetStylePresentation (PRContentAttr, element, tsch,
                                             ctxt, value);
                  *last = savedChar;
                }
            }
          if (value.pointer == NULL)
            {
              CSSParseError ("Invalid content value", (char*) p, cssRule);
              if (DoDialog)
                {
                  DisplayStyleValue ("", start_value, p);
                  start_value = p;
                }
              else
                cssRule = SkipProperty (cssRule, FALSE);
            }
          cssRule++;
        }
      else if (!strncasecmp (cssRule, "open-quote", 10))
        {
          cssRule += 10;
          /* @@@@@@ */
        }
      else if (!strncasecmp (cssRule, "close-quote", 11))
        {
          cssRule += 11;
          /* @@@@@@ */
        }
      else if (!strncasecmp (cssRule, "no-open-quote", 13))
        {
          cssRule += 13;
          /* @@@@@@ */
        }
      else if (!strncasecmp (cssRule, "no-close-quote", 14))
        {
          cssRule += 14;
          /* @@@@@@ */
        }
      else if (!strncasecmp (cssRule, "inherit", 7))
        {
          cssRule += 7;
          /* @@@@@@ */
          repeat = FALSE;
        }
      else
        {
          CSSParseError ("Invalid content value", (char*) p, cssRule);
          if (DoDialog)
            {
              DisplayStyleValue ("", start_value, p);
              start_value = p;
            }
          else
            cssRule = SkipProperty (cssRule, FALSE);
        }
      cssRule = SkipBlanksAndComments (cssRule);
      if (repeat)
        if (*cssRule == ';' || *cssRule == '}' || *cssRule == EOS ||
            *cssRule == '!')
          repeat = FALSE;
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSCounterOp: parse a CSS counter operation (increment or reset,
  depending on type).
  ----------------------------------------------------------------------*/
static char *ParseCSSCounterOp (Element element, PSchema tsch,
				PresentationContext ctxt,
				char *cssRule, unsigned int type)
{
  PresentationValue  pval;
  char              *start, *start_value, *p, *buff;
  char               saved;

  cssRule = SkipBlanksAndComments (cssRule);
  if (!strncasecmp (cssRule, "inherit", 7))
    {
      pval.typed_data.unit = VALUE_INHERIT;
      cssRule += 7;
      /* @@@@ do something */
    }
  else if (!strncasecmp (cssRule, "none", 4))
    {
      pval.typed_data.value = 0;
      cssRule += 4;
      /* @@@@ do something */
    }
  else
    {
      start_value = cssRule;
      /* there may be multiple counter names (each possibly followed by an
	 integer)  */
      do
	{
	  p = cssRule;
	  /* name of a counter */
	  start = cssRule;
	  cssRule = SkipWord (cssRule);
	  saved = *cssRule;
	  *cssRule = EOS;
          buff = TtaStrdup (start);
	  *cssRule = saved;
	  pval.pointer = buff;
	  /* set default value of parameter */
	  if (type == PRCounterReset)
	    pval.data = 0;
	  else if (type == PRCounterIncrement)
	    pval.data = 1;
	  /* use the actual value of parameter (integer) if it is specified */
	  cssRule = ParseNumber (cssRule, &pval);
	  if (pval.typed_data.unit != UNIT_INVALID)
	    /* there is an integer, the value of the parameter */
	    {
	      pval.data = pval.typed_data.value;
	      cssRule = SkipBlanksAndComments (cssRule);
	    }
	  if (DoDialog)
	    {
	      DisplayStyleValue ("", start_value, p);
	      start_value = p;
	    }
	  else if (DoApply)
	    TtaSetStylePresentation (type, element, tsch, ctxt, pval);
          TtaFreeMemory (buff);
	}
      while (*cssRule != ';' && *cssRule != '}' && *cssRule != EOS &&
	     *cssRule != ',');
    }
  cssRule = SkipBlanksAndComments (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSCounterIncrement: parse a CSS counter-increment property
  ----------------------------------------------------------------------*/
static char *ParseCSSCounterIncrement (Element element, PSchema tsch,
				       PresentationContext ctxt,
				       char *cssRule, CSSInfoPtr css,
				       ThotBool isHTML)
{
  return ParseCSSCounterOp (element, tsch, ctxt, cssRule, PRCounterIncrement);
}

/*----------------------------------------------------------------------
  ParseCSSCounterReset: parse a CSS counter-reset property
  ----------------------------------------------------------------------*/
static char *ParseCSSCounterReset (Element element, PSchema tsch,
				   PresentationContext ctxt,
				   char *cssRule, CSSInfoPtr css,
				   ThotBool isHTML)
{
  return ParseCSSCounterOp (element, tsch, ctxt, cssRule, PRCounterReset);
}

/*----------------------------------------------------------------------
  ParseCSSBackgroundImage: parse a CSS BackgroundImage attribute string.
  ----------------------------------------------------------------------*/
static char *ParseCSSBackgroundImage (Element element, PSchema tsch,
                                      PresentationContext ctxt,
                                      char *cssRule, CSSInfoPtr css,
                                      ThotBool isHTML)
{
  PresentationValue          image, value;
  char                       *ptr;

  image.typed_data.real = FALSE;
  value.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  if (!strncasecmp (cssRule, "none", 4))
    {
      cssRule += 4;
      if (DoDialog)
        DisplayStyleValue ("background-image", ptr, cssRule);
      else if (DoApply)
        {
          /* no background image */
          image.pointer = NULL;
          TtaSetStylePresentation (PRBackgroundPicture, element, tsch, ctxt,
                                   image);
        }
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      value.typed_data.unit = VALUE_INHERIT;
      cssRule += 7;
      if (DoDialog)
        DisplayStyleValue ("background-image", ptr, cssRule);
    }
  else if (!strncasecmp (cssRule, "url", 3))
    {  
      cssRule += 3;
      cssRule = SetCSSImage (element, tsch, ctxt, cssRule, css,
                             PRBackgroundPicture);
      if (ctxt->destroy)
        if (TtaGetStylePresentation (PRFillPattern, element, tsch, ctxt,
                                     &value) < 0)
          {
            /* there is no FillPattern rule -> remove ShowBox rule */
            value.typed_data.value = 1;
            value.typed_data.unit = UNIT_REL;
            value.typed_data.real = FALSE;
            TtaSetStylePresentation (PRShowBox, element, tsch, ctxt, value);
          }
    }
  else
    {
      cssRule = SkipWord (cssRule);
      CSSParseError ("Invalid background-image value", ptr, cssRule);
      cssRule = SkipProperty (cssRule, FALSE);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseACSSBackgroundRepeat: parse a CSS BackgroundRepeat attribute string.
  ----------------------------------------------------------------------*/
static char *ParseACSSBackgroundRepeat (Element element, PSchema tsch,
                                        PresentationContext ctxt,
                                        char *cssRule, CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   repeat;
  char               *start_value;

  cssRule = SkipBlanksAndComments (cssRule);
  start_value = cssRule;
  repeat.typed_data.value = REALSIZE;
  repeat.typed_data.unit = UNIT_BOX;
  repeat.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  if (!strncasecmp (cssRule, "no-repeat", 9))
    repeat.typed_data.value = REALSIZE;
  else if (!strncasecmp (cssRule, "repeat-y", 8))
    repeat.typed_data.value = YREPEAT;
  else if (!strncasecmp (cssRule, "repeat-x", 8))
    repeat.typed_data.value = XREPEAT;
  else if (!strncasecmp (cssRule, "repeat", 6))
    repeat.typed_data.value = REPEAT;
  else
    return (cssRule);

  cssRule = SkipWord (cssRule);
  /* check if it's an important rule */
  if (DoDialog)
    DisplayStyleValue ("background-repeat", start_value, cssRule);
  else if (DoApply)
    /* install the new presentation */
    TtaSetStylePresentation (PRBackgroundRepeat, element, tsch, ctxt, repeat);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBackgroundRepeat: parse a CSS BackgroundRepeat attribute string.
  ----------------------------------------------------------------------*/
static char *ParseCSSBackgroundRepeat (Element element, PSchema tsch,
                                       PresentationContext ctxt,
                                       char *cssRule, CSSInfoPtr css,
                                       ThotBool isHTML)
{

  char     *ptr;

  ptr = cssRule;
  cssRule = ParseACSSBackgroundRepeat (element, tsch, ctxt,
                                       cssRule, css, isHTML);

  if (ptr == cssRule)
    {
      cssRule = SkipValue ("Invalid background-repeat value", cssRule);
      /* check if it's an important rule */
    }
  return cssRule;
}

/*----------------------------------------------------------------------
  ParseACSSBackgroundAttachment: parse a CSS BackgroundAttachment
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseACSSBackgroundAttachment (Element element, PSchema tsch,
                                            PresentationContext ctxt,
                                            char *cssRule, CSSInfoPtr css,
                                            ThotBool isHTML)
{
  char               *start_value;

  cssRule = SkipBlanksAndComments (cssRule);
  start_value = cssRule;
  if (!strncasecmp (cssRule, "scroll", 6))
    {
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "fixed", 5))
    {
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      cssRule = SkipWord (cssRule);
    }
  if (start_value != cssRule && DoDialog)
    DisplayStyleValue ("background-attachment", start_value, cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBackgroundAttachment: parse a CSS BackgroundAttachment
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSBackgroundAttachment (Element element, PSchema tsch,
                                           PresentationContext ctxt,
                                           char *cssRule, CSSInfoPtr css,
                                           ThotBool isHTML)
{
  char     *ptr;

  ptr = cssRule;
  cssRule = ParseACSSBackgroundAttachment (element, tsch, ctxt,
                                           cssRule, css, isHTML);
  if (ptr == cssRule)
    cssRule = SkipValue ("Invalid background-attachment value", cssRule);
  return cssRule;
}

/*----------------------------------------------------------------------
  ParseACSSBackgroundPosition: parse a CSS BackgroundPosition
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseACSSBackgroundPosition (Element element, PSchema tsch,
                                          PresentationContext ctxt,
                                          char *cssRule, CSSInfoPtr css,
                                          ThotBool isHTML, ThotBool *across)
{
  PresentationValue   val;
  char               *ptr;

  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  val.typed_data.value = 0;
  val.typed_data.real = FALSE;
  val.typed_data.unit = UNIT_INVALID;
  if (!strncasecmp (cssRule, "left", 4))
    {
      val.typed_data.value = 0;
      val.typed_data.unit = UNIT_PERCENT;
      cssRule += 4;
      *across = TRUE;
    }
  else if (!strncasecmp (cssRule, "right", 5))
    {
      val.typed_data.value = 100;
      val.typed_data.unit = UNIT_PERCENT;
      cssRule += 5;
      *across = TRUE;
    }
  else if (!strncasecmp (cssRule, "center", 6))
    {
      val.typed_data.value = 50;
      val.typed_data.unit = UNIT_PERCENT;
      cssRule += 6;
    }
  else if (!strncasecmp (cssRule, "top", 3))
    {
      val.typed_data.value = 0;
      val.typed_data.unit = UNIT_PERCENT;
      cssRule += 3;
      *across = FALSE;
    }
  else if (!strncasecmp (cssRule, "bottom", 6))
    {
      val.typed_data.value = 100;
      val.typed_data.unit = UNIT_PERCENT;
      cssRule += 6;
      *across = FALSE;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      val.typed_data.unit = VALUE_INHERIT;
      cssRule += 7;
    }
  else
    /* <length> or <percentage> */
    {
      cssRule = ParseCSSUnit (cssRule, &val);
      if (val.typed_data.unit == UNIT_BOX && val.typed_data.value == 0)
        /* 0 with no unit. Accept */
        val.typed_data.unit = UNIT_PERCENT;
    }

  if (val.typed_data.unit != UNIT_INVALID && val.typed_data.unit != UNIT_BOX)
    {
      if (DoDialog)
        {
          if (val.typed_data.unit == VALUE_INHERIT)
            {
              DisplayStyleValue ("background-positionH", ptr, cssRule);
              DisplayStyleValue ("background-positionV", ptr, cssRule);
            }
          else if (*across)
              DisplayStyleValue ("background-positionH", ptr, cssRule);
          else
              DisplayStyleValue ("background-positionV", ptr, cssRule);
        }
      else if (DoApply)
        /* install the new presentation */
        {
          if (val.typed_data.unit == VALUE_INHERIT)
            /* "inherit" applies to both dimensions */
            {
              TtaSetStylePresentation (PRBackgroundHorizPos, element, tsch,
                                       ctxt, val);
              TtaSetStylePresentation (PRBackgroundVertPos, element, tsch,
                                       ctxt, val);
            }
          else if (*across)
            TtaSetStylePresentation (PRBackgroundHorizPos, element, tsch,
                                     ctxt, val);
          else
            TtaSetStylePresentation (PRBackgroundVertPos, element, tsch,
                                     ctxt, val);
        }
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBackgroundPosition: parse a CSS BackgroundPosition
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSBackgroundPosition (Element element, PSchema tsch,
                                         PresentationContext ctxt,
                                         char *cssRule, CSSInfoPtr css,
                                         ThotBool isHTML)
{
  char     *ptr;
  ThotBool  across;

  ptr = cssRule;
  across = TRUE;
  cssRule = ParseACSSBackgroundPosition (element, tsch, ctxt, cssRule, css,
                                         isHTML, &across);
  if (ptr == cssRule)
    cssRule = SkipValue ("Invalid background-position value", cssRule);
  else
    {
      cssRule = SkipBlanksAndComments (cssRule);
      if (*cssRule !=  ';' && *cssRule !=  '!' && *cssRule != EOS)
        {
          /* possible second value */
          ptr = cssRule;
          across = !across;
          cssRule = ParseACSSBackgroundPosition (element, tsch, ctxt, cssRule,
                                                 css, isHTML, &across);
          if (ptr == cssRule)
            cssRule = SkipValue ("Invalid background-position value", cssRule);
        }
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBackground: parse a CSS background attribute 
  ----------------------------------------------------------------------*/
static char *ParseCSSBackground (Element element, PSchema tsch,
                                 PresentationContext ctxt, char *cssRule,
                                 CSSInfoPtr css, ThotBool isHTML)
{
  char           *ptr;
  int             skippedNL;
  ThotBool        img, repeat, position, attach, color, across;

  cssRule = SkipBlanksAndComments (cssRule);
  img = repeat = position = attach = color = FALSE;
  across = TRUE;
  while (*cssRule != ';' && *cssRule != '}' && *cssRule != EOS && *cssRule != ',')
    {
      /* perhaps a Background Image */
      if (!strncasecmp (cssRule, "url", 3) || !strncasecmp (cssRule, "none", 4))
        {
          if (!strncasecmp (cssRule, "none", 4))
            {
              repeat = TRUE;
              ParseCSSBackgroundColor (element, tsch, ctxt, "transparent",
                                       css, isHTML);
            }
          cssRule = ParseCSSBackgroundImage (element, tsch, ctxt, cssRule,
                                             css, isHTML);
          img = TRUE;
        }
      /* perhaps a Background Attachment */
      else if (!strncasecmp (cssRule, "scroll", 6) ||
               !strncasecmp (cssRule, "fixed", 5))
        {
          cssRule = ParseACSSBackgroundAttachment (element, tsch, ctxt,
                                                   cssRule, css, isHTML);
          attach = repeat = TRUE;
        }
      /* perhaps a Background Repeat */
      else if (!strncasecmp (cssRule, "no-repeat", 9) ||
               !strncasecmp (cssRule, "repeat-y", 8)  ||
               !strncasecmp (cssRule, "repeat-x", 8)  ||
               !strncasecmp (cssRule, "repeat", 6))
        {
          cssRule = ParseACSSBackgroundRepeat (element, tsch, ctxt,
                                               cssRule, css, isHTML);
          repeat = TRUE;
        }
      /* perhaps a Background Position */
      else if (!strncasecmp (cssRule, "left", 4)   ||
               !strncasecmp (cssRule, "right", 5)  ||
               !strncasecmp (cssRule, "center", 6) ||
               !strncasecmp (cssRule, "top", 3)    ||
               !strncasecmp (cssRule, "bottom", 6) ||
               isdigit (*cssRule) || *cssRule == '.' || *cssRule == '-')
        {
          cssRule = ParseACSSBackgroundPosition (element, tsch, ctxt, cssRule,
                                                 css, isHTML, &across);
          across = !across;
          position = repeat = TRUE;
        }
      /* perhaps a Background Color */
      else if (!color)
        {
          skippedNL = NewLineSkipped;
          /* check if the rule has been found */
          ptr = cssRule;
          cssRule = ParseCSSBackgroundColor (element, tsch, ctxt,
                                             cssRule, css, isHTML);
          if (ptr == cssRule)
            {
              NewLineSkipped = skippedNL;
              /* rule not found */
              cssRule = SkipProperty (cssRule, FALSE);
            }
          else
            color = TRUE;
        }
      else
        cssRule = SkipProperty (cssRule, FALSE);

      cssRule = SkipBlanksAndComments (cssRule);
    }

  if (color && !img)
    ParseCSSBackgroundImage (element, tsch, ctxt, (char*)
        "none", css, isHTML);
  
  if (img && !repeat)
    ParseACSSBackgroundRepeat (element, tsch, ctxt,
        (char*)"repeat", css, isHTML);
  if (img && !position)
    ParseACSSBackgroundPosition (element, tsch, ctxt,
        (char*)"0% 0%", css, isHTML, &across);
  if (img && !attach)
    ParseACSSBackgroundAttachment (element, tsch, ctxt,
                                   (char*)"scroll", css, isHTML);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSPageBreakBefore: parse a CSS page-break-before attribute 
  ----------------------------------------------------------------------*/
static char *ParseCSSPageBreakBefore (Element element, PSchema tsch,
                                      PresentationContext ctxt, char *cssRule,
                                      CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   page;
  char               *start_value;

  page.typed_data.unit = UNIT_INVALID;
  page.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  start_value = cssRule;
  if (!strncasecmp (cssRule, "auto", 4))
    page.typed_data.value = PageAuto;
  else if (!strncasecmp (cssRule, "always", 6))
    {
      page.typed_data.unit = UNIT_REL;
      page.typed_data.value = PageAlways;
    }
  else if (!strncasecmp (cssRule, "avoid", 5))
    {
      page.typed_data.unit = UNIT_REL;
      page.typed_data.value = PageAvoid;
    }
  else if (!strncasecmp (cssRule, "left", 4))
    {
      page.typed_data.unit = UNIT_REL;
      page.typed_data.value = PageLeft;
    }
  else if (!strncasecmp (cssRule, "right", 5))
    {
      page.typed_data.unit = UNIT_REL;
      page.typed_data.value = PageRight;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      page.typed_data.unit = VALUE_INHERIT;
      page.typed_data.value = PageInherit;
    }
  cssRule = SkipWord (cssRule);
  /* install the new presentation */
  if ((page.typed_data.unit == UNIT_REL && page.typed_data.value == PageAlways)
      || page.typed_data.unit == VALUE_INHERIT)
    {
      if (DoDialog)
        DisplayStyleValue ("page-break-before", start_value, cssRule);
      else if (DoApply)
        TtaSetStylePresentation (PRPageBefore, element, tsch, ctxt, page);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSPageBreakAfter: parse a CSS page-break-after attribute 
  ----------------------------------------------------------------------*/
static char *ParseCSSPageBreakAfter (Element element, PSchema tsch,
                                     PresentationContext ctxt,
                                     char *cssRule, CSSInfoPtr css,
                                     ThotBool isHTML)
{
  PresentationValue   page;
  char               *start_value;

  page.typed_data.unit = UNIT_INVALID;
  page.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  start_value = cssRule;
  if (!strncasecmp (cssRule, "auto", 4))
    page.typed_data.value = PageAuto;
  else if (!strncasecmp (cssRule, "always", 6))
    {
      page.typed_data.unit = UNIT_REL;
      page.typed_data.value = PageAlways;
    }
  else if (!strncasecmp (cssRule, "avoid", 5))
    {
      page.typed_data.unit = UNIT_REL;
      page.typed_data.value = PageAvoid;
    }
  else if (!strncasecmp (cssRule, "left", 4))
    {
      page.typed_data.unit = UNIT_REL;
      page.typed_data.value = PageLeft;
    }
  else if (!strncasecmp (cssRule, "right", 5))
    {
      page.typed_data.unit = UNIT_REL;
      page.typed_data.value = PageRight;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      page.typed_data.unit = VALUE_INHERIT;
      page.typed_data.value = PageInherit;
    }
  cssRule = SkipWord (cssRule);
  /* install the new presentation */
  if (page.typed_data.unit == UNIT_REL || page.typed_data.unit == VALUE_INHERIT)
    {
      if (DoDialog)
        DisplayStyleValue ("page-break-after", start_value, cssRule);
      //else if (DoApply)
        // TtaSetStylePresentation (PRPageAfter, element, tsch, ctxt, page);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSPageBreakInside: parse a CSS page-break-inside attribute 
  ----------------------------------------------------------------------*/
static char *ParseCSSPageBreakInside (Element element, PSchema tsch,
                                      PresentationContext ctxt,
                                      char *cssRule, CSSInfoPtr css,
                                      ThotBool isHTML)
{
  PresentationValue   page;
  char               *start_value;

  page.typed_data.unit = UNIT_INVALID;
  page.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  start_value = cssRule;
  if (!strncasecmp (cssRule, "auto", 4))
    {
      /*page.typed_data.unit = UNIT_REL;*/
      page.typed_data.value = PageAuto;
    }
  else if (!strncasecmp (cssRule, "avoid", 5))
    {
      page.typed_data.unit = UNIT_REL;
      page.typed_data.value = PageAvoid;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      /* page.typed_data.unit = VALUE_INHERIT; */
      page.typed_data.value = PageInherit;
    }
  cssRule = SkipWord (cssRule);
  /* install the new presentation */
  if ((page.typed_data.unit == UNIT_REL || page.typed_data.unit == VALUE_INHERIT) &&
      page.typed_data.value == PageAvoid)
    {
      if (DoDialog)
        DisplayStyleValue ("page-break-inside", start_value, cssRule);
      //else if (DoApply)
        //TtaSetStylePresentation (PRPageInside, element, tsch, ctxt, page);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseSVGStrokeWidth: parse a SVG stroke-width property value.
  ----------------------------------------------------------------------*/
static char *ParseSVGStrokeWidth (Element element, PSchema tsch,
                                  PresentationContext ctxt, char *cssRule,
                                  CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   width;
  
  cssRule = SkipBlanksAndComments (cssRule);
  width.typed_data.value = 0;
  width.typed_data.unit = UNIT_INVALID;
  width.typed_data.real = FALSE;
  if (isdigit (*cssRule) || *cssRule == '.')
    {
      cssRule = ParseCSSUnit (cssRule, &width);
      if (width.typed_data.unit == UNIT_BOX)
        width.typed_data.unit = UNIT_PX;
    }
  else
    cssRule = SkipValue ("Invalid stroke-width value", cssRule);

  if (width.typed_data.unit != UNIT_INVALID && DoApply)
    {
      TtaSetStylePresentation (PRLineWeight, element, tsch, ctxt, width);
      width.typed_data.value = 1;
      width.typed_data.unit = UNIT_REL;
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSPosition: parse a CSS Position attribute string.
  ----------------------------------------------------------------------*/
static char *ParseCSSPosition (Element element, PSchema tsch,
                               PresentationContext ctxt, char *cssRule,
                               CSSInfoPtr css, ThotBool isHTML)
{
  char               *ptr;
  PresentationValue   pval;

  pval.typed_data.value = 0;
  pval.typed_data.unit = UNIT_BOX;
  pval.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  if (!strncasecmp (cssRule, "static", 6))
    {
      pval.typed_data.value = PositionStatic;
      cssRule += 6;
    }
  else if (!strncasecmp (cssRule, "relative", 8))
    {
      pval.typed_data.value = PositionRelative;
      cssRule += 8;
    }
  else if (!strncasecmp (cssRule, "absolute", 8))
    {
      pval.typed_data.value = PositionAbsolute;
      cssRule += 8;
    }
  else if (!strncasecmp (cssRule, "fixed", 5))
    {
      pval.typed_data.value = PositionFixed;
      cssRule += 5;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      pval.typed_data.unit = VALUE_INHERIT;
      cssRule += 7;
    }

  if (pval.typed_data.value == 0 && pval.typed_data.unit != VALUE_INHERIT)
    {
      cssRule = SkipValue ("Invalid position value", ptr);
      cssRule = SkipValue (NULL, cssRule);
    }
  else
    {
      cssRule = SkipBlanksAndComments (cssRule);
      if (*cssRule != EOS && *cssRule != ';')
        SkipValue ("Invalid position value", ptr);
      else if (DoDialog)
        DisplayStyleValue ("position", ptr, cssRule);
      else if (DoApply)
        TtaSetStylePresentation (PRPosition, element, tsch, ctxt, pval);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSTop: parse a CSS Top attribute
  ----------------------------------------------------------------------*/
static char *ParseCSSTop (Element element, PSchema tsch,
                          PresentationContext context, char *cssRule,
                          CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   val;
  char               *ptr;

  val.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the value */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      val.typed_data.unit = VALUE_AUTO;
      val.typed_data.value = 0;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      val.typed_data.unit = VALUE_INHERIT;
      cssRule = SkipWord (cssRule);
    }
  else
    cssRule = ParseCSSUnit (cssRule, &val);

  if (val.typed_data.unit == UNIT_INVALID ||
      (val.typed_data.value != 0 &&
       val.typed_data.unit == UNIT_BOX))
    {
      cssRule = SkipValue ("Invalid top value", ptr);
      if (val.typed_data.unit == UNIT_BOX)
        val.typed_data.unit = UNIT_PX;
      else
        return (cssRule);
    }
  if (DoDialog)
    DisplayStyleValue ("top", ptr, cssRule);
  else if (DoApply)
    TtaSetStylePresentation (PRTop, element, tsch, context, val);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSRight: parse a CSS Right attribute
  ----------------------------------------------------------------------*/
static char *ParseCSSRight (Element element, PSchema tsch,
                            PresentationContext context, char *cssRule,
                            CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   val;
  char               *ptr;

  val.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      val.typed_data.unit = VALUE_AUTO;
      val.typed_data.value = 0;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      val.typed_data.unit = VALUE_INHERIT;
      cssRule = SkipWord (cssRule);
    }
  else
    cssRule = ParseCSSUnit (cssRule, &val);

  if (val.typed_data.unit == UNIT_INVALID ||
      (val.typed_data.value != 0 &&
       val.typed_data.unit == UNIT_BOX))
    {
      cssRule = SkipValue ("Invalid right value", ptr);
      if (val.typed_data.unit == UNIT_BOX)
        val.typed_data.unit = UNIT_PX;
      else
        return (cssRule);
    }
  if (DoDialog)
        DisplayStyleValue ("right", ptr, cssRule);
  else if (DoApply)
    TtaSetStylePresentation (PRRight, element, tsch, context, val);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSBottom: parse a CSS Bottom attribute
  ----------------------------------------------------------------------*/
static char *ParseCSSBottom (Element element, PSchema tsch,
                             PresentationContext context, char *cssRule,
                             CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   val;
  char               *ptr;

  val.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      val.typed_data.unit = VALUE_AUTO;
      val.typed_data.value = 0;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      val.typed_data.unit = VALUE_INHERIT;
      cssRule = SkipWord (cssRule);
    }
  else
    cssRule = ParseCSSUnit (cssRule, &val);

  if (val.typed_data.unit == UNIT_INVALID ||
      (val.typed_data.value != 0 &&
       val.typed_data.unit == UNIT_BOX))
    {
      cssRule = SkipValue ("Invalid bottom value", ptr);
      if (val.typed_data.unit == UNIT_BOX)
        val.typed_data.unit = UNIT_PX;
      else
        return (cssRule);
    }
  if (DoDialog)
        DisplayStyleValue ("bottom", ptr, cssRule);
  else if (DoApply)
    TtaSetStylePresentation (PRBottom, element, tsch, context, val);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSLeft: parse a CSS Left attribute
  ----------------------------------------------------------------------*/
static char *ParseCSSLeft (Element element, PSchema tsch,
                           PresentationContext context, char *cssRule,
                           CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   val;
  char               *ptr;

  val.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      val.typed_data.unit = VALUE_AUTO;
      val.typed_data.value = 0;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      val.typed_data.unit = VALUE_INHERIT;
      cssRule = SkipWord (cssRule);
    }
  else
    cssRule = ParseCSSUnit (cssRule, &val);

  if (val.typed_data.unit == UNIT_INVALID ||
      (val.typed_data.value != 0 &&
       val.typed_data.unit == UNIT_BOX))
    {
      cssRule = SkipValue ("Invalid left value", ptr);
      if (val.typed_data.unit == UNIT_BOX)
        val.typed_data.unit = UNIT_PX;
      else
        return (cssRule);
    }
  if (DoDialog)
        DisplayStyleValue ("left", ptr, cssRule);
  else if (DoApply)
    TtaSetStylePresentation (PRLeft, element, tsch, context, val);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSZIndex: parse a CSS z-index attribute
  ----------------------------------------------------------------------*/
static char *ParseCSSZIndex (Element element, PSchema tsch,
                             PresentationContext context, char *cssRule,
                             CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   val;
  char               *ptr;

  val.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      val.typed_data.unit = VALUE_AUTO;
      val.typed_data.value = 0;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      val.typed_data.unit = VALUE_INHERIT;
      val.typed_data.value = 0;
      cssRule = SkipWord (cssRule);
    }
  else
    {
      cssRule = ParseCSSUnit (cssRule, &val);
      if (val.typed_data.unit != UNIT_BOX)
        {
          cssRule = SkipValue ("Invalid z-index value", ptr);
          return (cssRule);
        }
      val.typed_data.value = - val.typed_data.value;
    }
  if (DoDialog)
    DisplayStyleValue ("z-index", ptr, cssRule);
  else if (DoApply)
    TtaSetStylePresentation (PRDepth, element, tsch, context, val);
  return (cssRule);
}

/*----------------------------------------------------------------------
 *
 *	STYLE PROPERTY DECLARATIONS
 *
 *----------------------------------------------------------------------*/
/*
 * NOTE: Long property names MUST be placed before shortened ones !
 *        e.g. "font-size" must be placed before "font"
 */
static CSSProperty CSSProperties[] =
  {
    {"background-color", ParseCSSBackgroundColor},
    {"background-image", ParseCSSBackgroundImage},
    {"background-repeat", ParseCSSBackgroundRepeat},
    {"background-attachment", ParseCSSBackgroundAttachment},
    {"background-position", ParseCSSBackgroundPosition},
    {"background", ParseCSSBackground},
    {"border-top-width", ParseCSSBorderTopWidth},
    {"border-right-width", ParseCSSBorderRightWidth},
    {"border-bottom-width", ParseCSSBorderBottomWidth},
    {"border-left-width", ParseCSSBorderLeftWidth},
    {"border-width", ParseCSSBorderWidth},
    {"border-top-color", ParseCSSBorderColorTop},
    {"border-right-color", ParseCSSBorderColorRight},
    {"border-bottom-color", ParseCSSBorderColorBottom},
    {"border-left-color", ParseCSSBorderColorLeft},
    {"border-color", ParseCSSBorderColor},
    {"border-top-style", ParseCSSBorderStyleTop},
    {"border-right-style", ParseCSSBorderStyleRight},
    {"border-bottom-style", ParseCSSBorderStyleBottom},
    {"border-left-style", ParseCSSBorderStyleLeft},
    {"border-style", ParseCSSBorderStyle},
    {"border-top", ParseCSSBorderTop},
    {"border-right", ParseCSSBorderRight},
    {"border-bottom", ParseCSSBorderBottom},
    {"border-left", ParseCSSBorderLeft},
    {"border", ParseCSSBorder},
    {"bottom", ParseCSSBottom},
    {"clear", ParseCSSClear},
    {"color", ParseCSSForeground},
    {"content", ParseCSSContent},
    {"counter-increment", ParseCSSCounterIncrement},
    {"counter-reset", ParseCSSCounterReset},
    {"direction", ParseCSSDirection},
    {"display", ParseCSSDisplay},
    {"float", ParseCSSFloat},
    {"font-family", ParseCSSFontFamily},
    {"font-style", ParseCSSFontStyle},
    {"font-variant", ParseCSSFontVariant},
    {"font-weight", ParseCSSFontWeight},
    {"font-size-adjust", ParseCSSFontSizeAdjust},
    {"font-size", ParseCSSFontSize},
    {"font", ParseCSSFont},
    {"max-height", ParseCSSMaxHeight},
    {"min-height", ParseCSSMinHeight},
    {"height", ParseCSSHeight},
    {"left", ParseCSSLeft},
    {"letter-spacing", ParseCSSLetterSpacing},
    {"line-height", ParseCSSLineHeight},
    {"list-style-type", ParseCSSListStyleType},
    {"list-style-image", ParseCSSListStyleImage},
    {"list-style-position", ParseCSSListStylePosition},
    {"list-style", ParseCSSListStyle},
    {"margin-bottom", ParseCSSMarginBottom},
    {"margin-top", ParseCSSMarginTop},
    {"margin-right", ParseCSSMarginRight},
    {"margin-left", ParseCSSMarginLeft},
    {"margin", ParseCSSMargin},
    {"opacity", ParseCSSOpacity},
    {"padding-top", ParseCSSPaddingTop},
    {"padding-right", ParseCSSPaddingRight},
    {"padding-bottom", ParseCSSPaddingBottom},
    {"padding-left", ParseCSSPaddingLeft},
    {"padding", ParseCSSPadding},
    {"page-break-before", ParseCSSPageBreakBefore},
    {"page-break-after", ParseCSSPageBreakAfter},
    {"page-break-inside", ParseCSSPageBreakInside},
    {"position", ParseCSSPosition},
    {"right", ParseCSSRight},
    {"text-align", ParseCSSTextAlign},
    {"text-anchor", ParseCSSTextAnchor},
    {"text-indent", ParseCSSTextIndent},
    {"text-decoration", ParseCSSTextDecoration},
    {"text-transform", ParseCSSTextTransform},
    {"top", ParseCSSTop},
    {"unicode-bidi", ParseCSSUnicodeBidi},
    {"vertical-align", ParseCSSVerticalAlign},
    {"white-space", ParseCSSWhiteSpace},
    {"max-width", ParseCSSMaxWidth},
    {"min-width", ParseCSSMinWidth},
    {"width", ParseCSSWidth},
    {"visibility", ParseCSSVisibility},
    {"word-spacing", ParseCSSWordSpacing},
    {"z-index", ParseCSSZIndex},

    /* SVG extensions */
    {"fill-opacity", ParseSVGFillOpacity},
    {"fill-rule", ParseSVGFillRule},
    {"fill", ParseSVGFill},
    {"marker-end", ParseSVGMarkerEnd},
    {"marker-mid", ParseSVGMarkerMid},
    {"marker-start", ParseSVGMarkerStart},
    {"marker", ParseSVGMarker},
    {"opacity", ParseSVGOpacity},
    {"stop-color", ParseSVGStopColor},
    {"stop-opacity", ParseSVGStopOpacity},
    {"stroke-opacity", ParseSVGStrokeOpacity},
    {"stroke-width", ParseSVGStrokeWidth},
    {"stroke", ParseSVGStroke}
  };

#define NB_CSSSTYLEATTRIBUTE (sizeof(CSSProperties) / sizeof(CSSProperty))

/*----------------------------------------------------------------------
  ParseCSSRule: parse a CSS Style string                        
  we expect the input string describing the style to be of the form
  property: value [ ; property: value ]* 
  but tolerate incorrect or incomplete input                    
  ----------------------------------------------------------------------*/
void  ParseCSSRule (Element element, PSchema tsch, PresentationContext ctxt,
                    char *cssRule, CSSInfoPtr css, ThotBool isHTML)
{
  DisplayMode         dispMode = DisplayImmediately;
  char               *p = NULL, *next, *end;
  char               *valueStart;
  int                 lg;
  unsigned int        i;
  ThotBool            found;

  /* avoid too many redisplay */
  if (!DoDialog && ctxt->doc)
    {
      dispMode = TtaGetDisplayMode (ctxt->doc);
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (ctxt->doc, DeferredDisplay);
    }

  while (*cssRule != EOS)
    {
      cssRule = SkipBlanksAndComments (cssRule);
      if (*cssRule == ';' || *cssRule < 0x20 ||
          ((unsigned char)*cssRule) == 0xA0)
        cssRule++;
      else if (*cssRule < 0x41 || *cssRule > 0x7A ||
          (*cssRule > 0x5A && *cssRule < 0x61))
        {
          end = SkipProperty (cssRule, FALSE);
          if (cssRule[0] != '-')
            CSSParseError ("Invalid property", cssRule, end);
          cssRule = end; 
        }
      else if (*cssRule != EOS)
        {
          found = FALSE;
          /* look for the type of property */
          for (i = 0; i < NB_CSSSTYLEATTRIBUTE && !found; i++)
            {
              lg = strlen (CSSProperties[i].name);
              if (!strncasecmp (cssRule, CSSProperties[i].name, lg))
                {
                  p = cssRule + lg;
                  found = TRUE;
                  i--;
                }
            }

          // check if it's an important rule
          CheckImportantRule (cssRule, ctxt);
          if (i < NB_CSSSTYLEATTRIBUTE &&
              !strcasecmp (CSSProperties[i].name, "content") &&
              ((GenericContext)ctxt)->pseudo != PbBefore &&
              ((GenericContext)ctxt)->pseudo != PbAfter)
            /* property content is allowed only for pseudo-elements :before and
               :after */
            {
              end = SkipProperty (cssRule, FALSE);
              CSSParseError ("content is allowed only for pseudo-elements",
                             cssRule, end);
              cssRule = end;
            }
          else if (i == NB_CSSSTYLEATTRIBUTE)
            cssRule = SkipProperty (cssRule, !ctxt->destroy);
          else
            {
              /* update index and skip the ":" indicator if present */
              p = SkipBlanksAndComments (p);
              if (*p == ':')
                {
                  p++;
                  p = SkipBlanksAndComments (p);
                  /* try to parse the value associated with this property */
                  if (CSSProperties[i].parsing_function != NULL)
                    {
                      valueStart = p;
                      p = CSSProperties[i].parsing_function (element, tsch,
                                                             ctxt, p, css, isHTML);
                      if (!element && isHTML)
                        {
                          if  (ctxt->type == HTML_EL_Input)
                            /* it's a generic rule for the HTML element input.
                               Generate a Thot Pres rule for each kind of
                               input element */
                            {
                              ctxt->type = HTML_EL_Text_Input;
                              p = CSSProperties[i].parsing_function (element,
                                                                     tsch, ctxt, valueStart, css, isHTML);
                              ctxt->type = HTML_EL_Password_Input;
                              p = CSSProperties[i].parsing_function (element,
                                                                     tsch, ctxt, valueStart, css, isHTML);
                              ctxt->type = HTML_EL_File_Input;
                              p = CSSProperties[i].parsing_function (element,
                                                                     tsch, ctxt, valueStart, css, isHTML);
                              ctxt->type = HTML_EL_Checkbox_Input;
                              p = CSSProperties[i].parsing_function (element,
                                                                     tsch, ctxt, valueStart, css, isHTML);
                              ctxt->type = HTML_EL_Radio_Input;
                              p = CSSProperties[i].parsing_function (element,
                                                                     tsch, ctxt, valueStart, css, isHTML);
                              ctxt->type = HTML_EL_Submit_Input;
                              p = CSSProperties[i].parsing_function (element,
                                                                     tsch, ctxt, valueStart, css, isHTML);
                              ctxt->type = HTML_EL_Reset_Input;
                              p = CSSProperties[i].parsing_function (element,
                                                                     tsch, ctxt, valueStart, css, isHTML);
                              ctxt->type = HTML_EL_Button_Input;
                              p = CSSProperties[i].parsing_function (element,
                                                                     tsch, ctxt, valueStart, css, isHTML);
                              ctxt->type = HTML_EL_Input;
                            }
                          else if (ctxt->type == HTML_EL_ruby)
                            /* it's a generic rule for the HTML element ruby.
                               Generate a Thot Pres rule for each kind of
                               ruby element. */
                            {
                              ctxt->type = HTML_EL_simple_ruby;
                              p = CSSProperties[i].parsing_function (element,
                                                                     tsch, ctxt, valueStart, css, isHTML);
                              ctxt->type = HTML_EL_complex_ruby;
                              p = CSSProperties[i].parsing_function (element,
                                                                     tsch, ctxt, valueStart, css, isHTML);
                              ctxt->type = HTML_EL_ruby;
                            }
                        }
                      /* update index and skip the ";" separator if present */
                      next = SkipBlanksAndComments (p);
                      if (*next != EOS && *next != ';')
                        CSSParseError ("Missing closing ';' after", cssRule, p);
                      cssRule = next;
                    }
                }
              else
                cssRule = SkipProperty (cssRule, TRUE);
            }
          // skip important markup
          cssRule = SkipImportantRule (cssRule);

        }
      /* next property */
      cssRule = SkipBlanksAndComments (cssRule);
      if (*cssRule == '}')
        {
          cssRule++;
          CSSPrintError ("Invalid character", "}");
          cssRule = SkipBlanksAndComments (cssRule);
        }
      if (*cssRule == ',' ||
          *cssRule == ';')
        {
          cssRule++;
          cssRule = SkipBlanksAndComments (cssRule);
        }
    }

  /* restore the display mode */
  if (!DoDialog && ctxt->doc && dispMode == DisplayImmediately)
    TtaSetDisplayMode (ctxt->doc, dispMode);
}

/*----------------------------------------------------------------------
  ParseHTMLSpecificStyle: parse and apply a CSS Style string.
  This function must be called when a specific style is applied to an
  element.
  The parameter specificity is the specificity of the style, 0 if it is
  not really a CSS rule.
  ----------------------------------------------------------------------*/
void  ParseHTMLSpecificStyle (Element el, char *cssRule, Document doc,
                             int specificity, ThotBool destroy)
{
  DisplayMode         dispMode;
  PresentationContext ctxt;
  ElementType         elType;
  Element             asc;
  char               *buff, *ptr, *end;
  ThotBool            isHTML;

  /*  A rule applying to BODY is really meant to address HTML */
  elType = TtaGetElementType (el);
  NewLineSkipped = 0;
  /* store the current line for reporting errors and for displaying applied
     style rules. */
  LineNumber = TtaGetElementLineNumber (el);
  asc = el;
  /* if the element has just been created (such as a <span>) for a new style
  attribute, use the line number of its parent element, otherwise command
  "Show applied style" would not display this style. */
  while (LineNumber == 0 && asc)
    {
      asc = TtaGetParent (asc);
      if (asc)
	LineNumber = TtaGetElementLineNumber (asc);
    }
  if (destroy)
    /* no reported errors */
    ParsedDoc = 0;
  else if (ParsedDoc != doc)
    {
      /* update the context for reported errors */
      ParsedDoc = doc;
      Error_DocURL = DocumentURLs[doc];
    }
  isHTML = (strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0);
  /* create the context of the Specific presentation driver */
  ctxt = TtaGetSpecificStyleContext (doc);
  if (ctxt == NULL)
    return;
  ctxt->type = elType.ElTypeNum;
  ctxt->cssSpecificity = specificity;
  ctxt->cssLine = LineNumber;
  ctxt->destroy = destroy;
  /* first use of the context */
  ctxt->uses = 1;
  /* save the current display mode */
  dispMode = TtaGetDisplayMode (doc);
  /* Call the parser */
  DoDialog = FALSE; // not parsing for CSS dialog
  /* if it is a property applied to a COL or a COLGROUP element in a HTML table,
     associate the property to the corresponding Table_head or cell elements,
     depending on the property. */
  ptr = strstr (cssRule, "background-color");
  if (ptr && isHTML &&
      (elType.ElTypeNum == HTML_EL_COL || elType.ElTypeNum == HTML_EL_COLGROUP))
    {
      end = strstr (ptr, ";");
      if (end)
        {
          buff = TtaStrdup (ptr);
          end = strstr (buff, ";");
          *end = EOS;
          ColApplyCSSRule (el, (PresentationContext) ctxt, buff, NULL);
          TtaFreeMemory (buff);
        }
      else
        ColApplyCSSRule (el, (PresentationContext) ctxt, ptr, NULL);
    }
  else
    ParseCSSRule (el, NULL, ctxt, cssRule, NULL, isHTML);

  /* restore the display mode if necessary */
  if (dispMode != NoComputedDisplay)
    TtaSetDisplayMode (doc, dispMode);
  /* check if the context can be freed */
  ctxt->uses -= 1;
  if (ctxt->uses == 0)
    /* no image loading */
    TtaFreeMemory(ctxt);
}


/*----------------------------------------------------------------------
  AddClassName adds the class name into the class list of css if it's
  not already there.
  ----------------------------------------------------------------------*/
static void AddClassName (char *name, CSSInfoPtr css)
{
  int		         l, index, k, length, add;
  char          *buf;
  ThotBool       found, previous;

  l = strlen (name);
  if (l == 0 || css == NULL)
    return;
  if (css->class_list)
    {
      buf = css->class_list;
      length = strlen (css->class_list);
    }
  else
    {
      if (l > 200)
        length = l + 1;
      else
        length = 200;
      buf = (char *)TtaGetMemory (length * sizeof (char));
      memset (buf, 0, length);
      css->class_list = buf;
      css->lg_class_list = length;
      length = 0;
    }

  /* compare that name with all class names already known */
  index = 0;
  found = FALSE;
  previous = FALSE;
  while (index < length && !found && !previous)
    {
      k = 0;
      while (k < l && buf[index + k] != EOS && buf[index + k] != SPACE)
        {
          if (name[k] == buf[index+k])
            k++;
          else
            {
              previous = (name[k] < buf[index + k]);
              break;
            }
        }
      found = (k == l);
      if (!previous)
        {
          index += k;
          while (buf[index] != EOS && buf[index] != SPACE)
            index++;
          if (buf[index] == SPACE)
            index++;
        }
    }
  
  if (!found)
    /* this class name is not known, append it */
    {
      l++; /* add a space before */
      if (css->lg_class_list <= length + l)
        {
          // increase the list size
          if (l > 200)
            add = l + 1;
          else
            add = 200 ;
          buf = (char *)TtaRealloc (buf, css->lg_class_list + (add * sizeof (char)));
          if (buf == NULL)
            return;
          else
            {
            css->class_list = buf;
            memset (&buf[css->lg_class_list], 0, add);
            css->lg_class_list += add;
            }
        }

      if (previous)
        {
          // move the tail of the current list
          for (k = length; k >= index; k--)
            buf[k+l] = buf[k];
          /* add this new class name at the current position */
           strcpy (&buf[index], name);
          buf[index + l - 1] = SPACE;
        }
      else
        {
          /* add this new class name at the end */
          if (index != 0)
            buf[index++] = SPACE;
          strcpy (&buf[index], name);
        }
     }
}


/*----------------------------------------------------------------------
  ParseGenericSelector: Create a generic context for a given selector
  string.
  If the selector is made of multiple comma, it parses them one at a time
  and return the end of the selector string to be handled or NULL.
  The parameter ctxt gives the current style context which will be passed
  to Thotlib.
  The parameter css points to the current CSS context.
  The parameter link points to the link element.
  The parameter url gives the URL of the parsed style sheet.
  ----------------------------------------------------------------------*/
static char *ParseGenericSelector (char *selector, char *cssRule,
                                   GenericContext ctxt, Document doc,
                                   CSSInfoPtr css, Element link, char *url)
{
  ElementType        elType;
  PSchema            tsch;
  AttributeType      attrType;
  char              *deb, *cur, *sel, *next, *limit, c;
  char              *schemaName, *mappedName, *saveURL;
  char              *names[MAX_ANCESTORS];
  ThotBool           pseudoFirstChild[MAX_ANCESTORS];
  ElemRel            rel[MAX_ANCESTORS];
  char              *attrnames[MAX_ANCESTORS];
  int                attrnums[MAX_ANCESTORS];
  int                attrlevels[MAX_ANCESTORS];
  char              *attrvals[MAX_ANCESTORS];
  AttrMatch          attrmatch[MAX_ANCESTORS];
  int                nbnames, nbattrs;
  int                i, j;
  int                att, kind;
  int                specificity, xmlType;
  int                skippedNL;
  ThotBool           isHTML, noname, warn;
  ThotBool           level, quoted, doubleColon;
#define ATTR_ID 1
#define ATTR_CLASS 2
#define ATTR_PSEUDO 3

  // check if Amaya should report CSS warnings
  TtaGetEnvBoolean ("CSS_WARN", &warn);
  sel = ctxt->sel;
  sel[0] = EOS;
  // get the limit of the string
  limit = &sel[MAX_ANCESTORS * 50 -1];
  *limit = EOS;
  specificity = 0;
  for (i = 0; i < MAX_ANCESTORS; i++)
    {
      names[i] = NULL;
      pseudoFirstChild[i] = FALSE;
      rel[i] = RelAncestor;
      attrnames[i] = NULL;
      attrnums[i] = 0;
      attrlevels[i] = 0;
      attrvals[i] = NULL;
      attrmatch[i] = Txtmatch;
      ctxt->name[i] = 0;
      ctxt->firstChild[i] = FALSE;
      ctxt->attrType[i] = 0;
      ctxt->attrLevel[i] = 0;
      ctxt->attrText[i] = NULL;
      ctxt->attrMatch[i] = Txtmatch;
    }
  ctxt->box = 0;
  ctxt->var = 0;
  ctxt->pseudo = PbNone;
  ctxt->type = 0;
  DoDialog = FALSE; // not arsing for CSS dialog
  /* the specificity of the rule depends on the selector */
  ctxt->cssSpecificity = 0;
  /* localisation of the CSS rule */
  ctxt->cssLine = LineNumber + NewLineSkipped;
  ctxt->cssURL = url;

  skippedNL = NewLineSkipped;
  selector = SkipBlanksAndComments (selector);
  NewLineSkipped = skippedNL;
  cur = &sel[0];
  nbnames = 0;
  nbattrs = 0;
  while (1)
    {
      /* point to the following word in sel[] */
      deb = cur;
      /* copy an item of the selector into sel[] */
      /* put one word in the sel buffer */
      while (*selector != EOS && *selector != ',' &&
             *selector != '.' && *selector != ':' &&
             *selector != '#' && *selector != '[' &&
             *selector != '>' && *selector != '+' &&
             !TtaIsBlank (selector) && cur < limit)
        *cur++ = *selector++;
      *cur++ = EOS; /* close the first string  in sel[] */
      noname = TRUE;
      if (deb[0] != EOS)
        /* the selector starts with an element name */
        {
          if (deb[0] <= 64 && deb[0] != '*')
            {
              CSSPrintError ("Invalid element", deb);
              names[0] = NULL; /* no element name */
              DoApply = FALSE;
            }
          else
            {
              noname = FALSE;
              names[0] = deb;
              if (!strcmp (names[0], "html"))
                /* give a greater priority to the backgoud color of html */
                specificity += 3;
              else
                /* selector "*" has specificity zero */
                if (strcmp (names[0], "*"))
                  specificity += 1;
            }
        }
      else
        names[0] = NULL; /* no element name */

      rel[0] = RelVoid;
      /* now names[0] points to the beginning of the parsed item
         and cur to the next string to be parsed */
      while (*selector == '.' || *selector == ':' ||
             *selector == '#' || *selector == '[')
        {
          /* point to the following word in sel[] */
          deb = cur;
          if (*selector == '.')
            /* class */
            {
              selector++;
              while (*selector != '.' && *selector != ':' &&
                     *selector != '#' && *selector != '[' &&
                     *selector != EOS && *selector != ',' &&
                     *selector != '+' && *selector != '>' &&
                     !TtaIsBlank (selector) && cur < limit)
                {
                  if (*selector == '\\')
                    {
                      selector++;
                      if (*selector != EOS)
                        *cur++ = *selector++;
                    }
                  else
                    *cur++ = *selector++;
                }
              /* close the word */
              *cur++ = EOS;
              /* point to the class in sel[] if it's a valid name */
              if (deb[0] <= 64)
                {
                  CSSPrintError ("Invalid class", deb);
                  DoApply = FALSE;
                }
              else
                {
                  /* simulate selector [class ~= "xxx"] */
                  nbattrs++;
                  if (nbattrs == MAX_ANCESTORS)
                    /* abort parsing */
                    {
                      CSSPrintError ("Selector too long", deb);
                      return (selector);
                    }
                  for (i = nbattrs; i > 0; i--)
                    {
                      attrnames[i] = attrnames[i - 1];
                      attrnums[i] = attrnums[i - 1];
                      attrlevels[i] = attrlevels[i - 1];
                      attrvals[i] = attrvals[i - 1];
                      attrmatch[i] = attrmatch[i - 1];
                    }
                  attrnames[0] = NULL;
                  attrnums[0] = ATTR_CLASS;
                  attrlevels[0] = 0;
                  attrmatch[0] = Txtword;
                  attrvals[0] = deb;
                  specificity += 10;
                 }
            }
          else if (*selector == ':')
            /* pseudo-class or pseudo-element */
            {
              selector++;
              doubleColon = FALSE;
              if (*selector == ':')
                /* it's a double "::". Probably CSS3 syntax */
                {
                  selector++;
                  doubleColon = TRUE;
                }
              while (*selector != '.' && *selector != ':' &&
                     *selector != '#' && *selector != '[' &&
                     *selector != EOS && *selector != ',' &&
                     *selector != '+' && *selector != '>' &&
                     !TtaIsBlank (selector) && cur < limit)
                *cur++ = *selector++;
              /* close the word */
              *cur++ = EOS;
              /* point to the pseudo-class or pseudo-element in sel[] if it's
                 a valid name */
              if (!strcmp (deb, "first-child"))
                /* first-child pseudo-class */
                {
                  pseudoFirstChild[0] = TRUE;
                  specificity += 10;
                }
              else if (!strcmp (deb, "link") || !strcmp (deb, "visited"))
                /* link or visited pseudo-classes */
                {
                  nbattrs++;
                  if (nbattrs == MAX_ANCESTORS)
                    /* abort parsing */
                    {
                      CSSPrintError ("Selector too long", deb);
                      return (selector);
                    }
                  for (i = nbattrs; i > 0; i--)
                    {
                      attrnames[i] = attrnames[i - 1];
                      attrnums[i] = attrnums[i - 1];
                      attrlevels[i] = attrlevels[i - 1];
                      attrvals[i] = attrvals[i - 1];
                      attrmatch[i] = attrmatch[i - 1];
                    }
                  attrnames[0] = NULL;
                  attrnums[0] = ATTR_PSEUDO;
                  attrlevels[0] = 0;
                  attrmatch[0] = Txtmatch;
                  attrvals[0] = deb;
                  specificity += 10;
                }
              else if (!strcmp (deb, "hover") || !strcmp (deb, "active") ||
                       !strcmp (deb, "focus"))
                /* hover, active, focus pseudo-classes */
                {
                  attrnames[0] = NULL;
                  attrnums[0] = ATTR_PSEUDO;
                  attrlevels[0] = 0;
                  attrmatch[0] = Txtmatch;
                  attrvals[0] = deb;
                  specificity += 10;
                  /* not supported */
                  DoApply = FALSE;
                }
              else if (!strncmp (deb, "lang", 4))
                /* it's the lang pseudo-class */
                {
                  if (deb[4] != '(' || deb[strlen(deb)-1] != ')')
                    /* at least one parenthesis is missing. Error */
                    {
                      CSSPrintError ("Invalid :lang pseudo-class", deb);
                      DoApply = FALSE;
                    }
                  else
                    /* simulate selector [lang|="xxx"] */
                    {
                      nbattrs++;
                      if (nbattrs == MAX_ANCESTORS)
                        /* abort parsing */
                        {
                          CSSPrintError ("Selector too long", deb);
                          return (selector);
                        }
                      deb[strlen(deb)-1] = EOS;
                      deb[4] = EOS;
                      for (i = nbattrs; i > 0; i--)
                        {
                          attrnames[i] = attrnames[i - 1];
                          attrnums[i] = attrnums[i - 1];
                          attrlevels[i] = attrlevels[i - 1];
                          attrvals[i] = attrvals[i - 1];
                          attrmatch[i] = attrmatch[i - 1];
                        }
                      attrnames[0] = deb;
                      attrnums[0] = 0;
                      attrlevels[0] = 0;
                      attrmatch[0] = Txtsubstring;
                      attrvals[0] = &deb[5];
                      specificity += 10;
                    }
                }
              else if (!strcmp (deb, "first-line") ||
                       !strcmp (deb, "first-letter"))
                /* pseudo-elements first-line or first-letter */
                {
                  if (doubleColon && warn)
                    CSSPrintError ("Warning: \"::\" is CSS3 syntax", NULL);
                  specificity += 1;
                  /* not supported */
                  DoApply = FALSE;
                }
              else if (!strncmp (deb, "before", 6))
                /* pseudo-element before */
                {
                  if (doubleColon && warn)
                    CSSPrintError ("Warning: \"::before\" is CSS3 syntax",
                                   NULL);
                  ctxt->pseudo = PbBefore;
                  specificity += 1;
                }
              else if (!strncmp (deb, "after", 5))
                /* pseudo-element after */
                {
                  if (doubleColon && warn)
                    CSSPrintError ("Warning: \"::after\" is CSS3 syntax",
                                   NULL);
                  ctxt->pseudo = PbAfter;
                  specificity += 1;
                }
              else if (!strncmp (deb, "target", 6))
                {
                  if (warn)
                   CSSPrintError ("Warning: \":target\" is CSS3 syntax",
                                   NULL);
                  specificity += 1;
                  DoApply = FALSE;
                }
              else if (!strncmp (deb, "not", 3) ||
                       !strncmp (deb, "only", 4) ||
                       !strncmp (deb, "last", 4))
                {
                  if (warn)
                   CSSPrintError ("Warning: Not supported CSS3 syntax",
                                   NULL);
                  specificity += 1;
                  DoApply = FALSE;
                }
              else
                {
                  CSSPrintError ("Invalid pseudo-element", deb);
                  DoApply = FALSE;
                }
              if (names[0] && !strcmp (names[0], "*"))
                names[0] = NULL;
            }
          else if (*selector == '#')
            /* unique identifier */
            {
              selector++;
              while (*selector != '.' && *selector != ':' &&
                     *selector != '#' && *selector != '[' &&
                     *selector != '+' && *selector != '>' &&
                     *selector != EOS && *selector != ',' &&
                     !TtaIsBlank (selector) && cur < limit)
                *cur++ = *selector++;
              /* close the word */
              *cur++ = EOS;
              /* point to the attribute in sel[] if it's valid name */
              if (deb[0] <= 64)
                {
                  CSSPrintError ("Invalid id", deb);
                  DoApply = FALSE;
                }
              else
                {
                  nbattrs++;
                  if (nbattrs == MAX_ANCESTORS)
                    /* abort parsing */
                    {
                      CSSPrintError ("Selector too long", deb);
                      return (selector);
                    }
                  for (i = nbattrs; i > 0; i--)
                    {
                      attrnames[i] = attrnames[i - 1];
                      attrnums[i] = attrnums[i - 1];
                      attrlevels[i] = attrlevels[i - 1];
                      attrvals[i] = attrvals[i - 1];
                      attrmatch[i] = attrmatch[i - 1];
                    }
                  attrnames[0] = NULL;
                  attrnums[0] = ATTR_ID;
                  attrlevels[0] = 0;
                  attrmatch[0] = Txtmatch;
                  attrvals[0] = deb;
                  specificity += 100;
                  if (names[0] && !strcmp (names[0], "*"))
                    names[0] = NULL;
                }
            }
          else if (*selector == '[')
            {
              selector++;
              selector = SkipBlanksAndComments (selector);
              while (*selector != EOS && *selector != ']' &&
                     *selector != '=' && *selector != '~' &&
                     *selector != '|' && *selector != '^' &&
                      *selector != '$' &&  *selector != '*' &&
                     !TtaIsBlank (selector) && cur < limit)
                *cur++ = *selector++;
              /* close the word (attribute name) */
              *cur++ = EOS;
              /* point to the attribute in sel[] if it's valid name */
              if (deb[0] <= 64)
                {
                  CSSPrintError ("Invalid attribute", deb);
                  DoApply = FALSE;
                }
              else
                {
                  nbattrs++;
                  if (nbattrs == MAX_ANCESTORS)
                    /* abort parsing */
                    {
                      CSSPrintError ("Selector too long", deb);
                      return (selector);
                    }
                  for (i = nbattrs; i > 0; i--)
                    {
                      attrnames[i] = attrnames[i - 1];
                      attrnums[i] = attrnums[i - 1];
                      attrlevels[i] = attrlevels[i - 1];
                      attrvals[i] = attrvals[i - 1];
                      attrmatch[i] = attrmatch[i - 1];
                    }
                  attrnames[0] = deb;
                  attrnums[0] = 0;
                  attrlevels[0] = 0;
                  attrvals[0] = NULL;
                  attrmatch[0] = Txtmatch;
                  specificity += 10;
                  /* check matching */
                  selector = SkipBlanksAndComments (selector);
                  if (*selector == '~')
                    {
                      attrmatch[0] = Txtword;
                      selector++;
                    }
                  else if (*selector == '|' || *selector == '$' || *selector == '*')
                    {
                      if (*selector == '$' && warn)
                        CSSPrintError ("Warning: \"$=\" is CSS3 syntax", NULL);
                      if (*selector == '*' && warn)
                        CSSPrintError ("Warning: \"*=\" is CSS3 syntax", NULL);
                      attrmatch[0] = Txtsubstring;
                      selector++;
                    }
                  else if (*selector == '^')
                    {
                      attrmatch[0] = Txtsubstring;
                      selector++;
                    }
                  else
                    attrmatch[0] = Txtmatch;
                }
              if (*selector == '=')
                {
                  /* look for a value "xxxx" */
                  selector++;
                  selector = SkipBlanksAndComments (selector);
                  if (*selector != '"')
                    quoted = FALSE;
                  else
                    {
                      quoted = TRUE;
                      /* we are now parsing the attribute value */
                      selector++;
                    }
                  deb = cur;
                  while ((quoted && cur < limit &&
                          (*selector != '"' ||
                           (*selector == '"' && selector[-1] == '\\'))) ||
                         (!quoted && *selector != ']'))
                    {
                      if (*selector == EOS)
                        {
                          CSSPrintError ("Invalid attribute value", deb);
                          DoApply = FALSE;
                        }
                      else
                        {
                          if (attrmatch[0] == Txtword && TtaIsBlank (selector))
                            {
                              CSSPrintError ("No space allowed here: ", selector);
                              DoApply = FALSE;
                            }
                          *cur++ = *selector;
                        }
                      selector++;
                    }
                  /* there is a value */
                  if (quoted && *selector == '"')
                    {
                      selector++;
                      quoted = FALSE;
                    }
                  selector = SkipBlanksAndComments (selector);
                  if (*selector != ']')
                    {
                      CSSPrintError ("Invalid attribute value", deb);
                      DoApply = FALSE;
                    }
                  else
                    {
                      *cur++ = EOS;
                      attrvals[0] = deb;
                      selector++;
                    }
                }
              /* end of the attribute */
              else if (*selector != ']')
                {
                  selector[1] = EOS;
                  CSSPrintError ("Invalid attribute", selector);
                  selector += 2;
                  DoApply = FALSE;
                }
              else
                {
                  selector++;
                  if (names[0] && !strcmp (names[0], "*"))
                    names[0] = NULL;
                }
            }
          else
            {
              /* not supported selector */
              while (*selector != '.' && *selector != ':' &&
                     *selector != '#' && *selector != '[' &&
                     *selector != EOS && *selector != ',' &&
                     *selector != '+' && *selector != '>' &&
                     !TtaIsBlank (selector) && cur < limit)
                *cur++ = *selector++;
              /* close the word */
              *cur++ = EOS;
              CSSPrintError ("Selector not supported:", deb);
              DoApply = FALSE;	    
            }
        }

      skippedNL = NewLineSkipped;
      selector = SkipBlanksAndComments (selector);
      NewLineSkipped = skippedNL;

      if (noname && !pseudoFirstChild[0] && attrnums[0] == 0 && attrnames[0] == NULL)
        {
          *cur++ = EOS;
          CSSPrintError ("Invalid Selector", deb);
          DoApply = FALSE;	    
        }
      /* is it a multi-level selector? */
      if (*selector == EOS)
        /* end of the selector */
        break;
      else if (*selector == ',')
        {
          /* end of the current selector */
          selector++;
          skippedNL = NewLineSkipped;
          next = SkipBlanksAndComments (selector);
          NewLineSkipped = skippedNL;
          if (*next == EOS)
            /* nothing after the comma. Invalid selector */
            {
              CSSPrintError ("Syntax error:", selector);
              selector = NULL;
            }
          break;
        }
      else
        {
          if (*selector == '>')
            {
              /* handle parent */
              selector++;
              skippedNL = NewLineSkipped;
              selector = SkipBlanksAndComments (selector);
              NewLineSkipped = skippedNL;
              rel[0] = RelParent;
            }
          else if (*selector == '+')
            {
              /* handle immediate sibling */
              selector++;
              skippedNL = NewLineSkipped;
              selector = SkipBlanksAndComments (selector);
              NewLineSkipped = skippedNL;
              rel[0] = RelPrevious;
            }
          else
            rel[0] = RelAncestor;
          nbnames++; /* a new level in ancestor tables */
          if (nbnames == MAX_ANCESTORS)
            /* abort parsing */
            {
              CSSPrintError ("Selector too long", deb);
              return (selector);
            }
          /* shift the list to make room for the next part of the selector */
          for (i = nbnames; i > 0; i--)
            {
              names[i] = names[i - 1];
              pseudoFirstChild[i] = pseudoFirstChild[i - 1];
              rel[i] = rel[i - 1];
            }
          /* increase the level of all attributes */
          for (i = 0; i < nbattrs; i++)
              attrlevels[i]++;
        }
    }

  /* Now update the list of classes defined by the CSS */
  for (i = 0; i < nbattrs; i++)
    if (attrvals[i] && attrnums[i] == ATTR_CLASS)
      AddClassName (attrvals[i], css);

  /* Now set up the context block */
  i = 0;
  j = 0;
  /* default schema name */
  ctxt->schema = NULL;
  ctxt->nbElem = nbnames;
  elType.ElSSchema = NULL;
  elType.ElTypeNum = 0;
  schemaName = TtaGetSSchemaName(TtaGetDocumentSSchema (doc));
  if (!strcmp (schemaName, "HTML"))
    xmlType = XHTML_TYPE;
  else if (!strcmp (schemaName, "MathML"))
    xmlType = MATH_TYPE;
  else if (!strcmp (schemaName, "SVG"))
    xmlType = SVG_TYPE;
  else if (!strcmp (schemaName, "XLink"))
    xmlType = XLINK_TYPE;
  else if (!strcmp (schemaName, "Annot"))
    xmlType = ANNOT_TYPE;
  else
    xmlType = XML_TYPE;
  while (i <= nbnames)
    {
      ctxt->rel[i] = rel[i];
      ctxt->firstChild[i] = pseudoFirstChild[i];
      if (!names[i] && i > 0)
        ctxt->name[i] = HTML_EL_ANY_TYPE;
      else
        /* store element information */
        {
          /* get the element type of this name in the current document */
          if (xmlType == XML_TYPE)
            /* it's a generic XML document. Check the main document schema */
            {
              elType.ElSSchema = TtaGetDocumentSSchema (doc);
              elType.ElTypeNum = 0;
              if (names[i])
                TtaGetXmlElementType (names[i], &elType, &mappedName, doc);
              if (!elType.ElTypeNum)
                {
                  if (!names[i] || !strcmp (names[i], "*"))
                    elType.ElTypeNum = HTML_EL_ANY_TYPE;
                  else
                    elType.ElSSchema = NULL;
                }
            }
          else
            {
              if (!names[i] || !strcmp (names[i], "*"))
                {
                  elType.ElSSchema = TtaGetDocumentSSchema (doc);
                  elType.ElTypeNum = HTML_EL_ANY_TYPE;
                }
              else
                MapXMLElementType (xmlType, names[i], &elType, &mappedName, &c,
                                   &level, doc);
            }
          if (i == 0)
            /* rightmost part of the selector */
            {
              if (elType.ElSSchema == NULL)
                {
                  /* element name not found. Search in all loaded schemas */
                  if (names[i])
                    TtaGetXmlElementType (names[i], &elType, NULL, doc);
                  if (elType.ElSSchema)
                    {
                      /* the element type concerns an imported nature */
                      schemaName = TtaGetSSchemaName(elType.ElSSchema);
                      if (!strcmp (schemaName, "HTML"))
                        {
                          if (xmlType == XHTML_TYPE &&
                              DocumentMeta[doc] && DocumentMeta[doc]->xmlformat)
                            /* the selector was found but the case is not correct */
                            elType.ElSSchema = NULL;
                          else
                            xmlType = XHTML_TYPE;
                        }
                      else if (!strcmp (schemaName, "MathML"))
                        xmlType = MATH_TYPE;
                      else if (!strcmp (schemaName, "SVG"))
                        xmlType = SVG_TYPE;
                      else if (!strcmp (schemaName, "XLink"))
                        xmlType = XLINK_TYPE;
                      else if (!strcmp (schemaName, "Annot"))
                        xmlType = ANNOT_TYPE;
                      else
                        xmlType = XML_TYPE;
                    }
#ifdef XML_GENERIC
                  else if (xmlType == XML_TYPE)
                    {
                      /* Creation of a new element type in the main schema */
                      elType.ElSSchema = TtaGetDocumentSSchema (doc);
                      if (names[i])
                        TtaAppendXmlElement (names[i], &elType, &mappedName,
                                             doc);
                    }
#endif /* XML_GENERIC */
                  else
                    {
                      if (xmlType != XHTML_TYPE)
                        {
                          MapXMLElementType (XHTML_TYPE, names[i], &elType,
                                             &mappedName, &c, &level, doc);
                          if (elType.ElSSchema)
                            elType.ElSSchema = GetXHTMLSSchema (doc);
                        }
                      if (elType.ElSSchema == NULL && xmlType != MATH_TYPE)
                        {
                          MapXMLElementType (MATH_TYPE, names[i], &elType,
                                             &mappedName, &c, &level, doc);
                          if (elType.ElSSchema)
                            elType.ElSSchema = GetMathMLSSchema (doc);
                        }
                      if (elType.ElSSchema == NULL && xmlType != SVG_TYPE)
                        {
                          MapXMLElementType (SVG_TYPE, names[i], &elType,
                                             &mappedName, &c, &level, doc);
                          if (elType.ElSSchema)
                            elType.ElSSchema = GetSVGSSchema (doc);
                        }
                    }
                }

              if (elType.ElSSchema == NULL)
                /* cannot apply these CSS rules */
                DoApply = FALSE;
              else
                {
                  /* Store the element type contained in the rightmost part of
                     the selector */
                  ctxt->schema = elType.ElSSchema;
                  ctxt->type = elType.ElTypeNum;
                  ctxt->name[0] = elType.ElTypeNum;
                  ctxt->rel[0] = RelVoid;
                }
            }
          else
            /* not the rightmost part of the selector */
            {
              if (elType.ElTypeNum != 0)
                ctxt->name[i] = elType.ElTypeNum;
#ifdef XML_GENERIC
              else if (xmlType == XML_TYPE)
                {
                  TtaGetXmlElementType (names[i], &elType, NULL, doc);
                  if (elType.ElTypeNum == 0)
                    {
                      /* Creation of a new element type in the main schema */
                      elType.ElSSchema = TtaGetDocumentSSchema (doc);
                      TtaAppendXmlElement (names[i], &elType, &mappedName, doc);
                    }
                  if (elType.ElTypeNum != 0)
                    ctxt->name[i] = elType.ElTypeNum;
                }
#endif /* XML_GENERIC */
            }
        }

      /* store attribute information for this element */
      while (j < nbattrs && attrlevels[j] <= i)
        {
          if (attrnames[j] || attrnums[j])
            {
              if (attrnums[j] > 0)
                {
                  if (attrnums[j] == ATTR_CLASS)
                    {
                      if (xmlType == SVG_TYPE)
                        ctxt->attrType[j] = SVG_ATTR_class;
                      else if (xmlType == MATH_TYPE)
                        ctxt->attrType[j] = MathML_ATTR_class;
                      else if (xmlType == XHTML_TYPE)
                        ctxt->attrType[j] = HTML_ATTR_Class;
                      else
#ifdef XML_GENERIC
                        ctxt->attrType[j] = XML_ATTR_class;
#else /* XML_GENERIC */
                        ctxt->attrType[j] = HTML_ATTR_Class;
#endif /* XML_GENERIC */
                    }
                  else if (attrnums[j] == ATTR_PSEUDO)
                    {
                      if (xmlType == SVG_TYPE)
                        ctxt->attrType[j] = SVG_ATTR_PseudoClass;
                      else if (xmlType == MATH_TYPE)
                        ctxt->attrType[j] = MathML_ATTR_PseudoClass;
                      else if (xmlType == XHTML_TYPE)
                        ctxt->attrType[j] = HTML_ATTR_PseudoClass;
                      else
#ifdef XML_GENERIC
                        ctxt->attrType[j] = XML_ATTR_PseudoClass;
#else /* XML_GENERIC */
                        ctxt->attrType[j] = HTML_ATTR_PseudoClass;
#endif /* XML_GENERIC */
                    }
                  else if (attrnums[j] == ATTR_ID)
                    {
                      if (xmlType == SVG_TYPE)
                        ctxt->attrType[j] = SVG_ATTR_id;
                      else if (xmlType == MATH_TYPE)
                        ctxt->attrType[j] = MathML_ATTR_id;
                      else if (xmlType == XHTML_TYPE)
                        ctxt->attrType[j] = HTML_ATTR_ID;
                      else
#ifdef XML_GENERIC
                        ctxt->attrType[j] = XML_ATTR_xmlid;
#else /* XML_GENERIC */
                        ctxt->attrType[j] = HTML_ATTR_ID;
#endif /* XML_GENERIC */
                    }
                  attrType.AttrTypeNum = ctxt->attrType[j];
                  attrType.AttrSSchema =  ctxt->schema;
                }
              else if (attrnames[j])
                {
                  if (xmlType == XML_TYPE)
                    {
                      if (ctxt->schema)
                        attrType.AttrSSchema = ctxt->schema;
                      else
                        attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
                      TtaGetXmlAttributeType (attrnames[j], &attrType, doc);
                      att = attrType.AttrTypeNum;
                      if (ctxt->schema == NULL && att != 0)
                        ctxt->schema = attrType.AttrSSchema;
                    }
                  else
                    {
                      MapXMLAttribute (xmlType, attrnames[j], names[i], &level,
                                       doc, &att);
                      if (ctxt->schema == NULL && att != 0)
                        ctxt->schema = TtaGetDocumentSSchema (doc);
                    }
                  if (att == 0 && xmlType != XML_TYPE)
                    /* Attribute name not found: Search in the list of all
                       schemas loaded for this document */
                    {
                      attrType.AttrSSchema = NULL;
                      TtaGetXmlAttributeType (attrnames[j], &attrType, doc);
                      att = attrType.AttrTypeNum;
                      if (att != 0)
                        {
                          ctxt->schema = attrType.AttrSSchema;
                          schemaName = TtaGetSSchemaName(attrType.AttrSSchema);
                        }
                    }
                  attrType.AttrSSchema = ctxt->schema;
                  attrType.AttrTypeNum = att;
                  if ((i == 0 || xmlType == XML_TYPE) && att == 0)
                    {
#ifdef XML_GENERIC
                      if (xmlType == XML_TYPE)
                        {
                          /* The attribute is not yet present in the tree */
                          /* Create a new global attribute */
                          attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
                          TtaAppendXmlAttribute (attrnames[j], &attrType, doc);
                          att = attrType.AttrTypeNum;
                        }
#endif /* XML_GENERIC */
                      if (attrType.AttrSSchema == NULL)
                        /* cannot apply these CSS rules */
                        DoApply = FALSE;
                      else if (elType.ElSSchema)
                        ctxt->schema = elType.ElSSchema;
                      else
                        ctxt->schema = attrType.AttrSSchema;
                    }
                  if (att == 0)
                    {
                      CSSPrintError ("Unknown attribute", attrnames[j]);
                      DoApply = FALSE;	    
                    }
                  else
                    {
                      ctxt->attrType[j] = att;
                      if (att == DummyAttribute && !strcmp (schemaName,"HTML"))
                        /* it's the "type" attribute for an "input" element.
                           In the tree, it is represented by the element type,
                           not by an attribute */
                        {
                          ctxt->attrType[j] = 0;
                          if (attrvals[j] && attrmatch[i] == Txtmatch)
                            /* a value is specified for attribute type. This
                               value provides the Thot element type */
                            {
                              MapXMLAttributeValue (xmlType, attrvals[j],
                                                    &attrType, &kind);
                              /* attrType contains the element type */
                              if (i == 0)
                                ctxt->type = kind;
                              ctxt->name[i] = kind;
                            } 
                        }
                    }
                }
              if (ctxt->attrType[j])
                {
                  /* check the attribute type */
                  if (!strcmp (schemaName, "HTML"))
                    xmlType = XHTML_TYPE;
                  else if (!strcmp (schemaName, "MathML"))
                    xmlType = MATH_TYPE;
                  else if (!strcmp (schemaName, "SVG"))
                    xmlType = SVG_TYPE;
                  else if (!strcmp (schemaName, "XLink"))
                    xmlType = XLINK_TYPE;
                  else if (!strcmp (schemaName, "Annot"))
                    xmlType = ANNOT_TYPE;
                  else
                    xmlType = XML_TYPE;
                  kind = TtaGetAttributeKind (attrType);
                  if (kind == 0 && attrvals[j])
                    {
                      /* enumerated value */
                      MapXMLAttributeValue (xmlType, attrvals[j], &attrType,
                                            &kind);
                      /* store the attribute value */
                      ctxt->attrText[j] = (char *) kind;
                    }
                  else
                    ctxt->attrText[j] = attrvals[j];
                  /* update attrLevel */
                  ctxt->attrMatch[j] = attrmatch[j];
                  ctxt->attrLevel[j] = attrlevels[j];
                    }
              j++;
            }
        }
      /* add a new entry */
      i++;
      if (i == 1 && ctxt->schema == NULL)
        /* use the document schema */
        ctxt->schema = TtaGetDocumentSSchema (doc);
    }

  ctxt->important = FALSE;
  /* set the selector specificity */
  ctxt->cssSpecificity = specificity;
  /* Get the schema name of the main element */
  schemaName = TtaGetSSchemaName (ctxt->schema);
  isHTML = (strcmp (schemaName, "HTML") == 0);
  tsch = GetPExtension (doc, ctxt->schema, css, link);
  skippedNL = NewLineSkipped;
  if (DoApply && tsch && cssRule)
    {
      if (css)
        {
          /* point the right URL for loaded images */
          saveURL = css->url;
          css->url = url;
        }
      else
        saveURL = NULL;
      ParseCSSRule (NULL, tsch, (PresentationContext) ctxt, cssRule, css, isHTML);
      if (css)
        /* restore previous url */
        css->url = saveURL;
    }
  /* future CSS rules should apply */
  DoApply = TRUE;
  if (selector)
    NewLineSkipped = skippedNL;
  return (selector);
}

/*----------------------------------------------------------------------
  ParseStyleDeclaration: parse a style declaration stored in the style
  element of a document                       
  We expect the style string to be of the form:                   
  .pinky, .awful { color: pink; font-family: helvetica }        
  The parameter css points to the current CSS context.
  The parameter link points to the link element.
  The parameter url gives the URL of the parsed style sheet.
  ----------------------------------------------------------------------*/
static void ParseStyleDeclaration (Element el, char *cssRule, Document doc,
                                   CSSInfoPtr css, Element link, char *url,
                                   ThotBool destroy)
{
  GenericContext      ctxt;
  char               *decl_end;
  char               *sel_end;
  char               *selector;

  /* separate the selectors string */
  cssRule = SkipBlanksAndComments (cssRule);
  decl_end = cssRule;
  while (*decl_end != EOS && *decl_end != '{')
    {
      if (*decl_end == EOL)
        NewLineSkipped++;
      decl_end++;
    }
  if (*decl_end == EOS)
    {
      CSSPrintError ("Invalid selector", cssRule);
      return;
    }
  /* verify and clean the selector string */
  sel_end = decl_end - 1;
  while (*sel_end == SPACE || *sel_end == BSPACE ||
         *sel_end == EOL || *sel_end == __CR__)
    sel_end--;
  sel_end++;
  *sel_end = EOS;
  selector = cssRule;

  /* now, deal with the content ... */
  decl_end++;
  cssRule = decl_end;
  decl_end = &cssRule[strlen (cssRule) - 1];
  if (*decl_end != '{' && *decl_end != EOS)
    *decl_end = EOS;
  /*
   * parse the style attribute string and install the corresponding
   * presentation attributes on the new element
   */
  ctxt = TtaGetGenericStyleContext (doc);
  if (ctxt == NULL)
    return;
  ctxt->destroy = destroy;
  /* first use of the context */
  ctxt->uses = 1;
  while (selector && *selector != EOS)
    {
      if (ctxt->uses > 1)
        {
          /* this context is waiting for a callback */
          ctxt = TtaGetGenericStyleContext (doc);
          if (ctxt == NULL)
            return;
          ctxt->destroy = destroy;
          /* first use of the context */
          ctxt->uses = 1; 
        }
      selector = ParseGenericSelector (selector, cssRule, ctxt, doc, css,
                                       link, url);
    }
  /* check if the context can be freed */
  ctxt->uses -= 1;
  if (ctxt->uses == 0)
    /* no image loading */
    TtaFreeMemory (ctxt);
}

/************************************************************************
 *									*  
 *	EVALUATION FUNCTIONS / CASCADING AND OVERLOADING		*
 *									*  
 ************************************************************************/

/*----------------------------------------------------------------------
  IsImplicitClassName: return wether the Class name is an        
  implicit one, eg "H1" or "H2 EM" meaning it's a GI name       
  or an HTML context name.                                      
  ----------------------------------------------------------------------*/
int IsImplicitClassName (char *class_, Document doc)
{
  char         name[200];
  char        *cur = name;
  char        *first; 
  char         save;
  SSchema      schema;

  /* make a local copy */
  strncpy (name, class_, 199);
  name[199] = 0;

  /* loop looking if each word is a GI */
  while (*cur != 0)
    {
      first = cur;
      cur = SkipWord (cur);
      save = *cur;
      *cur = 0;
      schema = NULL;
      if (MapGI (first, &schema, doc) == -1)
        {
          return (0);
        }
      *cur = save;
      cur = SkipBlanksAndComments (cur);
    }
  return (1);
}

/************************************************************************
 *  Functions needed for support of HTML: translate to CSS equivalent   *
 ************************************************************************/

/*----------------------------------------------------------------------
  SetBodyAbsolutePosition:
  ----------------------------------------------------------------------*/
void SetBodyAbsolutePosition (Document doc)
{
  Element              root, body;
  ElementType          elType;

  if (DocumentTypes[doc] != docHTML)
    return;
  root = TtaGetMainRoot (doc);
  elType =  TtaGetElementType(root);
  elType.ElTypeNum = HTML_EL_BODY;
  body = TtaSearchTypedElement (elType, SearchInTree, root);
  if (body)
    ParseHTMLSpecificStyle (body, (char *)"position:absolute", doc, 200, FALSE);
}

/*----------------------------------------------------------------------
  HTMLSetBackgroundColor:
  ----------------------------------------------------------------------*/
void HTMLSetBackgroundColor (Document doc, Element el, int specificity,
                             char *color)
{
  char             css_command[1000];

  snprintf (css_command, 1000, "background-color: %50s", color);
  ParseHTMLSpecificStyle (el, css_command, doc, specificity, FALSE);
}

/*----------------------------------------------------------------------
  HTMLSetForegroundColor:                                        
  ----------------------------------------------------------------------*/
void HTMLSetForegroundColor (Document doc, Element el, int specificity,
                             char *color)
{
  char           css_command[1000];

  snprintf (css_command, 1000, "color: %50s", color);
  ParseHTMLSpecificStyle (el, css_command, doc, specificity, FALSE);
}

/*----------------------------------------------------------------------
  HTMLResetBackgroundColor:                                      
  ----------------------------------------------------------------------*/
void HTMLResetBackgroundColor (Document doc, Element el)
{
  char           css_command[1000];

  sprintf (css_command, "background: red");
  ParseHTMLSpecificStyle (el, css_command, doc, 0, TRUE);
}

/*----------------------------------------------------------------------
  HTMLResetBackgroundImage:                                      
  ----------------------------------------------------------------------*/
void HTMLResetBackgroundImage (Document doc, Element el)
{
  char           css_command[1000];

  snprintf (css_command, 1000, "background-image: url(xx); background-repeat: repeat");
  ParseHTMLSpecificStyle (el, css_command, doc, 0, TRUE);
}

/*----------------------------------------------------------------------
  HTMLResetForegroundColor:                                      
  ----------------------------------------------------------------------*/
void HTMLResetForegroundColor (Document doc, Element el)
{
  char           css_command[1000];

  /* it's not necessary to well know the current color but it must be valid */
  sprintf (css_command, "color: red");
  ParseHTMLSpecificStyle (el, css_command, doc, 0, TRUE);
}

/*----------------------------------------------------------------------
  HTMLSetAlinkColor:                                             
  ----------------------------------------------------------------------*/
void HTMLSetAlinkColor (Document doc, Element el, char *color)
{
  char           css_command[1000];

  snprintf (css_command, 1000, ":link { color: %50s }", color);
  ApplyCSSRules (el, css_command, doc, FALSE);
}

/*----------------------------------------------------------------------
  HTMLSetAactiveColor:                                           
  ----------------------------------------------------------------------*/
void HTMLSetAactiveColor (Document doc, Element el, char *color)
{
  char           css_command[1000];

  snprintf (css_command, 1000, ":active { color: %50s }", color);
  ApplyCSSRules (el, css_command, doc, FALSE);
}

/*----------------------------------------------------------------------
  HTMLSetAvisitedColor:                                          
  ----------------------------------------------------------------------*/
void HTMLSetAvisitedColor (Document doc, Element el, char *color)
{
  char           css_command[1000];

  snprintf (css_command, 1000, ":visited { color: %50s }", color);
  ApplyCSSRules (el, css_command, doc, FALSE);
}

/*----------------------------------------------------------------------
  HTMLResetAlinkColor:                                           
  ----------------------------------------------------------------------*/
void HTMLResetAlinkColor (Document doc, Element el)
{
  char           css_command[1000];

  sprintf (css_command, ":link { color: red }");
  ApplyCSSRules (el, css_command, doc, TRUE);
}

/*----------------------------------------------------------------------
  HTMLResetAactiveColor:                                                 
  ----------------------------------------------------------------------*/
void HTMLResetAactiveColor (Document doc, Element el)
{
  char           css_command[1000];

  sprintf (css_command, ":active { color: red }");
  ApplyCSSRules (el, css_command, doc, TRUE);
}

/*----------------------------------------------------------------------
  HTMLResetAvisitedColor:                                        
  ----------------------------------------------------------------------*/
void HTMLResetAvisitedColor (Document doc, Element el)
{
  char           css_command[1000];

  sprintf (css_command, ":visited { color: red }");
  ApplyCSSRules (el, css_command, doc, TRUE);
}

/*----------------------------------------------------------------------
  ApplyCSSRules: parse a CSS Style description stored in the header of
  a HTML document.
  ----------------------------------------------------------------------*/
void ApplyCSSRules (Element el, char *cssRule, Document doc, ThotBool destroy)
{
  CSSInfoPtr          css;
  PInfoPtr            pInfo;
  ThotBool            loadcss;

  /* check if we have to load CSS */
  TtaGetEnvBoolean ("LOAD_CSS", &loadcss);
  if (!loadcss)
    return;
  LineNumber = TtaGetElementLineNumber (el);
  css = SearchCSS (doc, NULL, el, &pInfo);
  if (css == NULL)
    {
      /* create the document css context */
      css = AddCSS (doc, doc, CSS_DOCUMENT_STYLE, CSS_ALL, NULL, NULL, el);
      pInfo = css->infos[doc];
    }
  else if (pInfo == NULL)
    /* create the entry into the css context */
    pInfo = AddInfoCSS (doc, css, CSS_DOCUMENT_STYLE, CSS_ALL, el);
  if (pInfo->PiEnabled)
    ParseStyleDeclaration (el, cssRule, doc, css, el, NULL, destroy);
  LineNumber = -1;
}

/*----------------------------------------------------------------------
  ReadCSSRules:  is the front-end function called by the document parser
  when detecting a <style type="text/css"> indicating it's the
  beginning of a CSS fragment or when reading a file .css.
  
  The CSS parser has to handle <!-- ... --> constructs used to
  prevent prehistoric browser from displaying the CSS as a text
  content. It will stop on any sequence "<x" where x is different
  from ! and will return x as to the caller. Theorically x should
  be equal to / for the </style> end of style.
  The parameter doc gives the document tree that contains CSS information.
  The parameter docRef gives the document to which CSS are to be applied.
  This function uses the current css context or creates it. It's able
  to work on the given buffer or call GetNextChar to read the parsed
  file.
  The parameter url gives the URL of the parsed style sheet.
  The parameter numberOfLinesRead gives the number of lines already
  read in the file.
  The parameter withUndo indicates whether the changes made in the document
  structure and content have to be registered in the Undo queue or not.
  ----------------------------------------------------------------------*/
char ReadCSSRules (Document docRef, CSSInfoPtr css, char *buffer, char *url,
                   int numberOfLinesRead, ThotBool withUndo, Element link)
{
  DisplayMode         dispMode;
  CSSInfoPtr          refcss = NULL;
  CSSmedia            css_media = CSS_ALL;
  PInfoPtr            pInfo;
  char                c;
  char               *cssRule, *base, *saveDocURL, *ptr;
  int                 index;
  int                 CSSindex;
  int                 CSScomment;
  int                 import;
  int                 openBlock;
  int                 newlines;
  int                 page;
  ThotBool            HTMLcomment;
  ThotBool            toParse, eof, quoted, s_quoted;
  ThotBool            ignore, media, lineComment;
  ThotBool            noRule, ignoreImport, fontface;

  CSScomment = MAX_CSS_LENGTH;
  HTMLcomment = FALSE;
  CSSindex = 0;
  toParse = FALSE;
  noRule = FALSE;
  media = FALSE;
  ignoreImport = FALSE;
  ignore = lineComment = FALSE;
  page = 0;
  quoted = s_quoted = FALSE;
  fontface = FALSE;
  eof = FALSE;
  openBlock = 0;
  import = MAX_CSS_LENGTH;
  c = SPACE;
  index = 0;
  base = NULL;
  /* entering the CSS parsing */
  Style_parsing++;
  /* number of new lines parsed */
  newlines = 0;
  /* avoid too many redisplay */
  dispMode = TtaGetDisplayMode (docRef);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (docRef, DeferredDisplay);

  /* look for the CSS context */
  if (css == NULL)
    css = SearchCSS (docRef, NULL, link, &pInfo);
  else
    pInfo = css->infos[docRef];
  if (css == NULL)
    {
      css = AddCSS (docRef, docRef, CSS_DOCUMENT_STYLE, CSS_ALL, NULL, NULL, link);
      pInfo = css->infos[docRef];
    }
  else if (pInfo == NULL)
    pInfo = AddInfoCSS (docRef, css, CSS_DOCUMENT_STYLE, CSS_ALL, link);
  /* look for the CSS descriptor that points to the extension schema */
  refcss = css;
  if (pInfo && pInfo->PiCategory == CSS_IMPORT)
    {
      while (refcss &&
             refcss->infos[docRef] && refcss->infos[docRef]->PiCategory == CSS_IMPORT)
        refcss = refcss->NextCSS;
      if (refcss)
        pInfo = refcss->infos[docRef];
    }

  /* register parsed CSS file and the document to which CSS are to be applied */
  if (DocumentMeta[docRef] == NULL || DocumentMeta[docRef]->method != CE_MAKEBOOK)
    ParsedDoc = docRef;
  else
    ParsedDoc = 0;
  /* clean up the list of classes */
  TtaFreeMemory (refcss->class_list);
  refcss->class_list = NULL;
  if (url)
    Error_DocURL = url;
  else
    /* the CSS source in within the document itself */
    Error_DocURL = DocumentURLs[docRef];
  LineNumber = numberOfLinesRead + 1;
  NewLineSkipped = 0;
  newlines = 0;

  /* Search for an UTF-8 BOM character (EF BB BF) */
  if (index == 0 && strlen(buffer) > 2 &&
      (unsigned char) buffer[0] == 0xEF &&
      (unsigned char) buffer[1] == 0xBB &&
      (unsigned char) buffer[2] == 0xBF)
    {
      index = 3;
    }

  /* Search for an UTF-16 Big Endian BOM character (FE FF) */
  if (index == 0 && strlen(buffer) > 1 &&
      (unsigned char) buffer[0] == 0xFE &&
      (unsigned char) buffer[1] == 0xFF)
    {
      index = 2;
    }

  /* Search for an UTF-16 Little Endian BOM character (FF FE) */
  if (index == 0 && strlen(buffer) > 1 &&
      (unsigned char) buffer[0] == 0xFF &&
      (unsigned char) buffer[1] == 0xFE)
    {
      index = 2;
    }
  
  while (CSSindex < MAX_CSS_LENGTH && c != EOS && !eof)
    {
      c = buffer[index++];
      eof = (c == EOS);
      CSSbuffer[CSSindex] = c;
      if (!lineComment &&
          (CSScomment == MAX_CSS_LENGTH || c == '*' || c == '/' || c == '<' || c == EOL))
        {
          /* we're not within a comment or we're parsing * or / */
          switch (c)
            {
            case '@': /* perhaps an import primitive */
              if (!fontface && !page && !quoted && !s_quoted)
                import = CSSindex;
              break;
            case ';':
              if (!quoted && !s_quoted && !media && import != MAX_CSS_LENGTH)
                { 
                  if (strncasecmp (&CSSbuffer[import+1], "import", 6))
                    /* it's not an import */
                    import = MAX_CSS_LENGTH;
                  /* save the text */
                  noRule = TRUE;
                }
              break;
            case '*':
              if (!quoted && !s_quoted && CSScomment == MAX_CSS_LENGTH && CSSindex > 0 &&
                  CSSbuffer[CSSindex - 1] == '/')
                /* start a comment */
                CSScomment = CSSindex - 1;
              break;
            case '/':
              if (!quoted && !s_quoted && CSSindex > 1 && CSScomment != MAX_CSS_LENGTH &&
                  CSSbuffer[CSSindex - 1] == '*' && CSSindex != CSScomment + 2)
                {
                  while (CSSindex > 0 && CSSindex >= CSScomment)
                    {
                      if ( CSSbuffer[CSSindex] == EOL)
                        {
                          LineNumber ++;
                          newlines --;
                        }
                      CSSindex--;
                    }
                  CSSindex = CSScomment - 1; /* will be incremented later */
                  CSScomment = MAX_CSS_LENGTH;
                  /* clean up the buffer */
                  if (newlines && CSSindex > 0)
                    while (CSSindex > 0 &&
                           (CSSbuffer[CSSindex] == SPACE ||
                            CSSbuffer[CSSindex] == BSPACE ||
                            CSSbuffer[CSSindex] == EOL ||
                            CSSbuffer[CSSindex] == TAB ||
                            CSSbuffer[CSSindex] == __CR__))
                      {
                        if ( CSSbuffer[CSSindex] == EOL)
                          {
                            LineNumber ++;
                            newlines --;
                          }
                        CSSindex--;
                      }
                }
              else if (!fontface && !page && !quoted && !s_quoted &&
                       CSScomment == MAX_CSS_LENGTH && CSSindex > 0 &&
                       CSSbuffer[CSSindex - 1] ==  '<')
                {
                  /* this is the closing tag ! */
                  CSSindex -= 2; /* remove </ from the CSS string */
                  noRule = TRUE;
                }
              else if (!quoted && !s_quoted &&
                       (CSSindex == 1 || (CSSindex > 1 && CSSbuffer[CSSindex - 2] == EOL))  &&
                       CSScomment == MAX_CSS_LENGTH &&
                       CSSbuffer[CSSindex - 1] == '/')
                {
                  CSSindex--;
                  lineComment = TRUE;
                }
                
              break;
            case '<':
              if (!fontface && !page && !quoted && !s_quoted &&
                  CSScomment == MAX_CSS_LENGTH)
                {
                  /* only if we're not parsing a comment */
                  c = buffer[index++];
                  eof = (c == EOS);
                  if (c == '!')
                    {
                      /* CSS within an HTML comment */
                      HTMLcomment = TRUE;
                      CSSindex++;
                      CSSbuffer[CSSindex] = c;
                    }
                  else if (c == EOS)
                    CSSindex++;
                }
              break;
            case '-':
              if (!fontface && !page && !quoted && !s_quoted &&
                  CSSindex > 0 && CSSbuffer[CSSindex - 1] == '-' &&
                  HTMLcomment)
                /* CSS within an HTML comment */
                noRule = TRUE;
              break;
            case '>':
              if (!fontface && !page && !quoted && !s_quoted && HTMLcomment)
                noRule = TRUE;
              break;
            case ' ':
              if (!quoted && !s_quoted && import != MAX_CSS_LENGTH && openBlock == 0)
                media = !strncasecmp (&CSSbuffer[import+1], "media", 5);
              break;
            case '{':
              if (!quoted && !s_quoted)
                {
                  openBlock++;
                  if (import != MAX_CSS_LENGTH)
                    {
                      if (openBlock == 1 && media)
                        {
                          /* is it the screen concerned? */
                          CSSbuffer[CSSindex+1] = EOS;
                          css_media = CheckMediaCSS (&CSSbuffer[import+7]);
                          if (TtaIsPrinting ())
                            ignore = (css_media != CSS_ALL && css_media != CSS_PRINT);
                          else
                            ignore = (css_media != CSS_ALL && css_media != CSS_SCREEN);
                          noRule = TRUE;
                        }
                      else if (!strncasecmp (&CSSbuffer[import+1], "page", 4))
                        /* it is a @page block */
                        {
                          page = openBlock;/*remember the level of this block*/
                          noRule = TRUE;
                        }
                      else if (!strncasecmp (&CSSbuffer[import+1], "font-face", 9))
                        {
                          fontface = TRUE;
                          noRule = TRUE;
                        }
                    }
                }
              break;
            case '}':
              if (!quoted && !s_quoted)
                {
                  if (page == openBlock)
                    /* closing the @page block */
                    {
                      noRule = TRUE;
                      page = 0; /* close the page section */
                    }
                  else if (fontface)
                    {
                      noRule = TRUE;
                      fontface = FALSE; /* close the fontface section */
                    }
                  else if (openBlock == 1 && import != MAX_CSS_LENGTH)
                    {
                      import = MAX_CSS_LENGTH;
                      noRule = TRUE;
                      ignore = FALSE;
                      media = FALSE;
                    }
                  else if (!page)
                    toParse = TRUE;
                  openBlock--;
                }
              break;
            case '"':
              if (quoted)
                {
                  if (CSSindex > 0 && CSSbuffer[CSSindex - 1] != '\\')
                    quoted = FALSE;
                }
              else if (!s_quoted)
                quoted = TRUE;
              break;
             case '\'':
              if (s_quoted)
                {
                  if (CSSindex > 0 && CSSbuffer[CSSindex - 1] != '\\')
                    s_quoted = FALSE;
                }
              else if (!quoted)
                s_quoted = TRUE;
              break;
           default:
              if (c == EOL)
                {
                  newlines++;
                }
              break;
            }
        }
      else if (c == EOL)
        {
          LineNumber++;
          lineComment = FALSE;
          c = __CR__;
        }

      if (!lineComment && c != __CR__)
        CSSindex++;

      if (CSSindex >= MAX_CSS_LENGTH && CSScomment < MAX_CSS_LENGTH)
        /* we're still parsing a comment: remove the text comment */
        CSSindex = CSScomment;

      if (CSSindex >= MAX_CSS_LENGTH || toParse || noRule)
        {
          CSSbuffer[CSSindex] = EOS;
          /* parse a not empty string */
          if (CSSindex > 0)
            {
              /* apply CSS rule if it's not just a saving of text */
              if (!noRule && !ignore)
                {
                  /* future import rules must be ignored */
                  ignoreImport = TRUE;
                  NewLineSkipped = 0;
                  ParseStyleDeclaration (NULL, CSSbuffer, docRef, refcss,
                                         pInfo->PiLink, url, FALSE);
                  LineNumber += newlines;
                  newlines = 0;
                }
              else if (import != MAX_CSS_LENGTH &&
                       !strncasecmp (&CSSbuffer[import+1], "import", 6))
                {
                  /* import section */
                  cssRule = &CSSbuffer[import+7];
                  cssRule = (char*)TtaSkipBlanks (cssRule);
                  /* save the current line number */
                  newlines += LineNumber;
                  if (!strncasecmp (cssRule, "url", 3))
                    {
                      cssRule = &cssRule[3];
                      cssRule = (char*)TtaSkipBlanks (cssRule);
                      if (*cssRule == '(')
                        {
                          cssRule++;
                          cssRule = (char*)TtaSkipBlanks (cssRule);
                          quoted = (*cssRule == '"' || *cssRule == '\'');
                          if (quoted)
                            cssRule++;
                          base = cssRule;
                          while (*cssRule != EOS && *cssRule != ')')
                            cssRule++;
                          if (quoted)
                            {
                              /* isolate the file name */
                              cssRule[-1] = EOS;
                              quoted = FALSE;
                            }
                          else
                            {
                              /* remove extra spaces */
                              if (cssRule[-1] == SPACE)
                                {
                                  *cssRule = SPACE;
                                  cssRule--;
                                  while (cssRule[-1] == SPACE)
                                    cssRule--;
                                }
                            }
                          *cssRule = EOS;
                        }
                    }
                  else if (*cssRule == '"')
                    {
                      /*
                        Do we have to accept single quotes?
                        Double quotes are accepted here.
                        Escaped quotes are not handled. See function SkipQuotedString
                      */
                      cssRule++;
                      cssRule = (char*)TtaSkipBlanks (cssRule);
                      base = cssRule;
                      while (*cssRule != EOS &&
                             (*cssRule != '"' ||
                              (*cssRule == '"' && cssRule[-1] == '\\')))
                        cssRule++;
                      /* isolate the file name */
                      *cssRule = EOS;
                    }
                  /* check if a media is defined */
                  cssRule++;
                  cssRule = (char*)TtaSkipBlanks (cssRule);
                  if (*cssRule != ';')
                    {
                      css_media = CheckMediaCSS (cssRule);
                      if (TtaIsPrinting ())
                        ignoreImport = (css_media != CSS_ALL && css_media != CSS_PRINT);
                      else
                        ignoreImport = (css_media != CSS_ALL && css_media != CSS_SCREEN);
                    }
                  if (!ignoreImport)
                    {
                      /* save the displayed URL when an error is reported */
                      saveDocURL = Error_DocURL;
                      ptr = TtaStrdup (base);
                      /* get the CSS URI in UTF-8 */
                      /*ptr = ReallocUTF8String (ptr, docRef);*/
                      LoadStyleSheet (base, docRef, (Element) css, css,
                                      url, pInfo->PiMedia,
                                      pInfo->PiCategory == CSS_USER_STYLE);
                      /* restore the displayed URL when an error is reported */
                      Error_DocURL = saveDocURL;
                      TtaFreeMemory (ptr);
                    }
                  /* restore the number of lines */
                  LineNumber = newlines;
                  newlines = 0;
                  NewLineSkipped = 0;
                  import = MAX_CSS_LENGTH;
                }
              else
                {
                  LineNumber += newlines;
                  newlines = 0;
                }
            }
          toParse = FALSE;
          noRule = FALSE;
          CSSindex = 0;
        }
    }
  /* closing the CSS parsing */
  Style_parsing--;
  if (RedisplayImages == 0 && RedisplayBGImage && Style_parsing == 0)
    {
      /* CSS parsing finishes after a BG image was loaded */
      RedisplayBGImage = FALSE;
      if (dispMode != NoComputedDisplay)
        {
          //printf ("ReadCSS Show BGimages\n");
          TtaSetDisplayMode (docRef, NoComputedDisplay);
          TtaSetDisplayMode (docRef, dispMode);
        }
    }
  else if (dispMode != NoComputedDisplay)
    /* restore the display mode */
    TtaSetDisplayMode (docRef, dispMode);

  /* Prepare the context for style attributes */
  Error_DocURL = DocumentURLs[docRef];
  LineNumber = -1;
  return (c);
}
