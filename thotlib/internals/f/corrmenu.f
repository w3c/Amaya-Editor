
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void Corr_Init ( void );
extern void ActionCorriger ( int fenetre );
extern void Corr_Action ( int fenetre );
extern void Corr_SuppAction ( int fenetre );
extern void CallbackCorrector ( int Ref, int TypeData, char *Data );

#else /* __STDC__ */

extern void Corr_Init (/* void */);
extern void ActionCorriger (/* int fenetre */);
extern void Corr_Action (/* int fenetre */);
extern void Corr_SuppAction (/* int fenetre */);
extern void CallbackCorrector (/* int Ref, int TypeData, char *Data */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
