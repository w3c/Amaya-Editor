/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2003
 *
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Initialization functions and button functions of Amaya application.
 *
 * Author: I. Vatton
 *         R. Guetari (W3C/INRIA) - Windows version
 */

/* Included headerfiles */

#undef THOT_EXPORT
#define THOT_EXPORT extern /* defined into css.c */
#include "amaya.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "css.h"
#include "trans.h"
#include "zlib.h"
#include "profiles.h"

/*
* A VIRER SUREMENT PAR LA SUITE
*
*
*
**/
#ifdef _GTK
  #include "gtkdialogapi.h"
  extern char      LostPicturePath [512];
#endif /* _GTK */


#ifdef _WINDOWS
  #include "resource.h"
#endif /* _WINDOWS */

#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
#include "stopN.xpm"
#include "stopR.xpm"
#include "save.xpm"
#include "saveNo.xpm"
#include "find.xpm"
#include "Reload.xpm"
#include "Browser.xpm"
#include "Editor.xpm"
#include "Print.xpm"
#include "Back.xpm"
#include "BackNo.xpm"
#include "Forward.xpm"
#include "ForwardNo.xpm"
#include "I.xpm"
#include "INo.xpm"
#include "B.xpm"
#include "BNo.xpm"
#include "T.xpm"
#include "TNo.xpm"
#include "H1.xpm"
#include "H1No.xpm"
#include "H2.xpm"
#include "H2No.xpm"
#include "H3.xpm"
#include "H3No.xpm"
#include "Num.xpm"
#include "NumNo.xpm"
#include "Bullet.xpm"
#include "BulletNo.xpm"
#include "Image.xpm"
#include "ImageNo.xpm"
#include "DL.xpm"
#include "DLNo.xpm"
#include "Link.xpm"
#include "LinkNo.xpm"
#include "Table.xpm"
#include "TableNo.xpm"
#include "home.xpm"
#endif /* #if defined(_MOTIF) || defned(_GTK) || defined(_WX) */

#ifdef AMAYA_PLUGIN
  #include "plugin.h"
  #if defined(_MOTIF) || defined(_GTK) || defined(_WX)
    #include "Plugin.xpm"
  #endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
#endif /* AMAYA_PLUGIN */

#include "XPointer_f.h"
#include "anim_f.h"
#include "animbuilder_f.h"
#include "libmanag_f.h"
#ifdef ANNOTATIONS
#include "annotlib.h"
#include "ANNOTevent_f.h"
#include "ANNOTfiles_f.h"
#include "ANNOTtools_f.h"
#endif /* ANNOTATIONS */
#ifdef BOOKMARKS
#include "BMevent_f.h"
#endif /* BOOKMARKS */

#ifdef _WINDOWS
/*
#ifndef __GNUC__
#include <direct.h>
#endif 
*/
int             Window_Curs;
char            DocToOpen[MAX_LENGTH];
#endif /* _WINDOWS */

#include "helpmenu.h"

static int          AmayaInitialized = 0;
static ThotBool     NewFile = FALSE;
static int          NewDocType = 0;
static int          NewDocProfile = 0;
static ThotBool     ShowErrors;
static ThotBool     BADMimeType = FALSE;
static ThotBool     CriticConfirm = FALSE;
/* the open document is the Amaya default page */
static ThotBool     WelcomePage = FALSE;
/* we have to mark the initial loading status to avoid to re-open the
   document view twice */
static int          Loading_method = CE_INIT;

#if defined(_NOGUI) && !defined(_WX) // TODO "&& !defined(_WX)" a virer a la fin de la migration wxWindows
static ThotIcon       stopR;
static ThotIcon       stopN;
static ThotIcon       iconSave;
static ThotIcon       iconSaveNo;
static ThotIcon       iconFind;
static ThotIcon       iconReload;
static ThotIcon       iconI;
static ThotIcon       iconINo;
static ThotIcon       iconB;
static ThotIcon       iconBNo;
static ThotIcon       iconT;
static ThotIcon       iconTNo;
static ThotIcon       iconImage;
static ThotIcon       iconImageNo;
static ThotIcon       iconBack;
static ThotIcon       iconBackNo;
static ThotIcon       iconForward;
static ThotIcon       iconForwardNo;
static ThotIcon       iconH1;
static ThotIcon       iconH1No;
static ThotIcon       iconH2;
static ThotIcon       iconH2No;
static ThotIcon       iconH3;
static ThotIcon       iconH3No;
static ThotIcon       iconPrint;
static ThotIcon       iconBullet;
static ThotIcon       iconBulletNo;
static ThotIcon       iconNum;
static ThotIcon       iconNumNo;
static ThotIcon       iconDL;
static ThotIcon       iconDLNo;
static ThotIcon       iconLink;
static ThotIcon       iconLinkNo;
static ThotIcon       iconTable;
static ThotIcon       iconTableNo;
static ThotIcon       iconBrowser;
static ThotIcon       iconEditor;
static ThotIcon       iconHome;
#ifdef AMAYA_PLUGIN
static ThotIcon       iconPlugin;
#endif /* AMAYA_PLUGIN */
#endif /* #ifdef _NOGUI */


#if defined(_MOTIF) || defined(_GTK) || defined(_WX)
static ThotIcon       stopR;
static ThotIcon       stopN;
static ThotIcon       iconSave;
static ThotIcon       iconSaveNo;
static ThotIcon       iconFind;
static ThotIcon       iconReload;
static ThotIcon       iconI;
static ThotIcon       iconINo;
static ThotIcon       iconB;
static ThotIcon       iconBNo;
static ThotIcon       iconT;
static ThotIcon       iconTNo;
static ThotIcon       iconImage;
static ThotIcon       iconImageNo;
static ThotIcon       iconBack;
static ThotIcon       iconBackNo;
static ThotIcon       iconForward;
static ThotIcon       iconForwardNo;
static ThotIcon       iconH1;
static ThotIcon       iconH1No;
static ThotIcon       iconH2;
static ThotIcon       iconH2No;
static ThotIcon       iconH3;
static ThotIcon       iconH3No;
static ThotIcon       iconPrint;
static ThotIcon       iconBullet;
static ThotIcon       iconBulletNo;
static ThotIcon       iconNum;
static ThotIcon       iconNumNo;
static ThotIcon       iconDL;
static ThotIcon       iconDLNo;
static ThotIcon       iconLink;
static ThotIcon       iconLinkNo;
static ThotIcon       iconTable;
static ThotIcon       iconTableNo;
static ThotIcon       iconBrowser;
static ThotIcon       iconEditor;
static ThotIcon       iconHome;
#ifdef AMAYA_PLUGIN
static ThotIcon       iconPlugin;
#endif /* AMAYA_PLUGIN */
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */

#ifdef _WINDOWS
#define stopR          0
#define stopN          0
#define iconBack       1
#define iconBackNo     1
#define iconForward    2
#define iconForwardNo  2
#define iconReload     3
#define iconSave       4
#define iconSaveNo     4
#define iconPrint      5
#define iconFind       6
#define iconI          7
#define iconINo        7
#define iconB          8
#define iconBNo        8
#define iconT          9
#define iconTNo        9
#define iconImage     11
#define iconImageNo   11
#define iconH1        12
#define iconH1No      12
#define iconH2        13
#define iconH2No      13
#define iconH3        14
#define iconH3No      14
#define iconBullet    15
#define iconBulletNo  15
#define iconNum       16
#define iconNumNo     16
#define	iconDL        17
#define	iconDLNo      17
#define iconLink      18
#define iconLinkNo    18
#define iconTable     19
#define iconTableNo   19
#ifdef AMAYA_PLUGIN
#define iconPlugin    20
#endif /* AMAYA_PLUGIN */
#define iconBrowser   23
#define iconEditor    26
#define iconHome      10

extern int       menu_item;
extern char      LostPicturePath [512];

#include "wininclude.h"
#endif /* _WINDOWS */

#include "AHTURLTools_f.h"
#include "css_f.h"
#include "EDITORactions_f.h"
#include "EDITimage_f.h"
#include "EDITstyle_f.h"
#include "fetchXMLname_f.h"
#ifdef _SVG
#include "SVGedit_f.h"
#endif /* _SVG */
#include "HTMLactions_f.h"
#include "HTMLbook_f.h"
#include "HTMLedit_f.h"
#include "HTMLhistory_f.h"
#include "HTMLimage_f.h"
#include "HTMLsave_f.h"
#include "html2thot_f.h"
#include "Mathedit_f.h"
#include "MENUconf_f.h"
#include "init_f.h"
#include "query_f.h"
#include "styleparser_f.h"
#include "templates_f.h"
#include "trans_f.h"
#include "transparse_f.h"
#include "UIcss_f.h"
#include "string.h"
#include "Xml2thot_f.h"

#ifdef _WINDOWS
  #include "wininclude.h"
#endif /* _WINDOWS */

#ifdef DAV
#define WEBDAV_EXPORT extern
#include "davlib.h"
#include "davlib_f.h"
#include "davlibRequests_f.h"
#include "davlibUI_f.h"
#endif /* DAV */

extern void InitMathML ();

#ifdef AMAYA_PLUGIN
extern void CreateFormPlugin (Document, View);
#endif /* AMAYA_PLUGIN */

/* the structure used for storing the context of the 
   GetAmayaDoc_callback function */
typedef struct _GETHTMLDocument_context
{
  Document   doc;
  Document   baseDoc;
  ThotBool   history;
  ThotBool   local_link;
  char      *target;
  char      *documentname;
  char      *initial_url;
  char      *form_data;
  ClickEvent method;
  ThotBool   inNewWindow;
  TTcbf     *cbf;
  void      *ctx_cbf;
} GETHTMLDocument_context;

/* the structure used for storing the context of the 
   Reload_callback function */
typedef struct _RELOAD_context
{
  Document newdoc;
  char *documentname;
  char *form_data;
  ClickEvent method;
  int position;	/* volume preceding the the first element to be shown */
  int distance; /* distance from the top of the window to the top of this
		   element (% of the window height) */
} RELOAD_context;

typedef enum
{
  OpenDocBrowser,
  HrefAttrBrowser,
  DocSaveBrowser
} TypeBrowserFile;

TypeBrowserFile WidgetParent;


/*----------------------------------------------------------------------
   DocumentMetaDataAlloc
   Creates a DocumentMeta element and initializes it to its default
   values.
  ----------------------------------------------------------------------*/
DocumentMetaDataElement *DocumentMetaDataAlloc (void)
{
  DocumentMetaDataElement *me;

  me = (DocumentMetaDataElement *) TtaGetMemory (sizeof (DocumentMetaDataElement));
  memset ((void *) me, 0, sizeof (DocumentMetaDataElement));
  me->method = CE_ABSOLUTE;
  return (me);
}

/*----------------------------------------------------------------------
   DocumentMetaClear
   Clears the dynamically allocated memory associated to a metadata
   element. Doesn't free the element or clears any of its other elements.
  ----------------------------------------------------------------------*/
void DocumentMetaClear (DocumentMetaDataElement *me)
{
  if (!me)
    return;

  if (me->form_data)
    {
      TtaFreeMemory (me->form_data);
      me->form_data = NULL;
    }
  if (me->initial_url)
    {
      TtaFreeMemory (me->initial_url);
      me->initial_url = NULL;
    }
  if (me->content_type)
    {
      TtaFreeMemory (me->content_type);
      me->content_type = NULL;
    }
  if (me->charset)
    {
      TtaFreeMemory (me->charset);
      me->charset = NULL;
    }
  if (me->content_length)
    {
      TtaFreeMemory (me->content_length);
      me->content_length = NULL;
    }
  if (me->content_location)
    {
      TtaFreeMemory (me->content_location);
      me->content_location = NULL;
    }
  if (me->full_content_location)
    {
      TtaFreeMemory (me->full_content_location);
      me->full_content_location = NULL;
    }
}

/*----------------------------------------------------------------------
   DocumentTypeString
   Returns a string that represents the document type or the current
   profile.
  ----------------------------------------------------------------------*/
char * DocumentTypeString (Document document)
{
  char *result;
  ThotBool isXml;

  result = NULL;
  switch (DocumentTypes[document])
    {
    case docText:
      result = "Text file";
      break;
    case docImage:
      result = "Image";
    case docCSS:
      result = "CSS style sheet";
      break;
    case docSource:
      result = "Document source";
      break;
    case docAnnot:
      result = "Annotation";
      break;
    case docLog:
      result = "Log file"; 
      break;
    case docSVG:
      result = "SVG";
      break;
    case docXml:
      result = "XML";
      break;
    case docLibrary:
      result = "HTML";
      break;
    default:
      break;
    }

  if (!result && DocumentMeta[document]) /* try the profiles */
    {
      isXml = DocumentMeta[document]->xmlformat;
      switch (TtaGetDocumentProfile (document))
	{
	case L_Other:
	  result = "Unknown";
	  break;
	case L_Xhtml11:
	  result = "XHTML 1.1";
	  break;
	case L_Basic:
	  result = "XHTML 1.0 Basic";
	  break;
	case L_Strict:
	  if (isXml)
	    result = "XHTML 1.0 Strict";
	  else
	    result = "HTML Strict";
	  break;
	case L_Transitional:
	  if (isXml)
	    result = "XHTML 1.0 Transitional";
	  else
	    result = "HTML Transitional";
	  break;
	case L_MathML:
	  result = "MathML";
	  break;
	}
    }

  return (result);
}

/*----------------------------------------------------------------------
   DocumentInfo
   Displays the document informations given by the header
  ----------------------------------------------------------------------*/
void DocumentInfo (Document document, View view)
{
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
  char         *content;

  /* Main form */
   TtaNewSheet (BaseDialog + DocInfoForm, TtaGetViewFrame (document, 1),
		"Document Information",
		0, NULL, FALSE, 8, 'L', D_DONE);

   /* Document information labels */
   TtaNewLabel (BaseDialog + DocInfoTitle1,
		BaseDialog + DocInfoForm,
		"________________");

   /* Document URL */
   TtaNewLabel (BaseDialog + DocInfoURLTitle,
		BaseDialog + DocInfoForm,
		TtaGetMessage (AMAYA, AM_HREF_VALUE));

   /* Document type */
   TtaNewLabel (BaseDialog + DocInfoDocTypeTitle,
		BaseDialog + DocInfoForm,
		"Document Type");

   /* Mime Type */
   TtaNewLabel (BaseDialog + DocInfoMimeTypeTitle,
		BaseDialog + DocInfoForm,
		"MIME Type");
   /* Charset */
   TtaNewLabel (BaseDialog + DocInfoCharsetTitle,
		BaseDialog + DocInfoForm,
		"Charset");

   /* Content Length */
   TtaNewLabel (BaseDialog + DocInfoContentTitle,
		BaseDialog + DocInfoForm,
		"Content Length");

   /* Content Location */
   TtaNewLabel (BaseDialog + DocInfoLocationTitle,
		BaseDialog + DocInfoForm,
		"Content Location");

   TtaNewLabel (BaseDialog + DocInfoTitle2,
		BaseDialog + DocInfoForm,
		"________________");

   /* Document information contents */
   TtaNewLabel (BaseDialog + DocInfoContent1,
		BaseDialog + DocInfoForm,
		"___________________________________________");
   /* Document URL */
   if (DocumentURLs[document] != NULL)
     content = DocumentURLs[document];
   else
     content = TtaGetMessage (AMAYA, AM_UNKNOWN);
   TtaNewLabel (BaseDialog + DocInfoURL,
		BaseDialog + DocInfoForm, content);

   /* Document Type */
   content = DocumentTypeString (document);
   if (!content)
     content = TtaGetMessage (AMAYA, AM_UNKNOWN);
   TtaNewLabel (BaseDialog + DocInfoDocType,
		BaseDialog + DocInfoForm, content);

   /* Mime Type */
   if (DocumentMeta[document] && DocumentMeta[document]->content_type != NULL)
     content = DocumentMeta[document]->content_type;
   else
     content = TtaGetMessage (AMAYA, AM_UNKNOWN);
   TtaNewLabel (BaseDialog + DocInfoMimeType,
		BaseDialog + DocInfoForm, content);

   /* Charset */
   if (DocumentMeta[document] && DocumentMeta[document]->charset != NULL)
     content = DocumentMeta[document]->charset;
   else
     content = TtaGetMessage (AMAYA, AM_UNKNOWN);
   TtaNewLabel (BaseDialog + DocInfoCharset,
		BaseDialog + DocInfoForm, content);

   /* Content Length */
   if (DocumentMeta[document] && DocumentMeta[document]->content_length != NULL)
     content = DocumentMeta[document]->content_length;
   else
     content = TtaGetMessage (AMAYA, AM_UNKNOWN);
   TtaNewLabel (BaseDialog + DocInfoContent,
		BaseDialog + DocInfoForm, content);

   /* Content Location */
   if (DocumentMeta[document] 
       && DocumentMeta[document]->full_content_location != NULL)
     content = DocumentMeta[document]->full_content_location;
   else
     content = TtaGetMessage (AMAYA, AM_UNKNOWN);
   TtaNewLabel (BaseDialog + DocInfoLocation,
		BaseDialog + DocInfoForm, content);

   /* end of dialogue */
   TtaNewLabel (BaseDialog + DocInfoContent2,
		BaseDialog + DocInfoForm,
		"___________________________________________");

   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + DocInfoForm, TRUE);

#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
   
#ifdef _WINDOWS
   CreateDocumentInfoDlgWindow (TtaGetViewFrame (document, view),
				document);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   IsDocumentLoaded returns the document identification if the        
   corresponding document is already loaded or 0.          
  ----------------------------------------------------------------------*/
Document IsDocumentLoaded (char *documentURL, char *form_data)
{
  int               i;
  ThotBool          found;

  if (!documentURL)
    return ((Document) None);

  i = 1;
  found = FALSE;
  /* look for the URL into the list of downloaded documents */
  while (!found && i < DocumentTableLength)
    {
      if (DocumentURLs[i])
	{
	  /* compare the url */
	  found = (!strcmp (documentURL, DocumentURLs[i]) ||
		   (DocumentMeta[i]->initial_url &&
		    !strcmp (documentURL, DocumentMeta[i]->initial_url)));
	  /* compare the form_data */
	  if (found && (!((!form_data && !DocumentMeta[i]->form_data) ||
		 (form_data && DocumentMeta[i]->form_data &&
		  !strcmp (form_data, DocumentMeta[i]->form_data)))))
	    found = FALSE;	  
	}
      if (!found)
	i++;
    }
  
  if (found)
    /* document is found */
    return ((Document) i);
  else
    /* document is not found */ 
    return ((Document) None);
}

/*----------------------------------------------------------------------
  CanReplaceCurrentDocument
  Return TRUE if the document has not been modified
  and if the user agrees to loose the changes he/she has made.
  ----------------------------------------------------------------------*/
ThotBool CanReplaceCurrentDocument (Document doc, View view)
{
   ThotBool	ret;

   ret = TRUE;
   if (TtaIsDocumentModified (doc) ||
       (!Synchronizing &&
	DocumentTypes[doc] != docLog && DocumentSource[doc] &&
	TtaIsDocumentModified (DocumentSource[doc])))
     {
       InitConfirm (doc, view, TtaGetMessage (AMAYA, AM_DOC_MODIFIED));
       if (UserAnswer)
	 {
	   TtaSetDocumentUnmodified (doc);
	   if (DocumentSource[doc])
	     TtaSetDocumentUnmodified (DocumentSource[doc]);
	   /* remove the corrsponding auto saved doc */
	   RemoveAutoSavedDoc (doc);
	 }
       else
	 ret = FALSE;
     }
   return ret;
}

/*----------------------------------------------------------------------
   ExtractParameters extract parameters from document nane.        
  ----------------------------------------------------------------------*/
void ExtractParameters (char *aName, char *parameters)
{
   int            lg, i;
   char          *ptr;
   char          *oldptr;

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
   FileExistTarget
   Removes the URL target separator ('#') before verifying if a file
   exists.
  ----------------------------------------------------------------------*/
static ThotBool     FileExistTarget (char *filename)
{
  char   *ptr;
  ThotBool result;

  ptr = strrchr (filename, '#');
  if (ptr)
    *ptr = EOS;
  result = TtaFileExist (filename);
  if (ptr)
    *ptr = '#';
  return result;
}

/*----------------------------------------------------------------------
  SetArrowButton
  Change the appearance of the Back (if back == TRUE) or Forward button
  for a given document.
  ----------------------------------------------------------------------*/
void SetArrowButton (Document document, ThotBool back, ThotBool on)
{
  int		index;
  ThotBool      state;
  ThotIcon	picture;

  if (back)
    {
      index = 2;
      if (on)
	{
	  state   = TRUE;
	  picture = iconBack;
	  TtaSetItemOn (document, 1, File, BBack);
	}
      else
	{
          state = FALSE;
	  picture = iconBackNo;
	  TtaSetItemOff (document, 1, File, BBack);
	}
    }
  else
    {
      index = 3;
      if (on)
	{
	  state = TRUE;
	  picture = iconForward;
	  TtaSetItemOn (document, 1, File, BForward);
        }
      else
	{
	  state = FALSE;
	  picture = iconForwardNo;
	  TtaSetItemOff (document, 1, File, BForward);
	}
    }
  TtaChangeButton (document, 1, index, picture, state);
}

/*----------------------------------------------------------------------
   ResetStop resets the stop button state                             
  ----------------------------------------------------------------------*/
void ResetStop (Document document)
{
  if (document)
    {
      if (FilesLoading[document] != 0)
	FilesLoading[document]--;
      if (FilesLoading[document] == 0)
	/* The last object associated to the document has been loaded */
	{
	  if (TtaGetViewFrame (document, 1) != 0) 
	    /* this document is displayed */
	    {
	      if(!(DocNetworkStatus[document] & AMAYA_NET_ERROR) &&
		 (DocNetworkStatus[document] & AMAYA_NET_ACTIVE))
		/* if there was no error message, display the LOADED message */
		TtaSetStatus (document, 1,
			      TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED), NULL);
	      TtaChangeButton (document, 1, iStop, stopN, FALSE);
#ifdef _GL
	      TtaPlay (document, 1);
#endif /* _GL */
	    }
	  DocNetworkStatus[document] = AMAYA_NET_INACTIVE;
	}
    }
}

/*----------------------------------------------------------------------
   ActiveTransfer initialize the current transfer                     
  ----------------------------------------------------------------------*/
void ActiveTransfer (Document document)
{
  if (document)
    {
      DocNetworkStatus[document] = AMAYA_NET_ACTIVE;
      FilesLoading[document] = 1;
      if (TtaGetViewFrame (document, 1) != 0)
	/* this document is displayed */
	TtaChangeButton (document, 1, iStop, stopR, TRUE);
    }
}

/*----------------------------------------------------------------------
   SetStopButton Activates the stop button if it's turned off
  ----------------------------------------------------------------------*/
void SetStopButton (Document document)
{
  if (document)
    {
      if (document != DocBook)
	{
	  if (DocNetworkStatus[document] != AMAYA_NET_ACTIVE)
	    DocNetworkStatus[document] = AMAYA_NET_ACTIVE;
	  if (FilesLoading[document] == 0)
	    FilesLoading[document] = 1;
	}

      if (TtaGetViewFrame (document, 1) != 0)
	/* this document is displayed */
	TtaChangeButton (document, 1, iStop, stopR, TRUE);
    }
}


/*----------------------------------------------------------------------
   SetFormReadWrite
   Set ReadWrite access to input elements
  ----------------------------------------------------------------------*/
static void SetFormReadWrite (Element el, Document doc)
{
   ElementType  elType;
   Element      child, next;

   while (el)
     {
       /* look at all elements within this form */
       elType = TtaGetElementType (el);
       child = TtaGetFirstChild (el);
       next = el;
       TtaNextSibling (&next);
       switch (elType.ElTypeNum)
	 {
	 case HTML_EL_Input:
	 case HTML_EL_Text_Input:
	 case HTML_EL_Password_Input:
	 case HTML_EL_File_Input:
	 case HTML_EL_Option:
	 case HTML_EL_Text_Area:	/* it's a Text_Area */
	   TtaSetAccessRight (child, ReadWrite, doc);
	   child = NULL;
	   break;
	 default:
	   break;
	 }
       if (child != NULL)
	 SetFormReadWrite (child, doc);
       el = next;
     }
}


/*----------------------------------------------------------------------
   SetDocumentReadOnly
   Set the whole document in ReadOnly mode except input elements
  ----------------------------------------------------------------------*/
static void SetDocumentReadOnly (Document doc)
{
   ElementType  elType;
   Element      el, elForm;

  TtaSetDocumentAccessMode (doc, 0);
  el = TtaGetMainRoot (doc);
  elType = TtaGetElementType (el);
  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
    {
      elType.ElTypeNum = HTML_EL_Form;
      elForm = TtaSearchTypedElement (elType, SearchForward, el);
      while (elForm != NULL)
	{
	  /* there is a form */
	  el = TtaGetFirstChild (elForm);
	  if (el != NULL)
	    SetFormReadWrite (el, doc);
	  elForm = TtaSearchTypedElement (elType, SearchForward, elForm);
	}
    }
}

/*----------------------------------------------------------------------
   Update the save button and corresponding menu entry according to the
   document status.
  ----------------------------------------------------------------------*/
void DocStatusUpdate (Document doc, ThotBool modified)
{
  Document    otherDoc;

  if (modified && TtaGetDocumentAccessMode (doc))
    /* the document has been modified and is not in Read-Only mode */
    {
       TtaSetItemOn (doc, 1, File, BSave);
       TtaChangeButton (doc, 1, iSave, iconSave, TRUE);
       /* if we have a pair source/structured document allow synchronization */
       otherDoc = DocumentSource[doc];
       if (!otherDoc)
	 otherDoc = GetDocFromSource (doc);
       if (otherDoc)
	 {
	   TtaSetItemOn (doc, 1, File, BSynchro);
	   TtaSetItemOn (otherDoc, 1, File, BSynchro);
	 }
       else if (DocumentTypes[doc] == docCSS)
	 TtaSetItemOn (doc, 1, File, BSynchro);
    }
  else
    /* the document is no longer modified */
    {
      TtaSetItemOff (doc, 1, File, BSave);
      TtaChangeButton (doc, 1, iSave, iconSaveNo, FALSE);
      if (TtaIsDocumentUpdated (doc))
	{
	  /* if we have a pair source/structured document allow synchronization */
	  otherDoc = DocumentSource[doc];
	  if (!otherDoc)
	    otherDoc = GetDocFromSource (doc);
	  if (otherDoc)
	    {
	      TtaSetItemOff (doc, 1, File, BSynchro);
	      TtaSetItemOff (otherDoc, 1, File, BSynchro);
	    }
	  else if (DocumentTypes[doc] == docCSS)
	    TtaSetItemOff (doc, 1, File, BSynchro);
	}
    }
}

/*----------------------------------------------------------------------
  UpdateBrowserMenus
  Update windows menus for the Browser mode
  ----------------------------------------------------------------------*/
static void UpdateBrowserMenus (Document doc)
{
  View       view;

  TtaChangeButton (doc, 1, iEditor, iconBrowser, TRUE);
  TtaSetToggleItem (doc, 1, Edit_, TEditMode, FALSE);

  TtaSetItemOff (doc, 1, Edit_, BUndo);
  TtaSetItemOff (doc, 1, Edit_, BRedo);
  TtaSetItemOff (doc, 1, Edit_, BCut);
  TtaSetItemOff (doc, 1, Edit_, BPaste);
  TtaSetItemOff (doc, 1, Edit_, BClear);
  
  if (DocumentTypes[doc] == docHTML ||
      DocumentTypes[doc] == docSVG ||
      DocumentTypes[doc] == docMath ||
      DocumentTypes[doc] == docXml ||
      DocumentTypes[doc] == docLibrary ||
      DocumentTypes[doc] == docImage)
    {
      TtaSetItemOn (doc, 1, Views, TShowMapAreas);
      TtaSetItemOn (doc, 1, Views, TShowTargets);
      TtaSetItemOn (doc, 1, Views, BShowAlternate);
      TtaSetItemOn (doc, 1, Views, BShowToC);
      TtaSetItemOn (doc, 1, Views, BShowStructure);
      TtaSetItemOn (doc, 1, Views, BShowLinks);
      TtaSetItemOn (doc, 1, Views, BShowSource);
      if (DocumentTypes[doc] == docXml)
	{
#ifdef XML_GENERIC      
	  TtaSetMenuOff (doc, 1, Annotations_);
	  TtaSetItemOff (doc, 1, Views, TShowMapAreas);
	  TtaSetItemOff (doc, 1, Views, TShowTargets);
	  TtaSetItemOff (doc, 1, Views, BShowAlternate);
	  TtaSetItemOff (doc, 1, Views, BShowToC);
#endif /* XML_GENERIC */
	}

      TtaChangeButton (doc, 1, iI, iconINo, FALSE);
      TtaChangeButton (doc, 1, iB, iconBNo, FALSE);
      TtaChangeButton (doc, 1, iT, iconTNo, FALSE);
      TtaChangeButton (doc, 1, iImage, iconImageNo, FALSE);
      TtaChangeButton (doc, 1, iH1, iconH1No, FALSE);
      TtaChangeButton (doc, 1, iH2, iconH2No, FALSE);
      TtaChangeButton (doc, 1, iH3, iconH3No, FALSE);
      TtaChangeButton (doc, 1,iBullet, iconBulletNo, FALSE);
      TtaChangeButton (doc, 1,iNum, iconNumNo, FALSE);
      TtaChangeButton (doc, 1,iDL, iconDLNo, FALSE);
      TtaChangeButton (doc, 1, iLink, iconLinkNo, FALSE);
      TtaChangeButton (doc, 1, iTable, iconTableNo, FALSE);
      SwitchIconMath (doc, 1, FALSE);

#ifdef _SVG
      SwitchIconGraph (doc, 1, FALSE);
      SwitchIconLibrary (doc, 1, FALSE);
#ifdef _GL
      SwitchIconAnim (doc, 1, FALSE);
#endif /*_GL*/
#endif /* _SVG */
      
      TtaSetItemOff (doc, 1, Edit_, BSpellCheck);
      TtaSetItemOff (doc, 1, Edit_, BTransform);
      TtaSetMenuOff (doc, 1, Types);
      TtaSetMenuOff (doc, 1, XMLTypes);
      TtaSetMenuOff (doc, 1, Links);
      TtaSetMenuOff (doc, 1, Style);
      TtaSetItemOff (doc, 1, Special, BMakeBook);
      TtaSetItemOff (doc, 1, Special, BMakeID);

      view = TtaGetViewFromName (doc, "Structure_view");
      if (view != 0 && TtaIsViewOpen (doc, view))
	{
	  TtaSetItemOff (doc, view, Edit_, BCut);
	  TtaSetItemOff (doc, view, Edit_, BPaste);
	  TtaSetItemOff (doc, view, Edit_, BClear);
	  TtaSetItemOff (doc, view, Edit_, BSpellCheck);
	  TtaSetItemOff (doc, view, Edit_, BTransform);
	  TtaSetMenuOff (doc, view, StructTypes);
	  TtaSetMenuOff (doc, view, Types);
	  TtaSetMenuOff (doc, view, XMLTypes);
	}
      view = TtaGetViewFromName (doc, "Alternate_view");
      if (view != 0 && TtaIsViewOpen (doc, view))
	{
	  TtaSetItemOff (doc, view, Edit_, BCut);
	  TtaSetItemOff (doc, view, Edit_, BPaste);
	  TtaSetItemOff (doc, view, Edit_, BClear);
	  TtaSetItemOff (doc, view, Edit_, BSpellCheck);
	  TtaSetMenuOff (doc, view, StructTypes);
	  TtaSetMenuOff (doc, view, Types);
	  TtaSetMenuOff (doc, view, XMLTypes);
	}
      view = TtaGetViewFromName (doc, "Links_view");
      if (view != 0 && TtaIsViewOpen (doc, view))
	{
	  TtaSetItemOff (doc, view, Edit_, BCut);
	  TtaSetItemOff (doc, view, Edit_, BPaste);
	  TtaSetItemOff (doc, view, Edit_, BClear);
	  TtaSetItemOff (doc, view, Edit_, BSpellCheck);
	  TtaSetItemOff (doc, view, Edit_, BTransform);
	  TtaSetMenuOff (doc, view, Types);
	  TtaSetMenuOff (doc, view, XMLTypes);
	}
      view = TtaGetViewFromName (doc, "Table_of_contents");
      if (view != 0 && TtaIsViewOpen (doc, view))
	{
	  TtaSetItemOff (doc, view, Edit_, BCut);
	  TtaSetItemOff (doc, view, Edit_, BPaste);
	  TtaSetItemOff (doc, view, Edit_, BClear);
	  TtaSetItemOff (doc, view, Edit_, BSpellCheck);
	  TtaSetItemOff (doc, view, Edit_, BTransform);
	  TtaSetMenuOff (doc, view, Types);
	  TtaSetMenuOff (doc, view, XMLTypes);
	}
    }
}

/*----------------------------------------------------------------------
   UpdateEditorMenus 
   Update windows menus for the Editor mode              
  ----------------------------------------------------------------------*/
static void UpdateEditorMenus (Document doc)
{
  View       view;

  TtaUpdateMenus (doc, 1, ReadOnlyDocument[doc]);
  TtaChangeButton (doc, 1, iEditor, iconEditor, TRUE);
  TtaSetToggleItem (doc, 1, Edit_, TEditMode, TRUE);
  TtaSetItemOn (doc, 1, Edit_, BUndo);
  TtaSetItemOn (doc, 1, Edit_, BRedo);
  TtaSetItemOn (doc, 1, Edit_, BCut);
  TtaSetItemOn (doc, 1, Edit_, BPaste);
  TtaSetItemOn (doc, 1, Edit_, BClear);

  if (DocumentTypes[doc] == docHTML ||
      DocumentTypes[doc] == docSVG ||
      DocumentTypes[doc] == docMath ||
      DocumentTypes[doc] == docXml ||
      DocumentTypes[doc] == docImage)
    {
      TtaSetItemOn (doc, 1, Edit_, BSpellCheck);
      TtaSetItemOn (doc, 1, Edit_, BTransform);
      TtaSetMenuOn (doc, 1, Links);
      TtaSetMenuOn (doc, 1, Style);
      TtaSetMenuOn (doc, 1, Attributes_);
      if (DocumentTypes[doc] != docMath &&
	  DocumentTypes[doc] != docXml)
	{
	  if (DocumentTypes[doc] != docHTML ||
	      TtaGetDocumentProfile (doc) != L_Strict)
	    TtaSetMenuOn (doc, 1, XMLTypes);
	  if (DocumentTypes[doc] == docHTML ||
	      DocumentTypes[doc] == docImage ||
	      DocumentTypes[doc] == docLibrary)
	    {
	      TtaSetMenuOn (doc, 1, Types);
	      TtaSetItemOn (doc, 1, Views, TShowMapAreas);
	      TtaSetItemOn (doc, 1, Views, TShowTargets);
	      TtaSetItemOn (doc, 1, Views, BShowAlternate);
	      TtaSetItemOn (doc, 1, Views, BShowToC);
	    }
	  TtaSetItemOn (doc, 1, Special, TSectionNumber);
	  TtaSetItemOn (doc, 1, Special, BMakeBook);
	  TtaSetItemOn (doc, 1, Special, BMakeID);

	  TtaChangeButton (doc, 1, iI, iconI, TRUE);
	  TtaChangeButton (doc, 1, iB, iconB, TRUE);
	  TtaChangeButton (doc, 1, iT, iconT, TRUE);
	  TtaChangeButton (doc, 1, iImage, iconImage, TRUE);
	  TtaChangeButton (doc, 1, iH1, iconH1, TRUE);
	  TtaChangeButton (doc, 1, iH2, iconH2, TRUE);
	  TtaChangeButton (doc, 1, iH3, iconH3, TRUE);
	  TtaChangeButton (doc, 1, iBullet, iconBullet, TRUE);
	  TtaChangeButton (doc, 1, iNum, iconNum, TRUE);
	  TtaChangeButton (doc, 1, iDL, iconDL, TRUE);
	  TtaChangeButton (doc, 1, iTable, iconTable, TRUE);
	  SwitchIconMath (doc, 1, TRUE);
#ifdef _SVG
	  SwitchIconGraph (doc, 1, TRUE);
	  SwitchIconLibrary (doc, 1, TRUE);
	  SwitchIconAnim (doc, 1, TRUE);
#ifdef _GL
	  SwitchIconAnimPlay (doc, 1, TRUE);
#endif /*_GL*/
#endif /* _SVG */
	}
      else
	{
	  TtaSetMenuOn (doc, 1, XMLTypes);
	  if ( DocumentTypes[doc] == docMath)
	    {
	      TtaSetItemOff (doc, 1, Links, BDeleteAnchor);
	      SwitchIconMath (doc, 1, TRUE);
	    }
	}
      TtaSetItemOn (doc, 1, Views, BShowStructure);
      TtaSetItemOn (doc, 1, Views, BShowLinks);
      TtaSetItemOn (doc, 1, Views, BShowSource);
      TtaChangeButton (doc, 1, iLink, iconLink, TRUE);

      view = TtaGetViewFromName (doc, "Structure_view");
      if (view != 0 && TtaIsViewOpen (doc, view))
	{
	  TtaSetItemOn (doc, view, Edit_, BCut);
	  TtaSetItemOn (doc, view, Edit_, BPaste);
	  TtaSetItemOn (doc, view, Edit_, BClear);
	  TtaSetItemOn (doc, view, Edit_, BSpellCheck);
	  TtaSetItemOn (doc, view, Edit_, BTransform);
	  if (DocumentTypes[doc] != docMath &&
	      DocumentTypes[doc] != docXml)
	    {
	      TtaSetMenuOn (doc, view, StructTypes);
	      TtaSetMenuOn (doc, view, Types);
	    }
	  if (DocumentTypes[doc] != docHTML ||
	      TtaGetDocumentProfile (doc) != L_Strict)
	    TtaSetMenuOn (doc, view, XMLTypes);
	}
      view = TtaGetViewFromName (doc, "Alternate_view");
      if (view != 0 && TtaIsViewOpen (doc, view))
	{
	  TtaSetItemOn (doc, view, Edit_, BCut);
	  TtaSetItemOn (doc, view, Edit_, BPaste);
	  TtaSetItemOn (doc, view, Edit_, BClear);
	  TtaSetItemOn (doc, view, Edit_, BSpellCheck);
	  TtaSetMenuOn (doc, view, StructTypes);
	  if (DocumentTypes[doc] != docMath &&
	      DocumentTypes[doc] != docXml)
	    TtaSetMenuOn (doc, view, Types);
	  if (DocumentTypes[doc] != docHTML ||
	      TtaGetDocumentProfile (doc) != L_Strict)
	    TtaSetMenuOn (doc, view, XMLTypes);
	}
      view = TtaGetViewFromName (doc, "Links_view");
      if (view != 0 && TtaIsViewOpen (doc, view))
	{
	  TtaSetItemOn (doc, view, Edit_, BCut);
	  TtaSetItemOn (doc, view, Edit_, BPaste);
	  TtaSetItemOn (doc, view, Edit_, BClear);
	  TtaSetItemOn (doc, view, Edit_, BSpellCheck);
	  TtaSetItemOn (doc, view, Edit_, BTransform);
	  if (DocumentTypes[doc] != docMath &&
	      DocumentTypes[doc] != docXml)
	    TtaSetMenuOn (doc, view, Types);
	  if (DocumentTypes[doc] != docHTML ||
	      TtaGetDocumentProfile (doc) != L_Strict)
	    TtaSetMenuOn (doc, view, XMLTypes);
	}
      view = TtaGetViewFromName (doc, "Table_of_contents");
      if (view != 0 && TtaIsViewOpen (doc, view))
	{
	  TtaSetItemOn (doc, view, Edit_, BCut);
	  TtaSetItemOn (doc, view, Edit_, BPaste);
	  TtaSetItemOn (doc, view, Edit_, BClear);
	  TtaSetItemOn (doc, view, Edit_, BSpellCheck);
	  TtaSetItemOn (doc, view, Edit_, BTransform);
	  if (DocumentTypes[doc] != docMath &&
	      DocumentTypes[doc] != docXml)
	    TtaSetMenuOn (doc, view, Types);
	  if (DocumentTypes[doc] != docHTML ||
	      TtaGetDocumentProfile (doc) != L_Strict)
	    TtaSetMenuOn (doc, view, XMLTypes);
	}
    }
}

/*----------------------------------------------------------------------
   ChangeToEdotirMode
   Similar to Editor mode except for  the variable ReadOnlyDocument
  ----------------------------------------------------------------------*/
void    ChangeToEditorMode (Document doc)
{
   Document  docSel;

   if (DocumentTypes[doc] == docXml)
     /* don't allow editing mode for XML document today */
     return;
   docSel = TtaGetSelectedDocument ();
   if (docSel == doc)
     TtaUnselect (doc);

   /* =============> The document is in Read-Write mode now */
   /* change the document status */
   if (TtaIsDocumentModified (doc))
     DocStatusUpdate (doc, TRUE);
   /* change the document status */
   TtaSetDocumentAccessMode (doc, 1);
   /* update windows menus */
   UpdateEditorMenus (doc);
}

/*----------------------------------------------------------------------
   ChangeToBrowserMode
   Similar to Browser mode except for the variable ReadOnlyDocument
  ----------------------------------------------------------------------*/
void    ChangeToBrowserMode (Document doc)
{
   Document  docSel;

   docSel = TtaGetSelectedDocument ();
   if (docSel == doc)
     TtaUnselect (doc);

   /* =============> The document is in Read-Only mode now */
   /* change the document status */
   SetDocumentReadOnly (doc);
   /* update windows menus */
   UpdateBrowserMenus (doc);
}

/*----------------------------------------------------------------------
   Change the Browser/Editor mode                    
  ----------------------------------------------------------------------*/
void SetBrowserEditor (Document doc, View view)
{
   Document  docSel;

   docSel = TtaGetSelectedDocument ();
   if (docSel == doc)
     TtaUnselect (doc);

   if (TtaGetDocumentAccessMode (doc))
     {
       /* =============> The document is in Read-Only mode now */
       /* change the document status */
       SetDocumentReadOnly (doc);
       ReadOnlyDocument[doc] = TRUE;
       /* update windows menus */
       UpdateBrowserMenus (doc);
     }
   else
     {
       /* =============> The document is in Read-Write mode now */
       /* change the document status */
       ReadOnlyDocument[doc] = FALSE;
       TtaSetDocumentAccessMode (doc, 1);
       /* change the document status */
       if (TtaIsDocumentModified (doc))
	 DocStatusUpdate (doc, TRUE);
       /* update windows menus */
       UpdateEditorMenus (doc);
     }
}

/*----------------------------------------------------------------------
  ShowLogFile
  Show error messages generated by the parser.
 -----------------------------------------------------------------------*/
void ShowLogFile (Document doc, View view)
{
  char     fileName [100];
  int      newdoc;

  sprintf (fileName, "%s%c%d%cPARSING.ERR",
	   TempFileDirectory, DIR_SEP, doc, DIR_SEP);
  newdoc = GetAmayaDoc (fileName, NULL, 0, doc, (ClickEvent)CE_LOG, FALSE, NULL,
			NULL, TtaGetDefaultCharset ());
  /* store the relation with the original document */
  if (newdoc)
    {
      DocumentSource[newdoc] = doc;
      TtaSetStatus (newdoc, 1, "   ", NULL);
    }
}

/*----------------------------------------------------------------------
  OpenParsingErrors
  ----------------------------------------------------------------------*/
ThotBool OpenParsingErrors (Document document)
{  
  char       fileName [100];

  sprintf (fileName, "%s%c%d%cPARSING.ERR",
	   TempFileDirectory, DIR_SEP, document, DIR_SEP); 
  if ((ErrFile = fopen (fileName, "w")) == NULL)
    return FALSE;
  else
    fprintf (ErrFile, TtaGetMessage (AMAYA, AM_LINK_LINE));      
  return TRUE;
}

/*----------------------------------------------------------------------
  RemoveParsingErrors
  ----------------------------------------------------------------------*/
void RemoveParsingErrors (Document document)
{  
  char       htmlErrFile [100];
  
  sprintf (htmlErrFile, "%s%c%d%cPARSING.ERR",
	   TempFileDirectory, DIR_SEP, document, DIR_SEP);
  if (TtaFileExist (htmlErrFile))
    TtaFileUnlink (htmlErrFile);
}

/*----------------------------------------------------------------------
  CleanUpParsingErrors
  Initialize the 'PARSING.ERR' file and the related global variables
  ----------------------------------------------------------------------*/
void CleanUpParsingErrors ()
{  
  HTMLErrorsFound = FALSE;
  XMLErrorsFound = FALSE;
  CSSErrorsFound = FALSE;
  XMLErrorsFoundInProfile = FALSE;
  XMLNotWellFormed = FALSE;
  XMLUnknownEncoding = FALSE;
  XMLCharacterNotSupported = FALSE;
  /* close the error file */
  if (ErrFile)
    {
      fclose (ErrFile);
      ErrFile = NULL;
    }
}

/*----------------------------------------------------------------------
  CheckParsingErrors
  Checks the errors during the parsing of the document and active
  (or not) the Show Log File menu entry                   
  ----------------------------------------------------------------------*/
void CheckParsingErrors (Document doc)
{
  char      *ptr, *reload;
#ifndef _PARSING
  char       profile [200];
  int        prof;
#endif /*_PARSING*/

  if (BADMimeType)
    {
      /* the mime type doesn't match the doctype */
       InitConfirm (doc, 1, TtaGetMessage (AMAYA, AM_INVALID_MIMETYPE));
      BADMimeType = FALSE;
   }
  if (ErrFile)
    {
      /* Active the menu entry */
      TtaSetItemOn (doc, 1, Views, BShowLogFile);
      if (XMLNotWellFormed || XMLCharacterNotSupported)
	{
	  /* Raise a popup message */
	  if (DocumentTypes[doc] == docHTML)
	    /* when propose a reload */
	    reload = TtaGetMessage (AMAYA, AM_BUTTON_RELOAD);
	  else
	    reload = NULL;
	  if (XMLCharacterNotSupported)
	    {
	      /* Some elements or attributes are not supported */
	      if (reload)
		ptr = TtaGetMessage (AMAYA, AM_XML_CHARACTER_RETRY);
	      else
		ptr = TtaGetMessage (AMAYA, AM_XML_CHARACTER_ERROR);
	    }
	  else
	    {
	      /* The document is not well-formed */
	      if (reload)
		ptr = TtaGetMessage (AMAYA, AM_XML_RETRY);
	      else
		ptr = TtaGetMessage (AMAYA, AM_XML_ERROR);
	    }
	  ConfirmError (doc, 1, ptr, reload,
			TtaGetMessage (AMAYA, AM_AFILTER_SHOW));
	  CleanUpParsingErrors ();
	  if (UserAnswer && reload)
	    ParseAsHTML (doc, 1);
	  else
	    {
	      ChangeToBrowserMode (doc);
	      if (ShowErrors || UserAnswer)
		{
		  ShowLogFile (doc, 1);
		  ShowSource (doc, 1);
		}
	    }
	}
#ifndef _PARSING
      else if (XMLErrorsFoundInProfile)
	{

	  /* Some elements or attributes are not supported */
	  /* in the current document profile */
	  prof = TtaGetDocumentProfile (doc);
	  if (prof == L_Basic)
	    {
	      strcpy (profile, TtaGetMessage (AMAYA, AM_XML_PROFILE));
	      strcat (profile, " XHTML Basic");
	    }
	  else if (prof == L_Strict)
	    {
	      strcpy (profile, TtaGetMessage (AMAYA, AM_XML_PROFILE));
	      if (DocumentMeta[doc]->xmlformat)
		strcat (profile, " XHTML 1.0 Strict");
	      else
		strcat (profile, " HTML 4.0 Strict");
	    }
	  else if (prof == L_Xhtml11)
	    {
	      strcpy (profile, TtaGetMessage (AMAYA, AM_XML_PROFILE));
	      strcat (profile, " XHTML 1.1");
	    }
	  else if (prof == L_Transitional)
	    strcpy (profile, "");

	  InitConfirm3L (doc, 1, profile, NULL,
			 TtaGetMessage (AMAYA, AM_XML_WARNING), FALSE);
	  CleanUpParsingErrors ();
	  if (UserAnswer)
	    {
	      ShowLogFile (doc, 1);
	      ShowSource (doc, 1);
	    }
	}
      else if (XMLErrorsFound)
	{
	  /* Some elements or attributes are not supported */
	  strcpy (profile, "");
	  InitConfirm (doc, 1, TtaGetMessage (AMAYA, AM_XML_WARNING));
	  CleanUpParsingErrors ();
	  if (UserAnswer)
	    {
	      ShowLogFile (doc, 1);
	      ShowSource (doc, 1);
	    }
	}
#endif /*_PARSING*/
      CleanUpParsingErrors ();
    }
  else
      TtaSetItemOff (doc, 1, Views, BShowLogFile);
}


/*----------------------------------------------------------------------
   UpdateTransfer updates the status of the current transfer
  ----------------------------------------------------------------------*/
void UpdateTransfer (Document document)
{
  if (document)
    FilesLoading[document]++;
}

/*----------------------------------------------------------------------
   StopTransfer stops the current transfer                            
  ----------------------------------------------------------------------*/
void StopTransfer (Document document, View view)
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
  else if (DocNetworkStatus[document] & AMAYA_NET_ACTIVE)
    {
      if (TtaGetViewFrame (document, 1) != 0)
	TtaChangeButton (document, 1, 1, stopN, FALSE);
      StopRequest (document);
      FilesLoading[document] = 0;
      DocNetworkStatus[document] = AMAYA_NET_INACTIVE;
      TtaSetStatus (document, 1, 
		    TtaGetMessage (AMAYA, AM_LOAD_ABORT), 
		    NULL);
    }
}

/*----------------------------------------------------------------------
  CompleteUrl
  In case of a user typed url without protocol specification
  and filepath like url (the ~ or / url beginning), 
  we add the http:// (more conveniant when you often type urls)
  so that you can now enter w3.org directly in the url bar.
  Return TRUE if the URL changed.
  ----------------------------------------------------------------------*/
static ThotBool  CompleteUrl(char **url)
{
    char *s;

  if (**url != DIR_SEP 
      && **url != '~'
#ifdef _WINDOWS
      && (*(url))[1] != ':'
#endif /* _WINDOWS */
      && !IsW3Path (*url) 
      && !IsFilePath (*url)
      && (strlen (*url) + 8) < MAX_LENGTH)
  {
      if (TtaFileExist (*url) == 0)
      {
	  s = (char *)TtaGetMemory (MAX_LENGTH);
	  strcpy (s, "http://");
	  strcat (s, *url);
	  *url = s;
	  return TRUE;
      }
  }
  return FALSE;
}


/*----------------------------------------------------------------------
   TextURL                                                      
   The Address text field in a document window has been modified by the user
   Load the corresponding document in that window.
  ----------------------------------------------------------------------*/
static void TextURL (Document doc, View view, char *text)
{
  char             *s = NULL;
  char             *url;
  ThotBool          change, updated;

  updated = FALSE;
  if (text)
    {
      /* remove any trailing '\n' chars that may have gotten there
	 after a cut and pase */
      change = RemoveNewLines (text);
      if (IsW3Path (text))
	url = text;
      else
	{
	  s = (char *)TtaGetMemory (MAX_LENGTH);
	  updated = CompleteUrl(&text);
	  change = NormalizeFile (text, s, AM_CONV_NONE);
	  if (updated)
	    /* free the allocated string */
	    TtaFreeMemory (text);
	  url = s;
	}
      if (!InNewWindow && !CanReplaceCurrentDocument (doc, view))
	{
	  /* restore the previous value @@ */
	  AddURLInCombobox (DocumentURLs[doc], NULL, FALSE);
	  TtaSetTextZone (doc, view, URL_list);
	  /* cannot load the new document */
	  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), text);
	  /* abort the command */
	  TtaFreeMemory (s);
	  return;
	}
      else
	/* do the same thing as a callback form open document form */
	CallbackDialogue (BaseDialog + URLName, STRING_DATA, url);
      TtaFreeMemory (s);
      InNewWindow = FALSE;
      CurrentDocument = doc;
      CallbackDialogue (BaseDialog + OpenForm, INTEGER_DATA, (char *) 1);
    }
}

/*----------------------------------------------------------------------
   SetWindowTitle
   Change the title of all windows (if view == 0) of document targetDoc
   or only the title of the window associated with the specified view.
   if it's not 0.
   The new title is the content of the TITLE element of document sourceDoc.
   If document sourceDoc does not have any TITLE element, nothing happen.
  ----------------------------------------------------------------------*/
void SetWindowTitle (Document sourceDoc, Document targetDoc, View view)
{
   ElementType         elType;
   Element             el;

   el = TtaGetMainRoot (sourceDoc);
   elType.ElSSchema = TtaGetDocumentSSchema (sourceDoc);
   if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
     /* sourceDoc is a HTML document */
     {
       /* search the Title element in sourceDoc */
       elType.ElTypeNum = HTML_EL_TITLE;
       el = TtaSearchTypedElement (elType, SearchForward, el);
       if (el)
	 UpdateTitle (el, sourceDoc);
     }
}

/*----------------------------------------------------------------------
  InitFormAnswer
  Dialogue form for answering text, user name and password
  ----------------------------------------------------------------------*/
void InitFormAnswer (Document document, View view, const char *auth_realm,
		     char *server)
{
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
   char *label;

   TtaNewForm (BaseDialog + FormAnswer, TtaGetViewFrame (document, view), 
	       TtaGetMessage (AMAYA, AM_GET_AUTHENTICATION),
	       TRUE, 1, 'L', D_CANCEL);

   label = (char *)TtaGetMemory (((server) ? strlen (server) : 0)
			   + strlen (TtaGetMessage (AMAYA, 
						     AM_AUTHENTICATION_REALM_SERVER))
			   + ((auth_realm) ? strlen (auth_realm) : 0)
			   + 20); /*a bit more than enough memory */
   sprintf (label, TtaGetMessage (AMAYA, AM_AUTHENTICATION_REALM_SERVER),
	    ((auth_realm) ? auth_realm : ""), 
	    ((server) ? server : ""));
   TtaNewLabel (BaseDialog + RealmText, BaseDialog + FormAnswer,
		label);
   TtaFreeMemory (label);
   
   TtaNewTextForm (BaseDialog + NameText, BaseDialog + FormAnswer,
		   TtaGetMessage (AMAYA, AM_NAME), NAME_LENGTH, 1, FALSE);

#ifdef _MOTIF
   TtaNewTextForm (BaseDialog + PasswordText, BaseDialog + FormAnswer,
		   TtaGetMessage (AMAYA, AM_PASSWORD), NAME_LENGTH, 1, TRUE);
#endif /* _MOTIF */
   
#ifdef _GTK
   TtaNewPwdForm (BaseDialog + PasswordText, BaseDialog + FormAnswer,
		  TtaGetMessage (AMAYA, AM_PASSWORD), NAME_LENGTH, 1, TRUE);
#endif /* _GTK */
   
   TtaSetTextForm (BaseDialog + NameText, Answer_name);
   TtaSetTextForm (BaseDialog + PasswordText, Answer_password);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + FormAnswer, FALSE);
   TtaWaitShowDialogue ();
   if (UserAnswer &&
       (Answer_name[0] == EOS || Answer_password[0] == EOS))
     {
       /* no login name or password, retry */
       TtaSetTextForm (BaseDialog + NameText, Answer_name);
       TtaSetTextForm (BaseDialog + PasswordText, Answer_password);
       TtaSetDialoguePosition ();
       TtaShowDialogue (BaseDialog + FormAnswer, FALSE);
       TtaWaitShowDialogue ();
     }
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */

#ifdef _WINDOWS
   CreateAuthenticationDlgWindow (TtaGetViewFrame (document, view),
				  (char *)auth_realm, server);
#endif /* _WINDOWS */

}


/*----------------------------------------------------------------------
  InitInfo
  Displays a message box with the given info text
  ----------------------------------------------------------------------*/
void InitInfo (char *label, char *info)
{
  if (!info || *info == EOS)
    return;
#ifdef _WINDOWS   
  MessageBox (NULL, info, label, MB_OK);
#endif /* !_WINDOWS */

#if defined(_MOTIF) || defined(_GTK) || defined(_WX)  
  TtaDisplayMessage (CONFIRM, info, NULL);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ConfirmError (Document document, View view, char *label,
		   char *extrabutton, char *confirmbutton)
{
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
   char      s[MAX_LENGTH];
   int       i, n;

   i = 0;
   n = 1;
   if (extrabutton)
     {
       /* display 3 buttons: extrabutton - show - cancel */
       strcpy (&s[i], extrabutton);
       n++;
       i += strlen (&s[i]) + 1;
     }
   strcpy (&s[i], confirmbutton);
   TtaNewSheet (BaseDialog + ConfirmForm, TtaGetViewFrame (document, view),
		TtaGetMessage (LIB, TMSG_LIB_CONFIRM),
		n, s, TRUE, 2, 'L', D_CANCEL);
   TtaNewLabel (BaseDialog + ConfirmText, BaseDialog + ConfirmForm, label);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + ConfirmForm, FALSE);
   /* wait for an answer */
   TtaWaitShowDialogue ();
   /* remove the critic section */
   CriticConfirm = FALSE;
#endif /* #if defined(_MOTIF) || defined(_GTK) */
   
#ifdef _WINDOWS
   CreateInitConfirmDlgWindow (TtaGetViewFrame (document, view),
			       extrabutton, label);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void InitConfirm3L (Document document, View view, char *label1, char *label2,
		    char *label3, ThotBool withCancel)
{
#if defined(_MOTIF) || defined(_GTK)
   /* IV: This widget can't be called twice, but it happens when downloading a
      document with protected images. This is a quick silution to avoid the
      sigsev, although it doesn't fix the problem */
   if (CriticConfirm)
     return;
   else
     CriticConfirm = TRUE;

  /* Confirm form */
  if (withCancel)
    TtaNewForm (BaseDialog + ConfirmForm, TtaGetViewFrame (document, view),  
		TtaGetMessage (LIB, TMSG_LIB_CONFIRM), FALSE, 3, 'L', D_CANCEL);
  else
    TtaNewDialogSheet (BaseDialog + ConfirmForm, TtaGetViewFrame (document, view),
		       TtaGetMessage(LIB, TMSG_LIB_CONFIRM),
		       1,  TtaGetMessage(LIB, TMSG_LIB_CONFIRM),
		       FALSE, 3, 'L');
   /* open as many label widgets as \n we find in the label */
   if (label1 && *label1 != EOS)
     TtaNewLabel (BaseDialog + Label1, BaseDialog + ConfirmForm, label1);
   else
     TtaNewLabel (BaseDialog + Label1, BaseDialog + ConfirmForm, "");
   /* open as many label widgets as \n we find in the label */
   if (label2 && *label2 != EOS)
     TtaNewLabel (BaseDialog + Label2, BaseDialog + ConfirmForm, label2);
   else
     TtaNewLabel (BaseDialog + Label2, BaseDialog + ConfirmForm, "");
   if (label3 && *label3  != EOS)
     TtaNewLabel (BaseDialog + Label3, BaseDialog + ConfirmForm, label3);
   else
     TtaNewLabel (BaseDialog + Label3, BaseDialog + ConfirmForm, "");
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + ConfirmForm, FALSE);
   /* wait for an answer */
   TtaWaitShowDialogue ();
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
   
#ifdef _WINDOWS
   CreateInitConfirm3LDlgWindow (TtaGetViewFrame (document, view),
				 TtaGetMessage (LIB, TMSG_LIB_CONFIRM), label1,
				 label2, label3, withCancel);
#endif /* _WINDOWS */

}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void InitConfirm (Document document, View view, char *label)
{
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
   /* Confirm form */

   /* JK: This widget can't be called twice, but it happens when downloading a
      document with protected images. This is a quick silution to avoid the
      sigsev, although it doesn't fix the problem */
   if (CriticConfirm)
     return;
   else
     CriticConfirm = TRUE;

   TtaNewForm (BaseDialog + ConfirmForm, TtaGetViewFrame (document, view),
               TtaGetMessage (LIB, TMSG_LIB_CONFIRM), TRUE, 2, 'L', D_CANCEL);
   TtaNewLabel (BaseDialog + ConfirmText, BaseDialog + ConfirmForm, label);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + ConfirmForm, FALSE);
   /* wait for an answer */
   TtaWaitShowDialogue ();
   /* remove the critic section */
   CriticConfirm = FALSE;
#endif /* #if defined(_MOTIF) || defined(_GTK) */   
   
#ifdef _WINDOWS
   CreateInitConfirmDlgWindow (TtaGetViewFrame (document, view), NULL, label);
#endif /* _WINDOWS */

}

/*----------------------------------------------------------------------
  InitCharset
  Asks the user for the charset  of a given URL. Used when saving a
  document with an unknown charset.
  ----------------------------------------------------------------------*/
void InitCharset (Document document, View view, char *url)
{
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
  char   s[MAX_LENGTH]; /* general purpose buffer */
  int    i;

  i = 0;
  strcpy (&s[i], "Bus-ascii");
  i += strlen (&s[i]) + 1;
  strcpy (&s[i], "BUTF-8");
  i += strlen (&s[i]) + 1;
  strcpy (&s[i], "Biso-8859-1");
  i += strlen (&s[i]) + 1;
  if (!strcmp (UserCharset, "us-ascii"))
    i = 0;
  else if (!strcmp (UserCharset, "iso-8859-1"))
    i = 2;
  else
    i = 1;
      
  TtaNewForm (BaseDialog + CharsetForm, TtaGetViewFrame (document, view),
	      TtaGetMessage (AMAYA, AM_SELECT_CHARSET), TRUE, 1, 'L', D_CANCEL);
  /* radio buttons */
  TtaNewSubmenu (BaseDialog + CharsetSel, BaseDialog + CharsetForm, 0,
		 NULL, 3, s, NULL, FALSE);
  TtaSetMenuForm (BaseDialog + CharsetSel, i);
      
  TtaSetDialoguePosition ();
  TtaShowDialogue (BaseDialog + CharsetForm, FALSE);
  /* wait for an answer */
  TtaWaitShowDialogue ();
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */

#ifdef _WINDOWS
  CreateCharsetDlgWindow (TtaGetViewFrame (document, view));
#endif /* _WINDOWS */

}

/*----------------------------------------------------------------------
  InitMimeType
  Asks the user for the MIME type of a given URL. Used when saving a
  document with an unknown MIME type.
  ----------------------------------------------------------------------*/
void InitMimeType (Document document, View view, char *url, char *status)
{
  char *mimetypes_list;
  int nbmimetypes;

  if (DocumentTypes[document] == docImage)
    {
      mimetypes_list = "image/png\0"
	"image/jpeg\0"
	"image/gif\0"
	"image/x-bitmap\0"
	"image/x-xpicmap\0";
      nbmimetypes = 5;
    }
  else if (DocumentTypes[document] == docSVG)
    {
      mimetypes_list = 	AM_SVG_MIME_TYPE"\0"
	"application/xml\0"
	"text/xml\0";
      nbmimetypes = 4;
    }
  else if (DocumentTypes[document] == docMath)
    {
      mimetypes_list = AM_MATHML_MIME_TYPE"\0"
	"application/xml\0"
	"text/xml\0";
      nbmimetypes = 3;
    }
  else if (DocumentTypes[document] == docHTML && DocumentMeta[document] &&
	   DocumentMeta[document]->xmlformat)
    {
      mimetypes_list = AM_XHTML_MIME_TYPE"\0"
	"text/html\0"
	"application/xml\0"
	"text/xml\0";
      nbmimetypes = 4;
    }
  else
    {
      mimetypes_list = "text/html\0"
	AM_XHTML_MIME_TYPE"\0"
	"application/xml\0"
	"text/xml\0"
	"text/plain\0"
	"text/css\0"
	"application/smil\0";
      nbmimetypes = 7;
    }

#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
   TtaNewForm (BaseDialog + MimeTypeForm, TtaGetViewFrame (document, view),
	       TtaGetMessage (AMAYA, AM_SELECT_MIMETYPE),  TRUE, 1, 'L', D_CANCEL);
   /* selector */
   TtaNewSelector (BaseDialog + MimeTypeSel, BaseDialog + MimeTypeForm, NULL,
		   nbmimetypes, mimetypes_list, 4, NULL, TRUE, FALSE);
   /* status */
   if (status && *status)
     TtaNewLabel (BaseDialog + MimeFormStatus, BaseDialog + MimeTypeForm, status);
   else
     TtaNewLabel (BaseDialog + MimeFormStatus, BaseDialog + MimeTypeForm,
       "     ");
   TtaSetSelector (BaseDialog + MimeTypeSel, -1,  UserMimeType);

   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + MimeTypeForm, FALSE);
   /* wait for an answer */
   TtaWaitShowDialogue ();
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */

#ifdef _WINDOWS
   CreateMimeTypeDlgWindow (TtaGetViewFrame (document, view), nbmimetypes,
	                        mimetypes_list);
#endif /* _WINDOWS */
}

/*-------------------------------------------------------------------------
  BrowserForm
  Initializes a form that ask the URI of the opened or new created document.
  -------------------------------------------------------------------------*/
static void BrowserForm (Document doc, View view, char *urlname)
{
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
   char      s[MAX_LENGTH];
   int       i;
   char      tempfile[MAX_LENGTH];

   /* Dialogue form for open URL or local */
   i = 0;
   strcpy (&s[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_CLEAR));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_PARSE));
   
   TtaNewSheet (BaseDialog + FileBrowserForm, TtaGetViewFrame (doc, 1),
		TtaGetMessage (AMAYA, AM_FILE_BROWSER), 3, s,
		TRUE, 2, 'L', D_CANCEL);
   TtaNewTextForm (BaseDialog + FileBrowserText, BaseDialog + FileBrowserForm,
		   TtaGetMessage (AMAYA, AM_SELECTION), 50, 1, TRUE);
   TtaNewLabel (BaseDialog + FileBrowserLocalName,
		BaseDialog + FileBrowserForm, " ");

   /* initialise the text fields in the dialogue box */
   tempfile[0] = EOS;
   if (urlname[0] != EOS)
       NormalizeFile (urlname, tempfile, AM_CONV_NONE);
   
   if (tempfile[0] != EOS && !IsW3Path (tempfile))
     { 
       if (TtaCheckDirectory (tempfile))
	 {
	   strcpy (DirectoryName, tempfile);
	   DocumentName[0] = EOS;
	 }
       else
	 TtaExtractName (tempfile, DirectoryName, DocumentName);
       if (DirectoryName[0] == EOS)
	 {
	   getcwd (DirectoryName, MAX_LENGTH);
	   DocumentName[0] = EOS;
	 }
       strcpy (s, DirectoryName);
       strcat (s, DIR_STR);
       strcat (s, DocumentName);
     }
   else
     {
       if (tempfile[0] != EOS && IsW3Path (tempfile))
	 {
	   getcwd (DirectoryName, MAX_LENGTH);
	   DocumentName[0] = EOS;
	 }
       else
	 {
	   if (DirectoryName[0] == EOS)
	     {
	       getcwd (DirectoryName, MAX_LENGTH);
	       DocumentName[0] = EOS;
	     }
	 }
       strcpy (s, DirectoryName);
       strcat (s, DIR_STR);
       strcat (s, DocumentName);
     }

   TtaListDirectory (DirectoryName, BaseDialog + FileBrowserForm,
		     TtaGetMessage (LIB, TMSG_DOC_DIR),
		     BaseDialog + BrowserDirSelect, ScanFilter,
		     TtaGetMessage (AMAYA, AM_FILES),
		     BaseDialog + BrowserDocSelect);
   TtaNewTextForm (BaseDialog + FileBrowserFilter,
		   BaseDialog + FileBrowserForm,
		   TtaGetMessage (AMAYA, AM_PARSE), 10, 1, TRUE);

   TtaSetTextForm (BaseDialog + FileBrowserText, s);
   TtaSetTextForm (BaseDialog + FileBrowserFilter, ScanFilter);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + FileBrowserForm, FALSE);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
}

/*----------------------------------------------------------------------
  InitOpenDocForm initializes a form that ask the URI of the opened or
  new created document.
  The parameter name gives a proposed document name.
  The parameter title gives the title of the the form.
  ----------------------------------------------------------------------*/
static void InitOpenDocForm (Document doc, View view, char *name, char *title,
			     DocumentType docType)
{
  char              s[MAX_LENGTH];
  ThotBool          remote;
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
  int               i;

  /* Dialogue form for open URL or local */
  i = 0;
  strcpy (&s[i], TtaGetMessage (AMAYA, AM_OPEN_URL));
  i += strlen (&s[i]) + 1;
  strcpy (&s[i], TtaGetMessage (AMAYA, AM_BROWSE));
  i += strlen (&s[i]) + 1;
  strcpy (&s[i], TtaGetMessage (AMAYA, AM_CLEAR));

  TtaNewSheet (BaseDialog + OpenForm, TtaGetViewFrame (doc, view),
	       title, 3, s, TRUE, 2, 'L', D_CANCEL);
  TtaNewTextForm (BaseDialog + URLName, BaseDialog + OpenForm,
		  TtaGetMessage (AMAYA, AM_LOCATION), 50, 1, TRUE);
  TtaNewLabel (BaseDialog + LocalName, BaseDialog + OpenForm, " ");
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */

  CurrentDocument = doc;
  /* generate the right name and URI */
  TtaExtractName (DocumentURLs[doc], s, DocumentName);
  remote = IsW3Path (DocumentURLs[doc]);
  if (remote)
    {
      if (name[0] != EOS)
	{
	  strcpy (LastURLName, s);
	  strcat (LastURLName, URL_STR);
	  strcat (LastURLName, name);
	}
      else
	strcpy (LastURLName, DocumentURLs[doc]);
      strcpy (s, LastURLName);
    }
  else
    {
      /* check if it's the default Welcome page */
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
      if (WelcomePage)
	{
	  getcwd (s, MAX_LENGTH);
	  if (name[0] != EOS)
	    {
	      strcat (s, DIR_STR);
	      strcat (s, name);
	    }
	}
      else
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
        
      if (name[0] == EOS)
	{
	  if (DocumentURLs[doc])
	    strcpy (s, DocumentURLs[doc]);
	  else
	    s[0] = EOS;
	}
      else
	{
	  strcpy (DirectoryName, s);
	  strcpy (DocumentName, name);
	  strcat (s, DIR_STR);
	  strcat (s, name);
	}
      strcpy (LastURLName, s);
    }

#ifdef  _WINDOWS
  CreateOpenDocDlgWindow (TtaGetViewFrame (doc, view), title, s, name,
			  DocSelect, DirSelect, docType);
#endif /* WINDOWS */

#if defined(_MOTIF) || defined(_GTK) || defined(_WX)  
  TtaSetTextForm (BaseDialog + URLName, s);
  TtaSetDialoguePosition ();
  TtaShowDialogue (BaseDialog + OpenForm, TRUE);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
  
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void  OpenDoc (Document doc, View view)
{
   if (CanReplaceCurrentDocument (doc, view))
     {
       /* load the new document */
       InNewWindow = FALSE;
           /* no specific type requested */
       InitOpenDocForm (doc, view, "",
			TtaGetMessage (AMAYA, AM_OPEN_DOCUMENT), docText);
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void OpenDocInNewWindow (Document document, View view)
{
   InNewWindow = TRUE;
   /* no specific type requested */
   InitOpenDocForm (document, view, "",
		    TtaGetMessage (AMAYA, AM_OPEN_IN_NEW_WINDOW),
	   docText);
}


/*----------------------------------------------------------------------
  OpenNew: create a new document
  ----------------------------------------------------------------------*/
void OpenNew (Document document, View view, int docType, int docProfile)
{
  /* create a new document */
  InNewWindow = TRUE;
  NewFile = TRUE;
  NewDocType = docType;
  NewDocProfile = docProfile;

  if (NewDocType == docHTML)
    {
      /* will scan html documents */
      strcpy (ScanFilter, "*.*htm*");
      if (docProfile == L_Basic)
	InitOpenDocForm (document, view, "New.html", TtaGetMessage (AMAYA, AM_NEW_HTML_BASIC), docHTML);
      else if (docProfile == L_Strict)
	InitOpenDocForm (document, view, "New.html", TtaGetMessage (AMAYA, AM_NEW_HTML_STRICT), docHTML);
      else if (docProfile == L_Xhtml11)
	InitOpenDocForm (document, view, "New.html", TtaGetMessage (AMAYA, AM_NEW_HTML11), docHTML);
      else
	InitOpenDocForm (document, view, "New.html", TtaGetMessage (AMAYA, AM_NEW_HTML_TRANSITIONAL), docHTML);
   }
  else if (NewDocType == docMath)
    {
      /* will scan html documents */
      strcpy (ScanFilter, "*.mml");
    InitOpenDocForm (document, view, "New.mml", TtaGetMessage (AMAYA, AM_NEW_MATHML), docMath);
    }
  else if (NewDocType == docSVG)
    {
      /* will scan html documents */
      strcpy (ScanFilter, "*.svg");
      InitOpenDocForm (document, view, "New.svg", TtaGetMessage (AMAYA, AM_NEW_SVG), docSVG);
    }
  else
    {
      /* will scan html documents */
      strcpy (ScanFilter, "*.css");
      InitOpenDocForm (document, view, "New.css", TtaGetMessage (AMAYA, AM_NEW_CSS), docCSS);
    }
}

/*----------------------------------------------------------------------
  Load the Home page
  ----------------------------------------------------------------------*/
void GoToHome (Document doc, View view)
{
  char     *s, *lang;

  s = TtaGetEnvString ("HOME_PAGE");
  lang = TtaGetVarLANG ();

  if (s == NULL)
    {
      /* the open document is the Amaya default page */
      WelcomePage = TRUE;
      s = TtaGetEnvString ("THOTDIR");
      sprintf (LastURLName, "%s%camaya%c%s.%s",
	       s, DIR_SEP, DIR_SEP, AMAYA_PAGE, lang);
      if (!TtaFileExist (LastURLName))
	sprintf (LastURLName, "%s%camaya%c%s",
		 s, DIR_SEP, DIR_SEP, AMAYA_PAGE);
    }
  else
    strcpy (LastURLName, s);

  if (doc == 0 || CanReplaceCurrentDocument (doc, view))
    {
      /* load the HOME document */
      InNewWindow = FALSE;
      CurrentDocument = doc;
      CallbackDialogue (BaseDialog + OpenForm, INTEGER_DATA, (char *) 1);
    }
}

/*----------------------------------------------------------------------
  UpdateDoctypeMenu
  ----------------------------------------------------------------------*/
void UpdateDoctypeMenu (Document doc)
{
  DocumentType    docType;
  SSchema         nature;
  char           *ptr;
  ThotBool	  useMathML, useSVG;
 
  docType = DocumentTypes[doc];

  TtaSetItemOn  (doc, 1, File, Doctype1);
  TtaSetItemOn  (doc, 1, File, BRemoveDoctype);
  TtaSetItemOff (doc, 1, File, BDoctypeXhtml11);
  TtaSetItemOff (doc, 1, File, BDoctypeXhtmlTransitional);
  TtaSetItemOff (doc, 1, File, BDoctypeXhtmlStrict);
  TtaSetItemOff (doc, 1, File, BDoctypeXhtmlBasic);
  TtaSetItemOff (doc, 1, File, BDoctypeHtmlTransitional);
  TtaSetItemOff (doc, 1, File, BDoctypeHtmlStrict);
  TtaSetItemOff (doc, 1, File, BDoctypeXhtmlPlusMath);
  TtaSetItemOff (doc, 1, File, BDoctypeMathML);
  TtaSetItemOff (doc, 1, File, BDoctypeSVG);


  if (docType == docText || docType == docCSS ||
      docType == docSource || docType == docLog)
    {
      /* Don't change the doctype for a text document */
      TtaSetItemOff (doc, 1, File, Doctype1);
      TtaSetItemOff (doc, 1, File, BRemoveDoctype);
      return;
    }

  /* look for the MathML nature used in the document */
  nature = NULL;
  useMathML = FALSE;
  useSVG = FALSE;
  do
    {
      TtaNextNature (doc, &nature);
      if (nature)
	{
	  ptr = TtaGetSSchemaName (nature);
	  if (!strcmp (ptr, "MathML"))
	    useMathML = TRUE;
	  if (!strcmp (ptr, "SVG"))
	    useSVG = TRUE;
	}
    }
  while (nature);

  switch (TtaGetDocumentProfile (doc))
    {
    case L_Other:
      if (docType == docHTML)
	{
	  if (useMathML || useSVG)
	    TtaSetItemOn (doc, 1, File, BDoctypeXhtmlPlusMath);
	  else if (DocumentMeta[doc]->xmlformat)
	    {
	      TtaSetItemOn (doc, 1, File, BDoctypeXhtml11);
	      TtaSetItemOn (doc, 1, File, BDoctypeXhtmlTransitional);
	      TtaSetItemOn (doc, 1, File, BDoctypeXhtmlStrict);
	      TtaSetItemOn (doc, 1, File, BDoctypeXhtmlBasic);
	    }
	  else
	    {
	      TtaSetItemOn (doc, 1, File, BDoctypeXhtmlTransitional);
	      TtaSetItemOn (doc, 1, File, BDoctypeXhtmlStrict);
	      TtaSetItemOn (doc, 1, File, BDoctypeXhtml11);
	      TtaSetItemOn (doc, 1, File, BDoctypeXhtmlBasic);
	      TtaSetItemOn (doc, 1, File, BDoctypeHtmlTransitional);
	      TtaSetItemOn (doc, 1, File, BDoctypeHtmlStrict);
	    }
	}
      else if (docType == docMath)
	TtaSetItemOn (doc, 1, File, BDoctypeMathML);
      else if (docType == docSVG)
	TtaSetItemOn (doc, 1, File, BDoctypeSVG);
      break;
    case L_Xhtml11:
      if (useMathML)
	TtaSetItemOn (doc, 1, File, BDoctypeXhtmlPlusMath);
      else
	{
	  TtaSetItemOn (doc, 1, File, BDoctypeXhtmlTransitional);
	  TtaSetItemOn (doc, 1, File, BDoctypeXhtmlStrict);
	  TtaSetItemOn (doc, 1, File, BDoctypeXhtmlBasic);
	}
      break;
    case L_Basic:
      TtaSetItemOn (doc, 1, File, BDoctypeXhtml11);
      TtaSetItemOn (doc, 1, File, BDoctypeXhtmlTransitional);
      TtaSetItemOn (doc, 1, File, BDoctypeXhtmlStrict);
      break;
    case L_Strict:
      if (DocumentMeta[doc]->xmlformat)
	{
	  TtaSetItemOn (doc, 1, File, BDoctypeXhtmlTransitional);
	  TtaSetItemOn (doc, 1, File, BDoctypeXhtml11);
	  TtaSetItemOn (doc, 1, File, BDoctypeXhtmlBasic);
	}
      else
	{
	  TtaSetItemOn (doc, 1, File, BDoctypeXhtmlTransitional);
	  TtaSetItemOn (doc, 1, File, BDoctypeXhtmlStrict);
	  TtaSetItemOn (doc, 1, File, BDoctypeXhtml11);
	  TtaSetItemOn (doc, 1, File, BDoctypeXhtmlBasic);
	  TtaSetItemOn (doc, 1, File, BDoctypeHtmlTransitional);
	}
      break;
    case L_Transitional:
      if (DocumentMeta[doc]->xmlformat)
	{
	  TtaSetItemOn (doc, 1, File, BDoctypeXhtmlStrict);
	  TtaSetItemOn (doc, 1, File, BDoctypeXhtml11);
	  TtaSetItemOn (doc, 1, File, BDoctypeXhtmlBasic);
	}
      else
	{
	  TtaSetItemOn (doc, 1, File, BDoctypeXhtmlTransitional);
	  TtaSetItemOn (doc, 1, File, BDoctypeXhtmlStrict);
	  TtaSetItemOn (doc, 1, File, BDoctypeXhtml11);
	  TtaSetItemOn (doc, 1, File, BDoctypeXhtmlBasic);
	  TtaSetItemOn (doc, 1, File, BDoctypeHtmlStrict);
	}
      break;
    case L_MathML:
      TtaSetItemOn (doc, 1, File, BDoctypeMathML);
      break;
    case L_SVG:
      TtaSetItemOn (doc, 1, File, BDoctypeSVG);
      break;
    }
}

/*----------------------------------------------------------------------
  AddDirAttributeToDocEl
  Set the HTML attribute dir on the Document element 
  ----------------------------------------------------------------------*/
void AddDirAttributeToDocEl (Document doc)
{
  Element root;
  Attribute     attr;
  AttributeType attrType;

  root = TtaGetMainRoot (doc);
  if (root)
    {
      attrType.AttrSSchema =  TtaGetSSchema ("HTML", doc);
      if (!attrType.AttrSSchema)
	return;
      attrType.AttrTypeNum = HTML_ATTR_dir;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (root, attr, doc);
      TtaSetAttributeValue (attr, HTML_ATTR_dir_VAL_ltr_, root, doc);
    }
}

/*----------------------------------------------------------------------
   InitDocAndView prepares the main view of a new document.
   logFile is TRUE if the new view is created to display a log file
   sourceOfDoc is not zero when we're opening the source view of a document.
  ----------------------------------------------------------------------*/
Document InitDocAndView (Document doc, char *docname, DocumentType docType,
			 Document sourceOfDoc, ThotBool readOnly, int profile,
			 ClickEvent method)
{
  View          mainView, structView, altView, linksView, tocView;
  Document      old_doc;
  Element       root, comment, leaf;
  ElementType   elType;
  char         *tmp, buffer[MAX_LENGTH], *string = NULL;
  int           x, y, w, h;
  int           requested_doc;
  Language	lang;
  ThotBool      isOpen, reinitialized, show;

#ifdef _WINDOWS
  Window_Curs = IDC_WINCURSOR;
#endif /* _WINDOWS */

  /* is there any editing function available */
  if (!TtaCanEdit ())
    /* change the document status */
    readOnly = TRUE;

  /* previous document */
  old_doc = doc;
  reinitialized = FALSE;

  if (doc != 0)
    /* the new document will replace another document in the same window */
    {
      /* keep in memory if the closed document is in read-only mode */
      if (ReadOnlyDocument[doc])
	readOnly = TRUE;
      else
	/* if there is a parsing error/warning for the old document */
	if (!TtaGetDocumentAccessMode (doc))
	  reinitialized = TRUE;

      if (DocumentTypes[doc] == docHTML ||
	  DocumentTypes[doc] == docSVG ||
	  DocumentTypes[doc] == docXml ||
	  DocumentTypes[doc] == docMath ||
	  DocumentTypes[doc] == docLibrary)
	{
	  /* close the Alternate view if it is open */
	  altView = TtaGetViewFromName (doc, "Alternate_view");
	  if (altView != 0 && TtaIsViewOpen (doc, altView))
	    TtaCloseView (doc, altView);
	  /* close the Structure view if it is open */
	  structView = TtaGetViewFromName (doc, "Structure_view");
	  if (structView != 0 && TtaIsViewOpen (doc, structView))
	    TtaCloseView (doc, structView);
	  /* close the Links view if it is open */
	  linksView = TtaGetViewFromName (doc, "Links_view");
	  if (linksView != 0 && TtaIsViewOpen (doc, linksView))
	    TtaCloseView (doc, linksView);
	  /* close the Table_of_contents view if it is open */
	  tocView = TtaGetViewFromName (doc, "Table_of_contents");
	  if (tocView != 0 && TtaIsViewOpen (doc, tocView))
	    TtaCloseView (doc, tocView);
	}
        /* remove the current selection */
	TtaUnselect (doc);
	UpdateContextSensitiveMenus (doc);
	TtaFreeView (doc, 1);
	isOpen = TRUE;
	/* use the same document identifier */
	requested_doc = doc;
	old_doc = 0;	/* the previous document doesn't exist any more */
	/* The toolkit has to do its job now */
	TtaHandlePendingEvents ();
     }
   else
     {
       /* open the new document in a fresh window */
       isOpen = FALSE;
       requested_doc = 0;
     }

   /* Init the new document */
   if (docType == docText || docType == docCSS ||
       docType == docSource || docType == docLog)
     doc = TtaInitDocument ("TextFile", docname, requested_doc);
   else if (docType == docAnnot)
     doc = TtaInitDocument ("Annot", docname, requested_doc);
#ifdef BOOKMARKS
   else if (docType == docBookmark)
     doc = TtaInitDocument ("Topics", docname, requested_doc);
#endif /* BOOKMARKS */
   else if (docType == docSVG)
     doc = TtaInitDocument ("SVG", docname, requested_doc);
   else if (docType == docMath)
     doc = TtaInitDocument ("MathML", docname, requested_doc);
#ifdef XML_GENERIC      
   else if (docType == docXml)
     doc = TtaInitDocument ("XML", docname, requested_doc);
#endif /* XML_GENERIC */
   else
     doc = TtaInitDocument ("HTML", docname, requested_doc);
   if (doc >= DocumentTableLength)
     {
       TtaCloseDocument (doc);
       doc = 0;
     }
   else if (doc > 0)
     {
       /* assign a presentation model to the document */
       if (docType == docText || docType == docCSS ||
           docType == docSource || docType == docLog)
	   TtaSetPSchema (doc, "TextFileP");
       else if (docType == docAnnot)
	   TtaSetPSchema (doc, "AnnotP");
#ifdef BOOKMARKS
       else if (docType == docBookmark)
	   TtaSetPSchema (doc, "TopicsP");
#endif /* BOOKMARKS */
       else if (docType == docSVG)
	   TtaSetPSchema (doc, "SVGP");
       else if (docType == docMath)
	   TtaSetPSchema (doc, "MathMLP");
#ifdef XML_GENERIC      
       else if (docType == docXml)
	   TtaSetPSchema (doc, "XMLP");
#endif /* XML_GENERIC */
       /* @@ shouldn't we have a Color and BW case for annots too? */
       else
	 {
	   if (TtaGetScreenDepth () > 1)
	     TtaSetPSchema (doc, "HTMLP");
	   else
	     TtaSetPSchema (doc, "HTMLPBW");
	   /* set attribute dir on the Document element. */
	   AddDirAttributeToDocEl (doc);
	 }
       if (docType == docSVG || docType == docMath)
	 /* add a comment proudly claiming that the document was created by
	    Amaya */
	 {
	   root = TtaGetRootElement (doc);
	   elType = TtaGetElementType (root);
	   if (docType == docSVG)
	     elType.ElTypeNum = SVG_EL_XMLcomment;
	   else
	     elType.ElTypeNum = MathML_EL_XMLcomment;
	   comment = TtaNewTree (doc, elType, "");
	   TtaSetStructureChecking (0, doc);
	   TtaInsertSibling (comment, root, TRUE, doc);
	   TtaSetStructureChecking (1, doc);
	   strcpy (buffer, " Created by ");
	   strcat (buffer, HTAppName);
	   strcat (buffer, " ");
	   strcat (buffer, HTAppVersion);
	   strcat (buffer, ", see http://www.w3.org/Amaya/ ");
	   leaf = TtaGetFirstLeaf (comment);
	   lang = TtaGetLanguageIdFromScript('L');
	   TtaSetTextContent (leaf, (unsigned char *)buffer, lang, doc);
	 }
       TtaSetNotificationMode (doc, 1);
       /* get the geometry of the main view */
       if (docType == docAnnot)
	 tmp = "Annot_Formatted_view";
       else if (docType == docBookmark)
	 tmp = "Topics_Formatted_view";
       else if (sourceOfDoc && docType == docSource)
	 tmp = "Source_view";
       else
	 tmp = "Formatted_view";
       TtaGetViewGeometry (doc, tmp, &x, &y, &w, &h);
       if (docType == docLog)
	 {
	   x += 100;
	   y += 60;
	   h = 300;
	   w = 550;
	 }
       else if (docType == docMath)
	 {
	   h = 300;
	   w = 580;
	 }
       else if (method == CE_HELP )
	 {
	   x += 500;
	   y += 200;
	   h = 500;
	   w = 800;
	 }
       else if (docType == docLibrary && method == CE_RELATIVE)
	 {
	   x += 500;
	   y += 200;
	   h = 500;
	   w = 400;
	 }
       /* change the position slightly to avoid hiding completely the main
	  view of other documents */
       x = x + (doc - 1) * 10;
       y = y + (doc - 1) * 10;
       /* open the main view */
       if (docType == docLog ||
	   (docType == docLibrary && method == CE_RELATIVE))
	 /* without menu bar */
	 mainView = TtaOpenMainView (doc, x, y, w, h, FALSE, TRUE);
       else
	 mainView = TtaOpenMainView (doc, x, y, w, h, TRUE, TRUE);
       
       if (mainView == 0)
	 {
	   TtaCloseDocument (doc);
	   return (0);
	 }
       
       /* store the profile of the new document */
       /* and update the menus according to it */
       TtaSetDocumentProfile (doc, profile);
       if (profile != 0)
	 TtaUpdateMenus (doc, 1, readOnly);
     
       /* By default no log file */
       TtaSetItemOff (doc, 1, Views, BShowLogFile);
#ifndef BOOKMARKS
       /* if bookmarks are not enabled, disable the menu */
       TtaSetMenuOff (doc, 1, Bookmarks_);
#endif /* BOOKMARKS */

       if (docType == docSource)
	 {
	   TtaSetItemOff (doc, 1, File, BHtmlBasic);
	   TtaSetItemOff (doc, 1, File, BHtmlStrict);
	   TtaSetItemOff (doc, 1, File, BHtml11);
	   TtaSetItemOff (doc, 1, File, BHtmlTransitional);
	   TtaSetItemOff (doc, 1, File, BMathml);
	   TtaSetItemOff (doc, 1, File, BSvg);
	   TtaSetItemOff (doc, 1, File, BTemplate);
	   TtaSetItemOff (doc, 1, File, BCss);
	   TtaSetItemOff (doc, 1, File, BOpenDoc);
	   TtaSetItemOff (doc, 1, File, BOpenInNewWindow);
	   TtaSetItemOff (doc, 1, File, BReload);
	   TtaSetItemOff (doc, 1, File, BBack);
	   TtaSetItemOff (doc, 1, File, BForward);
	   TtaSetItemOff (doc, 1, File, BSave);
	   TtaSetItemOff (doc, 1, File, BSynchro);
	   TtaSetMenuOff (doc, 1, Types);
	   TtaSetMenuOff (doc, 1, XMLTypes);
	   TtaSetMenuOff (doc, 1, Links);
	   TtaSetMenuOff (doc, 1, Views);
	   TtaSetMenuOff (doc, 1, Style);
	   TtaSetMenuOff (doc, 1, Special);
	   TtaSetMenuOff (doc, 1, Attributes_);
	 }
       else if (docType == docLog || docType == docLibrary)
	 {
	   TtaSetItemOff (doc, 1, File, BHtmlBasic);
	   TtaSetItemOff (doc, 1, File, BHtmlStrict);
	   TtaSetItemOff (doc, 1, File, BHtml11);
	   TtaSetItemOff (doc, 1, File, BHtmlTransitional);
	   TtaSetItemOff (doc, 1, File, BMathml);
	   TtaSetItemOff (doc, 1, File, BSvg);
	   TtaSetItemOff (doc, 1, File, BTemplate);
	   TtaSetItemOff (doc, 1, File, BCss);
	   TtaSetItemOff (doc, 1, File, BOpenDoc);
	   TtaSetItemOff (doc, 1, File, BOpenInNewWindow);
	   TtaSetItemOff (doc, 1, File, BReload);
	   TtaSetItemOff (doc, 1, File, BBack);
	   TtaSetItemOff (doc, 1, File, BForward);
	   TtaSetItemOff (doc, 1, File, BSave);
	   TtaSetItemOff (doc, 1, File, BSynchro);
	   TtaSetItemOff (doc, 1, File, BExit);
	   TtaSetMenuOff (doc, 1, Edit_);
	   TtaSetMenuOff (doc, 1, Types);
	   TtaSetMenuOff (doc, 1, XMLTypes);
	   TtaSetMenuOff (doc, 1, Links);
	   TtaSetMenuOff (doc, 1, Views);
	   TtaSetMenuOff (doc, 1, Style);
	   TtaSetMenuOff (doc, 1, Special);
	   TtaSetMenuOff (doc, 1, Attributes_);
#ifdef ANNOTATIONS
	   TtaSetMenuOff (doc, 1, Annotations_);
#endif /* ANNOTATIONS */
#ifdef BOOKMARKS
	   TtaSetMenuOff (doc, 1, Bookmarks_);
#endif /* BOOKMARKS */
	   TtaSetMenuOff (doc, 1, Help_);
	   TtcSwitchButtonBar (doc, 1); /* no button bar */
	   /* change the document status */
	   ReadOnlyDocument[doc] = FALSE;
	   TtaSetDocumentAccessMode (doc, 1);
	   if (docType == docLibrary)
	     {
#ifdef _SVG
	       if (InNewWindow == TRUE)
		 {
		   /* Initialize SVG Library Buffer string */
		   TtaAddTextZone (doc, 1, TtaGetMessage (AMAYA,  AM_OPEN_URL),
				   FALSE, (Proc)OpenLibraryCallback, SVGlib_list);
		 }
#endif /* _SVG */
	       if (string)
		 TtaFreeMemory (string);
	     }
	   else
	     TtcSwitchCommands (doc, 1); /* no command open */
	 }
#ifdef BOOKMARKS
       else if (docType == docBookmark)
	 {
	   TtaSetItemOff (doc, 1, File, New1);
	   TtaSetItemOff (doc, 1, File, BOpenDoc);
	   TtaSetItemOff (doc, 1, File, BOpenInNewWindow);
	   TtaSetItemOff (doc, 1, File, BReload);
	   TtaSetItemOff (doc, 1, File, BBack);
	   TtaSetItemOff (doc, 1, File, BForward);
	   TtaSetItemOff (doc, 1, File, BSave);
	   TtaSetItemOff (doc, 1, File, BSaveAs);
	   TtaSetItemOff (doc, 1, File, BSynchro);
	   TtaSetItemOff (doc, 1, File, Doctype1);
	   TtaSetItemOff (doc, 1, File, BDocInfo);
	   TtaSetItemOff (doc, 1, File, BSetUpandPrint);
	   TtaSetItemOff (doc, 1, File, BPrint);
	   TtaSetMenuOff (doc, 1, Edit_);
	   TtaSetMenuOff (doc, 1, Types);
	   TtaSetMenuOff (doc, 1, XMLTypes);
	   TtaSetMenuOff (doc, 1, Links);
	   TtaSetMenuOff (doc, 1, Views);
	   TtaSetMenuOff (doc, 1, Style);
	   TtaSetMenuOff (doc, 1, Special);
	   TtaSetMenuOff (doc, 1, Attributes_);
#ifdef ANNOTATIONS
	   TtaSetMenuOff (doc, 1, Annotations_);
#endif /* ANNOTATIONS */
	   TtaSetItemOff (doc, 1, Bookmarks_, BBookmarkFile);
	   TtcSwitchButtonBar (doc, 1); /* no button bar */
	   TtaAddTextZone (doc, 1, TtaGetMessage (AMAYA,  AM_OPEN_URL),
			   TRUE, (Proc) TextURL, NULL);
	   TtcSwitchCommands (doc, 1); /* no command open */
	 }
#endif /* BOOKMARKS */
       else if (!isOpen)
	 /* use a new window */
	 {
	   /* Create all buttons */
	   iStop =TtaAddButton (doc, 1, stopN, (Proc)StopTransfer,"StopTransfer",
				TtaGetMessage (AMAYA, AM_BUTTON_INTERRUPT),
				TBSTYLE_BUTTON, FALSE);
	   iBack = TtaAddButton (doc, 1, iconBackNo, (Proc)GotoPreviousHTML,
				 "GotoPreviousHTML",
				 TtaGetMessage (AMAYA, AM_BUTTON_PREVIOUS),
				 TBSTYLE_BUTTON, FALSE);
	   iForward = TtaAddButton (doc, 1, iconForwardNo, (Proc)GotoNextHTML,
				    "GotoNextHTML",
				    TtaGetMessage (AMAYA, AM_BUTTON_NEXT),
				    TBSTYLE_BUTTON, FALSE);
	   iReload = TtaAddButton (doc, 1, iconReload, (Proc)Reload, "Reload",
				   TtaGetMessage (AMAYA, AM_BUTTON_RELOAD),
				   TBSTYLE_BUTTON, TRUE);
	   iHome = TtaAddButton (doc, 1, iconHome, (Proc)GoToHome, "GoToHome",
				 TtaGetMessage (AMAYA, AM_BUTTON_HOME),
				 TBSTYLE_BUTTON, TRUE);

#ifdef _WINDOWS
	   /* cannot change the browser icone -> use active instead of */
	   iEditor = TtaAddButton (doc, 1, iconBrowser, (Proc)SetBrowserEditor,
				   "SetBrowserEditor",
				   TtaGetMessage (AMAYA, AM_BUTTON_BrowseEdit),
				   TBSTYLE_BUTTON, TRUE);
	   TtaChangeButton (doc, 1, iEditor, iconEditor, TRUE);
#endif /* _WINDOWS */

#if defined(_MOTIF) || defined(_GTK) || defined(_WX)  
	   iEditor = TtaAddButton (doc, 1, iconEditor, (Proc)SetBrowserEditor,
				   "SetBrowserEditor",
				   TtaGetMessage (AMAYA, AM_BUTTON_BrowseEdit),
				   TBSTYLE_BUTTON, TRUE);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
     
	   /* SEPARATOR */
	   TtaAddButton (doc, 1, None, NULL, NULL, NULL, TBSTYLE_SEP, FALSE);
	   iSave = TtaAddButton (doc, 1, iconSaveNo, (Proc)SaveDocument,
				 "SaveDocument",
				 TtaGetMessage (AMAYA, AM_BUTTON_SAVE),
				 TBSTYLE_BUTTON, FALSE);
	   iPrint = TtaAddButton (doc, 1, iconPrint, (Proc)SetupAndPrint,  "SetupAndPrint",
				  TtaGetMessage (AMAYA, AM_BUTTON_PRINT),
				  TBSTYLE_BUTTON, TRUE);
	   iFind = TtaAddButton (doc, 1, iconFind, (Proc)TtcSearchText,
				 "TtcSearchText", 
				 TtaGetMessage (AMAYA, AM_BUTTON_SEARCH),
				 TBSTYLE_BUTTON, TRUE);
	   /* SEPARATOR */
	   TtaAddButton (doc, 1, None, NULL, NULL, NULL, TBSTYLE_SEP, FALSE);
	   iI =  TtaAddButton (doc, 1, iconI, (Proc)SetOnOffEmphasis,
			       "SetOnOffEmphasis",
			       TtaGetMessage (AMAYA, AM_BUTTON_ITALICS),
			       TBSTYLE_CHECK, TRUE);
	   iB =  TtaAddButton (doc, 1, iconB, (Proc)SetOnOffStrong, "SetOnOffStrong",
			       TtaGetMessage (AMAYA, AM_BUTTON_BOLD),
			       TBSTYLE_CHECK, TRUE);
	   iT = TtaAddButton (doc, 1, iconT, (Proc)SetOnOffCode, "SetOnOffCode",
			      TtaGetMessage (AMAYA, AM_BUTTON_CODE),
			      TBSTYLE_CHECK, TRUE);
	   /* SEPARATOR */
	   TtaAddButton (doc, 1, None, NULL, NULL, NULL, TBSTYLE_SEP, FALSE);
	   iImage = TtaAddButton (doc, 1, iconImage, (Proc)CreateImage,
				  "CreateImage", 
				  TtaGetMessage (AMAYA, AM_BUTTON_IMG),
				  TBSTYLE_BUTTON, TRUE);
	   iH1 = TtaAddButton (doc, 1, iconH1, (Proc)CreateHeading1,
			       "CreateHeading1", 
			       TtaGetMessage (AMAYA, AM_BUTTON_H1),
			       TBSTYLE_BUTTON, TRUE);
	   iH2 = TtaAddButton (doc, 1, iconH2, (Proc)CreateHeading2,
			       "CreateHeading2", 
			       TtaGetMessage (AMAYA, AM_BUTTON_H2),
			       TBSTYLE_BUTTON, TRUE);
	   iH3 = TtaAddButton (doc, 1, iconH3, (Proc)CreateHeading3,
			       "CreateHeading3", 
			       TtaGetMessage (AMAYA, AM_BUTTON_H3),
			       TBSTYLE_BUTTON, TRUE);
	   iBullet = TtaAddButton (doc, 1, iconBullet, (Proc)CreateList,
				   "CreateList",
				   TtaGetMessage (AMAYA, AM_BUTTON_UL),
				   TBSTYLE_BUTTON, TRUE);
	   iNum = TtaAddButton (doc, 1, iconNum, (Proc)CreateNumberedList,
				"CreateNumberedList",
				TtaGetMessage (AMAYA, AM_BUTTON_OL),
				TBSTYLE_BUTTON, TRUE);
	   iDL = TtaAddButton (doc, 1, iconDL, (Proc)CreateDefinitionList,
			       "CreateDefinitionList",
			       TtaGetMessage (AMAYA, AM_BUTTON_DL),
			       TBSTYLE_BUTTON, TRUE);
	   iLink = TtaAddButton (doc, 1, iconLink, (Proc)CreateOrChangeLink,
				 "CreateOrChangeLink",
				 TtaGetMessage (AMAYA, AM_BUTTON_LINK),
				 TBSTYLE_BUTTON, TRUE);
	   iTable = TtaAddButton (doc, 1, iconTable, (Proc)CreateTable,
				  "CreateTable",
				  TtaGetMessage (AMAYA, AM_BUTTON_TABLE),
				  TBSTYLE_BUTTON, TRUE);
	   
#ifdef AMAYA_PLUGIN 

#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
	   TtaAddButton (doc, 1, iconPlugin, (Proc)TtaCreateFormPlugin,
			 "TtaCreateFormPlugin",
			 TtaGetMessage (AMAYA, AM_BUTTON_PLUGIN),
			 TBSTYLE_BUTTON, TRUE);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
     
#endif /* AMAYA_PLUGIN */
	   AddMathButton (doc, 1);
#ifdef _SVG
	   AddGraphicsButton (doc, 1);
	   AddLibraryButton (doc, 1);
	   AddAnimButton (doc, 1);
#ifdef _GL
	   AddAnimPlayButton (doc, 1);
#endif /*_GL*/

#endif /* _SVG */
	   if (docType == docAnnot)
	     {
	       /* turn off the menus that don't make sense in the annotation view */
	       TtaAddTextZone (doc, 1, TtaGetMessage (AMAYA,  AM_OPEN_URL),
			       TRUE, (Proc)TextURL, NULL);
	       TtcSwitchCommands (doc, 1); /* no command open */
	       TtaSetItemOff (doc, 1, Views, BShowAlternate);
	       TtaSetItemOff (doc, 1, Views, BShowToC);
	       TtaSetItemOff (doc, 1, Views, BShowSource);
#ifdef BOOKMARKS
	       TtaSetMenuOff (doc, 1, Bookmarks_);
#endif /* BOOKMARKS */
	     }
	   else
	     {
	       TtaAddTextZone (doc, 1, TtaGetMessage (AMAYA,  AM_OPEN_URL),
			       TRUE, (Proc)TextURL, URL_list);
	       /* turn off the assign annotation buttons (should be
		  contextual */
	       TtaSetItemOff (doc, 1, Annotations_, BReplyToAnnotation);
	       TtaSetItemOff (doc, 1, Annotations_, BMoveAnnotSel);
	       TtaSetItemOff (doc, 1, Annotations_, BMoveAnnotXPtr);
	       TtaSetItemOff (doc, 1, Annotations_, BPostAnnot);
	     }

	   /* initial state for menu entries */
	   TtaSetItemOff (doc, 1, File, BBack);
	   TtaSetItemOff (doc, 1, File, BForward);
	   TtaSetItemOff (doc, 1, File, BSave);
	   TtaSetItemOff (doc, 1, File, BSynchro);
	   /*TtaSetToggleItem (doc, 1, Views, TShowButtonbar, SButtons[doc]);*/
	   if (SButtons[doc])
	     TtaSetToggleItem (doc, 1, Views, TShowButtonbar, TRUE);
	   else
	     /* hide buttons */
	     TtcSwitchButtonBar (doc, 1);
	   /*TtaSetToggleItem (doc, 1, Views, TShowTextZone, SAddress[doc]);*/
	   if (SAddress[doc])
	     TtaSetToggleItem (doc, 1, Views, TShowTextZone, TRUE);
	   else
	     /* hide the address */
	     TtcSwitchCommands (doc, 1);
	   TtaSetToggleItem (doc, 1, Views, TShowMapAreas, MapAreas[doc]);
	   TtaSetToggleItem (doc, 1, Special, TSectionNumber, SNumbering[doc]);
	   TtaGetEnvBoolean ("SHOW_TARGET", &show);
	   if (show)
	     ShowTargets (doc, 1);
	   else
	     TtaSetToggleItem (doc, 1, Views, TShowTargets, FALSE);
	   TtaSetMenuOff (doc, 1, Attributes_);

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
#ifdef BOOKMARKS
       /* if there are multiple instances of Amaya, disable the bookmark menu */
       if (!GetBookmarksEnabled ())
	 TtaSetMenuOff (doc, 1, Bookmarks_);
#endif /* BOOKMARKS */
     }

   /* do we have to redraw buttons and menus? */
   if (!reinitialized)
     {
       if ((docType == docHTML || docType == docImage ||
	    docType == docSVG || docType == docLibrary ) &&
	   DocumentTypes[doc] != docHTML &&
	   DocumentTypes[doc] != docImage &&
	   DocumentTypes[doc] != docSVG &&
	   DocumentTypes[doc] != docLibrary)
	 /* we need to update menus and buttons */
	 reinitialized = TRUE;
       else if ((docType == docCSS || docType == docText) &&
		DocumentTypes[doc] != docCSS &&
		DocumentTypes[doc] != docText)
	 /* we need to update menus and buttons */
	 reinitialized = TRUE;
       else if (docType == docMath || docType == docAnnot || docType == docBookmark ||
	        docType == docXml || docType == docSource)
	 reinitialized = TRUE;
     }

   /* store the new document type */
   DocumentTypes[doc] = docType;

   /* set the document in Read-Only mode */
   if (readOnly)
     ReadOnlyDocument[doc] = TRUE;
   
   if (reinitialized || !isOpen)
     {
     /* now update menus and buttons according to the document status */
     if (DocumentTypes[doc] == docText ||
	 DocumentTypes[doc] == docCSS ||
	 DocumentTypes[doc] == docMath ||
	 DocumentTypes[doc] == docXml ||
	 DocumentTypes[doc] == docSource ||
	 ReadOnlyDocument[doc])
       {
	 TtaChangeButton (doc, 1, iI, iconINo, FALSE);
	 TtaChangeButton (doc, 1, iB, iconBNo, FALSE);
	 TtaChangeButton (doc, 1, iT, iconTNo, FALSE);
	 TtaChangeButton (doc, 1, iImage, iconImageNo, FALSE);
	 TtaChangeButton (doc, 1, iH1, iconH1No, FALSE);
	 TtaChangeButton (doc, 1, iH2, iconH2No, FALSE);
	 TtaChangeButton (doc, 1, iH3, iconH3No, FALSE);
	 TtaChangeButton (doc, 1, iBullet, iconBulletNo, FALSE);
	 TtaChangeButton (doc, 1, iNum, iconNumNo, FALSE);
	 TtaChangeButton (doc, 1, iDL, iconDLNo, FALSE);
	 if ((DocumentTypes[doc] != docXml &&
	      DocumentTypes[doc] != docMath) ||
	     ReadOnlyDocument[doc])
	   {
	     TtaChangeButton (doc, 1, iLink, iconLinkNo, FALSE);
	     SwitchIconMath (doc, 1, FALSE);
	   }
	 TtaChangeButton (doc, 1, iTable, iconTableNo, FALSE);
	 TtaSetItemOff (doc, 1, Special, BMakeBook);
	 TtaSetItemOff (doc, 1, Views, TShowMapAreas);
	 TtaSetItemOff (doc, 1, Views, TShowTargets);
	 TtaSetItemOff (doc, 1, Views, BShowAlternate);
	 TtaSetItemOff (doc, 1, Views, BShowLinks);
	 TtaSetItemOff (doc, 1, Views, BShowToC);
	 TtaSetMenuOff (doc, 1, Doctype1);
	 if (DocumentTypes[doc] != docXml || ReadOnlyDocument[doc])
	   {
#ifdef _SVG
	     SwitchIconGraph (doc, 1, FALSE);
	     SwitchIconLibrary (doc, 1, FALSE);
	     SwitchIconAnim (doc, 1, FALSE);
#ifdef _GL
      SwitchIconAnim (doc, 1, FALSE);
#endif /*_GL*/
#endif /* _SVG */
	   }
	 if (ReadOnlyDocument[doc])
	   {
	     /* the document is in ReadOnly mode */
	     SwitchIconMath (doc, 1, FALSE);
	     TtaSetItemOff (doc, 1, Edit_, BTransform);
	     TtaSetMenuOff (doc, 1, Links);
	     TtaSetMenuOff (doc, 1, Style);
	     TtaSetMenuOff (doc, 1, Types);
	     TtaSetMenuOff (doc, 1, XMLTypes);
	     TtaSetMenuOff (doc, 1, Attributes_);
	     TtaSetItemOff (doc, 1, Edit_, BUndo);
	     TtaSetItemOff (doc, 1, Edit_, BRedo);
	     TtaSetItemOff (doc, 1, Edit_, BCut);
	     TtaSetItemOff (doc, 1, Edit_, BPaste);
	     TtaSetItemOff (doc, 1, Edit_, BClear);
	     TtaSetItemOff (doc, 1, Edit_, BSpellCheck);
	     TtaSetToggleItem (doc, 1, Edit_, TEditMode, FALSE);
	     TtaSetItemOff (doc, 1, Special, TSectionNumber);
	     TtaSetItemOff (doc, 1, Special, BMakeID);
	     TtaChangeButton (doc, 1, iEditor, iconBrowser, TRUE);
	     TtaSetMenuOff (doc, 1, Doctype1);
	     TtaSetItemOff (doc, 1, Views, BShowStructure);
	     TtaSetItemOff (doc, 1, Views, BShowSource);
	   }
	 else
	   {
	     TtaSetToggleItem (doc, 1, Edit_, TEditMode, TRUE);
	     TtaSetMenuOff (doc, 1, Types);
	     if (DocumentTypes[doc] != docMath &&
		 DocumentTypes[doc] != docXml)
	       TtaSetMenuOff (doc, 1, XMLTypes);
	     else if (DocumentTypes[doc] == docMath)
	       TtaSetItemOff (doc, 1, Links, BDeleteAnchor);
	   }
       }
     else if (DocumentTypes[doc] == docHTML ||
	      DocumentTypes[doc] == docImage ||
	      DocumentTypes[doc] == docSVG ||
	      DocumentTypes[doc] == docXml ||
	      DocumentTypes[doc] == docLibrary ||
	      DocumentTypes[doc] == docMath)
       {
	 TtaSetToggleItem (doc, 1, Edit_, TEditMode, TRUE);
	 if (DocumentTypes[doc] == docHTML &&
	     TtaGetDocumentProfile (doc) == L_Strict)
	   TtaSetMenuOff (doc, 1, XMLTypes);
	 if (reinitialized)
	   /* the document is in ReadWrite mode */
	   UpdateEditorMenus (doc);
       }
     }


#ifdef DAV    /* after all, we active the WebDAV menu in the main view */
     TtaSetMenuOn (doc, DAV_VIEW, Cooperation_);     
#else
     TtaSetMenuOff (doc, 1, Cooperation_);
#endif  /* DAV */

#ifdef _SVG
     TtaSetItemOn (doc, 1, XML, BShowLibrary);
     TtaSetItemOn (doc, 1, XML, BAddNewModel);
#else /* !_SVG */
     TtaSetItemOff (doc, 1, XML, BShowLibrary);
     TtaSetItemOff (doc, 1, XML, BAddNewModel); 
#endif /* _SVG */
   
   return (doc);
}

/*----------------------------------------------------------------------
  CreateHTMLContainer creates an HTML container for an image
  ----------------------------------------------------------------------*/
static void CreateHTMLContainer (char *pathname, char *docname,
				 char *tempfile, ThotBool local)
{
  FILE *file;
  char *tempfile_new;
  char *ptr;

  if (!local)
    {
      /* Rename the current downloaded file (an image) so that we can
	 find it easily next time around. 
	 The convention is to change the image's extension to 'html',
	 and give the HTML's container the image's extension */
      tempfile_new = (char *)TtaGetMemory (strlen (tempfile) + strlen (docname) + 10);
      strcpy (tempfile_new, tempfile);
      ptr = strrchr (tempfile_new, DIR_SEP);
      ptr++;
      strcpy (ptr, docname);
      SetContainerImageName (tempfile_new);
      TtaFileUnlink (tempfile_new);
      if (TtaFileCopyUncompress (tempfile, tempfile_new))
	/* copy done */
	TtaFileUnlink (tempfile);
      else
	/* change the tempfile name */
	sprintf (tempfile_new, "%s", tempfile);
      TtaFreeMemory (tempfile_new);
    }
  /* create a temporary file for the container and make Amaya think
     that it is the current downloaded file */
  file = fopen (tempfile, "w");
  fprintf (file, "<html><head><title>%s</title></head><body>", docname);
  if (local)
    fprintf (file, "<img src=\"%s\">", pathname);
  else
    fprintf (file, "<img src=\"internal:%s\">", pathname);
  fprintf (file, "</body></html>");
  fclose (file);
}

/*----------------------------------------------------------------------
  MoveImageFile moves an image file (related to an HTML container) from 
  one directory to another
  ----------------------------------------------------------------------*/
static void MoveImageFile (Document source_doc, Document dest_doc,
			   char *documentname)
{
  char *source;
  char *target;
  char *imagefile;
  char *ptr;
  
  /* generate the name of the file where is stored the image */
  imagefile = (char *)TtaGetMemory (strlen (documentname) + 6);
  strcpy (imagefile, documentname);
  ptr = strrchr (imagefile, '.');
  if (!ptr)
     strcat (imagefile, ".html");
  else
     strcpy (&(ptr[1]), "html");

  /* create the source and dest file names */
  source = (char *)TtaGetMemory (strlen (TempFileDirectory) + strlen (imagefile) + 6);
  sprintf (source, "%s%c%d%c%s", 
	   TempFileDirectory, DIR_SEP, source_doc, DIR_SEP, imagefile);
  target = (char *)TtaGetMemory (strlen (TempFileDirectory) + strlen (imagefile) + 6);
  sprintf (target, "%s%c%d%c%s", 
	   TempFileDirectory, DIR_SEP, dest_doc, DIR_SEP, imagefile);

  /* move the file */
  TtaFileCopy (source, target);
  TtaFileUnlink (source);

  TtaFreeMemory (source);
  TtaFreeMemory (target);
  TtaFreeMemory (imagefile);
}

/*----------------------------------------------------------------------
   ParseAsHTML
   Load current document considering it's a HTML document
  ----------------------------------------------------------------------*/
void ParseAsHTML (Document doc, View view)
{
   char           *tempdocument = NULL;
   char            documentname[MAX_LENGTH];
   char            tempdir[MAX_LENGTH];
   int             i;

   if (!DocumentURLs[doc])
     /* the document is not loaded yet */
     return;
   if (!DocumentMeta[doc]->xmlformat)
     /* the document is not concerned by this option */
     return;
   if (!CanReplaceCurrentDocument (doc, view))
      /* abort the command */
      return;

   tempdocument = GetLocalPath (doc, DocumentURLs[doc]);
   TtaExtractName (tempdocument, tempdir, documentname);

   /* Initialize the LogFile variables */
   CleanUpParsingErrors ();

   /* remove the PARSING.ERR file */
   RemoveParsingErrors (doc);
   /* Remove the previous namespaces declaration */
   TtaFreeNamespaceDeclarations (doc);

   for (i = 1; i < DocumentTableLength; i++)
     if (DocumentURLs[i] != NULL && DocumentSource[i] == doc)
       {
	 DocumentSource[i] = 0;
	 if (DocumentTypes[i] == docLog)
	   {
	     /* close the window of the log file attached to the
		current document */
	     TtaCloseDocument (i);
	     TtaFreeMemory (DocumentURLs[i]);
	     DocumentURLs[i] = NULL;
	     /* switch off the button Show Log file */
	     TtaSetItemOff (doc, 1, Views, BShowLogFile);
	   }
       }

   /* Removes all CSS informations linked with the document */
   RemoveDocCSSs (doc);  
   /* Free access keys table */
   TtaRemoveDocAccessKeys (doc);
   DocumentMeta[doc]->xmlformat = FALSE;
   /* parse with the HTML parser */
   StartParser (doc, tempdocument, documentname, tempdir,
		tempdocument, FALSE);
   /* then request to save as XHTML */
   DocumentMeta[doc]->xmlformat = TRUE;
   /* fetch and display all images referred by the document */
   DocNetworkStatus[doc] = AMAYA_NET_ACTIVE;
   FetchAndDisplayImages (doc, AMAYA_LOAD_IMAGE, NULL);
   DocNetworkStatus[doc] = AMAYA_NET_INACTIVE;

   /* check parsing errors */
   CheckParsingErrors (doc);
   TtaFreeMemory (tempdocument);
}

/*----------------------------------------------------------------------
  LoadDocument parses the new document and stores its path (or
  URL) into the document table.
  For a local loading, the parameter tempfile must be an empty string.
  For a remote loading, the parameter tempfile gives the file name that
  contains the current copy of the remote file.
  ----------------------------------------------------------------------*/
static Document LoadDocument (Document doc, char *pathname,
			      char *form_data, char *initial_url,
			      int method, char *tempfile,
			      char *documentname, AHTHeaders *http_headers,
			      ThotBool history, ThotBool *inNewWindow)
{
  CSSInfoPtr          css;
  PInfoPtr            pInfo;
  Document            newdoc = 0;
  DocumentType        docType;
#ifdef ANNOTATIONS
  DocumentType        annotBodyType = docHTML;
#endif /* ANNOTATIONS */
  CHARSET             charset, httpcharset;
  CHARSET             metacharset = UNDEFINED_CHARSET;
  char                charsetname[MAX_LENGTH];
  char               *charEncoding;
  char               *tempdocument;
  char               *tempdir;
  char               *s;
  char               *content_type;
  char               *http_content_type;
  int                 i, j;
  int                 docProfile;
  DocumentType        thotType;
  char                local_content_type[MAX_LENGTH];
  ThotBool            unknown;
  ThotBool            contentImage, contentText, contentApplication;
  ThotBool            plainText;
  ThotBool            xmlDec, withDoctype, isXML, isknown;

  docType = docText;
  unknown = TRUE;
  tempdir = tempdocument = NULL;
  charsetname[0] = EOS;
  local_content_type[0] = EOS;
  http_content_type = HTTP_headers (http_headers, AM_HTTP_CONTENT_TYPE);
  /* make a copy we can modify */
  if (http_content_type)
    content_type = TtaStrdup (http_content_type);
  else
    content_type = NULL;

  /* Check informations within the document */
  if (tempfile[0] != EOS)
    CheckDocHeader (tempfile, &xmlDec, &withDoctype, &isXML, &isknown,
		    &docProfile, &charset, charsetname, &thotType);
  else
    CheckDocHeader (pathname, &xmlDec, &withDoctype, &isXML, &isknown,
		    &docProfile, &charset, charsetname, &thotType);

  /* if (charset == UNDEFINED_CHARSET && isXML && thotType == docHTML) */
  /* Check charset information in a meta */
  if (charset == UNDEFINED_CHARSET)
    {
      if (tempfile[0] != EOS)
	CheckCharsetInMeta (tempfile, &metacharset, charsetname);
      else
	CheckCharsetInMeta (pathname, &metacharset, charsetname);
    }

  if (method == CE_CSS)
    {
      /* we're loading a CSS file */
      docType = docCSS;
      unknown = FALSE;
    }

  if (content_type == NULL || content_type[0] == EOS)
    /* Local document - no content type  */
    {
      /* check file name extension */
      if (isXML)
	{
	  /* it seems to be a XML document */
	  if (DocumentTypes[doc] == docLibrary)
	    docType = docLibrary;
	  else
	    docType = thotType;
	  unknown = FALSE;
	}
     else if (IsCSSName (pathname))
	{
	  docType = docCSS;
	  docProfile = L_Other;
	  unknown = FALSE;
	}
     else if (IsTextName (pathname))
	{
	  docType = docText;
	  docProfile = L_Other;
	  unknown = FALSE;
	}
      else if (IsImageName (pathname))
	{
	  /* It's a local image file that we can display. We change the 
	     doctype flag so that we can create an HTML container later on */
	  docType = docImage;
	  unknown = FALSE;
	  docProfile = L_Transitional;
	}
     else if (IsMathMLName (pathname))
	{
	  docType = docMath;
	  docProfile = L_Other;
	  isXML = TRUE;
	  unknown = FALSE;
	}
     else if (IsSVGName (pathname))
	{
	  docType = docSVG;
	  docProfile = L_Other;
	  isXML = TRUE;
	  unknown = FALSE;
	}
#ifdef XML_GENERIC
      else if (IsXMLName (pathname))
	{
	  docType = docXml;
	  docProfile = L_Other;
	  isXML = TRUE;
	  unknown = FALSE;
	}
#endif /* XML_GENERIC */
#ifdef _SVG
      else if (IsLibraryName (pathname))
	{
	  docType = docLibrary;
	  unknown = FALSE;
	}
#endif /* _SVG */
      else if (docProfile != L_Other || IsHTMLName (pathname))
	{
	  /* it seems to be an HTML document */
	  docType = docHTML;
	  unknown = FALSE;
	}
      /* Assign a content type to that local document */
      if (docType == docCSS)
	strcpy (local_content_type , "text/css");
      else if (docType == docMath)
	strcpy (local_content_type , AM_MATHML_MIME_TYPE);
      else if (docType == docSVG)
	strcpy (local_content_type , AM_SVG_MIME_TYPE);
      else if (docType == docXml)
	strcpy (local_content_type , "text/xml");
      else if (docType == docText || docType == docCSS ||
	       docType == docSource || docType == docLog )
	strcpy (local_content_type , "text/plain");
      else if (docType == docHTML || docType == docLibrary)
	{
	  if (isXML && AM_UseXHTMLMimeType () )
	    strcpy (local_content_type , AM_XHTML_MIME_TYPE);
	  else
	    strcpy (local_content_type , "text/html");
	}
    }
   else
     /* The server returned a content type */
     {
       i = 0;
       while (content_type[i] != URL_SEP && content_type[i] != EOS)
	 i++;
       if (content_type[i] == URL_SEP)
	 {
	   content_type[i] = EOS;
	   j = i+1;
	   while (content_type[j] != ';' && content_type[j] != EOS)
	     j++;
	   if (content_type[j] == ';')
	     content_type[j] = EOS;
	   contentText = !strcasecmp (content_type, "text");
	   contentApplication = !strcasecmp (content_type, "application");
	   contentImage = !strcasecmp (content_type, "image");
	   if (contentText &&
	       !strncasecmp (&content_type[i+1], "html", 4))
	     {
	       if (thotType == docSVG)
		 {
		   /* ignore the mime type */
		   isXML = TRUE;
		   docType = thotType;
		   BADMimeType = TRUE;
		 }
	       else if (thotType == docMath)
		 {
		   /* ignore the mime type */
		   isXML = TRUE;
		   docType = thotType;
		   BADMimeType = TRUE;
		 }
	       else
		 {
		   /* it's an HTML document */
		   docType = docHTML;
		   if (docProfile == L_Other)
		     docProfile = L_Transitional;
		 }
	       unknown = FALSE;
	     }
	   else if (contentApplication &&
		    !strncasecmp (&content_type[i+1], "html", 4))
	     {
	       /* it's an HTML document */
	       docType = docHTML;
	       if (docProfile == L_Other)
		 docProfile = L_Transitional;
	       unknown = FALSE;
	     }
	   else if ((contentText || contentApplication) &&
		    (!strncasecmp (&content_type[i+1], "xhtml+xml", 9) ||
		     !strncasecmp (&content_type[i+1], "xhtml", 5)))
	     {
	       /* it's an xhtml document */
	       isXML = TRUE;
	       docType = docHTML;
	       if (docProfile == L_Other)
		 docProfile = L_Transitional;
	       unknown = FALSE;
	     }
	   else if ((contentText || contentApplication) &&
		    !strncasecmp (&content_type[i+1], "xml", 3) &&
		    content_type[i+1+3] == EOS)
	     {
	       /* Served as an XML document */
	       if (isXML && isknown &&
		   (thotType == docHTML ||
		    thotType == docSVG ||
		    thotType == docMath))
		 /* This type comes from the doctype or a namespace declaration */
		 docType = thotType;
	       else
		 {
#ifdef XML_GENERIC      
		   docType = docXml;
#else /* XML_GENERIC */
		   docType = docText;
#endif /* XML_GENERIC */
		   docProfile = L_Other;
		 }
	       isXML = TRUE;
	       unknown = FALSE;
	     }
	   else if (contentText &&
		    !strncasecmp (&content_type[i+1], "css", 3))
	     {
	       docType = docCSS;
	       docProfile = L_Other;
	       unknown = FALSE;
	     }
	   else if ((contentText || contentApplication) &&
		    (!strncasecmp (&content_type[i+1], "mathml", 6) ||
		     !strncasecmp (&content_type[i+1], "x-mathml", 8)))
	     {
	       /* it's an MathML document */
	       isXML = TRUE;
	       docType = docMath;
	       docProfile = L_MathML;
	       unknown = FALSE;
	     }
	   else if (contentText)
	     {
	       docType = docText;
	       docProfile = L_Other;
	       unknown = FALSE;
	     }
	   else if (contentApplication &&
		    !strncasecmp (&content_type[i+1], "x-sh", 4))
	     {
	       docType = docText;
	       docProfile = L_Other;
	       unknown = FALSE;
	     }	     
	   else if (contentApplication &&
		    !strncasecmp (&content_type[i+1], "xml-dtd", 7))
	     {
	       /* it's an DTD document */
	       docType = docText;
	       docProfile = L_Other;
	       unknown = FALSE;
	     }
	   else if (contentApplication &&
		    !strncasecmp (&content_type[i+1], "smil", 4))
	     {
	       /* it's a SMIL document. We handle it as an XML one */
	       isXML = TRUE;
#ifdef XML_GENERIC      
	       docType = docXml;
#else /* XML_GENERIC */
	       docType = docText;
#endif /* XML_GENERIC */
	       docProfile = L_Other;
	       unknown = FALSE;
	     }
	   else if (contentApplication &&
		    !strncasecmp (&content_type[i+1], "octet-stream", 12) &&
		    thotType == docSVG)
	     {
	       /* it's a SVG document. We handle it as an XML one */
	       isXML = TRUE;
	       docType = thotType;
	       unknown = FALSE;
	     }
	   else if (contentImage &&
		    !strncasecmp (&content_type[i+1], "svg", 3))
	     {
	       /* it's an XML document */
	       isXML = TRUE;
	       docType = docSVG;
	       docProfile = L_Other;
	       unknown = FALSE;
	     }
	   else if (contentImage)
	     {
	       /* we'll generate a HTML document */
	       if (IsImageType (&content_type[i+1]))
		 {
		   docType = docImage;
		   unknown = FALSE;
		   docProfile = L_Transitional;
		 }
	     }
	 }
     }
  
  if (unknown && tempfile[0] != EOS)
    {
      /* The document is not a supported format and cannot be parsed */
      /* rename the temporary file */
      strcpy (SavingFile, tempfile);
      SavingDocument = 0;
      SavingObject = 0;
      tempdocument = (char *)TtaGetMemory (MAX_LENGTH);
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
      if (method != CE_MAKEBOOK)
	{
	  /* do not register and open the document view */
	  if (DocumentURLs[doc])
	    {
	      /* save the URL of the old document in the history, if the same
		 window is reused, i.e. if the old document has not been
		 modified */
	      if (history && !TtaIsDocumentModified (doc))
		AddDocHistory (doc, DocumentURLs[doc], 
			       DocumentMeta[doc]->initial_url,
			       DocumentMeta[doc]->form_data,
			       DocumentMeta[doc]->method);
	    }

	  if (method == CE_RELATIVE)
	    {
	      if (TtaIsDocumentModified (doc) || docType == docCSS)
		{
		  /* open a new window to display the new document */
		  newdoc = InitDocAndView (0, documentname, docType, 0, FALSE,
					   docProfile, (ClickEvent)method);
		  ResetStop (doc);
		  /* clear the status line of previous document */
		  TtaSetStatus (doc, 1, " ", NULL);
		  ActiveTransfer (newdoc);
		}
	      else
		/* replace the current document by a new one */
		newdoc = InitDocAndView (doc, documentname, docType, 0, FALSE,
					 docProfile, (ClickEvent)method);
	    }
	  else if (method == CE_ABSOLUTE  || method == CE_HELP ||
		   method == CE_FORM_POST || method == CE_FORM_GET)
	    /* replace the current document by a new one */
	    newdoc = InitDocAndView (doc, documentname, docType, 0, FALSE,
				     docProfile, (ClickEvent)method);
#ifdef ANNOTATIONS
	  else if (method == CE_ANNOT) /*  && docType == docHTML) */
	    {
	      /* create the body */
	      ANNOT_CreateBodyTree (doc, docType);
	      /* and remember its type of the body */
	      annotBodyType = docType;
	      docType = docAnnot;
	      newdoc = doc;
	    }
#endif /* ANNOTATIONS */
	  else if (method == CE_LOG)
	    {
	      docType = docLog;
	      newdoc = doc;
	    }
	  else if (docType != DocumentTypes[doc] && DocumentTypes[doc] != docLibrary)
	    /* replace the current document by a new one */
	    newdoc = InitDocAndView (doc, documentname, docType, 0, FALSE,
				     docProfile, (ClickEvent)method);
	  else
	    {
	      /* document already initialized */
	      newdoc = doc;
	      /* store the profile of the new document */
	      /* and update the menus according to it */
	      TtaSetDocumentProfile (newdoc, docProfile);
	      TtaUpdateMenus (newdoc, 1, ReadOnlyDocument[newdoc]);
	    }
	}
      else
	newdoc = doc;

      if (docType == docImage)
      /* create an HTML container */
	{
	  if (content_type)
	    /* it's an image downloaded from the web */
	    CreateHTMLContainer (pathname, documentname, tempfile, FALSE);
	  else 
	    {
	      /* It's a local image file */
	      sprintf (tempfile, "%s%c%d%c%s", TempFileDirectory,
			DIR_SEP, newdoc, DIR_SEP, "contain.html");
	      CreateHTMLContainer (pathname, documentname, tempfile, TRUE);
	    }
	  ChangeToBrowserMode (doc);
	}

      /* what we have to do if doc and targetDocument are different */
      if (tempfile[0] != EOS)
	{
	  /* It is a document loaded from the Web */
	  if (!TtaFileExist (tempfile))
	    {
	      /* Nothing is loaded */
	      ResetStop (doc);
	      TtaFreeMemory (content_type);
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
	      /* if it's an IMAGEfile, we copy it too to the new directory */
	      if (DocumentTypes[newdoc] == docImage)
		MoveImageFile (doc, newdoc, documentname);
	    }
	  else if (DocumentTypes[newdoc] == docCSS)
	      TtaFileCopy (tempfile, tempdocument);
	    /* now we can rename the local name of a remote document */
	  else
	    /* now we can rename the local name of a remote document */
	    rename (tempfile, tempdocument);
	}
      else
	{
	  /* store a copy of the local document */
	  /* allocate and initialize tempdocument */
	  tempdocument = GetLocalPath (newdoc, pathname);
	  TtaFileCopy (pathname, tempdocument);
	}

      /* store a copy of CSS files in the directory 0 */
      if (DocumentTypes[newdoc] == docCSS)
	{ 
	  css = SearchCSS (0, pathname, NULL, &pInfo);
	  if (css == NULL)
	    {
	      /* store a copy of this new CSS context in .amaya/0 */
	      s = GetLocalPath (0, pathname);
	      TtaFileCopy (tempdocument, s);
	      /* initialize a new CSS context */
	      if (UserCSS && !strcmp (pathname, UserCSS))
		AddCSS (newdoc, 0, CSS_USER_STYLE, CSS_ALL, NULL, s, NULL);
	      else
		AddCSS (newdoc, 0, CSS_EXTERNAL_STYLE, CSS_ALL, pathname, s, NULL);
	      TtaFreeMemory (s);
	    }
	  else
	    css->doc = newdoc;
	}
      
      /* save the document name into the document table */
      s = TtaStrdup (pathname);
      if (DocumentURLs[newdoc] != NULL)
	{
	  TtaFreeMemory (DocumentURLs[newdoc]);
	  DocumentURLs[newdoc] = NULL;
	}
      /* if the document was already loaded, warn the user */
      if (IsDocumentLoaded (s, form_data))
	InitConfirm3L (newdoc, 1, TtaGetMessage (AMAYA, AM_DOUBLE_LOAD),
		       TtaGetMessage (AMAYA, AM_UNSAVE), NULL, FALSE);
      DocumentURLs[newdoc] = s;

      /* save the document's formdata into the document table */
      if (DocumentMeta[newdoc] != NULL)
	DocumentMetaClear (DocumentMeta[newdoc]);
      else
	DocumentMeta[newdoc] = DocumentMetaDataAlloc ();
      DocumentMeta[newdoc]->form_data = TtaStrdup (form_data);
      if (initial_url && strcmp (pathname, initial_url))
	DocumentMeta[newdoc]->initial_url = TtaStrdup (initial_url);
      else
	DocumentMeta[newdoc]->initial_url = NULL;
      DocumentMeta[newdoc]->method = (ClickEvent) method;
      DocumentSource[newdoc] = 0;
      DocumentMeta[newdoc]->xmlformat = isXML;

      /* Set character encoding */
      DocumentMeta[newdoc]->charset = NULL;
      charEncoding = HTTP_headers (http_headers, AM_HTTP_CHARSET);
      httpcharset = TtaGetCharset (charEncoding);

      if (httpcharset != UNDEFINED_CHARSET && charEncoding)
	{
	  TtaSetDocumentCharset (newdoc, httpcharset, FALSE);
	  DocumentMeta[newdoc]->charset = TtaStrdup (charEncoding);
	}
      else if (charset != UNDEFINED_CHARSET)
	{
	  TtaSetDocumentCharset (newdoc, charset, FALSE);
	  DocumentMeta[newdoc]->charset = TtaStrdup (charsetname);
	}
      else if (metacharset != UNDEFINED_CHARSET)
	{
	  TtaSetDocumentCharset (newdoc, metacharset, FALSE);
	  DocumentMeta[newdoc]->charset = TtaStrdup (charsetname);
	}
      else if (charsetname[0] != EOS)
	DocumentMeta[newdoc]->charset = TtaStrdup (charsetname);

      /*
      ** copy some HTTP headers to the metadata 
      */
      /* content-type */
      if (http_content_type)
	DocumentMeta[newdoc]->content_type = TtaStrdup (http_content_type);
      else
	if (local_content_type[0] != EOS)
	  /* assign a content type to the local files */
	  DocumentMeta[newdoc]->content_type = TtaStrdup (local_content_type);
	else
	  DocumentMeta[newdoc]->content_type = NULL;
      /* content-length */
      s = HTTP_headers (http_headers, AM_HTTP_CONTENT_LENGTH);
      if (s)
	DocumentMeta[newdoc]->content_length = TtaStrdup (s);
      else
	DocumentMeta[newdoc]->content_length = NULL;
      /* simplified content-location */
      s = HTTP_headers (http_headers, AM_HTTP_CONTENT_LOCATION);
      if (s)
	DocumentMeta[newdoc]->content_location = TtaStrdup (s);
      else
	DocumentMeta[newdoc]->content_location = NULL;
      /* full content-location */
      s = HTTP_headers (http_headers, AM_HTTP_FULL_CONTENT_LOCATION);
      if (s)
	DocumentMeta[newdoc]->full_content_location = TtaStrdup (s);
      else
	DocumentMeta[newdoc]->full_content_location = NULL;

      if (TtaGetViewFrame (newdoc, 1) != 0)
	/* this document is displayed */
	{
	  if (DocumentTypes[newdoc] == docLibrary)
	    {
#ifdef _SVG
	      SelectLibraryFromPath (DocumentURLs[newdoc]);
	      TtaSetTextZone (newdoc, 1, SVGlib_list);
#endif /* _SVG */
	    }
	  else
	    {
	      /* concatenate the URL and its form_data and then
		 display it on the amaya URL box */
	      i = strlen (pathname) + 5;
	      if (form_data && method != CE_FORM_POST)
		i += strlen (form_data);
	      s = (char *)TtaGetMemory (i);
	      if (form_data && method != CE_FORM_POST)
		sprintf (s, "%s?%s", pathname, form_data);
	      else
		strcpy (s, pathname);
	      /* add the URI in the combobox string
		 AddURLInCombobox (docname, FALSE);*/
	      TtaSetTextZone (newdoc, 1, URL_list);
	      TtaFreeMemory (s);
	    }
	}

      tempdir = (char *)TtaGetMemory (MAX_LENGTH);
      TtaExtractName (tempdocument, tempdir, documentname);
      /* Now we forget the method CE_INIT. It's a standard method */
      if (DocumentMeta[newdoc]->method == CE_INIT)
	DocumentMeta[newdoc]->method = CE_ABSOLUTE;

      if (docType == docHTML ||
	  docType == docSVG ||
#ifdef ANNOTATIONS
	  (docType == docAnnot && annotBodyType != docText) ||
#endif /* ANNOTATIONS */
	  docType == docXml ||
	  docType == docLibrary ||
	  docType == docMath)
	plainText = FALSE;
      else
	plainText = (docProfile == L_Other);
      
      /* Calls the corresponding parser */
      if (DocumentMeta[newdoc]->xmlformat && !plainText)
	StartXmlParser (newdoc,	tempdocument, documentname,
			tempdir, pathname, xmlDec, withDoctype);
      else
	StartParser (newdoc, tempdocument, documentname, tempdir,
		     pathname, plainText);
      
      TtaFreeMemory (tempdir);
   
      /* Update the Doctype menu */
      UpdateDoctypeMenu (newdoc);

      /* Set the document read-only when needed */
      if (ReadOnlyDocument[newdoc])
	SetDocumentReadOnly (newdoc);

      if (*inNewWindow || newdoc != doc)
	/* the document is displayed in a different window */
	/* reset the history of the new window */
	InitDocHistory (newdoc);
      /* the document is loaded now */
      *inNewWindow = FALSE;
      /* hide template entry if no template server is configured */
      if (TtaGetEnvString ("TEMPLATE_URL") == NULL)
 	TtaSetItemOff (newdoc, 1, File, BTemplate);
#ifdef ANNOTATIONS
      /* store the annotation body type (we have to wait until now as
	 the metadata didn't exist before) */
      if (docType == docAnnot)
	ANNOT_bodyType_set (newdoc, annotBodyType);
#endif /* ANNOTATIONS */
    }
  TtaFreeMemory (content_type);
  TtaFreeMemory (tempdocument);
  return (newdoc);
}

/*----------------------------------------------------------------------
  Reload_callback
  ----------------------------------------------------------------------*/
void Reload_callback (int doc, int status, char *urlName,
		      char *outputfile, AHTHeaders *http_headers,
		      void * context)
{
  Document          newdoc;
  char              *pathname;
  char              *tempfile;
  char              *documentname;
  char              *form_data;
  char              *initial_url, *ptr;
  ClickEvent         method;
  Document           res = 0;
  Element            el;
  RELOAD_context    *ctx;
  ThotBool           stopped_flag = FALSE, keep;

  /* restore the context associated with the request */
  ctx = (RELOAD_context *) context;
  documentname = ctx->documentname;
  newdoc = ctx->newdoc;
  form_data = ctx->form_data;
  method = ctx->method;

  if (!DocumentURLs[doc])
    /* the user has closed the corresponding document. Just free resources */
    {
      TtaFreeMemory (documentname);
      if (form_data)
	TtaFreeMemory (form_data);
      TtaFreeMemory (ctx);
      return;
    }

  tempfile = outputfile;

  pathname = (char *)TtaGetMemory (MAX_LENGTH);
  strcpy (pathname, urlName);
  
  if (status == 0)
     {
       TtaSetCursorWatch (0, 0);

       /* a bit of acrobatics so that we can retain the initial_url
	  without reallocating memory */
       initial_url = DocumentMeta[doc]->initial_url;
       DocumentMeta[doc]->initial_url = NULL;

       RemoveParsingErrors (newdoc);
       /* add the URI in the combobox string */
       if (method != CE_MAKEBOOK && method != CE_ANNOT &&
	   method != CE_LOG && method != CE_HELP &&
	   DocumentTypes[newdoc] != docLibrary &&
	   status == 0)
	 {
	   /* add the URI in the combobox string */
	   keep = (method == CE_ABSOLUTE || method == CE_INIT);
	   if (form_data && method == CE_FORM_GET)
	       AddURLInCombobox (pathname, form_data, keep);
	   else
	     AddURLInCombobox (pathname, NULL, keep);
	 }
       /* parse and display the document, res contains the new document
	  identifier, as given by the thotlib */
       res = LoadDocument (newdoc, pathname, form_data, NULL, method,
			   tempfile, documentname, http_headers, FALSE, &InNewWindow);
       if (!ReadOnlyDocument[doc])
	 UpdateEditorMenus (doc);

       if (res == 0)
	 {
	   /* cannot load the document */
	   ResetStop(newdoc);
	   newdoc = 0;
	   TtaFreeMemory (initial_url);
	 }
       else if (newdoc != res)
	 {
	   newdoc = res;
	   /* restore the initial_url */
	   DocumentMeta[newdoc]->initial_url = initial_url;
	 }
       
       if (newdoc)
	 {
	   W3Loading = 0;		/* loading is complete now */
#ifdef ANNOTATIONS
	   /* if it's an annotation, add the existing metadata */
	   if (DocumentTypes[newdoc] == docAnnot)
	     ANNOT_ReloadAnnotMeta (newdoc);
	   /* auto-load the annotations associated with the document */
	   if (ANNOT_CanAnnotate (newdoc))
	     ANNOT_AutoLoad (newdoc, 1);
#endif /* ANNOTATIONS */
	   TtaHandlePendingEvents ();
	   /* fetch and display all images referred by the document */
	   stopped_flag = FetchAndDisplayImages (newdoc, AMAYA_NOCACHE | AMAYA_LOAD_IMAGE, NULL);
	   if (stopped_flag == FALSE) 
	     {
	       TtaResetCursor (0, 0);
	       /* show the document at the same position as before Reload */
	       el = ElementAtPosition (newdoc, ctx->position);
	       TtaShowElement (newdoc, 1, el, ctx->distance);
	     }
	 }
     }

  if (stopped_flag == FALSE && newdoc)
    ResetStop(newdoc);

  /* check parsing errors */
  CheckParsingErrors (newdoc);

  if (DocumentTypes[newdoc] == docCSS)
    {
      /* reapply the CSS to relative documents */
      ptr = GetLocalPath (newdoc, DocumentURLs[newdoc]);
      UpdateStyleSheet (DocumentURLs[newdoc], ptr);
      TtaFreeMemory (ptr);
    }
#ifdef DAV
   /* MKP: if document has been loaded, we are       * 
    * able to discovery if the document is locked.   *
    * do a lock discovery, set LockIndicator button  */
   if (W3Loading == 0 && res> 0) 
    {
      DAVLockDiscovery (newdoc);
      DAVSetLockIndicator(newdoc);
    }
#endif  /* DAV */

  DocStatusUpdate (newdoc, FALSE);
  TtaFreeMemory (pathname);
  TtaFreeMemory (documentname);
  if (form_data)
    TtaFreeMemory (form_data);
  TtaFreeMemory (ctx);
}


/*----------------------------------------------------------------------
  Reload
  ----------------------------------------------------------------------*/
void Reload (Document doc, View view)
{
   char               *tempfile;
   char               *pathname;
   char               *documentname;
   char               *form_data;
   ClickEvent          method;
   RELOAD_context     *ctx;
   int                 toparse;
   int                 mode;
   int		       position;
   int		       distance;

   if (DocumentURLs[doc] == NULL)
      /* the document has not been loaded yet */
      return;
   if (DocumentTypes[doc] == docSource)
      /* don't reload a source document */
      return;
   /* abort all current exchanges concerning this document */
   StopTransfer (doc, 1);
   if (!CanReplaceCurrentDocument (doc, view))
      /* abort the command */
      return;
   /* reload the document */
   pathname = (char *)TtaGetMemory (MAX_LENGTH);
   documentname = (char *)TtaGetMemory (MAX_LENGTH);
   /* if the document is a template, restore the template script URL */
   if (DocumentMeta[doc]->method == CE_TEMPLATE)
      ReloadTemplateParams (&(DocumentURLs[doc]), &(DocumentMeta[doc]->method));
   NormalizeURL (DocumentURLs[doc], 0, pathname, documentname, NULL);

   if (!IsW3Path (pathname) && !TtaFileExist (pathname))
     {
       /* Free Memory */
       TtaFreeMemory (pathname);
       TtaFreeMemory (documentname);
       /* cannot reload this document */
       return;
     }

   if (DocumentMeta[doc]->form_data)
     form_data = TtaStrdup (DocumentMeta[doc]->form_data);
   else
     form_data = NULL;

   if (DocumentTypes[doc] == docAnnot)
     method = CE_ANNOT;
   else
     method = DocumentMeta[doc]->method;

   /* get the current position in the document */
   position = RelativePosition (doc, &distance);

   W3Loading = doc;	/* this document is currently in load */
   mode = AMAYA_ASYNC | AMAYA_NOCACHE | AMAYA_FLUSH_REQUEST;

   if (method == CE_FORM_POST)
     mode |= AMAYA_FORM_POST;

   tempfile = (char *)TtaGetMemory (MAX_LENGTH);
   tempfile[0] = EOS;
   toparse = 0;
   ActiveTransfer (doc);
   /* Create the context for the callback */

   ctx = (RELOAD_context*)TtaGetMemory (sizeof (RELOAD_context));
   ctx->newdoc = doc;
   ctx->documentname = documentname;
   ctx->form_data = form_data;
   ctx->method = method;
   ctx->position = position;
   ctx->distance = distance;

   if (IsW3Path (pathname))
     {
       /* load the document from the Web */
       toparse = GetObjectWWW (doc, 0, pathname, form_data, tempfile, 
			       mode,
			       NULL, NULL, (void (*)(int, int, char*, char*, const AHTHeaders*, void*)) Reload_callback, 
			       (void *) ctx, YES, NULL);
     }
   else if (TtaFileExist (pathname))
     Reload_callback (doc, 0, pathname, tempfile, NULL, (void *) ctx);

   TtaFreeMemory (tempfile);
   TtaFreeMemory (pathname);
   TtaHandlePendingEvents ();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ShowTargets (Document document, View view)
{
  int               visibility;
  View		    tocView;

  visibility = TtaGetSensibility (document, view);
  if (visibility == 4)
    {
      visibility = 5;
      TtaSetToggleItem (document, 1, Views, TShowTargets, FALSE);
    }
  else
    {
      visibility = 4;
      TtaSetToggleItem (document, 1, Views, TShowTargets, TRUE);
    }
  /* Change visibility threshold in the main view */
  TtaSetSensibility (document, view, visibility);
  /* Change visibility threshold in the table of content view if it's open */
  tocView = TtaGetViewFromName (document, "Table_of_contents");
  if (tocView && TtaIsViewOpen (document, tocView))
     TtaSetSensibility (document, tocView, visibility);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ZoomIn (Document document, View view)
{
  int               zoom, zoomVal;
  char             *zoomStr;

  zoom = TtaGetZoom (document, view);
  if (zoom < 10)
    {
      zoom++;
      TtaSetZoom (document, view, zoom);
    }

  /* compare to the standard value? */
  zoomStr = TtaGetEnvString ("ZOOM");
  if (zoomStr == NULL)
    zoomVal = 0;
  else
    {
      sscanf (zoomStr, "%d", &zoomVal);
      if (zoomVal > 10 || zoomVal < -10)
	zoomVal = 0;
    }
  if (zoom > zoomVal)
    TtaSetToggleItem (document, 1, Views, TZoomIn, TRUE);
  else
    TtaSetToggleItem (document, 1, Views, TZoomIn, FALSE);
  if (zoom < zoomVal)
    TtaSetToggleItem (document, 1, Views, TZoomOut, TRUE);
  else
    TtaSetToggleItem (document, 1, Views, TZoomOut, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ZoomOut (Document document, View view)
{
  int               zoom, zoomVal;
  char             *zoomStr;

  zoom = TtaGetZoom (document, view);
  if (zoom > -10)
    {
      zoom--;
      TtaSetZoom (document, view, zoom);
    }

  /* compare to the standard value? */
  zoomStr = TtaGetEnvString ("ZOOM");
  if (zoomStr == NULL)
    zoomVal = 0;
  else
    {
      sscanf (zoomStr, "%d", &zoomVal);
      if (zoomVal > 10 || zoomVal < -10)
	zoomVal = 0;
    }
  if (zoom > zoomVal)
    TtaSetToggleItem (document, 1, Views, TZoomIn, TRUE);
  else
    TtaSetToggleItem (document, 1, Views, TZoomIn, FALSE);
  if (zoom < zoomVal)
    TtaSetToggleItem (document, 1, Views, TZoomOut, TRUE);
  else
    TtaSetToggleItem (document, 1, Views, TZoomOut, FALSE);
}

/*----------------------------------------------------------------------
  ShowSource
  Display the source code of a document
  ----------------------------------------------------------------------*/
void ShowSource (Document document, View view)
{
   CHARSET          charset;
   char            *tempdocument;
   char            *s;
   char  	    documentname[MAX_LENGTH];
   char  	    tempdir[MAX_LENGTH];
   Document         sourceDoc;
   NotifyElement    event;

   if (!DocumentURLs[document])
     /* the document is not loaded yet */
#ifdef _SVG
     ShowSourceOfTimeline (document, view);
#else /* _SVG */
     return;
#endif /* _SVG */
   if (DocumentTypes[document] != docHTML &&
       DocumentTypes[document] != docSVG &&
       DocumentTypes[document] != docXml &&
       DocumentTypes[document] != docLibrary &&
       DocumentTypes[document] != docMath)
     /* it's not an HTML or an XML document */
     return;
   if (DocumentSource[document])
     /* the source code of this document is already shown */
     /* raise its window */
     TtaRaiseView (DocumentSource[document], 1);
   else
     {
     /* save the current state of the document into the temporary file */
     tempdocument = GetLocalPath (document, DocumentURLs[document]);
     if (TtaIsDocumentModified (document) || !TtaFileExist (tempdocument))
       {
	 SetNamespacesAndDTD (document);
	 if (DocumentTypes[document] == docLibrary ||
	     DocumentTypes[document] == docHTML)
	   {
	     if (TtaGetDocumentProfile (document) == L_Xhtml11)
	       TtaExportDocumentWithNewLineNumbers (document, tempdocument,
						    "HTMLT11");
	     else if (DocumentMeta[document]->xmlformat)
	       TtaExportDocumentWithNewLineNumbers (document, tempdocument,
						    "HTMLTX");
	     else
	       TtaExportDocumentWithNewLineNumbers (document, tempdocument,
						    "HTMLT");
	   }
	 else if (DocumentTypes[document] == docSVG)
	   TtaExportDocumentWithNewLineNumbers (document, tempdocument,
						"SVGT");
	 else if (DocumentTypes[document] == docMath)
	   TtaExportDocumentWithNewLineNumbers (document, tempdocument,
						"MathMLT");
#ifdef XML_GENERIC
	 else if (DocumentTypes[document] == docXml)
	   TtaExportDocumentWithNewLineNumbers (document, tempdocument, NULL);
#endif /* XML_GENERIC */
       }
     TtaExtractName (tempdocument, tempdir, documentname);
     /* open a window for the source code */
     sourceDoc = InitDocAndView (0, documentname, (DocumentType)docSource, document, FALSE,
				 L_Other, (ClickEvent)CE_ABSOLUTE);   
     if (sourceDoc > 0)
       {
	 DocumentSource[document] = sourceDoc;
	 s = TtaStrdup (DocumentURLs[document]);
	 DocumentURLs[sourceDoc] = s;
	 DocumentMeta[sourceDoc] = DocumentMetaDataAlloc ();
	 DocumentMeta[sourceDoc]->form_data = NULL;
	 DocumentMeta[sourceDoc]->initial_url = NULL;
	 DocumentMeta[sourceDoc]->method = CE_ABSOLUTE;
	 DocumentMeta[sourceDoc]->xmlformat = FALSE;
	 /* copy the MIME type, charset, and content location */
	 if (DocumentMeta[document]->content_type)
	   DocumentMeta[sourceDoc]->content_type = TtaStrdup (DocumentMeta[document]->content_type);
	 if (DocumentMeta[document]->charset)
	   DocumentMeta[sourceDoc]->charset = TtaStrdup (DocumentMeta[document]->charset);
	 if (DocumentMeta[document]->content_location)
	   DocumentMeta[sourceDoc]->content_location = TtaStrdup (DocumentMeta[document]->content_location);
	 if (DocumentMeta[document]->full_content_location)
	   DocumentMeta[sourceDoc]->full_content_location = TtaStrdup (DocumentMeta[document]->full_content_location);
	 DocumentTypes[sourceDoc] = docSource;
	 charset = TtaGetDocumentCharset (document);
	 if (charset == UNDEFINED_CHARSET)
	   {
	     if (DocumentMeta[document]->xmlformat)
	       TtaSetDocumentCharset (SavingDocument, UTF_8, FALSE);
	     else
	       TtaSetDocumentCharset (SavingDocument, ISO_8859_1, FALSE);
	   }
	 else
	   TtaSetDocumentCharset (sourceDoc, charset, FALSE);
	 DocNetworkStatus[sourceDoc] = AMAYA_NET_INACTIVE;
	 StartParser (sourceDoc, tempdocument, documentname, tempdir,
		      tempdocument, TRUE);
	 SetWindowTitle (document, sourceDoc, 0);
	 /* Set the document read-only when needed */
	 if (ReadOnlyDocument[document])
	   {
	     ReadOnlyDocument[sourceDoc] = TRUE;
	     SetDocumentReadOnly (sourceDoc);
	   }
	 TtcSwitchButtonBar (sourceDoc, 1); /* no button bar */
	 TtcSwitchCommands (sourceDoc, 1); /* no command open */
	 TtaSetItemOff (sourceDoc, 1, File, New1);
	 TtaSetItemOff (sourceDoc, 1, File, BHtmlBasic);
	 TtaSetItemOff (sourceDoc, 1, File, BHtmlStrict);
	 TtaSetItemOff (sourceDoc, 1, File, BHtml11);
	 TtaSetItemOff (sourceDoc, 1, File, BHtmlTransitional);
	 TtaSetItemOff (sourceDoc, 1, File, BMathml);
	 TtaSetItemOff (sourceDoc, 1, File, BSvg);
	 TtaSetItemOff (sourceDoc, 1, File, BTemplate);
	 TtaSetItemOff (sourceDoc, 1, File, BCss);
	 TtaSetItemOff (sourceDoc, 1, File, BOpenDoc);
	 TtaSetItemOff (sourceDoc, 1, File, BOpenInNewWindow);
	 TtaSetItemOff (sourceDoc, 1, File, BReload);
	 TtaSetItemOff (sourceDoc, 1, Edit_, BSpellCheck);
	 TtaSetItemOff (sourceDoc, 1, Edit_, BTransform);
	 TtaSetItemOff (sourceDoc, 1, Views, TShowButtonbar);
	 TtaSetItemOff (sourceDoc, 1, Views, TShowTextZone);
	 TtaSetMenuOff (sourceDoc, 1, Special);
	 TtaSetMenuOff (sourceDoc, 1, Help_);
	 /* Update the doctype menu */
	 UpdateDoctypeMenu (sourceDoc);
#ifdef ANNOTATIONS
	 TtaSetMenuOff (sourceDoc, 1, Annotations_);
#endif /* ANNOTATIONS */

    	 /* Switch the synchronization entry */
    	 if (TtaIsDocumentModified (document))
    	    DocStatusUpdate (document, TRUE);
	 /* Synchronize selections */
	 event.document = document;
	 SynchronizeSourceView (&event);
       }
     TtaFreeMemory (tempdocument);
     }
}

/*----------------------------------------------------------------------
  ShowStructure
  Open the structure view(s) of a document.
  ----------------------------------------------------------------------*/
void ShowStructure (Document document, View view)
{
   View                structView;
   int                 x, y, w, h;
   char                structureName[30];

   strcpy (structureName, "Structure_view");  
   structView = TtaGetViewFromName (document, structureName);
   if (structView != 0 && TtaIsViewOpen (document, structView))
     TtaRaiseView (document, structView);
   else
     {
       TtaGetViewGeometry (document, structureName, &x, &y, &w, &h);
       structView = TtaOpenView (document, structureName, x, y, w, h);
       
       if (structView != 0)
	 {
	   TtcSwitchButtonBar (document, structView); /* no button bar */
	   TtcSwitchCommands (document, structView); /* no command open */
	   if (ReadOnlyDocument[document])
	     {
	       TtaSetItemOff (document, structView, Edit_, BCut);
	       TtaSetItemOff (document, structView, Edit_, BPaste);
	       TtaSetItemOff (document, structView, Edit_, BClear);
	       TtaSetItemOff (document, structView, Edit_, BSpellCheck);
	       TtaSetItemOff (document, structView, Edit_, BTransform);
	       TtaSetMenuOff (document, structView, StructTypes);
	       TtaSetMenuOff (document, structView, Types);
	       TtaSetMenuOff (document, structView, Attributes_);
	     }
#ifdef XML_GENERIC      
	   if (DocumentTypes[document] == docXml)
	     {
	       TtaSetMenuOff (document, structView, StructTypes);
	       TtaSetMenuOff (document, structView, Types);
	       TtaSetItemOff (document, structView, Edit_, BTransform);
	     }
#endif /* XML_GENERIC */
	 }
     }
   SetWindowTitle (document, document, 0);
}

/*----------------------------------------------------------------------
  ShowAlternate
  Open the Alternate view of a document.
  ----------------------------------------------------------------------*/
void ShowAlternate (Document document, View view)
{
   View                altView;
   int                 x, y, w, h;

   altView = TtaGetViewFromName (document, "Alternate_view");
   if (view == altView)
      TtaRaiseView (document, view);
   else if (altView != 0 && TtaIsViewOpen (document, altView))
      TtaRaiseView (document, altView);
   else
     {
	TtaGetViewGeometry (document, "Alternate_view", &x, &y, &w, &h);
	altView = TtaOpenView (document, "Alternate_view", x, y, w, h);
	if (altView != 0)
	  {
	    SetWindowTitle (document, document, altView);
	    TtcSwitchButtonBar (document, altView); /* no button bar */
	    TtcSwitchCommands (document, altView); /* no command open */
	    if (ReadOnlyDocument[document])
	      {
		TtaSetItemOff (document, altView, Edit_, BCut);
		TtaSetItemOff (document, altView, Edit_, BPaste);
		TtaSetItemOff (document, altView, Edit_, BClear);
		TtaSetItemOff (document, altView, Edit_, BSpellCheck);
		TtaSetItemOff (document, altView, Edit_, BTransform);
		TtaSetMenuOff (document, altView, Types);
		TtaSetMenuOff (document, altView, Attributes_);
	      }
	  }
     }
}


/*----------------------------------------------------------------------
  ShowLinks
  Open the Links view of a document
  ----------------------------------------------------------------------*/
void ShowLinks (Document document, View view)
{
   View                linksView;
   int                 x, y, w, h;

   linksView = TtaGetViewFromName (document, "Links_view");
   if (view == linksView)
      TtaRaiseView (document, view);
   else if (linksView != 0 && TtaIsViewOpen (document, linksView))
      TtaRaiseView (document, linksView);
   else
     {
	TtaGetViewGeometry (document, "Links_view", &x, &y, &w, &h);
	linksView = TtaOpenView (document, "Links_view", x, y, w, h);
	if (linksView != 0)
	  {
	    SetWindowTitle (document, document, linksView);
	    TtcSwitchButtonBar (document, linksView); /* no button bar */
	    TtcSwitchCommands (document, linksView); /* no command open */
	    if (ReadOnlyDocument[document])
	      {
		TtaSetItemOff (document, linksView, Edit_, BCut);
		TtaSetItemOff (document, linksView, Edit_, BPaste);
		TtaSetItemOff (document, linksView, Edit_, BClear);
		TtaSetItemOff (document, linksView, Edit_, BSpellCheck);
		TtaSetItemOff (document, linksView, Edit_, BTransform);
		TtaSetMenuOff (document, linksView, Types);
		TtaSetMenuOff (document, linksView, Attributes_);
	      }
#ifdef XML_GENERIC      
	    if (DocumentTypes[document] == docXml)
	      {
		TtaSetMenuOff (document, linksView, Types);
		TtaSetItemOff (document, linksView, Edit_, BTransform);
	      }
#endif /* XML_GENERIC */
	  }
     }
}


/*----------------------------------------------------------------------
  ShowToC
  Open the Table of content view of a document
  ----------------------------------------------------------------------*/
void ShowToC (Document document, View view)
{
   View                tocView;
   int                 x, y, w, h;

   tocView = TtaGetViewFromName (document, "Table_of_contents");
   if (view == tocView)
      TtaRaiseView (document, view);
   else if (tocView != 0 && TtaIsViewOpen (document, tocView))
      TtaRaiseView (document, tocView);
   else
     {
	TtaGetViewGeometry (document, "Table_of_contents", &x, &y, &w, &h);
	tocView = TtaOpenView (document, "Table_of_contents", x, y, w, h);
	if (tocView != 0)
	  {
	    SetWindowTitle (document, document, tocView);
	    TtcSwitchButtonBar (document, tocView); /* no button bar */
	    TtcSwitchCommands (document, tocView); /* no command open */
	    if (ReadOnlyDocument[document])
	      {
		TtaSetItemOff (document, tocView, Edit_, BCut);
		TtaSetItemOff (document, tocView, Edit_, BPaste);
		TtaSetItemOff (document, tocView, Edit_, BClear);
		TtaSetItemOff (document, tocView, Edit_, BSpellCheck);
		TtaSetItemOff (document, tocView, Edit_, BTransform);
		TtaSetMenuOff (document, tocView, Types);
		TtaSetMenuOff (document, tocView, Attributes_);
	      }
	  }
     }
}


/*----------------------------------------------------------------------
   ViewToClose                                                      
  ----------------------------------------------------------------------*/
ThotBool ViewToClose (NotifyDialog *event)
{
   Document      document;
   View          view, structView, altView, linksView, tocView;

   view = event->view;
   document = event->document;
   structView = TtaGetViewFromName (document, "Structure_view");
   altView = TtaGetViewFromName (document, "Alternate_view");
   linksView = TtaGetViewFromName (document, "Links_view");
   tocView = TtaGetViewFromName (document, "Table_of_contents");
   if (view != 1)
     /* let Thot perform normal operation */
     return FALSE;
   else if (DocumentTypes[document] == docLibrary)
     {
       if (TtaIsDocumentModified (document))
	 {
	   InitConfirm (document, view, TtaGetMessage (AMAYA, AM_SAVE_DISK));
	   if (UserAnswer)
	     SaveDocument (document, view);
	   TtaSetDocumentUnmodified (document);
	 }
     }
   else if (!CanReplaceCurrentDocument (document, view))
     /* abort the command and don't let Thot perform normal operation */
     return TRUE;

   if (structView != 0 && TtaIsViewOpen (document, structView))
     TtaCloseView (document, structView);
   if (altView != 0 && TtaIsViewOpen (document, altView))
     TtaCloseView (document, altView);
   if (linksView != 0 && TtaIsViewOpen (document, linksView))
     TtaCloseView (document, linksView);
   if (tocView != 0 && TtaIsViewOpen (document, tocView))
     TtaCloseView (document, tocView);
   /* let Thot perform normal operation */
   return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void GetAmayaDoc_callback (int newdoc, int status, char *urlName,
			       char *outputfile, AHTHeaders *http_headers,
			       void * context)
{
  Element             elFound;
  Document            doc;
  Document            res;
  GETHTMLDocument_context *ctx;
  TTcbf              *cbf;
  ClickEvent         method;
  void               *ctx_cbf;
  char                tempdocument[MAX_LENGTH];
  char               *tempfile;
  char               *target;
  char               *pathname;
  char               *initial_url;
  char               *documentname;
  char               *form_data;
  char               *s;
  int                 i;
  ThotBool            inNewWindow;
  ThotBool            ok, keep;
  ThotBool            stopped_flag = FALSE;
  ThotBool            local_link;

  /* restore GETHTMLDocument's context */  
  ctx = (GETHTMLDocument_context *) context;
  if (!ctx)
    return;
  doc = ctx->doc;
  target = ctx->target;
  documentname = ctx->documentname;
  initial_url = ctx->initial_url;
  form_data = ctx->form_data;
  cbf = ctx->cbf;
  ctx_cbf = ctx->ctx_cbf;
  method = ctx->method;
  local_link = ctx->local_link;
  inNewWindow = ctx->inNewWindow;
  ok = TRUE;
  pathname = (char *)TtaGetMemory (MAX_LENGTH + 1);
  strncpy (pathname, urlName, MAX_LENGTH);
  pathname[MAX_LENGTH] = EOS;
  tempfile = (char *)TtaGetMemory (MAX_LENGTH + 1);
  if (method != CE_MAKEBOOK && method != CE_ANNOT &&
      method != CE_LOG && method != CE_HELP &&
      DocumentTypes[newdoc] != docLibrary &&
      status == 0)
    {
      /* add the URI in the combobox string */
      keep = (method == CE_ABSOLUTE || method == CE_INIT);
      if (form_data && method == CE_FORM_GET)
	  AddURLInCombobox (pathname, form_data, keep);
      else
	AddURLInCombobox (pathname, NULL, keep);
    }
  if (outputfile != NULL)
     {
       strncpy (tempfile, outputfile, MAX_LENGTH);
       tempfile[MAX_LENGTH] = EOS;
     }
   else
     tempfile[0] = EOS;
   
  /* now the new window is open */
  if (inNewWindow && (method == CE_RELATIVE || method == CE_ABSOLUTE))
    /* don't free the current loaded document */
    method = CE_INIT;
   if (!local_link)
     {
       /* memorize the initial newdoc value in doc because LoadDocument */
       /* will open a new document if newdoc is a modified document */
       if (status == 0)
	 {
	   /* the document was successfully loaded */
	   if (IsW3Path (pathname))
	     NormalizeURL (pathname, 0, tempdocument, documentname, NULL);
	   /* parse and display the document */
	   res = LoadDocument (newdoc, pathname, form_data, 
			       initial_url, method,
			       tempfile, documentname,
			       http_headers, ctx->history, &inNewWindow);
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
	       if (method == CE_MAKEBOOK)
		 stopped_flag = FetchAndDisplayImages (newdoc,
						       AMAYA_LOAD_IMAGE | AMAYA_MBOOK_IMAGE,
						       NULL);
	       else
		 {
		   stopped_flag = FetchAndDisplayImages (newdoc, AMAYA_LOAD_IMAGE, NULL);
#ifdef ANNOTATIONS
		   /* if it's an annotation, add the existing metadata */
		   if (DocumentTypes[newdoc] == docAnnot)
		     ANNOT_LoadAnnotation (ctx->baseDoc, newdoc);
		   /* auto-load the annotations associated with the document */
		   if (ANNOT_CanAnnotate (newdoc))
		       ANNOT_AutoLoad (newdoc, 1);
#endif /* ANNOTATIONS */
		 }
	     }
	   /* check parsing errors */
	   if (DocumentTypes[newdoc] == docLog || method == CE_MAKEBOOK)
	     CleanUpParsingErrors ();
	   else
	     CheckParsingErrors (newdoc);
	 }
       else
	 {
	   /* a stop or an error occured */
	   if (DocumentURLs[newdoc] == NULL)
	     {
	       /* save the document name into the document table */
	       s = TtaStrdup (pathname);
	       DocumentURLs[newdoc] = s;
	       if (DocumentTypes[newdoc] == docLibrary)
		 {
#ifdef _SVG
		   SelectLibraryFromPath (DocumentURLs[newdoc]);
		   TtaSetTextZone (newdoc, 1, SVGlib_list);
#endif /* _SVG */
		 }
	       else
		 TtaSetTextZone (newdoc, 1, URL_list);
	       /* save the document's formdata into the document table */
	       if (DocumentMeta[newdoc])
		   DocumentMetaClear (DocumentMeta[(int) newdoc]);
	       else
		 DocumentMeta[newdoc] = DocumentMetaDataAlloc ();
	       DocumentMeta[newdoc]->form_data = TtaStrdup (form_data);
	       if (strcmp (pathname, initial_url))
		 DocumentMeta[newdoc]->initial_url = TtaStrdup (initial_url);
	       else
		 DocumentMeta[newdoc]->initial_url = NULL;
	       DocumentMeta[newdoc]->method = method;
	       DocumentMeta[newdoc]->xmlformat = FALSE;
	       DocumentSource[newdoc] = 0;
	       ResetStop(newdoc);
	     }
	   W3Loading = 0;	/* loading is complete now */
	   ResetStop(newdoc);
	   ok = FALSE;
	   if (status == -2)
	     {
	       s = HTTP_headers (http_headers, AM_HTTP_REASON);
	       if (!s)
		 s = "";
	       sprintf (tempdocument, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), pathname);
	       InitConfirm3L (newdoc, 1, tempdocument, s, NULL, FALSE);
	     }
	 }

       if (ok && !stopped_flag)
	 ResetStop(newdoc);
     }

   /* select the target if present */
   if (ok && !stopped_flag && target != NULL && target[0] != EOS &&
       newdoc != 0)
     {
       /* attribute HREF contains the NAME of a target anchor */
       elFound = SearchNAMEattribute (newdoc, target, NULL, NULL);
       if (elFound != NULL)
	 {
	   /* show the target element in all views */
	   for (i = 1; i < 4; i++)
	     if (TtaIsViewOpen (newdoc, i))
	       TtaShowElement (newdoc, i, elFound, 0);
	 }
     }

   /*** if stopped_flag == true, how to deal with cbf? */
   if (cbf)
     (*cbf) (newdoc, status, pathname, tempfile, NULL, ctx_cbf);


#ifdef DAV
   /* MKP: if document has been loaded,  we are     * 
    * able to discovery if the document is locked.  *
    * do a lock discovery, set LockIndicator button */
   if (W3Loading == 0) 
    {
      DAVLockDiscovery (newdoc);
      DAVSetLockIndicator(newdoc);
    }
#endif       

   TtaFreeMemory (target);
   TtaFreeMemory (documentname);
   TtaFreeMemory (initial_url);
   TtaFreeMemory (pathname);
   TtaFreeMemory (tempfile);
   if (form_data)
     TtaFreeMemory (form_data);
   TtaFreeMemory (ctx);
}

/*----------------------------------------------------------------------
  GetAmayaDoc loads the document if it is not loaded yet and calls the
  parser if the document can be parsed.
    - documentPath: can be relative or absolute address.
    - form_data: the text to be posted.
    - doc: the document which can be removed if not updated.
    - baseDoc: the document which documentPath is relative to.
    - method: CE_FORM_POST for a post request, CE_RELATIVE for a double 
      click.
    - history: record the URL in the browsing history
  ----------------------------------------------------------------------*/
Document GetAmayaDoc (char *documentPath, char *form_data,
		      Document doc, Document baseDoc, ClickEvent method,
		      ThotBool history, TTcbf *cbf, void *ctx_cbf,
		      CHARSET charset)
{
  Document            newdoc, refdoc;
  CSSInfoPtr          css;
  PInfoPtr            pInfo;
  GETHTMLDocument_context *ctx = NULL;
  char               *tempfile;
  char               *tempdocument;
  char               *parameters;
  char               *target;
  char               *pathname;
  char               *documentname;
  char               *content_type = NULL;
  int                 toparse;
  int                 mode;
  int                 docType;
  ThotBool            ok;

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
  target       = (char *)TtaGetMemory (MAX_LENGTH);
  documentname = (char *)TtaGetMemory (MAX_LENGTH);
  parameters   = (char *)TtaGetMemory (MAX_LENGTH);
  tempfile     = (char *)TtaGetMemory (MAX_LENGTH);
  tempfile[0]  = EOS;
  pathname     = (char *)TtaGetMemory (MAX_LENGTH);
  /* Store DocumentURLs and DocHistory in UTF-8 */
  tempdocument = (char *)TtaConvertByteToMbs ((unsigned char *)documentPath, charset);
  ExtractParameters (tempdocument, parameters);
  /* Extract the target if necessary */
  ExtractTarget (tempdocument, target);
  /* Add the  base content if necessary */
  if (method == CE_RELATIVE || method == CE_FORM_GET ||
      method == CE_FORM_POST || method == CE_MAKEBOOK)
    NormalizeURL (tempdocument, baseDoc, pathname, documentname, NULL);
  else
     NormalizeURL (tempdocument, 0, pathname, documentname, NULL);
  /* check the document suffix */
  if (IsMathMLName (documentname))
    docType = docMath;
  else if (IsSVGName (documentname))
    docType = docSVG;
  else if (IsCSSName (documentname))
    docType = docCSS;
  else if (IsTextName (documentname))
     docType = docText;
#ifdef XML_GENERIC
  else if (IsXMLName (documentname))
    docType = docXml;
#endif /* XML_GENERIC */
#ifdef _SVG
  else if (IsLibraryName (documentname))
    docType = docLibrary;
#endif /* _SVG */
  else if (method == CE_CSS)
    docType = docCSS;
  else
    docType = docHTML;
  
   /* we skip the file: prefix if it is present and do other local
      file urls conversions */
  if (!IsW3Path (pathname))
    {
      /* we take the long way around to get the result
	 of normalizeFile, as the function doesn't allocate
	 memory dynamically (note: this can generate some MAX_LENGTH
	 problems) */
      if (method == CE_RELATIVE || method == CE_FORM_GET ||
	  method == CE_ANNOT || method == CE_FORM_POST ||
	  method == CE_MAKEBOOK)
	/* we're following a link, so do all the convertions on
	   the URL */
	NormalizeFile (pathname, tempfile, AM_CONV_ALL);
      else
	NormalizeFile (pathname, tempfile, AM_CONV_NONE);
      strcpy (pathname, tempfile);
      tempfile[0] = EOS;
    }

  /* check if the user is already browsing the document in another window */
  if (method == CE_FORM_GET)
    {
      newdoc = IsDocumentLoaded (pathname, form_data);
      /* we don't concatenate the new parameters as we give preference
	 to the form data */
    }
  else
    {
      /* concatenate the parameters before making the test */
      if (parameters[0] != EOS)
	{
	  strcat (pathname, "?");
	  strcat (pathname, parameters);
	}
      /* if it's a POST form, we search the document using the
	 form_data */
      if (method == CE_FORM_POST)
	newdoc = IsDocumentLoaded (pathname, form_data);
      else
	newdoc = IsDocumentLoaded (pathname, NULL);
    }

  if (newdoc != 0)
    /* the document is already loaded */
    {
      if (newdoc == doc)
	/* it's a move in the same document */
	{
	  if (history)
	     /* record the current position in the history */
	    AddDocHistory (newdoc, DocumentURLs[newdoc], 
			   DocumentMeta[doc]->initial_url,
			   DocumentMeta[doc]->form_data,
			   DocumentMeta[doc]->method);
	}
      else
	/* following the link to another open window */
	{
	  /* raise its window */
	  TtaRaiseView (newdoc, 1);
	  /* don't add it to the doc's historic */
	  history = FALSE;
	}
    }

  /* Create the context for the callback */
  ctx = (GETHTMLDocument_context*)TtaGetMemory (sizeof (GETHTMLDocument_context));
  ctx->doc = doc;
  ctx->baseDoc = baseDoc;
  ctx->history = history;
  ctx->target = target;
  ctx->documentname = documentname;
  ctx->initial_url = TtaStrdup (pathname);
  if (form_data)
    ctx->form_data = TtaStrdup (form_data);
  else
    ctx->form_data = NULL;
  ctx->method = method;
  ctx->cbf = cbf;
  ctx->ctx_cbf = ctx_cbf;
  ctx->local_link = 0;
  ctx->inNewWindow = InNewWindow;

  toparse = 0;
  if (newdoc == 0)
    {
      /* document not loaded yet */
      if ((method == CE_RELATIVE || method == CE_FORM_GET ||
	   method == CE_FORM_POST || method == CE_MAKEBOOK ||
	   method == CE_ANNOT) &&
	  !IsW3Path (pathname) && !TtaFileExist (pathname))
	{
	  /* the target document doesn't exist */
	  TtaSetStatus (baseDoc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), pathname);
	  ok = FALSE; /* do not continue */
	}
      else if (method == CE_LOG)
	/* need to create a new window for the document */
	newdoc = InitDocAndView (doc, documentname, (DocumentType)docLog, 0, FALSE,
				 L_Other, (ClickEvent)method);
      else if (method == CE_HELP)
	{
	  /* add the URI in the combobox string */
	  AddURLInCombobox (pathname, NULL, FALSE);
	  /* need to create a new window for the document */
	  newdoc = InitDocAndView (doc, documentname, (DocumentType)docType, 0, TRUE,
				   L_Other, (ClickEvent)method);
	  if (newdoc)
	    {
	      /* help document has to be in read-only mode */
	      TtcSwitchCommands (newdoc, 1); /* no command open */
	      TtaSetToggleItem (newdoc, 1, Views, TShowTextZone, FALSE);
	      TtaSetMenuOff (newdoc, 1, Help_);
	    }
	}
#ifdef ANNOTATIONS
      else if (method == CE_ANNOT)
	{
	  /* need to create a new window for the document */
	  newdoc = InitDocAndView (doc, documentname, (DocumentType)docAnnot, 0, FALSE,
				   L_Other, (ClickEvent)method);
	  /* we're downloading an annotation, fix the accept_header
	     (thru the content_type variable) to application/rdf */
	  content_type = "application/rdf";
	}
#endif /* ANNOTATIONS */
      else if (doc == 0 || InNewWindow)
	{
	  /* In case of initial document, open the view before loading */
	  /* add the URI in the combobox string */
	  AddURLInCombobox (pathname, NULL, FALSE);
	  newdoc = InitDocAndView (0, documentname, (DocumentType)docType, 0, FALSE,
				   L_Other, (ClickEvent)method);
	}
      else
	{
	  newdoc = doc;
	  /* stop current transfer for previous document */
	  if (method != CE_MAKEBOOK)
	    StopTransfer (baseDoc, 1);
	  else
	    /* temporary docs to make a book are not in ReadOnly mode */
	    DocumentTypes[newdoc] = docHTML;
	}
      
      if (newdoc == 0)
	/* cannot display the new document */
	ok = FALSE;
      if (ok)
	{
	  /* this document is currently in load */
	  W3Loading = newdoc;
	  ActiveTransfer (newdoc);
	  /* set up the transfer mode */
	  mode = AMAYA_ASYNC | AMAYA_FLUSH_REQUEST;

	  if (method == CE_FORM_POST)
	    mode = mode | AMAYA_FORM_POST | AMAYA_NOCACHE;
	  else if (method == CE_MAKEBOOK)
	    mode = AMAYA_ASYNC;

	  if (IsW3Path (pathname))
	    {
	      css = SearchCSS (0, pathname, NULL, &pInfo);
	      if (method == CE_MAKEBOOK || method == CE_RELATIVE)
		/* add the referer field in the GET */
		refdoc = doc;
	      else
		refdoc = 0;
	      if ((css == NULL) || (css != NULL && newdoc == doc))
		toparse =  GetObjectWWW (newdoc, refdoc, pathname, form_data,
					 tempfile, mode, NULL, NULL,
					 (void (*)(int, int, char*, char*, const AHTHeaders*, void*)) GetAmayaDoc_callback,
					 (void *) ctx, YES, content_type);
	      else
		{
		  /* it was already loaded, we need to open it */
		  TtaSetStatus (newdoc, 1,
				TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED), NULL);
		  /* just take a copy of the local temporary file */
		  strcpy (tempfile, css->localName);
		  GetAmayaDoc_callback (newdoc, 0, pathname,
					tempfile, NULL, (void *) ctx);
		  TtaHandlePendingEvents ();
		}
	    }
	  else
	    {
	      /* wasn't a document off the web, we need to open it */
	      TtaSetStatus (newdoc, 1,
			    TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED),
			    NULL);
	      GetAmayaDoc_callback (newdoc, 0, pathname, tempfile,
				    NULL, (void *) ctx);
	      TtaHandlePendingEvents ();
	    }
	}
    }
  else if (ok && newdoc != 0)
    {
      /* following a local link */
      TtaSetStatus (newdoc, 1, TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED), NULL);
      ctx->local_link = 1;
      GetAmayaDoc_callback (newdoc, 0, pathname, tempfile, NULL, (void *) ctx);
      TtaHandlePendingEvents ();
    }
  
  if (ok == FALSE)
    /* if the document isn't loaded off the web (because of an error, or 
       because it was already loaded), we invoke the callback function */
    {
      if (ctx->form_data)
	TtaFreeMemory (ctx->form_data);
      TtaFreeMemory (ctx->initial_url);
      if (ctx)
	TtaFreeMemory (ctx);
      if (cbf)
	(*cbf) (newdoc, -1, pathname, tempfile, NULL, ctx_cbf);
      /* Free the memory associated with the context */
      TtaFreeMemory (target);
      TtaFreeMemory (documentname);
    }
  TtaFreeMemory (parameters);
  TtaFreeMemory (tempfile);
  TtaFreeMemory (pathname);
  TtaFreeMemory (tempdocument);
  InNewWindow = FALSE;
  return (newdoc);
}

/*----------------------------------------------------------------------
  ChangeDoctype
  Mofity the doctype declaration for a HTML document saved as XML
  and vice-versa.
  isXml indicates is the new doctype corresponds to an XML document
  ----------------------------------------------------------------------*/
static void ChangeDoctype (ThotBool isXml)
{
  int          profile;
  Document     doc;
  ElementType  elType;
  Element      root, htmlRoot, doctype, doctypeLine, prevLine, text;

  doc = SavingDocument;
  root = TtaGetMainRoot (doc);
  elType.ElSSchema = TtaGetDocumentSSchema (doc);
  if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
    return;

  /* Update the namespace declaration for the HTML element */
  elType.ElTypeNum = HTML_EL_HTML;
  htmlRoot = TtaSearchTypedElement (elType, SearchForward, root);
  if (!htmlRoot)
    return;
  if (isXml)
    TtaSetANamespaceDeclaration (doc, htmlRoot, NULL, XHTML_URI);
  else
    TtaFreeElemNamespaceDeclarations (doc, htmlRoot);

  /* Search the DOCTYPE element */
  elType.ElTypeNum = HTML_EL_DOCTYPE;
  doctype = TtaSearchTypedElement (elType, SearchForward, root);
  if (!doctype)
    return;
  
  /* Remove the previous doctype element */
  TtaDeleteTree (doctype, doc);
  
  profile = TtaGetDocumentProfile (doc);
  /* Don't check the Thot abstract tree against the structure schema. */
  TtaSetStructureChecking (0, doc);
  
  doctype = TtaNewElement (doc, elType);
  TtaInsertFirstChild (&doctype, root, doc);
  /* Make the DOCTYPE element read-only */
  TtaSetAccessRight (doctype, ReadOnly, doc);
  /* create the first DOCTYPE_line element */
  elType.ElTypeNum = HTML_EL_DOCTYPE_line;
  doctypeLine = TtaNewElement (doc, elType);
  TtaInsertFirstChild (&doctypeLine, doctype, doc);
  elType.ElTypeNum = 1;
  text = TtaNewElement (doc, elType);
  if (text != NULL)
    {
      TtaInsertFirstChild (&text, doctypeLine, SavingDocument);
      if (isXml)
	{
	  /* XML document */
	  if (profile == L_Strict)
	    TtaSetTextContent (text, (unsigned char *)DOCTYPE1_XHTML10_STRICT, Latin_Script, doc);
	  else
	    TtaSetTextContent (text, (unsigned char *)DOCTYPE1_XHTML10_TRANSITIONAL, Latin_Script, doc);
	}
      else
	{
	  /* HTML document */
	  if (profile == L_Strict)
	    TtaSetTextContent (text, (unsigned char *)DOCTYPE1_HTML_STRICT, Latin_Script, doc);
	  else
	    TtaSetTextContent (text, (unsigned char *)DOCTYPE1_HTML_TRANSITIONAL, Latin_Script, doc);
	}
    }
  /* create the second DOCTYPE_line element */
  elType.ElTypeNum = HTML_EL_DOCTYPE_line;
  prevLine = doctypeLine;
  doctypeLine = TtaNewElement (doc, elType);
  TtaInsertSibling (doctypeLine, prevLine, FALSE, doc);
  elType.ElTypeNum = 1;
  text = TtaNewElement (doc, elType);
  if (text != NULL)
    {
      TtaInsertFirstChild (&text, doctypeLine, doc);
      if (isXml)
	{
	  /* XML document */
	  if (profile == L_Strict)
	    TtaSetTextContent (text, (unsigned char *)DOCTYPE2_XHTML10_STRICT, Latin_Script, doc);
	  else
	    TtaSetTextContent (text, (unsigned char *)DOCTYPE2_XHTML10_TRANSITIONAL, Latin_Script, doc);
	}
      else
	{
	  /* HTML document */
	  if (profile == L_Strict)
	    TtaSetTextContent (text, (unsigned char *)DOCTYPE2_HTML_STRICT, Latin_Script, doc);
	  else
	    TtaSetTextContent (text, (unsigned char *)DOCTYPE2_HTML_TRANSITIONAL, Latin_Script, doc);
	}
    }
  TtaSetStructureChecking (1, doc);  
}

/*----------------------------------------------------------------------
   UpdateSaveAsButtons
   Maintain consistency between buttons in the Save As dialog box
  ----------------------------------------------------------------------*/
static void UpdateSaveAsButtons ()
{
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
  int	active;

  if (SaveAsHTML || SaveAsXML)
    active = 1;
  else
    active = 0;
  TtaRedrawMenuEntry (BaseDialog + ToggleSave, 0, NULL, -1, active);
  TtaRedrawMenuEntry (BaseDialog + ToggleSave, 1, NULL, -1, active);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
}

/*----------------------------------------------------------------------
   SetFileSuffix
   Set the suffix of the file name used for saving a document
  ----------------------------------------------------------------------*/
static void SetFileSuffix ()
{
  char  	suffix[6];
  char         *filename;
  int		i, len;

  if (SavingDocument && SaveName[0] != EOS)
    {
     if (SaveAsHTML)
        strcpy (suffix, "html");
     else if (SaveAsXML)
        strcpy (suffix, "html");
     else if (SaveAsText)
        strcpy (suffix, "txt");
     else
        return;

     /* looks for a suffix at the end of the document name */
     len = strlen (SaveName);
     for (i = len-1; i > 0 && SaveName[i] != '.'; i--);
     if (SaveName[i] != '.')
       {
         /* there is no suffix */
         i = len;
         SaveName[i++] = '.';
       }
     else
       {
	 /* there is a suffix */
	 i++;
	 if (strncmp (suffix, &SaveName[i], 3) == 0)
	   /* the requested suffix is already here. Do nothing */
	   i = 0;
#ifdef _WINDOWS
	 strcpy (DocToOpen, SavePath);
	 if (strchr (SavePath, '/'))
	   strcat (DocToOpen, URL_STR);
	 else
	   strcat (DocToOpen, DIR_STR);
	 strcat (DocToOpen, SaveName);
#endif /* _WINDOWS */
       }
     
     if (i > 0)
       {
	 /* change or append the suffix */
	 strcpy (&SaveName[i], suffix);
	 /* display the new filename in the dialog box */
	 filename = (char *)TtaGetMemory (MAX_LENGTH);
	 strcpy (filename, SavePath);
	 if (strchr (SavePath, '/'))
           strcat (filename, URL_STR);
	 else
	   strcat (filename, DIR_STR);
	 strcat (filename, SaveName);

#ifdef _WINDOWS
	 sprintf (DocToOpen, filename);
#endif /* _WINDOWS */

#if defined(_MOTIF) || defined(_GTK) || defined(_WX)  
	 TtaSetTextForm (BaseDialog + NameSave, filename);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
   
	 TtaFreeMemory (filename);
       }
    }
}

/*----------------------------------------------------------------------
   Callback procedure for dialogue events.                            
  ----------------------------------------------------------------------*/
void CallbackDialogue (int ref, int typedata, char *data)
{
  char              tempfile[MAX_LENGTH];
  char              tempname[MAX_LENGTH];
  char              sep, *tmp, *ptr;
  int               val;
#if defined(_MOTIF) || defined(_WINDOWS) || defined(_WX) 
  int               i;
#endif /* #if defined(_MOTIF) || defined(_WINDOWS) || defined(_WX) */
  ThotBool          change, updated;

  tmp = NULL;
  if (typedata == STRING_DATA && data && strchr (data, '/'))
    sep = URL_SEP;
  else
    sep = DIR_SEP;
  val = (int) data;
  if (ref - BaseDialog >= OptionMenu &&
      ref - BaseDialog <= OptionMenu + MAX_SUBMENUS)
    /* a popup menu corresponding to a SELECT element or a submenu
       corresponding to an OPTGROUP element*/
    {
      ReturnOption = val;
      ReturnOptionMenu = ref - BaseDialog - OptionMenu;
      TtaDestroyDialogue (BaseDialog + OptionMenu);
    }
  else switch (ref - BaseDialog)
    {
    case OpenForm:
      /* *********Load URL or local document********* */
      if (val == 1)
	/* Confirm */
	{
	  TtaDestroyDialogue (BaseDialog + OpenForm);
	  TtaDestroyDialogue (BaseDialog + FileBrowserForm);
	  
	  if (LastURLName[0] != EOS)
	    {
	      TtaSetStatus (CurrentDocument, 1,
			    TtaGetMessage (AMAYA, AM_CANNOT_LOAD),
			    DocumentName);
	      /* update the list of URLs */
	      if (NewFile)
		InitializeNewDoc (LastURLName, NewDocType, 0, NewDocProfile);
	      /* load an URL */ 
	      else if (InNewWindow)
		GetAmayaDoc (LastURLName, NULL, 0, 0, (ClickEvent)Loading_method,
			     FALSE, NULL, NULL, TtaGetDefaultCharset ());
	      else
		GetAmayaDoc (LastURLName, NULL, CurrentDocument,
			     CurrentDocument, (ClickEvent)Loading_method, TRUE,
			     NULL, NULL, TtaGetDefaultCharset ());
	    }
	  else if (DirectoryName[0] != EOS && DocumentName[0] != EOS)
	    {
	      /* load a local file */
	      strcpy (tempfile, DirectoryName);
	      strcat (tempfile, DIR_STR);
	      strcat (tempfile, DocumentName);
	      /* update the list of URLs */
	      NormalizeFile (tempfile, tempname, AM_CONV_ALL);
	      if (FileExistTarget (tempname))
		{
		  if (InNewWindow)
		    GetAmayaDoc (tempfile, NULL, 0, 0, (ClickEvent)Loading_method,
				 FALSE, NULL, NULL, TtaGetDefaultCharset ());
		  else
		    GetAmayaDoc (tempfile, NULL, CurrentDocument,
				 CurrentDocument, (ClickEvent)Loading_method,
				 TRUE, NULL, NULL, TtaGetDefaultCharset ());
		}
	      else if (NewFile)
		InitializeNewDoc (tempfile, NewDocType, 0, NewDocProfile);
	      else
		{
		  if (IsMathMLName (tempfile))
		    NewDocType = docMath;
		  else if (IsSVGName (tempfile))
		    NewDocType = docSVG;
		  else if (IsCSSName (tempfile))
		    NewDocType = docCSS;
#ifdef XML_GENERIC
		  else if (IsXMLName (tempfile))
		    NewDocType = docXml;
#endif /* XML_GENERIC */
#ifdef _SVG
		  else if (IsLibraryName (tempfile))
		    NewDocType = docLibrary;
#endif /* _SVG */
		  else
		    NewDocType = docHTML;
		  InitializeNewDoc (tempfile, NewDocType, CurrentDocument, NewDocProfile);
		}
	    }
	  else if (DocumentName[0] != EOS)
	    {
	      ptr = DocumentName;
	      updated = CompleteUrl (&ptr);
	      if (updated)
		{
		  strcpy (DocumentName, ptr);
		  TtaFreeMemory (ptr);
		}
	      /* update the list of URLs */
	      if (InNewWindow)
		GetAmayaDoc (DocumentName, NULL, 0, 0, (ClickEvent)Loading_method,
			     FALSE, NULL, NULL, TtaGetDefaultCharset ());
	      else
		GetAmayaDoc (DocumentName, NULL, CurrentDocument,
			     CurrentDocument, (ClickEvent)Loading_method, TRUE,
			     NULL, NULL, TtaGetDefaultCharset ());
	    }
	  else if (DirectoryName[0] != EOS)
	    TtaSetStatus (CurrentDocument, 1,
			  TtaGetMessage (AMAYA, AM_CANNOT_LOAD),
			  DirectoryName);
	  else
	    TtaSetStatus (CurrentDocument, 1,
			  TtaGetMessage (AMAYA, AM_CANNOT_LOAD), "");
	  NewFile = FALSE;
	  CurrentDocument = 0;
	}
      else if (val == 2)
	{
	  /* Browse button */
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
	  WidgetParent = OpenDocBrowser;
	  BrowserForm (CurrentDocument, 1, &LastURLName[0]);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
	}
      else if (val == 3)
	{
	  /* Clear button */
	  LastURLName[0] = EOS;
	  DirectoryName[0] = EOS;
	  DocumentName[0] = EOS;
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
	  TtaSetTextForm (BaseDialog + URLName, LastURLName);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
	}
      else if (NewFile)
	{
	  /* the command is aborted */
	  CheckAmayaClosed ();
	  NewFile = FALSE;
	}
      break;

    case URLName:
      RemoveNewLines (data);
      ptr = data;
      updated = CompleteUrl (&ptr);
      if (IsW3Path (ptr))
	{
	  /* save the URL name */
	  strcpy (LastURLName, ptr);
	  DocumentName[0] = EOS;
	}
      else
	{
	  LastURLName[0] = EOS;
	  change = NormalizeFile (ptr, tempfile, AM_CONV_NONE);
	  if (!IsW3Path (tempfile))
	    {
	      if (TtaCheckDirectory (tempfile))
		{
		  strcpy (DirectoryName, tempfile);
		  DocumentName[0] = EOS;
		}
	      else
		TtaExtractName (tempfile, DirectoryName, DocumentName);
	    }
	  else 
	    {
	      /* save the URL name */
	      strcpy (LastURLName, tempfile);
	      DocumentName[0] = EOS;
	    }
	}
      if (updated)
	TtaFreeMemory (ptr);
      break;

    case DirSelect:
#ifdef _WINDOWS
      sprintf (DirectoryName, "%s", data);
#endif /* _WINDOWS */
      
#if defined(_MOTIF) || defined(_GTK) || defined(_WX)  
      if (DirectoryName[0] != EOS)
	{
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
			    TtaGetMessage (LIB, TMSG_DOC_DIR),
			    BaseDialog + DirSelect, ScanFilter,
			    TtaGetMessage (AMAYA, AM_FILES),
			    BaseDialog + DocSelect);
	  DocumentName[0] = EOS;
	}
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
      
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
      
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
      TtaSetTextForm (BaseDialog + URLName, tempfile);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
      
      break;
    case ConfirmForm:
      /* *********Confirm********* */
      UserAnswer = (val == 1);
      ShowErrors = (val == 2);
      TtaDestroyDialogue (BaseDialog + ConfirmForm);
      break;
    case FilterText:
      /* Filter value */
      if (strlen (data) <= NAME_LENGTH)
	strcpy (ScanFilter, data);

#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
      else
	TtaSetTextForm (BaseDialog + FilterText, ScanFilter);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
      
      break;
    case FormAnswer:
      /* *********Get an answer********* */
      if (val == 0)
	{
	  /* no answer */
	  Answer_text[0] = EOS;
	  Answer_name[0] = EOS;
	  Answer_password[0] = EOS;
	  UserAnswer = 0;
	}
      else
	UserAnswer = 1;
      TtaDestroyDialogue (BaseDialog + FormAnswer);
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

#if defined(_MOTIF) || defined(_WINDOWS) || defined(_WX) 
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
	    {
	      /* a valid backspace */
	      Lg_password--;
	      Answer_password[Lg_password] = EOS;
	      Display_password[Lg_password] = EOS;
	    }
	}
      else
	Answer_password[NAME_LENGTH - 1] = EOS;
#endif /* #if defined(_MOTIF) || defined(_WINDOWS) || defined(_WX) */
      
#ifdef _MOTIF
      if (i > 0)
	TtaSetTextForm (BaseDialog + PasswordText, Display_password);
#endif /* _MOTIF */

#ifdef _GTK
      strncpy (Answer_password, data, NAME_LENGTH);
      Answer_password[NAME_LENGTH - 1] = EOS;
#endif /* _GTK */
      
      break;

    /* *********Save document as********* */
    case RadioSave:
      /* Output format */
      switch (val)
	{
	case 0:	/* "Save as HTML" button */
	  SaveAsHTML = TRUE;
	  SaveAsXML = FALSE;
	  SaveAsText = FALSE;
	  UpdateSaveAsButtons ();
	  SetFileSuffix ();
	  break;
	case 1:	/* "Save as XML" button */
	  SaveAsXML = TRUE;
	  SaveAsHTML = FALSE;
	  SaveAsText = FALSE;
	  UpdateSaveAsButtons ();
	  SetFileSuffix ();
	  /* Set the document charset */
	  if (TtaGetDocumentCharset (SavingDocument) == UNDEFINED_CHARSET)
	    TtaSetDocumentCharset (SavingDocument, ISO_8859_1, FALSE);
	  break;
	case 2:	/* "Save as Text" button */
	  SaveAsText = TRUE;
	  SaveAsHTML = FALSE;
	  SaveAsXML = FALSE;

#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
	  TtaSetToggleMenu (BaseDialog + ToggleSave, 1, SaveAsXML);
	  TtaSetToggleMenu (BaseDialog + ToggleSave, 0, SaveAsHTML);
	  UpdateSaveAsButtons ();
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
    
	  SetFileSuffix ();
	  break;
	}
      break;
    case ToggleSave:
      switch (val)
	{
	case 0:	/* "Copy Images" button */
	  CopyImages = !CopyImages;
	  break;
	case 1:	/* "Transform URLs" button */
	  UpdateURLs = !UpdateURLs;
	  break; 
	}
      break;
    case SaveForm:
      if (val == 1)
	/* "Confirm" Button */
	{
	  /* protect against saving without a MIME type */
	  if (SavingDocument != 0)
	    {
	      if (DocumentTypes[SavingDocument] == docHTML &&
		  DocumentMeta[SavingDocument])
		{
		  if (DocumentMeta[SavingDocument]->xmlformat && SaveAsHTML)
		    /* XHTML -> HTML */
		    ChangeDoctype (FALSE);
		  if (!DocumentMeta[SavingDocument]->xmlformat && SaveAsXML)
		    /* HTML -> XHTML */
		    ChangeDoctype (TRUE);
		}
	      
	      if ((!DocumentMeta[SavingDocument] 
		   || !DocumentMeta[SavingDocument]->content_type
		   || DocumentMeta[SavingDocument]->content_type[0] == EOS)
		  && (UserMimeType[0] == EOS))
		{

#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
		  TtaNewLabel (BaseDialog + SaveFormStatus,
			       BaseDialog + SaveForm,
			       TtaGetMessage (AMAYA, AM_INVALID_MIMETYPE));
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */

#ifdef _WINDOWS      
		  SaveAsDlgStatus (TtaGetMessage (AMAYA, AM_INVALID_MIMETYPE));
#endif /* _WINDOWS */

      break;
		}
	    }
	  TtaDestroyDialogue (BaseDialog + SaveForm);
	  if (SavingDocument != 0)
	    DoSaveAs (UserCharset, UserMimeType);
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
	/* "Browse" button */
	{
    
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
	  WidgetParent = DocSaveBrowser;
	  strcpy (LastURLName, SavePath);
	  strcat (LastURLName, DIR_STR);
	  strcat (LastURLName, SaveName);
	  BrowserForm (SavingDocument, 1, LastURLName);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
    
	}
      else if (val == 3)
	/* "Clear" button */
	{
	  if (SavingDocument != 0)
	    {
	      SavePath[0] = EOS;
	      SaveImgsURL[0] = EOS;
	      SaveName[0] = EOS;

#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
	      TtaSetTextForm (BaseDialog + NameSave, SaveImgsURL);
	      TtaSetTextForm (BaseDialog + ImgDirSave, SaveImgsURL);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
        
	    }
	}
      else if (val == 4)
	/* "Charset" button */
	{
	  if (SavingDocument != 0)
	    {
	      if (DocumentTypes[SavingDocument] != docImage)
		{
		  /* clear the status message */
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
		  TtaNewLabel (BaseDialog + SaveFormStatus,
			       BaseDialog + SaveForm, " ");
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */

#ifdef _WINDOWS      
		  SaveAsDlgStatus ("");
#endif /* _WINDOWS */

      InitCharset (SavingDocument, 1, SavePath);
		  if (SaveFormTmp[0] != EOS)
		    {
		      strcpy (UserCharset, SaveFormTmp);

#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
		      TtaNewLabel (BaseDialog + CharsetSave,  
				   BaseDialog + SaveForm, UserCharset);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */

        }
		}
	      else
		{

#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
		  TtaNewLabel (BaseDialog + SaveFormStatus,
			       BaseDialog + SaveForm,
			       TtaGetMessage (AMAYA, AM_NOCHARSET_SUPPORT));
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */

#ifdef _WINDOWS
		  SaveAsDlgStatus (TtaGetMessage (AMAYA, AM_NOCHARSET_SUPPORT));
#endif /* _WINDOWS */

    }
	    }
	}
      else if (val == 5)
	/* "MIME type" button */
	{
	  if (SavingDocument != 0)
	    {
	      if (SavePath[0] && IsW3Path (SavePath))
		{
		  /* clear the status message */

#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
		  TtaNewLabel (BaseDialog + SaveFormStatus,
			       BaseDialog + SaveForm,
			       " ");
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */

#ifdef _WINDOWS      
		  SaveAsDlgStatus ("");
#endif /* _WINDOWS */

      InitMimeType (SavingDocument, 1, SavePath, NULL);
		  if (SaveFormTmp[0] != EOS)
		    {
		      strcpy (UserMimeType, SaveFormTmp);
		      
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
		      TtaNewLabel (BaseDialog + MimeTypeSave,  
				   BaseDialog + SaveForm, UserMimeType);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
          
		    }
		}
	      else
		{
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
		  TtaNewLabel (BaseDialog + SaveFormStatus,
			       BaseDialog + SaveForm,
			       TtaGetMessage (AMAYA, AM_NOMIMETYPE_SUPPORT));
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */

#ifdef _WINDOWS      
		  SaveAsDlgStatus (TtaGetMessage (AMAYA, AM_NOMIMETYPE_SUPPORT));
#endif /* _WINDOWS */
      
		}
	    }
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
      if (!IsW3Path (data))
	change = NormalizeFile (data, tempfile, AM_CONV_NONE);
      else
	strcpy (tempfile, data);
      
      if (*tempfile && tempfile[strlen (tempfile) - 1] == sep)
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
      break;
    case ImgDirSave:
      /* Image directory */
      if (!IsW3Path (data))
	change = NormalizeFile (data, SaveImgsURL, AM_CONV_NONE);
      else
	strcpy (SaveImgsURL, data);
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
	  /* Confirm button */
	  if (AttrHREFvalue[0] != EOS)
	    {
#ifdef _I18N_
	      tmp = (char *)TtaConvertByteToMbs ((unsigned char *)AttrHREFvalue, TtaGetDefaultCharset ());
	      /* create an attribute HREF for the Link_Anchor */
	      SetREFattribute (AttrHREFelement, AttrHREFdocument,
			       tmp, NULL);
	      TtaFreeMemory (tmp);
#else /* _I18N_ */
	      /* create an attribute HREF for the Link_Anchor */
	      SetREFattribute (AttrHREFelement, AttrHREFdocument,
			       AttrHREFvalue, NULL);
#endif /* _I18N_ */
	    }
	  TtaDestroyDialogue (BaseDialog + AttrHREFForm);
	  TtaDestroyDialogue (BaseDialog + FileBrowserForm);
	}
      else if (val == 2)
	/* Browse button */
	{
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
	  if (LinkAsXmlCSS || LinkAsCSS)
	    strcpy (ScanFilter, "*.css");
	  else if (!strcmp (ScanFilter, "*.css"))
	    strcpy (ScanFilter, "*");
	  WidgetParent = HrefAttrBrowser;
	  BrowserForm (AttrHREFdocument, 1, &AttrHREFvalue[0]);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
	}
      else if (val == 3)
	/* allow one to click the target */
	SelectDestination (AttrHREFdocument, AttrHREFelement, AttrHREFundoable, TRUE);
      else if (val == 4)
	{
	  /* Clear button */
	  AttrHREFvalue[0] = EOS;
    
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
	  TtaSetTextForm (BaseDialog + AttrHREFText, AttrHREFvalue);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
    
	}
      else 
	/* Cancel button */
	if (IsNewAnchor)
	  {
	    LinkAsCSS = FALSE;
	    LinkAsXmlCSS = FALSE;
	    /* remove the link if it was just created */
	    TtaCancelLastRegisteredSequence (AttrHREFdocument);	   
	    DeleteAnchor (AttrHREFdocument, 1);
	    TtaCancelLastRegisteredSequence (AttrHREFdocument);	   
	  }
      break;

    case AttrHREFText:
      /* save the HREF name */
      RemoveNewLines (data);
      if (IsW3Path (data))
	{
	  /* save the URL name */
	  strcpy (AttrHREFvalue, data);
	  DocumentName[0] = EOS;
	}
      else
	{
	  change = NormalizeFile (data, tempfile, AM_CONV_NONE);
	  if (TtaCheckDirectory (tempfile))
	    {
	      strcpy (DirectoryName, tempfile);
	      DocumentName[0] = EOS;
	    }
	  else
	    TtaExtractName (tempfile, DirectoryName, DocumentName);
	  strcpy (AttrHREFvalue, tempfile);
	}       
      break;
      
      /* *********File Browser*********** */
    case FileBrowserForm:
      if (val == 1)
	{
	  /* Confirm button */
	  /* it's no longer the default Welcome page */
	  WelcomePage = FALSE;
    
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
	  /* this code is only valid under Unix. */
	  /* In Windows, we're using a system widget */
	  strcpy (tempfile, DirectoryName);
	  strcat (tempfile, DIR_STR);
	  strcat (tempfile, DocumentName);
	  if (WidgetParent == HrefAttrBrowser)
	    {
	      TtaSetTextForm (BaseDialog + AttrHREFText, tempfile);
	      strcpy (AttrHREFvalue, tempfile);
	      CallbackDialogue (BaseDialog + AttrHREFForm, INTEGER_DATA, (char *) 1);
	    }
	  else if (WidgetParent == OpenDocBrowser)
	    {
	      TtaSetTextForm (BaseDialog + URLName, tempfile);
	      CallbackDialogue (BaseDialog + OpenForm, INTEGER_DATA, (char *) 1);
	    }
	  else if (WidgetParent == DocSaveBrowser)
	    {
	      TtaSetTextForm (BaseDialog + NameSave, tempfile);
	      CallbackDialogue (BaseDialog + NameSave, STRING_DATA, tempfile);
	    }
	  /* remove the browsing dialogue */
	  TtaDestroyDialogue (ref);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
    
	}
      else if (val == 2)
	/* Clear button */
	{
	  if (WidgetParent == OpenDocBrowser)
	    {
	      LastURLName[0] = EOS;
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
	      TtaSetTextForm (BaseDialog + FileBrowserText, LastURLName);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
        
	    }
	  else if (WidgetParent == HrefAttrBrowser)
	    {
	      tempname[0] = EOS; 	       
        
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
	      TtaSetTextForm (BaseDialog + FileBrowserText, tempname);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
        
	    }
	}
      else if (val == 3)
	/* Filter button */
	{
	  /* reinitialize directories and document lists */
	  TtaListDirectory (DirectoryName, BaseDialog + FileBrowserForm,
			    TtaGetMessage (LIB, TMSG_DOC_DIR),
			    BaseDialog + BrowserDirSelect,
			    ScanFilter, TtaGetMessage (AMAYA, AM_FILES),
			    BaseDialog + BrowserDocSelect);
	}
      else 
	/* Cancel button */
	{
	}
      break;
      
    case FileBrowserText:
      RemoveNewLines (data);
      if (IsW3Path (data))
	{
	  DocumentName[0] = EOS;
	}
      else
	{
	  change = NormalizeFile (data, tempfile, AM_CONV_NONE);
	  if (TtaCheckDirectory (tempfile))
	    {
	      strcpy (DirectoryName, tempfile);
	      DocumentName[0] = EOS;
	    }
	  else
	    TtaExtractName (tempfile, DirectoryName, DocumentName);
	  strcpy (AttrHREFvalue, tempfile);
	}       
      break;
      
      /* *********Browser DirSelect*********** */
    case BrowserDirSelect:

#ifdef _WINDOWS
      sprintf (DirectoryName, "%s", data);
#endif /* _WINDOWS */
      
#if defined(_MOTIF) || defined(_GTK) || defined(_WX)  
      if (DirectoryName[0] != EOS)
	{
	  if (!strcmp (data, ".."))
	    {
	      /* suppress last directory */
	      strcpy (tempname, DirectoryName);
	      TtaExtractName (tempname, DirectoryName, tempfile);
	      if (DirectoryName[0] == EOS)
		strcpy (DirectoryName, DIR_STR);
	    }
	  else
	    {
	      strcat (DirectoryName, DIR_STR);
	      strcat (DirectoryName, data);
	    }
	  
	  if (WidgetParent == OpenDocBrowser)
	    LastURLName[0] = EOS;
	  
	  TtaSetTextForm (BaseDialog + FileBrowserText, DirectoryName);
	  TtaListDirectory (DirectoryName, BaseDialog + FileBrowserForm,
			    TtaGetMessage (LIB, TMSG_DOC_DIR),
			    BaseDialog + BrowserDirSelect, ScanFilter,
			    TtaGetMessage (AMAYA, AM_FILES),
			    BaseDialog + BrowserDocSelect);
	  DocumentName[0] = EOS;
	}
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
      break;
      
      /* *********Browser DocSelect*********** */
    case BrowserDocSelect:
      /* set path on current directory */
      if (DirectoryName[0] == EOS)
	getcwd (DirectoryName, MAX_LENGTH);
      
      /* Extract suffix from document name */
      strcpy (DocumentName, data);
      
      if (WidgetParent == OpenDocBrowser)
	LastURLName[0] = EOS;
      
      /* construct the document full name */
      strcpy (tempfile, DirectoryName);
      strcat (tempfile, DIR_STR);
      strcat (tempfile, DocumentName);

#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
      TtaSetTextForm (BaseDialog + FileBrowserText, tempfile);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
      
      break;
      
      /* *********Browser Filter*********** */
    case FileBrowserFilter:
      /* Filter value */
      if (strlen(data) <= NAME_LENGTH)
	strcpy (ScanFilter, data);
      
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
      else
	TtaSetTextForm (BaseDialog + BrowserFilterText, ScanFilter);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
      
      break;
      
    case TitleForm:
      /* ********Change the document title*********** */
      TtaDestroyDialogue (BaseDialog + TitleForm);
      if (val == 1)
	SetNewTitle (CurrentDocument);
      break;
      
    case TitleText:
      /* new document name */
      strncpy (Answer_text, data, MAX_LENGTH);
      Answer_text[MAX_LENGTH - 1] = EOS;
      break;
      
    case ClassForm:
    case ClassSelect:
    case AClassForm:
    case AClassSelect:
      StyleCallbackDialogue (ref, typedata, data);
      break;
      
    case TableForm:
      /* *********Table Form*********** */
      UserAnswer = (val == 1);
      TtaDestroyDialogue (BaseDialog + TableForm);
      break;
    case TableRows:
      NumberRows = val;
      break;
    case TableCols:
      NumberCols = val;
      break;
    case TableBorder:
      TBorder = val;
      break;
    case MathEntityForm:
      /* ********* MathML entity form ********* */
      if (val == 0)
	/* no answer */
	MathMLEntityName[0] = EOS;
      break;
    case MathEntityText:
      strncpy (MathMLEntityName, data, MAX_LENGTH);
      MathMLEntityName[MAX_LENGTH - 1] = EOS;
      break;
    case MakeIdMenu:
      switch (val)
	{
	case 1:
	  CreateRemoveIDAttribute (IdElemName, IdDoc, TRUE, 
				   (IdApplyToSelection) ? TRUE: FALSE);
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
	  /* and show the status */
	  TtaNewLabel (BaseDialog + mIdStatus,
		       BaseDialog + MakeIdMenu,
		       IdStatus);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
	  break;
	case 2:
	  CreateRemoveIDAttribute (IdElemName, IdDoc, FALSE, 
				   (IdApplyToSelection) ? TRUE: FALSE);
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
	  /* and show the status */
	  TtaNewLabel (BaseDialog + mIdStatus,
		       BaseDialog + MakeIdMenu,
		       IdStatus);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
	  break;
	}
      break;
    case mElemName:
      strncpy (IdElemName, data, MAX_LENGTH);
      IdElemName[MAX_LENGTH - 1] = EOS;
      break;
    case mIdUseSelection:
      IdApplyToSelection = val;
      break;
      /* Charset Save As menu */
    case CharsetForm:
      {
	switch (val)
	  {
	  case 0:
	    SaveFormTmp[0] = EOS;
	    TtaDestroyDialogue (ref);
	    break;
	  case 1:
	    TtaDestroyDialogue (ref);
	    break;
	  }
      }
      break;
    case CharsetSel:
      switch (val)
	{
	case 0:
	  strcpy (SaveFormTmp, "us-ascii");
	  break;
	case 1:
	  strcpy (SaveFormTmp, "UTF-8");
	  break;
	case 2:
	  strcpy (SaveFormTmp, "iso-8859-1");
	  break;
	case 3:
	  strcpy (SaveFormTmp, "iso-8859-2");
	  break;
	}
      break;

      /* MIME type Save As menu */
    case MimeTypeForm:
      {
	switch (val)
	  {
	  case 0:
	    SaveFormTmp[0] = EOS;
	    TtaDestroyDialogue (ref);
	    break;
	  case 1:
	    {
	      char *src, *dst;
	      /* filter the UserMimeType */
	      src = dst = SaveFormTmp;
	      while (*src)
		{
		  if (!isascii (*src) 
		      || (!isalnum (*src) && *src != '/' && *src != '-'
			  && *src != '+'))
		    {
		      /* skip the character */
		      src++;
		      continue;;
		    }
		  
		  if (isupper (*src))
		    /* convert to lower case */
		    *dst = tolower (*src);
		  else
		    *dst = *src;
		  src++;
		  dst++;
		}
	      *dst = EOS;
	      /* validate the mime type */
	      if (SaveFormTmp[0] == EOS ||!strchr (SaveFormTmp, '/'))
		{
		  SaveFormTmp[0] = EOS;

#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
		  InitMimeType (SavingDocument, 1, SavePath,
				TtaGetMessage (AMAYA, AM_INVALID_MIMETYPE));
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */

#ifdef _WINDOWS
      /* the Window dialog won't be closed */
		  MimeTypeDlgStatus (TtaGetMessage (AMAYA, AM_INVALID_MIMETYPE));
#endif /* _WINDOWS */

    }
	      else
		TtaDestroyDialogue (ref);
	    }
	    break;
	  }
      }
      break;
    case MimeTypeSel:
      if (data)
	strcpy (SaveFormTmp, data);
      else
	SaveFormTmp[0] = EOS;
      break;
    }
}

/*----------------------------------------------------------------------
  RestoreOneAmayaDoc restores a saved file
  doc is the suggested doc to be loaded or 0.
  docname is the original name of the document.
  tempdoc is the name of the saved file.
  Return the new recovered document
  ----------------------------------------------------------------------*/
static int RestoreOneAmayaDoc (Document doc, char *tempdoc, char *docname,
			       DocumentType docType, ThotBool iscrash)
{
  AHTHeaders    http_headers;
  char          content_type[MAX_LENGTH];
  char          tempfile[MAX_LENGTH];
  int           newdoc, len;
  ThotBool      stopped_flag;

  W3Loading = doc;
  BackupDocument = doc;
  TtaExtractName (tempdoc, DirectoryName, DocumentName);
  AddURLInCombobox (docname, NULL, TRUE);
  newdoc = InitDocAndView (doc, DocumentName, (DocumentType)docType, 0, FALSE, L_Other,
			   (ClickEvent)CE_ABSOLUTE);
   if (newdoc != 0)
    {
      /* load the saved file */
      W3Loading = newdoc;
      if (IsW3Path (docname))
	{
	  /* it's a remote file */

	  /* clear the headers */
	  memset ((void *) &http_headers, 0, sizeof (AHTHeaders));
	  if (docType == docHTML)
	    {
	      strcpy (content_type, "text/html");
	      http_headers.content_type = content_type;
	    }
	  else
	      http_headers.content_type = NULL;
	  LoadDocument (newdoc, docname, NULL, NULL, CE_ABSOLUTE, 
			tempdoc, DocumentName, &http_headers, FALSE, &InNewWindow);
	}
      else
	{
	  /* it's a local file */
	  tempfile[0] = EOS;
	  /* load the temporary file */
	  LoadDocument (newdoc, tempdoc, NULL, NULL, CE_ABSOLUTE,
			tempfile, DocumentName, NULL, FALSE, &InNewWindow);
	  /* change its URL */
	  TtaFreeMemory (DocumentURLs[newdoc]);
	  len = strlen (docname) + 1;
	  DocumentURLs[newdoc] = TtaStrdup (docname);
	  DocumentSource[newdoc] = 0;
	  /* add the URI in the combobox string */
	  AddURLInCombobox (docname, NULL, FALSE);
	  TtaSetTextZone (newdoc, 1, URL_list);
	  /* change its directory name */
	  TtaSetDocumentDirectory (newdoc, DirectoryName);
	}
      TtaSetDocumentModified (newdoc);
      W3Loading = 0;		/* loading is complete now */
      DocNetworkStatus[newdoc] = AMAYA_NET_ACTIVE;
      stopped_flag = FetchAndDisplayImages (newdoc, AMAYA_LOAD_IMAGE, NULL);
      if (!stopped_flag)
	{
	  DocNetworkStatus[newdoc] = AMAYA_NET_INACTIVE;
	  /* almost one file is restored */
	  TtaSetStatus (newdoc, 1, TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED),
			NULL);
	}
      /* check parsing errors */
      CheckParsingErrors (newdoc);
      /* unlink this saved file */
      if (iscrash)
	TtaFileUnlink (tempdoc);
      /* Update the doctype menu */
      UpdateDoctypeMenu (newdoc);
    }
  BackupDocument = 0;
  return (newdoc);
}


/*----------------------------------------------------------------------
  RestoreAmayaDocs 
  Checks if Amaya has previously crashed.
  The file Crash.amaya gives the list of saved files.
  The file AutoSave.dat gives the list of auto-saved files
  ----------------------------------------------------------------------*/
static ThotBool RestoreAmayaDocs ()
{
  FILE       *f;
  char        tempname[MAX_LENGTH], tempdoc[MAX_LENGTH];
  char        docname[MAX_LENGTH];  
  char        line[MAX_LENGTH * 2];
  int         docType, i, j;
  ThotBool    aDoc, iscrash, restore;

  /* no document is opened */
  aDoc = FALSE;
  iscrash = FALSE;

  /* check if Amaya has crashed */
  sprintf (tempname, "%s%cCrash.amaya", TempFileDirectory, DIR_SEP);
  if (TtaFileExist (tempname))
    iscrash = TRUE;
  else if (!AmayaUniqueInstance)
    return FALSE;
  else
    sprintf (tempname, "%s%cAutoSave.dat", TempFileDirectory, DIR_SEP);
  
  if (TtaFileExist (tempname))
    {
      InitConfirm (0, 0, TtaGetMessage (AMAYA, AM_RELOAD_FILES));
      restore = UserAnswer;
      f = fopen (tempname, "r");
      if (f != NULL)
	{
	  InNewWindow = TRUE;
	  i = 0;
	  line[i] = EOS;
	  fread (&line[i], 1, 1, f);
	  while (line[0] == '"')
	    {
	      /* get the temp name */
	      do
		{
		  i++;
		  fread (&line[i], 1, 1, f);
		}
	      while (line[i] != '"');
	      line[i] = EOS;
	      strcpy (tempdoc, &line[1]);
	      /* skip spaces and the next first " */
	      do
		{
		  i++;
		  fread (&line[i], 1, 1, f);
		}
	      while (line[i] != '"');
	      /* get the origin name */
	      j = i + 1;
	      do
		{
		  i++;
		  fread (&line[i], 1, 1, f);
		}
	      while (line[i] != '"');
	      line[i] = EOS;
	      strcpy (docname, &line[j]);
	      /* skip spaces */
	      do
		{
		  i++;
		  fread (&line[i], 1, 1, f);
		}
	      while (line[i] == ' ');
	      /* get the docType */
	      j = i;
	      do
		{
		  i++;
		  fread (&line[i], 1, 1, f);
		}
	      while (line[i] != '\n');
	      line[i] = EOS;
	      sscanf (&line[j], "%d",  &docType);
	      if (tempdoc[0] != EOS && TtaFileExist (tempdoc))
		{
		  if (restore)
		    {
		      if (RestoreOneAmayaDoc (0, tempdoc, docname, (DocumentType) docType, iscrash))
			aDoc = TRUE;
		    }
		  else
		    /* unlink this saved file */
		    TtaFileUnlink (tempdoc);
		}
	      /* next saved file */
	      i = 0;
	      line[i] = EOS;
	      fread (&line[i], 1, 1, f);
	    }
	  InNewWindow = FALSE;	  
	  fclose (f);
	}

      if (iscrash)
	{
	  TtaFileUnlink (tempname);
	  sprintf (tempname, "%s%cAutoSave.dat", TempFileDirectory, DIR_SEP);
	  if (TtaFileExist (tempname))
	    {
	      f = fopen (tempname, "r");
	      if (f != NULL)
		{
		  i = 0;
		  line[i] = EOS;
		  fread (&line[i], 1, 1, f);
		  while (line[0] == '"')
		    {
		      /* get the temp name */
		      do
			{
			  i++;
			  fread (&line[i], 1, 1, f);
			}
		      while (line[i] != '"');
		      line[i] = EOS;
		      strcpy (tempdoc, &line[1]);
		      /* skip spaces and the next first " */
		      do
			{
			  i++;
			  fread (&line[i], 1, 1, f);
			}
		      while (line[i] != '"');
		      /* get the origin name */
		      j = i + 1;
		      do
			{
			  i++;
			  fread (&line[i], 1, 1, f);
			}
		      while (line[i] != '"');
		      line[i] = EOS;
		      strcpy (docname, &line[j]);
		      /* skip spaces */
		      do
			{
			  i++;
			  fread (&line[i], 1, 1, f);
			}
		      while (line[i] == ' ');
		      /* get the docType */
		      j = i;
		      do
			{
			  i++;
			  fread (&line[i], 1, 1, f);
			}
		      while (line[i] != '\n');
		      line[i] = EOS;
		      sscanf (&line[j], "%d",  &docType);
		      if (tempdoc[0] != EOS && TtaFileExist (tempdoc))
			  /* unlink the auto-saved file */
			  TtaFileUnlink (tempdoc);
		      /*next auto-saved file */
		      i = 0;
		      line[i] = EOS;
		      fread (&line[i], 1, 1, f);
		    }
		  fclose (f);
		}
	      TtaFileUnlink (tempname);
	    }
	}
      else
	{
	  if (!UserAnswer)
	    TtaFileUnlink (tempname);
	}

    }
  return (aDoc);
}


/*----------------------------------------------------------------------
  CheckMakeDirectory verifies if a directory name exists. If it doesn't
  exist, it tries to create it. If recusive == TRUE, it tries to create
  all the intermediary directories.
  Returns TRUE if the operation succeeds, FALSE otherwise.
  ----------------------------------------------------------------------*/
ThotBool CheckMakeDirectory (char *name, ThotBool recursive)
{
  ThotBool  i;
  char     *tmp_name;
  char     *ptr;
  char      tmp_char;

  /* protection against bad calls */
  if (!name || *name == EOS)
    return FALSE;

  /* does the directory exist? */
  if (TtaMakeDirectory (name))
    return TRUE;

  /* no, try to create it then */
  i = TtaMakeDirectory (name);

  /* were be able to create the directory (or found it already?) */
  if (!i)
    {
      /* don't do anything else */
      if (!recursive)
	return FALSE;
      
      /* try to create all the missing directories up to name */
      tmp_name = TtaStrdup (name);
      ptr = tmp_name;
      /* create all the intermediary directories */
      while (*ptr != EOS)
	{
	  if (*ptr != DIR_SEP)
	    ptr++;
	  else
	    {
	      tmp_char = *ptr;
	      *ptr = EOS;
	      i = TtaMakeDirectory (tmp_name);
	      if (!i)
		{
		  TtaFreeMemory (tmp_name);
		  return FALSE;
		}
	      *ptr = tmp_char;
	      ptr++;
	    }
	}
      /* create the last dir */
      i = TtaMakeDirectory (tmp_name);
      TtaFreeMemory (tmp_name);
      if (!i)
	return FALSE;
    }
  return TRUE;
}

/*----------------------------------------------------------------------
   FreeAmayaStructures cleans up memory ressources.
  ----------------------------------------------------------------------*/
void FreeAmayaStructures ()
{
  if (LastURLName)
    {
      /* now exit the application */
      TtaFreeMemory (LastURLName);
      LastURLName = NULL;
      TtaFreeMemory (DocumentName);
      TtaFreeMemory (DirectoryName);
      TtaFreeMemory (SavePath);
      TtaFreeMemory (SaveName);
      TtaFreeMemory (ObjectName);
      TtaFreeMemory (SaveImgsURL);
      TtaFreeMemory (SavingFile);
      TtaFreeMemory (SavedDocumentURL);
      TtaFreeMemory (AttrHREFvalue);
      TtaFreeMemory (UserCSS);
      TtaFreeMemory (URL_list);
      TtaFreeMemory (AutoSave_list);
      FreeHTMLParser ();
      FreeXmlParserContexts ();
      FreeDocHistory ();
      FreeTransform ();
      QueryClose ();
#ifdef ANNOTATIONS
      XPointer_bufferFree ();
      ANNOT_Quit ();
#endif /* ANNOTATIONS */
#ifdef BOOKMARKS
      BM_Quit ();
#endif /* BOOKMARKS */
      FreeAmayaCache (); 
    }
}


/*----------------------------------------------------------------------
  InitAmaya intializes Amaya variables and open the first document window.
  ----------------------------------------------------------------------*/
void InitAmaya (NotifyEvent * event)
{
   char               *s;
   char               *ptr;
   int                 i;
   float               val=0;
   ThotBool            restoredDoc;
   ThotBool            numbering, map, add, bt;

   if (AmayaInitialized)
      return;
   AmayaInitialized = 1;
   W3Loading = 0;
   BackupDocument = 0;
   /* initialize status */
   SelectionDoc = 0;
   ParsedDoc = 0;
   SelectionInPRE = FALSE;
   SelectionInComment = FALSE;
   SelectionInEM = FALSE;
   SelectionInSTRONG = FALSE;
   SelectionInCITE = FALSE;
   SelectionInABBR = FALSE;
   SelectionInACRONYM = FALSE;
   SelectionInINS = FALSE;
   SelectionInDEL = FALSE;
   SelectionInDFN = FALSE;
   SelectionInCODE = FALSE;
   SelectionInVAR = FALSE;
   SelectionInSAMP = FALSE;
   SelectionInKBD = FALSE;
   SelectionInI = FALSE;
   SelectionInB = FALSE;
   SelectionInTT = FALSE;
   SelectionInBIG = FALSE;
   SelectionInSMALL = FALSE;
   Synchronizing = FALSE;
   IdElemName[0] = EOS;
   /* Initialize the LogFile variables */
   CleanUpParsingErrors ();
   /* we're not linking an external CSS */
   LinkAsCSS = FALSE;
   LinkAsXmlCSS = FALSE;

   /* initialize icons */
#if defined(_NOGUI) && !defined(_WX) // TODO "&& !defined(_WX)" a virer a la fin de la migration wxWindows
   stopR = (ThotIcon) 0;
   stopN = (ThotIcon) 0;
   iconSave = (ThotIcon) 0;
   iconSaveNo = (ThotIcon) 0;
   iconFind = (ThotIcon) 0;
   iconReload = (ThotIcon) 0;
   iconHome = (ThotIcon) 0;
   iconI = (ThotIcon) 0;
   iconINo = (ThotIcon) 0;
   iconB = (ThotIcon) 0;
   iconBNo = (ThotIcon) 0;
   iconT = (ThotIcon) 0;
   iconTNo = (ThotIcon) 0;
   iconBack = (ThotIcon) 0;
   iconBackNo = (ThotIcon) 0;
   iconForward = (ThotIcon) 0;
   iconForwardNo = (ThotIcon) 0;
   iconBrowser = (ThotIcon) 0;
   iconEditor = (ThotIcon) 0;
   iconH1 = (ThotIcon) 0;
   iconH1No = (ThotIcon) 0;
   iconH2 = (ThotIcon) 0;
   iconH2No = (ThotIcon) 0;
   iconH3 = (ThotIcon) 0;
   iconH3No = (ThotIcon) 0;
   iconPrint = (ThotIcon) 0;
   iconBullet = (ThotIcon) 0;
   iconBulletNo = (ThotIcon) 0;
   iconNum = (ThotIcon) 0;
   iconNumNo = (ThotIcon) 0;
   iconImage = (ThotIcon) 0;
   iconImageNo = (ThotIcon) 0;
   iconDL = (ThotIcon) 0;
   iconDLNo = (ThotIcon) 0;
   iconLink = (ThotIcon) 0;
   iconLinkNo = (ThotIcon) 0;
   iconTable = (ThotIcon) 0;
   iconTableNo = (ThotIcon) 0;
#endif /* #ifdef _NOGUI */

#if defined(_GTK) || defined(_WX)
   stopR = (ThotIcon) TtaCreatePixmapLogo (stopR_xpm);
   stopN = (ThotIcon) TtaCreatePixmapLogo (stopN_xpm);
   iconSave = (ThotIcon) TtaCreatePixmapLogo (save_xpm);
   iconSaveNo = (ThotIcon) TtaCreatePixmapLogo (saveNo_xpm);
   iconFind = (ThotIcon) TtaCreatePixmapLogo (find_xpm);
   iconReload = (ThotIcon) TtaCreatePixmapLogo (Reload_xpm);
   iconHome = (ThotIcon) TtaCreatePixmapLogo (home_xpm);
   iconI = (ThotIcon) TtaCreatePixmapLogo (I_xpm);
   iconINo = (ThotIcon) TtaCreatePixmapLogo (INo_xpm);
   iconB = (ThotIcon) TtaCreatePixmapLogo (B_xpm);
   iconBNo = (ThotIcon) TtaCreatePixmapLogo (BNo_xpm);
   iconT = (ThotIcon) TtaCreatePixmapLogo (T_xpm);
   iconTNo = (ThotIcon) TtaCreatePixmapLogo (TNo_xpm);
   iconBack = (ThotIcon) TtaCreatePixmapLogo (Back_xpm);
   iconBackNo = (ThotIcon) TtaCreatePixmapLogo (BackNo_xpm);
   iconForward = (ThotIcon) TtaCreatePixmapLogo (Forward_xpm);
   iconForwardNo = (ThotIcon) TtaCreatePixmapLogo (ForwardNo_xpm);
   iconBrowser = (ThotIcon) TtaCreatePixmapLogo (Browser_xpm);
   iconEditor = (ThotIcon) TtaCreatePixmapLogo (Editor_xpm);
   iconH1 = (ThotIcon) TtaCreatePixmapLogo (H1_xpm);
   iconH1No = (ThotIcon) TtaCreatePixmapLogo (H1No_xpm);
   iconH2 = (ThotIcon) TtaCreatePixmapLogo (H2_xpm);
   iconH2No = (ThotIcon) TtaCreatePixmapLogo (H2No_xpm);
   iconH3 = (ThotIcon) TtaCreatePixmapLogo (H3_xpm);
   iconH3No = (ThotIcon) TtaCreatePixmapLogo (H3No_xpm);
   iconPrint = (ThotIcon) TtaCreatePixmapLogo (Print_xpm);
   iconBullet = (ThotIcon) TtaCreatePixmapLogo (Bullet_xpm);
   iconBulletNo = (ThotIcon) TtaCreatePixmapLogo (BulletNo_xpm);
   iconNum = (ThotIcon) TtaCreatePixmapLogo (Num_xpm);
   iconNumNo = (ThotIcon) TtaCreatePixmapLogo (NumNo_xpm);
   iconImage = (ThotIcon) TtaCreatePixmapLogo (Image_xpm);
   iconImageNo = (ThotIcon) TtaCreatePixmapLogo (ImageNo_xpm);
   iconDL = (ThotIcon) TtaCreatePixmapLogo (DL_xpm);
   iconDLNo = (ThotIcon) TtaCreatePixmapLogo (DLNo_xpm);
   iconLink = (ThotIcon) TtaCreatePixmapLogo (Link_xpm);
   iconLinkNo = (ThotIcon) TtaCreatePixmapLogo (LinkNo_xpm);
   iconTable = (ThotIcon) TtaCreatePixmapLogo (Table_xpm);
   iconTableNo = (ThotIcon) TtaCreatePixmapLogo (TableNo_xpm);
#endif /* _GTK  || defined(_WX) */

#ifdef _MOTIF   
   stopR = TtaCreatePixmapLogo (stopR_xpm);
   stopN = TtaCreatePixmapLogo (stopN_xpm);
   iconSave = TtaCreatePixmapLogo (save_xpm);
   iconSaveNo = TtaCreatePixmapLogo (saveNo_xpm);
   iconFind = TtaCreatePixmapLogo (find_xpm);
   iconReload = TtaCreatePixmapLogo (Reload_xpm);
   iconHome = TtaCreatePixmapLogo (home_xpm);
   iconI = TtaCreatePixmapLogo (I_xpm);
   iconINo = TtaCreatePixmapLogo (INo_xpm);
   iconB = TtaCreatePixmapLogo (B_xpm);
   iconBNo = TtaCreatePixmapLogo (BNo_xpm);
   iconT = TtaCreatePixmapLogo (T_xpm);
   iconTNo = TtaCreatePixmapLogo (TNo_xpm);
   iconBack = TtaCreatePixmapLogo (Back_xpm);
   iconBackNo = TtaCreatePixmapLogo (BackNo_xpm);
   iconForward = TtaCreatePixmapLogo (Forward_xpm);
   iconForwardNo = TtaCreatePixmapLogo (ForwardNo_xpm);
   iconBrowser = TtaCreatePixmapLogo (Browser_xpm);
   iconEditor = TtaCreatePixmapLogo (Editor_xpm);
   iconH1 = TtaCreatePixmapLogo (H1_xpm);
   iconH1No = TtaCreatePixmapLogo (H1No_xpm);
   iconH2 = TtaCreatePixmapLogo (H2_xpm);
   iconH2No = TtaCreatePixmapLogo (H2No_xpm);
   iconH3 = TtaCreatePixmapLogo (H3_xpm);
   iconH3No = TtaCreatePixmapLogo (H3No_xpm);
   iconPrint = TtaCreatePixmapLogo (Print_xpm);
   iconBullet = TtaCreatePixmapLogo (Bullet_xpm);
   iconBulletNo = TtaCreatePixmapLogo (BulletNo_xpm);
   iconNum = TtaCreatePixmapLogo (Num_xpm);
   iconNumNo = TtaCreatePixmapLogo (NumNo_xpm);
   iconImage = TtaCreatePixmapLogo (Image_xpm);
   iconImageNo = TtaCreatePixmapLogo (ImageNo_xpm);
   iconDL = TtaCreatePixmapLogo (DL_xpm);
   iconDLNo = TtaCreatePixmapLogo (DLNo_xpm);
   iconLink = TtaCreatePixmapLogo (Link_xpm);
   iconLinkNo = TtaCreatePixmapLogo (LinkNo_xpm);
   iconTable = TtaCreatePixmapLogo (Table_xpm);
   iconTableNo = TtaCreatePixmapLogo (TableNo_xpm);
#endif /* !_MOTIF */
    
#ifdef AMAYA_PLUGIN

  #ifdef _GTK
     iconPlugin = (ThotIcon) TtaCreatePixmapLogo (Plugin_xpm);
  #endif /* _GTK */

  #ifdef _MOTIF     
     iconPlugin = TtaCreatePixmapLogo (Plugin_xpm);
  #endif /* _MOTIF */
     
#endif /* AMAYA_PLUGIN */


   /* init transformation callback */
   TtaSetTransformCallback ((Func) TransformIntoType);
   TargetName = NULL;
   TtaSetAccessKeyFunction ((Proc) AccessKeyHandler);
   TtaSetEntityFunction ((Proc) MapEntityByCode);
   TtaSetCopyAndCutFunction ((Proc) RegisterURLSavedElements);
   /* Initialize the Amaya user and tmp directories */
   s = TtaGetEnvString ("APP_TMPDIR");
   if (!CheckMakeDirectory (s, TRUE))
     /* try to use the default value */
     {
       s = TtaGetDefEnvString ("APP_TMPDIR");
       if (CheckMakeDirectory (s, TRUE))
	 /* make it the current user one */
	 TtaSetEnvString ("APP_TMPDIR", s, TRUE);
       else
	 /* didn't work, so we exit */
	 {
	   sprintf (TempFileDirectory,
		    TtaGetMessage (AMAYA, AM_CANNOT_CREATE_DIRECTORY), s);
#ifdef _WINDOWS
	   MessageBox (NULL, TempFileDirectory, "Error", MB_OK);
#endif /* _WINDOWS */
     
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
	   fprintf (stderr, TempFileDirectory);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
     
	   exit (1);
	 }
     }

   /* add the temporary directory in document path */
   strcpy (TempFileDirectory, s);
   TtaAppendDocumentPath (TempFileDirectory);
 
#ifdef _WINDOWS
   s = TtaGetEnvString ("APP_HOME");
   if (!CheckMakeDirectory (s, TRUE))
     /* didn't work, so we exit */
     {
       sprintf (TempFileDirectory,
		TtaGetMessage (AMAYA, AM_CANNOT_CREATE_DIRECTORY), s);
       MessageBox (NULL, TempFileDirectory, "Error", MB_OK);
       exit (1);
     }
#endif /* _WINDOWS */
   /*
    * Build the User preferences file name:
    * $HOME/.amaya/amaya.css on Unix platforms
    * $HOME\amaya\amaya.css on Windows platforms
    */
   ptr = (char *)TtaGetMemory (MAX_LENGTH);
   sprintf (ptr, "%s%c%s.css", s, DIR_SEP, HTAppName);
   UserCSS = TtaStrdup (ptr);

   /* Initialize environment variables if they are not defined */
   TtaSetEnvBoolean ("SECTION_NUMBERING", FALSE, FALSE);
   TtaSetEnvBoolean ("SHOW_BUTTONS", TRUE, FALSE);
   TtaSetEnvBoolean ("SHOW_ADDRESS", TRUE, FALSE);
   TtaSetEnvBoolean ("SHOW_MAP_AREAS", FALSE, FALSE);
   TtaSetEnvBoolean ("SHOW_TARGET", FALSE, FALSE);
   TtaSetEnvBoolean ("LOAD_IMAGES", TRUE, FALSE);
   TtaSetEnvBoolean ("LOAD_OBJECTS", TRUE, FALSE);
   TtaSetEnvBoolean ("LOAD_CSS", TRUE, FALSE);
   TtaSetEnvBoolean ("SEND_REFERER", FALSE, FALSE);
   /* get current value */
   TtaGetEnvBoolean ("SECTION_NUMBERING", &numbering);
   TtaGetEnvBoolean ("SHOW_BUTTONS", &bt);
   TtaGetEnvBoolean ("SHOW_ADDRESS", &add);
   TtaGetEnvBoolean ("SHOW_MAP_AREAS", &map);
   /* Create and intialize resources needed for each document */
   /* Style sheets are strored in directory .amaya/0 */
   for (i = 0; i < DocumentTableLength; i++)
     {
       /* initialize document table */
       DocumentURLs[i] = NULL;
       DocumentTypes[i] = docHTML;
       DocumentSource[i] = 0;
       DocumentMeta[i] = NULL;
       ReadOnlyDocument[i] = FALSE;
       SNumbering[i] = numbering;
       MapAreas[i] = map;
       SButtons[i] = bt;
       SAddress[i] = add;
       /* initialize history */
       InitDocHistory (i);
     }

   /* allocate working buffers */
   LastURLName = (char *)TtaGetMemory (MAX_LENGTH);
   LastURLName[0] = EOS;
   DirectoryName = (char *)TtaGetMemory (MAX_LENGTH);
   SavedDocumentURL = NULL;

   /* set path on current directory */
   getcwd (DirectoryName, MAX_LENGTH);
   DocumentName = (char *)TtaGetMemory (MAX_LENGTH);
   memset (DocumentName, EOS, MAX_LENGTH);
   SavePath = (char *)TtaGetMemory (MAX_LENGTH);
   SavePath[0] = EOS;
   SaveName = (char *)TtaGetMemory (MAX_LENGTH);
   SaveName[0] = EOS;
   ObjectName = (char *)TtaGetMemory (MAX_LENGTH);
   ObjectName[0] = EOS;
   SaveImgsURL = (char *)TtaGetMemory (MAX_LENGTH);
   SaveImgsURL[0] = EOS;
   strcpy (ScanFilter, "*.*htm*");
   SaveAsHTML = TRUE;
   SaveAsXML = FALSE;
   SaveAsText = FALSE;
   CopyImages = FALSE;
   UpdateURLs = FALSE;
   SavingFile = (char *)TtaGetMemory (MAX_LENGTH);
   AttrHREFvalue = (char *)TtaGetMemory (MAX_LENGTH);
   AttrHREFvalue[0] = EOS;

#ifdef WITH_SOCKS
   SOCKSinit ("amaya");
#endif

   /* initialize parser mapping table and HTLib */
   InitMapping ();

   /* Define the backup function */
   TtaSetBackup (BackUpDocs);

   /* Define the auto-save function */
   TtaSetAutoSave ((Proc)GenerateAutoSavedDoc);
   /* Define the auto-save interval */
   TtaGetEnvInt ("AUTO_SAVE", &AutoSave_Interval);
   TtaSetDocumentBackUpInterval (AutoSave_Interval);

   TtaSetApplicationQuit (FreeAmayaStructures);
   TtaSetDocStatusUpdate ((Proc) DocStatusUpdate);
   AMAYA = TtaGetMessageTable ("amayamsg", AMAYA_MSG_MAX);
   /* allocate callbacks for amaya */
   BaseDialog = TtaSetCallback ((Proc)CallbackDialogue, MAX_REF);
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
   /* initialize the configuration menu context */
   InitConfMenu ();
#ifdef ANNOTATIONS
   ANNOT_Init ();
#endif /* ANNOTATIONS */
   /* initialize the libwww */
   QueryInit ();
   /* initialize the Amaya cache */
   InitAmayaCache ();
  /* now we know if it's a unique instance */
#ifdef BOOKMARKS
   BM_Init ();
#endif /* BOOKMARKS */

#ifdef _WINDOWS
   sprintf (LostPicturePath, "%s\\amaya\\lost.gif",
	     TtaGetEnvString ("THOTDIR"));
#endif /* _WINDOWS */
   
#ifdef _GTK
   sprintf (LostPicturePath, "%s/amaya/lost.gif",
	     TtaGetEnvString ("THOTDIR"));   
#endif /* _GTK */

   InitMathML ();
#ifdef _SVG
   InitSVG ();
   InitSVGAnim ();
   InitSVGLibraryManagerStructure ();
   InitLibrary();
#endif /* _SVG */
/* MKP: disable "Cooperation" menu if DAV is not defined or
 *      initialize davlib module otherwise */
#ifdef DAV
   InitDAV ();
#else /* DAV */
   DAVLibEnable = FALSE;
#endif /* DAV */
   URL_list = NULL;
   URL_list_len = 0;
   InitStringForCombobox ();

   AutoSave_list = NULL;
   AutoSave_list_len = 0;
   InitAutoSave ();

   CurrentDocument = 0;
   DocBook = 0;
   InNewWindow = FALSE;
   restoredDoc = RestoreAmayaDocs ();
   s = NULL;
   if (restoredDoc)
     {
       /* old documents are restored */
       TtaFreeMemory (ptr);
       return;
     }
   if (appArgc % 2 == 0)
     /* The last argument in the command line is the document to be opened */
     s = appArgv[appArgc - 1];
   if (s == NULL || s[0] == EOS)
     /* no argument: display the Home Page */
     s = TtaGetEnvString ("HOME_PAGE");
   if (URL_list && s && !strcasecmp (s, "$PREV"))
     {
       /* no argument and no Home: display the previous open URI */
       for (i = 0; URL_list[i] != EOS && URL_list[i] != EOL; i++)
	 ptr[i] = URL_list[i];
       ptr[i] = EOS;
       s = ptr;
     }

   if (s == NULL || s[0] == EOS)
      /* no argument, no Home, and no previous page: display default Amaya URL */
     GoToHome (0, 1);
   else if (IsW3Path (s))
     {
       /* it's a remote document */
       strcpy (LastURLName, s);
       CallbackDialogue (BaseDialog + OpenForm, INTEGER_DATA, (char *) 1);
     }
   else
     {
       NormalizeFile (s, LastURLName, AM_CONV_NONE);
       if (IsW3Path (LastURLName))
	 {
	   /* if the command line paremeter 
	      is a url without http://
	      it's a remote document or 
	      a new file (doesn't exist yet )
	      in the current path */
	   strcpy (s, LastURLName);
	   CallbackDialogue (BaseDialog + OpenForm, INTEGER_DATA, (char *) 1);
	 }
       else
	 {
	   /* check if it is an absolute or a relative name */

#ifdef _WINDOWS
	   if (LastURLName[0] == DIR_SEP || LastURLName[1] == ':')
	     /* it is an absolute name */
	     TtaExtractName (LastURLName, DirectoryName, DocumentName);
	   else
	     /* it is a relative name */
	     strcpy (DocumentName, LastURLName);
#endif /* !_WINDOWS */

#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
	   if (LastURLName[0] == DIR_SEP)
	     /* it is an absolute name */
	     TtaExtractName (LastURLName, DirectoryName, DocumentName);
	   else
	     /* it is a relative name */
	     strcpy (DocumentName, LastURLName);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */

     /* start with the local document */
	   LastURLName[0] = EOS;
	   CallbackDialogue (BaseDialog + OpenForm, INTEGER_DATA, (char *) 1);
	 }
     }
   TtaFreeMemory (ptr);
   ptr = NULL;
   Loading_method = CE_ABSOLUTE;
}

/*----------------------------------------------------------------------
  ChangeAttrOnRoot
  If the root element of the document does not have an attribute of
  type attrNum, create one.
  If the root has such an attribute, delete it.
  ----------------------------------------------------------------------*/
void ChangeAttrOnRoot (Document doc, int attrNum)
{
  Element	       root, el;
  AttributeType        attrType;
  Attribute	       attr;
  int		       position;
  int		       distance;
  ThotBool	       docModified;

  docModified = TtaIsDocumentModified (doc);
  root = TtaGetRootElement (doc);
  /* get the current position in the document */
  position = RelativePosition (doc, &distance);
  TtaSetDisplayMode (doc, NoComputedDisplay);
  attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
  attrType.AttrTypeNum = attrNum;
  attr = TtaGetAttribute (root, attrType);
  if (attr != NULL)
    /* the root element has that attribute. Remove it */
    TtaRemoveAttribute (root, attr, doc);
  else
    /* the root element does not have that attribute. Create it */
    {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (root, attr, doc);
      TtaSetAttributeValue (attr, 1, root, doc);
    }
  if (!docModified)
    {
      TtaSetDocumentUnmodified (doc);
      /* switch Amaya buttons and menus */
      DocStatusUpdate (doc, docModified);
    }
  TtaSetDisplayMode (doc, DisplayImmediately);
  /* show the document at the same position */
  el = ElementAtPosition (doc, position);
  TtaShowElement (doc, 1, el, distance);
}

/*----------------------------------------------------------------------
  ShowMapAreas
  Execute the "Show Map Areas" command
  ----------------------------------------------------------------------*/
void ShowMapAreas (Document doc, View view)
{
#ifdef _WINDOWS
  int frame = GetWindowNumber (doc, view);

  if (frame == 0 || frame > 10)
    TtaError (ERR_invalid_parameter);
  else
    {
      HMENU hmenu = WIN_GetMenu (frame); 
      if (!MapAreas[doc])
	{
          CheckMenuItem (hmenu, menu_item, MF_BYCOMMAND | MF_CHECKED); 
          MapAreas[doc] = TRUE;
	}
      else
	{
	  hmenu = WIN_GetMenu (frame); 
	  CheckMenuItem (hmenu, menu_item, MF_BYCOMMAND | MF_UNCHECKED); 
	  MapAreas[doc] = FALSE;
	}
   }
#endif /* _WINDOWS */
  
#if defined(_MOTIF) || defined(_GTK) || defined(_WX)  
  MapAreas[doc] = !MapAreas[doc];
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
  
  ChangeAttrOnRoot (doc, HTML_ATTR_ShowAreas);
}

/*----------------------------------------------------------------------
  ShowButtons
  Execute the "Show Buttons" command
  ----------------------------------------------------------------------*/
void ShowButtons (Document doc, View view)
{

#ifdef _WINDOWS
  int frame = GetWindowNumber (doc, view);

  if (frame == 0 || frame > 10)
    TtaError (ERR_invalid_parameter);
  else
    {
      HMENU hmenu = WIN_GetMenu (frame); 
      if (!SButtons[doc])
	{
          CheckMenuItem (hmenu, menu_item, MF_BYCOMMAND | MF_CHECKED); 
          SButtons[doc] = TRUE;
	}
      else
	{
	  hmenu = WIN_GetMenu (frame); 
	  CheckMenuItem (hmenu, menu_item, MF_BYCOMMAND | MF_UNCHECKED); 
	  SButtons[doc] = FALSE;
	}
   }
#endif /* _WINDOWS */
  
#if defined(_MOTIF) || defined(_GTK) || defined(_WX)  
  SButtons[doc] = !SButtons[doc];
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
  
  TtaSetToggleItem (doc, 1, Views, TShowButtonbar, SButtons[doc]);
  TtcSwitchButtonBar (doc, view);
}

/*----------------------------------------------------------------------
  ShowAddress
  Execute the "Show Address" command
  ----------------------------------------------------------------------*/
void ShowAddress (Document doc, View view)
{
  
#ifdef _WINDOWS
  int frame = GetWindowNumber (doc, view);

  if (frame == 0 || frame > 10)
    TtaError (ERR_invalid_parameter);
  else
    {
      HMENU hmenu = WIN_GetMenu (frame); 
      if (!SAddress[doc])
	{
          CheckMenuItem (hmenu, menu_item, MF_BYCOMMAND | MF_CHECKED); 
          SAddress[doc] = TRUE;
	}
      else
	{
	  hmenu = WIN_GetMenu (frame); 
	  CheckMenuItem (hmenu, menu_item, MF_BYCOMMAND | MF_UNCHECKED); 
	  SAddress[doc] = FALSE;
	}
   }
#endif /* _WINDOWS */
  
#if defined(_MOTIF) || defined(_GTK) || defined(_WX)  
  SAddress[doc] = !SAddress[doc];
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
  
  TtaSetToggleItem (doc, 1, Views, TShowTextZone, SAddress[doc]);
  TtcSwitchCommands (doc, view);
}

/*----------------------------------------------------------------------
  SectionNumbering
  Execute the "Section Numbering" command
  ----------------------------------------------------------------------*/
void SectionNumbering (Document doc, View view)
{
  
#ifdef _WINDOWS
  int frame = GetWindowNumber (doc, view);

  if (frame == 0 || frame > 10)
    TtaError (ERR_invalid_parameter);
  else
    {
      HMENU hmenu = WIN_GetMenu (frame); 
      if (!SNumbering[doc])
	{
          CheckMenuItem (hmenu, menu_item, MF_BYCOMMAND | MF_CHECKED); 
          SNumbering[doc] = TRUE;
	}
      else
	{
	  hmenu = WIN_GetMenu (frame); 
	  CheckMenuItem (hmenu, menu_item, MF_BYCOMMAND | MF_UNCHECKED); 
	  SNumbering[doc] = FALSE;
	}
   }
#endif /* _WINDOWS */
  
#if defined(_MOTIF) || defined(_GTK) || defined(_WX)  
  SNumbering[doc] = !SNumbering[doc];
  TtaSetToggleItem (doc, 1, Special, TSectionNumber, SNumbering[doc]);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
  
  ChangeAttrOnRoot (doc, HTML_ATTR_SectionNumbering);
}

/*----------------------------------------------------------------------
  MakeIDMenu
  A menu for adding or removing ID attributes in a document
  ----------------------------------------------------------------------*/
void MakeIDMenu (Document doc, View view)
{
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
  int i;
  char    s[MAX_LENGTH];
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */

  /* initialize the global variables */
  IdStatus[0] = EOS;
  IdDoc = doc;

  /* Create the dialogue form */
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
  i = 0;
  strcpy (&s[i], TtaGetMessage (AMAYA, ADD_ID));
  i += strlen (&s[i]) + 1;
  strcpy (&s[i], TtaGetMessage (AMAYA, REMOVE_ID));
  TtaNewSheet (BaseDialog + MakeIdMenu,
	       TtaGetViewFrame (doc, view),
	       TtaGetMessage (AMAYA, ADD_REMOVE_ID),
	       2, s, FALSE, 6, 'L', D_DONE);
  TtaNewTextForm (BaseDialog + mElemName,
		  BaseDialog + MakeIdMenu,
		  TtaGetMessage (AMAYA, ENTER_ELEMENT_NAME),
		  10, 1, FALSE);
  TtaSetTextForm (BaseDialog + mElemName, IdElemName);
  /* apply operation in */
  i = 0;
  sprintf (&s[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_IN_WHOLE_DOC));
  i += strlen (&s[i]) + 1;
  sprintf (&s[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_WITHIN_SEL));
  TtaNewSubmenu (BaseDialog + mIdUseSelection,
		 BaseDialog + MakeIdMenu, 0,
		 TtaGetMessage (AMAYA, APPLY_OPERATION),
		 2, s,
		 NULL, FALSE);
  /* status label */
  TtaNewLabel (BaseDialog + mIdStatus,
	       BaseDialog + MakeIdMenu,
	       " ");
  /* select the current radio button */
  TtaSetMenuForm (BaseDialog + mIdUseSelection, IdApplyToSelection);
  TtaSetDialoguePosition ();
  TtaShowDialogue (BaseDialog + MakeIdMenu, TRUE);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */

#ifdef _WINDOWS  
  CreateMakeIDDlgWindow (TtaGetViewFrame (doc, view));
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void HelpAmaya (Document document, View view)
{
   char                  localname[MAX_LENGTH];
#ifdef AMAYA_DEBUG
   Element             el;
   View                structView, altView, linksView, tocView;
   int                 n;
   FILE               *list;

  /* get the root element */
   strcpy (localname, TempFileDirectory);
   strcat (localname, DIR_STR);
   strcat (localname, "tree.debug");
   list = fopen (localname, "w");
   el = TtaGetMainRoot (document);
   TtaListAbstractTree (el, list);
   fclose (list);
   strcpy (localname, TempFileDirectory);
   strcat (localname, DIR_STR);
   strcat (localname, "view.debug");
   list = fopen (localname, "w");
   TtaListView (document, view, list);
   fclose (list);
   strcpy (localname, TempFileDirectory);
   strcat (localname, DIR_STR);
   strcat (localname, "boxes.debug");
   list = fopen (localname, "w");
   TtaListBoxes (document, view, list);
   fclose (list);
   structView = TtaGetViewFromName (document, "Structure_view");
   if (structView != 0 && TtaIsViewOpen (document, structView))
     {
       strcpy (localname, TempFileDirectory);
       strcat (localname, DIR_STR);
       strcat (localname, "structview.debug");
       list = fopen (localname, "w");
       TtaListView (document, structView, list);
       fclose (list);
       strcpy (localname, TempFileDirectory);
       strcat (localname, DIR_STR);
       strcat (localname, "structboxes.debug");
       list = fopen (localname, "w");
       TtaListBoxes (document, structView, list);
       fclose (list);
     }
   altView = TtaGetViewFromName (document, "Alternate_view");
   if (altView != 0 && TtaIsViewOpen (document, altView))
     {
       strcpy (localname, TempFileDirectory);
       strcat (localname, DIR_STR);
       strcat (localname, "altview.debug");
       list = fopen (localname, "w");
       TtaListView (document, altView, list);
       fclose (list);
       strcpy (localname, TempFileDirectory);
       strcat (localname, DIR_STR);
       strcat (localname, "altboxes.debug");
       list = fopen (localname, "w");
       TtaListBoxes (document, altView, list);
       fclose (list);
     }
   linksView = TtaGetViewFromName (document, "Links_view");
   if (linksView != 0 && TtaIsViewOpen (document, linksView))
     {
       strcpy (localname, TempFileDirectory);
       strcat (localname, DIR_STR);
       strcat (localname, "linksview.debug");
       list = fopen (localname, "w");
       TtaListView (document, linksView, list);
       fclose (list);
       strcpy (localname, TempFileDirectory);
       strcat (localname, DIR_STR);
       strcat (localname, "linksboxes.debug");
       list = fopen (localname, "w");
       TtaListBoxes (document, linksView, list);
       fclose (list);
     }
   tocView = TtaGetViewFromName (document, "Table_of_contents");
   if (tocView != 0 && TtaIsViewOpen (document, tocView))
     {
       strcpy (localname, TempFileDirectory);
       strcat (localname, DIR_STR);
       strcat (localname, "tocview.debug");
       list = fopen (localname, "w");
       TtaListView (document, tocView, list);
       fclose (list);
       strcpy (localname, TempFileDirectory);
       strcat (localname, DIR_STR);
       strcat (localname, "tocboxes.debug");
       list = fopen (localname, "w");
       TtaListBoxes (document, tocView, list);
       fclose (list);
     }
   /* list now CSS rules */
   strcpy (localname, TempFileDirectory);
   strcat (localname, DIR_STR);
   strcat (localname, "style.debug");
   list = fopen (localname, "w");
   TtaListStyleSchemas (document, list);
   fclose (list);
   /* list CSS rules applied to the current selection */
   strcpy (localname, TempFileDirectory);
   strcat (localname, DIR_STR);
   strcat (localname, "style_element.debug");
   list = fopen (localname, "w");
   n = TtaListStyleOfCurrentElement (document, list);
   if (n == 0)
     {
       fprintf (list, TtaGetMessage (AMAYA, AM_NO_STYLE_FOR_ELEM));
       fprintf (list, "\n");
     }
   fclose (list);
   /* list now shortcuts */
   strcpy (localname, TempFileDirectory);
   strcat (localname, DIR_STR);
   strcat (localname, "shortcuts.debug");
   list = fopen (localname, "w");
   TtaListShortcuts (document, list);
   fclose (list);
#endif /* AMAYA_DEBUG */

#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
   TtaNewDialogSheet (BaseDialog + AboutForm, TtaGetViewFrame (document, view),
		      HTAppName, 1, TtaGetMessage(LIB, TMSG_LIB_CONFIRM), TRUE, 1,'L');
#endif  /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
   
   strcpy (localname, HTAppName);
   strcat (localname, " - ");
   strcat (localname, HTAppVersion);
   strcat (localname, "     ");
   strcat (localname, HTAppDate);
   
#if defined(_MOTIF) || defined(_GTK) || defined(_WX) 
   TtaNewLabel(BaseDialog + Version, BaseDialog + AboutForm, localname);
   TtaNewLabel(BaseDialog + About1, BaseDialog + AboutForm,
	       TtaGetMessage(AMAYA, AM_ABOUT1));
   TtaNewLabel(BaseDialog + About2, BaseDialog + AboutForm,
	       TtaGetMessage(AMAYA, AM_ABOUT2));
   TtaShowDialogue (BaseDialog + AboutForm, FALSE);
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
   
#ifdef _WINDOWS
   CreateHelpDlgWindow (TtaGetViewFrame (document, view), localname,
			TtaGetMessage(AMAYA, AM_ABOUT1),
			TtaGetMessage(AMAYA, AM_ABOUT2));
#endif /* _WINDOWS */
   
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void HelpAtW3C (Document document, View view)
{
  char      localname[MAX_LENGTH];

#ifdef LC
  TtaShowNamespaceDeclarations (document);
#endif /* LC */
  strcpy (localname, AMAYA_PAGE_DOC);
  strcat (localname, "BinDist.html");
  document = GetAmayaDoc (localname, NULL, 0, 0, (ClickEvent)CE_HELP, FALSE, NULL,
			  NULL, TtaGetDefaultCharset ());
  InitDocHistory (document);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
static void DisplayHelp (int doc, int index)
{
  Document    document;
  char        localname[MAX_LENGTH];
  char       *s, *lang;

  lang = TtaGetVarLANG ();
  s = TtaGetEnvString ("THOTDIR");
  if (s != NULL)
    {
      /* get the documentation in the current language */
      sprintf (localname, "%s%cdoc%chtml%c%s.%s", s, DIR_SEP, DIR_SEP,
		DIR_SEP, Manual[index], lang);

      if (!TtaFileExist (localname))
      /* get the standard english documentation */
	sprintf (localname, "%s%cdoc%chtml%c%s", s, DIR_SEP, DIR_SEP,
		  DIR_SEP, Manual[index]);
    }
  document = GetAmayaDoc (localname, NULL, 0, 0, (ClickEvent)CE_HELP, FALSE, NULL,
			  NULL, TtaGetDefaultCharset ());
  InitDocHistory (document);
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpIndex (Document document, View view)
{
  DisplayHelp (document, INDEX);
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpBrowsing (Document document, View view)
{
  DisplayHelp (document, BROWSING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpSelecting (Document document, View view)
{
  DisplayHelp (document, SELECTING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpSearching (Document document, View view)
{
  DisplayHelp (document, SEARCHING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpViews (Document document, View view)
{
  DisplayHelp (document, VIEWS);
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpDocument (Document document, View view)
{
  DisplayHelp (document, DOCUMENT);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpCreating (Document document, View view)
{
  DisplayHelp (document, CREATING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpLinks (Document document, View view)
{
  DisplayHelp (document, LINKS);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpChanging (Document document, View view)
{
  DisplayHelp (document, CHANGING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpTables (Document document, View view)
{
  DisplayHelp (document, TABLES);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpMath (Document document, View view)
{
  DisplayHelp (document, MATH);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpSVG (Document document, View view)
{
  DisplayHelp (document, SVG);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpEditChar (Document document, View view)
{
  DisplayHelp (document, EDITCHAR);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpXml (Document document, View view)
{
  DisplayHelp (document, XML);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpImageMaps (Document document, View view)
{
  DisplayHelp (document, IMAGEMAPS);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpStyleSheets (Document document, View view)
{
  DisplayHelp (document, CSS);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpAttributes (Document document, View view)
{
  DisplayHelp (document, ATTRIBUTES);
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpSpellChecking (Document document, View view)
{
  DisplayHelp (document, SPELLCHECKING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpPublishing (Document document, View view)
{
  DisplayHelp (document, PUBLISHING);
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpWebDAV (Document document, View view)
{
  DisplayHelp (document, WEBDAV);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpPrinting (Document document, View view)
{
  DisplayHelp (document, PRINTING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpNumbering (Document document, View view)
{
  DisplayHelp (document, NUMBERING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpMakeBook (Document document, View view)
{
  DisplayHelp (document, MAKEBOOK);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpAnnotation (Document document, View view)
{
  DisplayHelp (document, ANNOTATE);
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpBookmarks (Document document, View view)
{
  DisplayHelp (document, BOOK_MARKS);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpConfigure (Document document, View view)
{
  DisplayHelp (document, CONFIGURE);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpShortCuts (Document document, View view)
{
  DisplayHelp (document, SHORTCUTS);
}

/*----------------------------------------------------------------------
Accessibility help page. Added by Charles McCN oct 99
 -----------------------------------------------------------------------*/
void HelpAccess (Document document, View view)
{
  DisplayHelp (document, ACCESS);
}


/*----------------------------------------------------------------------
   CheckAmayaClosed closes the application when there is any more
   opened document
  ----------------------------------------------------------------------*/
void CheckAmayaClosed ()
{
  int                i;

  /* is it the last loaded document ? */
  i = 1;
  while (i < DocumentTableLength && DocumentURLs[i] == NULL)
    i++;
  
  if (i == DocumentTableLength)
    {
      /* remove images loaded by shared CSS style sheets */
      RemoveDocumentImages (0);
#ifdef _SVG
      SVGLIB_FreeDocumentResource ();
#endif /* _SVG */
      /* remove the AutoSave file */
      TtaQuit ();
    }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CloseDocument (Document doc, View view)
{
  if (DocumentURLs[doc])
    TtcCloseDocument (doc, view);
  if (!W3Loading)
    CheckAmayaClosed ();

}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaClose (Document document, View view)
{
   int          i;
   ThotBool     documentClosed;

   /* invalid current loading */
   W3Loading = 0;

   documentClosed = TRUE;
   /* free each loaded document */
   for (i = 1; i < DocumentTableLength; i++)
      if (DocumentURLs[i] != NULL)
	{
	  TtcCloseDocument (i, 1);
	  documentClosed = (DocumentURLs[i] == NULL);
	  if (!documentClosed)
	    return;
	}
   /* remove images loaded by shared CSS style sheets */
   RemoveDocumentImages (0);
#ifdef _SVG
   SVGLIB_FreeDocumentResource ();
#endif /* _SVG */
   TtaQuit ();
}


/*----------------------------------------------------------------------
  AddURLInCombobox adds the new URL in the string for combobox
  Store that URL into the file only if keep is TRUE.
  ----------------------------------------------------------------------*/
void AddURLInCombobox (char *url_utf8, char *form_data, ThotBool keep)
{
  char     *urlstring, *app_home, *ptr, *url;
  int       i, j, len, nb, end;
  FILE     *file = NULL;

  if (url_utf8 == NULL || url_utf8[0] == EOS)
    return;
  if (form_data && form_data[0] != EOS)
    {
      ptr = (char *)TtaGetMemory (strlen (url_utf8) + strlen (form_data) + 2);
      sprintf (ptr, "%s?%s", url_utf8, form_data);
      url = (char *)TtaConvertMbsToByte ((unsigned char *)ptr, TtaGetDefaultCharset ());
      TtaFreeMemory (ptr);
    }
  else
    url = (char *)TtaConvertMbsToByte ((unsigned char *)url_utf8, TtaGetDefaultCharset ());
  urlstring = (char *) TtaGetMemory (MAX_LENGTH);
  /* open the file list_url.dat into APP_HOME directory */
  app_home = TtaGetEnvString ("APP_HOME");
  sprintf (urlstring, "%s%clist_url.dat", app_home, DIR_SEP);
  /* keep the previous list */
  ptr = URL_list;
  /* create a new list */
  len = strlen (url) + 1;
  i = 0;
  j = len;
  nb = 1;
  URL_list_len = URL_list_len + len + 1;
  URL_list = (char *)TtaGetMemory (URL_list_len);  
  if (keep)
    file = TtaWriteOpen (urlstring);
  *urlstring = EOS;
  if (!keep || file)
    {
      /* put the new url */
      strcpy (URL_list, url);
      if (keep)
	fprintf (file, "\"%s\"\n", url);
      if (ptr && *ptr != EOS)
	{
	  /* now write other urls */
	  while (ptr[i] != EOS && nb < MAX_URL_list)
	    {
	      end = strlen (&ptr[i]) + 1;
	      if ((URL_list_keep || i != 0) &&
		  (end != len || strncmp (url, &ptr[i], len)))
		{
		  /* add the newline between two urls */
		  strcpy (&URL_list[j], &ptr[i]);
		  if (keep)
		    fprintf (file, "\"%s\"\n", &ptr[i]);
		  j += end;
		  nb++;
		}
	      i += end;
	    }
	}
	  
      URL_list[j] = EOS;
      URL_list_keep = keep;
      if (keep)
	TtaWriteClose (file);
    }
  TtaFreeMemory (ptr);
  TtaFreeMemory (urlstring);
  TtaFreeMemory (url);
}

/*----------------------------------------------------------------------
  InitStringForCombobox
  Initializes the URLs string for combobox
  ----------------------------------------------------------------------*/
void InitStringForCombobox ()
{
  unsigned char     *urlstring, c;
  char              *app_home;
  FILE              *file;
  int                i, nb, len;

  /* remove the previous list */
  TtaFreeMemory (URL_list);
  URL_list_keep = TRUE;
  urlstring = (unsigned char *) TtaGetMemory (MAX_LENGTH);
  /* open the file list_url.dat into APP_HOME directory */
  app_home = TtaGetEnvString ("APP_HOME");
  sprintf ((char *)urlstring, "%s%clist_url.dat", app_home, DIR_SEP);
  file = TtaReadOpen ((char *)urlstring);
  *urlstring = EOS;
  if (file)
    {
      /* get the size of the file */
      fseek (file, 0L, 2);	/* end of the file */
      URL_list_len = ftell (file) + MAX_URL_list + 4;
      URL_list = (char *)TtaGetMemory (URL_list_len);
      URL_list[0] = EOS;
      fseek (file, 0L, 0);	/* beginning of the file */
      /* initialize the list by reading the file */
      i = 0;
      nb = 0;
      while (TtaReadByte (file, &c))
	{
	  if (c == '"')
	    {
	      len = 0;
	      urlstring[len] = EOS;
	      while (len < MAX_LENGTH && TtaReadByte (file, &c) && c != EOL)
		{
		  if (c == '"')
		    urlstring[len] = EOS;
		  else if (c == 13)
		    urlstring[len] = EOS;
		  else
		    urlstring[len++] = (char)c;
		}
	      if (i > 0 && len)
		/* add an EOS between two urls */
		URL_list[i++] = EOS;
	      if (len)
		{
		  nb++;
		  strcpy ((char *)&URL_list[i], (char *)urlstring);
		  i += len;
		}
	    }
	}
      URL_list[i + 1] = EOS;
      TtaReadClose (file);
    }
  TtaFreeMemory (urlstring);
}


/*----------------------------------------------------------------------
  RemoveDocFromSaveList remove the file from the AutoSave list
  ----------------------------------------------------------------------*/
void RemoveDocFromSaveList (char *save_name, char *initial_url, int doctype)
{
  char     *urlstring, *app_home, *ptr, *name, *url, *list_item;
  char     *ptr_end, *ptr_beg;
  int       i, j, len, nb, end;
  FILE     *file = NULL;

  if (save_name == NULL || save_name[0] == EOS)
    return;
  if (initial_url == NULL || initial_url[0] == EOS)
    return;

  name = (char *)TtaConvertMbsToByte ((unsigned char *)save_name,
				      TtaGetDefaultCharset ());
  url = (char *)TtaConvertMbsToByte ((unsigned char *)initial_url,
				     TtaGetDefaultCharset ());
  /* keep the previous list */
  ptr = AutoSave_list;

  /* create a new list */
  AutoSave_list = (char *)TtaGetMemory (AutoSave_list_len + 1);  
  len = strlen (url) + strlen (name) + 1;
  len += 17; /*doctype + quotation marks + spaces */
  list_item  = (char *)TtaGetMemory (len);
  sprintf (list_item, "\"%s\" \"%s\" %d", name, url, doctype);
  /* open the file AutoSave.dat into APP_HOME directory */
  app_home = TtaGetEnvString ("APP_HOME");
  urlstring = (char *) TtaGetMemory (MAX_LENGTH);
  sprintf (urlstring, "%s%cAutoSave.dat", app_home, DIR_SEP);

  if (TtaFileExist (urlstring))
    {
      file = TtaWriteOpen (urlstring);
      if (file && AutoSave_list)
	{
	  i = 0;
	  j = 0;
	  nb = 0;
	  /* remove the line (write other urls) */
	  if (ptr && *ptr != EOS)
	    {
	      while (ptr[i] != EOS && nb <= MAX_AutoSave_list)
		{
		  end = strlen (&ptr[i]) + 1;
		  ptr_end = strrchr (&ptr[i], '\"');
		  if (ptr_end)
		    {
		      *ptr_end = EOS;
		      ptr_beg = strrchr (&ptr[i], '\"');
		      if (ptr_beg)
			{
			  ptr_beg++;
			  if (strcmp (url, ptr_beg))
			    {
			      /* keep this entry */
			      *ptr_end = '\"';
			      strcpy (&AutoSave_list[j], &ptr[i]);
			      AutoSave_list_len += end;
			      fprintf (file, "%s\n", &ptr[i]);
			      j += end;
			      nb++;
			    }
			}
		      *ptr_end = '\"';
		    }
		  i += end;
		}
	    }
	  AutoSave_list[j] = EOS;
	  TtaWriteClose (file);
	  /* remove the backup file */
	  if (j == 0 && TtaFileExist (urlstring))
	    {
	      TtaFileUnlink (urlstring);
	      AutoSave_list = NULL;
	      TtaFreeMemory (AutoSave_list);
	      AutoSave_list_len = 0;
	    }
	}
    }
  else
    {
      TtaFreeMemory (AutoSave_list);
      AutoSave_list = NULL;
      AutoSave_list_len = 0;
    }
  
  if (ptr)
    TtaFreeMemory (ptr);
  if (urlstring)
    TtaFreeMemory (urlstring);
  if (name)
    TtaFreeMemory (name);
  if (url)
    TtaFreeMemory (url);
  if (list_item)
    TtaFreeMemory (list_item);
}

/*----------------------------------------------------------------------
  AddDocInSaveList adds the new URL into the AutoSave list
  ----------------------------------------------------------------------*/
void AddDocInSaveList (char *save_name, char *initial_url, int doctype)
{
  char     *urlstring, *app_home, *ptr, *name, *url;
  char     *ptr_end, *ptr_beg;
  int       i, j, len, nb, end;
  FILE     *file = NULL;

  if (AutoSave_Interval == 0)
    return;
  if (save_name == NULL || save_name[0] == EOS)
    return;
  if (initial_url == NULL || initial_url[0] == EOS)
    return;

  name = (char *)TtaConvertMbsToByte ((unsigned char *)save_name, TtaGetDefaultCharset ());
  url = (char *)TtaConvertMbsToByte ((unsigned char *)initial_url, TtaGetDefaultCharset ());

  /* keep the previous list */
  ptr = AutoSave_list;
  /* create a new list */
  len = strlen (url) + strlen (name) + 1;
  len += 7; /*doctype + quotation marks + spaces */
  AutoSave_list = (char *)TtaGetMemory (AutoSave_list_len + len + 1);  

  /* open the file AutoSave.dat into APP_HOME directory */
  urlstring = (char *) TtaGetMemory (MAX_LENGTH);
  app_home = TtaGetEnvString ("APP_HOME");
  sprintf (urlstring, "%s%cAutoSave.dat", app_home, DIR_SEP);
  file = TtaWriteOpen (urlstring);
  *urlstring = EOS;

  if (file)
    {
      i = 0;
      j = len;
      nb = 1;
      /* put the new line */
      sprintf (AutoSave_list, "\"%s\" \"%s\" %d", name, url, doctype);
      AutoSave_list_len = len + 1;
      fprintf (file, "%s\n", AutoSave_list);
      if (ptr && *ptr != EOS)
	{
	  /* now write other urls */
	  while (ptr[i] != EOS && nb < MAX_AutoSave_list)
	    {
	      end = strlen (&ptr[i]) + 1;

	      ptr_end = strrchr (&ptr[i], '\"');
	      if (ptr_end)
		{
		  *ptr_end = EOS;
		  ptr_beg = strrchr (&ptr[i], '\"');
		  if (ptr_beg)
		    {
		      ptr_beg++;
		      if ((end != len) || (strcmp (url, ptr_beg)))
			{
			  /* copy the url */
			  *ptr_end = '\"';
			  strcpy (&AutoSave_list[j], &ptr[i]);
			  AutoSave_list_len += end;
			  fprintf (file, "%s\n", &ptr[i]);
			  j += end;
			  nb++;
			}
		    }
		  *ptr_end = '\"';
		}
	      i += end;
	    }
	}	  
      AutoSave_list[j] = EOS;
      TtaWriteClose (file);
    }
  if (ptr)
    TtaFreeMemory (ptr);
  if (urlstring)
    TtaFreeMemory (urlstring);
  if (name)
    TtaFreeMemory (name);
  if (url)
    TtaFreeMemory (url);
}

/*----------------------------------------------------------------------
  InitAutoSave list
  ----------------------------------------------------------------------*/
void InitAutoSave ()
{
  unsigned char     *urlstring, c;
  char              *app_home;
  FILE              *file;
  int                i, nb, len;

  /* remove the previous list */
  TtaFreeMemory (AutoSave_list);
  AutoSave_list = NULL;
  AutoSave_list_len = 0;
  urlstring = (unsigned char *) TtaGetMemory (MAX_LENGTH);
  /* open the file AutoSave.dat into APP_HOME directory */
  app_home = TtaGetEnvString ("APP_HOME");
  sprintf ((char *)urlstring, "%s%cAutoSave.dat", app_home, DIR_SEP);
  file = TtaReadOpen ((char *)urlstring);
  *urlstring = EOS;
  if (file)
    {
      /* get the size of the file */
      fseek (file, 0L, 2);	/* end of the file */
      AutoSave_list_len = ftell (file) + MAX_URL_list + 4;
      AutoSave_list = (char *)TtaGetMemory (AutoSave_list_len);
      URL_list[0] = EOS;
      fseek (file, 0L, 0);	/* beginning of the file */
      /* initialize the list by reading the file */
      i = 0;
      nb = 0;
      while (TtaReadByte (file, &c))
	{
	  if (c == '"')
	    {
	      len = 0;
	      urlstring[len] = EOS;
	      while (len < MAX_LENGTH && TtaReadByte (file, &c) && c != EOL)
		{
		  if (c == '"')
		    urlstring[len] = EOS;
		  else if (c == 13)
		    urlstring[len] = EOS;
		  else
		    urlstring[len++] = (char)c;
		}
	      if (i > 0 && len)
		/* add an EOS between two urls */
		AutoSave_list[i++] = EOS;
	      if (len)
		{
		  nb++;
		  strcpy ((char *)&AutoSave_list[i], (char *)urlstring);
		  i += len;
		}
	    }
	}
      AutoSave_list[i + 1] = EOS;
      TtaReadClose (file);
    }
  TtaFreeMemory (urlstring);
}
