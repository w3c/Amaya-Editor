/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Authors: V. Quint, I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
 *
 */	

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
#include "dialog.h"
#ifndef NODISPLAY
#include "thotpattern.h"
#endif /* NODISPLAY */
#include "appdialogue.h"
#include "dictionary.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "platform_tv.h"
#include "appevents_tv.h"
#include "units_tv.h"
#undef THOT_EXPORT
#define THOT_EXPORT
#include "print_tv.h"
#include "edit_tv.h"
#include "thotcolor.h"
#include "thotcolor_tv.h"
#include "thotpalette_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

#include "appdialogue_f.h"
#include "applicationapi_f.h"
#include "checkaccess_f.h"
#include "config_f.h"
#include "dialogapi_f.h"
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
#include "structschema_f.h"
#include "structselect_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"

static Proc         AppClosingFunction = NULL;
#define VersionId TEXT ("V2.1")
ThotBool            PrintErrorMessages = TRUE;

#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 IdentDocument (PtrDocument pDoc)
#else  /* __STDC__ */
int                 IdentDocument (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 d;

   d = 1;
   while (LoadedDocument[d - 1] != pDoc && d < MAX_DOCUMENTS)
      d++;
   if (LoadedDocument[d - 1] == pDoc)
      return d;
   else
      return 0;
}

/*----------------------------------------------------------------------
   Termine l'insertion de caracteres dans une boite de texte       
  ----------------------------------------------------------------------*/
void                CloseInsertion ()
{
   if (ThotLocalActions[T_stopinsert] != NULL)
      (*ThotLocalActions[T_stopinsert]) ();
}



#ifndef NODISPLAY
/*----------------------------------------------------------------------
   GetViewInfo returns wiew number and assoc state of the          
   corresponding to the view of the document.             
   Parameters:                                                     
   document: the document.                                 
   view: the view.                                         
   Return value:                                                   
   corresponding view number.                              
   corresponding assoc state.                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetViewInfo (Document document, View view, int *viewnumber, ThotBool * assoc)
#else  /* __STDC__ */
void                GetViewInfo (document, view, viewnumber, assoc)
Document            document;
View                view;
int                *viewnumber;
ThotBool           *assoc;

#endif /* __STDC__ */
{

   *assoc = FALSE;
   *viewnumber = 0;

   if (view < 100)
      *viewnumber = (int) view;
   else
     {
	*assoc = TRUE;
	*viewnumber = (int) view - 100;
     }
}


/*----------------------------------------------------------------------
   GetWindowNumber returns the window corresponding to the view of 
   the document.                                           
   Parameters:                                                     
   document: the document.                                 
   view: the view.                                         
   Return value:                                                   
   corresponding window.                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 GetWindowNumber (Document document, View view)
#else  /* __STDC__ */
int                 GetWindowNumber (document, view)
Document            document;
View                view;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   ThotBool            assoc;
   int                 aView, win;

   win = 0;
   /* Checks parameters */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] != NULL)
      if (view < 1 || (view > MAX_VIEW_DOC && view < 100) || view > MAX_ASSOC_DOC + 100)
         TtaError (ERR_invalid_parameter);
      else
        {
	pDoc = LoadedDocument[document - 1];
	GetViewInfo (document, view, &aView, &assoc);
	if (assoc)
	   win = pDoc->DocAssocFrame[aView - 1];
	else
	   win = pDoc->DocViewFrame[aView - 1];
        }
   return win;
}

/*----------------------------------------------------------------------
   TtaIsPrinting returns TRUE is the application is printing.    
  ----------------------------------------------------------------------*/
ThotBool         TtaIsPrinting ()
{
  return (Printing);
}

/*----------------------------------------------------------------------
   TtaGetViewFrame returns the frame widget of the document view.    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotWidget          TtaGetViewFrame (Document document, View view)
#else  /* __STDC__ */
ThotWidget          TtaGetViewFrame (document, view)
Document            document;
View                view;
#endif /* __STDC__ */
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
#ifndef _WINDOWS
    return (FrameTable[frame].WdFrame);
#else  /* _WINDOWS */
    return (FrMainRef[frame]);
#endif /* _WINDOWS */
}
#endif

/*----------------------------------------------------------------------
   CoreHandler est un handler d'erreur fatale.                     
  ----------------------------------------------------------------------*/
static void         ErrorHandler ()
{
#  ifndef _WINDOWS
   signal (SIGBUS, SIG_DFL);
   signal (SIGPIPE, SIG_IGN);
#  endif /* _WINDOWS */
   signal (SIGSEGV, SIG_DFL);
#ifdef SIGABRT
   signal (SIGABRT, SIG_DFL);
#else
   signal (SIGIOT, SIG_DFL);
#endif
   ufprintf (stderr, TtaGetMessage (LIB, TMSG_DEBUG_ERROR));

   if (ThotLocalActions [T_backuponfatal] != NULL)
     {
       (*ThotLocalActions [T_backuponfatal]) ();
     }
   exit (1);
}


/*----------------------------------------------------------------------
   QuitHandler est un handler pour Interrupt.                      
  ----------------------------------------------------------------------*/
static void         QuitHandler ()
{
   signal (SIGINT, ErrorHandler);
#  ifndef _WINDOWS 
   signal (SIGQUIT, SIG_DFL);
#  endif /* _WINDOWS */
   signal (SIGTERM, ErrorHandler);
   if (ThotLocalActions [T_backuponfatal] != NULL)
     (*ThotLocalActions [T_backuponfatal]) ();
   exit (1);
#  ifndef _WINDOWS
   signal (SIGINT, QuitHandler);
   signal (SIGQUIT, QuitHandler);
   signal (SIGTERM, QuitHandler);
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   InitErrorHandler initialise le handler de core dump.             
  ----------------------------------------------------------------------*/
void                InitErrorHandler ()
{
#  ifndef _WINDOWS
   signal (SIGBUS, ErrorHandler);
   signal (SIGHUP, ErrorHandler);
   signal (SIGQUIT, QuitHandler);
#  endif /* _WINDOWS */

   signal (SIGSEGV, ErrorHandler);
#  ifdef SIGABRT
   signal (SIGABRT, ErrorHandler);
#  else
   signal (SIGIOT, ErrorHandler);
#  endif

   signal (SIGINT, QuitHandler);
   signal (SIGTERM, QuitHandler);
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
#ifdef __STDC__
void                TtaInitialize (CHAR_T* applicationName)
#else  /* __STDC__ */
void                TtaInitialize (applicationName)
CHAR_T*             applicationName;

#endif /* __STDC__ */
{
   int                 i;

   UserErrorCode = 0;
   ustrcpy (DefaultDocumentName, TEXT(""));
   InitEditorMemory ();		/* Initializes the memory managment of the editor */
   InitNatures ();		/* Initializes the table of Natures */

   FullStructureChecking = FALSE;

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
   /* Initializes the color table */
   NColors = MAX_COLOR;
   RGB_Table = RGB_colors;
   Color_Table = Name_colors;

#ifndef NODISPLAY
#  ifdef _WINDOWS
   TtPrinterDC = NULL;
   WIN_GetDeviceContext (-1);
   DOT_PER_INCHE = GetDeviceCaps(TtDisplay, LOGPIXELSY);
   WIN_ReleaseDeviceContext ();
   /* DOT_PER_INCHE = 72; */
#  else  /* !_WINDOWS */
   DOT_PER_INCHE = 72;
#  endif /* _WINDOWS */

   numOfJobs = 0;
   /* Initializes patterns */
   NbPatterns = sizeof (Name_patterns) / sizeof (STRING);

   Patterns = Name_patterns;
#endif /* NODISPLAY */
   /* load the message table of the Thot Library */
   i = TtaGetMessageTable (TEXT("libdialogue"), TMSG_LIB_MSG_MAX);
   switch (i)
	 {
	    case 0:
	       break;
	    case -1:
	       fprintf (stderr, "cannot find messages table\n");
	       exit (1);
	    default:
	       fprintf (stderr, "Previous messages table loaded\n");
	       exit (1);
	 }

   /* init the system error signal handler */
   InitErrorHandler ();

#ifndef NODISPLAY

   ConfigInit ();
   /* Initilizes the space character displaying mode */
   ShowSpace = 1;
   InputSpace = 0;
#endif

   CheckAccessLoadResources ();

}

/*----------------------------------------------------------------------
   TtaSetApplicationQuit register the procedure that must be called
   just before the application will be closed.
   That procedure has no parameters.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetApplicationQuit (Proc procedure)
#else  /* __STDC__ */
void                TtaSetApplicationQuit (procedure)
Proc                procedure;
#endif /* __STDC__ */
{
  AppClosingFunction = procedure;
}


/*----------------------------------------------------------------------
   TtaQuit

   Quits the Thot tool kit. No other function of the tool kit can then
   be called by the application.

  ----------------------------------------------------------------------*/
void                TtaQuit ()
{
#ifndef NODISPLAY
  FreeDocColors ();
  FreeAllMessages ();
  Prof_FreeTable ();
#endif /* NODISPLAY */
  FreeAll ();
#ifndef NODISPLAY
  FreeTranslations ();
  FreeMenus ();
#endif /* NODISPLAY */
  TtaFreeAppRegistry ();
  if (AppClosingFunction)
    (*AppClosingFunction) ();
  exit (0);
}

/*----------------------------------------------------------------------
   TtaSetErrorMessages

   Indicates to the tool kit whether error messages must be printed or not.

   Parameter:
   on: 1 if error messages must be printed, 0 if not.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetErrorMessages (int on)
#else  /* __STDC__ */
void                TtaSetErrorMessages (on)
int                 on;
#endif /* __STDC__ */
{
   PrintErrorMessages = (ThotBool) on;
}

/*----------------------------------------------------------------------
   TtaGetVersion

   Returns the identifier of the current version of the Thot editing tool kit.

   Return value:
   identifier of the current version.

  ----------------------------------------------------------------------*/
STRING              TtaGetVersion ()
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
int                 TtaGetErrorCode ()
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
#ifdef __STDC__
STRING              TtaGetStrError (int errorCode)
#else  /* __STDC__ */
STRING              TtaGetStrError (errorCode)
int                 errorCode;

#endif /* __STDC__ */
{
   STRING              strError = NULL;

   switch (errorCode)
	 {
	    case ERR_document_name:
	       strError = TEXT("invalid document name");
	       break;
	    case ERR_too_many_documents:
	       strError = TEXT("too many documents");
	       break;
	    case ERR_cannot_read_struct_schema:
	       strError = TEXT("cannot read structure schema");
	       break;
	    case ERR_empty_document:
	       strError = TEXT("empty document");
	       break;
	    case ERR_cannot_open_pivot_file:
	       strError = TEXT("cannot open pivot file");
	       break;
	    case ERR_invalid_document_parameter:
	       strError = TEXT("invalid document parameter");
	       break;
	    case ERR_read_only_document:
	       strError = TEXT("read only document");
	       break;
	    case ERR_invalid_associated_root:
	       strError = TEXT("invalid associated root");
	       break;
	    case ERR_invalid_parameter:
	       strError = TEXT("invalid parameter");
	       break;
	    case ERR_not_implemented:
	       strError = TEXT("feature not implemented");
	       break;
	    case ERR_invalid_attribute_value:
	       strError = TEXT("invalid attribute value");
	       break;
	    case ERR_attribute_element_mismatch:
	       strError = TEXT("attribute element mismatch");
	       break;
	    case ERR_buffer_too_small:
	       strError = TEXT("buffer too small");
	       break;
	    case ERR_invalid_element_type:
	       strError = TEXT("invalid element type");
	       break;
	    case ERR_element_does_not_match_DTD:
	       strError = TEXT("element does not match DTD");
	       break;
	    case ERR_invalid_attribute_type:
	       strError = TEXT("invalid attribute type");
	       break;
	    case ERR_duplicate_attribute:
	       strError = TEXT("duplicate attribute");
	       break;
	    case ERR_mandatory_attribute:
	       strError = TEXT("mandatory attribute");
	       break;
	    case ERR_cannot_set_link:
	       strError = TEXT("cannot set link");
	       break;
	    case ERR_no_selection_in_document:
	       strError = TEXT("no selection in document");
	       break;
	    case ERR_incorrect_tree:
	       strError = TEXT("incorrect tree");
	       break;
	    case ERR_cannot_open_view:
	       strError = TEXT("cannot open view");
	       break;
	    case ERR_there_are_open_views:
	       strError = TEXT("there are open views");
	       break;
	    case ERR_cannot_load_pschema:
	       strError = TEXT("cannot load pschema");
	       break;
	    case ERR_duplicate_presentation_rule:
	       strError = TEXT("duplicate presentation rule");
	       break;
	    case ERR_string_too_long:
	       strError = TEXT("string too long");
	       break;
	    case ERR_cannot_holophrast_a_root:
	       strError = TEXT("cannot holophrast a root");
	       break;
	    case ERR_cannot_holophrast_that_type:
	       strError = TEXT("cannot holophrast that type");
	       break;
	    case ERR_main_window_not_open:
	       strError = TEXT("main window not open");
	       break;
	    case ERR_element_already_inserted:
	       strError = TEXT("element already inserted");
	       break;
	    case ERR_too_many_languages:
	       strError = TEXT("language table is full");
	       break;
	    case ERR_language_not_found:
	       strError = TEXT("language not found");
	       break;
	    case ERR_no_presentation_schema:
	       strError = TEXT("no presentation schema");
	       break;
	    case ERR_element_has_no_box:
	       strError = TEXT("element has no box");
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
#ifdef __STDC__
void                ThotExit (int result)
#else  /* __STDC__ */
void                ThotExit (result)
int                 result;

#endif /* __STDC__ */
{
   fflush (stderr);
   fflush (stdout);
   if (result)
      ErrorHandler ();
   else
      exit (result);
}

/* end of module */
