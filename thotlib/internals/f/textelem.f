
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern PtrTextBuffer NewTextBuffer ( PtrTextBuffer pBuf );
extern void CreateTextBuffer ( PtrElement pEl );
extern void DeleteTextBuffer ( PtrTextBuffer *pBT );
extern void SplitTextElement ( PtrElement, int, PtrDocument, boolean);
extern void MergeTextElements ( PtrElement, PtrElement*, PtrDocument, boolean);
extern PtrTextBuffer CopyText ( PtrTextBuffer Srce, PtrElement pEl );
extern boolean StringAndTextEqual ( char *Chaine, PtrTextBuffer Texte );
extern boolean TextsEqual ( PtrTextBuffer Texte1, PtrTextBuffer Texte2 );
extern void CopyTextToText ( PtrTextBuffer Source, PtrTextBuffer Dest, int *LgCopiee );
extern void CopyStringToText ( char *Source, PtrTextBuffer Dest, int *LgCopiee );
extern void CopyTextToString ( PtrTextBuffer Source, char *Dest, int *Lg );
extern void ClearText ( PtrTextBuffer PremierBuffer );
extern void AddPointInPolyline ( PtrTextBuffer firstBuffer, int rank, int x, int y );
extern void DeletePointInPolyline ( PtrTextBuffer *firstBuffer, int rank );
extern void ModifyPointInPolyline ( PtrTextBuffer firstBuffer, int rank, int x, int y );

#else /* __STDC__ */

extern PtrTextBuffer NewTextBuffer (/* PtrTextBuffer pBuf */);
extern void CreateTextBuffer (/* PtrElement pEl */);
extern void DeleteTextBuffer (/* PtrTextBuffer *pBT */);
extern void SplitTextElement (/* PtrElement pEl, int lg1, PtrDocument pDoc, boolean WithAPPEvent */);
extern void MergeTextElements (/* PtrElement pEl, PtrElement *pLib, PtrDocument pDoc, boolean WithAPPEvent */);
extern PtrTextBuffer CopyText (/* PtrTextBuffer Srce, PtrElement pEl */);
extern boolean StringAndTextEqual (/* char *Chaine, PtrTextBuffer Texte */);
extern boolean TextsEqual (/* PtrTextBuffer Texte1, PtrTextBuffer Texte2 */);
extern void CopyTextToText (/* PtrTextBuffer Source, PtrTextBuffer Dest, int *LgCopiee */);
extern void CopyStringToText (/* char *Source, PtrTextBuffer Dest, int *LgCopiee */);
extern void CopyTextToString (/* PtrTextBuffer Source, char *Dest, int *Lg */);
extern void ClearText (/* PtrTextBuffer PremierBuffer */);
extern void AddPointInPolyline (/* PtrTextBuffer firstBuffer, int rank, int x, int y */);
extern void DeletePointInPolyline (/* PtrTextBuffer *firstBuffer, int rank */);
extern void ModifyPointInPolyline (/* PtrTextBuffer firstBuffer, int rank, int x, int y */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
