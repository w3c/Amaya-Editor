/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
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
 
/*
 * thotmsg.c : display API error messages
 *
 * Author: V. Quint (INRIA)
 *
 */

#include "thot_sys.h"
#include "libmsg.h"
#include "constmedia.h"
#include "typemedia.h"
#include "application.h"
#include "language.h"
#include "message.h"

#define THOT_EXPORT extern
#include "edit_tv.h"
 /* Identification des messages */
ThotBool            PrintErrorMessages;

#include "viewcommands_f.h"

/*----------------------------------------------------------------------
   TtaError affiche les messsges d'erreur de l'API.                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaError (int errorCode)
#else  /* __STDC__ */
void                TtaError (errorCode)
int                 errorCode;

#endif /* __STDC__ */
{
#ifdef THOT_DEBUG
   CHAR_T                buffer[50];

   UserErrorCode = errorCode;
   if (PrintErrorMessages)
     {
	fprintf (stderr, "Thot tool kit error: ");
	switch (errorCode)
	      {
		 case ERR_document_name:
		    usprintf (buffer, "invalid document name\n");
		    break;
		 case ERR_too_many_documents:
		    usprintf (buffer, "too many documents\n");
		    break;
		 case ERR_cannot_read_struct_schema:
		    usprintf (buffer, "cannot read structure schema\n");
		    break;
		 case ERR_empty_document:
		    usprintf (buffer, "empty document\n");
		    break;
		 case ERR_cannot_open_pivot_file:
		    usprintf (buffer, "cannot open pivot file\n");
		    break;
		 case ERR_invalid_document_parameter:
		    usprintf (buffer, "invalid document parameter");
		    break;
		 case ERR_read_only_document:
		    usprintf (buffer, "read only document\n");
		    break;
		 case ERR_invalid_associated_root:
		    usprintf (buffer, "invalid associated root\n");
		    break;
		 case ERR_invalid_parameter:
		    usprintf (buffer, "invalid parameter\n");
		    break;
		 case ERR_not_implemented:
		    usprintf (buffer, "feature not implemented\n");
		    break;
		 case ERR_invalid_attribute_value:
		    usprintf (buffer, "invalid attribute value\n");
		    break;
		 case ERR_attribute_element_mismatch:
		    usprintf (buffer, "attribute element mismatch\n");
		    break;
		 case ERR_buffer_too_small:
		    usprintf (buffer, "buffer too small\n");
		    break;
		 case ERR_invalid_element_type:
		    usprintf (buffer, "invalid element type\n");
		    break;
		 case ERR_element_does_not_match_DTD:
		    usprintf (buffer, "element does not match DTD\n");
		    break;
		 case ERR_invalid_attribute_type:
		    usprintf (buffer, "invalid attribute type\n");
		    break;
		 case ERR_duplicate_attribute:
		    usprintf (buffer, "duplicate attribute\n");
		    break;
		 case ERR_mandatory_attribute:
		    usprintf (buffer, "mandatory attribute\n");
		    break;
		 case ERR_cannot_set_link:
		    usprintf (buffer, "cannot set link\n");
		    break;
		 case ERR_no_selection_in_document:
		    usprintf (buffer, "no selection in document\n");
		    break;
		 case ERR_incorrect_tree:
		    usprintf (buffer, "incorrect tree\n");
		    break;
		 case ERR_cannot_open_view:
		    usprintf (buffer, "cannot open view\n");
		    break;
		 case ERR_there_are_open_views:
		    usprintf (buffer, "there are open views\n");
		    break;
		 case ERR_cannot_load_pschema:
		    usprintf (buffer, "cannot load pschema\n");
		    break;
		 case ERR_duplicate_presentation_rule:
		    usprintf (buffer, "duplicate presentation rule\n");
		    break;
		 case ERR_string_too_long:
		    usprintf (buffer, "string too long\n");
		    break;
		 case ERR_cannot_holophrast_a_root:
		    usprintf (buffer, "cannot holophrast a root\n");
		    break;
		 case ERR_cannot_holophrast_that_type:
		    usprintf (buffer, "cannot holophrast that type\n");
		    break;
		 case ERR_main_window_not_open:
		    usprintf (buffer, "main window not open\n");
		    break;
		 case ERR_element_already_inserted:
		    usprintf (buffer, "Element already inserted\n");
		    break;
		 case ERR_no_presentation_schema:
		    usprintf (buffer, "No presentation schema\n");
		    break;
		 case ERR_element_has_no_box:
		    usprintf (buffer, "Element has no box\n");
		    break;
		 case ERR_invalid_X_connection:
		    usprintf (buffer, "cannot connect to X server\n");
		    break;
		 case ERR_cannot_open_main_window:
		    usprintf (buffer, "cannot open main window\n");
		    break;
		 case ERR_invalid_button:
		    usprintf (buffer, "invalid button\n");
		    break;
		 case ERR_too_many_languages:
		    usprintf (buffer, "Language table is full\n");
		    break;
		 case ERR_language_not_found:
		    usprintf (buffer, "Language not found\n");
		    break;
		 default:
		    usprintf (buffer, "%d\n", errorCode);
		    break;
	      }
#   ifdef _WINDOWS
    /* MessageBox (NULL, buffer, "", MB_OK | MB_ICONERROR); */
#   else  /* _WINDOWS */
	fprintf (stderr, buffer);
#   endif /* _WINDOWS */
     }
#endif /* THOT_DEBUG */
}
