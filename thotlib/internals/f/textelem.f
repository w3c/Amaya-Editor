
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern PtrTextBuffer NewBufTexte ( PtrTextBuffer pBuf );
extern void CreeBufTexte ( PtrElement pEl );
extern void SuppBufTexte ( PtrTextBuffer *pBT );
extern void CoupeTexte ( PtrElement, int, PtrDocument, boolean);
extern void FondTexte ( PtrElement, PtrElement*, PtrDocument, boolean);
extern PtrTextBuffer CopieTexte ( PtrTextBuffer Srce, PtrElement pEl );
extern boolean ChaineEtTexteEgaux ( char *Chaine, PtrTextBuffer Texte );
extern boolean TextesEgaux ( PtrTextBuffer Texte1, PtrTextBuffer Texte2 );
extern void CopieTexteDansTexte ( PtrTextBuffer Source, PtrTextBuffer Dest, int *LgCopiee );
extern void CopieChaineDansTexte ( char *Source, PtrTextBuffer Dest, int *LgCopiee );
extern void CopieTexteDansChaine ( PtrTextBuffer Source, char *Dest, int *Lg );
extern void VideTexte ( PtrTextBuffer PremierBuffer );
extern void AddPointInPolyline ( PtrTextBuffer firstBuffer, int rank, int x, int y );
extern void DeletePointInPolyline ( PtrTextBuffer *firstBuffer, int rank );
extern void ModifyPointInPolyline ( PtrTextBuffer firstBuffer, int rank, int x, int y );

#else /* __STDC__ */

extern PtrTextBuffer NewBufTexte (/* PtrTextBuffer pBuf */);
extern void CreeBufTexte (/* PtrElement pEl */);
extern void SuppBufTexte (/* PtrTextBuffer *pBT */);
extern void CoupeTexte (/* PtrElement pEl, int lg1, PtrDocument pDoc, boolean WithAPPEvent */);
extern void FondTexte (/* PtrElement pEl, PtrElement *pLib, PtrDocument pDoc, boolean WithAPPEvent */);
extern PtrTextBuffer CopieTexte (/* PtrTextBuffer Srce, PtrElement pEl */);
extern boolean ChaineEtTexteEgaux (/* char *Chaine, PtrTextBuffer Texte */);
extern boolean TextesEgaux (/* PtrTextBuffer Texte1, PtrTextBuffer Texte2 */);
extern void CopieTexteDansTexte (/* PtrTextBuffer Source, PtrTextBuffer Dest, int *LgCopiee */);
extern void CopieChaineDansTexte (/* char *Source, PtrTextBuffer Dest, int *LgCopiee */);
extern void CopieTexteDansChaine (/* PtrTextBuffer Source, char *Dest, int *Lg */);
extern void VideTexte (/* PtrTextBuffer PremierBuffer */);
extern void AddPointInPolyline (/* PtrTextBuffer firstBuffer, int rank, int x, int y */);
extern void DeletePointInPolyline (/* PtrTextBuffer *firstBuffer, int rank */);
extern void ModifyPointInPolyline (/* PtrTextBuffer firstBuffer, int rank, int x, int y */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
