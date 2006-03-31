/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2005
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
#include "UIcss_f.h"
#include "css_f.h"
#include "fetchHTMLname_f.h"
#include "fetchXMLname_f.h"
#include "html2thot_f.h"
#include "init_f.h"
#include "styleparser_f.h"

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
  char                *name;
  PropertyParser       parsing_function;
}
CSSProperty;

static int           LineNumber = -1; /* The line where the error occurs */
static int           NewLineSkipped = 0;
static int           RedisplayImages = 0; /* number of BG images loading */
static int           RedisplayDoc = 0; /* document to be redisplayed */
static int           Style_parsing = 0; /* > 0 when parsing a set of CSS rules */
static ThotBool      RedisplayBGImage = FALSE; /* TRUE when a BG image is inserted */
static ThotBool      DoApply = TRUE;

/*----------------------------------------------------------------------
  SkipWord:                                                  
  ----------------------------------------------------------------------*/
static char *SkipWord (char *ptr)
{
  while (isalnum((int)*ptr) || *ptr == '-' || *ptr == '#' || *ptr == '%')
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
  while (ptr[0] == '/' &&
         ptr[1] == '*')
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
  When the line is 0 ask to expat the current line number
  ----------------------------------------------------------------------*/
static void CSSPrintError (char *msg, char *value)
{
  if (!TtaIsPrinting () && ParsedDoc > 0)
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
  When the line is 0 ask expat about the current line number
  ----------------------------------------------------------------------*/
static void CSSParseError (char *msg, char *value, char *endvalue)
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
static char *CSSCheckEndValue (char *cssRule, char *endvalue, char *msg)
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

  deb = ptr;
  while (*ptr != EOS && *ptr != ';' && *ptr != '}' && *ptr != '}')
    {
      if (*ptr == '"' || *ptr == '\'')
        ptr = SkipString (ptr);
      if (*ptr != EOS)
        ptr++;
    }
  /* print the skipped property */
  c = *ptr;
  *ptr = EOS;
  if (reportError && *deb != EOS &&
      strncasecmp (deb, "azimuth", 7) &&
      strncasecmp (deb, "border-collapse", 15) &&
      strncasecmp (deb, "border-spacing", 14) &&
      strncasecmp (deb, "caption-side", 12) &&
      strncasecmp (deb, "clip", 4) &&
      strncasecmp (deb, "counter-increment", 16) &&
      strncasecmp (deb, "counter-reset", 13) &&
      strncasecmp (deb, "cue-after", 9) &&
      strncasecmp (deb, "cue-before", 10) &&
      strncasecmp (deb, "cue", 3) &&
      strncasecmp (deb, "cursor", 6) &&
      strncasecmp (deb, "elevation", 9) &&
      strncasecmp (deb, "empty-cells", 11) &&
      strncasecmp (deb, "font-strech", 11) &&
      strncasecmp (deb, "letter-spacing", 14) &&
      strncasecmp (deb, "marker-offset", 12) &&
      strncasecmp (deb, "max-height", 10) &&
      strncasecmp (deb, "max-width", 9) &&
      strncasecmp (deb, "min-height", 10) &&
      strncasecmp (deb, "min-width", 9) &&
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
  *ptr = c;
  return (ptr);
}

/*----------------------------------------------------------------------
  SkipValue
  skips the value and display an error message if msg is not NULL
  ----------------------------------------------------------------------*/
static char *SkipValue (char *msg, char *ptr)
{
  char       *deb;
  char        c;

  deb = ptr;
  while (*ptr != EOS && *ptr != ';' && *ptr != '}' && *ptr != '}' && *ptr != '\n')
    {
      if (*ptr == '"' || *ptr == '\'')
        ptr = SkipString (ptr);
      if (*ptr != EOS)
        ptr++;
    }
  /* print the skipped property */
  c = *ptr;
  *ptr = EOS;
  if (msg && *deb != EOS && *deb != ',')
    CSSPrintError (msg, deb);
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

  if (*cssRule == '+')
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
  ParseCSSUnit:                                                  
  parse a CSS Unit substring and returns the corresponding      
  value and its unit.                                           
  ----------------------------------------------------------------------*/
char *ParseCSSUnit (char *cssRule, PresentationValue *pval)
{
  unsigned int        uni;

  pval->typed_data.unit = UNIT_REL;
  cssRule = ParseNumber (cssRule, pval);
  if (pval->typed_data.unit == UNIT_INVALID)
    cssRule = SkipWord (cssRule);
  else
    {
      cssRule = SkipBlanksAndComments (cssRule);
      uni = 0;
      while (CSSUnitNames[uni].sign)
        {
          if (!strncasecmp (CSSUnitNames[uni].sign, cssRule,
                            strlen (CSSUnitNames[uni].sign)))
            {
              pval->typed_data.unit = CSSUnitNames[uni].unit;
              return (cssRule + strlen (CSSUnitNames[uni].sign));
            }
          else
            uni++;
        }
      /* not in the list of predefined units */
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
      if (border->typed_data.value == 0)
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

  cssRule = SkipBlanksAndComments (cssRule);
  val->typed_data.unit = UNIT_INVALID;
  val->typed_data.real = FALSE;
  val->typed_data.value = 0;
  ptr = TtaGiveRGB (cssRule, &redval, &greenval, &blueval);
  if (!strncasecmp (cssRule, "InactiveCaptionText", 19))
    {
      cssRule += 19;
    }
  else if (!strncasecmp (cssRule, "ThreeDLightShadow", 17))
    {
      cssRule += 17;
    }
  else if (!strncasecmp (cssRule, "ThreeDDarkShadow", 16))
    {
      cssRule += 16;
    }
  else if (!strncasecmp (cssRule, "ButtonHighlight", 15) ||
           !strncasecmp (cssRule, "InactiveCaption", 15) ||
           !strncasecmp (cssRule, "ThreeDHighlight", 15))
    {
      cssRule += 15;
    }
  else if (!strncasecmp (cssRule, "InactiveBorder", 14) ||
           !strncasecmp (cssRule, "InfoBackground", 14))
    {
      cssRule += 14;
    }
  else if (!strncasecmp (cssRule, "ActiveCaption", 13) ||
           !strncasecmp (cssRule, "HighlightText", 13))
    {
      cssRule += 13;
    }
  else if (!strncasecmp (cssRule, "ActiveBorder", 12) ||
           !strncasecmp (cssRule, "AppWorkspace", 12) ||
           !strncasecmp (cssRule, "ButtonShadow", 12) ||
           !strncasecmp (cssRule, "ThreeDShadow", 12))
    {
      cssRule += 12;
    }
  else if (!strncasecmp (cssRule, "CaptionText", 11) ||
           !strncasecmp (cssRule, "WindowFrame", 11))
    {
      cssRule += 11;
    }
  else if (!strncasecmp (cssRule, "Background", 10) ||
           !strncasecmp (cssRule, "ButtonFace", 10) ||
           !strncasecmp (cssRule, "ButtonText", 10) ||
           !strncasecmp (cssRule, "ThreeDFace", 10) ||
           !strncasecmp (cssRule, "WindowText", 10))
    {
      cssRule += 10;
    }
  else if (!strncasecmp (cssRule, "Highlight", 9) ||
           !strncasecmp (cssRule, "Scrollbar", 9))
    {
      cssRule += 9;
    }
  else if (!strncasecmp (cssRule, "GrayText", 8) ||
           !strncasecmp (cssRule, "InfoText", 8) ||
           !strncasecmp (cssRule, "MenuText", 8))
    {
      cssRule += 8;
    }
  else if (!strncasecmp (cssRule, "Window", 6))
    {
      cssRule += 6;
    }
  else if (!strncasecmp (cssRule, "Menu", 5))
    {
      cssRule += 5;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      val->typed_data.unit = VALUE_INHERIT;
      cssRule += 7;
    }

  if (ptr == cssRule)
    {
      cssRule = SkipWord (cssRule);
      CSSParseError ("Invalid color value", ptr, cssRule);
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
static char *CheckImportantRule (char *cssRule, PresentationContext context)
{
  PresentationContextBlock dummyctxt;

  if (context == NULL)
    /* no context provided */
    context = &dummyctxt;

  cssRule = SkipBlanksAndComments (cssRule);
  /* update the line number */
  context->cssLine = LineNumber + NewLineSkipped;
  if (*cssRule != '!')
    context->important = FALSE;
  else
    {
      cssRule++;
      cssRule = SkipBlanksAndComments (cssRule);
      if (!strncasecmp (cssRule, "important", 9))
        {
          context->important = TRUE;
          cssRule += 9;
        }
      else
        context->important = FALSE;
    }
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
  
  cssRule = SkipBlanksAndComments (cssRule);
  cssRule = ParseABorderValue (cssRule, &border);
  /* check if it's an important rule */
  cssRule = CheckImportantRule (cssRule, context);
  if (border.typed_data.unit != UNIT_INVALID && DoApply)
    {
      TtaSetStylePresentation (PRBorderTopWidth, element, tsch, context, border);
      border.typed_data.value = 1;
      border.typed_data.unit = UNIT_REL;
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
  
  cssRule = SkipBlanksAndComments (cssRule);
  /* first parse the attribute string */
  cssRule = ParseABorderValue (cssRule, &border);
  /* check if it's an important rule */
  cssRule = CheckImportantRule (cssRule, context);
  if (border.typed_data.unit != UNIT_INVALID && DoApply)
    {
      TtaSetStylePresentation (PRBorderBottomWidth, element, tsch, context, border);
      border.typed_data.value = 1;
      border.typed_data.unit = UNIT_REL;
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
  
  cssRule = SkipBlanksAndComments (cssRule);
  /* first parse the attribute string */
  cssRule = ParseABorderValue (cssRule, &border);
  /* check if it's an important rule */
  cssRule = CheckImportantRule (cssRule, context);
  if (border.typed_data.unit != UNIT_INVALID && DoApply)
    {
      TtaSetStylePresentation (PRBorderLeftWidth, element, tsch, context, border);
      border.typed_data.value = 1;
      border.typed_data.unit = UNIT_REL;
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
  
  cssRule = SkipBlanksAndComments (cssRule);
  /* first parse the attribute string */
  cssRule = ParseABorderValue (cssRule, &border);
  /* check if it's an important rule */
  cssRule = CheckImportantRule (cssRule, context);
  if (border.typed_data.unit != UNIT_INVALID && DoApply)
    {
      TtaSetStylePresentation (PRBorderRightWidth, element, tsch, context, border);
      border.typed_data.value = 1;
      border.typed_data.unit = UNIT_REL;
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
  int   skippedNL;

  ptrT = SkipBlanksAndComments (cssRule);
  /* First parse Border-Top */
  ptrR = ParseCSSBorderTopWidth (element, tsch, context, ptrT, css, isHTML);
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

  if (!strncasecmp (cssRule, "transparent", 11))
    {
      best.typed_data.value = -2;  /* -2 means transparent */
      best.typed_data.unit = UNIT_REL;
      cssRule = SkipWord (cssRule);
    }
  else
    cssRule = ParseCSSColor (cssRule, &best);
  /* check if it's an important rule */
  cssRule = CheckImportantRule (cssRule, context);
  if (best.typed_data.unit != UNIT_INVALID && DoApply)
    {
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
  
  if (!strncasecmp (cssRule, "transparent", 11))
    {
      best.typed_data.value = -2;  /* -2 means transparent */
      best.typed_data.unit = UNIT_REL;
      cssRule = SkipWord (cssRule);
    }
  else
    cssRule = ParseCSSColor (cssRule, &best);
  /* check if it's an important rule */
  cssRule = CheckImportantRule (cssRule, context);
  if (best.typed_data.unit != UNIT_INVALID && DoApply)
    {
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

  if (!strncasecmp (cssRule, "transparent", 11))
    {
      best.typed_data.value = -2;  /* -2 means transparent */
      best.typed_data.unit = UNIT_REL;
      cssRule = SkipWord (cssRule);
    }
  else
    cssRule = ParseCSSColor (cssRule, &best);
  /* check if it's an important rule */
  cssRule = CheckImportantRule (cssRule, context);
  if (best.typed_data.unit != UNIT_INVALID && DoApply)
    {
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

  if (!strncasecmp (cssRule, "transparent", 11))
    {
      best.typed_data.value = -2;  /* -2 means transparent */
      best.typed_data.unit = UNIT_REL;
      cssRule = SkipWord (cssRule);
    }
  else
    cssRule = ParseCSSColor (cssRule, &best);
  cssRule = CheckImportantRule (cssRule, context);
  if (best.typed_data.unit != UNIT_INVALID && DoApply)
    {
      /* check if it's an important rule */
      /* install the new presentation */
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
  int   skippedNL;

  ptrT = SkipBlanksAndComments (cssRule);
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
  
  cssRule = SkipBlanksAndComments (cssRule);
  cssRule = ParseBorderStyle (cssRule, &border);
  cssRule = CheckImportantRule (cssRule, context);
  if (border.typed_data.unit != UNIT_INVALID && DoApply)
    {
      /* check if it's an important rule */
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
  
  cssRule = SkipBlanksAndComments (cssRule);
  cssRule = ParseBorderStyle (cssRule, &border);
  cssRule = CheckImportantRule (cssRule, context);
  if (border.typed_data.unit != UNIT_INVALID && DoApply)
    {
      /* check if it's an important rule */
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
  
  cssRule = SkipBlanksAndComments (cssRule);
  cssRule = ParseBorderStyle (cssRule, &border);
  cssRule = CheckImportantRule (cssRule, context);
  if (border.typed_data.unit != UNIT_INVALID && DoApply)
    {
      /* check if it's an important rule */
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
  
  cssRule = SkipBlanksAndComments (cssRule);
  cssRule = ParseBorderStyle (cssRule, &border);
  cssRule = CheckImportantRule (cssRule, context);
  if (border.typed_data.unit != UNIT_INVALID && DoApply)
    {
      /* check if it's an important rule */
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
  int   skippedNL;

  ptrT = SkipBlanksAndComments (cssRule);
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
  char           *ptr;
  ThotBool        style, width;

  cssRule = SkipBlanksAndComments (cssRule);
  /* register given values */
  if (!strncmp (cssRule, "none", 4))
    style = width = TRUE;
  else
    style = width = FALSE;
  while (*cssRule != ';' && *cssRule != '}' && *cssRule != EOS && *cssRule != ',')
    {
      ptr = cssRule;
      cssRule = ParseCSSBorderStyleTop (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
        {
          cssRule = ParseCSSBorderTopWidth (element, tsch, context, cssRule, css, isHTML);
          if (ptr == cssRule)
            cssRule = ParseCSSBorderColorTop (element, tsch, context, cssRule, css, isHTML);
          else
            width = TRUE;
          if (ptr == cssRule)
            {
              /* rule not found */
              cssRule = SkipValue ("Invalid border value", cssRule);
              cssRule = CheckImportantRule (cssRule, context);
              return (cssRule);
            }
        }
      else
        style = TRUE;
      cssRule = SkipBlanksAndComments (cssRule);
    }

  if (!width)
    ParseCSSBorderTopWidth (element, tsch, context, "medium", css, isHTML);
  if (!style)
    ParseCSSBorderStyleTop (element, tsch, context, "none", css, isHTML);
  cssRule = CheckImportantRule (cssRule, context);
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
  char           *ptr;
  ThotBool        style, width;

  cssRule = SkipBlanksAndComments (cssRule);
  /* register given values */
  if (!strncmp (cssRule, "none", 4))
    style = width = TRUE;
  else
    style = width = FALSE;
  while (*cssRule != ';' && *cssRule != '}' && *cssRule != EOS && *cssRule != ',')
    {
      ptr = cssRule;
      cssRule = ParseCSSBorderStyleLeft (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
        {
          cssRule = ParseCSSBorderLeftWidth (element, tsch, context, cssRule, css, isHTML);
          if (ptr == cssRule)
            cssRule = ParseCSSBorderColorLeft (element, tsch, context, cssRule, css, isHTML);
          else
            width = TRUE;
          if (ptr == cssRule)
            {
              /* rule not found */
              cssRule = SkipValue ("Invalid border value", cssRule);
              cssRule = CheckImportantRule (cssRule, context);
              return (cssRule);
            }
        }
      else
        style = TRUE;
      cssRule = SkipBlanksAndComments (cssRule);
    }

  if (!width)
    ParseCSSBorderLeftWidth (element, tsch, context, "medium", css, isHTML);
  if (!style)
    ParseCSSBorderStyleLeft (element, tsch, context, "none", css, isHTML);
  cssRule = CheckImportantRule (cssRule, context);
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
  char           *ptr;
  ThotBool        style, width;

  cssRule = SkipBlanksAndComments (cssRule);
  /* register given values */
  if (!strncmp (cssRule, "none", 4))
    style = width = TRUE;
  else
    style = width = FALSE;
  while (*cssRule != ';' && *cssRule != '}' && *cssRule != EOS && *cssRule != ',')
    {
      ptr = cssRule;
      cssRule = ParseCSSBorderStyleBottom (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
        {
          cssRule = ParseCSSBorderBottomWidth (element, tsch, context, cssRule, css, isHTML);
          if (ptr == cssRule)
            cssRule = ParseCSSBorderColorBottom (element, tsch, context, cssRule, css, isHTML);
          else
            width = TRUE;
          if (ptr == cssRule)
            {
              /* rule not found */
              cssRule = SkipValue ("Invalid border value", cssRule);
              cssRule = CheckImportantRule (cssRule, context);
              return (cssRule);
            }
        }
      else
        style = TRUE;
      cssRule = SkipBlanksAndComments (cssRule);
    }

  if (!width)
    ParseCSSBorderBottomWidth (element, tsch, context, "medium", css, isHTML);
  if (!style)
    ParseCSSBorderStyleBottom (element, tsch, context, "none", css, isHTML);
  cssRule = CheckImportantRule (cssRule, context);
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
  char            *ptr;
  ThotBool        style, width;

  cssRule = SkipBlanksAndComments (cssRule);
  /* register given values */
  if (!strncmp (cssRule, "none", 4))
    style = width = TRUE;
  else
    style = width = FALSE;
  while (*cssRule != ';' && *cssRule != '}' && *cssRule != EOS && *cssRule != ',')
    {
      ptr = cssRule;
      cssRule = ParseCSSBorderStyleRight (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
        {
          cssRule = ParseCSSBorderRightWidth (element, tsch, context, cssRule, css, isHTML);
          if (ptr == cssRule)
            cssRule = ParseCSSBorderColorRight (element, tsch, context, cssRule, css, isHTML);
          else
            width = TRUE;
          if (ptr == cssRule)
            {
              /* rule not found */
              cssRule = SkipValue ("Invalid border value", cssRule);
              cssRule = CheckImportantRule (cssRule, context);
              return (cssRule);
            }
        }
      else
        style = TRUE;
      cssRule = SkipBlanksAndComments (cssRule);
    }

  if (!width)
    ParseCSSBorderRightWidth (element, tsch, context, "medium", css, isHTML);
  if (!style)
    ParseCSSBorderStyleRight (element, tsch, context, "none", css, isHTML);
  cssRule = CheckImportantRule (cssRule, context);
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
  int   skippedNL;

  ptrT = SkipBlanksAndComments (cssRule);
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
    cssRule = SkipValue ("Invalid float value", cssRule);
  else
    {
      cssRule = CheckImportantRule (cssRule, context);
      if (DoApply)
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
      cssRule = CheckImportantRule (cssRule, context);
      cssRule = SkipValue (NULL, cssRule);
    }
  else
    {
      cssRule = SkipValue (NULL, cssRule);
      cssRule = CheckImportantRule (cssRule, context);
      if (DoApply)
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
  char               *ptr = cssRule;

  pval.typed_data.unit = UNIT_REL;
  pval.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
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
  cssRule = CheckImportantRule (cssRule, context);
  if (DoApply)
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
  char               *ptr = cssRule;

  pval.typed_data.unit = UNIT_REL;
  pval.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
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
      if (strncasecmp (cssRule, "table-row-group", 15) &&
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

  cssRule = CheckImportantRule (cssRule, context);
  if (DoApply)
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
  cssRule = SkipValue (NULL, cssRule);
  cssRule = CheckImportantRule (cssRule, context);
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

  pval.typed_data.unit = UNIT_REL;
  pval.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  if (!strncasecmp (cssRule, "disc", 4))
    {
      cssRule += 4;
      pval.typed_data.value = Disc;
    }
  else if (!strncasecmp (cssRule, "circle", 6))
    {
      cssRule += 6;
      pval.typed_data.value = Circle;
    }
  else if (!strncasecmp (cssRule, "square", 6))
    {
      cssRule += 6;
      pval.typed_data.value = Square;
    }
  else if (!strncasecmp (cssRule, "decimal-leading-zero", 20))
    {
      cssRule += 20;
      pval.typed_data.value = DecimalLeadingZero;
    }
  else if (!strncasecmp (cssRule, "decimal", 7))
    {
      cssRule += 7;
      pval.typed_data.value = Decimal;
    }
  else if (!strncasecmp (cssRule, "lower-roman", 11))
    {
      cssRule += 11;
      pval.typed_data.value = LowerRoman;
    }
  else if (!strncasecmp (cssRule, "upper-roman", 11))
    {
      cssRule += 11;
      pval.typed_data.value = UpperRoman;
    }
  else if (!strncasecmp (cssRule, "lower-greek", 11))
    {
      cssRule += 11;
      pval.typed_data.value = LowerGreek;
    }
  else if (!strncasecmp (cssRule, "lower-latin", 11))
    {
      cssRule += 11;
      pval.typed_data.value = LowerLatin;
    }
  else if (!strncasecmp (cssRule, "lower-alpha", 11))
    {
      cssRule += 11;
      pval.typed_data.value = LowerLatin;
    }
  else if (!strncasecmp (cssRule, "upper-latin", 11))
    {
      cssRule += 11;
      pval.typed_data.value = UpperLatin;
    }
  else if (!strncasecmp (cssRule, "upper-alpha", 11))
    {
      cssRule += 11;
      pval.typed_data.value = UpperLatin;
    }
  else if (!strncasecmp (cssRule, "armenian", 8))
    {
      cssRule += 8;
      pval.typed_data.value = Decimal;
    }
  else if (!strncasecmp (cssRule, "georgian", 8))
    {
      cssRule += 8;
      pval.typed_data.value = Decimal;
    }
  else if (!strncasecmp (cssRule, "none", 4))
    {
      cssRule += 4;
      pval.typed_data.value = ListStyleTypeNone;
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      cssRule += 7;
      pval.typed_data.unit = VALUE_INHERIT;
    }
  else
    {
      cssRule = SkipValue ("Invalid list-style-type value", cssRule);
      return (cssRule);
    }

  cssRule = CheckImportantRule (cssRule, context);
  if (DoApply)
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
      if (saved == ')')
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
  CSSInfoPtr                 css;
  PresentationContext        ctxt;
  PresentationValue          image;
  PresentationValue          value;

  callblock = (CSSImageCallbackPtr) extra;
  if (callblock == NULL)
    return;

  css = NULL;
  el = callblock->el;
  tsch = callblock->tsch;
  ctxt = callblock->ctxt;
  if (doc == 0 && !isnew)
    /* apply to the current document only */
    doc = ctxt->doc;
  if (doc)
    {
      /* avoid too many redisplay */
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, DeferredDisplay);
    }
  else
    {
      dispMode = TtaGetDisplayMode (RedisplayDoc);
      /* check if the CSS still exists */
      css = CSSList;
      while (css && css != callblock->css)
        css = css->NextCSS;
      if (css == NULL)
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
  if (doc)
    {
      if (dispMode == DisplayImmediately)
        /* restore the display mode */
        TtaSetDisplayMode (doc, dispMode);
    }
  else if (css && Style_parsing == 0 && RedisplayImages == 0 && RedisplayDoc)
    {
      /* all background images are now loaded */
      if (css->infos[RedisplayDoc] &&
          /* don't manage a document used by make book */
          (DocumentMeta[RedisplayDoc] == NULL ||
           DocumentMeta[RedisplayDoc]->method != CE_MAKEBOOK))
        {
          /* Change the Display Mode to take into account the new
             presentation */
          dispMode = TtaGetDisplayMode (RedisplayDoc);
#ifdef AMAYA_DEBUG
          //printf ("ParseCSSImageCallback Show BGimages\n");
#endif /* AMAYA_DEBUG */
          /* force the redisplay of this box */
          TtaSetDisplayMode (RedisplayDoc, NoComputedDisplay);
          TtaSetDisplayMode (RedisplayDoc, dispMode);
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
  char                      *url;
  PresentationValue          image;
  char                      *bg_image;
  char                       tempname[MAX_LENGTH];
  char                       imgname[MAX_LENGTH];

  if (element)
    el = element;
  else
    /* default element for FetchImage */
    el = TtaGetMainRoot (ctxt->doc);
  url = NULL;
  cssRule = ParseCSSUrl (cssRule, &url);
  cssRule = CheckImportantRule (cssRule, ctxt);
  if (ctxt->destroy)
    {
      /* remove the background image PRule */
      image.pointer = NULL;
      TtaSetStylePresentation (ruleType, element, tsch, ctxt,image);
    }
  else if (url && DoApply)
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
                  RedisplayDoc = ctxt->doc;
                  if (css->url)
                    /* the image concerns a CSS file */
                    NormalizeURL (url, 0, tempname, imgname, css->url);
                  else
                    /* the image concerns a style element */
                    NormalizeURL (url, ctxt->doc, tempname, imgname, NULL);
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
  if (url)
    TtaFreeMemory (url);
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
  PresentationValue   pval;

  pval.typed_data.unit = UNIT_REL;
  pval.typed_data.real = FALSE;
  url = NULL;
  cssRule = SkipBlanksAndComments (cssRule);
  if (!strncasecmp (cssRule, "none", 4))
    {
      cssRule += 4;
      cssRule = CheckImportantRule (cssRule, ctxt);
      pval.typed_data.value = 0;
      if (DoApply)
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
      cssRule = CheckImportantRule (cssRule, ctxt);
      if (DoApply)
        TtaSetStylePresentation (PRListStyleImage, element, tsch, ctxt, pval);
    }
  else
    {
      cssRule = SkipValue ("Invalid list-style-image value", cssRule);
      cssRule = CheckImportantRule (cssRule, ctxt);
    }
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

  pval.typed_data.unit = UNIT_REL;
  pval.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
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
      cssRule = CheckImportantRule (cssRule, context);
      return (cssRule);
    }

  cssRule = CheckImportantRule (cssRule, context);
  if (DoApply)
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
  char *ptr = cssRule;
  PresentationValue   align;

  align.typed_data.value = 0;
  align.typed_data.unit = UNIT_REL;
  align.typed_data.real = FALSE;

  cssRule = SkipBlanksAndComments (cssRule);
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
      cssRule = CheckImportantRule (cssRule, context);
      return (cssRule);
    }

  /*
   * install the new presentation.
   */
  cssRule = CheckImportantRule (cssRule, context);
  if (align.typed_data.value && DoApply)
    TtaSetStylePresentation (PRAdjust, element, tsch, context, align);
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
  char               *ptr = cssRule;

  align.typed_data.value = 0;
  align.typed_data.unit = UNIT_REL;
  align.typed_data.real = FALSE;

  cssRule = SkipBlanksAndComments (cssRule);
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
      cssRule = CheckImportantRule (cssRule, context);
      return (cssRule);
    }

  /*
   * install the new presentation.
   */
  cssRule = CheckImportantRule (cssRule, context);
  if (DoApply &&
      (align.typed_data.value || align.typed_data.unit == VALUE_INHERIT))
    TtaSetStylePresentation (PRAdjust, element, tsch, context, align);
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
  char               *ptr = cssRule;

  direction.typed_data.value = 0;
  direction.typed_data.unit = UNIT_REL;
  direction.typed_data.real = FALSE;

  cssRule = SkipBlanksAndComments (cssRule);
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
      cssRule = CheckImportantRule (cssRule, context);
      return (cssRule);
    }

  /*
   * install the new presentation.
   */
  cssRule = CheckImportantRule (cssRule, context);
  if (DoApply &&
      (direction.typed_data.value || direction.typed_data.unit == VALUE_INHERIT))
    TtaSetStylePresentation (PRDirection, element, tsch, context, direction);
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
  char               *ptr = cssRule;

  bidi.typed_data.value = 0;
  bidi.typed_data.unit = UNIT_REL;
  bidi.typed_data.real = FALSE;

  cssRule = SkipBlanksAndComments (cssRule);
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
      cssRule = CheckImportantRule (cssRule, context);
      return (cssRule);
    }

  /*
   * install the new presentation.
   */
  cssRule = CheckImportantRule (cssRule, context);
  if (DoApply &&
      (bidi.typed_data.value || bidi.typed_data.unit == VALUE_INHERIT))
    TtaSetStylePresentation (PRUnicodeBidi, element, tsch, context, bidi);
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

  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  cssRule = ParseCSSUnit (cssRule, &pval);
  if (pval.typed_data.value == 0)
    pval.typed_data.unit = UNIT_PX;
  else if (pval.typed_data.unit == UNIT_INVALID ||
           pval.typed_data.unit == UNIT_BOX)
    {
      CSSParseError ("Invalid text-indent value", ptr, cssRule);
      cssRule = CheckImportantRule (cssRule, context);
      return (cssRule);
    }
  /* install the attribute */
  cssRule = CheckImportantRule (cssRule, context);
  if (DoApply)
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
  cssRule = SkipValue (NULL, cssRule);
  cssRule = CheckImportantRule (cssRule, context);
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
          cssRule = CheckImportantRule (cssRule, context);
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

  cssRule = CheckImportantRule (cssRule, context);
  if (pval.typed_data.unit != UNIT_INVALID && DoApply)
    TtaSetStylePresentation (PRHorizRef, element, tsch, context, pval);
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
  char *ptr = cssRule;

  cssRule = SkipBlanksAndComments (cssRule);
  if (!strncasecmp (cssRule, "normal", 6))
    cssRule += 6;
  else if (!strncasecmp (cssRule, "pre", 3))
    cssRule += 3;
  else if (!strncasecmp (cssRule, "nowrap", 6))
    cssRule += 6;
  else if (!strncasecmp (cssRule, "pre-wrap", 8))
    cssRule += 8;
  else if (!strncasecmp (cssRule, "pre-line", 8))
    cssRule += 8;
  else if (!strncasecmp (cssRule, "inherit", 7))
    cssRule += 7;
  else
    cssRule = SkipValue ("Invalid white-space value", cssRule);

  cssRule = CheckImportantRule (cssRule, context);
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
  cssRule = SkipValue (NULL, cssRule);
  cssRule = CheckImportantRule (cssRule, context);
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

  cssRule = CheckImportantRule (cssRule, context);
  if (pval.typed_data.unit == UNIT_INVALID)
    CSSParseError ("Invalid line-height value", ptr, cssRule);
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
  char               *ptr = NULL, *ptr1 = NULL;
  ThotBool	       real, linespace = FALSE;

  pval.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
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
        {
          cssRule = SkipValue ("Invalid font-size value", cssRule);
          cssRule = CheckImportantRule (cssRule, context);
        }
      return (cssRule);
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
      if (pval.typed_data.value != 0 &&
          (pval.typed_data.unit == UNIT_INVALID ||
           pval.typed_data.unit == UNIT_BOX ||
           pval.typed_data.value < 0))
        /* not a valid value */
        return (cssRule);
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
  cssRule = CheckImportantRule (cssRule, context);
  if (!check && DoApply)
    TtaSetStylePresentation (PRSize, element, tsch, context, pval);
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
  cssRule = CheckImportantRule (cssRule, context);
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
        cssRule = SkipWord (cssRule);
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

  cssRule = CheckImportantRule (cssRule, context);
  if ((font.typed_data.value != 0 || font.typed_data.unit == VALUE_INHERIT) &&
      DoApply)
    /* install the new presentation */
    TtaSetStylePresentation (PRFont, element, tsch, context, font);
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
  cssRule = ParseACSSFontFamily (element, tsch, context, cssRule, css, isHTML);
  /* skip extra values */
  while (cssRule && *cssRule != ';' && *cssRule != '}' && *cssRule != EOS)
    cssRule++;
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

  weight.typed_data.value = 0;
  weight.typed_data.unit = UNIT_REL;
  weight.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  if (isdigit (*cssRule) && *cssRule != '0' &&
      cssRule[1] == '0' && cssRule[2] == '0' &&
      (cssRule[3] == EOS || cssRule[3] == SPACE || cssRule[3] == '/' ||
       cssRule[3] == ';' || cssRule[3] == '}' || cssRule[3] == EOL || 
       cssRule[3] == TAB || cssRule[3] ==  __CR__))
    {
      if (!strncasecmp (cssRule, "100", 3))
        {
          weight.typed_data.value = -3;
          cssRule = SkipWord (cssRule);
        }
      else if (!strncasecmp (cssRule, "200", 3))
        {
          weight.typed_data.value = -2;
          cssRule = SkipWord (cssRule);
        }
      else if (!strncasecmp (cssRule, "300", 3))
        {
          weight.typed_data.value = -1;
          cssRule = SkipWord (cssRule);
        }
      else if (!strncasecmp (cssRule, "400", 3))
        {
          weight.typed_data.value = 0;
          cssRule = SkipWord (cssRule);
        }
      else if (!strncasecmp (cssRule, "500", 3))
        {
          weight.typed_data.value = +1;
          cssRule = SkipWord (cssRule);
        }
      else if (!strncasecmp (cssRule, "600", 3))
        {
          weight.typed_data.value = +2;
          cssRule = SkipWord (cssRule);
        }
      else if (!strncasecmp (cssRule, "700", 3))
        {
          weight.typed_data.value = +3;
          cssRule = SkipWord (cssRule);
        }
      else if (!strncasecmp (cssRule, "800", 3))
        {
          weight.typed_data.value = +4;
          cssRule = SkipWord (cssRule);
        }
      else if (!strncasecmp (cssRule, "900", 3))
        {
          weight.typed_data.value = +5;
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
  else if (!strncasecmp (cssRule, "bolder", 6) ||
           !strncasecmp (cssRule, "lighter", 7))
    {
      /* not implemented */
      cssRule = SkipWord (cssRule);
      return (cssRule);
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
  cssRule = CheckImportantRule (cssRule, context);
  if (DoApply)
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
  
  ptr = cssRule;
  cssRule = ParseACSSFontWeight (element, tsch, context, cssRule, css, isHTML);
  if (ptr == cssRule)
    cssRule = SkipValue ("Invalid font-weight value", cssRule);
  cssRule = CheckImportantRule (cssRule, context);
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

  style.typed_data.value = 0;
  style.typed_data.unit = UNIT_REL;
  style.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  if (!strncasecmp (cssRule, "small-caps", 10))
    {
      /* Not supported yet */
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "normal", 6))
    {
      /* Not supported yet */
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      /* Not supported yet */
      cssRule = SkipWord (cssRule);
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
  cssRule = CheckImportantRule (cssRule, context);
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

  style.typed_data.value = 0;
  style.typed_data.unit = UNIT_REL;
  style.typed_data.real = FALSE;
  size.typed_data.value = 0;
  size.typed_data.unit = UNIT_REL;
  size.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
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
  cssRule = CheckImportantRule (cssRule, context);
  if (DoApply &&
      (style.typed_data.value != 0 || style.typed_data.unit == VALUE_INHERIT))
    {
      TtaSetStylePresentation (PRStyle, element, tsch, context, style);
    }
  if (size.typed_data.value != 0 && DoApply)
    {
      PresentationValue   previous_size;

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
  cssRule = CheckImportantRule (cssRule, context);
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
      ParseACSSFontFamily (element, tsch, context, cssRule, css, isHTML);
      cssRule += 7;
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
        ParseACSSFontVariant (element, tsch, context, "normal", css, isHTML);
      if (!style)
        ParseACSSFontStyle (element, tsch, context, "normal", css, isHTML);
      if (!weight)
        ParseACSSFontWeight (element, tsch, context, "normal", css, isHTML);
      /* now parse the font size and the font family */
      if (*cssRule != ';' && *cssRule != '}' && *cssRule != EOS)
        cssRule = ParseACSSFontSize (element, tsch, context, cssRule, css, isHTML, FALSE);
      if (*cssRule != ';' && *cssRule != '}' && *cssRule != EOS)
        cssRule = ParseACSSFontFamily (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
        {
          cssRule = SkipValue ("Invalid font value", cssRule);
          cssRule = CheckImportantRule (cssRule, context);
        }
    }
  cssRule = SkipBlanksAndComments (cssRule);
  if (*cssRule != ';' && *cssRule != '}' && *cssRule != EOS)
    {
      cssRule = SkipValue ("Invalid font value", cssRule);
      cssRule = CheckImportantRule (cssRule, context);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSTextDecoration: parse a CSS text decor string   
  we expect the input string describing the attribute to be     
  underline, overline, line-through, blink or none.
  ----------------------------------------------------------------------*/
static char *ParseCSSTextDecoration (Element element, PSchema tsch,
                                     PresentationContext context, char *cssRule,
                                     CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue   decor;
  char               *ptr = cssRule;

  decor.typed_data.value = 0;
  decor.typed_data.unit = UNIT_REL;
  decor.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  if (!strncasecmp (cssRule, "none", 4))
    {
      decor.typed_data.value = NoUnderline;
      cssRule += 4;
    }
  else if (!strncasecmp (cssRule, "underline", 9))
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
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      decor.typed_data.unit = VALUE_INHERIT;
      cssRule += 7;
    }
  else
    {
      cssRule = SkipValue ("Invalid text-decoration value", cssRule);
      cssRule = CheckImportantRule (cssRule, context);
      return (cssRule);
    }

  /*
   * install the new presentation.
   */
  cssRule = CheckImportantRule (cssRule, context);
  if (DoApply &&
      (decor.typed_data.value || decor.typed_data.unit == VALUE_INHERIT))
    TtaSetStylePresentation (PRUnderline, element, tsch, context, decor);
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

  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      val.typed_data.unit = VALUE_AUTO;
      val.typed_data.value = 0;
      val.typed_data.real = FALSE;
      cssRule += 4;
      cssRule = CheckImportantRule (cssRule, context);
      cssRule = CheckImportantRule (cssRule, context);
      cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid height value");
    }
  else
    cssRule = ParseCSSUnit (cssRule, &val);

  if (val.typed_data.value != 0 &&
      (val.typed_data.unit == UNIT_INVALID ||
       val.typed_data.unit == UNIT_BOX))
    {
      CSSParseError ("height value", ptr, cssRule);
      val.typed_data.unit = UNIT_PX;
    }
  cssRule = CheckImportantRule (cssRule, context);
  if (DoApply)
    /* install the new presentation */
    TtaSetStylePresentation (PRHeight, element, tsch, context, val);
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

  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      val.typed_data.unit = VALUE_AUTO;
      val.typed_data.value = 0;
      val.typed_data.real = FALSE;
      cssRule += 4;
      cssRule = CheckImportantRule (cssRule, context);
      cssRule = CSSCheckEndValue (ptr, cssRule, "Invalid width value");
    }
  else
    cssRule = ParseCSSUnit (cssRule, &val);
  if (val.typed_data.value != 0 &&
      (val.typed_data.unit == UNIT_INVALID ||
       val.typed_data.unit == UNIT_BOX))
    {
      CSSParseError ("Invalid width value", ptr, cssRule);
      cssRule = CheckImportantRule (cssRule, context);
      val.typed_data.unit = UNIT_PX;
    }

  cssRule = CheckImportantRule (cssRule, context);
  if (DoApply)
    /* install the new presentation */
    TtaSetStylePresentation (PRWidth, element, tsch, context, val);
  return (cssRule);
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
  
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      margin.typed_data.unit = VALUE_AUTO;
      margin.typed_data.value = 0;
      margin.typed_data.real = FALSE;
      cssRule += 4;
      cssRule = CheckImportantRule (cssRule, context);
    }
  else
    cssRule = ParseCSSUnit (cssRule, &margin);

  cssRule = CheckImportantRule (cssRule, context);
  if (margin.typed_data.value != 0 &&
      (margin.typed_data.unit == UNIT_INVALID ||
       margin.typed_data.unit == UNIT_BOX))
    CSSParseError ("Invalid margin-top value", ptr, cssRule);
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
  
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      margin.typed_data.unit = VALUE_AUTO;
      margin.typed_data.value = 0;
      margin.typed_data.real = FALSE;
      cssRule += 4;
      cssRule = CheckImportantRule (cssRule, context);
    }
  else
    cssRule = ParseCSSUnit (cssRule, &margin);

  cssRule = CheckImportantRule (cssRule, context);
  if (margin.typed_data.value != 0 &&
      (margin.typed_data.unit == UNIT_INVALID ||
       margin.typed_data.unit == UNIT_BOX))
    CSSParseError ("Invalid margin-bottom value", ptr, cssRule);
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
  
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      margin.typed_data.unit = VALUE_AUTO;
      margin.typed_data.value = 0;
      margin.typed_data.real = FALSE;
      cssRule += 4;
      cssRule = CheckImportantRule (cssRule, context);
    }
  else
    cssRule = ParseCSSUnit (cssRule, &margin);

  cssRule = CheckImportantRule (cssRule, context);
  if (margin.typed_data.value != 0 &&
      (margin.typed_data.unit == UNIT_INVALID ||
       margin.typed_data.unit == UNIT_BOX))
    CSSParseError ("Invalid margin-left value", ptr, cssRule);
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
  
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      margin.typed_data.unit = VALUE_AUTO;
      margin.typed_data.value = 0;
      margin.typed_data.real = FALSE;
      cssRule += 4;
      cssRule = CheckImportantRule (cssRule, context);
    }
  else
    cssRule = ParseCSSUnit (cssRule, &margin);

  cssRule = CheckImportantRule (cssRule, context);
  if (margin.typed_data.value != 0 &&
      (margin.typed_data.unit == UNIT_INVALID ||
       margin.typed_data.unit == UNIT_BOX))
    CSSParseError ("Invalid margin-right value", ptr, cssRule);
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
  int   skippedNL;

  ptrT = SkipBlanksAndComments (cssRule);
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
  
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  cssRule = ParseCSSUnit (cssRule, &padding);

  cssRule = CheckImportantRule (cssRule, context);
  if (padding.typed_data.value != 0 &&
      (padding.typed_data.unit == UNIT_INVALID ||
       padding.typed_data.unit == UNIT_BOX))
    {
      CSSParseError ("Invalid padding-top value", ptr, cssRule);
      padding.typed_data.value = 0;
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
  
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  cssRule = ParseCSSUnit (cssRule, &padding);
  if (padding.typed_data.value == 0)
    padding.typed_data.unit = UNIT_EM;

  cssRule = CheckImportantRule (cssRule, context);
  if (padding.typed_data.value != 0 &&
      (padding.typed_data.unit == UNIT_INVALID ||
       padding.typed_data.unit == UNIT_BOX))
    {
      CSSParseError ("Invalid padding-bottom value", ptr, cssRule);
      padding.typed_data.value = 0;
    }
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
  
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  cssRule = ParseCSSUnit (cssRule, &padding);
  if (padding.typed_data.value == 0)
    padding.typed_data.unit = UNIT_EM;

  cssRule = CheckImportantRule (cssRule, context);
  if (padding.typed_data.value != 0 &&
      (padding.typed_data.unit == UNIT_INVALID ||
       padding.typed_data.unit == UNIT_BOX))
    {
      CSSParseError ("Invalid padding-left value", ptr, cssRule);
      padding.typed_data.value = 0;
    }
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
  
  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  cssRule = ParseCSSUnit (cssRule, &padding);
  if (padding.typed_data.value == 0)
    padding.typed_data.unit = UNIT_EM;

  cssRule = CheckImportantRule (cssRule, context);
  if (padding.typed_data.value != 0 &&
      (padding.typed_data.unit == UNIT_INVALID ||
       padding.typed_data.unit == UNIT_BOX))
    {
      CSSParseError ("Invalid padding-right value", ptr, cssRule);
      padding.typed_data.value = 0;
    }
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
  int   skippedNL;

  ptrT = SkipBlanksAndComments (cssRule);
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
  PresentationValue   best;
  char               *p;

  p = cssRule;
  cssRule = ParseCSSColor (cssRule, &best);
  cssRule = CheckImportantRule (cssRule, context);
  if (best.typed_data.unit != UNIT_INVALID && DoApply)
    {
      if (*cssRule != EOS && *cssRule !=';')
        {
          cssRule = SkipProperty (cssRule, FALSE);
          CSSParseError ("Invalid value", p, cssRule);
        }
      else
        /* install the new presentation */
        TtaSetStylePresentation (PRForeground, element, tsch, context, best);
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

  best.typed_data.unit = UNIT_INVALID;
  best.typed_data.real = FALSE;
  if (!strncasecmp (cssRule, "transparent", 11))
    {
      best.typed_data.value = PATTERN_NONE;
      best.typed_data.unit = UNIT_REL;
      cssRule = SkipWord (cssRule);
      cssRule = CheckImportantRule (cssRule, context);
      if (DoApply)
        TtaSetStylePresentation (PRFillPattern, element, tsch, context, best);
    }
  else
    {
      cssRule = ParseCSSColor (cssRule, &best);
      cssRule = CheckImportantRule (cssRule, context);
      if (best.typed_data.unit != UNIT_INVALID && DoApply)
        {
          /* install the new presentation. */
          TtaSetStylePresentation (PRBackground, element, tsch, context, best);
          /* thot specificity: need to set fill pattern for background color */
          best.typed_data.value = PATTERN_BACKGROUND;
          best.typed_data.unit = UNIT_REL;
          TtaSetStylePresentation (PRFillPattern, element, tsch, context, best);
          best.typed_data.value = 1;
          best.typed_data.unit = UNIT_REL;
          TtaSetStylePresentation (PRShowBox, element, tsch, context, best);
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
  PresentationValue     best;
  char                  *url;

  best.typed_data.unit = UNIT_INVALID;
  best.typed_data.real = FALSE;
  if (!strncasecmp (cssRule, "none", 4))
    {
      best.typed_data.value = -2;  /* -2 means transparent */
      best.typed_data.unit = UNIT_REL;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "currentColor", 12))
    {
      best.typed_data.unit = VALUE_INHERIT;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "url", 3))
    {  
      cssRule += 3;
      cssRule = ParseCSSUrl (cssRule, &url);
      /* **** do something with the url ***** */;
      TtaFreeMemory (url);
      /* **** caution: another color value may follow the uri (in case
         the uri could ne be dereferenced) *** */
    }
  else
    cssRule = ParseCSSColor (cssRule, &best);

  cssRule = CheckImportantRule (cssRule, context);
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
  PresentationValue     best;
  char                  *url;

  best.typed_data.unit = UNIT_INVALID;
  best.typed_data.real = FALSE;
  if (!strncasecmp (cssRule, "none", 4))
    {
      best.typed_data.value = PATTERN_NONE;
      best.typed_data.unit = UNIT_REL;
      cssRule = CheckImportantRule (cssRule, context);
      if (DoApply)
        TtaSetStylePresentation (PRFillPattern, element, tsch, context, best);
      cssRule = SkipWord (cssRule);
      return (cssRule);
    }
  else if (!strncasecmp (cssRule, "currentColor", 12))
    {
      best.typed_data.unit = VALUE_INHERIT;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "url", 3))
    {  
      cssRule += 3;
      cssRule = ParseCSSUrl (cssRule, &url);
      /* **** do something with the url ***** */;
      TtaFreeMemory (url);
      /* **** caution: another color value may follow the uri (in case
         the uri could ne be dereferenced) *** */
    }
  else
    cssRule = ParseCSSColor (cssRule, &best);

  cssRule = CheckImportantRule (cssRule, context);
  if (best.typed_data.unit != UNIT_INVALID && DoApply)
    {
      /* install the new presentation. */
      TtaSetStylePresentation (PRBackground, element, tsch, context, best);
      /* thot specificity: need to set fill pattern for background color */
      best.typed_data.value = PATTERN_BACKGROUND;
      best.typed_data.unit = UNIT_REL;
      TtaSetStylePresentation (PRFillPattern, element, tsch, context, best);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseSVGOpacity: parse a SVG opacity property
  ----------------------------------------------------------------------*/
static char *ParseSVGOpacity (Element element, PSchema tsch,
                              PresentationContext context, char *cssRule,
                              CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue     best;

  best.typed_data.unit = UNIT_INVALID;
  best.typed_data.real = FALSE;
  cssRule = ParseClampedUnit (cssRule, &best);
  cssRule = CheckImportantRule (cssRule, context);
  if (DoApply)
    /* install the new presentation. */
    TtaSetStylePresentation (PROpacity, element, tsch, context, best);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseSVGStrokeOpacity: parse a SVG stroke-opacity property
  ----------------------------------------------------------------------*/
static char *ParseSVGStrokeOpacity (Element element, PSchema tsch,
                                    PresentationContext context, char *cssRule,
                                    CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue     best;

  best.typed_data.unit = UNIT_INVALID;
  best.typed_data.real = FALSE;
  cssRule = ParseClampedUnit (cssRule, &best);
  cssRule = CheckImportantRule (cssRule, context);
  if (DoApply)
    /* install the new presentation. */
    TtaSetStylePresentation (PRStrokeOpacity, element, tsch, context, best);
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseSVGFillOpacity: parse a SVG fil-opacityl property
  ----------------------------------------------------------------------*/
static char *ParseSVGFillOpacity (Element element, PSchema tsch,
                                  PresentationContext context, char *cssRule,
                                  CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue     best;

  best.typed_data.unit = UNIT_INVALID;
  best.typed_data.real = FALSE;
  cssRule = ParseClampedUnit (cssRule, &best);
  cssRule = CheckImportantRule (cssRule, context);
  if (DoApply)
    /* install the new presentation. */
    TtaSetStylePresentation (PRFillOpacity, element, tsch, context, best);
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
  PresentationValue   value;
  char                *last, *start, quoteChar, savedChar;
  int                 length, val;
  unsigned char       *buffer, *p;
  ThotBool            repeat;

  value.typed_data.unit = UNIT_REL;
  value.typed_data.real = FALSE;
  value.typed_data.value = 0;
  if (DoApply)
    TtaSetStylePresentation (PRContent, element, tsch, ctxt, value);
  cssRule = SkipBlanksAndComments (cssRule);
  repeat = TRUE;
  while (repeat)
    {
      p = (unsigned char*) cssRule;
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
          last = cssRule;
          last = SkipString (last);
          length = last - cssRule;
          /* get a buffer to store the string */
          buffer = (unsigned char*) TtaGetMemory (length);
          p = buffer; /* beginning of the string */
          cssRule++;
          while (*cssRule != EOS && *cssRule != quoteChar)
            {
              if (*cssRule == '\\')
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
                      TtaWCToMBstring ((wchar_t) val, &p);
                      *cssRule = savedChar;
                    }
                  else
                    {
                      *p = *cssRule;
                      p++; cssRule++;
                    }
                }
              else
                {
                  *p = *cssRule;
                  p++; cssRule++;
                }
            }
          *p = EOS;
          if (*cssRule != quoteChar)
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
        }
      else if (!strncasecmp (cssRule, "counter", 7))
        {
          cssRule += 7;
          /* @@@@@@ */
          cssRule = SkipProperty (cssRule, FALSE);
        }
      else if (!strncasecmp (cssRule, "counters", 8))
        {
          cssRule += 8;
          /* @@@@@@ */
          cssRule = SkipProperty (cssRule, FALSE);
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
                  if (last[-1] == SPACE)
                    {
                      *last = SPACE;
                      last--;
                      while (last[-1] == SPACE)
                        last--;
                    }
                  savedChar = *last;
                  *last = EOS;
                  value.typed_data.unit = UNIT_REL;
                  value.typed_data.real = FALSE;
                  value.pointer = start;
                  if (DoApply)
                    TtaSetStylePresentation (PRContentAttr, element, tsch,
                                             ctxt, value);
                  *last = savedChar;
                }
            }
          if (value.pointer == NULL)
            {
              CSSParseError ("Invalid content value", (char*) p, cssRule);
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
          cssRule = SkipProperty (cssRule, FALSE);
        }
      cssRule = SkipBlanksAndComments (cssRule);
      if (repeat)
        if (*cssRule == ';' || *cssRule == '}' || *cssRule == EOS ||
            *cssRule == '!')
          repeat = FALSE;
    }
  cssRule = CheckImportantRule (cssRule, ctxt);
  return (cssRule);
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

  cssRule = SkipBlanksAndComments (cssRule);
  if (!strncasecmp (cssRule, "none", 4))
    {
      cssRule += 4;
      cssRule = CheckImportantRule (cssRule, ctxt);
      if (DoApply)
        {
          /* no background image */
          image.pointer = NULL;
          TtaSetStylePresentation (PRBackgroundPicture, element, tsch, ctxt,
                                   image);
        }
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
  cssRule = CheckImportantRule (cssRule, ctxt);
  if (DoApply)
    /* install the new presentation */
    TtaSetStylePresentation (PRPictureMode, element, tsch, ctxt, repeat);
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
  cssRule = ParseACSSBackgroundRepeat (element, tsch, ctxt,
                                       cssRule, css, isHTML);
  if (cssRule)
    {
      cssRule = SkipValue ("Invalid background-repeat value", cssRule);
      /* check if it's an important rule */
      cssRule = CheckImportantRule (cssRule, ctxt);
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
  cssRule = SkipBlanksAndComments (cssRule);
  if (!strncasecmp (cssRule, "scroll", 6))
    {
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "fixed", 5))
    {
      cssRule = SkipWord (cssRule);
    }
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
    {
      cssRule = SkipValue ("Invalid background-attachement value", cssRule);
      /* check if it's an important rule */
      cssRule = CheckImportantRule (cssRule, ctxt);
    }
  return cssRule;
}

/*----------------------------------------------------------------------
  ParseACSSBackgroundPosition: parse a CSS BackgroundPosition
  attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseACSSBackgroundPosition (Element element, PSchema tsch,
                                          PresentationContext ctxt,
                                          char *cssRule, CSSInfoPtr css,
                                          ThotBool isHTML)
{
  ThotBool              ok;

  cssRule = SkipBlanksAndComments (cssRule);
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
  else if (isdigit (*cssRule) || *cssRule == '.' || *cssRule == '-')
    {
      while (*cssRule != EOS && *cssRule != SPACE &&
             *cssRule != ',' && *cssRule != ';')
        cssRule++;
    }
  else
    ok = FALSE;

  if (ok)
    {
      /* check if it's an important rule */
      cssRule = CheckImportantRule (cssRule, ctxt);
    }
  cssRule = SkipBlanksAndComments (cssRule);
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

  ptr = cssRule;
  cssRule = ParseACSSBackgroundPosition (element, tsch, ctxt,
                                         cssRule, css, isHTML);
  if (ptr == cssRule)
    {
      cssRule = SkipValue ("Invalid background-position value", cssRule);
      /* check if it's an important rule */
      cssRule = CheckImportantRule (cssRule, ctxt);
    }
  else if (*cssRule !=  ';' && *cssRule != EOS)
    {
      /* possible second value */
      ptr = cssRule;
      cssRule = ParseACSSBackgroundPosition (element, tsch, ctxt,
                                             cssRule, css, isHTML);
      if (ptr == cssRule)
        {
          cssRule = SkipValue ("Invalid background-position value", cssRule);
          /* check if it's an important rule */
          cssRule = CheckImportantRule (cssRule, ctxt);
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
  ThotBool        img, repeat, position, attach, color;

  cssRule = SkipBlanksAndComments (cssRule);
  img = repeat = position = attach = color = FALSE;
  while (*cssRule != ';' && *cssRule != '}' && *cssRule != EOS && *cssRule != ',')
    {
      /* perhaps a Background Image */
      if (!strncasecmp (cssRule, "url", 3) || !strncasecmp (cssRule, "none", 4))
        {
          if (!strncasecmp (cssRule, "none", 4))
            repeat = TRUE;
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
          cssRule = ParseACSSBackgroundPosition (element, tsch, ctxt,
                                                 cssRule, css, isHTML);
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
    ParseCSSBackgroundImage (element, tsch, ctxt, "none", css, isHTML);
  
  if (img && !repeat)
    ParseACSSBackgroundRepeat (element, tsch, ctxt,
                               "repeat", css, isHTML);
  if (img && !position)
    ParseACSSBackgroundPosition (element, tsch, ctxt,
                                 "0% 0%", css, isHTML);
  if (img && !attach)
    ParseACSSBackgroundAttachment (element, tsch, ctxt,
                                   "scroll", css, isHTML);
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

  page.typed_data.unit = UNIT_INVALID;
  page.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
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
  /* check if it's an important rule */
  cssRule = CheckImportantRule (cssRule, ctxt);
  /* install the new presentation */
  if (DoApply &&
      ((page.typed_data.unit == UNIT_REL && page.typed_data.value == PageAlways)
       || page.typed_data.unit == VALUE_INHERIT))
    TtaSetStylePresentation (PRPageBefore, element, tsch, ctxt, page);
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

  page.typed_data.unit = UNIT_INVALID;
  page.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
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
  /* check if it's an important rule */
  cssRule = CheckImportantRule (cssRule, ctxt);
#if 0
  /* install the new presentation */
  if (DoApply &&
      (page.typed_data.unit == UNIT_REL ||
       page.typed_data.unit == VALUE_INHERIT))
    /* TtaSetStylePresentation (PRPageAfter, element, tsch, ctxt, page) */;
#endif /* 0 */
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

  page.typed_data.unit = UNIT_INVALID;
  page.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
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
  cssRule = CheckImportantRule (cssRule, ctxt);
  /* install the new presentation */
  /*if ((page.typed_data.unit == UNIT_REL ||
    page.typed_data.unit == VALUE_INHERIT) &&
    page.typed_data.value == PageAvoid && DoApply)
    TtaSetStylePresentation (PRPageInside, element, tsch, ctxt, page);*/
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

  /* check if it's an important rule */
  cssRule = CheckImportantRule (cssRule, ctxt);
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
      cssRule = CheckImportantRule (cssRule, ctxt);
      cssRule = SkipValue (NULL, cssRule);
    }
  else
    {
      cssRule = CheckImportantRule (cssRule, ctxt);
      cssRule = SkipBlanksAndComments (cssRule);
      if (*cssRule != EOS && *cssRule != ';')
        SkipValue ("Invalid position value", ptr);
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

  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the value */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      val.typed_data.unit = VALUE_AUTO;
      val.typed_data.value = 0;
      val.typed_data.real = FALSE;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      val.typed_data.unit = VALUE_INHERIT;
      cssRule = SkipWord (cssRule);
    }
  else
    cssRule = ParseCSSUnit (cssRule, &val);
  if (val.typed_data.value != 0 &&
      (val.typed_data.unit == UNIT_INVALID ||
       val.typed_data.unit == UNIT_BOX))
    {
      cssRule = SkipValue ("top value", ptr);
      val.typed_data.unit = UNIT_PX;
    }
  cssRule = CheckImportantRule (cssRule, context);
  if (DoApply)
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

  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      val.typed_data.unit = VALUE_AUTO;
      val.typed_data.value = 0;
      val.typed_data.real = FALSE;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      val.typed_data.unit = VALUE_INHERIT;
      cssRule = SkipWord (cssRule);
    }
  else
    cssRule = ParseCSSUnit (cssRule, &val);
  if (val.typed_data.value != 0 &&
      (val.typed_data.unit == UNIT_INVALID ||
       val.typed_data.unit == UNIT_BOX))
    {
      cssRule = SkipValue ("right value", ptr);
      val.typed_data.unit = UNIT_PX;
    }
  cssRule = CheckImportantRule (cssRule, context);
  if (DoApply)
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

  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      val.typed_data.unit = VALUE_AUTO;
      val.typed_data.value = 0;
      val.typed_data.real = FALSE;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      val.typed_data.unit = VALUE_INHERIT;
      cssRule = SkipWord (cssRule);
    }
  else
    cssRule = ParseCSSUnit (cssRule, &val);
  if (val.typed_data.value != 0 &&
      (val.typed_data.unit == UNIT_INVALID ||
       val.typed_data.unit == UNIT_BOX))
    {
      cssRule = SkipValue ("bottom value", ptr);
      val.typed_data.unit = UNIT_PX;
    }
  cssRule = CheckImportantRule (cssRule, context);
  if (DoApply)
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

  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4))
    {
      val.typed_data.unit = VALUE_AUTO;
      val.typed_data.value = 0;
      val.typed_data.real = FALSE;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      val.typed_data.unit = VALUE_INHERIT;
      cssRule = SkipWord (cssRule);
    }
  else
    cssRule = ParseCSSUnit (cssRule, &val);
  if (val.typed_data.value != 0 &&
      (val.typed_data.unit == UNIT_INVALID ||
       val.typed_data.unit == UNIT_BOX))
    {
      cssRule = SkipValue ("left value", ptr);
      val.typed_data.unit = UNIT_PX;
    }
  cssRule = CheckImportantRule (cssRule, context);
  if (DoApply)
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

  cssRule = SkipBlanksAndComments (cssRule);
  ptr = cssRule;
  /* first parse the attribute string */
  if (!strncasecmp (cssRule, "auto", 4) ||
      !strncasecmp (cssRule, "inherit", 7))
    {
      val.typed_data.unit = VALUE_AUTO;
      val.typed_data.value = 0;
      val.typed_data.real = FALSE;
      cssRule = SkipWord (cssRule);
    }
  else
    {
      cssRule = ParseCSSUnit (cssRule, &val);
      if (val.typed_data.unit != UNIT_BOX)
        {
          cssRule = SkipValue ("z-index value", ptr);
          val.typed_data.unit = UNIT_BOX;
        }
    }
  cssRule = CheckImportantRule (cssRule, context);
  /***
      if (DoApply)
      TtaSetStylePresentation (PR, element, tsch, context, val);
  ***/
  return (cssRule);
}

/*----------------------------------------------------------------------
 *
 *	FUNCTIONS STYLE DECLARATIONS
 *
 *----------------------------------------------------------------------*/
/*
 * NOTE: Long attribute name MUST be placed before shortened ones !
 *        e.g. "FONT-SIZE" must be placed before "FONT"
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
    {"width", ParseCSSWidth},
    {"visibility", ParseCSSVisibility},
    {"word-spacing", ParseCSSWordSpacing},
    {"z-index", ParseCSSZIndex},

    /* SVG extensions */
    {"fill-opacity", ParseSVGFillOpacity},
    {"fill", ParseSVGFill},
    {"opacity", ParseSVGOpacity},
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
static void  ParseCSSRule (Element element, PSchema tsch,
                           PresentationContext ctxt, char *cssRule,
                           CSSInfoPtr css, ThotBool isHTML)
{
  DisplayMode         dispMode;
  char               *p = NULL, *next, *end;
  char               *valueStart;
  int                 lg;
  unsigned int        i;
  ThotBool            found;

  /* avoid too many redisplay */
  dispMode = TtaGetDisplayMode (ctxt->doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (ctxt->doc, DeferredDisplay);

  while (*cssRule != EOS)
    {
      cssRule = SkipBlanksAndComments (cssRule);
      if (*cssRule == '#')
        {
          end = SkipProperty (cssRule, FALSE);
          CSSParseError ("Invalid property",
                         cssRule, end);
          cssRule = end; 
        }
      else if (*cssRule < 0x41 || *cssRule > 0x7A ||
          (*cssRule > 0x5A && *cssRule < 0x60))
        cssRule++;
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
            cssRule = SkipProperty (cssRule, TRUE);
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
                        CSSParseError ("Missing closing ';'", cssRule, p);
                      cssRule = next;
                    }
                }
              else
                cssRule = SkipProperty (cssRule, TRUE);
            }
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
  if (dispMode == DisplayImmediately)
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
  ThotBool            isHTML;

  /*  A rule applying to BODY is really meant to address HTML */
  elType = TtaGetElementType (el);
  NewLineSkipped = 0;
  /* store the current line for eventually reported errors */
  LineNumber = TtaGetElementLineNumber (el);
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
  ParseCSSRule (el, NULL, (PresentationContext) ctxt, cssRule, NULL, isHTML);
  /* restore the display mode if necessary */
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
  char              *deb, *cur, *sel, *next, c;
  char              *schemaName, *mappedName, *saveURL;
  char              *names[MAX_ANCESTORS];
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
  ThotBool           isHTML;
  ThotBool           level, quoted, doubleColon;
#define ATTR_ID 1
#define ATTR_CLASS 2
#define ATTR_PSEUDO 3

  sel = ctxt->sel;
  sel[0] = EOS;
  specificity = 0;
  for (i = 0; i < MAX_ANCESTORS; i++)
    {
      names[i] = NULL;
      rel[i] = RelAncestor;
      attrnames[i] = NULL;
      attrnums[i] = 0;
      attrlevels[i] = 0;
      attrvals[i] = NULL;
      attrmatch[i] = Txtmatch;
      ctxt->name[i] = 0;
      ctxt->attrType[i] = 0;
      ctxt->attrLevel[i] = 0;
      ctxt->attrText[i] = NULL;
      ctxt->attrMatch[i] = Txtmatch;
    }
  ctxt->box = 0;
  ctxt->var = 0;
  ctxt->pseudo = PbNone;
  ctxt->type = 0;
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
             !TtaIsBlank (selector))
        *cur++ = *selector++;
      *cur++ = EOS; /* close the first string  in sel[] */
      if (deb[0] != EOS)
        /* the selector starts with an element name */
        {
          if (deb[0] <= 64 && deb[0] != '*')
            {
              CSSPrintError ("Invalid element", deb);
              return NULL;
            }
          else
            {
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
                     !TtaIsBlank (selector))
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
                     !TtaIsBlank (selector))
                *cur++ = *selector++;
              /* close the word */
              *cur++ = EOS;
              /* point to the pseudo-class or pseudo-element in sel[] if it's
                 a valid name */
              if (deb[0] <= 64)
                {
                  CSSPrintError ("Invalid pseudo-element", deb);
                  DoApply = FALSE;
                }
              else
                {
                  if (!strcmp (deb, "first-letter") ||
                      !strcmp (deb, "first-line"))
                    {
                      if (doubleColon)
                        CSSPrintError ("Warning: \"::\" is CSS3 syntax", NULL);
                      /* not supported */
                      DoApply = FALSE;
                    }
                  else if (!strcmp (deb, "hover") ||
                           !strcmp (deb, "focus"))
                    /* not supported */
                    DoApply = FALSE;
                  else
                    specificity += 10;
                  if (!strncmp (deb, "before", 6))
                    {
                      if (doubleColon)
                        CSSPrintError ("Warning: \"::before\" is CSS3 syntax",
                                     NULL);
                      ctxt->pseudo = PbBefore;
                    }
                  else if (!strncmp (deb, "after", 5))
                    {
                      if (doubleColon)
                        CSSPrintError ("Warning: \"::after\" is CSS3 syntax",
                                       NULL);
                      ctxt->pseudo = PbAfter;
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
                        }
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
                      attrnums[0] = ATTR_PSEUDO;
                      attrlevels[0] = 0;
                      attrmatch[0] = Txtmatch;
                      attrvals[0] = deb;
                    }
                  if (names[0] && !strcmp (names[0], "*"))
                    names[0] = NULL;
                }
            }
          else if (*selector == '#')
            /* unique identifier */
            {
              selector++;
              while (*selector != '.' && *selector != ':' &&
                     *selector != '#' && *selector != '[' &&
                     *selector != '+' && *selector != '>' &&
                     *selector != EOS && *selector != ',' &&
                     !TtaIsBlank (selector))
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
                     !TtaIsBlank (selector))
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
                  specificity += 10;
                  /* check matching */
                  selector = SkipBlanksAndComments (selector);
                  if (*selector == '~')
                    {
                      attrmatch[0] = Txtword;
                      selector++;
                    }
                  else if (*selector == '|')
                    {
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
                  while ((quoted &&
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
                     !TtaIsBlank (selector))
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
              /*CSSPrintError ("Syntax error:", selector);*/
              return NULL;
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
      if (!names[i])
        ctxt->name[i] = HTML_EL_ANY_TYPE;
      else
        /* store element information */
        {
          /* get the element type of this name in the current document */
          if (xmlType == XML_TYPE)
            /* it's a generic XML document. Check the main document schema */
            {
              elType.ElSSchema = TtaGetDocumentSSchema (doc);
              TtaGetXmlElementType (names[i], &elType, &mappedName, doc);
              if (!elType.ElTypeNum)
                {
                  if (!strcmp (names[i], "*"))
                    elType.ElTypeNum = HTML_EL_ANY_TYPE;
                  else
                    elType.ElSSchema = NULL;
                }
            }
          else
            {
              if (!strcmp (names[i], "*"))
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
                      TtaAppendXmlElement (names[i], &elType, &mappedName, doc);
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
                  if (att == 0 && ctxt->schema == NULL)
                    /* Attribute name not found: Search in the list of all
                       schemas loaded for this document */
                    {
                      attrType.AttrSSchema = NULL;
                      TtaGetXmlAttributeType (attrnames[j], &attrType, doc);
                      att = attrType.AttrTypeNum;
                      if (att != 0)
                        ctxt->schema = attrType.AttrSSchema;
                    }
                  attrType.AttrSSchema = ctxt->schema;
                  attrType.AttrTypeNum = att;
                  if (i == 0 && att == 0 && ctxt->schema == NULL)
                    {
                      /* Not found -> search in the list of loaded schemas */
                      attrType.AttrSSchema = NULL;
                      TtaGetXmlAttributeType (attrnames[j], &attrType, doc);
                      att = attrType.AttrTypeNum;
                      if (attrType.AttrSSchema)
                        /* the element type concerns an imported nature */
                        schemaName = TtaGetSSchemaName(attrType.AttrSSchema);
#ifdef XML_GENERIC
                      else if (xmlType == XML_TYPE)
                        {
                          /* The attribute is not yet present in the tree */
                          /* Create a new global attribute */
                          attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
                          TtaAppendXmlAttribute (attrnames[j], &attrType, doc);
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
  if (tsch && cssRule)
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
         *sel_end == EOL || *sel_end == CR)
    sel_end--;
  sel_end++;
  *sel_end = EOS;
  selector = cssRule;

  /* now, deal with the content ... */
  decl_end++;
  cssRule = decl_end;
  decl_end = &cssRule[strlen (cssRule) - 1];
  if (*decl_end != '{')
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
    selector = ParseGenericSelector (selector, cssRule, ctxt, doc, css,
                                     link, url);
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
  HTMLSetBackgroundColor:
  ----------------------------------------------------------------------*/
void HTMLSetBackgroundColor (Document doc, Element el, int specificity,
                             char *color)
{
  char             css_command[1000];

  sprintf (css_command, "background-color: %s", color);
  ParseHTMLSpecificStyle (el, css_command, doc, specificity, FALSE);
}

/*----------------------------------------------------------------------
  HTMLSetForegroundColor:                                        
  ----------------------------------------------------------------------*/
void HTMLSetForegroundColor (Document doc, Element el, int specificity,
                             char *color)
{
  char           css_command[1000];

  sprintf (css_command, "color: %s", color);
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

  sprintf (css_command, "background-image: url(xx); background-repeat: repeat");
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

  sprintf (css_command, ":link { color: %s }", color);
  ApplyCSSRules (el, css_command, doc, FALSE);
}

/*----------------------------------------------------------------------
  HTMLSetAactiveColor:                                           
  ----------------------------------------------------------------------*/
void HTMLSetAactiveColor (Document doc, Element el, char *color)
{
  char           css_command[1000];

  sprintf (css_command, ":active { color: %s }", color);
  ApplyCSSRules (el, css_command, doc, FALSE);
}

/*----------------------------------------------------------------------
  HTMLSetAvisitedColor:                                          
  ----------------------------------------------------------------------*/
void HTMLSetAvisitedColor (Document doc, Element el, char *color)
{
  char           css_command[1000];

  sprintf (css_command, ":visited { color: %s }", color);
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
  int                 openRule;
  int                 newlines;
  ThotBool            HTMLcomment;
  ThotBool            toParse, eof, quoted, s_quoted;
  ThotBool            ignore, media, page, lineComment;
  ThotBool            noRule, ignoreImport, fontface;

  CSScomment = MAX_CSS_LENGTH;
  HTMLcomment = FALSE;
  CSSindex = 0;
  toParse = FALSE;
  noRule = FALSE;
  media = FALSE;
  ignoreImport = FALSE;
  ignore = lineComment = FALSE;
  page = FALSE;
  quoted = s_quoted = FALSE;
  fontface = FALSE;
  eof = FALSE;
  openRule = 0;
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
  ParsedDoc = docRef;
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
                  CSSbuffer[CSSindex - 1] == '*')
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
              if (!quoted && !s_quoted && import != MAX_CSS_LENGTH && openRule == 0)
                media = !strncasecmp (&CSSbuffer[import+1], "media", 5);
              break;
            case '{':
              if (!quoted && !s_quoted)
                {
                  openRule++;
                  if (import != MAX_CSS_LENGTH)
                    {
                      if (openRule == 1 && media)
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
                        {
                          page = TRUE;
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
                  openRule--;
                  if (page)
                    {
                      noRule = TRUE;
                      page = FALSE; /* close the page section */
                    }
                  else if (fontface)
                    {
                      noRule = TRUE;
                      fontface = FALSE; /* close the fontface section */
                    }
                  else if (openRule == 0 && import != MAX_CSS_LENGTH)
                    {
                      import = MAX_CSS_LENGTH;
                      noRule = TRUE;
                      ignore = FALSE;
                      media = FALSE;
                    }
                  else
                    toParse = TRUE;
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
          c = CR;
        }

      if (!lineComment && c != CR)
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
                  cssRule = TtaSkipBlanks (cssRule);
                  /* save the current line number */
                  newlines += LineNumber;
                  if (!strncasecmp (cssRule, "url", 3))
                    {
                      cssRule = &cssRule[3];
                      cssRule = TtaSkipBlanks (cssRule);
                      if (*cssRule == '(')
                        {
                          cssRule++;
                          cssRule = TtaSkipBlanks (cssRule);
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
                      cssRule = TtaSkipBlanks (cssRule);
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
                  cssRule = TtaSkipBlanks (cssRule);
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
          RedisplayDoc = 0;
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
