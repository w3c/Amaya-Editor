/* -- Copyright (c) 1990 - 1996 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__
extern void print(Document document, View view);
extern void printsetup(Document document, View view);
extern void RetMenuImprimer(int ref, int val, char *txt);

#else /* __STDC__ */
extern void print(/*Document document, View view*/);
extern void printsetup(/*Document document, View view*/);
extern void RetMenuImprimer(/* int ref, int val, char *txt */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
