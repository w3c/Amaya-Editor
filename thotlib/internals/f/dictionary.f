
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern boolean Corr_alphabet ( void );
extern void asci2code ( char * chaine );
extern void code2asci ( char * chaine );
extern void Corr_pretraitement ( PtrDico dico );
extern boolean Corr_ReloadDico ( PtrDico *pDico );
extern void Dico_Init ( void );
extern boolean TtaLoadDocumentDictionary ( PtrDocument document, PtrDico *pDico, boolean ToCreate );
extern boolean TtaLoadLanguageDictionaries ( Language languageId );
extern void TtaUnLoadDictionary ( Language languageId );
extern Dictionary TtaGetPrincipalDictionary ( Language languageId );
extern Dictionary TtaGetSecondaryDictionary ( Language languageId );

#else /* __STDC__ */

extern boolean Corr_alphabet (/* void */);
extern void asci2code (/* char * chaine */);
extern void code2asci (/* char * chaine */);
extern void Corr_pretraitement (/* PtrDico dico */);
extern boolean Corr_ReloadDico (/* PtrDico *pDico */);
extern void Dico_Init (/* void */);
extern boolean TtaLoadDocumentDictionary (/* PtrDocument document, PtrDico *pDico, boolean ToCreate */);
extern boolean TtaLoadLanguageDictionaries (/* Language languageId */);
extern void TtaUnLoadDictionary (/* Language languageId */);
extern Dictionary TtaGetPrincipalDictionary (/* Language languageId */);
extern Dictionary TtaGetSecondaryDictionary (/* Language languageId */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
