
/* -- Copyright (c) 1996 Inria  All rights reserved. -- */

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
