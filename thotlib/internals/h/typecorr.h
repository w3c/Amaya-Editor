/** typecorr.h **/
#ifndef _TYPECORR_H_
#define _TYPECORR_H_

#define NbLtr     120  /* nb max de lettres de l'alphabet < 256 isolatin1  */

 typedef char    FileName[MAX_NAME_LENGTH];    /* nom de fichier */

 typedef  char *PtrChaine;   /* pointeur sur la chaine des mots du dico */
 typedef  char *PtrCommuns;  /* pointeur sur la liste des nb lettres communes */
 typedef int *PtrMots;   /* pointeur sur la liste des pointeurs sur les mots */

 typedef struct _Dictionnaire *PtrDico;

/* un dictionnaire */
typedef struct _Dictionnaire
{
      PtrDico     DicoSuivant;        /* pour le chainage des blocs libres*/
      PtrDocument DicoDoc;                  /* document utilisant ce dico */
      Name         DicoNom;                         /* nom du dictionnaire */
      Buffer      DicoDirectory;             /* directory du dictionnaire */
      boolean     DicoReadOnly;          /* dictionnaire en lecture seule */
      boolean     DicoCharge;                     /* dictionnaire charge' */
      boolean     DicoModifie;          /* le dictionnaire a ete modifie' */
      Language    DicoLangue;                /* la langue du dictionnaire */
       int MAXcars;       /* max caracteres pour chaine et commun */
       int MAXmots;       /* max entiers alloues pour pdico */
      PtrChaine   chaine;       /* ptr sur les caracteres du dictionnaire */
      PtrMots     pdico;                /* ptr sur la liste des pointeurs */
                                       /*    sur les mots du dictionnaire */
      PtrCommuns  commun;     /* ptr sur la liste des nb lettres communes */
      int         plgdico[MAX_WORD_LEN];     /* ptr sur les mots de meme taille */
      int         nbmots;     /* nb effectif de mots dans le dictionnaire */
      int         nbcars;                  /* nb car effectif dans chaine */

}    Dictionnaire;
 
#endif
