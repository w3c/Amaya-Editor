/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2002
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Spell checking
 *
 * Authors: I. Vatton (INRIA), H. Richy (INRIA)
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
#include "dictionary.h"

#define THOT_EXPORT extern
#include "spell_tv.h"
#undef THOT_EXPORT
#define THOT_EXPORT
#include "word_tv.h"
/* description des chiffres romains (majuscule) */
static char         NRomain[] = {
  'M', 'C', 'D', 'L', 'X', 'V', 'I', EOS
};
static char         NRomainIsole[] = {
  'C', 'L', 'V', EOS
};

extern unsigned char Code[256];
extern unsigned      ReverseCode[NbLtr];
static ThotBool      Clavier_charge;
static int           Tsub[NbLtr][NbLtr];
static int           KI = 3,	/* insertion */
                     KO = 3,	/* omission */
                     KB = 0,	/* bonne substition */
                     KM = 1,	/* substitution moyenne */
                     KD = 3,	/* substitution par defaut */
                     KP = 3;	/* permutation */
static int           Seuil[MAX_WORD_LEN];
static int           Delta[MAX_WORD_LEN];
static int           Scores[MAX_PROPOSAL_CHKR + 1];	/* Tab des scores des correction */
static int           WordsList[MAX_PROPOSAL_CHKR + 1];	/* Tab des numeros de mots errones */
static PtrDict       DictsList[MAX_PROPOSAL_CHKR + 1];
static int           type_err;	/* Type du mot errone (majuscule, SmallLettering) */

#include "applicationapi_f.h"
#include "dictionary_f.h"
#include "exceptions_f.h"
#include "fileaccess_f.h"
#include "hyphen_f.h"
#include "memory_f.h"
#include "registry_f.h"
#include "search_f.h"
#include "spellchecker_f.h"
#include "structselect_f.h"
#include "undo_f.h"
#include "word_f.h"


/*----------------------------------------------------------------------
   TtaLoadDocumentDictionary loads the document dictionary.          
   Returns -1 if the dictionary can't be loaded.                     
   Returns 1 if the dictionary is loaded.                            
  ----------------------------------------------------------------------*/
ThotBool TtaLoadDocumentDictionary (PtrDocument document, int *pDictionary,
				    ThotBool ToCreate)
{
  char              *extenddic;
  char               path[MAX_PATH], dictname[MAX_PATH];

  /* dictionary name = document name */
  *pDictionary = (int) NULL;
  
  extenddic = TtaGetEnvString ("EXTENDDICT");
  if (extenddic != NULL)
    {
      TtaExtractName (extenddic, path, dictname);
      if (dictname[0] == EOS)
	strcpy (dictname, "dictionary.DCT");
    }
  else
    {
      path[0] = EOS;
      strcpy (dictname, "dictionary.DCT");
    }

  if (path[0] == EOS ||  !TtaCheckDirectory (path))
    strcpy (path, TtaGetEnvString ("APP_HOME"));
  LoadTreatedDict ((PtrDict *) pDictionary, 0, document, dictname,
		   path, FALSE, ToCreate);
  return (*pDictionary != EOS);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void Asci2Code (char *string)
{
   int                 i;

   i = 0;
   while (string[i] != EOS)
     {
	string[i] = Code[(unsigned char) string[i]];
	i++;
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void Code2Asci (char *string)
{
   int                 i = 0;

   while (string[i] != EOS)
     {
	string[i] = ReverseCode[(unsigned char) string[i]];
	i++;
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SetUpperCase (char *string)
{
   int                 i = 0;

   while ((string[i]) != EOS)
     {
	if (isimin (string[i]) != 0)
	   string[i] = toupper (string[i]);

	i++;
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SetCapital (char *string)
{
   if (isimin (string[0]) != 0)
      string[0] = toupper (string[0]);

}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool IsUpperCase (char *string)
{
   int                 maj = 1;
   int                 i = 0;
   char                c;

   while ((c = string[i]) != EOS && (maj != 0))
     {
	maj = isimaj (string[i]);
	i++;
     }
   return (maj == 0) ? FALSE : TRUE;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool IsCapital (char *string)
{
   int                 cap = 0;
   int                 i = 0;
   char                c;

   if (string[0] != EOS && isimaj (string[0]) != 0)
     {
	cap = 1;
	i++;
	while ((c = string[i]) != EOS && (cap != 0))
	  {
	     cap = !(isimaj (string[i]));
	     i++;
	  }
     }
   return (cap == 0) ? FALSE : TRUE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool IsIso (char *string)
{
   int                 iso = 1;
   int                 i = 0;

   while (string[i] != EOS && (iso != 0))
     {
	iso = isalphiso (string[i]) || string[i] == '-' || string[i] == '\'';
	i++;
     }
   return (iso == 0) ? FALSE : TRUE;
}


/*----------------------------------------------------------------------
   WordInDictionary recherche dichotomique de "word" dans "dict[]"
   retourne -2 si le dict n'existe pas                                     
   retourne -1 si le dict est vide                                         
   retourne -3  si le mot est present dans le dict                          
   retourne >=0 si le mot n'est pas dans le dict (indice de sa place future) 
  ----------------------------------------------------------------------*/
int WordInDictionary (unsigned char *word, PtrDict dict)
{
   int                 inf, sup, med, rescomp, size;

   if (dict == NULL)
     /* dictionnaire n'existe pas */
     return (-2);
   else if (dict->DictNbWords < 0)
     /* dictionnaire vide */
     return (-1);

   size = strlen (word);
   /* premier mot */
   inf = dict->DictLengths[size];
   if (size >= MAX_WORD_LEN || (dict->DictLengths[size + 1] - 1 > dict->DictNbWords))
     sup = dict->DictNbWords;
   else
     /* dernier mot */
     sup = dict->DictLengths[size + 1] - 1;

   while (sup >= inf)
     /* Recherche dichotomique */
     {
	med = (sup + inf) / 2;
	rescomp = strcmp (&dict->DictString[dict->DictWords[med]], word);
	if (rescomp == 0)
	  /* Mot present dans le dict */
	  return (-3);
	else if (rescomp > 0)
	  /* Poursuite de la recherche */
	  sup = med - 1;
	else
	  inf = med + 1;
     }
   /* Mot absent dans le dict */
   return (inf);
}


/*----------------------------------------------------------------------
   CheckWord retourne                                       
   1 si le mot appartient a un dict                        
   ou est de longueur 1 ou 2                             
   0 si le mot est inconnu ou vide                         
   -1 s'il n'y a pas de dictionnaire pour verifier ce mot   
  ----------------------------------------------------------------------*/
int CheckWord (unsigned char *word, Language language, PtrDict dict)
{
  PtrDict             globalDict;
  PtrDict             personalDict;
  int                 res;
  unsigned char       word1[MAX_WORD_LEN];

  /* On refuse de corriger l'ISOlatin-1 */
  if (language == 0)
    return -1;
  if (word[0] == EOS)
    return 0;
  if (strlen (word) < 2)
    return 1;
  else
    {
      strcpy (word1, word);
      SmallLettering (word1);
      Asci2Code (word1);
      globalDict = (PtrDict) TtaGetPrincipalDictionary (language);
      personalDict = (PtrDict) TtaGetSecondaryDictionary (language);
      res = WordInDictionary (word1, dict);
      if (res != -3)
	{
	  /* not found */
	  res = WordInDictionary (word1, globalDict);
	  if (res == -2)
	    return (-1);	/* PAS de globalDict */
          else if (res != -3)
            {
              /* check extra dictionaries */
              if (WordInDictionary (word1, personalDict) == -3)
                /* found */
                res = -3;
	    }
	}
      if (res != -3)
	{			/* calculer le type du mot errone' */
	  if (IsUpperCase (word))
	    type_err = 3;	/* MAJ */
	  else if (IsCapital (word))
	    type_err = 2;	/* Capitale */
	  else
	    type_err = 1;	/* SmallLettering ou melange */
	  /* recopier ce mot errone' dans ChkrErrWord */
	  strcpy (ChkrErrWord, word);
	  return 0;
	}
      else
	return 1;
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static int Insert (int x, int pWord, PtrDict dict)
{
   int                 i, k;

   if (x < Scores[NC - 1])
     {
	i = 0;
	while (Scores[i] < x)
	   i++;
	for (k = NC - 1; k > i; k--)
	  {
	     Scores[k] = Scores[k - 1];
	     WordsList[k] = WordsList[k - 1];
	     DictsList[k] = DictsList[k - 1];
	  }
	Scores[i] = x;
	WordsList[i] = pWord;
	DictsList[i] = dict;
     }
   return (Scores[NC - 1]);
}

/*----------------------------------------------------------------------
   InsertWord 
   inserer un mot nouveau dans un dict 
   retourne -2 si le dictionnaire est inaccessible             
   -1 si le dictionnaire est plein                    
   0 si le mot est deja dans le dictionnaire         
   1 si le mot a pu etre ajoute dans le dictionnaire 
   par defaut : tous les mots nouveaux sont insecables        
  ----------------------------------------------------------------------*/
static int InsertWord (PtrDict dict, unsigned char *word)
{
   int                 size, place, i, k;

   place = WordInDictionary (word, dict);
   if (place == -2)
      return (-2);		/* dictionnaire inaccessible */
   if (place == -3)
      return (0);		/* mot deja present dans le dictionnaire */
   if (place == -1)		/* dictionnaire vide */
      place = 0;		/* indice pour inserer dans ce dictionaire */
   size = strlen (word) + 1;
   /* JK: Changed the limit to - 2 instead of -1 because we had a
      memory overwrite problem otherwise */
   if ((dict->DictNbWords >= dict->DictMaxWords - 2) || (dict->DictNbChars >= dict->DictMaxChars + size))
      return (-1);
   else
     {
	for (i = dict->DictNbWords; i >= place; i--)
	  /* deplacement des mots */
	  {
	     int                 debut, fin, index;

	     debut = dict->DictWords[i];
	     fin = dict->DictWords[i + 1] - 1;
	     index = fin + size;
	     for (k = fin; k >= debut; k--)	/* deplacement d'un mot */
		dict->DictString[index--] = dict->DictString[k];
	  }
	/* insertion nouveau mot */
	strcpy (&dict->DictString[dict->DictWords[place]], word);
	/* mise a jour des pointeurs sur les mots */
	for (i = dict->DictNbWords + 1; i >= place; i--)
	   dict->DictWords[i + 1] = dict->DictWords[i] + size;
	/* mise a jour des pointeurs sur les longueurs de mots */
	for (i = size; i < MAX_WORD_LEN; i++)
	   dict->DictLengths[i]++;

	dict->DictNbWords++;
	dict->DictNbChars += size;
	dict->DictModified = TRUE;
	return (1);
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void Cmp (unsigned char *wordtest, PtrDict dict)
{
  int                 dist[MAX_WORD_LEN][MAX_WORD_LEN];
  unsigned char       wordcmp[MAX_WORD_LEN];
  unsigned char       currentWord[MAX_WORD_LEN];
  int                 lg, idx, sup, pWord, seuilCourant;
  int                 i, j, k, x, y, z;
  int                 difference, iteration, size, largeur, word;
  int                 minimum;
  int                 deb, fin;
  int                 derniere_ligne = MAX_WORD_LEN; /* last computed line */
  int                 dist_mini;
  int                 dernier_liste;
  
  x = 0;
  if (dict == NULL)
    return;
  else if (dict->DictNbWords < 0)
    /* empty dictionary */
    return;
  
  /* initialisation des distances aux bords */
  dist[0][0] = 0;
  for (i = 1; i < MAX_WORD_LEN; i++)
    dist[i][0] = dist[i - 1][0] + KO;
  for (i = 1; i < MAX_WORD_LEN; i++)
    dist[0][i] = dist[0][i - 1] + KI;
  idx = 0;
  strcpy (wordcmp, wordtest);
  SmallLettering (wordcmp);
  Asci2Code (wordcmp);
  lg = strlen (wordcmp);
  seuilCourant = Seuil[lg];
  largeur = Delta[lg];
  
  /* parcours du dictionnaire
     avec d'abord les mots de meme size,
     puis EVENTUELLEMENT les mots de +- 1 lettre
     puis EVENTUELLEMENT les mots de +- 2 lettres etc.
     */
  difference = 0;
  for (iteration = 0; iteration <= 2 * largeur; iteration++)
    {
      difference = (difference > 0) ? difference - iteration : difference + iteration;
      size = lg - difference;
      if (size >= 0 && size < MAX_WORD_LEN)
	{
	  /*
	    determination si le calcul des mots de cette size doivent etre calcules. 
	    dist_mini determine le cout minimum obligatoire pour cette size de mot 
	    si ce cout est deja superieur au seuil courant, il est inutile de traiter
	    ces mots
	  */
	  dist_mini = (difference > 0) ? difference * KI : -difference * KO;
	  /*if (dist_mini > seuilCourant)
	    continue;*/
	  
	  /* calcul des indices de debut et fin de dictionnaire */
	  word = dict->DictLengths[size];
	  idx = dict->DictWords[word];
	  if (dict->DictLengths[size + 1] - 1 > dict->DictNbWords)
	    sup = dict->DictWords[dict->DictNbWords];
	  else
	    sup = dict->DictWords[dict->DictLengths[size + 1]];
	  
	  /* initialisation des valeurs en dehors des diagonales de calculs effectifs */
	  for (j = 1; j <= size; j++)
	    {
	      i = j - largeur - 1;
	      if (difference > 0)
		i = i + difference;
	      if (i > 0)
		dist[i][j] = seuilCourant + 1;
	      i = j + largeur + 1;
	      if (difference < 0)
		i = i + difference;
	      if (i <= lg)
		dist[i][j] = seuilCourant + 1;
	    }
	  
	  /* parcours du dictionnaire */
	  while (idx < sup && idx < dict->DictNbChars)
	    {
	      pWord = idx;
	      k = dict->DictCommon[word++];
	      /* si le calcul du mot precedent a ete stoppe a l'indice derniere_ligne
		 et que le mot courant possede un nombre de lettres communes superieur
		 a cette valeur, il est inutile de faire le calcul */
	      if (k <= derniere_ligne)
		{
		  strcpy (currentWord, &dict->DictString[pWord]);
		  
		  /* calcul */
		  for (j = k; j <= size; j++)
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
		      if (fin > lg)
			fin = lg;
		      for (i = deb; i <= fin; i++)
			{
			  x = dist[i][j - 1] + KI;
			  y = dist[i - 1][j] + KO;
			  z = dist[i - 1][j - 1] + Tsub[currentWord[j - 1]][wordcmp[i - 1]];
			  x = (x < y) ? x : y;
			  x = (x < z) ? x : z;
			  if ((i > 1) && (j > 1))
			    {
			      y = dist[i - 2][j - 2] + Tsub[currentWord[j - 2]][wordcmp[i - 1]]
				+ Tsub[currentWord[j - 1]][wordcmp[i - 2]] + KP;
			      x = (x < y) ? x : y;
			    }
			  dist[i][j] = x;
			  /* mise a jour du minimum de la colonne */
			  minimum = (x < minimum) ? x : minimum;
			}
		      /* sortie de boucle si les resultats sont deja tous superieurs au seuil */
		      if (minimum > seuilCourant)
			break;
		    }
		  if (x <= seuilCourant)
		    {		  
		      dernier_liste = Insert (x, pWord, dict);
		      if (dernier_liste < seuilCourant)
			seuilCourant = dernier_liste;
		    }
		}
	      idx += size + 1;
	    }
	}
    }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void         LoadSpellChecker ()
{
   int                 i;
   int                 pWord;

   strcpy (ChkrCorrection[0], ChkrErrWord);
   for (i = 0; i < NC; i++)
     {
	pWord = WordsList[i];
	if (pWord >= 0)
	  {
	     strcpy (ChkrCorrection[i + 1], &DictsList[i]->DictString[pWord]);
	     Code2Asci (ChkrCorrection[i + 1]);
	     switch (type_err)
		   {
		      case 1:	/* SmallLettering ou melange */
			 break;
		      case 2:
			 SetCapital (ChkrCorrection[i + 1]);
			 break;
		      case 3:
			 SetUpperCase (ChkrCorrection[i + 1]);
			 break;
		   }
	  }
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void         SaveDictFile (PtrDict docDict)
{
   FILE               *f;
   int                 i, j;
   char                tempbuffer[THOT_MAX_CHAR];
   char                word[MAX_WORD_LEN];

   FindCompleteName (docDict->DictName, "", docDict->DictDirectory, tempbuffer, &i);
   if (docDict->DictNbWords >= 0)
     {
	f = fopen (tempbuffer, "w");
	if (f != NULL)
	  {
	    /* enregistrer d'abord nb words and nb chars effectifs */
	     i = docDict->DictNbWords;
	     j = docDict->DictNbChars;
	     fprintf (f, "%d %d\n", i, j);

	     for (i = 0; i <= docDict->DictNbWords; i++)
	       {
		  strcpy (word, &docDict->DictString[docDict->DictWords[i]]);
		  Code2Asci (word);
		  fprintf (f, "%s\n", word);
		  /* ajouter le CR de fin de ligne */
	       }
	     fclose (f);
	     /* OK sauvegarde dictionnaire document */
	     TtaDisplayMessage (INFO, TtaGetMessage (CORR, OK_SAVE),
				docDict->DictName);
	     /* toutes les mises a jour sont enregistrees */
	     docDict->DictModified = FALSE;
	  }
	else
	   /* erreur sauvegarde dictionnaire document */
	   TtaDisplaySimpleMessage (INFO, CORR, NO_SAVE);
     }
}

/*----------------------------------------------------------------------
   AddWord                                                 
   ajoute un mot nouveau dans le dictionnaire docDict          
   apres avoir verifie qu'il n'y etait pas deja                 
   (s'il n'y a plus de place : ferme et ouvre avec plus d'espace) 
   creation eventuelle et initialisation de ChkrFileDict             
  ----------------------------------------------------------------------*/
void AddWord (unsigned char *word, PtrDict * pDict)
{
   unsigned char       word1[MAX_WORD_LEN];
   int                 ret;
   Name                DiNom;
   ThotBool            OKinsere = TRUE;
   PtrDict             docDict;

   docDict = *pDict;
   strcpy (word1, word);
   /* verifier que ce mot est bien en caracteres iso */
   if (IsIso (word1))
     {
	/* ajout d'un mot dans le dictionnaire docDict */
	SmallLettering (word1);
	Asci2Code (word1);
	ret = InsertWord (docDict, word1);
	switch (ret)
	      {
		 case -2:
		    /* le docDict est encore vide : le creer */
		    if (TtaLoadDocumentDictionary (ChkrRange->SDocument, (int *) pDict, TRUE))
		      {
			 docDict = *pDict;
			 /* le dictionnaire du document est maintenant charge' */
			 ret = InsertWord (docDict, word1);
			 TreateDictionary (docDict);
		      }
		    else
		      {
			 /* impossible d'ajouter dans le dictionnaire du document ??? */
			 TtaDisplayMessage (INFO, TtaGetMessage (CORR, REFUSE_WORD), word);
			 OKinsere = FALSE;
		      }
		    break;
		 case -1:
		    /* dictionnaire plein */
		    SaveDictFile (docDict);
		    /* rechargement du dictionnaire avec plus de memoire */
		    strcpy (DiNom, docDict->DictName); 
		    if (ReloadDictionary (pDict) == TRUE)
		      {
			 docDict = *pDict;
			 if (InsertWord (docDict, word1) > 0)
			    TreateDictionary (docDict);
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
		    TreateDictionary (docDict);
		    break;
	      }
	if (OKinsere == TRUE)
	  {
	     /* avertir l'utilisateur de la reussite de l'ajout dans le dict */
	     TtaDisplayMessage (INFO, TtaGetMessage (CORR, ADD_WORD), word);
	     /*  enregistrer le dictionnaire du document apres chaque mise a jour */
	     SaveDictFile (docDict);
	  }
     }
   else	
     /* car. incorrect, ajout du mot refus\351 */
      TtaDisplayMessage (INFO, TtaGetMessage (CORR, REFUSE_WORD), word);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void InitChecker ()
{
   int                 j;

   /* Initialisation des scores */
   for (j = 0; j <= NC; j++)
     {
	Scores[j] = 1000;
	WordsList[j] = -1;
	DictsList[j] = NULL;
     }

   /* Initialisation de la correction */
   for (j = 0; j <= NC; j++)
      strcpy (ChkrCorrection[j],"$");
}



/*----------------------------------------------------------------------
   GiveProposal          
   met dans ChkrCorrection les propositions de correction du mot       
   qui se trouve dans ChkrErrWord                                   
  ----------------------------------------------------------------------*/
void GiveProposal (Language language, PtrDict docDict)
{
   PtrDict             globalDict;
   PtrDict             personalDict;
   PtrDict             acronymDict;
   PtrDict             nameDict;

   /* ATTENTION : ChkrErrWord contient le mot a corriger */
   InitChecker ();	/* raz des scores */

   globalDict = (PtrDict) TtaGetPrincipalDictionary (language);
   personalDict = (PtrDict) TtaGetSecondaryDictionary (language);
   acronymDict = (PtrDict) TtaGetPrincipalDictionary (0);
   nameDict = (PtrDict) TtaGetSecondaryDictionary (0);

   /* on suppose que ChkrErrWord contient la description du mot errone' */
   Cmp (ChkrErrWord, globalDict);

   /* calcul local avec le dictionnaire personnel s'il existe */
   if (personalDict != NULL)
      Cmp (ChkrErrWord, personalDict);

   /* calcul local avec le dictionnaire du document s'il n'est pas vide */
   if (docDict != NULL)
      Cmp (ChkrErrWord, docDict);

   /* calcul local avec le dictionnaire de sigles s'il n'est pas vide */
   if (acronymDict != NULL)
      Cmp (ChkrErrWord, acronymDict);

   /* calcul local avec le dictionnaire de noms s'il n'est pas vide */
   if (nameDict != NULL)
      Cmp (ChkrErrWord, nameDict);
   /* remplissage de ChkrCorrection  */
   LoadSpellChecker ();
}



/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void init_Tsub (FILE *ftsub)
{
   unsigned char       ch1[80], ch2[80], ch3[80];
   unsigned char       x, y, valeur;
   int                 i, j;
   int                 coeff;

   /* initialisation de Tsub */
   for (i = 0; i < NbLtr; i++)
      for (j = 0; j < NbLtr; j++)
	 Tsub[i][j] = KD;
   while (fscanf (ftsub, "%s%s%s", ch1, ch2, ch3) != EOF)
     {
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
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void init_param (FILE * fd)
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
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void DefaultParams (int lettres)
{
   int                 i;

   NC = 3;
   for (i = 1; i < MAX_WORD_LEN; i++)
     {
	Seuil[i] = KI + KI * (i - 1) / lettres;
	Delta[i] = 1 + ((i - 1) / lettres);
     }
}


/*----------------------------------------------------------------------
   ParametrizeChecker retourne  0 en cas d'impossibilite d'initialisation  
   1 si OK                                         
   positionne Clavier_charge                                   
  ----------------------------------------------------------------------*/
int ParametrizeChecker ()
{
   FILE               *fparam;
   FILE               *ftsub;
   char                paramnom[MAX_LENGTH];
   char                clavnom[MAX_LENGTH];
   char               *corrpath;
   int                 ret;

   ret = 1;
   /* initialisations des parametres du correcteur */
   if (Clavier_charge == FALSE)
     {
	/* remplir corrpath pour acces aux fichiers param et clavier */
	corrpath = TtaGetEnvString ("DICOPAR");
	if (corrpath == NULL)
	  ret = 0;
	else
	  {
	     /* Lecture du fichier parametres */
	     strcpy (paramnom, corrpath);
         strcat (paramnom, DIR_STR);
	     strcat (paramnom, "param");
	     if ((fparam = fopen (paramnom, "r")) != NULL)
	       /* Existence du fichier */
		init_param (fparam);
	     else
	       {
		  /* valeur par defaut LGR = 4 */
		  DefaultParams (4);
	       }

	     /* Lecture du  fichier clavier */
	     strcpy (clavnom, corrpath);
         strcat (clavnom, DIR_STR);
	     strcat (clavnom, "clavier");
	     if ((ftsub = fopen (clavnom, "r")) != NULL)
	       /* Existence du fichier */
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
}


/*----------------------------------------------------------------------
   WordReplace                                               
  ----------------------------------------------------------------------*/
void WordReplace (unsigned char *orgWord,  unsigned char *newWord)
{
   int                 idx;
   int                 stringLength;	/* longueur de cette chaine */
   CHAR_T             *nString, *oString;
   int                 newStringLen;	/* longueur de cette chaine */

   /* replace the wrong word by the right word */
   oString = TtaConvertByteToCHAR (orgWord, ISO_8859_1);
   nString = TtaConvertByteToCHAR (newWord, ISO_8859_1);
   stringLength = ustrlen (oString);
   /* initialiser newStringLen et pChaineRemplace */
   newStringLen = ustrlen (nString);

   /* substitue la nouvelle chaine et la selectionne */
   if (ChkrRange->SStartToEnd)
     {
	idx = ChkrIndChar - stringLength;
	OpenHistorySequence (ChkrRange->SDocument, ChkrElement, ChkrElement,
			     NULL, idx, idx + stringLength);
	AddEditOpInHistory (ChkrElement, ChkrRange->SDocument, TRUE, TRUE);
	CloseHistorySequence (ChkrRange->SDocument);
	ReplaceString (ChkrRange->SDocument, ChkrElement, idx,
		       stringLength, nString, newStringLen, TRUE);
	/* met a jour ChkrIndChar */
	ChkrIndChar = idx + newStringLen;

	/* met eventuellement a jour la borne de fin du domaine de recherche */
	if (ChkrElement == ChkrRange->SEndElement)
	   /* la borne est dans l'element ou` on a fait le remplacement */
	   if (ChkrRange->SEndChar > 1)
	      /* la borne n'est pas a la fin de l'element, on decale la borne */
	      ChkrRange->SEndChar += newStringLen - stringLength;
     }
   else
     {
	idx = ChkrIndChar;
	OpenHistorySequence (ChkrRange->SDocument, ChkrElement, ChkrElement,
			     NULL, idx, idx+stringLength-1);
	AddEditOpInHistory (ChkrElement, ChkrRange->SDocument, TRUE, TRUE);
	CloseHistorySequence (ChkrRange->SDocument);
	ReplaceString (ChkrRange->SDocument, ChkrElement, idx,
		       stringLength, nString, newStringLen, TRUE);
     }

   TtaFreeMemory (nString);
   TtaFreeMemory (oString);
   /* met eventuellement a jour la selection initiale */
   UpdateDuringSearch (ChkrElement, newStringLen - stringLength);
}


/*----------------------------------------------------------------------
   CheckChangeSelection retourne vrai si la selection a change      
   depuis la derniere recherche d'erreur.                       
  ----------------------------------------------------------------------*/
ThotBool CheckChangeSelection ()
{
   PtrDocument         docsel;
   PtrElement          pEl1, pElN;
   int                 c1, cN;
   ThotBool            ok;

   /* Si le correcteur n'a pas debute */
   if (ChkrElement == NULL)
      /* Ce n'est pas la peine de faire cette verification */
      return (FALSE);

   docsel = ChkrRange->SDocument;
   if (ChkrRange->SStartToEnd)
     {
	pEl1 = ChkrElement;
	c1 = ChkrIndChar;
	ok = GetCurrentSelection (&ChkrRange->SDocument, &ChkrElement, &pElN,
				  &cN, &ChkrIndChar);
	if (ChkrIndChar < cN)
	  /* selection is just a caret */
	  ChkrIndChar = cN;
	if (pElN && ChkrIndChar == 0)
	  /* the whole element is selected */
	  ChkrIndChar = pElN->ElVolume;
     }
   else
     {
	pEl1 = ChkrElement;
	c1 = ChkrIndChar;
	ok = GetCurrentSelection (&ChkrRange->SDocument, &pElN, &ChkrElement,
				  &ChkrIndChar, &cN);
	if (pElN && ChkrIndChar == 0)
	  /* the whole element is selected */
	  ChkrIndChar = 1;
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
	return (ok);
     }
}


/*----------------------------------------------------------------------
   CheckCharList teste si ce caractere appartient a la liste listcar    
   retourne TRUE si oui                               
   FALSE sinon                               
  ----------------------------------------------------------------------*/
static ThotBool CheckCharList (unsigned char car, unsigned char *listcar)
{
  int                 i, l;

  l = strlen (listcar);
  for (i = 0; i < l; i++)
    {
      if (car == listcar[i])
	return (TRUE);
    }
  return (FALSE);
}

/*----------------------------------------------------------------------
   IncludeAChar returns TRUE if the word includes a character of the
   list ListCar.
  ----------------------------------------------------------------------*/
static ThotBool IncludeAChar (unsigned char *word)
{
  int                 i;
  int                 len;
  ThotBool            result;

  result = FALSE;
  len = strlen (word);
  if (len > 0)
    {
      for (i = 0; i < len && (result == FALSE); i++)
	if (CheckCharList (word[i], RejectedChar) == TRUE)
	  result = TRUE;
    }
  return result;
}

/*----------------------------------------------------------------------
   IncludeANumber returns TRUE if the word include a number.
  ----------------------------------------------------------------------*/
static ThotBool IncludeANumber (unsigned char *word)
{
  int                 i;
  int                 len;
  ThotBool            result;

  result = FALSE;
  len = strlen (word);
  if (len > 0)
    {
      for (i = 0; i < len && (result == FALSE); i++)
	if (word[i] >= '0' && word[i] <= '9')
	  result = TRUE;
    }
  return result;
}

/*----------------------------------------------------------------------
   IsANumber retourne TRUE si le mot est forme' uniquement de    
   chiffres decimaux arabes.                                       
  ----------------------------------------------------------------------*/
static ThotBool IsANumber (unsigned char *word)
{
  int                 i;
  int                 len;
  ThotBool            result;

  result = FALSE;
  len = strlen (word);
  if (len > 0)
    {
      
      result = TRUE;
      for (i = 0; i < len && result; i++)
	if (word[i] < 48 || word[i] > 57)
	  result = FALSE;
    }
  return result;
}

/*----------------------------------------------------------------------
   InRoman retourne TRUE si le mot est forme' uniquement de       
   chiffres romains.                                               
  ----------------------------------------------------------------------*/
static ThotBool InRoman (unsigned char *word)
{
  int                 i, j, nbcar;
  int                 len, lg1;
  unsigned char       c;
  ThotBool            result;

  result = FALSE;
  len = strlen (word);
  if (len > 0)
    {
      result = TRUE;
      for (i = 0; i < len && result; i++)
	if (CheckCharList (word[i], NRomain) != TRUE)
	  result = FALSE;
    }
  if (result == TRUE)
    /* analyse plus fine de ce "possible" chiffre romain */
    {
      /* pas plus de 3 fois la meme lettre successivement dans ce nombre */
      lg1 = len - 3;
      for (i = 0; i < lg1 && result; i++)
	{
	  c = word[i];
	  nbcar = 1;
	  for (j = i + 1; j < len && word[j] == c; j++)
	    nbcar++;
	  if (nbcar > 3)
	    /* ce n'est pas un "bon" chiffre romain */
	    result = FALSE;
	}

      /* pas plus de 1 fois V, L ou D successivement dans ce nombre */
      for (i = 0; i < len - 1 && result; i++)
	{
	  if (CheckCharList (word[i], NRomainIsole) == TRUE &&
	      word[i + 1] == word[i])
	    result = FALSE;
	}
      
      /* ne pas considerer "M" comme un romain */
      if (strlen (word) == 1 && word[0] == 'M')
	result = FALSE;
      /* verifier aussi l'ordre des I V X L C D M */
      /* A FAIRE */
    }
  return result;
}


/*----------------------------------------------------------------------
   IgnoreWord                                                      
   retourne TRUE si mot doit etre ignore (avec capitale, romain,      
   chiffre arabe, car. special)                                       
   retourne FALSE sinon : le mot sera alors verifie par le correcteur 
  ----------------------------------------------------------------------*/
static ThotBool IgnoreWord (unsigned char *word)
{

  ThotBool            result = FALSE;

  /* les mots en capitale */
  if (IgnoreUppercase && IsUpperCase (word))
    return (TRUE);
  /* les mots contenant un chiffre arabe */
  if (IgnoreArabic && IncludeANumber (word))
    return (TRUE);
  /* les chiffres romains */
  if (IgnoreRoman && InRoman (word))
    return (TRUE);
  /* les mots contenant au moins l'un des caracteres de RejectedChar[] */
  if (IgnoreSpecial)
    result = IncludeAChar (word);
  return result;
}


/*----------------------------------------------------------------------
   NextSpellingError retourne le mot errone' suivant et le           
   selectionne dans la vue courante du document.                
   Le mot errone' est mis dans ChkrErrWord.                           
  ----------------------------------------------------------------------*/
void NextSpellingError (unsigned char *word, PtrDict docDict)
{
  Language            language;
#ifdef _I18N_
  CHAR_T              s[MAX_WORD_LEN];
  int                 j;
#else /* _I18N_ */
  CHAR_T             *s;
#endif /* _I18N */
  int                 i;
  ThotBool            ok, novalid;

#ifdef _I18N_
  /* get the CHAR_T string */
  j = 0;
  while (word[j] != EOS)
    {
      s[j] = TtaGetWCFromChar (word[j], ISO_8859_1);
      j++;
    }
  s[j] = EOS;
#else /* _I18N_ */
  s = word;
#endif /* _I18N */
  i = 1;
  do
    {
      /* Recherche un mot a corriger */
      novalid = TRUE;
      ok = TRUE;
      while (ok && novalid)
	{
	  if (ChkrRange->SStartToEnd)
	    ok = SearchNextWord (&ChkrElement, &i, &ChkrIndChar, s, ChkrRange);
	  else
	    ok = SearchPreviousWord (&ChkrElement, &ChkrIndChar, &i, s, ChkrRange);
	  /* Is it a valid selection ? */
	  if (ok && ChkrElement->ElParent != NULL)
	    novalid = (TypeHasException (ExcNoSpellCheck,
					ChkrElement->ElParent->ElTypeNumber,
					ChkrElement->ElParent->ElStructSchema));
	}

#ifdef _I18N_
      /* get back the Iso string */
      j = 0;
      while (s[j] != 0)
	{
	  word[j] = TtaGetCharFromWC (s[j], ISO_8859_1);
	  j++;
	}
      word[j] = EOS;
#endif /* _I18N */
      
      if (ok)
	{
	  /* verifie si c'est un nombre ou s'il doit etre ignore' */
	  if (IsANumber (word) || IgnoreWord (word))
	    i = -1;
	  else
	    {
	      /* Charge si necessaire les dictionnaires de la langue */
	      language = ChkrElement->ElLanguage;
	      if (language != ChkrLanguage)
		{
		  ChkrLanguage = language;
		  TtaLoadLanguageDictionaries (ChkrLanguage);
		}
	      i = CheckWord (word, ChkrLanguage, docDict);
	      /* 1   = mot trouve dans l'un des dict */
	      /* 0   = mot inconnu ou vide */
	      /* -1  = pas de dict pour verifier un mot de cette language */
	      if (i != 1)
		/* est-ce un chiffre romain correct */
		i = (InRoman (word) == TRUE) ? 1 : i;
	    }
	}
    }
  /* saute les mots qui sont dans une langue SANS dictionnaire */
  while (ok && (i > 0 || i == -1));

  i = strlen (word);
  if (i > 0)
    {
      /* a word is found, select it */
      if (ChkrRange->SStartToEnd)
	SelectString (ChkrRange->SDocument, ChkrElement,
		      ChkrIndChar - i, ChkrIndChar);
      else
	SelectString (ChkrRange->SDocument, ChkrElement,
		      ChkrIndChar, ChkrIndChar + i);
    }
  strcpy (ChkrErrWord, word);
}
