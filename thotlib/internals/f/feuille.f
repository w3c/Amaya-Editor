
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern BOOL EstFeuille ( Element element );
extern BOOL EstFeuilleText ( Element element );
extern BOOL EstSautPage ( Element element );
extern BOOL EstPolyline ( Element element );
extern void NextSiblingText ( Element *Sibling );
extern void PreviousSiblingText ( Element *Sibling );
extern void NextSiblingNoText ( Element *Sibling );
extern Element GetFirstSigChild ( Element element, BOOL *Flag );
extern void NextSigSibling ( Element *element, BOOL *Flag );
extern void ptdarret ( void );

#else /* __STDC__ */

extern BOOL EstFeuille (/* Element element */);
extern BOOL EstFeuilleText (/* Element element */);
extern BOOL EstSautPage (/* Element element */);
extern BOOL EstPolyline (/* Element element */);
extern void NextSiblingText (/* Element *Sibling */);
extern void PreviousSiblingText (/* Element *Sibling */);
extern void NextSiblingNoText (/* Element *Sibling */);
extern Element GetFirstSigChild (/* Element element, BOOL *Flag */);
extern void NextSigSibling (/* Element *element, BOOL *Flag */);
extern void ptdarret (/* void */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
