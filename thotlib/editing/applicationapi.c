/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Authors: V. Quint, I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) Windows version
 *
 */	

#ifdef _WX
/* uneable debug because strange variables are declared in wx/memory.h header (maybe a wxwidgets bug) */
#undef __WXDEBUG__
#include "wx/app.h"
int WX_SearchResult = 0;
#endif /* _WX */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmenu.h"
#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "message.h"
#include "language.h"
#include "application.h"
#include "typecorr.h"

#include "consttra.h"
#include "typetra.h"
#include "appaction.h"
#include "appstruct.h"
#ifndef NODISPLAY
#include "dialog.h"
#include "thotpattern.h"
#endif /* NODISPLAY */
#include "appdialogue.h"
#include "dictionary.h"
#include "picture.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "platform_tv.h"
#include "appevents_tv.h"
#include "units_tv.h"

#include "print_tv.h"
#include "edit_tv.h"
#include "thotcolor.h"
#include "thotcolor_tv.h"
//#include "thotpalette_tv.h"
#include "frame_tv.h"
#include "boxes_tv.h"
#include "appdialogue_tv.h"

#include "appdialogue_f.h"
#include "applicationapi_f.h"
#include "checkaccess_f.h"
#include "callback_f.h"
#include "callbackinit_f.h"
#include "config_f.h"
#include "dialogapi_f.h"
#include "documentapi_f.h"
#include "font_f.h"
#include "inites_f.h"
#include "input_f.h"
#include "language_f.h"
#include "memory_f.h"
#include "message_f.h"
#include "profiles_f.h"
#include "references_f.h"
#include "schemas_f.h"
#include "schtrad_f.h"
#include "searchmenu_f.h"
#include "structcommands_f.h"
#include "structschema_f.h"
#include "structselect_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"

static Proc         AppClosingFunction = NULL;
#define VersionId "V3.1"
static ThotBool            PrintErrorMessages = TRUE;

#ifdef _WINGUI
#include "wininclude.h"
#endif /* _WINGUI */

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int IdentDocument (PtrDocument pDoc)
{
  int                 d;

  if (!pDoc)
    return 0;
  d = 1;
  while (LoadedDocument[d - 1] != pDoc && d < MAX_DOCUMENTS)
    d++;
  if (LoadedDocument[d - 1] == pDoc)
    return d;
  else
    return 0;
}

#ifndef NODISPLAY
/*----------------------------------------------------------------------
  GetWindowNumber returns the frame corresponding to the view of 
  the document.                                           
  Parameters:                                                     
  document: the document.                                 
  view: the view.                                         
  Return value:
  the corresponding frame id                                                   
  ----------------------------------------------------------------------*/
int GetWindowNumber (Document document, View view)
{
  int                 win;

  win = 0;
  /* Check parameters */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] != NULL)
    {
      if (view < 1 || view > MAX_VIEW_DOC)
        TtaError (ERR_invalid_parameter);
      else
        win = LoadedDocument[document - 1]->DocViewFrame[view - 1];
    }
  return win;
}

/*----------------------------------------------------------------------
  TtaIsPrinting returns TRUE is the application is printing.    
  ----------------------------------------------------------------------*/
ThotBool TtaIsPrinting ()
{
  return (Printing);
}

/*----------------------------------------------------------------------
  TtaGetViewFrame returns the frame widget of the document view.    
  ----------------------------------------------------------------------*/
ThotWidget TtaGetViewFrame (Document document, View view)
{
  int                 frame;

  if (document == 0 && view == 0)
    return 0;
  else
    {
      frame = GetWindowNumber (document, view);
      if (frame == 0)
        return 0;
    }
  /* Si les parametres sont invalides */
  if (frame > MAX_FRAME)
    {
      TtaError (ERR_invalid_parameter);
      return 0;
    }
  else

#ifdef _WINGUI
    return (FrMainRef[frame]);
#endif /* _WINGUI */
#if defined(_GTK) || defined(_WX)
  return ((ThotWidget)FrameTable[frame].WdFrame);
#endif /* #if defined(_GTK) || defined(_WX) */
}
#endif

/*----------------------------------------------------------------------
  CoreHandler est un handler d'erreur fatale.                     
  ----------------------------------------------------------------------*/
static void ErrorHandler ()
{
#ifndef _WINDOWS
  signal (SIGBUS, SIG_DFL);
  signal (SIGPIPE, SIG_IGN);
#endif /* _WINDOWS */
  signal (SIGSEGV, SIG_DFL);
#ifdef SIGABRT
  signal (SIGABRT, SIG_DFL);
#else
  signal (SIGIOT, SIG_DFL);
#endif
  fprintf (stderr, TtaGetMessage (LIB, TMSG_DEBUG_ERROR));
  if (ThotLocalActions [T_backuponfatal] != NULL)
    (*ThotLocalActions [T_backuponfatal]) ();
  {
#if defined(_GTK) && !defined(NODISPLAY)
    gtk_exit (1);
#endif /* _GTK && !NODISPLAY */	
#if defined(_WX) && !defined(NODISPLAY)
    wxExit();
#else /* _WX && !defined(NODISPLAY) */     
    exit (1);
#endif /* _WX && !defined(NODISPLAY) */     
  }
}


/*----------------------------------------------------------------------
  QuitHandler est un handler pour Interrupt.                      
  ----------------------------------------------------------------------*/
static void QuitHandler ()
{
  signal (SIGINT, (void (*)(int))ErrorHandler);
#ifndef _WINDOWS
  signal (SIGQUIT, SIG_DFL);
#endif /* _WINDOWS */
  signal (SIGTERM, (void (*)(int))ErrorHandler);
  if (ThotLocalActions [T_backuponfatal] != NULL)
    (*ThotLocalActions [T_backuponfatal]) ();
  {
#if defined(_GTK) && !defined(NODISPLAY)
    gtk_exit (1);
#endif /* _GTK && !NODISPLAY */	
#if defined(_WX) && !defined(NODISPLAY)
    wxExit();
#else /* _WX && !defined(NODISPLAY) */     
    exit (1);
#endif /* _WX && !defined(NODISPLAY) */     
  }
#if defined(_GTK) || defined(_WX) && !defined(_WINDOWS)
  signal (SIGINT, (void (*)(int))QuitHandler);
  signal (SIGQUIT, (void (*)(int))QuitHandler);
  signal (SIGTERM, (void (*)(int))QuitHandler);
#endif /* #if defined(_GTK) || defined(_WX) && !defined(_WINDOWS) */
}

/*----------------------------------------------------------------------
  InitErrorHandler initialise le handler de core dump.             
  ----------------------------------------------------------------------*/
void InitErrorHandler ()
{
#ifndef _WINDOWS
  signal (SIGBUS, (void (*)(int))ErrorHandler);
  signal (SIGHUP, (void (*)(int))ErrorHandler);
  signal (SIGQUIT, (void (*)(int))QuitHandler);
#endif /* _WINDOWS */

  signal (SIGSEGV, (void (*)(int))ErrorHandler);
#  ifdef SIGABRT
  signal (SIGABRT, (void (*)(int))ErrorHandler);
#  else
  signal (SIGIOT, (void (*)(int))ErrorHandler);
#  endif

  signal (SIGINT, (void (*)(int))QuitHandler);
  signal (SIGTERM, (void (*)(int))QuitHandler);
}

/*----------------------------------------------------------------------
  TtaUseDotForFloat returns TRUE if floats take the form xx.yy
  ----------------------------------------------------------------------*/
ThotBool TtaUseDotForFloat ()
{
  return UseDotForFloat;
}

/*----------------------------------------------------------------------
  TtaInitialize

  Initializes the Thot editing tool kit for an application. This function must be
  called before any other function of the tool kit.
  Parameter:
  applicationName: the argv[0] of the application that requires services
  from the tool kit.  This name is used for accessing the ressources
  defined in file .Xdefaults.
  ----------------------------------------------------------------------*/
void TtaInitialize (char *applicationName)
{
  int                 i;
  float               val = 0.;

  UserErrorCode = 0;
  InitEditorMemory ();	      /* Initializes the memory managment of the editor */
  InitNatures ();	      /* Initializes the table of Natures */
  Dragging = FALSE;

  /* test if the system uses dot or comma in the float syntax */
  sscanf (".5", "%f", &val);
  UseDotForFloat = (val == 0.5);

  FullStructureChecking = FALSE;
  ShowOnePage = FALSE;
  /* Initializes the table of documents */
  for (i = 0; i < MAX_DOCUMENTS; i++)
    LoadedDocument[i] = NULL;

  /* Initializes the table of local actions */
  for (i = 0; i < MAX_LOCAL_ACTIONS; i++)
    ThotLocalActions[i] = NULL;

  DocAutoSave = NULL;
  PrintingDoc = 0;
  /* Initializes the table of translation schemas loaded */
  InitTranslationSchemasTable ();
  /* Read the environment variables concerning the directories */
  InitLanguage ();

#ifndef NODISPLAY
#ifdef _WINGUI
  TtPrinterDC = NULL;
  WIN_Main_Wd = NULL;
#endif /* _WINGUI */
  DOT_PER_INCH = 72;
  numOfJobs = 0;
  /* Initializes patterns */
  NbPatterns = sizeof (Name_patterns) / sizeof (char *);
  Patterns = Name_patterns;
  SVGCreating = FALSE;
#endif /* NODISPLAY */
  /* load the message table of the Thot Library */
  i = TtaGetMessageTable ("libdialogue", TMSG_LIB_MSG_MAX);
  switch (i)
    {
    case 0:
      break;
    case -1:
      fprintf (stderr, "cannot find messages table\n");
#if defined(_GTK) && !defined(NODISPLAY)
      gtk_exit (1);
#endif /* _GTK && !NODISPLAY */	
#if defined(_WX) && !defined(NODISPLAY)
      wxExit();
#else /* _WX && !defined(NODISPLAY) */     
      exit (1);
#endif /* _WX && !defined(NODISPLAY) */     
    default:
      fprintf (stderr, "Previous messages table loaded\n");
#if defined(_GTK) && !defined(NODISPLAY)
      gtk_exit (1);
#endif /* _GTK && !NODISPLAY */	
#if defined(_WX) && !defined(NODISPLAY)
      wxExit();
#else /* _WX && !defined(NODISPLAY) */     
      exit (1);
#endif /* _WX && !defined(NODISPLAY) */     
    }

  /* init the system error signal handler */
  InitErrorHandler ();
#ifndef NODISPLAY
  ConfigInit ();
#endif
  CheckAccessLoadResources ();
}

/*----------------------------------------------------------------------
  TtaSetApplicationQuit register the procedure that must be called
  just before the application will be closed.
  That procedure has no parameters.
  ----------------------------------------------------------------------*/
void TtaSetApplicationQuit (Proc procedure)
{
  AppClosingFunction = procedure;
}


/*----------------------------------------------------------------------
  TtaQuit

  Quit the Thot tool kit. No other function of the tool kit can then
  be called by the application.
  ----------------------------------------------------------------------*/
void TtaQuit ()
{
  PtrDocument       pDoc;
  NotifyDialog      notifyDoc;
  int               d;

#ifndef NODISPLAY
  TtaFreeAllCatalogs ();
#endif /* NODISPLAY */
  /* close all opened document */
  for (d = 0; d < MAX_DOCUMENTS - 1; d++)
    if (LoadedDocument[d])
      {
        /* free the document tree */
        UnloadTree (d + 1);
        notifyDoc.event = TteDocClose;
        notifyDoc.document = d + 1;
        notifyDoc.view = 0;
        CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
        pDoc = LoadedDocument[d];
        if (pDoc)
          {
            /* free document schemas */
            FreeDocumentSchemas (pDoc);
            FreeDocument (pDoc);
            LoadedDocument[d] = NULL;
          }
      }
#ifndef NODISPLAY
  /* remove the contents of the cut buffer related to the document */
  FreeSavedElements ();
  /* free all context related to search commands */
  CleanSearchContext ();
  FreeDocColors ();
  FreeAllMessages ();
  Prof_FreeTable ();
  FreeTranslations ();
  FreeMenus ();
  ThotFreeAllFonts ();
  TteFreeAllEventsList ();
  ConfigFree ();
  FreeAllPicCache ();
#endif /* NODISPLAY */
  FreeAll ();
  TtaFreeAppRegistry ();
  if (AppClosingFunction)
    (*AppClosingFunction) ();
  {
#if defined(_GTK) && !defined(NODISPLAY)
    gtk_exit (0);
#endif /* _GTK && !NODISPLAY */	
#if defined(_WX) && !defined(NODISPLAY)
    /*     wxExit();*/
#else /* _WX && !defined(NODISPLAY) */     
    exit (0);
#endif /* _WX && !defined(NODISPLAY) */     
  }
}

/*----------------------------------------------------------------------
  TtaSetErrorMessages

  Indicates to the tool kit whether error messages must be printed or not.
  Parameter:
  on: 1 if error messages must be printed, 0 if not.
  ----------------------------------------------------------------------*/
void TtaSetErrorMessages (int on)
{
  PrintErrorMessages = (on != 0);
}

/*----------------------------------------------------------------------
  TtaGetVersion

  Returns the identifier of the current version of the Thot editing tool kit.
  Return value:
  identifier of the current version.
  ----------------------------------------------------------------------*/
const char *TtaGetVersion ()
{
  UserErrorCode = 0;
  return (VersionId);
}

/*----------------------------------------------------------------------
  TtaGetErrorCode

  Returns the error code set by the last call to the Thot editing tool kit.
  See file application.h for the possible values.
  Return value:
  last error code, 0 if the last call was successful.
  ----------------------------------------------------------------------*/
int TtaGetErrorCode ()
{
  return UserErrorCode;
}

/*----------------------------------------------------------------------
  TtaGetStrError

  Returns a pointer to the message text for a given error code.
  Parameter:
  errorCode: an error code.
  Return value:
  No return value
  See also:
  TtaGetErrorCode
  ----------------------------------------------------------------------*/
const char *TtaGetStrError (int errorCode)
{
  const char             *strError = NULL;

  switch (errorCode)
    {
    case ERR_document_name:
      strError = "invalid document name";
      break;
    case ERR_too_many_documents:
      strError = "too many documents";
      break;
    case ERR_cannot_read_struct_schema:
      strError = "cannot read structure schema";
      break;
    case ERR_empty_document:
      strError = "empty document";
      break;
    case ERR_cannot_open_pivot_file:
      strError = "cannot open pivot file";
      break;
    case ERR_invalid_document_parameter:
      strError = "invalid document parameter";
      break;
    case ERR_read_only_document:
      strError = "read only document";
      break;
    case ERR_invalid_parameter:
      strError = "invalid parameter";
      break;
    case ERR_not_implemented:
      strError = "feature not implemented";
      break;
    case ERR_invalid_attribute_value:
      strError = "invalid attribute value";
      break;
    case ERR_attribute_element_mismatch:
      strError = "attribute element mismatch";
      break;
    case ERR_buffer_too_small:
      strError = "buffer too small";
      break;
    case ERR_invalid_element_type:
      strError = "invalid element type";
      break;
    case ERR_element_does_not_match_DTD:
      strError = "element does not match DTD";
      break;
    case ERR_invalid_attribute_type:
      strError = "invalid attribute type";
      break;
    case ERR_duplicate_attribute:
      strError = "duplicate attribute";
      break;
    case ERR_mandatory_attribute:
      strError = "mandatory attribute";
      break;
    case ERR_cannot_set_link:
      strError = "cannot set link";
      break;
    case ERR_no_selection_in_document:
      strError = "no selection in document";
      break;
    case ERR_incorrect_tree:
      strError = "incorrect tree";
      break;
    case ERR_cannot_open_view:
      strError = "cannot open view";
      break;
    case ERR_there_are_open_views:
      strError = "there are open views";
      break;
    case ERR_cannot_load_pschema:
      strError = "cannot load pschema";
      break;
    case ERR_duplicate_presentation_rule:
      strError = "duplicate presentation rule";
      break;
    case ERR_string_too_long:
      strError = "string too long";
      break;
    case ERR_cannot_holophrast_a_root:
      strError = "cannot holophrast a root";
      break;
    case ERR_cannot_holophrast_that_type:
      strError = "cannot holophrast that type";
      break;
    case ERR_main_window_not_open:
      strError = "main window not open";
      break;
    case ERR_element_already_inserted:
      strError = "element already inserted";
      break;
    case ERR_too_many_languages:
      strError = "language table is full";
      break;
    case ERR_language_not_found:
      strError = "language not found";
      break;
    case ERR_no_presentation_schema:
      strError = "no presentation schema";
      break;
    case ERR_element_has_no_box:
      strError = "element has no box";
      break;
    default:
      strError = NULL;
      break;
    }
  return (strError);
}


/*----------------------------------------------------------------------
  ThotExit termine l'application Thot.                             
  ----------------------------------------------------------------------*/
void ThotExit (int result)
{
  fflush (stderr);
  fflush (stdout);
  if (result)
    ErrorHandler ();
  else
    {
#if defined(_GTK) && !defined(NODISPLAY)
      gtk_exit (result);
#endif /* _GTK && !NODISPLAY */	
#if defined(_WX) && !defined(NODISPLAY)
      wxExit();
#else /* _WX && !defined(NODISPLAY) */     
      exit (result);
#endif /* _WX && !defined(NODISPLAY) */     
    }
}

