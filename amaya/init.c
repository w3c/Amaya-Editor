/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2007
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

#ifdef _WX
  #include "wx/wx.h"
#endif /* _WX */

#undef THOT_EXPORT
#define THOT_EXPORT extern /* defined into css.c */
#include "amaya.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "css.h"
#include "trans.h"
#include "zlib.h"
#include "profiles.h"
#include "MENUconf.h"


#include "containers.h"
#include "insertelem_f.h"

#ifdef TEMPLATES
#include "templates.h"
#include "templates_f.h"
#include "templateDeclarations_f.h"
#endif /* TEMPLATES */

#ifdef _GTK
  #include "gtkdialogapi.h"
  extern char      LostPicturePath [MAX_LENGTH];
#endif /* _GTK */
#ifdef _WINDOWS
  #include <commctrl.h>
  extern char      LostPicturePath [MAX_LENGTH];
#endif /* _WINDOWS */
#ifdef _WINGUI
  #include "resource.h"
#endif /* _WINGUI */
#ifdef _WX
  #include "message_wx.h"
  #include "wxdialogapi_f.h"
  #include "windowtypes_wx.h"
  #include "paneltypes_wx.h"
  #include "registry_wx.h"
  #include "AmayaWindowIterator.h"
  #include "AmayaParams.h"
  
  #include "wxdialog/SendByMailDlgWX.h"
  #include "email.h"
  
  extern XmlEntity *pMathEntityTable;
#endif /* _WX */

#include "appdialogue_wx.h"

#if defined(_GTK)
#include "stopN.xpm"
#include "stopR.xpm"
#include "save.xpm"
#include "saveNo.xpm"
#include "find.xpm"
#include "Reload.xpm"
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
#endif /* #if defned(_GTK) */

#ifdef _WINGUI
/*
  #ifndef __GNUC__
  #include <direct.h>
  #endif 
*/
int             Window_Curs;
char            DocToOpen[MAX_LENGTH];
#endif /* _WINGUI */


static int          AmayaInitialized = 0;
static ThotBool     NewFile = FALSE;
static int          NewDocType = 0;
static int          NewDocProfile = 0;
static ThotBool     NewXML = TRUE;
static ThotBool     BADMimeType = FALSE;
static ThotBool     CriticConfirm = FALSE;
static ThotBool     CriticCheckError = FALSE;
/* the open document is the Amaya default page */
static ThotBool     WelcomePage = FALSE;
/* we have to mark the initial loading status to avoid to re-open the
   document view twice */
static int          Loading_method = CE_INIT;

#if defined(_GTK) || defined(_WX)
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
static ThotIcon       iconHome;
#ifdef _WX
static ThotIcon       iconLogo;
#endif /* _WX */
#endif /* #if defined(_GTK) || defined(_WX) */

#ifdef _WINGUI
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
#define iconHome      10
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

extern int       menu_item;

#include "wininclude.h"
#endif /* _WINGUI */
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
#include "HTMLpresentation_f.h"
#include "HTMLsave_f.h"
#include "HTMLtable_f.h"
#include "html2thot_f.h"
#include "Mathedit_f.h"
#include "MENUconf_f.h"
#include "init_f.h"
#include "query_f.h"
#include "styleparser_f.h"
#include "trans_f.h"
#include "transparse_f.h"
#include "UIcss_f.h"
#include "string.h"
#include "Xml2thot_f.h"
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
#ifdef _WX
#include "wxdialogapi_f.h"
#endif /* _WX */

#ifdef DAV
#define WEBDAV_EXPORT extern
#include "davlib.h"
#include "davlib_f.h"
#include "davlibRequests_f.h"
#include "davlibUI_f.h"
#endif /* DAV */

extern void InitMathML ();
extern void InitTemplates ();

/* the structure used for storing the context of the 
   GetAmayaDoc_callback function */
typedef struct _AmayaDoc_context
{
  Document   doc;
  Document   baseDoc;
  ThotBool   history;
  ThotBool   local_link;
  char      *target;
  char      *documentname; /* the document name */
  char      *initial_url;  /* initial loaded URL */
  char      *form_data;
  int        method;
  ThotBool   inNewWindow;
  TTcbf     *cbf;
  void      *ctx_cbf;
} AmayaDoc_context;

/* the structure used for storing the context of the 
   Reload_callback function */
typedef struct _RELOAD_context
{
  Document   newdoc;
  char      *documentname;
  char      *form_data;
  int        method;
  int        position;	/* volume preceding the the first element to be shown */
  int        distance; /* distance from the top of the window to the top of this
                   element (% of the window height) */
  int        visibility; /* register the current visibility */
  ThotBool   maparea; /* register the current maparea */
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

  TtaFreeMemory (me->form_data);
  me->form_data = NULL;
  TtaFreeMemory (me->initial_url);
  me->initial_url = NULL;
  TtaFreeMemory (me->content_type);
  me->content_type = NULL;
  TtaFreeMemory (me->charset);
  me->charset = NULL;
  TtaFreeMemory (me->content_length);
  me->content_length = NULL;
  TtaFreeMemory (me->content_location);
  me->content_location = NULL;
  TtaFreeMemory (me->full_content_location);
  me->full_content_location = NULL;
  TtaFreeMemory (me->reason);
  me->reason = NULL;
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
#if defined(_GTK)
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
  if (DocumentMeta[document] && DocumentMeta[document]->content_type)
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
  if (DocumentMeta[document] && DocumentMeta[document]->content_length)
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
#endif /* #if defined(_GTK) */
   
#ifdef _WINGUI
  CreateDocumentInfoDlgWindow (TtaGetViewFrame (document, view),
                               document);
#endif /* _WINGUI */
#ifdef _WX
  ThotBool created = CreateDocInfoDlgWX (BaseDialog + DocInfoForm,
                                         TtaGetViewFrame (document, view), document);
  if (created)
    {
      TtaSetDialoguePosition ();
      TtaShowDialogue (BaseDialog + DocInfoForm, FALSE);
      /* wait for an answer */
      TtaWaitShowDialogue ();
    }
#endif /* _WX */
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
      if (DocumentURLs[i] && DocumentTypes[i] != docTemplate)
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
  ExtractParameters extract parameters from document nane.        
  ----------------------------------------------------------------------*/
void ExtractParameters (char *aName, char *parameters)
{
  long int       lg, i;
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

      i = (long int) (oldptr) - (long int) (aName);	/* name length */
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
  result = TtaFileExist(filename);
  if (ptr)
    *ptr = '#';
  return result;
}

/*----------------------------------------------------------------------
  SetArrowButton
  Change the appearance of the Back (if back == TRUE) or Forward button
  for a given document.
  ----------------------------------------------------------------------*/
void SetArrowButton (Document doc, ThotBool back, ThotBool on)
{
  int		index;
  ThotBool      state;
  ThotIcon	picture;

  if (back)
    {
      index = iBack;
      if (on)
        {
          state   = TRUE;
          picture = iconBack;
          TtaSetItemOn (doc, 1, File, BBack);
          if (DocumentSource[doc])
            /* update the document source too */
            TtaSetItemOn (DocumentSource[doc], 1, File, BBack);
        }
      else
        {
          state = FALSE;
          picture = iconBackNo;
          TtaSetItemOff (doc, 1, File, BBack);
          if (DocumentSource[doc])
            /* update the document source too */
            TtaSetItemOff (DocumentSource[doc], 1, File, BBack);
        }
    }
  else
    {
      index = iForward;
      if (on)
        {
          state = TRUE;
          picture = iconForward;
          TtaSetItemOn (doc, 1, File, BForward);
          if (DocumentSource[doc])
            /* update the document source too */
            TtaSetItemOn (DocumentSource[doc], 1, File, BForward);
        }
      else
        {
          state = FALSE;
          picture = iconForwardNo;
          TtaSetItemOff (doc, 1, File, BForward);
          if (DocumentSource[doc])
            /* update the document source too */
            TtaSetItemOff (DocumentSource[doc], 1, File, BForward);
        }
    }
#ifndef _WX
  TtaChangeButton (doc, 1, index, picture, state);
  if (DocumentSource[doc])
    TtaChangeButton (DocumentSource[doc], 1, index, picture, state);
#endif /* _WX */
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
#ifndef _WX
              TtaChangeButton (document, 1, iStop, stopN, FALSE);
#else /* _WX */
              TtaEnableAction(document, "StopTransfer", FALSE);
#endif /* _WX */
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
#ifndef _WX
        TtaChangeButton (document, 1, iStop, stopR, TRUE);
#else /* _WX */
      TtaEnableAction(document, "StopTransfer", TRUE);
#endif /* _WX */
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
#ifndef _WX
        /* this document is displayed */
        TtaChangeButton (document, 1, iStop, stopR, TRUE);
#else /* _WX */
      TtaEnableAction(document, "StopTransfer", TRUE);
#endif /* _WX */
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
  Update the save button and the corresponding menu entry according to the
  document status.
  ----------------------------------------------------------------------*/
void DocStatusUpdate (Document doc, ThotBool modified)
{
  Document    otherDoc;

  if (modified && TtaGetDocumentAccessMode (doc))
    /* the document has been modified and is not in Read-Only mode */
    {
      TtaSetItemOn (doc, 1, File, BSave);
#ifndef _WX
      TtaChangeButton (doc, 1, iSave, iconSave, TRUE);
#endif /* _WX */
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
#ifndef _WX
      TtaChangeButton (doc, 1, iSave, iconSaveNo, FALSE);
#endif /* _WX */
      if (TtaIsDocumentUpdated (doc))
        {
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
  SetCharsetMenuOff sets Off the charset menu in the current view.
  ----------------------------------------------------------------------*/
static void SetCharsetMenuOff (Document doc, View view)
{
  TtaSetItemOff (doc, view, File, BUTF_8);
  TtaSetItemOff (doc, view, File, BISO_8859_1);
  TtaSetItemOff (doc, view, File, BISO_8859_15);
  TtaSetItemOff (doc, view, File, BWINDOWS_1252);
  TtaSetItemOff (doc, view, File, BISO_8859_2);
  TtaSetItemOff (doc, view, File, BWINDOWS_1250);
  TtaSetItemOff (doc, view, File, BISO_8859_3);
  TtaSetItemOff (doc, view, File, BISO_8859_4);
  TtaSetItemOff (doc, view, File, BWINDOWS_1257);
  TtaSetItemOff (doc, view, File, BISO_8859_5);
  TtaSetItemOff (doc, view, File, BWINDOWS_1251);
  TtaSetItemOff (doc, view, File, BKOI8_R);
  TtaSetItemOff (doc, view, File, BISO_8859_6);
  TtaSetItemOff (doc, view, File, BWINDOWS_1256);
  TtaSetItemOff (doc, view, File, BISO_8859_7);
  TtaSetItemOff (doc, view, File, BWINDOWS_1253);
  TtaSetItemOff (doc, view, File, BISO_8859_8);
  TtaSetItemOff (doc, view, File, BWINDOWS_1255);
  TtaSetItemOff (doc, view, File, BISO_8859_9);
  TtaSetItemOff (doc, view, File, BWINDOWS_1254);
  TtaSetItemOff (doc, view, File, BGB_2312);
  TtaSetItemOff (doc, view, File, BISO_2022_JP);
  TtaSetItemOff (doc, view, File, BEUC_JP);
  TtaSetItemOff (doc, view, File, BSHIFT_JIS);
}

/*----------------------------------------------------------------------
  SetTableMenuOff sets Off the table menu in the current view.
  ----------------------------------------------------------------------*/
void SetTableMenuOff (Document doc, View view)
{
  TtaSetItemOff (doc, view, Types, BCaption);
  TtaSetItemOff (doc, view, Types, BColgroup);
  TtaSetItemOff (doc, view, Types, BCol);
  TtaSetItemOff (doc, view, Types, BTHead);
  TtaSetItemOff (doc, view, Types, BTBody);
  TtaSetItemOff (doc, view, Types, BTFoot);
  TtaSetItemOff (doc, view, Types, BDataCell);
  TtaSetItemOff (doc, view, Types, BHeadingCell);
  TtaSetItemOff (doc, view, Types, BCellHExtend);
  TtaSetItemOff (doc, view, Types, BCellVExtend);
  TtaSetItemOff (doc, view, Types, BCellHShrink);
  TtaSetItemOff (doc, view, Types, BCellVShrink);
  TtaSetItemOff (doc, view, Types, BSelectRow);
  TtaSetItemOff (doc, view, Types, BCreateRowB);
  TtaSetItemOff (doc, view, Types, BCreateRowA);
  TtaSetItemOff (doc, view, Types, BSelectColumn);
  TtaSetItemOff (doc, view, Types, BCreateColumnB);
  TtaSetItemOff (doc, view, Types, BCreateColumnA);
  TtaSetItemOff (doc, view, Types, BPasteBefore);
  TtaSetItemOff (doc, view, Types, BPasteAfter);
  TtaSetItemOff (doc, view, XMLTypes, BMCellHExtend);
  TtaSetItemOff (doc, view, XMLTypes, BMCellVExtend);
  TtaSetItemOff (doc, view, XMLTypes, BMCellHShrink);
  TtaSetItemOff (doc, view, XMLTypes, BMCellVShrink);
  TtaSetItemOff (doc, view, XMLTypes, BMSelectRow);
  TtaSetItemOff (doc, view, XMLTypes, BMCreateRowB);
  TtaSetItemOff (doc, view, XMLTypes, BMCreateRowA);
  TtaSetItemOff (doc, view, XMLTypes, BMSelectColumn);
  TtaSetItemOff (doc, view, XMLTypes, BMCreateColumnB);
  TtaSetItemOff (doc, view, XMLTypes, BMCreateColumnA);
  TtaSetItemOff (doc, view, XMLTypes, BMPasteBefore);
  TtaSetItemOff (doc, view, XMLTypes, BMPasteAfter);
}

/*----------------------------------------------------------------------
  SetTableMenuOn sets On the table menu in the current view.
  ----------------------------------------------------------------------*/
void SetTableMenuOn (Document doc, View view)
{
  if (TableMenuActive)
    {
      /* a table element is selected */
      TtaSetItemOn (doc, view, Types, BCaption);
      TtaSetItemOn (doc, view, Types, BColgroup);
      TtaSetItemOn (doc, view, Types, BCol);
      TtaSetItemOn (doc, view, Types, BTHead);
      TtaSetItemOn (doc, view, Types, BTBody);
      TtaSetItemOn (doc, view, Types, BTFoot);
      TtaSetItemOn (doc, view, Types, BDataCell);
      TtaSetItemOn (doc, view, Types, BHeadingCell);
      TtaSetItemOn (doc, view, Types, BCellHExtend);
      TtaSetItemOn (doc, view, Types, BCellVExtend);
      TtaSetItemOn (doc, view, Types, BCellHShrink);
      TtaSetItemOn (doc, view, Types, BCellVShrink);
      TtaSetItemOn (doc, view, Types, BSelectRow);
      TtaSetItemOn (doc, view, Types, BCreateRowB);
      TtaSetItemOn (doc, view, Types, BCreateRowA);
      TtaSetItemOn (doc, view, Types, BSelectColumn);
      TtaSetItemOn (doc, view, Types, BCreateColumnB);
      TtaSetItemOn (doc, view, Types, BCreateColumnA);
    }
  else
    {
      TtaSetItemOff (doc, view, Types, BCaption);
      TtaSetItemOff (doc, view, Types, BColgroup);
      TtaSetItemOff (doc, view, Types, BCol);
      TtaSetItemOff (doc, view, Types, BTHead);
      TtaSetItemOff (doc, view, Types, BTBody);
      TtaSetItemOff (doc, view, Types, BTFoot);
      TtaSetItemOff (doc, view, Types, BDataCell);
      TtaSetItemOff (doc, view, Types, BHeadingCell);
      TtaSetItemOff (doc, view, Types, BCellHExtend);
      TtaSetItemOff (doc, view, Types, BCellVExtend);
      TtaSetItemOff (doc, view, Types, BCellHShrink);
      TtaSetItemOff (doc, view, Types, BCellVShrink);
      TtaSetItemOff (doc, view, Types, BSelectRow);
      TtaSetItemOff (doc, view, Types, BCreateRowB);
      TtaSetItemOff (doc, view, Types, BCreateRowA);
      TtaSetItemOff (doc, view, Types, BSelectColumn);
      TtaSetItemOff (doc, view, Types, BCreateColumnB);
      TtaSetItemOff (doc, view, Types, BCreateColumnA);
      TtaSetItemOff (doc, view, Types, BPasteBefore);
      TtaSetItemOff (doc, view, Types, BPasteAfter);
    }
  if (MTableMenuActive)
    {
      /* a matrice element is selected */
      TtaSetItemOn (doc, view, XMLTypes, BMCellHExtend);
      TtaSetItemOn (doc, view, XMLTypes, BMCellVExtend);
      TtaSetItemOn (doc, view, XMLTypes, BMCellHShrink);
      TtaSetItemOn (doc, view, XMLTypes, BMCellVShrink);
      TtaSetItemOn (doc, view, XMLTypes, BMSelectRow);
      TtaSetItemOn (doc, view, XMLTypes, BMCreateRowB);
      TtaSetItemOn (doc, view, XMLTypes, BMCreateRowA);
      TtaSetItemOn (doc, view, XMLTypes, BMSelectColumn);
      TtaSetItemOn (doc, view, XMLTypes, BMCreateColumnB);
      TtaSetItemOn (doc, view, XMLTypes, BMCreateColumnA);
    }
  else
    {
      TtaSetItemOff (doc, view, XMLTypes, BMCellHExtend);
      TtaSetItemOff (doc, view, XMLTypes, BMCellVExtend);
      TtaSetItemOff (doc, view, XMLTypes, BMCellHShrink);
      TtaSetItemOff (doc, view, XMLTypes, BMCellVShrink);
      TtaSetItemOff (doc, view, XMLTypes, BMSelectRow);
      TtaSetItemOff (doc, view, XMLTypes, BMCreateRowB);
      TtaSetItemOff (doc, view, XMLTypes, BMCreateRowA);
      TtaSetItemOff (doc, view, XMLTypes, BMSelectColumn);
      TtaSetItemOff (doc, view, XMLTypes, BMCreateColumnB);
      TtaSetItemOff (doc, view, XMLTypes, BMCreateColumnA);
      TtaSetItemOff (doc, view, XMLTypes, BMPasteBefore);
      TtaSetItemOff (doc, view, XMLTypes, BMPasteAfter);
    }
}


/*----------------------------------------------------------------------
  UpdateEditorMenus 
  Update windows menus for the Editor mode              
  ----------------------------------------------------------------------*/
void UpdateEditorMenus (Document doc)
{
  View       view;
  int        profile;
  ThotBool   isXhtml11;

  profile = TtaGetDocumentProfile (doc);
  isXhtml11 = (DocumentMeta[doc] && DocumentMeta[doc]->xmlformat &&
               profile != L_Strict && profile != L_Basic);

#ifndef _WX
  /* update specific menu entries */
  TtaUpdateMenus (doc, 1, FALSE);
#endif /* _WX */

  /* Update the doctype menu */
  UpdateDoctypeMenu (doc);
#ifdef _WX
  /* structure information is active only in the structure view */
  if (profile == L_Basic)
    {
      TtaSetItemOff (doc, 1, Types, BStyle);
      TtaSetItemOff (doc, 1, Types, BScript);
      TtaSetItemOff (doc, 1, Types, BNoScript);
    }
  else
    {
      TtaSetItemOn (doc, 1, Types, BStyle);
      TtaSetItemOn (doc, 1, Types, BScript);
      TtaSetItemOn (doc, 1, Types, BNoScript);
    }
#else /* _WX */
  /* structure information is active only in the structure view */
  TtaSetItemOff (doc, 1, Types, BStyle);
  TtaSetItemOff (doc, 1, Types, BComment);
  TtaSetItemOff (doc, 1, Types, BBase);
  TtaSetItemOff (doc, 1, Types, BMeta);
  TtaSetItemOff (doc, 1, Types, BLink);
  TtaSetItemOff (doc, 1, Types, BScript);
  TtaSetItemOff (doc, 1, Types, BNoScript);
  TtaSetItemOff (doc, 1, XMLTypes, BXMLComment);
#endif /* _WX */
  /* invalid all table edits as long as the selection is out of a table */
  if (TtaIsDocumentSelected (doc))
    SetTableMenuOn (doc, 1);
  else
    SetTableMenuOff (doc, 1);

#ifndef _WX
  /* SG: I dont understand why these items are enabled here ...
   * because SwitchUndo, SwitchPaste functions do the work before  */
  TtaSetItemOn (doc, 1, Edit_, BUndo);
  TtaSetItemOn (doc, 1, Edit_, BRedo);
  TtaSetItemOn (doc, 1, Edit_, BCut);
  TtaSetItemOn (doc, 1, Edit_, BPaste);
  TtaSetItemOn (doc, 1, Edit_, BClear);
#endif /* _WX */

  if (DocumentTypes[doc] == docHTML ||
      DocumentTypes[doc] == docAnnot ||
      DocumentTypes[doc] == docSVG ||
      DocumentTypes[doc] == docMath ||
      DocumentTypes[doc] == docXml ||
      DocumentTypes[doc] == docImage)
    {
      TtaSetItemOn (doc, 1, Edit_, BSpellCheck);
      TtaSetMenuOn (doc, 1, Style);
      if (DocumentTypes[doc] == docMath)
        {
          SwitchIconMath (doc, 1, TRUE);
          TtaSetMenuOn (doc, 1, XMLTypes);
          TtaSetItemOn (doc, 1, Edit_, BTransform);
        }
      else
        {
          if (DocumentTypes[doc] == docHTML)
            {
              if (isXhtml11)
                TtaSetMenuOn (doc, 1, XMLTypes);
              else
                TtaSetMenuOff (doc, 1, XMLTypes);
              TtaSetItemOn (doc, 1, Edit_, BTransform);
            }
          else if (DocumentTypes[doc] != docImage)
            TtaSetMenuOn (doc, 1, XMLTypes);
          TtaSetMenuOn (doc, 1, Types);
          TtaSetMenuOn (doc, 1, Links);

#ifndef _WX
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
          TtaChangeButton (doc, 1, iLink, iconLink, TRUE);
#endif /* _WX */

          SwitchIconMath (doc, 1, TRUE);
#ifdef _SVG
          SwitchIconGraph (doc, 1, TRUE);
          SwitchIconLibrary (doc, 1, TRUE);
#ifdef _GL
          SwitchIconAnimPlay (doc, 1, TRUE);
#endif /*_GL*/
#endif /* _SVG */
        }

      view = TtaGetViewFromName (doc, "Structure_view");
      if (view != 0 && TtaIsViewOpen (doc, view))
        {
          /* update specific menu entries */
#ifndef _WX
          TtaUpdateMenus (doc, view, FALSE);
          TtaSetItemOff (doc, view, File, BShowLogFile);
          TtaSetItemOff (doc, view, File, BSynchro);
          SetTableMenuOff (doc, view); /* no table commands */
#endif /* _WX */
          TtaSetItemOn (doc, view, Edit_, BCut);
          TtaSetItemOn (doc, view, Edit_, BPaste);
          TtaSetItemOn (doc, view, Edit_, BClear);
          TtaSetItemOn (doc, view, Edit_, BSpellCheck);
          TtaSetItemOn (doc, view, Edit_, BTransform);
          if (DocumentTypes[doc] != docMath)
            {
              TtaSetMenuOn (doc, view, Types);
              if (DocumentTypes[doc] != docHTML || isXhtml11)
                TtaSetMenuOn (doc, view, XMLTypes);
            }
        }
      view = TtaGetViewFromName (doc, "Alternate_view");
      if (view != 0 && TtaIsViewOpen (doc, view))
        {
          /* update specific menu entries */
#ifndef _WX
          TtaUpdateMenus (doc, view, FALSE);
          TtaSetItemOff (doc, view, File, BShowLogFile);
          TtaSetItemOff (doc, view, File, BSynchro);
          /* structure information is active only in the structure view */
          TtaSetItemOff (doc, view, Types, BStyle);
          TtaSetItemOff (doc, view, Types, BComment);
          TtaSetItemOff (doc, view, Types, BBase);
          TtaSetItemOff (doc, view, Types, BMeta);
          TtaSetItemOff (doc, view, Types, BLink);
          TtaSetItemOff (doc, view, Types, BScript);
          TtaSetItemOff (doc, view, Types, BNoScript);
          TtaSetItemOff (doc, view, XMLTypes, BXMLComment);
          SetTableMenuOff (doc, view); /* no table commands */
          SetCharsetMenuOff (doc, view); /* no charset commands */
#endif /* _WX */
          TtaSetItemOn (doc, view, Edit_, BCut);
          TtaSetItemOn (doc, view, Edit_, BPaste);
          TtaSetItemOn (doc, view, Edit_, BClear);
          TtaSetItemOn (doc, view, Edit_, BSpellCheck);
          if (isXhtml11)
            TtaSetMenuOn (doc, view, XMLTypes);
        }
      view = TtaGetViewFromName (doc, "Links_view");
      if (view != 0 && TtaIsViewOpen (doc, view))
        {
          /* update specific menu entries */
#ifndef _WX
          TtaUpdateMenus (doc, view, FALSE);
          TtaSetItemOff (doc, view, File, BShowLogFile);
          TtaSetItemOff (doc, view, File, BSynchro);
          /* structure information is active only in the structure view */
          TtaSetItemOff (doc, view, Types, BStyle);
          TtaSetItemOff (doc, view, Types, BComment);
          TtaSetItemOff (doc, view, Types, BBase);
          TtaSetItemOff (doc, view, Types, BMeta);
          TtaSetItemOff (doc, view, Types, BLink);
          TtaSetItemOff (doc, view, Types, BScript);
          TtaSetItemOff (doc, view, Types, BNoScript);
          TtaSetItemOff (doc, view, XMLTypes, BXMLComment);
          SetTableMenuOff (doc, view); /* no table commands */
#endif /* _WX */
          TtaSetItemOn (doc, view, Edit_, BCut);
          TtaSetItemOn (doc, view, Edit_, BPaste);
          TtaSetItemOn (doc, view, Edit_, BClear);
          TtaSetItemOn (doc, view, Edit_, BSpellCheck);
          TtaSetItemOn (doc, view, Edit_, BTransform);
          if (DocumentTypes[doc] != docMath)
            {
              TtaSetMenuOn (doc, view, Types);
              if (DocumentTypes[doc] != docHTML || isXhtml11)
                TtaSetMenuOn (doc, view, XMLTypes);
            }
        }
      view = TtaGetViewFromName (doc, "Table_of_contents");
      if (view != 0 && TtaIsViewOpen (doc, view))
        {
          /* update specific menu entries */
#ifndef _WX
          TtaUpdateMenus (doc, view, FALSE);
          TtaSetItemOff (doc, view, File, BShowLogFile);
          TtaSetItemOff (doc, view, File, BSynchro);
          /* structure information is active only in the structure view */
          TtaSetItemOff (doc, view, Types, BStyle);
          TtaSetItemOff (doc, view, Types, BComment);
          TtaSetItemOff (doc, view, Types, BBase);
          TtaSetItemOff (doc, view, Types, BMeta);
          TtaSetItemOff (doc, view, Types, BLink);
          TtaSetItemOff (doc, view, Types, BScript);
          TtaSetItemOff (doc, view, Types, BNoScript);
          TtaSetItemOff (doc, view, XMLTypes, BXMLComment);
          SetTableMenuOff (doc, view); /* no table commands */
#endif /* _WX */
          TtaSetItemOn (doc, view, Edit_, BCut);
          TtaSetItemOn (doc, view, Edit_, BPaste);
          TtaSetItemOn (doc, view, Edit_, BClear);
          TtaSetItemOn (doc, view, Edit_, BSpellCheck);
          TtaSetItemOn (doc, view, Edit_, BTransform);
          if (isXhtml11)
            TtaSetMenuOn (doc, view, XMLTypes);
        }
    }
}


/*----------------------------------------------------------------------
  ShowLogFile
  Show error messages generated by the parser.
  -----------------------------------------------------------------------*/
void ShowLogFile (Document doc, View view)
{
  char     fileName[200];
  int      newdoc;

  if (DocumentTypes[doc] == docSource)
    doc = GetDocFromSource (doc);
  if (doc)
    {
      sprintf (fileName, "%s%c%d%cPARSING.ERR",
               TempFileDirectory, DIR_SEP, doc, DIR_SEP);
      newdoc = GetAmayaDoc (fileName, NULL, 0, doc, CE_LOG, FALSE,
                            NULL, NULL);
      /* store the relation with the original document */
      if (newdoc)
        {
          DocumentSource[newdoc] = doc;
          TtaSetStatus (newdoc, 1, "   ", NULL);
        }
    }
}

/*----------------------------------------------------------------------
  OpenParsingErrors
  ----------------------------------------------------------------------*/
ThotBool OpenParsingErrors (Document document)
{  
  char       fileName[200];

  if (document == 0 || DocumentTypes[document] == docFree)
    return FALSE;

  sprintf (fileName, "%s%c%d%cPARSING.ERR",
           TempFileDirectory, DIR_SEP, document, DIR_SEP);
  /* check what error file is open */
  if (DocumentTypes[document] == docCSS)
    {
      CSSErrFile = TtaWriteOpen (fileName);
      if ((CSSErrFile == NULL))
        return FALSE;
      else
        fprintf (CSSErrFile, TtaGetMessage (AMAYA, AM_LINK_LINE));      
    }
  else
    {
      ErrFile = TtaWriteOpen (fileName);
      if ((ErrFile == NULL))
        return FALSE;
      else
        fprintf (ErrFile, TtaGetMessage (AMAYA, AM_LINK_LINE));      
    }
  return TRUE;
}

/*----------------------------------------------------------------------
  RemoveParsingErrors
  ----------------------------------------------------------------------*/
void RemoveParsingErrors (Document document)
{  
  char       htmlErrFile[200];
  
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
  XMLInvalidToken = FALSE;
  XMLUnknownEncoding = FALSE;
  XMLCharacterNotSupported = FALSE;
  /* close the error file */
  if (ErrFile)
    {
      TtaWriteClose (ErrFile);
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
  char      *ptr;
  char       fileName[200];
  char       text [200];
#ifndef _WX
  int        prof;
#endif /* _WX */
  ThotBool   closeLog = FALSE;

  // Avoid recursive call
  if (CriticCheckError)
    return;
  CriticCheckError = TRUE;
#ifndef _WINGUI
  CloseLogs (doc);
  closeLog = TRUE;
#endif /* _WINGUI */

  if (BADMimeType)
    {
      /* the mime type doesn't match the doctype */
      InitConfirm (doc, 1, TtaGetMessage (AMAYA, AM_INVALID_MIMETYPE));
      BADMimeType = FALSE;
    }
  if (ErrFile)
    {
      /* Active the menu entry */
      TtaSetItemOn (doc, 1, File, BShowLogFile);
      if (DocumentSource[doc])
        /* update the document source too */
        TtaSetItemOn (DocumentSource[doc], 1, File, BShowLogFile);
	
      if (XMLCharacterNotSupported || XMLInvalidToken)
        {
          /* Invalid characters */
          if (XMLInvalidToken)
            ptr = TtaGetMessage (AMAYA, AM_XML_CHARACTER_RETRY);
          else
            ptr = TtaGetMessage (AMAYA, AM_XML_CHARACTER_ERROR);
          InitConfirm3L (doc, 1, 
                         TtaGetMessage (AMAYA, AM_XML_CHARACTER_ERROR),
                         TtaGetMessage (AMAYA, AM_XML_CHARACTER_RETRY),
                         NULL, FALSE);
          CleanUpParsingErrors ();
        }
      else if (XMLNotWellFormed)
        {
          /* Raise a popup message */
          /* The document is not well-formed */
          if (DocumentTypes[doc] == docHTML &&
              (!DocumentMeta[doc] || !DocumentMeta[doc]->compound))
            {
              ptr = TtaGetMessage (AMAYA, AM_XML_RETRY);
              // save the original log file
              sprintf (fileName, "%s%c%d%cPARSING.ERR",
                       TempFileDirectory, DIR_SEP, doc, DIR_SEP);
              strcpy (text, fileName);
              strcat (text, ".org");
              CleanUpParsingErrors ();
              CloseLogs (doc);
              TtaFileUnlink (text);
              TtaFileRename (fileName, text);
              ParseAsHTML (doc, 1);
              // reset XML format for future saving
              DocumentMeta[doc]->xmlformat = TRUE;
              // restore the original log file
              CleanUpParsingErrors ();
              CloseLogs (doc);
              TtaFileUnlink (fileName);
              TtaFileRename (text, fileName);
              closeLog = TRUE;
              if (SavingDocument && SavingDocument == DocumentSource[doc])
                {
                  ConfirmError (doc, 1, ptr,
                                TtaGetMessage (AMAYA, AM_SAVE_ANYWAY),
                                NULL);
                  if (!ExtraChoice && !UserAnswer)
                    // GTK or WX version: stop the save process
                    SavingDocument = 0;
                }
              else
                {
                  //InitConfirm3L (doc, 1, ptr, NULL, NULL, FALSE);
                  ConfirmError (doc, 1, ptr,
                                TtaGetMessage (AMAYA, AM_AFILTER_SHOW),
                                NULL);
                  if (ExtraChoice || UserAnswer)
                    {
                      ShowSource (doc, 1);
                      // GTK or WX version: show errors
                       ShowLogFile (doc, 1);
                    }
                }
            }
          else
            {
              ptr = TtaGetMessage (AMAYA, AM_XML_ERROR);
              if (SavingDocument == DocumentSource[doc])
                // stop the save process
                SavingDocument = 0;
              // Set the document in read-only mode
              TtaSetDocumentAccessMode (doc, 0);
              ConfirmError (doc, 1, ptr,
                            TtaGetMessage (AMAYA, AM_AFILTER_SHOW),
                            NULL);
                  if (ExtraChoice || UserAnswer)
                    {
                      // GTK or WX version: show errors
                      CleanUpParsingErrors ();
                      CloseLogs (doc);
                      closeLog = TRUE;
                      ShowLogFile (doc, 1);
                      ShowSource (doc, 1);
                    }
            }
        }
#ifndef _WX
      else if (XMLErrorsFoundInProfile)
        {
          /* Some elements or attributes are not supported */
          /* in the current document profile */
          prof = TtaGetDocumentProfile (doc);
          if (prof == L_Basic)
            {
              strcpy (text, TtaGetMessage (AMAYA, AM_XML_PROFILE));
              strcat (text, " XHTML Basic");
            }
          else if (prof == L_Strict)
            {
              strcpy (text, TtaGetMessage (AMAYA, AM_XML_PROFILE));
              if (DocumentMeta[doc]->xmlformat)
                strcat (text, " XHTML 1.0 Strict");
              else
                strcat (text, " HTML 4.0 Strict");
            }
          else if (prof == L_Xhtml11)
            {
              strcpy (text, TtaGetMessage (AMAYA, AM_XML_PROFILE));
              strcat (text, " XHTML 1.1");
            }
          else
            strcpy (text, "");

          InitConfirm3L (doc, 1, text, NULL,
                         TtaGetMessage (AMAYA, AM_XML_WARNING), FALSE);
          CleanUpParsingErrors ();
          if (UserAnswer)
            {
              CloseLogs (doc);
              closeLog = TRUE;
              ShowLogFile (doc, 1);
              ShowSource (doc, 1);
            }
        }
      else if (XMLErrorsFound)
        {
          /* Parsing errors detected */
          strcpy (text, "");
          InitConfirm (doc, 1, TtaGetMessage (AMAYA, AM_XML_WARNING));
          CleanUpParsingErrors ();
          if (UserAnswer)
            {
              CloseLogs (doc);
              closeLog = TRUE;
              ShowLogFile (doc, 1);
              ShowSource (doc, 1);
            }
        }
#endif /* _WX */
      CleanUpParsingErrors ();
      if (!closeLog)
        {
          CloseLogs (doc);
          TtaSetItemOn (doc, 1, File, BShowLogFile);
          if (DocumentSource[doc])
            /* update the document source too */
            TtaSetItemOn (DocumentSource[doc], 1, File, BShowLogFile);
        }
    }
  else
    {
      CloseLogs (doc);
      TtaSetItemOff (doc, 1, File, BShowLogFile);
      if (DocumentSource[doc])
        /* update the document source too */
        TtaSetItemOff (DocumentSource[doc], 1, File, BShowLogFile);
    }
  CriticCheckError = FALSE;
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
#ifndef _WX
        TtaChangeButton (document, 1, iStop, stopN, FALSE);
#else /* _WX */
      TtaEnableAction(document, "StopTransfer", FALSE);
#endif /* _WX */
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
  char *s, *ptr;

  if (IsFilePath (*url))
    {
      // remove file:
      ptr = &((*(url))[5]);
      s = (char *)TtaGetMemory (MAX_LENGTH);
        s[0] = EOS;
      *url = s;
      if (ptr[0] == '/' && ptr[1] == '/' && ptr[2] == '/')
        ptr = &ptr[2];
#ifdef _WINDOWS
      else if (ptr[0] == '/' && ptr[1] == '/' && ptr[3] == ':')
        ptr = &ptr[2];
      if (ptr[1] != ':')
        {
          char    *ptr2;
          // add a default device
          ptr2 = getenv ("HOMEDRIVE");
          if (ptr2)
            strcpy (s, ptr2);
          else
            strcpy (s, "c:");
		  s= &s[2];
        }
#endif /* _WINDOWS */
	  while (*ptr != EOS)
	  {
		if (*ptr == '/')
	      *s = DIR_SEP;
		else
          *s = *ptr;
	    s++;
		ptr++;
	  }
      *s = EOS;
      return TRUE;
    }
  else if (**url != DIR_SEP 
      && **url != '~'
#ifdef _WINDOWS
      && (*(url))[1] != ':'
#endif /* _WINDOWS */
      && !IsW3Path (*url) 
      && (strlen (*url) + 8) < MAX_LENGTH)
    {
      if (!TtaFileExist(*url))
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
  if (text && text[0] != EOS)
    {
      /* remove any trailing '\n' chars that may have gotten there
         after a cut and paste */
      change = RemoveNewLines (text);
      if (IsW3Path (text))
        url = text;
      else
        {
          s = (char *)TtaGetMemory (MAX_LENGTH);
          updated = CompleteUrl (&text);
          change = NormalizeFile (text, s, AM_CONV_NONE);
          if (updated)
            /* free the allocated string */
            TtaFreeMemory (text);
          url = s;
        }
      if (DocumentTypes[doc] == docSource)
        doc = GetDocFromSource (doc);
      if (DocumentURLs[doc] && url &&
          !strcmp (DocumentURLs[doc], url))
        {
          // nothing to do
          TtaFreeMemory (s);
          return;
        }
      if (!DontReplaceOldDoc && !CanReplaceCurrentDocument (doc, view))
        {
          /* restore the previous value @@ */
          AddURLInCombobox (DocumentURLs[doc], NULL, FALSE);
          TtaSetTextZone (doc, view, URL_list);
          /* cannot load the new document */
          TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), url);
          /* abort the command */
          TtaFreeMemory (s);
          return;
        }
      else
        /* do the same thing as a callback form open document form */
        CallbackDialogue (BaseDialog + URLName, STRING_DATA, url);

      TtaFreeMemory (s);
      DontReplaceOldDoc = FALSE;
      NewFile = FALSE;
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
  Element             root, title;
#ifdef _SVG
  Element             child;
#endif /* _SVG */
  SSchema             docSSchema;

  title = NULL;
  root = TtaGetRootElement (sourceDoc);
  docSSchema = TtaGetDocumentSSchema (sourceDoc);
  if (!strcmp (TtaGetSSchemaName (docSSchema), "HTML"))
    /* sourceDoc is a HTML document */
    {
      /* search the Title element in sourceDoc */
      elType.ElSSchema = docSSchema;
      elType.ElTypeNum = HTML_EL_TITLE;
      title = TtaSearchTypedElement (elType, SearchForward, root);
    }
#ifdef _SVG
  else if (!strcmp (TtaGetSSchemaName (docSSchema), "SVG"))
    {
      child = TtaGetFirstChild (root);
      while (child && !title)
        {
          elType = TtaGetElementType (child);
          if (elType.ElSSchema == docSSchema &&
              elType.ElTypeNum == SVG_EL_title)
            title = child;
          else
            TtaNextSibling (&child);
        }
    }
#endif /* _SVG */
  if (title)
    UpdateTitle (title, sourceDoc);
}

/*----------------------------------------------------------------------
  InitFormAnswer
  Dialogue form for answering text, user name and password
  ----------------------------------------------------------------------*/
void InitFormAnswer (Document document, View view, const char *auth_realm,
                     char *server)
{
#ifdef _GTK
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
  TtaNewPwdForm (BaseDialog + PasswordText, BaseDialog + FormAnswer,
                 TtaGetMessage (AMAYA, AM_PASSWORD), NAME_LENGTH, 1, TRUE);
   
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
#endif /* _GTK */
#ifdef _WX
  ThotBool created;

  created = CreateAuthentDlgWX (BaseDialog + FormAnswer,
                                TtaGetViewFrame (document, view),
                                (char *)auth_realm, server);
  if (created)
    {
      TtaSetDialoguePosition ();
      TtaShowDialogue (BaseDialog + FormAnswer, FALSE);
    }
  TtaWaitShowDialogue ();
#endif /* _WX */
#ifdef _WINGUI
  CreateAuthenticationDlgWindow (TtaGetViewFrame (document, view),
                                 (char *)auth_realm, server);
#endif /* _WINGUI */

}


/*----------------------------------------------------------------------
  InitInfo
  Displays a message box with the given info text
  ----------------------------------------------------------------------*/
void InitInfo (char *label, char *info)
{
  if (!info || *info == EOS)
    return;
#ifdef _WINGUI   
  MessageBox (NULL, info, label, MB_OK);
#endif /* _WINGUI */
#if defined(_GTK) || defined(_WX)  
  TtaDisplayMessage (CONFIRM, info, NULL);
#endif /* #if defined(_GTK) || defined(_WX) */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ConfirmError (Document document, View view, char *label,
                   char *extrabutton, char *confirmbutton)
{
#ifdef _GTK
  char      s[MAX_LENGTH];
  int       i, n;

  i = 0;
  n = 0;
  if (confirmbutton)
    {
      strcpy (&s[i], confirmbutton);
      i += strlen (&s[i]) + 1;
      n++;
    }
  if (extrabutton)
    {
      /* display 3 buttons: extrabutton - show - cancel */
      strcpy (&s[i], extrabutton);
      n++;
    }
  TtaNewSheet (BaseDialog + ConfirmForm, TtaGetViewFrame (document, view),
               TtaGetMessage (LIB, TMSG_LIB_CONFIRM),
               n, s, TRUE, 2, 'L', D_DISCARD);
  TtaNewLabel (BaseDialog + ConfirmText, BaseDialog + ConfirmForm, label);
  TtaSetDialoguePosition ();
  TtaShowDialogue (BaseDialog + ConfirmForm, FALSE);
  /* wait for an answer */
  TtaWaitShowDialogue ();
#endif /* _GTK */
#ifdef _WX
  ThotBool created = CreateInitConfirmDlgWX (BaseDialog + ConfirmForm,
                                             TtaGetViewFrame (document, view),
                                             NULL, extrabutton, confirmbutton,
                                             label, NULL, NULL);
  if (created)
    {
      TtaSetDialoguePosition ();
      TtaShowDialogue (BaseDialog + ConfirmForm, FALSE);
      /* wait for an answer */
      TtaWaitShowDialogue ();
    }
#endif /* _WX */
#ifdef _WINGUI
  if (confirmbutton)
    CreateInitConfirmDlgWindow (TtaGetViewFrame (document, view),
                                extrabutton, confirmbutton, label);
  else
    CreateInitConfirmDlgWindow (TtaGetViewFrame (document, view),
                                confirmbutton, extrabutton, label);
#endif /* _WINGUI */
  /* remove the critic section */
  CriticConfirm = FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void InitConfirm3L (Document document, View view, char *label1, char *label2,
                    char *label3, ThotBool withCancel)
{
  /* IV: This widget can't be called twice, but it happens when downloading a
     document with protected images. This is a quick silution to avoid the
     sigsev, although it doesn't fix the problem */
  if (CriticConfirm)
    return;
  else
    CriticConfirm = TRUE;

#ifdef _GTK
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
#endif /* _GTK */
#ifdef _WX
  char *confirm = NULL;
  if (withCancel)
    confirm = TtaGetMessage (LIB, TMSG_LIB_CONFIRM);
  ThotBool created = CreateInitConfirmDlgWX (BaseDialog + ConfirmForm,
                                             TtaGetViewFrame (document, view),
                                             NULL /* title */,
                                             NULL /* extra */,
                                             confirm,
                                             label1, label2, label3);
  if (created)
    {
      TtaSetDialoguePosition ();
      TtaShowDialogue (BaseDialog + ConfirmForm, FALSE);
      /* wait for an answer */
      TtaWaitShowDialogue ();
      //if (AmayaIsAlive ())
      //  TtaDestroyDialogue (BaseDialog + ConfirmForm);   
    }
#endif /* _WX */
#ifdef _WINGUI
  CreateInitConfirm3LDlgWindow (TtaGetViewFrame (document, view),
                                TtaGetMessage (LIB, TMSG_LIB_CONFIRM),
                                label1, label2, label3, withCancel);
#endif /* _WINGUI */
  CriticConfirm = FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void InitConfirm (Document document, View view, char *label)
{
  /* Confirm form */
  /* JK: This widget can't be called twice, but it happens when downloading a
     document with protected images. This is a quick silution to avoid the
     sigsev, although it doesn't fix the problem */
  if (CriticConfirm)
    return;
  else
    CriticConfirm = TRUE;

#ifdef _GTK
  TtaNewForm (BaseDialog + ConfirmForm, TtaGetViewFrame (document, view),
              TtaGetMessage (LIB, TMSG_LIB_CONFIRM), TRUE, 2, 'L', D_CANCEL);
  TtaNewLabel (BaseDialog + ConfirmText, BaseDialog + ConfirmForm, label);
  TtaSetDialoguePosition ();
  TtaShowDialogue (BaseDialog + ConfirmForm, FALSE);
  /* wait for an answer */
  TtaWaitShowDialogue ();
#endif /* _GTK */   
#ifdef _WX
  ThotBool created = CreateInitConfirmDlgWX (BaseDialog + ConfirmForm,
                                             TtaGetViewFrame (document, view),
                                             NULL /* title */,
                                             NULL /* extra */,
                                             TtaGetMessage (LIB, TMSG_LIB_CONFIRM),
                                             label, NULL, NULL);
  if (created)
    {
      TtaSetDialoguePosition ();
      TtaShowDialogue (BaseDialog + ConfirmForm, FALSE);
      /* wait for an answer */
      TtaWaitShowDialogue ();
    }
#endif /* _WX */
#ifdef _WINGUI
  CreateInitConfirmDlgWindow (TtaGetViewFrame (document, view),
                              NULL, NULL, label);
#endif /* _WINGUI */
  /* remove the critic section */
  CriticConfirm = FALSE;
}

/*----------------------------------------------------------------------
  InitCharset
  Asks the user for the charset  of a given URL. Used when saving a
  document with an unknown charset.
  ----------------------------------------------------------------------*/
void InitCharset (Document document, View view, char *url)
{
#if defined(_GTK)
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
                 NULL, 3, s, NULL, 0, FALSE);
  TtaSetMenuForm (BaseDialog + CharsetSel, i);
      
  TtaSetDialoguePosition ();
  TtaShowDialogue (BaseDialog + CharsetForm, FALSE);
  /* wait for an answer */
  TtaWaitShowDialogue ();
#endif /* defined(_GTK) */

#ifdef _WINGUI
  CreateCharsetDlgWindow (TtaGetViewFrame (document, view));
#endif /* _WINGUI */

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

#if defined(_GTK)
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
#endif /* #if defined(_GTK) */

#ifdef _WINGUI
  CreateMimeTypeDlgWindow (TtaGetViewFrame (document, view), nbmimetypes,
                           mimetypes_list);
#endif /* _WINGUI */
}

/*-------------------------------------------------------------------------
  BrowserForm
  Initializes a form that ask the URI of the opened or new created document.
  -------------------------------------------------------------------------*/
static void BrowserForm (Document doc, View view, char *urlname)
{
#if defined(_GTK)
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
      /* it's a local file,
         initialize the file selector to the current position */
      if (TtaCheckDirectory (tempfile))
        {
          strcpy (DirectoryName, tempfile);
          DocumentName[0] = EOS;
        }
      else
        TtaExtractName (tempfile, DirectoryName, DocumentName);
    }
  /* use the last selection in other cases */
  if (DirectoryName[0] == EOS)
    {
      getcwd (DirectoryName, MAX_LENGTH);
      DocumentName[0] = EOS;
    }
  strcpy (s, DirectoryName);
  strcat (s, DIR_STR);
  strcat (s, DocumentName);
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
#endif /* #if defined(_GTK)*/
}

/*----------------------------------------------------------------------
  InitOpenDocForm initializes a form that ask the URI of the opened or
  new created document.
  The parameter name gives a proposed document name (New document).
  The parameter title gives the title of the the form.
  ----------------------------------------------------------------------*/
static void InitOpenDocForm (Document doc, View view, char *name, char *title,
                             DocumentType docType)
{
  char              s [MAX_LENGTH];
  char             *thotdir;
  ThotBool          remote;
#ifdef _WX
  wxString          homedir;
  ThotBool          created;
#else /* _WX */
#ifdef _WINDOWS
  char             *d;
#endif /* _WINDOWS */
  char             *homedir;
#endif /* _WX */
#ifdef _GTK
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
#endif /* _GTK */

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
      if (name[0] == EOS)
        {
          if (DocumentURLs[doc])
            strcpy (s, DocumentURLs[doc]);
          else
            s[0] = EOS;
        }
      else
        {
          // Avoid to create new documents into Amaya space
          thotdir = TtaGetEnvString ("THOTDIR");
          if (!strncmp (s, thotdir, strlen (thotdir)))
            {
#ifdef _WX
              homedir = TtaGetHomeDir();
              strcpy(s, (const char *)homedir.mb_str(wxConvUTF8));
#else /* _WX */
#ifdef _WINDOWS
              d = getenv ("HOMEDRIVE");
              homedir = getenv ("HOMEPATH");
              if (d && *d && homedir)
                sprintf (s, "%s%s", d, homedir);
#else /* _WINDOWS */
              homedir = getenv ("HOME");
			  if (homedir)
                strcpy(s, homedir);
#endif /* _WINDOWS */
#endif /* _WX */
            }
          strcpy (DirectoryName, s);
          strcpy (DocumentName, name);
          strcat (s, DIR_STR);
          strcat (s, name);
        }
      strcpy (LastURLName, s);
    }

#ifdef  _WINGUI
  CreateOpenDocDlgWindow (TtaGetViewFrame (doc, view), title, s, name,
                          DocSelect, DirSelect, docType);
#endif /* _WINGUI */
#ifdef _WX
  /* here we pass also 'URL_list', because we want generate a combobox choice list */
  created = CreateOpenDocDlgWX( BaseDialog + OpenForm,
                                TtaGetViewFrame (doc, view), title, URL_list, s, name,
                                DocSelect, DirSelect, docType, NewFile );
  if (created)
    {
      TtaSetDialoguePosition ();
      TtaShowDialogue (BaseDialog + OpenForm, TRUE);
    }
#endif /* _WX */
#ifdef _GTK
  TtaSetTextForm (BaseDialog + URLName, s);
  TtaSetDialoguePosition ();
  TtaShowDialogue (BaseDialog + OpenForm, TRUE);
#endif /* _GTK */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void  OpenDoc (Document doc, View view)
{
#ifndef _WX
  if (CanReplaceCurrentDocument (doc, view))
    {
      /* load the new document */
      DontReplaceOldDoc = FALSE;
      InNewWindow = FALSE;
#endif /* _WX */
      NewFile = FALSE;
       /* no specific type requested */
      InitOpenDocForm (doc, view, "",
                       TtaGetMessage (LIB, TMSG_BUTTON_OPEN), docText);
#ifndef _WX
    }
#endif /* _WX */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void OpenDocInNewWindow (Document document, View view)
{
#ifndef _WX
  DontReplaceOldDoc = TRUE;
  InNewWindow = TRUE;
#endif /* _WX */
  NewFile = FALSE;
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
  DontReplaceOldDoc = TRUE;
  NewFile = TRUE;
  NewDocType = docType;
  NewDocProfile = docProfile;
  NewXML = TRUE;
  Answer_text[0] = EOS;
  if (NewDocType == docHTML)
    {
#ifdef _WX
      char *s = TtaGetEnvString ("XHTML_Profile");
      char *compound = TtaGetMessage (AMAYA, AM_COMPOUND_DOCUMENT);
      if (s && compound && !strcmp (s, compound))
        InitOpenDocForm (document, view, "New.xml",
                         TtaGetMessage (LIB, TMSG_BUTTON_NEW), docHTML);
      else
        InitOpenDocForm (document, view, "New.html",
                         TtaGetMessage (LIB, TMSG_BUTTON_NEW), docHTML);
#else /* _WX */
      if (docProfile == L_Basic)
        InitOpenDocForm (document, view, "New.html",
                         TtaGetMessage (AMAYA, AM_NEW_HTML_BASIC), docHTML);
      else if (docProfile == L_Strict)
        InitOpenDocForm (document, view, "New.html",
                         TtaGetMessage (AMAYA, AM_NEW_HTML_STRICT), docHTML);
      else if (docProfile == L_Transitional)
        InitOpenDocForm (document, view, "New.html",
                         TtaGetMessage (AMAYA, AM_NEW_HTML_TRANSITIONAL), docHTML);
      else
        InitOpenDocForm (document, view, "New.html",
                         TtaGetMessage (AMAYA, AM_NEW_HTML11), docHTML);
#endif /* _WX */
      /* will scan html documents */
      strcpy (ScanFilter, "*.*htm*");
    }
  else if (NewDocType == docMath)
    {
      /* will scan html documents */
      strcpy (ScanFilter, "*.mml");
      InitOpenDocForm (document, view, "New.mml",
                       TtaGetMessage (AMAYA, AM_NEW_MATHML), docMath);
    }
  else if (NewDocType == docSVG)
    {
      /* will scan html documents */
      strcpy (ScanFilter, "*.svg");
      InitOpenDocForm (document, view, "New.svg",
                       TtaGetMessage (AMAYA, AM_NEW_SVG), docSVG);
    }
  else
    {
      /* will scan html documents */
      strcpy (ScanFilter, "*.css");
      InitOpenDocForm (document, view, "New.css",
                       TtaGetMessage (AMAYA, AM_NEW_CSS), docCSS);
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
      DontReplaceOldDoc = FALSE;
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
  ThotBool	      useMathML, useSVG, useHTML, useMath, withDocType;
 
  docType = DocumentTypes[doc];
  if (docType != docText && docType != docCSS &&
      docType != docSource && docType != docLog && docType != docBookmark &&
      TtaGetDocumentAccessMode (doc))
    {
      /* look for a MathML or SVG nature within the document */
      nature = NULL;
      useMathML = useSVG =  useHTML = FALSE;
      do
        {
          TtaNextNature (doc, &nature);
          if (nature)
            {
              ptr = TtaGetSSchemaName (nature);
              if (!strcmp (ptr, "MathML"))
                useMathML = TRUE;
              else if (!strcmp (ptr, "SVG"))
                useSVG = TRUE;
            }
        }
      while (nature);

      HasADoctype (doc, &withDocType, &useMath);
      if (withDocType)
        {
          /* there is a Doctype */
          TtaSetItemOn  (doc, 1, File, BRemoveDoctype);
          TtaSetItemOff (doc, 1, File, BAddDoctype);
        }
      else
        {
          /* there is no Doctype */
          TtaSetItemOff  (doc, 1, File, BRemoveDoctype);
          if (docType == docHTML ||
              (!useMathML && !useSVG && !useHTML))
            TtaSetItemOn (doc, 1, File, BAddDoctype);
          else
            /* no Doctype available */
            TtaSetItemOff (doc, 1, File, BAddDoctype);
        }

      if (docType == docHTML)
        {
          /* allow to change the DocType:
             A confirmation will be requested if some attribues
             or elements may be lost */
          if (TtaGetDocumentProfile(doc) == L_Xhtml11) /* already done */
            TtaSetItemOff (doc, 1, File, BDoctypeXhtml11);
          else
            TtaSetItemOn (doc, 1, File, BDoctypeXhtml11);

          if (TtaGetDocumentProfile(doc) == L_Transitional &&
              DocumentMeta[doc]->xmlformat == TRUE) /* already done */
            TtaSetItemOff (doc, 1, File, BDoctypeXhtmlTransitional);
          else
            TtaSetItemOn (doc, 1, File, BDoctypeXhtmlTransitional);

          if (TtaGetDocumentProfile(doc) == L_Strict &&
              DocumentMeta[doc]->xmlformat == TRUE) /* already done */
            TtaSetItemOff (doc, 1, File, BDoctypeXhtmlStrict);
          else
            TtaSetItemOn (doc, 1, File, BDoctypeXhtmlStrict);

          if (TtaGetDocumentProfile(doc) == L_Basic) /* already done */
            TtaSetItemOff (doc, 1, File, BDoctypeXhtmlBasic);
          else
            TtaSetItemOn (doc, 1, File, BDoctypeXhtmlBasic);

          if (TtaGetDocumentProfile(doc) == L_Transitional &&
              DocumentMeta[doc]->xmlformat != TRUE) /* already done */
            TtaSetItemOff (doc, 1, File, BDoctypeHtmlTransitional);
          else
            TtaSetItemOn (doc, 1, File, BDoctypeHtmlTransitional);

          if (TtaGetDocumentProfile(doc) == L_Strict &&
              DocumentMeta[doc]->xmlformat != TRUE) /* already done */
            TtaSetItemOff (doc, 1, File, BDoctypeHtmlStrict);
          else
            TtaSetItemOn (doc, 1, File, BDoctypeHtmlStrict);
        }
      else
        {
          TtaSetItemOff (doc, 1, File, BDoctypeXhtml11);
          TtaSetItemOff (doc, 1, File, BDoctypeXhtmlTransitional);
          TtaSetItemOff (doc, 1, File, BDoctypeXhtmlStrict);
          TtaSetItemOff (doc, 1, File, BDoctypeXhtmlBasic);
          TtaSetItemOff (doc, 1, File, BDoctypeHtmlTransitional);
          TtaSetItemOff (doc, 1, File, BDoctypeHtmlStrict);
        }
    }
  else
    {
      TtaSetItemOff (doc, 1, File, BRemoveDoctype);
      TtaSetItemOff (doc, 1, File, BAddDoctype);
      TtaSetItemOff (doc, 1, File, BDoctypeXhtml11);
      TtaSetItemOff (doc, 1, File, BDoctypeXhtmlTransitional);
      TtaSetItemOff (doc, 1, File, BDoctypeXhtmlStrict);
      TtaSetItemOff (doc, 1, File, BDoctypeXhtmlBasic);
      TtaSetItemOff (doc, 1, File, BDoctypeHtmlTransitional);
      TtaSetItemOff (doc, 1, File, BDoctypeHtmlStrict);
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
  GiveWindowGeometry
  Gets registered window parameters
  ----------------------------------------------------------------------*/
static void GiveWindowGeometry (Document doc, int docType, int method,
                                int *x, int *y, int *w, int *h)
{
  char      *label;

  /* get the window geometry */
  if (docType == docAnnot)
    label = "Annot_Formatted_view";
  else if (docType == docBookmark)
    label = "Topics_Formatted_view";
#ifndef _WX
  else if (docType == docSource)
    label = "Source_view";
  else
    label = "Formatted_view";
#else /* _WX */
  else
    label = "Wx_Window";
#endif /* _WX */
  TtaGetViewGeometry (doc, label, x, y, w, h);

#ifdef _WX
  if (w == 0 || h == 0)
    TtaGetViewGeometry (doc, "Formatted_view", x, y, w, h);
#else /* _WX */
  if (docType == docMath)
    {
      *h = 300;
      *w = 580;
    }
#endif /* _WX */

  /* change the position slightly to avoid hiding completely the main
     view of other documents */
  *x = *x + (doc - 1) * 10;
  *y = *y + (doc - 1) * 10;
  if (docType == docLog)
    {
      *x += 100;
      *y += 60;
      *h = 300;
      *w = 600;
    }
  else if (method == CE_HELP  || docType == docBookmark)
    {
      *x += 300;
      *y += 200;
      *h = 500;
      *w = 800;
    }
  else if (docType == docLibrary)
    {
      *x += 300;
      *y += 200;
      *h = 500;
      *w = 400;
    }
}

/*----------------------------------------------------------------------
  InitDocAndView prepares the main view of a new document.
  logFile is TRUE if the new view is created to display a log file
  sourceOfDoc is not zero when we're opening the source view of a document.
  params:
  + Document oldDoc: the old document id
  + ThotBool replaceOldDoc: true if the new doc should replace the old one
  + ThotBool inNewWindow: true if the new doc should be created in a new window
  + ...
  ----------------------------------------------------------------------*/
Document InitDocAndView (Document oldDoc, ThotBool replaceOldDoc,
                         ThotBool inNewWindow,
                         char *docname, DocumentType docType,
                         Document sourceOfDoc, ThotBool readOnly, int profile,
                         int method)
{
  Document      doc; /* the new doc */
  View          mainView, structView, altView, linksView, tocView;
  Element       root, comment, leaf;
  ElementType   elType;
  char          buffer[MAX_LENGTH];
  int           x, y, w, h;
  int           requested_doc, visibility = 5;
  Language	lang;
  ThotBool      isOpen, reinitialized = FALSE, show;
  /* specific to wxWidgets user interface */
  /* ------------------------------------ */
  /* this is the window id identifying where the document should be shown 
   * this window_id is a document attribute and the corresponding
   * window widget has been allocated before this function call */
  int           window_id = -1;
  /* this is the page id identifying where the document should be shown
   * into a window (window_id), each page can contain one or more documents */
  int           page_id   = -1;
  /* this is the choosen position in the given page (top or bottom) */
  int           page_position = 0;
  /* ------------------------------------ */
  
#ifdef _WINGUI
  Window_Curs = IDC_WINCURSOR;
#endif /* _WINGUI */

  /* is there any editing function available */
  if (!TtaCanEdit ())
    /* change the document status */
    readOnly = TRUE;

#ifdef _WX
  /* if it is a source document, reload its corresponding document */
  if (DocumentTypes[oldDoc] == docSource)
    oldDoc = GetDocFromSource (oldDoc);

  /* if the old doc doesn't exist it's not possible to create a new tab,
   * just open it in a new window */
  if (oldDoc == 0 && inNewWindow == 0)
    inNewWindow = TRUE;
#endif /* _WX */

  /* previous document */
  doc = oldDoc;
  if (replaceOldDoc && oldDoc > 0)
    /* the new document will replace another document in the same window */
    {
      // transmit the visibility to the new document
      visibility = TtaGetSensibility (doc, 1);
#ifdef _WX
      /* get the old document window */
      window_id = TtaGetDocumentWindowId( doc, -1 );
      /* get the old document page id */
      TtaGetDocumentPageId( doc, -1, &page_id, &page_position );
      /* force the document's page position because sometime oldDoc is
         a source document placed on the bottom part of the page */
      page_position = 1;
#endif /* _WX */
      if (DocumentTypes[doc] == docHTML ||
          DocumentTypes[doc] == docSVG ||
          DocumentTypes[doc] == docXml ||
          DocumentTypes[doc] == docMath ||
          DocumentTypes[doc] == docLibrary ||
          DocumentTypes[doc] == docImage)
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
      UpdateContextSensitiveMenus (doc, 1);
#ifdef _WX
      HideHSplitToggle (doc, 1);
      HideVSplitToggle (doc, 1);
#endif /* _WX */
      TtaFreeView (doc, 1);
      isOpen = TRUE;
      /* use the same document identifier */
      requested_doc = doc;
      oldDoc = 0;	/* the previous document doesn't exist any more */
      /* The toolkit has to do its job now */
      TtaHandlePendingEvents ();
    }
  else if (inNewWindow)
    {
      /* open the new document in a fresh window */
      isOpen = FALSE;
      requested_doc = 0;
    }
  else
    {
      /* open the new document in the same window but in a fresh page */
      requested_doc = 0;
#ifdef _WX
      isOpen = TRUE;
      if (docType == docSource)
        {
          /* source view is open it into the same page as formatted view */
          window_id = TtaGetDocumentWindowId( doc, -1 );
          TtaGetDocumentPageId( doc, -1, &page_id, &page_position );
          page_position = 2;
        }
      else
        {
          // the document is displayed in a window
          window_id = TtaGetDocumentWindowId( doc, -1 );
          wxASSERT(window_id > 0);
          page_id   = TtaGetFreePageId( window_id );
          page_position = 1;
        }
#else /* _WX */
      isOpen = FALSE; /* the menu and icons are already created */
#endif /* _WX */
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
        {
          TtaSetPSchema (doc, "AnnotP");
          profile = L_Annot; /* force the profile */
        }
#ifdef BOOKMARKS
      else if (docType == docBookmark)
        {
          TtaSetPSchema (doc, "TopicsP");
          profile = L_Bookmarks; /* force the profile */
        }
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
          TtaSetStructureChecking (FALSE, doc);
          TtaInsertSibling (comment, root, TRUE, doc);
          TtaSetStructureChecking (TRUE, doc);
          strcpy (buffer, " Created by ");
          strcat (buffer, TtaGetAppName());
          strcat (buffer, " ");
          strcat (buffer, TtaGetAppVersion());
          strcat (buffer, ", see http://www.w3.org/Amaya/ ");
          leaf = TtaGetFirstLeaf (comment);
          lang = TtaGetLanguageIdFromScript('L');
          TtaSetTextContent (leaf, (unsigned char *)buffer, lang, doc);
        }

      if (docType == docBookmark)
        TtaSetNotificationMode (doc, 0); /* we don't need notif. in bookmarks */
      else
        TtaSetNotificationMode (doc, 1);
       
      /* store the profile of the new document */
      TtaSetDocumentProfile (doc, profile);

      if (method == CE_MAKEBOOK || method == CE_TEMPLATE)
        // it's not necessary to create the document window
        return doc;

      /* gets registered window parameters */
      GiveWindowGeometry (doc, docType, method, &x, &y, &w, &h);
#ifdef _WX
      /* create a new window if needed */
      if (window_id == -1)
        {
          /* get the parent window because the log should stay on top of his parent */
          int parent_window_id = TtaGetDocumentWindowId( oldDoc, -1 );
          if (docType == docLog)
            window_id = TtaMakeWindow(x, y, w, h, WXAMAYAWINDOW_SIMPLE, parent_window_id );
          else
            /* a normal window should never had a parent ! */
            window_id = TtaMakeWindow(x, y, w, h, WXAMAYAWINDOW_NORMAL, 0 );
          page_id = TtaGetFreePageId( window_id );
          page_position = 1;
        }
#endif /* _WX */

#ifdef _WX
      /* init default documents menus states: enable/disable, toggle/untoggle
       * need to be done before frame creation because the active frame will
       * use it to refresh the menus */
      if (!replaceOldDoc || !isOpen)
        TtaInitMenuItemStats(doc);
      TtaInitTopMenuStats(doc);
#endif /* _WX */

      DocumentTypes[doc] = docType;
      /* open the main view */
      if (docType == docLog)
        {
          /* without menu bar */
          mainView = TtaOpenMainView (doc, DocumentTypeNames[docType], x, y,
                                      w, h, FALSE, FALSE,
                                      window_id, page_id, page_position);
          if (docType == docLog)
            // apply style attached to log files
            SetStyleOfLog (doc);
        }
      else if (docType == docLibrary && method == CE_RELATIVE)
        /* without menu bar */
        mainView = TtaOpenMainView (doc, DocumentTypeNames[docType], x, y,
                                    w, h, FALSE, TRUE,
                                    window_id, page_id, page_position);
      else
        mainView = TtaOpenMainView (doc, DocumentTypeNames[docType], x, y,
                                    w, h, TRUE, TRUE,
                                    window_id, page_id, page_position);
      if (mainView == 0)
        {
          TtaCloseDocument (doc);
          return (0);
        }

#ifdef _WX
      /* init the zoom factor to 0 if the document isn't replaced */
      if (!replaceOldDoc)
        TtaSetZoom (doc, -1, 0);
#endif /* _WX */

       /* update the menus according to the profile */
       /* By default no log file */
      TtaSetItemOff (doc, 1, File, BShowLogFile);
#ifndef TEMPLATES
      TtaSetItemOff (doc, 1, File, BTemplate);
#endif /* TEMPLATES */

#ifndef DAV    /* don't active the WebDAV menu if flag is off */
      TtaSetMenuOff (doc, 1, Cooperation_);
      TtaSetItemOff (doc, 1, Edit_, BCoopConfig);
#endif  /* DAV */

      /* do we have to redraw buttons and menus? */
      reinitialized = (docType != DocumentTypes[doc]);
      if (docType == docLog || docType == docLibrary || docType == docSource)
        {
#ifdef _WX
#ifdef _SVG
          if (docType == docLibrary)
            /* Initialize SVG Library Buffer string */
            TtaAddTextZone (doc, 1, TtaGetMessage (AMAYA,  AM_OPEN_URL),
                            FALSE, (Proc)OpenLibraryCallback, SVGlib_list);
#endif /* _SVG */
#endif /* _WX */

#ifndef _WX
          TtcSwitchButtonBar (doc, 1); /* no button bar */
          if (docType != docLibrary)
            TtcSwitchCommands (doc, 1); /* no command open */
#ifdef _SVG
          else
            /* Initialize SVG Library Buffer string */
            TtaAddTextZone (doc, 1, TtaGetMessage (AMAYA,  AM_OPEN_URL),
                            FALSE, (Proc)OpenLibraryCallback, SVGlib_list);
#endif /* _SVG */
#endif /* _WX */
        }
      else if (!isOpen)
        /* if isOpen is true, it means that a new window has been opened for this document 
         * we must initialize each things that must be initialized once for the window */
        {
          /* create the  button bar */
#ifndef _WX
          /* use a new window: Create all buttons */
          iStop =TtaAddButton (doc, 1, stopN, (Proc)StopTransfer,"StopTransfer",
                               TtaGetMessage (LIB,TMSG_BUTTON_INTERRUPT),
                               TBSTYLE_BUTTON, FALSE);
          iBack = TtaAddButton (doc, 1, iconBackNo, (Proc)GotoPreviousHTML,
                                "GotoPreviousHTML",
                                TtaGetMessage (LIB,TMSG_BUTTON_PREVIOUS),
                                TBSTYLE_BUTTON, FALSE);
          iForward = TtaAddButton (doc, 1, iconForwardNo, (Proc)GotoNextHTML,
                                   "GotoNextHTML",
                                   TtaGetMessage (LIB,TMSG_BUTTON_NEXT),
                                   TBSTYLE_BUTTON, FALSE);
          iReload = TtaAddButton (doc, 1, iconReload, (Proc)Reload, "Reload",
                                  TtaGetMessage (LIB,TMSG_BUTTON_RELOAD),
                                  TBSTYLE_BUTTON, TRUE);
          iHome = TtaAddButton (doc, 1, iconHome, (Proc)GoToHome, "GoToHome",
                                TtaGetMessage (LIB,TMSG_BUTTON_HOME),
                                TBSTYLE_BUTTON, TRUE);
          /* SEPARATOR */
          TtaAddButton (doc, 1, None, NULL, NULL, NULL, TBSTYLE_SEP, FALSE);
          iSave = TtaAddButton (doc, 1, iconSaveNo, (Proc)SaveDocument,
                                "SaveDocument",
                                TtaGetMessage (LIB,TMSG_BUTTON_SAVE),
                                TBSTYLE_BUTTON, FALSE);
          iPrint = TtaAddButton (doc, 1, iconPrint, (Proc)SetupAndPrint,  "SetupAndPrint",
                                 TtaGetMessage (LIB,TMSG_BUTTON_PRINT),
                                 TBSTYLE_BUTTON, TRUE);
          iFind = TtaAddButton (doc, 1, iconFind, (Proc)TtcSearchText,
                                "TtcSearchText", 
                                TtaGetMessage (LIB,TMSG_BUTTON_SEARCH),
                                TBSTYLE_BUTTON, TRUE);
          /* SEPARATOR */
          TtaAddButton (doc, 1, None, NULL, NULL, NULL, TBSTYLE_SEP, FALSE);
          iI =  TtaAddButton (doc, 1, iconI, (Proc)SetOnOffEmphasis,
                              "SetOnOffEmphasis",
                              TtaGetMessage (LIB,TMSG_BUTTON_ITALICS),
                              TBSTYLE_CHECK, TRUE);
          iB =  TtaAddButton (doc, 1, iconB, (Proc)SetOnOffStrong, "SetOnOffStrong",
                              TtaGetMessage (LIB,TMSG_BUTTON_BOLD),
                              TBSTYLE_CHECK, TRUE);
          iT = TtaAddButton (doc, 1, iconT, (Proc)SetOnOffCode, "SetOnOffCode",
                             TtaGetMessage (LIB,TMSG_BUTTON_CODE),
                             TBSTYLE_CHECK, TRUE);
          /* SEPARATOR */
          TtaAddButton (doc, 1, None, NULL, NULL, NULL, TBSTYLE_SEP, FALSE);
          iImage = TtaAddButton (doc, 1, iconImage, (Proc)CreateImage,
                                 "CreateImage", 
                                 TtaGetMessage (LIB,TMSG_BUTTON_IMG),
                                 TBSTYLE_BUTTON, TRUE);
          iH1 = TtaAddButton (doc, 1, iconH1, (Proc)CreateHeading1,
                              "CreateHeading1", 
                              TtaGetMessage (LIB,TMSG_BUTTON_H1),
                              TBSTYLE_BUTTON, TRUE);
          iH2 = TtaAddButton (doc, 1, iconH2, (Proc)CreateHeading2,
                              "CreateHeading2", 
                              TtaGetMessage (LIB,TMSG_BUTTON_H2),
                              TBSTYLE_BUTTON, TRUE);
          iH3 = TtaAddButton (doc, 1, iconH3, (Proc)CreateHeading3,
                              "CreateHeading3", 
                              TtaGetMessage (LIB,TMSG_BUTTON_H3),
                              TBSTYLE_BUTTON, TRUE);
          iBullet = TtaAddButton (doc, 1, iconBullet, (Proc)CreateList,
                                  "CreateList",
                                  TtaGetMessage (LIB,TMSG_BUTTON_UL),
                                  TBSTYLE_BUTTON, TRUE);
          iNum = TtaAddButton (doc, 1, iconNum, (Proc)CreateNumberedList,
                               "CreateNumberedList",
                               TtaGetMessage (LIB,TMSG_BUTTON_OL),
                               TBSTYLE_BUTTON, TRUE);
          iDL = TtaAddButton (doc, 1, iconDL, (Proc)CreateDefinitionList,
                              "CreateDefinitionList",
                              TtaGetMessage (LIB,TMSG_BUTTON_DL),
                              TBSTYLE_BUTTON, TRUE);
          iLink = TtaAddButton (doc, 1, iconLink, (Proc)CreateOrChangeLink,
                                "CreateOrChangeLink",
                                TtaGetMessage (LIB,TMSG_BUTTON_LINK),
                                TBSTYLE_BUTTON, TRUE);
          iTable = TtaAddButton (doc, 1, iconTable, (Proc)CreateTable,
                                 "CreateTable",
                                 TtaGetMessage (LIB,TMSG_BUTTON_TABLE),
                                 TBSTYLE_BUTTON, TRUE);
          AddMathButton (doc, 1);
#ifdef _SVG
          AddGraphicsButton (doc, 1);
          AddLibraryButton (doc, 1);
#ifdef _GL
          AddAnimPlayButton (doc, 1);
#endif /*_GL*/
#endif /* _SVG */
#endif /* _WX */

#ifndef _WX
          TtaAddTextZone (doc, 1, TtaGetMessage (AMAYA,  AM_OPEN_URL),
                          TRUE, (Proc)TextURL, URL_list);
#endif /* _WX */

#if defined(_GTK) || defined(_WINGUI)
          /* initial state for menu entries */
          TtaSetItemOff (doc, 1, File, BBack);
          TtaSetItemOff (doc, 1, File, BForward);
          TtaSetItemOff (doc, 1, File, BSave);
          TtaSetItemOff (doc, 1, File, BSynchro);

          /* button bar On/Off => TODO for WX */
          if (SButtons[doc] && docType != docBookmark)
            TtaSetToggleItem (doc, 1, Views, TShowButtonbar, TRUE);
          else
            /* hide buttons */
            TtcSwitchButtonBar (doc, 1);
	   
          /* Url bar On/Off => TODO for WX */
          if (SAddress[doc])
            TtaSetToggleItem (doc, 1, Views, TShowTextZone, TRUE);
          else
            /* hide the address */
            TtcSwitchCommands (doc, 1);

          /* MapArea menu item */
          TtaSetToggleItem (doc, 1, Views, TShowMapAreas, MapAreas[doc]);
          TtaSetMenuOff (doc, 1, Attributes_);

          /* SplitView menu items */
          TtaSetToggleItem (doc, 1, Views, TSplitHorizontally, HSplit[doc]);
          TtaSetToggleItem (doc, 1, Views, TSplitVertically, VSplit[doc]);

          TtaSetMenuOff (doc, 1, Attributes_);

          /* if we open the new document in a new view, control */
          /* is transferred from previous document to new document */
          if (oldDoc != doc && oldDoc != 0)
            {
              ResetStop (oldDoc);
              /* clear the status line of previous document */
              TtaSetStatus (oldDoc, 1, " ", NULL);
              ActiveTransfer (doc);
            }
#endif /* _GTK || _WINGUI */
        } /* isOpen */
    }

  if (!replaceOldDoc || !isOpen)
    {
#ifdef _WX
      /* initial state for menu entries */
      TtaSetItemOff (doc, 1, File, BBack);
      TtaSetItemOff (doc, 1, File, BForward);
      TtaSetItemOff (doc, 1, File, BSave);
      TtaSetItemOff (doc, 1, File, BSynchro);
      TtaSetMenuOff (doc, 1, Attributes_);

      /* init MapAreas menu item */
      TtaSetToggleItem (doc, 1, Views, TShowMapAreas, MapAreas[doc]);
#endif /* _WX */
    }

  /* store the new document type */
  DocumentTypes[doc] = docType;
#if _WX
  /* now be sure that the urlbar is setup */
  TtaAddTextZone ( doc, 1, TtaGetMessage (AMAYA,  AM_OPEN_URL),
                   TRUE, (Proc)TextURL, URL_list );
#endif /* _WX */

  if ((DocumentTypes[doc] == docHTML ||
       DocumentTypes[doc] == docSVG ||
       DocumentTypes[doc] == docXml ||
       DocumentTypes[doc] == docMath ||
       DocumentTypes[doc] == docLibrary)
#ifndef _WX
      && !replaceOldDoc
#endif /* _WX */
      )
    {
      /* init show target menu item */
      TtaGetEnvBoolean ("SHOW_TARGET", &show);
      if (show)
        ShowTargets (doc, 1);
      else
        TtaSetToggleItem (doc, 1, Views, TShowTargets, FALSE);
    }
  if (reinitialized || !isOpen)
    {
      if (reinitialized && visibility == 4)
        {
          // restore the visibility
          TtaSetSensibility (doc, 1, visibility);
          TtaSetToggleItem (doc, 1, Views, TShowTargets, TRUE);
        }
      /* now update menus and buttons according to the document status */
      if (DocumentTypes[doc] == docLog ||
          DocumentTypes[doc] == docLibrary ||
          DocumentTypes[doc] == docBookmark)
        {
          if (DocumentTypes[doc] != docBookmark)
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
              TtaSetItemOff (doc, 1, File, BReload);
              TtaSetItemOff (doc, 1, File, BBack);
              TtaSetItemOff (doc, 1, File, BForward);
#ifdef ANNOTATIONS
              TtaSetMenuOff (doc, 1, Annotations_);
              TtaSetItemOff (doc, 1, Edit_, BConfigAnnotations);
#endif /* ANNOTATIONS */
            }
          TtaSetItemOff (doc, 1, File, BSave);
          TtaSetItemOff (doc, 1, File, BSynchro);
          TtaSetItemOff (doc, 1, File, BDocInfo);
          TtaSetItemOff (doc, 1, File, BSetUpandPrint);
          TtaSetItemOff (doc, 1, File, BPrint);
          SetCharsetMenuOff (doc, 1); /* no charset commands */
          /* invalid the DoctypeMenu */
          TtaSetItemOff (doc, 1, File, BRemoveDoctype);
          TtaSetItemOff (doc, 1, File, BAddDoctype);
          TtaSetItemOff (doc, 1, File, BDoctypeXhtml11);
          TtaSetItemOff (doc, 1, File, BDoctypeXhtmlTransitional);
          TtaSetItemOff (doc, 1, File, BDoctypeXhtmlStrict);
          TtaSetItemOff (doc, 1, File, BDoctypeXhtmlBasic);
          TtaSetItemOff (doc, 1, File, BDoctypeHtmlTransitional);
          TtaSetItemOff (doc, 1, File, BDoctypeHtmlStrict);
          TtaSetItemOff (doc, 1, Edit_, BTransform);
          TtaSetMenuOff (doc, 1, Types);
          TtaSetMenuOff (doc, 1, XMLTypes);
          TtaSetMenuOff (doc, 1, Links);
          TtaSetMenuOff (doc, 1, Views);
          TtaSetMenuOff (doc, 1, Style);
          TtaSetMenuOff (doc, 1, Attributes_);
          TtaSetMenuOff (doc, 1, Help_);
          if (docType != docBookmark)
            {
              TtaSetItemOff (doc, 1, File, BExit);
              TtaSetMenuOff (doc, 1, Edit_);
              if (docType != docLibrary)
                TtaSetDocumentAccessMode (doc, 0);
            }
        }
      else if (DocumentTypes[doc] == docText ||
               DocumentTypes[doc] == docSource ||
               DocumentTypes[doc] == docCSS ||
               DocumentTypes[doc] == docMath)
        {
#ifdef _WX
          TtaSetMenuOn (doc, 1, Style);
          if (DocumentTypes[doc] == docMath)
            {
              TtaSetItemOn (doc, 1, Style, BCreateClass);
              TtaSetItemOn (doc, 1, Style, BShowAppliedStyle);
              TtaSetItemOn (doc, 1, Style, BLinkCSS);
              TtaSetItemOn (doc, 1, Style, BOpenCSS);
              TtaSetItemOn (doc, 1, Style, BDisableCSS);
              TtaSetItemOn (doc, 1, Style, BEnableCSS);
              TtaSetItemOn (doc, 1, Style, BRemoveCSS);
            }
          else
            {
              TtaSetItemOff (doc, 1, Style, BCreateClass);
              TtaSetItemOff (doc, 1, Style, BShowAppliedStyle);
              TtaSetItemOff (doc, 1, Style, BLinkCSS);
              TtaSetItemOff (doc, 1, Style, BOpenCSS);
              TtaSetItemOff (doc, 1, Style, BDisableCSS);
              TtaSetItemOff (doc, 1, Style, BEnableCSS);
              TtaSetItemOff (doc, 1, Style, BRemoveCSS);
            }
#else /* _WX */
          if (DocumentTypes[doc] == docMath)
            TtaSetMenuOn (doc, 1, Style);
          else
            TtaSetMenuOff (doc, 1, Style);
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
          TtaChangeButton (doc, 1, iTable, iconTableNo, FALSE);
          TtaChangeButton (doc, 1, iLink, iconLinkNo, FALSE);
#endif /* _WX */
          TtaSetItemOff (doc, 1, Views, TShowMapAreas);
          TtaSetItemOff (doc, 1, Views, TShowTargets);
          TtaSetMenuOff (doc, 1, Doctype1);
          TtaSetMenuOff (doc, 1, Types);
          TtaSetMenuOff (doc, 1, Links);
          if (DocumentTypes[doc] == docMath)
            {
              TtaSetItemOff (doc, 1, XMLTypes, BShowLibrary);
              TtaSetItemOff (doc, 1, XMLTypes, BAddNewModel);
              TtaSetMenuOn (doc, 1, XMLTypes);
              TtaSetMenuOn (doc, 1, Views);
              TtaSetMenuOn (doc, 1, Attributes_);
#ifdef _SVG
              SwitchIconGraph (doc, 1, FALSE);
              SwitchIconLibrary (doc, 1, FALSE);
#endif /* _SVG */
            }
          else
            {
#ifdef _WX
              if (DocumentTypes[doc] != docSource)
#endif /* _WX */
                TtaSetMenuOff (doc, 1, Views);
              TtaSetItemOff (doc, 1, Edit_, BTransform);
              SwitchIconMath (doc, 1, FALSE);
              TtaSetMenuOff (doc, 1, XMLTypes);
              TtaSetMenuOff (doc, 1, Attributes_);
#ifdef ANNOTATIONS
              TtaSetMenuOff (doc, 1, Annotations_);
#endif /* ANNOTATIONS */
            }
        }
      else if (DocumentTypes[doc] == docAnnot)
        {
#ifndef _WX
          TtcSwitchCommands (doc, 1); /* no command open */
#endif /* _WX */
          TtaSetItemOff (doc, 1, Views, BShowAlternate);
          TtaSetItemOff (doc, 1, Views, BShowToC);
          TtaSetItemOff (doc, 1, Views, BShowSource);
          TtaSetItemOff (doc, 1, Views, BShowTimeLine);
        }
      else
        {
          TtaSetMenuOn (doc, 1, Views);
          TtaSetItemOn (doc, 1, Views, TShowTargets);
          TtaSetItemOn (doc, 1, Views, BShowLinks);
          /* turn off the assign annotation buttons (should be
             contextual */
          TtaSetItemOff (doc, 1, Annotations_, BReplyToAnnotation);
          TtaSetItemOff (doc, 1, Annotations_, BMoveAnnotSel);
          TtaSetItemOff (doc, 1, Annotations_, BMoveAnnotXPtr);
          TtaSetItemOff (doc, 1, Annotations_, BPostAnnot);
          if (DocumentTypes[doc] == docHTML)
            {
              TtaSetItemOn (doc, 1, Views, TShowMapAreas);
              TtaSetItemOn (doc, 1, Views, BShowAlternate);
              TtaSetItemOn (doc, 1, Views, BShowToC);
              TtaSetItemOn (doc, 1, Types, BTitle);
              if (profile == L_Strict || profile == L_Basic)
                {
                  TtaSetMenuOff (doc, 1, XMLTypes);
                  TtaSetItemOff (doc, 1, Views, BShowTimeLine);
                }
              else
                TtaSetItemOn (doc, 1, Views, BShowTimeLine);
            }
          else
            {
              TtaSetItemOff (doc, 1, Views, TShowMapAreas);
              TtaSetItemOff (doc, 1, Views, BShowToC);
              if (DocumentTypes[doc] == docSVG)
                {
                  TtaSetItemOn (doc, 1, Views, BShowAlternate);
                  TtaSetItemOff (doc, 1, Types, BTitle);
                  TtaSetItemOn (doc, 1, Views, BShowTimeLine);
                }
              else
                {
                  TtaSetItemOff (doc, 1, Views, BShowAlternate);
                  TtaSetItemOff (doc, 1, Views, BShowTimeLine);
                }
            }
        }
    }

#ifdef _WX
  // show the window if it's not allready done
  TtaShowWindow( window_id, TRUE );
#endif /* _WX */
  return (doc);
}

/*----------------------------------------------------------------------
  CreateHTMLContainer create an HTML container for an image
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
  file = TtaWriteOpen (tempfile);
  fprintf (file, "<html><head><title>%s</title></head><body>", docname);
  if (local)
    fprintf (file, "<img src=\"%s\" alt=\"image\">", pathname);
  else
    fprintf (file, "<img src=\"internal:%s\" alt=\"image\">", pathname);
  fprintf (file, "</body></html>");
  TtaWriteClose (file);
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
  ReparseAs
  Load current document considering it's a HTML document and/or
  with a new charset (charset != UNDEFINED_CHARSET).
  ----------------------------------------------------------------------*/
void ReparseAs (Document doc, View view, ThotBool asHTML,
                CHARSET charset)
{
  CHARSET         doc_charset;
  DocumentType    thotType;
  char           *localFile = NULL;
  char            documentname[MAX_LENGTH];
  char            s[MAX_LENGTH], charsetname[MAX_LENGTH];
  int             i, parsingLevel;
  ThotBool        plaintext;
  ThotBool        xmlDec, withDoctype, isXML, useMath, isKnown;

  if (DocumentURLs[doc] == NULL ||
      (asHTML && !DocumentMeta[doc]->xmlformat))
    /* the document is not concerned by this option */
    return;
  if (!CanReplaceCurrentDocument (doc, view))
    /* abort the command */
    return;

  /* Initialize the LogFile variables */
  CleanUpParsingErrors ();
  /* remove the PARSING.ERR file */
  RemoveParsingErrors (doc);
  /* Remove the previous namespaces declaration */
  TtaFreeNamespaceDeclarations (doc);
  localFile = GetLocalPath (doc, DocumentURLs[doc]);
  TtaExtractName (localFile, s, documentname);
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
            TtaSetItemOff (doc, 1, File, BShowLogFile);
          }
      }

  /* Removes all CSS informations linked with the document */
  RemoveDocCSSs (doc);  
  /* Free access keys table */
  TtaRemoveDocAccessKeys (doc);
  if (asHTML)
    {
      DocumentMeta[doc]->xmlformat = FALSE;
      DocumentMeta[doc]->compound = FALSE;
    }
  if (charset != UNDEFINED_CHARSET &&
      charset != TtaGetDocumentCharset (doc))
    {
      /* Update the charset info */
      TtaSetDocumentCharset (doc, charset, FALSE);
      if (DocumentMeta[doc]->charset)
        TtaFreeMemory (DocumentMeta[doc]->charset);
      DocumentMeta[doc]->charset = TtaStrdup (TtaGetCharsetName (charset));
    }
  /* parse with the HTML parser */
  if (DocumentMeta[doc]->xmlformat)
    {
      /* check if there is an XML declaration with a charset declaration */
      charsetname[0] = EOS;
      CheckDocHeader (localFile, &xmlDec, &withDoctype, &isXML, &useMath, &isKnown,
                      &parsingLevel, &doc_charset, charsetname, &thotType);
      StartXmlParser (doc, localFile, documentname, s,
                      localFile, xmlDec, withDoctype, useMath, FALSE);
    }
  else
    {
      plaintext = (DocumentTypes[doc] == docCSS ||
                   DocumentTypes[doc] == docText);
      StartParser (doc, localFile, documentname, s, localFile, plaintext, FALSE);
    }

  /* fetch and display all images referred by the document */
  DocNetworkStatus[doc] = AMAYA_NET_ACTIVE;
  FetchAndDisplayImages (doc, AMAYA_LOAD_IMAGE, NULL);
  DocNetworkStatus[doc] = AMAYA_NET_INACTIVE;
  if (!asHTML &&
      !XMLNotWellFormed && !XMLInvalidToken && !XMLCharacterNotSupported &&
      !XMLErrorsFoundInProfile && !XMLErrorsFound)
    {
      /* No error found -> Update the source of the document */
      TtaSetDocumentModified (doc);
      Synchronize (doc, view);
      TtaSetDocumentUnmodified (doc);
    }
  /* check parsing errors */
  CheckParsingErrors (doc);
  TtaFreeMemory (localFile);
}

/*----------------------------------------------------------------------
  ParseAsHTML
  Load current document considering it's a HTML document
  ----------------------------------------------------------------------*/
void ParseAsHTML (Document doc, View view)
{
  char           *localFile = NULL;
  char            documentname[MAX_LENGTH];
  char            s[MAX_LENGTH];

  /* Initialize the LogFile variables */
  CleanUpParsingErrors ();
  /* remove the PARSING.ERR file */
  RemoveParsingErrors (doc);
  /* Remove the previous namespaces declaration */
  TtaFreeNamespaceDeclarations (doc);
  localFile = GetLocalPath (doc, DocumentURLs[doc]);
  TtaExtractName (localFile, s, documentname);
  /* Removes all CSS informations linked with the document */
  RemoveDocCSSs (doc);  
  /* Free access keys table */
  TtaRemoveDocAccessKeys (doc);
  // ignore the XML format
  if (DocumentMeta && DocumentMeta[doc]->xmlformat)
    DocumentMeta[doc]->xmlformat = FALSE;
  StartParser (doc, localFile, documentname, s, localFile, FALSE, FALSE);
  // restore the XML format
  if (DocumentMeta && DocumentMeta[doc]->xmlformat)
    DocumentMeta[doc]->xmlformat = TRUE;
  /* fetch and display all images referred by the document */
  DocNetworkStatus[doc] = AMAYA_NET_ACTIVE;
  FetchAndDisplayImages (doc, AMAYA_LOAD_IMAGE, NULL);
  DocNetworkStatus[doc] = AMAYA_NET_INACTIVE;
  /* check parsing errors */
  CheckParsingErrors (doc);
  TtaFreeMemory (localFile);
}

#ifdef _SVG
/*----------------------------------------------------------------------
  DisplaySVGtitle
  Get the first child of the root element of document doc that is of
  type title and display it as the title of the document window.
  doc must be a SVG document.
  ----------------------------------------------------------------------*/
static void DisplaySVGtitle (Document doc)
{
  Element       root, child;
  SSchema       SVGschema;
  ElementType   elType;
  ThotBool      found;

  root = TtaGetRootElement (doc);
  SVGschema = TtaGetDocumentSSchema (doc);
  child = TtaGetFirstChild (root);
  found = FALSE;
  while (child && !found)
    {
      elType = TtaGetElementType (child);
      if (elType.ElSSchema == SVGschema && elType.ElTypeNum == SVG_EL_title)
        found = TRUE;
      else
        TtaNextSibling (&child);
    }
  if (found)
    UpdateTitle (child, doc);
}
#endif /* _SVG */

/*----------------------------------------------------------------------
  LoadDocument parses the new document and stores its path (or
  URL) into the document table.
  For a local loading, the parameter tempfile must be an empty string.
  For a remote loading, the parameter tempfile gives the file name that
  contains the current copy of the remote file.
  Parameter realdocname is not NULL when the document is restored
  ----------------------------------------------------------------------*/
Document LoadDocument (Document doc, char *pathname,
                       char *form_data, char *initial_url,
                       int method, char *tempfile,
                       char *documentname, AHTHeaders *http_headers,
                       ThotBool history, ThotBool *inNewWindow,
                       char *realdocname)
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
  char               *tempdir;
  char               *s, *localdoc;
  char               *content_type, *reason;
  char               *http_content_type;
  int                 i, j;
  int                 docProfile;
  DocumentType        thotType;
  char                local_content_type[MAX_LENGTH];
  ThotBool            unknown;
  ThotBool            contentImage, contentText, contentApplication;
  ThotBool            plainText;
  ThotBool            xmlDec, withDoctype, useMath, isXML, isknown;
  ThotBool            isRDF;

  isRDF = FALSE;
  docType = docText;
  unknown = TRUE;
  tempdir = localdoc = NULL;
  charsetname[0] = EOS;
  local_content_type[0] = EOS;
  http_content_type = HTTP_headers (http_headers, AM_HTTP_CONTENT_TYPE);
  /* sometimes video or audio content is sent with an erroneous mime-type
     "text/plain". In that case, ignore the mime-type (glitch) */
  if (http_content_type && !strcmp (http_content_type, "text/plain") &&
      IsUndisplayedName (pathname))
    http_content_type = NULL;
  /* make a copy we can modify */
  if (http_content_type)
    content_type = TtaStrdup (http_content_type);
  else
    content_type = NULL;

  /* Check informations within the document */
  if (tempfile[0] != EOS)
    s = tempfile;
  else
    s = pathname;

  CheckDocHeader (s, &xmlDec, &withDoctype, &isXML, &useMath, &isknown,
                  &docProfile, &charset, charsetname, &thotType);

  /* Check charset information in a meta */
  if (charset == UNDEFINED_CHARSET)
    CheckCharsetInMeta (s, &metacharset, charsetname);
 
  if (method == CE_CSS)
    {
      /* we're loading a CSS file */
      docType = docCSS;
      /* if a CSS document has a xml header, CheckDocHeader will detect that the document is XML !
       * (ex: http://www.inrialpes.fr has a css with a xml header :( )
       * when we know that the document is CSS (method == CE_CSS) we should force docType to docCSS */
      isXML   = FALSE;
      unknown = FALSE;
    }
  else if (method == CE_LOG)
    {
      docType = docLog;
      isXML   = FALSE;
      unknown = FALSE;
    }
  else if (content_type == NULL || content_type[0] == EOS)
    /* Local document - no content type  */
    {
      /* check file name extension */
      if (isXML)
        {
          /* it seems to be a XML document */
          if (DocumentTypes[doc] == docLibrary && thotType == docHTML)
            docType = docLibrary;
          else
            docType = thotType;
          unknown = FALSE;
          if (IsRDFName (pathname))
            isRDF = TRUE;
        }
      else if (IsRDFName (pathname))
        {
          /* it's an RDF document. By default we assume we will
             parse it as generic XML */
          docType = thotType;
          isRDF = TRUE;
          unknown = FALSE;
        }
      else if (IsCSSName (pathname))
        {
          docType = docCSS;
          docProfile = L_CSS;
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
          docProfile = L_MathML;
          isXML = TRUE;
          unknown = FALSE;
        }
      else if (IsSVGName (pathname))
        {
          docType = docSVG;
          docProfile = L_SVG;
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
      if (isRDF)
        strcpy (local_content_type , "application/xml+rdf");
      else if (docType == docCSS)
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
        /* not defined yet */
        local_content_type[0] = EOS;
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
                   (!strncasecmp (&content_type[i+1], "x-sh", 4) ||
                    !strncasecmp (&content_type[i+1], "x-javascript", 12)))
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
          else if (MultipleBookmarks() &&
                   !strncasecmp (&content_type[i+1], "rdf+xml", 7))
            {
              /* it's an RDF document. By default we assume we will
                 parse it as generic XML */
              isXML = TRUE;
              isRDF = TRUE;
#ifdef XML_GENERIC      
              docType = docXml;
#else /* XML_GENERIC */
              docType = docText;
#endif /* XML_GENERIC */
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

#ifdef BOOKMARKS
  if (isRDF)
    {
      /* verify if it contains bookmarks */
      /* we detected some bookmarks, in this rdf document */
      if (BM_Open (pathname, s))
        docType = docBookmark;
    }
#endif /* BOOKMARKS */
  
  if (unknown && tempfile[0] != EOS)
    {
      /* The document is not a supported format and cannot be parsed */
      /* rename the temporary file */
      strcpy (SavingFile, tempfile);
      SavingDocument = 0;
      SavingObject = 0;
      localdoc = (char *)TtaGetMemory (MAX_LENGTH);
      TtaExtractName (pathname, tempfile, localdoc);
      /* reinitialize directories and document lists */
      strcpy (SavePath, DirectoryName);
      strcpy (SaveName, localdoc);
      strcpy (localdoc, SavePath);
      strcat (localdoc, DIR_STR);
      strcat (localdoc, SaveName);
      ResetStop (doc);
      InitSaveObjectForm (doc, 1, SavingFile, localdoc);
    }
  else if (pathname[0] != EOS)
    {
      if (method != CE_MAKEBOOK && method != CE_TEMPLATE)
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
                  newdoc = InitDocAndView (doc,
                                           FALSE /* replaceOldDoc */,
                                           TRUE /* inNewWindow */,
                                           documentname, docType, 0, FALSE,
                                           docProfile, method);
                  ResetStop (doc);
                  /* clear the status line of previous document */
                  TtaSetStatus (doc, 1, " ", NULL);
                  ActiveTransfer (newdoc);
                }
              else
                /* replace the current document by a new one */
                newdoc = InitDocAndView (doc,
                                         TRUE /* replaceOldDoc */,
                                         FALSE /* inNewWindow */,
                                         documentname, docType, 0, FALSE,
                                         docProfile, method);
            }
          else if (method == CE_ABSOLUTE  || method == CE_HELP ||
                   method == CE_FORM_POST || method == CE_FORM_GET)
            /* replace the current document by a new one */
            newdoc = InitDocAndView (doc,
                                     TRUE /* replaceOldDoc */,
                                     FALSE /* inNewWindow */,
                                     documentname, docType, 0, FALSE,
                                     docProfile, method);
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
#ifdef _WX
              /* the LOG window labels have UTF8 charset on wxWidgets interface */
              charset = UTF_8;
#endif /* _WX */
            }
          else if (docType != DocumentTypes[doc] && DocumentTypes[doc] != docLibrary)
            /* replace the current document by a new one */
            newdoc = InitDocAndView (doc,
                                     TRUE /* replaceOldDoc */,
                                     FALSE /* inNewWindow */,
                                     documentname, docType, 0, FALSE,
                                     docProfile, method);
          else
            {
              /* document already initialized */
              newdoc = doc;
              /* store the profile of the new document */
              /* and update the menus according to it */
              TtaSetDocumentProfile (newdoc, docProfile);
            }
        }
      else
        {
          DocumentTypes[doc] = docType;
          newdoc = doc;
        }

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
          /* allocate and initialize a teporary document */
          localdoc = GetLocalPath (newdoc, pathname);
          TtaFileUnlink (localdoc);
          if (doc != newdoc)
            {
              /* now we can rename the local name of a remote document */
              TtaFileCopy (tempfile, localdoc);
              TtaFileUnlink (tempfile);
              /* if it's an IMAGEfile, we copy it too to the new directory */
              if (DocumentTypes[newdoc] == docImage)
                MoveImageFile (doc, newdoc, documentname);
            }
          else if (DocumentTypes[newdoc] == docCSS)
            TtaFileCopy (tempfile, localdoc);
          /* now we can rename the local name of a remote document */
          else
            /* now we can rename the local name of a remote document */
            TtaFileRename (tempfile, localdoc);
        }
      else
        {
          /* store a copy of the local document */
          /* allocate and initialize a teporary document */
          localdoc = GetLocalPath (newdoc, pathname);
	  
          TtaFileCopy (pathname, localdoc);
        }

#ifdef BOOKMARKS
      if (docType == docBookmark)
        /* update the corresponding bookmark context to point to the new tmpfile */
        BM_TempFileSet (pathname, localdoc);
#endif /* BOOKMARKS */


      /* store a copy of CSS files in the directory 0 */
      if (DocumentTypes[newdoc] == docCSS)
        { 
          css = SearchCSS (0, pathname, NULL, &pInfo);
          if (css == NULL)
            {
              /* store a copy of this new CSS context in .amaya/0 */
              s = GetLocalPath (0, pathname);
              TtaFileCopy (localdoc, s);
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
      if (realdocname)
        s = TtaStrdup (realdocname);
      else
        s = TtaStrdup (pathname);
      if (DocumentURLs[newdoc] != NULL)
        {
          TtaFreeMemory (DocumentURLs[newdoc]);
          DocumentURLs[newdoc] = NULL;
        }
      /* if the document was already loaded, warn the user */
      if (method != CE_TEMPLATE && IsDocumentLoaded (s, form_data))
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
      reason = HTTP_headers (http_headers, AM_HTTP_REASON);
      if (reason && strcasecmp (reason, "OK"))
        DocumentMeta[newdoc]->reason = TtaStrdup (reason);
      DocumentMeta[newdoc]->method = method;
      DocumentSource[newdoc] = 0;
      DocumentMeta[newdoc]->xmlformat = isXML;
      DocumentMeta[newdoc]->compound = FALSE;

      /* Clear the current status path */
      if (docType != docLog)
        TtaSetStatusSelectedElement (newdoc, 1, NULL);

      /* Set character encoding */
      DocumentMeta[newdoc]->charset = NULL;
      charEncoding = HTTP_headers (http_headers, AM_HTTP_CHARSET);
      httpcharset = TtaGetCharset (charEncoding);

#ifdef TEMPLATES
      DocumentMeta[newdoc]->template_version = NULL;
#endif /* TEMPLATES */

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
      else if (docType == docCSS || docType == docText)
        {
          TtaSetDocumentCharset (newdoc, ISO_8859_1, FALSE);
          DocumentMeta[newdoc]->charset = TtaStrdup ("iso-8859-1");
        }

      /*
      ** copy some HTTP headers to the metadata 
      */
      /* content-type */
      if (http_content_type)
          DocumentMeta[newdoc]->content_type = TtaStrdup (http_content_type);
      else if (local_content_type[0] != EOS)
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
          if (DocumentTypes[newdoc] != docLog)
            {
#ifdef _SVG
              if (DocumentTypes[newdoc] == docLibrary)
                {
                  SelectLibraryFromPath (DocumentURLs[newdoc]);
                  TtaSetTextZone (newdoc, 1, SVGlib_list);
                }
              else
#endif /* _SVG */
                TtaSetTextZone (newdoc, 1, URL_list);
            }
        }

      tempdir = (char *)TtaGetMemory (MAX_LENGTH);
      TtaExtractName (localdoc, tempdir, documentname);
      /* Now we forget the method CE_INIT. It's a standard method */
      if (DocumentMeta[newdoc]->method == CE_INIT)
        DocumentMeta[newdoc]->method = CE_ABSOLUTE;

      if (docType == docHTML ||
          docType == docSVG ||
#ifdef ANNOTATIONS
          (docType == docAnnot && annotBodyType != docText) ||
#endif /* ANNOTATIONS */
          docType == docBookmark ||
          docType == docXml ||
          docType == docLibrary ||
          docType == docMath)
        plainText = FALSE;
      else
        plainText = (docProfile == L_Other || docProfile == L_CSS);
    
#ifdef BOOKMARKS
      if (docType == docBookmark)
        BM_ViewBookmarks (newdoc, 1, FALSE);
      else
#endif /* BOOKMARKS */
        {
          // Get user information about read IDs
          TtaGetEnvBoolean ("CHECK_READ_IDS", &Check_read_ids);
          /* Calls the corresponding parser */
          if (DocumentMeta[newdoc]->xmlformat && !plainText)
            StartXmlParser (newdoc,	localdoc, documentname, tempdir,
                            pathname, xmlDec, withDoctype, useMath, FALSE);
          else
            StartParser (newdoc, localdoc, documentname, tempdir,
                         pathname, plainText, FALSE);
        }
      TtaFreeMemory (tempdir);
   
      /* Update the Doctype menu */
      UpdateEditorMenus (newdoc);

      if (*inNewWindow || newdoc != doc)
        /* the document is displayed in a different window */
        /* reset the history of the new window */
        InitDocHistory (newdoc);

#ifdef _SVG
      if (docType == docSVG)
        /* for a SVG document, get the first title element that is a child
           of the SVG root element and display its contents as the window
           title */
        DisplaySVGtitle (newdoc);
#endif /* _SVG */

      /* the document is loaded now */
      *inNewWindow = FALSE;

#ifdef ANNOTATIONS
      /* store the annotation body type (we have to wait until now as
         the metadata didn't exist before) */
      if (docType == docAnnot)
        ANNOT_bodyType_set (newdoc, annotBodyType);
#endif /* ANNOTATIONS */
    }
#ifdef _WX
  // refresh the XML panel
  TtaRefreshElementMenu (newdoc, 1);
#endif /* _WX */
  TtaFreeMemory (content_type);
  TtaFreeMemory (localdoc);
  return (newdoc);
}

/*----------------------------------------------------------------------
  Reload_callback
  The urlName is encoded with the default charset.
  ----------------------------------------------------------------------*/
void Reload_callback (int doc, int status, char *urlName, char *outputfile, 
                      char *proxyName, AHTHeaders *http_headers, void * context)
                     
{
  Document           newdoc;
  char              *tempfile;
  char              *documentname;
  char              *form_data;
  char              *initial_url, *ptr;
  int                method;
  Document           res = 0;
  Element            el;
  RELOAD_context    *ctx;
  int                visibility;
  ThotBool           stopped_flag = FALSE, keep;

  /* restore the context associated with the request */
  ctx = (RELOAD_context *) context;
  documentname = ctx->documentname;
  newdoc = ctx->newdoc;
  form_data = ctx->form_data;
  method = ctx->method;
  visibility = ctx->visibility;
  MapAreas[doc] = ctx->maparea;
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
  if (status == 0)
    {
      TtaSetCursorWatch (0, 0);

      /* a bit of acrobatics so that we can retain the initial_url
         without reallocating memory */
      initial_url = DocumentMeta[doc]->initial_url;
      DocumentMeta[doc]->initial_url = NULL;

      RemoveParsingErrors (newdoc);
      /* add the URI in the combobox string */
      if (method != CE_MAKEBOOK && method != CE_TEMPLATE && method != CE_ANNOT &&
          method != CE_LOG && method != CE_HELP &&
          DocumentTypes[newdoc] != docLibrary &&
          status == 0)
        {
          /* add the URI in the combobox string */
          keep = (method == CE_ABSOLUTE || method == CE_INIT);
          if (form_data && method == CE_FORM_GET)
            AddURLInCombobox (urlName, form_data, keep);
          else
            AddURLInCombobox (urlName, NULL, keep);
        }
      /* parse and display the document, res contains the new document
         identifier, as given by the thotlib */
      res = LoadDocument (newdoc, urlName, form_data, NULL, method,
                          tempfile, documentname, http_headers, FALSE,
                          &DontReplaceOldDoc, NULL);
      UpdateEditorMenus (doc);
      if (visibility == 4)
        {
          // restore the visibility
          TtaSetSensibility (doc, 1, visibility);
          TtaSetToggleItem (doc, 1, Views, TShowTargets, TRUE);
        }
      if (MapAreas[doc])
        // set Map areas visible
        TtaSetToggleItem (doc, 1, Views, TShowMapAreas, TRUE);

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
    {
      ResetStop (newdoc);
#ifdef _GL
      if (FilesLoading[newdoc] == 0 &&
          TtaGetViewFrame (newdoc, 1) != 0)
        /* all files included in this document have been loaded and the
           document is displayed. Animations can be played now */
        TtaPlay (newdoc, 1);
#endif /* _GL */
    }

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
#endif /* DAV */

  if (DocumentTypes[newdoc] == docBookmark && TtaIsDocumentModified (newdoc))
    DocStatusUpdate (newdoc, TRUE);
  else
    DocStatusUpdate (newdoc, FALSE);

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
  int                 method;
  RELOAD_context     *ctx;
  int                 toparse;
  int                 mode;
  int		       position;
  int		       distance;

  /* if it is a source document, reload its corresponding document */
  if (DocumentTypes[doc] == docSource)
    doc = GetDocFromSource (doc);
  if (DocumentURLs[doc] == NULL)
    /* the document has not been loaded yet */
    return;

  /* abort all current exchanges concerning this document */
  StopTransfer (doc, 1);
  if (!CanReplaceCurrentDocument (doc, view))
    /* abort the command */
    return;
  /* reload the document */
  pathname = (char *)TtaGetMemory (MAX_LENGTH);
  documentname = (char *)TtaGetMemory (MAX_LENGTH);

#ifdef TODO
  /* if the document is a template, restore the template script URL */
  if (DocumentMeta[doc] && DocumentMeta[doc]->method == CE_TEMPLATE)
    ReloadTemplateParams (&(DocumentURLs[doc]), &(DocumentMeta[doc]->method));
#endif //TODO
  
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
  ctx->visibility = TtaGetSensibility (doc, 1);
  ctx->maparea = MapAreas[doc];
  if (IsW3Path (pathname))
    {
      /* load the document from the Web */
      toparse = GetObjectWWW (doc, 0, pathname, form_data, tempfile, 
                              mode,
                              NULL, NULL, (void (*)(int, int, char*, char*, char*, const AHTHeaders*, void*)) Reload_callback, 
                              (void *) ctx, YES, NULL);
    }
  else if (TtaFileExist (pathname))
    Reload_callback (doc, 0, pathname, tempfile, NULL, NULL, (void *) ctx);

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
void ShowSource (Document doc, View view)
{
  CHARSET          charset;
  char            *localFile;
  char            *s;
  char  	    documentname[MAX_LENGTH];
  char  	    tempdir[MAX_LENGTH];
  Document         sourceDoc;
  NotifyElement    event;

  if (!DocumentURLs[doc])
    /* the document is not loaded yet */
    return;
  if (DocumentTypes[doc] != docHTML &&
      DocumentTypes[doc] != docSVG &&
      DocumentTypes[doc] != docXml &&
      DocumentTypes[doc] != docLibrary &&
      DocumentTypes[doc] != docMath)
    /* it's not an HTML or an XML document */
    return;
  if (!strcmp (DocumentURLs[doc], "empty"))
    return;
  if (DocumentSource[doc])
    /* the source code of this document is already shown */
    /* raise its window */
    TtaRaiseView (DocumentSource[doc], 1);
  else
    {
      /* save the current state of the document into the temporary file */
      localFile = GetLocalPath (doc, DocumentURLs[doc]);
      if (TtaIsDocumentModified (doc) || !TtaFileExist (localFile))
        {
          SetNamespacesAndDTD (doc);
          if (DocumentTypes[doc] == docLibrary ||
              DocumentTypes[doc] == docHTML)
            {
              if (TtaGetDocumentProfile (doc) == L_Xhtml11 || TtaGetDocumentProfile (doc) == L_Basic)
                TtaExportDocumentWithNewLineNumbers (doc, localFile,
                                                     "HTMLT11");
              else if (DocumentMeta[doc]->xmlformat)
                TtaExportDocumentWithNewLineNumbers (doc, localFile,
                                                     "HTMLTX");
              else
                TtaExportDocumentWithNewLineNumbers (doc, localFile,
                                                     "HTMLT");
            }
          else if (DocumentTypes[doc] == docSVG)
            TtaExportDocumentWithNewLineNumbers (doc, localFile,
                                                 "SVGT");
          else if (DocumentTypes[doc] == docMath)
            TtaExportDocumentWithNewLineNumbers (doc, localFile,
                                                 "MathMLT");
#ifdef XML_GENERIC
          else if (DocumentTypes[doc] == docXml)
            TtaExportDocumentWithNewLineNumbers (doc, localFile, NULL);
#endif /* XML_GENERIC */
        }

      TtaExtractName (localFile, tempdir, documentname);
      /* open a window for the source code */
#ifdef _WX
      sourceDoc = InitDocAndView (doc,
                                  FALSE /* replaceOldDoc */,
                                  FALSE /* inNewWindow */,
                                  documentname, (DocumentType)docSource, doc, FALSE,
                                  TtaGetDocumentProfile (doc),
                                  (int)CE_ABSOLUTE);   
#else /* _WX */
      sourceDoc = InitDocAndView (doc,
                                  FALSE /* replaceOldDoc */,
                                  TRUE /* inNewWindow */,
                                  documentname, (DocumentType)docSource, doc, FALSE,
                                  L_Other, (int)CE_ABSOLUTE);   
#endif /* _WX */

      if (sourceDoc > 0)
        {
          DocumentSource[doc] = sourceDoc;
          s = TtaStrdup (DocumentURLs[doc]);
          DocumentURLs[sourceDoc] = s;
          DocumentMeta[sourceDoc] = DocumentMetaDataAlloc ();
          DocumentMeta[sourceDoc]->form_data = NULL;
          DocumentMeta[sourceDoc]->initial_url = NULL;
          DocumentMeta[sourceDoc]->method = CE_ABSOLUTE;
          DocumentMeta[sourceDoc]->xmlformat = FALSE;
          DocumentMeta[sourceDoc]->compound = FALSE;
          /* copy the MIME type, charset, and content location */
          if (DocumentMeta[doc]->content_type)
            DocumentMeta[sourceDoc]->content_type = TtaStrdup (DocumentMeta[doc]->content_type);
          if (DocumentMeta[doc]->charset)
            DocumentMeta[sourceDoc]->charset = TtaStrdup (DocumentMeta[doc]->charset);
          if (DocumentMeta[doc]->content_location)
            DocumentMeta[sourceDoc]->content_location = TtaStrdup (DocumentMeta[doc]->content_location);
          if (DocumentMeta[doc]->full_content_location)
            DocumentMeta[sourceDoc]->full_content_location = TtaStrdup (DocumentMeta[doc]->full_content_location);
          DocumentTypes[sourceDoc] = docSource;
          charset = TtaGetDocumentCharset (doc);
          if (charset == UNDEFINED_CHARSET)
            {
              if (DocumentMeta[doc]->xmlformat)
                TtaSetDocumentCharset (SavingDocument, UTF_8, FALSE);
              else
                TtaSetDocumentCharset (SavingDocument, ISO_8859_1, FALSE);
            }
          else
            TtaSetDocumentCharset (sourceDoc, charset, FALSE);
          DocNetworkStatus[sourceDoc] = AMAYA_NET_INACTIVE;
          StartParser (sourceDoc, localFile, documentname, tempdir,
                       localFile, TRUE, FALSE);
          SetWindowTitle (doc, sourceDoc, 0);
          /* Switch the synchronization entry */
          if (TtaIsDocumentModified (doc))
            {
              // views are synchronized
              TtaSetDocumentModified (sourceDoc);
              TtaSetDocumentUnupdated (sourceDoc);
              TtaSetDocumentUnupdated (doc);
              //DocStatusUpdate (doc, TRUE);
            }
          /* Synchronize selections */
          event.document = doc;
          event.element = NULL;
          UpdateEditorMenus (sourceDoc);
          SetCharsetMenuOff (sourceDoc, 1);
          /* update back/forward buttons */
          if (HasPreviousDoc (doc))
            SetArrowButton (DocumentSource[doc], TRUE, TRUE);
          else
            SetArrowButton (DocumentSource[doc], TRUE, FALSE);
          if (HasNextDoc (doc))
            SetArrowButton (DocumentSource[doc], FALSE, TRUE);
          else
            SetArrowButton (DocumentSource[doc], FALSE, FALSE);

#ifdef _WX
          /* update toggle buttons */
	  if (HSplit[doc] == TRUE && VSplit[doc] == FALSE)
	    ShowHSplitToggle (sourceDoc, 1);
	  if (HSplit[doc] == FALSE && VSplit[doc] == TRUE)
	    ShowVSplitToggle (sourceDoc, 1);
#endif /* _WX */

          // check if a parsing error is detected
          sprintf (tempdir, "%s%c%d%cPARSING.ERR",
                   TempFileDirectory, DIR_SEP, doc, DIR_SEP);
          if (TtaFileExist (tempdir))
            TtaSetItemOn (sourceDoc, 1, File, BShowLogFile);
          SynchronizeSourceView (&event);
        }
      TtaFreeMemory (localFile);
    }

  /* the new opened view must get the focus */
  TtaRedirectFocus();
}

/*----------------------------------------------------------------------
  ShowFormatted
  Open the formatted view(s) of a document. (or give it focus)
  ----------------------------------------------------------------------*/
void ShowFormatted (Document doc, View view)
{
  View                structView;
  int                 x, y, w, h;
  char                structureName[30];

  if (DocumentTypes[doc] == docSource)
    /* work on the formatted document */
    doc = GetDocFromSource (doc);
  strcpy (structureName, "Formatted_view");  
  structView = TtaGetViewFromName (doc, structureName);
  if (structView != 0 && TtaIsViewOpen (doc, structView))
    TtaRaiseView (doc, structView);
  else
    {
      TtaGetViewGeometry (doc, structureName, &x, &y, &w, &h);
      structView = TtaOpenView (doc, structureName, x, y, w, h);
      if (structView != 0)
        {
#ifndef _WX
          TtcSwitchButtonBar (doc, structView); /* no button bar */
          TtcSwitchCommands (doc, structView); /* no command open */
#endif /* _WX */
          UpdateEditorMenus (doc);
        }
    }
  SetWindowTitle (doc, doc, 0);

  /* the new opened view must get the focus */
  TtaRedirectFocus();
}

/*----------------------------------------------------------------------
  ShowStructure
  Open the structure view(s) of a document.
  ----------------------------------------------------------------------*/
void ShowStructure (Document doc, View view)
{
  View                structView;
  int                 x, y, w, h;
  char                structureName[30];

  if (!strcmp (DocumentURLs[doc], "empty"))
    return;
  if (DocumentTypes[doc] == docSource)
    /* work on the formatted document */
    doc = GetDocFromSource (doc);
  strcpy (structureName, "Structure_view");  
  structView = TtaGetViewFromName (doc, structureName);
  if (structView != 0 && TtaIsViewOpen (doc, structView))
    TtaRaiseView (doc, structView);
  else
    {
      TtaGetViewGeometry (doc, structureName, &x, &y, &w, &h);
      structView = TtaOpenView (doc, structureName, x, y, w, h);
      if (structView != 0)
        {
#ifndef _WX
          TtcSwitchButtonBar (doc, structView); /* no button bar */
          TtcSwitchCommands (doc, structView); /* no command open */
#endif /* _WX */
          UpdateEditorMenus (doc);
        }
    }
  SetWindowTitle (doc, doc, 0);

  /* the new opened view must get the focus */
  TtaRedirectFocus();
}

/*----------------------------------------------------------------------
  ShowAlternate
  Open the Alternate view of a document.
  ----------------------------------------------------------------------*/
void ShowAlternate (Document doc, View view)
{
  View                altView;
  int                 x, y, w, h;

  if (!strcmp (DocumentURLs[doc], "empty"))
    return;
  if (DocumentTypes[doc] == docSource)
    /* work on the formatted document */
    doc = GetDocFromSource (doc);
  altView = TtaGetViewFromName (doc, "Alternate_view");
  if (view == altView)
    TtaRaiseView (doc, view);
  else if (altView != 0 && TtaIsViewOpen (doc, altView))
    TtaRaiseView (doc, altView);
  else
    {
      TtaGetViewGeometry (doc, "Alternate_view", &x, &y, &w, &h);
      altView = TtaOpenView (doc, "Alternate_view", x, y, w, h);
      if (altView != 0)
        {
          SetWindowTitle (doc, doc, altView);
#ifndef _WX
          TtcSwitchButtonBar (doc, altView); /* no button bar */
          TtcSwitchCommands (doc, altView); /* no command open */
#endif /* _WX */
          UpdateEditorMenus (doc);
        }
    }

  /* the new opened view must get the focus */
  TtaRedirectFocus();
}


/*----------------------------------------------------------------------
  ShowLinks
  Open the Links view of a document
  ----------------------------------------------------------------------*/
void ShowLinks (Document doc, View view)
{
  View                linksView;
  int                 x, y, w, h;

  if (!strcmp (DocumentURLs[doc], "empty"))
    return;
  if (DocumentTypes[doc] == docSource)
    /* work on the formatted document */
    doc = GetDocFromSource (doc);
  linksView = TtaGetViewFromName (doc, "Links_view");
  if (view == linksView)
    TtaRaiseView (doc, view);
  else if (linksView != 0 && TtaIsViewOpen (doc, linksView))
    TtaRaiseView (doc, linksView);
  else
    {
      TtaGetViewGeometry (doc, "Links_view", &x, &y, &w, &h);
      linksView = TtaOpenView (doc, "Links_view", x, y, w, h);
      if (linksView != 0)
        {
          SetWindowTitle (doc, doc, linksView);
#ifndef _WX
          TtcSwitchButtonBar (doc, linksView); /* no button bar */
          TtcSwitchCommands (doc, linksView); /* no command open */
#endif /* _WX */
          UpdateEditorMenus (doc);
        }
    }

  /* the new opened view must get the focus */
  TtaRedirectFocus();
}


/*----------------------------------------------------------------------
  ShowToC
  Open the Table of content view of a document
  ----------------------------------------------------------------------*/
void ShowToC (Document doc, View view)
{
  View                tocView;
  int                 x, y, w, h;

  if (!strcmp (DocumentURLs[doc], "empty"))
    return;
  if (DocumentTypes[doc] == docSource)
    /* work on the formatted document */
    doc = GetDocFromSource (doc);
  tocView = TtaGetViewFromName (doc, "Table_of_contents");
  if (view == tocView)
    TtaRaiseView (doc, view);
  else if (tocView != 0 && TtaIsViewOpen (doc, tocView))
    TtaRaiseView (doc, tocView);
  else
    {
      TtaGetViewGeometry (doc, "Table_of_contents", &x, &y, &w, &h);
      tocView = TtaOpenView (doc, "Table_of_contents", x, y, w, h);
      if (tocView != 0)
        {
          SetWindowTitle (doc, doc, tocView);
#ifndef _WX
          TtcSwitchButtonBar (doc, tocView); /* no button bar */
          TtcSwitchCommands (doc, tocView); /* no command open */
#endif /* _WX */
          UpdateEditorMenus (doc);
        }
    }

  /* the new opened view must get the focus */
  TtaRedirectFocus();
}


/*----------------------------------------------------------------------
  RequestView: Thot request to open a view to show the current selection
  ----------------------------------------------------------------------*/
ThotBool RequestView (NotifyDialog *event)
{
  ShowStructure (event->document, event->view);
  /* Inform Thot that the view is open by Amaya */
  return TRUE;
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
    {
      /* Save the current view geometry */
      if (view == structView)
        SaveGeometryOnExit (document, "Structure_view");
      if (view == altView)
        SaveGeometryOnExit (document, "Alternate_view");
      if (view == linksView)
        SaveGeometryOnExit (document, "Links_view");
      if (view == tocView)
        SaveGeometryOnExit (document, "Table_of_contents");
      /* let Thot perform normal operation */
      return FALSE;
    }
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
  else if (DocumentTypes[document] == docSource)
    TtaSetDocumentUnmodified (document);
  else if (!CanReplaceCurrentDocument (document, view))
    /* abort the command and don't let Thot perform normal operation */
    return TRUE;

  SaveGeometryOnExit (document, NULL); /* Save geometry of every view */
  if (structView!= 0 && TtaIsViewOpen (document, structView))
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
  The urlName is encoded with the default charset.
  ----------------------------------------------------------------------*/
void GetAmayaDoc_callback (int newdoc, int status, char *urlName, char *outputfile,
                           char *proxyName, AHTHeaders *http_headers, void * context)
{
  Element             elFound;
  Document            doc;
  Document            res;
  AmayaDoc_context   *ctx;
  TTcbf              *cbf;
  int                 method;
  void               *ctx_cbf;
  char                tempdocument[MAX_LENGTH];
  char                proxymsg[MAX_LENGTH];
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
  ctx = (AmayaDoc_context *) context;
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
  if (method != CE_MAKEBOOK && method != CE_TEMPLATE && method != CE_ANNOT &&
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
                              http_headers, ctx->history, &inNewWindow, NULL);
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
              if (method == CE_MAKEBOOK || method == CE_TEMPLATE)
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
          if (DocumentTypes[newdoc] == docLog ||
              method == CE_MAKEBOOK || method == CE_TEMPLATE)
            CleanUpParsingErrors ();
          else
            CheckParsingErrors (newdoc);
#ifdef TEMPLATES
          CheckTemplate (newdoc);
#endif /* TEMPLATES */
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
              DocumentMeta[newdoc]->compound = FALSE;
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
              if (proxyName != NULL)
		{
		  strcpy (proxymsg, "Used proxy: ");
		  strcat (proxymsg, proxyName);
		  InitConfirm3L (newdoc, 1, tempdocument, s, proxymsg, FALSE);
		}
	      else
		InitConfirm3L (newdoc, 1, tempdocument, s, NULL, FALSE);
            }
        }

      if (ok && !stopped_flag &&
          DocumentTypes[newdoc] != docLog &&
          method != CE_MAKEBOOK && method != CE_TEMPLATE)
        {
          ResetStop (newdoc);
#ifdef _GL
          if (FilesLoading[newdoc] == 0 &&
              TtaGetViewFrame (newdoc, 1) != 0)
            /* all files included in this document have been loaded and the
               document is displayed. Animations can be played now */
            TtaPlay (newdoc, 1);
#endif /* _GL */
        }
    }

  /* select the target if present */
  if (ok && !stopped_flag && target != NULL && target[0] != EOS &&
      newdoc != 0 && method != CE_MAKEBOOK && method != CE_TEMPLATE)
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
    (*cbf) (newdoc, status, pathname, tempfile, NULL, NULL, ctx_cbf);


#ifdef DAV
  /* MKP: if document has been loaded,  we are     * 
   * able to discovery if the document is locked.  *
   * do a lock discovery, set LockIndicator button */
  if (W3Loading == 0) 
    {
      DAVLockDiscovery (newdoc);
      DAVSetLockIndicator(newdoc);
    }
#endif /* DAV */  
  TtaFreeMemory (target);
  TtaFreeMemory (documentname);
  TtaFreeMemory (initial_url);
  TtaFreeMemory (pathname);
  TtaFreeMemory (tempfile);
  if (form_data)
    TtaFreeMemory (form_data);
  TtaFreeMemory (ctx);
  /* check if a refresh is requested */
  CheckRefresh (newdoc);
}


/*----------------------------------------------------------------------
  GetAmayaDoc loads the document if it is not loaded yet and calls the
  parser if the document can be parsed.
  - urlname: can be relative or absolute address.
  It's encoded with the default charset.
  - form_data: the text to be posted.
  - doc: the document which can be removed if not updated.
  - baseDoc: the document which urlname is relative to.
  - method: CE_FORM_POST for a post request, CE_RELATIVE for a double 
  click.
  - history: record the URL in the browsing history
  ----------------------------------------------------------------------*/
Document GetAmayaDoc (char *urlname, char *form_data,
                      Document doc, Document baseDoc, int method,
                      ThotBool history, TTcbf *cbf, void *ctx_cbf)
{
  Document            newdoc = 0, refdoc;
  CSSInfoPtr          css;
  PInfoPtr            pInfo;
  AmayaDoc_context   *ctx = NULL;
  CHARSET             doc_charset;
  char               *tempfile;
  char               *parameters;
  char               *target;
  char               *initial_url;
  char               *documentname;
  char               *content_type = NULL;
  char                charsetname[MAX_LENGTH];
  int                 parsingLevel;
  int                 toparse;
  int                 mode;
  int                 docType;
  ThotBool            xmlDec, withDoctype, isXML, useMath, isKnown;
  ThotBool            ok;
  char                lg_uri[10];

  /* Extract parameters if necessary */
  if (strlen (urlname) > MAX_LENGTH - 1) 
    {
      sprintf (lg_uri, "%d", MAX_LENGTH);
      TtaSetStatus (baseDoc, 1, TtaGetMessage (AMAYA, AM_TOO_LONG_URL), lg_uri);
      return (0);
    }
#ifndef _WX
  else
    /* clean up the status line */
    TtaSetStatus (baseDoc, 1, " ", NULL);
#endif /* _WX */

  /* check if the request comes from the source document */
  if (doc && DocumentTypes[doc] == docSource)
    doc = GetDocFromSource (doc);
  ok = TRUE;
  target       = (char *)TtaGetMemory (MAX_LENGTH);
  documentname = (char *)TtaGetMemory (MAX_LENGTH);
  parameters   = (char *)TtaGetMemory (MAX_LENGTH);
  tempfile     = (char *)TtaGetMemory (MAX_LENGTH);
  tempfile[0]  = EOS;
  initial_url     = (char *)TtaGetMemory (MAX_LENGTH);
  ExtractParameters (urlname, parameters);
  /* Extract the target if necessary */
  ExtractTarget (urlname, target);
  /* Add the  base content if necessary */
  if (method == CE_RELATIVE || method == CE_FORM_GET ||
      method == CE_FORM_POST || method == CE_MAKEBOOK || method == CE_TEMPLATE)
    NormalizeURL (urlname, baseDoc, initial_url, documentname, NULL);
  else
    NormalizeURL (urlname, 0, initial_url, documentname, NULL);
  /* check the document suffix */
  if (IsMathMLName (documentname))
    docType = docMath;
  else if (IsSVGName (documentname))
    docType = docSVG;
  else if (IsCSSName (documentname))
    docType = docCSS;
#ifdef TEMPLATES
  else if (IsXTiger (documentname))
    // @@@@ by default it's a HTML document
    docType = docHTML;
#endif /* TEMPLATES */
  else if (IsXMLName (documentname))
    {
      docType = docXml;
      //TODO Check that urlname is a valid URL
      if (!IsW3Path (urlname))
        CheckDocHeader(urlname, &xmlDec, &withDoctype, &isXML, &useMath, &isKnown,
                       &parsingLevel, &doc_charset, charsetname, (DocumentType*)&docType);
    }
#ifdef _SVG
  else if (IsLibraryName (documentname))
    docType = docLibrary;
#endif /* _SVG */
  else if (method == CE_CSS)
    docType = docCSS;
  else if (method == CE_LOG)
    docType = docLog;
  else if (IsTextName (documentname))
    docType = docText;
  else
    docType = docHTML;
  
  /* we skip the file: prefix if it is present and do other local
     file urls conversions */
  if (!IsW3Path (initial_url))
    {
      /* we take the long way around to get the result
         of normalizeFile, as the function doesn't allocate
         memory dynamically (note: this can generate some MAX_LENGTH
         problems) */
      if (method == CE_RELATIVE || method == CE_FORM_GET ||
          method == CE_ANNOT || method == CE_FORM_POST ||
          method == CE_MAKEBOOK || method == CE_TEMPLATE)
        /* we're following a link, so do all the convertions on
           the URL */
        NormalizeFile (initial_url, tempfile, AM_CONV_ALL);
      else
        NormalizeFile (initial_url, tempfile, AM_CONV_NONE);
      strcpy (initial_url, tempfile);
      tempfile[0] = EOS;
    }

  /* check if the user is already browsing the document in another window */
  if (method == CE_FORM_GET)
    {
      newdoc = IsDocumentLoaded (initial_url, form_data);
      /* we don't concatenate the new parameters as we give preference
         to the form data */
    }
  else
    {
      /* concatenate the parameters before making the test */
      if (parameters[0] != EOS)
        {
          strcat (initial_url, "?");
          strcat (initial_url, parameters);
        }
      /* if it's a POST form, we search the document using the
         form_data */
      if (method == CE_FORM_POST)
        newdoc = IsDocumentLoaded (initial_url, form_data);
      else if (method != CE_TEMPLATE)
        newdoc = IsDocumentLoaded (initial_url, NULL);
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
  ctx = (AmayaDoc_context*)TtaGetMemory (sizeof (AmayaDoc_context));
  ctx->doc = doc;
  ctx->baseDoc = baseDoc;
  ctx->history = history;
  ctx->target = target;
  ctx->documentname = documentname;
  ctx->initial_url = TtaStrdup (initial_url);
  if (form_data)
    ctx->form_data = TtaStrdup (form_data);
  else
    ctx->form_data = NULL;
  ctx->method = method;
  ctx->cbf = cbf;
  ctx->ctx_cbf = ctx_cbf;
  ctx->local_link = 0;
  ctx->inNewWindow = DontReplaceOldDoc;

  toparse = 0;
  if (newdoc == 0)
    {
      /*
        DocumentURLs and DocHistory are coded in the default charset */
      /* document not loaded yet */
      if ((method == CE_RELATIVE || method == CE_FORM_GET ||
           method == CE_FORM_POST || method == CE_MAKEBOOK || method == CE_TEMPLATE ||
           method == CE_ANNOT) &&
          !IsW3Path (initial_url) && !TtaFileExist (initial_url))
        {
          /* the target document doesn't exist */
          TtaSetStatus (baseDoc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), initial_url);
          ok = FALSE; /* do not continue */
        }
      else if (method == CE_LOG)
        /* need to create a new window for the document */
        newdoc = InitDocAndView ( baseDoc, /* SG: 'doc' is always NULL, maybe it should be baseDoc */
                                  FALSE /* replaceOldDoc */,
                                  TRUE /* inNewWindow */,
                                  documentname, (DocumentType)docLog, 0, FALSE,
                                  L_Other, method);
      else if (method == CE_HELP)
        {
          /* add the URI in the combobox string */
          AddURLInCombobox (initial_url, NULL, FALSE);
#ifdef _WX
          /* need to create a new window for the document */
          newdoc = InitDocAndView (baseDoc,
                                   !DontReplaceOldDoc /* replaceOldDoc */,
                                   InNewWindow /* inNewWindow */,
                                   documentname, (DocumentType)docType, 0, TRUE,
                                   L_Other, method);
#else /* _WX */
          /* need to create a new window for the document */
          newdoc = InitDocAndView (doc,
                                   FALSE /* replaceOldDoc */,
                                   TRUE /* inNewWindow */,
                                   documentname, (DocumentType)docType, 0, TRUE,
                                   L_Other, method);
#endif /* _WX */
          if (newdoc)
            {
              /* help document has to be in read-only mode */
#ifndef _WX
              TtcSwitchCommands (newdoc, 1); /* no command open */
#endif /* _WX */
              TtaSetToggleItem (newdoc, 1, Views, TShowTextZone, FALSE);
              TtaSetMenuOff (newdoc, 1, Help_);
            }
        }
#ifdef ANNOTATIONS
      else if (method == CE_ANNOT)
        {
          /* need to create a new window for the document */
          newdoc = InitDocAndView (doc,
                                   FALSE /* replaceOldDoc */,
                                   TRUE /* inNewWindow */,
                                   documentname, (DocumentType)docAnnot, 0, FALSE,
                                   L_Annot, method);
          /* we're downloading an annotation, fix the accept_header
             (thru the content_type variable) to application/rdf */
          content_type = "application/rdf";
        }
#endif /* ANNOTATIONS */
      else if (method != CE_MAKEBOOK && (doc == 0 || DontReplaceOldDoc))
        {
          /* In case of initial document, open the view before loading */
          /* add the URI in the combobox string */
          if (method != CE_MAKEBOOK && method != CE_TEMPLATE)
              AddURLInCombobox (initial_url, NULL, FALSE);
          newdoc = InitDocAndView (doc,
                                   FALSE /* replaceOldDoc */,
#ifndef _WX
                                   TRUE /* inNewWindow */,
#else /* _WX */
                                   InNewWindow /* inNewWindow */,
#endif /* _WX */
                                   documentname, (DocumentType)docType, 0, FALSE,
                                   L_Other, method);
        }
      else
        {
          newdoc = doc;
          /* stop current transfer for previous document */
          if (method != CE_MAKEBOOK && method != CE_TEMPLATE)
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
          else if (method == CE_MAKEBOOK || method == CE_TEMPLATE)
            mode = AMAYA_ASYNC;

          if (IsW3Path (initial_url))
            {
              css = SearchCSS (0, initial_url, NULL, &pInfo);
              if (method == CE_MAKEBOOK || method == CE_RELATIVE  || method == CE_TEMPLATE)
                /* add the referer field in the GET */
                refdoc = doc;
              else
                refdoc = 0;
              if ((css == NULL) || (css != NULL && newdoc == doc))
                toparse =  GetObjectWWW (newdoc, refdoc, initial_url, form_data,
                                         tempfile, mode, NULL, NULL,
                                         (void (*)(int, int, char*, char*, char*, const AHTHeaders*, void*)) GetAmayaDoc_callback,
                                         (void *) ctx, YES, content_type);
              else
                {
                  /* it was already loaded, we need to open it */
                  TtaSetStatus (newdoc, 1,
                                TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED), NULL);
                  /* just take a copy of the local temporary file */
                  strcpy (tempfile, css->localName);
                  GetAmayaDoc_callback (newdoc, 0, initial_url, tempfile,
                                        NULL, NULL, (void *) ctx);
                  TtaHandlePendingEvents ();
                }
            }
          else
            {
              /* wasn't a document off the web, we need to open it */
              TtaSetStatus (newdoc, 1,
                            TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED),
                            NULL);
              GetAmayaDoc_callback (newdoc, 0, initial_url, tempfile,
                                    NULL, NULL, (void *) ctx);
              TtaHandlePendingEvents ();
            }
        }
    }
  else if (ok && newdoc != 0)
    {
      /* following a local link */
      TtaSetStatus (newdoc, 1, TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED), NULL);
      ctx->local_link = 1;
      GetAmayaDoc_callback (newdoc, 0, initial_url, tempfile, NULL, NULL, (void *) ctx);
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
        (*cbf) (newdoc, -1, initial_url, tempfile, NULL, NULL, ctx_cbf);
      /* Free the memory associated with the context */
      TtaFreeMemory (target);
      TtaFreeMemory (documentname);
    }

  TtaFreeMemory (parameters);
  TtaFreeMemory (tempfile);
  TtaFreeMemory (initial_url);
  DontReplaceOldDoc = FALSE;
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
  TtaSetStructureChecking (FALSE, doc);
  
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
  TtaSetStructureChecking (TRUE, doc);  
}

/*----------------------------------------------------------------------
  UpdateSaveAsButtons
  Maintain consistency between buttons in the Save As dialog box
  ----------------------------------------------------------------------*/
static void UpdateSaveAsButtons ()
{
#if defined(_GTK)
  int	active;

  if (SaveAsHTML || SaveAsXML)
    active = 1;
  else
    active = 0;
  TtaRedrawMenuEntry (BaseDialog + ToggleSave, 0, NULL, (ThotColor)-1, active);
  TtaRedrawMenuEntry (BaseDialog + ToggleSave, 1, NULL, (ThotColor)-1, active);
#endif /* defined(_GTK) */
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
#ifdef _WINGUI
          strcpy (DocToOpen, SavePath);
          if (strchr (SavePath, '/'))
            strcat (DocToOpen, URL_STR);
          else
            strcat (DocToOpen, DIR_STR);
          strcat (DocToOpen, SaveName);
#endif /* _WINGUI */
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

#ifdef _WINGUI
          sprintf (DocToOpen, filename);
#endif /* _WINGUI */

#ifdef _GTK
          TtaSetTextForm (BaseDialog + NameSave, filename);
#endif /* defined(_GTK) */

#ifdef _WX
          strcpy(SaveFileName, filename);
#endif /* _WX */

          TtaFreeMemory (filename);
        }
      else
        {
#ifdef _WX
          SaveFileName[0] = EOS;
#endif /* _WX */
        }
    }
}

/*----------------------------------------------------------------------
  Callback procedure for dialogue events.                            
  ----------------------------------------------------------------------*/
void CallbackDialogue (int ref, int typedata, char *data)
{
  Document          doc = 0;
  char              tempfile[MAX_LENGTH];
  char              tempname[MAX_LENGTH];
  char              sep, *ptr;
  long int          val;
#ifdef _WX
  int               i;
#endif /* _WX */
  ThotBool          change, updated;

  if (typedata == STRING_DATA && data && strchr (data, '/'))
    sep = URL_SEP;
  else
    sep = DIR_SEP;
  val = (long int) data;
  if (ref - BaseDialog == OptionMenu)
    /* a popup menu corresponding to a SELECT element */
    ReturnOption = val;
  else switch (ref - BaseDialog)
    {
    case OpenForm:
      /* *********Load URL or local document********* */
      if (val == 1)
        /* Confirm */
        {
#ifdef _WX
          /* get the current document (CurrentDocument not valid) */
          TtaGiveActiveView (&doc, &i);
          if (DocumentSource[doc] && DocumentTypes[doc] != docSource)
            /* it could be a log or show document */
            doc = DocumentSource[doc];
          CurrentDocument = doc;
          if ((CurrentDocument == 0) || /* this is the first loaded doc */
              (!InNewWindow && DontReplaceOldDoc) || /* in a new tab */
              (InNewWindow) || /* in a new window */
              (!DontReplaceOldDoc && CanReplaceCurrentDocument(CurrentDocument, 1)) /* wants to replace the current document */
               )
            {
#else /* _WX */
              /* Do not destroy this dialog on WX to prevent a crash on mac version */
              TtaDestroyDialogue (BaseDialog + OpenForm);
              TtaDestroyDialogue (BaseDialog + FileBrowserForm);
#endif /* _WX */
              if (LastURLName[0] != EOS)
                {
                  TtaSetStatus (CurrentDocument, 1,
                                TtaGetMessage (AMAYA, AM_CANNOT_LOAD),
                                DocumentName);
                  /* update the list of URLs */
                  if (NewFile)
                    InitializeNewDoc (LastURLName, NewDocType,
                                      doc, NewDocProfile, NewXML);
                  /* load an URL */ 
                  else if (DontReplaceOldDoc)
                    GetAmayaDoc (LastURLName, NULL, doc, doc,
                                 Loading_method,
                                 FALSE, NULL, NULL);
                  else
                    GetAmayaDoc (LastURLName, NULL, CurrentDocument,
                                 CurrentDocument, Loading_method,
                                 TRUE, NULL, NULL);
                }
              else if (DirectoryName[0] != EOS && DocumentName[0] != EOS)
                {
                  /* load a local file */
                  strcpy (tempfile, DirectoryName);
                  strcat (tempfile, DIR_STR);
                  strcat (tempfile, DocumentName);
                  /* update the list of URLs */
                  NormalizeFile (tempfile, tempname, AM_CONV_ALL);
                  if (NewFile)
                    InitializeNewDoc (tempfile, NewDocType,
                                      doc, NewDocProfile, NewXML);
                  else if (FileExistTarget (tempname))
                    {
                      if (DontReplaceOldDoc)
                        GetAmayaDoc (tempfile, NULL,
                                     doc, doc,
                                     Loading_method,
                                     FALSE, NULL, NULL);
                      else
                        GetAmayaDoc (tempfile, NULL, CurrentDocument,
                                     CurrentDocument, Loading_method,
                                     TRUE, NULL, NULL);
                    }
                  else
                    {
#ifdef IV
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
                      InitializeNewDoc (tempfile, NewDocType, CurrentDocument,
                                        NewDocProfile, NewXML);
#endif /* IV */
                      NotFoundDoc (tempfile, CurrentDocument);
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
                  if (DontReplaceOldDoc)
                    GetAmayaDoc (DocumentName, NULL, doc, doc,
                                 Loading_method,
                                 FALSE, NULL, NULL);
                  else
                    GetAmayaDoc (DocumentName, NULL, CurrentDocument,
                                 CurrentDocument, Loading_method,
                                 TRUE, NULL, NULL);
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
#ifdef _WX
            }
#endif /* _WX */
        }
      else if (val == 2)
        {
          /* Browse button */
#if defined(_GTK) || defined(_WX) 
          WidgetParent = OpenDocBrowser;
          BrowserForm (CurrentDocument, 1, &LastURLName[0]);
#endif /* defined(_GTK) || defined(_WX) */
        }
      else if (val == 3)
        {
          /* Clear button */
          LastURLName[0] = EOS;
          DirectoryName[0] = EOS;
          DocumentName[0] = EOS;
#if defined(_GTK) || defined(_WX) 
          TtaSetTextForm (BaseDialog + URLName, LastURLName);
#endif /* defined(_GTK) || defined(_WX) */
        }
#ifdef _WX
      else if (val == 0)
        {
          /* if the user has clicked on Cancel button, just destroy the dialog */
          TtaDestroyDialogue ( ref );
        }
#endif /* _WX */
      else if (NewFile)
        {
          /* the command is aborted */
          CheckAmayaClosed ();
          NewFile = FALSE;
        }
      break;

      /* this callback is used to select a destination for the new open document :
       * 0 = replace current one
       * 1 = in new tab
       * 2 = in new window */
    case OpenLocation:
      {
        if (val == 0) /* replace old doc */
          {
            DontReplaceOldDoc = FALSE;
            InNewWindow       = FALSE;
          }
        else if (val == 1) /* in new tab */
          {
            DontReplaceOldDoc = TRUE;
            InNewWindow       = FALSE;
          }
        else if (val == 2) /* in new window */
          {
            DontReplaceOldDoc = TRUE;
            InNewWindow       = TRUE;
          }
      }
      break;
    case DocInfoDocType:
      /* only used by WX version to get back the new profile */
      if (NewDocType == docHTML)
        {
          char *compound = TtaGetMessage (AMAYA, AM_COMPOUND_DOCUMENT);
          if (data && compound && !strcmp (data, compound))
            NewDocProfile = L_Other;
          else
            {
              if (!strstr (data, "XHTML"))
                NewXML = FALSE;
              if (strstr (data, "Strict"))
                NewDocProfile = L_Strict;
              else if (strstr (data, "Basic"))
                NewDocProfile = L_Basic;
              else if (strstr (data, "Transitional"))
                NewDocProfile = L_Transitional;
              else if (NewXML)
                NewDocProfile = L_Xhtml11;
              else
                NewDocProfile = L_Transitional;
            }
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
#ifdef _WINGUI
      sprintf (DirectoryName, "%s", data);
#endif /* _WINGUI */      
#if defined(_GTK) || defined(_WX)  
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
#endif /* defined(_GTK) || defined(_WX) */
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
#if defined(_GTK) || defined(_WX) 
      TtaSetTextForm (BaseDialog + URLName, tempfile);
#endif /* defined(_GTK) || defined(_WX) */
      break;

    case ConfirmForm:
      /* *********Confirm********* */
      UserAnswer = (val == 1);
      ExtraChoice = (val == 2);
      TtaDestroyDialogue (BaseDialog + ConfirmForm);
      break;

    case FilterText:
      /* Filter value */
      if (strlen (data) <= NAME_LENGTH)
        strcpy (ScanFilter, data);
#if defined(_GTK) || defined(_WX) 
      else
        TtaSetTextForm (BaseDialog + FilterText, ScanFilter);
#endif /* defined(_GTK) || defined(_WX) */
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
#ifdef _WX
      i = strlen (data);
      if (i < NAME_LENGTH - 1)
        {
          Lg_password = 0;
          while (Lg_password < i)
            {
              /* a new char */
              Answer_password[Lg_password] = data[Lg_password];
              Display_password[Lg_password] = '*';
              Answer_password[++Lg_password] = EOS;
              Display_password[Lg_password] = EOS;
            }
        }
      else
        Answer_password[NAME_LENGTH - 1] = EOS;
#endif /* _WX */
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
#if defined(_GTK)
          TtaSetToggleMenu (BaseDialog + ToggleSave, 1, SaveAsXML);
          TtaSetToggleMenu (BaseDialog + ToggleSave, 0, SaveAsHTML);
          UpdateSaveAsButtons ();
#endif /* defined(_GTK) */
    
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

#if defined(_GTK) || defined(_WX) 
                  TtaNewLabel (BaseDialog + SaveFormStatus,
                               BaseDialog + SaveForm,
                               TtaGetMessage (AMAYA, AM_INVALID_MIMETYPE));
#endif /* defined(_GTK) || defined(_WX) */
#ifdef _WINGUI      
                  SaveAsDlgStatus (TtaGetMessage (AMAYA, AM_INVALID_MIMETYPE));
#endif /* _WINGUI */
                  break;
                }
            }
          TtaDestroyDialogue (BaseDialog + SaveForm);
          if (SavingDocument)
            DoSaveAs (UserCharset, UserMimeType);
          else if (SavingObject)
            {
              DoSaveObjectAs ();
              // make sure the saving is closed
              SavingObject = 0;
            }
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
#if defined(_GTK) || defined(_WX) 
          WidgetParent = DocSaveBrowser;
          strcpy (LastURLName, SavePath);
          strcat (LastURLName, DIR_STR);
          strcat (LastURLName, SaveName);
          BrowserForm (SavingDocument, 1, LastURLName);
#endif /* defined(_GTK) || defined(_WX) */
        }
      else if (val == 3)
        /* "Clear" button */
        {
          if (SavingDocument != 0)
            {
              SavePath[0] = EOS;
              SaveImgsURL[0] = EOS;
              SaveName[0] = EOS;
#if defined(_GTK)
              TtaSetTextForm (BaseDialog + NameSave, SaveImgsURL);
              TtaSetTextForm (BaseDialog + ImgDirSave, SaveImgsURL);
#endif /* defined(_GTK) */
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
#if defined(_GTK)
                  TtaNewLabel (BaseDialog + SaveFormStatus,
                               BaseDialog + SaveForm, " ");
#endif /* defined(_GTK) */
#ifdef _WINGUI      
                  SaveAsDlgStatus ("");
#endif /* _WINGUI */
                  InitCharset (SavingDocument, 1, SavePath);
                  if (SaveFormTmp[0] != EOS)
                    {
                      strcpy (UserCharset, SaveFormTmp);
#if defined(_GTK)
                      TtaNewLabel (BaseDialog + CharsetSave,  
                                   BaseDialog + SaveForm, UserCharset);
#endif /* #if defined(_GTK) */
                    }
                }
              else
                {
#if defined(_GTK) || defined(_WX) 
                  TtaNewLabel (BaseDialog + SaveFormStatus,
                               BaseDialog + SaveForm,
                               TtaGetMessage (AMAYA, AM_NOCHARSET_SUPPORT));
#endif /* #if defined(_GTK) || defined(_WX) */
#ifdef _WINGUI
                  SaveAsDlgStatus (TtaGetMessage (AMAYA, AM_NOCHARSET_SUPPORT));
#endif /* _WINGUI */
                }
            }
        }
      else if (val == 5)
        /* "MIME type" button */
        {
          if (SavingDocument != 0)
            {
              if (SavePath[0])
                {
                  /* clear the status message */

#if defined(_GTK) || defined(_WX) 
                  TtaNewLabel (BaseDialog + SaveFormStatus,
                               BaseDialog + SaveForm,
                               " ");
#endif /* #if defined(_GTK) || defined(_WX) */
#ifdef _WINGUI      
                  SaveAsDlgStatus ("");
#endif /* _WINGUI */
                  InitMimeType (SavingDocument, 1, SavePath, NULL);
                  if (SaveFormTmp[0] != EOS)
                    {
                      strcpy (UserMimeType, SaveFormTmp);   
#if defined(_GTK) || defined(_WX) 
                      TtaNewLabel (BaseDialog + MimeTypeSave,  
                                   BaseDialog + SaveForm, UserMimeType);
#endif /* #if defined(_GTK) || defined(_WX) */
                    }
                }
              else
                {
#if defined(_GTK) || defined(_WX) 
                  TtaNewLabel (BaseDialog + SaveFormStatus,
                               BaseDialog + SaveForm,
                               TtaGetMessage (AMAYA, AM_NOMIMETYPE_SUPPORT));
#endif /* #if defined(_GTK) || defined(_WX) */
#ifdef _WINGUI      
                  SaveAsDlgStatus (TtaGetMessage (AMAYA, AM_NOMIMETYPE_SUPPORT));
#endif /* _WINGUI */
                }
            }
        }
      else
        /* "Cancel" button */
        {
          TtaDestroyDialogue (BaseDialog + SaveForm);
          if (SavingObject)
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

    case CssDirSave:
      /* Image directory */
      if (!IsW3Path (data))
        change = NormalizeFile (data, SaveCssURL, AM_CONV_NONE);
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
            /* create an attribute HREF for the Link_Anchor */
            SetREFattribute (AttrHREFelement, AttrHREFdocument,
                             AttrHREFvalue, NULL);
          TtaDestroyDialogue (BaseDialog + AttrHREFForm);
          TtaDestroyDialogue (BaseDialog + FileBrowserForm);
        }
      else if (val == 2)
        /* Browse button */
        {
#if defined(_GTK) || defined(_WX) 
          if (LinkAsXmlCSS || LinkAsCSS)
            strcpy (ScanFilter, "*.css");
          else if (!strcmp (ScanFilter, "*.css"))
            strcpy (ScanFilter, "*");
          WidgetParent = HrefAttrBrowser;
          BrowserForm (AttrHREFdocument, 1, &AttrHREFvalue[0]);
#endif /* #if defined(_GTK) || defined(_WX) */
        }
      else if (val == 3)
        /* allow one to click the target */
        SelectDestination (AttrHREFdocument, AttrHREFelement, AttrHREFundoable, TRUE);
      else if (val == 4)
        {
          /* Clear button */
          AttrHREFvalue[0] = EOS;
#if defined(_GTK) || defined(_WX) 
          TtaSetTextForm (BaseDialog + AttrHREFText, AttrHREFvalue);
#endif /* #if defined(_GTK) || defined(_WX) */
        }
      else 
        /* Cancel button */
        if (IsNewAnchor)
          {
            LinkAsCSS = FALSE;
            LinkAsXmlCSS = FALSE;
            /* remove the link if it was just created */
            //TtaCancelLastRegisteredSequence (AttrHREFdocument);
            DoDeleteAnchor (AttrHREFdocument, 1, FALSE);
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
#if defined(_GTK) || defined(_WX) 
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
#endif /* defined(_GTK) || defined(_WX) */
        }
      else if (val == 2)
        /* Clear button */
        {
          if (WidgetParent == OpenDocBrowser)
            {
              LastURLName[0] = EOS;
#if defined(_GTK) || defined(_WX) 
              TtaSetTextForm (BaseDialog + FileBrowserText, LastURLName);
#endif /* #if defined(_GTK) || defined(_WX) */
        
            }
          else if (WidgetParent == HrefAttrBrowser)
            {
              tempname[0] = EOS; 	       
#if defined(_GTK) || defined(_WX) 
              TtaSetTextForm (BaseDialog + FileBrowserText, tempname);
#endif /* #if defined(_GTK) || defined(_WX) */
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
#ifdef _WINGUI
      sprintf (DirectoryName, "%s", data);
#endif /* _WINGUI */
#if defined(_GTK) || defined(_WX)  
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
#endif /* #if defined(_GTK) || defined(_WX) */
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
#if defined(_GTK) || defined(_WX) 
      TtaSetTextForm (BaseDialog + FileBrowserText, tempfile);
#endif /* #if defined(_GTK) || defined(_WX) */
      break;
      
      /* *********Browser Filter*********** */
    case FileBrowserFilter:
      /* Filter value */
      if (strlen(data) <= NAME_LENGTH)
        strcpy (ScanFilter, data);
#if defined(_GTK) || defined(_WX) 
      else
        TtaSetTextForm (BaseDialog + BrowserFilterText, ScanFilter);
#endif /* #if defined(_GTK) || defined(_WX) */
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
      TtaDestroyDialogue (ref);
      break;
    case MathEntityText:
      strncpy (MathMLEntityName, data, MAX_LENGTH);
      MathMLEntityName[MAX_LENGTH - 1] = EOS;
      break;
    case MakeIdMenu:
      switch (val)
        {
        case 1:
          CreateRemoveIDAttribute (IdElemName, IdDoc, TRUE, IdApplyToSelection);
#if defined(_GTK) || defined(_WX) 
          /* and show the status */
          TtaNewLabel (BaseDialog + mIdStatus,
                       BaseDialog + MakeIdMenu,
                       IdStatus);
#endif /* #if defined(_GTK) || defined(_WX) */
          break;
        case 2:
          CreateRemoveIDAttribute (IdElemName, IdDoc, FALSE, IdApplyToSelection);
#if defined(_GTK) || defined(_WX) 
          /* and show the status */
          TtaNewLabel (BaseDialog + mIdStatus,
                       BaseDialog + MakeIdMenu,
                       IdStatus);
#endif /* #if defined(_GTK) || defined(_WX) */
          break;
        }
      break;
    case mElemName:
      strncpy (IdElemName, data, MAX_LENGTH);
      IdElemName[MAX_LENGTH - 1] = EOS;
      break;
    case mIdUseSelection:
      IdApplyToSelection = (val != 0);
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
#if defined(_GTK)
                  InitMimeType (SavingDocument, 1, SavePath,
                                TtaGetMessage (AMAYA, AM_INVALID_MIMETYPE));
#endif /* _GTK */
#ifdef _WINGUI
                  /* the Window dialog won't be closed */
                  MimeTypeDlgStatus (TtaGetMessage (AMAYA, AM_INVALID_MIMETYPE));
#endif /* _WINGUI */
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
  RestoreOneAmayaDoc restores a saved file doc is the suggested doc to
  be loaded or 0.
  docname is the original name of the document.
  tempdoc is the name of the saved file.
  Return the new recovered document
  URIs are encoded with the default charset.
  ----------------------------------------------------------------------*/
static int RestoreOneAmayaDoc (Document doc, char *tempdoc, char *docname,
                               DocumentType docType, ThotBool iscrash)
{
  AHTHeaders    http_headers;
  char          content_type[MAX_LENGTH];
  char           tempfile[MAX_LENGTH];
  int           newdoc;
  ThotBool      stopped_flag;

  W3Loading = doc;
  BackupDocument = doc;
  TtaExtractName (tempdoc, DirectoryName, DocumentName);
  AddURLInCombobox (docname, NULL, TRUE);
  newdoc = InitDocAndView (doc,
                           FALSE /* replaceOldDoc */,
                           TRUE /* inNewWindow */,
                           DocumentName, (DocumentType)docType, 0, FALSE,
                           L_Other, CE_ABSOLUTE);
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
                        tempdoc, DocumentName, &http_headers, FALSE,
                        &DontReplaceOldDoc, docname);
        }
      else
        {
          /* it's a local file */
          tempfile[0] = EOS;
          /* load the temporary file */
          LoadDocument (newdoc, tempdoc, NULL, NULL, CE_ABSOLUTE,
                        tempfile, DocumentName, NULL, FALSE,
                        &DontReplaceOldDoc, docname);
          /* change its URL */
          TtaFreeMemory (DocumentURLs[newdoc]);
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
          ResetStop (newdoc);
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
      f = TtaReadOpen (tempname);
      if (f != NULL)
        {
          DontReplaceOldDoc = TRUE;
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
                      if (RestoreOneAmayaDoc (0, tempdoc, docname,
                                              (DocumentType) docType, iscrash))
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
          DontReplaceOldDoc = FALSE;	  
          TtaReadClose (f);
        }

      if (iscrash)
        {
          TtaFileUnlink (tempname);
          sprintf (tempname, "%s%cAutoSave.dat", TempFileDirectory, DIR_SEP);
          if (TtaFileExist (tempname))
            {
              f = TtaReadOpen (tempname);
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
                  TtaReadClose (f);
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
  FreeAmayaIcons cleans up icons objects.
  ----------------------------------------------------------------------*/
void FreeAmayaIcons ()
{
  /* free allocated icons */
#if defined(_WX)
  if (stopR)
    delete stopR;
  if (stopN) 	
    delete stopN;
  if (iconSave) 	
    delete iconSave;
  if (iconSaveNo) 	
    delete iconSaveNo;
  if (iconFind) 	
    delete iconFind;
  if (iconReload) 	
    delete iconReload;
  if (iconHome) 	
    delete iconHome;
  if (iconI) 	
    delete iconI;
  if (iconINo) 	
    delete iconINo;
  if (iconB) 	
    delete iconB;
  if (iconBNo) 	
    delete iconBNo;
  if (iconT) 	
    delete iconT;
  if (iconTNo) 	
    delete iconTNo;
  if (iconBack) 	
    delete iconBack;
  if (iconBackNo) 	
    delete iconBackNo;
  if (iconForward) 	
    delete iconForward;
  if (iconForwardNo) 
    delete iconForwardNo;
  if (iconH1) 	
    delete iconH1;
  if (iconH1No) 	
    delete iconH1No;
  if (iconH2) 	
    delete iconH2;
  if (iconH2No) 	
    delete iconH2No;
  if (iconH3) 	
    delete iconH3;
  if (iconH3No) 	
    delete iconH3No;
  if (iconPrint) 	
    delete iconPrint;
  if (iconBullet) 	
    delete iconBullet;
  if (iconBulletNo) 
    delete iconBulletNo;
  if (iconNum) 
    delete iconNum;
  if (iconNumNo) 	
    delete iconNumNo;
  if (iconImage) 	
    delete iconImage;
  if (iconImageNo) 	
    delete iconImageNo;
  if (iconDL) 
    delete iconDL;
  if (iconDLNo) 
    delete iconDLNo;
  if (iconLink) 	
    delete iconLink;
  if (iconLinkNo) 
    delete iconLinkNo;
  if (iconTable) 	
    delete iconTable;
  if (iconTableNo) 	
    delete iconTableNo;
  if (iconLogo) 	
    delete iconLogo;

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
  iconLogo = (ThotIcon) 0;
#endif /* defined(_WX) */
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
      TtaFreeMemory (SaveCssURL);
      TtaFreeMemory (SavingFile);
      TtaFreeMemory (SavedDocumentURL);
      TtaFreeMemory (AttrHREFvalue);
      TtaFreeMemory (UserCSS);
      TtaFreeMemory (URL_list);
      TtaFreeMemory (Template_list);
      TtaFreeMemory (AutoSave_list);
      FreeHTMLParser ();
      FreeXmlParserContexts ();
      FreeDocHistory ();
      FreeTransform ();
      QueryClose ();

      FreeAmayaIcons ();
   
      /* free mathml allocations */
      FreeMathML();

      /* free svg allocations */
      FreeSVG ();

#ifdef ANNOTATIONS
      XPointer_bufferFree ();
      ANNOT_Quit ();
#endif /* ANNOTATIONS */
#ifdef BOOKMARKS
      BM_Quit ();
#endif /* BOOKMARKS */

#ifdef TEMPLATES
      FreeTemplateEnvironment ();
#endif /* TEMPLATES */

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
  ThotBool            restoredDoc;
  ThotBool            map, add, bt;

  if (AmayaInitialized)
    return;
  AmayaInitialized = 1;
  ErrFile = NULL;
  CSSErrFile = NULL;
  W3Loading = 0;
  BackupDocument = 0;
  DoDialog = TRUE; /* don't update the CSS dialog by default */
  /* the first window should be open in a new window */
  InNewWindow = TRUE;
  /* initialize status */
  SelectionDoc = 0;
  ParsedDoc = 0;
  ParsedCSS = 0;
  Error_DocURL = NULL; /* no current error */
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
  TableMenuActive = FALSE;
  MTableMenuActive = FALSE;
  Check_read_ids = FALSE;
  IdElemName[0] = EOS;
  /* Initialize the LogFile variables */
  CleanUpParsingErrors ();
  /* we're not linking an external CSS */
  LinkAsCSS = FALSE;
  LinkAsXmlCSS = FALSE;

  /* initialize icons */
#ifdef _WX
  stopR         = (ThotIcon) 0;
  stopN         = (ThotIcon) 0;
  iconSave      = (ThotIcon) 0;
  iconSaveNo    = (ThotIcon) 0;
  iconFind      = (ThotIcon) 0;
  iconReload    = (ThotIcon) 0;
  iconHome      = (ThotIcon) 0;
  iconBack      = (ThotIcon) 0;
  iconBackNo    = (ThotIcon) 0;
  iconForward   = (ThotIcon) 0;
  iconForwardNo = (ThotIcon) 0;
  iconPrint     = (ThotIcon) 0;
  iconI = (ThotIcon) 0;
  iconINo = (ThotIcon) 0;
  iconB = (ThotIcon) 0;
  iconBNo = (ThotIcon) 0;
  iconT = (ThotIcon) 0;
  iconTNo = (ThotIcon) 0;
  iconH1 = (ThotIcon) 0;
  iconH1No = (ThotIcon) 0;
  iconH2 = (ThotIcon) 0;
  iconH2No = (ThotIcon) 0;
  iconH3 = (ThotIcon) 0;
  iconH3No = (ThotIcon) 0;
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
  iconLogo = (ThotIcon) 0;
#endif /* _WX */
#ifdef _GTK
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
#endif /* _GTK */

  /* init transformation callback */
  TtaSetTransformCallback ((Func2) TransformIntoType);
  TargetName = NULL;
  TtaSetAccessKeyFunction ((Proc2) AccessKeyHandler);
  TtaSetEntityFunction ((Proc4) MapEntityByCode);
  TtaSetDoctypeFunction ((Proc3) HasADoctype);
  TtaSetCopyAndCutFunction ((Proc1) RegisterURLSavedElements);
  TtaSetCopyCellFunction ((Proc3) CopyCell);
  TtaSetCopyRowFunction ((Proc3) CopyRow);
  TtaSetNextCellInColumnFunction ((Proc5) NextCellInColumn);
  TtaSetFocusChange ((Proc1) FocusChanged);
  TtaSetAttributeChangeFunction ((Proc2) AttributeChange);
  /* Initialize the Amaya user and tmp directories */
  s = TtaGetEnvString ("APP_TMPDIR");
  if (!TtaCheckMakeDirectory (s, TRUE))
    /* try to use the default value */
    {
      s = TtaGetDefEnvString ("APP_TMPDIR");
      if (TtaCheckMakeDirectory (s, TRUE))
        /* make it the current user one */
        TtaSetEnvString ("APP_TMPDIR", s, TRUE);
      else
        /* didn't work, so we exit */
        {
          sprintf (TempFileDirectory,
                   TtaGetMessage (AMAYA, AM_CANNOT_CREATE_DIRECTORY), s);
#ifdef _WINGUI
          MessageBox (NULL, TempFileDirectory, "Error", MB_OK);
#endif /* _WINGUI */
#if defined(_GTK) || defined(_WX) 
          fprintf (stderr, TempFileDirectory);
#endif /* defined(_GTK) || defined(_WX) */
          exit (1);
        }
    }

  /* add the temporary directory in document path */
  strcpy (TempFileDirectory, s);
  TtaAppendDocumentPath (TempFileDirectory);
#ifdef _WINGUI
  s = TtaGetEnvString ("APP_HOME");
  if (!TtaCheckMakeDirectory (s, TRUE))
    /* didn't work, so we exit */
    {
      sprintf (TempFileDirectory,
               TtaGetMessage (AMAYA, AM_CANNOT_CREATE_DIRECTORY), s);
      MessageBox (NULL, TempFileDirectory, "Error", MB_OK);
      exit (1);
    }
#endif /* _WINGUI */
  /*
   * Build the User preferences file name:
   * $HOME/.amaya/amaya.css on Unix platforms
   * $HOME\amaya\amaya.css on Windows platforms
   */
  ptr = (char *)TtaGetMemory (MAX_LENGTH);
  sprintf (ptr, "%s%c%s.css", s, DIR_SEP, TtaGetAppName());
  UserCSS = TtaStrdup (ptr);
  InitUserStyleSheet (UserCSS);
  TtaFreeMemory (ptr);
  /* check if the new location is set */
  ptr = TtaGetEnvString ("NEW_LOCATION");
  if (ptr == NULL)
    {
      i = 0;
      TtaGetEnvInt("OPENING_LOCATION", &i);
      TtaSetEnvInt ("NEW_LOCATION", i+1, TRUE);/* new tab */
    }
  else
    ptr = NULL;

  /* Initialize environment variables if they are not defined */
  TtaSetEnvBoolean ("SHOW_BUTTONS", TRUE, FALSE);
  TtaSetEnvBoolean ("SHOW_ADDRESS", TRUE, FALSE);
  TtaSetEnvBoolean ("SHOW_MAP_AREAS", FALSE, FALSE);
  TtaSetEnvBoolean ("SHOW_TARGET", FALSE, FALSE);
  TtaSetEnvBoolean ("LOAD_IMAGES", TRUE, FALSE);
  TtaSetEnvBoolean ("LOAD_OBJECTS", TRUE, FALSE);
  TtaSetEnvBoolean ("LOAD_CSS", TRUE, FALSE);
  TtaSetEnvBoolean ("SEND_REFERER", FALSE, FALSE);
  TtaSetEnvBoolean ("INSERT_NBSP", FALSE, FALSE);
  TtaSetEnvBoolean ("GENERATE_MATHPI", TRUE, FALSE);
#ifdef _WX
  TtaSetEnvBoolean ("CLOSE_WHEN_APPLY", TRUE, FALSE);
#endif /* _WX */
  /* get current value */
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
      MapAreas[i] = map;
      HSplit[i] = FALSE;
      VSplit[i] = FALSE;
      SButtons[i] = bt;
      SAddress[i] = add;
      /* initialize history */
      InitDocHistory (i);
    }

  /* allocate working buffers */
  LastURLName = (char *)TtaGetMemory (MAX_LENGTH);
  LastURLName[0] = EOS;
  DirectoryName = (char *)TtaGetMemory (MAX_LENGTH);
  DirectoryName[0] = EOS;
  SavedDocumentURL = NULL;
  /* set path on current directory */
#ifdef _WX
  wxString homedir = TtaGetHomeDir ();
  strcpy (DirectoryName, (const char *)homedir.mb_str(wxConvUTF8));
#else /* _WX */
#ifdef _WINDOWS
  s = getenv ("HOMEDRIVE");
  ptr = getenv ("HOMEPATH");
  if (s && *s && ptr)
    {
      sprintf (DirectoryName, "%s%s", s, ptr);
      s = NULL;
      ptr = NULL;
    }
#else /* _WINDOWS */
  s = getenv ("HOME");
  if (s && *s)
    {
      strcpy (DirectoryName, s);
      s = NULL;
    }
#endif /* _WINDOWS */
#endif /* _WX */
  if (DirectoryName[0] == EOS || !TtaDirExists (DirectoryName))
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
  SaveCssURL = (char *)TtaGetMemory (MAX_LENGTH);
  SaveCssURL[0] = EOS;
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
  TtaSetAutoSave ((Proc1)GenerateAutoSavedDoc);
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
  sprintf (LostPicturePath, "%s\\amaya\\lost.gif", TtaGetEnvString ("THOTDIR"));
#endif /* _WINDOWS */
#ifdef _GTK
  sprintf (LostPicturePath, "%s/amaya/lost.gif", TtaGetEnvString ("THOTDIR"));   
#endif /* _GTK */
  InitMathML ();
#ifdef _SVG
  InitSVG ();
  InitSVGAnim ();
  /*InitSVGLibraryManagerStructure ();*/
  InitLibrary();
#endif /* _SVG */
  /* MKP: disable "Cooperation" menu if DAV is not defined or
   *      initialize davlib module otherwise */
#ifdef DAV
  InitDAV ();
#else /* DAV */
  DAVLibEnable = FALSE;
#endif /* DAV */

#ifdef TEMPLATES
  InitTemplates();
#endif

  InsertableElement_Init();

  URL_list = NULL;
  URL_list_len = 0;
  InitStringForCombobox ();

  AutoSave_list = NULL;
  AutoSave_list_len = 0;
  InitAutoSave ();

  CurrentDocument = 0;
  DocBook = 0;
  DontReplaceOldDoc = FALSE;
  restoredDoc = RestoreAmayaDocs ();
  s = NULL;
  if (restoredDoc)
    {
      /* old documents are restored */
      TtaFreeMemory (ptr);
      return;
    }
#ifdef _WX
  int arg_doc_id = 1;
  while (arg_doc_id < appArgc)
    {
      s = appArgv[arg_doc_id];
      OpenNewDocFromArgv(s);
      arg_doc_id++;
    }
  /* load the homepage if nothing has been found in the command line */
  if (arg_doc_id == 1)
    OpenNewDocFromArgv(NULL);
#else /* _WX */
  if (appArgc % 2 == 0)
    /* The last argument in the command line is the document to be opened */
    s = appArgv[appArgc - 1];

  // load the document with its url (s)
  OpenNewDocFromArgv(s);
#endif /* _WX */


  TtaFreeMemory (ptr);
  ptr = NULL;
  Loading_method = CE_ABSOLUTE;

#ifdef _WX
  /* register openurl callback in order to call it when twice amaya instance are running */
  TtaRegisterOpenURLCallback( (void (*)(void*))OpenNewDocFromArgv );

  /* send math entities to MathML panel */
  AmayaParams p;
  p.param1 = 1; /* init action */
  p.param2 = (void*)pMathEntityTable;
  TtaSendDataToPanel( WXAMAYA_PANEL_SPECHAR, p );
#endif /* _WX */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void OpenNewDocFromArgv( char * url )
{
  int i;
  char ptr[MAX_LENGTH];
  char * s = url;
  
#ifdef _WX
  /* load the document in the default location */
  LoadDefaultOpeningLocation (TRUE);
#endif /* _WX */

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
    {
      /* no argument, no Home, and no previous page: display default Amaya URL */
      GoToHome (0, 1);
    }
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
#else /* _WINDOWS */
            if (LastURLName[0] == DIR_SEP)
#endif /* _WINDOWS */
	     /* it is an absolute name */
              TtaExtractName (LastURLName, DirectoryName, DocumentName);
            else
              {
                /* it is a relative name */
                getcwd (DirectoryName, MAX_LENGTH);
                strcpy (DocumentName, LastURLName);
              }
          /* start with the local document */
          LastURLName[0] = EOS;
          CallbackDialogue (BaseDialog + OpenForm, INTEGER_DATA, (char *) 1);
        }
    }
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
      TtaSetAttributeValue (attr, 1, NULL, doc);
      TtaAttachAttribute (root, attr, doc);
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
  ShowPanel
  show/hide the side panel
  ----------------------------------------------------------------------*/
void ShowPanel (Document doc, View view)
{
#ifdef _WX
  int frame_id = GetWindowNumber (doc, view);
  TtaToggleOnOffSidePanel( frame_id );
#endif /* _WX */
}

/*----------------------------------------------------------------------
  FullScreen
  enable/disable fullscreen
  ----------------------------------------------------------------------*/
void FullScreen (Document doc, View view)
{
#ifdef _WX
  int frame_id = GetWindowNumber (doc, view);
  TtaToggleOnOffFullScreen( frame_id );
#endif /* _WX */
}

/*----------------------------------------------------------------------
  SplitHorizontally
  split horizontally the view
  ----------------------------------------------------------------------*/
void SplitHorizontally (Document doc, View view)
{  
#ifdef _WX
  int frame_id = GetWindowNumber (doc, view);
  TtaSplitViewHorizontally( frame_id );
#endif /* _WX */
}

/*----------------------------------------------------------------------
  SplitVertically
  split vertically the view
  ----------------------------------------------------------------------*/
void SplitVertically (Document doc, View view)
{
#ifdef _WX
  int frame_id = GetWindowNumber (doc, view);
  TtaSplitViewVertically( frame_id );
#endif /* _WX */
}

/*----------------------------------------------------------------------
  ShowHSplitToggle
  Show toggle mark
  ----------------------------------------------------------------------*/
void ShowHSplitToggle (Document doc, View view)
{  
#ifdef _WX
  HSplit[doc] = TRUE;
  TtaSetToggleItem (doc, view, Views, TSplitHorizontally, HSplit[doc]);
  // Set V toggle off
  HideVSplitToggle (doc, view);
  // Update the document source toggle
  if (DocumentSource[doc])
    {
      HSplit[DocumentSource[doc]] = TRUE;
      TtaSetToggleItem (DocumentSource[doc], 1, Views,
			TSplitHorizontally, HSplit[DocumentSource[doc]]);
    }
#endif /* _WX */
}
/*----------------------------------------------------------------------
  ShowVSplitToggle
  Show toggle mark
  ----------------------------------------------------------------------*/
void ShowVSplitToggle (Document doc, View view)
{  
#ifdef _WX
  VSplit[doc] = TRUE;
  TtaSetToggleItem (doc, view, Views, TSplitVertically, VSplit[doc]);
  // Set H toggle off
  HideHSplitToggle (doc, view);
  // Update the document source toggle
  if (DocumentSource[doc])
    {
      VSplit[DocumentSource[doc]] = TRUE;
      TtaSetToggleItem (DocumentSource[doc], 1, Views,
			TSplitVertically, VSplit[DocumentSource[doc]]);
    }
#endif /* _WX */
}

/*----------------------------------------------------------------------
  HideHSplitToggle
  Hide toggle mark
  ----------------------------------------------------------------------*/
void HideHSplitToggle (Document doc, View view)
{  
#ifdef _WX
  HSplit[doc] = FALSE;
  TtaSetToggleItem (doc, view, Views, TSplitHorizontally, HSplit[doc]);
  // Update the document source toggle
  if (DocumentSource[doc])
    {
      HSplit[DocumentSource[doc]] = FALSE;
      TtaSetToggleItem (DocumentSource[doc], 1, Views,
			TSplitHorizontally, HSplit[DocumentSource[doc]]);
    }
#endif /* _WX */
}

/*----------------------------------------------------------------------
  HideVSplitToggle
  Hide toggle mark
  ----------------------------------------------------------------------*/
void HideVSplitToggle (Document doc, View view)
{  
#ifdef _WX
  VSplit[doc] = FALSE;
  TtaSetToggleItem (doc, view, Views, TSplitVertically, VSplit[doc]);
  // Update the document source toggle
  if (DocumentSource[doc])
    {
      VSplit[DocumentSource[doc]] = FALSE;
      TtaSetToggleItem (DocumentSource[doc], 1, Views,
			TSplitVertically, VSplit[DocumentSource[doc]]);
    }
#endif /* _WX */
}

/*----------------------------------------------------------------------
  ShowMapAreas
  Execute the "Show Map Areas" command
  ----------------------------------------------------------------------*/
void ShowMapAreas (Document doc, View view)
{
#ifdef _WINGUI
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
#endif /* _WINGUI */
  
#if defined(_GTK) || defined(_WX)  
  MapAreas[doc] = !MapAreas[doc];
  TtaSetToggleItem (doc, view, Views, TShowMapAreas, MapAreas[doc]);
#endif /* _GTK || _WX */
  ChangeAttrOnRoot (doc, HTML_ATTR_ShowAreas);
}

/*----------------------------------------------------------------------
  ShowButtons
  Execute the "Show Buttons" command
  ----------------------------------------------------------------------*/
void ShowButtons (Document doc, View view)
{

#ifdef _WINGUI
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
#endif /* _WINGUI */
  
#if defined(_GTK) || defined(_WX)  
  SButtons[doc] = !SButtons[doc];
#endif /* #if defined(_GTK) || defined(_WX) */
  
  TtaSetToggleItem (doc, 1, Views, TShowButtonbar, SButtons[doc]);
#ifndef _WX
  TtcSwitchButtonBar (doc, view);
#endif /* _WX */
}

/*----------------------------------------------------------------------
  ShowAddress
  Execute the "Show Address" command
  ----------------------------------------------------------------------*/
void ShowAddress (Document doc, View view)
{
  
#ifdef _WINGUI
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
#endif /* _WINGUI */
  
#if defined(_GTK) || defined(_WX)  
  SAddress[doc] = !SAddress[doc];
#endif /* #if defined(_GTK) || defined(_WX) */
  
  TtaSetToggleItem (doc, 1, Views, TShowTextZone, SAddress[doc]);
#ifndef _WX
  TtcSwitchCommands (doc, view);
#endif /* _WX */
}

/*----------------------------------------------------------------------
  MakeIDMenu
  A menu for adding or removing ID attributes in a document
  ----------------------------------------------------------------------*/
void MakeIDMenu (Document doc, View view)
{
#if defined(_GTK) || defined(_WX) 
  int     i;
  char    s[MAX_LENGTH];
#endif /* #if defined(_GTK) || defined(_WX) */

  /* initialize the global variables */
  IdStatus[0] = EOS;
  IdDoc = doc;

  /* Create the dialogue form */
#if defined(_GTK) || defined(_WX) 
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
                 NULL, 0 /* no max length */, FALSE);
  /* status label */
  TtaNewLabel (BaseDialog + mIdStatus,
               BaseDialog + MakeIdMenu,
               " ");
  /* select the current radio button */
  TtaSetMenuForm (BaseDialog + mIdUseSelection, IdApplyToSelection);
  TtaSetDialoguePosition ();
  TtaShowDialogue (BaseDialog + MakeIdMenu, TRUE);
#endif /* #if defined(_GTK) || defined(_WX) */

#ifdef _WINGUI  
  CreateMakeIDDlgWindow (TtaGetViewFrame (doc, view));
#endif /* _WINGUI */

#ifdef _WX
  if (CreateMakeIdDlgWX (BaseDialog + MakeIdMenu, TtaGetViewFrame (doc, view)))
    {
      TtaSetDialoguePosition ();
      TtaShowDialogue (BaseDialog + MakeIdMenu, FALSE);
    }
#endif /* _WX */
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
  CloseTab close the current active page
  Shortcut : CTRL x + CTRL p
  ----------------------------------------------------------------------*/
void AmayaCloseTab (Document doc, View view)
{
#ifdef _WX
  int page_id       = -1;
  int page_position = 0;
  int window_id     = 0;
  
  window_id = TtaGetDocumentWindowId( doc, view );
  if (TtaUniqueTabInWindow (doc))
    TtaCloseWindow( window_id );
  else
    {
      /* Get the window id and page id of current document and
         close the corresponding page */
      TtaGetDocumentPageId( doc, view, &page_id, &page_position );
      TtaClosePage( window_id, page_id );

      /* Close the windows if it contains no more page */
      TtaCleanUpWindow( window_id );
    }
#endif /* _WX */
}

/*----------------------------------------------------------------------
  CloseOtherTabs close all tabs but selected
  ----------------------------------------------------------------------*/
void CloseOtherTabs( Document doc, View view)
{
#ifdef _WX
  int page_id       = -1;
  int page_position = 0;
  int window_id     = 0;
  
  window_id = TtaGetDocumentWindowId( doc, view );
  /* Get the window id and page id of current document and
     close the corresponding page */
  TtaGetDocumentPageId (doc, view, &page_id, &page_position);
  TtaCloseAllPageButThis (window_id, page_id);
#endif /* _WX */
}

/*----------------------------------------------------------------------
  NewTab open a new tab with an empty page
  ----------------------------------------------------------------------*/
void NewTab (Document doc, View view)
{
#ifdef _WX
  char  *s = (char *)TtaGetMemory (MAX_LENGTH);
#ifdef _WINDOWS
  sprintf (s, "%s\\empty", TtaGetEnvString ("THOTDIR"));
#else /* _WINDOWS */
  sprintf (s, "%s/empty", TtaGetEnvString ("THOTDIR"));
#endif /* _WINDOWS */
  /* load an empty document */
  ThotCallback (BaseDialog + URLName,  STRING_DATA, s);
  DontReplaceOldDoc = TRUE;
  InNewWindow       = FALSE;
  ThotCallback (BaseDialog + OpenForm, INTEGER_DATA, (char*)1);
  TtaFreeMemory (s);
#endif /* _WX */
}

/*----------------------------------------------------------------------
  RefreshTab force the page to refresh
  ----------------------------------------------------------------------*/
void RefreshTab (Document doc, View view)
{
  DisplayMode         dispMode;

  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    {
      TtaSetDisplayMode (doc, NoComputedDisplay);
      RedisplayDoc (doc);
      TtaSetDisplayMode (doc, dispMode);
      //TtaPlay (doc, 1);
    }
}

/*----------------------------------------------------------------------
  RefreshAllTabs force all pages to refresh
  ----------------------------------------------------------------------*/
void RefreshAllTabs (Document doc, View view)
{
#ifdef _WX
  int      i;
  int      ref_id = 0, window_id;
  
  ref_id = TtaGetDocumentWindowId (doc, view);
  if (ref_id == 0)
    return;
  for (i = 1; i < MAX_DOCUMENTS; i++)
    {
      if (DocumentURLs[i])
        {
          window_id = TtaGetDocumentWindowId (i, 1);
          if (window_id == ref_id)
            RefreshTab (i, 1);
        }
    }
#endif /* _WX */
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaCloseWindow (Document doc, View view)
{
  /* Save the current windows geometry */
  SaveGeometryOnExit( doc, NULL);
#ifdef _WX
  /* get the document's parent window and try to close it */
  int window_id = TtaGetDocumentWindowId( doc, view );
  TtaCloseWindow( window_id );
#else /* _WX */
  if (DocumentURLs[doc] && view == 1)
    {
      TtcCloseDocument (doc, view);
      if (!W3Loading)
        CheckAmayaClosed ();
    }
  else
    TtcCloseView (doc, view);
#endif /* _WX */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaClose (Document document, View view)
{
#ifdef _WX
  AmayaWindowIterator it;
  for( it.first(); !it.isDone(); it.next() )
    {
      /* close each window one by one */
      TtaCloseWindow( it.currentWindowId() );
    }
#endif /* _WX */

#ifndef _WX
  int          i;
  ThotBool     documentClosed;

  /* invalid current loading */
  W3Loading = 0;

  documentClosed = TRUE;

  /* Save the current windows geometry */
  SaveGeometryOnExit( document, NULL );

  /* free each loaded document */
  for (i = DocumentTableLength; i > 0; i--)
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
#endif /* _WX */
}


/*----------------------------------------------------------------------
  AddURLInCombobox adds the new pathname in the combobox list.
  Store that URL into the file only if keep is TRUE.
  URIs are encoded with the default charset.
  ----------------------------------------------------------------------*/
void AddURLInCombobox (char *pathname, char *form_data, ThotBool keep)
{
  char          *urlstring, *app_home, *old_list, *url;
  int            i, j, len, nb, end, max;
  FILE          *file = NULL;
  unsigned char *localname;
  CHARSET        encoding;

  if (pathname == NULL || pathname[0] == EOS)
    return;
  if (form_data && form_data[0] != EOS)
    {
      url = (char *)TtaGetMemory (strlen (pathname) + strlen (form_data) + 2);
      sprintf (url, "%s?%s", pathname, form_data);
    }
  else
    url = TtaStrdup (pathname);
  urlstring = (char *) TtaGetMemory (MAX_LENGTH);
  /* open the file list_url.dat into APP_HOME directory */
  app_home = TtaGetEnvString ("APP_HOME");
#ifdef _WX
  /* force url encoding to utf8 */
  sprintf (urlstring, "%s%clist_url_utf8.dat", app_home, DIR_SEP);
  encoding = UTF_8;
#else /* _WX */
  sprintf (urlstring, "%s%clist_url.dat", app_home, DIR_SEP);
  encoding = TtaGetLocaleCharset();
#endif /* _WX */
  /* keep the previous list */
  old_list = URL_list;
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
      if (!strcmp (url, "empty"))
        // empty is a keyword to display an empty document    
        keep = FALSE; // never keep an empty file
      strcpy (URL_list, url);
      if (keep)
        {
          if (encoding != UTF_8)
            {
              localname = TtaConvertMbsToByte ((unsigned char *)url, encoding);
              fprintf (file, "\"%s\"\n", localname);
              TtaFreeMemory (localname);
            }
          else
            fprintf (file, "\"%s\"\n", url);
        }

      max = GetMaxURLList();
      if (old_list && old_list[i] != EOS)
        {
          /* now write other urls */
          while (old_list[i] != EOS && nb < max)
            {
              end = strlen (&old_list[i]) + 1;
              if ((URL_list_keep || i != 0) &&
                  (end != len || strncmp (url, &old_list[i], len)))
                {
                  /* add the newline between two urls */
                  strcpy (&URL_list[j], &old_list[i]);
                  if (keep)
                    {
                      if (encoding != UTF_8)
                        {
                          localname = TtaConvertMbsToByte ((unsigned char *)&old_list[i], encoding);
                          fprintf (file, "\"%s\"\n", localname);
                          TtaFreeMemory (localname);
                        }
                      else
                        fprintf (file, "\"%s\"\n", &old_list[i]);
                    }
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
  TtaFreeMemory (old_list);
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

#ifdef _WX
  /* force url encoding to utf8 */
  sprintf ((char *)urlstring, "%s%clist_url_utf8.dat", app_home, DIR_SEP);
#else /* _WX */
  sprintf ((char *)urlstring, "%s%clist_url.dat", app_home, DIR_SEP);
#endif /* _WX */
  
  file = TtaReadOpen ((char *)urlstring);
  *urlstring = EOS;
  if (file)
    {
      /* get the size of the file */
      fseek (file, 0L, 2);	/* end of the file */
      URL_list_len = (ftell (file) * 4) + GetMaxURLList() + 4;
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
              urlstring[len] = EOS;
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
void RemoveDocFromSaveList (char *name, char *initial_url, int doctype)
{
  char     *urlstring, *app_home, *ptr, *list_item;
  char     *ptr_end, *ptr_beg;
  int       i, j, len, nb, end;
  FILE     *file = NULL;

  if (name == NULL || name[0] == EOS)
    return;
  if (initial_url == NULL || initial_url[0] == EOS)
    return;
  /* keep the previous list */
  ptr = AutoSave_list;
  /* create a new list */
  AutoSave_list = (char *)TtaGetMemory (AutoSave_list_len + 1);  
  len = strlen (initial_url) + strlen (name) + 1;
  len += 17; /*doctype + quotation marks + spaces */
  list_item  = (char *)TtaGetMemory (len);
  sprintf (list_item, "\"%s\" \"%s\" %d", name, initial_url, doctype);
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
                          if (strcmp (initial_url, ptr_beg))
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
              TtaFreeMemory (AutoSave_list);
              AutoSave_list = NULL;
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
  
  TtaFreeMemory (ptr);
  TtaFreeMemory (urlstring);
  TtaFreeMemory (list_item);
}

/*----------------------------------------------------------------------
  AddDocInSaveList adds the new URL into the AutoSave list
  ----------------------------------------------------------------------*/
void AddDocInSaveList (char *name, char *initial_url, int doctype)
{
  char     *urlstring, *app_home, *ptr;
  char     *ptr_end, *ptr_beg;
  int       i, j, len, nb, end;
  FILE     *file = NULL;

  if (AutoSave_Interval == 0)
    return;
  if (name == NULL || name[0] == EOS)
    return;
  if (initial_url == NULL || initial_url[0] == EOS)
    return;
  /* keep the previous list */
  ptr = AutoSave_list;
  /* create a new list */
  len = strlen (initial_url) + strlen (name) + 1;
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
      sprintf (AutoSave_list, "\"%s\" \"%s\" %d", name, initial_url, doctype);
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
                      if ((end != len) || (strcmp (initial_url, ptr_beg)))
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

  TtaFreeMemory (ptr);
  TtaFreeMemory (urlstring);
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
      AutoSave_list_len = ftell (file) + GetMaxURLList() + 4;
      AutoSave_list = (char *)TtaGetMemory (AutoSave_list_len);
      AutoSave_list[0] = EOS;
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
              urlstring[len] = EOS;
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

/*----------------------------------------------------------------------
  SaveGeometryOnExit
  save the current document geometry only if "Save geometry on exit" is enable
  ----------------------------------------------------------------------*/
void SaveGeometryOnExit (int document, const char * view_name)
{
  ThotBool saveGeometry = FALSE;

  TtaGetEnvBoolean ("SAVE_GEOMETRY", &saveGeometry);
  if (saveGeometry)
    /* Save the current windows geometry */
    SetGeometryConf ( document, view_name );
}

/*----------------------------------------------------------------------
  LoadDefaultOpeningLocation controls where the new document is displayed
  The parameter noReplace says the replace is not allowed
  ----------------------------------------------------------------------*/
void LoadDefaultOpeningLocation (ThotBool noReplace)
{
  int where_id = 1;

  /* get the default location in thotrc */
  TtaGetEnvInt ("NEW_LOCATION", &where_id);
  if (noReplace && where_id == 0)
    where_id++; /* zero based in the config file */

  ThotCallback(BaseDialog + OpenLocation , INTEGER_DATA, (char*)where_id);
}

/*----------------------------------------------------------------------
  GetMaxURLList()
  returns the max urls which can be stored
  ----------------------------------------------------------------------*/
int GetMaxURLList()
{
  int max;
  TtaSetDefEnvString ("MAX_URL_LIST", "20", FALSE); // by default 20 urls
  TtaGetEnvInt ("MAX_URL_LIST", &max);
  return max;
}

/*----------------------------------------------------------------------
  SetMaxURLList()
  set a new limit for : max urls which can be stored
  ----------------------------------------------------------------------*/
void SetMaxURLList(int max)
{
  TtaSetEnvInt ("MAX_URL_LIST", max, TRUE);
}

/*----------------------------------------------------------------------
  ClearURLList()
  empty the url list
  ----------------------------------------------------------------------*/
void ClearURLList()
{
  char * app_home;
  char   filename[MAX_LENGTH];
  FILE * file = NULL;

  /* cleanup the urllist variable */
  URL_list[0] = EOS;

  /* cleanup the urllist file */ 
  app_home = TtaGetEnvString ("APP_HOME");
#ifdef _WX
  /* force url encoding to utf8 */
  sprintf ((char *)filename, "%s%clist_url_utf8.dat", app_home, DIR_SEP);
#else /* _WX */
  sprintf ((char *)filename, "%s%clist_url.dat", app_home, DIR_SEP);
#endif /* _WX */
  file = TtaWriteOpen(filename);
  TtaWriteClose(file);

#ifdef _WX
  /* then cleanup the combobox widget */
  AmayaWindowIterator it;
  for( it.first(); !it.isDone(); it.next() )
    {
      /* empty url list on each windows */
      TtaEmptyURLBar( it.currentWindowId() );
    }
#endif /* _WX */
}


#ifdef _WX
#include <wx/sstream.h>
#include <wx/wfstream.h>
#include <wx/dir.h>
#include "../thotlib/internals/h/SMTP.h"
#endif /* _WX */

/*----------------------------------------------------------------------
 * Retrieve a valid temporary directory path.
 * Dont create the file, just assumes that it doesnt exist and can be created. 
 * The returned value must be deleted.
 * Returns NULL if any problem occurs.
 * Dont use mktemp because doesnt exist on windows
  ----------------------------------------------------------------------*/
char* CreateTempDirectory(const char* name)
{
  static int i = 0, len;
  char                 buff[MAX_LENGTH];
  char                 temppath[MAX_LENGTH];
  
  strcpy(temppath, TtaGetEnvString ("APP_TMPDIR"));
  len = strlen(temppath);
  if(len==0)
    return NULL;
  if(temppath[len]!=DIR_SEP)
  {
    temppath[len] = DIR_SEP;
    temppath[len+1] = 0;
    
  }
  
  while(i<10000)
  {
    sprintf(buff, "%s%s%04d", temppath, name, i);
    if(!TtaCheckDirectory(buff))
    {
      if(TtaCheckMakeDirectory(buff, TRUE))
        return TtaStrdup(buff);
    }
    i++;
  }
  return NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SendByMail (Document document, View view)
{
#ifdef _WX
  char                 buff[MAX_LENGTH];
  ElementType          elType;
  Element              docEl, el, text;
  int                  len;
  Language             lang;
  EMail                mail;
  wxArrayString        arr;
  int                  i;
  SendByMailDlgWX dlg(0, NULL);
  
  char* temppath = CreateTempDirectory("sendmail/");

  char* server = TtaGetEnvString("EMAILS_SMTP_SERVER");
  char* from   = TtaGetEnvString("EMAILS_FROM_ADDRESS");
  char* docPath, *docType, *docChar;
  
  int   port;
  int   error;
  ThotBool retry = TRUE;

  TtaGetEnvInt ("EMAILS_SMTP_PORT", &port);

  if(server==NULL || from==NULL ||
      strlen(server)==0 || strlen(from)==0 || port==0)
  {
    TtaDisplaySimpleMessage(INFO, AMAYA, AM_EMAILS_NO_SERVER);
    // TODO Show the properties dialog at the "emails" tab.
    return;
  }

  Synchronize(document, view); 

  SaveTempCopy(document, temppath);

  if (DocumentTypes[document] == docHTML)
  {
    docEl = TtaGetMainRoot (document);
    elType = TtaGetElementType(docEl);
    elType.ElTypeNum = HTML_EL_TITLE;
    el = TtaSearchTypedElement (elType, SearchInTree, docEl);
    text = TtaGetFirstChild (el);
    len = TtaGetTextLength(text);
    if(len>0)
    {
      len = MAX_LENGTH-1;
      TtaGiveTextContent(text, (unsigned char*)buff, &len, &lang);
      buff[len] = 0;
    }
    else
      buff[0] = 0;
    dlg.SetSubject(TtaConvMessageToWX(buff));
  }

  while(retry)
  {
    if(dlg.ShowModal()==wxID_OK)
    {
      mail = TtaNewEMail( (const char*)dlg.GetSubject().mb_str(wxConvUTF8),
                            (const char*) dlg.GetMessage().mb_str(wxConvUTF8),
                            from);
      if(mail!=NULL)
      {
        arr = dlg.GetRecipients();
        for(i=0; i<(int)arr.GetCount(); i++)
        {
          TtaAddEMailToRecipient(mail, (const char*) arr[i].mb_str(wxConvUTF8));
        }
        docPath = GetLocalPath(document, DocumentURLs[document]);
        docType = DocumentMeta[document]->content_type;
        docChar = DocumentMeta[document]->charset;
  
        // Send document as attachment
        if(dlg.SendAsAttachment())
          TtaAddEMailAttachmentFile(mail, docType, docPath);
        // Send document as mail message
        else if(dlg.SendAsContent())
          TtaAddEMailAlternativeFile(mail, docType, docPath, docChar);
        
        // Send all attached files (images, css ...) as attachments.
        if(dlg.SendAsAttachment()||dlg.SendAsContent())
        {
          wxFileName    msgName(wxString(docPath, wxConvUTF8));
          wxArrayString files;
          wxDir::GetAllFiles(wxString(temppath, wxConvLibc), &files, wxT(""), wxDIR_FILES);
          for(int i=0; i<(int)files.GetCount(); i++)
          {
            wxFileName filename(files[i]);
            if(filename.GetFullName()!=msgName.GetFullName())
            {
              TtaAddEMailAttachmentFile(mail, "", (const char*)filename.GetFullPath().mb_str(wxConvUTF8));
            }
          }
        }
        
        error = 0;
        if(TtaSendEMail(mail, server, port, &error))
        {
          TtaSetStatus(document, view, TtaGetMessage (AMAYA, AM_EMAILS_SENT), NULL);
        }
      }

      switch(error)
      {
        case EMAIL_OK:
          retry = FALSE;
          break;
        case EMAIL_SERVER_NOT_RESPOND:
          TtaDisplaySimpleMessage(INFO, AMAYA, AM_EMAILS_ERR_SERVER_RESPOND);
          break;
        case EMAIL_SERVER_REJECT:
          TtaDisplaySimpleMessage(INFO, AMAYA, AM_EMAILS_ERR_SERVER_REJECT);
          break;
        case EMAIL_FROM_BAD_ADDRESS:
          TtaDisplaySimpleMessage(INFO, AMAYA, AM_EMAILS_ERR_FROM_ADDR);
          break;
        case EMAIL_TO_BAD_ADDRESS:
          TtaDisplaySimpleMessage(INFO, AMAYA, AM_EMAILS_ERR_RCPT_ADDR);
          break;
        case EMAIL_BAD_CONTENT:
          TtaDisplaySimpleMessage(INFO, AMAYA, AM_EMAILS_ERR_BAD_CONTENT);
          break;
        default:
          TtaDisplaySimpleMessage(INFO, AMAYA, AM_EMAILS_ERR_UNKNOW);
          break;
      }
      
    }
    else
      break;
  }
  
  // Remove temp dir content.
  
  wxArrayString files;
  wxDir::GetAllFiles(wxString(temppath, wxConvUTF8), &files, wxT(""), wxDIR_FILES);
  for(int i=0; i<(int)files.GetCount(); i++)
  {
    wxRemoveFile(files[i]);
  }
  
  wxRmdir(wxString(temppath, wxConvUTF8));
  
#endif /* _WX */
}
