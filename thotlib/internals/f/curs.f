
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern ThotCursor ThotCreateThotCursor ( unsigned int name, ThotColorStruct *fgcol, ThotColorStruct *bgcol, int disp );
extern void InitCurs ( void );

#else /* __STDC__ */

extern ThotCursor ThotCreateThotCursor (/* unsigned int name, ThotColorStruct *fgcol, ThotColorStruct *bgcol, int disp */);
extern void InitCurs (/* void */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
