/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
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

#define Template_URI_o  "http://wam.inrialpes.fr/xtiger"
#define Template_URI    "http://ns.inria.org/xtiger"
#define Template_URI_f  "http://ns.inria.fr/xtiger"
#ifndef __CEXTRACT__

/*----------------------------------------------------------------------
  This function returns the current appversion comming from configure
  ----------------------------------------------------------------------*/
const char * TtaGetAppVersion();

/*----------------------------------------------------------------------
  This function returns the app name
  ( if was : #define HTAppName     "amaya" )
  ----------------------------------------------------------------------*/
const char * TtaGetAppName();

/*----------------------------------------------------------------------
  This function returns the app date
  (it was : #define HTAppDate     __DATE__ )
  ----------------------------------------------------------------------*/
const char * TtaGetAppDate();

/*----------------------------------------------------------------------
  This function returns the app year
  ----------------------------------------------------------------------*/
const char * TtaGetAppYear();

     
/*----------------------------------------------------------------------
  TtaUseDotForFloat returns TRUE if floats take the form xx.yy
 ----------------------------------------------------------------------*/
extern ThotBool TtaUseDotForFloat ();

/*----------------------------------------------------------------------
   TtaInitialize
   Initializes the Thot editing tool kit for an application. This function
   must be called before any other function of the tool kit.
   Parameter:
   applicationName: the argv[0] of the application that requires services
   from the tool kit.  This name is used for accessing the ressources
   defined in file .Xdefaults.
  ----------------------------------------------------------------------*/
extern void TtaInitialize (char *applicationName);

/*----------------------------------------------------------------------
   TtaSetApplicationQuit register the procedure that must be called
   just before the application will be closed.
   That procedure has no parameters.
  ----------------------------------------------------------------------*/
extern void TtaSetApplicationQuit (Proc procedure);

/*----------------------------------------------------------------------
  TtaSetEntityFunction registers the function that gives entity names:
  procedure (int entityValue, Document doc, ThotBool withMath, char **entityName)
  ----------------------------------------------------------------------*/
extern void TtaSetEntityFunction (Proc4 procedure);

/*----------------------------------------------------------------------
  TtaSetDoctypeFunction registers the function that say if the document
  has a doctype declaration.
  procedure (Document doc, ThotBool *found, ThotBool *useMath);
  ----------------------------------------------------------------------*/
extern void TtaSetDoctypeFunction (Proc3 procedure);

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
extern const char *TtaGetVersion (void);

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
extern const char *TtaGetStrError (int errorCode);

/*----------------------------------------------------------------------
   TtaExtractName: extracts the directory and the file name.       
   aDirectory and aName must be arrays of characters       
   which sizes are sufficient to contain the path and      
   the file name.                                          
  ----------------------------------------------------------------------*/
extern void TtaExtractName (const char *text, char *aDirectory, char *aName);

/*----------------------------------------------------------------------
   TtaIsPrinting returns TRUE is the application is printing.    
  ----------------------------------------------------------------------*/
extern ThotBool TtaIsPrinting ();

/*----------------------------------------------------------------------
  TtaGetTime returns the current date in a formatted string.
  Inspired from the hypermail source code: hypermail/src/date.c
  ----------------------------------------------------------------------*/
extern void TtaGetTime (char *s, CHARSET charset);

#ifndef IV_DEBUG

extern void  *TtaGetMemory ( unsigned int n );
extern void  TtaFreeMemory ( void *ptr );
extern void* TtaRealloc (void *ptr, unsigned int n);

#else /*_DEBUG*/

#ifdef _WINGUI

/*****************VISUAL STUDIO MEMORY LEAK DETECTION 
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxconenablingmemoryleakdetection.asp
**********************/
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#undef malloc
#define malloc(size) _malloc_dbg(size,_NORMAL_BLOCK,__FILE__,__LINE__)

#undef free
#define free(ptr) _free_dbg(ptr, _NORMAL_BLOCK)

#undef realloc 
#define realloc(ptr, size) _realloc_dbg(ptr, size, _NORMAL_BLOCK,__FILE__,__LINE__)

#endif /*_WINGUI*/

#undef TtaGetMemory
#define TtaGetMemory(size) malloc(((size)?size:1))    

#undef TtaFreeMemory
#define TtaFreeMemory(ptr) free(ptr)    

#undef TtaRealloc
#define TtaRealloc(ptr, size) realloc(ptr, size)   

#endif /*_DEBUG*/

extern char*     TtaStrdup (const char* str);

#endif /* __CEXTRACT__ */

#endif
