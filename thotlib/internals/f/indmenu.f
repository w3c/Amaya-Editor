
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void Ind_Init ( void );
extern void Ind_Action ( int fenetre );
extern void Ind_SuppAction ( int fenetre );
extern void CallbackIndex ( int Ref, int TypeData, char *Data );
extern void Indexer ( PtrDocument pDoc );

#else /* __STDC__ */

extern void Ind_Init (/* void */);
extern void Ind_Action (/* int fenetre */);
extern void Ind_SuppAction (/* int fenetre */);
extern void CallbackIndex (/* int Ref, int TypeData, char *Data */);
extern void Indexer (/* PtrDocument pDoc */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
