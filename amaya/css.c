/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * CSS.c : Handle all the dialogs and interface functions needed 
 * to manipulate CSS.
 *
 * Authors: D. Veillard
 *          I. Vatton
 *
 */

/* Amaya includes  */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"

/*
 * HOTLIST :
 *
 *  [X] handle the user's preferences correctly
 *
 *  [/] check the CSS hotlist refill (keep last used first).
 *
 *  [X] ensure that one external CSS file is not added twice to the List
 *
 *  [X] create the dialogs needed to browse and add an external style sheet
 *
 *  [X] create the dialogs and the internal functions needed to save a CSS file.
 *
 *  [X] implement the new rule copy / merge operations
 *
 *  [/] ensure that upon reload of a CSS or leaving the CSS dialog all
 *      modifications are saved or standard version is released.
 *
 *  [X] verify that the rule order (pschema list) follow the CSS specs.
 *
 *  [X] remove tempfile upon closing.
 *
 *  [/] saving and restoring non-P extensions of CSS like background color
 *      and zoom.
 *
 *  [X] removing an external style sheet.
 *
 *  [ ] saving CSS to a given file, e.g. extracting document's style to
 *      an new external style sheet.
 *
 *  [X] Use the new ButtonList Widgets.
 *
 *  [ ] Give feedback on Parsing errors ... Gasp !
 *
 */


#define AMAYA_SAVE_DIR "AMAYA_SAVE_DIR"

static char         currentBRPI[2000] = "";
static int          CSSLEntry = -1;
static int          CSSREntry = -1;

PRuleInfoPtr        RListRPI = NULL;
PRuleInfoPtr        LListRPI = NULL;
CSSInfoPtr          RCSS = NULL;
CSSInfoPtr          LCSS = NULL;
CSSInfoPtr          ListCSS = NULL;
Document            currentDocument = -1;
CSSInfoPtr          User_CSS = NULL;
int                 BaseCSSDialog = -1;
int                 RListRPIIndex = -1;
int                 LListRPIIndex = -1;
char                currentRRPI[500] = "";
char                currentLRPI[500] = "";
char               *CSSDocumentName = NULL;
char               *CSSDirectoryName = NULL;
char               *amaya_save_dir = NULL;
char               *currentLCSS = NULL;
char               *currentRCSS = NULL;
boolean             RListRPIModified = FALSE;
boolean             LListRPIModified = FALSE;
boolean             NonPPresentChanged = FALSE;

#ifdef AMAYA_DEBUG
#define MSG(msg) fprintf(stderr,msg)
#else
static char        *last_message = NULL;

#define MSG(msg) last_message = msg
#endif

#include "css_f.h"
#include "init_f.h"
#include "p2css_f.h"
#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
#include "query_f.h"
#endif
#include "AHTURLTools_f.h"
#include "HTMLstyle_f.h"
#include "UIcss_f.h"

/*----------------------------------------------------------------------
   ParseHTMLStyleSheet : Parse an HTML Style Sheet fragment        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ParseHTMLStyleSheet (char *fragment, Document doc, CSSInfoPtr css)
#else
static void         ParseHTMLStyleSheet (fragment, doc, css)
char               *fragment;
Document            doc;
CSSInfoPtr          css;
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
	ParseCSSGenericStyle (SelecBuffer, StyleBuffer, doc, css);

	StyleIndex = 0;
	SelecIndex = 0;
	in_selec = 1;
	in_style = 0;
	return;
     }
   while ((*fragment != 0) && (SelecIndex < 1000) && (StyleIndex < 3000))
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
	 }
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
	       ParseCSSGenericStyle (SelecBuffer, StyleBuffer, doc, css);
	       
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

#ifdef DEBUG_CSS
/*----------------------------------------------------------------------
   PrintListCSS                                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PrintListCSS (FILE * output)
#else
static void         PrintListCSS (output)
FILE               *output;
#endif
{
   CSSInfoPtr          css = ListCSS;
   int                 i;

   fprintf (output, "ListCSS :\n");
   while (css != NULL)
     {
       switch (css->category)
	 {
	 case CSS_Unknown:
	   fprintf (output, "unknown, ");
	   break;
	 case CSS_USER_STYLE:
	   fprintf (output, "personal, ");
	   break;
	 case CSS_DOCUMENT_STYLE:
	   fprintf (output, "document, ");
	   break;
	 case CSS_EXTERNAL_STYLE:
	   fprintf (output, "external, ");
	   break;
	 case CSS_BROWSED_STYLE:
	   fprintf (output, "browsed, ");
	   break;
	 }

       if (css->name)
	 fprintf (output, "name %s", css->name);
       else
	 fprintf (output, "noname");
       if (css->url)
	 fprintf (output, " : url %s, docs : ", css->url);
       else
	 fprintf (output, " : no url , docs : ");

       for (i = 0; i <= DocumentTableLength; i++)
	 if (css->documents[i])
	   fprintf (output, "%d ", i);
       fprintf (output, "\n");
       css = css->NextCSS;
     }
}
#endif /* DEBUG_CSS */

/*----------------------------------------------------------------------
   AddCSS                                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AddCSS (CSSInfoPtr css)
#else
void                AddCSS (css)
CSSInfoPtr          css;
#endif
{
   if (css == NULL)
      return;
   css->NextCSS = ListCSS;
   ListCSS = css;
}

/*----------------------------------------------------------------------
   FreeCSS                                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeCSS (CSSInfoPtr css)
#else
void                FreeCSS (css)
CSSInfoPtr          css;
#endif
{
   if (css)
     {
	css->category = CSS_Unknown;
	css->NextCSS = NULL;	/* be safe ! */
	if (css->url)
	   TtaFreeMemory (css->url);
	if (css->css_rule)
	   TtaFreeMemory (css->css_rule);
	if (css->tempfile)
	  {
	     if (TtaFileExist (css->tempfile) != 0)
	       {
		  if (TtaFileUnlink (css->tempfile) != 0)
		    {
#ifdef DEBUG_CSS
		       fprintf (stderr, "cannot remove %s\n", css->tempfile);
#endif /* DEBUG_CSS */
		    }
	       }
	     TtaFreeMemory (css->tempfile);
	  }
	TtaFreeMemory (css);
     }
}

/*----------------------------------------------------------------------
   InitDocumentCSS                                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitDocumentCSS (Document doc)
#else
void                InitDocumentCSS (doc)
Document            doc;
#endif
{
#ifdef DEBUG_CSS
   fprintf (stderr, "InitDocumentCSS(doc %d) :\n", doc);
   PrintListCSS (stderr);
#endif /* DEBUG_CSS */
}

/*----------------------------------------------------------------------
   CleanDocumentCSS                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CleanDocumentCSS (Document doc)
#else
void                CleanDocumentCSS (doc)
Document            doc;
#endif
{
   CSSInfoPtr          prev = NULL;
   CSSInfoPtr          css = ListCSS;
   int                 i;
   boolean             in_use;

#ifdef DEBUG_CSS
   fprintf (stderr, "CleanDocumentCSS(doc %d) :\n", doc);
   PrintListCSS (stderr);
#endif /* DEBUG_CSS */

   while (css != NULL)
     {
	if (css == User_CSS)
	  {
	     /* Never, ever remove the Users's preference style ! */
	     prev = css;
	     css = prev->NextCSS;
	     continue;
	  }
	if (css->documents[doc])
	  {
	     in_use = FALSE;
	     css->documents[doc] = FALSE;
	     for (i = 0; i <= DocumentTableLength; i++)
		if (css->documents[i])
		  {
		     in_use = (boolean) TRUE;
		     break;
		  }
	     if (in_use)
	       {
		  if (prev == NULL)
		    {
		       ListCSS = css->NextCSS;
		       FreeCSS (css);
		       css = ListCSS;
		    }
		  else
		    {
		       prev->NextCSS = css->NextCSS;
		       FreeCSS (css);
		       css = prev->NextCSS;
		    }
	       }
	  }
	else
	  {
	     prev = css;
	     css = prev->NextCSS;
	  }
     }

#ifdef DEBUG_CSS
   PrintListCSS (stderr);
#endif /* DEBUG_CSS */

   /* if the CSS form was opened for this document, close it */
   if (currentDocument == doc)
     {
	TtaDestroyDialogue (BaseCSSDialog + FormCSS);
	currentDocument = -1;
     }
}

/*----------------------------------------------------------------------
   NewCSS                                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CSSInfoPtr          NewCSS (void)
#else
CSSInfoPtr          NewCSS ()
#endif
{
   CSSInfoPtr          css;
   int                 i;

   css = (CSSInfoPtr) TtaGetMemory (sizeof (CSSInfo));

   css->NextCSS = NULL;
   css->state = CSS_STATE_Unknown;
   css->name = NULL;
   css->tempfile = NULL;
   css->url = NULL;
   css->pschema = NULL;
   css->mschema = NULL;
   css->category = CSS_Unknown;
   css->css_rule = NULL;
   for (i = 0; i <= DocumentTableLength; i++)
      css->documents[i] = FALSE;
   css->view_background_color = -1;
   css->magnification = -1000;
   return (css);
}


/*----------------------------------------------------------------------
   SearchCSS                                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CSSInfoPtr          SearchCSS (Document doc, CSSCategory category, char *url)
#else
CSSInfoPtr          SearchCSS (doc, category, url)
Document            doc;
CSSCategory         category;
char               *url;
#endif
{
  CSSInfoPtr          css = ListCSS;

  while (css != NULL)
    {
      if (css->documents[doc])
	{
	  switch (category)
	    {
	    case CSS_Unknown:
	    case CSS_EXTERNAL_STYLE:
	    case CSS_BROWSED_STYLE:
	      if (css->url != NULL && !strcmp (url, css->url))
		return (css);
	      break;
	    case CSS_DOCUMENT_STYLE:
	      if (category == css->category)
		return (css);
	      break;
	    case CSS_USER_STYLE:
	      if (category == css->category)
		return (css);
	      break;
	    default:
	      break;
	    }
	}
      css = css->NextCSS;
    }
  return (NULL);
}

/*----------------------------------------------------------------------
   ClearCSS                                                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ClearCSS (CSSInfoPtr css)
#else
void                ClearCSS (css)
CSSInfoPtr          css;
#endif
{
   PRuleInfoPtr        list, rpi;

   /*
    * extract the list of presentation rules from the Generic
    * Presentation schema. Remove all the associated rules in
    * the internal P structures, and free the list.
    */
   rpi = list = PSchema2RPI (currentDocument, css);
   while (rpi != NULL)
     {
	RemoveRPI (currentDocument, rpi);
	rpi = rpi->NextRPI;
     }
   CleanListRPI (&list);
}

/*----------------------------------------------------------------------
   RebuildCSS : rebuild the whole internal structures with the    
   original CSS rules.                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RebuildCSS (CSSInfoPtr css)
#else
void                RebuildCSS (css)
CSSInfoPtr          css;
#endif
{
  if (css->css_rule == NULL)
    {
#ifdef DEBUG_CSS
      fprintf (stderr, "RebuildCSS %s ABORTED : no css_rule ... GASP !\n", css->url);
#endif /* DEBUG_CSS */
      return;
    }
  if (css->pschema == NULL)
    {
#ifdef DEBUG_CSS
      fprintf (stderr, "RebuildCSS %s ABORTED : no pschema ... GASP !\n", css->url);
#endif /* DEBUG_CSS */
      return;
    }
  switch (css->state)
    {
    case CSS_STATE_Unknown:
#ifdef DEBUG_CSS
      fprintf (stderr, "RebuildCSS %s : unknown state\n", css->url);
#endif /* DEBUG_CSS */
      break;
    case CSS_STATE_Modified:
      break;
    case CSS_STATE_Unmodified:
#ifdef DEBUG_CSS
      fprintf (stderr, "RebuildCSS %s : unmodified\n", css->url);
#endif /* DEBUG_CSS */
      break;
    }
  ClearCSS (css);
  switch (css->category)
    {
    case CSS_Unknown:
    case CSS_EXTERNAL_STYLE:
    case CSS_USER_STYLE:
      ParseHTMLStyleSheet (css->css_rule, currentDocument, css);
      break;
    case CSS_DOCUMENT_STYLE:
      ParseHTMLStyleHeader (NULL, css->css_rule, currentDocument, TRUE);
      break;
    case CSS_BROWSED_STYLE:
      break;
    }
}

/*----------------------------------------------------------------------
   GetDocumentStyle : Returns the current CSS document style.     
   Should never return NULL, a new one is allocated and   
   inserted if necessary. In this case the pschema field  
   remains NULL.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CSSInfoPtr          GetDocumentStyle (Document doc)
#else
CSSInfoPtr          GetDocumentStyle (doc)
Document            doc;
#endif
{
  CSSInfoPtr          css = ListCSS;

  while (css != NULL)
    {
      if (css->documents[doc])
	{
	  switch (css->category)
	    {
	    case CSS_Unknown:
	    case CSS_USER_STYLE:
	    case CSS_EXTERNAL_STYLE:
	    case CSS_BROWSED_STYLE:
	      break;
	    case CSS_DOCUMENT_STYLE:
	      return (css);
	    }
	}
      css = css->NextCSS;
    }
  css = NewCSS ();
  css->name = TtaGetMessage (AMAYA, AM_DOC_STYLE);
  css->category = CSS_DOCUMENT_STYLE;
  css->documents[doc] = TRUE;
  AddCSS (css);
  return (css);
}
/*----------------------------------------------------------------------
   GetDocumentGenericPresentation                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PSchema         GetDocumentGenericPresentation (Document doc, char *structName)
#else
PSchema         GetDocumentGenericPresentation (doc, structName)
Document        doc;
char           *structName;
#endif
{
  CSSInfoPtr          css = ListCSS;

  while (css != NULL)
    {
      if (css->documents[doc])
	{
	  switch (css->category)
	    {
	    case CSS_Unknown:
	    case CSS_USER_STYLE:
	    case CSS_EXTERNAL_STYLE:
	    case CSS_BROWSED_STYLE:
	      break;
	    case CSS_DOCUMENT_STYLE:
	      if (!strcmp(structName, "HTML"))
		return (css->pschema);
	      else
		return (css->mschema);
	    }
	}
      css = css->NextCSS;
    }
  return (NULL);
}

/*----------------------------------------------------------------------
   GetUserGenericPresentation                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CSSInfoPtr          GetUserGenericPresentation (void)
#else
CSSInfoPtr          GetUserGenericPresentation ()
#endif
{
  CSSInfoPtr          css = ListCSS;

  while (css != NULL)
    {
      switch (css->category)
	{
	case CSS_Unknown:
	case CSS_DOCUMENT_STYLE:
	case CSS_EXTERNAL_STYLE:
	case CSS_BROWSED_STYLE:
	  break;
	case CSS_USER_STYLE:
	  return (css);
	  break;
	}
      css = css->NextCSS;
    }
  return (NULL);
}

/*----------------------------------------------------------------------
   RebuildHTMLStyleHeader : Rebuild the Style header of a document 
   using the current state of the associated PSchema.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RebuildHTMLStyleHeader (Document doc)
#else
void                RebuildHTMLStyleHeader (doc)
Document            doc;

#endif
{
  ElementType         elType;
  Element             el, child, next;
  Attribute           at;
  AttributeType       atType;
  PRuleInfoPtr        list, rpi;
  CSSInfoPtr          css;
  int                 modified;

  /* prerequisite : store the modified status of the document */
  modified = TtaIsDocumentModified (doc);
  /*
   * First search the Styles subtree in the HTML structure and
   * destroy its children.
   */
  elType.ElSSchema = TtaGetDocumentSSchema (doc);
  elType.ElTypeNum = HTML_EL_Styles;
  el = TtaSearchTypedElement (elType, SearchInTree, TtaGetMainRoot (doc));
  if (el != NULL)
    {
      child = TtaGetFirstChild (el);
      while (child)
	{
	  next = child;
	  TtaNextSibling (&next);
	  TtaDeleteTree (child, doc);
	  child = next;
	}
    }
  else
    /* create the corresponding Styles header in the document structure */
    el = CreateWWWElement (doc, HTML_EL_Styles);
  
  /*
   * extract presentation rules from the Generic Presentation schema */
  css = GetDocumentStyle (doc);
  if (css->pschema == NULL)
    {
#ifdef DEBUG_CSS
      fprintf (stderr, "RebuildHTMLStyleHeader(%d) : no generic presentation\n", doc);
#endif /* DEBUG_CSS */
      return;
    }
  list = PSchema2RPI (doc, css);
  if (list == NULL)
    return;
  rpi = list;
  elType.ElTypeNum = HTML_EL_TEXT_UNIT;
  while (rpi != NULL)
    {
      /* create the element StyleRule */
      el = CreateNewWWWElement (doc, HTML_EL_StyleRule);
      
      /* attach a text child containing the CSS value */
      child = TtaNewElement (doc, elType);
      TtaInsertFirstChild (&child, el, doc);
      TtaSetTextContent (child, rpi->css_rule, TtaGetDefaultLanguage (), doc);
      
      /* attach the Selector attribute and set it content */
      atType.AttrSSchema = TtaGetDocumentSSchema (doc);
      atType.AttrTypeNum = HTML_ATTR_Selector;
      at = TtaNewAttribute (atType);
      TtaAttachAttribute (el, at, doc);
      TtaSetAttributeText (at, rpi->selector, el, doc);
      
      rpi = rpi->NextRPI;
    }
  
  CleanListRPI (&list);
  /* final : restore the modified status of the document */
  if (! modified)
    TtaSetDocumentUnmodified(doc);
}

/*----------------------------------------------------------------------
   ParseHTMLStyleHeader : parse an CSS Style description stored  
   in the header of a HTML document                               
   We expect the style string to be of the form :                    
   [                                                                 
   e.g: H2 { color: blue } pinky { color: pink }                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ParseHTMLStyleHeader (Element el, char *cssRule, Document doc, boolean rebuild)
#else
void                ParseHTMLStyleHeader (el, cssRule, doc, rebuild)
Element             el;
char               *cssRule;
Document            doc;
boolean             rebuild;
#endif
{
  AttributeType       newAtType;
  Attribute           newAt;
  PSchema             pSchema;
  PSchema             cour, prev = NULL;
  CSSInfoPtr          css;
  char               *decl_end;
  char                saved;
  char               *buf;
  int                 len;

  /*
   * Create a specific Presentation structure for this document
   * and add it at the end of the list (sorted by increased priority).
   */
  css = GetDocumentStyle (doc);
  if (css->pschema == NULL)
    {
      pSchema = TtaNewPSchema ();
      css->name = TtaGetMessage (AMAYA, AM_DOC_STYLE);
      css->pschema = pSchema;
      css->category = CSS_DOCUMENT_STYLE;
      css->documents[doc] = TRUE;
      css->url = TtaStrdup (DocumentURLs[doc]);
      css->css_rule = TtaStrdup (cssRule);
      cour = TtaGetFirstPSchema (doc, NULL);
      while (cour != NULL)
	{
	  prev = cour;
	  TtaNextPSchema (&cour, doc, NULL);
	}
      TtaAddPSchema (pSchema, prev, FALSE, doc, NULL);
    }
  else
    {
      pSchema = css->pschema;
      if (!rebuild)
	{
	  if (css->css_rule == NULL)
	    css->css_rule = TtaStrdup (cssRule);
	  else
	    {
	      /*
	       * concatenate the existing css rule with the next fragment.
	       */
	      buf = css->css_rule;
	      len = strlen (css->css_rule) + 1 + strlen (cssRule) + 1;
	      css->css_rule = TtaGetMemory (len);
	      if (css->css_rule == NULL)
		css->css_rule = buf;
	      else
		{
		  sprintf (css->css_rule, "%s\n%s", buf, cssRule);
		  TtaFreeMemory (buf);
		}
	    }
	}
    }
  
  /*
   * Set the attribute style to the content of the string.
   */
  if (el != NULL)
    {
      newAtType.AttrSSchema = TtaGetDocumentSSchema (doc);
      newAtType.AttrTypeNum = HTML_ATTR_Style_;
      newAt = TtaGetAttribute (el, newAtType);
      if (newAt == NULL)
	{
	  newAt = TtaNewAttribute (newAtType);
	  TtaAttachAttribute (el, newAt, doc);
	}
      TtaSetAttributeText (newAt, cssRule, el, doc);
    }
  /*
   * now, parse the the whole string ...
   * we need to split it in a set of style declaration.
   */
  cssRule = SkipBlanks (cssRule);
  while (*cssRule != EOS)
    {
      cssRule = SkipBlanks (cssRule);
      decl_end = cssRule;
      while (*decl_end != EOS && *decl_end != '}')
	decl_end++;
      if (*decl_end == EOS)
	{
	  fprintf (stderr, "Invalid STYLE header : %s\n", cssRule);
	  return;
	}
      /*
       * add a 0 to split, treat the declaration,
       * put back the char and continue from this point.
       */
      decl_end++;
      saved = *decl_end;
      *decl_end = EOS;
      ParseStyleDeclaration (el, cssRule, doc, css);
      
      *decl_end = saved;
      cssRule = decl_end;
      cssRule = SkipBlanks (cssRule);
    }
  
  /*
   * Rebuild the Styles tree in the Document structure.
   */
  if (rebuild)
    RebuildHTMLStyleHeader (doc);
  if (NonPPresentChanged)
    ApplyExtraPresentation (doc);
  
}

/*----------------------------------------------------------------------
   LoadHTMLStyleSheet : Load an external Style Sheet found at the  
   URL given in argument.                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                LoadHTMLStyleSheet (char *URL, Document doc)
#else
void                LoadHTMLStyleSheet (URL, doc)
char               *URL;
Document            doc;

#endif
{
  struct stat         buf;
  FILE               *res;
  PSchema             pSchema, prev;
  CSSInfoPtr          css;
  char                tempfile[MAX_LENGTH];
  char                tempname[MAX_LENGTH];
  char                tempURL[MAX_LENGTH];
  char               *buffer = NULL;
  int                 len;
  int                 local = FALSE;
  int                 toparse;

  if (TtaGetViewFrame (doc, 1) != 0)
    {
      /* this document is displayed -> load the CSS */
      tempfile[0] = EOS;
      NormalizeURL (URL, doc, tempURL, tempname, NULL);
      
      if (IsW3Path (tempURL))
	{
	  /* check against double inclusion */
	  css = SearchCSS (doc, CSS_EXTERNAL_STYLE, tempURL);
	  if (css != NULL)
	    return;
	  
	  toparse = GetObjectWWW (doc, tempURL, NULL, tempfile, AMAYA_SYNC, NULL, NULL, NULL, NULL, NO, NULL);
	  if (toparse)
	    {
	      fprintf (stderr, "LoadHTMLStyleSheet \"%s\" failed\n", URL);
	      return;
	    }
	}
      else
	{
	  local = TRUE;
	  strcpy (tempfile, tempURL);
	}

      if (tempfile[0] == EOS)
	{
	  fprintf (stderr, "LoadHTMLStyleSheet \"%s\" failed\n", URL);
	  return;
	}
      /* load the resulting file in memory */
      res = fopen (tempfile, "r");
      if (res == NULL)
	{
	  fprintf (stderr, "LoadHTMLStyleSheet \"%s\" : cannot open file\n", URL);
	  if (!local)
	    TtaFileUnlink (tempfile);
	  return;
	}
      if (fstat (fileno (res), &buf))
	{
	  fprintf (stderr, "LoadHTMLStyleSheet \"%s\" : cannot stat file\n", URL);
	  fclose (res);
	  if (!local)
	    TtaFileUnlink (tempfile);
	  return;
	}
      buffer = (char *) TtaGetMemory (buf.st_size + 1000);
      if (buffer == NULL)
	{
	  fprintf (stderr, "LoadHTMLStyleSheet \"%s\" : out of mem\n", URL);
	  fclose (res);
	  if (!local)
	    TtaFileUnlink (tempfile);
	  return;
	}
      len = fread (buffer, buf.st_size, 1, res);
      if (len != 1)
	{
	  fprintf (stderr, "LoadHTMLStyleSheet \"%s\" : read failed\n", URL);
	  fclose (res);
	  if (!local)
	    TtaFileUnlink (tempfile);
	  TtaFreeMemory (buffer);
	  return;
	}
      buffer[buf.st_size] = 0;
      fclose (res);
      if (!local)
	TtaFileUnlink (tempfile);

      /*
       * allocate a new Presentation structure, parse the whole thing
       * and free the buffer.
       */
      pSchema = TtaNewPSchema ();
      prev = TtaGetFirstPSchema (doc, NULL);
      TtaAddPSchema (pSchema, prev, TRUE, doc, NULL);
      css = NewCSS ();
      css->tempfile = TtaStrdup (tempfile);
      css->name = "External Style";
      css->category = CSS_EXTERNAL_STYLE;
      css->pschema = pSchema;
      css->documents[doc] = TRUE;
      css->url = TtaStrdup (URL);
      css->css_rule = TtaStrdup (buffer);
      css->state = CSS_STATE_Unmodified;
      AddCSS (css);
      ParseHTMLStyleSheet (buffer, doc, css);
      TtaFreeMemory (buffer);
    }
}

/*----------------------------------------------------------------------
   LoadHTMLExternalStyleSheet : Load an external Style Sheet found 
   at the URL given in argument, but do not link it to the       
   document.                                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                LoadHTMLExternalStyleSheet (char *URL, Document doc, int merge)
#else
void                LoadHTMLExternalStyleSheet (URL, doc, int merge)
char               *URL;
Document            doc;
int                 merge;
#endif
{
   char                tempfile[MAX_LENGTH];
   char                tempname[MAX_LENGTH];
   char                tempURL[MAX_LENGTH];
   struct stat         buf;
   char               *buffer = NULL;
   FILE               *res;
   int                 len;
   PSchema             first, pSchema;
   CSSInfoPtr          css, user;
   Element             link;
   Attribute           at;
   AttributeType       atType;
   char               *proto, *host, *dir, *file;
   boolean             toparse;
   int                 local = FALSE;

   strcpy (tempURL, URL);
   ExplodeURL (tempURL, &proto, &host, &dir, &file);

   /* check against double loading */
   if (merge)
      css = SearchCSS (doc, CSS_EXTERNAL_STYLE, URL);
   else
      css = SearchCSS (doc, CSS_BROWSED_STYLE, URL);
   if (css != NULL)
      return;

   /* load the CSS */
   tempfile[0] = EOS;
   NormalizeURL (URL, doc, tempURL, tempname, NULL);

   if (IsW3Path (tempURL))
     {
	/* check against double inclusion */
	css = SearchCSS (doc, CSS_EXTERNAL_STYLE, tempURL);
	if (css != NULL)
	   return;

	toparse = GetObjectWWW (doc, tempURL, NULL, tempfile, AMAYA_SYNC, NULL, NULL, NULL, NULL, NO, NULL);
	if (toparse)
	  {
	     fprintf (stderr, "LoadHTMLExternalStyleSheet \"%s\" failed\n", URL);
	     return;
	  }
     }
   else
     {
	local = TRUE;
	strcpy (tempfile, URL);
     }
   if (tempfile[0] == EOS)
     {
	fprintf (stderr, "LoadHTMLExternalStyleSheet \"%s\" failed\n", URL);
	return;
     }
   /*
    * load the resulting file in memory.
    */

   res = fopen (tempfile, "r");
   if (res == NULL)
     {
	fprintf (stderr, "LoadHTMLExternalStyleSheet \"%s\" : cannot open file\n", URL);
	return;
     }
   if (fstat (fileno (res), &buf))
     {
	fprintf (stderr, "LoadHTMLExternalStyleSheet \"%s\" : cannot stat file\n", URL);
	fclose (res);
	return;
     }
   buffer = (char*) TtaGetMemory (buf.st_size + 1000);
   if (buffer == NULL)
     {
	fprintf (stderr, "LoadHTMLExternalStyleSheet \"%s\" : out of mem\n", URL);
	fclose (res);
	return;
     }
   len = fread (buffer, buf.st_size, 1, res);
   if (len != 1)
     {
	fprintf (stderr, "LoadHTMLExternalStyleSheet \"%s\" : read failed\n", URL);
	fclose (res);
	return;
     }
   buffer[buf.st_size] = 0;
   fclose (res);
   if (!local)
      TtaFileUnlink (tempfile);

   /*
    * allocate a new Presentation structure, parse the whole thing
    * and free the buffer.
    */
   pSchema = TtaNewPSchema ();
   css = NewCSS ();
   css->name = NULL;
   css->tempfile = TtaStrdup (tempfile);
   if (merge)
      css->category = CSS_EXTERNAL_STYLE;
   else
      css->category = CSS_BROWSED_STYLE;
   css->pschema = pSchema;
   css->documents[doc] = TRUE;
   css->url = TtaStrdup (URL);
   css->css_rule = TtaStrdup (buffer);
   css->state = CSS_STATE_Unmodified;
   AddCSS (css);
   ParseHTMLStyleSheet (buffer, doc, css);
   TtaFreeMemory (buffer);

   if (merge)
     {
	/*
	 * Create the LINK in the document head. And set up the
	 * REL attribute corresponding to LINK.
	 */
	link = CreateNewWWWElement (doc, HTML_EL_LINK);
	if (link)
	  {
	     atType.AttrSSchema = TtaGetDocumentSSchema (doc);
	     atType.AttrTypeNum = HTML_ATTR_HREF_;
	     at = TtaGetAttribute (link, atType);
	     if (!at)
	       {
		  at = TtaNewAttribute (atType);
		  TtaAttachAttribute (link, at, doc);
	       }
	     TtaSetAttributeText (at, URL, link, doc);

	     atType.AttrSSchema = TtaGetDocumentSSchema (doc);
	     atType.AttrTypeNum = HTML_ATTR_REL;
	     at = TtaGetAttribute (link, atType);
	     if (!at)
	       {
		  at = TtaNewAttribute (atType);
		  TtaAttachAttribute (link, at, doc);
	       }
	     TtaSetAttributeText (at, "STYLESHEET", link, doc);
	  }
	/*
	 * insert the PSchema in the list associated to the document,
	 * it's priority is set to be the least one among the external
	 * styles sheet referenced, just after the user's preferences
	 * if any. Enforce the recalculation of the image.
	 */
	first = TtaGetFirstPSchema (doc, NULL);
	user = GetUserGenericPresentation ();
	if ((user != NULL) && (user->pschema == first))
	     TtaAddPSchema (pSchema, first, FALSE, doc, NULL);
	else
	     TtaAddPSchema (pSchema, first, TRUE, doc, NULL);
     }
}

/*----------------------------------------------------------------------
   LoadUserStyleSheet : Load the user Style Sheet found in it's    
   home directory or the default one in THOTDIR.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                LoadUserStyleSheet (Document doc)
#else
void                LoadUserStyleSheet (doc)
Document            doc;
#endif
{
  CSSInfoPtr          css;
  struct stat         buf;
  FILE               *res;
  PSchema             pSchema, prev;
  char                tempfile[MAX_LENGTH];
  char               *buffer = NULL;
  char               *home;
  char               *thotdir;
  int                 len;

  if (User_CSS != NULL)
    ApplyExtraPresentation (doc);

  thotdir = TtaGetEnvString ("THOTDIR");
  home = TtaGetEnvString ("HOME");
  /* try to load the user preferences */
  if (home)
    {
      sprintf (tempfile, "%s%s.%s.css", home, DIR_STR, HTAppName);
      res = fopen (tempfile, "r");
      if (res != NULL)
	{
	  if (fstat (fileno (res), &buf))
	    fclose (res);
	  else
	    {
	      buffer = (char*) TtaGetMemory (buf.st_size + 1000);
	      if (buffer == NULL)
		fclose (res);
	      else
		{
		  len = fread (buffer, buf.st_size, 1, res);
		  if (len != 1)
		    {
		      TtaFreeMemory (buffer);
		      buffer = NULL;
		      fclose (res);
		    }
		  else
		    {
		      buffer[buf.st_size] = 0;
		      fclose (res);
		    }
		}
	    }
	}
    }

  if ((buffer == NULL) && (thotdir))
    {
      /* file not found */
      sprintf (tempfile, "%s%samaya%s%s.css", thotdir, DIR_STR, DIR_STR, HTAppName);
      res = fopen (tempfile, "r");
      if (res != NULL)
	{
	  if (fstat (fileno (res), &buf))
	    fclose (res);
	  else
	    {
	      buffer = (char*) TtaGetMemory (buf.st_size + 1000);
	      if (buffer == NULL)
		fclose (res);
	      else
		{
		  len = fread (buffer, buf.st_size, 1, res);
		  if (len != 1)
		    {
		      TtaFreeMemory (buffer);
		      buffer = NULL;
		      fclose (res);
		    }
		  else
		    {
		      buffer[buf.st_size] = 0;
		      fclose (res);
		    }
		}
	    }
	}
    }

  /*
   * allocate a new Presentation structure, parse the whole thing
   * and free the buffer.
   */
  if (buffer != NULL)
    {
      css = NewCSS ();
      css->name = TtaGetMessage (AMAYA, AM_USER_PREFERENCES);
      css->category = CSS_USER_STYLE;
      css->pschema = TtaNewPSchema ();
      css->mschema = NULL;
      css->documents[doc] = TRUE;
      pSchema = TtaGetFirstPSchema (doc, NULL);
      prev = NULL;
      while (pSchema != NULL)
	{
	  prev = pSchema;
	  TtaNextPSchema (&pSchema, doc, NULL);
	}
      TtaAddPSchema (css->pschema, prev, TRUE, doc, NULL);
      css->state = CSS_STATE_Unmodified;
      css->url = TtaStrdup (tempfile);
      css->css_rule = buffer;
      AddCSS (css);
      User_CSS = css;
      ParseHTMLStyleSheet (buffer, doc, css);
    }
}


/*----------------------------------------------------------------------
   CSSSetMagnification : called by the parser when a Magnification 
   is specified by a CSS, this is not supported at the P level. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CSSSetMagnification (Document doc, PSchema gpres, int zoom)
#else
void                CSSSetMagnification (doc, gpres, zoom)
Document            doc;
PSchema             gpres;
int                 zoom;

#endif
{
   CSSInfoPtr          css = ListCSS;

   while (css != NULL)
     {
	if (css->pschema == gpres)
	  {
	     css->magnification = zoom;
	     NonPPresentChanged = TRUE;
	     return;
	  }
	css = css->NextCSS;
     }
}

/*----------------------------------------------------------------------
   ApplyFinalStyle : Change the presentation attributes of the     
   whole document to reflect the style rules. To respect   
   the order imposed by CSS, the User's defined rule are   
   stored at the head, then the external style sheets,     
   and last the document own styles if any.                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ApplyFinalStyle (Document doc)
#else
void                ApplyFinalStyle (doc)
Document            doc;
#endif
{

  /*RebuildHTMLStyleHeader (doc);*/
   LoadUserStyleSheet (doc);
}

/*----------------------------------------------------------------------
   MergeNewCSS : parse a set of CSS rules and add them to an 
   existing pschema.                                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         MergeNewCSS (char *cssRule, Document doc, CSSInfoPtr css)
#else
static void         MergeNewCSS (cssRule, doc, css)
char               *cssRule;
Document            doc;
CSSInfoPtr          css;
#endif
{
   char               *decl_end;
   char                saved;
#if 0
   CSSInfoPtr          css;
#endif

#ifdef DEBUG_CSS
   fprintf (stderr, "MergeNewCSS(\"%s\",..)\n", cssRule);
#endif /* DEBUG_CSS */

   /*
    * now, parse the the whole string ...
    * we need to split it in a set of style declaration.
    */
   cssRule = SkipBlanks (cssRule);
   while (*cssRule != 0)
     {
	cssRule = SkipBlanks (cssRule);
	decl_end = cssRule;
	while ((*decl_end != 0) && (*decl_end != '}'))
	   decl_end++;
	if (*decl_end == 0)
	  {
	     fprintf (stderr, "Invalid STYLE : %s\n", cssRule);
	     return;
	  }
	/*
	 * add a 0 to split, treat the declaration,
	 * put back the char and continue from this point.
	 */
	decl_end++;
	saved = *decl_end;
	*decl_end = 0;
	ParseStyleDeclaration (NULL, cssRule, doc, css);

	*decl_end = saved;
	cssRule = decl_end;
	cssRule = SkipBlanks (cssRule);
     }

   /*
    * Rebuild the Styles tree in the Document structure.
    */
   RebuildHTMLStyleHeader (doc);

}

/*----------------------------------------------------------------------
   RemoveCSS : remove an existing CSS file from a document.        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RemoveCSS (char *name, Document doc)
#else
void                RemoveCSS (name, doc)
char               *cssRule;
Document            doc;

#endif
{
   CSSInfoPtr          css = NULL, prev;
   int                 i;

   if (name == NULL)
      return;
   if (name[0] == EOS)
      return;

#ifdef DEBUG_CSS
   fprintf (stderr, "RemoveCSS(\"%s\",%d)\n", name, doc);
#endif /* DEBUG_CSS */

   /*
    * search the css structure associated to the selected name.
    */
   if (!strcmp (name, TtaGetMessage (AMAYA, AM_DOC_STYLE)))
      css = SearchCSS (doc, CSS_DOCUMENT_STYLE, name);
   else if (!strcmp (name, TtaGetMessage (AMAYA, AM_USER_PREFERENCES)))
      css = SearchCSS (doc, CSS_USER_STYLE, name);
   else
      css = SearchCSS (doc, CSS_EXTERNAL_STYLE, name);

   if (css == NULL)
      return;

   /*
    * depending on the kind of document, remove it from the PSchema
    * chain associated to the current document.
    */
   TtaRemovePSchema (css->pschema, doc, NULL);
   css->documents[doc] = FALSE;
   if (css->category != CSS_USER_STYLE)
     {
	if (css->category == CSS_DOCUMENT_STYLE)
	  {
	     css->pschema = NULL;
	     RebuildHTMLStyleHeader (doc);
	  }
	for (i = 0; i <= DocumentTableLength; i++)
	   if (css->documents[i])
	      break;
	if (i > DocumentTableLength)
	  {
	     css->pschema = NULL;
	     prev = ListCSS;
	     if (prev == css)
	       {
		  ListCSS = ListCSS->NextCSS;
	       }
	     else
	       {
		  while ((prev != NULL) && (prev->NextCSS != css))
		     prev = prev->NextCSS;
		  if (prev->NextCSS == css)
		    {
		       if (prev == ListCSS)
			  ListCSS = css->NextCSS;
		       else
			  prev->NextCSS = css->NextCSS;
		    }
	       }
	     FreeCSS (css);
	  }
     }
}

/*----------------------------------------------------------------------
   SearchRPISel : Search an RPI based on the value of it's selector  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PRuleInfoPtr        SearchRPISel (char *selector, PRuleInfoPtr list)
#else
PRuleInfoPtr        SearchRPISel (selector, list)
char               *selector;
PRuleInfoPtr        list;

#endif
{
   while (list != NULL)
     {
	if (!strcasecmp (list->selector, selector))
	   return (list);
	list = list->NextRPI;
     }
   return (NULL);
}

/*----------------------------------------------------------------------
   SelectRPIEntry : show the current entry selected, left, right or  
   bottom.                                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SelectRPIEntry (char which, int index, char *value)
#else
void                SelectRPIEntry (which, index, value)
char                which;
int                 index;
char               *value;
#endif
{
   PRuleInfoPtr        rpi;

   switch (which)
	 {
	    case 'L':
	       /* unselect the Right selector entry */
	       currentRRPI[0] = EOS;
	       TtaSetSelector (BaseCSSDialog + RPIRList, -1, "");
	       RListRPIIndex = -1;

	       /* set up the Left selector entry and the bottom CSS rule text */
	       if (value)
		 {
		   rpi = SearchRPISel (value, LListRPI);
		   if (rpi)
		     /* !!!! check overflow !!!! */
		     sprintf (currentBRPI, "%s { %s }", rpi->selector, rpi->css_rule);
		   else
		     currentBRPI[0] = EOS;
		   strcpy (currentLRPI, value);
		 }
	       else
		 {
		   currentBRPI[0] = EOS;
		   currentLRPI[0] = EOS;
		 }
	       if (index >= 0)
		 {
		   TtaSetSelector (BaseCSSDialog + RPILList, index, NULL);
		   LListRPIIndex = index;
		 }
	       else
		 {
		   TtaSetSelector (BaseCSSDialog + RPILList, -1, "");
		   LListRPIIndex = -1;
		 }

	       /* show the CSS code for that item */
	       TtaSetTextForm (BaseCSSDialog + RPIText, currentBRPI);

	       break;
	    case 'R':
	       /* unselect the Left selector entry */
	       currentLRPI[0] = EOS;
	       TtaSetSelector (BaseCSSDialog + RPILList, -1, "");
	       LListRPIIndex = -1;

	       /* set up the Right selector entry and the bottom CSS rule text */
	       if (value)
		 {
		    rpi = SearchRPISel (value, RListRPI);
		    if (rpi)
		      {
			 /* !!!! check overflow !!!! */
			 sprintf (currentBRPI, "%s { %s }",
				  rpi->selector, rpi->css_rule);
		      }
		    else
		      {
			 currentBRPI[0] = EOS;
		      }
		    strcpy (currentRRPI, value);
		 }
	       else
		 {
		    currentBRPI[0] = EOS;
		    currentRRPI[0] = EOS;
		 }
	       if (index >= 0)
		 {
		    TtaSetSelector (BaseCSSDialog + RPIRList, index, NULL);
		    RListRPIIndex = index;
		 }
	       else
		 {
		    TtaSetSelector (BaseCSSDialog + RPIRList, -1, "");
		    RListRPIIndex = -1;
		 }

	       /* show the CSS code for that item */
	       TtaSetTextForm (BaseCSSDialog + RPIText, currentBRPI);

	       break;
	    case 'B':
	       /* unselect the Left and Right selector entry */
	       currentLRPI[0] = EOS;
	       TtaSetSelector (BaseCSSDialog + RPILList, -1, "");
	       LListRPIIndex = -1;
	       currentRRPI[0] = EOS;
	       TtaSetSelector (BaseCSSDialog + RPIRList, -1, "");
	       RListRPIIndex = -1;

	       /* store the bottom CSS rule text */
	       strcpy (currentBRPI, value);

	       break;
	 }
}

/*----------------------------------------------------------------------
   BuildCSSList : Build the whole list of CSS in use by a document   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 BuildCSSList (Document doc, char *buf, int size, char *first)
#else
int                 BuildCSSList (doc, buf, size, first)
Document            doc;
char               *buf;
int                 size;
char               *first;
#endif
{
   int                 remainder = size;
   int                 len;
   int                 nb = 0;
   int                 index = 0;
   char               *val = NULL;
   CSSInfoPtr          css;

   if (doc < 0 || doc > DocumentTableLength)
     return (0);

   /* add the first element if specified */
   buf[0] = EOS;
#ifdef IV
   if (first)
     {
	strcpy (&buf[index], first);
	len = strlen (first);
	len++;
	remainder -= len;
	index += len;
	nb++;
     }
#endif
   css = ListCSS;
   while (css != NULL)
     {
       if (css->documents[doc])
	 {
	   switch (css->category)
	     {
	     case CSS_Unknown:
	       val = css->url;
	       break;
	     case CSS_USER_STYLE:
	       val = TtaGetMessage (AMAYA, AM_USER_PREFERENCES);
	       break;
	     case CSS_DOCUMENT_STYLE:
	       val = TtaGetMessage (AMAYA, AM_DOC_STYLE);
	       break;
	     case CSS_EXTERNAL_STYLE:
	       val = css->url;
	       break;
	     default:
	       val = NULL;
	     }
	 }

       if (val != NULL /*&& strcmp (val, buf)*/)
	 /* ensure unicity of first */
	 {
	   len = strlen (val);
	   len++;
	   if (len >= remainder)
	     {
	       MSG ("BuildCSSList : Too many styles\n");
	       break;
	     }
	   strcpy (&buf[index], val);
	   remainder -= len;
	   index += len;
	   nb++;
	 }
       css = css->NextCSS;
     }

#ifdef DEBUG_CSS
   fprintf (stderr, "BuildCSSList : found %d CSS\n", nb);
#endif /* DEBUG_CSS */
   return (nb);
}

/*----------------------------------------------------------------------
   GetlistEntry                                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char               *GetlistEntry (char *list, int entry)
#else
char               *GetlistEntry (list, entry)
char               *list;
int                 entry;
#endif
{
   while (entry > 0)
     {
	while (*++list) ;
	list++;
	entry--;
     }
   return (list);
}

/*----------------------------------------------------------------------
   RedrawLCSS                                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RedrawLCSS (char *name)
#else
void                RedrawLCSS (name)
char               *name;
#endif
{
   char                buffer[3000];
   char                nb_css;
   int                 index;
   Document            doc = currentDocument;

   if (name && currentRCSS && !strcmp (currentRCSS, name))
     {
	TtaSetSelector (BaseCSSDialog + CSSLName, CSSLEntry, "");
	return;
     }
   /* rebuild the list and redraw the CSS selector */
   nb_css = BuildCSSList (doc, buffer, 3000, name);
   if (nb_css < 1)
     return;
   TtaNewSelector (BaseCSSDialog + CSSLName, BaseCSSDialog + FormCSS,
		   TtaGetMessage (AMAYA, AM_CSS_FILE_1), nb_css,
		   buffer, 3, NULL, FALSE, TRUE);

   if (!name)
     name = GetlistEntry (buffer, 0);
   /* look if the name given is an existing CSS element */
   for (index = 0; index < nb_css; index++)
     if (!strcmp (name, GetlistEntry (buffer, index)))
       break;
   
   if (index >= nb_css)
     {
       MSG ("non-existent CSS selected\n");
       return;
     }

   if (currentLCSS)
     TtaFreeMemory (currentLCSS);
   currentLCSS = TtaStrdup (name);

   TtaSetSelector (BaseCSSDialog + CSSLName, index, NULL);
   CSSLEntry = index;
   if (!strcmp (name, TtaGetMessage (AMAYA, AM_DOC_STYLE)))
     LCSS = SearchCSS (doc, CSS_DOCUMENT_STYLE, name);
   else if (!strcmp (name, TtaGetMessage (AMAYA, AM_USER_PREFERENCES)))
     LCSS = SearchCSS (doc, CSS_USER_STYLE, name);
   else
     LCSS = SearchCSS (doc, CSS_EXTERNAL_STYLE, name);
   if (LCSS == NULL)
     {
       MSG ("CSS selected not found in list\n");
       return;
     }
}

/*----------------------------------------------------------------------
   RedrawLRPI                                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RedrawLRPI (char *name)
#else
void                RedrawLRPI (name)
char               *name;
#endif
{
   char                buffer[3000];
   char                nb_rpi;
   int                 index = -1;
   Document            doc = currentDocument;

   /* rebuild the list and redraw the RPI selector */
   if (LCSS)
     {
       nb_rpi = BuildRPIList (doc, LCSS, buffer, 3000);
       CleanListRPI (&LListRPI);
       LListRPI = PSchema2RPI (doc, LCSS);
     }
   else
     nb_rpi = 0;
   if (!name)
     TtaNewSelector (BaseCSSDialog + RPILList, BaseCSSDialog + FormCSS,
		     TtaGetMessage (AMAYA, AM_RULE_LIST_FILE_1), nb_rpi,
		     buffer, 6, NULL, FALSE, TRUE);

   if (name)
     {
	/*
	 * look if the name given is an existing RPI element.
	 */
	for (index = 0; index < nb_rpi; index++)
	   if (!strcmp (name, GetlistEntry (buffer, index)))
	      break;

	if (index >= nb_rpi)
	   index = -1;
	SelectRPIEntry ('L', index, name);
     }
}


/*----------------------------------------------------------------------
   RedrawRCSS                                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RedrawRCSS (char *name)
#else
void                RedrawRCSS (name)
char               *name;
#endif
{
   char                buffer[3000];
   char                nb_css;
   int                 index;
   Document            doc = currentDocument;

   if (name && currentLCSS && !strcmp (currentLCSS, name))
     {
	TtaSetSelector (BaseCSSDialog + CSSRName, CSSREntry, "");
	return;
     }
   /* rebuild the list and redraw the CSS selector */
   nb_css = BuildCSSList (doc, buffer, 3000, name);
   if (nb_css < 1)
     return;
   TtaNewSelector (BaseCSSDialog + CSSRName, BaseCSSDialog + FormCSS,
		   TtaGetMessage (AMAYA, AM_CSS_FILE_2), nb_css,
		   buffer, 3, NULL, FALSE, TRUE);

   if (!name)
     name = GetlistEntry (buffer, 1);
   /* look if the name given is an existing CSS element */
   for (index = 0; index < nb_css; index++)
     if (!strcmp (name, GetlistEntry (buffer, index)))
       break;

   if (index >= nb_css)
     {
       MSG ("non-existent CSS selected\n");
       return;
     }
   if (currentRCSS)
     TtaFreeMemory (currentRCSS);
   currentRCSS = TtaStrdup (name);
   
   TtaSetSelector (BaseCSSDialog + CSSRName, index, NULL);
   CSSREntry = index;
   if (!strcmp (name, TtaGetMessage (AMAYA, AM_DOC_STYLE)))
     RCSS = SearchCSS (doc, CSS_DOCUMENT_STYLE, name);
   else if (!strcmp (name, TtaGetMessage (AMAYA, AM_USER_PREFERENCES)))
     RCSS = SearchCSS (doc, CSS_USER_STYLE, name);
   else
     RCSS = SearchCSS (doc, CSS_EXTERNAL_STYLE, name);
   if (RCSS == NULL)
     {
       MSG ("CSS selected not found in list\n");
       return;
     }
}

/*----------------------------------------------------------------------
   RedrawRRPI                                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RedrawRRPI (char *name)
#else
void                RedrawRRPI (name)
char               *name;

#endif
{
   char                buffer[3000];
   char                nb_rpi;
   int                 index = -1;
   Document            doc = currentDocument;

   /* rebuild the list and redraw the RPI selector */
   if (RCSS)
     {
       nb_rpi = BuildRPIList (doc, RCSS, buffer, 3000);
       CleanListRPI (&RListRPI);
       RListRPI = PSchema2RPI (doc, RCSS);
     }
   else
     nb_rpi = 0;
   if (!name)
     TtaNewSelector (BaseCSSDialog + RPIRList, BaseCSSDialog + FormCSS,
		     TtaGetMessage (AMAYA, AM_RULE_LIST_FILE_2), nb_rpi,
		     buffer, 6, NULL, FALSE, TRUE);

   if (name)
     {
	/*
	 * look if the name given is an existing RPI element.
	 */
	for (index = 0; index < nb_rpi; index++)
	   if (!strcmp (name, GetlistEntry (buffer, index)))
	      break;

	if (index >= nb_rpi)
	   index = -1;
     }
   SelectRPIEntry ('R', index, name);
}

/*----------------------------------------------------------------------
   CSSHandleMerge : procedure used as a front end for merge operations
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CSSHandleMerge (char which, boolean copy)
#else
void                CSSHandleMerge (which, copy)
char                which;
boolean                copy;

#endif
{
   CSSInfoPtr          css = NULL;
   PSchema             pschema;
   PSchema             cour, prev = NULL;
   Document            doc = currentDocument;
   char                from;
   char                value[2000];
   int                 index;

   /*
    * store the current state of the style menu.
    * and do a few sanity checkings.
    */
   if (currentBRPI[0] == EOS)
      return;
   if (LListRPIIndex != -1)
     {
	from = 'L';
	index = LListRPIIndex;
	strcpy (value, currentLRPI);
     }
   else if (RListRPIIndex != -1)
     {
	from = 'R';
	index = RListRPIIndex;
	strcpy (value, currentRRPI);
     }
   else
     {
	from = 'B';
	index = -1;
	strcpy (value, currentBRPI);
     }
   if (from == which)
      return;

   /*
    * Then find the associated pschema.
    */
   switch (which)
	 {
	    case 'L':
	       css = LCSS;
	       break;
	    case 'R':
	       css = RCSS;
	       break;
	 }
   if (css == NULL)
      return;
   pschema = css->pschema;
   if (pschema == NULL)
     {
	/*
	 * allocate a new PSchema and insert it in the list depending
	 * on the type of style.
	 */
	pschema = TtaNewPSchema ();
	if (pschema == NULL)
	   return;
	switch (css->category)
	      {
		 case CSS_DOCUMENT_STYLE:
		    cour = TtaGetFirstPSchema (doc, NULL);
		    while (cour != NULL)
		      {
			 prev = cour;
			 TtaNextPSchema (&cour, doc, NULL);
		      }
		    TtaAddPSchema (pschema, prev, FALSE, doc, NULL);
		    break;
		 case CSS_Unknown:
		 case CSS_EXTERNAL_STYLE:
		 case CSS_BROWSED_STYLE:
		    cour = TtaGetFirstPSchema (doc, NULL);
		    TtaAddPSchema (pschema, cour, FALSE, doc, NULL);
		    break;
		 case CSS_USER_STYLE:
		    cour = TtaGetFirstPSchema (doc, NULL);
		    TtaAddPSchema (pschema, cour, TRUE, doc, NULL);
		    break;
	      }
	css->pschema = pschema;
     }
   /*
    * If this is a copy operation, first destroy the existing RPI's.
    */
   if (copy)
     {
	SetHTMLStyleParserDestructiveMode (TRUE);
	MergeNewCSS (currentBRPI, doc, css);
	SetHTMLStyleParserDestructiveMode (FALSE);
     }
   /*
    * build the internal structures corresponding to the
    * new rules.
    */
   MergeNewCSS (currentBRPI, doc, css);
   css->state = CSS_STATE_Modified;

   /*
    * change the presentation of the CSS dialogs to reflect the
    * new state of the internal structures.
    */
   switch (which)
	 {
	    case 'L':
	       LListRPIModified = TRUE;
	       RedrawLRPI (NULL);
	       switch (from)
		     {
			case 'R':
			   /* keep the previously selected element */
			   SelectRPIEntry ('R', index, value);
			   break;
			case 'B':
			   /* maintain the rule as inserted */
			   SelectRPIEntry ('B', -1, value);
			   break;
		     }
	       break;
	    case 'R':
	       RListRPIModified = TRUE;
	       RedrawRRPI (NULL);
	       switch (from)
		     {
			case 'L':
			   /* keep the previously selected element */
			   SelectRPIEntry ('L', index, value);
			   break;
			case 'B':
			   /* maintain the rule as inserted */
			   SelectRPIEntry ('B', -1, value);
			   break;
		     }
	       break;
	 }

}

/*----------------------------------------------------------------------
   RebuildAllCSS : rebuild the whole internal structures with the  
   all the original CSS rules.                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RebuildAllCSS (void)
#else
void                RebuildAllCSS ()
#endif
{
   CSSInfoPtr          css = ListCSS;

   while (css != NULL)
     {
	if (css->documents[currentDocument])
	   RebuildCSS (css);
	css = css->NextCSS;
     }
   RedrawLCSS (TtaGetMessage (AMAYA, AM_DOC_STYLE));
   RedrawRCSS (NULL);
   RedrawLRPI (NULL);
   RedrawRRPI (NULL);
}

/*----------------------------------------------------------------------
   InitCSS                                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitCSS (void)
#else
void                InitCSS ()
#endif
{
   /* Read the AMAYA_SAVE_DIR environment variable for publishing */
   amaya_save_dir = TtaGetEnvString (AMAYA_SAVE_DIR);
#ifdef DEBUG_CSS
   if (amaya_save_dir == NULL)
     {
	fprintf (stderr, "Please set up the environment variable AMAYA_SAVE_DIR :\n\
It is used for publishing when saving through the network is unavailable\n\
 e.g.: if AMAYA_SAVE_DIR=/users/joe/html, Amaya will try to save an URL\n\
       http://my_server/pub/file.html to :\n\
       /users/joe/html/pub/file.html or\n\
       /users/joe/html/file.html     in that order.\n\n\
       if AMAYA_SAVE_DIR=/users/joe/html/%%s, Amaya will try to save an URL\n\
       http://my_server/pub/file.html to :\n\
       /users/joe/html/my_server/pub/file.html or\n\
       /users/joe/html/my_server/file.html or\n\
       /users/joe/html/pub/file.html or\n\
       /users/joe/html/file.html int that order.\n");
     }
#endif /* DEBUG_CSS */

   /* initialize the dialogs */
   BaseCSSDialog = TtaSetCallback (CSSCallbackDialogue, NB_CSS_DIALOGS);
   CSSDirectoryName = TtaGetMemory (MAX_LENGTH);
   CSSDocumentName = TtaGetMemory (MAX_LENGTH);
}
