/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Authors: D. Veillard
 *          I. Vatton
 *
 */

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"

#include "rcopy.xpm"
#include "lcopy.xpm"
#include "rmerge.xpm"
#include "lmerge.xpm"
#include "destroy.xpm"

static CSSBrowseStatus CSSBrowseState = CSS_BROWSE_None;
static char         currentExternalCSS[500] = "";
static char         currentDeleteCSS[500] = "";
static boolean      CSSUserAnswer = FALSE;
static CSSInfoPtr   sauve_css = NULL;
static boolean      RListRPIModified;
static char         URL[500];

extern CSSInfoPtr   ListCSS;
extern Document     currentDocument;
extern CSSInfoPtr   User_CSS;
extern CSSInfoPtr   LCSS;
extern CSSInfoPtr   RCSS;
extern PRuleInfoPtr LListRPI;
extern PRuleInfoPtr RListRPI;
extern char        *CSSDocumentName;
extern char        *CSSDirectoryName;
extern char        *amaya_save_dir;
extern char         currentLRPI[500];
extern char         currentRRPI[500];
extern char        *currentLCSS;
extern char        *currentRCSS;
extern int          LListRPIIndex;
extern int          RListRPIIndex;
extern int          BaseCSSDialog;
extern boolean      LListRPIModified;
extern boolean      NonPPresentChanged;


#include "css_f.h"
#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
#include "query_f.h"
#endif /* !AMAYA_JAVA  && !AMAYA_ILU */
#include "p2css_f.h"
#include "AHTURLTools_f.h"
#include "UIcss_f.h"

#ifdef AMAYA_DEBUG
#define MSG(msg) fprintf(stderr,msg)
#else
static char        *last_message = NULL;
 
#define MSG(msg) last_message = msg
#endif

/*----------------------------------------------------------------------
   InitCSSDialog                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitCSSDialog (Document doc, View view)
#else
void                InitCSSDialog (doc, view)
Document            doc;
View                view;
#endif
{
#  ifndef _WINDOWS
   static Pixmap       opIcons[5] = {0, 0, 0, 0, 0};
   char                menu[3000];

   if (currentDocument != -1)
      return;
   currentDocument = doc;
   if (opIcons[0] == 0)
      opIcons[0] = TtaCreatePixmapLogo (lmerge_xpm);
   if (opIcons[1] == 0)
      opIcons[1] = TtaCreatePixmapLogo (lcopy_xpm);
   if (opIcons[2] == 0)
      opIcons[2] = TtaCreatePixmapLogo (destroy_xpm);
   if (opIcons[3] == 0)
      opIcons[3] = TtaCreatePixmapLogo (rcopy_xpm);
   if (opIcons[4] == 0)
      opIcons[4] = TtaCreatePixmapLogo (rmerge_xpm);

   sprintf (menu, "%s%c%s%c%s%c%s%c%s", TtaGetMessage (AMAYA, AM_SAVE), EOS,
	    TtaGetMessage (AMAYA, AM_SAVE_AS), EOS,
	    TtaGetMessage (AMAYA, AM_RELOAD), EOS,
	    TtaGetMessage (AMAYA, AM_NEW), EOS,
	    TtaGetMessage (AMAYA, AM_DELETE));
   TtaNewSheet (BaseCSSDialog + FormCSS, TtaGetViewFrame (doc, 1), 
	      TtaGetMessage (AMAYA, AM_CSS), 5, menu, TRUE, 3, 'L', D_DONE);
   TtaNewSelector (BaseCSSDialog + CSSLName, BaseCSSDialog + FormCSS,
		   TtaGetMessage (AMAYA, AM_CSS_FILE_1), 1, " ", 3, NULL, FALSE, TRUE);
   TtaNewLabel (BaseCSSDialog + CSSLabel, BaseCSSDialog + FormCSS, " CSS ");
   TtaNewSelector (BaseCSSDialog + CSSRName, BaseCSSDialog + FormCSS,
		   TtaGetMessage (AMAYA, AM_CSS_FILE_2), 1, " ", 3, NULL, FALSE, TRUE);
   TtaNewSelector (BaseCSSDialog + RPILList, BaseCSSDialog + FormCSS,
		   TtaGetMessage (AMAYA, AM_RULE_LIST_FILE_1), 1,
		   " ", 5, NULL, FALSE, TRUE);
   TtaNewIconMenu (BaseCSSDialog + RPIActions, BaseCSSDialog + FormCSS, 0,
		   NULL, 5, opIcons, FALSE);
   TtaNewSelector (BaseCSSDialog + RPIRList, BaseCSSDialog + FormCSS,
		   TtaGetMessage (AMAYA, AM_RULE_LIST_FILE_2), 1,
		   " ", 5, NULL, FALSE, TRUE);
   TtaNewTextForm (BaseCSSDialog + RPIText, BaseCSSDialog + FormCSS,
		   "CSS Rule", 70, 3, TRUE);

   RedrawLCSS (TtaGetMessage (AMAYA, AM_DOC_STYLE));
   RedrawRCSS (NULL);
   RedrawLRPI (NULL);
   RedrawRRPI (NULL);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseCSSDialog + FormCSS, TRUE);
#  else  /* _WINDOWS */
   MessageBox (TtaGetViewFrame (doc, 1), "Not implemented yet", "CSS", MB_OK);
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   DeleteExternalCSS : dialog used to delete an external CSS file.   
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                DeleteExternalCSS (Document doc, View view)
#else
void                DeleteExternalCSS (doc, view)
Document            doc;
View                view;

#endif
{
   /* char *format = "Delete a Style Sheet"; */
   char                buffer[3000];
   char                nb_css;

   if (currentDocument == -1)
      return;

   TtaNewSheet (BaseCSSDialog + FormDeleteCSS, TtaGetViewFrame (doc, 1), 
		TtaGetMessage (AMAYA, AM_DELETE_CSS), 1, TtaGetMessage (AMAYA, AM_DELETE), TRUE, 3, 'L', D_DONE);

   /* rebuild the list and redraw the CSS selector */
   nb_css = BuildCSSList (doc, buffer, 3000, NULL);
   TtaNewSelector (BaseCSSDialog + ListDeleteCSS, BaseCSSDialog + FormDeleteCSS,
		   TtaGetMessage (AMAYA, AM_DELETE_STYLE_SHEET), nb_css, buffer, 6, NULL, FALSE, TRUE);

   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseCSSDialog + FormDeleteCSS, TRUE);
}

/*----------------------------------------------------------------------
   SelectExternalCSS : dialog used to select and load an external   
   CSS file.                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SelectExternalCSS (Document doc, View view)
#else
void                SelectExternalCSS (doc, view)
Document            doc;
View                view;
#endif
{
   char                buffer[3000];
   char                menu[3000];
   char                nb_css;

   if (currentDocument == -1)
      return;

   sprintf (menu, "%s%c%s%c%s", TtaGetMessage (AMAYA, AM_ADD), EOS,
	    TtaGetMessage (AMAYA, AM_SHOW), EOS,
	    TtaGetMessage (AMAYA, AM_BROWSE));
   TtaNewSheet (BaseCSSDialog + FormExternalCSS, TtaGetViewFrame (doc, 1), 
     TtaGetMessage (AMAYA, AM_EXTERNAL_CSS), 3, menu, TRUE, 3, 'L', D_DONE);

   /* rebuild the list and redraw the CSS selector */
   nb_css = 0;
   buffer[0] = '\0';
   TtaNewSelector (BaseCSSDialog + ListExternalCSS, BaseCSSDialog + FormExternalCSS,
		   TtaGetMessage (AMAYA, AM_SELECT_EXTERNAL_STYLE_SHEET), nb_css, buffer, 6, NULL, TRUE, TRUE);

   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseCSSDialog + FormExternalCSS, TRUE);
}

/*----------------------------------------------------------------------
   CSSConfirm                                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CSSConfirm (Document document, View view, char *label)
#else
void                CSSConfirm (document, view, label)
Document            document;
View                view;
char               *label;
#endif
{
   /* Confirm form */
   TtaNewForm (BaseCSSDialog + CSSFormConfirm, TtaGetViewFrame (document, view),
	       TtaGetMessage (LIB, TMSG_LIB_CONFIRM), TRUE, 2, 'L', D_DONE);
   TtaNewLabel (BaseCSSDialog + CSSTextConfirm, BaseCSSDialog + CSSFormConfirm, label);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseCSSDialog + CSSFormConfirm, FALSE);
   /* wait for an answer */
   TtaWaitShowDialogue ();
}


/*----------------------------------------------------------------------
   ApplyExtraPresentation : Change the presentation attributes     
   of the document which are not handled by P structures,  
   like the background color and the magnification.        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ApplyExtraPresentation (Document doc)
#else
void                ApplyExtraPresentation (doc)
Document            doc;
#endif
{
  CSSInfoPtr          css;
  int                 color = -1;
  int                 zoom = 0, old_zoom;

  css = GetDocumentStyle (doc);
  if (css->pschema != NULL)
    {
      if (css->view_background_color != -1)
	color = css->view_background_color;
      if (css->magnification != -1000)
	zoom = css->magnification;
    }
  
  while (css != NULL)
    {
      if (css->documents[doc])
	{
	  switch (css->category)
	    {
	    case CSS_EXTERNAL_STYLE:
	      if (css->view_background_color != -1)
		color = css->view_background_color;
	      if (css->magnification != -1000)
		zoom = css->magnification;
	      break;
	    default:
	      break;
	    }
	}
      css = css->NextCSS;
    }
  
  css = User_CSS;
  if (css != NULL)
    {
      if (css->view_background_color != -1)
	color = css->view_background_color;
      if (css->magnification != -1000)
	zoom = css->magnification;
    }
#ifdef DEBUG_CSS
  fprintf (stderr, "ApplyExtraPresentation(color = %d)\n", color);
#endif

  if (zoom != -1000)
    {
      old_zoom = TtaGetZoom (doc, 1);
      if (zoom != old_zoom)
	TtaSetZoom (doc, 1, zoom - old_zoom);
    }
  NonPPresentChanged = FALSE;
}

/*----------------------------------------------------------------------
   InitBrowse : dialog used to save an external CSS file, the user's 
   preferences or the HTML File itself.                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitBrowse (Document doc, View view, char *url)
#else
void                InitBrowse (doc, view, url)
Document            doc;
View                view;
char               *url;

#endif
{
  TtaExtractName (url, CSSDirectoryName, CSSDocumentName);
  if (CSSDocumentName[0] == EOS)
    strcpy (CSSDocumentName, "noname.css");

   strcpy (URL, CSSDirectoryName);
   strcat (URL, DIR_STR);
   strcat (URL, CSSDocumentName);

   /* Dialogue form for saving as */
   TtaNewForm (BaseCSSDialog + CSSFormSauver, TtaGetViewFrame (doc, view), 
	       TtaGetMessage (AMAYA, AM_SAVE_AS), TRUE, 2, 'L', D_DONE);
   TtaListDirectory (CSSDirectoryName, BaseCSSDialog + CSSFormSauver,
		     TtaGetMessage (LIB, TMSG_DOC_DIR),
		     BaseCSSDialog + CSSSauvDir, "css",
	       TtaGetMessage (AMAYA, AM_FILES), BaseCSSDialog + CSSSauvDoc);
   TtaNewTextForm (BaseCSSDialog + CSSNomURL, BaseCSSDialog + CSSFormSauver,
		   TtaGetMessage (LIB, TMSG_DOCUMENT_NAME), 50, 1, TRUE);
   TtaSetTextForm (BaseCSSDialog + CSSNomURL, URL);
   TtaShowDialogue (BaseCSSDialog + CSSFormSauver, FALSE);
}

/*----------------------------------------------------------------------
   DumpCSSToFile                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      DumpCSSToFile (Document doc, CSSInfoPtr css, char *filename)
#else
static boolean      DumpCSSToFile (doc, css, output)
Document            doc;
CSSInfoPtr          css;
char               *filename;
#endif
{
   struct tm          *tm;
   time_t              current_date;
   PRuleInfoPtr        rpi, list;
   FILE               *output;
   char               *buffer, *cour, *user;
   int                 size;
   boolean             ok;

   ok = FALSE;
   if (css != NULL && css->pschema != NULL && filename != NULL)
     {
       list = PSchema2RPI (doc, css);
       /* calculate the output file size */
       size = 1000;    	/* overestimated header size ... */
       for (rpi = list; rpi != NULL; rpi = rpi->NextRPI)
	 /*     selector " { "  css_rule   " }\n\r" */
	 size += strlen (rpi->selector) + 3 + strlen (rpi->css_rule) + 4;

       /* allocate it */
       buffer = TtaGetMemory (size);
       if (buffer != NULL)
	 {
	   output = fopen (filename, "w");
	   if (output != NULL)
	     {
	       ok = TRUE;
	       /* fill in the header with pertinent informations */
	       cour = buffer;
	       sprintf (cour, "/*\n * CSS 1.0 Style Sheet produced by Amaya\n * \n");

	       while (*cour != 0)
		 cour++;
	       if (css->name)
		 {
		   sprintf (cour, " * %s\n *\n", css->name);
		   while (*cour != 0)
		     cour++;
		 }

	       if (css->url)
		 {
		   sprintf (cour, " * URL : %s\n", css->url);
		   while (*cour != 0)
		     cour++;
		 }
	       sprintf (cour, " * Last updated ");

	       while (*cour != 0)
		 cour++;
	       (void) time (&current_date);
	       tm = localtime (&current_date);
	       (void) strftime (cour, 100, "%x %X", tm);

	       while (*cour != 0)
		 cour++;
	       user = TtaGetEnvString ("USER");
	       if (user == NULL)
		 user = "unknown user";
	       sprintf (cour, " by %s on ", user);
	   
	       while (*cour != 0)
		 cour++;
	       (void) gethostname (cour, 100);
	       
	       while (*cour != 0)
		 cour++;
	       sprintf (cour, "\n */\n\n");
	       
	       while (*cour != 0)
		 cour++;
	       /* dump the rules to the buffer, and free them */
	       for (rpi = list; rpi != NULL; rpi = rpi->NextRPI)
		 {
		   strcpy (cour, rpi->selector);
		   while (*cour != 0)
		     cour++;
		   strcpy (cour, " { ");
		   while (*cour != 0)
		     cour++;
		   strcpy (cour, rpi->css_rule);
		   while (*cour != 0)
		     cour++;
		   strcpy (cour, " }\n");
		   while (*cour != 0)
		     cour++;
		 }

	       /* mark the end */
	       strcpy (cour, "\n/* CSS end */\n");

	       /* save it to the file */
	       fwrite (buffer, strlen (buffer), 1, output);
	       fclose (output);

	       /* update the css_rule field in the css_structure */
	       if (css->css_rule != NULL)
		 TtaFreeMemory (css->css_rule);
	       css->css_rule = buffer;

	       /* mark the rule as unchanged */
	       css->state = CSS_STATE_Unmodified;
	     }
	 }
       CleanListRPI (&list);
     }
   return (ok);
}

/*----------------------------------------------------------------------
   SaveCSS : Save locally or use the PUT method to save a CSS file.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean   SaveCSS (Document doc, View view, CSSInfoPtr css)
#else
static boolean   SaveCSS (doc, view, css)
Document         doc;
View             view;
CSSInfoPtr       css;
#endif
{
  char               *filename;
  boolean             ok = TRUE;

  if (css == NULL || css->url == NULL)
    return (FALSE);
  else if (IsW3Path (css->url))
    {
      filename = GetLocalPath (doc, css->url);
      ok = DumpCSSToFile (doc, css, filename);
      if (ok)
	ok = (boolean) PutObjectWWW (doc, filename, css->url, AMAYA_SYNC,
				     unknown_type, NULL, NULL);
      TtaFreeMemory (filename);
    }
  else
    ok = DumpCSSToFile (doc, css, css->url);
  
  if (ok)
    {
      TtaDisplayMessage (CONFIRM, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), css->url);
      return (FALSE);
    }
  else
    {
      TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_SAVED), css->url);
      return (TRUE);
     }
}

/*----------------------------------------------------------------------
   CSSCallbackDialogue : procedure for style dialogue events          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CSSCallbackDialogue (int ref, int typedata, char *data)
#else
void                CSSCallbackDialogue (ref, typedata, data)
int                 ref;
int                 typedata;
char               *data;
#endif
{
  int                 val;
  PRuleInfoPtr        rpi = NULL;
  char                tempfile[MAX_LENGTH];
  char                tempname[MAX_LENGTH];
  char               *s;

  val = (int) data;
#ifdef DEBUG_CSS
  if (typedata == INTEGER_DATA)
    fprintf (stderr, "CSSCallbackDialogue(%d,%d) \n",
	     ref, (int) data);
  else
    fprintf (stderr, "CSSCallbackDialogue(%d,\"%s\") \n",
	     ref, (char *) data);
#endif

  if (currentDocument == -1)
    return;

  switch (ref - BaseCSSDialog)
    {
    case FormCSS:
      switch (val)
	{
	case 0:
	  currentDocument = -1;
	  if (currentLCSS)
	    TtaFreeMemory (currentLCSS);
	  currentLCSS = NULL;
	  if (currentRCSS)
	    TtaFreeMemory (currentRCSS);
	  currentRCSS = NULL;
	  TtaDestroyDialogue (BaseCSSDialog + FormCSS);
	  break;
	case 1:
	case 2:
	  if (CSSBrowseState == CSS_BROWSE_None && sauve_css == NULL)
	    {
	      sauve_css = ListCSS;
	      while (sauve_css != NULL)
		{
		  while (sauve_css != NULL &&
			 (sauve_css->state != CSS_STATE_Modified ||
			  sauve_css->category == CSS_DOCUMENT_STYLE))
		    sauve_css = sauve_css->NextCSS;
		  if (sauve_css != NULL)
		    {
		      if (val == 1)
			SaveCSS (currentDocument, 1, sauve_css);
		      else
			{
			  CSSBrowseState = CSS_BROWSE_SaveAll;
			  InitBrowse (currentDocument, 1, sauve_css->url);
			}
		    }
		  else
		    TtaSetStatus (currentDocument, 1, TtaGetMessage (AMAYA, AM_NOTHING_TO_SAVE), "");
		}
	    }
	  break;
	case 3:
	  RebuildAllCSS ();
	  break;
	case 4:
	  SelectExternalCSS (currentDocument, 1);
	  break;
	case 5:
	  DeleteExternalCSS (currentDocument, 1);
	  break;
	default:
	  TtaDestroyDialogue (BaseCSSDialog + FormCSS);
	  currentDocument = -1;
	  break;
	}
      break;
    case CSSLName:
      RedrawLCSS (data);
      RedrawLRPI (NULL);
      break;
    case RPILList:
      RedrawLRPI (data);
      break;
    case CSSRName:
      RedrawRCSS (data);
      RedrawRRPI (NULL);
      break;
    case RPIRList:
      RedrawRRPI (data);
      break;
    case RPIActions:
      switch (val)
	{
	case 0:
	  CSSHandleMerge ('R', FALSE);
	  break;
	case 1:
	  CSSHandleMerge ('R', TRUE);
	  break;
	case 2:
	  /* Delete the RPI currently selected */
	  if (currentLRPI[0] != EOS)
	    {
	      rpi = SearchRPISel (currentLRPI, LListRPI);
	      LListRPIModified = TRUE;
	      LCSS->state = CSS_STATE_Modified;
	    }
	  else if (currentRRPI[0] != EOS)
	    {
	      rpi = SearchRPISel (currentRRPI, RListRPI);
	      RCSS->state = CSS_STATE_Modified;
	      RListRPIModified = TRUE;
	    }

	  if (rpi != NULL)
	    {
	      RemoveRPI (currentDocument, rpi);
	      RedrawRRPI (NULL);
	      RebuildHTMLStyleHeader (currentDocument);
	    }
	  break;
	case 3:
	  CSSHandleMerge ('L', TRUE);
	  break;
	case 4:
	  CSSHandleMerge ('L', FALSE);
	  break;
	}
      break;
    case RPIText:
      SelectRPIEntry ('B', -1, data);
      break;
    case ListExternalCSS:
      strcpy (currentExternalCSS, data);
      break;
    case FormExternalCSS:
      switch (val)
	{
	case 0:
	  currentExternalCSS[0] = EOS;
	  break;
	case 1:
	  /*
	   * add currentExternalCSS to the list of external
	   * style sheets of the document.
	   */
	  LoadHTMLExternalStyleSheet (currentExternalCSS, currentDocument, TRUE);
	  if (RListRPIIndex == -1 || LListRPIIndex != -1)
	    {
	      RCSS = SearchCSS (currentDocument, CSS_EXTERNAL_STYLE,
				currentExternalCSS);
	      RedrawRCSS (currentExternalCSS);
	      RedrawRRPI (NULL);
	    }
	  else
	    {
	      LCSS = SearchCSS (currentDocument, CSS_EXTERNAL_STYLE,
				currentExternalCSS);
	      RedrawLCSS (currentExternalCSS);
	      RedrawLRPI (NULL);
	    }
	  break;
	case 2:
	  /*
	   * show the list of CSS rules found in the external
	   * style sheets pointed by currentExternalCSS.
	   */
	  LoadHTMLExternalStyleSheet (currentExternalCSS,
				      currentDocument, FALSE);
	  if (RListRPIIndex == -1 || LListRPIIndex != -1)
	    {
	      RCSS = SearchCSS (currentDocument, CSS_BROWSED_STYLE,
				currentExternalCSS);
	      RedrawRCSS (currentExternalCSS);
	      RedrawRRPI (NULL);
	    }
	  else
	    {
	      LCSS = SearchCSS (currentDocument, CSS_BROWSED_STYLE,
				currentExternalCSS);
	      RedrawLCSS (currentExternalCSS);
	      RedrawLRPI (NULL);
	    }
	  break;
	case 3:
	  /*
	   * start a browsing window,
	   * looking for CSS files on the filesystem.
	   */
	  break;
	}
      break;
    case ListDeleteCSS:
      strcpy (currentDeleteCSS, data);
      break;
    case FormDeleteCSS:
      switch (val)
	{
	case 0:
	  currentDeleteCSS[0] = EOS;
	  break;
	case 1:
	  /*
	   * remove currentDeleteCSS from the list of external
	   * style sheets of the document.
	   */
	  RemoveCSS (currentDeleteCSS, currentDocument);
	  currentDeleteCSS[0] = EOS;
	  TtaDestroyDialogue (BaseCSSDialog + FormDeleteCSS);
	  break;
	}
      break;
      
    case CSSFormConfirm:
      CSSUserAnswer = (val == 1);
      TtaDestroyDialogue (BaseCSSDialog + CSSFormConfirm);
      break;

      /* Handling of the Save CSS Form */
    case CSSSauvDir:
      if (!strcmp (data, ".."))
	{
	  /* suppress last directory */
	  strcpy (tempname, CSSDirectoryName);
	  TtaExtractName (tempname, CSSDirectoryName, tempfile);
	}
      else
	{
	  strcat (CSSDirectoryName, DIR_STR);
	  strcat (CSSDirectoryName, data);
	}
      TtaSetTextForm (BaseCSSDialog + CSSNomURL, CSSDirectoryName);
      TtaListDirectory (CSSDirectoryName, BaseCSSDialog + CSSFormSauver,
			TtaGetMessage (LIB, TMSG_DOC_DIR),
			BaseCSSDialog + CSSSauvDir, "css",
			TtaGetMessage (AMAYA, AM_FILES), BaseCSSDialog + CSSSauvDoc);
      CSSDocumentName[0] = EOS;
      break;
    case CSSSauvDoc:
      if (CSSDirectoryName[0] == EOS)
	{
	  /* set path on current directory */
	  s = (char *) TtaGetEnvString ("PWD");
	  if (s != NULL)
	    strcpy (CSSDirectoryName, s);
	}
      /* Extract suffix from document name */
      strcpy (CSSDocumentName, data);
      /* construct the document full name */
      strcpy (tempfile, CSSDirectoryName);
      strcat (tempfile, DIR_STR);
      strcat (tempfile, CSSDocumentName);
      TtaSetTextForm (BaseCSSDialog + CSSNomURL, tempfile);
      break;
      
    case CSSFormSauver:
      if (val == 1)
	{
	  /* Ok for saving document */
	  strcpy (tempfile, CSSDirectoryName);
	  strcat (tempfile, DIR_STR);
	  strcat (tempfile, CSSDocumentName);
	  if (CSSBrowseState == CSS_BROWSE_SaveAll)
	    {
	      if (TtaFileExist (tempfile))
		{
		  /* ask confirmation */
		  sprintf (tempname, TtaGetMessage (LIB, TMSG_FILE_EXIST), tempfile);
		  CSSConfirm (currentDocument, 1, tempname);
		  if (CSSUserAnswer)
		    /* save the local document */
		    DumpCSSToFile (currentDocument, sauve_css, tempfile);
		  sauve_css = sauve_css->NextCSS;
		  while ((sauve_css != NULL) &&
			 ((sauve_css->state != CSS_STATE_Modified) ||
			  (sauve_css->category == CSS_DOCUMENT_STYLE)))
		    sauve_css = sauve_css->NextCSS;
		  if (sauve_css != NULL)
		    InitBrowse (currentDocument, 1, sauve_css->url);
		  else
		    {
		      TtaDestroyDialogue (BaseCSSDialog + CSSFormSauver);
		      CSSBrowseState = CSS_BROWSE_None;
		    }
		}
	      else
		{
		  /* save the local document */
		  DumpCSSToFile (currentDocument, sauve_css, tempfile);
		  
		  sauve_css = sauve_css->NextCSS;
		  while ((sauve_css != NULL) &&
			 ((sauve_css->state != CSS_STATE_Modified) ||
			  (sauve_css->category == CSS_DOCUMENT_STYLE)))
		    sauve_css = sauve_css->NextCSS;
		  if (sauve_css != NULL)
		    InitBrowse (currentDocument, 1, sauve_css->url);
		  else
		    {
		      TtaDestroyDialogue (BaseCSSDialog + CSSFormSauver);
		      CSSBrowseState = CSS_BROWSE_None;
		    }
		}
	    }
	}
      else
	{
	  if (CSSBrowseState == CSS_BROWSE_SaveAll)
	    {
	      if (sauve_css != NULL)
		sauve_css = sauve_css->NextCSS;
	      while ((sauve_css != NULL) &&
		     ((sauve_css->state != CSS_STATE_Modified) ||
		      (sauve_css->category == CSS_DOCUMENT_STYLE)))
		sauve_css = sauve_css->NextCSS;
	      if (sauve_css != NULL)
		InitBrowse (currentDocument, 1, sauve_css->url);
	      else
		{
		  TtaDestroyDialogue (BaseCSSDialog + CSSFormSauver);
		  CSSBrowseState = CSS_BROWSE_None;
		}
	    }
	}
      break;
    case CSSNomURL:
      /* Extract document name */
      if (TtaCheckDirectory (data) && data[strlen (data) - 1] != DIR_SEP)
	{
	  strcpy (CSSDirectoryName, data);
	  CSSDocumentName[0] = EOS;
	  /* reinitialize directories and document lists */
	  TtaListDirectory (CSSDirectoryName, BaseCSSDialog + CSSFormSauver,
			    TtaGetMessage (LIB, TMSG_DOC_DIR), BaseCSSDialog + CSSSauvDir,
			    ".css", TtaGetMessage (AMAYA, AM_FILES), BaseCSSDialog + CSSSauvDoc);
	}
      else if (IsW3Path (data))
	{
	  /* reset the CSSDirectoryName */
	  strcpy (tempfile, CSSDirectoryName);
	  strcat (tempfile, DIR_STR);
	  TtaExtractName (data, CSSDirectoryName, tempname);
	  strcat (tempfile, tempname);
	  /* reinitialize directories and document lists */
	  TtaListDirectory (CSSDirectoryName, BaseCSSDialog + CSSFormSauver,
			    TtaGetMessage (LIB, TMSG_DOC_DIR), BaseCSSDialog + CSSSauvDir,
			    ".css", TtaGetMessage (AMAYA, AM_FILES), BaseCSSDialog + CSSSauvDoc);
	  TtaSetTextForm (BaseCSSDialog + CSSNomURL, tempfile);
	}
      else
	TtaExtractName (data, CSSDirectoryName, CSSDocumentName);
      break;
    default:
      break;
    }
}
