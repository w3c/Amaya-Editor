
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void GDRRefCible ( Element CibleOrigin, Element CibleTrans, Document DocOrigin, Document DocTrans );
extern void GDRRefElem ( Element ElemRefOrigin, Element ElemRefTrans, Document DocOrigin, Document DocTrans );
extern void GDRRefAttr ( Attribute AttrRefOrigin, Attribute AttrRefTrans, Element ElemRefOrigin, Element ElemRefTrans, Document DocOrigin, Document DocTrans );
extern void GDRFindListeRef ( Element ElemRef, Attribute AttrRef, Element *ElemRefTrans, Attribute *AttrRefTrans );
extern void GDRFindListeCible ( Element ElemRef, Attribute AttrRef, Element *ElemCible, Document *DocCible );

#else /* __STDC__ */

extern void GDRRefCible (/* Element CibleOrigin, Element CibleTrans, Document DocOrigin, Document DocTrans */);
extern void GDRRefElem (/* Element ElemRefOrigin, Element ElemRefTrans, Document DocOrigin, Document DocTrans */);
extern void GDRRefAttr (/* Attribute AttrRefOrigin, Attribute AttrRefTrans, Element ElemRefOrigin, Element ElemRefTrans, Document DocOrigin, Document DocTrans */);
extern void GDRFindListeRef (/* Element ElemRef, Attribute AttrRef, Element *ElemRefTrans, Attribute *AttrRefTrans */);
extern void GDRFindListeCible (/* Element ElemRef, Attribute AttrRef, Element *ElemCible, Document *DocCible */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
