/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Authors: V. Quint, I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA): Adaptation for Windows NT/95 
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
#include "app.h"
#include "dialog.h"

#include "thotpattern.h"
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

#include "checkaccess_f.h"
#include "config_f.h"
#include "dialogapi_f.h"
#include "input_f.h"
#include "language_f.h"
#include "memory_f.h"
#include "references_f.h"
#include "schemas_f.h"
#include "schtrad_f.h"
#include "searchmenu_f.h"
#include "structschema_f.h"
#include "structselect_f.h"
#include "tree_f.h"
#include "applicationapi_f.h"

#define VersionId "V2.0"

int                 UserErrorCode;
boolean             PrintErrorMessages = TRUE;

#ifndef _WIN_PRINT
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
#endif /* _WIN_PRINT */


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
   perror (TtaGetMessage (LIB, TMSG_DEBUG_ERROR));

   if (ThotLocalActions [T_backuponfatal] != NULL)
     {
       perror (TtaGetMessage (LIB, TMSG_DEBUG_SAV_FILES));
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
void                TtaInitialize (char *applicationName)
#else  /* __STDC__ */
void                TtaInitialize (applicationName)
char               *applicationName;

#endif /* __STDC__ */
{
   int                 i;

   UserErrorCode = 0;
   strcpy (DefaultDocumentName, "");
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
   DOT_PER_INCHE = 72;
   numOfJobs = 0;
   /* Initializes patterns */
   NbPatterns = sizeof (Name_patterns) / sizeof (char *);

   Patterns = Name_patterns;
   /* load the message table of the Thot Library */
   i = TtaGetMessageTable ("libdialogue", TMSG_LIB_MSG_MAX);
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
   /* no external action declared at that time */
   ActionList = NULL;

   ConfigInit ();
   TtaConfigReadConfigFiles (SchemaPath);
   /* Initilizes the space character displaying mode */
   ShowSpace = 1;
   InputSpace = 0;
   InitSelection ();
#endif

   CheckAccessLoadResources ();

}

/*----------------------------------------------------------------------
   TtaQuit

   Quits the Thot tool kit. No other function of the tool kit can then
   be called by the application.

  ----------------------------------------------------------------------*/
void                TtaQuit ()
{
   TtaSaveAppRegistry ();

#  ifndef NODISPLAY
#  ifdef _WINDOWS
   if (!TtIsTrueColor)
	  if (TtCmap && !DeleteObject (TtCmap))
         WinErrorBox (WIN_Main_Wd);
#  endif /* _WINDOWS */
   FreeAllMessages ();
#  endif /* NODISPLAY */
   FreeAll ();
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
   PrintErrorMessages = (boolean) on;
}

/*----------------------------------------------------------------------
   TtaGetVersion

   Returns the identifier of the current version of the Thot editing tool kit.

   Return value:
   identifier of the current version.

  ----------------------------------------------------------------------*/
char               *TtaGetVersion ()
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
char               *TtaGetStrError (int errorCode)
#else  /* __STDC__ */
char               *TtaGetStrError (errorCode)
int                 errorCode;

#endif /* __STDC__ */
{
   char               *strError = NULL;

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
	    case ERR_invalid_associated_root:
	       strError = "invalid associated root";
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


/*----------------------------------------------------------------------
   TtaExtractName: extracts the directory and the file name.       
   aDirectory and aName must be arrays of characters       
   which sizes are sufficient to contain the path and      
   the file name.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaExtractName (char *text, char *aDirectory, char *aName)

#else  /* __STDC__ */
void                TtaExtractName (text, aDirectory, aName)
char               *text;
char               *aDirectory;
char               *aName;

#endif /* __STDC__ */
{
   int                 lg, i, j;
   char               *ptr, *oldptr;
   char                URL_DIR_SEP;

   if (text == NULL || aDirectory == NULL || aName == NULL)
      return;			/* No input text or error in input parameters */

   if (text && strchr (text, '/'))
	  URL_DIR_SEP = '/';
   else 
	   URL_DIR_SEP = DIR_SEP;
   
   aDirectory[0] = EOS;
   aName[0] = EOS;

   lg = strlen (text);
   if (lg)
     {
	/* the text is not empty */
	ptr = oldptr = &text[0];
	do
	  {
#        ifndef _WINDOWS
	     ptr = strrchr (oldptr, DIR_SEP);
#        else  /* _WINDOWS */
	     ptr = strrchr (oldptr, URL_DIR_SEP);
#        endif /* _WINDOWS */
		 if (ptr != NULL)
		oldptr = &ptr[1];
	  }
	while (ptr != NULL);

	i = (int) (oldptr) - (int) (text);	/* the length of the directory part */
	if (i > 1)
	  {
	     strncpy (aDirectory, text, i);
	     j = i - 1;
	     /* Suppresses the / characters at the end of the path */
	     while (aDirectory[j] == URL_DIR_SEP)
		aDirectory[j--] = EOS;
	  }
	if (i != lg)
	   strcpy (aName, oldptr);
     }
#    ifdef _WINDOWS
     lg = strlen (aName);
     if (!strcasecmp (&aName[lg - 4], ".exe"))
        aName[lg - 4] = EOS;
#    endif /* _WINDOWS */
}

/* end of module */
