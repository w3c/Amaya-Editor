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
 
#ifndef _PSCHEMA_H_
#define _PSCHEMA_H_

/* presentation schema */
typedef int        *PSchema;

#include "typebase.h"
#include "document.h"

#ifndef __CEXTRACT__
#ifdef __STDC__


/*----------------------------------------------------------------------
   TtaNewPSchema

   Creates a new (empty) presentation schema.

   Parameters:
   No parameter

   Return value:
   the new presentation schema.
  ----------------------------------------------------------------------*/
extern PSchema      TtaNewPSchema ();
/*----------------------------------------------------------------------
   TtaUnlinkPSchema

   Unlinks a presentation schema from a document or a nature in a 
   document.

   Parameters:
   schema: the presentation schema to be unlinked.
   document: the document to which that presentation schema is related.
   nature: the structure schema of a nature of the document, NULL if schema
           is relative to the main structure schema of the document.
  ----------------------------------------------------------------------*/
extern void         TtaUnlinkPSchema (PSchema schema, Document document, SSchema nature);

/*----------------------------------------------------------------------
   TtaRemovePSchema

   Removes a presentation schema from a document and destroys that schema
   if it is not used by other documents.

   Parameters:
   schema: the presentation schema to be deleted.
   document: the document to which that presentation schema is related.

  ----------------------------------------------------------------------*/
extern void         TtaRemovePSchema (PSchema schema, Document document, SSchema nature);

/*----------------------------------------------------------------------
   TtaAddPSchema

   Associates an additional presentation schema with a document. All additional
   presentation schemas for a given document are stored in an ordered list, in
   increasing priority order. TtaAddPSchema inserts the new schema in that list
   immediately before (or after, according to parameter "before") another schema
   which is already part of the list (parameter "oldSchema").

   Parameters:
   schema: the presentation schema to be added
   oldSchema: a presentation schema that is already associated with the
   document. NULL when adding the first additional presentation schema
   for the document.
   before: if TRUE, the new presentation schema is inserted just before
   oldSchema, else it is inserted just after. Meaningless if
   oldSchema is NULL.
   document: the document to which the presentation schema is added.

  ----------------------------------------------------------------------*/
extern void         TtaAddPSchema (PSchema schema, PSchema oldSchema, ThotBool before, Document document, SSchema nature);

/*----------------------------------------------------------------------
   TtaGetFirstPSchema

   Returns the first (i.e. lowest priority) additional presentation schema
   related with a document.
   Returns NULL if no additional presentation schema is related with the document.

   Parameter:
   document: the document of interest.

  ----------------------------------------------------------------------*/
extern PSchema      TtaGetFirstPSchema (Document document, SSchema nature);

/*----------------------------------------------------------------------
   TtaNextPSchema

   Gets the next additional presentation schema, in the increasing order of
   priority.

   Parameter:
   schema: the schema whose successor is asked.
   document: the document of interest.

   Return parameter:
   schema: the next schema, or NULL if there is no next schema.

  ----------------------------------------------------------------------*/
extern void         TtaNextPSchema ( /*INOUT*/ PSchema * schema, Document document, SSchema nature);

#else  /* __STDC__ */

extern PSchema      TtaNewPSchema ();
extern void         TtaUnlinkPSchema (/* PSchema schema, Document document, SSchema nature */);
extern void         TtaRemovePSchema ( /* PSchema schema, Document document */ );
extern void         TtaAddPSchema ( /* PSchema schema, PSchema oldSchema, ThotBool before, Document document */ );
extern PSchema      TtaGetFirstPSchema ( /* Document document */ );
extern void         TtaNextPSchema ( /* PSchema *schema, Document document */ );

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
