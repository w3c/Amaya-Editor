
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void WriteVersionNumber ( BinFile fich );
extern void PutShort ( BinFile fich, int n );
extern void PutUnite ( BinFile fich, boolean b );
extern void EcritNat ( PtrSSchema pSS, BinFile fich, PtrDocument pDoc );
extern void PutLabel ( BinFile fich, LabelString Lab );
extern void PutAttribut ( BinFile fichpiv, PtrAttribute pAttr, PtrDocument pDoc );
extern void PutReglePres ( BinFile fichpiv, PtrPRule pRegle );
extern void Externalise ( BinFile fichpiv, PtrElement *El, PtrDocument pDoc, boolean AvecDescendants );
extern void BuildDocNatureTable ( PtrDocument pDoc );
extern void WriteTableLangues ( BinFile fich, PtrDocument pDoc );
extern void WritePivotHeader ( BinFile fich, PtrDocument pDoc );
extern void WriteNomsSchemasDoc(BinFile fich, PtrDocument pDoc);
extern void SauveDoc ( BinFile fich, PtrDocument pDoc);
extern void SauveRefSortantes ( char *NomFich, PtrDocument pDoc );
extern void SauveRef ( PtrChangedReferredEl PremChng, PathBuffer NomFich );
extern void SauveExt ( PtrReferredDescr PremElemRef, PathBuffer NomFich );
extern void UpdateExt ( PtrDocument pDoc );
extern void UpdateRef ( PtrDocument pDoc );
extern void ChangeNomExt ( PtrDocument pDoc, Name NouveauNom, boolean CopieDoc );
extern void ChangeNomRef ( PtrDocument pDoc, Name NouveauNom );

#else /* __STDC__ */

extern void WriteVersionNumber (/* BinFile fich */);
extern void PutShort (/* BinFile fich, int n */);
extern void PutUnite (/* BinFile fich, boolean b */);
extern void EcritNat (/* PtrSSchema pSS, BinFile fich, PtrDocument pDoc */);
extern void PutLabel (/* BinFile fich, LabelString Lab */);
extern void PutAttribut (/* BinFile fichpiv, PtrAttribute pAttr, PtrDocument pDoc */);
extern void PutReglePres (/* BinFile fichpiv, PtrPRule pRegle */);
extern void Externalise (/* BinFile fichpiv, PtrElement *El, PtrDocument pDoc, boolean AvecDescendants */);
extern void BuildDocNatureTable (/* PtrDocument pDoc */);
extern void WriteTableLangues (/* BinFile fich, PtrDocument pDoc */);
extern void WritePivotHeader (/* BinFile fich, PtrDocument pDoc */);
extern void WriteNomsSchemasDoc(/*BinFile fich, PtrDocument pDoc*/);
extern void SauveDoc (/* BinFile fich, PtrDocument pDoc */);
extern void SauveRefSortantes (/* char *NomFich, PtrDocument pDoc */);
extern void SauveRef (/* PtrChangedReferredEl PremChng, PathBuffer NomFich */);
extern void SauveExt (/* PtrReferredDescr PremElemRef, PathBuffer NomFich */);
extern void UpdateExt (/* PtrDocument pDoc */);
extern void UpdateRef (/* PtrDocument pDoc */);
extern void ChangeNomExt (/* PtrDocument pDoc, Name NouveauNom, boolean CopieDoc */);
extern void ChangeNomRef (/* PtrDocument pDoc, Name NouveauNom */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
