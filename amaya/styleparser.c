/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2003
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

typedef struct _BackgroundImageCallbackBlock
{
  Element                     el;
  PSchema                     tsch;
  CSSInfoPtr                  css;
  union
  {
    PresentationContextBlock  specific;
    GenericContextBlock       generic;
  } context;
}
BackgroundImageCallbackBlock, *BackgroundImageCallbackPtr;

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

struct unit_def
{
   char               *sign;
   unsigned int        unit;
};

static struct unit_def CSSUnitNames[] =
{
   {"pt", UNIT_PT},
   {"pc", UNIT_PC},
   {"in", UNIT_IN},
   {"cm", UNIT_CM},
   {"mm", UNIT_MM},
   {"em", UNIT_EM},
   {"px", UNIT_PX},
   {"ex", UNIT_XHEIGHT},
   {"%", UNIT_PERCENT}
};

#define NB_UNITS (sizeof(CSSUnitNames) / sizeof(struct unit_def))
static char         *DocURL = NULL; /* The parsed CSS file */
static Document      ParsedDoc; /* The document to which CSS are to be applied */
static int           LineNumber = -1; /* The line where the error occurs */
static int           NewLineSkipped = 0;
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

      if (DocURL)
	{
	  fprintf (ErrFile, "*** Errors/warnings in %s\n", DocURL);
	  /* set to NULL as long as the CSS file doesn't change */
	  DocURL = NULL;
	}
      CSSErrorsFound = TRUE;
      if (LineNumber < 0)
	fprintf (ErrFile, "  In Style attribute, %s \"%s\"\n", msg, value);
      else
	fprintf (ErrFile, "  line %d: %s \"%s\"\n", LineNumber+NewLineSkipped,
		 msg, value);
    }
}

/*----------------------------------------------------------------------
   CSSParseError
   print the error message msg on stderr.
   When the line is 0 ask to expat the current line number
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
   SkipProperty skips a property and display and error message
  ----------------------------------------------------------------------*/
static char *SkipProperty (char *ptr)
{
  char       *deb;
  char        c;

  deb = ptr;
  while (*ptr != EOS && *ptr != ';' && *ptr != '}')
    {
      if (*ptr == '"' && (ptr == deb || ptr[-1] != '\\'))
	{
	  /* skip to the end of the string "..." */
	  ptr++;
	  while (*ptr != EOS &&
		 (*ptr != '"' || (*ptr == '"' && ptr[-1] == '\\')))
	    ptr++;
	}
      ptr++;
    }
  /* print the skipped property */
  c = *ptr;
  *ptr = EOS;
#ifdef CSS_WARNING
  if (*deb != EOS)
    CSSPrintError ("CSS property ignored", deb);
#endif /* CSS_WARNING */
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
  while (*ptr != EOS && *ptr != ';' && *ptr != '}')
    {
      if (*ptr == '"' && (ptr == deb || ptr[-1] != '\\'))
	{
	  /* skip to the end of the string "..." */
	  ptr++;
	  while (*ptr != '"' || (ptr[0] == '"' && ptr[-1] == '\\'))
	    ptr++;
	}
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
   ParseClampedUnit:                                                  
   parse a CSS Unit substring and returns the corresponding      
   value and its unit.
   [0,1]
   or
   [0,100] in %
  ----------------------------------------------------------------------*/
char *ParseClampedUnit (char *text, PresentationValue *pval)
{
  float                clamped_value;
  int                  int_clamped_value;

  if (*(text + strlen (text) -1) == '%')
    {
      int_clamped_value = atoi (text);
      if (int_clamped_value < 0 || int_clamped_value > 100)
	int_clamped_value = 1000;
      else
	int_clamped_value = int_clamped_value * 10;
    }
  else
    {
      clamped_value = (float) atof (text);
      if (clamped_value < 0.0 || clamped_value > 1.0)
	int_clamped_value = 1000;
      else 
	int_clamped_value = (int) (clamped_value * 1000.);
    }
  pval->typed_data.unit = UNIT_REL;
  pval->typed_data.value = int_clamped_value;
  pval->typed_data.real = FALSE;
  pval->data = int_clamped_value;   
  return (SkipWord (text));
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
      for (uni = 0; uni < NB_UNITS; uni++)
	{
	  if (!strncasecmp (CSSUnitNames[uni].sign, cssRule,
			     strlen (CSSUnitNames[uni].sign)))
	    {
	      pval->typed_data.unit = CSSUnitNames[uni].unit;
	      return (cssRule + strlen (CSSUnitNames[uni].sign));
	    }
	}
      /* not in the list of predefined units */
      pval->typed_data.unit = UNIT_BOX;
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseBorderValue                                       
  ----------------------------------------------------------------------*/
static char *ParseBorderValue (char *cssRule, PresentationValue *border)
{
  char             *ptr;

  /* first parse the attribute string */
   border->typed_data.value = 0;
   border->typed_data.unit = UNIT_INVALID;
   border->typed_data.real = FALSE;
   if (!strncasecmp (cssRule, "thin", 4))
     {
       border->typed_data.unit = UNIT_PX;
       border->typed_data.value = 1;
       cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "medium", 6))
     {
       border->typed_data.unit = UNIT_PX;
       border->typed_data.value = 3;
       cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "thick", 5))
     {
       border->typed_data.unit = UNIT_PX;
       border->typed_data.value = 5;
       cssRule = SkipWord (cssRule);
     }
   else if (isdigit (*cssRule) || *cssRule == '.')
     {
       ptr = cssRule;
       cssRule = ParseCSSUnit (cssRule, border);
       if (border->typed_data.value == 0)
	 border->typed_data.unit = UNIT_PX;
       else if (border->typed_data.unit == UNIT_INVALID ||
		border->typed_data.unit == UNIT_BOX)
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
     border->typed_data.value = BorderStyleNone;
   else if (!strncasecmp (cssRule, "hidden", 6))
     border->typed_data.value = BorderStyleHidden;
   else if (!strncasecmp (cssRule, "dotted", 6))
     border->typed_data.value = BorderStyleDotted;
   else if (!strncasecmp (cssRule, "dashed", 6))
     border->typed_data.value = BorderStyleDashed;
   else if (!strncasecmp (cssRule, "solid", 5))
     border->typed_data.value = BorderStyleSolid;
   else if (!strncasecmp (cssRule, "double", 6))
     border->typed_data.value = BorderStyleDouble;
   else if (!strncasecmp (cssRule, "groove", 6))
     border->typed_data.value = BorderStyleGroove;
   else if (!strncasecmp (cssRule, "ridge", 5))
     border->typed_data.value = BorderStyleRidge;
   else if (!strncasecmp (cssRule, "inset", 5))
     border->typed_data.value = BorderStyleInset;
   else if (!strncasecmp (cssRule, "outset", 6))
     border->typed_data.value = BorderStyleOutset;
   else
     {
       /* invalid style */
       border->typed_data.unit = UNIT_INVALID;
       return (cssRule);
     }
   /* the value is parsed now */
   cssRule = SkipWord (cssRule);
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
  if (!strncasecmp (cssRule, "inherit", 7))
    {
      cssRule = SkipWord (cssRule);
      return (cssRule);
    }
  if (ptr == cssRule)
    {
      cssRule = SkipWord (cssRule);
      CSSParseError ("Invalid color value", ptr, cssRule);
      val->typed_data.value = 0;
      val->typed_data.unit = UNIT_INVALID;
    }
  else
    {
      best = TtaGetThotColor (redval, greenval, blueval);
      val->typed_data.value = best;
      val->typed_data.unit = UNIT_REL;
      cssRule = ptr;
    }
  val->typed_data.real = FALSE;
  return (cssRule);
}

/*----------------------------------------------------------------------
  CheckImportantRule updates the field important of the context.
  ----------------------------------------------------------------------*/
static char *CheckImportantRule (char *cssRule, PresentationContext context)
{
  cssRule = SkipBlanksAndComments (cssRule);
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
  cssRule = ParseBorderValue (cssRule, &border);
  if (border.typed_data.unit != UNIT_INVALID && DoApply)
    {
      /* check if it's an important rule */
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
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
  cssRule = ParseBorderValue (cssRule, &border);
  if (border.typed_data.unit != UNIT_INVALID && DoApply)
    {
      /* check if it's an important rule */
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
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
  cssRule = ParseBorderValue (cssRule, &border);
  if (border.typed_data.unit != UNIT_INVALID && DoApply)
    {
      /* check if it's an important rule */
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
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
  cssRule = ParseBorderValue (cssRule, &border);
  if (border.typed_data.unit != UNIT_INVALID && DoApply)
    {
      /* check if it's an important rule */
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
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
  if (*ptrR == ';' || *ptrR == EOS || *ptrR == ',')
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
      if (*ptrB == ';' || *ptrB == EOS || *ptrB == ',')
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
	  if (*ptrL == ';' || *ptrL == EOS || *ptrL == ',')
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

  cssRule = ParseCSSColor (cssRule, &best);
  if (best.typed_data.unit != UNIT_INVALID && DoApply)
    {
      /* check if it's an important rule */
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
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
  
  cssRule = ParseCSSColor (cssRule, &best);
  if (best.typed_data.unit != UNIT_INVALID && DoApply)
    {
      /* check if it's an important rule */
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
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

  cssRule = ParseCSSColor (cssRule, &best);
  if (best.typed_data.unit != UNIT_INVALID && DoApply)
    {
      /* check if it's an important rule */
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
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

  cssRule = ParseCSSColor (cssRule, &best);
  if (best.typed_data.unit != UNIT_INVALID && DoApply)
    {
      /* check if it's an important rule */
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
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
  if (*ptrR == ';' || *ptrR == EOS || *ptrR == ',')
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
      if (*ptrB == ';' || *ptrB == EOS || *ptrB == ',')
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
	  if (*ptrL == ';' || *ptrL == EOS || *ptrL == ',')
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
  if (border.typed_data.unit != UNIT_INVALID && DoApply)
    {
      /* check if it's an important rule */
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
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
  if (border.typed_data.unit != UNIT_INVALID && DoApply)
    {
      /* check if it's an important rule */
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
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
  if (border.typed_data.unit != UNIT_INVALID && DoApply)
    {
      /* check if it's an important rule */
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
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
  if (border.typed_data.unit != UNIT_INVALID && DoApply)
    {
      /* check if it's an important rule */
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
      TtaSetStylePresentation (PRBorderRightStyle, element, tsch, context, border);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderStyleStyle: parse a CSS border-style        
   attribute string.                                          
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
  if (*ptrR == ';' || *ptrR == EOS || *ptrR == ',')
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
      if (*ptrB == ';' || *ptrB == EOS || *ptrB == ',')
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
	  if (*ptrL == ';' || *ptrL == EOS || *ptrL == ',')
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

  cssRule = SkipBlanksAndComments (cssRule);
  while (*cssRule != ';' && *cssRule != EOS && *cssRule != ',')
    {
      ptr = cssRule;
      cssRule = ParseCSSBorderStyleTop (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	cssRule = ParseCSSBorderTopWidth (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	cssRule = ParseCSSBorderColorTop (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	/* rule not found */
	cssRule = SkipValue ("Invalid border value", cssRule);
      cssRule = SkipBlanksAndComments (cssRule);
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
  char           *ptr;

  cssRule = SkipBlanksAndComments (cssRule);
  while (*cssRule != ';' && *cssRule != EOS && *cssRule != ',')
    {
      ptr = cssRule;
      cssRule = ParseCSSBorderStyleLeft (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	cssRule = ParseCSSBorderLeftWidth (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	cssRule = ParseCSSBorderColorLeft (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	/* rule not found */
	cssRule = SkipValue ("Invalid border value", cssRule);
      cssRule = SkipBlanksAndComments (cssRule);
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
  char           *ptr;

  cssRule = SkipBlanksAndComments (cssRule);
  while (*cssRule != ';' && *cssRule != EOS && *cssRule != ',')
    {
      ptr = cssRule;
      cssRule = ParseCSSBorderStyleBottom (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	cssRule = ParseCSSBorderBottomWidth (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	cssRule = ParseCSSBorderColorBottom (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	/* rule not found */
	cssRule = SkipValue ("Invalid border value", cssRule);
      cssRule = SkipBlanksAndComments (cssRule);
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
  char            *ptr;

  cssRule = SkipBlanksAndComments (cssRule);
  while (*cssRule != ';' && *cssRule != EOS && *cssRule != ',')
    {
      ptr = cssRule;
      cssRule = ParseCSSBorderStyleRight (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	cssRule = ParseCSSBorderRightWidth (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	cssRule = ParseCSSBorderColorRight (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	/* rule not found */
	cssRule = SkipValue ("Invalid border value", cssRule);
      cssRule = SkipBlanksAndComments (cssRule);
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
  int   skippedNL;

  ptrT = SkipBlanksAndComments (cssRule);
  /* First parse Border-Top */
  ptrR = ParseCSSBorderTop (element, tsch, context, ptrT, css, isHTML);
  ptrR = SkipBlanksAndComments (ptrR);
  if (*ptrR == ';' || *ptrR == EOS || *ptrR == ',')
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
  PresentationValue   pval;
  char               *ptr;

  pval.typed_data.value = 0;
  pval.typed_data.unit = UNIT_BOX;
  pval.typed_data.real = FALSE;
  if (!strncasecmp (cssRule, "inherit", 7))
    {
      cssRule = SkipWord (cssRule);
      return (cssRule);
    }
  if (!strncasecmp (cssRule, "none", 4))
    pval.typed_data.value = FloatNone;
  else if (!strncasecmp (cssRule, "left", 4))
    pval.typed_data.value = FloatLeft;
  else if (!strncasecmp (cssRule, "right", 5))
    pval.typed_data.value = FloatRight;

  ptr = cssRule;
  cssRule = SkipWord (cssRule);
  if (pval.typed_data.value == 0)
    CSSParseError ("Invalid float value", ptr, cssRule);
  else
    {
      if (DoApply)
	{
	  if (tsch)
	    cssRule = CheckImportantRule (cssRule, context);
	  TtaSetStylePresentation (PRFloat, element, tsch, context, pval);
	}
    }
  cssRule = SkipValue (NULL, cssRule);
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
  char               *ptr;

  pval.typed_data.value = 0;
  pval.typed_data.unit = UNIT_BOX;
  pval.typed_data.real = FALSE;
  if (!strncasecmp (cssRule, "inherit", 7))
    {
      cssRule = SkipWord (cssRule);
      return (cssRule);
    }
  if (!strncasecmp (cssRule, "none", 4))
    pval.typed_data.value = ClearNone;
  else if (!strncasecmp (cssRule, "left", 4))
    pval.typed_data.value = ClearLeft;
  else if (!strncasecmp (cssRule, "right", 5))
    pval.typed_data.value = ClearRight;
  else if (!strncasecmp (cssRule, "both", 4))
    pval.typed_data.value = ClearBoth;

  ptr = cssRule;
  cssRule = SkipWord (cssRule);
  if (pval.typed_data.value == 0)
    CSSParseError ("Invalid clear value", ptr, cssRule);
  else
    {
      if (DoApply)
	{
	  if (tsch)
	    cssRule = CheckImportantRule (cssRule, context);
	  TtaSetStylePresentation (PRClear, element, tsch, context, pval);
	}
    }
  cssRule = SkipValue (NULL, cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSContent: parse a CSS content value    
  ----------------------------------------------------------------------*/
static char *ParseCSSContent (Element element, PSchema tsch,
			      PresentationContext context, char *cssRule,
			      CSSInfoPtr css, ThotBool isHTML)
{
  cssRule = SkipValue (NULL, cssRule);
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

  pval.typed_data.unit = UNIT_REL;
  pval.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  if (!strncasecmp (cssRule, "none", 4))
    {
      pval.typed_data.value = 0;
      if (DoApply)
	{
	  if (tsch)
	    cssRule = CheckImportantRule (cssRule, context);
	  TtaSetStylePresentation (PRVisibility, element, tsch, context, pval);
	}
      cssRule = SkipWord (cssRule);
    }
  else
    {
      if (!strncasecmp (cssRule, "block", 5))
	pval.typed_data.value = Block;
      else if (!strncasecmp (cssRule, "inline", 6))
	pval.typed_data.value = Inline;
      else if (!strncasecmp (cssRule, "list-item", 9))
	pval.typed_data.value = ListItem;
      else if (!strncasecmp (cssRule, "run-in", 6))
	pval.typed_data.value = RunIn;
      else if (!strncasecmp (cssRule, "compact", 7))
	pval.typed_data.value = Compact;
      else if (!strncasecmp (cssRule, "marker", 6))
	pval.typed_data.value = Marker;
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
	      strncasecmp (cssRule, "table", 5) &&
	      strncasecmp (cssRule, "inherit", 7))
	    {
	      ptr = cssRule;
	      cssRule = SkipWord (cssRule);
	      CSSParseError ("Invalid display value", ptr, cssRule);
	    }
	  return (cssRule);
	}
      
      if (DoApply)
	{
	  if (tsch)
	    cssRule = CheckImportantRule (cssRule, context);
	  TtaSetStylePresentation (PRDisplay, element, tsch, context, pval);
	}
      cssRule = SkipWord (cssRule);
    }
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
  cssRule = SkipValue (NULL, cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSListStyleImage: parse a CSS list-style-image
   attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSListStyleImage (Element element, PSchema tsch,
				     PresentationContext context, char *cssRule,
				     CSSInfoPtr css, ThotBool isHTML)
{
  cssRule = SkipValue (NULL, cssRule);
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
  cssRule = SkipValue (NULL, cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSListStyle: parse a CSS list-style            
   attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSListStyle (Element element, PSchema tsch,
				PresentationContext context, char *cssRule,
				CSSInfoPtr css, ThotBool isHTML)
{
  cssRule = SkipValue (NULL, cssRule);
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
   PresentationValue   align;
   char               *ptr;

   align.typed_data.value = 0;
   align.typed_data.unit = UNIT_REL;
   align.typed_data.real = FALSE;

   cssRule = SkipBlanksAndComments (cssRule);
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
	align.typed_data.value = Justify;
	cssRule = SkipWord (cssRule);
     }
   else
     {
       ptr = cssRule;
       cssRule = SkipWord (cssRule);
       CSSParseError ("Invalid text-align value", ptr, cssRule);
	return (cssRule);
     }

   /*
    * install the new presentation.
    */
   if (align.typed_data.value && DoApply)
     {
       if (tsch)
	 cssRule = CheckImportantRule (cssRule, context);
       TtaSetStylePresentation (PRAdjust, element, tsch, context, align);
     }
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
   if (!strncasecmp (cssRule, "ltr", 3))
     {
       direction.typed_data.value = LeftToRight;
       cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "rtl", 3))
     {
       direction.typed_data.value = RightToLeft;
       cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "inherit", 7))
     {
       /* not implemented */
       cssRule = SkipWord (cssRule);
       return (cssRule);
     }
   else
     {
       ptr = cssRule;
       cssRule = SkipWord (cssRule);
       CSSParseError ("Invalid direction value", ptr, cssRule);
       return (cssRule);
     }

   /*
    * install the new presentation.
    */
   if (direction.typed_data.value && DoApply)
     {
       if (tsch)
	 cssRule = CheckImportantRule (cssRule, context);
       TtaSetStylePresentation (PRDirection, element, tsch, context, direction);
     }
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
   if (!strncasecmp (cssRule, "normal", 6))
     {
       bidi.typed_data.value = Normal;
       cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "embed", 5))
     {
       bidi.typed_data.value = Embed;
       cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "override", 8))
     {
       bidi.typed_data.value = Override;
       cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "inherit", 7))
     {
       /* not implemented */
       cssRule = SkipWord (cssRule);
       return (cssRule);
     }
   else
     {
       ptr = cssRule;
       cssRule = SkipWord (cssRule);
       CSSParseError ("Invalid unicode-bidi value", ptr, cssRule);
       return (cssRule);
     }

   /*
    * install the new presentation.
    */
   if (bidi.typed_data.value && DoApply)
     {
       if (tsch)
	 cssRule = CheckImportantRule (cssRule, context);
       TtaSetStylePresentation (PRUnicodeBidi, element, tsch, context, bidi);
     }
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
       return (cssRule);
     }
   /* install the attribute */
   if (DoApply)
     {
       if (tsch)
	 cssRule = CheckImportantRule (cssRule, context);
       TtaSetStylePresentation (PRIndent, element, tsch, context, pval);
     }
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
  cssRule = SkipValue (NULL, cssRule);
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
   cssRule = SkipBlanksAndComments (cssRule);
   if (!strncasecmp (cssRule, "normal", 6))
     cssRule = SkipWord (cssRule);
   else if (!strncasecmp (cssRule, "pre", 3))
     cssRule = SkipWord (cssRule);
   else
     return (cssRule);
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
  char                *ptr;

  ptr = cssRule;
  if (!strncasecmp (cssRule, "normal", 6))
    {
      pval.typed_data.unit = UNIT_REL;
      pval.typed_data.real = TRUE;
      pval.typed_data.value = 1100;
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "inherit", 7))
    {
      cssRule = SkipWord (cssRule);
      return (cssRule);
    }
  else
    cssRule = ParseCSSUnit (cssRule, &pval);

  if (pval.typed_data.unit == UNIT_INVALID)
    CSSParseError ("Invalid line-height value", ptr, cssRule);
  else if (DoApply)
    {
      /* install the new presentation */
      if (pval.typed_data.unit == UNIT_BOX)
	pval.typed_data.unit = UNIT_EM;
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
      TtaSetStylePresentation (PRLineSpacing, element, tsch, context, pval);
    }
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
   ElementType         elType;
   PresentationValue   pval;
   char               *ptr = NULL, *ptr1 = NULL;
   ThotBool	       real;

   pval.typed_data.real = FALSE;
   cssRule = SkipBlanksAndComments (cssRule);
   if (!strncasecmp (cssRule, "larger", 6))
     {
	pval.typed_data.unit = UNIT_PERCENT;
	pval.typed_data.value = 130;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "smaller", 7))
     {
	pval.typed_data.unit = UNIT_PERCENT;
	pval.typed_data.value = 80;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "xx-small", 8))
     {
	pval.typed_data.unit = UNIT_REL;
	pval.typed_data.value = 1;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "x-small", 7))
     {
	pval.typed_data.unit = UNIT_REL;
	pval.typed_data.value = 2;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "small", 5))
     {
	pval.typed_data.unit = UNIT_REL;
	pval.typed_data.value = 3;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "medium", 6))
     {
	pval.typed_data.unit = UNIT_REL;
	pval.typed_data.value = 4;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "large", 5))
     {
	pval.typed_data.unit = UNIT_REL;
	pval.typed_data.value = 5;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "x-large", 7))
     {
	pval.typed_data.unit = UNIT_REL;
	pval.typed_data.value = 6;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "xx-large", 8))
     {
	pval.typed_data.unit = UNIT_REL;
	pval.typed_data.value = 7;
	cssRule = SkipWord (cssRule);
     }
   else if (!isdigit (*cssRule) && *cssRule != '.')
     return (cssRule);
   else
     {
       /* look for a '/' within the current cssRule */
       ptr1 = strchr (cssRule, ';');
       ptr = strchr (cssRule, '/');
       if (ptr && (ptr1 == NULL || ptr < ptr1))
	 {
	   /* keep the line spacing rule */
	   ptr[0] = EOS;
	   ptr = &ptr[1];
	 }
       else
	 ptr = NULL;
       
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
   if (DoApply)
     {
       if (tsch)
	 cssRule = CheckImportantRule (cssRule, context);
       TtaSetStylePresentation (PRSize, element, tsch, context, pval);
     }
   if (ptr)
     cssRule = ParseCSSLineHeight (element, tsch, context, ptr, css, isHTML);
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
  PresentationValue   font;
  char              quoteChar;

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

  if (!strncasecmp (cssRule, "times", 5) &&
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
      if (quoteChar != EOS)
         cssRule = SkipQuotedString (cssRule, quoteChar);
      else
         cssRule = SkipWord (cssRule);
      cssRule = SkipBlanksAndComments (cssRule);
      if (*cssRule == ',')
	{
	  cssRule++;
	  cssRule = ParseCSSFontFamily (element, tsch, context, cssRule, css, isHTML);
	  return (cssRule);
	}
    }

  if (font.typed_data.value != 0)
     {
       cssRule = SkipBlanksAndComments (cssRule);
      if (*cssRule == ',')
	{
	  cssRule++;
	  cssRule = SkipValue (NULL, cssRule);
	}
       /* install the new presentation */
       if (DoApply)
	 {
	   if (tsch)
	     cssRule = CheckImportantRule (cssRule, context);
	   TtaSetStylePresentation (PRFont, element, tsch, context, font);
	 }
     }
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
   PresentationValue   weight;

   weight.typed_data.value = 0;
   weight.typed_data.unit = UNIT_REL;
   weight.typed_data.real = FALSE;
   cssRule = SkipBlanksAndComments (cssRule);
   if (!strncasecmp (cssRule, "100", 3) && !isalpha (cssRule[3]))
     {
	weight.typed_data.value = -3;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "200", 3) && !isalpha (cssRule[3]))
     {
	weight.typed_data.value = -2;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "300", 3) && ! isalpha(cssRule[3]))
     {
	weight.typed_data.value = -1;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "normal", 6) || (!strncasecmp (cssRule, "400", 3) && !isalpha (cssRule[3])))
     {
	weight.typed_data.value = 0;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "500", 3) && !isalpha (cssRule[3]))
     {
	weight.typed_data.value = +1;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "600", 3) && !isalpha (cssRule[3]))
     {
	weight.typed_data.value = +2;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "bold", 4) || (!strncasecmp (cssRule, "700", 3) && !isalpha (cssRule[3])))
     {
	weight.typed_data.value = +3;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "800", 3) && !isalpha (cssRule[3]))
     {
	weight.typed_data.value = +4;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "900", 3) && !isalpha (cssRule[3]))
     {
	weight.typed_data.value = +5;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "inherit", 7) || !strncasecmp (cssRule, "bolder", 6) || !strncasecmp (cssRule, "lighter", 7))
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
    if (weight.typed_data.value > 0)
       weight.typed_data.value = WeightBold;
    else
       weight.typed_data.value = WeightNormal;

   /* install the new presentation */
    if (DoApply)
     {
       if (tsch)
	 cssRule = CheckImportantRule (cssRule, context);
       TtaSetStylePresentation (PRWeight, element, tsch, context, weight);
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
   else
       return (cssRule);

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
       /* not implemented */
       cssRule = SkipWord (cssRule);
       return (cssRule);
     }
   else
     {
       /* invalid font style */
       return (cssRule);
     }

   /*
    * install the new presentation.
    */
   if (style.typed_data.value != 0 && DoApply)
     {
       if (tsch)
	 cssRule = CheckImportantRule (cssRule, context);
        TtaSetStylePresentation (PRStyle, element, tsch, context, style);
     }
   if (size.typed_data.value != 0 && DoApply)
     {
	PresentationValue   previous_size;

	if (!TtaGetStylePresentation (PRSize, element, tsch, context, &previous_size))
	  {
	     /* !!!!!!!!!!!!!!!!!!!!!!!! Unite + relatif !!!!!!!!!!!!!!!! */
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
  ParseCSSFont: parse a CSS font attribute string
  we expect the input string describing the attribute to be
  !!!!!!                                  
  ----------------------------------------------------------------------*/
static char *ParseCSSFont (Element element, PSchema tsch,
			   PresentationContext context, char *cssRule,
			   CSSInfoPtr css, ThotBool isHTML)
{
  char           *ptr;
  int             skippedNL;

  cssRule = SkipBlanksAndComments (cssRule);
  if (!strncasecmp (cssRule, "caption", 7))
    ;
  else if (!strncasecmp (cssRule, "icon", 4))
    ;
  else if (!strncasecmp (cssRule, "menu", 4))
    ;
  else if (!strncasecmp (cssRule, "message-box", 11))
    ;
  else if (!strncasecmp (cssRule, "small-caption", 13))
    ;
  else if (!strncasecmp (cssRule, "status-bar", 10))
    ;
  else
    {
      while (*cssRule != ';' && *cssRule != EOS)
	{
	  ptr = cssRule;
	  skippedNL = NewLineSkipped;
	  cssRule = ParseCSSFontStyle (element, tsch, context, cssRule, css, isHTML);
	  if (ptr == cssRule)
	    {
	      NewLineSkipped = skippedNL;
	      cssRule = ParseCSSFontVariant (element, tsch, context, cssRule, css, isHTML);
	    }
	  if (ptr == cssRule)
	    {
	      NewLineSkipped = skippedNL;
	      cssRule = ParseCSSFontWeight (element, tsch, context, cssRule, css, isHTML);
	    }
	  if (ptr == cssRule)
	    {
	      NewLineSkipped = skippedNL;
	      cssRule = ParseCSSFontSize (element, tsch, context, cssRule, css, isHTML);
	    }
	  if (ptr == cssRule)
	    {
	      NewLineSkipped = skippedNL;
	      cssRule = ParseCSSFontFamily (element, tsch, context, cssRule, css, isHTML);
	    }
	  if (ptr == cssRule)
	    cssRule = SkipValue ("Invalid font value", cssRule);
	  cssRule = SkipBlanksAndComments (cssRule);
	}
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
   char               *ptr;

   decor.typed_data.value = 0;
   decor.typed_data.unit = UNIT_REL;
   decor.typed_data.real = FALSE;
   cssRule = SkipBlanksAndComments (cssRule);
   if (!strncasecmp (cssRule, "none", strlen ("none")))
     {
	decor.typed_data.value = NoUnderline;
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "underline", strlen ("underline")))
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
   else if (!strncasecmp (cssRule, "blink", strlen ("blink")))
     {
	/* the blink text-decoration attribute is not supported */
	cssRule = SkipWord (cssRule);
     }
   else if (!strncasecmp (cssRule, "inherit", 7))
     {
       cssRule = SkipWord (cssRule);
       return (cssRule);
     }
   else
     {
       ptr = cssRule;
       cssRule = SkipWord (cssRule);
       CSSParseError ("Invalid text-decoration value", ptr, cssRule);
	return (cssRule);
     }

   /*
    * install the new presentation.
    */
   if (decor.typed_data.value && DoApply)
     {
       if (tsch)
	 cssRule = CheckImportantRule (cssRule, context);
       TtaSetStylePresentation (PRUnderline, element, tsch, context, decor);
     }
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
      cssRule = SkipWord (cssRule);
    }
  else
    cssRule = ParseCSSUnit (cssRule, &val);
  if (val.typed_data.value != 0 &&
      (val.typed_data.unit == UNIT_INVALID ||
       val.typed_data.unit == UNIT_BOX))
    CSSParseError ("height value", ptr, cssRule);
  else if (DoApply)
    {
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
      /* install the new presentation */
      TtaSetStylePresentation (PRHeight, element, tsch, context, val);
    }
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
      cssRule = SkipWord (cssRule);
    }
  else
      cssRule = ParseCSSUnit (cssRule, &val);
  if (val.typed_data.value != 0 &&
      (val.typed_data.unit == UNIT_INVALID ||
       val.typed_data.unit == UNIT_BOX))
    CSSParseError ("Invalid width value", ptr, cssRule);
  else if (DoApply)
    {
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
      /* install the new presentation */
      TtaSetStylePresentation (PRWidth, element, tsch, context, val);
    }
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
      cssRule = SkipWord (cssRule);
    }
  else
    cssRule = ParseCSSUnit (cssRule, &margin);
  if (margin.typed_data.value != 0 &&
      (margin.typed_data.unit == UNIT_INVALID ||
       margin.typed_data.unit == UNIT_BOX))
    CSSParseError ("Invalid margin-top value", ptr, cssRule);
  else if (DoApply)
     {
       if (tsch)
	 cssRule = CheckImportantRule (cssRule, context);
       TtaSetStylePresentation (PRMarginTop, element, tsch, context, margin);
     }
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
      cssRule = SkipWord (cssRule);
    }
  else
    cssRule = ParseCSSUnit (cssRule, &margin);
  if (margin.typed_data.value != 0 &&
      (margin.typed_data.unit == UNIT_INVALID ||
       margin.typed_data.unit == UNIT_BOX))
    CSSParseError ("Invalid margin-bottom value", ptr, cssRule);
  else if (DoApply)
     {
       if (tsch)
	 cssRule = CheckImportantRule (cssRule, context);
       TtaSetStylePresentation (PRMarginBottom, element, tsch, context, margin);
     }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseCSSMarginLeft: parse a CSS margin-left attribute string
  ----------------------------------------------------------------------*/
static char *ParseCSSMarginLeft (Element element, PSchema tsch,
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
      cssRule = SkipWord (cssRule);
    }
  else
    cssRule = ParseCSSUnit (cssRule, &margin);
  if (margin.typed_data.value != 0 &&
      (margin.typed_data.unit == UNIT_INVALID ||
       margin.typed_data.unit == UNIT_BOX))
    CSSParseError ("Invalid margin-left value", ptr, cssRule);
  else if (DoApply)
  if (margin.typed_data.unit != UNIT_INVALID && DoApply)
     {
       if (tsch)
	 cssRule = CheckImportantRule (cssRule, context);
       TtaSetStylePresentation (PRMarginLeft, element, tsch, context, margin);
     }
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
      cssRule = SkipWord (cssRule);
    }
  else
    cssRule = ParseCSSUnit (cssRule, &margin);
  if (margin.typed_data.value != 0 &&
      (margin.typed_data.unit == UNIT_INVALID ||
       margin.typed_data.unit == UNIT_BOX))
    CSSParseError ("Invalid margin-right value", ptr, cssRule);
  else if (DoApply)
     {
       if (tsch)
	 cssRule = CheckImportantRule (cssRule, context);
       TtaSetStylePresentation (PRMarginRight, element, tsch, context, margin);
     }
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
  ptrR = ParseCSSMarginTop (element, tsch, context, ptrT, css, isHTML);
  ptrR = SkipBlanksAndComments (ptrR);
  if (*ptrR == ';' || *ptrR == EOS || *ptrR == ',')
    {
      skippedNL = NewLineSkipped;
      cssRule = ptrR;
      /* apply the Margin-Top to all */
      ptrR = ParseCSSMarginRight (element, tsch, context, ptrT, css, isHTML);
      NewLineSkipped = skippedNL;
      ptrR = ParseCSSMarginBottom (element, tsch, context, ptrT, css, isHTML);
      NewLineSkipped = skippedNL;
      ptrR = ParseCSSMarginLeft (element, tsch, context, ptrT, css, isHTML);
    }
  else
    {
      /* parse Margin-Right */
      ptrB = ParseCSSMarginRight (element, tsch, context, ptrR, css, isHTML);
      ptrB = SkipBlanksAndComments (ptrB);
      if (*ptrB == ';' || *ptrB == EOS || *ptrB == ',')
	{
	  skippedNL = NewLineSkipped;
	  cssRule = ptrB;
	  /* apply the Margin-Top to Margin-Bottom */
	  ptrB = ParseCSSMarginBottom (element, tsch, context, ptrT, css, isHTML);
	  NewLineSkipped = skippedNL;
	  /* apply the Margin-Right to Margin-Left */
	  ptrB = ParseCSSMarginLeft (element, tsch, context, ptrR, css, isHTML);
	}
      else
	{
	  /* parse Margin-Bottom */
	  ptrL = ParseCSSMarginBottom (element, tsch, context, ptrB, css, isHTML);
	  ptrL = SkipBlanksAndComments (ptrL);
	  if (*ptrL == ';' || *ptrL == EOS || *ptrL == ',')
	    {
	      cssRule = ptrL;
	      /* apply the Margin-Right to Margin-Left */
	      ptrL = ParseCSSMarginLeft (element, tsch, context, ptrR, css, isHTML);
	    }
	  else
	    /* parse Margin-Left */
	    cssRule = ParseCSSMarginLeft (element, tsch, context, ptrL, css, isHTML);
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
  if (padding.typed_data.value != 0 &&
      (padding.typed_data.unit == UNIT_INVALID ||
       padding.typed_data.unit == UNIT_BOX))
    {
      CSSParseError ("Invalid padding-top value", ptr, cssRule);
      padding.typed_data.value = 0;
    }
  else if (DoApply)
     {
       if (tsch)
	 cssRule = CheckImportantRule (cssRule, context);
       TtaSetStylePresentation (PRPaddingTop, element, tsch, context, padding);
     }
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
  if (padding.typed_data.value != 0 &&
      (padding.typed_data.unit == UNIT_INVALID ||
       padding.typed_data.unit == UNIT_BOX))
    {
      CSSParseError ("Invalid padding-bottom value", ptr, cssRule);
      padding.typed_data.value = 0;
    }
  else if (DoApply)
     {
       if (tsch)
	 cssRule = CheckImportantRule (cssRule, context);
       TtaSetStylePresentation (PRPaddingBottom, element, tsch, context, padding);
     }
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
  if (padding.typed_data.value != 0 &&
      (padding.typed_data.unit == UNIT_INVALID ||
       padding.typed_data.unit == UNIT_BOX))
    {
      CSSParseError ("Invalid padding-left value", ptr, cssRule);
      padding.typed_data.value = 0;
    }
  else if (DoApply)
    {
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
      TtaSetStylePresentation (PRPaddingLeft, element, tsch, context, padding);
    }
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
  if (padding.typed_data.value != 0 &&
      (padding.typed_data.unit == UNIT_INVALID ||
       padding.typed_data.unit == UNIT_BOX))
    {
      CSSParseError ("Invalid padding-right value", ptr, cssRule);
      padding.typed_data.value = 0;
    }
  else if (DoApply)
    {
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
      TtaSetStylePresentation (PRPaddingRight, element, tsch, context, padding);
    }
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

  cssRule = ParseCSSColor (cssRule, &best);
  if (best.typed_data.unit != UNIT_INVALID && DoApply)
     {
       if (tsch)
	 cssRule = CheckImportantRule (cssRule, context);
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
      if (DoApply)
	{
	  if (tsch)
	    cssRule = CheckImportantRule (cssRule, context);
	  TtaSetStylePresentation (PRFillPattern, element, tsch, context, best);
	}
      cssRule = SkipWord (cssRule);
    }
  else
    {
      cssRule = ParseCSSColor (cssRule, &best);
      if (best.typed_data.unit != UNIT_INVALID && DoApply)
	{
	  if (tsch)
	    cssRule = CheckImportantRule (cssRule, context);
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

  best.typed_data.unit = UNIT_INVALID;
  best.typed_data.real = FALSE;
  if (!strncasecmp (cssRule, "none", 4))
    {
      best.typed_data.value = -2;  /* -2 means transparent */
      best.typed_data.unit = UNIT_REL;
      if (DoApply)
	{
	  if (tsch)
	    cssRule = CheckImportantRule (cssRule, context);
	  TtaSetStylePresentation (PRForeground, element, tsch, context, best);
	}
      cssRule = SkipWord (cssRule);
    }
  else
    {
      cssRule = ParseCSSColor (cssRule, &best);
      if (best.typed_data.unit != UNIT_INVALID && DoApply)
	{
	  if (tsch)
	    cssRule = CheckImportantRule (cssRule, context);
	  /* install the new presentation */
	  TtaSetStylePresentation (PRForeground, element, tsch, context, best);
	}
    }
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

  best.typed_data.unit = UNIT_INVALID;
  best.typed_data.real = FALSE;
  if (!strncasecmp (cssRule, "none", 4))
    {
      best.typed_data.value = PATTERN_NONE;
      best.typed_data.unit = UNIT_REL;
      if (DoApply)
	{
	  if (tsch)
	    cssRule = CheckImportantRule (cssRule, context);
	  TtaSetStylePresentation (PRFillPattern, element, tsch, context, best);
	}
      cssRule = SkipWord (cssRule);
    }
  else
    {
      cssRule = ParseCSSColor (cssRule, &best);
      if (best.typed_data.unit != UNIT_INVALID && DoApply)
	{
	  if (tsch)
	    cssRule = CheckImportantRule (cssRule, context);
	  /* install the new presentation. */
	  TtaSetStylePresentation (PRBackground, element, tsch, context, best);
	  /* thot specificity: need to set fill pattern for background color */
	  best.typed_data.value = PATTERN_BACKGROUND;
	  best.typed_data.unit = UNIT_REL;
	  TtaSetStylePresentation (PRFillPattern, element, tsch, context, best);
	}
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
  ParseSVGOpacity: parse a SVG fill property
  ----------------------------------------------------------------------*/
static char *ParseSVGOpacity (Element element, PSchema tsch,
			      PresentationContext context, char *cssRule,
			      CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue     best;

  best.typed_data.unit = UNIT_INVALID;
  best.typed_data.real = FALSE;
  cssRule = ParseClampedUnit (cssRule, &best);
  if (DoApply)
    {
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
      /* install the new presentation. */
      TtaSetStylePresentation (PROpacity, element,
			       tsch, context, best);
    }
  return (cssRule);
}
/*----------------------------------------------------------------------
  ParseSVGOpacity: parse a SVG fill property
  ----------------------------------------------------------------------*/
static char *ParseSVGStrokeOpacity (Element element, PSchema tsch,
			      PresentationContext context, char *cssRule,
			      CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue     best;

  best.typed_data.unit = UNIT_INVALID;
  best.typed_data.real = FALSE;
  cssRule = ParseClampedUnit (cssRule, &best);
  if (DoApply)
    {
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
      /* install the new presentation. */
      TtaSetStylePresentation (PRStrokeOpacity, element,
			       tsch, context, best);
    }
  return (cssRule);
}
/*----------------------------------------------------------------------
  ParseSVGOpacity: parse a SVG fill property
  ----------------------------------------------------------------------*/
static char *ParseSVGFillOpacity (Element element, PSchema tsch,
			      PresentationContext context, char *cssRule,
			      CSSInfoPtr css, ThotBool isHTML)
{
  PresentationValue     best;

  best.typed_data.unit = UNIT_INVALID;
  best.typed_data.real = FALSE;
  cssRule = ParseClampedUnit (cssRule, &best);
  if (DoApply)
    {
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
      /* install the new presentation. */
      TtaSetStylePresentation (PRFillOpacity, element,
			       tsch, context, best);
    }
  return (cssRule);
}
/*----------------------------------------------------------------------
  ParseCSSBackgroundImageCallback: Callback called asynchronously by
  FetchImage when a background image has been fetched.
  ----------------------------------------------------------------------*/
void ParseCSSBackgroundImageCallback (Document doc, Element element,
				      char *file, void *extra,
				      ThotBool isnew)
{
  DisplayMode                dispMode;
  BackgroundImageCallbackPtr callblock;
  Element                    el;
  PSchema                    tsch;
  CSSInfoPtr                 css;
  PresentationContext        context;
  PresentationValue          image;
  PresentationValue          value;

  callblock = (BackgroundImageCallbackPtr) extra;
  if (callblock == NULL)
    return;

  css = NULL;
  el = callblock->el;
  tsch = callblock->tsch;
  context = &callblock->context.specific;
  if (doc == 0 && !isnew)
    /* apply to the current document only */
    doc = context->doc;
  if (doc)
    {
      /* avoid too many redisplay */
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
	TtaSetDisplayMode (doc, DeferredDisplay);
    }
  else
    {
      /* check if the CSS still exists */
      css = CSSList;
      while (css && css != callblock->css)
	css = css->NextCSS;
      if (css == NULL)
	tsch = NULL;
    }

  if (el || tsch)
    {
      /* Ok the image was fetched, finish the background-image handling */
      image.typed_data.unit = UNIT_REL;
      image.typed_data.real = FALSE;
      image.pointer = file;
      TtaSetStylePresentation (PRBackgroundPicture, el, tsch, context, image);
      
      /* enforce the showbox */
      value.typed_data.value = 1;
      value.typed_data.unit = UNIT_REL;
      value.typed_data.real = FALSE;
      TtaSetStylePresentation (PRShowBox, el, tsch, context, value);      
    }

  TtaFreeMemory (callblock);
  /* restore the display mode */
  if (doc)
    {
      if (dispMode == DisplayImmediately)
	TtaSetDisplayMode (doc, dispMode);
    }
  else if (css)
    {
      for (doc = 1; doc < DocumentTableLength; doc++)
	if (css->documents[doc] && css->enabled[doc] &&
	    /* don't manage a document used by make book */
	    (DocumentMeta[doc] == NULL ||
	     DocumentMeta[doc]->method != CE_MAKEBOOK))
	  {
	    /* Change the Display Mode to take into account the new presentation */
	    dispMode = TtaGetDisplayMode (doc);
	    if (dispMode == DisplayImmediately)
	      {
		TtaSetDisplayMode (doc, NoComputedDisplay);
		/* Restore the display mode */
		TtaSetDisplayMode (doc, dispMode);
	      }
	  }
    }
}

/*----------------------------------------------------------------------
   GetCSSBackgroundURL searches a CSS BackgroundImage url within
   the styleString.
   Returns NULL or a new allocated url string.
  ----------------------------------------------------------------------*/
char *GetCSSBackgroundURL (char *styleString)
{
  char            *b, *e, *ptr;
  int              len;

  ptr = NULL;
  b = strstr (styleString, "url");
  if (b != NULL)
    {
      b += 3;
      b = SkipBlanksAndComments (b);
      if (*b == '(')
	{
	  b++;
	  b = SkipBlanksAndComments (b);
	  /*** Escaped quotes are not handled. See function SkipQuotedString */
	  if (*b == '"')
	    {
	      b++;
	      /* search the url end */
	      e = b;
	      while (*e != EOS && *e != '"')
		e++;
	    }
	  else if (*b == '\'')
	    {
	      b++;
	      /* search the url end */
	      e = b;
	      while (*e != EOS && *e != '\'')
		e++;
	    }
	  else
	    {
	      /* search the url end */
	      e = b;
	      while (*e != EOS && *e != ')')
		e++;
	    }
	  if (*e != EOS)
	    {
	      len = (int)(e - b);
	      ptr = (char*) TtaGetMemory (len+1);
	      strncpy (ptr, b, len);
	      ptr[len] = EOS;
	    }
	}
    }
  return (ptr);
}


/*----------------------------------------------------------------------
  ParseCSSBackgroundImage: parse a CSS BackgroundImage attribute string.
  ----------------------------------------------------------------------*/
static char *ParseCSSBackgroundImage (Element element, PSchema tsch,
				      PresentationContext context,
				      char *cssRule, CSSInfoPtr css,
				      ThotBool isHTML)
{
  GenericContext             ctxt = (GenericContext) context;
  Element                    el;
  BackgroundImageCallbackPtr callblock;
  PresentationValue          image, value;
  char                      *url;
  char                      *bg_image;
  char                       saved;
  char                      *base;
  char                       tempname[MAX_LENGTH];
  char                       imgname[MAX_LENGTH];

  if (element)
    el = element;
  else
    /* default element for FetchImage */
    el = TtaGetMainRoot (context->doc);
    
  url = NULL;
  cssRule = SkipBlanksAndComments (cssRule);
  if (!strncasecmp (cssRule, "none", 4))
    {
      image.pointer = NULL;
      TtaSetStylePresentation (PRBackgroundPicture, element, tsch, context,
			       image);
      cssRule += 4;
    }
  else if (!strncasecmp (cssRule, "url", 3))
    {  
      cssRule += 3;
      cssRule = SkipBlanksAndComments (cssRule);
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
	  saved = *cssRule;
	  *cssRule = EOS;
	  url = TtaStrdup (base);
	  *cssRule = saved;
	  if (saved == '"' || saved == '\'')
	    /* we need to skip the quote character and possinble spaces */
	    {
	      cssRule++;	    
	      cssRule = SkipBlanksAndComments (cssRule);
	    }
	}
      cssRule++;

      if (context->destroy)
	{
	  /* remove the background image PRule */
	  image.pointer = NULL;
	  TtaSetStylePresentation (PRBackgroundPicture, element, tsch, context, image);
	  if (TtaGetStylePresentation (PRFillPattern, element, tsch, context, &value) < 0)
	    {
	      /* there is no FillPattern rule -> remove ShowBox rule */
	      value.typed_data.value = 1;
	      value.typed_data.unit = UNIT_REL;
	      value.typed_data.real = FALSE;
	      TtaSetStylePresentation (PRShowBox, element, tsch, context, value);
	    }
	}
      else if (url)
	{
	  bg_image = TtaGetEnvString ("ENABLE_BG_IMAGES");
	  if (bg_image == NULL || !strcasecmp (bg_image, "yes"))
	    /* background images are enabled */
	    {
	      callblock = (BackgroundImageCallbackPtr) TtaGetMemory (sizeof (BackgroundImageCallbackBlock));
	      if (callblock)
		{
		  callblock->el = element;
		  callblock->tsch = tsch;
		  callblock->css = css;
		  if (element == NULL)
		    memcpy (&callblock->context.generic, ctxt,
			    sizeof (GenericContextBlock));
		  else
		    memcpy (&callblock->context.specific, context,
			    sizeof(PresentationContextBlock));

		  /* check if the image url is related to an external CSS */
		  if (css)
		    {
		      if (css->url)
			/* the image concerns a CSS file */
			NormalizeURL (url, 0, tempname, imgname, css->url);
		      else
			/* the image concerns a style element */
			NormalizeURL (url, context->doc, tempname, imgname, NULL);
		      /* fetch and display background image of element */
		      FetchImage (0, el, tempname, AMAYA_LOAD_IMAGE,
				  ParseCSSBackgroundImageCallback, callblock);
		    }
		  else
		    FetchImage (context->doc, el, url, AMAYA_LOAD_IMAGE,
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
  ParseCSSBackgroundRepeat: parse a CSS BackgroundRepeat attribute string.
  ----------------------------------------------------------------------*/
static char *ParseCSSBackgroundRepeat (Element element, PSchema tsch,
				       PresentationContext context,
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
    repeat.typed_data.value = VREPEAT;
  else if (!strncasecmp (cssRule, "repeat-x", 8))
    repeat.typed_data.value = HREPEAT;
  else if (!strncasecmp (cssRule, "repeat", 6))
    repeat.typed_data.value = REPEAT;
  else
    return (cssRule);

   /* install the new presentation */
  if (DoApply)
    {
      /* check if it's an important rule */
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
      TtaSetStylePresentation (PRPictureMode, element, tsch, context, repeat);
    }
  cssRule = SkipWord (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBackgroundAttachment: parse a CSS BackgroundAttachment
   attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSBackgroundAttachment (Element element, PSchema tsch,
					   PresentationContext context,
					   char *cssRule, CSSInfoPtr css,
					   ThotBool isHTML)
{
  char     *ptr;

  cssRule = SkipBlanksAndComments (cssRule);
  if (!strncasecmp (cssRule, "scroll", 6))
    {
      /* force no-repeat for that background image */
      ptr = "no-repeat";
      ParseCSSBackgroundRepeat (element, tsch, context,
				ptr, css, isHTML);
      cssRule = SkipWord (cssRule);
    }
  else if (!strncasecmp (cssRule, "fixed", 5))
    {
      /* force no-repeat for that background image */
      ptr = "no-repeat";
      ParseCSSBackgroundRepeat (element, tsch, context,
				ptr, css, isHTML);
      cssRule = SkipWord (cssRule);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBackgroundPosition: parse a CSS BackgroundPosition
   attribute string.                                          
  ----------------------------------------------------------------------*/
static char *ParseCSSBackgroundPosition (Element element, PSchema tsch,
					 PresentationContext context,
					 char *cssRule, CSSInfoPtr css,
					 ThotBool isHTML)
{
  PresentationValue     repeat;
  char                 *ptr;
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
  else if (isdigit (*cssRule) || *cssRule == '.')
    {
      cssRule = SkipWord (cssRule);
    }
  else
    ok = FALSE;

  if (ok && DoApply)
    {
      /* force no-repeat for that background image */
      ptr = "no-repeat";
      ParseCSSBackgroundRepeat (element, tsch, context,
				ptr, css, isHTML);
      /* force realsize for the background image */
      repeat.typed_data.value = REALSIZE;
      repeat.typed_data.unit = UNIT_REL;
      repeat.typed_data.real = FALSE;
      /* check if it's an important rule */
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
      /*TtaSetStylePresentation (PRPictureMode, element, tsch, context, repeat);*/
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBackground: parse a CSS background attribute 
  ----------------------------------------------------------------------*/
static char *ParseCSSBackground (Element element, PSchema tsch,
				 PresentationContext context, char *cssRule,
				 CSSInfoPtr css, ThotBool isHTML)
{
  char     *ptr;
  int   skippedNL;

  cssRule = SkipBlanksAndComments (cssRule);
  while (*cssRule != ';' && *cssRule != EOS && *cssRule != ',')
    {
      /* perhaps a Background Image */
      if (!strncasecmp (cssRule, "url", 3) || !strncasecmp (cssRule, "none", 4))
         cssRule = ParseCSSBackgroundImage (element, tsch, context, cssRule,
					    css, isHTML);
      /* perhaps a Background Attachment */
      else if (!strncasecmp (cssRule, "scroll", 6) ||
               !strncasecmp (cssRule, "fixed", 5))
	cssRule = ParseCSSBackgroundAttachment (element, tsch, context,
						cssRule, css, isHTML);
      /* perhaps a Background Repeat */
      else if (!strncasecmp (cssRule, "no-repeat", 9) ||
               !strncasecmp (cssRule, "repeat-y", 8)  ||
               !strncasecmp (cssRule, "repeat-x", 8)  ||
               !strncasecmp (cssRule, "repeat", 6))
	cssRule = ParseCSSBackgroundRepeat (element, tsch, context,
					    cssRule, css, isHTML);
      /* perhaps a Background Position */
      else if (!strncasecmp (cssRule, "left", 4)   ||
               !strncasecmp (cssRule, "right", 5)  ||
               !strncasecmp (cssRule, "center", 6) ||
               !strncasecmp (cssRule, "top", 3)    ||
               !strncasecmp (cssRule, "bottom", 6) ||
               isdigit (*cssRule) || *cssRule == '.')
           cssRule = ParseCSSBackgroundPosition (element, tsch, context,
						 cssRule, css, isHTML);
      /* perhaps a Background Color */
      else
	{
	  skippedNL = NewLineSkipped;
	  /* check if the rule has been found */
	  ptr = cssRule;
	  cssRule = ParseCSSBackgroundColor (element, tsch, context,
					     cssRule, css, isHTML);
	  if (ptr == cssRule)
	    {
	      NewLineSkipped = skippedNL;
	      /* rule not found */
	      cssRule = SkipProperty (cssRule);
	    }
	}
      cssRule = SkipBlanksAndComments (cssRule);
    }
   return (cssRule);
}

/*----------------------------------------------------------------------
 ParseCSSPageBreakBefore: parse a CSS page-break-before attribute 
  ----------------------------------------------------------------------*/
static char *ParseCSSPageBreakBefore (Element element, PSchema tsch,
				      PresentationContext context, char *cssRule,
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
      /*page.typed_data.unit = UNIT_REL;*/
      page.typed_data.value = PageInherit;
    }
  cssRule = SkipWord (cssRule);
  /* install the new presentation */
  if (page.typed_data.unit == UNIT_REL &&
      page.typed_data.value == PageAlways && DoApply)
    {
      /* check if it's an important rule */
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
      TtaSetStylePresentation (PRPageBefore, element, tsch, context, page);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
 ParseCSSPageBreakAfter: parse a CSS page-break-after attribute 
  ----------------------------------------------------------------------*/
static char *ParseCSSPageBreakAfter (Element element, PSchema tsch,
				     PresentationContext context,
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
      /*page.typed_data.unit = UNIT_REL;*/
      page.typed_data.value = PageInherit;
    }
  cssRule = SkipWord (cssRule);
  /* install the new presentation */
  /*if (page.typed_data.unit == UNIT_REL && DoApply)
    {
    if (tsch)
    cssRule = CheckImportantRule (cssRule, context);
    TtaSetStylePresentation (PRPageAfter, element, tsch, context, page);
    }*/
  return (cssRule);
}

/*----------------------------------------------------------------------
 ParseCSSPageBreakInside: parse a CSS page-break-inside attribute 
  ----------------------------------------------------------------------*/
static char *ParseCSSPageBreakInside (Element element, PSchema tsch,
				      PresentationContext context,
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
      /*page.typed_data.unit = UNIT_REL;*/
      page.typed_data.value = PageInherit;
    }
  cssRule = SkipWord (cssRule);
  /* install the new presentation */
  /*if (page.typed_data.unit == UNIT_REL &&
    page.typed_data.value == PageAvoid && DoApply)
    {
    if (tsch)
    cssRule = CheckImportantRule (cssRule, context);
    TtaSetStylePresentation (PRPageInside, element, tsch, context, page);
    }*/
  return (cssRule);
}


/*----------------------------------------------------------------------
   ParseSVGStrokeWidth: parse a SVG stroke-width property value.   
  ----------------------------------------------------------------------*/
static char *ParseSVGStrokeWidth (Element element, PSchema tsch,
				  PresentationContext context, char *cssRule,
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
  if (width.typed_data.unit != UNIT_INVALID && DoApply)
    {
      /* check if it's an important rule */
      if (tsch)
	cssRule = CheckImportantRule (cssRule, context);
      TtaSetStylePresentation (PRLineWeight, element, tsch, context, width);
      width.typed_data.value = 1;
      width.typed_data.unit = UNIT_REL;
    }
  return (cssRule);
}

/************************************************************************
 *									*  
 *	FUNCTIONS STYLE DECLARATIONS             			*
 *									*  
 ************************************************************************/
/*
 * NOTE: Long attribute name MUST be placed before shortened ones !
 *        e.g. "FONT-SIZE" must be placed before "FONT"
 */
static CSSProperty CSSProperties[] =
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
   {"line-height", ParseCSSLineHeight},

   {"direction", ParseCSSDirection},
   {"unicode-bidi", ParseCSSUnicodeBidi},

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

   {"width", ParseCSSWidth},
   {"height", ParseCSSHeight},
   {"float", ParseCSSFloat},
   {"clear", ParseCSSClear},
   {"content", ParseCSSContent},

   {"display", ParseCSSDisplay},
   {"white-space", ParseCSSWhiteSpace},

   {"list-style-type", ParseCSSListStyleType},
   {"list-style-image", ParseCSSListStyleImage},
   {"list-style-position", ParseCSSListStylePosition},
   {"list-style", ParseCSSListStyle},

   {"page-break-before", ParseCSSPageBreakBefore},
   {"page-break-after", ParseCSSPageBreakAfter},
   {"page-break-inside", ParseCSSPageBreakInside},

   /* SVG extensions */
   {"stroke-opacity", ParseSVGStrokeOpacity},
   {"stroke-width", ParseSVGStrokeWidth},
   {"stroke", ParseSVGStroke},

   {"fill-opacity", ParseSVGFillOpacity},
   {"fill", ParseSVGFill},

   {"opacity", ParseSVGOpacity}
};

#define NB_CSSSTYLEATTRIBUTE (sizeof(CSSProperties) / sizeof(CSSProperty))

/*----------------------------------------------------------------------
   ParseCSSRule: parse a CSS Style string                        
   we expect the input string describing the style to be of the  
   form: PRORPERTY: DESCRIPTION [ ; PROPERTY: DESCRIPTION ] * 
   but tolerate incorrect or incomplete input                    
  ----------------------------------------------------------------------*/
static void  ParseCSSRule (Element element, PSchema tsch,
			   PresentationContext context, char *cssRule,
			   CSSInfoPtr css, ThotBool isHTML)
{
  DisplayMode         dispMode;
  char               *p = NULL;
  int                 lg;
  unsigned int        i;
  ThotBool            found;

  /* avoid too many redisplay */
  dispMode = TtaGetDisplayMode (context->doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (context->doc, DeferredDisplay);

  while (*cssRule != EOS)
    {
      cssRule = SkipBlanksAndComments (cssRule);
      if (*cssRule < 0x41 || *cssRule > 0x7A ||
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
	  if (i == NB_CSSSTYLEATTRIBUTE)
	    cssRule = SkipProperty (cssRule);
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
		      p = CSSProperties[i].parsing_function (element, tsch, context,
							     p, css, isHTML);
		      /* update index and skip the ";" separator if present */
		      cssRule = p;
		    }
		}
	      else
		cssRule = SkipProperty (cssRule);
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
    TtaSetDisplayMode (context->doc, dispMode);
}

/*----------------------------------------------------------------------
 AddBorderStyleValue
 -----------------------------------------------------------------------*/
static void AddBorderStyleValue (char *buffer, int value)
{
  switch (value)
    {
    case BorderStyleNone:
      strcat (buffer, "none");
      break;
    case BorderStyleHidden:
      strcat (buffer, "hidden");
      break;
    case BorderStyleDotted:
      strcat (buffer, "dotted");
      break;
    case BorderStyleDashed:
      strcat (buffer, "dashed");
      break;
    case BorderStyleSolid:
      strcat (buffer, "solid");
      break;
    case BorderStyleDouble:
      strcat (buffer, "double");
      break;
    case BorderStyleGroove:
      strcat (buffer, "groove");
      break;
    case BorderStyleRidge:
      strcat (buffer, "ridge");
      break;
    case BorderStyleInset:
      strcat (buffer, "inset");
      break;
    case BorderStyleOutset:
      strcat (buffer, "outset");
      break;
    }
}

/*----------------------------------------------------------------------
 PToCss:  translate a PresentationSetting to the
     equivalent CSS string, and add it to the buffer given as the
     argument. It is used when extracting the CSS string from actual
     presentation.
     el is the element for which the style rule is generated
 
  All the possible values returned by the presentation drivers are
  described in thotlib/include/presentation.h
 -----------------------------------------------------------------------*/
void PToCss (PresentationSetting settings, char *buffer, int len, Element el)
{
  ElementType         elType;
  float               fval = 0;
  unsigned short      red, green, blue;
  int                 add_unit = 0;
  unsigned int        unit, i;
  ThotBool            real = FALSE;

  buffer[0] = EOS;
  if (len < 40)
    return;

  unit = settings->value.typed_data.unit;
  if (settings->value.typed_data.real)
    {
      real = TRUE;
      fval = (float) settings->value.typed_data.value;
      fval /= 1000.;
    }

  switch (settings->type)
    {
    case PRVisibility:
      break;
    case PRHeight:
      if (real)
	sprintf (buffer, "height: %g", fval);
      else
	sprintf (buffer, "height: %d", settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRWidth:
      if (real)
	sprintf (buffer, "width: %g", fval);
      else
	sprintf (buffer, "width: %d", settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRMarginTop:
      if (real)
	sprintf (buffer, "margin-top: %g", fval);
      else
	sprintf (buffer, "margin-top: %d",settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRMarginBottom:
      if (real)
	sprintf (buffer, "margin-bottom: %g", fval);
      else
	sprintf (buffer, "margin-bottom: %d",
		 settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRMarginLeft:
      if (real)
	sprintf (buffer, "margin-left: %g", fval);
      else
	sprintf (buffer, "margin-left: %d", settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRMarginRight:
      if (real)
	sprintf (buffer, "margin-right: %g", fval);
      else
	sprintf (buffer, "margin-right: %d", settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRPaddingTop:
      if (real)
	sprintf (buffer, "padding-top: %g", fval);
      else
	sprintf (buffer, "padding-top: %d", settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRPaddingBottom:
      if (real)
	sprintf (buffer, "padding-bottom: %g", fval);
      else
	sprintf (buffer, "padding-bottom: %d",
		 settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRPaddingLeft:
      if (real)
	sprintf (buffer, "padding-left: %g", fval);
      else
	sprintf (buffer, "padding-left: %d", settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRPaddingRight:
      if (real)
	sprintf (buffer, "padding-right: %g", fval);
      else
	sprintf (buffer, "padding-right: %d",
		  settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRBorderTopWidth:
      if (real)
	sprintf (buffer, "border-top-width: %g", fval);
      else
	sprintf (buffer, "border-top-width: %d",
		 settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRBorderBottomWidth:
      if (real)
	sprintf (buffer, "border-bottom-width: %g", fval);
      else
	sprintf (buffer, "border-bottom-width: %d",
		 settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRBorderLeftWidth:
      if (real)
	sprintf (buffer, "border-left-width: %g", fval);
      else
	sprintf (buffer, "border-left-width: %d",
		 settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRBorderRightWidth:
      if (real)
	sprintf (buffer, "border-right-width: %g", fval);
      else
	sprintf (buffer, "border-right-width: %d",
		 settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRBorderTopColor:
      TtaGiveThotRGB (settings->value.typed_data.value, &red, &green, &blue);
      elType = TtaGetElementType(el);
      sprintf (buffer, "border-top-color: #%02X%02X%02X", red, green, blue);
      break;
    case PRBorderRightColor:
      TtaGiveThotRGB (settings->value.typed_data.value, &red, &green, &blue);
      elType = TtaGetElementType(el);
      sprintf (buffer, "border-right-color: #%02X%02X%02X", red, green, blue);
      break;
    case PRBorderBottomColor:
      TtaGiveThotRGB (settings->value.typed_data.value, &red, &green, &blue);
      elType = TtaGetElementType(el);
      sprintf (buffer, "border-bottom-color: #%02X%02X%02X", red, green, blue);
      break;
    case PRBorderLeftColor:
      TtaGiveThotRGB (settings->value.typed_data.value, &red, &green, &blue);
      elType = TtaGetElementType(el);
      sprintf (buffer, "border-left-color: #%02X%02X%02X", red, green, blue);
      break;
    case PRBorderTopStyle:
      strcpy (buffer, "border-top-style: ");
      AddBorderStyleValue (buffer, settings->value.typed_data.value);
      break;
    case PRBorderRightStyle:
      strcpy (buffer, "border-right-style: ");
      AddBorderStyleValue (buffer, settings->value.typed_data.value);
      break;
    case PRBorderBottomStyle:
      strcpy (buffer, "border-bottom-style: ");
      AddBorderStyleValue (buffer, settings->value.typed_data.value);
      break;
    case PRBorderLeftStyle:
      strcpy (buffer, "border-left-style: ");
      AddBorderStyleValue (buffer, settings->value.typed_data.value);
      break;
    case PRSize:
      if (unit == UNIT_REL)
	{
	  if (real)
	    {
	      sprintf (buffer, "font-size: %g", fval);
	      add_unit = 1;
	    }
	  else
	    switch (settings->value.typed_data.value)
	      {
	      case 1:
		strcpy (buffer, "font-size: xx-small");
		break;
	      case 2:
		strcpy (buffer, "font-size: x-small");
		break;
	      case 3:
		strcpy (buffer, "font-size: small");
		break;
	      case 4:
		strcpy (buffer, "font-size: medium");
		break;
	      case 5:
		strcpy (buffer, "font-size: large");
		break;
	      case 6:
		strcpy (buffer, "font-size: x-large");
		break;
	      case 7:
	      case 8:
	      case 9:
	      case 10:
	      case 11:
	      case 12:
		strcpy (buffer, "font-size: xx-large");
		break;
	      }
	}
      else
	{
	  if (real)
	    sprintf (buffer, "font-size: %g", fval);
	  else
	    sprintf (buffer, "font-size: %d",
		      settings->value.typed_data.value);
	  add_unit = 1;
	}
      break;
    case PRStyle:
      switch (settings->value.typed_data.value)
	{
	case StyleRoman:
	  strcpy (buffer, "font-style: normal");
	  break;
	case StyleItalics:
	  strcpy (buffer, "font-style: italic");
	  break;
	case StyleOblique:
	  strcpy (buffer, "font-style: oblique");
	  break;
	}
      break;
    case PRWeight:
      switch (settings->value.typed_data.value)
	{
	case WeightBold:
	  strcpy (buffer, "font-weight: bold");
	  break;
	case WeightNormal:
	  strcpy (buffer, "font-weight: normal");
	  break;
	}
      break;
    case PRFont:
      switch (settings->value.typed_data.value)
	{
	case FontHelvetica:
	  strcpy (buffer, "font-family: helvetica");
	  break;
	case FontTimes:
	  strcpy (buffer, "font-family: times");
	  break;
	case FontCourier:
	  strcpy (buffer, "font-family: courier");
	  break;
	}
      break;
    case PRUnderline:
      switch (settings->value.typed_data.value)
	{
	case Underline:
	  strcpy (buffer, "text-decoration: underline");
	  break;
	case Overline:
	  strcpy (buffer, "text-decoration: overline");
	  break;
	case CrossOut:
	  strcpy (buffer, "text-decoration: line-through");
	  break;
	}
      break;
    case PRThickness:
      break;
    case PRIndent:
      if (real)
	sprintf (buffer, "text-indent: %g", fval);
      else
	sprintf (buffer, "text-indent: %d",
		  settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRLineSpacing:
      if (real)
	sprintf (buffer, "line-height: %g", fval);
      else
	sprintf (buffer, "line-height: %d",
		  settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRDepth:
      break;
    case PRAdjust:
      switch (settings->value.typed_data.value)
	{
	case AdjustLeft:
	  strcpy (buffer, "text-align: left");
	  break;
	case AdjustRight:
	  strcpy (buffer, "text-align: right");
	  break;
	case Centered:
	  strcpy (buffer, "text-align: center");
	  break;
	case LeftWithDots:
	  strcpy (buffer, "text-align: left");
	  break;
        case Justify:
	  strcpy (buffer, "text-align: justify");
	  break;
	}
      break;
    case PRDirection:
      switch (settings->value.typed_data.value)
	{
	case LeftToRight:
	  strcpy (buffer, "direction: ltr");
	  break;
	case RightToLeft:
	  strcpy (buffer, "direction: rtl");
	  break;
	}
      break;
    case PRUnicodeBidi:
      switch (settings->value.typed_data.value)
	{
	case Normal:
	  strcpy (buffer, "unicode-bidi: normal");
	  break;
	case Embed:
	  strcpy (buffer, "unicode-bidi: embed");
	  break;
	case Override:
	  strcpy (buffer, "unicode-bidi: bidi-override");
	  break;
	}
      break;
    case PRLineStyle:
      break;
    case PRDisplay:
      switch (settings->value.typed_data.value)
	{
	case Inline:
	  strcpy (buffer, "display: inline");
	  break;
	case Block:
	  strcpy (buffer, "display: block");
	  break;
	case ListItem:
	  strcpy (buffer, "display: list-item");
	  break;
	case RunIn:
	  strcpy (buffer, "display: runin");
	  break;
	case Compact:
	  strcpy (buffer, "display: compact");
	  break;
	case Marker:
	  strcpy (buffer, "display: marker");
	  break;
	default:
	  break;
	}
      break;
    case PRFloat:
      switch (settings->value.typed_data.value)
	{
	case FloatNone:
	  strcpy (buffer, "float: none");
	  break;
	case FloatLeft:
	  strcpy (buffer, "float: left");
	  break;
	case FloatRight:
	  strcpy (buffer, "float: right");
	  break;
	default:
	  break;
	}
      break;
    case PRClear:
      switch (settings->value.typed_data.value)
	{
	case ClearNone:
	  strcpy (buffer, "clear: none");
	  break;
	case ClearLeft:
	  strcpy (buffer, "clear: left");
	  break;
	case ClearRight:
	  strcpy (buffer, "clear: right");
	  break;
	case ClearBoth:
	  strcpy (buffer, "clear: both");
	  break;
	default:
	  break;
	}
      break;
    case PRLineWeight:
      elType = TtaGetElementType(el);
#ifdef _SVG
      if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "SVG"))
#endif /* _SVG */
	{
	  if (real)
	    sprintf (buffer, "stroke-width: %g", fval);
	  else
	    sprintf (buffer, "stroke-width: %d",
		      settings->value.typed_data.value);
	}
      add_unit = 1;
      break;
    case PRFillPattern:
      break;
    case PRBackground:
      TtaGiveThotRGB (settings->value.typed_data.value, &red, &green, &blue);
      elType = TtaGetElementType(el);
#ifdef _SVG
      if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "SVG") == 0)
	sprintf (buffer, "fill: #%02X%02X%02X", red, green, blue);
      else
#endif /* _SVG */
         sprintf (buffer, "background-color: #%02X%02X%02X", red, green,
		   blue);
      break;
    case PRForeground:
      TtaGiveThotRGB (settings->value.typed_data.value, &red, &green, &blue);
      elType = TtaGetElementType(el);
#ifdef _SVG
      if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "SVG") == 0)
	sprintf (buffer, "stroke: #%02X%02X%02X", red, green, blue);
      else
#endif /* _SVG */
	sprintf (buffer, "color: #%02X%02X%02X", red, green, blue);
      break;
    case PRHyphenate:
    case PRVertOverflow:
    case PRHorizOverflow:
      break;
    case PROpacity:
      sprintf (buffer, "opacity: %g", fval);
      break;
    case PRStrokeOpacity:
      sprintf (buffer, "stroke-opacity: %g", fval);
      break;
    case PRFillOpacity:
      sprintf (buffer, "fill-opacity: %g", fval);
      break;
    case PRBackgroundPicture:
      if (settings->value.pointer != NULL)
	sprintf (buffer, "background-image: url(%s)",
		  (char*)(settings->value.pointer));
      else
	sprintf (buffer, "background-image: none");
      break;
    case PRPictureMode:
      switch (settings->value.typed_data.value)
	{
	case REALSIZE:
	  sprintf (buffer, "background-repeat: no-repeat");
	  break;
	case REPEAT:
	  sprintf (buffer, "background-repeat: repeat");
	  break;
	case VREPEAT:
	  sprintf (buffer, "background-repeat: repeat-y");
	  break;
	case HREPEAT:
	  sprintf (buffer, "background-repeat: repeat-x");
	  break;
	}
      break;
    default:
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
   PresentationContext context;
   ElementType         elType;
   ThotBool            isHTML;

   /*  A rule applying to BODY is really meant to address HTML */
   elType = TtaGetElementType (el);

   /* store the current line for eventually reported errors */
   LineNumber = TtaGetElementLineNumber (el);
   if (destroy)
     /* no reported errors */
     ParsedDoc = 0;
   else if (ParsedDoc != doc)
     {
       /* update the context for reported errors */
       ParsedDoc = doc;
       DocURL = DocumentURLs[doc];
     }
   isHTML = (strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0);
   /* create the context of the Specific presentation driver */
   context = TtaGetSpecificStyleContext (doc);
   if (context == NULL)
     return;
   context->type = elType.ElTypeNum;
   context->cssSpecificity = specificity;
   context->destroy = destroy;
   /* Call the parser */
   ParseCSSRule (el, NULL,
		 (PresentationContext) context,
		 cssRule, NULL, isHTML);
   /* free the context */
   TtaFreeMemory(context);
}


/*----------------------------------------------------------------------
   ParseGenericSelector: Create a generic context for a given 
   selector string. If the selector is made of multiple comma- 
   separated selector items, it parses them one at a time and  
   return the end of the selector string to be handled or NULL 
  ----------------------------------------------------------------------*/
static char *ParseGenericSelector (char *selector, char *cssRule, char *sel,
				   GenericContext ctxt, Document doc,
				   CSSInfoPtr css)
{
  ElementType        elType;
  PSchema            tsch;
  AttributeType      attrType;
  char              *deb, *cur, c;
  char              *schemaName, *mappedName;
  char              *names[MAX_ANCESTORS];
  char              *ids[MAX_ANCESTORS];
  char              *classes[MAX_ANCESTORS];
  char              *pseudoclasses[MAX_ANCESTORS];
  char              *attrs[MAX_ANCESTORS];
  char              *attrvals[MAX_ANCESTORS];
  AttrMatch          attrmatch[MAX_ANCESTORS];
  int                i, j, k, max;
  int                att, maxAttr, kind;
  int                specificity, xmlType;
  ThotBool           isHTML;
  ThotBool           level, quoted;

  sel[0] = EOS;
  specificity = 0;
  for (i = 0; i < MAX_ANCESTORS; i++)
    {
      names[i] = NULL;
      ids[i] = NULL;
      classes[i] = NULL;
      pseudoclasses[i] = NULL;
      attrs[i] = NULL;
      attrvals[i] = NULL;
      attrmatch[i] = Txtmatch;
      ctxt->name[i] = 0;
      ctxt->names_nb[i] = 0;
      ctxt->attrType[i] = 0;
      ctxt->attrLevel[i] = 0;
      ctxt->attrText[i] = NULL;
      ctxt->attrMatch[i] = Txtmatch;
    }
  ctxt->box = 0;
  ctxt->type = 0;
  /* the specificity of the rule depends on the selector */
  ctxt->cssSpecificity = 0;
  
  selector = SkipBlanksAndComments (selector);
  cur = &sel[0];
  max = 0; /* number of loops */
  while (1)
    {
      /* point to the following word in sel[] */
      deb = cur;
      /* copy an item of the selector into sel[] */
      /* put one word in the sel buffer */
      while (*selector != EOS && *selector != ',' &&
             *selector != '.' && *selector != ':' &&
             *selector != '#' && *selector != '[' &&
             *selector != '*' && *selector != '>' &&
	     !TtaIsBlank (selector))
            *cur++ = *selector++;
      *cur++ = EOS; /* close the first string  in sel[] */
      if (deb[0] != EOS)
	{
	  names[0] = deb;
	  if (!strcmp (names[0], "html"))
	    /* give a greater priority to the backgoud color of html */
	    specificity += 3;
	  else
	    specificity += 1;
	}
      else
	names[0] = NULL;
      classes[0] = NULL;
      pseudoclasses[0] = NULL;
      ids[0] = NULL;
      attrs[0] = NULL;
      attrvals[0] = NULL;

      /* now names[0] points to the beginning of the parsed item
	 and cur to the next chain to be parsed */
      while (*selector == '.' || *selector == ':' ||
	     *selector == '#' || *selector == '[' ||
	     *selector == '*')
      {
	/* point to the following word in sel[] */
	deb = cur;
	if (*selector == '*' &&
	    (selector[1] == '.' || selector[1] == ':' ||
	     selector[1] == '#' || selector[1] == '['))
	  selector++;
	if (*selector == '.')
	  {
	    selector++;
	    while (*selector != EOS && *selector != ',' &&
		   *selector != '.' && *selector != ':' &&
		   !TtaIsBlank (selector))
	      *cur++ = *selector++;
	    /* close the word */
	    *cur++ = EOS;
	    /* point to the class in sel[] if it's valid name */
	    if (deb[0] <= 64)
	      {
		CSSPrintError ("Invalid class", deb);
		DoApply = FALSE;
	      }
	    else
	      {
		classes[0] = deb;
		/* a "class" attribute on an element may contain several
		   words, one for each class it matches */
		attrmatch[0] = Txtword;
		specificity += 10;
	      }
	  }
	else if (*selector == ':')
	  {
	    selector++;
	    while (*selector != EOS && *selector != ',' &&
		   *selector != '.' && *selector != ':' &&
		   !TtaIsBlank (selector))
	      *cur++ = *selector++;
	    /* close the word */
	    *cur++ = EOS;
	    /* point to the pseudoclass in sel[] if it's valid name */
	    if (deb[0] <= 64)
	      {
		CSSPrintError ("Invalid pseudoclass", deb);
		DoApply = FALSE;
	      }
	    else
	      {
		if (!strcmp (deb, "first-letter") ||
		    !strcmp (deb, "first-line") ||
		    !strcmp (deb, "before") ||
		    !strcmp (deb, "after"))
		  /* not supported */
		  DoApply = FALSE;
		else
		  specificity += 10;
		pseudoclasses[0]= deb;
	      }
	  }
	else if (*selector == '#')
	  {
	    selector++;
	    while (*selector != EOS && *selector != ',' &&
		   *selector != '.' && *selector != ':' &&
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
		ids[0] = deb;
		specificity += 100;
	      }
	  }
	else if (*selector == '[')
	  {
	    selector++;
	    while (*selector != EOS && *selector != ']' &&
		   *selector != '=' && *selector != '~' &&
		   *selector != '|' && *selector != '^' &&
		   *selector != '!')
	      *cur++ = *selector++;
	    /* check matching */
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
	    else
	      attrmatch[0] = Txtmatch;
	    /* close the word */
	    *cur++ = EOS;
	    /* point to the attribute in sel[] if it's valid name */
	    if (deb[0] <= 64)
	      {
		CSSPrintError ("Invalid attribute", deb);
		DoApply = FALSE;
	      }
	    else
	      {
		attrs[0] = deb;
		specificity += 10;
	      }
	    if (*selector == '=')
	      {
		/* look for a value "xxxx" */
		selector++;
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
			*cur++ = tolower (*selector);
			selector++;
		      }
		  }
		/* there is a value */
		if (quoted && *selector == '"')
		  {
		    selector++;
		    quoted = FALSE;
		  }
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
	      selector++;
	  }
	else
	  {
	    /* not supported selector */
	    while (*selector != EOS && *selector != ',' &&
		   *selector != '.' && *selector != ':' &&
		   !TtaIsBlank (selector))
	      *cur++ = *selector++;
	    /* close the word */
	    *cur++ = EOS;
	    CSSPrintError ("Not supported selector", deb);
	    DoApply = FALSE;	    
	  }
      }

      selector = SkipBlanksAndComments (selector);
      /* is it a multi-level selector? */
      if (*selector == EOS)
	/* end of the selector */
	break;
      else if (*selector == ',')
	{
	  /* end of the current selector */
	  selector++;
	  break;
	}
      else
	{
	  if (*selector == '>')
	    {
	      /* handle immediat parent as a simple parent */
	      selector++;
	      selector = SkipBlanksAndComments (selector);
	    }
	  /* shifts the list to make room for the new name */
	  max++; /* a new level in ancestor tables */
	  if (max == MAX_ANCESTORS)
	    /* abort the CSS parsing */
	    return (selector);
	  for (i = max; i > 0; i--)
	    {
	      names[i] = names[i - 1];
	      ids[i] = ids[i - 1];
	      classes[i] = classes[i - 1];
	      pseudoclasses[i] = pseudoclasses[i - 1];
	      attrs[i] = attrs[i - 1];
	      attrvals[i] = attrvals[i - 1];
	      attrmatch[i] = attrmatch[i - 1];
	    }
	}
    }

  /* Now set up the context block */
  i = 0;
  k = 0;
  j = 0;
  maxAttr = 0;
  /* default schema name */
  ctxt->schema = NULL;
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
  while (i <= max)
    {
      if (names[i])
	{
	  /* get the element type of this name in the current document */
	  MapXMLElementType (xmlType, names[i], &elType, &mappedName, &c, &level, doc);
	  if (i == 0)
	    {
	      if (elType.ElSSchema == NULL)
		{
		  /* Search in the list of loaded schemas */
		  TtaGetXmlElementType (names[i], &elType, NULL, doc);
		  if (elType.ElSSchema)
		    {
		      /* the element type concerns an imported nature */
		      schemaName = TtaGetSSchemaName(elType.ElSSchema);
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
		  /* Store the element type */
		  ctxt->type = elType.ElTypeNum;
		  ctxt->name[0] = elType.ElTypeNum;
		  ctxt->names_nb[0] = 0;
		  ctxt->schema = elType.ElSSchema;
		}
	    }
	  else if (elType.ElTypeNum != 0)
	    {
	      /* look at the current context to see if the type is already
		 stored */
	      j = 1;
	      while (j < k && ctxt->name[j] != elType.ElTypeNum)
		j++;
	      if (j == k)
		{
		  ctxt->name[j] = elType.ElTypeNum;
		  if (j != 0)
		    ctxt->names_nb[j] = 1;
		}
	      else
		/* increment the number of ancestor levels */
		ctxt->names_nb[j]++;
	    }
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
		{
		  /* look at the current context to see if the type is already
		     stored */
		  j = 1;
		  while (j < k && ctxt->name[j] != elType.ElTypeNum)
		    j++;
		  if (j == k)
		    {
		      ctxt->name[j] = elType.ElTypeNum;
		      if (j != 0)
			ctxt->names_nb[j] = 1;
		    }
		  else
		    /* increment the number of ancestor levels */
		    ctxt->names_nb[j]++;
		}
	    }
#endif /* XML_GENERIC */
	  else
	    j = k;
	}
      else
	j = k;

      /* store attributes information */
      if (classes[i])
	{
	  ctxt->attrText[j] = classes[i];
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
	  ctxt->attrMatch[j] = attrmatch[i];
	  /* add a new entry */
	  maxAttr = i + 1;
	  /* update attrLevel */
	  ctxt->attrLevel[j] = i;
	  j++;
	}
      if (pseudoclasses[i])
	{
	  ctxt->attrText[j] = pseudoclasses[i];
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
	  ctxt->attrMatch[j] = attrmatch[i];
	  /* add a new entry */
	  maxAttr = i + 1;
	  /* update attrLevel */
	  ctxt->attrLevel[j] = i;
	  j++;
	}
      if (ids[i])
	{
	  ctxt->attrText[j] = ids[i];
	  if (xmlType == SVG_TYPE)
	    ctxt->attrType[j] = SVG_ATTR_id;
	  else if (xmlType == MATH_TYPE)
	    ctxt->attrType[j] = MathML_ATTR_id;
	  else if (xmlType == XHTML_TYPE)
	    ctxt->attrType[j] = HTML_ATTR_ID;
	  else
#ifdef XML_GENERIC
	    ctxt->attrType[j] = XML_ATTR_id;
#else /* XML_GENERIC */
	    ctxt->attrType[j] = HTML_ATTR_ID;
#endif /* XML_GENERIC */
	  ctxt->attrMatch[j] = attrmatch[i];
	  /* add a new entry */
	  maxAttr = i + 1;
	  /* update attrLevel */
	  ctxt->attrLevel[j] = i;
	  j++;
	}
      if (attrs[i])
	{
	  /* it's an attribute */
	  MapXMLAttribute (xmlType, attrs[i], names[i], &level, doc, &att);
	  if (att == DummyAttribute && !strcmp (schemaName, "HTML"))
	    /* it's the "type" attribute for an "input" element. In the tree
	       it's represented by the element type, not by an attribute */
	    att = 0;
	  ctxt->attrType[j] = att;
	  ctxt->attrMatch[j] = attrmatch[i];
	  attrType.AttrSSchema = ctxt->schema;
	  attrType.AttrTypeNum = att;
	  if (i == 0 && att == 0 && ctxt->schema == NULL)
	    {
	      /* Not found -> search in the list of loaded schemas */
	      attrType.AttrSSchema = NULL;
	      TtaGetXmlAttributeType (attrs[i], &attrType, doc);
	      ctxt->attrType[j] = attrType.AttrTypeNum;
	      if (attrType.AttrSSchema)
		/* the element type concerns an imported nature */
		schemaName = TtaGetSSchemaName(attrType.AttrSSchema);
#ifdef XML_GENERIC
	      else if (xmlType == XML_TYPE)
		{
		  /* The attribute is not yet present in the tree */
		  /* Create a new global attribute */
		  attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
		  TtaAppendXmlAttribute (attrs[i], &attrType, doc);
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
	  if (kind == 0 && attrvals[i])
	    {
	      /* enumerated value */
	      MapXMLAttributeValue (xmlType, attrvals[i], attrType, &kind);
	      /* store the attribute value */
	      ctxt->attrText[j] = (char *) kind;
	    }
	  else
	    ctxt->attrText[j] = attrvals[i];
	  maxAttr = i + 1;
	  /* update attrLevel */
	  ctxt->attrLevel[j] = i;
	  j++;
	}
      i++;
      /* add a new entry */
      k++;
      if (k < j)
	k = j;
      if (i == 1 && ctxt->schema == NULL)
	/* use the document schema */
	ctxt->schema = TtaGetDocumentSSchema (doc);
    }
  /* set the selector specificity */
  ctxt->cssSpecificity = specificity;
  /* sort the list of ancestors by name order */
  max = k;
  i = 1;
  while (i < max)
    {
      for (k = i + 1; k < max; k++)
	if (ctxt->name[i] > ctxt->name[k])
	  {
	    j = ctxt->name[i];
	    ctxt->name[i] = ctxt->name[k];
	    ctxt->name[k] = j;
	    j = ctxt->names_nb[i];
	    ctxt->names_nb[i] = ctxt->names_nb[k];
	    ctxt->names_nb[k] = j;
	    j = ctxt->attrType[i];
	    ctxt->attrType[i] = ctxt->attrType[k];
	    ctxt->attrType[k] = j;
	    cur = ctxt->attrText[i];
	    ctxt->attrText[i] = ctxt->attrText[k];
	    ctxt->attrText[k] = cur;
	  }
      i++;
    }

  /* Get the schema name of the main element */
  schemaName = TtaGetSSchemaName (ctxt->schema);
  isHTML = (strcmp (schemaName, "HTML") == 0);
  tsch = GetPExtension (doc, ctxt->schema, css);
  if (tsch && cssRule)
    ParseCSSRule (NULL, tsch, (PresentationContext) ctxt, cssRule, css, isHTML);
  /* future CSS rules should apply */
  DoApply = TRUE;
  return (selector);
}

/*----------------------------------------------------------------------
   ParseStyleDeclaration: parse a style declaration    
   stored in the style element of a document                       
   We expect the style string to be of the form:                   
   [                                                                
   e.g: pinky, awful { color: pink, font-family: helvetica }        
  ----------------------------------------------------------------------*/
static void  ParseStyleDeclaration (Element el, char *cssRule, Document doc,
				    CSSInfoPtr css, ThotBool destroy)
{
  GenericContext      ctxt;
  char                sel[MAX_ANCESTORS * 50];
  char               *decl_end;
  char               *sel_end;
  char               *selector;

  /* separate the selectors string */
  cssRule = SkipBlanksAndComments (cssRule);
  decl_end = cssRule;
  while (*decl_end != EOS && *decl_end != '{')
    decl_end++;
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

  while (selector && *selector != EOS)
    selector = ParseGenericSelector (selector, cssRule, sel, ctxt, doc, css);
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
int         IsImplicitClassName (char *class, Document doc)
{
   char         name[200];
   char        *cur = name;
   char        *first; 
   char         save;
   SSchema      schema;

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
 *									*  
 *  Functions needed for support of HTML: translate to CSS equivalent   *
 *									*  
 ************************************************************************/

/*----------------------------------------------------------------------
   HTMLSetBackgroundColor:
  ----------------------------------------------------------------------*/
void    HTMLSetBackgroundColor (Document doc, Element el, char *color)
{
   char             css_command[100];

   sprintf (css_command, "background-color: %s", color);
   ParseHTMLSpecificStyle (el, css_command, doc, 0, FALSE);
}

/*----------------------------------------------------------------------
   HTMLSetForegroundColor:                                        
  ----------------------------------------------------------------------*/
void HTMLSetForegroundColor (Document doc, Element el, char *color)
{
   char           css_command[100];

   sprintf (css_command, "color: %s", color);
   ParseHTMLSpecificStyle (el, css_command, doc, 0, FALSE);
}

/*----------------------------------------------------------------------
   HTMLResetBackgroundColor:                                      
  ----------------------------------------------------------------------*/
void HTMLResetBackgroundColor (Document doc, Element el)
{
   char           css_command[100];

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
   char           css_command[100];

   /* it's not necessary to well know the current color but it must be valid */
   sprintf (css_command, "color: red");
   ParseHTMLSpecificStyle (el, css_command, doc, 0, TRUE);
}

/*----------------------------------------------------------------------
   HTMLSetAlinkColor:                                             
  ----------------------------------------------------------------------*/
void HTMLSetAlinkColor (Document doc, char *color)
{
   char           css_command[100];

   sprintf (css_command, "a:link { color: %s }", color);
   ApplyCSSRules (NULL, css_command, doc, FALSE);
}

/*----------------------------------------------------------------------
   HTMLSetAactiveColor:                                           
  ----------------------------------------------------------------------*/
void HTMLSetAactiveColor (Document doc, char *color)
{
   char           css_command[100];

   sprintf (css_command, "a:active { color: %s }", color);
   ApplyCSSRules (NULL, css_command, doc, FALSE);
}

/*----------------------------------------------------------------------
   HTMLSetAvisitedColor:                                          
  ----------------------------------------------------------------------*/
void                HTMLSetAvisitedColor (Document doc, char *color)
{
   char           css_command[100];

   sprintf (css_command, "a:visited { color: %s }", color);
   ApplyCSSRules (NULL, css_command, doc, FALSE);
}

/*----------------------------------------------------------------------
   HTMLResetAlinkColor:                                           
  ----------------------------------------------------------------------*/
void                HTMLResetAlinkColor (Document doc)
{
   char           css_command[100];

   sprintf (css_command, "a:link { color: red }");
   ApplyCSSRules (NULL, css_command, doc, TRUE);
}

/*----------------------------------------------------------------------
   HTMLResetAactiveColor:                                                 
  ----------------------------------------------------------------------*/
void                HTMLResetAactiveColor (Document doc)
{
   char           css_command[100];

   sprintf (css_command, "a:active { color: red }");
   ApplyCSSRules (NULL, css_command, doc, TRUE);
}

/*----------------------------------------------------------------------
   HTMLResetAvisitedColor:                                        
  ----------------------------------------------------------------------*/
void                HTMLResetAvisitedColor (Document doc)
{
   char           css_command[100];

   sprintf (css_command, "a:visited { color: red }");
   ApplyCSSRules (NULL, css_command, doc, TRUE);
}

/*----------------------------------------------------------------------
  ApplyCSSRules: parse a CSS Style description stored in the
  header of a HTML document.
  ----------------------------------------------------------------------*/
void ApplyCSSRules (Element el, char *cssRule, Document doc, ThotBool destroy)
{
  CSSInfoPtr        css;

  css = SearchCSS (doc, NULL, el);
  if (css == NULL)
    /* create the document css */
    css = AddCSS (doc, doc, CSS_DOCUMENT_STYLE, NULL, NULL, el);
  ParseStyleDeclaration (el, cssRule, doc, css, destroy); 
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
   The parameter url gives the URL of the style shheet parsed.
   Parameter numberOfLinesRead indicates the number of lines already
   read in the file.
   Parameter withUndo indicates whether the changes made in the document
   structure and content have to be registered in the Undo queue or not.
  ----------------------------------------------------------------------*/
char ReadCSSRules (Document docRef, CSSInfoPtr css, char *buffer, char *url,
		   int numberOfLinesRead, ThotBool withUndo,
		   Element styleElement)
{
  DisplayMode         dispMode;
  char                c;
  char               *cssRule, *base, *saveDocURL, *ptr;
  int                 index;
  int                 CSSindex;
  int                 CSScomment;
  int                 import;
  int                 openRule;
  int                 newlines;
  ThotBool            HTMLcomment;
  ThotBool            toParse, eof, quoted;
  ThotBool            ignoreMedia, media;
  ThotBool            noRule, ignoreImport, skip;
  CSSInfoPtr          refcss = NULL;

  CSScomment = MAX_CSS_LENGTH;
  HTMLcomment = FALSE;
  CSSindex = 0;
  toParse = FALSE;
  noRule = FALSE;
  media =  FALSE;
  ignoreImport = FALSE;
  ignoreMedia = FALSE;
  import = MAX_CSS_LENGTH;
  eof = FALSE;
  openRule = 0;
  c = SPACE;
  index = 0;
  base = NULL;
  quoted = FALSE;
  skip = FALSE;
  /* number of new lines parsed */
  newlines = 0;
  /* avoid too many redisplay */
  dispMode = TtaGetDisplayMode (docRef);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (docRef, DeferredDisplay);

  /* look for the CSS context */
  if (css == NULL)
    css = SearchCSS (docRef, NULL, styleElement);
  if (css == NULL)
    css = AddCSS (docRef, docRef, CSS_DOCUMENT_STYLE, NULL, NULL,
		  styleElement);
  /* look for the CSS descriptor that points to the extension schema */
  refcss = css;
  if (import)
    {
      while (refcss && refcss->category == CSS_IMPORT)
	refcss = refcss->NextCSS;
    }

  /* register parsed CSS file and the document to which CSS are to be applied*/
  ParsedDoc = docRef;
  if (url)
    DocURL = url;
  else
    /* the CSS source in within the document itself */
    DocURL = DocumentURLs[docRef];
  LineNumber = numberOfLinesRead + 1;
  NewLineSkipped = 0;
  while (CSSindex < MAX_CSS_LENGTH && c != EOS && !eof)
    {
      c = buffer[index++];
      eof = (c == EOS);
      CSSbuffer[CSSindex] = c;
      if (skip)
	{
	  if (c == '}')
	    {
	      /* end of the @font-face */
	      skip = FALSE;
	      import = MAX_CSS_LENGTH;
	      noRule = TRUE;
	      CSSindex = 0;
	    }
	  if (c == EOL)
	    LineNumber++;
	  c = CR;
	}
      else if (CSScomment == MAX_CSS_LENGTH ||
	  c == '*' || c == '/' || c == '<')
	{
	  /* we're not within a comment or we're parsing * or / */
	  switch (c)
	    {
	    case '@': /* perhaps an import primitive */
	      if (!quoted)
		import = CSSindex;
	      break;
	    case ';':
	      if (!quoted && !media && import != MAX_CSS_LENGTH)
		{ 
		  if (strncasecmp (&CSSbuffer[import+1], "import", 6))
		    /* it's not an import */
		    import = MAX_CSS_LENGTH;
		  /* save the text */
		  noRule = TRUE;
		}
	      break;
	    case '*':
	      if (!quoted && CSScomment == MAX_CSS_LENGTH && CSSindex > 0 &&
		  CSSbuffer[CSSindex - 1] == '/')
		/* start a comment */
		CSScomment = CSSindex - 1;
	      break;
	    case '/':
	      if (!quoted && CSSindex > 1 && CSScomment != MAX_CSS_LENGTH &&
		  CSSbuffer[CSSindex - 1] == '*')
		{
		  /* close a comment:and ignore its contents */
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
	      else if (!quoted && CSScomment == MAX_CSS_LENGTH && CSSindex > 0 &&
		       CSSbuffer[CSSindex - 1] ==  '<')
		{
		  /* this is the closing tag ! */
		  CSSindex -= 2; /* remove </ from the CSS string */
		  noRule = TRUE;
		} 
	      break;
	    case '<':
	      if (!quoted && CSScomment == MAX_CSS_LENGTH)
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
	      if (!quoted && CSSindex > 0 && CSSbuffer[CSSindex - 1] == '-' &&
		  HTMLcomment)
		/* CSS within an HTML comment */
		noRule = TRUE;
	      break;
	    case '>':
	      if (!quoted && HTMLcomment)
		noRule = TRUE;
	      break;
	    case ' ':
	      if (!quoted && import != MAX_CSS_LENGTH && openRule == 0)
		media = !strncasecmp (&CSSbuffer[import+1], "media", 5);
	      break;
	    case '{':
	      if (!quoted)
		{
		  openRule++;
		  if (import != MAX_CSS_LENGTH && openRule == 1 && media)
		    {
		      /* is it the screen concerned? */
		      CSSbuffer[CSSindex+1] = EOS;
		      if (TtaIsPrinting ())
			base = strstr (&CSSbuffer[import], "print");
		      else
			base = strstr (&CSSbuffer[import], "screen");
		      if (base == NULL)
			base = strstr (&CSSbuffer[import], "all");
		      if (base == NULL)
			ignoreMedia = TRUE;
		      noRule = TRUE;
		    }
		  else if (import != MAX_CSS_LENGTH &&
			   !strncasecmp (&CSSbuffer[import], "@font-face", 10))
		    skip = TRUE;
		}
	      break;
	    case '}':
	      if (!quoted)
		{
		  openRule--;
		  if (import != MAX_CSS_LENGTH && openRule == 0)
		    {
		      import = MAX_CSS_LENGTH;
		      noRule = TRUE;
		      ignoreMedia = FALSE;
		      media = FALSE;
		    }
		  else
		    toParse = TRUE;
		}
	      break;
	    case '"':
	      if (quoted)
		{
		  if (CSSbuffer[CSSindex - 1] != '\\')
		    quoted = FALSE;
		}
	      else
		quoted = TRUE;
	      break;
	    default:
	      if (c == EOL)
		newlines++;
	      break;
	    }
        }
      else if (c == EOL)
	LineNumber++;
      if (c != CR)
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
              if (!noRule && !ignoreMedia)
		{
		  /* future import rules must be ignored */
		  ignoreImport = TRUE;
		  ParseStyleDeclaration (NULL, CSSbuffer, docRef, refcss, FALSE);
		  LineNumber += newlines;
		  newlines = 0;
		  NewLineSkipped = 0;
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
			  *cssRule = EOS;
			}
		    }
		  else if (*cssRule == '"')
		    {
		      /*
			Do we have to accept single quotes?
			Double quotes are acceted here.
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
		      if (TtaIsPrinting ())
			ignoreImport = (strncasecmp (cssRule, "print", 5) &&
					strncasecmp (cssRule, "all", 3));
		      else
			ignoreImport = (strncasecmp (cssRule, "screen", 6) &&
					strncasecmp (cssRule, "all", 3));
		    }
		  if (!ignoreImport)
		    {
		      /* save the displayed URL when an error is reported */
		      saveDocURL = DocURL;
		      ptr = TtaStrdup (base);
		      /* get the CSS URI in UTF-8 */
		      ptr = ReallocUTF8String (ptr, docRef);
		      LoadStyleSheet (base, docRef, NULL, css,
				      css->media[docRef],
				      css->category == CSS_USER_STYLE);
		      /* restore the displayed URL when an error is reported */
		      DocURL = saveDocURL;
		      TtaFreeMemory (ptr);
		    }
		  /* restore the number of lines */
		  LineNumber = newlines;
		  newlines = 0;
		  import = MAX_CSS_LENGTH;
		}
		
	    }
	  toParse = FALSE;
	  noRule = FALSE;
	  CSSindex = 0;
        }
    }
  /* restore the display mode */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (docRef, dispMode);

  /* Prepare the context for style attributes */
  DocURL = DocumentURLs[docRef];
  LineNumber = -1;
  return (c);
}

