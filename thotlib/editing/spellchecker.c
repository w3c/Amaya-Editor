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
#include "dictionary.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "spell_tv.h"
#undef THOT_EXPORT
#define THOT_EXPORT
#include "word_tv.h"

extern UCHAR_T Code[256];
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
#ifdef __STDC__
ThotBool            TtaLoadDocumentDictionary (PtrDocument document, int *pDictionary, ThotBool ToCreate)
#else  /* __STDC__ */
ThotBool            TtaLoadDocumentDictionary (document, pDictionary, ToCreate)
PtrDocument         document;
int                *pDictionary;
ThotBool            ToCreate;
#endif /* __STDC__ */
{
  STRING            extenddic;
  CHAR_T               path[MAX_PATH], dictname[MAX_PATH];

  /* dictionary name = document name */
  *pDictionary = (int) NULL;
  
  extenddic = TtaGetEnvString ("EXTENDDICT");
  if (extenddic != NULL)
    {
      TtaExtractName (extenddic, path, dictname);
      if (dictname[0] == EOS)
	ustrcpy (dictname, TEXT("dictionary.DCT"));
    }
  else
    {
      path[0] = EOS;
      ustrcpy (dictname, TEXT("dictionary.DCT"));
    }

  if (path[0] == EOS ||  !TtaCheckDirectory (path))
    ustrcpy (path, TtaGetEnvString ("APP_HOME"));
  LoadTreatedDict ((PtrDict *) pDictionary, 0, document, dictname,
		   path, FALSE, ToCreate);
  return (*pDictionary != EOS);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         Asci2Code (STRING string)
#else  /* __STDC__ */
static void         Asci2Code (string)
STRING              string;
#endif /* __STDC__ */
{
   int                 i;

   i = 0;
   while (string[i] != EOS)
     {
	string[i] = Code[(UCHAR_T) string[i]];
	i++;
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         Code2Asci (STRING string)
#else  /* __STDC__ */
static void         Code2Asci (string)
STRING              string;
#endif /* __STDC__ */
{
   int                 i = 0;

   while (string[i] != EOS)
     {
	string[i] = ReverseCode[(UCHAR_T) string[i]];
	i++;
     }
}

#ifdef IV
/*----------------------------------------------------------------------
  minMAJ returns TRUE if the string doesn't contains only lowercases 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            minMAJ (STRING string)
#else  /* __STDC__ */
ThotBool            minMAJ (string)
STRING              string;
#endif /* __STDC__ */
{
   int                 maj = 0;
   int                 i = 0;
   CHAR_T                c;

   while ((c = string[i]) != EOS && (maj == 0))
     {
	maj = isimaj (string[i]);
	i++;
     }
   return (maj == 1) ? TRUE : FALSE;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T                tomin (CHAR_T caract)
#else  /* __STDC__ */
CHAR_T                tomin (caract)
CHAR_T                caract;
#endif /* __STDC__ */
{
   CHAR_T                c;

   c = caract;
   if (isimaj (caract) != 0)
      c = tolower (caract);

   return (c);
}
#endif /* IV */

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         SetUpperCase (STRING string)
#else  /* __STDC__ */
void         SetUpperCase (string)
STRING              string;
#endif /* __STDC__ */
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
#ifdef __STDC__
void         SetCapital (STRING string)
#else  /* __STDC__ */
void         SetCapital (string)
STRING              string;
#endif /* __STDC__ */
{
   if (isimin (string[0]) != 0)
      string[0] = toupper (string[0]);

}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool     IsUpperCase (STRING string)
#else  /* __STDC__ */
ThotBool     IsUpperCase (string)
STRING              string;
#endif /* __STDC__ */
{
   int                 maj = 1;
   int                 i = 0;
   CHAR_T                c;

   while ((c = string[i]) != EOS && (maj != 0))
     {
	maj = isimaj (string[i]);
	i++;
     }
   return (maj == 0) ? FALSE : TRUE;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool     IsCapital (STRING string)
#else  /* __STDC__ */
ThotBool     IsCapital (string)
STRING              string;
#endif /* __STDC__ */
{
   int                 cap = 0;
   int                 i = 0;
   CHAR_T                c;

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
#ifdef __STDC__
ThotBool            IsIso (STRING string)
#else  /* __STDC__ */
ThotBool            IsIso (string)
STRING              string;
#endif /* __STDC__ */
{
   int                 iso = 1;
   int                 i = 0;

   while (string[i] != EOS && (iso != 0))
     {
	iso = isalphiso (string[i]) || string[i] == TEXT('-') || string[i] == TEXT('\'');
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
#ifdef __STDC__
int                 WordInDictionary (CHAR_T word[MAX_WORD_LEN], PtrDict dict)
#else  /* __STDC__ */
int                 WordInDictionary (word, dict)
CHAR_T                word[MAX_WORD_LEN];
PtrDict             dict;
#endif /* __STDC__ */
{
   int                 inf, sup, med, rescomp, size;

   if (dict == NULL)
     /* dictionnaire n'existe pas */
     return (-2);
   else if (dict->DictNbWords < 0)
     /* dictionnaire vide */
     return (-1);

   size = ustrlen (word);
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
	rescomp = ustrcmp (&dict->DictString[dict->DictWords[med]], word);
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
#ifdef __STDC__
int                 CheckWord (CHAR_T word[MAX_WORD_LEN], Language language, PtrDict dict)
#else  /* __STDC__ */
int                 CheckWord (word, language, dict)
CHAR_T                word[MAX_WORD_LEN];
Language            language;
PtrDict             dict;
#endif /* __STDC__ */
{
   ThotBool            present = TRUE;
   int                 res;
   CHAR_T                word1[MAX_WORD_LEN];
   CHAR_T                wordmin[MAX_WORD_LEN];
   PtrDict             globalDict;
   PtrDict             personalDict;
   PtrDict             dictsigle;
   PtrDict             dictname;

   /* On refuse de corriger l'ISOlatin-1 */
   if (language == 0)
      return (-1);

   if (word[0] == EOS)
      return (0);		/* mot vide */

   if (ustrlen (word) >= 2)
     {
	ustrcpy (word1, word);
	SmallLettering (word1);
	ustrcpy (wordmin, word1);
	Asci2Code (word1);

	globalDict = (PtrDict) TtaGetPrincipalDictionary (language);
	personalDict = (PtrDict) TtaGetSecondaryDictionary (language);
	res = WordInDictionary (word1, dict);

	if (res != -3 || res == -2)
	   /* mot absent de dict ou dict vide ou pas de dict */
	  {
	     res = WordInDictionary (word1, globalDict);
	     if (res == -2)
		return (-1);	/* PAS de globalDict */
	     else
	       {
		  if (res != -3)	/* mot absent de globalDict */
		    {		/* word1 n'appartient pas a ces 2 dicts obligatoires */
		       present = FALSE;
		       dictsigle = (PtrDict) TtaGetPrincipalDictionary (0);
		       dictname = (PtrDict) TtaGetSecondaryDictionary (0);
		       if (WordInDictionary (word1, personalDict) == -3
			   || WordInDictionary (word1, dictsigle) == -3
			   || WordInDictionary (word1, dictname) == -3)
			  /* word1 appartient a l'un de ces dict facultatifs */
			  present = TRUE;
		    }
	       }
	  }

	if (present == FALSE)
	  {			/* calculer le type du mot errone' */
	     if (IsUpperCase (word) == TRUE)
		type_err = 3;	/* MAJ */
	     else
	       {
		  if (IsCapital (word) == TRUE)
		     type_err = 2;	/* Capitale */
		  else
		     type_err = 1;	/* SmallLettering ou melange */
	       }
	     /* recopier ce mot errone' dans ChkrErrWord */
	     ustrcpy (ChkrErrWord, word);
	  }
     }
   res = (present == TRUE) ? 1 : 0;
   return (res);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          Insert (int x, int pWord, PtrDict dict)
#else  /* __STDC__ */
static int          Insert (x, pWord, dict)
int                 x;
int                 pWord;
PtrDict             dict;
#endif /* __STDC__ */
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
#ifdef __STDC__
static int          InsertWord (PtrDict dict, CHAR_T word[MAX_WORD_LEN])
#else  /* __STDC__ */
static int          InsertWord (dict, word)
PtrDict             dict;
CHAR_T                word[MAX_WORD_LEN];
#endif /* __STDC__ */
{
   int                 size, place, i, k;

   place = WordInDictionary (word, dict);
   if (place == -2)
      return (-2);		/* dictionnaire inaccessible */
   if (place == -3)
      return (0);		/* mot deja present dans le dictionnaire */
   if (place == -1)		/* dictionnaire vide */
      place = 0;		/* indice pour inserer dans ce dictionaire */
   size = ustrlen (word) + 1;
   if ((dict->DictNbWords >= dict->DictMaxWords - 1) || (dict->DictNbChars >= dict->DictMaxChars + size))
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
	ustrcpy (&dict->DictString[dict->DictWords[place]], word);
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
#ifdef __STDC__
static void         Cmp (CHAR_T wordtest[], PtrDict dict)
#else  /* __STDC__ */
static void         Cmp (wordtest, dict)
CHAR_T                wordtest[];
PtrDict             dict;
#endif /* __STDC__ */
{
  int                 dist[MAX_WORD_LEN][MAX_WORD_LEN];
  UCHAR_T       wordcmp[MAX_WORD_LEN];
  int                 Lg, idx, sup, pWord, seuilCourant;
  int                 i, j, k, x, y, z;
  int                 difference, iteration, size, largeur, word;
  UCHAR_T       currentWord[MAX_WORD_LEN];
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
  
  ustrcpy (wordcmp, wordtest);
  SmallLettering (wordcmp);
  Asci2Code (wordcmp);
  Lg = ustrlen (wordcmp);
  seuilCourant = Seuil[Lg];
  largeur = Delta[Lg];
  
  /* parcours du dictionnaire
     avec d'abord les mots de meme size,
     puis EVENTUELLEMENT les mots de +- 1 lettre
     puis EVENTUELLEMENT les mots de +- 2 lettres etc.
     */
  difference = 0;
  for (iteration = 0; iteration <= 2 * largeur; iteration++)
    {
      difference = (difference > 0) ? difference - iteration : difference + iteration;
      size = Lg - difference;
      if (size >= 0 && size < MAX_WORD_LEN)
	{
	  /*
	    determination si le calcul des mots de cette size doivent etre calcules. 
	    dist_mini determine le cout minimum obligatoire pour cette size de mot 
	    si ce cout est deja superieur au seuil courant, il est inutile de traiter
	    ces mots
	  */
	  dist_mini = (difference > 0) ? difference * KI : -difference * KO;
	  if (dist_mini > seuilCourant)
	    continue;
	  
	  /* calcul des indices de debut et fin de dictionnaire */
	  word = dict->DictLengths[size];
	  idx = dict->DictWords[word];
	  if (size >= MAX_WORD_LEN || (dict->DictLengths[size + 1] - 1 > dict->DictNbWords))
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
	      if (i <= Lg)
		dist[i][j] = seuilCourant + 1;
	    }
	  
	  /* parcours du dictionnaire */
	  while (idx < sup)
	    {
	      pWord = idx;
	      k = dict->DictCommon[word++];
	      /* si le calcul du mot precedent a ete stoppe a l'indice derniere_ligne
		 et que le mot courant possede un nombre de lettres communes superieur
		 a cette valeur, il est inutile de faire le calcul */
	      if (k <= derniere_ligne)
		{
		  ustrcpy (currentWord, &dict->DictString[pWord]);
		  
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
		      if (fin > Lg)
			fin = Lg;
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

   ustrcpy (ChkrCorrection[0], ChkrErrWord);
   for (i = 0; i < NC; i++)
     {
	pWord = WordsList[i];
	if (pWord >= 0)
	  {
	     ustrcpy (ChkrCorrection[i + 1], &DictsList[i]->DictString[pWord]);
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
#ifdef __STDC__
static void         SaveDictFile (PtrDict docDict)
#else  /* __STDC__ */
static void         SaveDictFile (docDict)
PtrDict             docDict;
#endif /* __STDC__ */
{
   FILE               *f;
   int                 i, j;
   CHAR_T                tempbuffer[THOT_MAX_CHAR];
   CHAR_T                word[MAX_WORD_LEN];

   FindCompleteName (docDict->DictName, _EMPTYSTR_, docDict->DictDirectory, tempbuffer, &i);
   if (docDict->DictNbWords >= 0)
     {
	f = ufopen (tempbuffer, _WriteMODE_);
	if (f != NULL)
	  {
	    /* enregistrer d'abord nb words and nb chars effectifs */
	     i = docDict->DictNbWords;
	     j = docDict->DictNbChars;
	     fprintf (f, "%d %d\n", i, j);

	     for (i = 0; i <= docDict->DictNbWords; i++)
	       {
		  ustrcpy (word, &docDict->DictString[docDict->DictWords[i]]);
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
#ifdef __STDC__
void                AddWord (CHAR_T word[MAX_WORD_LEN], PtrDict * pDict)
#else  /* __STDC__ */
void                AddWord (word, pDict)
CHAR_T                word[MAX_WORD_LEN];
PtrDict            *pDict;
#endif /* __STDC__ */
{
   CHAR_T                word1[MAX_WORD_LEN];
   int                 ret;
   Name                DiNom;
   ThotBool            OKinsere = TRUE;
   PtrDict             docDict;

   docDict = *pDict;
   ustrcpy (word1, word);
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
		    ustrcpy (DiNom, docDict->DictName);
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
static void            InitChecker ()
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
      ustrcpy (ChkrCorrection[j],TEXT("$"));
}



/*----------------------------------------------------------------------
   GiveProposal          
   met dans ChkrCorrection les propositions de correction du mot       
   qui se trouve dans ChkrErrWord                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GiveProposal (Language language, PtrDict docDict)
#else  /* __STDC__ */
void                GiveProposal (language, docDict)
Language            language;
PtrDict             docDict;
#endif /* __STDC__ */
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
#ifdef __STDC__
static void         init_Tsub (FILE * ftsub)
#else  /* __STDC__ */
static void         init_Tsub (ftsub)
FILE               *ftsub;
#endif /* __STDC__ */
{
   int                 i, j;
   UCHAR_T       ch1[80], ch2[80], ch3[80];

   /* initialisation de Tsub */
   for (i = 0; i < NbLtr; i++)
      for (j = 0; j < NbLtr; j++)
	 Tsub[i][j] = KD;
   while (fscanf (ftsub, "%s%s%s", ch1, ch2, ch3) != EOF)
     {
	int                 coeff;
	UCHAR_T       x, y, valeur;

	usscanf (ch1, TEXT("%c"), &x);
	usscanf (ch2, TEXT("%c"), &y);
	usscanf (ch3, TEXT("%c"), &valeur);
	switch (valeur)
	      {
		 case TEXT('b'):
		    coeff = KB;
		    break;
		 case TEXT('m'):
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
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DefaultParams (int lettres)
#else  /* __STDC__ */
static void         DefaultParams (lettres)
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
}


/*----------------------------------------------------------------------
   ParametrizeChecker retourne  0 en cas d'impossibilite d'initialisation  
   1 si OK                                         
   positionne Clavier_charge                                   
  ----------------------------------------------------------------------*/
int                 ParametrizeChecker ()
{
   int                 ret;
   FILE               *fparam;
   Buffer              paramnom;
   FILE               *ftsub;
   Buffer              clavnom;
   STRING              corrpath;

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
	     ustrcpy (paramnom, corrpath);
	     ustrcat (paramnom, TEXT("/param"));
	     if ((fparam = fopen (paramnom, "r")) != NULL)
	       /* Existence du fichier */
		init_param (fparam);
	     else
	       {
		  /* valeur par defaut LGR = 4 */
		  DefaultParams (4);
	       }

	     /* Lecture du  fichier clavier */
	     ustrcpy (clavnom, corrpath);
	     ustrcat (clavnom, TEXT("/clavier"));
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
#ifdef __STDC__
void                WordReplace (CHAR_T orgWord[MAX_WORD_LEN], CHAR_T newWord[MAX_WORD_LEN])
#else  /* __STDC__ */
void                WordReplace (orgWord, newWord)
CHAR_T                orgWord[MAX_WORD_LEN];
CHAR_T                newWord[MAX_WORD_LEN];
#endif /* __STDC__ */
{
   int                 idx;
   int                 stringLength;	/* longueur de cette chaine */
   CHAR_T                pChaineRemplace[MAX_WORD_LEN]; /* la chaine de remplacement */
   int                 LgChaineRempl;	/* longueur de cette chaine */

   /* remplacer le mot errone par le mot corrige */
   stringLength = ustrlen (orgWord);
   /* initialiser LgChaineRempl et pChaineRemplace */
   LgChaineRempl = ustrlen (newWord);
   ustrcpy (pChaineRemplace, newWord);

   /* substitue la nouvelle chaine et la selectionne */
   if (ChkrRange->SStartToEnd)
     {
	idx = ChkrIndChar - stringLength + 1;
	OpenHistorySequence (ChkrRange->SDocument, ChkrElement, ChkrElement,
			     idx, idx+stringLength-1);
	AddEditOpInHistory (ChkrElement, ChkrRange->SDocument, TRUE, TRUE);
	CloseHistorySequence (ChkrRange->SDocument);
	ReplaceString (ChkrRange->SDocument, ChkrElement, idx,
		       stringLength, pChaineRemplace, LgChaineRempl, TRUE);
	/* met a jour ChkrIndChar */
	ChkrIndChar = idx + LgChaineRempl - 1;

	/* met eventuellement a jour la borne de fin du domaine de recherche */
	if (ChkrElement == ChkrRange->SEndElement)
	   /* la borne est dans l'element ou` on a fait le remplacement */
	   if (ChkrRange->SEndChar != 0)
	      /* la borne n'est pas a la fin de l'element, on decale la borne */
	      ChkrRange->SEndChar += LgChaineRempl - stringLength;
     }
   else
     {
	idx = ChkrIndChar + 1;
	OpenHistorySequence (ChkrRange->SDocument, ChkrElement, ChkrElement,
			     idx, idx+stringLength-1);
	AddEditOpInHistory (ChkrElement, ChkrRange->SDocument, TRUE, TRUE);
	CloseHistorySequence (ChkrRange->SDocument);
	ReplaceString (ChkrRange->SDocument, ChkrElement, idx,
		       stringLength, pChaineRemplace, LgChaineRempl, TRUE);
     }

   /* met eventuellement a jour la selection initiale */
   UpdateDuringSearch (ChkrElement, LgChaineRempl - stringLength);
}


/*----------------------------------------------------------------------
   CheckChangeSelection retourne vrai si la selection a change      
   depuis la derniere recherche d'erreur.                       
  ----------------------------------------------------------------------*/
ThotBool            CheckChangeSelection ()
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
}


/*----------------------------------------------------------------------
   CheckCharList teste si ce caractere appartient a la liste listcar    
   retourne TRUE si oui                               
   FALSE sinon                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     CheckCharList (CHAR_T car, STRING listcar)
#else  /* __STDC__ */
static ThotBool     CheckCharList (car, listcar)
CHAR_T                car;
STRING              listcar;
#endif /* __STDC__ */
{
   int                 i;

   for (i = 0; (size_t) i < ustrlen (listcar); i++)
     {
	if (car == listcar[i])
	   return (TRUE);
     }
   return (FALSE);
}

/*----------------------------------------------------------------------
   ACeCar retourne TRUE si le mot contient un des caracteres       
   contenu dans ListCar.                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     ACeCar (CHAR_T word[MAX_WORD_LEN])
#else  /* __STDC__ */
static ThotBool     ACeCar (word)
CHAR_T                word[MAX_WORD_LEN];
#endif /* __STDC__ */
{

   ThotBool            result;
   int                 i;
   int                 longueur;

   result = FALSE;
   longueur = ustrlen (word);
   if (longueur > 0)
     {
	for (i = 0; i < longueur && (result == FALSE); i++)
	   if (CheckCharList (word[i], RejectedChar) == TRUE)
	      result = TRUE;
     }
   return result;
}

/*----------------------------------------------------------------------
   IncludeANumber                                                       
   retourne TRUE si le mot contient au moins un chiffre arabe.     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     IncludeANumber (CHAR_T word[MAX_WORD_LEN])
#else  /* __STDC__ */
static ThotBool     IncludeANumber (word)
CHAR_T                word[MAX_WORD_LEN];
#endif /* __STDC__ */
{

   ThotBool            result;
   int                 i;
   int                 longueur;

   result = FALSE;
   longueur = ustrlen (word);
   if (longueur > 0)
     {
	for (i = 0; i < longueur && (result == FALSE); i++)
	   if (word[i] >= TEXT('0') && word[i] <= TEXT('9'))
	      result = TRUE;
     }
   return result;
}

/*----------------------------------------------------------------------
   IsANumber retourne TRUE si le mot est forme' uniquement de    
   chiffres decimaux arabes.                                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     IsANumber (CHAR_T word[MAX_WORD_LEN])
#else  /* __STDC__ */
static ThotBool     IsANumber (word)
CHAR_T                word[MAX_WORD_LEN];
#endif /* __STDC__ */
{

   ThotBool            result;
   int                 i;
   int                 longueur;

   result = FALSE;
   longueur = ustrlen (word);
   if (longueur > 0)
     {

	result = TRUE;
	for (i = 0; i < longueur && result; i++)
	   if (word[i] < TEXT('0') || word[i] > TEXT('9'))
	      result = FALSE;
     }
   return result;
}

/*----------------------------------------------------------------------
   InRoman retourne TRUE si le mot est forme' uniquement de       
   chiffres romains.                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     InRoman (CHAR_T word[MAX_WORD_LEN])
#else  /* __STDC__ */
static ThotBool     InRoman (word)
CHAR_T                word[MAX_WORD_LEN];
#endif /* __STDC__ */
{
   /* description des chiffres romains (majuscule) */
   static CHAR_T         NRomain[] = {
#         if defined(_I18N_) || defined(__JIS__)
          L'M', L'C', L'D', L'L', L'X', L'V', L'I'
#         else /* defined(_I18N_) || defined(__JIS__) */
          'M', 'C', 'D', 'L', 'X', 'V', 'I'
#         endif /* defined(_I18N_) || defined(__JIS__) */    
   };
   static CHAR_T         NRomainIsole[] = {
#         if defined(_I18N_) || defined(__JIS__)
          L'C', L'L', L'V'
#         else /* defined(_I18N_) || defined(__JIS__) */
          'C', 'L', 'V'
#         endif /* defined(_I18N_) || defined(__JIS__) */    
   };

   ThotBool            result;
   int                 i, j, nbcar;
   int                 longueur, lg1;
   CHAR_T                cecar;

   result = FALSE;
   longueur = ustrlen (word);
   if (longueur > 0)
     {
	result = TRUE;
	for (i = 0; i < longueur && result; i++)
	   if (CheckCharList (word[i], NRomain) != TRUE)
	      result = FALSE;
     }
   if (result == TRUE)
      /* analyse plus fine de ce "possible" chiffre romain */
     {
	/* pas plus de 3 fois la meme lettre successivement dans ce nombre */
	lg1 = longueur - 3;
	for (i = 0; i < lg1 && result; i++)
	  {
	     cecar = word[i];
	     nbcar = 1;
	     for (j = i + 1; j < longueur && word[j] == cecar; j++)
		nbcar++;
	     if (nbcar > 3)
		/* ce n'est pas un "bon" chiffre romain */
		result = FALSE;
	  }

	/* pas plus de 1 fois V, L ou D successivement dans ce nombre */
	for (i = 0; i < longueur - 1 && result; i++)
	  {
	     if (CheckCharList (word[i], NRomainIsole) == TRUE)
	       {
		  if (word[i + 1] == word[i])
		     result = FALSE;
	       }
	  }

	/* ne pas considerer "M" comme un romain */
 if (ustrlen (word) == 1 && word[0] == TEXT('M'))
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
#ifdef __STDC__
static ThotBool     IgnoreWord (CHAR_T word[MAX_WORD_LEN])
#else  /* __STDC__ */
static ThotBool     IgnoreWord (word)
CHAR_T                word[MAX_WORD_LEN];
#endif /* __STDC__ */
{

   ThotBool            result = FALSE;

   /* les mots en capitale */
   if (IgnoreUppercase)
      if (IsUpperCase (word))
	 return (TRUE);

   /* les mots contenant un chiffre arabe */
   if (IgnoreArabic)
      if (IncludeANumber (word))
	 return (TRUE);

   /* les chiffres romains */
   if (IgnoreRoman)
      if (InRoman (word))
	 return (TRUE);

   /* les mots contenant au moins l'un des caracteres de RejectedChar[] */
   if (IgnoreSpecial)
      result = ACeCar (word);

   return result;
}


/*----------------------------------------------------------------------
   NextSpellingError retourne le mot errone' suivant et le           
   selectionne dans la vue courante du document.                
   Le mot errone' est mis dans ChkrErrWord.                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                NextSpellingError (CHAR_T word[MAX_WORD_LEN], PtrDict docDict)
#else  /* __STDC__ */
void                NextSpellingError (word, docDict)
CHAR_T                word[MAX_WORD_LEN];
PtrDict             docDict;

#endif /* __STDC__ */
{
   Language            language;
   ThotBool            ok, novalid;
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
		ok = SearchNextWord (&ChkrElement, &ChkrIndChar, word, ChkrRange);
	     else
		ok = SearchPreviousWord (&ChkrElement, &ChkrIndChar, word, ChkrRange);
	     /* Is it a valid selection ? */
	     if (ok && ChkrElement->ElParent != NULL)
		novalid = TypeHasException (ExcNoSpellCheck, ChkrElement->ElParent->ElTypeNumber, ChkrElement->ElParent->ElStructSchema);
	  }

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

   i = ustrlen (word);
   if (i > 0)
     {
	/* on a trouve un mot */
	/* selectionner le mot a corriger */
	if (ChkrRange->SStartToEnd)
	   SelectString (ChkrRange->SDocument, ChkrElement, ChkrIndChar - i + 1, ChkrIndChar);
	else
	   SelectString (ChkrRange->SDocument, ChkrElement, ChkrIndChar + 1, ChkrIndChar + i);
     }
   ustrcpy (ChkrErrWord, word);
}
