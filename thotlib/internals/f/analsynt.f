
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void initsynt ( void );
extern int trnb ( iline index, iline lg );
extern void transchar ( void );
extern void getword ( iline start, iline *wi, iline *wl, nature *wn );
extern void analword ( iline wi, iline wl, nature wn, grmcode *c, rnb *r, int *nb, rnb *pr );
extern void termsynt ( void );
extern void initgrm ( gname fn );

#else /* __STDC__ */

extern void initsynt (/* void */);
extern int trnb (/* iline index, iline lg */);
extern void transchar (/* void */);
extern void getword (/* iline start, iline *wi, iline *wl, nature *wn */);
extern void analword (/* iline wi, iline wl, nature wn, grmcode *c, rnb *r, int *nb, rnb *pr */);
extern void termsynt (/* void */);
extern void initgrm (/* gname fn */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
