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
 *         R. Guetari (W3C/INRIA): Unicode.
 *
 */

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "undo.h"
#include "registry.h"
#include "fetchHTMLname.h"
#include "uaccess.h"

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
typedef CHAR_T* (*PropertyParser) (Element element,
				    PSchema tsch,
				    PresentationContext context,
				    CHAR_T* cssRule,
				    CSSInfoPtr css,
				    ThotBool isHTML);
#else
typedef CHAR_T* (*PropertyParser) ();
#endif

/* Description of the set of CSS properties supported */
typedef struct CSSProperty
  {
     CHAR_T*              name;
     PropertyParser       parsing_function;
  }
CSSProperty;

#include "HTMLstyleColor.h"

struct unit_def
{
   CHAR_T*             sign;
   unsigned int        unit;
};

static struct unit_def CSSUnitNames[] =
{
   {TEXT("pt"), STYLE_UNIT_PT},
   {TEXT("pc"), STYLE_UNIT_PC},
   {TEXT("in"), STYLE_UNIT_IN},
   {TEXT("cm"), STYLE_UNIT_CM},
   {TEXT("mm"), STYLE_UNIT_MM},
   {TEXT("em"), STYLE_UNIT_EM},
   {TEXT("px"), STYLE_UNIT_PX},
   {TEXT("ex"), STYLE_UNIT_XHEIGHT},
   {TEXT("%"), STYLE_UNIT_PERCENT}
};

#define NB_UNITS (sizeof(CSSUnitNames) / sizeof(struct unit_def))

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static unsigned int hexa_val (CHAR_T c)
#else
static unsigned int hexa_val (c)
CHAR_T              c;
#endif
{
   if (c >= TEXT('0') && c <= TEXT('9'))
      return (c - TEXT('0'));
   if (c >= TEXT('a') && c <= TEXT('f'))
      return (c - TEXT('a') + 10);
   if (c >= TEXT('A') && c <= TEXT('F'))
      return (c - TEXT('A') + 10);
   return (0);
}

/*----------------------------------------------------------------------
   SkipWord:                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*     SkipWord (CHAR_T* ptr)
#else
static CHAR_T*     SkipWord (ptr)
CHAR_T*            ptr;
#endif
{
# ifdef _WINDOWS
  /* iswalnum is supposed to be supported by the i18n veriosn of libc 
     use it when available */
  while (iswalnum (*ptr) || *ptr == TEXT('-') || *ptr == TEXT('%'))
# else  /* !_WINDOWS */
  while (isalnum((int)*ptr) || *ptr == TEXT('-') || *ptr == TEXT('%'))
# endif /* !_WINDOWS */
        ptr++;
  return (ptr);
}

/*----------------------------------------------------------------------
   SkipBlanksAndComments:                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char*        SkipBlanksAndComments (char* ptr)
#else
char*        SkipBlanksAndComments (ptr)
char*        ptr;
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
   SkipWCBlanksAndComments:                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T*        SkipWCBlanksAndComments (CHAR_T* ptr)
#else
CHAR_T*        SkipWCBlanksAndComments (ptr)
CHAR_T*        ptr;
#endif
{
  ptr = TtaSkipWCBlanks (ptr);
  while (ptr[0] == TEXT('/') && ptr[1] == TEXT('*'))
    {
      /* look for the end of the comment */
      ptr = &ptr[2];
      while (ptr[0] != WC_EOS && (ptr[0] != TEXT('*') || ptr[1] != TEXT('/')))
	ptr++;
      if (ptr[0] != WC_EOS)
	ptr = &ptr[2];
      ptr = TtaSkipWCBlanks (ptr);
    }
  return (ptr);
}

/*----------------------------------------------------------------------
   SkipQuotedString:                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        SkipQuotedString (CHAR_T* ptr, CHAR_T quote)
#else
static CHAR_T*        SkipQuotedString (ptr, quote)
CHAR_T*               ptr;
CHAR_T                quote;
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
    else if (*ptr == WC_EOS)
       stop = TRUE;
    else if (*ptr == TEXT('\\'))
       /* escape character */
       {
       ptr++;
       if ((*ptr >= TEXT('0') && *ptr <= TEXT('9')) || (*ptr >= TEXT('A') && *ptr <= TEXT('F')) ||
	   (*ptr >= TEXT('a') && *ptr <= TEXT('f')))
	  {
	  ptr++;
          if ((*ptr >= TEXT('0') && *ptr <= TEXT('9')) || (*ptr >= TEXT('A') && *ptr <= TEXT('F')) ||
	      (*ptr >= TEXT('a') && *ptr <= TEXT('f')))
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
CHAR_T*     SkipProperty (CHAR_T* ptr)
#else
CHAR_T*     SkipProperty (ptr)
CHAR_T*     ptr;
#endif
{
  while (*ptr != WC_EOS && *ptr != TEXT(';') && *ptr != TEXT('}'))
    ptr++;
  return (ptr);
}

/*----------------------------------------------------------------------
   ParseCSSUnit :                                                  
   parse a CSS Unit substring and returns the corresponding      
   value and its unit.                                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*       ParseCSSUnit (CHAR_T* cssRule, PresentationValue *pval)
#else
static CHAR_T*       ParseCSSUnit (cssRule, pval)
CHAR_T*              cssRule;
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
  cssRule = SkipWCBlanksAndComments (cssRule);
  if (*cssRule == TEXT('-'))
    {
      minus = 1;
      cssRule++;
      cssRule = SkipWCBlanksAndComments (cssRule);
    }

  if (*cssRule == TEXT('+'))
    {
      cssRule++;
      cssRule = SkipWCBlanksAndComments (cssRule);
    }

  while ((*cssRule >= TEXT('0')) && (*cssRule <= TEXT('9')))
    {
      val *= 10;
      val += *cssRule - TEXT('0');
      cssRule++;
      valid = 1;
    }

  if (*cssRule == TEXT('.'))
    {
      real = TRUE;
      f = val;
      val = 0;
      cssRule++;
      /* keep only 3 digits */
      if (*cssRule >= TEXT('0') && *cssRule <= TEXT('9'))
	{
	  val = (*cssRule - TEXT('0')) * 100;
	  cssRule++;
	  if (*cssRule >= TEXT('0') && *cssRule <= TEXT('9'))
	    {
	      val += (*cssRule - TEXT('0')) * 10;
	      cssRule++;
	      if ((*cssRule >= TEXT('0')) && (*cssRule <= TEXT('9')))
		{
		  val += *cssRule - TEXT('0');
		  cssRule++;
		}
	    }

	  while (*cssRule >= TEXT('0') && *cssRule <= TEXT('9'))
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
      cssRule = SkipWCBlanksAndComments (cssRule);
      for (uni = 0; uni < NB_UNITS; uni++)
	{
	  if (!ustrncasecmp (CSSUnitNames[uni].sign, cssRule, ustrlen (CSSUnitNames[uni].sign)))
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

/*----------------------------------------------------------------------
   ParseBorderValue                                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*      ParseBorderValue (CHAR_T* cssRule, PresentationValue *border)
#else
static CHAR_T*      ParseBorderValue (cssRule, border)
CHAR_T*             cssRule;
PresentationValue *border
#endif
{
  /* first parse the attribute string */
   border->typed_data.value = 0;
   border->typed_data.unit = STYLE_UNIT_INVALID;
   border->typed_data.real = FALSE;
   if (!ustrncasecmp (cssRule, TEXT("thin"), 4))
     {
       border->typed_data.unit = STYLE_UNIT_PX;
       border->typed_data.value = 1;
       cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("medium"), 6))
     {
       border->typed_data.unit = STYLE_UNIT_PX;
       border->typed_data.value = 3;
       cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("thick"), 5))
     {
       border->typed_data.unit = STYLE_UNIT_PX;
       border->typed_data.value = 5;
       cssRule = SkipWord (cssRule);
     }
   else if (TtaIsDigit (*cssRule))
     cssRule = ParseCSSUnit (cssRule, border);
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseBorderStyle                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*      ParseBorderStyle (CHAR_T* cssRule, PresentationValue *border)
#else
static CHAR_T*      ParseBorderStyle (cssRule, border)
CHAR_T*             cssRule;
PresentationValue *border
#endif
{
  /* first parse the attribute string */
   border->typed_data.value = 0;
   border->typed_data.unit = STYLE_UNIT_PX;
   border->typed_data.real = FALSE;
   if (!ustrncasecmp (cssRule, TEXT("none"), 4))
     border->typed_data.value = STYLE_BORDERNONE;
   else if (!ustrncasecmp (cssRule, TEXT("hidden"), 6))
     border->typed_data.value = STYLE_BORDERHIDDEN;
   else if (!ustrncasecmp (cssRule, TEXT("dotted"), 6))
     border->typed_data.value = STYLE_BORDERDOTTED;
   else if (!ustrncasecmp (cssRule, TEXT("dashed"), 6))
     border->typed_data.value = STYLE_BORDERDASHED;
   else if (!ustrncasecmp (cssRule, TEXT("solid"), 5))
     border->typed_data.value = STYLE_BORDERSOLID;
   else if (!ustrncasecmp (cssRule, TEXT("double"), 6))
     border->typed_data.value = STYLE_BORDERDOUBLE;
   else if (!ustrncasecmp (cssRule, TEXT("groove"), 6))
     border->typed_data.value = STYLE_BORDERGROOVE;
   else if (!ustrncasecmp (cssRule, TEXT("ridge"), 5))
     border->typed_data.value = STYLE_BORDERRIDGE;
   else if (!ustrncasecmp (cssRule, TEXT("inset"), 5))
     border->typed_data.value = STYLE_BORDERINSET;
   else if (!ustrncasecmp (cssRule, TEXT("outset"), 6))
     border->typed_data.value = STYLE_BORDEROUTSET;
   else
     {
       /* invalid style */
       border->typed_data.unit = STYLE_UNIT_INVALID;
       return (cssRule);
     }
   /* the value is parsed now */
   cssRule = SkipWord (cssRule);
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
static CHAR_T*       ParseCSSColor (CHAR_T* cssRule, PresentationValue * val)
#else
static CHAR_T*       ParseCSSColor (cssRule, val)
CHAR_T*              cssRule;
PresentationValue    *val;
#endif
{
  CHAR_T              colname[100];
  CHAR_T*             ptr;
  unsigned short      redval = (unsigned short) -1;
  unsigned short      greenval = 0;	/* composant of each RGB       */
  unsigned short      blueval = 0;	/* default to red if unknown ! */
  unsigned int        i, len;
  int                 r, g, b;
  int                 best = 0;	/* best color in list found */
  ThotBool            failed;

  cssRule = SkipWCBlanksAndComments (cssRule);
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
  if ((*cssRule == TEXT('#')) ||
      (isxdigit (cssRule[0]) && isxdigit (cssRule[1]) && isxdigit (cssRule[2])))
    {
      if (*cssRule == TEXT('#'))
	cssRule++;
      failed = FALSE;
      /* we expect an hexa encoding like F00 or FF0000 */
      if ((!isxdigit (cssRule[0])) || (!isxdigit (cssRule[1])) || (!isxdigit (cssRule[2])))
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
      cssRule = SkipWCBlanksAndComments (cssRule);
      if (*cssRule == TEXT('('))
	{
	  cssRule++;
	  cssRule = SkipWCBlanksAndComments (cssRule);
	  failed = FALSE;
	  if (*cssRule == TEXT('%'))
	    {
	      /* encoded as rgb(%red,%green,&blue) */
	      usscanf (cssRule, TEXT("%%%d"), &r);
	      while (*cssRule != WC_EOS && *cssRule != TEXT(','))
		cssRule++;
	      cssRule++;
	      usscanf (cssRule, TEXT("%%%d"), &g);
	      while (*cssRule != WC_EOS && *cssRule != TEXT(','))
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
	      while (*cssRule != WC_EOS && *cssRule != TEXT(','))
		cssRule++;
	      cssRule++;
	      usscanf (cssRule, TEXT("%d"), &g);
	      while (*cssRule != WC_EOS && *cssRule != TEXT(','))
		cssRule++;
	      cssRule++;
	      usscanf (cssRule, TEXT("%d"), &b);
	      redval = (unsigned short)r;
	      greenval = (unsigned short)g;
	      blueval = (unsigned short)b;
	    }
	  /* search the rgb end */
	  while (*cssRule != WC_EOS && *cssRule != TEXT(')'))
	    cssRule++;
	  cssRule++;
	}
      else
	cssRule = SkipProperty (cssRule);
    }
  else if (TtaIsAlpha (*cssRule))
    {
      /* we expect a color name like "red", store it in colname */
      ptr = cssRule;
      len = (sizeof (colname) / sizeof (CHAR_T)) - 1;
      for (i = 0; i < len && ptr[i] != WC_EOS; i++)
	{
	  if (!TtaIsAlnum (ptr[i]) && ptr[i] != WC_EOS)
	    {
	      ptr += i;
	      break;
	    }
	  colname[i] = ptr[i];
	}
      colname[i] = WC_EOS;
      
      /* Lookup the color name in our own color name database */
      for (i = 0; i < NBCOLORNAME; i++)
	if (!ustrcasecmp (ColornameTable[i].name, colname))
	  {
	    redval = ColornameTable[i].red;
	    greenval = ColornameTable[i].green;
	    blueval = ColornameTable[i].blue;
	    failed = FALSE;
	    cssRule = ptr;
	    i = NBCOLORNAME;
	  }
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
   ParseCSSBorderTopWidth : parse a CSS BorderTopWidth
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSBorderTopWidth (Element element, PSchema tsch,
                    PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSBorderTopWidth (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue   border;
  
  cssRule = SkipWCBlanksAndComments (cssRule);
  cssRule = ParseBorderValue (cssRule, &border);
  if (border.typed_data.unit != STYLE_UNIT_INVALID)
    {
      TtaSetStylePresentation (PRBorderTopWidth, element, tsch, context, border);
      border.typed_data.value = 1;
      border.typed_data.unit = STYLE_UNIT_REL;
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderBottomWidth : parse a CSS BorderBottomWidth
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSBorderBottomWidth (Element element, PSchema tsch,
                    PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSBorderBottomWidth (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue   border;
  
  cssRule = SkipWCBlanksAndComments (cssRule);
  /* first parse the attribute string */
  cssRule = ParseBorderValue (cssRule, &border);
  if (border.typed_data.unit != STYLE_UNIT_INVALID)
    {
      TtaSetStylePresentation (PRBorderBottomWidth, element, tsch, context, border);
      border.typed_data.value = 1;
      border.typed_data.unit = STYLE_UNIT_REL;
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderLeftWidth : parse a CSS BorderLeftWidth
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSBorderLeftWidth (Element element, PSchema tsch,
                    PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSBorderLeftWidth (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue   border;
  
  cssRule = SkipWCBlanksAndComments (cssRule);
  /* first parse the attribute string */
  cssRule = ParseBorderValue (cssRule, &border);
  if (border.typed_data.unit != STYLE_UNIT_INVALID)
    {
      TtaSetStylePresentation (PRBorderLeftWidth, element, tsch, context, border);
      border.typed_data.value = 1;
      border.typed_data.unit = STYLE_UNIT_REL;
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderRightWidth : parse a CSS BorderRightWidth
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSBorderRightWidth (Element element, PSchema tsch,
                    PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSBorderRightWidth (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue   border;
  
  cssRule = SkipWCBlanksAndComments (cssRule);
  /* first parse the attribute string */
  cssRule = ParseBorderValue (cssRule, &border);
  if (border.typed_data.unit != STYLE_UNIT_INVALID)
    {
      TtaSetStylePresentation (PRBorderRightWidth, element, tsch, context, border);
      border.typed_data.value = 1;
      border.typed_data.unit = STYLE_UNIT_REL;
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderWidth : parse a CSS BorderWidth
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSBorderWidth (Element element, PSchema tsch,
                    PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSBorderWidth (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  CHAR_T *ptrT, *ptrR, *ptrB, *ptrL;

  ptrT = SkipWCBlanksAndComments (cssRule);
  /* First parse Border-Top */
  ptrR = ParseCSSBorderTopWidth (element, tsch, context, ptrT, css, isHTML);
  ptrR = SkipWCBlanksAndComments (ptrR);
  if (*ptrR == TEXT(';') || *ptrR == WC_EOS || *ptrR == TEXT(','))
    {
      cssRule = ptrR;
      /* apply the Border-Top to all */
      ptrR = ParseCSSBorderRightWidth (element, tsch, context, ptrT, css, isHTML);
      ptrR = ParseCSSBorderBottomWidth (element, tsch, context, ptrT, css, isHTML);
      ptrR = ParseCSSBorderLeftWidth (element, tsch, context, ptrT, css, isHTML);
    }
  else
    {
      /* parse Border-Right */
      ptrB = ParseCSSBorderRightWidth (element, tsch, context, ptrR, css, isHTML);
      ptrB = SkipWCBlanksAndComments (ptrB);
      if (*ptrB == TEXT(';') || *ptrB == WC_EOS || *ptrB == TEXT(','))
	{
	  cssRule = ptrB;
	  /* apply the Border-Top to Border-Bottom */
	  ptrB = ParseCSSBorderBottomWidth (element, tsch, context, ptrT, css, isHTML);
	  /* apply the Border-Right to Border-Left */
	  ptrB = ParseCSSBorderLeftWidth (element, tsch, context, ptrR, css, isHTML);
	}
      else
	{
	  /* parse Border-Bottom */
	  ptrL = ParseCSSBorderBottomWidth (element, tsch, context, ptrB, css, isHTML);
	  ptrL = SkipWCBlanksAndComments (ptrL);
	  if (*ptrL == TEXT(';') || *ptrL == WC_EOS || *ptrL == TEXT(','))
	    {
	      cssRule = ptrL;
	      /* apply the Border-Right to Border-Left */
	      ptrL = ParseCSSBorderLeftWidth (element, tsch, context, ptrR, css, isHTML);
	    }
	  else
	    /* parse Border-Left */
	    cssRule = ParseCSSBorderLeftWidth (element, tsch, context, ptrL, css, isHTML);
	  cssRule = SkipWCBlanksAndComments (cssRule);
	}
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderColorTop : parse a CSS BorderColorTop
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*      ParseCSSBorderColorTop (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*      ParseCSSBorderColorTop (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*             cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
   PresentationValue   best;

   cssRule = ParseCSSColor (cssRule, &best);
   if (best.typed_data.unit != STYLE_UNIT_INVALID)
     /* install the new presentation */
     TtaSetStylePresentation (PRBorderTopColor, element, tsch, context, best);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderColorLeft : parse a CSS BorderColorLeft
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*      ParseCSSBorderColorLeft (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*      ParseCSSBorderColorLeft (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*             cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
   PresentationValue   best;

   cssRule = ParseCSSColor (cssRule, &best);
   if (best.typed_data.unit != STYLE_UNIT_INVALID)
     /* install the new presentation */
     TtaSetStylePresentation (PRBorderLeftColor, element, tsch, context, best);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderColorBottom : parse a CSS BorderColorBottom
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*      ParseCSSBorderColorBottom (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSBorderColorBottom (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*             cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
   PresentationValue   best;

   cssRule = ParseCSSColor (cssRule, &best);
   if (best.typed_data.unit != STYLE_UNIT_INVALID)
     /* install the new presentation */
     TtaSetStylePresentation (PRBorderBottomColor, element, tsch, context, best);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderColorRight : parse a CSS BorderColorRight
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*      ParseCSSBorderColorRight (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSBorderColorRight (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*             cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
   PresentationValue   best;

   cssRule = ParseCSSColor (cssRule, &best);
   if (best.typed_data.unit != STYLE_UNIT_INVALID)
     /* install the new presentation */
     TtaSetStylePresentation (PRBorderRightColor, element, tsch, context, best);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderColor : parse a CSS border-color        
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSBorderColor (Element element, PSchema tsch,
                    PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSBorderColor (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  CHAR_T *ptrT, *ptrR, *ptrB, *ptrL;

  ptrT = SkipWCBlanksAndComments (cssRule);
  /* First parse Border-Top */
  ptrR = ParseCSSBorderColorTop (element, tsch, context, ptrT, css, isHTML);
  ptrR = SkipWCBlanksAndComments (ptrR);
  if (*ptrR == TEXT(';') || *ptrR == WC_EOS || *ptrR == TEXT(','))
    {
      cssRule = ptrR;
      /* apply the Border-Top to all */
      ptrR = ParseCSSBorderColorRight (element, tsch, context, ptrT, css, isHTML);
      ptrR = ParseCSSBorderColorBottom (element, tsch, context, ptrT, css, isHTML);
      ptrR = ParseCSSBorderColorLeft (element, tsch, context, ptrT, css, isHTML);
    }
  else
    {
      /* parse Border-Right */
      ptrB = ParseCSSBorderColorRight (element, tsch, context, ptrR, css, isHTML);
      ptrB = SkipWCBlanksAndComments (ptrB);
      if (*ptrB == TEXT(';') || *ptrB == WC_EOS || *ptrB == TEXT(','))
	{
	  cssRule = ptrB;
	  /* apply the Border-Top to Border-Bottom */
	  ptrB = ParseCSSBorderColorBottom (element, tsch, context, ptrT, css, isHTML);
	  /* apply the Border-Right to Border-Left */
	  ptrB = ParseCSSBorderColorLeft (element, tsch, context, ptrR, css, isHTML);
	}
      else
	{
	  /* parse Border-Bottom */
	  ptrL = ParseCSSBorderColorBottom (element, tsch, context, ptrB, css, isHTML);
	  ptrL = SkipWCBlanksAndComments (ptrL);
	  if (*ptrL == TEXT(';') || *ptrL == WC_EOS || *ptrL == TEXT(','))
	    {
	      cssRule = ptrL;
	      /* apply the Border-Right to Border-Left */
	      ptrL = ParseCSSBorderColorLeft (element, tsch, context, ptrR, css, isHTML);
	    }
	  else
	    /* parse Border-Left */
	    cssRule = ParseCSSBorderColorLeft (element, tsch, context, ptrL, css, isHTML);
	  cssRule = SkipWCBlanksAndComments (cssRule);
	}
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderStyleTop : parse a CSS BorderStyleTop
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSBorderStyleTop (Element element, PSchema tsch,
                    PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSBorderStyleTop (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue   border;
  
  cssRule = SkipWCBlanksAndComments (cssRule);
  cssRule = ParseBorderStyle (cssRule, &border);
  if (border.typed_data.unit != STYLE_UNIT_INVALID)
    TtaSetStylePresentation (PRBorderTopStyle, element, tsch, context, border);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderStyleLeft : parse a CSS BorderStyleLeft
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSBorderStyleLeft (Element element, PSchema tsch,
                    PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSBorderStyleLeft (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue   border;
  
  cssRule = SkipWCBlanksAndComments (cssRule);
  cssRule = ParseBorderStyle (cssRule, &border);
  if (border.typed_data.unit != STYLE_UNIT_INVALID)
    TtaSetStylePresentation (PRBorderLeftStyle, element, tsch, context, border);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderStyleBottom : parse a CSS BorderStyleBottom
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSBorderStyleBottom (Element element, PSchema tsch,
                    PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSBorderStyleBottom (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue   border;
  
  cssRule = SkipWCBlanksAndComments (cssRule);
  cssRule = ParseBorderStyle (cssRule, &border);
  if (border.typed_data.unit != STYLE_UNIT_INVALID)
    TtaSetStylePresentation (PRBorderBottomStyle, element, tsch, context, border);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderStyleRight : parse a CSS BorderStyleRight
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSBorderStyleRight (Element element, PSchema tsch,
                    PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSBorderStyleRight (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue   border;
  
  cssRule = SkipWCBlanksAndComments (cssRule);
  cssRule = ParseBorderStyle (cssRule, &border);
  if (border.typed_data.unit != STYLE_UNIT_INVALID)
    TtaSetStylePresentation (PRBorderRightStyle, element, tsch, context, border);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderStyleStyle : parse a CSS border-style        
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSBorderStyle (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSBorderStyle (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  CHAR_T *ptrT, *ptrR, *ptrB, *ptrL;

  ptrT = SkipWCBlanksAndComments (cssRule);
  /* First parse Border-Top */
  ptrR = ParseCSSBorderStyleTop (element, tsch, context, ptrT, css, isHTML);
  ptrR = SkipWCBlanksAndComments (ptrR);
  if (*ptrR == TEXT(';') || *ptrR == WC_EOS || *ptrR == TEXT(','))
    {
      cssRule = ptrR;
      /* apply the Border-Top to all */
      ptrR = ParseCSSBorderStyleRight (element, tsch, context, ptrT, css, isHTML);
      ptrR = ParseCSSBorderStyleBottom (element, tsch, context, ptrT, css, isHTML);
      ptrR = ParseCSSBorderStyleLeft (element, tsch, context, ptrT, css, isHTML);
    }
  else
    {
      /* parse Border-Right */
      ptrB = ParseCSSBorderStyleRight (element, tsch, context, ptrR, css, isHTML);
      ptrB = SkipWCBlanksAndComments (ptrB);
      if (*ptrB == TEXT(';') || *ptrB == WC_EOS || *ptrB == TEXT(','))
	{
	  cssRule = ptrB;
	  /* apply the Border-Top to Border-Bottom */
	  ptrB = ParseCSSBorderStyleBottom (element, tsch, context, ptrT, css, isHTML);
	  /* apply the Border-Right to Border-Left */
	  ptrB = ParseCSSBorderStyleLeft (element, tsch, context, ptrR, css, isHTML);
	}
      else
	{
	  /* parse Border-Bottom */
	  ptrL = ParseCSSBorderStyleBottom (element, tsch, context, ptrB, css, isHTML);
	  ptrL = SkipWCBlanksAndComments (ptrL);
	  if (*ptrL == TEXT(';') || *ptrL == WC_EOS || *ptrL == TEXT(','))
	    {
	      cssRule = ptrL;
	      /* apply the Border-Right to Border-Left */
	      ptrL = ParseCSSBorderStyleLeft (element, tsch, context, ptrR, css, isHTML);
	    }
	  else
	    /* parse Border-Left */
	    cssRule = ParseCSSBorderStyleLeft (element, tsch, context, ptrL, css, isHTML);
	  cssRule = SkipWCBlanksAndComments (cssRule);
	}
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderTop : parse a CSS BorderTop
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*      ParseCSSBorderTop (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*      ParseCSSBorderTop (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*             cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  CHAR_T*           ptr;

  cssRule = SkipWCBlanksAndComments (cssRule);
  while (*cssRule != TEXT(';') && *cssRule != WC_EOS && *cssRule != TEXT(','))
    {
      ptr = cssRule;
      cssRule = ParseCSSBorderStyleTop (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	cssRule = ParseCSSBorderTopWidth (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	cssRule = ParseCSSBorderColorTop (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	/* rule not found */
	cssRule = SkipProperty (cssRule);
      cssRule = SkipWCBlanksAndComments (cssRule);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderLeft : parse a CSS BorderLeft
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*      ParseCSSBorderLeft (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*      ParseCSSBorderLeft (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*             cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  CHAR_T*           ptr;

  cssRule = SkipWCBlanksAndComments (cssRule);
  while (*cssRule != TEXT(';') && *cssRule != WC_EOS && *cssRule != TEXT(','))
    {
      ptr = cssRule;
      cssRule = ParseCSSBorderStyleLeft (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	cssRule = ParseCSSBorderLeftWidth (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	cssRule = ParseCSSBorderColorLeft (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	/* rule not found */
	cssRule = SkipProperty (cssRule);
      cssRule = SkipWCBlanksAndComments (cssRule);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderBottom : parse a CSS BorderBottom
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*      ParseCSSBorderBottom (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*      ParseCSSBorderBottom (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*             cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  CHAR_T*           ptr;

  cssRule = SkipWCBlanksAndComments (cssRule);
  while (*cssRule != TEXT(';') && *cssRule != WC_EOS && *cssRule != TEXT(','))
    {
      ptr = cssRule;
      cssRule = ParseCSSBorderStyleBottom (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	cssRule = ParseCSSBorderBottomWidth (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	cssRule = ParseCSSBorderColorBottom (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	/* rule not found */
	cssRule = SkipProperty (cssRule);
      cssRule = SkipWCBlanksAndComments (cssRule);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorderRight : parse a CSS BorderRight
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSBorderRight (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSBorderRight (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  CHAR_T*            ptr;

  cssRule = SkipWCBlanksAndComments (cssRule);
  while (*cssRule != TEXT(';') && *cssRule != WC_EOS && *cssRule != TEXT(','))
    {
      ptr = cssRule;
      cssRule = ParseCSSBorderStyleRight (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	cssRule = ParseCSSBorderRightWidth (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	cssRule = ParseCSSBorderColorRight (element, tsch, context, cssRule, css, isHTML);
      if (ptr == cssRule)
	/* rule not found */
	cssRule = SkipProperty (cssRule);
      cssRule = SkipWCBlanksAndComments (cssRule);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBorder : parse a CSS border        
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSBorder (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSBorder (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  CHAR_T *ptrT, *ptrR;

  ptrT = SkipWCBlanksAndComments (cssRule);
  /* First parse Border-Top */
  ptrR = ParseCSSBorderTop (element, tsch, context, ptrT, css, isHTML);
  ptrR = SkipWCBlanksAndComments (ptrR);
  if (*ptrR == TEXT(';') || *ptrR == WC_EOS || *ptrR == TEXT(','))
    {
      cssRule = ptrR;
      /* apply the Border-Top to all */
      ptrR = ParseCSSBorderRight (element, tsch, context, ptrT, css, isHTML);
      ptrR = ParseCSSBorderBottom (element, tsch, context, ptrT, css, isHTML);
      ptrR = ParseCSSBorderLeft (element, tsch, context, ptrT, css, isHTML);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSClear : parse a CSS clear attribute string    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSClear (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSClear (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
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
static CHAR_T*        ParseCSSDisplay (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSDisplay (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
   PresentationValue   pval;

   pval.typed_data.unit = STYLE_UNIT_REL;
   pval.typed_data.real = FALSE;
   cssRule = SkipWCBlanksAndComments (cssRule);
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
static CHAR_T*        ParseCSSFloat (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSFloat (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
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
static CHAR_T*        ParseCSSLetterSpacing (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSLetterSpacing (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
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
static CHAR_T*        ParseCSSListStyleType (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSListStyleType (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
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
static CHAR_T*        ParseCSSListStyleImage (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSListStyleImage (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
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
static CHAR_T*        ParseCSSListStylePosition (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSListStylePosition (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
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
static CHAR_T*        ParseCSSListStyle (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSListStyle (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
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
static CHAR_T*        ParseCSSTextAlign (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSTextAlign (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
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

   cssRule = SkipWCBlanksAndComments (cssRule);
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
static CHAR_T*        ParseCSSTextIndent (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSTextIndent (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
   PresentationValue   pval;

   cssRule = SkipWCBlanksAndComments (cssRule);
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
static CHAR_T*        ParseCSSTextTransform (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSTextTransform (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
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
static CHAR_T*        ParseCSSVerticalAlign (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSVerticalAlign (element, tsch, context, cssRule, css, isHTML)
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
static CHAR_T*        ParseCSSWhiteSpace (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSWhiteSpace (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
   cssRule = SkipWCBlanksAndComments (cssRule);
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
static CHAR_T*        ParseCSSWordSpacing (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSWordSpacing (element, tsch, context, cssRule, css, isHTML)
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
static CHAR_T*        ParseCSSLineSpacing (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSLineSpacing (element, tsch, context, cssRule, css, isHTML)
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
static CHAR_T*        ParseCSSFontSize (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSFontSize (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
   PresentationValue   pval;
   CHAR_T*             ptr = NULL;
   ThotBool	       real;

   pval.typed_data.real = FALSE;
   cssRule = SkipWCBlanksAndComments (cssRule);
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
	   ptr[0] = WC_EOS;
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
static CHAR_T*        ParseCSSFontFamily (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSFontFamily (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue   font;
  CHAR_T              quoteChar;

  font.typed_data.value = 0;
  font.typed_data.unit = STYLE_UNIT_REL;
  font.typed_data.real = FALSE;
  cssRule = SkipWCBlanksAndComments (cssRule);
  if (*cssRule == TEXT('"') || *cssRule == TEXT('\''))
     {
     quoteChar = *cssRule;
     cssRule++;
     }
  else
     quoteChar = WC_EOS;

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
      if (quoteChar) {
         cssRule = SkipQuotedString (cssRule, quoteChar);
      } else
         cssRule = SkipWord (cssRule);
      cssRule = SkipWCBlanksAndComments (cssRule);
      if (*cssRule == TEXT(','))
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
static CHAR_T*        ParseCSSFontWeight (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSFontWeight (element, tsch, context, cssRule, css, isHTML)
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
   cssRule = SkipWCBlanksAndComments (cssRule);
   if (!ustrncasecmp (cssRule, TEXT("100"), 3) && !TtaIsAlpha (cssRule[3]))
     {
	weight.typed_data.value = -3;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("200"), 3) && !TtaIsAlpha (cssRule[3]))
     {
	weight.typed_data.value = -2;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("300"), 3) && !TtaIsAlpha (cssRule[3]))
     {
	weight.typed_data.value = -1;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("normal"), 6) || (!ustrncasecmp (cssRule, TEXT("400"), 3) && !TtaIsAlpha (cssRule[3])))
     {
	weight.typed_data.value = 0;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("500"), 3) && !TtaIsAlpha (cssRule[3]))
     {
	weight.typed_data.value = +1;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("600"), 3) && !TtaIsAlpha (cssRule[3]))
     {
	weight.typed_data.value = +2;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("bold"), 4) || (!ustrncasecmp (cssRule, TEXT("700"), 3) && !TtaIsAlpha (cssRule[3])))
     {
	weight.typed_data.value = +3;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("800"), 3) && !TtaIsAlpha (cssRule[3]))
     {
	weight.typed_data.value = +4;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("900"), 3) && !TtaIsAlpha (cssRule[3]))
     {
	weight.typed_data.value = +5;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("inherit"), 7) || !ustrncasecmp (cssRule, TEXT("bolder"), 6) || !ustrncasecmp (cssRule, TEXT("lighter"), 7))
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
static CHAR_T*        ParseCSSFontVariant (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSFontVariant (element, tsch, context, cssRule, css, isHTML)
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
   cssRule = SkipWCBlanksAndComments (cssRule);
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
static CHAR_T*        ParseCSSFontStyle (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSFontStyle (element, tsch, context, cssRule, css, isHTML)
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
   cssRule = SkipWCBlanksAndComments (cssRule);
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
static CHAR_T*        ParseCSSFont (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSFont (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  CHAR_T*           ptr;

  cssRule = SkipWCBlanksAndComments (cssRule);
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
      if (*cssRule == TEXT('/'))
	{
	  cssRule++;
	  SkipWCBlanksAndComments (cssRule);
	  cssRule = SkipWord (cssRule);
	}
      cssRule = ParseCSSFontFamily (element, tsch, context, cssRule, css, isHTML);
      cssRule = SkipWCBlanksAndComments (cssRule);
      while (*cssRule != TEXT(';') && *cssRule != WC_EOS)
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
static CHAR_T*        ParseCSSTextDecoration (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSTextDecoration (element, tsch, context, cssRule, css, isHTML)
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
   cssRule = SkipWCBlanksAndComments (cssRule);
   if (!ustrncasecmp (cssRule, TEXT("underline"), strlen ("underline")))
     {
	decor.typed_data.value = Underline;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("overline"), strlen ("overline")))
     {
	decor.typed_data.value = Overline;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("line-through"), strlen ("line-through")))
     {
	decor.typed_data.value = CrossOut;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("box"), strlen ("box")))
     {
       /* the box text-decoration attribute is not yet supported */
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("boxshadow"), strlen ("boxshadow")))
     {
       /* the boxshadow text-decoration attribute is not yet supported */
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("box3d"), strlen ("box3d")))
     {
       /* the box3d text-decoration attribute is not yet supported */
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("cartouche"), strlen ("cartouche")))
     {
	/*the cartouche text-decoration attribute is not yet supported */
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("blink"), strlen ("blink")))
     {
	/*the blink text-decoration attribute will not be supported */
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, TEXT("none"), strlen ("none")))
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
   ParseCSSHeight : parse a CSS height attribute                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSHeight (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSHeight (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
   cssRule = SkipWCBlanksAndComments (cssRule);

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
static CHAR_T*        ParseCSSWidth (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSWidth (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
   cssRule = SkipWCBlanksAndComments (cssRule);

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
static CHAR_T*        ParseCSSMarginTop (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSMarginTop (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue   margin;
  
  cssRule = SkipWCBlanksAndComments (cssRule);
  /* first parse the attribute string */
  cssRule = ParseCSSUnit (cssRule, &margin);
  if (margin.typed_data.unit != STYLE_UNIT_INVALID)
    {
      TtaSetStylePresentation (PRMarginTop, element, tsch, context, margin);
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
static CHAR_T*        ParseCSSMarginBottom (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSMarginBottom (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue   margin;
  
  cssRule = SkipWCBlanksAndComments (cssRule);
  /* first parse the attribute string */
  cssRule = ParseCSSUnit (cssRule, &margin);
  if (margin.typed_data.unit != STYLE_UNIT_INVALID)
    TtaSetStylePresentation (PRMarginBottom, element, tsch, context, margin);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSMarginLeft : parse a CSS margin-left          
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSMarginLeft (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSMarginLeft (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue   margin;
  
  cssRule = SkipWCBlanksAndComments (cssRule);
  /* first parse the attribute string */
  cssRule = ParseCSSUnit (cssRule, &margin);
  if (margin.typed_data.unit != STYLE_UNIT_INVALID)
    {
      TtaSetStylePresentation (PRMarginLeft, element, tsch, context, margin);
      if (margin.typed_data.value < 0)
	TtaSetStylePresentation (PRHorizOverflow, element, tsch, context, margin);
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSMarginRight : parse a CSS margin-right        
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSMarginRight (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSMarginRight (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue   margin;
  
  cssRule = SkipWCBlanksAndComments (cssRule);
  /* first parse the attribute string */
  cssRule = ParseCSSUnit (cssRule, &margin);
  if (margin.typed_data.unit != STYLE_UNIT_INVALID)
      TtaSetStylePresentation (PRMarginRight, element, tsch, context, margin);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSMargin : parse a CSS margin attribute string. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSMargin (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSMargin (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  CHAR_T *ptrT, *ptrR, *ptrB, *ptrL;

  ptrT = SkipWCBlanksAndComments (cssRule);
  /* First parse Margin-Top */
  ptrR = ParseCSSMarginTop (element, tsch, context, ptrT, css, isHTML);
  ptrR = SkipWCBlanksAndComments (ptrR);
  if (*ptrR == TEXT(';') || *ptrR == WC_EOS || *ptrR == TEXT(','))
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
      ptrB = SkipWCBlanksAndComments (ptrB);
      if (*ptrB == TEXT(';') || *ptrB == WC_EOS || *ptrB == TEXT(','))
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
	  ptrL = SkipWCBlanksAndComments (ptrL);
	  if (*ptrL == TEXT(';') || *ptrL == WC_EOS || *ptrL == TEXT(','))
	    {
	      cssRule = ptrL;
	      /* apply the Margin-Right to Margin-Left */
	      ptrL = ParseCSSMarginLeft (element, tsch, context, ptrR, css, isHTML);
	    }
	  else
	    /* parse Margin-Left */
	    cssRule = ParseCSSMarginLeft (element, tsch, context, ptrL, css, isHTML);
	  cssRule = SkipWCBlanksAndComments (cssRule);
	}
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSPaddingTop : parse a CSS PaddingTop
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSPaddingTop (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSPaddingTop (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue   padding;
  
  cssRule = SkipWCBlanksAndComments (cssRule);
  /* first parse the attribute string */
  cssRule = ParseCSSUnit (cssRule, &padding);
  if (padding.typed_data.unit != STYLE_UNIT_INVALID)
      TtaSetStylePresentation (PRPaddingTop, element, tsch, context, padding);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSPaddingBottom : parse a CSS PaddingBottom
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSPaddingBottom (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSPaddingBottom (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue   padding;
  
  cssRule = SkipWCBlanksAndComments (cssRule);
  /* first parse the attribute string */
  cssRule = ParseCSSUnit (cssRule, &padding);
  if (padding.typed_data.unit != STYLE_UNIT_INVALID)
      TtaSetStylePresentation (PRPaddingBottom, element, tsch, context, padding);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSPaddingLeft : parse a CSS PaddingLeft
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSPaddingLeft (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSPaddingLeft (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue   padding;
  
  cssRule = SkipWCBlanksAndComments (cssRule);
  /* first parse the attribute string */
  cssRule = ParseCSSUnit (cssRule, &padding);
  if (padding.typed_data.unit != STYLE_UNIT_INVALID)
      TtaSetStylePresentation (PRPaddingLeft, element, tsch, context, padding);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSPaddingRight : parse a CSS PaddingRight
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSPaddingRight (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSPaddingRight (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  PresentationValue   padding;
  
  cssRule = SkipWCBlanksAndComments (cssRule);
  /* first parse the attribute string */
  cssRule = ParseCSSUnit (cssRule, &padding);
  if (padding.typed_data.unit != STYLE_UNIT_INVALID)
      TtaSetStylePresentation (PRPaddingRight, element, tsch, context, padding);
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSPadding : parse a CSS padding attribute string. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSPadding (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSPadding (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  CHAR_T *ptrT, *ptrR, *ptrB, *ptrL;

  ptrT = SkipWCBlanksAndComments (cssRule);
  /* First parse Padding-Top */
  ptrR = ParseCSSPaddingTop (element, tsch, context, ptrT, css, isHTML);
  ptrR = SkipWCBlanksAndComments (ptrR);
  if (*ptrR == TEXT(';') || *ptrR == WC_EOS || *ptrR == TEXT(','))
    {
      cssRule = ptrR;
      /* apply the Padding-Top to all */
      ptrR = ParseCSSPaddingRight (element, tsch, context, ptrT, css, isHTML);
      ptrR = ParseCSSPaddingBottom (element, tsch, context, ptrT, css, isHTML);
      ptrR = ParseCSSPaddingLeft (element, tsch, context, ptrT, css, isHTML);
    }
  else
    {
      /* parse Padding-Right */
      ptrB = ParseCSSPaddingRight (element, tsch, context, ptrR, css, isHTML);
      ptrB = SkipWCBlanksAndComments (ptrB);
      if (*ptrB == TEXT(';') || *ptrB == WC_EOS || *ptrB == TEXT(','))
	{
	  cssRule = ptrB;
	  /* apply the Padding-Top to Padding-Bottom */
	  ptrB = ParseCSSPaddingBottom (element, tsch, context, ptrT, css, isHTML);
	  /* apply the Padding-Right to Padding-Left */
	  ptrB = ParseCSSPaddingLeft (element, tsch, context, ptrR, css, isHTML);
	}
      else
	{
	  /* parse Padding-Bottom */
	  ptrL = ParseCSSPaddingBottom (element, tsch, context, ptrB, css, isHTML);
	  ptrL = SkipWCBlanksAndComments (ptrL);
	  if (*ptrL == TEXT(';') || *ptrL == WC_EOS || *ptrL == TEXT(','))
	    {
	      cssRule = ptrL;
	      /* apply the Padding-Right to Padding-Left */
	      ptrL = ParseCSSPaddingLeft (element, tsch, context, ptrR, css, isHTML);
	    }
	  else
	    /* parse Padding-Left */
	    cssRule = ParseCSSPaddingLeft (element, tsch, context, ptrL, css, isHTML);
	  cssRule = SkipWCBlanksAndComments (cssRule);
	}
    }
  return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSForeground : parse a CSS foreground attribute 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CHAR_T*        ParseCSSForeground (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSForeground (element, tsch, context, cssRule, css, isHTML)
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
static CHAR_T*      ParseCSSBackgroundColor (Element element, PSchema tsch,
                    PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*      ParseCSSBackgroundColor (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*             cssRule;
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
  if (!ustrncasecmp (cssRule, TEXT("transparent"), strlen ("transparent")))
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
void ParseCSSBackgroundImageCallback (Document doc, Element element, STRING file, void *extra)
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
CHAR_T*             GetCSSBackgroundURL (CHAR_T* styleString)
#else
CHAR_T*             GetCSSBackgroundURL (styleString)
CHAR_T*             styleString;
#endif
{
  CHAR_T *b, *e, *ptr;
  int                 len;

  ptr = NULL;
  b = ustrstr (styleString, TEXT("url"));
  if (b != NULL)
    {
      b += 3;
      b = SkipWCBlanksAndComments (b);
      if (*b == TEXT('('))
	{
	  b++;
	  b = SkipWCBlanksAndComments (b);
	  /*** Caution: Strings can either be written with double quotes or
	       with single quotes. Only double quotes are handled here.
	       Escaped quotes are not handled. See function SkipQuotedString */
	  if (*b == TEXT('"'))
	    {
	      b++;
	      /* search the url end */
	      e = b;
	      while (*e != WC_EOS && *e != TEXT('"'))
		e++;
	    }
	  else
	    {
	      /* search the url end */
	      e = b;
	      while (*e != WC_EOS && *e != TEXT(')'))
		e++;
	    }
	  if (*e != WC_EOS)
	    {
	      len = (int)(e - b);
	      ptr = (CHAR_T*) TtaAllocString (len+1);
	      ustrncpy (ptr, b, len);
	      ptr[len] = WC_EOS;
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
static CHAR_T*      ParseCSSBackgroundImage (Element element, PSchema tsch,
                    PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*      ParseCSSBackgroundImage (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*             cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  Element                    el;
  GenericContext             gblock;
  PresentationContextBlock*  sblock;
  BackgroundImageCallbackPtr callblock;
  PresentationValue          image, value;
  CHAR_T*                    url;
  STRING                     bg_image;
  CHAR_T                     saved;
  CHAR_T*                    base;
  CHAR_T                     tempname[MAX_LENGTH];
  CHAR_T                     imgname[MAX_LENGTH];
  unsigned int               savedtype = 0;
  ThotBool                   moved;

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
  cssRule = SkipWCBlanksAndComments (cssRule);
  if (!ustrncasecmp (cssRule, TEXT("url"), 3))
    {  
      cssRule += 3;
      cssRule = SkipWCBlanksAndComments (cssRule);
      if (*cssRule == '(')
	{
	  cssRule++;
	  cssRule = SkipWCBlanksAndComments (cssRule);
	  /*** Caution: Strings can either be written with double quotes or
	    with single quotes. Only double quotes are handled here.
	    Escaped quotes are not handled. See function SkipQuotedString */
	  if (*cssRule == '"')
	    {
	      cssRule++;
	      base = cssRule;
	      while (*cssRule != WC_EOS && *cssRule != TEXT('"'))
		cssRule++;
	    }
	  else
	    {
	      base = cssRule;
	      while (*cssRule != EOS && *cssRule != ')')
		cssRule++;
	    }
	  saved = *cssRule;
	  *cssRule = WC_EOS;
	  url = TtaWCSdup (base);
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
		      FetchImage (context->doc, el, tempname, AMAYA_LOAD_IMAGE, ParseCSSBackgroundImageCallback, callblock);
		    }
		  else
		    FetchImage (context->doc, el, url, AMAYA_LOAD_IMAGE, ParseCSSBackgroundImageCallback, callblock);
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
static CHAR_T*        ParseCSSBackgroundRepeat (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSBackgroundRepeat (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*             cssRule;
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
  cssRule = SkipWCBlanksAndComments (cssRule);
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
static CHAR_T*        ParseCSSBackgroundAttachment (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSBackgroundAttachment (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*             cssRule;
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

   cssRule = SkipWCBlanksAndComments (cssRule);
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
static CHAR_T*        ParseCSSBackgroundPosition (Element element, PSchema tsch,
				 PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*        ParseCSSBackgroundPosition (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*             cssRule;
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

   cssRule = SkipWCBlanksAndComments (cssRule);
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
   else if (TtaIsDigit (*cssRule))
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
static CHAR_T*      ParseCSSBackground (Element element, PSchema tsch,
				    PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static CHAR_T*      ParseCSSBackground (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*             cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  CHAR_T*           ptr;

  cssRule = SkipWCBlanksAndComments (cssRule);
  while (*cssRule != TEXT(';') && *cssRule != WC_EOS && *cssRule != TEXT(','))
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
               !ustrncasecmp (cssRule, TEXT("repeat-y"), 8)  ||
               !ustrncasecmp (cssRule, TEXT("repeat-x"), 8)  ||
               !ustrncasecmp (cssRule, TEXT("repeat"), 6))
	cssRule = ParseCSSBackgroundRepeat (element, tsch, context, cssRule, css, isHTML);
      /* perhaps a Background Position */
      else if (!ustrncasecmp (cssRule, TEXT("left"), 4)   ||
               !ustrncasecmp (cssRule, TEXT("right"), 5)  ||
               !ustrncasecmp (cssRule, TEXT("center"), 6) ||
               !ustrncasecmp (cssRule, TEXT("top"), 3)    ||
               !ustrncasecmp (cssRule, TEXT("bottom"), 6) ||
               TtaIsDigit (*cssRule))
           cssRule = ParseCSSBackgroundPosition (element, tsch, context, cssRule, css, isHTML);
      /* perhaps a Background Color */
      else
	{
	  /* check if the rule has been found */
	  ptr = cssRule;
	  cssRule = ParseCSSBackgroundColor (element, tsch, context, cssRule, css, isHTML);
	  if (ptr == cssRule)
	    /* rule not found */
	    cssRule = SkipProperty (cssRule);
	}
      cssRule = SkipWCBlanksAndComments (cssRule);
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
   {TEXT("font-family"), ParseCSSFontFamily},
   {TEXT("font-style"), ParseCSSFontStyle},
   {TEXT("font-variant"), ParseCSSFontVariant},
   {TEXT("font-weight"), ParseCSSFontWeight},
   {TEXT("font-size"), ParseCSSFontSize},
   {TEXT("font"), ParseCSSFont},

   {TEXT("color"), ParseCSSForeground},
   {TEXT("background-color"), ParseCSSBackgroundColor},
   {TEXT("background-image"), ParseCSSBackgroundImage},
   {TEXT("background-repeat"), ParseCSSBackgroundRepeat},
   {TEXT("background-attachment"), ParseCSSBackgroundAttachment},
   {TEXT("background-position"), ParseCSSBackgroundPosition},
   {TEXT("background"), ParseCSSBackground},

   {TEXT("word-spacing"), ParseCSSWordSpacing},
   {TEXT("letter-spacing"), ParseCSSLetterSpacing},
   {TEXT("text-decoration"), ParseCSSTextDecoration},
   {TEXT("vertical-align"), ParseCSSVerticalAlign},
   {TEXT("text-transform"), ParseCSSTextTransform},
   {TEXT("text-align"), ParseCSSTextAlign},
   {TEXT("text-indent"), ParseCSSTextIndent},
   {TEXT("line-height"), ParseCSSLineSpacing},

   {TEXT("margin-top"), ParseCSSMarginTop},
   {TEXT("margin-right"), ParseCSSMarginRight},
   {TEXT("margin-bottom"), ParseCSSMarginBottom},
   {TEXT("margin-left"), ParseCSSMarginLeft},
   {TEXT("margin"), ParseCSSMargin},

   {TEXT("padding-top"), ParseCSSPaddingTop},
   {TEXT("padding-right"), ParseCSSPaddingRight},
   {TEXT("padding-bottom"), ParseCSSPaddingBottom},
   {TEXT("padding-left"), ParseCSSPaddingLeft},
   {TEXT("padding"), ParseCSSPadding},

   {TEXT("border-top-width"), ParseCSSBorderTopWidth},
   {TEXT("border-right-width"), ParseCSSBorderRightWidth},
   {TEXT("border-bottom-width"), ParseCSSBorderBottomWidth},
   {TEXT("border-left-width"), ParseCSSBorderLeftWidth},
   {TEXT("border-width"), ParseCSSBorderWidth},
   {TEXT("border-top-color"), ParseCSSBorderColorTop},
   {TEXT("border-right-color"), ParseCSSBorderColorRight},
   {TEXT("border-bottom-color"), ParseCSSBorderColorBottom},
   {TEXT("border-left-color"), ParseCSSBorderColorLeft},
   {TEXT("border-color"), ParseCSSBorderColor},
   {TEXT("border-top-style"), ParseCSSBorderStyleTop},
   {TEXT("border-right-style"), ParseCSSBorderStyleRight},
   {TEXT("border-bottom-style"), ParseCSSBorderStyleBottom},
   {TEXT("border-left-style"), ParseCSSBorderStyleLeft},
   {TEXT("border-style"), ParseCSSBorderStyle},
   {TEXT("border-top"), ParseCSSBorderTop},
   {TEXT("border-right"), ParseCSSBorderRight},
   {TEXT("border-bottom"), ParseCSSBorderBottom},
   {TEXT("border-left"), ParseCSSBorderLeft},
   {TEXT("border"), ParseCSSBorder},

   {TEXT("width"), ParseCSSWidth},
   {TEXT("height"), ParseCSSHeight},
   {TEXT("float"), ParseCSSFloat},
   {TEXT("clear"), ParseCSSClear},

   {TEXT("display"), ParseCSSDisplay},
   {TEXT("white-space"), ParseCSSWhiteSpace},

   {TEXT("list-style-type"), ParseCSSListStyleType},
   {TEXT("list-style-image"), ParseCSSListStyleImage},
   {TEXT("list-style-position"), ParseCSSListStylePosition},
   {TEXT("list-style"), ParseCSSListStyle}
};
#define NB_CSSSTYLEATTRIBUTE (sizeof(CSSProperties) / sizeof(CSSProperty))

/*----------------------------------------------------------------------
   ParseCSSRule : parse a CSS Style string                        
   we expect the input string describing the style to be of the  
   form : PRORPERTY : DESCRIPTION [ ; PROPERTY : DESCRIPTION ] * 
   but tolerate incorrect or incomplete input                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ParseCSSRule (Element element, PSchema tsch, PresentationContext context, CHAR_T* cssRule, CSSInfoPtr css, ThotBool isHTML)
#else
static void         ParseCSSRule (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
CHAR_T*               cssRule;
CSSInfoPtr          css;
ThotBool            isHTML;
#endif
{
  DisplayMode         dispMode;
  CHAR_T*             p = NULL;
  int                 lg;
  unsigned int        i;
  ThotBool            found;

  /* avoid too many redisplay */
  dispMode = TtaGetDisplayMode (context->doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (context->doc, DeferredDisplay);

  while (*cssRule != WC_EOS)
    {
      cssRule = SkipWCBlanksAndComments (cssRule);
      
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
	  cssRule = SkipWCBlanksAndComments (cssRule);
	  if (*cssRule == TEXT(':'))
	    {
	      cssRule++;
	      cssRule = SkipWCBlanksAndComments (cssRule);
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
      cssRule = SkipWCBlanksAndComments (cssRule);
      if (*cssRule == TEXT(',') || *cssRule == TEXT(';'))
	{
	  cssRule++;
	  cssRule = SkipWCBlanksAndComments (cssRule);
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
void                 PToCss (PresentationSetting settings, CHAR_T* buffer, int len)
#else
void                 PToCss (settings, buffer, len)
PresentationSetting  settings;
CHAR_T*                param;
int                  len
#endif
{
  float               fval = 0;
  unsigned short      red, green, blue;
  int                 add_unit = 0;
  unsigned int        unit, i;
  ThotBool            real = FALSE;

  buffer[0] = WC_EOS;
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
    case PRMarginTop:
      if (real)
	usprintf (buffer, TEXT("marging-top: %g"), fval);
      else
	usprintf (buffer, TEXT("marging-top: %d"), settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRMarginLeft:
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
	usprintf (buffer, TEXT("background-image: url(%s)"), (char*)(settings->value.pointer));
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
void                ParseHTMLSpecificStyle (Element el, CHAR_T* cssRule, Document doc, ThotBool destroy)
#else
void                ParseHTMLSpecificStyle (el, cssRule, doc, destroy)
Element             elem;
CHAR_T*             cssRule;
Document            doc;
ThotBool            destroy;
#endif
{
   PresentationContext context;
   ElementType         elType;
   ThotBool            isHTML;

   /*  A rule applying to BODY is really meant to address HTML */
   elType = TtaGetElementType (el);
   isHTML = (ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")) == 0);
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
static CHAR_T*   ParseGenericSelector (CHAR_T* selector, CHAR_T* cssRule,
			   GenericContext ctxt, Document doc, CSSInfoPtr css)
#else
static CHAR_T*   ParseGenericSelector (selector, cssRule, ctxt, doc, css)
CHAR_T*          selector;
CHAR_T*          cssRule;
GenericContext  ctxt;
Document        doc;
CSSInfoPtr      css;
#endif
{
  ElementType         elType;
  PSchema             tsch;
  AttributeType       attrType;
  CHAR_T              sel[MAX_ANCESTORS * 50];
  CHAR_T              *deb, *cur;
  CHAR_T*             structName;
  CHAR_T*             names[MAX_ANCESTORS];
  CHAR_T*             ids[MAX_ANCESTORS];
  CHAR_T*             classes[MAX_ANCESTORS];
  CHAR_T*             pseudoclasses[MAX_ANCESTORS];
  CHAR_T*             attrs[MAX_ANCESTORS];
  CHAR_T*             attrvals[MAX_ANCESTORS];
  int                 i, j, k, max, maxAttr;
  ThotBool            isHTML;

  sel[0] = WC_EOS;
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
  
  selector = SkipWCBlanksAndComments (selector);
  cur = &sel[0];
  max = 0; /* number of loops */
  while (1)
    {
      deb = cur;
      /* copy an item of the selector into sel[] */
      /* put one word in the sel buffer */
      while (*selector != WC_EOS && *selector != TEXT(',') &&
             *selector != TEXT('.') && *selector != TEXT(':') &&
             *selector != TEXT('#') && !TtaIsWCBlank (selector))
            *cur++ = *selector++;
      *cur++ = WC_EOS; /* close the first string  in sel[] */
      if (deb[0] != WC_EOS)
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
      if (*selector == TEXT(':') || *selector == TEXT('.') || *selector == TEXT('#'))
	/* keep the element name which precedes the id or
	 pseudo class or the class */
	deb = cur;

      if (*selector == TEXT('.'))
	{
	  /* copy into sel[] the class */
	  classes[0] = cur;
	  selector++;
	  while (*selector != WC_EOS && *selector != TEXT(',') &&
		 *selector != TEXT('.') && *selector != TEXT(':') &&
		 !TtaIsWCBlank (selector))
	    *cur++ = *selector++;
	  *cur++ = WC_EOS;
	}
      else if (*selector == TEXT(':'))
	{
	  /* copy into sel[] the pseudoclass */
	  pseudoclasses[0]= cur;
	  selector++;
	  while (*selector != WC_EOS && *selector != TEXT(',') &&
             *selector != TEXT('.') && *selector != TEXT(':') &&
             !TtaIsWCBlank (selector))
            *cur++ = *selector++;
	  *cur++ = WC_EOS;
	}
      else if (*selector == TEXT('#'))
	{
	  /* copy into sel[] the attribute */
	  ids[0] = cur;
	  selector++;
	  while (*selector != WC_EOS && *selector != TEXT(',') &&
             *selector != TEXT('.') && *selector != TEXT(':') &&
             !TtaIsWCBlank (selector))
            *cur++ = *selector++;
	  *cur++ = WC_EOS;
	}
      else if (*selector == TEXT('['))
	{
	  /* copy into sel[] the attribute */
	  attrs[0] = cur;
	  selector++;
	  while (*selector != WC_EOS && *selector != TEXT(']') && *selector != TEXT('='))
	    *cur++ = *selector++;
	  if (*cur == TEXT('='))
	    {
	      /* there is a value "xxxx" */
	      *cur++ = WC_EOS;
	      while (*selector != WC_EOS && *selector != TEXT(']') && *selector != TEXT('"'))
		selector++;
	      if (*selector != WC_EOS)
		{
		  /* we are now parsing the attribute value */
		  attrvals[0] = cur;
		  selector++;
		  while (*selector != WC_EOS && *selector != TEXT('"'))
		    *cur++ = *selector++;
		  if (*selector != WC_EOS)
		    selector++;
		}
	    }
	  *cur++ = WC_EOS;
	}

      selector = SkipWCBlanksAndComments (selector);

      /* is it a multi-level selector? */
      if (*selector == WC_EOS)
	/* end of the selector */
	break;
      else if (*selector == TEXT(','))
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
  isHTML = (ustrcmp (TtaGetSSchemaName (ctxt->schema), TEXT("HTML")) == 0);
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
static void         ParseStyleDeclaration (Element el, CHAR_T* cssRule, Document doc, CSSInfoPtr css, ThotBool destroy)
#else
static void         ParseStyleDeclaration (el, cssRule, doc, css, destroy)
Element             el;
STRING              cssRule;
Document            doc;
CSSInfoPtr          css;
ThotBool            destroy;
#endif
{
  GenericContext        ctxt;
  CHAR_T*               decl_end;
  CHAR_T*               sel_end;
  CHAR_T*               selector;
  CHAR_T                saved1;
  CHAR_T                saved2;

  /* separate the selectors string */
  cssRule = SkipWCBlanksAndComments (cssRule);
  decl_end = cssRule;
  while ((*decl_end != WC_EOS) && (*decl_end != TEXT('{')))
    decl_end++;
  if (*decl_end == WC_EOS)
    return;
  /* verify and clean the selector string */
  sel_end = decl_end - 1;
  while (*sel_end == WC_SPACE || *sel_end == WC_BSPACE ||
	 *sel_end == WC_EOL || *sel_end == WC_CR)
    sel_end--;
  sel_end++;
  saved1 = *sel_end;
  *sel_end = WC_EOS;
  selector = cssRule;

  /* now, deal with the content ... */
  decl_end++;
  cssRule = decl_end;
  while (*decl_end != WC_EOS && *decl_end != TEXT('}'))
    decl_end++;
  if (*decl_end == WC_EOS)
    {
      fprintf (stderr, "Invalid STYLE declaration : %s\n", cssRule);
      return;
    }
  saved2 = *decl_end;
  *decl_end = WC_EOS;

  /*
   * parse the style attribute string and install the corresponding
   * presentation attributes on the new element
   */
  ctxt = TtaGetGenericStyleContext (doc);
  if (ctxt == NULL)
    return;
  ctxt->destroy = destroy;

  while ((selector != NULL) && (*selector != WC_EOS))
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
int                 IsImplicitClassName (CHAR_T* class, Document doc)
#else
int                 IsImplicitClassName (class, doc)
CHAR_T*             class;
Document            doc;
#endif
{
   CHAR_T           name[200];
   CHAR_T*          cur = name;
   CHAR_T*          first; 
   CHAR_T           save;
   SSchema          schema;

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
	cur = SkipWCBlanksAndComments (cur);
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
void                HTMLSetBackgroundColor (Document doc, Element el, CHAR_T* color)
#else
void                HTMLSetBackgroundColor (doc, el, color)
Document            doc;
Element             el;
CHAR_T*             color;
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
void                HTMLSetBackgroundImage (Document doc, Element el, int repeat, CHAR_T* image)
#else
void                HTMLSetBackgroundImage (doc, el, repeat, image)
Document            doc;
Element             el;
int                 repeat;
CHAR_T*             image;
#endif
{
   CHAR_T           css_command[400];

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
void                HTMLSetForegroundColor (Document doc, Element el, CHAR_T* color)
#else
void                HTMLSetForegroundColor (doc, el, color)
Document            doc;
Element             el;
CHAR_T*             color;
#endif
{
   CHAR_T           css_command[100];

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
   CHAR_T           css_command[100];

   usprintf (css_command, TEXT("background: red"));
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
   CHAR_T           css_command[1000];

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
   CHAR_T           css_command[100];

   /* it's not necessary to well know the current color but it must be valid */
   usprintf (css_command, TEXT("color: red"));
   ParseHTMLSpecificStyle (el, css_command, doc, TRUE);
}

/*----------------------------------------------------------------------
   HTMLSetAlinkColor :                                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLSetAlinkColor (Document doc, CHAR_T* color)
#else
void                HTMLSetAlinkColor (doc, color)
Document            doc;
CHAR_T*             color;
#endif
{
   CHAR_T           css_command[100];

   usprintf (css_command, TEXT("a:link { color : %s }"), color);
   ApplyCSSRules (NULL, css_command, doc, FALSE);
}

/*----------------------------------------------------------------------
   HTMLSetAactiveColor :                                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLSetAactiveColor (Document doc, CHAR_T* color)
#else
void                HTMLSetAactiveColor (doc, color)
Document            doc;
CHAR_T*             color;
#endif
{
   CHAR_T           css_command[100];

   usprintf (css_command, TEXT("a:active { color : %s }"), color);
   ApplyCSSRules (NULL, css_command, doc, FALSE);
}

/*----------------------------------------------------------------------
   HTMLSetAvisitedColor :                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HTMLSetAvisitedColor (Document doc, CHAR_T* color)
#else
void                HTMLSetAvisitedColor (doc, color)
Document            doc;
CHAR_T*             color;
#endif
{
   CHAR_T           css_command[100];

   usprintf (css_command, TEXT("a:visited { color : %s }"), color);
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
   CHAR_T           css_command[100];

   usprintf (css_command, TEXT("a:link { color : red }"));
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
   CHAR_T           css_command[100];

   usprintf (css_command, TEXT("a:active { color : red }"));
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
   CHAR_T           css_command[100];

   usprintf (css_command, TEXT("a:visited { color : red }"));
   ApplyCSSRules (NULL, css_command, doc, TRUE);
}

/*----------------------------------------------------------------------
  ApplyCSSRules: parse an CSS Style description stored in the
  header of a HTML document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ApplyCSSRules (Element el, CHAR_T* cssRule, Document doc, ThotBool destroy)
#else
void                ApplyCSSRules (el, cssRule, doc, destroy)
Element             el;
CHAR_T*             cssRule;
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
CHAR_T              ReadCSSRules (Document docRef, CSSInfoPtr css, CHAR_T* buffer, ThotBool withUndo)
#else
CHAR_T              ReadCSSRules (docRef, css, buffer, withUndo)
Document            docRef;
CSSInfoPtr          css;
CHAR_T*             buffer;
ThotBool            withUndo;
#endif
{
  CHAR_T              c;
  CHAR_T              *cssRule, *base;
  DisplayMode         dispMode;
  int                 index;
  int                 CSSindex;
  int                 CSScomment;
  int                 import;
  int                 openRule;
  ThotBool            HTMLcomment;
  ThotBool            toParse, eof;
  ThotBool            ignoreMedia, media;
  ThotBool            noRule;

  CSScomment = MAX_CSS_LENGTH;
  HTMLcomment = FALSE;
  CSSindex = 0;
  toParse = FALSE;
  noRule = FALSE;
  media =  FALSE;
  ignoreMedia = FALSE;
  import = MAX_CSS_LENGTH;
  eof = FALSE;
  openRule = 0;
  c = WC_SPACE;
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

  while (CSSindex < MAX_CSS_LENGTH && c != EOS && !eof) {
        c = buffer[index++];
        eof = (c == WC_EOS);
        CSSbuffer[CSSindex] = c;
        if (CSScomment == MAX_CSS_LENGTH || c == TEXT('*') || c == TEXT('/') || c == TEXT('<')) {
           /* we're not within a comment or we're parsing * or / */
           switch (c) {
                  case TEXT('@'): /* perhaps an import primitive */
                       import = CSSindex;
                       break;
                  case TEXT(';'):
                       if (import != MAX_CSS_LENGTH && !media) { 
                          if (ustrncasecmp (&CSSbuffer[import+1], TEXT("import"), 6))
                             /* it's not an import */
                             import = MAX_CSS_LENGTH;
                         /* save the text */
                         noRule = TRUE;
                       }
                       break;
                  case TEXT('*'):
                       if (CSScomment == MAX_CSS_LENGTH && CSSindex > 0 && CSSbuffer[CSSindex - 1] == TEXT('/'))
                          /* start a comment */
                          CSScomment = CSSindex - 1;
                       break;
                  case TEXT('/'):
                       if (CSSindex > 1 && CSScomment != MAX_CSS_LENGTH && CSSbuffer[CSSindex - 1] == TEXT('*')) {
                          /* close a comment:and ignore its contents */
                          CSSindex = CSScomment - 1; /* will be incremented later */
                          CSScomment = MAX_CSS_LENGTH;
                       } else if (CSScomment == MAX_CSS_LENGTH && CSSindex > 0 && CSSbuffer[CSSindex - 1] ==  TEXT('<')) {
                              /* this is the closing tag ! */
                              CSSindex -= 2; /* remove </ from the CSS string */
                              noRule = TRUE;
                       } 
                       break;
                  case TEXT('<'):
                       if (CSScomment == MAX_CSS_LENGTH) {
                          /* only if we're not parsing a comment */
                          c = buffer[index++];
                          eof = (c == WC_EOS);
                          if (c == TEXT('!')) {
                             /* CSS within an HTML comment */
                             HTMLcomment = TRUE;
                             CSSindex++;
                             CSSbuffer[CSSindex] = c;
                          } else if (c == WC_EOS)
                                 CSSindex++;
                       }
                       break;
                  case TEXT('-'):
                       if (CSSindex > 0 && CSSbuffer[CSSindex - 1] == TEXT('-') && HTMLcomment)
                          /* CSS within an HTML comment */
                          noRule = TRUE;
                       break;
                  case TEXT('>'):
                       if (HTMLcomment)
                          noRule = TRUE;
                       break;
                  case TEXT(' '):
                       if (import != MAX_CSS_LENGTH && openRule == 0)
                          media = !ustrncmp (&CSSbuffer[import+1], TEXT("media"), 5);
                       break;
                  case TEXT('{'):
                       openRule++;
                       if (import != MAX_CSS_LENGTH && openRule == 1 && media) {
                          /* is it the screen concerned? */
                          CSSbuffer[CSSindex+1] = WC_EOS;
                          if (TtaIsPrinting ())
                             base = ustrstr (&CSSbuffer[import], TEXT("print"));
                          else
                               base = ustrstr (&CSSbuffer[import], TEXT("screen"));
                          if (base == NULL)
                             ignoreMedia = TRUE;
                          noRule = TRUE;
                       }
                       break;
                  case TEXT('}'):
                       openRule--;
                       if (import != MAX_CSS_LENGTH && openRule == 0) {
                          import = MAX_CSS_LENGTH;
                          noRule = TRUE;
                          ignoreMedia = FALSE;
                          media = FALSE;
                       } else
                              toParse = TRUE;
                       break;
                  default:
                       break;
           }
        }    
        if (c != WC_CR)
           CSSindex++;

        if (CSSindex >= MAX_CSS_LENGTH && CSScomment < MAX_CSS_LENGTH)
           /* we're still parsing a comment: remove the text comment */
           CSSindex = CSScomment;

        if (CSSindex >= MAX_CSS_LENGTH || toParse || noRule) {
           CSSbuffer[CSSindex] = WC_EOS;
           /* parse a not empty string */
           if (CSSindex > 0) {
              /* apply CSS rule if it's not just a saving of text */
              if (!noRule && !ignoreMedia)
                 ParseStyleDeclaration (NULL, CSSbuffer, docRef, css, FALSE);
              else if (import != MAX_CSS_LENGTH && !ustrncasecmp (&CSSbuffer[import+1], TEXT("import"), 6)) {
                   /* import section */
                   cssRule = &CSSbuffer[import+7];
                   cssRule = TtaSkipWCBlanks (cssRule);
                   if (!ustrncasecmp (cssRule, TEXT("url"), 3)) {
                      cssRule = &cssRule[3];
                      cssRule = TtaSkipWCBlanks (cssRule);
                      if (*cssRule == TEXT('(')) {
                         cssRule++;
                         cssRule = TtaSkipWCBlanks (cssRule);
                         base = cssRule;
                         while (*cssRule != WC_EOS && *cssRule != TEXT(')'))
                                cssRule++;
                         *cssRule = WC_EOS;
                         LoadStyleSheet (base, docRef, NULL, css, css->media[docRef]);
                      }
                   }
                   /*** Caution: Strings can either be written with double quotes or
                        with single quotes. Only double quotes are handled here.
                        Escaped quotes are not handled. See function SkipQuotedString */
                   else if (*cssRule == TEXT('"')) {
                        cssRule++;
                        base = cssRule;
                        while (*cssRule != WC_EOS && *cssRule != TEXT('"'))
                              cssRule++;
                        *cssRule = WC_EOS;
                        LoadStyleSheet (base, docRef, NULL, css, css->media[docRef]);
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
