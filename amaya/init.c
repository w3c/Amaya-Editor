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
 *         R. Guetari (W3C/INRIA) - Windows NT/95
 */

/* Included headerfiles */
#define THOT_EXPORT
#include "amaya.h"
#include "css.h"
#include "trans.h"

#ifndef _WINDOWS
#include "stopN.xpm"
#include "stopR.xpm"
#include "save.xpm"
#include "find.xpm"
#include "Back.xpm"
#include "BackNo.xpm"
#include "Forward.xpm"
#include "ForwardNo.xpm"
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
#endif /* !_WINDOWS */

#ifdef _WINDOWS
#include "resource.h"
#endif /* _WINDOWS */

#ifdef AMAYA_PLUGIN
#include "plugin.h"
#include "Plugin.xpm"
#endif /* AMAYA_PLUGIN */

#ifdef AMAYA_JAVA
#include "Java.xpm"
#endif /* AMAYA_JAVA */

#ifdef _WINDOWS
/*
#ifndef __GNUC__
#include <direct.h>
#endif 
*/
int  Window_Curs;

char docToOpen [256];
extern boolean viewClosed;
/* extern bmpID;  */
#define AMAYA_PAGE "\\amaya\\AmayaPage.html"
#else  /* _WINDOWS */
#define AMAYA_PAGE "/amaya/AmayaPage.html"
#endif /* _WINDOWS */

#define AMAYA_PAGE_DOC "http://www.w3.org/Amaya/User/"

static int          AmayaInitialized = 0;
static char        *Manual[] = {
"Browsing.html",
"Selecting.html",
"Searching.html",
"Views.html",
"Creating.html",
"Links.html",
"Changing.html",
"Tables.html",
"Math.html",
"ImageMaps.html",
"StyleSheets.html",
"Attributes.html",
"Publishing.html",
"Printing.html",
"MakeBook.html",
"Configure.html"
};

#ifndef _WINDOWS
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
static Pixmap       iconBackNo;
static Pixmap       iconForward;
static Pixmap       iconForwardNo;
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
#endif /* _WINDOWS */

#ifdef _WINDOWS
#define stopR          0
#define iconBack       1
#define iconForward    2
#define inconReload    3
#define inconSave      4
#define iconPrint      5
#define iconFind       6
#define inconI         7
#define inconB         8
#define iconT          9
#define iconCSS       10
#define inconImage    11
#define iconH1        12
#define iconH2        13
#define iconH3        14
#define iconBullet    15
#define iconNum       16
#define	iconDL        17
#define iconLink      18
#define iconTable     19
#ifdef AMAYA_PLUGIN
#define iconPlugin    20
#endif AMAYA_PLUGIN
#define stopN         22
#define iconBackNo    23
#define iconForwardNo 24

static BOOL itemChecked = FALSE;
extern int currentFrame;
extern int menu_item;
extern char LostPicturePath [512];
#endif /* _WINDOWS */

#include "css_f.h"
#include "HTMLhistory_f.h"
#include "html2thot_f.h"
#include "init_f.h"
#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
#include "query_f.h"
#endif /* !AMAYA_JAVA  && !AMAYA_ILU */
#include "trans_f.h"
#include "AHTURLTools_f.h"
#include "EDITORactions_f.h"
#include "EDITimage_f.h"
#include "EDITstyle_f.h"
#ifdef MATHML
#include "Mathedit_f.h"
#endif /* MATHML */
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

/* the structure used for the GETHTMLDocument_callback function */
typedef struct _GETHTMLDocument_context {
  Document doc;
  Document baseDoc;
  boolean ok;
  boolean history;
  boolean local_link;
  char *target;
  char *documentname;
  char *tempdocument;
  TTcbf *cbf;
  void *ctx_cbf;
} GETHTMLDocument_context;


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
   char               *otherURL;
   char                URL_DIR_STR[2];

   if (documentURL && strchr (documentURL, '/'))
	  sprintf (URL_DIR_STR, "/");
   else 
	   sprintf (URL_DIR_STR, DIR_STR);

   if (!documentURL)
      return ((Document) None);

   i = 1;
   found = FALSE;
   otherURL = TtaGetMemory (MAX_LENGTH);
   strcpy (otherURL, documentURL);
   strcat (otherURL, URL_DIR_STR);
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

   TtaFreeMemory (otherURL);
   if (i < DocumentTableLength)
      /* document is found */
      return ((Document) i);
   else
      /* document is not found */
      return ((Document) None);
}

/*----------------------------------------------------------------------
  CanReplaceCurrentDocument
  Return TRUE if the document has not been modified of if the user
  agrees to loose the changes he/she has made.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean		CanReplaceCurrentDocument (Document document, View view)
#else
boolean		CanReplaceCurrentDocument (document, view)
Document	document;
View		view;
#endif
{
   boolean	ret;

   ret = TRUE;
   if (TtaIsDocumentModified (document))
     {
	InitConfirm (document, view, TtaGetMessage (AMAYA, AM_DOC_MODIFIED));
	if (UserAnswer)
	   TtaSetDocumentUnmodified (document);
	else
	   ret = FALSE;
     }
   return ret;
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
     /* bad parameters */
     return;

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
     /* bad target */
     return;

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
     /* bad suffix */
     return;

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
  SetArrowButton
  Change the appearance of the Back (if back == TRUE) or Forward button
  for a given document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetArrowButton (Document document, boolean back, boolean on)
#else
void                SetArrowButton (document, back, on)
Document            document;
boolean		    back;
boolean		    on;
#endif
{
  int		index;
#ifdef _WINDOWS
  BYTE state;
  int  picture;
#else /* !_WINDOWS */
  Pixmap	picture;
#endif /* _WINDOWS */

  if (back)
    {
      index = 2;
#ifdef _WINDOWS 
      if (on)
	{
	  state   = TRUE;
	  picture = iconBack; 
	}
      else
	{
          state = FALSE;
	  picture = iconBackNo;
	}
#else  /* !_WINDOWS */
      if (on)
	picture = iconBack;
      else
	picture = iconBackNo;
#endif /* _WINDOWS */
    }
  else
    {
      index = 3;
#ifdef _WINDOWS
      if (on)
	{
	  state = TRUE;
	  picture = iconForward;
        }
      else
	{
	  state = FALSE;
	  picture = iconForwardNo;
	}
#else /* !_WINDOWS */
      if (on)
	picture = iconForward;
      else
	picture = iconForwardNo;
#endif /* _WINDOWS */
    }
#ifdef _WINDOWS
  WIN_TtaChangeButton (document, 1, index, picture, state);
#else  /* !_WINDOWS */
  TtaChangeButton (document, 1, index, picture);
#endif /* _WINDOWS */
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
    FilesLoading[document]--;
#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
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
#ifdef _WINDOWS
	  WIN_TtaChangeButton (document, 1, 1, stopN, FALSE);
#else  /* !_WINDOWS */
	  TtaChangeButton (document, 1, 1, stopN);
#endif /* _WINDOWS */
	}
      DocNetworkStatus[document] = AMAYA_NET_INACTIVE;
    }
#endif
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
#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
  DocNetworkStatus[document] = AMAYA_NET_ACTIVE;
#endif
  FilesLoading[document] = 1;
#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
  if (TtaGetViewFrame (document, 1) != 0)
    /* this document is displayed */
#ifdef _WINDOWS 
    WIN_TtaChangeButton (document, 1, 1 , stopR, TRUE);
#else  /* _WINDOWS */
    TtaChangeButton (document, 1, 1, stopR);
#endif /* _WINDOWS */
#endif
}

/*----------------------------------------------------------------------
   SetStopButton Activates the stop button if it's turned off
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetStopButton (Document document)
#else
void                SetStopButton (document)
Document            doc;
#endif
{
#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
  if (DocNetworkStatus[document] != AMAYA_NET_ACTIVE)
    DocNetworkStatus[document] = AMAYA_NET_ACTIVE;
#endif
  if (FilesLoading[document] == 0)
    FilesLoading[document] = 1;
#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
  if (TtaGetViewFrame (document, 1) != 0)
    /* this document is displayed */
#ifdef _WINDOWS 
    WIN_TtaChangeButton (document, 1, 1 , stopR, TRUE);
#else  /* _WINDOWS */
    TtaChangeButton (document, 1, 1, stopR);
#endif /* _WINDOWS */
#endif
}
/*----------------------------------------------------------------------
   ActiveMakeBook initialize the current transfer                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ActiveMakeBook (Document document)
#else
void                ActiveMakeBook (document)
Document            doc;
#endif
{
  DocBook = document;
  IncludedDocument = 0;
  if (TtaGetViewFrame (document, 1) != 0)
    /* this document is displayed */
#ifdef _WINDOWS 
    WIN_TtaChangeButton (document, 1, 1 , stopR, TRUE);
#else  /* _WINDOWS */
    TtaChangeButton (document, 1, 1, stopR);
#endif /* _WINDOWS */
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
  if (document == 0)
    return;
  else if (document == DocBook)
    {
      /* Make Book function stopped */
      DocBook = 0;
      /* stop transfer of the sub-document */
      StopRequest (document);
      StopRequest (IncludedDocument);
      FilesLoading[document] = 0;
      DocNetworkStatus[document] = AMAYA_NET_INACTIVE;
    }
#if defined(AMAYA_JAVA)
  else if (FilesLoading[document] != 0)
    StopRequest (document);
#else 
#if defined(AMAYA_ILU)
  else if (FilesLoading[document] != 0)
    {
      StopRequest (document);
      FilesLoading[document] = 0;
    }
#else
  else if (DocNetworkStatus[document] & AMAYA_NET_ACTIVE)
    {
      if (TtaGetViewFrame (document, 1) != 0)
#ifndef _WINDOWS
	TtaChangeButton (document, 1, 1, stopN);
#else
	WIN_TtaChangeButton (document, 1, 1, stopN, FALSE);
#endif /* !_WINDOWS */
      StopRequest (document);
      FilesLoading[document] = 0;
      DocNetworkStatus[document] = AMAYA_NET_INACTIVE;
    }
#endif /* !AMAYA_ILU */
#endif /* !AMAYA_JAVA */
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
   TextURL                                                      
   The Address text field in a document window has been modified by the user
   Load the corresponding document in that window.
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
  char             *s;
  boolean           change;

  change = FALSE;
  if (text)
    {
      if (!IsW3Path (text))
	{
	  s = TtaGetMemory (MAX_LENGTH);
	  change = NormalizeFile (text, s);
	  if (!TtaFileExist (s))
	    {
	      /* It is not a valid URL */
	      /*TtaSetTextZone (document, view, 1, DocumentURLs[document]);*/
	      TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), text);
	      TtaFreeMemory (s);
	      return;
	    }
	}

      if (!CanReplaceCurrentDocument (document, view))
	{
	  /* restore the previous value */
	  TtaSetTextZone (document, view, 1, DocumentURLs[document]);
	  /* abort the command */
	  return;
	}

      /* do the same thing as a callback form open document form */
      if (change)
	{
	  /* change the text value */
	  TtaSetTextZone (document, view, 1, s);
	  CallbackDialogue (BaseDialog + URLName, STRING_DATA, s);
	  TtaFreeMemory (s);
	}
      else
	CallbackDialogue (BaseDialog + URLName, STRING_DATA, text);
      InNewWindow = FALSE;
      CurrentDocument = document;
      CallbackDialogue (BaseDialog + OpenForm, INTEGER_DATA, (char *) 1);
    }
}

/*----------------------------------------------------------------------
   TextTitle
   The Tile text field in a document window has been modified by the user
   Update the TITLE element for the corresponding document.
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
   Element             el, child;

   /* search the Title element */
   el = TtaGetMainRoot (document);
   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_TITLE;
   el = TtaSearchTypedElement (elType, SearchForward, el);
   child = TtaGetFirstChild (el);
   if (child == NULL)
     {
       /* insert the text element */
       elType.ElTypeNum = HTML_EL_TEXT_UNIT;
       child = TtaNewElement (document, elType);
       TtaInsertFirstChild  (&child, el, document);
     }
   TtaSetTextContent (child, text, TtaGetDefaultLanguage (), document);
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
#  ifndef _WINDOWS
   TtaNewForm (BaseDialog + FormAnswer, TtaGetViewFrame (document, view), 
      TtaGetMessage (AMAYA, AM_GET_AUTHENTICATION), TRUE, 1, 'L', D_CANCEL);
   TtaNewTextForm (BaseDialog + NameText, BaseDialog + FormAnswer,
		   TtaGetMessage (AMAYA, AM_NAME), NAME_LENGTH, 1, FALSE);
   TtaNewTextForm (BaseDialog + PasswordText, BaseDialog + FormAnswer,
		   TtaGetMessage (AMAYA, AM_PASSWORD), NAME_LENGTH, 1, TRUE);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + FormAnswer, FALSE);
   TtaWaitShowDialogue ();
#  else /* _WINDOWS */
   CreateAuthenticationDlgWindow (TtaGetViewFrame (document, view));
#  endif /* _WINDOWS */
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
#  ifndef _WINDOWS
   /* Confirm form */
   TtaNewForm (BaseDialog + ConfirmForm, TtaGetViewFrame (document, view),  TtaGetMessage (LIB, TMSG_LIB_CONFIRM), TRUE, 2, 'L', D_CANCEL);
   TtaNewLabel (BaseDialog + ConfirmText, BaseDialog + ConfirmForm, label);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + ConfirmForm, FALSE);
   /* wait for an answer */
   TtaWaitShowDialogue ();
#  else  /* _WINDOWS */
   CreateInitConfirmDlgWindow (TtaGetViewFrame (document, view), BaseDialog + ConfirmForm, TtaGetMessage (LIB, TMSG_LIB_CONFIRM), label);
#  endif /* _WINDOWS */
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
#  ifndef _WINDOWS
   /* For Windoz have a look to OPENFILENAME structure and
      GetOpenFileName function */
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
#  else /* _WINDOWS */
   CreateOpenDocDlgWindow (TtaGetViewFrame (document, view), docToOpen, BaseDialog, OpenForm)	;
   if (InNewWindow)
      GetHTMLDocument (docToOpen, NULL, 0, 0, CE_FALSE, NULL, 0, FALSE);
   else 
      GetHTMLDocument (docToOpen, NULL, document, document, CE_FALSE, NULL, 0, TRUE);
#  endif /* _WINDOWS */
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
   if (CanReplaceCurrentDocument (document, view))
     {
       /* load the new document */
       InNewWindow = FALSE;
       InitOpenDocForm (document, view);
     }
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
   char               *tempname;
   char               *temppath;
   View                mainView, structView, altView, linksView, tocView;
   Document            old_doc;
   boolean             opened;
   int                 x, y, w, h;
#  ifdef _WINDOWS
   int                 indexButton = 0;
#  endif /* _WINDOWS */

#  ifdef _WINDOWS
   Window_Curs = IDC_WINCURSOR;
   /* bmpID = IDB_AMAYALOGO; */
#  endif /* _WINDOWS */

   old_doc = doc;		/* previous document */
   if (doc != 0 && !TtaIsDocumentModified (doc))
     {
        /* close the Alternate view if it is open */
	altView = TtaGetViewFromName (doc, "Alternate_view");
	if (altView != 0)
	   if (TtaIsViewOpened (doc, altView))
	     {
		TtaCloseView (doc, altView);
	     }
        /* close the Structure view if it is open */
	structView = TtaGetViewFromName (doc, "Structure_view");
	if (structView != 0)
	   if (TtaIsViewOpened (doc, structView))
	     {
		TtaCloseView (doc, structView);
	     }
#ifdef MATHML
	structView = TtaGetViewFromName (doc, "Math_Structure_view");
	if (structView != 0)
	   if (TtaIsViewOpened (doc, structView))
	      TtaCloseView (doc, structView);
#endif /* MATHML */
        /* close the Links view if it is open */
	linksView = TtaGetViewFromName (doc, "Links_view");
	if (linksView != 0)
	   if (TtaIsViewOpened (doc, linksView))
	     {
		TtaCloseView (doc, linksView);
	     }
        /* close the Table_of_contents view if it is open */
	tocView = TtaGetViewFromName (doc, "Table_of_contents");
	if (tocView != 0)
	   if (TtaIsViewOpened (doc, tocView))
	     {
		TtaCloseView (doc, tocView);
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
   temppath = TtaGetMemory (MAX_LENGTH);
   tempname = TtaGetMemory (MAX_LENGTH);
   TtaExtractName (pathname, temppath, tempname);
   if (tempname[0] == EOS)
      /* there is a slash at the end of the path */
      strcpy (tempname, "noname.html");

   doc = TtaNewDocument ("HTML", tempname);
   TtaFreeMemory (tempname);
   if (doc >= DocumentTableLength)
     {
       TtaCloseDocument (doc);
       doc = 0;
     }
   else if (doc > 0)
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
	     TtaFreeMemory (temppath);
	     return (0);
	  }
	if (!opened)
	  {
	     HelpDocuments[doc] = FALSE;
	     /* Add a button */
#        ifndef _WINDOWS
	     TtaAddButton (doc, 1, stopN, StopTransfer,
			   TtaGetMessage (AMAYA, AM_BUTTON_INTERRUPT));
	     TtaAddButton (doc, 1, iconBackNo, GotoPreviousHTML,
			   TtaGetMessage (AMAYA, AM_BUTTON_PREVIOUS));
	     TtaAddButton (doc, 1, iconForwardNo, GotoNextHTML,
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

#        else /* _WINDOWS */

		 WIN_TtaAddButton (doc, 1, stopR, StopTransfer, TtaGetMessage (AMAYA, AM_BUTTON_INTERRUPT), TBSTYLE_BUTTON, FALSE);
	     WIN_TtaAddButton (doc, 1, iconBack, GotoPreviousHTML, TtaGetMessage (AMAYA, AM_BUTTON_PREVIOUS), TBSTYLE_BUTTON, FALSE);
	     WIN_TtaAddButton (doc, 1, iconForward, GotoNextHTML, TtaGetMessage (AMAYA, AM_BUTTON_NEXT), TBSTYLE_BUTTON, FALSE);

	     WIN_TtaAddButton (doc, 1, inconReload, Reload, TtaGetMessage (AMAYA, AM_BUTTON_RELOAD), TBSTYLE_BUTTON, TBSTATE_ENABLED);
	     WIN_TtaAddButton (doc, 1, 0, NULL, NULL, TBSTYLE_SEP, TBSTATE_ENABLED); /* SEPARATOR */

	     WIN_TtaAddButton (doc, 1, inconSave, SaveDocument, TtaGetMessage (AMAYA, AM_BUTTON_SAVE), TBSTYLE_BUTTON, TBSTATE_ENABLED);
	     WIN_TtaAddButton (doc, 1, iconPrint, TtcPrint, TtaGetMessage (AMAYA, AM_BUTTON_PRINT), TBSTYLE_BUTTON, TBSTATE_ENABLED);
	     WIN_TtaAddButton (doc, 1, iconFind, TtcSearchText,	TtaGetMessage (AMAYA, AM_BUTTON_SEARCH), TBSTYLE_BUTTON, TBSTATE_ENABLED);
	     WIN_TtaAddButton (doc, 1, 0, NULL, NULL, TBSTYLE_SEP, TBSTATE_ENABLED);  /* SEPARATOR */

	     IButton =  WIN_TtaAddButton (doc, 1, inconI, SetCharEmphasis, TtaGetMessage (AMAYA, AM_BUTTON_ITALICS), TBSTYLE_CHECK, TBSTATE_ENABLED);
	     BButton =  WIN_TtaAddButton (doc, 1, inconB, SetCharStrong, TtaGetMessage (AMAYA, AM_BUTTON_BOLD), TBSTYLE_CHECK, TBSTATE_ENABLED);
	     TTButton = WIN_TtaAddButton (doc, 1, iconT, SetCharCode, TtaGetMessage (AMAYA, AM_BUTTON_CODE), TBSTYLE_CHECK, TBSTATE_ENABLED);
	     WIN_TtaAddButton (doc, 1, iconCSS, InitCSSDialog, TtaGetMessage (AMAYA, AM_BUTTON_CSS), TBSTYLE_BUTTON, TBSTATE_ENABLED);
	     WIN_TtaAddButton (doc, 1, 0, NULL, NULL, TBSTYLE_SEP, TBSTATE_ENABLED);  /* SEPARATOR */

	     WIN_TtaAddButton (doc, 1, inconImage, CreateImage, TtaGetMessage (AMAYA, AM_BUTTON_IMG), TBSTYLE_BUTTON, TBSTATE_ENABLED);
	     WIN_TtaAddButton (doc, 1, iconH1, CreateHeading1, TtaGetMessage (AMAYA, AM_BUTTON_H1), TBSTYLE_BUTTON, TBSTATE_ENABLED);
	     WIN_TtaAddButton (doc, 1, iconH2, CreateHeading2, TtaGetMessage (AMAYA, AM_BUTTON_H2), TBSTYLE_BUTTON, TBSTATE_ENABLED);
	     WIN_TtaAddButton (doc, 1, iconH3, CreateHeading3, TtaGetMessage (AMAYA, AM_BUTTON_H3), TBSTYLE_BUTTON, TBSTATE_ENABLED);
	     WIN_TtaAddButton (doc, 1, iconBullet, CreateList, TtaGetMessage (AMAYA, AM_BUTTON_UL), TBSTYLE_BUTTON, TBSTATE_ENABLED);
	     WIN_TtaAddButton (doc, 1, iconNum, CreateNumberedList, TtaGetMessage (AMAYA, AM_BUTTON_OL), TBSTYLE_BUTTON, TBSTATE_ENABLED);
	     WIN_TtaAddButton (doc, 1, iconDL, CreateDefinitionList, TtaGetMessage (AMAYA, AM_BUTTON_DL), TBSTYLE_BUTTON, TBSTATE_ENABLED);
	     WIN_TtaAddButton (doc, 1, iconLink, CreateOrChangeLink, TtaGetMessage (AMAYA, AM_BUTTON_LINK), TBSTYLE_BUTTON, TBSTATE_ENABLED);
	     WIN_TtaAddButton (doc, 1, iconTable, CreateTable, TtaGetMessage (AMAYA, AM_BUTTON_TABLE), TBSTYLE_BUTTON, TBSTATE_ENABLED);
#        endif /* _WINDOWS */

#        ifdef AMAYA_PLUGIN 
	     TtaAddButton (doc, 1, iconPlugin, TtaCreateFormPlugin,
			   TtaGetMessage (AMAYA, AM_BUTTON_PLUGIN));
#        endif /* AMAYA_PLUGIN */

#ifdef AMAYA_JAVA
	     TtaAddButton (doc, 1, iconJava, CreateFormJava,
			   TtaGetMessage (AMAYA, AM_BUTTON_JAVA));
#endif /* AMAYA_JAVA */
#ifdef MATHML
	     AddMathButton (doc, 1);
#endif /* MATHML */
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
   TtaFreeMemory (temppath);
   return (doc);
}

/*----------------------------------------------------------------------
   LoadHTMLDocument parse of the new document and         
   store its path (or URL) into the document table.       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Document     LoadHTMLDocument (Document doc, char *pathname, char *tempfile, char *documentname, char *content_type, boolean history)
#else
static Document     LoadHTMLDocument (doc, pathname, tempfile, documentname, content_type, history)
Document            doc;
char               *pathname;
char               *tempfile;
char               *documentname;
char               *content_type;
boolean		    history;
#endif
{
  Document            newdoc = 0;
  char               *tempdocument;
  char               *tempdir;
  char               *s;
  int                 i, j;
  boolean	       PlainText;
  boolean	       HTMLfile;

  PlainText = FALSE;
  HTMLfile = FALSE;
  tempdir = tempdocument = NULL;
  if (content_type == NULL || content_type[0] == EOS)
    /* no content type */
    {
      if (IsHTMLName (pathname))
	/* local document */
	/* try to guess the document type after its file name extension */
	HTMLfile = TRUE;
      else if (!IsImageName (pathname))
	PlainText = TRUE;
      else if (tempfile[0] != EOS)
	/* It's a document loaded from the Web */
	/* Let's suppose it's HTML */
	HTMLfile = TRUE;
      }
   else
     /* the server returned a content type */
     {
       i = 0;
       while (content_type[i] != '/' && content_type[i] != EOS)
	 i++;
       if (content_type[i] == '/')
	 {
	   content_type[i] = EOS;
	   j = i+1;
	   while (content_type[j] != ';' && content_type[j] != EOS)
	     j++;
	   if (content_type[j] == ';')
	     content_type[j] = EOS;
	   if (strcasecmp (content_type, "text") == 0)
	     {
	       if (strncasecmp (&content_type[i+1], "html", 4) == 0)
		 HTMLfile = TRUE;
	       else
		 PlainText = TRUE;
	     }
	 }
     }
  if (!PlainText && !HTMLfile && tempfile[0] != EOS)
    {
      /* The document is not an HTML file and cannot be parsed */
      /* rename the temporary file */
      strcpy (SavingFile, tempfile);
      SavingDocument = 0;
      SavingObject = 0;
      tempdocument = TtaGetMemory (MAX_LENGTH);
      TtaExtractName (pathname, tempfile, tempdocument);
      /* reinitialize directories and document lists */
      strcpy (pathname, DirectoryName);
      strcat (pathname, DIR_STR);
      strcat (pathname, tempdocument);
      strcpy (SavePath, DirectoryName);
      strcpy (SaveName, tempdocument);
      ResetStop (doc);
      InitSaveObjectForm (doc, 1, SavingFile, pathname);
    }
  else if (pathname[0] != EOS)
    {
      if (DocumentURLs[doc])
	{
	  /* save the URL of the old document in the history, if the same
	     window is reused, i.e. if the old document has not been
	     modified */
	  if (history)
	    if (!TtaIsDocumentModified (doc))
	      AddDocHistory (doc, DocumentURLs[doc]);
	  /* free the previous document */
	  newdoc = InitDocView (doc, pathname);
	}
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
	  /* allocate and initialize tempdocument */
	  tempdocument = GetLocalPath (newdoc, pathname);
	  TtaFileUnlink (tempdocument);
	  if (doc != newdoc)
	    {
	      /* now we can rename the local name of a remote document */
	      TtaFileCopy (tempfile, tempdocument);
	      TtaFileUnlink (tempfile);
	    }
	  else
	    /* now we can rename the local name of a remote document */
	    rename (tempfile, tempdocument);
	}
      else
	{
	  /* It is a local document */
	  tempdocument = TtaGetMemory (MAX_LENGTH);
	  strcpy (tempdocument, pathname);
	}
      
      /* save the document name into the document table */
      i = strlen (pathname) + 1;
      s = TtaGetMemory (i);
      strcpy (s, pathname);
      if (DocumentURLs[newdoc] != NULL)
	TtaFreeMemory (DocumentURLs[(int) newdoc]);
      DocumentURLs[newdoc] = s;
      if (TtaGetViewFrame (newdoc, 1) != 0)
	/* this document is displayed */
	TtaSetTextZone (newdoc, 1, 1, s);
      
      tempdir = TtaGetMemory (MAX_LENGTH);
      TtaExtractName (tempdocument, tempdir, documentname);
      StartHTMLParser (newdoc, tempdocument, documentname, tempdir, pathname,
		       PlainText);
      TtaFreeMemory (tempdir);
      if (newdoc != doc)
	/* the document is displayed in a different window */
	/* reset the history of the new window */
	InitDocHistory (newdoc);
    }
  TtaFreeMemory (tempdocument);
  return (newdoc);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                Reload_callback (int doc, int status, char *urlName,
                                     char *outputfile, char *content_type,
				       void * context)
#else  /* __STDC__ */
void                Reload_callback (doc, status, urlName, outputfile, 
				       content_type, context)
int doc;
int status;
char *urlName;
char *outputfile;
char *content_type;
void *context;

#endif
{
  Document newdoc;
  char *pathname;
  char *tempfile;
  char *documentname;
  Document res;


  documentname = (char *) context;

  newdoc = doc;
  tempfile = outputfile;

  pathname = TtaGetMemory (MAX_LENGTH);
  strcpy (pathname, urlName);

  if (status == 0)
     {
       TtaSetCursorWatch (0, 0);
       /* do we need to control the last slash here? */
       res = LoadHTMLDocument (newdoc, pathname, tempfile, documentname,
			       content_type, FALSE);
	W3Loading = 0;		/* loading is complete now */
	TtaHandlePendingEvents ();
	/* fetch and display all images referred by the document */
	if (res != 0)
	  FetchAndDisplayImages (res, AMAYA_NOCACHE);
	TtaResetCursor (0, 0);
     }
  ResetStop(newdoc);
  TtaFreeMemory (pathname);
  TtaFreeMemory (documentname);
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
   Document            newdoc;
   char               *tempfile;
   char               *pathname;
   char               *documentname;
   char                content_type[NAME_LENGTH];
   int                 toparse;

   if (DocumentURLs[(int) document] == NULL)
      /* the document has not been loaded yet */
      return;

   content_type[0] = '\0';

   /* abort all current exchanges concerning this document */
   StopTransfer (document, 1);

   if (!CanReplaceCurrentDocument (document, view))
      /* abort the command */
      return;

   /* reload the document */
   pathname = TtaGetMemory (MAX_LENGTH);
   documentname = TtaGetMemory (MAX_LENGTH);
   NormalizeURL (DocumentURLs[(int) document], 0, pathname, documentname, NULL);

   if (!IsW3Path (pathname) && !TtaFileExist (pathname)) {
      /* Free Memory ***/
      TtaFreeMemory (pathname);
	  TtaFreeMemory (documentname);
     /* cannot reload this document */
     return;
   }

   W3Loading = document;	/* this document is currently in load */
   newdoc = InitDocView (document, pathname);

#if defined(AMAYA_JAVA) || defined(AMAYA_ILU)
   /* Check against concurrent loading on the same frame */
   if (FilesLoading[newdoc])
     {
       TtaFreeMemory (pathname);
       TtaFreeMemory (documentname);
       return;
     }
#endif 

   tempfile = TtaGetMemory (MAX_LENGTH);
   tempfile[0] = EOS;
   toparse = 0;
   ActiveTransfer (newdoc);
   if (IsW3Path (pathname))
     {
       /* load the document from the Web */
#ifdef AMAYA_JAVA
       toparse = GetObjectWWW (newdoc, pathname, NULL, tempfile, 
			       AMAYA_SYNC | AMAYA_NOCACHE,
			       NULL, NULL, NULL, NULL, YES, &content_type[0]);
#else /* AMAYA_JAVA */
       toparse = GetObjectWWW (newdoc, pathname, NULL, tempfile, 
			       AMAYA_ASYNC,
			       NULL, NULL, (void *) Reload_callback, 
			       (void *) documentname, YES, (char *) content_type);
#endif /* AMAYA_JAVA */
     }
   else if (TtaFileExist (pathname))
     Reload_callback (newdoc, 0, pathname, tempfile, NULL, (void *) documentname);
   TtaFreeMemory (tempfile);
   TtaFreeMemory (pathname);
   
   TtaHandlePendingEvents ();
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
#ifdef MATHML
   View                mathView;
#endif /* MATHML */
   int                 x, y, w, h;

   structView = TtaGetViewFromName (document, "Structure_view");
   if (structView != 0 && TtaIsViewOpened (document, structView))
     TtaRaiseView (document, structView);
   else
     {
       TtaGetViewGeometry (document, "Structure_view", &x, &y, &w, &h);
       structView = TtaOpenView (document, "Structure_view", x, y, w, h);
       TtcSwitchButtonBar (document, structView);
       TtcSwitchCommands (document, structView);
     }
#ifdef MATHML
   mathView = TtaGetViewFromName (document, "Math_Structure_view");
   if (mathView != 0 && TtaIsViewOpened (document, mathView))
     TtaRaiseView (document, mathView);
   else
     {
       TtaGetViewGeometry (document, "Math_Structure_view", &x, &y, &w, &h);
       mathView = TtaOpenView (document, "Math_Structure_view", x, y, w, h);
       if (mathView != 0)
	 {
	   TtcSwitchButtonBar (document, mathView);
	   TtcSwitchCommands (document, mathView);
	 }
     }
#endif /* MATHML */
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
      TtaRaiseView (document, view);
   else if (altView != 0 && TtaIsViewOpened (document, altView))
      TtaRaiseView (document, altView);
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
      TtaRaiseView (document, view);
   else if (linksView != 0 && TtaIsViewOpened (document, linksView))
      TtaRaiseView (document, linksView);
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
      TtaRaiseView (document, view);
   else if (tocView != 0 && TtaIsViewOpened (document, tocView))
      TtaRaiseView (document, tocView);
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
   Document      document;
   View          view, structView, altView, linksView, tocView;
#ifdef MATHML
   View          mathView;
#endif /* MATHML */

   view = event->view;
   document = event->document;
   structView = TtaGetViewFromName (document, "Structure_view");
   altView = TtaGetViewFromName (document, "Alternate_view");
   linksView = TtaGetViewFromName (document, "Links_view");
   tocView = TtaGetViewFromName (document, "Table_of_contents");
#ifdef MATHML
   mathView = TtaGetViewFromName (document, "Math_Structure_view");
#endif /* MATHML */
   if (view != 1)
     /* let Thot perform normal operation */
     return FALSE;
   else
     /* closing main view */
     if (!CanReplaceCurrentDocument (document, view))
	/* abort the command and don't let Thot perform normal operation */
	return TRUE;

   HelpDocuments[document] = FALSE;
   if (structView != 0 && TtaIsViewOpened (document, structView))
     TtaCloseView (document, structView);
   if (altView != 0 && TtaIsViewOpened (document, altView))
     TtaCloseView (document, altView);
   if (linksView != 0 && TtaIsViewOpened (document, linksView))
     TtaCloseView (document, linksView);
   if (tocView != 0 && TtaIsViewOpened (document, tocView))
     TtaCloseView (document, tocView);
#ifdef MATHML
   if (mathView != 0 && TtaIsViewOpened (document, mathView))
     TtaCloseView (document, mathView);
#endif /* MATHML */
   /* let Thot perform normal operation */
   return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void               GetHTMLDocument_callback (int newdoc, int status, 
					     char *urlName,
					     char *outputfile, 
					     char *content_type,
					     void * context)
#else  /* __STDC__ */
void               GetHTMLDocument_callback (newdoc, status, urlName,
                                             outputfile, content_type, 
                                             context)
int newdoc;
int status;
char *urlName;
char *outputfile;
char *content_type;
void *context;

#endif
{
   Element             elFound;
   Document            doc;
   Document            baseDoc;
   Document            res;
   char               *tempfile;
   char               *target;
   char               *pathname;
   char               *documentname;
   char               *tempdocument;
   char               *s;
   int                 i;
   boolean	       history;
   boolean             ok;
   boolean             local_link;
   GETHTMLDocument_context *ctx;
   TTcbf              *cbf;
   void               *ctx_cbf;

   /* restore GETHTMLDocument's context */  
   ctx = (GETHTMLDocument_context *) context;

   if (!ctx)
     return;

   baseDoc = ctx->baseDoc;
   doc = ctx->doc;
   ok = ctx->ok;
   history = ctx->history;
   target = ctx->target;
   documentname = ctx->documentname;
   tempdocument = ctx->tempdocument;
   cbf = ctx->cbf;
   ctx_cbf = ctx->ctx_cbf;
   local_link = ctx->local_link;

   pathname = TtaGetMemory (MAX_LENGTH + 1);
   strncpy (pathname, urlName, MAX_LENGTH);
   pathname[MAX_LENGTH] = EOS;
   tempfile = TtaGetMemory (MAX_LENGTH + 1);
   strncpy (tempfile, outputfile, MAX_LENGTH);
   tempfile[MAX_LENGTH] = EOS;
   
   if (ok && !local_link)
     {
       /* memorize the initial newdoc value in doc because LoadHTMLDocument */
       /* will open a new document if newdoc is a modified document */
       if (status == 0)
	 {
	   if (IsW3Path (pathname) && !strcmp (documentname, "noname.html"))
	     /* keep the real name */
	     NormalizeURL (pathname, 0, tempdocument, documentname, NULL);

	   /* do we need to control the last slash here? */
	   res = LoadHTMLDocument (newdoc, pathname, tempfile, 
				   documentname, content_type, history);
	   W3Loading = 0;		/* loading is complete now */
	   if (res == 0)
	     {
	       /* cannot load the document */
	       ResetStop(newdoc);
	       newdoc = 0;
	       ok = FALSE;
	     }
	   else if (newdoc != res)
	     newdoc = res;
	   
	   if (ok)
	     {
	       /* fetch and display all images referred by the document */
	       if (doc == baseDoc)
		 /* it's not a temporary document */
		 FetchAndDisplayImages (newdoc, 0);
	     }
	 }
       else
	 {
	   if (DocumentURLs[newdoc] == NULL)
	     {
	       /* save the document name into the document table */
	       i = strlen (pathname) + 1;
	       s = TtaGetMemory (i);
	       strcpy (s, pathname);
	       DocumentURLs[newdoc] = s;
	       TtaSetTextZone (newdoc, 1, 1, s);
	     }
	   W3Loading = 0;	/* loading is complete now */
	 }
       
       if (ok)
	 ResetStop(newdoc);
     }

   /* select the target if present */
   if (ok && target[0] != EOS && newdoc != 0)
     {
       /* attribute HREF contains the NAME of a target anchor */
       elFound = SearchNAMEattribute (newdoc, target, NULL);
       if (elFound != NULL)
	 {
	   /* show the target element in all views */
	   for (i = 1; i < 4; i++)
	     if (TtaIsViewOpened (newdoc, i))
	       TtaShowElement (newdoc, i, elFound, 0);
	 }
     }

   /* if there's a callback associated with GetHTMLDocument, call it */
   if (cbf)
     (*cbf) (newdoc, status, pathname, tempfile, NULL, ctx_cbf);

   TtaFreeMemory (target);
   TtaFreeMemory (documentname);
   TtaFreeMemory (pathname);
   TtaFreeMemory (tempfile);
   TtaFreeMemory (tempdocument);
   TtaFreeMemory (ctx);
   if (HelpDocuments[newdoc])
     TtaSetDocumentAccessMode (newdoc, 0);
}

/*----------------------------------------------------------------------
  GetHTMLDocument loads the document if it is not loaded yet and    
  calls the parser if the document can be parsed.
    - documentPath: can be relative or absolute address.
    - form_data: the text to be posted.
    - doc: the document which can be removed if not updated.
    - baseDoc: the document which documentPath is relative to.
    - CE_event: CE_FORM_POST for a post request, CE_TRUE for a double click.
    - history: record the URL in the browsing history
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Document            GetHTMLDocument (const char *documentPath, char *form_data, Document doc, Document baseDoc, ClickEvent CE_event, boolean history, TTcbf *cbf, void *ctx_cbf)
#else
Document            GetHTMLDocument (documentPath, form_data, doc, baseDoc, CE_event, history, void *cbf, void *ctx_cbf)
const char         *documentPath;
char               *form_data;
Document            doc;
Document            baseDoc;
ClickEvent          CE_event;
boolean		    history;
TTcbf              *cbf;
void               *ctx_cbf;

#endif
{
   Document            newdoc;
   char               *tempfile;
   char               *tempdocument;
   char               *parameters;
   char               *target;
   char               *pathname;
   char               *documentname;
   int                 toparse;
   int                 slash;
   int                 mode;
   boolean             ok;
   GETHTMLDocument_context *ctx = NULL;


   /* Extract parameters if necessary */
   if (strlen (documentPath) > MAX_LENGTH - 1) 
     {
       TtaSetStatus (baseDoc, 1, TtaGetMessage (AMAYA, AM_TOO_LONG_URL), "512");
       return (0);
     }
   else
     /* clean up the status line */
     TtaSetStatus (baseDoc, 1, " ", NULL);
 
   ok = TRUE;
   tempdocument = TtaGetMemory (MAX_LENGTH);
   target = TtaGetMemory (MAX_LENGTH);
   documentname = TtaGetMemory (MAX_LENGTH);
   parameters = TtaGetMemory (MAX_LENGTH);
   tempfile = TtaGetMemory (MAX_LENGTH);
   pathname = TtaGetMemory (MAX_LENGTH);

   strcpy (tempdocument, documentPath);
   ExtractParameters (tempdocument, parameters);
   /* Extract the target if necessary */
   ExtractTarget (tempdocument, target);
   /* Add the  base content if necessary */
   if (CE_event == CE_TRUE || CE_event == CE_FORM_GET
       || CE_event == CE_FORM_POST || CE_event == CE_MAKEBOOK)
     NormalizeURL (tempdocument, baseDoc, pathname, documentname, NULL);
   else
     NormalizeURL (tempdocument, 0, pathname, documentname, NULL);

   if (parameters[0] == EOS)
     newdoc = IsDocumentLoaded (pathname);
   else
     {
       /* we need to ask the server */
       newdoc = 0;
       strcat (pathname, "?");
       strcat (pathname, parameters);
     }
   
   if ((CE_event == CE_FORM_POST) || (CE_event == CE_FORM_GET))
     /* special checks for forms */
     {
       /* we always have a fresh newdoc for forms */
       newdoc = 0;
       if (!IsW3Path (pathname))
	 {
	   /* the target document doesn't exist */
	   TtaSetStatus (baseDoc, 1, 
			 TtaGetMessage (AMAYA, AM_CANNOT_LOAD), pathname);
	   ok = FALSE; /* do not continue */
	 }
     }
   
   if (ok && newdoc != 0 && history)
     /* it's just a move in the same document */
     /* record the current position in the history */
     AddDocHistory (newdoc, DocumentURLs[newdoc]);

   if (ok)
     {
       /* Create the context for the callback */
       ctx = TtaGetMemory (sizeof (GETHTMLDocument_context));
       ctx->doc = doc;
       ctx->baseDoc = baseDoc;
       ctx->ok = ok;
       ctx->history = history;
       ctx->target = target;
       ctx->documentname = documentname;
       ctx->tempdocument = tempdocument;
       ctx->cbf = cbf;
       ctx->ctx_cbf = ctx_cbf;
       ctx->local_link = 0;
     }

   if (ok && newdoc == 0)
     {
       /* document not loaded yet */
       if ((CE_event == CE_TRUE || CE_event == CE_FORM_GET
	    || CE_event == CE_FORM_POST || CE_event == CE_MAKEBOOK)
	   && !IsW3Path (pathname) 
	   && !TtaFileExist (pathname))
	 {
	   /* the target document doesn't exist */
	   TtaSetStatus (baseDoc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD),
			 pathname);
	   ok = FALSE; /* do not continue */
	 }
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
		 ok = FALSE;
	       else if (CE_event == CE_HELP)
		 {
		   /* help document has to be in read-only mode */
		   TtcSwitchCommands (newdoc, 1);
		   HelpDocuments[newdoc] = TRUE;
		   TtaSetMenuOff (newdoc, 1, Edit_);
		   TtaSetMenuOff (newdoc, 1, Types);
		   TtaSetMenuOff (newdoc, 1, Links);
		   TtaSetMenuOff (newdoc, 1, Style);
		   TtaSetMenuOff (newdoc, 1, Special);
		   TtaSetMenuOff (newdoc, 1, Attributes_);
		   TtaSetMenuOff (newdoc, 1, Help_);
		   TtaSetItemOff (newdoc, 1, File, BNew);

		   TtaSetItemOff (newdoc, 1, File, BOpenDoc);
		   TtaSetItemOff (newdoc, 1, File, BOpenInNewWindow);
		   TtaSetItemOff (newdoc, 1, File, BReload);
		   TtaSetItemOff (newdoc, 1, File, BSave);
		 }
	     }
	   else
	     {
	       /* stop current transfer for previous document */
	       if (CE_event != CE_MAKEBOOK)
		 StopTransfer (baseDoc, 1);
	       newdoc = doc;
	     }

#if defined(AMAYA_JAVA) || defined(AMAYA_ILU)
	   if (ok)
	     {
	       /* Check against concurrent loading on the same frame */
	       if (FilesLoading[newdoc])
		 {
		   newdoc = 0;
		   ok = FALSE;
		 }
	     }
#endif /* AMAYA_JAVA */
	   
	   if (ok)
	     {
	       /* this document is currently in load */
	       W3Loading = newdoc;
	       ActiveTransfer (newdoc);
	       /* set up the transfer mode */
	       if (CE_event == CE_FORM_POST)
		 mode = AMAYA_SYNC | AMAYA_FORM_POST;
	       else if (CE_event == CE_MAKEBOOK)
		 mode = AMAYA_ASYNC;
	       else
#if defined(AMAYA_JAVA) || defined(AMAYA_ILU)
		 mode = AMAYA_ASYNC;
#else
	         mode = AMAYA_ASYNC;
#endif /* AMAYA_JAVA */
	       if (IsW3Path (pathname))
		 {
		   if (CE_event == CE_FORM_POST)
		     toparse =  GetObjectWWW (newdoc,
					     pathname,
					      form_data, 
					     tempfile,
					      mode,
					      NULL,
					      NULL, 
					      (void *) GetHTMLDocument_callback,
					      (void *) ctx,
					      YES,
					      NULL);
		   else
		     {
		       if (!strcmp (documentname, "noname.html"))
			 {
			   slash = strlen (pathname);
			   if (slash && pathname[slash - 1] != '/')
			     strcat (pathname, "/");
			   
			   toparse = GetObjectWWW (newdoc,
						   pathname,
						   NULL, 
						   tempfile,
						   mode,
						   NULL,
						   NULL,
						   (void *) GetHTMLDocument_callback, 
						   (void *) ctx,
						   YES,
						   NULL);
			   
			 }
		       else 
			 toparse = GetObjectWWW (newdoc, 
						 pathname,
						 NULL, 
						 tempfile, 
						 mode,
						 NULL, 
						 NULL,
						 (void *) GetHTMLDocument_callback, 
						 (void *) ctx,
						 YES,
						 NULL);
		     }
		 }
	       else {
		 /* wasn't a document off the web, we need to open it */
		 TtaSetStatus (newdoc, 1, TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED), NULL);
		 GetHTMLDocument_callback (newdoc, 0,
					   pathname,
					   tempfile, 
					   NULL,
					   (void *) ctx);
		 TtaHandlePendingEvents ();
	       }
	     }
	 }
     }
   else if (ok && newdoc != 0)
     /* following a local link */
     {
       TtaSetStatus (newdoc, 1, TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED), NULL);
       ctx->local_link = 1;
       GetHTMLDocument_callback (newdoc, 0,
				 pathname,
				 tempfile, 
				 NULL,
				 (void *) ctx);
       TtaHandlePendingEvents ();
     }
   else if (ok == FALSE)
     /* if the document isn't loaded off the web (because of an error, or
	because it was already loaded), we invoke the callback function */
     {
       if (cbf)
	 (*cbf) (newdoc, -1, pathname, tempfile, NULL, ctx_cbf);
     }

   /* free the allocated memory */
   if (ok == FALSE)
     {
       /* Free the memory associated with the context */
       TtaFreeMemory (target);
       TtaFreeMemory (documentname);
       TtaFreeMemory (tempdocument);
       if (ctx)
	 TtaFreeMemory (ctx);
     }

   TtaFreeMemory (parameters);
   TtaFreeMemory (tempfile);
   TtaFreeMemory (pathname);
   if (HelpDocuments[newdoc])
     TtaSetDocumentAccessMode (newdoc, 0);
   return (newdoc);

   TtaHandlePendingEvents ();
}


/*----------------------------------------------------------------------
   UpdateSaveAsButtons
   Maintain consistency between buttons in the Save As dialog box
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void	UpdateSaveAsButtons ()
#else
static void	UpdateSaveAsButtons ()

#endif
{
  int	active;

  if (SaveAsHTML)
     active = 1;
  else
     active = 0;
  TtaRedrawMenuEntry (BaseDialog + ToggleSave, 3, NULL, -1, active);
  TtaRedrawMenuEntry (BaseDialog + ToggleSave, 4, NULL, -1, active);

}

/*----------------------------------------------------------------------
   SetFileSuffix
   Set the suffix of the file name used for saving a document
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void	SetFileSuffix ()
#else
static void	SetFileSuffix ()

#endif
{
  char		      suffix[6];
  char               *filename;
  int		      i, len;

  if (SavingDocument != 0 && SaveName[0] != EOS)
    {
      if (SaveAsHTML)
#ifdef _WINDOWS
	strcpy (suffix, "htm");
#else /* _WINDOWS */
	strcpy (suffix, "html");
#endif /* _WINDOWS */
       else if (SaveAsText)
	strcpy (suffix, "txt");
      else
	 return;

      /* looks for a suffix at the end of the document name */
      len = strlen (SaveName);
      for (i = len-1; i > 0 && SaveName[i] != '.'; i--);
      if (SaveName[i] != '.')
	/* there is no suffix */
	{
	  i = len;
	  SaveName[i++] = '.';
	}
      else
	/* there is a suffix */
	{
	  i++;
	  if (strncmp (suffix, &SaveName[i], 3) == 0)
	    /* the requested suffix is already here. Do nothing */
	    i = 0;
	}
      
      if (i > 0)
	/* change or append the suffix */
	{
	  strcpy (&SaveName[i], suffix);
	  /* display the new filename in the dialog box */
	  filename = TtaGetMemory (MAX_LENGTH);
	  strcpy (filename, DirectoryName);
	  strcat (filename, DIR_STR);
	  strcat (filename, SaveName);
	  TtaSetTextForm (BaseDialog + NameSave, filename);
	  TtaFreeMemory (filename);
	}
    }
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
  AttributeType       attrType;
  Attribute           attrHREF;
  char               *tempfile;
  char               *tempname;
  char                url_sep;
  int                 val, i;
  boolean             change;

  if (typedata == STRING_DATA && data && strchr (data, '/'))
    url_sep = '/';
  else 
    url_sep = DIR_SEP;

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
		     GetHTMLDocument (LastURLName, NULL, 0, 0, CE_FALSE, FALSE, NULL, NULL);
		   else
		     GetHTMLDocument (LastURLName, NULL, CurrentDocument, CurrentDocument, CE_FALSE, TRUE, NULL, NULL);
		 }
	       else if (DirectoryName[0] != EOS && DocumentName[0] != EOS)
		 {
		   /* load a local file */
		   tempfile = TtaGetMemory (MAX_LENGTH);
		   strcpy (tempfile, DirectoryName);
		   strcat (tempfile, DIR_STR);
		   strcat (tempfile, DocumentName);
		   if (TtaFileExist (tempfile))
		     {
		       if (InNewWindow)
			 GetHTMLDocument (tempfile, NULL, 0, 0, CE_FALSE, FALSE, NULL, NULL);
		       else
			 GetHTMLDocument (tempfile, NULL, CurrentDocument, CurrentDocument, CE_FALSE, TRUE, NULL, NULL);
		     }
		   else
		     TtaSetStatus (CurrentDocument, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), tempfile);
		   TtaFreeMemory (tempfile);
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
	   tempfile = TtaGetMemory (MAX_LENGTH);
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
	   TtaFreeMemory (tempfile);
	 }
       
       break;
     case DirSelect:
       if (DirectoryName[0] != EOS)
	 {
	   if (!strcmp (data, ".."))
	     {
	       /* suppress last directory */
	       tempname = TtaGetMemory (MAX_LENGTH);
	       tempfile = TtaGetMemory (MAX_LENGTH);
	       strcpy (tempname, DirectoryName);
	       TtaExtractName (tempname, DirectoryName, tempfile);
	       TtaFreeMemory (tempfile);
	       TtaFreeMemory (tempname);
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
	 }
       break;
     case DocSelect:
       if (DirectoryName[0] == EOS)
	 /* set path on current directory */
	 getcwd (DirectoryName, MAX_LENGTH);
       
       /* Extract suffix from document name */
       strcpy (DocumentName, data);
       LastURLName[0] = EOS;
       /* construct the document full name */
       tempfile = TtaGetMemory (MAX_LENGTH);
       strcpy (tempfile, DirectoryName);
       strcat (tempfile, DIR_STR);
       strcat (tempfile, DocumentName);
       TtaSetTextForm (BaseDialog + URLName, tempfile);
       TtaFreeMemory (tempfile);
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
       
       /* *********Save document as********* */
     case ToggleSave:
       /* Output format */
       switch (val)
	 {
	 case 0:	/* "Save as HTML" button */
	   SaveAsHTML = !SaveAsHTML;
	   SaveAsText = !SaveAsHTML;
#      ifndef _WINDOWS
	   TtaSetToggleMenu (BaseDialog + ToggleSave, 1, SaveAsText);
#       endif /* _WINDOWS */
	   UpdateSaveAsButtons ();
	   SetFileSuffix ();
	   break;
	 case 1:	/* "Save as Text" button */
	   SaveAsText = !SaveAsText;
	   SaveAsHTML = !SaveAsText;
#      ifndef _WINDOWS
	   TtaSetToggleMenu (BaseDialog + ToggleSave, 0, SaveAsHTML);
#      endif /* _WINDOWS */
	   UpdateSaveAsButtons ();
	   SetFileSuffix ();
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
	 /* "Confirm" Button */
	 {
	   TtaDestroyDialogue (BaseDialog + SaveForm);
	   if (SavingDocument != 0)
	     DoSaveAs ();
	   else if (SavingObject != 0)
	     DoSaveObjectAs ();

	   /* Move the information into LastURLName or DirectoryName */
	   if (IsW3Path (SavePath))
	     {
	       strcpy (LastURLName, SavePath);
	       strcat (LastURLName, "/");
	       strcat (LastURLName, SaveName);
	       DirectoryName[0] = EOS;
	     }
	   else
	     {
	       LastURLName[0] = EOS;
	       strcpy (DirectoryName, SavePath);
	       strcat (DocumentName, SaveName);
	     }
	 }
       else if (val == 2)
	 /* "Clear" button */
	 {
	   if (SavingDocument != 0)
	     {
	       SavePath[0] = EOS;
	       SaveImgsURL[0] = EOS;
	       SaveName[0] = EOS;
	       TtaSetTextForm (BaseDialog + NameSave, SaveImgsURL);
	       TtaSetTextForm (BaseDialog + ImgDirSave, SaveImgsURL);
	     }
	 }
       else if (val == 3)
	 /* "Filter" button */
	 {
	   /* reinitialize directories and document lists */
	   if (SavingDocument != 0)
	     TtaListDirectory (SavePath, BaseDialog + SaveForm,
			       TtaGetMessage (LIB, TMSG_DOC_DIR),
			       BaseDialog + DirSave, ScanFilter,
			       TtaGetMessage (AMAYA, AM_FILES), BaseDialog + DocSave);
	 }
       else
	 /* "Cancel" button */
	 {
	   TtaDestroyDialogue (BaseDialog + SaveForm);
	   if (SavingObject != 0)
	      /* delete temporary file */
	      DeleteTempObjectFile ();
	   SavingDocument = 0;
	   SavingObject = 0;
	 }
       break;
     case NameSave:
       /* Document location */
       tempfile = TtaGetMemory (MAX_LENGTH);
       if (!IsW3Path (data))
	 {
	   change = NormalizeFile (data, tempfile);
	   if (change)
	     TtaSetTextForm (BaseDialog + NameSave, tempfile);
	 }
       else
	 strcpy (tempfile, data);
       
       if (tempfile[strlen (tempfile) - 1] == url_sep)
	 {
	   strcpy (SavePath, tempfile);
	   SaveName[0] = EOS;
	 }
       else
	 {
	   /* Extract document name */
	   if (SavingDocument != 0)
	     TtaExtractName (tempfile, SavePath, SaveName);
	   else
	     TtaExtractName (tempfile, SavePath, ObjectName);
	 }
       TtaFreeMemory (tempfile);
       break;
     case ImgDirSave:
       /* Image directory */
       strcpy (SaveImgsURL, data);
       break;
     case DirSave:
       if (!IsW3Path (SavePath))
	 {
	   /* Document directories */
	   tempfile = TtaGetMemory (MAX_LENGTH);
	   if (!strcmp (data, ".."))
	     {
	       /* suppress last directory */
	       tempname = TtaGetMemory (MAX_LENGTH);
	       strcpy (tempname, SavePath);
	       TtaExtractName (tempname, SavePath, tempfile);
	       TtaFreeMemory (tempname);
	     }
	   else
	     {
	       strcat (SavePath, DIR_STR);
	       strcat (SavePath, data);
	     }
	   strcpy (tempfile, SavePath);
	   strcat (tempfile, DIR_STR);
	   if (SavingDocument != 0)
	     strcat (tempfile, DocumentName);
	   else
	     strcat (tempfile, ObjectName);
	   TtaSetTextForm (BaseDialog + NameSave, SavePath);
	   TtaListDirectory (SavePath, BaseDialog + SaveForm,
			     TtaGetMessage (LIB, TMSG_DOC_DIR), BaseDialog + DirSave,
			     ScanFilter, TtaGetMessage (AMAYA, AM_FILES), BaseDialog + DocSave);
	   TtaFreeMemory (tempfile);
	 }
       break;
     case DocSave:
       /* Files */
       if (SaveName[0] == EOS)
	 /* set path on current directory */
	 getcwd (SavePath, MAX_LENGTH);
       
       strcpy (SaveName, data);
       /* construct the document full name */
       tempfile = TtaGetMemory (MAX_LENGTH);
       strcpy (tempfile, SavePath);
       strcat (tempfile, DIR_STR);
       strcat (tempfile, SaveName);
       TtaSetTextForm (BaseDialog + NameSave, tempfile);
       TtaFreeMemory (tempfile);
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
       if (AttrHREFvalue[0] != EOS)
	 TtaSetAttributeText (attrHREF, AttrHREFvalue, AttrHREFelement, AttrHREFdocument);
       else
	 TtaSetAttributeText (attrHREF, "XXX", AttrHREFelement, AttrHREFdocument);
       TtaSetDocumentModified (AttrHREFdocument);
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
  InitAmaya intializes Amaya variables and opent the first document
  window.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitAmaya (NotifyEvent * event)
#else
void                InitAmaya (event)
NotifyEvent        *event;
#endif
{
   int                 i;
   char               *s, *tempname;

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
#  ifndef _WINDOWS
   stopR = TtaCreatePixmapLogo (stopR_xpm);
   stopN = TtaCreatePixmapLogo (stopN_xpm);
   iconSave = TtaCreatePixmapLogo (save_xpm);
   iconFind = TtaCreatePixmapLogo (find_xpm);
   iconReload = TtaCreatePixmapLogo (Reload_xpm);
   iconI = TtaCreatePixmapLogo (I_xpm);
   iconB = TtaCreatePixmapLogo (B_xpm);
   iconT = TtaCreatePixmapLogo (T_xpm);
   iconBack = TtaCreatePixmapLogo (Back_xpm);
   iconBackNo = TtaCreatePixmapLogo (BackNo_xpm);
   iconForward = TtaCreatePixmapLogo (Forward_xpm);
   iconForwardNo = TtaCreatePixmapLogo (ForwardNo_xpm);
   iconH1 = TtaCreatePixmapLogo (H1_xpm);
   iconH2 = TtaCreatePixmapLogo (H2_xpm);
   iconH3 = TtaCreatePixmapLogo (H3_xpm);
   iconPrint = TtaCreatePixmapLogo (Print_xpm);
   iconCSS = TtaCreatePixmapLogo (css_xpm);
   iconBullet = TtaCreatePixmapLogo (Bullet_xpm);
   iconNum = TtaCreatePixmapLogo (Num_xpm);
   iconImage = TtaCreatePixmapLogo (Image_xpm);
   iconDL = TtaCreatePixmapLogo (DL_xpm);
   iconLink = TtaCreatePixmapLogo (Link_xpm);
   iconTable = TtaCreatePixmapLogo (Table_xpm);
#  ifdef AMAYA_PLUGIN
   iconPlugin = TtaCreatePixmapLogo (Plugin_xpm);
   TtaRegisterPixmap("Plugin", iconPlugin);
#  endif /* AMAYA_PLUGIN */
#  ifdef AMAYA_JAVA
   TtaRegisterPixmap("stopR", stopR);
   TtaRegisterPixmap("stopN", stopN);
   TtaRegisterPixmap("Save", iconSave);
   TtaRegisterPixmap("Find", iconFind);
   TtaRegisterPixmap("Reload", iconReload);
   TtaRegisterPixmap("Italic", iconI);
   TtaRegisterPixmap("Bold", iconB);
   TtaRegisterPixmap("TeleType", iconT);
   TtaRegisterPixmap("Back", iconBack);
   TtaRegisterPixmap("BackNo", iconBackNo);
   TtaRegisterPixmap("Forward", iconForward);
   TtaRegisterPixmap("ForwardNo", iconForwardNo);
   TtaRegisterPixmap("H1", iconH1);
   TtaRegisterPixmap("H2", iconH2);
   TtaRegisterPixmap("H3", iconH3);
   TtaRegisterPixmap("Print", iconPrint);
   TtaRegisterPixmap("CSS", iconCSS);
   TtaRegisterPixmap("Bullet", iconBullet);
   TtaRegisterPixmap("Numbered", iconNum);
   TtaRegisterPixmap("Definition", iconDL);
   TtaRegisterPixmap("Link", iconLink);
   TtaRegisterPixmap("Table", iconTable);
   iconJava = TtaCreatePixmapLogo (Java_xpm);
   TtaRegisterPixmap("Java", iconJava);
#  endif /* AMAYA_JAVA */
#  endif /* !_WINDOWS */
#ifdef MATHML
   InitMathML ();
# endif /* MATHML */

   TargetName = NULL;
   /* initialize temporary directory for loaded files */
   s = (char *) TtaGetEnvString ("HOME");

   if (s)
      strcpy (TempFileDirectory, s);
   else
#  ifdef _WINDOWS
     if (!TtaFileExist ("C:\\TEMP"))
        mkdir ("C:\\TEMP");
   strcpy (TempFileDirectory, "C:\\TEMP\\AMAYA");
#  else  /* !_WINDOWS */
     strcpy (TempFileDirectory, "/tmp");
   strcat (TempFileDirectory, "/.amaya");
#  endif /* _WINDOWS */

   i = mkdir (TempFileDirectory, S_IRWXU);
   if (i != 0 && errno != EEXIST)
     {
#  ifndef _WINDOWS
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

   /* Create and intialize resources needed for each document */
   for (i = 1; i < DocumentTableLength; i++)
     {
       /* initialize document table */
       DocumentURLs[i] = NULL;
       HelpDocuments[i] = FALSE;
       /* initialize history */
       InitDocHistory (i);
       /* Create a temporary sub-directory for storing the HTML and
	  image files */
       tempname = TtaGetMemory (MAX_LENGTH);
       sprintf (tempname, "%s%c%d", TempFileDirectory, DIR_SEP, i);
       if (!TtaCheckDirectory (tempname))
          mkdir (tempname, S_IRWXU);
       TtaFreeMemory (tempname);
     }

   /* allocate working buffers */
   LastURLName = TtaGetMemory (MAX_LENGTH);
   LastURLName[0] = EOS;
   DirectoryName = TtaGetMemory (MAX_LENGTH);

   /* set path on current directory */
   getcwd (DirectoryName, MAX_LENGTH);
   DocumentName = TtaGetMemory (MAX_LENGTH);
   DocumentName[0] = EOS;
   SavePath = TtaGetMemory (MAX_LENGTH);
   SavePath[0] = EOS;
   SaveName = TtaGetMemory (MAX_LENGTH);
   SaveName[0] = EOS;
   ObjectName = TtaGetMemory (MAX_LENGTH);
   ObjectName[0] = EOS;
   SaveImgsURL = TtaGetMemory (MAX_LENGTH);
   SaveImgsURL[0] = EOS;
   strcpy (ScanFilter, ".*htm*");
   SaveAsHTML = TRUE;
   SaveAsText = FALSE;
   CopyImages = TRUE;
   UpdateURLs = FALSE;
   SavingFile = TtaGetMemory (MAX_LENGTH);
   AttrHREFvalue = TtaGetMemory (MAX_LENGTH);
   AttrHREFvalue[0] = EOS;

#ifdef WITH_SOCKS
   SOCKSinit ("amaya");
#endif

   /* initialize parser mapping table and HTLib */
   InitMapping ();
#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
   QueryInit ();
#endif

   /* Define the backup function */
   TtaSetBackup (BackUpDocs);
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

   CurrentDocument = 0;
   DocBook = 0;
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
       s = (char *) TtaGetEnvString ("THOTDIR");
       if (s != NULL) {
#         ifdef _WINDOWS
          sprintf (LostPicturePath, "%s\\amaya\\lost.gif", s);              
#         endif /* _WINDOWS */
          strcpy (LastURLName, s);
          }
       else
	LastURLName[0] = EOS;
       strcat (LastURLName, AMAYA_PAGE);
       CallbackDialogue (BaseDialog + OpenForm, INTEGER_DATA, (char *) 1);
       /* Amaya-page in read-only TtaSetDocumentAccessMode (1, 0); */
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
#  ifdef _WINDOWS
   int frame = GetWindowNumber (document, view);

   if (currentFrame == 0 || currentFrame > 10)
      TtaError (ERR_invalid_parameter);
   else {
        HMENU hmenu = WIN_GetMenu (currentFrame); 
        if (!itemChecked) {
          CheckMenuItem (hmenu, menu_item, MF_BYCOMMAND | MF_CHECKED); 
          itemChecked = TRUE ;
       } else {
              hmenu = WIN_GetMenu (currentFrame); 
              CheckMenuItem (hmenu, menu_item, MF_BYCOMMAND | MF_UNCHECKED); 
              itemChecked = FALSE ;
       }
   }
#  endif /* _WINDOWS */
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
   char                localname[MAX_LENGTH];
#ifdef AMAYA_DEBUG
   Element             el;
   View                structView;
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
   structView = TtaGetViewFromName (document, "Structure_view");
   if (structView != 0 && TtaIsViewOpened (document, structView))
     {
       strcpy (localname, TempFileDirectory);
       strcat (localname, "/structview.debug");
       list = fopen (localname, "w");
       TtaListView (document, structView, list);
       fclose (list);
       strcpy (localname, TempFileDirectory);
       strcat (localname, "/structboxes.debug");
       list = fopen (localname, "w");
       TtaListBoxes (document, structView, list);
       fclose (list);
     }
#endif /* AMAYA_DEBUG */

# ifndef _WINDOWS
  TtaNewDialogSheet (BaseDialog + FormAbout, TtaGetViewFrame (document, view), HTAppName, 1,TtaGetMessage(LIB, TMSG_LIB_CONFIRM), TRUE, 1,'L');
# endif  /* _WINDOWS */
  strcpy (localname, HTAppName);
  strcat (localname, " - ");
  strcat (localname, HTAppVersion);
# ifndef _WINDOWS
  TtaNewLabel(BaseDialog + Version, BaseDialog + FormAbout, localname);
  TtaNewLabel(BaseDialog + About1, BaseDialog + FormAbout, TtaGetMessage(AMAYA, AM_ABOUT1));
  TtaNewLabel(BaseDialog + About2, BaseDialog + FormAbout, TtaGetMessage(AMAYA, AM_ABOUT2));
  TtaShowDialogue (BaseDialog + FormAbout, FALSE);
# else  /* _WINDOWS */
  CreateHelpDlgWindow (TtaGetViewFrame (document, view), localname, TtaGetMessage(AMAYA, AM_ABOUT1), TtaGetMessage(AMAYA, AM_ABOUT2));
# endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void DisplayHelp (int index)
#else /* __STDC__*/
static void DisplayHelp (index)
int         index;
#endif /* __STDC__*/
{
  Document    document;
  char    localname[MAX_LENGTH];
  char   *s;
  
  localname[0] = EOS;
  s = (char *) TtaGetEnvString ("THOTDIR");
  if (s != NULL)
    {
      strcat (localname, s);
#     ifdef _WINDOWS
      strcat (localname, "\\doc\\amaya\\");
#     else /* !_WINDOWS */
      strcat (localname, "/doc/amaya/");
#     endif /* _WINDOWS */
      strcat (localname, Manual[index]);
    }

  if (!TtaFileExist (localname))
    {
      strcpy (localname, AMAYA_PAGE_DOC);
      strcat (localname, Manual[index]);
    }
  document = GetHTMLDocument (localname, NULL, 0, 0, CE_HELP, FALSE, NULL, NULL);
  InitDocHistory (document);
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpBrowsing (Document document, View view)
#else /* __STDC__*/
void HelpBrowsing (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (0);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpSelecting (Document document, View view)
#else /* __STDC__*/
void HelpSelecting (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (1);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpSearching (Document document, View view)
#else /* __STDC__*/
void HelpSearching (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (2);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpViews (Document document, View view)
#else /* __STDC__*/
void HelpViews (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (3);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpCreating (Document document, View view)
#else /* __STDC__*/
void HelpCreating (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (4);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpLinks (Document document, View view)
#else /* __STDC__*/
void HelpLinks (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (5);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpChanging (Document document, View view)
#else /* __STDC__*/
void HelpChanging (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (6);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpTables (Document document, View view)
#else /* __STDC__*/
void HelpTables (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (7);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpMath (Document document, View view)
#else /* __STDC__*/
void HelpMath (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (8);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpImageMaps (Document document, View view)
#else /* __STDC__*/
void HelpImageMaps (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (9);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpStyleSheets (Document document, View view)
#else /* __STDC__*/
void HelpStyleSheets (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (10);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpAttributes (Document document, View view)
#else /* __STDC__*/
void HelpAttributes (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (11);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpPublishing (Document document, View view)
#else /* __STDC__*/
void HelpPublishing (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (12);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpPrinting (Document document, View view)
#else /* __STDC__*/
void HelpPrinting (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (13);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpMakeBook (Document document, View view)
#else /* __STDC__*/
void HelpMakeBook (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (14);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void HelpConfigure (Document document, View view)
#else /* __STDC__*/
void HelpConfigure (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  DisplayHelp (15);
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
#  ifdef _WINDOWS
   viewClosed = TRUE;
#  endif /* _WINDOWS */

   W3Loading = 0;

   documentClosed = TRUE;
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

#  ifdef AMAYA_JAVA
   CloseJava();
#  endif
#  ifndef _WINDOWS
   exit(0);
#  endif /* !_WINDOWS */
}

