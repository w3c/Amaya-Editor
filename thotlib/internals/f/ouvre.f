
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern boolean OuvreDoc ( Name, PtrDocument, boolean, boolean, PtrSSchema, boolean);
extern PtrDocument pDocument ( DocumentIdentifier IdentDocu );
extern void SupprDoc ( PtrDocument pDoc );

#else /* __STDC__ */

extern boolean OuvreDoc (/* Name NomDoc, PtrDocument pDoc, boolean ChargeDocExt, boolean Squelette, PtrSSchema pSCharge, boolean avecMsgAPP */);
extern PtrDocument pDocument (/* DocumentIdentifier IdentDocu */);
extern void SupprDoc (/* PtrDocument pDoc */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
