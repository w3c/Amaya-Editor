
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern boolean TestListCar(char car, char *listcar);
extern boolean AUnNombre(char mot[MAX_CHAR]);
extern boolean EnRomain(char mot[MAX_CHAR]);
extern boolean minMAJ(char * chaine);
extern boolean EstUnNombre(char mot[MAX_CHAR]);
extern void majuscule ( char * chaine );
extern void capital ( char *chaine );
extern char tomin ( char caract );
extern boolean ismajuscule ( char * chaine );
extern boolean iscapital ( char * chaine );
extern boolean isiso ( char * chaine );
extern int existmot ( char mot[30], PtrDico dico );
extern int Corr_verifiermot ( char mot[30], Language langue, PtrDico dico );
extern void Corr_ajoutermot ( char mot[30], PtrDico *pDico );
extern void Corr_init_correction ( void );
extern void Corr_proposer ( Language langue, PtrDico dicodocu );
extern void param_defauts ( int lettres );
extern int Corr_Param ( void );
extern void Corr_remplacermot ( char motorigine[30], char motnouveau[30] );
extern boolean CorrChangeSelection ( void );
extern void Corr_errsuivant ( char mot[100], PtrDico dicodocu );
extern void sauverdicofichier ( PtrDico dicodocu );

#else /* __STDC__ */

extern boolean TestListCar(/* char car, char *listcar */);
extern boolean AUnNombre(/* char mot[MAX_CHAR] */);
extern boolean EnRomain(/* char mot[MAX_CHAR] */);
extern boolean minMAJ(/* char * chaine */);
extern boolean EstUnNombre(/* char mot[MAX_CHAR] */);
extern void majuscule (/* char * chaine */);
extern void capital (/* char *chaine */);
extern char tomin (/* char caract */);
extern boolean ismajuscule (/* char * chaine */);
extern boolean iscapital (/* char * chaine */);
extern boolean isiso (/* char * chaine */);
extern int existmot (/* char mot[30], PtrDico dico */);
extern int Corr_verifiermot (/* char mot[30], Language langue, PtrDico dico */);
extern void Corr_ajoutermot (/* char mot[30], PtrDico *pDico */);
extern void Corr_init_correction (/* void */);
extern void Corr_proposer (/* Language langue, PtrDico dicodocu */);
extern void param_defauts (/* int lettres */);
extern int Corr_Param (/* void */);
extern void Corr_remplacermot (/* char motorigine[30], char motnouveau[30] */);
extern boolean CorrChangeSelection (/* void */);
extern void Corr_errsuivant (/* char mot[100], PtrDico dicodocu */);
extern void sauverdicofichier (/* PtrDico dicodocu */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
