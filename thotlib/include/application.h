/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */
 
#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "typebase.h"
#include "fileaccess.h"
#include "registry.h"
#include "appstruct.h"

/* error codes */

#define ERR_document_name		1
#define ERR_too_many_documents		2
#define ERR_cannot_read_struct_schema	3
#define ERR_empty_document		4
#define ERR_cannot_open_pivot_file	5
#define ERR_invalid_document_parameter	6
#define ERR_read_only_document		8
#define ERR_invalid_associated_root	9
#define ERR_invalid_parameter		10
#define ERR_not_implemented		11
#define ERR_invalid_attribute_value	12
#define ERR_attribute_element_mismatch	14
#define ERR_buffer_too_small		15
#define ERR_invalid_element_type	16
#define ERR_element_does_not_match_DTD	17
#define ERR_invalid_attribute_type	18
#define ERR_duplicate_attribute		19
#define ERR_mandatory_attribute		20
#define ERR_cannot_set_link		21
#define ERR_no_selection_in_document	22
#define ERR_incorrect_tree		23
#define ERR_cannot_open_view		24
#define ERR_there_are_open_views	25
#define ERR_cannot_load_pschema		26
#define ERR_duplicate_presentation_rule	27
#define ERR_string_too_long		28
#define ERR_cannot_holophrast_a_root	29
#define ERR_cannot_holophrast_that_type	30
#define ERR_main_window_not_open	31
#define ERR_element_already_inserted	32
#define ERR_no_presentation_schema	33

#define ERR_element_has_no_box		34
#define ERR_no_selection_in_view	35
#define ERR_invalid_X_connection	50
#define ERR_cannot_open_main_window	51
#define ERR_invalid_button		52
#define ERR_cannot_create_dialogue	53
#define ERR_invalid_parent_dialogue	54
#define ERR_invalid_reference		55
#define ERR_too_many_languages 		100
#define ERR_language_not_found 		101

#ifndef __CEXTRACT__
#ifdef __STDC__


/*----------------------------------------------------------------------
   TtaInitialize
   Initializes the Thot editing tool kit for an application. This function
   must be called before any other function of the tool kit.
   Parameter:
   applicationName: the argv[0] of the application that requires services
   from the tool kit.  This name is used for accessing the ressources
   defined in file .Xdefaults.
  ----------------------------------------------------------------------*/
extern void TtaInitialize (CHAR_T* applicationName);

/*----------------------------------------------------------------------
   TtaSetApplicationQuit register the procedure that must be called
   just before the application will be closed.
   That procedure has no parameters.
  ----------------------------------------------------------------------*/
extern void TtaSetApplicationQuit (Proc procedure);

/*----------------------------------------------------------------------
   TtaQuit
   Quits the Thot tool kit. No other function of the tool kit can then
   be called by the application.
  ----------------------------------------------------------------------*/
extern void TtaQuit (void);

/*----------------------------------------------------------------------
   TtaSetErrorMessages
   Indicates to the tool kit whether error messages must be printed or not.
   Parameter:
   on: 1 if error messages must be printed, 0 if not.
  ----------------------------------------------------------------------*/
extern void TtaSetErrorMessages (int on);

/*----------------------------------------------------------------------
   TtaGetVersion
   Returns the identifier of the current version of the Thot editing tool kit.
   Return value:
   identifier of the current version.
  ----------------------------------------------------------------------*/
extern STRING TtaGetVersion (void);

/*----------------------------------------------------------------------
   TtaGetErrorCode
   Returns the error code set by the last call to the Thot editing tool kit.
   See file application.h for the possible values.
   Return value:
   last error code, 0 if the last call was successful.
  ----------------------------------------------------------------------*/
extern int TtaGetErrorCode (void);

/*----------------------------------------------------------------------
   TtaGetStrError
   Returns a pointer to the message text for a given error code.
   Parameter:
   errorCode: an error code.
   Return value:
   No return value
   See also: TtaGetErrorCode
  ----------------------------------------------------------------------*/
extern STRING TtaGetStrError (int errorCode);

/*----------------------------------------------------------------------
   TtaExtractName: extracts the directory and the file name.       
   aDirectory and aName must be arrays of characters       
   which sizes are sufficient to contain the path and      
   the file name.                                          
  ----------------------------------------------------------------------*/
extern void TtaExtractName (CHAR_T* text, /*OUT*/ CHAR_T* aDirectory, /*OUT*/ CHAR_T* aName);

/*----------------------------------------------------------------------
   TtaIsPrinting returns TRUE is the application is printing.    
  ----------------------------------------------------------------------*/
extern ThotBool TtaIsPrinting ();

extern void*     TtaGetMemory (unsigned int size);
extern void      TtaFreeMemory (void *buffer);
extern STRING    TtaAllocString ( unsigned int n );
extern void*     TtaRealloc (void *ptr, unsigned int n);
extern char*     TtaStrdup (char* str);
extern CHAR_T*   TtaWCSdup (CHAR_T* str);
extern char*     TtaWC2ISOdup (CHAR_T* str);
extern CHAR_T*   TtaISO2WCdup (char* str);

#else  /* __STDC__ */
extern void      TtaInitialize ( /* CHAR_T* applicationName */ );
extern void      TtaSetApplicationQuit (/* Proc procedure */);
extern void      TtaQuit ( /* void */ );
extern void      TtaSetErrorMessages ( /* int on */ );
extern STRING    TtaGetVersion ( /* void */ );
extern int       TtaGetErrorCode ( /* void */ );
extern STRING    TtaGetStrError ( /* int errorCode */ );
extern void      TtaExtractName ( /* CHAR_T* text, CHAR_T* aDirectory, CHAR_T* aName */ );
extern ThotBool  TtaIsPrinting ();
extern void*     TtaGetMemory ( /* int size */ );
extern void      TtaFreeMemory ( /* STRING buffer */ );
extern STRING    TtaAllocString ( /* unsigned int n */ );
extern void*     TtaRealloc ( /* void *ptr, unsigned int n */ );
extern char*    TtaStrdup ( /* char *str */ );
extern CHAR_T*   TtaWCSdup (/* CHAR_T* str */);
extern char*     TtaWC2ISOdup (/* CHAR_T* str */);
extern CHAR_T*   TtaISO2WCdup (/* char* str */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
