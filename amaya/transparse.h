#ifndef __TRANSPARSE_F__
#define __TRANSPARSE_F__

#ifdef __STDC__
extern void freelist (ListSymb* pl) ; 
extern int ppStartParser (char* name) ; 
#else  /* __STDC__ */ 
extern void freelist () ; 
extern int ppStartParser () ; 
#endif /* __STDC__ */

#endif /* __TRANSPARSE_F__ */
