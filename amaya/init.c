/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Initialization functions and button functions of Amaya application.
 *
 * Author: I. Vatton
 */

/* Included headerfiles */
#define THOT_EXPORT
#include "amaya.h"
#include "css.h"
#include "trans.h"

#include "stopN.xpm"
#include "stopR.xpm"
#include "save.xpm"
#include "find.xpm"
#include "Back.xpm"
#include "Forward.xpm"
#include "Reload.xpm"
#include "I.xpm"
#include "B.xpm"
#include "T.xpm"
#include "H1.xpm"
#include "H2.xpm"
#include "H3.xpm"
#include "Num.xpm"
#include "Bullet.xpm"
#include "Print.xpm"
#include "css.xpm"
#include "Image.xpm"
#include "DL.xpm"
#include "Link.xpm"
#include "Table.xpm"
#ifdef AMAYA_PLUGIN
#include "plugin.h"
#include "Plugin.xpm"
#endif /* AMAYA_PLUGIN */
#ifdef AMAYA_JAVA
#include "Java.xpm"
#endif /* AMAYA_JAVA */

#ifdef _WINDOWS
#ifndef __GNUC__
#include <direct.h>
#endif
#endif


#ifdef WITH_SOCKS
char                __res = 0;
#endif
#define AMAYA_PAGE "http://www.w3.org/pub/WWW/Amaya/User/Manual.html"
static int          AmayaInitialized = 0;
static Pixmap       stopR;
static Pixmap       stopN;
static Pixmap       iconSave;
static Pixmap       iconFind;
static Pixmap       iconReload;
static Pixmap       iconI;
static Pixmap       iconB;
static Pixmap       iconT;
static Pixmap       iconImage;
static Pixmap       iconBack;
static Pixmap       iconForward;
static Pixmap       iconH1;
static Pixmap       iconH2;
static Pixmap       iconH3;
static Pixmap       iconPrint;
static Pixmap       iconCSS;
static Pixmap       iconBullet;
static Pixmap       iconNum;
static Pixmap       iconDL;
static Pixmap       iconLink;
static Pixmap       iconTable;
#ifdef AMAYA_PLUGIN
static Pixmap       iconPlugin;
#endif /* AMAYA_PLUGIN */
#ifdef AMAYA_JAVA
static Pixmap       iconJava;
#endif /* AMAYA_JAVA */

#include "css_f.h"
#include "HTMLhistory_f.h"
#include "html2thot_f.h"
#include "init_f.h"
#ifndef AMAYA_JAVA
#include "query_f.h"
#endif
#include "trans_f.h"
#include "AHTURLTools_f.h"
#include "EDITORactions_f.h"
#include "EDITimage_f.h"
#include "EDITstyle_f.h"
#include "HTMLactions_f.h"
#include "HTMLbook_f.h"
#include "HTMLedit_f.h"
#include "HTMLhistory_f.h"
#include "HTMLimage_f.h"
#include "HTMLsave_f.h"
#include "HTMLstyle_f.h"
#include "UIcss_f.h"

#ifdef AMAYA_PLUGIN
extern void CreateFormPlugin (Document, View);
#endif /* AMAYA_PLUGIN */
#ifdef AMAYA_JAVA
#include "javaamaya_f.h"
extern void CreateFormJava (Document, View);
#endif

/*----------------------------------------------------------------------
   IsDocumentLoaded returns the document identification if the        
   corresponding document is already loaded or 0.          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Document            IsDocumentLoaded (char *documentURL)
#else
Document            IsDocumentLoaded (documentURL)
char               *documentURL;

#endif
{
   int                 i;
   boolean             found;
   char                otherURL[MAX_LENGTH];

   if (!documentURL)
      return ((Document) None);

   i = 1;
   found = FALSE;
   strcpy (otherURL, documentURL);
   strcat (otherURL, DIR_STR);
   while (!found && i < DocumentTableLength)
     {
	if (!DocumentURLs[i])
	   i++;
	else
	  {
	     found = (strcmp (documentURL, DocumentURLs[i]) == 0
		      || strcmp (otherURL, DocumentURLs[i]) == 0);
	     if (!found)
		i++;
	  }
     }

   if (i < DocumentTableLength)
      /* document is found */
      return ((Document) i);
   else
      /* document is not found */
      return ((Document) None);
}


/*----------------------------------------------------------------------
   ExtractParameters extract parameters from document nane.        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ExtractParameters (char *aName, char *parameters)
#else
void                ExtractParameters (aName, parameters)
char               *aName;
char               *parameters;

#endif
{
   int                 lg, i;
   char               *ptr, *oldptr;

   if (!parameters || !aName)
      return;			/* bad parameters */

   parameters[0] = EOS;
   lg = strlen (aName);
   if (lg)
     {
	/* the name is not empty */
	oldptr = ptr = &aName[0];
	do
	  {
	     ptr = strrchr (oldptr, '?');
	     if (ptr)
		oldptr = &ptr[1];
	  }
	while (ptr);

	i = (int) (oldptr) - (int) (aName);	/* name length */
	if (i > 1)
	  {
	     aName[i - 1] = EOS;
	     if (i != lg)
		strcpy (parameters, oldptr);
	  }
     }
}


/*----------------------------------------------------------------------
   ExtractTarget extract the target name from document nane.        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ExtractTarget (char *aName, char *target)
#else
static void         ExtractTarget (aName, target)
char               *aName;
char               *target;

#endif
{
   int                 lg, i;
   char               *ptr, *oldptr;

   if (!target || !aName)
      return;			/* bad target */

   target[0] = EOS;
   lg = strlen (aName);
   if (lg)
     {
	/* the name is not empty */
	oldptr = ptr = &aName[0];
	do
	  {
	     ptr = strrchr (oldptr, '#');
	     if (ptr)
		oldptr = &ptr[1];
	  }
	while (ptr);

	i = (int) (oldptr) - (int) (aName);	/* name length */
	if (i > 1)
	  {
	     aName[i - 1] = EOS;
	     if (i != lg)
		strcpy (target, oldptr);
	  }
     }
}


/*----------------------------------------------------------------------
   ExtractSuffix extract suffix from document nane.                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ExtractSuffix (char *aName, char *aSuffix)
#else
void                ExtractSuffix (aName, aSuffix)
char               *aName;
char               *aSuffix;

#endif
{
   int                 lg, i;
   char               *ptr, *oldptr;

   if (!aSuffix || !aName)
      return;			/* bad suffix */

   aSuffix[0] = EOS;
   lg = strlen (aName);
   if (lg)
     {
	/* the name is not empty */
	oldptr = ptr = &aName[0];
	do
	  {
	     ptr = strrchr (oldptr, '.');
	     if (ptr)
		oldptr = &ptr[1];
	  }
	while (ptr);

	i = (int) (oldptr) - (int) (aName);	/* name length */
	if (i > 1)
	  {
	     aName[i - 1] = EOS;
	     if (i != lg)
		strcpy (aSuffix, oldptr);
	  }
     }
}

/*----------------------------------------------------------------------
   ResetStop resets the stop button state                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ResetStop (Document document)
#else
void                ResetStop (document)
Document            document;

#endif
{
   if (FilesLoading[document] != 0)
     {
       FilesLoading[document]--;
#ifndef AMAYA_JAVA
       if (FilesLoading[document] == 0)
	 /* The last object associated to the document has been loaded */
	 {
	   if (TtaGetViewFrame (document, 1) != 0) 
	     /* this document is displayed */
	     {
	       if(!(DocNetworkStatus[document] & AMAYA_NET_ERROR) &&
		  (DocNetworkStatus[document] & AMAYA_NET_ACTIVE))
		 /* if there was no error message, display the LOADED message */
		 TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED), NULL);
	       TtaChangeButton (document, 1, 1, stopN);
	     }
	   DocNetworkStatus[document] = AMAYA_NET_INACTIVE;
	 }
#endif
     }
}

/*----------------------------------------------------------------------
   ActiveTransfer initialize the current transfer                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ActiveTransfer (Document document)
#else
void                ActiveTransfer (document)
Document            doc;

#endif
{
#ifndef AMAYA_JAVA
  DocNetworkStatus[document] = AMAYA_NET_ACTIVE;
#endif
  FilesLoading[document] = 1;
#ifndef AMAYA_JAVA
  if (TtaGetViewFrame (document, 1) != 0)
    /* this document is displayed */
    TtaChangeButton (document, 1, 1, stopR);
#endif
}

/*----------------------------------------------------------------------
   UpdateTransfer updates the status of the current transfer
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateTransfer (Document document)
#else
void                UpdateTransfer (document)
Document            doc;

#endif
{
    FilesLoading[document]++;
}

/*----------------------------------------------------------------------
   StopTransfer stops the current transfer                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                StopTransfer (Document document, View view)
#else
void                StopTransfer (document, view)
Document            doc;
View                view;

#endif
{
#ifdef AMAYA_JAVA
  if (FilesLoading[document] != 0)
    {
      StopRequest (document);
    }
#else
  if (DocNetworkStatus[document] & AMAYA_NET_ACTIVE)
    {
      TtaChangeButton (document, 1, 1, stopN);
      StopRequest (document);
      FilesLoading[document] = 0;
      DocNetworkStatus[document] = AMAYA_NET_INACTIVE;
    }
#endif /* AMAYA_JAVA */
}

/*----------------------------------------------------------------------
   SetCharEmphasis                                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetCharEmphasis (Document document, View view)
#else
void                SetCharEmphasis (document, view)
Document            doc;
View                view;

#endif
{
   SetCharFontOrPhrase (document, HTML_EL_Emphasis);
}

/*----------------------------------------------------------------------
   SetCharStrong                                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetCharStrong (Document document, View view)
#else
void                SetCharStrong (document, view)
Document            doc;
View                view;

#endif
{
   SetCharFontOrPhrase (document, HTML_EL_Strong);
}

/*----------------------------------------------------------------------
   SetCharCode                                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetCharCode (Document document, View view)
#else
void                SetCharCode (document, view)
Document            doc;
View                view;

#endif
{
   SetCharFontOrPhrase (document, HTML_EL_Code);
}

/*----------------------------------------------------------------------
   TextAction                                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         TextURL (Document document, View view, char *text)
#else
static void         TextURL (document, view, text)
Document            doc;
View                view;
char               *text;

#endif
{

   if (text)
     {
	if (IsW3Path (text) || TtaFileExist (text))
	   strcpy (LastURLName, text);
	else
	  {
	     /* It is not a valid URL */
	     TtaSetTextZone (document, view, 1, DocumentURLs[document]);
	     TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), text);
	     return;
	  }
     }
   if (TtaIsDocumentModified (document))
     {
	InitConfirm (document, view, TtaGetMessage (AMAYA, AM_DOC_MODIFIED));
	if (UserAnswer)
	   TtaSetDocumentUnmodified (document);
	else
	  {
	     /* restore the previous value */
	     strcpy (LastURLName, DocumentURLs[document]);
	     TtaSetTextZone (document, view, 1, LastURLName);
	     /* abort the command */
	     return;
	  }
     }
   document = GetHTMLDocument (LastURLName, NULL, document, document, DC_FALSE);
}

/*----------------------------------------------------------------------
   TextAction                                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         TextTitle (Document document, View view, char *text)
#else
static void         TextTitle (document, view, text)
Document            doc;
View                view;
char               *text;

#endif
{
   ElementType         elType;
   Element             el;

   /* search the Title element */
   el = TtaGetMainRoot (document);
   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_TITLE;
   el = TtaSearchTypedElement (elType, SearchForward, el);
   el = TtaGetFirstChild (el);
   TtaSetTextContent (el, text, TtaGetDefaultLanguage (), document);
   TtaSetDocumentModified (document);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitFormAnswer (Document document, View view)
#else
void                InitFormAnswer (document, view)
Document            document;
View                view;

#endif
{
   /* Dialogue form for answering text, user name and password */
   TtaNewForm (BaseDialog + FormAnswer, TtaGetViewFrame (document, view), 
      TtaGetMessage (AMAYA, AM_GET_AUTHENTICATION), TRUE, 1, 'L', D_CANCEL);
   TtaNewTextForm (BaseDialog + NameText, BaseDialog + FormAnswer,
		   TtaGetMessage (AMAYA, AM_NAME), NAME_LENGTH, 1, FALSE);
   TtaNewTextForm (BaseDialog + PasswordText, BaseDialog + FormAnswer,
		   TtaGetMessage (AMAYA, AM_PASSWORD), NAME_LENGTH, 1, TRUE);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + FormAnswer, FALSE);
   TtaWaitShowDialogue ();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitConfirm (Document document, View view, char *label)
#else
void                InitConfirm (document, view, label)
Document            document;
View                view;
char               *label;

#endif
{
   /* Confirm form */
   TtaNewForm (BaseDialog + ConfirmForm, TtaGetViewFrame (document, view),  TtaGetMessage (LIB, TMSG_LIB_CONFIRM), TRUE, 2, 'L', D_CANCEL);
   TtaNewLabel (BaseDialog + ConfirmText, BaseDialog + ConfirmForm, label);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + ConfirmForm, FALSE);
   /* wait for an answer */
   TtaWaitShowDialogue ();
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InitOpenDocForm (Document document, View view)
#else
static void         InitOpenDocForm (document, view)
Document            document;
View                view;

#endif
{
   int               i;
   char              s[MAX_LENGTH];

   CurrentDocument = document;
   /* Dialogue form for open URL or local */
   i = 0;
   strcpy (&s[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_CLEAR));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_PARSE));

   TtaNewSheet (BaseDialog + OpenForm, TtaGetViewFrame (document, view),  TtaGetMessage (AMAYA, AM_OPEN_URL),
		3, s, TRUE, 2, 'L', D_CANCEL);
   TtaNewTextForm (BaseDialog + URLName, BaseDialog + OpenForm,
		   TtaGetMessage (AMAYA, AM_OPEN_URL), 50, 1, TRUE);
   TtaNewLabel (BaseDialog + LocalName, BaseDialog + OpenForm, " ");
   TtaListDirectory (DirectoryName, BaseDialog + OpenForm,
		     TtaGetMessage (LIB, TMSG_DOC_DIR),		/* std thot msg */
		     BaseDialog + DirSelect, ScanFilter,
		     TtaGetMessage (AMAYA, AM_FILES), BaseDialog + DocSelect);
   if (LastURLName[0] != EOS)
      TtaSetTextForm (BaseDialog + URLName, LastURLName);
   else
     {
	strcpy (s, DirectoryName);
	strcat (s, DIR_STR);
	strcat (s, DocumentName);
	TtaSetTextForm (BaseDialog + URLName, s);
     }


   TtaNewTextForm (BaseDialog + FilterText, BaseDialog + OpenForm,
		   TtaGetMessage (AMAYA, AM_PARSE), 10, 1, TRUE);
   TtaSetTextForm (BaseDialog + FilterText, ScanFilter);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + OpenForm, FALSE);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                OpenDocInNewWindow (Document document, View view)
#else
void                OpenDocInNewWindow (document, view)
Document            document;
View                view;

#endif
{
   InNewWindow = TRUE;
   InitOpenDocForm (document, view);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                OpenDoc (Document document, View view)
#else
void                OpenDoc (document, view)
Document            document;
View                view;

#endif
{
   if (TtaIsDocumentModified (document))
     {
	InitConfirm (document, view, TtaGetMessage (AMAYA, AM_DOC_MODIFIED));
	if (UserAnswer)
	   TtaSetDocumentUnmodified (document);
	else
	   /* abort the command */
	   return;
     }
   /* load the new document */
   InNewWindow = FALSE;
   InitOpenDocForm (document, view);
}

/*----------------------------------------------------------------------
   InitDocView prepares the main view of a new document.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Document     InitDocView (Document doc, char *pathname)
#else
static Document     InitDocView (doc, pathname)
Document            doc;
char               *pathname;

#endif
{
   char                tempname[MAX_LENGTH];
   char                temppath[MAX_LENGTH];
   View                mainView, structView, altView, linksView, tocView;
   Document            old_doc;
   boolean             opened;
   int                 x, y, w, h;

   old_doc = doc;		/* previous document */
   if (doc != 0 && !TtaIsDocumentModified (doc))
     {
        /* close the Alternate view if it is open */
	altView = TtaGetViewFromName (doc, "Alternate_view");
	if (altView != 0)
	   if (TtaIsViewOpened (doc, altView))
	     {
		TtaCloseView (doc, altView);
		/* reset the corresponding Toggle item in the Views menu */
		TtaSetToggleItem (doc, 1, Views, TShowAlternate, FALSE);
	     }
        /* close the Structure view if it is open */
	structView = TtaGetViewFromName (doc, "Structure_view");
	if (structView != 0)
	   if (TtaIsViewOpened (doc, structView))
	     {
		TtaCloseView (doc, structView);
		/* reset the corresponding Toggle item in the Views menu */
		TtaSetToggleItem (doc, 1, Views, TShowStructure, FALSE);
	     }
        /* close the Links view if it is open */
	linksView = TtaGetViewFromName (doc, "Links_view");
	if (linksView != 0)
	   if (TtaIsViewOpened (doc, linksView))
	     {
		TtaCloseView (doc, linksView);
		/* reset the corresponding Toggle item in the Views menu */
		TtaSetToggleItem (doc, 1, Views, TShowLinks, FALSE);
	     }
        /* close the Table_of_contents view if it is open */
	tocView = TtaGetViewFromName (doc, "Table_of_contents");
	if (tocView != 0)
	   if (TtaIsViewOpened (doc, tocView))
	     {
		TtaCloseView (doc, tocView);
		/* reset the corresponding Toggle item in the Views menu */
		TtaSetToggleItem (doc, 1, Views, TShowToC, FALSE);
	     }
	TtaSetToggleItem (doc, 1, Views, TShowMapAreas, FALSE);
	TtaSetToggleItem (doc, 1, Special, TSectionNumber, FALSE);
	/* remove the current selection */
	TtaUnselect (doc);
	UpdateContextSensitiveMenus (doc);
	TtaFreeView (doc, 1);
	opened = TRUE;
	old_doc = 0;	/* the previous document doesn't exist any more */
     }
   else
      opened = FALSE;

   /* open the main view */
   TtaExtractName (pathname, temppath, tempname);
   if (tempname[0] == EOS)
      /* there is a slash at the end of the path */
      strcpy (tempname, "noname.html");

   doc = TtaNewDocument ("HTML", tempname);
   if (doc < DocumentTableLength)
     {
	/* assign a presentation model to the document */
	if (TtaGetScreenDepth () > 1)
	   TtaSetPSchema (doc, "HTMLP");
	else
	   TtaSetPSchema (doc, "HTMLPBW");

	/* open the main view */
	TtaSetNotificationMode (doc, 1);
	TtaGetViewGeometry (doc, "Formatted_view", &x, &y, &w, &h);
	/* change the position slightly to avoid hiding completely the main
	   view of other documents */
	x += (((int) doc) - 1) * 5;
	y += (((int) doc) - 1) * 5;
	mainView = TtaOpenMainView (doc, x, y, w, h);
	if (mainView == 0)
	  {
	     TtaCloseDocument (doc);
	     return 0;
	  }
	if (!opened)
	  {
	     /* Add a button */
	     TtaAddButton (doc, 1, stopN, StopTransfer,
			   TtaGetMessage (AMAYA, AM_BUTTON_INTERRUPT));
	     TtaAddButton (doc, 1, iconBack, GotoPreviousHTML,
			   TtaGetMessage (AMAYA, AM_BUTTON_PREVIOUS));
	     TtaAddButton (doc, 1, iconForward, GotoNextHTML,
			   TtaGetMessage (AMAYA, AM_BUTTON_NEXT));
	     TtaAddButton (doc, 1, iconReload, Reload,
			   TtaGetMessage (AMAYA, AM_BUTTON_RELOAD));
	     TtaAddButton (doc, 1, None, NULL, NULL);

	     TtaAddButton (doc, 1, iconSave, SaveDocument,
			   TtaGetMessage (AMAYA, AM_BUTTON_SAVE));
	     TtaAddButton (doc, 1, iconPrint, TtcPrint,
			   TtaGetMessage (AMAYA, AM_BUTTON_PRINT));
	     TtaAddButton (doc, 1, iconFind, TtcSearchText,
			   TtaGetMessage (AMAYA, AM_BUTTON_SEARCH));
	     TtaAddButton (doc, 1, None, NULL, NULL);

	     IButton =  TtaAddButton (doc, 1, iconI, SetCharEmphasis,
				     TtaGetMessage (AMAYA, AM_BUTTON_ITALICS));
	     BButton =  TtaAddButton (doc, 1, iconB, SetCharStrong,
				      TtaGetMessage (AMAYA, AM_BUTTON_BOLD));
	     TTButton = TtaAddButton (doc, 1, iconT, SetCharCode,
				      TtaGetMessage (AMAYA, AM_BUTTON_CODE));
	     TtaAddButton (doc, 1, iconCSS, InitCSSDialog,
			   TtaGetMessage (AMAYA, AM_BUTTON_CSS));
	     TtaAddButton (doc, 1, None, NULL, NULL);

	     TtaAddButton (doc, 1, iconImage, CreateImage,
			   TtaGetMessage (AMAYA, AM_BUTTON_IMG));
	     TtaAddButton (doc, 1, iconH1, CreateHeading1,
			   TtaGetMessage (AMAYA, AM_BUTTON_H1));
	     TtaAddButton (doc, 1, iconH2, CreateHeading2,
			   TtaGetMessage (AMAYA, AM_BUTTON_H2));
	     TtaAddButton (doc, 1, iconH3, CreateHeading3,
			   TtaGetMessage (AMAYA, AM_BUTTON_H3));
	     TtaAddButton (doc, 1, iconBullet, CreateList,
			   TtaGetMessage (AMAYA, AM_BUTTON_UL));
	     TtaAddButton (doc, 1, iconNum, CreateNumberedList,
			   TtaGetMessage (AMAYA, AM_BUTTON_OL));
	     TtaAddButton (doc, 1, iconDL, CreateDefinitionList,
			   TtaGetMessage (AMAYA, AM_BUTTON_DL));
	     TtaAddButton (doc, 1, iconLink, CreateOrChangeLink,
			   TtaGetMessage (AMAYA, AM_BUTTON_LINK));
	     TtaAddButton (doc, 1, iconTable, CreateTable,
			   TtaGetMessage (AMAYA, AM_BUTTON_TABLE));
#ifdef AMAYA_PLUGIN
	     TtaAddButton (doc, 1, iconPlugin, TtaCreateFormPlugin,
			   TtaGetMessage (AMAYA, AM_BUTTON_PLUGIN));
#endif /* AMAYA_PLUGIN */
#ifdef AMAYA_JAVA
	     TtaAddButton (doc, 1, iconJava, CreateFormJava,
			   TtaGetMessage (AMAYA, AM_BUTTON_JAVA));
#endif /* AMAYA_JAVA */
	     TtaAddTextZone (doc, 1, TtaGetMessage (AMAYA, AM_LOCATION), TRUE,
			     TextURL);
	     TtaAddTextZone (doc, 1, TtaGetMessage (AMAYA, AM_TITLE), TRUE,
			     TextTitle);

	     /* save the path or URL of the document */
	     TtaSetDocumentDirectory (doc, temppath);
	     /* disable auto save */
	     TtaSetDocumentBackUpInterval (doc, 0);

	     TtaSetToggleItem (doc, 1, Views, TShowButtonbar, TRUE);
	     TtaSetToggleItem (doc, 1, Views, TShowTextZone, TRUE);
	     TtaSetToggleItem (doc, 1, Views, TShowMapAreas, FALSE);

	     /* if we open the new document in a new view, control */
	     /* is transferred from previous document to new document */
	     if (old_doc != doc && old_doc != 0)
	       {
		  ResetStop (old_doc);
		  /* clear the status line of previous document */
		  TtaSetStatus (old_doc, 1, " ", NULL);
		  ActiveTransfer (doc);
	       }
	  }
     }
   return (doc);
}

/*----------------------------------------------------------------------
   LoadHTMLDocument starts the parsing of the new document and         
   stores its path (or URL) into the document table.       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Document     LoadHTMLDocument (Document doc, char *pathname, char *tempfile, char *documentname)
#else
static Document     LoadHTMLDocument (doc, pathname, tempfile, documentname)
Document            doc;
char               *pathname;
char               *tempfile;
char               *documentname;

#endif
{
   char                tempdocument[MAX_LENGTH];
   char                tempdir[MAX_LENGTH];
   int                 i;
   char               *s;
   Document            newdoc;
   FILE               *tmp_fp;

   if (!IsTextName (pathname) && tempfile[0] != EOS)
     {
	/* The document is not an HTML file and cannot be parsed */
	/* rename the temporary file */
	strcpy (SavingFile, tempfile);
	SavingDocument = (Document) None;
	SavingObject = (Document) None;
	TtaExtractName (pathname, tempfile, tempdocument);
	/* reinitialize directories and document lists */
	strcpy (pathname, DirectoryName);
	strcat (pathname, DIR_STR);
	strcat (pathname, tempdocument);
	ResetStop (doc);
	InitSaveObjectForm (doc, 1, SavingFile, pathname);
     }
   else if (pathname[0] != EOS)
     {
	if (DocumentURLs[(int) doc])
	   /* free the previous document */
	   newdoc = InitDocView (doc, pathname);
	else
	   newdoc = doc;

	/* what we have to do if doc and targetDocument are different */
	if (tempfile[0] != EOS)
	  {
	     /* It is a document loaded from the Web */
	     if (!TtaFileExist (tempfile))
	       {
		  /* Nothing is loaded */
		  ResetStop (doc);
		  return (0);
	       }
	     /* we have to rename the temporary file */
#            ifndef _WINDOWS
	     sprintf (tempdir, "%s%s%d%s", TempFileDirectory, DIR_STR, newdoc, DIR_STR);
#            else  /* _WINDOWS */
	     sprintf (tempdir, "C:\\TEMP\\AMAYA\\%d\\", newdoc);
#            endif /* _WINDOWS */
	     strcpy (tempdocument, tempdir);
	     strcat (tempdocument, documentname);
	     if (doc != newdoc)
	       {
#                 ifndef _WINDOWS
		  tmp_fp = fopen (tempdir, "r");
#                 else  /* _WINDOWS */
		  tmp_fp = fopen (tempdir, "rb");
#                 endif /* _WINDOWS */
		  if (tmp_fp == 0)
		     /*directory did not exist */
		     mkdir (tempdir, S_IRWXU);
		  else
		     fclose (tmp_fp);
		  /* now we can rename the local name of a remote document */
		  TtaFileCopy (tempfile, tempdocument);
		  TtaFileUnlink (tempfile);
	       }
	     else
	       {
		  /* now we can rename the local name of a remote document */
		  TtaFileUnlink (tempdocument);
		  rename (tempfile, tempdocument);
	       }
	  }
	else
	  {
	     /* It is a local document */
	     strcpy (tempdocument, pathname);
	     TtaExtractName (tempdocument, tempdir, documentname);
	  }

	/* save the document name into the document table */
	i = strlen (pathname) + 1;
	s = TtaGetMemory (i);
	strcpy (s, pathname);
	DocumentURLs[(int) newdoc] = s;
	if (TtaGetViewFrame (newdoc, 1) != 0)
	   /* this document is displayed */
	   TtaSetTextZone (newdoc, 1, 1, s);
	StartHTMLParser (newdoc, tempdocument, documentname, tempdir, pathname);
	/* add this URL in history file */
	AddHTMLHistory (DocumentURLs[newdoc]);
     }
   return (newdoc);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                Reload (Document document, View view)
#else
void                Reload (document, view)
Document            document;
View                view;

#endif
{
   char                tempfile[MAX_LENGTH];
   char                pathname[MAX_LENGTH];
   char                documentname[MAX_LENGTH];
   Document            newdoc;
   Document            res;
   int                 toparse;

   if (DocumentURLs[(int) document] == NULL)
      /* the document has not been loaded yet */
      return;

   /* abort all current echanges concerning this docid */
   StopTransfer (document, 1);

   if (TtaIsDocumentModified (document))
     {
	InitConfirm (document, view, TtaGetMessage (AMAYA, AM_DOC_MODIFIED));
	if (UserAnswer)
	   TtaSetDocumentUnmodified (document);
	else
	   /* abort the command */
	   return;
     }
   /* reload the document */
   NormalizeURL (DocumentURLs[(int) document], 0, pathname, documentname);
   W3Loading = document;	/* this document is currently in load */
   newdoc = InitDocView (document, pathname);

#ifdef AMAYA_JAVA
   /*
    * Check against concurrent loading on the same frame.
    */
   if (FilesLoading[newdoc]) {
       return;
   }
#endif 

   tempfile[0] = EOS;
   toparse = 0;
   ActiveTransfer (newdoc);
   if (IsW3Path (pathname))
     {
	/* load the document from the Web */
       toparse = GetObjectWWW (newdoc, pathname, NULL, tempfile,
			       AMAYA_SYNC | AMAYA_NOCACHE,
			       NULL, NULL, NULL, NULL, YES);
	TtaHandlePendingEvents ();
     }
   if (toparse != -1)
     {
        TtaSetCursorWatch (0, 0);
	/* do we need to control the last slash here? */
	res = LoadHTMLDocument (newdoc, pathname, tempfile, documentname);
	W3Loading = 0;		/* loading is complete now */
	TtaHandlePendingEvents ();
	/* fetch and display all images referred by the document */
	FetchAndDisplayImages (res, AMAYA_NOCACHE);
	TtaResetCursor (0, 0);
     }
   ResetStop(newdoc);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ShowStructure (Document document, View view)
#else
void                ShowStructure (document, view)
Document            document;
View                view;

#endif
{
   View                structView;
   int                 x, y, w, h;

   structView = TtaGetViewFromName (document, "Structure_view");
   if (view == structView)
     {
	TtaCloseView (document, view);
	TtaSetToggleItem (document, 1, Views, TShowStructure, FALSE);
     }
   else if (structView != 0 && TtaIsViewOpened (document, structView))
      TtaCloseView (document, structView);
   else
     {
	TtaGetViewGeometry (document, "Structure_view", &x, &y, &w, &h);
	structView = TtaOpenView (document, "Structure_view", x, y, w, h);
	TtcSwitchButtonBar (document, structView);
	TtcSwitchCommands (document, structView);
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ShowAlternate (Document document, View view)
#else
void                ShowAlternate (document, view)
Document            document;
View                view;

#endif
{
   View                altView;
   int                 x, y, w, h;

   altView = TtaGetViewFromName (document, "Alternate_view");
   if (view == altView)
     {
	TtaCloseView (document, view);
	TtaSetToggleItem (document, 1, Views, TShowAlternate, FALSE);
     }
   else if (altView != 0 && TtaIsViewOpened (document, altView))
      TtaCloseView (document, altView);
   else
     {

	TtaGetViewGeometry (document, "Alternate_view", &x, &y, &w, &h);
	altView = TtaOpenView (document, "Alternate_view", x, y, w, h);
	TtcSwitchButtonBar (document, altView);
	TtcSwitchCommands (document, altView);
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ShowLinks (Document document, View view)
#else
void                ShowLinks (document, view)
Document            document;
View                view;

#endif
{
   View                linksView;
   int                 x, y, w, h;

   linksView = TtaGetViewFromName (document, "Links_view");
   if (view == linksView)
     {
	TtaCloseView (document, view);
	TtaSetToggleItem (document, 1, Views, TShowLinks, FALSE);
     }
   else if (linksView != 0 && TtaIsViewOpened (document, linksView))
      TtaCloseView (document, linksView);
   else
     {

	TtaGetViewGeometry (document, "Links_view", &x, &y, &w, &h);
	linksView = TtaOpenView (document, "Links_view", x, y, w, h);
	TtcSwitchButtonBar (document, linksView);
	TtcSwitchCommands (document, linksView);
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ShowToC (Document document, View view)
#else
void                ShowToC (document, view)
Document            document;
View                view;

#endif
{
   View                tocView;
   int                 x, y, w, h;

   tocView = TtaGetViewFromName (document, "Table_of_contents");
   if (view == tocView)
     {
	TtaCloseView (document, view);
	TtaSetToggleItem (document, 1, Views, TShowToC, FALSE);
     }
   else if (tocView != 0 && TtaIsViewOpened (document, tocView))
      TtaCloseView (document, tocView);
   else
     {

	TtaGetViewGeometry (document, "Table_of_contents", &x, &y, &w, &h);
	tocView = TtaOpenView (document, "Table_of_contents", x, y, w, h);
	TtcSwitchButtonBar (document, tocView);
	TtcSwitchCommands (document, tocView);
     }
}


/*----------------------------------------------------------------------
   ViewToOpen                                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             ViewToOpen (NotifyDialog * event)
#else
boolean             ViewToOpen (event)
NotifyDialog       *event;

#endif
{
   View                view, sview;
   int                 x, y, w, h;

   view = event->view;
   if (view == 1)
     return FALSE;		/* let Thot perform normal operation */
   else
     {
     sview = TtaGetViewFromName (event->document, "Structure_view");
     if (view == sview)
       {
       TtaGetViewGeometry (event->document, "Structure_view", &x, &y, &w, &h);
       view = TtaOpenView (event->document, "Structure_view", x, y, w, h);
       TtcSwitchButtonBar (event->document, view);
       TtcSwitchCommands (event->document, view);
       TtaSetToggleItem (event->document, 1, Views, TShowStructure, TRUE);
       }
     else
       {
       sview = TtaGetViewFromName (event->document, "Alternate_view");
       if (view == sview)
         {
	 TtaGetViewGeometry (event->document, "Alternate_view", &x, &y, &w, &h);
	 view = TtaOpenView (event->document, "Alternate_view", x, y, w, h);
	 TtcSwitchButtonBar (event->document, view);
	 TtcSwitchCommands (event->document, view);
	 TtaSetToggleItem (event->document, 1, Views, TShowAlternate, TRUE);
	 }
       else
	 {
	 sview = TtaGetViewFromName (event->document, "Links_view");
	 if (view == sview)
	   {
	   TtaGetViewGeometry (event->document, "Links_view", &x, &y, &w, &h);
	   view = TtaOpenView (event->document, "Links_view", x, y, w, h);
	   TtcSwitchButtonBar (event->document, view);
	   TtcSwitchCommands (event->document, view);
	   TtaSetToggleItem (event->document, 1, Views, TShowLinks, TRUE);
	   }
	 else
	   {
	   sview = TtaGetViewFromName (event->document, "Table_of_contents");
	   if (view == sview)
	     {
	     TtaGetViewGeometry (event->document, "Table_of_contents", &x, &y,
				 &w, &h);
	     view = TtaOpenView (event->document, "Table_of_contents", x, y, w,
				 h);
	     TtcSwitchButtonBar (event->document, view);
	     TtcSwitchCommands (event->document, view);
	     TtaSetToggleItem (event->document, 1, Views, TShowToC, TRUE);
	     }
	   }
	 }
       }
       return TRUE;		/* don't let Thot perform normal operation */
     }
}


/*----------------------------------------------------------------------
   ViewToClose                                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             ViewToClose (NotifyDialog * event)
#else
boolean             ViewToClose (event)
NotifyDialog       *event;

#endif
{
   Document            document;
   View                view, structView, altView, linksView, tocView;

   view = event->view;
   document = event->document;
   structView = TtaGetViewFromName (document, "Structure_view");
   altView = TtaGetViewFromName (document, "Alternate_view");
   linksView = TtaGetViewFromName (document, "Links_view");
   tocView = TtaGetViewFromName (document, "Table_of_contents");
   if (view != 1)
     {
	if (view == structView)
	  TtaSetToggleItem (document, 1, Views, TShowStructure, FALSE);
	else if (view == altView)
	  TtaSetToggleItem (document, 1, Views, TShowAlternate, FALSE);
	else if (view == linksView)
	  TtaSetToggleItem (document, 1, Views, TShowLinks, FALSE);
	else if (view == tocView)
	  TtaSetToggleItem (document, 1, Views, TShowToC, FALSE);
	return FALSE;		/* let Thot perform normal operation */
     }
   else
     /* closing main view */
     if (TtaIsDocumentModified (document))
     {
	InitConfirm (document, view, TtaGetMessage (AMAYA, AM_DOC_MODIFIED));
	if (UserAnswer)
	   TtaSetDocumentUnmodified (document);
	else
	   /* abort the command */
	  return TRUE;		/* don't let Thot perform normal operation */
     }

   if (structView != 0 && TtaIsViewOpened (document, structView))
     TtaCloseView (document, structView);
   if (altView != 0 && TtaIsViewOpened (document, altView))
     TtaCloseView (document, altView);
   if (linksView != 0 && TtaIsViewOpened (document, linksView))
     TtaCloseView (document, linksView);
   if (tocView != 0 && TtaIsViewOpened (document, tocView))
     TtaCloseView (document, tocView);
   return FALSE;		/* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
  GetHTMLDocument loads the document if it is not loaded yet and    
  calls the parser if the document can be parsed.
    - documentPath: can be relative or absolute adress.
    - form_data: the text to be posted.
    - doc: the document which can be removed if not updated.
    - baseDoc: the document which documentPath is relative to.
    - DC_event: DC_FORM_POST for a post request, DC_TRUE for a double click.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Document            GetHTMLDocument (char *documentPath, char *form_data, Document doc, Document baseDoc, DoubleClickEvent DC_event)
#else
Document            GetHTMLDocument (documentPath, form_data, doc, baseDoc, DC_event)
char               *documentPath;
char               *form_data;
Document            doc;
Document            baseDoc;
DoubleClickEvent    DC_event;

#endif
{
   Element             elFound;
   Document            newdoc, res;
   char                tempfile[MAX_LENGTH];
   char                tempdocument[MAX_LENGTH];
   char                parameters[MAX_LENGTH];
   char                target[MAX_LENGTH];
   char                pathname[MAX_LENGTH];
   char                documentname[MAX_LENGTH];
   int                 toparse;
   int                 i;
   int                 slash;
   char               *s;

   /* Extract parameters if necessary */
   strcpy (tempdocument, documentPath);
   ExtractParameters (tempdocument, parameters);
   /* Extract the target if necessary */
   ExtractTarget (tempdocument, target);
   /* Add the  base content if necessary */
   if (DC_event & DC_TRUE)
      NormalizeURL (tempdocument, baseDoc, pathname, documentname);
   else
      NormalizeURL (tempdocument, 0, pathname, documentname);

   if (parameters[0] == EOS)
      newdoc = IsDocumentLoaded (pathname);
   else
     {
	/* we need to ask the server */
	newdoc = 0;
	strcat (pathname, "?");
	strcat (pathname, parameters);
     }

   if ((DC_event & DC_FORM_POST) || (DC_event & DC_FORM_GET))
     {
	/* special checks for forms */
	if (!IsW3Path (pathname))
	  {
	     /* the target document doesn't exist */
	     TtaSetStatus (baseDoc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), pathname);
	     return (0);
	  }
	/* we always have a fresh newdoc for forms */
	newdoc = 0;
     }

   if (newdoc == 0)
     {
	/* document not loaded yet */
	if (DC_event & DC_TRUE && !IsW3Path (pathname) && !TtaFileExist (pathname))
	   /* the target document doesn't exist */
	   TtaSetStatus (baseDoc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), pathname);
	else
	  {
	     tempfile[0] = EOS;
	     toparse = 0;
	     /* In case of initial document, open the view before loading */
	     if (doc == 0)
	       {
		 newdoc = InitDocView (doc, pathname);
		 if (newdoc == 0)
		   /* cannot display the new document */
		   return (newdoc);
	       }
	     else
	       {
		  /* stop current transfer for previous document */
		  StopTransfer (baseDoc, 1);
		  newdoc = doc;
	       }

#ifdef AMAYA_JAVA
             /* Check against concurrent loading on the same frame */
	     if (FilesLoading[newdoc])
	       return(0);
#endif /* AMAYA_JAVA */

	     /* this document is currently in load */
	     W3Loading = newdoc;
	     ActiveTransfer (newdoc);
	     if (IsW3Path (pathname))
	       {
		 /* load the document from the Web */
		  if (DC_event & DC_FORM_POST)
		    toparse = GetObjectWWW (newdoc, pathname, form_data, tempfile,
					    AMAYA_FORM_POST | AMAYA_SYNC,
					    NULL, NULL, NULL, NULL, YES);
		  else
		    {
		      if (!strcmp (documentname, "noname.html"))
			{
			  slash = strlen (pathname);
			  if (slash && pathname[slash - 1] != DIR_SEP)
			    strcat (pathname, DIR_STR);
			  toparse = GetObjectWWW (newdoc, pathname, NULL, tempfile, AMAYA_SYNC, NULL, NULL, NULL, NULL, YES);
			  /* keep the real name */
			  NormalizeURL (pathname, 0, tempdocument, documentname);
			}
		      else 
			toparse = GetObjectWWW (newdoc, pathname, NULL, tempfile, AMAYA_SYNC, NULL, NULL, NULL, NULL, YES);
		    }
		  TtaHandlePendingEvents ();
	       }

	     /* memorize the initial newdoc value in doc because LoadHTMLDocument */
	     /* will opem a new document if newdoc is a modified document */
	     if (toparse != -1)
	       {
		  /* do we need to control the last slash here? */
		  res = LoadHTMLDocument (newdoc, pathname, tempfile, documentname);
		  W3Loading = 0;		/* loading is complete now */
		  if (res == 0)
		    {
		      /* cannot load the document */
		      ResetStop(newdoc);
		      return (res);
		    }
		  else if (newdoc != res)
		    newdoc = res;

		  TtaHandlePendingEvents ();
		  /* fetch and display all images referred by the document */
		  if (doc == baseDoc)
		    /* it's not a temporary document */
		    FetchAndDisplayImages (newdoc, 0);
	       }
	     else
	       {
		 if (DocumentURLs[(int) newdoc] == NULL)
		   {
		     /* save the document name into the document table */
		     i = strlen (pathname) + 1;
		     s = TtaGetMemory (i);
		     strcpy (s, pathname);
		     DocumentURLs[(int) newdoc] = s;
		     TtaSetTextZone (newdoc, 1, 1, s);
		   }
		 W3Loading = 0;	/* loading is complete now */
	       }
	     ResetStop(newdoc);
	  }
     }
   /* select the target if present */
   if (target[0] != EOS && newdoc != 0)
     {
       /* attribute HREF contains the NAME of a target anchor */
       elFound = SearchNAMEattribute (newdoc, target, NULL);
       if (elFound != NULL)
	 {
	   /* show the target element in all views */
	   for (i = 1; i < 4; i++)
	     if (TtaIsViewOpened (newdoc, i))
	       TtaShowElement (newdoc, i, elFound, 10);
	 }
     }
   return (newdoc);
}


/*----------------------------------------------------------------------
   NormalizeFile normalizes  local names.                             
   Return TRUE if target and src differ.                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             NormalizeFile (char *src, char *target)
#else
boolean             NormalizeFile (src, target)
char               *src;
char               *target;

#endif
{
   char               *s;
   boolean             change;

   change = FALSE;
   if (src[0] == '~')
     {
	/* replace ~ */
	s = (char *) TtaGetEnvString ("HOME");
	strcpy (target, s);
	strcat (target, &src[1]);
	change = TRUE;
     }
   else if (strncmp (src, "file:", 5) == 0)
     {
	/* remove the prefix file: */
	if (src[5] == EOS)
	   strcpy (target, DIR_STR);
	else if (src[0] == '~')
	  {
	    /* replace ~ */
	    s = (char *) TtaGetEnvString ("HOME");
	    strcpy (target, s);
	    strcat (target, &src[5]);
	  }
	else
	   strcpy (target, &src[5]);
	change = TRUE;
     }
   else
      strcpy (target, src);

   return (change);
}


/*----------------------------------------------------------------------
   Callback procedure for dialogue events.                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackDialogue (int ref, int typedata, char *data)
#else
void                CallbackDialogue (ref, typedata, data)
int                 ref;
int                 typedata;
char               *data;

#endif
{
  int                 val, i;
  char                tempfile[MAX_LENGTH];
  char                tempname[MAX_LENGTH];
  AttributeType       attrType;
  Attribute           attrHREF;
  boolean             change;

   val = (int) data;
   switch (ref - BaseDialog)
     {
     case OptionMenu:
       ReturnOption = val;
       TtaDestroyDialogue (BaseDialog + OptionMenu);
       break;
     case OpenForm:
       /* *********Load URL or local document********* */
       if (val == 2)
	 /* Clear */
	 {
	   LastURLName[0] = EOS;
	   TtaSetTextForm (BaseDialog + URLName, LastURLName);
	 }
       else if (val == 3)
	 /* Parse */
	 {
	   /* reinitialize directories and document lists */
	   TtaListDirectory (DirectoryName, BaseDialog + OpenForm,
			     TtaGetMessage (LIB, TMSG_DOC_DIR), BaseDialog + DirSelect,
			     ScanFilter, TtaGetMessage (AMAYA, AM_FILES), BaseDialog + DocSelect);
	 }
       else
	 {
	   TtaDestroyDialogue (BaseDialog + OpenForm);
	   if (val == 1)
	     /* OK */
	     {
	       if (LastURLName[0] != EOS)
		 {
		   /* load an URL */
		   if (InNewWindow)
		     GetHTMLDocument (LastURLName, NULL, 0, 0, DC_FALSE);
		   else
		     GetHTMLDocument (LastURLName, NULL, CurrentDocument, CurrentDocument, DC_FALSE);
		 }
	       else if (DirectoryName[0] != EOS && DocumentName[0] != EOS)
		 {
		   /* load a local file */
		   strcpy (tempfile, DirectoryName);
		   strcat (tempfile, DIR_STR);
		   strcat (tempfile, DocumentName);
		   if (TtaFileExist (tempfile))
		     {
		       if (InNewWindow)
			 GetHTMLDocument (tempfile, NULL, 0, 0, DC_FALSE);
		       else
			 GetHTMLDocument (tempfile, NULL, CurrentDocument, CurrentDocument, DC_FALSE);
		     }
		   else
		     TtaSetStatus (CurrentDocument, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), tempfile);
		 }
	       else
		 {
		   if (DocumentName[0] != EOS)
		     TtaSetStatus (CurrentDocument, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), DocumentName);
		   else if (DirectoryName[0] != EOS)
		     TtaSetStatus (CurrentDocument, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), DirectoryName);
		   else
		     TtaSetStatus (CurrentDocument, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), "");
		 }
	       CurrentDocument = 0;
	     }
	 }
       break;
     case URLName:
       if (IsW3Path (data))
	 {
	   /* save the URL name */
	   strcpy (LastURLName, data);
	   DocumentName[0] = EOS;
	 }
       else
	 {
	   LastURLName[0] = EOS;
	   change = NormalizeFile (data, tempfile);
	   if (change)
	     TtaSetTextForm (BaseDialog + URLName, tempfile);
	   
	   if (TtaCheckDirectory (tempfile))
	     {
	       strcpy (DirectoryName, tempfile);
	       DocumentName[0] = EOS;
	     }
	   else
	     TtaExtractName (tempfile, DirectoryName, DocumentName);
	 }
       
       break;
     case DirSelect:
       if (!strcmp (data, ".."))
	 {
	   /* suppress last directory */
	   strcpy (tempname, DirectoryName);
	   TtaExtractName (tempname, DirectoryName, tempfile);
	 }
       else
	 {
	   strcat (DirectoryName, DIR_STR);
	   strcat (DirectoryName, data);
	 }
       TtaSetTextForm (BaseDialog + URLName, DirectoryName);
       TtaListDirectory (DirectoryName, BaseDialog + OpenForm,
			 TtaGetMessage (LIB, TMSG_DOC_DIR), BaseDialog + DirSelect,
			 ScanFilter, TtaGetMessage (AMAYA, AM_FILES), BaseDialog + DocSelect);
       DocumentName[0] = EOS;
       break;
     case DocSelect:
       if (DirectoryName[0] == EOS)
	 /* set path on current directory */
	 getcwd (DirectoryName, MAX_LENGTH);
       
       /* Extract suffix from document name */
       strcpy (DocumentName, data);
       LastURLName[0] = EOS;
       /* construct the document full name */
       strcpy (tempfile, DirectoryName);
       strcat (tempfile, DIR_STR);
       strcat (tempfile, DocumentName);
       TtaSetTextForm (BaseDialog + URLName, tempfile);
       break;
       
     case ConfirmForm:
       /* *********Confirm********* */
       UserAnswer = (val == 1);
       TtaDestroyDialogue (BaseDialog + ConfirmForm);
       break;
     case FilterText:
       /* Filter value */
       if (strlen(data) <= NAME_LENGTH)
	 strcpy (ScanFilter, data);
       else
	 TtaSetTextForm (BaseDialog + FilterText, ScanFilter);
       break;
     case FormAnswer:
       /* *********Get an answer********* */
       if (val == 0)
	 {
	   /* no answer */
	   Answer_text[0] = EOS;
	   Answer_name[0] = EOS;
	   Answer_password[0] = EOS;
	 }
       break;
     case AnswerText:
       strncpy (Answer_text, data, MAX_LENGTH);
       Answer_text[MAX_LENGTH - 1] = EOS;
       break;
     case NameText:
       strncpy (Answer_name, data, NAME_LENGTH);
       Answer_text[NAME_LENGTH - 1] = EOS;
       break;
     case PasswordText:
       i = strlen (data);
       if (i < NAME_LENGTH - 1)
	 {
	   if (Lg_password < i)
	     {
	       /* a new char */
	       Answer_password[Lg_password] = data[Lg_password];
	       Display_password[Lg_password] = '*';
	       Answer_password[++Lg_password] = EOS;
	       Display_password[Lg_password] = EOS;
	     }
	   else if (Lg_password > i)
	     {		/* a valid backspace */
	       
	       Lg_password--;
	       Answer_password[Lg_password] = EOS;
	       Display_password[Lg_password] = EOS;
	     }
	 }
       else
	 Answer_password[NAME_LENGTH - 1] = EOS;
       if (i > 0)
	 TtaSetTextForm (BaseDialog + PasswordText, Display_password);
       break;
       
     case ToggleSave:
       /* *********Save document as********* */
       switch (val)
	 {
	 case 0:	/* "Save as HTML" button */
	   SaveAsHTML = !SaveAsHTML;
	   SaveAsText = !SaveAsHTML;
	   TtaSetToggleMenu (BaseDialog + ToggleSave, 1,
			     SaveAsText);
	   break;
	 case 1:	/* "Save as Text" button */
	   SaveAsText = !SaveAsText;
	   SaveAsHTML = !SaveAsText;
	   TtaSetToggleMenu (BaseDialog + ToggleSave, 0,
			     SaveAsHTML);
	   CopyImages = FALSE;
	   TtaSetToggleMenu (BaseDialog + ToggleSave, 3,
			     CopyImages);
	   UpdateURLs = FALSE;
	   TtaSetToggleMenu (BaseDialog + ToggleSave, 4,
			     UpdateURLs);
	   break;
	 case 3:	/* "Copy Images" button */
	   CopyImages = !CopyImages;
	   break;
	 case 4:	/* "Transform URLs" button */
	   UpdateURLs = !UpdateURLs;
	   break;
	 }
       break;
     case SaveForm:
       if (val == 1)
	 {
	   TtaDestroyDialogue (BaseDialog + SaveForm);
	   if (SavingDocument != 0)
	     DoSaveAs ();
	   else if (SavingObject != 0)
	     DoSaveObjectAs ();
	 }
       else if (val == 2)
	 /* Clear */
	 {
	   if (SavingDocument != 0)
	     {
	       DirectoryName[0] = EOS;
	       SaveImgsURL[0] = EOS;
	       DocumentName[0] = EOS;
	       TtaSetTextForm (BaseDialog + NameSave, SaveImgsURL);
	       TtaSetTextForm (BaseDialog + ImgDirSave, SaveImgsURL);
	     }
	 }
       else if (val == 3)
	 /* Parse */
	 {
	   /* reinitialize directories and document lists */
	   if (SavingDocument != 0)
	     TtaListDirectory (DirectoryName, BaseDialog + SaveForm,
			       TtaGetMessage (LIB, TMSG_DOC_DIR),
			       BaseDialog + DirSave, ScanFilter,
			       TtaGetMessage (AMAYA, AM_FILES), BaseDialog + DocSave);
	 }
       else
	 {
	   TtaDestroyDialogue (BaseDialog + SaveForm);
	   SavingDocument = 0;
	   SavingObject = 0;
	 }
       break;
     case NameSave:
       if (!IsW3Path (data))
	 {
	   change = NormalizeFile (data, tempfile);
	   if (change)
	     TtaSetTextForm (BaseDialog + NameSave, tempfile);
	 }
       else
	 strcpy (tempfile, data);
       
       if (tempfile[strlen (tempfile) - 1] == DIR_SEP)
	 {
	   strcpy (DirectoryName, tempfile);
	   DocumentName[0] = EOS;
	   /* reinitialize directories and document lists */
	   TtaListDirectory (DirectoryName, BaseDialog + SaveForm,
			     TtaGetMessage (LIB, TMSG_DOC_DIR), BaseDialog + DirSave,
			     ScanFilter, TtaGetMessage (AMAYA, AM_FILES), BaseDialog + DocSave);
	 }
       else
	 {
	   /* Extract document name */
	   if (SavingDocument != (Document) None)
	     TtaExtractName (tempfile, DirectoryName, DocumentName);
	   else
	     TtaExtractName (tempfile, DirectoryName, ObjectName);
	 }
       break;
     case ImgDirSave:
       strcpy (SaveImgsURL, data);
       break;
     case DirSave:
       if (!strcmp (data, ".."))
	 {
	   /* suppress last directory */
	   strcpy (tempname, DirectoryName);
	   TtaExtractName (tempname, DirectoryName, tempfile);
	 }
       else
	 {
	   strcat (DirectoryName, DIR_STR);
	   strcat (DirectoryName, data);
	 }
       strcpy (tempfile, DirectoryName);
       strcat (tempfile, DIR_STR);
       if (SavingDocument != (Document) None)
	 strcat (tempfile, DocumentName);
       else
	 strcat (tempfile, ObjectName);
       TtaSetTextForm (BaseDialog + NameSave, DirectoryName);
       TtaListDirectory (DirectoryName, BaseDialog + SaveForm,
			 TtaGetMessage (LIB, TMSG_DOC_DIR), BaseDialog + DirSave,
			 ScanFilter, TtaGetMessage (AMAYA, AM_FILES), BaseDialog + DocSave);
       break;
     case DocSave:
       if (DirectoryName[0] == EOS)
	 /* set path on current directory */
	 getcwd (DirectoryName, MAX_LENGTH);
       
       strcpy (DocumentName, data);
       /* construct the document full name */
       strcpy (tempfile, DirectoryName);
       strcat (tempfile, DIR_STR);
       strcat (tempfile, DocumentName);
       TtaSetTextForm (BaseDialog + NameSave, tempfile);
       break;
     case ConfirmSave:
       /* *********SaveConfirm********* */
       UserAnswer = (val == 1);
       TtaDestroyDialogue (BaseDialog + ConfirmSave);
       if (!UserAnswer)
	 {
	   SavingDocument = 0;
	   SavingObject = 0;
	 }
       break;
       
     case AttrHREFForm:
       /* *********HREF Attribute*********** */
       if (val == 1)
	 {
	   /* create an attribute HREF for the Link_Anchor */
	   attrType.AttrSSchema = TtaGetDocumentSSchema (AttrHREFdocument);
	   attrType.AttrTypeNum = HTML_ATTR_HREF_;
	   attrHREF = TtaGetAttribute (AttrHREFelement, attrType);
	   if (attrHREF == 0)
	     {
	       /* create an attribute HREF for the element */
	       attrHREF = TtaNewAttribute (attrType);
	       TtaAttachAttribute (AttrHREFelement, attrHREF, AttrHREFdocument);
	     }
	   TtaSetAttributeText (attrHREF, AttrHREFvalue, AttrHREFelement, AttrHREFdocument);
	 }
       break;
     case AttrHREFText:
       /* save the HREF name */
       strcpy (AttrHREFvalue, data);
       break;
     case ClassForm:
     case ClassSelect:
     case AClassForm:
     case AClassSelect:
       StyleCallbackDialogue (ref, typedata, data);
       break;
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitAmaya (NotifyEvent * event)
#else
void                InitAmaya (event)
NotifyEvent        *event;

#endif
{
   int                 i;
   char               *s;

   if (AmayaInitialized)
      return;
   AmayaInitialized = 1;

   /* initialize status */
   SelectionDoc = 0;
   SelectionInPRE = FALSE;
   SelectionInComment = FALSE;
   SelectionInEM = FALSE;
   SelectionInSTRONG = FALSE;
   SelectionInCITE = FALSE;
   SelectionInDFN = FALSE;
   SelectionInCODE = FALSE;
   SelectionInVAR = FALSE;
   SelectionInSAMP = FALSE;
   SelectionInKBD = FALSE;
   SelectionInI = FALSE;
   SelectionInB = FALSE;
   SelectionInTT = FALSE;
   SelectionInU = FALSE;
   SelectionInSTRIKE = FALSE;
   SelectionInBIG = FALSE;
   SelectionInSMALL = FALSE;
   /* initialize icons */
   stopR = TtaCreatePixmapLogo (stopR_xpm);
   TtaRegisterPixmap("stopR", stopR);
   stopN = TtaCreatePixmapLogo (stopN_xpm);
   TtaRegisterPixmap("stopN", stopN);
   iconSave = TtaCreatePixmapLogo (save_xpm);
   TtaRegisterPixmap("Save", iconSave);
   iconFind = TtaCreatePixmapLogo (find_xpm);
   TtaRegisterPixmap("Find", iconFind);
   iconReload = TtaCreatePixmapLogo (Reload_xpm);
   TtaRegisterPixmap("Reload", iconReload);
   iconI = TtaCreatePixmapLogo (I_xpm);
   TtaRegisterPixmap("Italic", iconI);
   iconB = TtaCreatePixmapLogo (B_xpm);
   TtaRegisterPixmap("Bold", iconB);
   iconT = TtaCreatePixmapLogo (T_xpm);
   TtaRegisterPixmap("TeleType", iconT);
   iconBack = TtaCreatePixmapLogo (Back_xpm);
   TtaRegisterPixmap("Back", iconBack);
   iconForward = TtaCreatePixmapLogo (Forward_xpm);
   TtaRegisterPixmap("Forward", iconForward);
   iconH1 = TtaCreatePixmapLogo (H1_xpm);
   TtaRegisterPixmap("H1", iconH1);
   iconH2 = TtaCreatePixmapLogo (H2_xpm);
   TtaRegisterPixmap("H2", iconH2);
   iconH3 = TtaCreatePixmapLogo (H3_xpm);
   TtaRegisterPixmap("H3", iconH3);
   iconPrint = TtaCreatePixmapLogo (Print_xpm);
   TtaRegisterPixmap("Print", iconPrint);
   iconCSS = TtaCreatePixmapLogo (css_xpm);
   TtaRegisterPixmap("CSS", iconCSS);
   iconBullet = TtaCreatePixmapLogo (Bullet_xpm);
   TtaRegisterPixmap("Bullet", iconBullet);
   iconNum = TtaCreatePixmapLogo (Num_xpm);
   TtaRegisterPixmap("Numbered", iconNum);
   iconImage = TtaCreatePixmapLogo (Image_xpm);
   iconDL = TtaCreatePixmapLogo (DL_xpm);
   TtaRegisterPixmap("Definition", iconDL);
   iconLink = TtaCreatePixmapLogo (Link_xpm);
   TtaRegisterPixmap("Link", iconLink);
   iconTable = TtaCreatePixmapLogo (Table_xpm);
   TtaRegisterPixmap("Table", iconTable);
#  ifdef AMAYA_PLUGIN
   iconPlugin = TtaCreatePixmapLogo (Plugin_xpm);
   TtaRegisterPixmap("Plugin", iconPlugin);
#  endif
#ifdef AMAYA_JAVA
   iconJava = TtaCreatePixmapLogo (Java_xpm);
   TtaRegisterPixmap("Java", iconJava);
#endif

   TargetName = NULL;
   /* initialize temporary directory for loaded files */
   s = (char *) TtaGetEnvString ("HOME");

   if (s)
      strcpy (TempFileDirectory, s);
   else
#  ifdef _WINDOWS
     {
	s = (char *) TtaGetEnvString ("TEMP");
	if (s)
	   strcpy (TempFileDirectory, s);
	else
	   strcpy (TempFileDirectory, "C:\\TEMP");
     }
   strcat (TempFileDirectory, DIR_STR);
   strcat (TempFileDirectory, ".amaya");
   i = _mkdir (TempFileDirectory);
   if (i != 0 && errno != EEXIST)
     {
	strcpy (TempFileDirectory, "C:\\TEMP\\AMAYA");
	i = _mkdir (TempFileDirectory);
	if (i != 0 && errno != EEXIST)
#  else  /* !_WINDOWS */
     strcpy (TempFileDirectory, "/tmp");
   strcat (TempFileDirectory, DIR_STR);
   strcat (TempFileDirectory, ".amaya");
   i = mkdir (TempFileDirectory, S_IRWXU);
   if (i != 0 && errno != EEXIST)
     {
	strcpy (TempFileDirectory, "/tmp/.amaya");
	i = mkdir (TempFileDirectory, S_IRWXU);
	if (i != 0 && errno != EEXIST)
#  endif /* !_WINDOWS */
	  {
	     fprintf (stderr, "cannot create %s\n", TempFileDirectory);
	     exit (1);
	  }
     }
   /* add the temporary directory in document path */
   TtaAppendDocumentPath (TempFileDirectory);

   /* allocate working buffers */
   LastURLName = TtaGetMemory (MAX_LENGTH);
   LastURLName[0] = EOS;
   DirectoryName = TtaGetMemory (MAX_LENGTH);

   /* set path on current directory */
   getcwd (DirectoryName, MAX_LENGTH);
   DocumentName = TtaGetMemory (MAX_LENGTH);
   DocumentName[0] = EOS;
   ObjectName = TtaGetMemory (MAX_LENGTH);
   ObjectName[0] = EOS;
   SaveImgsURL = TtaGetMemory (MAX_LENGTH);
   SaveImgsURL[0] = EOS;
   strcpy (ScanFilter, ".*html*");
   SaveAsHTML = TRUE;
   SaveAsText = FALSE;
   CopyImages = 1;
   UpdateURLs = 1;
   SavingFile = TtaGetMemory (MAX_LENGTH);
   AttrHREFvalue = TtaGetMemory (MAX_LENGTH);
   AttrHREFvalue[0] = EOS;

#ifdef WITH_SOCKS
   SOCKSinit ("amaya");
#endif

   /* initialize parser mapping table and HTLib */
   InitMapping ();

#ifndef AMAYA_JAVA
   QueryInit ();
#endif

   AMAYA = TtaGetMessageTable ("amayamsg", AMAYA_MSG_MAX);
   /* allocate callbacks for amaya */
   BaseDialog = TtaSetCallback (CallbackDialogue, MAX_REF);

   /* init the Picture context */
   InitImage ();
   /* init the Picture context */
   InitPrint ();
   /* init the CSS context */
   InitCSS ();
   /* initialize the structure transformation context */
   InitTransform ();
   /* initialize automaton for the HTML parser */
   InitAutomaton ();

   /* initialize document table */
   for (i = 1; i < DocumentTableLength; i++)
       DocumentURLs[i] = NULL;

   CurrentDocument = 0;
   InNewWindow = FALSE;
   if (appArgc % 2 == 0)
      /* The last argument in the command line is the document to be opened */
      s = appArgv[appArgc - 1];
   else
      /* No argument in the command line. Try the variable HOME_PAGE */
      s = (char *) TtaGetEnvString ("HOME_PAGE");

   if (!s)
      /* No argument in the command line, no HOME_PAGE variable. Open the */
      /* default Amaya URL */
     {
	strcpy (LastURLName, AMAYA_PAGE);
	CallbackDialogue (BaseDialog + OpenForm, INTEGER_DATA, (char *) 1);
     }
   else if (IsW3Path (s))
     {
	/* it is a remote document */
	strcpy (LastURLName, s);
	CallbackDialogue (BaseDialog + OpenForm, INTEGER_DATA, (char *) 1);
     }
   else
     {
     if (strncmp(s, "file://localhost", 16) == 0)
	s += 16;
     if (TtaFileExist (s))
        {
	NormalizeFile (s, LastURLName);
	/* check if it is an absolute or a relative name */
#       ifdef _WINDOWS
	if ((LastURLName[0] == DIR_SEP) || (LastURLName[1] == ':'))
#       else  /* !_WINDOWS */
	if (LastURLName[0] == DIR_SEP)
#       endif /* !_WINDOWS */
	   /* it is an absolute name */
	   TtaExtractName (LastURLName, DirectoryName, DocumentName);
	else
	   /* it is a relative name */
	   strcpy (DocumentName, LastURLName);
	/* start with the local document */
	LastURLName[0] = EOS;
	CallbackDialogue (BaseDialog + OpenForm, INTEGER_DATA, (char *) 1);
        }
    else
        /* Create a new document */
        New (0, 1);
    }
}

/*----------------------------------------------------------------------
  ChangeAttrOnRoot
  If the root element of the document does not have an attribute of
  type attrNum, create one.
  If the root has such an attribute, delete it.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         ChangeAttrOnRoot (Document document, int attrNum)
#else
void         ChangeAttrOnRoot (document, view)
Document            document;
int                 attrNum;
#endif
{
   Element	    root;
   AttributeType    attrType;
   Attribute	    attr;
   boolean	    docModified;

   docModified = TtaIsDocumentModified (document);
   root = TtaGetMainRoot (document);
   attrType.AttrSSchema = TtaGetDocumentSSchema (document);
   attrType.AttrTypeNum = attrNum;
   attr = TtaGetAttribute (root, attrType);
   if (attr != NULL)
      /* the root element has a SectionNumbering attribute. Remove it */
      TtaRemoveAttribute (root, attr, document);
   else
      /* the root element has no SectionNumbering attribute. Create one */
      {
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (root, attr, document);
        TtaSetAttributeValue (attr, 1, root, document);
      }
   if (!docModified)
	TtaSetDocumentUnmodified (document);
}

/*----------------------------------------------------------------------
  ShowMapAreas
  Execute the "Show Map Areas" command
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ShowMapAreas (Document document, View view)
#else
void                ShowMapAreas (document, view)
Document            document;
View                view;

#endif
{
   ChangeAttrOnRoot (document, HTML_ATTR_ShowAreas);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HelpAmaya (Document document, View view)
#else
void                HelpAmaya (document, view)
Document            document;
View                view;

#endif
{
#ifdef AMAYA_DEBUG
   Element             el;
   char                localname[MAX_LENGTH];
   FILE               *list;

   /* get the root element */
   el = TtaGetMainRoot (document);
   strcpy (localname, TempFileDirectory);
   strcat (localname, "/tree.debug");
   list = fopen (localname, "w");
   TtaListAbstractTree (el, list);
   fclose (list);
   strcpy (localname, TempFileDirectory);
   strcat (localname, "/view.debug");
   list = fopen (localname, "w");
   TtaListView (document, view, list);
   fclose (list);
   strcpy (localname, TempFileDirectory);
   strcat (localname, "/boxes.debug");
   list = fopen (localname, "w");
   TtaListBoxes (document, view, list);
   fclose (list);
#endif /* AMAYA_DEBUG */
   document = GetHTMLDocument (AMAYA_PAGE, NULL, 0, 0, DC_FALSE);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AmayaClose (Document document, View view)
#else
void                AmayaClose (document, view)
Document            document;
View                view;

#endif
{
   int              i;
   boolean          documentClosed;

   /* invalid current loading */
   W3Loading = 0;

   documentClosed = True;
   /* free each loaded document */
   for (i = 1; i < DocumentTableLength; i++)
      if (DocumentURLs[i] != NULL)
	{
	  TtcCloseDocument (i, 1);
	  documentClosed = (DocumentURLs[i] == NULL);
	  if (!documentClosed)
	    /* the close has been aborted */
	    return;
	}

#ifdef AMAYA_JAVA
   CloseJava();
#endif
   exit(0);
}
