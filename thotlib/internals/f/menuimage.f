
/* -- Copyright (c) 1990 - 1994 Inria/Imag  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void RetMenuImage ( int ref, int val, char *txt );
extern int MenuImage ( char *nom, boolean result, int *typim, int *pres, PtrBox ibox );
extern void ImageMenuLoadResources ( void );

#else /* __STDC__ */

extern void RetMenuImage (/* int ref, int val, char *txt */);
extern int MenuImage (/* char *nom, boolean result, int *typim, int *pres, PtrBox ibox */);
extern void ImageMenuLoadResources (/* void */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
