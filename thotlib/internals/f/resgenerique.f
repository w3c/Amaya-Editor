
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int GDRGetUnit(SSchema pSchema, Boolean PremiereBorne);
extern void GDRCreeArbreTypes(PtrSchemaResdyn pResdyn, int NumRegle, pTreeSch TyPere);
extern void GDRImpType(PtrSchemaResdyn pResdyn, pTreeSch pType, int Niv);
extern PtrSchemaResdyn GDRCalculResdyn ( PtrSchemaResdyn *pAllResdyn, SSchema pSchema);


#else /* __STDC__ */

extern int GDRGetUnit(/* SSchema pSchema, Boolean PremiereBorne */);
extern void GDRCreeArbreTypes(/*PtrSchemaResdyn pResdyn, int NumRegle, pTreeSch TyPere */);
extern void GDRImpType(/* PtrSchemaResdyn pResdyn, pTreeSch pType, int Niv */);
extern PtrSchemaResdyn GDRCalculResdyn (/* PtrSchemaResdyn *pAllResdyn, SSchema pSchema */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */


