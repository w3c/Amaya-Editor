/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/* Included headerfiles */
#define EXPORT extern
#include "amaya.h"
#include "css.h"

/*----------------------------------------------------------------------

             GESTION des styles : suivant CSS de Hakon Lie


 Postit :

[X] Faire la coupure pour element de texte

[X] maintien de l'attribut style .

[X] probleme d'attributs class qui entrent en conflit avec
    de la presentation.
    http://praslin/tests/bug.html
 R: oubli de desactivage checking en sortie des parsing de classe
    si non trouvee !

[X] Gestion de l'export des couleurs sous forme hexa

[ ] Parsing de font: ..... attribut de style condense

[ ] Gestion des boites et du positionnement

[X] gestion de style contextuels

[ ] chargement des background par include ...

[X] chargement de feuille de style externe (c.f. css.c)

  ----------------------------------------------------------------------*/

#include "css_f.h"
#include "html2thot_f.h"
#include "HTMLstyle_f.h"

#ifdef AMAYA_DEBUG
#define MSG(msg) fprintf(stderr,msg)
#else
static char        *last_message = NULL;

#define MSG(msg) last_message = msg
#endif


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

/*----------------------------------------------------------------------
   
   COLOR CONVERSION FUNCTIONS                        
   
  ----------------------------------------------------------------------*/

#include "HTMLstyleColor.h"

/*----------------------------------------------------------------------
   
   PARSER FRONT-END                                  
   
  ----------------------------------------------------------------------*/

/*
 * CSSparser :  is the front-end function called by the HTML parser
 *      when detecting a <STYLE TYPE="text/css"> indicating it's the
 *      beginning of a CSS fragment. readfunc is a function used to
 *      read one character at a time from the input stream.
 *
 *      The CSS parser has to handle <!-- ... --> constructs used to
 *      prevent prehistoric browser from displaying the CSS as a text
 *      content. It will stop on any sequence "<x" where x is different
 *      from ! and will return x as to the caller. Theorically x should
 *      be equal to / for the </STYLE> end of style marker but who knows !
 */

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
		    /* skip the -- sequence, basically -(-(1)) == 1 */
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

/*----------------------------------------------------------------------
   
   PARSING DEFINITIONS                               
   
  ----------------------------------------------------------------------*/

/*
 * This flag is used to switch the parser to a destructive mode where
 * instead of adding the corresponding style, the rule are deleted.
 * Manipulate with care !!!
 */

static boolean         HTMLStyleParserDestructiveMode = False;

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

#ifdef __STDC__
static char        *ParseHTML3StyleBackground (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleForeground (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleFont (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleFontSize (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleFontWeight (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleFontStyle (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleFontFamily (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleTextDecoration (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleHeight (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleWidth (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleMarginTop (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleMarginBottom (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleBgBlendDir (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleBgPosition (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleBgStyle (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleBorderColor (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleBorderColorInternal (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleBorderStyle (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleBorderStyleInternal (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleBorderWidth (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleBorderWidthInternal (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleClear (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleDisplay (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleFloat (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleLetterSpacing (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleLineSpacing (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleListStyle (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleMagnification (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleMarginLeft (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleMarginRight (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleMargin (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StylePack (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StylePadding (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleTextAlign (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleTextIndent (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleTextTransform (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleVerticalAlign (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleWhiteSpace (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleWordSpacing (PresentationTarget target,
				PresentationContext context, char *attrstr);
static char        *ParseHTML3StyleTest (PresentationTarget target,
				PresentationContext context, char *attrstr);

#else
static char        *ParseHTML3StyleBackground ();
static char        *ParseHTML3StyleForeground ();
static char        *ParseHTML3StyleFont ();
static char        *ParseHTML3StyleFontSize ();
static char        *ParseHTML3StyleFontWeight ();
static char        *ParseHTML3StyleFontStyle ();
static char        *ParseHTML3StyleFontFamily ();
static char        *ParseHTML3StyleTextDecoration ();
static char        *ParseHTML3StyleHeight ();
static char        *ParseHTML3StyleWidth ();
static char        *ParseHTML3StyleMarginTop ();
static char        *ParseHTML3StyleMarginBottom ();
static char        *ParseHTML3StyleBgBlendDir ();
static char        *ParseHTML3StyleBgPosition ();
static char        *ParseHTML3StyleBgStyle ();
static char        *ParseHTML3StyleBorderColor ();
static char        *ParseHTML3StyleBorderColorInternal ();
static char        *ParseHTML3StyleBorderStyle ();
static char        *ParseHTML3StyleBorderStyleInternal ();
static char        *ParseHTML3StyleBorderWidth ();
static char        *ParseHTML3StyleBorderWidthInternal ();
static char        *ParseHTML3StyleClear ();
static char        *ParseHTML3StyleDisplay ();
static char        *ParseHTML3StyleFloat ();
static char        *ParseHTML3StyleLetterSpacing ();
static char        *ParseHTML3StyleListStyle ();
static char        *ParseHTML3StyleLineSpacing ();
static char        *ParseHTML3StyleMagnification ();
static char        *ParseHTML3StyleMarginLeft ();
static char        *ParseHTML3StyleMarginRight ();
static char        *ParseHTML3StyleMargin ();
static char        *ParseHTML3StylePack ();
static char        *ParseHTML3StylePadding ();
static char        *ParseHTML3StyleTextAlign ();
static char        *ParseHTML3StyleTextIndent ();
static char        *ParseHTML3StyleTextTransform ();
static char        *ParseHTML3StyleVerticalAlign ();
static char        *ParseHTML3StyleWhiteSpace ();
static char        *ParseHTML3StyleWordSpacing ();

#endif
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
   {"align", ParseHTML3StyleTextAlign},
   {"background", ParseHTML3StyleBackground},
   {"bg-blend-direction", ParseHTML3StyleBgBlendDir},
   {"bg-position", ParseHTML3StyleBgPosition},
   {"bg-style", ParseHTML3StyleBgStyle},
   {"border-color", ParseHTML3StyleBorderColor},
   {"border-color-internal", ParseHTML3StyleBorderColorInternal},
   {"border-style", ParseHTML3StyleBorderStyle},
   {"border-style-internal", ParseHTML3StyleBorderStyleInternal},
   {"border-width", ParseHTML3StyleBorderWidth},
   {"border-width-internal", ParseHTML3StyleBorderWidthInternal},
   {"clear", ParseHTML3StyleClear},
   {"color", ParseHTML3StyleForeground},
   {"display", ParseHTML3StyleDisplay},
   {"float", ParseHTML3StyleFloat},
   {"font-size", ParseHTML3StyleFontSize},
   {"font-weight", ParseHTML3StyleFontWeight},
   {"font-style", ParseHTML3StyleFontStyle},
   {"font-family", ParseHTML3StyleFontFamily},
   {"font-leading", ParseHTML3StyleLineSpacing},
   {"font", ParseHTML3StyleFont},
   {"height", ParseHTML3StyleHeight},
   {"letter-spacing", ParseHTML3StyleLetterSpacing},
   {"line-height", ParseHTML3StyleLineSpacing},
   {"list-style", ParseHTML3StyleListStyle},
   {"magnification", ParseHTML3StyleMagnification},
   {"margin-bottom", ParseHTML3StyleMarginBottom},
   {"margin-left", ParseHTML3StyleMarginLeft},
   {"margin-right", ParseHTML3StyleMarginRight},
   {"margin-top", ParseHTML3StyleMarginTop},
   {"margin", ParseHTML3StyleMargin},
   {"pack", ParseHTML3StylePack},
   {"padding", ParseHTML3StylePadding},
   {"test", ParseHTML3StyleTest},
   {"text-align", ParseHTML3StyleTextAlign},
   {"text-decoration", ParseHTML3StyleTextDecoration},
   {"text-indent", ParseHTML3StyleTextIndent},
   {"text-transform", ParseHTML3StyleTextTransform},
   {"vertical-align", ParseHTML3StyleVerticalAlign},
   {"width", ParseHTML3StyleWidth},
   {"white-space", ParseHTML3StyleWhiteSpace},
   {"word-spacing", ParseHTML3StyleWordSpacing},
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
#define TODO { fprintf(stderr,"code incomplete file %s line %d\n",\
                       __FILE__,__LINE__); };
#define MSG(msg) fprintf(stderr,msg)
#else
	/*static char        *last_message = NULL; */

#define TODO
	/*#define MSG(msg) last_message = msg */
#endif


/*----------------------------------------------------------------------
   
   UNITS CONVERSION FUNCTIONS                        
   
  ----------------------------------------------------------------------*/

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

   pval->unit = DRIVERP_UNIT_REL;

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
	pval->unit = DRIVERP_UNIT_INVALID;
	pval->value = 0;
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
	     pval->unit = HTML3UnitNames[uni].unit;
	     if (real)
	       {
		  DRIVERP_UNIT_SET_FLOAT (pval->unit);
		  if (minus)
		     pval->value = -(f * 1000 + val);
		  else
		     pval->value = f * 1000 + val;
	       }
	     else
	       {
		  if (minus)
		     pval->value = -val;
		  else
		     pval->value = val;
	       }
	     return (attrstr + strlen (HTML3UnitNames[uni].sign));
	  }
     }

   /*
    * not in the list of predefined units.
    */
   pval->unit = DRIVERP_UNIT_REL;
   if (minus)
      pval->value = -val;
   else
      pval->value = val;
   return (attrstr);
}

/*----------------------------------------------------------------------
   
   PARSING FUNCTIONS                                 
   
  ----------------------------------------------------------------------*/

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
   if (((attrstr[0] == 'u') || (attrstr[0] = 'U')) &&
       ((attrstr[0] == 'r') || (attrstr[0] = 'R')) &&
       ((attrstr[0] == 'l') || (attrstr[0] = 'L')))
     {
	char               *base;

	attrstr += 3;
	SKIP_BLANK (attrstr);
	if (*attrstr != '(')
	   goto not_url;
	attrstr++;
	SKIP_BLANK (attrstr);
	base = attrstr;
	while ((*attrstr != EOS) && (!IS_BLANK (attrstr)) &&
	       (*attrstr != ')'))
	   attrstr++;

	if (url != NULL)
	  {
	     char                sauve;

	     sauve = *attrstr;
	     *attrstr = EOS;
	     *url = TtaStrdup (base);
	     *attrstr = sauve;
	  }
	attrstr++;
	return (attrstr);
     }
 not_url:
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
int                 GetHTML3Names (Element elem, Document doc, char **lst, int max)
#else
int                 GetHTML3Names (elem, doc, lst, max)
Element             elem;
Document            doc;
char              **lst;
int                 max;

#endif
{
   char               *res;
   int                 deep;
   Element             father = elem;

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

/*----------------------------------------------------------------------
   
   GENERIC FUNCTIONS FOR CONVERTING BEETWEEN                 
   PRESENTATION ATTRIBUTES AND THE CORRESPONDING STYLE            
   
  ----------------------------------------------------------------------*/

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
   PresentationValue   unused =
   {0, 0};

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
     }
}

/*----------------------------------------------------------------------
   GetHTML3StyleString : return a string corresponding to the CSS    
   description of the presentation attribute applied to a       
   element                                                      
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
   PRule               rule = NULL;
   int                 type;
   SpecificValue       pval;
   int                 free;
   unsigned short      red, green, blue;
   int                 nb_rules;
   char                mybuf[100];

/*******
    AttributeType   atType;
    Attribute       at;
 *******/
   ElementType         elType;
   SpecificContextBlock block;
   PresentationContext context;

   if ((buf == NULL) || (len == NULL) || (*len <= 0))
      return;

   free = *len - 1;
   buf[0] = 0;

/******
    atType.AttrSSchema = TtaGetDocumentSSchema(doc);
    atType.AttrTypeNum = HTML_ATTR_Class;
    at = TtaGetAttribute(elem,atType);
    if (at) {
        *len = 0;
        return;
    }
    atType.AttrSSchema = TtaGetDocumentSSchema(doc);
    atType.AttrTypeNum = HTML_ATTR_ImplicitClass;
    at = TtaGetAttribute(elem,atType);
    if (at) {
        *len = 0;
        return;
    }
 ******/

   elType = TtaGetElementType (elem);

   /*
    * create the context of the Specific presentation driver.
    */
   context = (PresentationContext) & block;
   block.drv = &SpecificStrategy;
   block.doc = doc;
   block.schema = TtaGetDocumentSSchema (doc);

   nb_rules = 0;

   TtaNextPRule (elem, &rule);
   do
     {
	mybuf[0] = 0;
	if (rule)
	  {
	     type = TtaGetPRuleType (rule);
	     switch (type)
		   {
	    /******
                case PRHeight :
                    if ((context->drv->GetHeight == NULL) ||
		         context->drv->GetHeight(elem,context,&pval))
		        break;
                    sprintf(mybuf,"height : %dpt",pval.value);
                    break;
                case PRWidth :
                    if ((context->drv->GetWidth == NULL) ||
		         context->drv->GetWidth(elem,context,&pval))
		        break;
                    sprintf(mybuf,"width : %dpt",pval.value);
                    break;
	     *****/
		      case PRForeground:
			 if ((context->drv->GetForegroundColor == NULL) ||
			     context->drv->GetForegroundColor (elem, context, &pval))
			    break;
			 TtaGiveThotRGB (pval.value, &red, &green, &blue);
			 sprintf (mybuf, "color : #%02X%02X%02X", red, green, blue);
			 break;
		      case PRBackground:
			 if ((context->drv->GetBackgroundColor == NULL) ||
			     context->drv->GetBackgroundColor (elem, context, &pval))
			    break;
			 TtaGiveThotRGB (pval.value, &red, &green, &blue);
			 sprintf (mybuf, "background : #%02X%02X%02X", red, green, blue);
			 type = elType.ElTypeNum;
			 if ((type == HTML_EL_HTML) || (type == HTML_EL_BODY) ||
			     (type == HTML_EL_HEAD))
			   {
			      TtaSetViewBackgroundColor (doc, 1, pval.value);
			   }
			 break;
		      case PRSize:
			 if ((context->drv->GetFontSize == NULL) ||
			   context->drv->GetFontSize (elem, context, &pval))
			    break;
			 sprintf (mybuf, "font-size : %dpt", pval.value);
			 break;
		      case PRStyle:
			 if ((context->drv->GetFontStyle == NULL) ||
			  context->drv->GetFontStyle (elem, context, &pval))
			    break;
			 switch (pval.value)
			       {
				  case DRIVERP_FONT_BOLD:
				     sprintf (mybuf, "font-weight : bold");
				     break;
				  case DRIVERP_FONT_ITALICS:
				     sprintf (mybuf, "font-style : italic");
				     break;
				  case DRIVERP_FONT_ROMAN:
				     sprintf (mybuf, "font-style : roman");
				     break;
				  case DRIVERP_FONT_BOLDITALICS:
				     sprintf (mybuf, "font-weight : bold, font-style : italic");
				     break;
				  case DRIVERP_FONT_OBLIQUE:
				     sprintf (mybuf, "font-style : oblique");
				     break;
				  case DRIVERP_FONT_BOLDOBLIQUE:
				     sprintf (mybuf, "font-weight : bold, font-style : oblique");
				     break;
			       }
			 break;
		      case PRFont:
			 if ((context->drv->GetFontFamily == NULL) ||
			 context->drv->GetFontFamily (elem, context, &pval))
			    break;
			 switch (pval.value)
			       {
				  case DRIVERP_FONT_TIMES:
				     sprintf (mybuf, "font-family : times");
				     break;
				  case DRIVERP_FONT_HELVETICA:
				     sprintf (mybuf, "font-family : helvetica");
				     break;
				  case DRIVERP_FONT_COURIER:
				     sprintf (mybuf, "font-family : courier");
				     break;
			       }
			 break;
		      case PRUnderline:
			 if ((context->drv->GetTextUnderlining == NULL) ||
			     context->drv->GetTextUnderlining (elem, context, &pval))
			    break;
			 switch (pval.value)
			       {
				  case DRIVERP_UNDERLINE:
				     sprintf (mybuf, "text-decoration : underline");
				     break;
				  case DRIVERP_OVERLINE:
				     sprintf (mybuf, "text-decoration : overline");
				     break;
				  case DRIVERP_CROSSOUT:
				     sprintf (mybuf, "text-decoration : line-through");
				     break;
			       }
			 break;
		      case PRLineSpacing:
			 if ((context->drv->GetLineSpacing == NULL) ||
			 context->drv->GetLineSpacing (elem, context, &pval))
			    break;
			 sprintf (mybuf, "line-height : %dpt", pval.value);
			 break;
		      case PRAdjust:
			 if ((context->drv->GetAlignment == NULL) ||
			  context->drv->GetAlignment (elem, context, &pval))
			    break;
			 switch (pval.value)
			       {
				  case DRIVERP_ADJUSTLEFT:
				     sprintf (mybuf, "text-align : left");
				     break;
				  case DRIVERP_ADJUSTRIGHT:
				     sprintf (mybuf, "text-align : right");
				     break;
				  case DRIVERP_ADJUSTCENTERED:
				     sprintf (mybuf, "text-align : center");
				     break;
				  case DRIVERP_ADJUSTLEFTWITHDOTS:
				     sprintf (mybuf, "text-align : left");
				     break;
			       }
			 break;
		      case PRJustify:
			 if ((context->drv->GetJustification == NULL) ||
			     context->drv->GetJustification (elem, context, &pval))
			    break;
			 if (pval.value == DRIVERP_JUSTIFIED)
			    sprintf (mybuf, "align : justify");
			 break;
		      case PRIndent:
			 if ((context->drv->GetIndent == NULL) ||
			     context->drv->GetIndent (elem, context, &pval))
			    break;
			 sprintf (mybuf, "text-indent : %dpt", pval.value);
			 break;
		   }
	  }
	else
	   break;
	if (mybuf[0] != 0)
	  {
	     if (nb_rules != 0)
	       {
		  strncat (buf, "; ", free - strlen (buf));
	       }
	     strncat (buf, mybuf, free - strlen (buf));
	     nb_rules++;
	  }
	/*
	 * get next rule and add a separator if necessary.
	 */
	TtaNextPRule (elem, &rule);
     }
   while ((rule != NULL) && (strlen (buf) < free));

   if (nb_rules == 0)
     {
	*len = 0;
	buf[0] = 0;
     }
   else
     {
	*len = strlen (buf);
	buf[strlen (buf)] = 0;
     }
}

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
   SpecificContextBlock block;
   PresentationContext context;
   PresentationValue   unused =
   {0, 0};

#ifdef DEBUG_STYLES
   fprintf (stderr, "ParseHTMLSpecificStyle(%s,%s,%d)\n",
	    GetHTML3Name (elem, doc), attrstr, doc);
#endif
   /*
    * create the context of the Specific presentation driver.
    */
   context = (PresentationContext) & block;
   target = (PresentationTarget) elem;
   block.drv = &SpecificStrategy;
   block.doc = doc;
   block.schema = TtaGetDocumentSSchema (doc);


   /*
    * Call the parsor.
    */
   if (HTMLStyleParserDestructiveMode)
     {
	if (context->drv->CleanPresentation != NULL)
	   context->drv->CleanPresentation (target,
				     (PresentationContext) context, unused);
     }
   else
      ParseHTMLStyleDecl (target, (PresentationContext) context, attrstr);
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
   char                attrelemname[150];
   char               *deb = &sel[0];
   char               *elem = &sel[0];
   char               *cur = &sel[0];
   int                 type, attr, attrval;
   char               *ancestors[MAX_ANCESTORS];
   int                 i, j;

   for (i = 0; i < MAX_ANCESTORS; i++)
      ancestors[i] = NULL;

   /*
    * first format the first selector item, uniformizing blanks.
    */
   SKIP_BLANK (selector);
   sel[0] = 0;
   class[0] = 0;
   pseudoclass[0] = 0;
   attrelemname[0] = 0;
   while (1)
     {
	/* put one word in the sel buffer */
	while ((*selector != 0) && (*selector != ',') &&
	       (*selector != '.') && (*selector != ':') &&
	       (!IS_BLANK (selector)))
	   *cur++ = *selector++;
	*cur++ = 0;

	if ((*selector == ':') || (*selector == '.'))
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
	  {
	     /* end of the selector */
	     break;
	  }
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
	else if (IS_BLANK (selector))
	  {
	     SKIP_BLANK (selector);
	  }
     }

   elem = ancestors[0];
   if ((elem == NULL) || (*elem == 0))
      elem = &class[0];
   if (*elem == 0)
      elem = &pseudoclass[0];
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
   else
     {
	ctxt->class = NULL;
	ctxt->classattr = 0;
     }
   ctxt->type = ctxt->attr = ctxt->attrval = ctxt->attrelem = 0;
   if (attrelemname[0] != EOS)
     {
	GIType (attrelemname, &ctxt->attrelem);
     }
   for (i = 0; i < MAX_ANCESTORS; i++)
      ctxt->ancestors[i] = 0;
   for (i = 0; i < MAX_ANCESTORS; i++)
      ctxt->ancestors_nb[i] = 0;
   GIType (elem, &ctxt->type);
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
     {
	PresentationValue   unused =
	{0, 0};

	if (HTMLStyleParserDestructiveMode)
	  {
	     if (ctxt->drv->CleanPresentation != NULL)
		ctxt->drv->CleanPresentation (target, (PresentationContext) ctxt,
					      unused);
	  }
	else
	   ParseHTMLStyleDecl (target, (PresentationContext) ctxt, attrstr);
     }
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
   while ((selector != NULL) && (*selector != 0))
      selector = ParseHTMLGenericSelector (selector, attrstr, ctxt,
					   doc, gPres);
   FreeGenericContext (ctxt);

#ifdef DEBUG_STYLES
   fprintf (stderr, "ParseHTMLGenericStyle(%s,%s,%d) done\n",
	    selector, attrstr, doc);
#endif
}

/*----------------------------------------------------------------------
   
   PARSING FUNCTIONS FOR EACH ATTRIBUTES                      
   
  ----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
   ParseHTML3StyleTest : For testing purposes only !!!             
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleTest (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleTest (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   /*
    * test for low level Boxes support ...
    */
   PresentationValue   pval =
   {10, DRIVERP_UNIT_REL};
#if 0
   GenericContext      blk = (GenericContext) context;
#endif

   if (context->drv->SetBox)
      context->drv->SetBox (target, context, pval);

   if (context->drv->SetForegroundColor)
      context->drv->SetForegroundColor (target, context, pval);

   SKIP_WORD (attrstr);
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleBgBlendDir : parse an HTML3 bg-blend-direction   
   attribute string                                           
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleBgBlendDir (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleBgBlendDir (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   MSG ("ParseHTML3StyleBgBlendDir ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleBgPosition : parse an HTML3 bg-position          
   attribute string                                           
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleBgPosition (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleBgPosition (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   MSG ("ParseHTML3StyleBgPosition ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleBgStyle : parse an HTML3 bg-style attribute string 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleBgStyle (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleBgStyle (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   MSG ("ParseHTML3StyleBgStyle ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleBorderColor : parse an HTML3 border-color        
   attribute string.                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleBorderColor (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleBorderColor (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   MSG ("ParseHTML3StyleBorderColor ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleBorderStyle : parse an HTML3 border-style        
   attribute string.                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleBorderStyle (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleBorderStyle (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   MSG ("ParseHTML3StyleBorderStyle ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleBorderWidth : parse an HTML3 border-width        
   attribute string.                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleBorderWidth (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleBorderWidth (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   MSG ("ParseHTML3StyleBorderWidth ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleBorderColorInternal : parse an HTML3 border-color        
   attribute string.                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleBorderColorInternal (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleBorderColorInternal (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   MSG ("ParseHTML3StyleBorderColorInternal ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleBorderStyleInternal : parse an HTML3 border-style        
   attribute string.                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleBorderStyleInternal (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleBorderStyleInternal (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   MSG ("ParseHTML3StyleBorderStyleInternal ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleBorderWidthInternal : parse an HTML3 border-width        
   attribute string.                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleBorderWidthInternal (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleBorderWidthInternal (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   MSG ("ParseHTML3StyleBorderWidthInternal ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleClear : parse an HTML3 clear attribute string    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleClear (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleClear (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   MSG ("ParseHTML3StyleClear ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleDisplay : parse an HTML3 display attribute string        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleDisplay (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleDisplay (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   PresentationValue   pval;

   SKIP_BLANK (attrstr);
   if (IS_WORD (attrstr, "block"))
     {
	pval.unit = DRIVERP_UNIT_REL;
	pval.value = DRIVERP_NOTINLINE;
	if (context->drv->SetInLine)
	   context->drv->SetInLine (target, context, pval);
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "inline"))
     {
	pval.unit = DRIVERP_UNIT_REL;
	pval.value = DRIVERP_INLINE;
	if (context->drv->SetInLine)
	   context->drv->SetInLine (target, context, pval);
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "none"))
     {
	pval.unit = DRIVERP_UNIT_REL;
	pval.value = DRIVERP_HIDE;
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
   ParseHTML3StyleFloat : parse an HTML3 float attribute string    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleFloat (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleFloat (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   MSG ("ParseHTML3StyleFloat ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleLetterSpacing : parse an HTML3 letter-spacing    
   attribute string.                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleLetterSpacing (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleLetterSpacing (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   MSG ("ParseHTML3StyleLetterSpacing ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleListStyle : parse an HTML3 list-style            
   attribute string.                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleListStyle (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleListStyle (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   MSG ("ParseHTML3StyleListStyle ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleMagnification : parse an HTML3 magnification     
   attribute string.                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleMagnification (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleMagnification (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   PresentationValue   pval;

   SKIP_BLANK (attrstr);
   attrstr = ParseHTML3Unit (attrstr, &pval);
   if ((pval.unit == DRIVERP_UNIT_REL) && (pval.value >= -10) &&
       (pval.value <= 10))
     {
	if (context->drv == &GenericStrategy)
	  {
	     GenericContext      block = (GenericContext) context;

	     if ((block->type == HTML_EL_HTML) ||
		 (block->type == HTML_EL_BODY) ||
		 (block->type == HTML_EL_HEAD))
	       {
		  CSSSetMagnification (block->doc, (PSchema) target, pval.value);
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
   ParseHTML3StyleMarginLeft : parse an HTML3 margin-left          
   attribute string.                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleMarginLeft (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleMarginLeft (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   MSG ("ParseHTML3StyleMarginLeft ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleMarginRight : parse an HTML3 margin-right        
   attribute string.                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleMarginRight (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleMarginRight (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   MSG ("ParseHTML3StyleMarginRight ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleMargin : parse an HTML3 margin attribute string. 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleMargin (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleMargin (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   MSG ("ParseHTML3StyleMargin ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StylePack : parse an HTML3 pack attribute string.     
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StylePack (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StylePack (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   MSG ("ParseHTML3StylePack ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StylePadding : parse an HTML3 padding attribute string. 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StylePadding (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StylePadding (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   MSG ("ParseHTML3StylePadding ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleTextAlign : parse an HTML3 text-align            
   attribute string.                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleTextAlign (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleTextAlign (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   PresentationValue   align =
   {0, 1};
   PresentationValue   justify =
   {0, 1};

   SKIP_BLANK (attrstr);
   if (IS_WORD (attrstr, "left"))
     {
	align.value = AdjustLeft;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "right"))
     {
	align.value = AdjustRight;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "center"))
     {
	align.value = Centered;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "justify"))
     {
	justify.value = Justified;
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
   if (align.value)
     {
	if (context->drv->SetAlignment)
	   context->drv->SetAlignment (target, context, align);
     }
   if (justify.value)
     {
	if (context->drv->SetJustification)
	   context->drv->SetJustification (target, context, justify);
	if (context->drv->SetHyphenation)
	   context->drv->SetHyphenation (target, context, justify);
     }
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleTextIndent : parse an HTML3 text-indent          
   attribute string.                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleTextIndent (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleTextIndent (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   PresentationValue   pval;

   SKIP_BLANK (attrstr);
   attrstr = ParseHTML3Unit (attrstr, &pval);
   if (pval.unit == DRIVERP_UNIT_INVALID)
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
   ParseHTML3StyleTextTransform : parse an HTML3 text-transform    
   attribute string.                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleTextTransform (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleTextTransform (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   MSG ("ParseHTML3StyleTextTransform ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleVerticalAlign : parse an HTML3 vertical-align    
   attribute string.                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleVerticalAlign (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleVerticalAlign (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   MSG ("ParseHTML3StyleVerticalAlign ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleWhiteSpace : parse an HTML3 white-space          
   attribute string.                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleWhiteSpace (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleWhiteSpace (target, context, attrstr)
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
   ParseHTML3StyleWordSpacing : parse an HTML3 word-spacing        
   attribute string.                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleWordSpacing (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleWordSpacing (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   MSG ("ParseHTML3StyleWordSpacing ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleFont : parse an HTML3 font attribute string      
   we expect the input string describing the attribute to be     
   !!!!!!                                                  
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleFont (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleFont (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   MSG ("ParseHTML3StyleFont ");
   TODO
      return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleFontSize : parse an HTML3 font size attr string  
   we expect the input string describing the attribute to be     
   xx-small, x-small, small, medium, large, x-large, xx-large      
   or an absolute size, or an imcrement relative to the parent     
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleFontSize (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleFontSize (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   PresentationValue   pval;

   SKIP_BLANK (attrstr);
   if (IS_WORD (attrstr, "xx-small"))
     {
	pval.unit = DRIVERP_UNIT_REL;
	pval.value = 1;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "x-small"))
     {
	pval.unit = DRIVERP_UNIT_REL;
	pval.value = 2;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "small"))
     {
	pval.unit = DRIVERP_UNIT_REL;
	pval.value = 3;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "medium"))
     {
	pval.unit = DRIVERP_UNIT_REL;
	pval.value = 4;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "large"))
     {
	pval.unit = DRIVERP_UNIT_REL;
	pval.value = 5;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "x-large"))
     {
	pval.unit = DRIVERP_UNIT_REL;
	pval.value = 6;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "xx-large"))
     {
	pval.unit = DRIVERP_UNIT_REL;
	pval.value = 8;
	SKIP_WORD (attrstr);
     }
   else
     {
	attrstr = ParseHTML3Unit (attrstr, &pval);
	if (pval.unit == DRIVERP_UNIT_INVALID)
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
   ParseHTML3StyleFontFamily : parse an HTML3 font family string   
   we expect the input string describing the attribute to be     
   a common generic font style name                                
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleFontFamily (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleFontFamily (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   PresentationValue   font =
   {0, 1};

   SKIP_BLANK (attrstr);
   if (IS_CASE_WORD (attrstr, "times"))
     {
	font.value = DRIVERP_FONT_TIMES;
	SKIP_PROPERTY (attrstr);
     }
   else if (IS_CASE_WORD (attrstr, "serif"))
     {
	font.value = DRIVERP_FONT_TIMES;
	SKIP_PROPERTY (attrstr);
     }
   else if (IS_CASE_WORD (attrstr, "helvetica"))
     {
	font.value = DRIVERP_FONT_HELVETICA;
	SKIP_PROPERTY (attrstr);
     }
   else if (IS_CASE_WORD (attrstr, "sans-serif"))
     {
	font.value = DRIVERP_FONT_HELVETICA;
	SKIP_PROPERTY (attrstr);
     }
   else if (IS_CASE_WORD (attrstr, "courier"))
     {
	font.value = DRIVERP_FONT_COURIER;
	SKIP_PROPERTY (attrstr);
     }
   else if (IS_CASE_WORD (attrstr, "monospace"))
     {
	font.value = DRIVERP_FONT_COURIER;
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
   ParseHTML3StyleFontWeight : parse an HTML3 font weight string   
   we expect the input string describing the attribute to be     
   extra-light, light, demi-light, medium, demi-bold, bold, extra-bold
   or a number encoding for the previous values                       
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleFontWeight (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleFontWeight (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   PresentationValue   weight =
   {0, 1};
   int                 val;

   SKIP_BLANK (attrstr);
   if (IS_WORD (attrstr, "extra-light"))
     {
	weight.value = -3;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "light"))
     {
	weight.value = -2;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "demi-light"))
     {
	weight.value = -1;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "medium"))
     {
	weight.value = 0;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "extra-bold"))
     {
	weight.value = +3;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "bold"))
     {
	weight.value = +2;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "demi-bold"))
     {
	weight.value = +1;
	SKIP_WORD (attrstr);
     }
   else if (sscanf (attrstr, "%d", &val) > 0)
     {
	if ((val < -3) || (val > 3))
	  {
	     fprintf (stderr, "invalid font weight %d\n", val);
	     weight.value = 0;
	  }
	else
	   weight.value = val;
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
   switch (weight.value)
	 {
	    case 3:
	    case 2:
	    case 1:
	       weight.value = DRIVERP_FONT_BOLD;
	       break;
	    case -3:
	    case -2:
	    case -1:
	       weight.value = DRIVERP_FONT_ITALICS;
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
   ParseHTML3StyleFontStyle : parse an HTML3 font style string     
   we expect the input string describing the attribute to be     
   italic, oblique, small-caps or normal                         
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleFontStyle (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleFontStyle (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   PresentationValue   style =
   {0, 1};
   PresentationValue   size =
   {0, 1};

   SKIP_BLANK (attrstr);
   if (IS_WORD (attrstr, "italic"))
     {
	style.value = DRIVERP_FONT_ITALICS;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "oblique"))
     {
	style.value = DRIVERP_FONT_OBLIQUE;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "small-caps"))
     {
	style.value = DRIVERP_FONT_BOLD;
	size.value = -2;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "normal"))
     {
	style.value = DRIVERP_FONT_ROMAN;
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
   if (style.value != 0)
     {
	PresentationValue   previous_style;

	if ((context->drv->GetFontStyle) &&
	    (!context->drv->GetFontStyle (target, context, &previous_style)))
	  {
	     if (previous_style.value == DRIVERP_FONT_BOLD)
	       {
		  if (style.value == DRIVERP_FONT_ITALICS)
		     style.value = DRIVERP_FONT_BOLDITALICS;
		  if (style.value == DRIVERP_FONT_OBLIQUE)
		     style.value = DRIVERP_FONT_BOLDOBLIQUE;
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
   if (size.value != 0)
     {
	PresentationValue   previous_size;

	if ((context->drv->GetFontSize) &&
	    (!context->drv->GetFontSize (target, context, &previous_size)))
	  {
	     /* !!!!!!!!!!!!!!!!!!!!!!!! Unite + relatif !!!!!!!!!!!!!!!! */
	     size.value += previous_size.value;
	     if (context->drv->SetFontSize)
		context->drv->SetFontSize (target, context, size);
	  }
	else
	  {
	     size.value = 10;
	     if (context->drv->SetFontSize)
		context->drv->SetFontSize (target, context, size);
	  }
     }
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleLineSpacing : parse an HTML3 font leading string 
   we expect the input string describing the attribute to be     
   value% or value                                               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleLineSpacing (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleLineSpacing (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   PresentationValue   lead;

   attrstr = ParseHTML3Unit (attrstr, &lead);
   if (lead.unit == DRIVERP_UNIT_INVALID)
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
   ParseHTML3StyleTextDecoration : parse an HTML3 text decor string   
   we expect the input string describing the attribute to be     
   underline, overline, line-through, box, shadowbox, box3d,       
   cartouche, blink or none                                        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleTextDecoration (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleTextDecoration (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   PresentationValue   decor =
   {0, 1};

   SKIP_BLANK (attrstr);
   if (IS_WORD (attrstr, "underline"))
     {
	decor.value = Underline;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "overline"))
     {
	decor.value = Overline;
	SKIP_WORD (attrstr);
     }
   else if (IS_WORD (attrstr, "line-through"))
     {
	decor.value = CrossOut;
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
	decor.value = NoUnderline;
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
   if (decor.value)
     {
	if (context->drv->SetTextUnderlining)
	   context->drv->SetTextUnderlining (target, context, decor);
     }
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleColor : parse an HTML3 color attribute string    
   we expect the input string describing the attribute to be     
   either a color name, a 3 tuple or an hexadecimal encoding.    
   The color used will be approximed from the current color      
   table                                                         
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleColor (char *attrstr, PresentationValue * val)
#else
static char        *ParseHTML3StyleColor (attrstr, val)
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

   val->unit = DRIVERP_UNIT_INVALID;
   val->value = 0;

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
   val->unit = DRIVERP_UNIT_INVALID;
   val->value = 0;
   return (attrstr);

 found_RGB:
   best = TtaGetThotColor (redval, greenval, blueval);
   val->value = best;
   val->unit = DRIVERP_UNIT_REL;

   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleWidth : parse an HTML3 width attribute           
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleWidth (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleWidth (target, context, attrstr)
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
	MSG ("ParseHTML3StyleWidth : auto ");
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
   ParseHTML3StyleMarginTop : parse an HTML3 margin-top attribute  
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleMarginTop (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleMarginTop (target, context, attrstr)
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
   if (margin.unit == DRIVERP_UNIT_INVALID)
     {
	MSG ("invalid margin top\n");
	return (attrstr);
     }
   if (context->drv->SetVPos)
      context->drv->SetVPos (target, context, margin);
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleMarginBottom : parse an HTML3 margin-bottom      
   attribute                                                 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleMarginBottom (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleMarginBottom (target, context, attrstr)
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
   if (margin.unit == DRIVERP_UNIT_INVALID)
     {
	MSG ("invalid margin top\n");
	return (attrstr);
     }
   if (context->drv->SetVPos)
      context->drv->SetVPos (target, context, margin);
   return (attrstr);
}

/*----------------------------------------------------------------------
   ParseHTML3StyleHeight : parse an HTML3 height attribute                 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleHeight (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleHeight (target, context, attrstr)
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
	MSG ("ParseHTML3StyleHeight : auto ");
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
   ParseHTML3StyleForeground : parse an HTML3 foreground attribute 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleForeground (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleForeground (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   PresentationValue   best;

   attrstr = ParseHTML3StyleColor (attrstr, &best);

   if (best.unit == DRIVERP_UNIT_INVALID)
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
   ParseHTML3StyleBackground : parse an HTML3 background attribute 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static char        *ParseHTML3StyleBackground (PresentationTarget target,
				 PresentationContext context, char *attrstr)
#else
static char        *ParseHTML3StyleBackground (target, context, attrstr)
PresentationTarget  target;
PresentationContext context;
char               *attrstr;

#endif
{
   PresentationValue   best;
   char               *url;

   best.unit = DRIVERP_UNIT_INVALID;

   if (IS_CASE_WORD (attrstr, "url"))
     {
	/*
	 * we don't currently support URL just parse it to skip it.
	 */
	attrstr = ParseHTMLURL (attrstr, &url);
	TtaFreeMemory (url);
     }
   attrstr = ParseHTML3StyleColor (attrstr, &best);

   if (best.unit == DRIVERP_UNIT_INVALID)
     {
	return (attrstr);
     }
   /*
    * if the background is set on the HTML or BODY element,
    * set the background color for the full window.
    */
   if (context->drv == &GenericStrategy)
     {
	GenericContext      block = (GenericContext) context;

	if ((block->type == HTML_EL_HTML) || (block->type == HTML_EL_BODY) ||
	    (block->type == HTML_EL_HEAD))
	  {
	     CSSSetBackground (block->doc, (PSchema) target, best.value);
	     return (attrstr);
	  }
     }
   else if (context->drv == &SpecificStrategy)
     {
	SpecificContextBlock *block = (SpecificContextBlock *) context;
	SpecificTarget      elem = (SpecificTarget) target;
	char               *name = GetHTML3Name (elem, block->doc);

	if ((!strcmp (name, "HTML")) || (!strcmp (name, "BODY")) ||
	    (!strcmp (name, "HEAD")))
	  {
	     TtaSetViewBackgroundColor (block->doc, 1, best.value);
	     return (attrstr);
	  }
     }
   /*
    * install the new presentation.
    */
   if (context->drv->SetBackgroundColor)
      context->drv->SetBackgroundColor (target, context, best);
   /* thot specificity : need to set fill pattern for background color */
   best.value = 2;
   best.unit = DRIVERP_UNIT_REL;
   if (context->drv->SetFillPattern)
      context->drv->SetFillPattern (target, context, best);
   return (attrstr);
}

/*----------------------------------------------------------------------
   
   PARSING FUNCTIONS FOR HEADER STYLE DECLARATIONS                
   
  ----------------------------------------------------------------------*/

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
}

/*----------------------------------------------------------------------
   
   EVALUATION FUNCTIONS / CASCADING AND OVERLOADING              
   
  ----------------------------------------------------------------------*/

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

/*----------------------------------------------------------------------
   
   LIBRARY FUNCTIONS of GENERAL INTERREST                    
   
  ----------------------------------------------------------------------*/

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
   SearchClass : search a given class in the style rules           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Element             SearchClass (char *class, Document doc)
#else
Element             SearchClass (class, doc)
char               *class;
Document            doc;

#endif
{
   Element             el;
   ElementType         elType;
   AttributeType       atType;
   Attribute           at;
   char                name[101];
   int                 len;

   /*
    * Browse the Style list in the document head.
    */
   elType.ElSSchema = TtaGetDocumentSSchema (doc);
   elType.ElTypeNum = HTML_EL_StyleRule;
   el = TtaSearchTypedElement (elType, SearchInTree, TtaGetMainRoot (doc));

   while (el != NULL)
     {
	atType.AttrSSchema = TtaGetDocumentSSchema (doc);
	atType.AttrTypeNum = HTML_ATTR_Selector;

	at = TtaGetAttribute (el, atType);
	if (at)
	  {
	     len = 100;
	     TtaGiveTextAttributeValue (at, name, &len);
	     name[len + 1] = 0;
	     if (!strcmp (class, name))
		break;
	  }
	/* get next StyleRule */
	TtaNextSibling (&el);
     }
   return (el);
}

/*----------------------------------------------------------------------
   ApplyStyleRule : Change the presentation attributes of          
   a given element to reflect a style rule.                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ApplyStyleRule (Element elem, Element stylerule, Document doc)
#else
void                ApplyStyleRule (elem, stylerule, doc)
Element             elem;
Element             stylerule;
Document            doc;

#endif
{
   PRule               rule, new;
   PRule               orig;
   int                 ruleType;

   /*
    * copy the new presentation specific attributes found.
    */
   rule = NULL;
   do
     {
	TtaNextPRule (stylerule, &rule);
	if (rule)
	  {
	     /*
	      * if there is a rule for this type supress it.
	      */
	     ruleType = TtaGetPRuleType (rule);
	     orig = TtaGetPRule (elem, ruleType);
	     if (orig)
		TtaRemovePRule (elem, orig, doc);

	     /*
	      * and copy the new one.
	      */
	     new = TtaCopyPRule (rule);
	     TtaAttachPRule (elem, new, doc);
	  }
     }
   while (rule != NULL);
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
   PresentationValue   unused =
   {0, 0};

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
     }
   atType.AttrSSchema = TtaGetDocumentSSchema (doc);
   atType.AttrTypeNum = HTML_ATTR_Style_;

   at = TtaGetAttribute (elem, atType);
   if (at != NULL)
     {
	TtaRemoveAttribute (elem, at, doc);
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

/*----------------------------------------------------------------------
   
   Needed for support of HTML 3.2                       
   
  ----------------------------------------------------------------------*/

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

   sprintf (css_command, "background : %s", color);
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

   sprintf (css_command, "color : %s", color);
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
   PRule               rule;
   ElementType         elType;
   int                 type;

   rule = TtaGetPRule (elem, PRBackground);
   if (rule)
      TtaRemovePRule (elem, rule, doc);
   elType = TtaGetElementType (elem);
   type = elType.ElTypeNum;
   if ((type == HTML_EL_HTML) || (type == HTML_EL_BODY) ||
       (type == HTML_EL_HEAD))
      TtaResetViewBackgroundColor (doc, 1);
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
   PRule               rule;

   rule = TtaGetPRule (elem, PRForeground);
   if (rule)
      TtaRemovePRule (elem, rule, doc);
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
   ParseHTMLStyleHeader (NULL, css_command, doc, True);
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
   ParseHTMLStyleHeader (NULL, css_command, doc, True);
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
   ParseHTMLStyleHeader (NULL, css_command, doc, True);
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
   SetHTMLStyleParserDestructiveMode (True);
   ParseHTMLStyleHeader (NULL, css_command, doc, True);
   SetHTMLStyleParserDestructiveMode (False);
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
   SetHTMLStyleParserDestructiveMode (True);
   ParseHTMLStyleHeader (NULL, css_command, doc, True);
   SetHTMLStyleParserDestructiveMode (False);
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
   SetHTMLStyleParserDestructiveMode (True);
   ParseHTMLStyleHeader (NULL, css_command, doc, True);
   SetHTMLStyleParserDestructiveMode (False);
}
