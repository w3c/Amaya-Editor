/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/** typecorr.h **/
#ifndef _TYPECORR_H_
#define _TYPECORR_H_

#define NbLtr     120  /* nb max de lettres de l'alphabet < 256 isolatin1  */

 typedef char    FileName[MAX_NAME_LENGTH];    /* nom de fichier */

 typedef  char *PtrChaine;   /* pointeur sur la chaine des mots du dico */
 typedef  char *PtrCommuns;  /* pointeur sur la liste des nb lettres communes */
 typedef int *PtrMots;   /* pointeur sur la liste des pointeurs sur les mots */

 typedef struct _Dictionnaire *PtrDict;

/* un dictionnaire */
typedef struct _Dictionnaire
{
      PtrDict     DictSuivant;        /* pour le chainage des blocs libres*/
      PtrDocument DictDoc;                  /* document utilisant ce dico */
      Name         DictNom;                         /* nom du dictionnaire */
      Buffer      DictDirectory;             /* directory du dictionnaire */
      boolean     DictReadOnly;          /* dictionnaire en lecture seule */
      boolean     DictCharge;                     /* dictionnaire charge' */
      boolean     DictModifie;          /* le dictionnaire a ete modifie' */
      Language    DictLangue;                /* la langue du dictionnaire */
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
