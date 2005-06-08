/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
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
#include "thot_gui.h"
#include "thot_sys.h"
#include "libmsg.h"
#include "constmedia.h"
#include "typemedia.h"
#include "application.h"
#include "language.h"
#include "message.h"

#define THOT_EXPORT extern
#include "edit_tv.h"

#ifdef THOT_DEBUG
 /* Identification des messages */
static ThotBool            PrintErrorMessages;
#endif /* #ifdef THOT_DEBUG */

/*----------------------------------------------------------------------
   TtaError affiche les messsges d'erreur de l'API.                
  ----------------------------------------------------------------------*/
void TtaError (int errorCode)
{
#ifdef THOT_DEBUG
   char                buffer[50];

   UserErrorCode = errorCode;
   if (PrintErrorMessages)
     {
	fprintf (stderr, "Thot tool kit error: ");
	switch (errorCode)
	  {
	  case ERR_document_name:
	    sprintf (buffer, "invalid document name\n");
	    break;
	  case ERR_too_many_documents:
	    sprintf (buffer, "too many documents\n");
	    break;
	  case ERR_cannot_read_struct_schema:
	    sprintf (buffer, "cannot read structure schema\n");
	    break;
	  case ERR_empty_document:
	    sprintf (buffer, "empty document\n");
	    break;
	  case ERR_cannot_open_pivot_file:
	    sprintf (buffer, "cannot open pivot file\n");
	    break;
	  case ERR_invalid_document_parameter:
	    sprintf (buffer, "invalid document parameter");
	    break;
	  case ERR_read_only_document:
	    sprintf (buffer, "read only document\n");
	    break;
	  case ERR_invalid_associated_root:
	    sprintf (buffer, "invalid associated root\n");
	    break;
	  case ERR_invalid_parameter:
	    sprintf (buffer, "invalid parameter\n");
	    break;
	  case ERR_not_implemented:
	    sprintf (buffer, "feature not implemented\n");
	    break;
	  case ERR_invalid_attribute_value:
	    sprintf (buffer, "invalid attribute value\n");
	    break;
	  case ERR_attribute_element_mismatch:
	    sprintf (buffer, "attribute element mismatch\n");
	    break;
	  case ERR_buffer_too_small:
	    sprintf (buffer, "buffer too small\n");
	    break;
	  case ERR_invalid_element_type:
	    sprintf (buffer, "invalid element type\n");
	    break;
	  case ERR_element_does_not_match_DTD:
	    sprintf (buffer, "element does not match DTD\n");
	    break;
	  case ERR_invalid_attribute_type:
	    sprintf (buffer, "invalid attribute type\n");
	    break;
	  case ERR_duplicate_attribute:
	    sprintf (buffer, "duplicate attribute\n");
	    break;
	  case ERR_mandatory_attribute:
	    sprintf (buffer, "mandatory attribute\n");
	    break;
	  case ERR_cannot_set_link:
	    sprintf (buffer, "cannot set link\n");
	    break;
	  case ERR_no_selection_in_document:
	    sprintf (buffer, "no selection in document\n");
	    break;
	  case ERR_incorrect_tree:
	    sprintf (buffer, "incorrect tree\n");
	    break;
	  case ERR_cannot_open_view:
	    sprintf (buffer, "cannot open view\n");
	    break;
	  case ERR_there_are_open_views:
	    sprintf (buffer, "there are open views\n");
	    break;
	  case ERR_cannot_load_pschema:
	    sprintf (buffer, "cannot load pschema\n");
	    break;
	  case ERR_duplicate_presentation_rule:
	    sprintf (buffer, "duplicate presentation rule\n");
	    break;
	  case ERR_string_too_long:
	    sprintf (buffer, "string too long\n");
	    break;
	  case ERR_cannot_holophrast_a_root:
	    sprintf (buffer, "cannot holophrast a root\n");
	    break;
	  case ERR_cannot_holophrast_that_type:
	    sprintf (buffer, "cannot holophrast that type\n");
	    break;
	  case ERR_main_window_not_open:
	    sprintf (buffer, "main window not open\n");
	    break;
	  case ERR_element_already_inserted:
	    sprintf (buffer, "Element already inserted\n");
	    break;
	  case ERR_no_presentation_schema:
	    sprintf (buffer, "No presentation schema\n");
	    break;
	  case ERR_element_has_no_box:
	    sprintf (buffer, "Element has no box\n");
	    break;
	  case ERR_invalid_X_connection:
	    sprintf (buffer, "cannot connect to X server\n");
	    break;
	  case ERR_cannot_open_main_window:
	    sprintf (buffer, "cannot open main window\n");
	    break;
	  case ERR_invalid_button:
	    sprintf (buffer, "invalid button\n");
	    break;
	  case ERR_too_many_languages:
	    sprintf (buffer, "Language table is full\n");
	    break;
	  case ERR_language_not_found:
	    sprintf (buffer, "Language not found\n");
	    break;
	  default:
	    sprintf (buffer, "%d\n", errorCode);
	    break;
	  }
#ifndef _WINGUI
	fprintf (stderr, buffer);
#endif /* _WINGUI */
     }
#endif /* THOT_DEBUG */
}
