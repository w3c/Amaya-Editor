/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Author: D. Veillard
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


extern CSSInfoPtr   ListCSS;
extern Document     currentDocument;
extern CSSInfoPtr   User_CSS;
extern char        *CSSDocumentName;
extern char        *CSSDirectoryName;
extern char        *amaya_save_dir;
extern boolean         NonPPresentChanged;
extern int          BaseCSSDialog;
extern CSSInfoPtr   LCSS;
extern CSSInfoPtr   RCSS;
extern char         currentLRPI[500];
extern char         currentRRPI[500];
extern int          LListRPIIndex;
extern int          RListRPIIndex;
extern PRuleInfoPtr LListRPI;
extern PRuleInfoPtr RListRPI;
extern boolean         LListRPIModified;
static boolean         RListRPIModified;

#include "css_f.h"
#include "p2css_f.h"
#include "HTMLhistory_f.h"
#include "AHTURLTools_f.h"
#include "UIcss_f.h"


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
   nb_css = BuildCSSHistoryList (doc, buffer, 3000, NULL);
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
   RedisplayDocument : force the Thot kernel to rebuild the internal 
   image representation to reflect changes in    
   the generic presentation for example.         
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                RedisplayDocument (Document doc)
#else
void                RedisplayDocument (doc)
Document            doc;

#endif
{
/***********************
  int mode;

  mode = TtaGetDisplayMode(doc);
  if (mode == DisplayImmediately) {
      TtaSetDisplayMode (doc, NoComputedDisplay);
      TtaSetDisplayMode (doc, DisplayImmediately);
  }
 ***********************/
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

  if (color != -1)
    TtaSetViewBackgroundColor (doc, 1, color);
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
   char               *proto, *host, *dir, *file;
   boolean                dir_ok = FALSE;
   static char         URL[500];

   strcpy (URL, url);
   ExplodeURL (URL, &proto, &host, &dir, &file);

   if (file == NULL)
      file = "noname.css";

   if ((host != NULL) && (amaya_save_dir != NULL))
     {
	if (dir != NULL)
	  {
	     sprintf (CSSDirectoryName, amaya_save_dir, host);
	     strcat (CSSDirectoryName, DIR_STR);
	     strcat (CSSDirectoryName, dir);
	     dir_ok = TtaCheckDirectory (CSSDirectoryName);
	  }
	if (!dir_ok)
	  {
	     sprintf (CSSDirectoryName, amaya_save_dir, host);
	     dir_ok = TtaCheckDirectory (CSSDirectoryName);
	  }
	if ((dir != NULL) && (!dir_ok))
	  {
	     sprintf (CSSDirectoryName, amaya_save_dir, "");
	     strcat (CSSDirectoryName, DIR_STR);
	     strcat (CSSDirectoryName, dir);
	     dir_ok = TtaCheckDirectory (CSSDirectoryName);
	  }
	if (!dir_ok)
	  {
	     sprintf (CSSDirectoryName, amaya_save_dir, "");
	     dir_ok = TtaCheckDirectory (CSSDirectoryName);
	  }
     }
   if ((host == NULL) && (dir != NULL))
     {
	/* local file : users preferences for example */
	strcpy (CSSDirectoryName, dir);
	dir_ok = TtaCheckDirectory (CSSDirectoryName);
     }
   if (!dir_ok)
     {
	getcwd (CSSDirectoryName, MAX_LENGTH);
     }
   strcpy (CSSDocumentName, file);
   /* don't try to use file or dir from here, they reference URL ! */
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
   PRuleInfoPtr        rpi;
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
			   TtaDestroyDialogue (BaseCSSDialog + FormCSS);
			   break;
			case 1:
			   if (CSSBrowseState != CSS_BROWSE_None)
			      break;
			   if (sauve_css != NULL)
			      break;
			   sauve_css = ListCSS;
			   while (sauve_css != NULL)
			     {
				while ((sauve_css != NULL) &&
				((sauve_css->state != CSS_STATE_Modified) ||
				(sauve_css->category == CSS_DOCUMENT_STYLE)))
				   sauve_css = sauve_css->NextCSS;
				if (sauve_css != NULL)
				  {
				     if (SaveCSSThroughNet (currentDocument,
							 1, sauve_css) == 0)
					continue;
				     CSSBrowseState = CSS_BROWSE_SaveAll;
				     InitBrowse (currentDocument, 1, sauve_css->url);
				  }
			     }
			   break;
			case 2:
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
				if (rpi == NULL)
				   break;
				RemoveRPI (currentDocument, rpi);
				LListRPIModified = TRUE;
				LCSS->state = CSS_STATE_Modified;
				RedrawLRPI (NULL);
				RebuildHTMLStyleHeader (currentDocument);
				RedisplayDocument (currentDocument);
			     }
			   else if (currentRRPI[0] != EOS)
			     {
				rpi = SearchRPISel (currentRRPI, RListRPI);
				if (rpi == NULL)
				   break;
				RemoveRPI (currentDocument, rpi);
				RCSS->state = CSS_STATE_Modified;
				RListRPIModified = TRUE;
				RedrawRRPI (NULL);
				RebuildHTMLStyleHeader (currentDocument);
				RedisplayDocument (currentDocument);
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
			   LoadHTMLExternalStyleSheet (currentExternalCSS,
						     currentDocument, TRUE);
			   RedisplayDocument (currentDocument);
			   if ((RListRPIIndex == -1) || (LListRPIIndex != -1))
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
			   if ((RListRPIIndex == -1) || (LListRPIIndex != -1))
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

/*----------------------------------------------------------------------
 *									*
 *		Handling of the Save CSS Form				*
 *									*
  ----------------------------------------------------------------------*/
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
				{
				   /* save the local document */
				   DumpCSSToFile (currentDocument, sauve_css, tempfile);
				}
			      sauve_css = sauve_css->NextCSS;
			      while ((sauve_css != NULL) &&
				((sauve_css->state != CSS_STATE_Modified) ||
			       (sauve_css->category == CSS_DOCUMENT_STYLE)))
				 sauve_css = sauve_css->NextCSS;
			      if (sauve_css != NULL)
				{
				   InitBrowse (currentDocument, 1, sauve_css->url);
				}
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
				{
				   InitBrowse (currentDocument, 1, sauve_css->url);
				}
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
				      ".*htm*", TtaGetMessage (AMAYA, AM_FILES), BaseCSSDialog + CSSSauvDoc);
		 }
	       else
		 {
		    if (IsW3Path (data))
		      {
			 /* reset the CSSDirectoryName */
			 strcpy (tempfile, CSSDirectoryName);
			 strcat (tempfile, DIR_STR);
			 TtaExtractName (data, CSSDirectoryName, tempname);
			 strcat (tempfile, tempname);
			 /* reinitialize directories and document lists */
			 TtaListDirectory (CSSDirectoryName, BaseCSSDialog + CSSFormSauver,
					   TtaGetMessage (LIB, TMSG_DOC_DIR), BaseCSSDialog + CSSSauvDir,
					   ".*htm*", TtaGetMessage (AMAYA, AM_FILES), BaseCSSDialog + CSSSauvDoc);
			 TtaSetTextForm (BaseCSSDialog + CSSNomURL, tempfile);
		      }
		    else
		       TtaExtractName (data, CSSDirectoryName, CSSDocumentName);
		 }


	       break;
	    default:
	       break;
	 }
}
