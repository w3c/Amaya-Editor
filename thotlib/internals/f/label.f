
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int NewLabel ( PtrDocument pDoc );
extern int GetCurrentLabel ( PtrDocument pDoc );
extern void SetCurrentLabel ( PtrDocument pDoc, int label );

#else /* __STDC__ */

extern int NewLabel (/* PtrDocument pDoc */);
extern int GetCurrentLabel (/* PtrDocument pDoc */);
extern void SetCurrentLabel (/* PtrDocument pDoc, int label */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
