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
 * Author: I. Vatton
 *
 */

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "undo.h"
#include "fetchHTMLname.h"

typedef struct _BackgroundImageCallbackBlock
{
  Element                     el;
  PSchema                     tsch;
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
#include "html2thot_f.h"
#include "styleparser_f.h"

#define MAX_BUFFER_LENGTH 200
/*
 * A PropertyParser is a function used to parse  the
 * description substring associated to a given style attribute
 * e.g. : "red" for a color attribute or "12pt bold helvetica"
 * for a font attribute.
 */
#ifdef __STDC__
typedef STRING (*PropertyParser) (Element element,
				    PSchema tsch,
				    PresentationContext context,
				    STRING cssRule,
				    CSSInfoPtr css,
				    ThotBool isHTML);
#else
typedef STRING (*PropertyParser) ();
#endif

/* Description of the set of CSS properties supported */
typedef struct CSSProperty
  {
     STRING               name;
     PropertyParser       parsing_function;
  }
CSSProperty;

#include "HTMLstyleColor.h"

struct unit_def
{
   STRING              sign;
   unsigned int        unit;
};

static struct unit_def CSSUnitNames[] =
{
   {TEXT ("pt"), STYLE_UNIT_PT},
   {TEXT ("pc"), STYLE_UNIT_PC},
   {TEXT ("in"), STYLE_UNIT_IN},
   {TEXT ("cm"), STYLE_UNIT_CM},
   {TEXT ("mm"), STYLE_UNIT_MM},
   {TEXT ("em"), STYLE_UNIT_EM},
   {TEXT ("px"), STYLE_UNIT_PX},
   {TEXT ("ex"), STYLE_UNIT_XHEIGHT},
   {TEXT ("%"), STYLE_UNIT_PERCENT}
};

#define NB_UNITS (sizeof(CSSUnitNames) / sizeof(struct unit_def))

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static unsigned int hexa_val (CHAR_T c)
#else
static unsigned int hexa_val (c)
CHAR_T                c;
#endif
{
   if (c >= '0' && c <= '9')
      return (c - '0');
   if (c >= 'a' && c <= 'f')
      return (c - 'a' + 10);
   if (c >= 'A' && c <= 'F')
      return (c - 'A' + 10);
   return (0);
}

/*----------------------------------------------------------------------
   SkipWord:                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       SkipWord (STRING ptr)
#else
static STRING       SkipWord (ptr)
STRING              ptr;
#endif
{
  while (isalnum(*ptr) || *ptr == '-' || *ptr == '%')
    ptr++;
  return (ptr);
}

/*----------------------------------------------------------------------
   SkipBlanksAndComments:                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING       SkipBlanksAndComments (STRING ptr)
#else
STRING       SkipBlanksAndComments (ptr)
STRING       ptr;
#endif
{
  ptr = TtaSkipBlanks (ptr);
  while (ptr[0] == '/' && ptr[1] == '*')
    {
      /* look for the end of the comment */
      ptr = &ptr[2];
      while (ptr[0] != EOS && (ptr[0] != '*' || ptr[1] != '/'))
	ptr++;
      if (ptr[0] != EOS)
	ptr = &ptr[2];
      ptr = TtaSkipBlanks (ptr);
    }
  return (ptr);
}

/*----------------------------------------------------------------------
   SkipQuotedString:                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       SkipQuotedString (STRING ptr, CHAR_T quote)
#else
static STRING       SkipQuotedString (ptr, quote)
STRING              ptr;
CHAR_T		   quote;
#endif
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
   SkipProperty:                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING       SkipProperty (STRING ptr)
#else
STRING       SkipProperty (ptr)
STRING              ptr;
#endif
{
  while (*ptr != EOS && *ptr != ';' && *ptr != '}')
    ptr++;
  return (ptr);
}

/*----------------------------------------------------------------------
   ParseCSSUnit :                                                  
   parse a CSS Unit substring and returns the corresponding      
   value and its unit.                                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSUnit (STRING cssRule, PresentationValue *pval)
#else
static STRING       ParseCSSUnit (cssRule, pval)
STRING              cssRule;
PresentationValue  *pval;
#endif
{
  int                 val = 0;
  int                 minus = 0;
  int                 valid = 0;
  int                 f = 0;
  unsigned int        uni;
  ThotBool            real = FALSE;

  pval->typed_data.unit = STYLE_UNIT_REL;
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
      cssRule = SkipWord (cssRule);
      pval->typed_data.unit = STYLE_UNIT_INVALID;
      pval->typed_data.value = 0;
    }
  else
    {
      cssRule = SkipBlanksAndComments (cssRule);
      for (uni = 0; uni < NB_UNITS; uni++)
	{
	  if (!ustrncasecmp (CSSUnitNames[uni].sign, cssRule,
			     ustrlen (CSSUnitNames[uni].sign)))
	    {
	      pval->typed_data.unit = CSSUnitNames[uni].unit;
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
	      return (cssRule + ustrlen (CSSUnitNames[uni].sign));
	    }
	}

      /* not in the list of predefined units */
      pval->typed_data.unit = STYLE_UNIT_REL;
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

/************************************************************************
 *									*  
 *			PARSING FUNCTIONS				*
 *									*  
 ************************************************************************/

/************************************************************************
 *									*  
 *	CORE OF THE CSS PARSER : THESE TAKE THE CSS STRINGS 		*
 *	PRODUCE THE CORECT DRIVER CONTEXT, AND DO THE PARSING		*
 *									*  
 ************************************************************************/

/*----------------------------------------------------------------------
   ParseCSSBorderTopWidth : parse a CSS BorderTopWidth
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSBorderTopWidth (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSBorderTopWidth (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderRightWidth : parse a CSS BorderRightWidth
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSBorderRightWidth (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSBorderRightWidth (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderBottomWidth : parse a CSS BorderBottomWidth
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSBorderBottomWidth (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSBorderBottomWidth (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderLeftWidth : parse a CSS BorderLeftWidth
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSBorderLeftWidth (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSBorderLeftWidth (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderWidth : parse a CSS BorderWidth
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSBorderWidth (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSBorderWidth (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderTop : parse a CSS BorderTop
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSBorderTop (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSBorderTop (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderRight : parse a CSS BorderRight
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSBorderRight (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSBorderRight (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderBottom : parse a CSS BorderBottom
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSBorderBottom (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSBorderBottom (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderLeft : parse a CSS BorderLeft
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSBorderLeft (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSBorderLeft (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderColor : parse a CSS border-color        
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSBorderColor (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSBorderColor (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderStyle : parse a CSS border-style        
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSBorderStyle (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSBorderStyle (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorder : parse a CSS border        
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSBorder (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSBorder (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSClear : parse a CSS clear attribute string    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSClear (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSClear (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSDisplay : parse a CSS display attribute string        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSDisplay (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSDisplay (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
   PresentationValue   pval;

   pval.typed_data.unit = STYLE_UNIT_REL;
   pval.typed_data.real = FALSE;
   cssRule = SkipBlanksAndComments (cssRule);
   if (!ustrncasecmp (cssRule, TEXT("block"), 5))
     {
	pval.typed_data.value = STYLE_NOTINLINE;
	TtaSetStylePresentation (PRLine, element, tsch, context, pval);
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("inline"), 6))
     {
	pval.typed_data.value = STYLE_INLINE;
	TtaSetStylePresentation (PRLine, element, tsch, context, pval);
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("none"), 4))
     {
	pval.typed_data.value = STYLE_HIDE;
	TtaSetStylePresentation (PRVisibility, element, tsch, context, pval);
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("list-item"), 9))
     cssRule = SkipProperty (cssRule);
   else
     fprintf (stderr, "invalid display value %s\n", cssRule);

   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSFloat : parse a CSS float attribute string    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSFloat (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSFloat (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSLetterSpacing : parse a CSS letter-spacing    
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSLetterSpacing (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSLetterSpacing (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSListStyleType : parse a CSS list-style-type
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSListStyleType (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSListStyleType (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSListStyleImage : parse a CSS list-style-image
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSListStyleImage (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSListStyleImage (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSListStylePosition : parse a CSS list-style-position
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSListStylePosition (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSListStylePosition (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSListStyle : parse a CSS list-style            
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSListStyle (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSListStyle (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSTextAlign : parse a CSS text-align            
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSTextAlign (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSTextAlign (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
   PresentationValue   align;
   PresentationValue   justify;

   align.typed_data.value = 0;
   align.typed_data.unit = STYLE_UNIT_REL;
   align.typed_data.real = FALSE;
   justify.typed_data.value = 0;
   justify.typed_data.unit = STYLE_UNIT_REL;
   justify.typed_data.real = FALSE;

   cssRule = SkipBlanksAndComments (cssRule);
   if (!ustrncasecmp (cssRule, TEXT("left"), 4))
     {
	align.typed_data.value = AdjustLeft;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("right"), 5))
     {
	align.typed_data.value = AdjustRight;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("center"), 6))
     {
	align.typed_data.value = Centered;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("justify"), 7))
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
       TtaSetStylePresentation (PRAdjust, element, tsch, context, align);
     }
   if (justify.typed_data.value)
     {
       TtaSetStylePresentation (PRJustify, element, tsch, context, justify);
       TtaSetStylePresentation (PRHyphenate, element, tsch, context, justify);
     }
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSTextIndent : parse a CSS text-indent          
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSTextIndent (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSTextIndent (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
   PresentationValue   pval;

   cssRule = SkipBlanksAndComments (cssRule);
   cssRule = ParseCSSUnit (cssRule, &pval);
   if (pval.typed_data.unit == STYLE_UNIT_INVALID)
     return (cssRule);
   /* install the attribute */
   TtaSetStylePresentation (PRIndent, element, tsch, context, pval);
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSTextTransform : parse a CSS text-transform    
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSTextTransform (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSTextTransform (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSVerticalAlign : parse a CSS vertical-align    
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSVerticalAlign (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSVerticalAlign (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSWhiteSpace : parse a CSS white-space          
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSWhiteSpace (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSWhiteSpace (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
   cssRule = SkipBlanksAndComments (cssRule);
   if (!ustrncasecmp (cssRule, TEXT("normal"), 6))
     cssRule = SkipWord (cssRule);
   else if (!ustrncasecmp (cssRule, TEXT("pre"), 3))
     cssRule = SkipWord (cssRule);
   else
     return (cssRule);
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSWordSpacing : parse a CSS word-spacing        
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSWordSpacing (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSWordSpacing (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSLineSpacing : parse a CSS font leading string 
   we expect the input string describing the attribute to be     
   value% or value                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSLineSpacing (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSLineSpacing (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
   PresentationValue   lead;

   cssRule = ParseCSSUnit (cssRule, &lead);
   if (lead.typed_data.unit == STYLE_UNIT_INVALID)
     {
       /* invalid line spacing */
	return (cssRule);
     }
   /* install the new presentation */
   TtaSetStylePresentation (PRLineSpacing, element, tsch, context, lead);
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSFontSize : parse a CSS font size attr string  
   we expect the input string describing the attribute to be     
   xx-small, x-small, small, medium, large, x-large, xx-large      
   or an absolute size, or an imcrement relative to the parent     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSFontSize (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSFontSize (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
   PresentationValue   pval;
   STRING              ptr = NULL;
   ThotBool	       real;

   pval.typed_data.real = FALSE;
   cssRule = SkipBlanksAndComments (cssRule);
   if (!ustrncasecmp (cssRule, TEXT("larger"), 6))
     {
	pval.typed_data.unit = STYLE_UNIT_PERCENT;
	pval.typed_data.value = 130;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("smaller"), 7))
     {
	pval.typed_data.unit = STYLE_UNIT_PERCENT;
	pval.typed_data.value = 80;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("xx-small"), 8))
     {
	pval.typed_data.unit = STYLE_UNIT_REL;
	pval.typed_data.value = 1;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("x-small"), 7))
     {
	pval.typed_data.unit = STYLE_UNIT_REL;
	pval.typed_data.value = 2;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("small"), 5))
     {
	pval.typed_data.unit = STYLE_UNIT_REL;
	pval.typed_data.value = 3;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("medium"), 6))
     {
	pval.typed_data.unit = STYLE_UNIT_REL;
	pval.typed_data.value = 4;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("large"), 5))
     {
	pval.typed_data.unit = STYLE_UNIT_REL;
	pval.typed_data.value = 5;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("x-large"), 7))
     {
	pval.typed_data.unit = STYLE_UNIT_REL;
	pval.typed_data.value = 6;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("xx-large"), 8))
     {
	pval.typed_data.unit = STYLE_UNIT_REL;
	pval.typed_data.value = 7;
	cssRule = SkipWord (cssRule);
     }
   else
     {
       /* look for a '/' within the current cssRule */
       ptr = ustrchr (cssRule, TEXT('/'));
       if (ptr != NULL)
	 {
	   /* keep the line spacing rule */
	   ptr[0] = EOS;
	   ptr = &ptr[1];
	 }
       cssRule = ParseCSSUnit (cssRule, &pval);
       if (pval.typed_data.unit == STYLE_UNIT_INVALID ||
           pval.typed_data.value < 0)
	 return (cssRule);
       if (pval.typed_data.unit == STYLE_UNIT_REL && pval.typed_data.value > 0)
	 /* CSS relative sizes have to be higher than Thot ones */
	 pval.typed_data.value += 1;
       else 
	 {
	   real = pval.typed_data.real;
	   if (pval.typed_data.unit == STYLE_UNIT_EM)
	     {
	       if (real)
		 {
		   pval.typed_data.value /= 10;
		   pval.typed_data.real = FALSE;
		   real = FALSE;
		 }
	       else
		 pval.typed_data.value *= 100;
	       pval.typed_data.unit = STYLE_UNIT_PERCENT;
	     }
	 }

     }

   /* install the presentation style */
   TtaSetStylePresentation (PRSize, element, tsch, context, pval);

   if (ptr != NULL)
     cssRule = ParseCSSLineSpacing (element, tsch, context, ptr, css, isHTML);
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSFontFamily : parse a CSS font family string   
   we expect the input string describing the attribute to be     
   a common generic font style name                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSFontFamily (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSFontFamily (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue   font;
  CHAR_T		      quoteChar;

  font.typed_data.value = 0;
  font.typed_data.unit = STYLE_UNIT_REL;
  font.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  if (*cssRule == '"' || *cssRule == '\'')
     {
     quoteChar = *cssRule;
     cssRule++;
     }
  else
     quoteChar = '\0';

  if (!ustrncasecmp (cssRule, TEXT("times"), 5))
      font.typed_data.value = STYLE_FONT_TIMES;
  else if (!ustrncasecmp (cssRule, TEXT("serif"), 5))
      font.typed_data.value = STYLE_FONT_TIMES;
  else if (!ustrncasecmp (cssRule, TEXT("helvetica"), 9) ||
	   !ustrncasecmp (cssRule, TEXT("verdana"), 7))
      font.typed_data.value = STYLE_FONT_HELVETICA;
  else if (!ustrncasecmp (cssRule, TEXT("sans-serif"), 10))
      font.typed_data.value = STYLE_FONT_HELVETICA;
  else if (!ustrncasecmp (cssRule, TEXT("courier"), 7))
      font.typed_data.value = STYLE_FONT_COURIER;
  else if (!ustrncasecmp (cssRule, TEXT("monospace"), 9))
      font.typed_data.value = STYLE_FONT_COURIER;
  else
    /* unknown font name.  Skip it */
    {
      if (quoteChar)
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
     cssRule = SkipProperty (cssRule);
     /* install the new presentation */
     TtaSetStylePresentation (PRFont, element, tsch, context, font);
     }
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSFontWeight : parse a CSS font weight string   
   we expect the input string describing the attribute to be     
   normal, bold, bolder, lighter, 100, 200, 300, ... 900, inherit.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSFontWeight (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSFontWeight (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
   PresentationValue   weight;

   weight.typed_data.value = 0;
   weight.typed_data.unit = STYLE_UNIT_REL;
   weight.typed_data.real = FALSE;
   cssRule = SkipBlanksAndComments (cssRule);
   if (!ustrncasecmp (cssRule, TEXT("100"), 3) && !isalpha (cssRule[3]))
     {
	weight.typed_data.value = -3;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("200"), 3) && !isalpha (cssRule[3]))
     {
	weight.typed_data.value = -2;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("300"), 3) && !isalpha (cssRule[3]))
     {
	weight.typed_data.value = -1;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("normal"), 6) ||
	    (!ustrncasecmp (cssRule, TEXT("400"), 3) && !isalpha (cssRule[3])))
     {
	weight.typed_data.value = 0;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("500"), 3) && !isalpha (cssRule[3]))
     {
	weight.typed_data.value = +1;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("600"), 3) && !isalpha (cssRule[3]))
     {
	weight.typed_data.value = +2;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("bold"), 4) ||
	    (!ustrncasecmp (cssRule, TEXT("700"), 3) && !isalpha (cssRule[3])))
     {
	weight.typed_data.value = +3;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("800"), 3) && !isalpha (cssRule[3]))
     {
	weight.typed_data.value = +4;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("900"), 3) && !isalpha (cssRule[3]))
     {
	weight.typed_data.value = +5;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("inherit"), 7) ||
	    !ustrncasecmp (cssRule, TEXT("bolder"), 6) ||
	    !ustrncasecmp (cssRule, TEXT("lighter"), 7))
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
       weight.typed_data.value = STYLE_WEIGHT_BOLD;
    else
       weight.typed_data.value = STYLE_WEIGHT_NORMAL;

   /* install the new presentation */
   TtaSetStylePresentation (PRWeight, element, tsch, context, weight);
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSFontVariant : parse a CSS font variant string     
   we expect the input string describing the attribute to be     
   normal or small-caps
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSFontVariant (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSFontVariant (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
   PresentationValue   style;

   style.typed_data.value = 0;
   style.typed_data.unit = STYLE_UNIT_REL;
   style.typed_data.real = FALSE;
   cssRule = SkipBlanksAndComments (cssRule);
   if (!ustrncasecmp (cssRule, TEXT("small-caps"), 10))
     {
       /* Not supported yet */
       cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("normal"), 6))
     {
       /* Not supported yet */
       cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("inherit"), 7))
     {
       /* Not supported yet */
       cssRule = SkipWord (cssRule);
     }
   else
       return (cssRule);

   return (cssRule);
}


/*----------------------------------------------------------------------
   ParseCSSFontStyle : parse a CSS font style string     
   we expect the input string describing the attribute to be     
   italic, oblique or normal                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSFontStyle (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSFontStyle (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
   PresentationValue   style;
   PresentationValue   size;

   style.typed_data.value = 0;
   style.typed_data.unit = STYLE_UNIT_REL;
   style.typed_data.real = FALSE;
   size.typed_data.value = 0;
   size.typed_data.unit = STYLE_UNIT_REL;
   size.typed_data.real = FALSE;
   cssRule = SkipBlanksAndComments (cssRule);
   if (!ustrncasecmp (cssRule, TEXT("italic"), 6))
     {
	style.typed_data.value = STYLE_FONT_ITALICS;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("oblique"), 7))
     {
	style.typed_data.value = STYLE_FONT_OBLIQUE;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("normal"), 6))
     {
	style.typed_data.value = STYLE_FONT_ROMAN;
	cssRule = SkipWord (cssRule);
     }
   else
     {
       /* invalid font style */
	return (cssRule);
     }

   /*
    * install the new presentation.
    */
   if (style.typed_data.value != 0)
        TtaSetStylePresentation (PRStyle, element, tsch, context, style);
   if (size.typed_data.value != 0)
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
   ParseCSSFont : parse a CSS font attribute string      
   we expect the input string describing the attribute to be     
   !!!!!!                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSFont (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSFont (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  STRING            ptr;

  cssRule = SkipBlanksAndComments (cssRule);
  if (!ustrncasecmp (cssRule, TEXT("caption"), 7))
    ;
  else if (!ustrncasecmp (cssRule, TEXT("icon"), 4))
    ;
  else if (!ustrncasecmp (cssRule, TEXT("menu"), 4))
    ;
  else if (!ustrncasecmp (cssRule, TEXT("message-box"), 11))
    ;
  else if (!ustrncasecmp (cssRule, TEXT("small-caption"), 13))
    ;
  else if (!ustrncasecmp (cssRule, TEXT("status-bar"), 10))
    ;
  else
      {
	ptr = cssRule;
	cssRule = ParseCSSFontStyle (element, tsch, context, cssRule, css, isHTML);
	if (ptr == cssRule)
	  cssRule = ParseCSSFontVariant (element, tsch, context, cssRule, css, isHTML);
	if (ptr == cssRule)
	  cssRule = ParseCSSFontWeight (element, tsch, context, cssRule, css, isHTML);
	cssRule = ParseCSSFontSize (element, tsch, context, cssRule, css, isHTML);
	if (*cssRule == '/')
	  {
	    cssRule++;
	    SkipBlanksAndComments (cssRule);
	    cssRule = SkipWord (cssRule);
	  }
	cssRule = ParseCSSFontFamily (element, tsch, context, cssRule, css, isHTML);
	cssRule = SkipBlanksAndComments (cssRule);
	while (*cssRule != ';' && *cssRule != EOS)
	  {
	    /* now skip remainding info */
	    cssRule++;
	  }
      }
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSTextDecoration : parse a CSS text decor string   
   we expect the input string describing the attribute to be     
   underline, overline, line-through, box, shadowbox, box3d,       
   cartouche, blink or none                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSTextDecoration (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSTextDecoration (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
   PresentationValue   decor;

   decor.typed_data.value = 0;
   decor.typed_data.unit = STYLE_UNIT_REL;
   decor.typed_data.real = FALSE;
   cssRule = SkipBlanksAndComments (cssRule);
   if (!ustrncasecmp (cssRule, TEXT("underline"), ustrlen (TEXT("underline"))))
     {
	decor.typed_data.value = Underline;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("overline"), ustrlen (TEXT("overline"))))
     {
	decor.typed_data.value = Overline;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("line-through"), ustrlen (TEXT("line-through"))))
     {
	decor.typed_data.value = CrossOut;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("box"), ustrlen (TEXT("box"))))
     {
       /* the box text-decoration attribute is not yet supported */
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("boxshadow"), ustrlen (TEXT("boxshadow"))))
     {
       /* the boxshadow text-decoration attribute is not yet supported */
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("box3d"), ustrlen (TEXT("box3d"))))
     {
       /* the box3d text-decoration attribute is not yet supported */
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("cartouche"), ustrlen (TEXT("cartouche"))))
     {
	/*the cartouche text-decoration attribute is not yet supported */
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("blink"), ustrlen (TEXT("blink"))))
     {
	/*the blink text-decoration attribute will not be supported */
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("none"), ustrlen (TEXT("none"))))
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
       TtaSetStylePresentation (PRUnderline, element, tsch, context, decor);
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
static STRING       ParseCSSColor (STRING cssRule, PresentationValue * val)
#else
static STRING       ParseCSSColor (cssRule, val)
STRING              cssRule;
PresentationValue  *val;
#endif
{
  CHAR_T                colname[100];
  unsigned short      redval = (unsigned short) -1;
  unsigned short      greenval = 0;	/* composant of each RGB       */
  unsigned short      blueval = 0;	/* default to red if unknown ! */
  unsigned int        i, len;
  int                 r, g, b;
  int                 best = 0;	/* best color in list found */
  ThotBool            failed;

  cssRule = SkipBlanksAndComments (cssRule);
  val->typed_data.unit = STYLE_UNIT_INVALID;
  val->typed_data.real = FALSE;
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
	  cssRule = &cssRule[3];
	}
      else if ((!isxdigit (cssRule[4])) || (!isxdigit (cssRule[5])))
	fprintf (stderr, "Invalid color encoding %s\n", cssRule - 1);
      else
	{
	  /* encoded as on 3 digits #FF00FF */
	  redval = hexa_val (cssRule[0]) * 16 + hexa_val (cssRule[1]);
	  greenval = hexa_val (cssRule[2]) * 16 + hexa_val (cssRule[3]);
	  blueval = hexa_val (cssRule[4]) * 16 + hexa_val (cssRule[5]);
	  cssRule = &cssRule[6];
	}
    }
  else if (!ustrncasecmp (cssRule, TEXT("rgb"), 3))
    {
      cssRule = &cssRule[3];
      cssRule = SkipBlanksAndComments (cssRule);
      if (*cssRule == '(')
	{
	  cssRule++;
	  cssRule = SkipBlanksAndComments (cssRule);
	  failed = FALSE;
	  if (*cssRule == '%')
	    {
	      /* encoded as rgb(%red,%green,&blue) */
	      usscanf (cssRule, TEXT("%%%d"), &r);
	      while (*cssRule != EOS && *cssRule != TEXT(','))
		cssRule++;
	      cssRule++;
	      usscanf (cssRule, TEXT("%%%d"), &g);
	      while (*cssRule != EOS && *cssRule != TEXT(','))
		cssRule++;
	      cssRule++;
	      usscanf (cssRule, TEXT("%%%d"), &b);
	      redval = (unsigned short)(r * 255 / 100);
	      greenval = (unsigned short)(g * 255 / 100);
	      blueval = (unsigned short)(b * 255 / 100);
	    }
	  else
	    {
	      /* encoded as rgb(red,green,blue) */
	      usscanf (cssRule, TEXT("%d"), &r);
	      while (*cssRule != EOS && *cssRule != TEXT(','))
		cssRule++;
	      cssRule++;
	      usscanf (cssRule, TEXT("%d"), &g);
	      while (*cssRule != EOS && *cssRule != TEXT(','))
		cssRule++;
	      cssRule++;
	      usscanf (cssRule, TEXT("%d"), &b);
	      redval = (unsigned short)r;
	      greenval = (unsigned short)g;
	      blueval = (unsigned short)b;
	    }
	  /* search the rgb end */
	  while (*cssRule != EOS && *cssRule != ')')
	    cssRule++;
	  cssRule++;
	}
      else
	cssRule = SkipProperty (cssRule);
    }
  else if (isalpha (*cssRule))
    {
      /* we expect a color name like "red", store it in colname */
      len = sizeof (colname) - 1;
      for (i = 0; i < len && cssRule[i] != EOS; i++)
	{
	  if (!isalnum (cssRule[i]) && cssRule[i] != EOS)
	    {
	      cssRule += i;
	      break;
	    }
	  colname[i] = cssRule[i];
	}
      colname[i] = EOS;
      
      /* Lookup the color name in our own color name database */
      for (i = 0; i < NBCOLORNAME; i++)
	if (!ustrcasecmp (ColornameTable[i].name, colname))
	  {
	    redval = ColornameTable[i].red;
	    greenval = ColornameTable[i].green;
	    blueval = ColornameTable[i].blue;
	    failed = FALSE;
	    i = NBCOLORNAME;
	  }
      /******** Lookup the color name in Thot color name database
      if (failed)
	{
	  TtaGiveRGB (colname, &redval, &greenval, &blueval);
	  failed = FALSE;
	}
      **********/
    }
  
  if (failed)
    {
      val->typed_data.value = 0;
      val->typed_data.unit = STYLE_UNIT_INVALID;
    }
  else
    {
      best = TtaGetThotColor (redval, greenval, blueval);
      val->typed_data.value = best;
      val->typed_data.unit = STYLE_UNIT_REL;
    }
  val->typed_data.real = FALSE;
 return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSHeight : parse a CSS height attribute                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSHeight (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSHeight (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
   cssRule = SkipBlanksAndComments (cssRule);

   /* first parse the attribute string */
   if (!ustrcasecmp (cssRule, TEXT("auto")))
     {
	cssRule = SkipWord (cssRule);
	/* ParseCSSHeight : auto */
	return (cssRule);
     }
   else
     cssRule = SkipProperty (cssRule);
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSWidth : parse a CSS width attribute           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSWidth (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSWidth (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
   cssRule = SkipBlanksAndComments (cssRule);

   /* first parse the attribute string */
   if (!ustrcasecmp (cssRule, TEXT("auto")))
     {
	cssRule = SkipWord (cssRule);
	return (cssRule);
     }
   else
     cssRule = SkipProperty (cssRule);
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSMarginTop : parse a CSS margin-top attribute  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSMarginTop (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSMarginTop (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue   margin;
  
  cssRule = SkipBlanksAndComments (cssRule);
  /* first parse the attribute string */
  cssRule = ParseCSSUnit (cssRule, &margin);
  if (margin.typed_data.unit != STYLE_UNIT_INVALID && margin.typed_data.value != 0)
    {
      TtaSetStylePresentation (PRTMargin, element, tsch, context, margin);
      if (margin.typed_data.value < 0)
	TtaSetStylePresentation (PRVertOverflow, element, tsch, context, margin);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSMarginBottom : parse a CSS margin-bottom      
   attribute                                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSMarginBottom (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSMarginBottom (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue   margin;
  
  cssRule = SkipBlanksAndComments (cssRule);
  /* first parse the attribute string */
  cssRule = ParseCSSUnit (cssRule, &margin);
  margin.typed_data.value = - margin.typed_data.value;
  /*if (margin.typed_data.unit != STYLE_UNIT_INVALID)
    TtaSetStylePresentation (PRBMargin, element, tsch, context, margin)*/;
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSMarginLeft : parse a CSS margin-left          
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSMarginLeft (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSMarginLeft (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue   margin;
  
  cssRule = SkipBlanksAndComments (cssRule);
  /* first parse the attribute string */
  cssRule = ParseCSSUnit (cssRule, &margin);
  if (margin.typed_data.unit != STYLE_UNIT_INVALID && margin.typed_data.value != 0)
    {
      TtaSetStylePresentation (PRLMargin, element, tsch, context, margin);
      if (margin.typed_data.value < 0)
	TtaSetStylePresentation (PRHorizOverflow, element, tsch, context, margin);
      TtaSetStylePresentation (PRRMargin, element, tsch, context, margin);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSMarginRight : parse a CSS margin-right        
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSMarginRight (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSMarginRight (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue   margin;
  
  cssRule = SkipBlanksAndComments (cssRule);
  /* first parse the attribute string */
  cssRule = ParseCSSUnit (cssRule, &margin);
  /*if (margin.typed_data.unit != STYLE_UNIT_INVALID)
      TtaSetStylePresentation (PRRMargin, element, tsch, context, margin)*/;
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSMargin : parse a CSS margin attribute string. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSMargin (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSMargin (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  STRING            ptrT, ptrR, ptrB, ptrL;

  ptrT = SkipBlanksAndComments (cssRule);
  /* First parse Margin-Top */
  ptrR = ParseCSSMarginTop (element, tsch, context, ptrT, css, isHTML);
  ptrR = SkipBlanksAndComments (ptrR);
  if (*ptrR == ';' || *ptrR == EOS || *ptrR == ',')
    {
      cssRule = ptrR;
      /* apply the Margin-Top to all */
      ptrR = ParseCSSMarginRight (element, tsch, context, ptrT, css, isHTML);
      ptrR = ParseCSSMarginBottom (element, tsch, context, ptrT, css, isHTML);
      ptrR = ParseCSSMarginLeft (element, tsch, context, ptrT, css, isHTML);
    }
  else
    {
      /* parse Margin-Right */
      ptrB = ParseCSSMarginRight (element, tsch, context, ptrR, css, isHTML);
      ptrB = SkipBlanksAndComments (ptrB);
      if (*ptrB == ';' || *ptrB == EOS || *ptrB == ',')
	{
	  cssRule = ptrB;
	  /* apply the Margin-Top to Margin-Bottom */
	  ptrB = ParseCSSMarginBottom (element, tsch, context, ptrT, css, isHTML);
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
   ParseCSSPaddingTop : parse a CSS PaddingTop
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSPaddingTop (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSPaddingTop (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSPaddingRight : parse a CSS PaddingRight
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSPaddingRight (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSPaddingRight (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}


/*----------------------------------------------------------------------
   ParseCSSPaddingBottom : parse a CSS PaddingBottom
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSPaddingBottom (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSPaddingBottom (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSPaddingLeft : parse a CSS PaddingLeft
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSPaddingLeft (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSPaddingLeft (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSPadding : parse a CSS padding attribute string. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSPadding (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSPadding (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSForeground : parse a CSS foreground attribute 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSForeground (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSForeground (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
   PresentationValue   best;

   cssRule = ParseCSSColor (cssRule, &best);
   if (best.typed_data.unit != STYLE_UNIT_INVALID)
     /* install the new presentation */
     TtaSetStylePresentation (PRForeground, element, tsch, context, best);
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBackgroundColor : parse a CSS background color attribute 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSBackgroundColor (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSBackgroundColor (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue     best;
  unsigned int          savedtype = 0;
  ThotBool              moved;

  /* move the BODY rule to the HTML element */
  moved = (context->type == HTML_EL_BODY && isHTML);
  if (moved)
    {
      if (element)
	element = TtaGetMainRoot (context->doc);
      else
	{
	  savedtype = context->type;
	  context->type = HTML_EL_HTML;
	}
    }

  best.typed_data.unit = STYLE_UNIT_INVALID;
  best.typed_data.real = FALSE;
  if (!ustrncasecmp (cssRule, TEXT("transparent"), ustrlen (TEXT("transparent"))))
    {
      best.typed_data.value = STYLE_PATTERN_NONE;
      best.typed_data.unit = STYLE_UNIT_REL;
      TtaSetStylePresentation (PRFillPattern, element, tsch, context, best);
    }
  else
    {
      cssRule = ParseCSSColor (cssRule, &best);
      if (best.typed_data.unit != STYLE_UNIT_INVALID)
	{
	  /* install the new presentation. */
	  TtaSetStylePresentation (PRBackground, element, tsch, context, best);
	  /* thot specificity : need to set fill pattern for background color */
	  best.typed_data.value = STYLE_PATTERN_BACKGROUND;
	  best.typed_data.unit = STYLE_UNIT_REL;
	  TtaSetStylePresentation (PRFillPattern, element, tsch, context, best);
	  best.typed_data.value = 1;
	  best.typed_data.unit = STYLE_UNIT_REL;
	  TtaSetStylePresentation (PRShowBox, element, tsch, context, best);
	}
    }
  cssRule = SkipWord (cssRule);

  /* restore the refered element */
  if (moved && !element)
    context->type = savedtype;
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBackgroundImageCallback : Callback called asynchronously by
   FetchImage when a background image has been fetched.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void ParseCSSBackgroundImageCallback (Document doc, Element element, STRING file,
                                      void *extra)
#else
void ParseCSSBackgroundImageCallback (doc, element, file, extra)
Document doc;
Element  element;
STRING   file;
void    *extra;
#endif
{
  DisplayMode         dispMode;
  BackgroundImageCallbackPtr callblock = (BackgroundImageCallbackPtr) extra;
  Element             el;
  PSchema             tsch;
  PresentationContext context;
  PresentationValue   image;
  PresentationValue   repeat;
  PresentationValue   value;

  if (callblock == NULL)
    return;

  /* avoid too many redisplay */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);

  el = callblock->el;
  tsch = callblock->tsch;
  context = &callblock->context.specific;

  /* Ok the image was fetched, finish the background-image handling */
  image.pointer = file;
  TtaSetStylePresentation (PRBackgroundPicture, el, tsch, context, image);

  /* If there is no default repeat mode, enforce a V-Repeat */
  if (TtaGetStylePresentation (PRPictureMode, el, tsch, context, &repeat) < 0)
    {
      repeat.typed_data.value = STYLE_REPEAT;
      repeat.typed_data.unit = STYLE_UNIT_REL;
      repeat.typed_data.real = FALSE;
      TtaSetStylePresentation (PRPictureMode, el, tsch, context, repeat);
    }

  /* If there is no default repeat mode, enforce a V-Repeat */
  value.typed_data.value = 1;
  value.typed_data.unit = STYLE_UNIT_REL;
  value.typed_data.real = FALSE;
  TtaSetStylePresentation (PRShowBox, el, tsch, context, value);

  TtaFreeMemory (callblock);
  /* restore the display mode */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);
}


/*----------------------------------------------------------------------
   GetCSSBackgroundURL searches a CSS BackgroundImage url within
   the styleString.
   Returns NULL or a new allocated url string.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING              GetCSSBackgroundURL (STRING styleString)
#else
STRING              GetCSSBackgroundURL (styleString)
STRING              styleString;
#endif
{
  STRING              b, e, ptr;
  int                 len;

  ptr = NULL;
  b = ustrstr (styleString, TEXT("url"));
  if (b != NULL)
    {
      b += 3;
      b = SkipBlanksAndComments (b);
      if (*b == '(')
	{
	  b++;
	  b = SkipBlanksAndComments (b);
	  /*** Caution: Strings can either be written with double quotes or
	       with single quotes. Only double quotes are handled here.
	       Escaped quotes are not handled. See function SkipQuotedString */
	  if (*b == '"')
	    {
	      b++;
	      /* search the url end */
	      e = b;
	      while (*e != EOS && *e != '"')
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
	      ptr = (STRING) TtaGetMemory (len+1);
	      ustrncpy (ptr, b, len);
	      ptr[len] = EOS;
	    }
	}
    }
  return (ptr);
}


/*----------------------------------------------------------------------
   ParseCSSBackgroundImage : parse a CSS BackgroundImage
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSBackgroundImage (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSBackgroundImage (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  Element               el;
  GenericContext        gblock;
  PresentationContextBlock  *sblock;
  BackgroundImageCallbackPtr callblock;
  PresentationValue     image, value;
  STRING                url;
  STRING                bg_image;
  CHAR_T                saved;
  STRING                base;
  CHAR_T                tempname[MAX_LENGTH];
  CHAR_T                imgname[MAX_LENGTH];
  unsigned int          savedtype = 0;
  ThotBool              moved;

  /* default element for FetchImage */
  el = TtaGetMainRoot (context->doc);
  /* move the BODY rule to the HTML element */
  moved = (context->type == HTML_EL_BODY && isHTML);
  if (moved)
    {
      if (element)
	element = el;
      else
	{
	  savedtype = context->type;
	  context->type = HTML_EL_HTML;
	}
    }
  else if (element)
    el = element;

  url = NULL;
  cssRule = SkipBlanksAndComments (cssRule);
  if (!ustrncasecmp (cssRule, TEXT("url"), 3))
    {  
      cssRule += 3;
      cssRule = SkipBlanksAndComments (cssRule);
      if (*cssRule == '(')
	{
	  cssRule++;
	  cssRule = SkipBlanksAndComments (cssRule);
	  /*** Caution: Strings can either be written with double quotes or
	    with single quotes. Only double quotes are handled here.
	    Escaped quotes are not handled. See function SkipQuotedString */
	  if (*cssRule == '"')
	    {
	      cssRule++;
	      base = cssRule;
	      while (*cssRule != EOS && *cssRule != '"')
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
	  if (saved == '"')
	    /* we need to skip two characters */
	    cssRule++;	    
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
	      value.typed_data.unit = STYLE_UNIT_REL;
	      value.typed_data.real = FALSE;
	      TtaSetStylePresentation (PRShowBox, element, tsch, context, value);
	    }
	}
      else if (url)
	{
	  bg_image = TtaGetEnvString ("ENABLE_BG_IMAGES");
	  if (bg_image == NULL || !ustrcasecmp (bg_image, TEXT("yes")))
	    {
	      callblock = (BackgroundImageCallbackPtr) TtaGetMemory(sizeof(BackgroundImageCallbackBlock));
	      if (callblock != NULL)
		{
		  callblock->el = element;
		  callblock->tsch = tsch;
		  if (element == NULL)
		    {
		      gblock = (GenericContext) context;
		      memcpy (&callblock->context.generic, gblock,
			      sizeof (GenericContextBlock));
		    }
		  else
		    {
		      sblock = context;
		      memcpy (&callblock->context.specific, sblock,
			      sizeof(PresentationContextBlock));
		    }

		  /* check if the image url is related to an external CSS */
		  if (css != NULL && css->category == CSS_EXTERNAL_STYLE)
		    {
		      NormalizeURL (url, 0, tempname, imgname, css->url);
		      /* fetch and display background image of element */
		      FetchImage (context->doc, el, tempname, AMAYA_LOAD_IMAGE,
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

  /* restore the refered element */
  if (moved && !element)
    context->type = savedtype;
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBackgroundRepeat : parse a CSS BackgroundRepeat
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSBackgroundRepeat (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSBackgroundRepeat (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue   repeat;
  unsigned int        savedtype = 0;
  ThotBool            moved;

  /* move the BODY rule to the HTML element */
  moved = (context->type == HTML_EL_BODY && isHTML);
  if (moved)
    {
      if (element)
	element = TtaGetMainRoot (context->doc);
      else
	{
	  savedtype = context->type;
	  context->type = HTML_EL_HTML;
	}
    }

  repeat.typed_data.value = STYLE_REALSIZE;
  repeat.typed_data.unit = STYLE_UNIT_REL;
  repeat.typed_data.real = FALSE;
  cssRule = SkipBlanksAndComments (cssRule);
  if (!ustrncasecmp (cssRule, TEXT("no-repeat"), 9))
    repeat.typed_data.value = STYLE_REALSIZE;
  else if (!ustrncasecmp (cssRule, TEXT("repeat-y"), 8))
    repeat.typed_data.value = STYLE_VREPEAT;
  else if (!ustrncasecmp (cssRule, TEXT("repeat-x"), 8))
    repeat.typed_data.value = STYLE_HREPEAT;
  else if (!ustrncasecmp (cssRule, TEXT("repeat"), 6))
    repeat.typed_data.value = STYLE_REPEAT;
  else
    return (cssRule);

   /* install the new presentation */
  TtaSetStylePresentation (PRPictureMode, element, tsch, context, repeat);
  cssRule = SkipWord (cssRule);

  /* restore the refered element */
  if (moved && !element)
    context->type = savedtype;
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBackgroundAttachment : parse a CSS BackgroundAttachment
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSBackgroundAttachment (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSBackgroundAttachment (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  unsigned int          savedtype = 0;
  ThotBool              moved;

  /* move the BODY rule to the HTML element */
  moved = (context->type == HTML_EL_BODY && isHTML);
  if (moved)
    {
      if (element)
	element = TtaGetMainRoot (context->doc);
      else
	{
	  savedtype = context->type;
	  context->type = HTML_EL_HTML;
	}
    }

   cssRule = SkipBlanksAndComments (cssRule);
   if (!ustrncasecmp (cssRule, TEXT("scroll"), 6))
     cssRule = SkipWord (cssRule);
   else if (!ustrncasecmp (cssRule, TEXT("fixed"), 5))
     cssRule = SkipWord (cssRule);

  /* restore the refered element */
  if (moved && !element)
    context->type = savedtype;
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBackgroundPosition : parse a CSS BackgroundPosition
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSBackgroundPosition (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSBackgroundPosition (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue     repeat;
  unsigned int          savedtype = 0;
  ThotBool              moved;
  ThotBool              ok;

  /* move the BODY rule to the HTML element */
  moved = (context->type == HTML_EL_BODY && isHTML);
  if (moved)
    {
      if (element)
	element = TtaGetMainRoot (context->doc);
      else
	{
	  savedtype = context->type;
	  context->type = HTML_EL_HTML;
	}
    }

   cssRule = SkipBlanksAndComments (cssRule);
   ok = TRUE;
   if (!ustrncasecmp (cssRule, TEXT("left"), 4))
     cssRule = SkipWord (cssRule);
   else if (!ustrncasecmp (cssRule, TEXT("right"), 5))
     cssRule = SkipWord (cssRule);
   else if (!ustrncasecmp (cssRule, TEXT("center"), 6))
     cssRule = SkipWord (cssRule);
   else if (!ustrncasecmp (cssRule, TEXT("top"), 3))
     cssRule = SkipWord (cssRule);
   else if (!ustrncasecmp (cssRule, TEXT("bottom"), 6))
     cssRule = SkipWord (cssRule);
   else if (isdigit (*cssRule))
     cssRule = SkipWord (cssRule);
   else
     ok = FALSE;

   if (ok)
     {
       /* force realsize for the background image */
       repeat.typed_data.value = STYLE_REALSIZE;
       repeat.typed_data.unit = STYLE_UNIT_REL;
       repeat.typed_data.real = FALSE;
       TtaSetStylePresentation (PRPictureMode, element, tsch, context, repeat);
     }

  /* restore the refered element */
  if (moved && !element)
    context->type = savedtype;
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBackground : parse a CSS background attribute 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSBackground (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static STRING       ParseCSSBackground (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  STRING            ptr;

  cssRule = SkipBlanksAndComments (cssRule);
  while (*cssRule != ';' && *cssRule != EOS && *cssRule != ',')
    {
      /* perhaps a Backgroud Image */
      if (!ustrncasecmp (cssRule, TEXT("url"), 3))
	cssRule = ParseCSSBackgroundImage (element, tsch, context, cssRule, css, isHTML);
      /* perhaps a Background Attachment */
      else if (!ustrncasecmp (cssRule, TEXT("scroll"), 6) ||
	       !ustrncasecmp (cssRule, TEXT("fixed"), 5))
	cssRule = ParseCSSBackgroundAttachment (element, tsch, context, cssRule, css, isHTML);
      /* perhaps a Background Repeat */
      else if (!ustrncasecmp (cssRule, TEXT("no-repeat"), 9) ||
	       !ustrncasecmp (cssRule, TEXT("repeat-y"), 8) ||
	       !ustrncasecmp (cssRule, TEXT("repeat-x"), 8) ||
	       !ustrncasecmp (cssRule, TEXT("repeat"), 6))
	cssRule = ParseCSSBackgroundRepeat (element, tsch, context, cssRule, css, isHTML);
      /* perhaps a Background Position */
      else if (!ustrncasecmp (cssRule, TEXT("left"), 4) ||
	       !ustrncasecmp (cssRule, TEXT("right"), 5) ||
	       !ustrncasecmp (cssRule, TEXT("center"), 6) ||
	       !ustrncasecmp (cssRule, TEXT("top"), 3) ||
	       !ustrncasecmp (cssRule, TEXT("bottom"), 6) ||
	       isdigit (*cssRule))
	cssRule = ParseCSSBackgroundPosition (element, tsch, context, cssRule, css, isHTML);
      /* perhaps a Background Color */
      else
	{
	  /* check if the rule has been found */
	  ptr = cssRule;
	  cssRule = ParseCSSBackgroundColor (element, tsch, context, cssRule, css, isHTML);
	  if (ptr== cssRule)
	    /* rule not found */
	    cssRule = SkipProperty (cssRule);
	}
      cssRule = SkipBlanksAndComments (cssRule);
    }
   return (cssRule);
}



/************************************************************************
 *									*  
 *	FUNCTIONS STYLE DECLARATIONS             			*
 *									*  
 ************************************************************************/
/*
 * NOTE : Long attribute name MUST be placed before shortened ones !
 *        e.g. "FONT-SIZE" must be placed before "FONT"
 */
static CSSProperty CSSProperties[] =
{
   {TEXT ("font-family"), ParseCSSFontFamily},
   {TEXT ("font-style"), ParseCSSFontStyle},
   {TEXT ("font-variant"), ParseCSSFontVariant},
   {TEXT ("font-weight"), ParseCSSFontWeight},
   {TEXT ("font-size"), ParseCSSFontSize},
   {TEXT ("font"), ParseCSSFont},

   {TEXT ("color"), ParseCSSForeground},
   {TEXT ("background-color"), ParseCSSBackgroundColor},
   {TEXT ("background-image"), ParseCSSBackgroundImage},
   {TEXT ("background-repeat"), ParseCSSBackgroundRepeat},
   {TEXT ("background-attachment"), ParseCSSBackgroundAttachment},
   {TEXT ("background-position"), ParseCSSBackgroundPosition},
   {TEXT ("background"), ParseCSSBackground},

   {TEXT ("word-spacing"), ParseCSSWordSpacing},
   {TEXT ("letter-spacing"), ParseCSSLetterSpacing},
   {TEXT ("text-decoration"), ParseCSSTextDecoration},
   {TEXT ("vertical-align"), ParseCSSVerticalAlign},
   {TEXT ("text-transform"), ParseCSSTextTransform},
   {TEXT ("text-align"), ParseCSSTextAlign},
   {TEXT ("text-indent"), ParseCSSTextIndent},
   {TEXT ("line-height"), ParseCSSLineSpacing},

   {TEXT ("margin-top"), ParseCSSMarginTop},
   {TEXT ("margin-right"), ParseCSSMarginRight},
   {TEXT ("margin-bottom"), ParseCSSMarginBottom},
   {TEXT ("margin-left"), ParseCSSMarginLeft},
   {TEXT ("margin"), ParseCSSMargin},

   {TEXT ("padding-top"), ParseCSSPaddingTop},
   {TEXT ("padding-right"), ParseCSSPaddingRight},
   {TEXT ("padding-bottom"), ParseCSSPaddingBottom},
   {TEXT ("padding-left"), ParseCSSPaddingLeft},
   {TEXT ("padding"), ParseCSSPadding},

   {TEXT ("border-top-width"), ParseCSSBorderTopWidth},
   {TEXT ("border-right-width"), ParseCSSBorderRightWidth},
   {TEXT ("border-bottom-width"), ParseCSSBorderBottomWidth},
   {TEXT ("border-left-width"), ParseCSSBorderLeftWidth},
   {TEXT ("border-width"), ParseCSSBorderWidth},
   {TEXT ("border-color"), ParseCSSBorderColor},
   {TEXT ("border-style"), ParseCSSBorderStyle},
   {TEXT ("border-top"), ParseCSSBorderTop},
   {TEXT ("border-right"), ParseCSSBorderRight},
   {TEXT ("border-bottom"), ParseCSSBorderBottom},
   {TEXT ("border-left"), ParseCSSBorderLeft},
   {TEXT ("border"), ParseCSSBorder},

   {TEXT ("width"), ParseCSSWidth},
   {TEXT ("height"), ParseCSSHeight},
   {TEXT ("float"), ParseCSSFloat},
   {TEXT ("clear"), ParseCSSClear},

   {TEXT ("display"), ParseCSSDisplay},
   {TEXT ("white-space"), ParseCSSWhiteSpace},

   {TEXT ("list-style-type"), ParseCSSListStyleType},
   {TEXT ("list-style-image"), ParseCSSListStyleImage},
   {TEXT ("list-style-position"), ParseCSSListStylePosition},
   {TEXT ("list-style"), ParseCSSListStyle}
};
#define NB_CSSSTYLEATTRIBUTE (sizeof(CSSProperties) / sizeof(CSSProperty))

/*----------------------------------------------------------------------
   ParseCSSRule : parse a CSS Style string                        
   we expect the input string describing the style to be of the  
   form : PRORPERTY : DESCRIPTION [ ; PROPERTY : DESCRIPTION ] * 
   but tolerate incorrect or incomplete input                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ParseCSSRule (Element element, PSchema tsch, PresentationContext context, STRING cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static void         ParseCSSRule (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  DisplayMode         dispMode;
  STRING              p = NULL;
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
      
      found = FALSE;
      /* look for the type of property */
      for (i = 0; i < NB_CSSSTYLEATTRIBUTE && !found; i++)
	{
	  lg = ustrlen (CSSProperties[i].name);
	  if (!ustrncasecmp (cssRule, CSSProperties[i].name, lg))
	    {
	      cssRule += lg;
	      found = TRUE;
	      i--;
	    }
	}

      if (i == NB_CSSSTYLEATTRIBUTE)
	cssRule = SkipProperty (cssRule);
      else
	{
	  /* update index and skip the ":" indicator if present */
	  cssRule = SkipBlanksAndComments (cssRule);
	  if (*cssRule == ':')
	    {
	      cssRule++;
	      cssRule = SkipBlanksAndComments (cssRule);
	    }
	  /* try to parse the attribute associated to this attribute */
	  if (CSSProperties[i].parsing_function != NULL)
	    {
	      p = CSSProperties[i].parsing_function (element, tsch, context, cssRule, css, isHTML);
	      /* update index and skip the ";" separator if present */
	      cssRule = p;
	    }
	}
      /* next property */
      cssRule = SkipBlanksAndComments (cssRule);
      if (*cssRule == ',' || *cssRule == ';')
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
 PToCss :  translate a PresentationSetting to the
     equivalent CSS string, and add it to the buffer given as the
      argument. It is used when extracting the CSS string from actual
      presentation.
 
  All the possible values returned by the presentation drivers are
  described in thotlib/include/presentation.h
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void                 PToCss (PresentationSetting settings, STRING buffer, int len)
#else
void                 PToCss (settings, buffer, len)
PresentationSetting  settings;
STRING               param;
int                  len
#endif
{
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
      fval /= 1000;
    }

  switch (settings->type)
    {
    case PRVisibility:
      break;
    case PRFont:
      switch (settings->value.typed_data.value)
	{
	case STYLE_FONT_HELVETICA:
	  ustrcpy (buffer, TEXT("font-family: helvetica"));
	  break;
	case STYLE_FONT_TIMES:
	  ustrcpy (buffer, TEXT("font-family: times"));
	  break;
	case STYLE_FONT_COURIER:
	  ustrcpy (buffer, TEXT("font-family: courier"));
	  break;
	}
      break;
    case PRStyle:
      switch (settings->value.typed_data.value)
	{
	case STYLE_FONT_ROMAN:
	  ustrcpy (buffer, TEXT("font-style: normal"));
	  break;
	case STYLE_FONT_ITALICS:
	  ustrcpy (buffer, TEXT("font-style: italic"));
	  break;
	case STYLE_FONT_OBLIQUE:
	  ustrcpy (buffer, TEXT("font-style: oblique"));
	  break;
	}
      break;
    case PRWeight:
      switch (settings->value.typed_data.value)
	{
	case STYLE_WEIGHT_BOLD:
	  ustrcpy (buffer, TEXT("font-weight: bold"));
	  break;
	case STYLE_WEIGHT_NORMAL:
	  ustrcpy (buffer, TEXT("font-weight: normal"));
	  break;
	}
      break;
    case PRSize:
      if (unit == STYLE_UNIT_REL)
	{
	  if (real)
	    {
	      usprintf (buffer, TEXT("font-size: %g"), fval);
	      add_unit = 1;
	    }
	  else
	    switch (settings->value.typed_data.value)
	      {
	      case 1:
		ustrcpy (buffer, TEXT("font-size: xx-small"));
		break;
	      case 2:
		ustrcpy (buffer, TEXT("font-size: x-small"));
		break;
	      case 3:
		ustrcpy (buffer, TEXT("font-size: small"));
		break;
	      case 4:
		ustrcpy (buffer, TEXT("font-size: medium"));
		break;
	      case 5:
		ustrcpy (buffer, TEXT("font-size: large"));
		break;
	      case 6:
		ustrcpy (buffer, TEXT("font-size: x-large"));
		break;
	      case 7:
	      case 8:
	      case 9:
	      case 10:
	      case 11:
	      case 12:
		ustrcpy (buffer, TEXT("font-size: xx-large"));
		break;
	      }
	}
      else
	{
	  if (real)
	    usprintf (buffer, TEXT("font-size: %g"), fval);
	  else
	    usprintf (buffer, TEXT("font-size: %d"), settings->value.typed_data.value);
	  add_unit = 1;
	}
      break;
    case PRUnderline:
      switch (settings->value.typed_data.value)
	{
	case STYLE_UNDERLINE:
	  ustrcpy (buffer, TEXT("text-decoration: underline"));
	  break;
	case STYLE_OVERLINE:
	  ustrcpy (buffer, TEXT("text-decoration: overline"));
	  break;
	case STYLE_CROSSOUT:
	  ustrcpy (buffer, TEXT("text-decoration: line-through"));
	  break;
	}
      break;
    case PRIndent:
      if (real)
	usprintf (buffer, TEXT("text-indent: %g"), fval);
      else
	usprintf (buffer, TEXT("text-indent: %d"), settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRLineSpacing:
      if (real)
	usprintf (buffer, TEXT("line-height: %g"), fval);
      else
	usprintf (buffer, TEXT("line-height: %d"), settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRJustify:
      if (settings->value.typed_data.value == STYLE_JUSTIFIED)
	usprintf (buffer, TEXT("text-align: justify"));
      break;
    case PRAdjust:
      switch (settings->value.typed_data.value)
	{
	case STYLE_ADJUSTLEFT:
	  ustrcpy (buffer, TEXT("text-align: left"));
	  break;
	case STYLE_ADJUSTRIGHT:
	  ustrcpy (buffer, TEXT("text-align: right"));
	  break;
	case STYLE_ADJUSTCENTERED:
	  ustrcpy (buffer, TEXT("text-align: center"));
	  break;
	case STYLE_ADJUSTLEFTWITHDOTS:
	  ustrcpy (buffer, TEXT("text-align: left"));
	  break;
	}
      break;
    case PRHyphenate:
      break;
    case PRFillPattern:
      break;
    case PRBackground:
      TtaGiveThotRGB (settings->value.typed_data.value, &red, &green, &blue);
      usprintf (buffer, TEXT("background-color: #%02X%02X%02X"), red, green, blue);
      break;
    case PRForeground:
      TtaGiveThotRGB (settings->value.typed_data.value, &red, &green, &blue);
      usprintf (buffer, TEXT("color: #%02X%02X%02X"), red, green, blue);
      break;
    case PRTMargin:
      if (real)
	usprintf (buffer, TEXT("marging-top: %g"), fval);
      else
	usprintf (buffer, TEXT("marging-top: %d"), settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRLMargin:
      if (real)
	usprintf (buffer, TEXT("margin-left: %g"), fval);
      else
	usprintf (buffer, TEXT("margin-left: %d"), settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRHeight:
      if (real)
	usprintf (buffer, TEXT("height: %g"), fval);
      else
	usprintf (buffer, TEXT("height: %d"), settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRWidth:
      if (real)
	usprintf (buffer, TEXT("width: %g"), fval);
      else
	usprintf (buffer, TEXT("width: %d"), settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRLine:
      if (settings->value.typed_data.value == STYLE_INLINE)
	ustrcpy (buffer, TEXT("display: inline"));
      else if (settings->value.typed_data.value == STYLE_NOTINLINE)
	ustrcpy (buffer, TEXT("display: block"));
      break;
    case PRBackgroundPicture:
      if (settings->value.pointer != NULL)
	usprintf (buffer, TEXT("background-image: url(%s)"), (STRING)(settings->value.pointer));
      else
	usprintf (buffer, TEXT("background-image: none"));
      break;
    case PRPictureMode:
      switch (settings->value.typed_data.value)
	{
	case STYLE_REALSIZE:
	  usprintf (buffer, TEXT("background-repeat: no-repeat"));
	  break;
	case STYLE_REPEAT:
	  usprintf (buffer, TEXT("background-repeat: repeat"));
	  break;
	case STYLE_VREPEAT:
	  usprintf (buffer, TEXT("background-repeat: repeat-y"));
	  break;
	case STYLE_HREPEAT:
	  usprintf (buffer, TEXT("background-repeat: repeat-x"));
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
	      ustrcat (buffer, CSSUnitNames[i].sign);
	      break;
	    }
	}
    }
}

/*----------------------------------------------------------------------
   ParseHTMLSpecificStyle : parse and apply a CSS Style string.
   This function must be called when a specific style is applied to an
   element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ParseHTMLSpecificStyle (Element el, STRING cssRule, Document doc, ThotBool destroy)
#else
void                ParseHTMLSpecificStyle (el, cssRule, doc, destroy)
Element             elem;
STRING              cssRule;
Document            doc;
ThotBool            destroy;
#endif
{
   PresentationContext context;
   ElementType         elType;
   ThotBool            isHTML;

   /*  A rule applying to BODY is really meant to address HTML */
   elType = TtaGetElementType (el);
   isHTML = (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")) == 0);
   /* create the context of the Specific presentation driver */
   context = TtaGetSpecificStyleContext (doc);
   if (context == NULL)
     return;
   context->type = elType.ElTypeNum;
   context->destroy = destroy;
   /* Call the parser */
   ParseCSSRule (el, NULL, (PresentationContext) context, cssRule, NULL, isHTML);
   /* free the context */
   TtaFreeMemory(context);
}

/*----------------------------------------------------------------------
   ParseGenericSelector : Create a generic context for a given 
   selector string. If the selector is made of multiple comma- 
   separated selector items, it parses them one at a time and  
   return the end of the selector string to be handled or NULL 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING  ParseGenericSelector (STRING selector, STRING cssRule,
			   GenericContext ctxt, Document doc, CSSInfoPtr css)
#else
static STRING  ParseGenericSelector (selector, cssRule, ctxt, doc, css)
STRING         selector;
STRING         cssRule;
GenericContext  ctxt;
Document        doc;
CSSInfoPtr      css;
#endif
{
  ElementType         elType;
  PSchema             tsch;
  AttributeType       attrType;
  CHAR_T              sel[MAX_ANCESTORS * 50];
  STRING              deb, cur;
  STRING              structName;
  STRING              names[MAX_ANCESTORS];
  STRING              ids[MAX_ANCESTORS];
  STRING              classes[MAX_ANCESTORS];
  STRING              pseudoclasses[MAX_ANCESTORS];
  STRING              attrs[MAX_ANCESTORS];
  STRING              attrvals[MAX_ANCESTORS];
  int                 i, j, k, max, maxAttr;
  ThotBool            isHTML;

  sel[0] = EOS;
  for (i = 0; i < MAX_ANCESTORS; i++)
    {
      names[i] = NULL;
      ids[i] = NULL;
      classes[i] = NULL;
      pseudoclasses[i] = NULL;
      attrs[i] = NULL;
      attrvals[i] = NULL;

      ctxt->name[i] = 0;
      ctxt->names_nb[i] = 0;
      ctxt->attrType[i] = 0;
      ctxt->attrText[i] = NULL;
    }
  ctxt->box = 0;
  ctxt->type = 0;
  
  selector = SkipBlanksAndComments (selector);
  cur = &sel[0];
  max = 0; /* number of loops */
  while (1)
    {
      deb = cur;
      /* copy an item of the selector into sel[] */
      /* put one word in the sel buffer */
      while (*selector != EOS && *selector != ',' &&
	     *selector != '.' && *selector != ':' &&
	     *selector != '#' && !TtaIsBlank (selector))
	*cur++ = *selector++;
      *cur++ = EOS; /* close the first string  in sel[] */
      if (deb[0] != EOS)
	names[0] = deb;
      else
	names[0] = NULL;
      classes[0] = NULL;
      pseudoclasses[0] = NULL;
      ids[0] = NULL;
      attrs[0] = NULL;
      attrvals[0] = NULL;

      /* now names[0] points to the beginning of the parsed item
	 and cur to the next chain to be parsed */
      if (*selector == ':' || *selector == '.' || *selector == '#')
	/* keep the element name which precedes the id or
	 pseudo class or the class */
	deb = cur;

      if (*selector == '.')
	{
	  /* copy into sel[] the class */
	  classes[0] = cur;
	  selector++;
	  while (*selector != EOS && *selector != ',' &&
		 *selector != '.' && *selector != ':' &&
		 !TtaIsBlank (selector))
	    *cur++ = *selector++;
	  *cur++ = EOS;
	}
      else if (*selector == ':')
	{
	  /* copy into sel[] the pseudoclass */
	  pseudoclasses[0]= cur;
	  selector++;
	  while (*selector != EOS && *selector != ',' &&
		 *selector != '.' && *selector != ':' &&
		 !TtaIsBlank (selector))
	    *cur++ = *selector++;
	  *cur++ = EOS;
	}
      else if (*selector == '#')
	{
	  /* copy into sel[] the attribute */
	  ids[0] = cur;
	  selector++;
	  while (*selector != EOS && *selector != ',' &&
		 *selector != '.' && *selector != ':' &&
		 !TtaIsBlank (selector))
	    *cur++ = *selector++;
	  *cur++ = EOS;
	}
      else if (*selector == '[')
	{
	  /* copy into sel[] the attribute */
	  attrs[0] = cur;
	  selector++;
	  while (*selector != EOS && *selector != ']' && *selector != '=')
	    *cur++ = *selector++;
	  if (*cur == '=')
	    {
	      /* there is a value "xxxx" */
	      *cur++ = EOS;
	      while (*selector != EOS && *selector != ']' && *selector != '"')
		selector++;
	      if (*selector != EOS)
		{
		  /* we are now parsing the attribute value */
		  attrvals[0] = cur;
		  selector++;
		  while (*selector != EOS && *selector != '"')
		    *cur++ = *selector++;
		  if (*selector != EOS)
		    selector++;
		}
	    }
	  *cur++ = EOS;
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
	      attrs[i] = attrs[i - 1];
	      attrvals[i] = attrvals[i - 1];
	      pseudoclasses[i] = pseudoclasses[i - 1];
	    }
	}
    }

  /* Now set up the context block */
  i = 0;
  k = 0;
  j = 0;
  maxAttr = 0;
  while (i <= max)
    {
      if (names[i])
	{
	  /* get the new element type of this name */
	  GIType (names[i], &elType, doc);
	  if (i == 0)
	    {
	      /* Store the element type */
	      ctxt->type = elType.ElTypeNum;
	      ctxt->name[0] = elType.ElTypeNum;
	      ctxt->names_nb[0] = 0;
	      ctxt->schema = elType.ElSSchema;
	      k++;
	    }
	  else if (elType.ElTypeNum != 0)
	    {
	      /* look at the current context to see if the type is already
		 stored */
	      j = 0;
	      while (j < k && ctxt->name[j] != elType.ElTypeNum)
		j++;
	      if (j == k)
		{
		  /* add a new entry */
		  k++;
		  ctxt->name[j] = elType.ElTypeNum;
		  if (j != 0)
		  ctxt->names_nb[j] = 1;
		}
	      else
		/* increment the number of ancestor levels */
		ctxt->names_nb[j]++;
	    }
	  else
	    {
	      /* add a new entry */
	      j = k;
	      k++;
	    }
	}
      else
	{
	  /* add a new entry */
	  j = k;
	  k++;
	}

      if (classes[i] || pseudoclasses[i] || ids[i] || attrs[i])
	if (maxAttr > 0)
	  /* Thot is not able to manage this kind of selector -> abort */
	  return (selector);
	else
	  maxAttr++;

      /* store attributes information */
      if (classes[i])
	{
	  ctxt->attrText[j] = classes[i];
	  ctxt->attrType[j] = HTML_ATTR_Class;
	}
      else if (pseudoclasses[i])
	{
	  ctxt->attrText[j] = pseudoclasses[i];
	  ctxt->attrType[j] = HTML_ATTR_PseudoClass;
	}
      else if (ids[i])
	{
	  ctxt->attrText[j] = ids[i];
	  ctxt->attrType[j] = HTML_ATTR_ID;
	}
      else if (attrs[i])
	{
	  MapHTMLAttribute (attrs[i], &attrType, names[i], doc);
	  ctxt->attrText[j] = attrvals[i];
	  ctxt->attrType[j] = attrType.AttrTypeNum;
	}
      i++;
    }

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
  if (ctxt->schema == NULL)
    ctxt->schema = TtaGetDocumentSSchema (doc);
  isHTML = (ustrcmp(TtaGetSSchemaName (ctxt->schema), TEXT("HTML")) == 0);
  tsch = GetPExtension (doc, ctxt->schema, css);
  structName = TtaGetSSchemaName (ctxt->schema);
  if (cssRule)
    ParseCSSRule (NULL, tsch, (PresentationContext) ctxt, cssRule, css, isHTML);
  return (selector);
}

/*----------------------------------------------------------------------
   ParseStyleDeclaration : parse one HTML style declaration    
   stored in the header of a HTML document                       
   We expect the style string to be of the form :                   
   [                                                                
   e.g: pinky, awful { color: pink, font-family: helvetica }        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ParseStyleDeclaration (Element el, STRING cssRule, Document doc, CSSInfoPtr css, ThotBool destroy)
#else
static void         ParseStyleDeclaration (el, cssRule, doc, css, destroy)
Element             el;
STRING              cssRule;
Document            doc;
CSSInfoPtr          css;
ThotBool            destroy;
#endif
{
  GenericContext      ctxt;
  STRING              decl_end;
  STRING              sel_end;
  STRING              selector;
  CHAR_T                saved1;
  CHAR_T                saved2;

  /* separate the selectors string */
  cssRule = SkipBlanksAndComments (cssRule);
  decl_end = cssRule;
  while ((*decl_end != EOS) && (*decl_end != '{'))
    decl_end++;
  if (*decl_end == EOS)
    return;
  /* verify and clean the selector string */
  sel_end = decl_end - 1;
  while (*sel_end == SPACE || *sel_end == BSPACE ||
	 *sel_end == EOL || *sel_end == __CR__)
    sel_end--;
  sel_end++;
  saved1 = *sel_end;
  *sel_end = EOS;
  selector = cssRule;

  /* now, deal with the content ... */
  decl_end++;
  cssRule = decl_end;
  while (*decl_end != EOS && *decl_end != '}')
    decl_end++;
  if (*decl_end == EOS)
    {
      fprintf (stderr, "Invalid STYLE declaration : %s\n", cssRule);
      return;
    }
  saved2 = *decl_end;
  *decl_end = EOS;

  /*
   * parse the style attribute string and install the corresponding
   * presentation attributes on the new element
   */
  ctxt = TtaGetGenericStyleContext (doc);
  if (ctxt == NULL)
    return;
  ctxt->destroy = destroy;

  while ((selector != NULL) && (*selector != EOS))
    selector = ParseGenericSelector (selector, cssRule, ctxt, doc, css);
  TtaFreeMemory (ctxt);

  /* restore the string to its original form ! */
  *sel_end = saved1;
  *decl_end = saved2;
}

/************************************************************************
 *									*  
 *	EVALUATION FUNCTIONS / CASCADING AND OVERLOADING		*
 *									*  
 ************************************************************************/

/*----------------------------------------------------------------------
   IsImplicitClassName : return wether the Class name is an        
   implicit one, eg "H1" or "H2 EM" meaning it's a GI name       
   or an HTML context name.                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 IsImplicitClassName (STRING class, Document doc)
#else
int                 IsImplicitClassName (class, doc)
STRING              class;
Document            doc;
#endif
{
   CHAR_T           name[200];
   STRING           cur = name, first; 
   CHAR_T           save;
   SSchema	    schema;

   /* make a local copy */
   ustrncpy (name, class, 199);
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
 *  Functions Needed for support of HTML 3.2 : translate to CSS equiv   *
 *									*  
 ************************************************************************/

/*----------------------------------------------------------------------
   HTMLSetBackgroundColor :
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLSetBackgroundColor (Document doc, Element el, STRING color)
#else
void                HTMLSetBackgroundColor (doc, el, color)
Document            doc;
Element             el;
STRING              color;
#endif
{
   CHAR_T             css_command[100];

   usprintf (css_command, TEXT("background-color: %s"), color);
   ParseHTMLSpecificStyle (el, css_command, doc, FALSE);
}

/*----------------------------------------------------------------------
   HTMLSetBackgroundImage :
   repeat = repeat value
   image = url of background image
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLSetBackgroundImage (Document doc, Element el, int repeat, STRING image)
#else
void                HTMLSetBackgroundImage (doc, el, repeat, image)
Document            doc;
Element             el;
int                 repeat;
STRING              image;
#endif
{
   CHAR_T                css_command[400];

   /******* check buffer overflow ********/
   usprintf (css_command, TEXT("background-image: url(%s); background-repeat: "), image);
   if (repeat == STYLE_REPEAT)
     ustrcat (css_command, TEXT("repeat"));
   else if (repeat == STYLE_HREPEAT)
     ustrcat (css_command, TEXT("repeat-x"));
   else if (repeat == STYLE_VREPEAT)
     ustrcat (css_command, TEXT("repeat-y"));
   else
     ustrcat (css_command, TEXT("no-repeat"));
   ParseHTMLSpecificStyle (el, css_command, doc, FALSE);
}

/*----------------------------------------------------------------------
   HTMLSetForegroundColor :                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLSetForegroundColor (Document doc, Element el, STRING color)
#else
void                HTMLSetForegroundColor (doc, el, color)
Document            doc;
Element             el;
STRING              color;
#endif
{
   CHAR_T             css_command[100];

   usprintf (css_command, TEXT("color: %s"), color);
   ParseHTMLSpecificStyle (el, css_command, doc, FALSE);
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
   CHAR_T             css_command[100];

   usprintf (css_command, TEXT("background: xx"));
   ParseHTMLSpecificStyle (el, css_command, doc, TRUE);
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
   CHAR_T             css_command[1000];

   usprintf (css_command, TEXT("background-image: url(xx); background-repeat: repeat"));
   ParseHTMLSpecificStyle (el, css_command, doc, TRUE);
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
   CHAR_T             css_command[100];

   usprintf (css_command, TEXT("color: xx"));
   ParseHTMLSpecificStyle (el, css_command, doc, TRUE);
}

/*----------------------------------------------------------------------
   HTMLSetAlinkColor :                                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLSetAlinkColor (Document doc, STRING color)
#else
void                HTMLSetAlinkColor (doc, color)
Document            doc;
STRING              color;
#endif
{
   CHAR_T                css_command[100];

   usprintf (css_command, TEXT("A:link { color : %s }"), color);
   ApplyCSSRules (NULL, css_command, doc, FALSE);
}

/*----------------------------------------------------------------------
   HTMLSetAactiveColor :                                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLSetAactiveColor (Document doc, STRING color)
#else
void                HTMLSetAactiveColor (doc, color)
Document            doc;
STRING              color;
#endif
{
   CHAR_T                css_command[100];

   usprintf (css_command, TEXT("A:active { color : %s }"), color);
   ApplyCSSRules (NULL, css_command, doc, FALSE);
}

/*----------------------------------------------------------------------
   HTMLSetAvisitedColor :                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLSetAvisitedColor (Document doc, STRING color)
#else
void                HTMLSetAvisitedColor (doc, color)
Document            doc;
STRING              color;
#endif
{
   CHAR_T                css_command[100];

   usprintf (css_command, TEXT("A:visited { color : %s }"), color);
   ApplyCSSRules (NULL, css_command, doc, FALSE);
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
   CHAR_T                css_command[100];

   usprintf (css_command, TEXT("A:link { color : red }"));
   ApplyCSSRules (NULL, css_command, doc, TRUE);
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
   CHAR_T                css_command[100];

   usprintf (css_command, TEXT("A:active { color : red }"));
   ApplyCSSRules (NULL, css_command, doc, TRUE);
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
   CHAR_T                css_command[100];

   usprintf (css_command, TEXT("A:visited { color : red }"));
   ApplyCSSRules (NULL, css_command, doc, TRUE);
}

/*----------------------------------------------------------------------
  ApplyCSSRules: parse an CSS Style description stored in the
  header of a HTML document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ApplyCSSRules (Element el, STRING cssRule, Document doc, ThotBool destroy)
#else
void                ApplyCSSRules (el, cssRule, doc, destroy)
Element             el;
STRING              cssRule;
Document            doc;
ThotBool            destroy;
#endif
{
  CSSInfoPtr        css;

  css = SearchCSS (doc, NULL);
  if (css == NULL)
    /* create the document css */
    css = AddCSS (doc, doc, CSS_DOCUMENT_STYLE, NULL, NULL);
  ParseStyleDeclaration (el, cssRule, doc, css, destroy); 
}

/*----------------------------------------------------------------------
   ReadCSSRules :  is the front-end function called by the HTML parser
   when detecting a <STYLE TYPE="text/css"> indicating it's the
   beginning of a CSS fragment or when reading a file .css.
  
   The CSS parser has to handle <!-- ... --> constructs used to
   prevent prehistoric browser from displaying the CSS as a text
   content. It will stop on any sequence "<x" where x is different
   from ! and will return x as to the caller. Theorically x should
   be equal to / for the </STYLE> end of style.

   The parameter doc gives the document tree that contains CSS information.
   The parameter docRef gives the document to which CSS are to be applied.
   This function uses the current css context or creates it. It's able
   to work on the given buffer or call GetNextChar to read the parsed
   file.
   Parameter withUndo indicates whether the changes made in the document
   structure and content have to be registered in the Undo queue or not
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T                ReadCSSRules (Document docRef, CSSInfoPtr css, STRING buffer, ThotBool withUndo)
#else
CHAR_T                ReadCSSRules (docRef, css, buffer, withUndo)
Document            docRef;
CSSInfoPtr          css;
STRING              buffer;
ThotBool            withUndo;
#endif
{
  CHAR_T              c;
  STRING              cssRule, base;
  DisplayMode         dispMode;
  int                 index;
  int                 CSSindex;
  int                 CSScomment;
  int                 import;
  int                 openRule;
  ThotBool            HTMLcomment;
  ThotBool            toParse, eof;
  ThotBool            ignoreMedia;
  ThotBool            noRule;

  CSScomment = MAX_CSS_LENGTH;
  HTMLcomment = FALSE;
  CSSindex = 0;
  toParse = FALSE;
  noRule = FALSE;
  ignoreMedia = FALSE;
  import = MAX_CSS_LENGTH;
  eof = FALSE;
  openRule = 0;
  c = SPACE;
  index = 0;
  /* avoid too many redisplay */
  dispMode = TtaGetDisplayMode (docRef);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (docRef, DeferredDisplay);

  /* look for the CSS context */
  if (css == NULL)
    css = SearchCSS (docRef, NULL);
  if (css == NULL)
    css = AddCSS (docRef, docRef, CSS_DOCUMENT_STYLE, NULL, NULL);

  while (CSSindex < MAX_CSS_LENGTH && c != EOS && !eof)
    {
      c = buffer[index++];
      eof = (c == EOS);
      CSSbuffer[CSSindex] = c;
      if (CSScomment == MAX_CSS_LENGTH || c == '*' || c == '/' || c == '<')
	{
	  /* we're not within a comment or we're parsing * or / */
	  switch (c)
	    {
	    case '@':
	      /* perhaps an import primitive */
	      import = CSSindex;
	      break;
	    case ';':
	      if (import != MAX_CSS_LENGTH)
		{
		  if (ustrncasecmp (&CSSbuffer[import+1], TEXT("import"), 6))
		    /* it's not an import */
		    import = MAX_CSS_LENGTH;
		  /* save the text */
		  noRule = TRUE;
		}
	      break;
	    case '*':
	      if (CSScomment == MAX_CSS_LENGTH &&
		  CSSindex > 0 && CSSbuffer[CSSindex - 1] == '/')
		/* start a comment */
		CSScomment = CSSindex - 1;
	      break;
	    case '/':
	      if (CSSindex > 1 &&
		  CSScomment != MAX_CSS_LENGTH &&
		  CSSbuffer[CSSindex - 1] == '*')
		{
		  /* close a comment:and ignore its contents */
		  CSSindex = CSScomment - 1; /* will be incremented later */
		  CSScomment = MAX_CSS_LENGTH;
		}
	      else if (CSScomment == MAX_CSS_LENGTH &&
		       CSSindex > 0 &&
		       CSSbuffer[CSSindex - 1] ==  '<')
		{
		  /* this is the closing tag ! */
		  CSSindex -= 2; /* remove </ from the CSS string */
		  noRule = TRUE;
		}
	      break;
	    case '<':
	      if (CSScomment == MAX_CSS_LENGTH)
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
	      if (CSSindex > 0 && CSSbuffer[CSSindex - 1] == '-' && HTMLcomment)
		/* CSS within an HTML comment */
		noRule = TRUE;
	      break;
	    case '>':
	      if (HTMLcomment)
		noRule = TRUE;
	      break;
	    case '{':
	      openRule++;
	      if (import != MAX_CSS_LENGTH && openRule == 1)
		{
		  /* is it the screen concerned? */
		  CSSbuffer[CSSindex+1] = EOS;
		  if (TtaIsPrinting ())
		    base = ustrstr (&CSSbuffer[import], TEXT("print"));
		  else
		    base = ustrstr (&CSSbuffer[import], TEXT("screen"));
		  if (base == NULL)
		    ignoreMedia = TRUE;
		  noRule = TRUE;
		}
	      break;
	    case '}':
	      openRule--;
	      if (import != MAX_CSS_LENGTH && openRule == 0)
		{
		  import = MAX_CSS_LENGTH;
		  noRule = TRUE;
		  ignoreMedia = FALSE;
		}
	      else if (ignoreMedia && openRule == 0)
		{
		  ignoreMedia = FALSE;
		  noRule = TRUE;
		}
	      else
		toParse = TRUE;
	      break;
	    default:
	      break;
	    }
	}
      if (c != __CR__)
	CSSindex++;

      if (CSSindex >= MAX_CSS_LENGTH && CSScomment < MAX_CSS_LENGTH)
	/* we're still parsing a comment: remove the text comment */
	CSSindex = CSScomment;

      if  (CSSindex >= MAX_CSS_LENGTH || toParse || noRule)
	{
	  CSSbuffer[CSSindex] = EOS;
	  /* parse a not empty string */
	  if (CSSindex > 0)
	    {
	      /* apply CSS rule if it's not just a saving of text */
	      if (!noRule && !ignoreMedia)
		ParseStyleDeclaration (NULL, CSSbuffer, docRef, css, FALSE);
	      else if (import != MAX_CSS_LENGTH &&
		       !ustrncasecmp (&CSSbuffer[import+1], TEXT("import"), 6))
		{
		  /* import section */
		  cssRule = &CSSbuffer[import+7];
		  cssRule = TtaSkipBlanks (cssRule);
		  if (!ustrncasecmp (cssRule, TEXT("url"), 3))
		    {
		      cssRule = &cssRule[3];
		      cssRule = TtaSkipBlanks (cssRule);
		      if (*cssRule == '(')
			{
			  cssRule++;
			  cssRule = TtaSkipBlanks (cssRule);
			  base = cssRule;
			  while (*cssRule != EOS && *cssRule != ')')
			    cssRule++;
			  *cssRule = EOS;
			  LoadStyleSheet (base, docRef, NULL, css);
			}
		    }
		  /*** Caution: Strings can either be written with double quotes or
		       with single quotes. Only double quotes are handled here.
		       Escaped quotes are not handled. See function SkipQuotedString */
		  else if (*cssRule == '"')
		    {
		      cssRule++;
		      base = cssRule;
		      while (*cssRule != EOS && *cssRule != '"')
			cssRule++;
		      *cssRule = EOS;
		      LoadStyleSheet (base, docRef, NULL, css);
		    }
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
  return (c);
}
