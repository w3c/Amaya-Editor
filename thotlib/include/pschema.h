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

extern PSchema      TtaNewPSchema ();
extern void         TtaRemovePSchema (PSchema schema, Document document);
extern void         TtaAddPSchema (PSchema schema, PSchema oldSchema, boolean before, Document document);
extern PSchema      TtaGetFirstPSchema (Document document);
extern void         TtaNextPSchema (PSchema * schema, Document document);

#else  /* __STDC__ */

extern PSchema      TtaNewPSchema ();
extern void         TtaRemovePSchema ( /* PSchema schema, Document document */ );
extern void         TtaAddPSchema ( /* PSchema schema, PSchema oldSchema, boolean before, Document document */ );
extern PSchema      TtaGetFirstPSchema ( /* Document document */ );
extern void         TtaNextPSchema ( /* PSchema *schema, Document document */ );

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
