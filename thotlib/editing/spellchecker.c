/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */
 
/*
 * Spell checking
 *
 * Author: H. Richy (INRISA)
 *
 */
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "corrmsg.h"
#include "message.h"
#include "language.h"
#include "constmot.h"
#include "constcorr.h"
#include "typecorr.h"
#include "fileaccess.h"

/* ----- LES VARIABLES ----- */
/* les variables importes des autres modules du correcteur */
#undef EXPORT
#define EXPORT extern
#include "spell_tv.h"
#undef EXPORT
#define EXPORT
#include "word_tv.h"

extern unsigned char Code[256];

/* -----------* LES VARIABLES locales de calcul.c *------------------------- */

static boolean      Clavier_charge;

#define NbLtr     120		/* nb max de lettres de l'alphabet < 256 isolatin1  */
static int          Tsub[NbLtr][NbLtr];
static int          KI = 3,	/* insertion */
                    KO = 3,	/* omission */
                    KB = 0,	/* bonne substition */
                    KM = 1,	/* substitution moyenne */
                    KD = 3,	/* substitution par defaut */
                    KP = 3;	/* permutation */
static int          Seuil[MAX_WORD_LEN];
static int          Delta[MAX_WORD_LEN];
static int          listscore[MAX_PROPOSAL_CHKR + 1];	/* Tab des scores des correction */
static int          listadrmot[MAX_PROPOSAL_CHKR + 1];	/* Tab des numeros de mots errones */
static PtrDict      listdico[MAX_PROPOSAL_CHKR + 1];
static int          type_err;	/* Type du mot errone (majuscule, SmallLettering) */

/* ----- LES PROCEDURES ----- */
#include "word_f.h"
#include "hyphen_f.h"

#include "structselect_f.h"
#include "search_f.h"
#include "dictionary_f.h"
#include "spellchecker_f.h"
#include "registry_f.h"
#include "exceptions_f.h"
#include "fileaccess_f.h"

/* procedures locales au module */
static int          inserermot ();
static int          inserer ();
static void         Cmp ();
static void         charger_corr ();


/***************************** minMAJ *********************************/
/*    retourne TRUE si la chaine ne contient pas que des minuscules */
#ifdef __STDC__
boolean             minMAJ (char *chaine)
#else  /* __STDC__ */
boolean             minMAJ (chaine)
char               *chaine;

#endif /* __STDC__ */

{
   int                 maj = 0;
   int                 i = 0;
   char                c;

   while ((c = chaine[i]) != '\0' && (maj == 0))
     {
	maj = isimaj (chaine[i]);
	i++;
     }
   return (maj == 1) ? TRUE : FALSE;
}

/***************************** majuscule *********************************/
#ifdef __STDC__
void                majuscule (char *chaine)

#else  /* __STDC__ */
void                majuscule (chaine)
char               *chaine;

#endif /* __STDC__ */

{
   int                 i = 0;

   while ((chaine[i]) != '\0')
     {
	if (isimin (chaine[i]) != 0)
	   chaine[i] = toupper (chaine[i]);

	i++;
     }
}

/***************************** capital *********************************/

#ifdef __STDC__
void                capital (char *chaine)

#else  /* __STDC__ */
void                capital (chaine)
char               *chaine;

#endif /* __STDC__ */

{
   if (isimin (chaine[0]) != 0)
      chaine[0] = toupper (chaine[0]);

}

/***************************** tomin *********************************/

#ifdef __STDC__
char                tomin (char caract)

#else  /* __STDC__ */
char                tomin (caract)
char                caract;

#endif /* __STDC__ */

{
   char                c;

   c = caract;
   if (isimaj (caract) != 0)
      c = tolower (caract);

   return (c);
}

/***************************** ismajuscule *********************************/

#ifdef __STDC__
boolean             ismajuscule (char *chaine)

#else  /* __STDC__ */
boolean             ismajuscule (chaine)
char               *chaine;

#endif /* __STDC__ */

{
   int                 maj = 1;
   int                 i = 0;
   char                c;

   while ((c = chaine[i]) != '\0' && (maj != 0))
     {
	maj = isimaj (chaine[i]);
	i++;
     }
   return (maj == 0) ? FALSE : TRUE;
}

/***************************** iscapital *********************************/

#ifdef __STDC__
boolean             iscapital (char *chaine)

#else  /* __STDC__ */
boolean             iscapital (chaine)
char               *chaine;

#endif /* __STDC__ */

{
   int                 cap = 0;
   int                 i = 0;
   char                c;

   if (chaine[0] != '\0' && isimaj (chaine[0]) != 0)
     {
	cap = 1;
	i++;
	while ((c = chaine[i]) != '\0' && (cap != 0))
	  {
	     cap = !(isimaj (chaine[i]));
	     i++;
	  }
     }
   return (cap == 0) ? FALSE : TRUE;
}

/***************************** isiso *********************************/

#ifdef __STDC__
boolean             isiso (char *chaine)

#else  /* __STDC__ */
boolean             isiso (chaine)
char               *chaine;

#endif /* __STDC__ */

{
   int                 iso = 1;
   int                 i = 0;

   while (chaine[i] != '\0' && (iso != 0))
     {
	iso = isalphiso (chaine[i]) || chaine[i] == '-' || chaine[i] == '\'';
	i++;
     }
   return (iso == 0) ? FALSE : TRUE;
}				/* end of isiso */


/*----------------------------------------------------------------------
   existmot                                   
------------* Recherche dichotomique de "mot" dans "dico[]" *------------
   retourne -2 si le dico n'existe pas                                     
   retourne -1 si le dico est vide                                         
   retourne -3  si le mot est present dans le dico                          
   retourne >=0 si le mot n'est pas dans le dico (indice de sa place future) 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 existmot (char mot[MAX_WORD_LEN], PtrDict dico)

#else  /* __STDC__ */
int                 existmot (mot, dico)
char                mot[MAX_WORD_LEN];
PtrDict             dico;

#endif /* __STDC__ */

{
   int                 inf, sup, med, rescomp, taille;

   if (dico == NULL)
      return (-2);		/* dictionnaire n'existe pas */
   else if (dico->nbmots < 0)	/* dictionnaire vide */
      return (-1);

   taille = strlen (mot);
   inf = dico->plgdico[taille];	/* premier mot */
   if (taille >= MAX_WORD_LEN || (dico->plgdico[taille + 1] - 1 > dico->nbmots))
      sup = dico->nbmots;
   else
      sup = dico->plgdico[taille + 1] - 1;	/* dernier mot */

   while (sup >= inf)		/* Recherche dichotomique */
     {
	med = (sup + inf) / 2;
	rescomp = strcmp (&dico->chaine[dico->pdico[med]], mot);
	if (rescomp == 0)
	   return (-3);		/* Mot present dans le dico */
	else if (rescomp > 0)
	   sup = med - 1;	/* Poursuite de la recherche */
	else
	   inf = med + 1;
     }
   return (inf);		/* Mot absent dans le dico */
}


/*----------------------------------------------------------------------
   Corr_verifiermot retourne                                       
   1 si le mot appartient a un dico                        
   ou est de longueur 1 ou 2                             
   0 si le mot est inconnu ou vide                         
   -1 s'il n'y a pas de dictionnaire pour verifier ce mot   
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 Corr_verifiermot (char mot[MAX_WORD_LEN], Language langue, PtrDict dico)

#else  /* __STDC__ */
int                 Corr_verifiermot (mot, langue, dico)
char                mot[MAX_WORD_LEN];
Language            langue;
PtrDict             dico;

#endif /* __STDC__ */

{
   boolean             present = TRUE;
   int                 res;
   char                mot1[MAX_WORD_LEN];
   char                motmin[MAX_WORD_LEN];
   PtrDict             dicogene;
   PtrDict             dicoperso;
   PtrDict             dicosigle;
   PtrDict             diconame;

   /* On refuse de corriger l'ISOlatin-1 */
   if (langue == 0)
      return (-1);

   if (mot[0] == '\0')
      return (0);		/* mot vide */

   if (strlen (mot) >= 2)
     {
	strcpy (mot1, mot);
	SmallLettering (mot1);
	strcpy (motmin, mot1);
	asci2code (mot1);

	dicogene = (PtrDict) TtaGetPrincipalDictionary (langue);
	dicoperso = (PtrDict) TtaGetSecondaryDictionary (langue);
	res = existmot (mot1, dico);

	if (res != -3 || res == -2)
	   /* mot absent de dico ou dico vide ou pas de dico */
	  {
	     res = existmot (mot1, dicogene);
	     if (res == -2)
		return (-1);	/* PAS de dicogene */
	     else
	       {
		  if (res != -3)	/* mot absent de dicogene */
		    {		/* mot1 n'appartient pas a ces 2 dicos obligatoires */
		       present = FALSE;
		       dicosigle = (PtrDict) TtaGetPrincipalDictionary (0);
		       diconame = (PtrDict) TtaGetSecondaryDictionary (0);
		       if (existmot (mot1, dicoperso) == -3
			   || existmot (mot1, dicosigle) == -3
			   || existmot (mot1, diconame) == -3)
			  /* mot1 appartient a l'un de ces dico facultatifs */
			  present = TRUE;
		    }
	       }
	  }

	if (present == FALSE)
	  {			/* calculer le type du mot errone' */
	     if (ismajuscule (mot) == TRUE)
		type_err = 3;	/* MAJ */
	     else
	       {
		  if (iscapital (mot) == TRUE)
		     type_err = 2;	/* Capitale */
		  else
		     type_err = 1;	/* SmallLettering ou melange */
	       }
	     /* recopier ce mot errone' dans ChkrErrWord */
	     strcpy (ChkrErrWord, mot);
	  }
     }
   res = (present == TRUE) ? 1 : 0;
   return (res);
}				/*Corr_verifiermot */

/*----------------------------------------------------------------------
   Corr_ajoutermot                                                 
   ajoute un mot nouveau dans le dictionnaire dicodocu          
   apres avoir verifie qu'il n'y etait pas deja                 
   (s'il n'y a plus de place : ferme et ouvre avec plus d'espace) 
   creation eventuelle et initialisation de ChkrFileDict             
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                Corr_ajoutermot (char mot[MAX_WORD_LEN], PtrDict * pDict)

#else  /* __STDC__ */
void                Corr_ajoutermot (mot, pDict)
char                mot[MAX_WORD_LEN];
PtrDict            *pDict;

#endif /* __STDC__ */

{
   char                mot1[MAX_WORD_LEN];
   int                 ret;
   Name                DiNom;
   boolean             OKinsere = TRUE;
   PtrDict             dicodocu;

   dicodocu = *pDict;
   strcpy (mot1, mot);
   /* verifier que ce mot est bien en caracteres iso */
   if (isiso (mot1))
     {
	/* ajout d'un mot dans le dictionnaire dicodocu */
	SmallLettering (mot1);
	asci2code (mot1);
	ret = inserermot (dicodocu, mot1);
	switch (ret)
	      {
		 case -2:
		    /* le dicodocu est encore vide : le creer */
		    if (TtaLoadDocumentDictionary (ChkrRange->SDocument, (int *) pDict, TRUE))
		      {
			 dicodocu = *pDict;
			 /* le dictionnaire du document est maintenant charge' */
			 ret = inserermot (dicodocu, mot1);
			 Corr_pretraitement (dicodocu);
		      }
		    else
		      {
			 /* impossible d'ajouter dans le dictionnaire du document ??? */
			 TtaDisplayMessage (INFO, TtaGetMessage (CORR, REFUSE_WORD), mot);
			 OKinsere = FALSE;
		      }
		    break;
		 case -1:
		    /* dictionnaire plein */
		    sauverdicofichier (dicodocu);
		    /* rechargement du dictionnaire avec plus de memoire */
		    strcpy (DiNom, dicodocu->DictNom);
		    if (Corr_ReloadDict (pDict) == TRUE)
		      {
			 dicodocu = *pDict;
			 if (inserermot (dicodocu, mot1) > 0)
			    Corr_pretraitement (dicodocu);
		      }
		    else
		       OKinsere = FALSE;
		    break;
		 case 0:
		    /* le mot etait deja dans le dictionnaire */
		    /* ne rien faire */
		    OKinsere = FALSE;
		    break;
		 case 1:
		    /* mot ajoute' dans le dictionnaire */
		    Corr_pretraitement (dicodocu);
		    break;
	      }
	if (OKinsere == TRUE)
	  {
	     /* avertir l'utilisateur de la reussite de l'ajout dans le dico */
	     TtaDisplayMessage (INFO, TtaGetMessage (CORR, ADD_WORD), mot);
	     /*  enregistrer le dictionnaire du document apres chaque mise a jour */
	     sauverdicofichier (dicodocu);
	  }			/* end of if */
     }
   else				/* car. incorrect, ajout du mot refus\351 */
      TtaDisplayMessage (INFO, TtaGetMessage (CORR, REFUSE_WORD), mot);
}				/* end of Corr_ajoutermot */


/********************** Corr_init_correction *********************************/

#ifdef __STDC__
void                Corr_init_correction ()

#else  /* __STDC__ */
void                Corr_init_correction ()
#endif				/* __STDC__ */

{
   int                 j;

   /* Initialisation des scores */
   for (j = 0; j <= NC; j++)
     {
	listscore[j] = 1000;
	listadrmot[j] = -1;
	listdico[j] = NULL;
     }

   /* Initialisation de la correction */
   for (j = 0; j <= NC; j++)
      strcpy (ChkrCorrection[j], "$");
}				/*Corr_init_correction */



/*----------------------------------------------------------------------
   Corr_proposer          
   met dans ChkrCorrection les propositions de correction du mot       
   qui se trouve dans ChkrErrWord                                   
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                Corr_proposer (Language langue, PtrDict dicodocu)

#else  /* __STDC__ */
void                Corr_proposer (langue, dicodocu)
Language            langue;
PtrDict             dicodocu;

#endif /* __STDC__ */

{
   PtrDict             dicogene;
   PtrDict             dicoperso;
   PtrDict             dicosigle;
   PtrDict             diconame;

   /* ATTENTION : ChkrErrWord contient le mot a corriger */
   Corr_init_correction ();	/* raz des scores */

   dicogene = (PtrDict) TtaGetPrincipalDictionary (langue);
   dicoperso = (PtrDict) TtaGetSecondaryDictionary (langue);
   dicosigle = (PtrDict) TtaGetPrincipalDictionary (0);
   diconame = (PtrDict) TtaGetSecondaryDictionary (0);

   /* on suppose que ChkrErrWord contient la description du mot errone' */
   Cmp (ChkrErrWord, dicogene);

   /* calcul local avec le dictionnaire personnel s'il existe */
   if (dicoperso != NULL)
      Cmp (ChkrErrWord, dicoperso);

   /* calcul local avec le dictionnaire du document s'il n'est pas vide */
   if (dicodocu != NULL)
      Cmp (ChkrErrWord, dicodocu);

   /* calcul local avec le dictionnaire de sigles s'il n'est pas vide */
   if (dicosigle != NULL)
      Cmp (ChkrErrWord, dicosigle);

   /* calcul local avec le dictionnaire de noms s'il n'est pas vide */
   if (diconame != NULL)
      Cmp (ChkrErrWord, diconame);

   charger_corr ();		/* remplissage de ChkrCorrection  */
}				/*Corr_proposer */


/******************************** init_Tsub *****************************/

#ifdef __STDC__
static void         init_Tsub (FILE * ftsub)

#else  /* __STDC__ */
static void         init_Tsub (ftsub)
FILE               *ftsub;

#endif /* __STDC__ */

{
   int                 i, j;
   unsigned char       ch1[80], ch2[80], ch3[80];

   /* initialisation de Tsub */
   for (i = 0; i < NbLtr; i++)
      for (j = 0; j < NbLtr; j++)
	 Tsub[i][j] = KD;
   while (fscanf (ftsub, "%s%s%s", ch1, ch2, ch3) != EOF)
     {
	int                 coeff;
	unsigned char       x, y, valeur;

	sscanf (ch1, "%c", &x);
	sscanf (ch2, "%c", &y);
	sscanf (ch3, "%c", &valeur);
	switch (valeur)
	      {
		 case 'b':
		    coeff = KB;
		    break;
		 case 'm':
		    coeff = KM;
		    break;
		 default:
		    coeff = KD;
		    break;
	      }
	Tsub[Code[x]][Code[y]] = coeff;
     }
   fclose (ftsub);
}				/*init_Tsub */

/*********************************** init_param ****************************/

#ifdef __STDC__
static void         init_param (FILE * fd)

#else  /* __STDC__ */
static void         init_param (fd)
FILE               *fd;

#endif /* __STDC__ */

{
   int                 i, cc, ii, oo, pp, bb, mm, dd, ss, rr;

/* lecture de la premiere ligne: 
   nbre de corrections, insertion, omission, permutation,
   bonne substitution, substitution moyenne, mauvaise substitution */

   fscanf (fd, "%d%d%d%d%d%d%d", &cc, &ii, &oo, &pp, &bb, &mm, &dd);
   if (cc > 0)
      NC = cc;
   else
      NC = 3;			/* valeur par defaut */
   if (ii > 0)
      KI = ii;
   if (oo > 0)
      KO = oo;
   if (pp > 0)
      KP = pp;
   if (bb > 0)
      KB = bb;
   if (mm > 0)
      KM = mm;
   if (dd > 0)
      KD = dd;
   /* lecture des lignes suivantes  */
   while (fscanf (fd, "%d%d%d", &i, &ss, &rr) != EOF)
     {
	if (i < MAX_WORD_LEN)
	  {
	     if (ss > 0)
		Seuil[i] = ss;
	     if (rr > 0)
		Delta[i] = rr;
	  }
	else
	   TtaDisplaySimpleMessage (INFO, CORR, NO_PARAM);
	/* erreur fichier parametre */
     }
}				/* end of init_param */

/********************************* param_defauts *****************************/

#ifdef __STDC__
void                param_defauts (int lettres)

#else  /* __STDC__ */
void                param_defauts (lettres)
int                 lettres;

#endif /* __STDC__ */

{
   int                 i;

   NC = 3;
   for (i = 1; i < MAX_WORD_LEN; i++)
     {
	Seuil[i] = KI + KI * (i - 1) / lettres;
	Delta[i] = 1 + ((i - 1) / lettres);
     }
}				/* end of param_defaults */


/*----------------------------------------------------------------------
   Corr_Param retourne  0 en cas d'impossibilite d'initialisation  
   1 si OK                                         
   positionne Clavier_charge                                   
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 Corr_Param ()

#else  /* __STDC__ */
int                 Corr_Param ()
#endif				/* __STDC__ */

{
   int                 ret;
   FILE               *fparam;
   Buffer              paramnom;
   FILE               *ftsub;
   Buffer              clavnom;
   char               *corrpath;

   ret = 1;
   /* initialisations des parametres du correcteur */
   if (Clavier_charge == FALSE)
     {
	/* remplir corrpath pour acces aux fichiers param et clavier */
	corrpath = TtaGetEnvString ("DICOPAR");
	if (corrpath == NULL)
	  {
	     /* pas de variable d'environnement DICOPAR */
	     TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_MISSING_DICOPAR), "DICOPAR");
	     ret = 0;
	  }
	else
	  {
	     /* Lecture du fichier parametres */
	     strcpy (paramnom, corrpath);
	     strcat (paramnom, "/param");
	     if ((fparam = fopen (paramnom, "r")) != NULL)	/* Existence du fichier */
		init_param (fparam);
	     else
	       {
		  /* valeur par defaut LGR = 4 */
		  param_defauts (4);
	       }

	     /* Lecture du  fichier clavier */
	     strcpy (clavnom, corrpath);
	     strcat (clavnom, "/clavier");
	     if ((ftsub = fopen (clavnom, "r")) != NULL)	/* Existence du fichier */
	       {
		  init_Tsub (ftsub);
		  Clavier_charge = TRUE;
	       }
	     else
	       {
		  TtaDisplaySimpleMessage (INFO, CORR, NO_KEYBOARD);
		  ret = 0;
	       }
	  }
     }
   return (ret);
}				/*Corr_Param */


/*----------------------------------------------------------------------
   Corr_remplacermot                                               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                Corr_remplacermot (char motorigine[MAX_WORD_LEN], char motnouveau[MAX_WORD_LEN])

#else  /* __STDC__ */
void                Corr_remplacermot (motorigine, motnouveau)
char                motorigine[MAX_WORD_LEN];
char                motnouveau[MAX_WORD_LEN];

#endif /* __STDC__ */

{
   int                 idx;
   int                 LgChaineCh;	/* longueur de cette chaine */
   char                pChaineRemplace[MAX_CHAR];	/* la chaine de remplacement */
   int                 LgChaineRempl;	/* longueur de cette chaine */

   /* remplacer le mot errone par le mot corrige */
   LgChaineCh = strlen (motorigine);
   /* initialiser LgChaineRempl et pChaineRemplace */
   LgChaineRempl = strlen (motnouveau);
   strcpy (pChaineRemplace, motnouveau);

   /* substitue la nouvelle chaine et la selectionne */
   if (ChkrRange->SStartToEnd)
     {
	idx = ChkrIndChar - LgChaineCh + 1;
	ReplaceString (ChkrRange->SDocument, ChkrElement, idx,
		       LgChaineCh, pChaineRemplace, LgChaineRempl, TRUE);
	/* met a jour ChkrIndChar */
	ChkrIndChar = idx + LgChaineRempl - 1;

	/* met eventuellement a jour la borne de fin du domaine de recherche */
	if (ChkrElement == ChkrRange->SEndElement)
	   /* la borne est dans l'element ou` on a fait le remplacement */
	   if (ChkrRange->SEndChar != 0)
	      /* la borne n'est pas a la fin de l'element, on decale la borne */
	      ChkrRange->SEndChar += LgChaineRempl - LgChaineCh;
     }
   else
     {
	idx = ChkrIndChar + 1;
	ReplaceString (ChkrRange->SDocument, ChkrElement, idx,
		       LgChaineCh, pChaineRemplace, LgChaineRempl, TRUE);
     }

   /* met eventuellement a jour la selection initiale */
   UpdateDuringSearch (ChkrElement, LgChaineRempl - LgChaineCh);

}				/* end of Corr_remplacermot */


/*----------------------------------------------------------------------
   CorrChangeSelection retourne vrai si la selection a change      
   depuis la derniere recherche d'erreur.                       
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             CorrChangeSelection ()

#else  /* __STDC__ */
boolean             CorrChangeSelection ()
#endif				/* __STDC__ */

{
   PtrDocument         docsel;
   PtrElement          pEl1, pElN;
   int                 c1, cN;
   boolean             ok;

   /* Si le correcteur n'a pas debute */
   if (ChkrElement == NULL)
      /* Ce n'est pas la peine de faire cette verification */
      return (FALSE);

   docsel = ChkrRange->SDocument;
   if (ChkrRange->SStartToEnd)
     {
	pEl1 = ChkrElement;
	c1 = ChkrIndChar;
	ok = GetCurrentSelection (&ChkrRange->SDocument, &ChkrElement, &pElN, &cN, &ChkrIndChar);
     }
   else
     {
	pEl1 = ChkrElement;
	c1 = ChkrIndChar;
	ok = GetCurrentSelection (&ChkrRange->SDocument, &pElN, &ChkrElement, &ChkrIndChar, &cN);
     }

   if (!ok)
     {
	/* Il n'y a pas encore de selection actuelle dans ce document */
	ChkrRange->SDocument = docsel;
	pEl1 = ChkrElement;
	pElN = pEl1;
	c1 = ChkrIndChar;
	cN = c1;
     }

   if (docsel != ChkrRange->SDocument)
      /* La selection a change de document */
      return (TRUE);
   else
     {
	ok = (ChkrElement != pEl1 || ChkrElement != pElN || ChkrIndChar != c1);
	/* S'il s'agit d'une nouvelle selection dans le document */
	if (ok && ChkrIndChar != 0)
	   ChkrIndChar--;
	return (ok);
     }
}				/*CorrChangeSelection */


/*----------------------------------------------------------------------
   TestListCar teste si ce caractere appartient a la liste listcar    
   retourne TRUE si oui                               
   FALSE sinon                               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             TestListCar (char car, char *listcar)

#else  /* __STDC__ */
boolean             TestListCar (car, listcar)
char                car;
char               *listcar;

#endif /* __STDC__ */

{
   int                 i;

   for (i = 0; i < strlen (listcar); i++)
     {
	if (car == listcar[i])
	   return (TRUE);
     }
   return (FALSE);
}				/*TestListCar */

/*----------------------------------------------------------------------
   ACeCar retourne TRUE si le mot contient un des caracteres       
   contenu dans ListCar.                                    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static boolean      ACeCar (char mot[MAX_CHAR])

#else  /* __STDC__ */
static boolean      ACeCar (mot)
char                mot[MAX_CHAR];

#endif /* __STDC__ */

{

   boolean             result;
   int                 i;
   int                 longueur;

   result = FALSE;
   longueur = strlen (mot);
   if (longueur > 0)
     {
	for (i = 0; i < longueur && (result == FALSE); i++)
	   if (TestListCar (mot[i], RejectedChar) == TRUE)
	      result = TRUE;
     }
   return result;
}

/*----------------------------------------------------------------------
   AUnNombre                                                       
   retourne TRUE si le mot contient au moins un chiffre arabe.     
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             AUnNombre (char mot[MAX_CHAR])

#else  /* __STDC__ */
boolean             AUnNombre (mot)
char                mot[MAX_CHAR];

#endif /* __STDC__ */

{

   boolean             result;
   int                 i;
   int                 longueur;

   result = FALSE;
   longueur = strlen (mot);
   if (longueur > 0)
     {
	for (i = 0; i < longueur && (result == FALSE); i++)
	   if (mot[i] >= '0' && mot[i] <= '9')
	      result = TRUE;
     }
   return result;
}

/*** debut ajout ****/
/*----------------------------------------------------------------------
   EstUnNombre retourne TRUE si le mot est forme' uniquement de    
   chiffres decimaux arabes.                                       
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             EstUnNombre (char mot[MAX_CHAR])

#else  /* __STDC__ */
boolean             EstUnNombre (mot)
char                mot[MAX_CHAR];

#endif /* __STDC__ */

{

   boolean             result;
   int                 i;
   int                 longueur;

   result = FALSE;
   longueur = strlen (mot);
   if (longueur > 0)
     {

	result = TRUE;
	for (i = 0; i < longueur && result; i++)
	   if (mot[i] < '\0' || mot[i] > '9')
	      result = FALSE;
     }
   return result;
}
/*** fin ajout ****/

/*----------------------------------------------------------------------
   EnRomain retourne TRUE si le mot est forme' uniquement de       
   chiffres romains.                                               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             EnRomain (char mot[MAX_CHAR])

#else  /* __STDC__ */
boolean             EnRomain (mot)
char                mot[MAX_CHAR];

#endif /* __STDC__ */

{
   /* description des chiffres romains (majuscule) */
   static char         NRomain[] =
   {'M', 'C', 'D', 'L', 'X', 'V', 'I'};
   static char         NRomainIsole[] =
   {'C', 'L', 'V'};

   boolean             result;
   int                 i, j, nbcar;
   int                 longueur, lg1;
   char                cecar;

   result = FALSE;
   longueur = strlen (mot);
   if (longueur > 0)
     {
	result = TRUE;
	for (i = 0; i < longueur && result; i++)
	   if (TestListCar (mot[i], NRomain) != TRUE)
	      result = FALSE;
     }
   if (result == TRUE)
      /* analyse plus fine de ce "possible" chiffre romain */
     {
	/* pas plus de 3 fois la meme lettre successivement dans ce nombre */
	lg1 = longueur - 3;
	for (i = 0; i < lg1 && result; i++)
	  {
	     cecar = mot[i];
	     nbcar = 1;
	     for (j = i + 1; j < longueur && mot[j] == cecar; j++)
		nbcar++;
	     if (nbcar > 3)
		/* ce n'est pas un "bon" chiffre romain */
		result = FALSE;
	  }

	/* pas plus de 1 fois V, L ou D successivement dans ce nombre */
	for (i = 0; i < longueur - 1 && result; i++)
	  {
	     if (TestListCar (mot[i], NRomainIsole) == TRUE)
	       {
		  if (mot[i + 1] == mot[i])
		     result = FALSE;
	       }
	  }

	/*HR *//* ne pas considerer "M" comme un romain */
/*HR */ if (strlen (mot) == 1 && mot[0] == 'M')
/*HR */ result = FALSE;


	/* verifier aussi l'ordre des I V X L C D M */
	/* A FAIRE */

     }
   return result;
}


/*----------------------------------------------------------------------
   IgnorerMot                                                      
   retourne TRUE si mot doit etre ignore (avec capitale, romain,      
   chiffre arabe, car. special)                                       
   retourne FALSE sinon : le mot sera alors verifie par le correcteur 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static boolean      IgnorerMot (char mot[MAX_CHAR])

#else  /* __STDC__ */
static boolean      IgnorerMot (mot)
char                mot[MAX_CHAR];

#endif /* __STDC__ */

{

   boolean             result = FALSE;

   /* les mots en capitale */
   if (IgnoreUppercase == TRUE)
      if (ismajuscule (mot) == TRUE)
	 return (TRUE);

   /* les mots contenant un chiffre arabe */
   if (IgnoreArabic == TRUE)
      if (AUnNombre (mot) == TRUE)
	 return (TRUE);

   /* les chiffres romains */
   if (IgnoreRoman == TRUE)
      if (EnRomain (mot) == TRUE)
	 return (TRUE);

   /* les mots contenant au moins l'un des caracteres de RejectedChar[] */
   if (IgnoreSpecial == TRUE)
      result = ACeCar (mot);

   return result;
}


/*----------------------------------------------------------------------
   Corr_errsuivant retourne le mot errone' suivant et le           
   selectionne dans la vue courante du document.                
   Le mot errone' est mis dans ChkrErrWord.                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                Corr_errsuivant (char mot[MAX_CHAR], PtrDict dicodocu)
#else  /* __STDC__ */
void                Corr_errsuivant (mot, dicodocu)
char                mot[MAX_CHAR];
PtrDict             dicodocu;

#endif /* __STDC__ */
{
   Language            langue;
   boolean             ok, novalid;
   int                 i;

   i = 0;
   do
     {
	/* Recherche un mot a corriger */
	novalid = TRUE;
        ok = TRUE;
	while (ok && novalid)
	  {
	     if (ChkrRange->SStartToEnd)
		ok = SearchNextWord (&ChkrElement, &ChkrIndChar, mot, ChkrRange);
	     else
		ok = SearchPreviousWord (&ChkrElement, &ChkrIndChar, mot, ChkrRange);
	     /* Is it a valid selection ? */
	     if (ok && ChkrElement->ElParent != NULL)
		novalid = TypeHasException (ExcNoSpellCheck, ChkrElement->ElParent->ElTypeNumber, ChkrElement->ElParent->ElStructSchema);
	  }

	if (ok)
	  {
	     /* verifie si c'est un nombre ou s'il doit etre ignore' */
	     if (EstUnNombre (mot) || IgnorerMot (mot))
		i = -1;
	     else
	       {
		  /* Charge si necessaire les dictionnaires de la langue */
		  langue = ChkrElement->ElLanguage;
		  if (langue != ChkrLanguage)
		    {
		       ChkrLanguage = langue;
		       TtaLoadLanguageDictionaries (ChkrLanguage);
		    }
		  i = Corr_verifiermot (mot, ChkrLanguage, dicodocu);
		  /* 1   = mot trouve dans l'un des dico */
		  /* 0   = mot inconnu ou vide */
		  /* -1  = pas de dico pour verifier un mot de cette langue */
		  if (i != 1)
		     /* est-ce un chiffre romain correct */
		     i = (EnRomain (mot) == TRUE) ? 1 : i;
	       }
	  }
     }
   /* saute les mots qui sont dans une langue SANS dictionnaire */
   while (ok && (i > 0 || i == -1));

   i = strlen (mot);
   if (i > 0)
     {
	/* on a trouve un mot */
	/* selectionner le mot a corriger */
	if (ChkrRange->SStartToEnd)
	   SelectString (ChkrRange->SDocument, ChkrElement, ChkrIndChar - i + 1, ChkrIndChar);
	else
	   SelectString (ChkrRange->SDocument, ChkrElement, ChkrIndChar + 1, ChkrIndChar + i);
     }
   strcpy (ChkrErrWord, mot);
}				/*Corr_errsuivant */


/************************ sauverdicodocu *********************************/

#ifdef __STDC__
void                sauverdicofichier (PtrDict dicodocu)

#else  /* __STDC__ */
void                sauverdicofichier (dicodocu)
PtrDict             dicodocu;

#endif /* __STDC__ */

{
   FILE               *fichier;
   int                 i, j;
   char                tempbuffer[MAX_CHAR];
   char                mot[MAX_WORD_LEN];

   FindCompleteName (dicodocu->DictNom, "DIC", dicodocu->DictDirectory, tempbuffer, &i);
   if (dicodocu->nbmots >= 0)
     {
	fichier = fopen (tempbuffer, "w");
	if (fichier != NULL)
	  {			/* enregistrer d'abord MAXmots et MAXcars effectifs */
	     i = dicodocu->nbmots;
	     j = dicodocu->nbcars;
	     fprintf (fichier, "%d %d\n", i, j);

	     for (i = 0; i <= dicodocu->nbmots; i++)
	       {
		  strcpy (mot, &dicodocu->chaine[dicodocu->pdico[i]]);
		  code2asci (mot);
		  fprintf (fichier, "%s\n", mot);
		  /* ajouter le CR de fin de ligne */
	       }
	     fclose (fichier);
	     /* OK sauvegarde dictionnaire document */
	     TtaDisplayMessage (INFO, TtaGetMessage (CORR, OK_SAVE),
				dicodocu->DictNom);
	     /* toutes les mises a jour sont enregistrees */
	     dicodocu->DictModifie = FALSE;
	  }
	else
	   /* erreur sauvegarde dictionnaire document */
	   TtaDisplaySimpleMessage (INFO, CORR, NO_SAVE);
     }
}				/*sauverdicodocu */

/*----------------------------------------------------------------------
   inserermot 
   inserer un mot nouveau dans un dico 
   retourne -2 si le dictionnaire est inaccessible             
   -1 si le dictionnaire est plein                    
   0 si le mot est deja dans le dictionnaire         
   1 si le mot a pu etre ajoute dans le dictionnaire 
   par defaut : tous les mots nouveaux sont insecables        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static int          inserermot (PtrDict dico, char mot[MAX_WORD_LEN])

#else  /* __STDC__ */
static int          inserermot (dico, mot)
PtrDict             dico;
char                mot[MAX_WORD_LEN];

#endif /* __STDC__ */

{
   int                 taille, place, i, k;

   place = existmot (mot, dico);
   if (place == -2)
      return (-2);		/* dictionnaire inaccessible */
   if (place == -3)
      return (0);		/* mot deja present dans le dictionnaire */
   if (place == -1)		/* dictionnaire vide */
      place = 0;		/* indice pour inserer dans ce dictionaire */
   taille = strlen (mot) + 1;
   if ((dico->nbmots >= dico->MAXmots - 1) || (dico->nbcars >= dico->MAXcars + taille))
      return (-1);
   else
     {
	for (i = dico->nbmots; i >= place; i--)		/* deplacement des mots */
	  {
	     int                 debut, fin, index;

	     debut = dico->pdico[i];
	     fin = dico->pdico[i + 1] - 1;
	     index = fin + taille;
	     for (k = fin; k >= debut; k--)	/* deplacement d'un mot */
		dico->chaine[index--] = dico->chaine[k];
	  }
	/* insertion nouveau mot */
	strcpy (&dico->chaine[dico->pdico[place]], mot);
	/* mise a jour des pointeurs sur les mots */
	for (i = dico->nbmots + 1; i >= place; i--)
	   dico->pdico[i + 1] = dico->pdico[i] + taille;
	/* mise a jour des pointeurs sur les longueurs de mots */
	for (i = taille; i < MAX_WORD_LEN; i++)
	   dico->plgdico[i]++;

	dico->nbmots++;
	dico->nbcars += taille;
	dico->DictModifie = TRUE;
	return (1);
     }
}


/******************************** inserer *********************************/

#ifdef __STDC__
static int          inserer (int x, int adrmot, PtrDict dico)

#else  /* __STDC__ */
static int          inserer (x, adrmot, dico)
int                 x;
int                 adrmot;
PtrDict             dico;

#endif /* __STDC__ */

{
   int                 i, k;

   if (x < listscore[NC - 1])
     {
	i = 0;
	while (listscore[i] < x)
	   i++;
	for (k = NC - 1; k > i; k--)
	  {
	     listscore[k] = listscore[k - 1];
	     listadrmot[k] = listadrmot[k - 1];
	     listdico[k] = listdico[k - 1];
	  }
	listscore[i] = x;
	listadrmot[i] = adrmot;
	listdico[i] = dico;
     }
   return (listscore[NC - 1]);
}

/************************************ Cmp ***********************************/

#ifdef __STDC__
static void         Cmp (char mottest[], PtrDict dico)

#else  /* __STDC__ */
static void         Cmp (mottest, dico)
char                mottest[];
PtrDict             dico;

#endif /* __STDC__ */

{
   int                 dist[MAX_WORD_LEN][MAX_WORD_LEN];
   unsigned char       motcmp[MAX_WORD_LEN];
   int                 Lg, idx, sup, adrmot, seuilCourant;
   int                 i, j, k, x, y, z;
   int                 difference, iteration, taille, largeur, mot;
   unsigned char       mot_courant[MAX_WORD_LEN];
   int                 minimum;
   int                 deb, fin;

   x = 0;
   if (dico == NULL)
      return;
   else if (dico->nbmots < 0)
      return;			/* dictionnaire vide */

   /* initialisation des distances aux bords */
   dist[0][0] = 0;
   for (i = 1; i < MAX_WORD_LEN; i++)
      dist[i][0] = dist[i - 1][0] + KO;
   for (i = 1; i < MAX_WORD_LEN; i++)
      dist[0][i] = dist[0][i - 1] + KI;

   strcpy (motcmp, mottest);
   SmallLettering (motcmp);
   asci2code (motcmp);
   Lg = strlen (motcmp);
   seuilCourant = Seuil[Lg];
   largeur = Delta[Lg];

/* parcours du dictionnaire
   avec d'abord les mots de meme taille,
   puis EVENTUELLEMENT les mots de +- 1 lettre
   puis EVENTUELLEMENT les mots de +- 2 lettres etc.
 */

   difference = 0;
   for (iteration = 0; iteration <= 2 * largeur; iteration++)
     {
	int                 derniere_ligne = MAX_WORD_LEN;	/* derniere ligne de calcul effectuee */
	int                 dist_mini;

	difference = (difference > 0) ? difference - iteration : difference + iteration;
	taille = Lg - difference;

/*
   determination si le calcul des mots de cette taille doivent etre calcules. 
   dist_mini determine le cout minimum obligatoire pour cette taille de mot 
   si ce cout est deja superieur au seuil courant, il est inutile de traiter
   ces mots
 */
	dist_mini = (difference > 0) ? difference * KI : -difference * KO;
	if (dist_mini > seuilCourant)
	   continue;

/* calcul des indices de debut et fin de dictionnaire */
	mot = dico->plgdico[taille];
	idx = dico->pdico[mot];
	if (taille >= MAX_WORD_LEN || (dico->plgdico[taille + 1] - 1 > dico->nbmots))
	   sup = dico->pdico[dico->nbmots];
	else
	   sup = dico->pdico[dico->plgdico[taille + 1]];

/* initialisation des valeurs en dehors des diagonales de calculs effectifs */

	for (j = 1; j <= taille; j++)
	  {
	     i = j - largeur - 1;
	     if (difference > 0)
		i = i + difference;
	     if (i > 0)
		dist[i][j] = seuilCourant + 1;
	     i = j + largeur + 1;
	     if (difference < 0)
		i = i + difference;
	     if (i <= Lg)
		dist[i][j] = seuilCourant + 1;
	  }

/* parcours du dictionnaire */
	while (idx < sup)
	  {

	     adrmot = idx;
	     k = dico->commun[mot++];
/* si le calcul du mot precedent a ete stoppe a l'indice derniere_ligne
   et que le mot courant possede un nombre de lettres communes superieur
   a cette valeur, il est inutile de faire le calcul */
	     if (k <= derniere_ligne)
	       {
		  strcpy (mot_courant, &dico->chaine[adrmot]);

/* calcul */
		  for (j = k; j <= taille; j++)
		    {
		       minimum = dist[0][j];
		       derniere_ligne = j;
		       deb = j - largeur;
		       if (difference > 0)
			  deb = deb + difference;
		       if (deb < 1)
			  deb = 1;
		       fin = j + largeur;
		       if (difference < 0)
			  fin = fin + difference;
		       if (fin > Lg)
			  fin = Lg;
		       for (i = deb; i <= fin; i++)
			 {
			    x = dist[i][j - 1] + KI;
			    y = dist[i - 1][j] + KO;
			    z = dist[i - 1][j - 1] + Tsub[mot_courant[j - 1]][motcmp[i - 1]];
			    x = (x < y) ? x : y;
			    x = (x < z) ? x : z;
			    if ((i > 1) && (j > 1))
			      {
				 y = dist[i - 2][j - 2] + Tsub[mot_courant[j - 2]][motcmp[i - 1]]
				    + Tsub[mot_courant[j - 1]][motcmp[i - 2]] + KP;
				 x = (x < y) ? x : y;
			      }
			    dist[i][j] = x;
			    /* mise a jour du minimum de la colonne */
			    minimum = (x < minimum) ? x : minimum;
			 }
/* sortie de boucle si les resultats sont deja tous superieurs au seuil */
		       if (minimum > seuilCourant)
			  break;
		    }		/* for j */
		  if (x <= seuilCourant)
		    {
		       int                 dernier_liste;

		       dernier_liste = inserer (x, adrmot, dico);
		       if (dernier_liste < seuilCourant)
			  seuilCourant = dernier_liste;
		    }
	       }		/* if k<= derniere_ligne */
	     idx += taille + 1;
	  }			/* while ( idx < sup ) */
     }				/* for iteration */
}				/* end of cmp */

/***************************** charger_corr *********************************/
#ifdef __STDC__
static void         charger_corr ()
#else  /* __STDC__ */
static void         charger_corr ()
#endif				/* __STDC__ */
{
   int                 i;
   int                 adr;

   strcpy (ChkrCorrection[0], ChkrErrWord);
   for (i = 0; i < NC; i++)
     {
	adr = listadrmot[i];
	if (adr >= 0)
	  {
	     strcpy (ChkrCorrection[i + 1], &listdico[i]->chaine[adr]);
	     code2asci (ChkrCorrection[i + 1]);
	     switch (type_err)
		   {
		      case 1:	/* SmallLettering ou melange */
			 break;
		      case 2:
			 capital (ChkrCorrection[i + 1]);
			 break;
		      case 3:
			 majuscule (ChkrCorrection[i + 1]);
			 break;
		   }
	  }
     }
}
