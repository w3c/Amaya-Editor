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
typedef char    *(*PropertyParser) (Element element,
				    PSchema tsch,
				    PresentationContext context,
				    STRING cssRule,
				    CSSInfoPtr css,
				    boolean isHTML);
#else
typedef char    *(*PropertyParser) ();
#endif

/* Description of the set of CSS properties supported */
typedef struct CSSProperty
  {
     STRING               name;
     PropertyParser parsing_function;
  }
CSSProperty;

#define MAX_DEEP 10
#include "HTMLstyleColor.h"

struct unit_def
{
   STRING              sign;
   unsigned int        unit;
};

static struct unit_def CSSUnitNames[] =
{
   {"pt", STYLE_UNIT_PT},
   {"pc", STYLE_UNIT_PC},
   {"in", STYLE_UNIT_IN},
   {"cm", STYLE_UNIT_CM},
   {"mm", STYLE_UNIT_MM},
   {"em", STYLE_UNIT_EM},
   {"px", STYLE_UNIT_PX},
   {"ex", STYLE_UNIT_XHEIGHT},
   {"%", STYLE_UNIT_PERCENT}
};

#define NB_UNITS (sizeof(CSSUnitNames) / sizeof(struct unit_def))

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static unsigned int hexa_val (CHAR c)
#else
static unsigned int hexa_val (c)
CHAR                c;
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
   SkipQuotedString:                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       SkipQuotedString (STRING ptr, CHAR quote)
#else
static STRING       SkipQuotedString (ptr, quote)
STRING              ptr;
CHAR		   quote;
#endif
{
  boolean	stop;

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
  boolean             real = FALSE;

  pval->typed_data.unit = STYLE_UNIT_REL;
  pval->typed_data.real = FALSE;
  cssRule = TtaSkipBlanks (cssRule);
  if (*cssRule == '-')
    {
      minus = 1;
      cssRule++;
      cssRule = TtaSkipBlanks (cssRule);
    }

  if (*cssRule == '+')
    {
      cssRule++;
      cssRule = TtaSkipBlanks (cssRule);
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
      cssRule = TtaSkipBlanks (cssRule);
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

/*----------------------------------------------------------------------
   GetCSSName : return a string corresponding to the CSS name of   
   an element                                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING              GetCSSName (Element el, Document doc)
#else
STRING              GetCSSName (el, doc)
Element             el;
Document            doc;
#endif
{
  STRING              res = GITagName (el);

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
static int      GetCSSNames (Element el, Document doc, STRING *lst, int max)
#else
static int      GetCSSNames (el, doc, lst, max)
Element         el;
Document        doc;
STRING*         lst;
int             max;
#endif
{
   STRING       res;
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
	if (!ustrcmp (res, "BODY"))
	   break;
	if (!ustrcmp (res, "HTML"))
	   break;

	/* store this level in the array */
	lst[deep] = res;
	deep++;
     }
   return (deep);
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
static void          PToCss (PresentationSetting settings, STRING buffer, int len)
#else
static void          PToCss (settings, buffer, len)
PresentationSetting  settings;
STRING               param;
int                  len
#endif
{
  float               fval = 0;
  unsigned short      red, green, blue;
  int                 add_unit = 0;
  unsigned int        unit, i;
  boolean             real = FALSE;

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
	  ustrcpy (buffer, "font-family: helvetica");
	  break;
	case STYLE_FONT_TIMES:
	  ustrcpy (buffer, "font-family: times");
	  break;
	case STYLE_FONT_COURIER:
	  ustrcpy (buffer, "font-family: courier");
	  break;
	}
      break;
    case PRStyle:
      switch (settings->value.typed_data.value)
	{
	case STYLE_FONT_BOLD:
	  ustrcpy (buffer, "font-weight: bold");
	  break;
	case STYLE_FONT_ROMAN:
	  ustrcpy (buffer, "font-style: normal");
	  break;
	case STYLE_FONT_ITALICS:
	  ustrcpy (buffer, "font-style: italic");
	  break;
	case STYLE_FONT_BOLDITALICS:
	  ustrcpy (buffer, "font-weight: bold; font-style: italic");
	  break;
	case STYLE_FONT_OBLIQUE:
	  ustrcpy (buffer, "font-style: oblique");
	  break;
	case STYLE_FONT_BOLDOBLIQUE:
	  ustrcpy (buffer, "font-weight: bold; font-style: oblique");
	  break;
	}
      break;
    case PRSize:
      if (unit == STYLE_UNIT_REL)
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
		ustrcpy (buffer, "font-size: xx-small");
		break;
	      case 2:
		ustrcpy (buffer, "font-size: x-small");
		break;
	      case 3:
		ustrcpy (buffer, "font-size: small");
		break;
	      case 4:
		ustrcpy (buffer, "font-size: medium");
		break;
	      case 5:
		ustrcpy (buffer, "font-size: large");
		break;
	      case 6:
		ustrcpy (buffer, "font-size: x-large");
		break;
	      case 7:
	      case 8:
	      case 9:
	      case 10:
	      case 11:
	      case 12:
		ustrcpy (buffer, "font-size: xx-large");
		break;
	      }
	}
      else
	{
	  if (real)
	    sprintf (buffer, "font-size: %g", fval);
	  else
	    sprintf (buffer, "font-size: %d", settings->value.typed_data.value);
	  add_unit = 1;
	}
      break;
    case PRUnderline:
      switch (settings->value.typed_data.value)
	{
	case STYLE_UNDERLINE:
	  ustrcpy (buffer, "text-decoration: underline");
	  break;
	case STYLE_OVERLINE:
	  ustrcpy (buffer, "text-decoration: overline");
	  break;
	case STYLE_CROSSOUT:
	  ustrcpy (buffer, "text-decoration: line-through");
	  break;
	}
      break;
    case PRIndent:
      if (real)
	sprintf (buffer, "text-indent: %g", fval);
      else
	sprintf (buffer, "text-indent: %d", settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRLineSpacing:
      if (real)
	sprintf (buffer, "line-height: %g", fval);
      else
	sprintf (buffer, "line-height: %d", settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRJustify:
      if (settings->value.typed_data.value == STYLE_JUSTIFIED)
	sprintf (buffer, "text-align: justify");
      break;
    case PRAdjust:
      switch (settings->value.typed_data.value)
	{
	case STYLE_ADJUSTLEFT:
	  ustrcpy (buffer, "text-align: left");
	  break;
	case STYLE_ADJUSTRIGHT:
	  ustrcpy (buffer, "text-align: right");
	  break;
	case STYLE_ADJUSTCENTERED:
	  ustrcpy (buffer, "text-align: center");
	  break;
	case STYLE_ADJUSTLEFTWITHDOTS:
	  ustrcpy (buffer, "text-align: left");
	  break;
	}
      break;
    case PRHyphenate:
      break;
    case PRFillPattern:
      break;
    case PRBackground:
      TtaGiveThotRGB (settings->value.typed_data.value, &red, &green, &blue);
      sprintf (buffer, "background-color: #%02X%02X%02X", red, green, blue);
      break;
    case PRForeground:
      TtaGiveThotRGB (settings->value.typed_data.value, &red, &green, &blue);
      sprintf (buffer, "color: #%02X%02X%02X", red, green, blue);
      break;
    case PRTMargin:
      if (real)
	sprintf (buffer, "marging-top: %g", fval);
      else
	sprintf (buffer, "marging-top: %d", settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRLMargin:
      if (real)
	sprintf (buffer, "margin-left: %g", fval);
      else
	sprintf (buffer, "margin-left: %d", settings->value.typed_data.value);
      add_unit = 1;
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
    case PRLine:
      if (settings->value.typed_data.value == STYLE_INLINE)
	ustrcpy (buffer, "display: inline");
      else if (settings->value.typed_data.value == STYLE_NOTINLINE)
	ustrcpy (buffer, "display: block");
      break;
    case PRBackgroundPicture:
      if (settings->value.pointer != NULL)
	sprintf (buffer, "background-image: url(%s)", (STRING)settings->value.pointer);
      else
	sprintf (buffer, "background-image: none");
      break;
    case PRPictureMode:
      switch (settings->value.typed_data.value)
	{
	case STYLE_REALSIZE:
	  sprintf (buffer, "background-repeat: no-repeat");
	  break;
	case STYLE_REPEAT:
	  sprintf (buffer, "background-repeat: repeat");
	  break;
	case STYLE_VREPEAT:
	  sprintf (buffer, "background-repeat: repeat-y");
	  break;
	case STYLE_HREPEAT:
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
	      ustrcat (buffer, CSSUnitNames[i].sign);
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
static void  SpecificSettingsToCSS (Element el, Document doc, PresentationSetting settings, void *param)
#else
static void  SpecificSettingsToCSS (el, doc, settings, param)
Element              el;
Document             doc;
PresentationSetting  settings;
void                *param;
#endif
{
  LoadedImageDesc    *imgInfo;
  STRING              css_rules = param;
  CHAR                string[150];
  STRING              ptr;

  string[0] = EOS;
  if (settings->type == PRBackgroundPicture)
    {
      /* transform absolute URL into relative URL */
      imgInfo = SearchLoadedImage((STRING)settings->value.pointer, 0);
      if (imgInfo != NULL)
	ptr = MakeRelativeURL (imgInfo->originalName, DocumentURLs[doc]);
      else
	ptr = MakeRelativeURL ((STRING)settings->value.pointer, DocumentURLs[doc]);
      settings->value.pointer = ptr;
      PToCss (settings, string, sizeof(string));
      TtaFreeMemory (ptr);
    }
  else
    PToCss (settings, string, sizeof(string));

  if (string[0] != EOS && *css_rules != EOS)
    ustrcat (css_rules, "; ");
  if (string[0] != EOS)
    ustrcat (css_rules, string);
}

/*----------------------------------------------------------------------
   GetHTMLStyleString : return a string corresponding to the CSS    
   description of the presentation attribute applied to a       
   element.
   For stupid reasons, if the target element is HTML or BODY,
   one returns the concatenation of both element style strings.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetHTMLStyleString (Element el, Document doc, STRING buf, int *len)
#else
void                GetHTMLStyleString (el, doc, buf, len)
Element             el;
Document            doc;
STRING              buf;
int                *len;
#endif
{
  ElementType          elType;

  if ((buf == NULL) || (len == NULL) || (*len <= 0))
    return;

  /*
   * this will transform all the Specific Settings associated to
   * the element to one CSS string.
   */
  buf[0] = EOS;
  TtaApplyAllSpecificSettings (el, doc, SpecificSettingsToCSS, &buf[0]);
  *len = ustrlen (buf);

  /*
   * BODY / HTML elements specific handling.
   */
  elType = TtaGetElementType (el);
  if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
    {
      if (elType.ElTypeNum == HTML_EL_HTML)
	{
	  elType.ElTypeNum = HTML_EL_BODY;
	  el = TtaSearchTypedElement(elType, SearchForward, el);
	  if (!el)
	    return;
	  if (*len > 0)
	    ustrcat(buf,";");
	  *len = ustrlen (buf);
	  TtaApplyAllSpecificSettings (el, doc, SpecificSettingsToCSS, &buf[*len]);
	  *len = ustrlen (buf);
	}
      else if (elType.ElTypeNum == HTML_EL_BODY)
	{
	  el = TtaGetParent (el);
	  if (!el)
	    return;
	  if (*len > 0)
	    ustrcat(buf,";");
	  *len = ustrlen (buf);
	  TtaApplyAllSpecificSettings (el, doc, SpecificSettingsToCSS, &buf[*len]);
	  *len = ustrlen (buf);
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
   ParseCSSBorderTopWidth : parse a CSS BorderTopWidth
   attribute string.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSBorderTopWidth (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSBorderTopWidth (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSBorderRightWidth (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSBorderBottomWidth (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSBorderLeftWidth (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSBorderWidth (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSBorderTop (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSBorderRight (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSBorderBottom (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSBorderLeft (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSBorderColor (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSBorderStyle (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSBorder (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSClear (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSDisplay (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
   PresentationValue   pval;

   pval.typed_data.unit = STYLE_UNIT_REL;
   pval.typed_data.real = FALSE;
   cssRule = TtaSkipBlanks (cssRule);
   if (!ustrncasecmp (cssRule, "block", 5))
     {
	pval.typed_data.value = STYLE_NOTINLINE;
	TtaSetStylePresentation (PRLine, element, tsch, context, pval);
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "inline", 6))
     {
	pval.typed_data.value = STYLE_INLINE;
	TtaSetStylePresentation (PRLine, element, tsch, context, pval);
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "none", 4))
     {
	pval.typed_data.value = STYLE_HIDE;
	TtaSetStylePresentation (PRVisibility, element, tsch, context, pval);
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "list-item", 9))
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSFloat (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSLetterSpacing (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSListStyleType (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSListStyleImage (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSListStylePosition (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSListStyle (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSTextAlign (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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

   cssRule = TtaSkipBlanks (cssRule);
   if (!ustrncasecmp (cssRule, "left", 4))
     {
	align.typed_data.value = AdjustLeft;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "right", 5))
     {
	align.typed_data.value = AdjustRight;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "center", 6))
     {
	align.typed_data.value = Centered;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "justify", 7))
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSTextIndent (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
   PresentationValue   pval;

   cssRule = TtaSkipBlanks (cssRule);
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSTextTransform (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSVerticalAlign (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSWhiteSpace (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
   cssRule = TtaSkipBlanks (cssRule);
   if (!ustrncasecmp (cssRule, "normal", 6))
     cssRule = SkipWord (cssRule);
   else if (!ustrncasecmp (cssRule, "pre", 3))
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSWordSpacing (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
  cssRule = SkipProperty (cssRule);
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSFontSize (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
   PresentationValue   pval;
   boolean	       real;

   pval.typed_data.real = FALSE;
   cssRule = TtaSkipBlanks (cssRule);
   if (!ustrncasecmp (cssRule, "larger", 6))
     {
	pval.typed_data.unit = STYLE_UNIT_PERCENT;
	pval.typed_data.value = 130;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "smaller", 7))
     {
	pval.typed_data.unit = STYLE_UNIT_PERCENT;
	pval.typed_data.value = 80;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "xx-small", 8))
     {
	pval.typed_data.unit = STYLE_UNIT_REL;
	pval.typed_data.value = 1;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "x-small", 7))
     {
	pval.typed_data.unit = STYLE_UNIT_REL;
	pval.typed_data.value = 2;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "small", 5))
     {
	pval.typed_data.unit = STYLE_UNIT_REL;
	pval.typed_data.value = 3;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "medium", 6))
     {
	pval.typed_data.unit = STYLE_UNIT_REL;
	pval.typed_data.value = 4;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "large", 5))
     {
	pval.typed_data.unit = STYLE_UNIT_REL;
	pval.typed_data.value = 5;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "x-large", 7))
     {
	pval.typed_data.unit = STYLE_UNIT_REL;
	pval.typed_data.value = 6;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "xx-large", 8))
     {
	pval.typed_data.unit = STYLE_UNIT_REL;
	pval.typed_data.value = 7;
	cssRule = SkipWord (cssRule);
     }
   else
     {
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
		   real = FALSE;
		 }
	       else
		 pval.typed_data.value *= 100;
	       pval.typed_data.unit = STYLE_UNIT_PERCENT;
	     }
	 }
     }

   /* install the attribute */
   TtaSetStylePresentation (PRSize, element, tsch, context, pval);
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSFontFamily : parse a CSS font family string   
   we expect the input string describing the attribute to be     
   a common generic font style name                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSFontFamily (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSFontFamily (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
  PresentationValue   font;
  CHAR		      quoteChar;

  font.typed_data.value = 0;
  font.typed_data.unit = STYLE_UNIT_REL;
  font.typed_data.real = FALSE;
  cssRule = TtaSkipBlanks (cssRule);
  if (*cssRule == '"' || *cssRule == '\'')
     {
     quoteChar = *cssRule;
     cssRule++;
     }
  else
     quoteChar = '\0';

  if (!ustrncasecmp (cssRule, "times", 5))
      font.typed_data.value = STYLE_FONT_TIMES;
  else if (!ustrncasecmp (cssRule, "serif", 5))
      font.typed_data.value = STYLE_FONT_TIMES;
  else if (!ustrncasecmp (cssRule, "helvetica", 9) ||
	   !ustrncasecmp (cssRule, "verdana", 7))
      font.typed_data.value = STYLE_FONT_HELVETICA;
  else if (!ustrncasecmp (cssRule, "sans-serif", 10))
      font.typed_data.value = STYLE_FONT_HELVETICA;
  else if (!ustrncasecmp (cssRule, "courier", 7))
      font.typed_data.value = STYLE_FONT_COURIER;
  else if (!ustrncasecmp (cssRule, "monospace", 9))
      font.typed_data.value = STYLE_FONT_COURIER;
  else
    /* unknown font name.  Skip it */
    {
      if (quoteChar)
	 cssRule = SkipQuotedString (cssRule, quoteChar);
      else
         cssRule = SkipWord (cssRule);
      cssRule = TtaSkipBlanks (cssRule);
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
   extra-light, light, demi-light, medium, demi-bold, bold, extra-bold
   or a number encoding for the previous values                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSFontWeight (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSFontWeight (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
   PresentationValue   weight, previous_style;

   weight.typed_data.value = 0;
   weight.typed_data.unit = STYLE_UNIT_REL;
   weight.typed_data.real = FALSE;
   cssRule = TtaSkipBlanks (cssRule);
   if (!ustrncasecmp (cssRule, "100", 3) && !isalpha (cssRule[3]))
     {
	weight.typed_data.value = -3;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "200", 3) && !isalpha (cssRule[3]))
     {
	weight.typed_data.value = -2;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "300", 3) && !isalpha (cssRule[3]))
     {
	weight.typed_data.value = -1;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "normal", 6) ||
	    (!ustrncasecmp (cssRule, "400", 3) && !isalpha (cssRule[3])))
     {
	weight.typed_data.value = 0;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "500", 3) && !isalpha (cssRule[3]))
     {
	weight.typed_data.value = +1;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "600", 3) && !isalpha (cssRule[3]))
     {
	weight.typed_data.value = +2;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "bold", 4) ||
	    (!ustrncasecmp (cssRule, "700", 3) && !isalpha (cssRule[3])))
     {
	weight.typed_data.value = +3;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "800", 3) && !isalpha (cssRule[3]))
     {
	weight.typed_data.value = +4;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "900", 3) && !isalpha (cssRule[3]))
     {
	weight.typed_data.value = +5;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "inherit", 7) ||
	    !ustrncasecmp (cssRule, "bolder", 6) ||
	    !ustrncasecmp (cssRule, "lighter", 7))
     {
     /* not implemented */
     cssRule = SkipWord (cssRule);
     return (cssRule);
     }
   else
     return (cssRule);

   /*
    * Here we have to reduce since font weight is not well supported
    * by the Thot presentation API.
    */
    if (!TtaGetStylePresentation (PRStyle, element, tsch, context, &previous_style))
       {
       if (previous_style.typed_data.value == STYLE_FONT_ITALICS ||
	   previous_style.typed_data.value == STYLE_FONT_BOLDITALICS)
	  if (weight.typed_data.value > 0)
	     weight.typed_data.value = STYLE_FONT_BOLDITALICS;
	  else
	     weight.typed_data.value = STYLE_FONT_ITALICS;
       else if (previous_style.typed_data.value == STYLE_FONT_OBLIQUE ||
	        previous_style.typed_data.value == STYLE_FONT_BOLDOBLIQUE)
	  if (weight.typed_data.value > 0)
	    weight.typed_data.value = STYLE_FONT_BOLDOBLIQUE;
	  else
	    weight.typed_data.value = STYLE_FONT_OBLIQUE;
       else if (previous_style.typed_data.value == STYLE_FONT_ROMAN ||
	        previous_style.typed_data.value == STYLE_FONT_BOLD)
	  if (weight.typed_data.value > 0)
	    weight.typed_data.value = STYLE_FONT_BOLD;
	  else
	    weight.typed_data.value = STYLE_FONT_ROMAN;
       }
   else
       if (weight.typed_data.value > 0)
         weight.typed_data.value = STYLE_FONT_BOLD;
       else
         weight.typed_data.value = STYLE_FONT_ROMAN;

   /* install the new presentation */
    TtaSetStylePresentation (PRStyle, element, tsch, context, weight);
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSFontVariant : parse a CSS font variant string     
   we expect the input string describing the attribute to be     
   normal or small-caps
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSFontVariant (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSFontVariant (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
   PresentationValue   style;

   style.typed_data.value = 0;
   style.typed_data.unit = STYLE_UNIT_REL;
   style.typed_data.real = FALSE;
   cssRule = TtaSkipBlanks (cssRule);
   if (!ustrncasecmp (cssRule, "small-caps", 10))
     {
       /* Not supported yet */
       cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "normal", 6))
     {
       /* Not supported yet */
       cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "inherit", 7))
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSFontStyle (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
   cssRule = TtaSkipBlanks (cssRule);
   if (!ustrncasecmp (cssRule, "italic", 6))
     {
	style.typed_data.value = STYLE_FONT_ITALICS;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "oblique", 7))
     {
	style.typed_data.value = STYLE_FONT_OBLIQUE;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "normal", 6))
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
     {
	PresentationValue   previous_style;

	if (!TtaGetStylePresentation (PRStyle, element, tsch, context, &previous_style))
	  {
	     if (previous_style.typed_data.value == STYLE_FONT_BOLD)
	       {
		  if (style.typed_data.value == STYLE_FONT_ITALICS)
		     style.typed_data.value = STYLE_FONT_BOLDITALICS;
		  if (style.typed_data.value == STYLE_FONT_OBLIQUE)
		     style.typed_data.value = STYLE_FONT_BOLDOBLIQUE;
	       }
	     TtaSetStylePresentation (PRStyle, element, tsch, context, style);
	  }
	else
	  {
	    TtaSetStylePresentation (PRStyle, element, tsch, context, style);
	  }
     }
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSFont (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
  STRING            ptr;

  cssRule = TtaSkipBlanks (cssRule);
  if (!ustrncasecmp (cssRule, "caption", 7))
    ;
  else if (!ustrncasecmp (cssRule, "icon", 4))
    ;
  else if (!ustrncasecmp (cssRule, "menu", 4))
    ;
  else if (!ustrncasecmp (cssRule, "message-box", 11))
    ;
  else if (!ustrncasecmp (cssRule, "small-caption", 13))
    ;
  else if (!ustrncasecmp (cssRule, "status-bar", 10))
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
	    TtaSkipBlanks (cssRule);
	    cssRule = SkipWord (cssRule);
	  }
	cssRule = ParseCSSFontFamily (element, tsch, context, cssRule, css, isHTML);
	cssRule = TtaSkipBlanks (cssRule);
	while (*cssRule != ';' && *cssRule != EOS)
	  {
	    /* now skip remainding info */
	    cssRule++;
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
static STRING       ParseCSSLineSpacing (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSLineSpacing (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
   PresentationValue   lead;

   cssRule = ParseCSSUnit (cssRule, &lead);
   if (lead.typed_data.unit == STYLE_UNIT_INVALID)
     {
       /* invalid line spacing */
	return (cssRule);
     }
   /*
    * install the new presentation.
    */
   TtaSetStylePresentation (PRLineSpacing, element, tsch, context, lead);
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSTextDecoration (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
   PresentationValue   decor;

   decor.typed_data.value = 0;
   decor.typed_data.unit = STYLE_UNIT_REL;
   decor.typed_data.real = FALSE;
   cssRule = TtaSkipBlanks (cssRule);
   if (!ustrncasecmp (cssRule, "underline", ustrlen ("underline")))
     {
	decor.typed_data.value = Underline;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "overline", ustrlen ("overline")))
     {
	decor.typed_data.value = Overline;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "line-through", ustrlen ("line-through")))
     {
	decor.typed_data.value = CrossOut;
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "box", ustrlen ("box")))
     {
       /* the box text-decoration attribute is not yet supported */
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "boxshadow", ustrlen ("boxshadow")))
     {
       /* the boxshadow text-decoration attribute is not yet supported */
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "box3d", ustrlen ("box3d")))
     {
       /* the box3d text-decoration attribute is not yet supported */
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "cartouche", ustrlen ("cartouche")))
     {
	/*the cartouche text-decoration attribute is not yet supported */
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "blink", ustrlen ("blink")))
     {
	/*the blink text-decoration attribute will not be supported */
	cssRule = SkipWord (cssRule);
     }
   else if (!ustrncasecmp (cssRule, "none", ustrlen ("none")))
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
  CHAR                colname[100];
  unsigned short      redval = (unsigned short) -1;
  unsigned short      greenval = 0;	/* composant of each RGB       */
  unsigned short      blueval = 0;	/* default to red if unknown ! */
  unsigned int        i, len;
  int                 r, g, b;
  int                 best = 0;	/* best color in list found */
  boolean             failed;

  cssRule = TtaSkipBlanks (cssRule);
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
  else if (!ustrncasecmp (cssRule, "rgb", 3))
    {
      cssRule += 3;
      cssRule = TtaSkipBlanks (cssRule);
      if (*cssRule == '(')
	{
	  cssRule++;
	  cssRule = TtaSkipBlanks (cssRule);
	  failed = FALSE;
	  if (*cssRule == '%')
	    {
	      /* encoded as rgb(%red,%green,&blue) */
	      sscanf (cssRule, "%%%d", &r);
	      while (*cssRule != EOS && *cssRule != ',')
		cssRule++;
	      cssRule++;
	      sscanf (cssRule, "%%%d", &g);
	      while (*cssRule != EOS && *cssRule != ',')
		cssRule++;
	      cssRule++;
	      sscanf (cssRule, "%%%d", &b);
	      redval = (unsigned short)(r * 255 / 100);
	      greenval = (unsigned short)(g * 255 / 100);
	      blueval = (unsigned short)(b * 255 / 100);
	    }
	  else
	    {
	      /* encoded as rgb(red,green,blue) */
	      sscanf (cssRule, "%d", &r);
	      while (*cssRule != EOS && *cssRule != ',')
		cssRule++;
	      cssRule++;
	      sscanf (cssRule, "%d", &g);
	      while (*cssRule != EOS && *cssRule != ',')
		cssRule++;
	      cssRule++;
	      sscanf (cssRule, "%d", &b);
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
      /* Lookup the color name in Thot color name database */
      if (failed)
	{
	  TtaGiveRGB (colname, &redval, &greenval, &blueval);
	  failed = FALSE;
	}
    }
  
  if (failed)
    val->typed_data.value = 0;
  else
    {
      best = TtaGetThotColor (redval, greenval, blueval);
      val->typed_data.value = best;
    }
  val->typed_data.unit = STYLE_UNIT_REL;
  val->typed_data.real = FALSE;
 return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSHeight : parse a CSS height attribute                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSHeight (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSHeight (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
   cssRule = TtaSkipBlanks (cssRule);

   /* first parse the attribute string */
   if (!ustrcasecmp (cssRule, "auto"))
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSWidth (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
   cssRule = TtaSkipBlanks (cssRule);

   /* first parse the attribute string */
   if (!ustrcasecmp (cssRule, "auto"))
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSMarginTop (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
  PresentationValue   margin;
  
  cssRule = TtaSkipBlanks (cssRule);
  /* first parse the attribute string */
  cssRule = ParseCSSUnit (cssRule, &margin);
  if (margin.typed_data.unit != STYLE_UNIT_INVALID)
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSMarginBottom (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
  PresentationValue   margin;
  
  cssRule = TtaSkipBlanks (cssRule);
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSMarginLeft (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
  PresentationValue   margin;
  
  cssRule = TtaSkipBlanks (cssRule);
  /* first parse the attribute string */
  cssRule = ParseCSSUnit (cssRule, &margin);
  if (margin.typed_data.unit != STYLE_UNIT_INVALID)
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSMarginRight (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
  PresentationValue   margin;
  
  cssRule = TtaSkipBlanks (cssRule);
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSMargin (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
  STRING            ptrT, ptrR, ptrB, ptrL;

  ptrT = TtaSkipBlanks (cssRule);
  /* First parse Margin-Top */
  ptrR = ParseCSSMarginTop (element, tsch, context, ptrT, css, isHTML);
  ptrR = TtaSkipBlanks (ptrR);
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
      ptrB = TtaSkipBlanks (ptrB);
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
	  ptrL = TtaSkipBlanks (ptrL);
	  if (*ptrL == ';' || *ptrL == EOS || *ptrL == ',')
	    {
	      cssRule = ptrL;
	      /* apply the Margin-Right to Margin-Left */
	      ptrL = ParseCSSMarginLeft (element, tsch, context, ptrR, css, isHTML);
	    }
	  else
	    /* parse Margin-Left */
	    cssRule = ParseCSSMarginLeft (element, tsch, context, ptrL, css, isHTML);
	  cssRule = TtaSkipBlanks (cssRule);
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSPaddingTop (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSPaddingRight (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSPaddingBottom (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSPaddingLeft (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSPadding (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSForeground (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
   PresentationValue   best;

   cssRule = ParseCSSColor (cssRule, &best);

   if (best.typed_data.unit == STYLE_UNIT_INVALID)
     {
	return (cssRule);
     }
   /*
    * install the new presentation.
    */
   TtaSetStylePresentation (PRForeground, element, tsch, context, best);
   return (cssRule);
}

/*----------------------------------------------------------------------
   ParseCSSBackgroundColor : parse a CSS background color attribute 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       ParseCSSBackgroundColor (Element element, PSchema tsch,
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSBackgroundColor (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
  PresentationValue     best;
  unsigned int          savedtype = 0;
  boolean               moved;

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
  if (!ustrncasecmp (cssRule, "transparent", ustrlen ("transparent")))
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
   BackgroundImageCallbackPtr callblock = (BackgroundImageCallbackPtr) extra;
   Element             el;
   PSchema             tsch;
   PresentationContext context;
   PresentationValue   image;
   PresentationValue   repeat;
   PresentationValue   value;

   if (callblock == NULL)
     return;
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

   /* Update the rendering */
   TtaUpdateStylePresentation (el, tsch, context);

   TtaFreeMemory (callblock);
}


/*----------------------------------------------------------------------
   UpdateCSSBackgroundImage searches strings url() or url("") within
   the styleString and make it relative to the newpath.
   oldpath = old document path
   newpath = new document path
   imgpath = new image directory
   If the image is not moved, the imgpath has to be NULL else the new
   image url is obtained by concatenation of imgpath and the image name.
   Returns NULL or a new allocated styleString.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING              UpdateCSSBackgroundImage (STRING oldpath, STRING newpath, STRING imgpath, STRING styleString)
#else
STRING              UpdateCSSBackgroundImage (oldpath, newpath, imgpath, styleString)
STRING              oldpath;
STRING              newpath;
STRING              imgpath;
STRING              styleString;
#endif
{
  STRING              b, e, ptr, oldptr, sString;
  CHAR                old_url[MAX_LENGTH];
  CHAR                tempname[MAX_LENGTH];
  CHAR                imgname[MAX_LENGTH];
  STRING              new_url;
  int                 len;

  ptr = NULL;
  sString = styleString;
  b = ustrstr (sString, "url");
  while (b != NULL)
    {
      /* we need to compare this url with the new doc path */
      b += 3;
      b = TtaSkipBlanks (b);
      if (*b == '(')
	{
	  b++;
	  b = TtaSkipBlanks (b);
	  /*** Caution: Strings can either be written with double quotes or
	       with single quotes. Only double quotes are handled here.
	       Escaped quotes are not handled. See function SkipQuotedString */
	  if (*b == '"')
	    {
	      /* search the url end */
	      b++;
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
	      ustrncpy (old_url, b, len);
	      old_url[len] = EOS;
	      /* get the old full image name */
	      NormalizeURL (old_url, 0, tempname, imgname, oldpath);
	      /* build the new full image name */
	      if (imgpath != NULL)
		NormalizeURL (imgname, 0, tempname, imgname, imgpath);
	      new_url = MakeRelativeURL (tempname, newpath);
	      
	      /* generate the new style string */
	      if (ptr != NULL)
		{
		  oldptr = ptr;
		  len = - len + ustrlen (oldptr) + ustrlen (new_url) + 1;
		  ptr = (STRING) TtaGetMemory (len);	  
		  len = (int)(b - oldptr);
		  ustrncpy (ptr, oldptr, len);
		  sString = &ptr[len];
		  /* new name */
		  ustrcpy (sString, new_url);
		  /* following text */
		  ustrcat (sString, e);
		  TtaFreeMemory (oldptr);
		}
	      else
		{
		  len = - len + ustrlen (styleString) + ustrlen (new_url) + 1;
		  ptr = (STRING) TtaGetMemory (len);
		  len = (int)(b - styleString);
		  ustrncpy (ptr, styleString, len);
		  sString = &ptr[len];
		  /* new name */
		  ustrcpy (sString, new_url);
		  /* following text */
		  ustrcat (sString, e);
		}
	      TtaFreeMemory (new_url);
	    }
	  else
	    sString = b;
	}
      else
	sString = b;
      /* next background-image */
      b = ustrstr (sString, "url");      
    }
  return (ptr);
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
  b = ustrstr (styleString, "url");
  if (b != NULL)
    {
      b += 3;
      b = TtaSkipBlanks (b);
      if (*b == '(')
	{
	  b++;
	  b = TtaSkipBlanks (b);
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSBackgroundImage (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
  Element               el;
  GenericContext        gblock;
  PresentationContextBlock  *sblock;
  BackgroundImageCallbackPtr callblock;
  PresentationValue     image, value;
  STRING                url;
  STRING                bg_image;
  CHAR                  saved;
  STRING                base;
  CHAR                  tempname[MAX_LENGTH];
  CHAR                  imgname[MAX_LENGTH];
  unsigned int          savedtype = 0;
  boolean               moved;

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
  cssRule = TtaSkipBlanks (cssRule);
  if (!ustrncasecmp (cssRule, "url", 3))
    {  
      cssRule += 3;
      cssRule = TtaSkipBlanks (cssRule);
      if (*cssRule == '(')
	{
	  cssRule++;
	  cssRule = TtaSkipBlanks (cssRule);
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
	  if (bg_image == NULL || !ustrcasecmp (bg_image,"yes"))
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
		      FetchImage (context->doc, el, tempname, 0,
				  ParseCSSBackgroundImageCallback, callblock);
		    }
		  else
		    FetchImage (context->doc, el, url, 0,
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSBackgroundRepeat (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
  PresentationValue   repeat;
  unsigned int        savedtype = 0;
  boolean             moved;

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
  cssRule = TtaSkipBlanks (cssRule);
  if (!ustrncasecmp (cssRule, "no-repeat", 9))
    repeat.typed_data.value = STYLE_REALSIZE;
  else if (!ustrncasecmp (cssRule, "repeat-y", 8))
    repeat.typed_data.value = STYLE_VREPEAT;
  else if (!ustrncasecmp (cssRule, "repeat-x", 8))
    repeat.typed_data.value = STYLE_HREPEAT;
  else if (!ustrncasecmp (cssRule, "repeat", 6))
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSBackgroundAttachment (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
  unsigned int          savedtype = 0;
  boolean               moved;

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

   cssRule = TtaSkipBlanks (cssRule);
   if (!ustrncasecmp (cssRule, "scroll", 6))
     cssRule = SkipWord (cssRule);
   else if (!ustrncasecmp (cssRule, "fixed", 5))
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSBackgroundPosition (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
  PresentationValue     repeat;
  unsigned int          savedtype = 0;
  boolean               moved;
  boolean               ok;

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

   cssRule = TtaSkipBlanks (cssRule);
   ok = TRUE;
   if (!ustrncasecmp (cssRule, "left", 4))
     cssRule = SkipWord (cssRule);
   else if (!ustrncasecmp (cssRule, "right", 5))
     cssRule = SkipWord (cssRule);
   else if (!ustrncasecmp (cssRule, "center", 6))
     cssRule = SkipWord (cssRule);
   else if (!ustrncasecmp (cssRule, "top", 3))
     cssRule = SkipWord (cssRule);
   else if (!ustrncasecmp (cssRule, "bottom", 6))
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
				 PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static STRING       ParseCSSBackground (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
  STRING            ptr;

  cssRule = TtaSkipBlanks (cssRule);
  while (*cssRule != ';' && *cssRule != EOS && *cssRule != ',')
    {
      /* perhaps a Backgroud Image */
      if (!ustrncasecmp (cssRule, "url", 3))
	cssRule = ParseCSSBackgroundImage (element, tsch, context, cssRule, css, isHTML);
      /* perhaps a Background Attachment */
      else if (!ustrncasecmp (cssRule, "scroll", 6) ||
	       !ustrncasecmp (cssRule, "fixed", 5))
	cssRule = ParseCSSBackgroundAttachment (element, tsch, context, cssRule, css, isHTML);
      /* perhaps a Background Repeat */
      else if (!ustrncasecmp (cssRule, "no-repeat", 9) ||
	       !ustrncasecmp (cssRule, "repeat-y", 8) ||
	       !ustrncasecmp (cssRule, "repeat-x", 8) ||
	       !ustrncasecmp (cssRule, "repeat", 6))
	cssRule = ParseCSSBackgroundRepeat (element, tsch, context, cssRule, css, isHTML);
      /* perhaps a Background Position */
      else if (!ustrncasecmp (cssRule, "left", 4) ||
	       !ustrncasecmp (cssRule, "right", 5) ||
	       !ustrncasecmp (cssRule, "center", 6) ||
	       !ustrncasecmp (cssRule, "top", 3) ||
	       !ustrncasecmp (cssRule, "bottom", 6) ||
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
      cssRule = TtaSkipBlanks (cssRule);
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
   {"list-style", ParseCSSListStyle}
};
#define NB_CSSSTYLEATTRIBUTE (sizeof(CSSProperties) / sizeof(CSSProperty))

/*----------------------------------------------------------------------
   ParseCSSRule : parse a CSS Style string                        
   we expect the input string describing the style to be of the  
   form : PRORPERTY : DESCRIPTION [ ; PROPERTY : DESCRIPTION ] * 
   but tolerate incorrect or incomplete input                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ParseCSSRule (Element element, PSchema tsch, PresentationContext context, STRING cssRule, CSSInfoPtr css, boolean isHTML)
#else
static void         ParseCSSRule (element, tsch, context, cssRule, css, isHTML)
Element             element;
PSchema             tsch;
PresentationContext context;
STRING              cssRule;
CSSInfoPtr          css;
boolean             isHTML;
#endif
{
  STRING              p = NULL;
  int                 lg;
  unsigned int        i, savedtype;
  boolean             found;

  while (*cssRule != EOS)
    {
      cssRule = TtaSkipBlanks (cssRule);
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
	  cssRule = TtaSkipBlanks (cssRule);
	  if (*cssRule == ':')
	    {
	      cssRule++;
	      cssRule = TtaSkipBlanks (cssRule);
	    }
	  /* try to parse the attribute associated to this attribute */
	  if (CSSProperties[i].parsing_function != NULL)
	    p = CSSProperties[i].parsing_function (element, tsch, context, cssRule, css, isHTML);
	  
	  /* Update the rendering */
	  TtaUpdateStylePresentation (element, tsch, context);
	  /* In case of specific rules on BODY element */
	  if (context->type == HTML_EL_BODY && isHTML)
	    {
	      if (element)
		TtaUpdateStylePresentation (TtaGetMainRoot (context->doc), NULL, context);
	      else
		{
		  savedtype = context->type;
		  context->type = HTML_EL_HTML;
		  TtaUpdateStylePresentation (NULL, tsch, context);
		  context->type = savedtype;
		}
	    }
	    
	  /* update index and skip the ";" separator if present */
	  cssRule = p;
	}
      /* next property */
      cssRule = TtaSkipBlanks (cssRule);
      if (*cssRule == ',' || *cssRule == ';')
	{
	  cssRule++;
	  cssRule = TtaSkipBlanks (cssRule);
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
void                ParseHTMLSpecificStyle (Element el, STRING cssRule, Document doc, boolean destroy)
#else
void                ParseHTMLSpecificStyle (el, cssRule, doc, destroy)
Element             elem;
STRING              cssRule;
Document            doc;
boolean             destroy;
#endif
{
   PresentationContext context;
   ElementType         elType;
   boolean             isHTML;

   /*  A rule applying to BODY is really meant to address HTML */
   elType = TtaGetElementType (el);
   isHTML = (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0);
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
   ParseHTMLGenericSelector : Create a generic context for a given 
   selector string. If the selector is made of multiple comma- 
   separated selector items, it parses them one at a time and  
   return the end of the selector string to be handled or NULL 

   Need to add multi-DTD support here !!!!
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING  ParseHTMLGenericSelector (STRING selector, STRING cssRule,
			   GenericContext ctxt, Document doc, CSSInfoPtr css)
#else
static STRING  ParseHTMLGenericSelector (selector, cssRule, ctxt, doc, css)
STRING         selector;
STRING         cssRule;
GenericContext  ctxt;
Document        doc;
CSSInfoPtr      css;
#endif
{
  ElementType         elType;
  PSchema             tsch;
  CHAR                sel[150];
  CHAR                class[150];
  CHAR                pseudoclass[150];
  CHAR                id[150];
  CHAR                attrelemname[150];
  STRING              deb;
  STRING              elem, structName;
  STRING              cur;
  STRING              ancestors[MAX_ANCESTORS];
  int                 i, j;
  boolean             isHTML, classOnly;

  sel[0] = EOS;
  class[0] = EOS;
  pseudoclass[0] = EOS;
  id[0] = EOS;
  classOnly = FALSE;
  attrelemname[0] = EOS;
  deb = cur = elem = &sel[0];
  for (i = 0; i < MAX_ANCESTORS; i++)
    {
      ancestors[i] = NULL;
      ctxt->ancestors[i] = 0;
      ctxt->ancestors_nb[i] = 0;
    }

  /* first format the first selector item, uniformizing blanks */
  selector = TtaSkipBlanks (selector);
  while (1)
    {
      cur = deb;
      /* put one word in the sel buffer */
      while (*selector != EOS && *selector != ',' &&
	     *selector != '.' && *selector != ':' &&
	     *selector != '#' && !TtaIsBlank (selector))
	*cur++ = *selector++;
      *cur++ = EOS;
      
      /* now deb points to the parsed type and cur to the next chain to be parsed */
      elem = deb;
      if (*selector == ':' || *selector == '.' || *selector == '#')
	/* keep the name as attrelemname */
	strcpy (attrelemname, elem);
      deb = cur;

      if (*selector == '.')
	{
	  /* read the class id : only one allowed by selector */
	  class[0] = EOS;
	  cur = class;
	  classOnly = (elem == NULL || *elem == EOS);
	  selector++;
	  while (*selector != EOS && *selector != ',' &&
		 *selector != '.' && *selector != ':' &&
		 !TtaIsBlank (selector))
	    *cur++ = *selector++;
	  *cur++ = EOS;
	}
      else if (*selector == ':')
	{
	  /* read the pseudoclass id : only one allowed by selector */
	  pseudoclass[0] = EOS;
	  cur = pseudoclass;
	  selector++;
	  while (*selector != EOS && *selector != ',' &&
		 *selector != '.' && *selector != ':' &&
		 !TtaIsBlank (selector))
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
	  while (*selector != EOS && *selector != ',' &&
		 *selector != '.' && *selector != ':' &&
		 !TtaIsBlank (selector))
	    *cur++ = *selector++;
	  *cur++ = EOS;
	  cur = deb;
	}
      else if (TtaIsBlank (selector))
	{
	  selector = TtaSkipBlanks (selector);
	  /* Thot can not take class and pseudoclass into account for
	     ancestors. Ignore this selector */
	  class[0] = EOS;
	  pseudoclass[0] = EOS;
	  id[0] = EOS;
	  if (attrelemname[0] != EOS)
	     {
	       ancestors[0] = NULL;
	       while (*selector != EOS && *selector != ',')
		 selector++;
	       break;
	     }
	}

      /* store elem in the list if the string is non-empty */
      if (*elem != EOS)
	{
	  /* shifts the list to make room for the new elem */
	  for (i = MAX_ANCESTORS - 1; i > 0; i--)
	    if (ancestors[i - 1] != NULL)
	      ancestors[i] = ancestors[i - 1];
	  /* store the new elem */
	  ancestors[0] = elem;
	}

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
  if (elem == NULL || elem[0] == EOS)
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
    }
  
  GIType (elem, &elType, doc);
  ctxt->type = elType.ElTypeNum;
  ctxt->schema = elType.ElSSchema;
  if (elType.ElSSchema == NULL)
    ctxt->schema = TtaGetDocumentSSchema (doc);
  isHTML = (ustrcmp(TtaGetSSchemaName (ctxt->schema), "HTML") == 0);
  tsch = GetPExtension (doc, ctxt->schema, css);
  structName = TtaGetSSchemaName (ctxt->schema);
  if (ctxt->type == 0 && ctxt->attr == 0 &&
      ctxt->attrval == 0 && ctxt->classattr == 0)
    {
      ctxt->class = elem;
      ctxt->classattr = HTML_ATTR_Class;
    }
  
  if (classOnly)
    i = 0;
  else
    i = 1;
  while (i < MAX_ANCESTORS && ancestors[i] != NULL)
    {
      GIType (ancestors[i], &elType, doc);
      if (elType.ElTypeNum != 0)
	{
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
      i++;
    }

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
static void         ParseStyleDeclaration (Element el, STRING cssRule, Document doc, CSSInfoPtr css, boolean destroy)
#else
static void         ParseStyleDeclaration (el, cssRule, doc, css, destroy)
Element             el;
STRING              cssRule;
Document            doc;
CSSInfoPtr          css;
boolean             destroy;
#endif
{
  GenericContext      ctxt;
  STRING              decl_end;
  STRING              sel_end;
  STRING              selector;
  CHAR                saved1;
  CHAR                saved2;

  /* separate the selectors string */
  cssRule = TtaSkipBlanks (cssRule);
  decl_end = cssRule;
  while ((*decl_end != EOS) && (*decl_end != '{'))
    decl_end++;
  if (*decl_end == EOS)
    return;
  /* verify and clean the selector string */
  sel_end = decl_end - 1;
  while (*sel_end == SPACE || *sel_end == '\b' ||
	 *sel_end == EOL || *sel_end == '\r')
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
    selector = ParseHTMLGenericSelector (selector, cssRule, ctxt, doc, css);
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
   EvaluateClassContext : gives a score for an element in a tree   
   in function of a selector. Three argument enter in the          
   evaluation process :                                            
   - the class name associated to the element                    
   - the selector string associated to the rule                  
   - the element and it's place in the tree                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 EvaluateClassContext (Element el, STRING class, STRING selector, Document doc)
#else
int                 EvaluateClassContext (el, class, selector, doc)
Element             el;
STRING              class;
STRING              selector;
Document            doc;
#endif
{
  Element             father;
  STRING              elHtmlName;
  STRING              end_str;
  STRING              sel = selector;
  STRING              names[MAX_DEEP];
  int                 result = 0;

  elHtmlName = GetCSSName (el, doc);
  GetCSSNames (el, doc, names, MAX_DEEP);

  /*
   * look for a selector (ELEM)
   */
  selector = TtaSkipBlanks (selector);
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
      if (!ustrcasecmp (class, elHtmlName))
	result = 1000;
      else if (!ustrcasecmp (class, selector))
	result = 100;
    }
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
int                 EvaluateClassSelector (Element el, STRING class, STRING selector, Document doc)
#else
int                 EvaluateClassSelector (el, class, selector, doc)
Element             el;
STRING              class;
STRING              selector;
Document            doc;
#endif
{
   int                 l = ustrlen (class);
   int                 L = ustrlen (selector);
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
      if ((*selector == *class) && (!ustrncmp (class, selector, l)))
	 return (val = ((l * 1000) / L));
      else
	 selector++;

   return (val);
}

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
   CHAR             name[200];
   STRING           cur = &name[0], first; 
   CHAR             save;
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
	cur = TtaSkipBlanks (cur);
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
   CHAR             css_command[100];

   sprintf (css_command, "background-color: %s", color);
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
   CHAR                css_command[400];

   /******* check buffer overflow ********/
   sprintf (css_command, "background-image: url(%s); background-repeat: ", image);
   if (repeat == STYLE_REPEAT)
     ustrcat (css_command, "repeat");
   else if (repeat == STYLE_HREPEAT)
     ustrcat (css_command, "repeat-x");
   else if (repeat == STYLE_VREPEAT)
     ustrcat (css_command, "repeat-y");
   else
     ustrcat (css_command, "no-repeat");
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
   CHAR             css_command[100];

   sprintf (css_command, "color: %s", color);
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
   CHAR             css_command[100];

   sprintf (css_command, "background: xx");
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
   CHAR             css_command[1000];

   sprintf (css_command, "background-image: url(xx); background-repeat: repeat");
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
   CHAR             css_command[100];

   sprintf (css_command, "color: xx");
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
   CHAR                css_command[100];

   sprintf (css_command, "A:link { color : %s }", color);
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
   CHAR                css_command[100];

   sprintf (css_command, "A:active { color : %s }", color);
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
   CHAR                css_command[100];

   sprintf (css_command, "A:visited { color : %s }", color);
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
   CHAR                css_command[100];

   sprintf (css_command, "A:link { color : red }");
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
   CHAR                css_command[100];

   sprintf (css_command, "A:active { color : red }");
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
   CHAR                css_command[100];

   sprintf (css_command, "A:visited { color : red }");
   ApplyCSSRules (NULL, css_command, doc, TRUE);
}

/*----------------------------------------------------------------------
  ApplyCSSRules: parse an CSS Style description stored in the
  header of a HTML document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ApplyCSSRules (Element el, STRING cssRule, Document doc, boolean destroy)
#else
void                ApplyCSSRules (el, cssRule, doc, destroy)
Element             el;
STRING              cssRule;
Document            doc;
boolean             destroy;
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
   to work on the given buffer or call GetNextInputChar to read the parsed
   file.
   Parameter withUndo indicates whether the changes made in the document
   structure and content have to be registered in the Undo queue or not
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CHAR                ReadCSSRules (Document doc, Document docRef, CSSInfoPtr css, STRING buffer, boolean withUndo)
#else
CHAR                ReadCSSRules (doc, docRef, css, buffer, withUndo)
Document            doc;
Document            docRef;
CSSInfoPtr          css;
STRING              buffer;
boolean             withUndo;
#endif
{
  Attribute           attr;
  AttributeType       attrType;
  ElementType         elType;
  Element             parent, el, title, createdEl;
  CHAR                c;
  STRING              cssRule, base;
  STRING              schemaName;
  int                 lg, index;
  int                 CSSindex;
  int                 CSScomment;
  int                 import;
  int                 openRule;
  boolean             HTMLcomment;
  boolean             toParse, eof;
  boolean             ignoreMedia;
  boolean             noRule, CSSparsing;

  createdEl = NULL;
  CSScomment = MAX_CSS_LENGTH;
  HTMLcomment = FALSE;
  CSSindex = 0;
  CSSparsing = TRUE;
  toParse = FALSE;
  noRule = FALSE;
  ignoreMedia = FALSE;
  import = MAX_CSS_LENGTH;
  eof = FALSE;
  openRule = 0;
  c = SPACE;
  index = 0;
  if (doc != 0)
    {
      parent = TtaGetMainRoot (doc);
      elType = TtaGetElementType (parent);
      schemaName = TtaGetSSchemaName (elType.ElSSchema);
      if (!ustrcmp (schemaName, "HTML"))
	{
	  /* it's the STYLE section of the HTML document */
	  elType.ElTypeNum = HTML_EL_HEAD;
	  el = TtaSearchTypedElement (elType, SearchForward, parent);
	  if (el == NULL)
	    {
	      el = TtaNewTree (doc, elType, "");
	      TtaInsertFirstChild (&el, parent, doc);
	      parent = el;
	      if (withUndo && !createdEl)
		 /* remember the element to be registered in the Undo queue */
		 createdEl = el;
	    }
	  elType.ElTypeNum = HTML_EL_STYLE_;
	  parent = el;
	  el = TtaSearchTypedElement (elType, SearchForward, parent);
	  /* if the Style element doesn't exist we create it now */
	  if (el == NULL)
	    {
	      el = TtaNewTree (doc, elType, "");
	      /* insert the new style element after the title if it exists */
	      elType.ElTypeNum = HTML_EL_TITLE;
	      title = TtaSearchTypedElement (elType, SearchForward, parent);
	      if (title != NULL)
		TtaInsertSibling (el, title, FALSE, doc);
	      else
		TtaInsertFirstChild (&el, parent, doc);
	      attrType.AttrSSchema = elType.ElSSchema;
	      attrType.AttrTypeNum = HTML_ATTR_Notation;
	      attr = TtaNewAttribute (attrType);
	      TtaAttachAttribute (el, attr, doc);
	      TtaSetAttributeText (attr, "text/css", el, doc);
	      if (withUndo && !createdEl)
		 /* remember the element to be registered in the Undo queue */
		 createdEl = el;
	    }
	  /* if the Text element doesn't exist we create it now */
	  parent = el;
	  el = TtaGetLastChild (parent);
	  if (el == NULL)
	    {
	      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
	      el = TtaNewTree (doc, elType, "");
	      TtaInsertFirstChild (&el, parent, doc);
	      if (withUndo && !createdEl)
		 /* remember the element to be registered in the Undo queue */
		 createdEl = el;
	    }
	  if (css == NULL)
	    css = SearchCSS (doc, NULL);
	  if (css == NULL)
	    css = AddCSS (doc, docRef, CSS_DOCUMENT_STYLE, NULL, NULL);
	}
      else if (!ustrcmp (schemaName, "TextFile"))
	/* it's a CSS document */
	el = TtaGetLastChild (parent);
      else
	/* it's an unknown document */
	return (c);
    }
  else
      el = NULL;

  while (CSSindex < MAX_CSS_LENGTH && c != EOS && CSSparsing && !eof)
    {
      if (buffer != NULL)
	{
	  c = buffer[index++];
	  eof = (c == EOS);
	}
      else
	c = GetNextInputChar (&eof);
      CSSbuffer[CSSindex] = c;
      switch (c)
	{
	case '@':
	  /* perhaps an import primitive */
	  import = CSSindex;
	  break;
	case ';':
	  if (import != MAX_CSS_LENGTH)
	    {
	      if (ustrncasecmp (&CSSbuffer[import+1], "import", 6))
		/* it's not an import */
		import = MAX_CSS_LENGTH;
	      /* save the text */
	      noRule = TRUE;
	    }
	  break;
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
	      CSSparsing = FALSE;
	      CSSindex -= 2; /* remove </ from the CSS string */
	    }	    
	  break;
	case '<':
	  if (buffer != NULL)
	    {
	      c = buffer[index++];
	      eof = (c == EOS);
	    }
	  else
	    c = GetNextInputChar (&eof);
	  if (c == '!')
	    {
	      /* CSS within an HTML comment */
	      HTMLcomment = TRUE;
	      CSSindex++;
	      CSSbuffer[CSSindex] = c;
	    }
	  else if (c == '/' && CSScomment == MAX_CSS_LENGTH)
	    {
	      CSSindex--;
	      /* Ok we consider this as a closing tag ! */
	      CSSparsing = FALSE;
	    }
	  else if (c == EOS)
	    CSSindex++;
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
	      base = ustrstr (&CSSbuffer[import], "screen");
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
	  else
	    toParse = TRUE;
	  break;
        default:
	  break;
	}
      if (c != EOS)
	CSSindex++;
      if  (CSSindex >= MAX_CSS_LENGTH || !CSSparsing || toParse || noRule)
	{
	  CSSbuffer[CSSindex] = EOS;
	  /* parse a not empty string */
	  if (CSSindex > 0)
	    {
	      if (el != NULL)
		{
	          if (withUndo && !createdEl)
		     /* no HEAD or STYLE element has been created above.
		     Register the previous value of the STYLE element
		     in the Undo queue */
		     TtaRegisterElementReplace (el, doc);
		  /* add information in the document tree */
		  lg = TtaGetTextLength (el);
		  TtaInsertTextContent (el, lg, CSSbuffer, doc);
	          if (withUndo && createdEl)
		     /* a HEAD or STYLE element has been created above, it is
		     complete now. Register it in the Undo queue */
		     TtaRegisterElementCreate (createdEl, doc);
		}
	      /* apply CSS rule if it's not just a saving of text */
	      if (!noRule && !ignoreMedia)
		ParseStyleDeclaration (el, CSSbuffer, docRef, css, FALSE);
	      else if (import != MAX_CSS_LENGTH &&
		       !ustrncasecmp (&CSSbuffer[import+1], "import", 6))
		{
		  /* import section */
		  cssRule = &CSSbuffer[import+7];
		  cssRule = TtaSkipBlanks (cssRule);
		  if (!ustrncasecmp (cssRule, "url", 3))
		    {
		      cssRule += 3;
		      cssRule = TtaSkipBlanks (cssRule);
		      if (*cssRule == '(')
			{
			  cssRule++;
			  cssRule = TtaSkipBlanks (cssRule);
			  base = cssRule;
			  while (*cssRule != EOS && *cssRule != ')')
			    cssRule++;
			  *cssRule = EOS;
			  LoadStyleSheet (base, docRef, css);
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
		      LoadStyleSheet (base, docRef, css);
		    }
		  import = MAX_CSS_LENGTH;
		}
	    }
	  toParse = FALSE;
	  noRule = FALSE;
	  CSSindex = 0;
	}
    }
  return (c);
}
