
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void Conv_Cible ( Element CibleOrigin, Element CibleTrans, Document DocOrigin, Document DocTrans );
extern void Conv_Elem_Ref ( Element ElemRefOrigin, Element ElemRefTransf, Document DocOrigin, Document DocTrans );
extern void Conv_Attr_Ref ( Attribute AttrRefOrigin, Attribute AttrRefTransf, Element ElemRefOrigin, Element ElemRefTransf, Document DocOrigin, Document DocTrans );

#else /* __STDC__ */

extern void Conv_Cible (/* Element CibleOrigin, Element CibleTrans, Document DocOrigin, Document DocTrans */);
extern void Conv_Elem_Ref (/* Element ElemRefOrigin, Element ElemRefTransf, Document DocOrigin, Document DocTrans */);
extern void Conv_Attr_Ref (/* Attribute AttrRefOrigin, Attribute AttrRefTransf, Element ElemRefOrigin, Element ElemRefTransf, Document DocOrigin, Document DocTrans */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
