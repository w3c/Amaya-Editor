/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * p2css.c : file used to analyse a PSchema and translate it to CSS or
 *           modify it on the fly. Complementary to api/genericdriver.c.
 *
 * Author: D. Veillard
 *
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"

#include "html2thot_f.h"
#include "p2css_f.h"
#include "HTMLstyle_f.h"

/* DEBUG_RPI turn on verbose debugging of the RPI manipulations */

#ifdef AMAYA_DEBUG
#define MSG(msg) fprintf(stderr,msg)
#else
static char        *last_message = NULL;

#define MSG(msg) last_message = msg
#endif

/************************************************************************
 *									*
 * RPI handling : RPI are structures containing a generic CSS rule,	*
 *	i.e. the selector, the presentation rules, a pointer to the	*
 *	associated generic context and control data.			*
 *									*
 ************************************************************************/


static FILE        *output = NULL;


/*----------------------------------------------------------------------
   PrintRPI                                                        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                PrintRPI (PRuleInfoPtr rpi)
#else
void                PrintRPI (rpi)
PRuleInfoPtr        rpi;

#endif
{
   fprintf (output, "    @%X : sel %s : css %s", (unsigned int) rpi,
	    rpi->selector, rpi->css_rule);
   switch (rpi->state)
	 {
	    case UnknownRPI:
	       fprintf (output, ", unknown state\n");
	       break;
	    case ModifiedRPI:
	       fprintf (output, ", modified\n");
	       break;
	    case RemovedRPI:
	       fprintf (output, ", removed\n");
	       break;
	    case NormalRPI:
	       fprintf (output, "\n");
	       break;
	 }
}

/*----------------------------------------------------------------------
   PrintListRPI                                                    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                PrintListRPI (PRuleInfoPtr list)
#else
void                PrintListRPI (list)
PRuleInfoPtr        list;

#endif
{
   PRuleInfoPtr        rpi = list;

   fprintf (output, "ListRPI :\n");
   while (rpi != NULL)
     {
	PrintRPI (rpi);
	rpi = rpi->NextRPI;
     }
}

/*----------------------------------------------------------------------
   AddRPI                                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                AddRPI (PRuleInfoPtr rpi, PRuleInfoPtr * list)
#else
void                AddRPI (rpi, list)
PRuleInfoPtr        rpi;
PRuleInfoPtr        list;

#endif
{
   if (rpi == NULL)
      return;

   if (rpi->ctxt != NULL) {
       int i;

       if (rpi->ctxt->type == HTML_EL_BODY)
	   rpi->ctxt->type = HTML_EL_HTML;
       for (i = 0;i < MAX_ANCESTORS; i++)
	   if (rpi->ctxt->ancestors[i] == HTML_EL_BODY)
	       rpi->ctxt->ancestors[i] = HTML_EL_HTML;
       if (rpi->ctxt->attrelem == HTML_EL_BODY)
	   rpi->ctxt->attrelem = HTML_EL_HTML;
   }
   rpi->NextRPI = *list;
   *list = rpi;
}

/*----------------------------------------------------------------------
   FreeRPI                                                         
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                FreeRPI (PRuleInfoPtr rpi)
#else
void                FreeRPI (rpi)
PRuleInfoPtr        rpi;

#endif
{
   if (rpi)
     {
	if (rpi->ctxt != NULL)
	  {
	     FreeGenericContext (rpi->ctxt);
	     rpi->ctxt = NULL;
	  }
	if (rpi->selector != NULL)
	  {
	     TtaFreeMemory (rpi->selector);
	     rpi->selector = NULL;
	  }
	if (rpi->css_rule != NULL)
	  {
	     TtaFreeMemory (rpi->css_rule);
	     rpi->css_rule = NULL;
	  }
	rpi->NextRPI = NULL;
	TtaFreeMemory ((char *) rpi);
     }
}

/*----------------------------------------------------------------------
   CleanListRPI                                                    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                CleanListRPI (PRuleInfoPtr * list)
#else
void                CleanListRPI (list)
PRuleInfoPtr       *list;

#endif
{
   PRuleInfoPtr        rpi;

   if (list == NULL)
      return;
   while (*list != NULL)
     {
	rpi = *list;
	*list = rpi->NextRPI;
	FreeRPI (rpi);
     }
}

/*----------------------------------------------------------------------
   NewRPI                                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PRuleInfoPtr        NewRPI (Document doc)
#else
PRuleInfoPtr        NewRPI (doc)
Document            doc;

#endif
{
   PRuleInfoPtr        rpi;

#ifdef DEBUG_RPI
   fprintf (output, "NewRPI\n");
#endif

   rpi = (PRuleInfoPtr) TtaGetMemory (sizeof (PRuleInfo));

   rpi->NextRPI = NULL;
   rpi->state = UnknownRPI;
   rpi->pschema = NULL;
   rpi->ctxt = GetGenericContext (doc);
   rpi->selector = NULL;
   rpi->css_rule = NULL;
   return (rpi);
}

/*----------------------------------------------------------------------
   CmpRPI                                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 CmpRPI (PRuleInfoPtr cour, PRuleInfoPtr rpi)
#else
int                 CmpRPI (cour, rpi)
PRuleInfoPtr        cour;
PRuleInfoPtr        rpi;

#endif
{
   int                 i;

   if (rpi->pschema != cour->pschema)
      return (1);
   if (rpi->ctxt->type != cour->ctxt->type)
      return (1);
   if (rpi->ctxt->attr != cour->ctxt->attr)
      return (1);
   if (rpi->ctxt->attrval != cour->ctxt->attrval)
      return (1);
   if (rpi->ctxt->class != cour->ctxt->class)
      return (1);
   if (rpi->ctxt->classattr != cour->ctxt->classattr)
      return (1);
   if (rpi->ctxt->attrelem != cour->ctxt->attrelem)
      return (1);
   for (i = 0; i < MAX_ANCESTORS; i++)
      if (rpi->ctxt->ancestors[i] != cour->ctxt->ancestors[i])
	 return (1);
   for (i = 0; i < MAX_ANCESTORS; i++)
      if (rpi->ctxt->ancestors_nb[i] != cour->ctxt->ancestors_nb[i])
	 return (1);
   return (0);
}

/*----------------------------------------------------------------------
   SearchRPI                                                       
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PRuleInfoPtr        SearchRPI (PRuleInfoPtr cour, PRuleInfoPtr list)
#else
PRuleInfoPtr        SearchRPI (cour)
PRuleInfoPtr        cour;

#endif
{
   PRuleInfoPtr        rpi = list;

   while (rpi != NULL)
     {
	if (!CmpRPI (rpi, cour))
	  {
#ifdef DEBUG_RPI
	     fprintf (output, "SearchRPI : found %s\n", rpi->selector);
#endif
	     return (rpi);
	  }
	rpi = rpi->NextRPI;
     }
   return (NULL);
}

/*----------------------------------------------------------------------
   RemoveRPI                                                       
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                RemoveRPI (Document doc, PRuleInfoPtr cour)
#else
void                RemoveRPI (doc, cour)
Document            doc;
PRuleInfoPtr        cour;

#endif
{
   PresentationContext context;
   PresentationValue   unused;
   unused.data = 0;

   if (cour == NULL)
      return;

   /*
    * call the Generic Presentation driver to clean the associated
    * in-core structures.
    */
   context = (PresentationContext) cour->ctxt;
   cour->ctxt->destroy = 1;
   ParseHTMLStyleDecl (cour->pschema, context, cour->css_rule);
   cour->ctxt->destroy = 0;

   /*
    * mark the RPI as removed.
    */
   cour->state = RemovedRPI;
}

/************************************************************************
 *									*
 * RPI scanning : These functions are used to scan a PSchema and	*
 *	extract all the generic CSS rules it contains and store them	*
 *	in a RPI list.							*
 *									*
 ************************************************************************/


/*
 * GenericSettingsToCSS :  Callback for ApplyAllGenericSettings,
 *     enrich the CSS string.
 */

#ifdef __STDC__
static void         GenericSettingsToCSS (GenericTarget target,
     GenericContext ctxt,PresentationSetting settings, void *param)
#else
static void         GenericSettingsToCSS (settings, param)
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

/*
 * GenericContextToRPI :  transform a Generic Context to a RPI and
 *      add it to the list given as the argument.
 */

#ifdef __STDC__
static void         GenericContextToRPI (GenericTarget target,
					 GenericContext ctxt, void *param)
#else
static void         GenericContextToRPI (target, ctxt, param)
GenericTarget       target;
GenericContext      ctxt;
void               *param;

#endif
{
   PRuleInfo           local;
   char                string[5000];
   PRuleInfoPtr        new;
   PRuleInfoPtr       *list = (PRuleInfoPtr *) param;
   char               *css_rule;
   int                 i, j;

   /*int                 new_rule = 0; */
   int                 exist;

   local.pschema = target;
   local.ctxt = ctxt;

   string[0] = EOS;

   /*
    * this will transform all the Generic Settings associated to
    * the context to one CSS string.
    */
   ApplyAllGenericSettings (target, ctxt, GenericSettingsToCSS, &string[0]);

   /*
    * if we were unable to translate this rule in CSS, return
    */
   if (string[0] == 0)
      return;

   /* search if such an RPI is already registered */
   exist = TRUE;
   new = SearchRPI (&local, *list);
   if (new == NULL)
     {
	exist = FALSE;
	new = NewRPI (ctxt->doc);
	new->pschema = target;
	new->state = NormalRPI;
	new->ctxt->type = local.ctxt->type;
	if (new->ctxt->type == HTML_EL_HTML)
	    new->ctxt->type = HTML_EL_BODY;
	new->ctxt->attr = local.ctxt->attr;
	new->ctxt->attrval = local.ctxt->attrval;
	new->ctxt->attrelem = local.ctxt->attrelem;
        if (new->ctxt->attrelem == HTML_EL_HTML)
	    new->ctxt->attrelem = HTML_EL_BODY;
	new->ctxt->class = local.ctxt->class;
	new->ctxt->classattr = local.ctxt->classattr;
	for (i = 0; i < MAX_ANCESTORS; i++) {
	   new->ctxt->ancestors[i] = local.ctxt->ancestors[i];
	   if (new->ctxt->ancestors[i] == HTML_EL_HTML)
	       new->ctxt->ancestors[i] = HTML_EL_BODY;
	}
	for (i = 0; i < MAX_ANCESTORS; i++)
	   new->ctxt->ancestors_nb[i] = local.ctxt->ancestors_nb[i];
	/*new_rule = 1; */
     }
   /*
    * append this CSS rule to the RPI description
    */
   if (new->css_rule == NULL)
     {
	css_rule = TtaGetMemory (strlen (string) + 4);
	strcpy (css_rule, string);
     }
   else
     {
	css_rule = TtaGetMemory (strlen (string) + strlen (new->css_rule) + 8);
	strcpy (css_rule, new->css_rule);
	strcat (css_rule, "; ");
	strcat (css_rule, string);
	TtaFreeMemory (new->css_rule);
     }
   new->css_rule = css_rule;

   /*
    * create the selector for the element if it doesn t exist yet
    */
   if (new->selector == NULL)
     {
	string[0] = 0;
	i = 0;
	for (; i < MAX_ANCESTORS; i++)
	   if (new->ctxt->ancestors[i])
	     {
		for (j = 0; j <= new->ctxt->ancestors_nb[i]; j++)
		  {
		     if (string[0] != 0)
			strcat (string, " ");
		     strcat (string, GITagNameByType (new->ctxt->ancestors[i]));
		  }
	     }
	if (new->ctxt->attr)
	  {
	     if (string[0] != 0)
		strcat (string, " ");
	     strcat (string, "???");
	  }
	if (new->ctxt->type)
	  {
	     if (string[0] != 0)
		strcat (string, " ");
	     strcat (string, GITagNameByType (new->ctxt->type));
	  }
	if ((new->ctxt->class) && (new->ctxt->classattr == HTML_ATTR_Class))
	  {
	     if (string[0] != 0)
		strcat (string, " ");
	     if (new->ctxt->attrelem)
	       {
		  strcat (string, GITagNameByType (new->ctxt->attrelem));
	       }
	     strcat (string, ".");
	     strcat (string, new->ctxt->class);
	  }
	if ((new->ctxt->class) && (new->ctxt->classattr == HTML_ATTR_PseudoClass))
	  {
	     if (string[0] != 0)
		strcat (string, " ");
	     if (new->ctxt->attrelem)
	       {
		  strcat (string, GITagNameByType (new->ctxt->attrelem));
	       }
	     strcat (string, ":");
	     strcat (string, new->ctxt->class);
	  }
	if ((new->ctxt->class) && (new->ctxt->classattr == HTML_ATTR_ID))
	  {
	     if (string[0] != 0)
		strcat (string, " ");
	     if (new->ctxt->attrelem)
	       {
		  strcat (string, GITagNameByType (new->ctxt->attrelem));
	       }
	     strcat (string, "#");
	     strcat (string, new->ctxt->class);
	  }
	new->selector = TtaGetMemory (strlen (string) + 4);
	strcpy (new->selector, string);
     }
   if (!exist)
      AddRPI (new, list);

#ifdef DEBUG_RPI
   fprintf (output, "GenericContextToRPI result : \n");
   PrintRPI (new);
#endif
}

/*----------------------------------------------------------------------
 *									*
 * RPI scanning front-end : these function are used to actually	        *
 *	retrieve or print RPI lists associated to documents.		*
 *									*
  ----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
   PSchema2RPI : return the list of all RPI associated to a PSchema  
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PRuleInfoPtr        PSchema2RPI (Document doc, PSchema gPres, int zoom, int background)
#else
PRuleInfoPtr        PSchema2RPI (doc, gPres, zoom, background)
Document            doc;
PSchema             gPres;
int                 zoom;
int                 background;

#endif
{
   PRuleInfoPtr        lrpi = NULL;

   /* clean local structures */
   CleanListRPI (NULL);

   /*
    * Hack to support presentation rules not contained in the P structures.
    * Add a first rule on BODY if zoom or background are sets
    */
   if (((zoom >= -10) && (zoom <= 10) && (zoom != 0)) ||
       (background >= 0))
     {
	char                rule[150];
	PRuleInfoPtr        rpi = NewRPI (doc);
	unsigned short      red, green, blue;

	if (rpi == NULL)
	   return (NULL);
	rpi->state = NormalRPI;
	rpi->pschema = gPres;
	rpi->ctxt->type = HTML_EL_BODY;
	rpi->selector = TtaStrdup ("BODY");

	if (((zoom >= -10) && (zoom <= 10) && (zoom != 0)) &&
	    (background >= 0))
	  {
	     TtaGiveThotRGB (background, &red, &green, &blue);
	     sprintf (rule, "magnification : %d; background : #%02X%02X%02X",
		      zoom, red, green, blue);
	  }
	else if ((zoom >= -10) && (zoom <= 10) && (zoom != 0))
	  {
	     sprintf (rule, "magnification : %d", zoom);
	  }
	else if (background >= 0)
	  {
	     TtaGiveThotRGB (background, &red, &green, &blue);
	     sprintf (rule, "background : #%02X%02X%02X", red, green, blue);
	  }
	rpi->css_rule = TtaStrdup (rule);
	rpi->NextRPI = lrpi;
	lrpi = rpi;
     }
   /*
    * build the RPI list using the presentation driver browsing functions.
    */
   ApplyAllGenericContext (doc, gPres, GenericContextToRPI, &lrpi);

   /*
    * transmit the result to the uper layer.
    */
   return (lrpi);
}

/*----------------------------------------------------------------------
   PSchema2CSS                                                     
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 PSchema2CSS (Document doc, PSchema gPres, int zoom, int background,
				 char *output_file)
#else
int                 PSchema2CSS (doc, gPres, output_file, zoom, background)
Document            doc;
PSchema             gPres;
char               *output_file;
int                 zoom;
int                 background;

#endif
{
   PRuleInfoPtr        rpi, list;

   if (output_file != NULL)
     {
	output = fopen (output_file, "w");
	if (output == NULL)
	   output = stderr;
	else
	   fprintf (output, "/**********************************\n");
     }
   else if (output == NULL)
      output = stderr;
   list = rpi = PSchema2RPI (doc, gPres, zoom, background);

   if (output != stderr)
     {
	fprintf (output, " **********************************/\n");
	while (rpi != NULL)
	  {
	     fprintf (output, "%s { %s }\n", rpi->selector, rpi->css_rule);
	     rpi = rpi->NextRPI;
	  }
	fclose (output);
     }
   CleanListRPI (&list);
   return (0);
}

/*----------------------------------------------------------------------
   BuildRPIList : Build the whole list of CSS in use by a document   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 BuildRPIList (Document doc, PSchema gPres, int zoom, int background,
				  char *buf, int size, char *first)
#else
int                 BuildRPIList (doc, gPres, zoom, background, buf, size, first)
Document            doc;
PSchema             gPres;
int                 zoom;
int                 background;
char               *buf;
int                 size;
char               *first;

#endif
{
   int                 free = size;
   int                 len;
   int                 nb = 0;
   int                 index = 0;
   PRuleInfoPtr        rpi, list;

   /*
    * ad the first element if specified.
    */
   buf[0] = 0;
   if (first)
     {
	strcpy (&buf[index], first);
	len = strlen (first);
	len++;
	free -= len;
	index += len;
	nb++;
     }
   list = rpi = PSchema2RPI (doc, gPres, zoom, background);
   while (rpi != NULL)
     {
	len = strlen (rpi->selector);
	len++;
	if (len >= free)
	  {
	     MSG ("BuildRPIList : Too many styleruless\n");
	     break;
	  }
	if (!strcmp (rpi->selector, buf))
	  {			/* ensure unicity / first */
	     rpi = rpi->NextRPI;
	     continue;
	  }
	strcpy (&buf[index], rpi->selector);
	free -= len;
	index += len;
	nb++;
	rpi = rpi->NextRPI;
     }

#ifdef DEBUG_RPI
   fprintf (stderr, "BuildRPIList : heap @0x%X : found %d RPI\n", (int) list, nb);
#endif

   CleanListRPI (&list);

   return (nb);
}
