
/* -- Copyright (c) 1990 - 1994 Inria/Imag  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void retconfirm ( int ref, int typedata, char *data );
extern void retcreatedoc ( int ref, int typedata, char *data );
extern void TtcCreateDocument ( Document document, View view );

#else /* __STDC__ */

extern void retconfirm (/* int ref, int typedata, char *data */);
extern void retcreatedoc (/* int ref, int typedata, char *data */);
extern void TtcCreateDocument (/* Document document, View view */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
