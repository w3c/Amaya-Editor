/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void LibBufEditeur ( void );
extern void CmdCopy ( void );
extern void CmdPaste ( void );
extern void CmdCut ( boolean sauve );
extern void CmdSplit (/* void */);
extern void CmdSurround (/* void */);
extern void CmdChangeType (/* void */);
extern void Carriage ( void );
extern void CreateNewElement ( int NumTypeACreer, PtrSSchema SchTypeACreer, PtrDocument pDoc, boolean Before );
extern void RetMenuSurround ( int entree );
extern void RetMenuChangeType ( int entree );
extern void DeleteNextChar ( PtrElement pEl, boolean Before );

#else /* __STDC__ */

extern void LibBufEditeur (/* void */);
extern void CmdCopy (/* void */);
extern void CmdPaste (/* void */);
extern void CmdCut (/* boolean sauve */);
extern void CmdSplit (/* void */);
extern void CmdSurround (/* void */);
extern void CmdChangeType (/* void */);
extern void Carriage (/* void */);
extern void CreateNewElement (/* int NumTypeACreer, PtrSSchema SchTypeACreer, PtrDocument pDoc, boolean Before */);
extern void RetMenuSurround (/* int entree */);
extern void RetMenuChangeType (/* int entree */);
extern void mergeprevious (/* Document document, View view */);
extern void DeleteNextChar (/* PtrElement pEl, boolean Before */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
