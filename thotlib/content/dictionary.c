
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*=======================================================================*/
/*|                                                                     | */
/*|     Thot Toolkit: Application Program Interface                     | */
/*|     --->Dictionnary managment                                       | */
/*|                                                                     | */
/*|                     I. Vatton       November 93                     | */
/*|                                                                     | */
/*=======================================================================*/

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "application.h"
#include "language.h"
#include "constmenu.h"
#include "storage.h"
#include "typecorr.h"
#include "libmsg.h"
#include "message.h"
#include "dictionary.h"

#define MAX_DICOS   2		/* nombre max de dictionnaires associes a une langue */
#define MAXLIGNE   80		/* longueur d'une ligne dans le dictionnaire */
#define MaxDicos   15		/* nombre max de dictionnaires simultanes */

extern struct Langue_Ctl LangTable[MAX_LANGUAGES];
extern struct Langue_Ctl TypoLangTable[MAX_LANGUAGES];
extern int          FreeEntry;

static char        *dicopath;	/* variable d'environnement DICOPAR */
static boolean      Alphabet_charge;
static unsigned     invcode[NbLtr];	/* code inverse */
static PtrDico      TabDicos[MaxDicos];

/* Valeurs exportables */
unsigned char       Code[256];	/* code des caracteres de l'alphabet */

/* procedures importees */
#include "environ.f"
#include "memory.f"
#include "storage.f"
#include "thotmsg.f"
#include "dofile.f"
#include "filesystem.f"

/***************************** ALPHABET ************************************/
boolean             Corr_alphabet ()
{
   FILE               *falpha;
   Buffer              alphanom;
   boolean             ret = False;
   unsigned char       x;
   int                 i;

   if (dicopath != NULL)
      strcpy (alphanom, dicopath);
   else
      strcpy (alphanom, "");

   strcat (alphanom, "/alphabet");	/* alphabet iso */
   if ((falpha = fopen (alphanom, "r")) != NULL)
     {
	ret = True;
	for (i = 0; i < 256; i++)
	   Code[i] = (unsigned char) 100;
	i = 1;
	while ((fscanf (falpha, "%c ", &x) != EOF) && (i < NbLtr))
	  {
	     Code[x] = (unsigned char) i;
	     invcode[i++] = (unsigned char) x;
	  }
	fclose (falpha);
     }
   return (ret);
}

/***************************** asci2code *********************************/
#ifdef __STDC__
void                asci2code (char *chaine)
#else  /* __STDC__ */
void                asci2code (chaine)
char               *chaine;

#endif /* __STDC__ */
{
   int                 i;

   i = 0;
   while (chaine[i] != '\0')
     {
	chaine[i] = Code[(unsigned char) chaine[i]];
	i++;
     }
}				/*asci2code */


/***************************** code2asci *********************************/
#ifdef __STDC__
void                code2asci (char *chaine)
#else  /* __STDC__ */
void                code2asci (chaine)
char               *chaine;

#endif /* __STDC__ */
{
   int                 i = 0;

   while (chaine[i] != '\0')
     {
	chaine[i] = invcode[(unsigned char) chaine[i]];
	i++;
     }
}				/*code2asci */


/************************ Corr_pretraitement *****************************/
/* 
   Pretraitement du dictionnaire pour determiner les lettres
   communes entre deux mots consecutifs
 */
#ifdef __STDC__
void                Corr_pretraitement (PtrDico dico)
#else  /* __STDC__ */
void                Corr_pretraitement (dico)
PtrDico             dico;

#endif /* __STDC__ */
{
   int                 mot, i;
   char                dernier_mot[MAX_WORD_LEN];
   char                mot_courant[MAX_WORD_LEN];

   /* pas de pretraitement sur un dico vide  (nbmots = -1) */
   if (dico->nbmots >= 0)
     {
	dernier_mot[0] = 0;
	mot_courant[0] = 0;

	for (mot = 0; mot < dico->nbmots; mot++)
	  {
	     int                 k = 0;

	     strcpy (dernier_mot, mot_courant);
	     strcpy (mot_courant, &dico->chaine[dico->pdico[mot]]);

	     if (strlen (dernier_mot) != strlen (mot_courant))
	       {
		  /* changement de taille de mot */
		  /* => pas calcul de lettres communes */
		  dico->commun[mot] = 1;
	       }
	     else
	       {
		  /* recherche des lettres communes entre deux mots consecutifs */
		  /* afin de ne pas refaire des calculs */
		  while (mot_courant[k] == dernier_mot[k])
		     k++;
		  dico->commun[mot] = k + 1;
	       }
	  }
	for (i = mot; i < dico->MAXmots; i++)
	   dico->commun[i] = 1;	/* pour finir en beaute !!! */
     }
}				/*Corr_pretraitement */


/* ---------------------------------------------------------------------  */
/* |    LibDictionnaire libere le dictionnaire dont le descripteur est  | */
/* |             pointe par pDico.                                      | */
/* ---------------------------------------------------------------------  */
#ifdef __STDC__
static void         LibDictionnaire (PtrDico * pDico)
#else  /* __STDC__ */
static void         LibDictionnaire (pDico)
PtrDico            *pDico;

#endif /* __STDC__ */
{
   int                 d;

   if (*pDico != NULL)
      /* cherche dans la table le descripteur du dictionnaire a liberer */
     {
	d = 0;
	while (d < MaxDicos && TabDicos[d] != *pDico)
	   d++;
	if (TabDicos[d] == *pDico)
	  {
	     /* liberer la chaine et la liste des mots ... */
	     FreeChaine (*pDico);
	     FreeDico (*pDico);
	     TabDicos[d] = NULL;
	     *pDico = NULL;	/* Ah mais! */
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* CreeDictionnaire   acquiert et initialise un contexte de dictionnaire. */
/* Au retour, pDico contient le pointeur sur le contexte de dictionnaire. */
/* ou NULL si manque de memoire                                           */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         CreeDictionnaire (PtrDico * pDico, PtrDocument document)
#else  /* __STDC__ */
static void         CreeDictionnaire (pDico, document)
PtrDico            *pDico;
PtrDocument         document;

#endif /* __STDC__ */
{
  int                 d;

  /* cherche un pointeur de descripteur de dictionnaire libre */
  d = 0;
  while (d < MaxDicos && TabDicos[d] != NULL)
    d++;

  if (d < MaxDicos && TabDicos[d] != NULL)
    {
      /* il faut faire de la place  */
      /* en vidant un dictionnnaire FICHIER inutilise */
      d = 0;
	/* de preference utilise par un autre document */
      while (d < MaxDicos && TabDicos[d]->DicoDoc == document)
	d++;
      
      if (d == MaxDicos || TabDicos[d]->DicoReadOnly == True)
	{
	  /* rechercher un dico FICHIER */
	  d = 0;
	  while (d < MaxDicos && TabDicos[d]->DicoReadOnly == True)
	    d++;
	}
	/* vider ce dictionnaire FICHIER  */
      if (d < MaxDicos)	
	{
	  TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_LOAD_DICO_ERROR),
			     TabDicos[d]->DicoNom);
	  LibDictionnaire (&TabDicos[d]);
	  
	}
     }				/* end of if (TabDicos[d] != NULL) */
  
  if (d < MaxDicos)
    
    {
      /* acquiert un descripteur de dictionnaire */
      GetDico (&TabDicos[d]);
      *pDico = TabDicos[d];
      (*pDico)->DicoDoc = document;
    }
}				/*CreeDictionnaire */


/* ---------------------------------------------------------------------- */
/* |    ChercheNomDico recherche le dictionnaire dont le nom est donne  | */
/* |      et retourne dans pDico le pointeur sur son descripteur ou NULL| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         ChercheNomDico (PtrDico * pDico, char *dinom, char *dicodir)
#else  /* __STDC__ */
static void         ChercheNomDico (pDico, dinom, dicodir)
PtrDico            *pDico;
char               *dinom;
char               *dicodir;

#endif /* __STDC__ */
{
   int                 d;
   boolean             trouve;

   trouve = False;
   d = 0;
   while (d < MaxDicos && (TabDicos[d] != NULL) && (!trouve))
     {
	trouve = (strcmp (TabDicos[d]->DicoNom, dinom) == 0
		  && strcmp (TabDicos[d]->DicoDirectory, dicodir) == 0);
	/* on pourrait ajouter d'autres tests : langue... */
	d++;
     }
   if (trouve)
      *pDico = TabDicos[d - 1];
   else
      *pDico = NULL;
}				/*ChercheNomDico */


/* ---------------------------------------------------------------------- */
/* |    TestDico verifie si le fichier de nom DiNom existe              | */
/* |   retourne -1 si le fichier est absent (inaccessible)              | */
/* |   retourne 0 si le fichier .DIC existe (non traite)                | */
/* |   retourne 1 si le fichier .dic existe (traite)                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static int          TestDico (char *DiNom, char *dicodir)
#else  /* __STDC__ */
static int          TestDico (DiNom, dicodir)
char               *DiNom;
char               *dicodir;

#endif /* __STDC__ */
{
   int                 ret, i;
   char                tempbuffer[MAX_CHAR];

   DoFileName (DiNom, "dic", dicodir, tempbuffer, &i);
   if (FileExist (tempbuffer) == 0)	/* fichier inconnu */
     {
	/* chercher un dictionnaire NON pre-traite */
	DoFileName (DiNom, "DIC", dicodir, tempbuffer, &i);
	if (FileExist (tempbuffer) == 0)
	  {			/* fichier .DIC inconnu */
	     /* chercher un dictionnaire LEX non pre-traite */
	     DoFileName (DiNom, "LEX", dicodir, tempbuffer, &i);
	     if (FileExist (tempbuffer) == 0)	/* fichier inconnu */
		ret = -1;	/* fichier inconnu */
	     else
		ret = 2;	/* le fichier .LEX existe */
	  }
	else
	   ret = 0;		/* le fichier .DIC existe */
     }
   else
      ret = 1;			/* fichier .dic */
   return (ret);
}				/*TestDico */


/****************************** creer *************************************/
/* un dictionnaire a partir d'un fichier pretraite'                       */
/* PROCEDURE PORTABLE SUR VAX                                             */
/**************************************************************************/
#ifdef __STDC__
static int          creer (FILE * fdico, PtrDico dict)
#else  /* __STDC__ */
static int          creer (fdico, dict)
FILE               *fdico;
PtrDico             dict;

#endif /* __STDC__ */
{
   int                 i;

   /* suite du chargement ... */
   i = 1;
   while (i <= dict->nbcars)
     {
	BIOreadByte (fdico, &(dict->chaine[i - 1]));
	i++;
     }

   i = 1;
   while (i <= dict->nbmots)
     {
	BIOreadByte (fdico, &(dict->commun[i - 1]));
	i++;
     }

   for (i = 0; i < dict->nbmots; i++)
      BIOreadInteger (fdico, &dict->pdico[i]);

   for (i = 0; i < MAX_WORD_LEN; i++)
      BIOreadInteger (fdico, &dict->plgdico[i]);

   /* chargement termine */
   dict->DicoCharge = True;
   return (1);			/* OK */
}				/* end of creer */


/********************************** charger ******************************/
/*  retourne 1 si le chargement est reussi ou 0 sinon                    */
/*************************************************************************/
#ifdef __STDC__
static int          charger (FILE * fdico, PtrDico dict)
#else  /* __STDC__ */
static int          charger (fdico, dict)
FILE               *fdico;
PtrDico             dict;

#endif /* __STDC__ */
{
   char                motlu[MAX_WORD_LEN];
   char                lignelue[MAXLIGNE];
   char               *plignelue;
   int                 i, k, Taille, nblu, dermot;
   int                 Max_Car, Max_Mot;
   int                 TailleCourante = 0;
   int                 nbcar = 0;	/* Nb de caracteres du dictionnaire */

   /* dictionnaire en cours de chargement ... */
   Max_Car = dict->MAXcars;
   Max_Mot = dict->MAXmots;
   plignelue = &lignelue[0];	/* pointeur sur le premier caractere lu */

   /* Chargement du dico */
   while (fgets (plignelue, MAXLIGNE, fdico) != NULL)
     {
	nblu = sscanf (plignelue, "%s", motlu);
	if ((nblu > 0)
	    && (dict->nbmots < Max_Mot - 1)
	    && ((Taille = strlen (motlu)) < MAX_WORD_LEN)
	    && (Taille + nbcar + 1 < Max_Car - 1))
	  {
	     dict->nbmots++;
	     plignelue = plignelue + Taille;
	     dict->pdico[dict->nbmots] = nbcar;
	     if (Taille != TailleCourante)
	       {
		  for (k = TailleCourante + 1; k <= Taille; k++)
		     dict->plgdico[k] = dict->nbmots;
		  TailleCourante = Taille;
	       }
	     for (k = 0; k < Taille; k++)
		dict->chaine[nbcar++] = (char) Code[(unsigned char) motlu[k]];
	     dict->chaine[nbcar++] = '\0';	/* marqueur de fin de mot */

	     /* passer au mot suivant: lire ligne suivante */
	     plignelue = &lignelue[0];	/* pointeur sur le premier caractere lu */
	  }
	else if (nblu != -1)	/* ce n'est pas la fin du dico */
	      {
		/* impossible de charger ce dictionnaire */
		TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_LOAD_DICO_ERROR),
				   dict->DicoNom);
		/* liberer le dico */
		LibDictionnaire (&dict);	/* => dict = nil */
		return (0);
	      }
     }

   /* creation d'un mot vide a la fin du dictionnaire */
   dermot = dict->nbmots + 1;
   dict->pdico[dermot] = nbcar;
   /* mise a jour des pointeurs */
   for (i = TailleCourante + 1; i < MAX_WORD_LEN; i++)
      dict->plgdico[i] = dermot;
   dict->chaine[nbcar] = '\0';
   dict->nbcars = nbcar;
   dict->DicoCharge = True;
   return (1);
}				/*charger */


/* ---------------------------------------------------------------------- */
/* | PrepDictionnaire : cherche a charger le dictionnaire DiNom         | */
/* |  de langue lang et de type tdico                                   | */
/* |  Traite le dico si atraiter = True.                                | */
/* | retourne dans pDico le pointeur sur son descripteur ou NULL        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PrepDictionnaire (PtrDico * pDico, char *diconame, PtrDocument document, char *dicodir, Language lang, boolean readonly, boolean traite, boolean atraiter)
#else  /* __STDC__ */
static void         PrepDictionnaire (pDico, diconame, document, dicodir, lang, readonly, traite, atraiter)
PtrDico            *pDico;
char               *diconame;
PtrDocument         document;
char               *dicodir;
Language            lang;
boolean             readonly;
boolean             traite;
boolean             atraiter;

#endif /* __STDC__ */
{
   char                tempbuffer[MAX_CHAR];
   boolean             nouveau = False;
   boolean             ret;
   FILE               *fichdico;	/* pointeur fichier */
   PtrDico             pdict;
   int                 i, im, ic;

   *pDico = NULL;
   /* OUVERTURE DU FICHIER */
   if (traite)
      DoFileName (diconame, "dic", dicodir, tempbuffer, &i);
   else
     {
	if (atraiter)
	   DoFileName (diconame, "DIC", dicodir, tempbuffer, &i);
	else
	   DoFileName (diconame, "LEX", dicodir, tempbuffer, &i);
     }
   if (readonly == False)
     {				/* dictionnaire modifiable */
	if (FileExist (tempbuffer) != 0)
	  {
	     fichdico = fopen (tempbuffer, "rw");	/* maj de dictionnaire */
	     TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_DICO), diconame);
	  }
	else
	  {
	     nouveau = True;
	     fichdico = fopen (tempbuffer, "w+");	/* nouveau dictionnaire */
	     TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_NOUV_DIC), diconame);
	  }
     }
   else
     {
	/* dictionnaire READONLY (generalement pre-traite) */
	if (traite == True)
	   fichdico = BIOreadOpen (tempbuffer);
	else
	   fichdico = fopen (tempbuffer, "r");
	TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_DICO), diconame);
     }

   if (fichdico == NULL)
      return;			/* dictionnaire inacessible : *pDico = NULL */

   /* CHARGEMENT DU DICTIONNAIRE */
   /* creation des structures du dictionnaire */
   /* si manque de place, un dico d'un autre document sera vide' */
   CreeDictionnaire (pDico, document);
   if (*pDico == NULL)
     {
	fclose (fichdico);
	return;			/* refus de memoire */
     }

   pdict = *pDico;
   pdict->DicoDoc = document;
   pdict->DicoLangue = lang;
   strcpy (pdict->DicoDirectory, dicodir);
   strcpy (pdict->DicoNom, diconame);
   pdict->DicoReadOnly = readonly;

   /* calculer la place a allouer pour ce dictionnaire */
   if (nouveau == False)
     {
	if (traite)		/* dictionnaire deja traite' */
	  {			/* ces nombres seront utiles pour l'allocation dynamique */
	     ret = BIOreadInteger (fichdico, &i);
	     if (ret == False)
	       {
		  BIOreadClose (fichdico);
		  return;	/* refus de memoire */
	       }
	     pdict->MAXmots = i;
	     pdict->nbmots = i;
	     BIOreadInteger (fichdico, &i);
	     pdict->MAXcars = i;
	     pdict->nbcars = i;
	  }
	else
	  {
	     /* lire les tailles de chaines requises en debut du fichier */
	     tempbuffer[0] = '\0';
	     fgets (tempbuffer, 100, fichdico);
	     if (tempbuffer[0] != '\0')
	       {
		  if (sscanf (tempbuffer, "%d%d", &im, &ic) == 2)
		    {
		       pdict->MAXmots = im;
		       pdict->MAXcars = ic;
		    }
		  else
		    {
		       /* Le fichier ne contient pas en tete le nombre de mots ... */
		       /* -> on se replace au debut du fichier */
		       /* fseek(fichdico, 0L, 0); */
		       /* impossible de charger ce dictionnaire */
		       TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_LOAD_DICO_ERROR), diconame);
		       /* liberer le dictionnaire alloue */
		       LibDictionnaire (pDico);
		       *pDico = NULL;
		       fclose (fichdico);
		       return;
		    }
	       }
	     else
	       {
		  /* seul un dictionnaire FICHIER  peut etre initialement vide */
		  if (readonly != False)
		    {
		       fclose (fichdico);
		       return;	/* ERREUR LECTURE FICHIER DICTIONNAIRE EMPTY */
		    }

	       }		/* end of else (scanf) */
	  }			/* end of else (traite) */
     }				/* end of if (nouveau == False) */
   /* allouer l'espace pour chaine, commun et pdico */
   /* prevoir la place de 50 mots et 600 caracteres en plus si non readonly */
   if (GetChaine (pDico, readonly) == -1)
     {
	/* pas assez de memoire pour ouvrir ce dictionnaire */
	TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_NO_LOAD), diconame);
	/* liberer aussi le dictionnaire alloue */
	LibDictionnaire (pDico);
	*pDico = NULL;
	fclose (fichdico);
	return;
     }				/* end of if GetChaine == -1 */

   if (traite == True)
     {				/* lecture d'un fichier pretraite */
	if (creer (fichdico, pdict) != 1)
	   *pDico = NULL;	/* incident en cours de lecture du fichier pretraite */
     }
   else
     {				/* lecture d'un fichier non traite */
	if (charger (fichdico, pdict) == 1)
	  {
	     if (atraiter)
		/* pretraitement de ce dictionnaire */
		Corr_pretraitement (pdict);
	  }
	else
	   *pDico = NULL;	/* le chargement du dictionnaire a echoue */
     }				/* end of if (traite == True) */
   fclose (fichdico);
}				/*PrepDictionnaire */


/* ---------------------------------------------------------------------- */
/* |   LoadDico                                                         | */
/* |  Returns -1 if the dictionary can't be loaded.                     | */
/* |           0 if the dictionary was already loaded.                  | */
/* |           1 if the dictionary is loaded.                           | */
/* |  returns in pdico : a pointer to dictionary                        | */
/* |   cree le dictionnaire si tocreate = True                          | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static int          LoadDico (PtrDico * pdico, Language lang, PtrDocument document, char *diconame, char *dicodir, boolean readonly, boolean tocreate)

#else  /* __STDC__ */
static int          LoadDico (pdico, lang, document, diconame, dicodir, readonly, tocreate)
PtrDico            *pdico;
Language            lang;
PtrDocument         document;
char               *diconame;
char               *dicodir;
boolean             readonly;
boolean             tocreate;

#endif /* __STDC__ */

{
   PtrDico             pdict;
   int                 i;
   int                 ret = 0;

   pdict = *pdico;
   /* ce dictionnaire est-il deja charge dans TabDicos */
   ChercheNomDico (&pdict, diconame, dicodir);
   if (pdict == NULL)
     {
	/* le dictionnaire demande' va etre charge, s'il existe ... */
	i = TestDico (diconame, dicodir);
	switch (i)
	      {
		 case (-1):
		    if (readonly == True)
		      {
			 /* fichier inaccesible */
			 *pdico = NULL;
			 ret = -1;
			 break;
		      }
		    if (tocreate != True)
		      {
			 /* ne pas creer maintenant le dictionnaire non readonly */
			 *pdico = NULL;
			 ret = -1;
			 break;
		      }
		    /* sinon : creer le nouveau dictionnaire non readonly */
		 case (0):
		    /* fichier .DIC */
		    PrepDictionnaire (&pdict, diconame, document,
				      dicodir, lang, readonly, False, True);
		    ret = (pdict == NULL) ? -1 : 1;
		    break;
		 case (1):
		    /* fichier .dic */
		    PrepDictionnaire (&pdict, diconame, document,
				      dicodir, lang, readonly, True, False);
		    ret = (pdict == NULL) ? -1 : 1;
		    break;
		 case (2):
		    /* fichier .LEX */
		    PrepDictionnaire (&pdict, diconame, document,
				      dicodir, lang, readonly, False, False);
		    ret = (pdict == NULL) ? -1 : 1;
		    break;
	      }			/* end of switch */

     }				/* end of if (pdict == NULL) */
   else
      /* le dictionnaire etait deja charge pour un autre document */
      /* simple mise a jour du contexte de ce dictionnaire */
      pdict->DicoDoc = document;

   *pdico = pdict;
   return ret;
}				/*LoadDico */


/* ---------------------------------------------------------------------- */
/* |    Corr_ReloadDico recharge un dictionnaire                        | */
/* | retourne True si le dico FICHIER est trouve et bien charge         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             Corr_ReloadDico (PtrDico * pDico)

#else  /* __STDC__ */
boolean             Corr_ReloadDico (pDico)
PtrDico            *pDico;

#endif /* __STDC__ */

{
   PtrDico             pdict;
   PtrDocument         document;
   int                 d;
   Name                 DiNom;

   document = NULL;
   if (*pDico == NULL)
      return (False);

   pdict = *pDico;
   if (pdict != NULL)
      /* cherche dans la table le descripteur du dictionnaire a liberer */
     {
	d = 0;
	while (d < MaxDicos && TabDicos[d] != pdict)
	   d++;
	if (TabDicos[d] == pdict)
	  {
	     /* Recupere les infos sur le dictionnaire */
	     strcpy (DiNom, pdict->DicoNom);
	     document = pdict->DicoDoc;
	     /* liberer la chaine et la liste des mots ... */
	     FreeChaine (pdict);
	     FreeDico (pdict);
	     TabDicos[d] = NULL;
	     pdict = NULL;
	  }
     }

   d = LoadDico (pDico, '\0', document, DiNom,
		 document->DocDirectory, False, True);
   if (d == -1)
      return (False);

   return (True);
}				/*Corr_ReloadDico */


/* ---------------------------------------------------------------------- */
/* |    Dico_Init                                                       | */
/* ---------------------------------------------------------------------- */
void                Dico_Init ()
{
   int                 i;

   /* initialisation des pointeurs de dictionnaire */
   for (i = 0; i < MaxDicos; i++)
      TabDicos[i] = NULL;

   /* initialisation des environnements necessaires aux dictionnaires */
   dicopath = TtaGetEnvString ("DICOPAR");
   if (dicopath == NULL)
     {
	/* la variable d'environnement DICOPAR n'existe pas */
	TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_NO_DICOPAR), "DICOPAR");
     }
   Alphabet_charge = Corr_alphabet ();
   if (Alphabet_charge == False)
      TtaDisplaySimpleMessage (INFO, LIB, LIB_NO_ALPHABET);
}				/* end proc Dico_Init */


/* ---------------------------------------------------------------------- */
/* |  TtaLoadDocumentDictionary loads the document dictionary.          | */
/* |  Returns -1 if the dictionary can't be loaded.                     | */
/* |  Returns 1 if the dictionary is loaded.                            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             TtaLoadDocumentDictionary (PtrDocument document, int *pDico, boolean ToCreate)
#else  /* __STDC__ */
boolean             TtaLoadDocumentDictionary (document, pDico, ToCreate)
PtrDocument         document;
int                *pDico;
boolean             ToCreate;

#endif /* __STDC__ */
{
   /* int        res; */
   char               *dicodoc;

   /* nom du dico = nom du document */
   *pDico = (int) NULL;

   dicodoc = (char *) TtaGetEnvString ("DICODOC");
   if (dicodoc != NULL)
      (void) LoadDico ((PtrDico *) pDico, 0, document, dicodoc,
                        document->DocDirectory, False, ToCreate);
   else
      (void) LoadDico ((PtrDico *) pDico, 0, document, document->DocDName, document->DocDirectory, False, ToCreate);
   return (*pDico != '\0');
}				/*TtaLoadDocumentDictionary */


/* ----------------------------------------------------------------------
   TtaLoadLanguageDictionaries

   Loads the dictionary associated with a language, if it is not loaded yet
   and registers that a dictionary associated with this language has been loaded.

   Returns -1 if the mandatory dictionary cann't be loaded.
   0 if no dictionary has been loaded
   1 if the mandatory dictionary is loaded.

   Parameters:
   languageId: name of the concerned language.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             TtaLoadLanguageDictionaries (Language languageId)

#else  /* __STDC__ */
boolean             TtaLoadLanguageDictionaries (languageId)
Language            languageId;

#endif /* __STDC__ */

{
   int                 lang;
   PtrDico             ptrdico;
   int                 ret;

   /* Si la variable dicopath n'est pas chargee -> ne rien faire */
   if (dicopath == NULL)
      return False;

   ret = 1;
   lang = (int) languageId;
   /* Verifie si le dictionnaire principal est deja charge */
   if (LangTable[lang].LangDico[0] == NULL)
     {
	/* Chargement du dictionnaire principal */
	if (LangTable[lang].LangPrincipal[0] != '\0')
	  {
	     ret = LoadDico (&ptrdico, lang, NULL, LangTable[lang].LangPrincipal, dicopath, True, False);
	     if (ret > 0)
		LangTable[lang].LangDico[0] = (Dictionary) ptrdico;
	  }
     }

   /* Verifie si le dictionnaire secondaire est deja charge */
   if (LangTable[lang].LangDico[1] == NULL)
     {
	/* Chargement du dictionnaire secondaire */
	if (LangTable[lang].LangSecondary[0] != '\0')
	  {
	     ret = LoadDico (&ptrdico, lang, NULL, LangTable[lang].LangSecondary, dicopath, True, False);
	     if (ret > 0)
		LangTable[lang].LangDico[1] = (Dictionary) ptrdico;
	  }
     }
   return (LangTable[lang].LangDico[0] != NULL || LangTable[lang].LangDico[1] != NULL);
}				/*TtaLoadLanguageDictionaries */


/* ----------------------------------------------------------------------
   TtaLoadTypoDictionaries

   Loads the dictionary associated with a typolanguage, if it is not loaded yet
   and registers that a dictionary associated with this language has been loaded.

   Returns -1 if the mandatory dictionary cann't be loaded.
   0 if no dictionary has been loaded
   1 if the mandatory dictionary is loaded.

   Parameters:
   languageId: name of the concerned language.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             TtaLoadTypoDictionaries (Language languageId)

#else  /* __STDC__ */
boolean             TtaLoadTypoDictionaries (languageId)
Language            languageId;

#endif /* __STDC__ */

{
   int                 lang;
   PtrDico             ptrdico;
   int                 ret;

   /* Si la variable dicopath n'est pas chargee -> ne rien faire */
   if (dicopath == NULL)
      return False;

   ret = 1;
   lang = (int) languageId;
   /* Verifie si le dictionnaire principal est deja charge */
   if (TypoLangTable[lang].LangDico[0] == NULL)
     {
	/* Chargement du dictionnaire principal */
	if (TypoLangTable[lang].LangPrincipal[0] != '\0')
	  {
	     ret = LoadDico (&ptrdico, lang, NULL,
		  TypoLangTable[lang].LangPrincipal, dicopath, True, False);
	     if (ret > 0)
		TypoLangTable[lang].LangDico[0] = (Dictionary) ptrdico;
	  }
     }

   /* Verifie si le dictionnaire secondaire est deja charge */
   if (TypoLangTable[lang].LangDico[1] == NULL)
     {
	/* Chargement du dictionnaire secondaire */
	if (TypoLangTable[lang].LangSecondary[0] != '\0')
	  {
	     ret = LoadDico (&ptrdico, lang, NULL,
		  TypoLangTable[lang].LangSecondary, dicopath, True, False);
	     if (ret > 0)
		TypoLangTable[lang].LangDico[1] = (Dictionary) ptrdico;
	  }
     }
   return (TypoLangTable[lang].LangDico[0] != NULL
	   || TypoLangTable[lang].LangDico[1] != NULL);
}				/*TtaLoadTypoDictionaries */


/* ----------------------------------------------------------------------
   TtaUnLoadLanguageDictionaries

   Unloads dictionaries associated with a given language.

   Parameters:
   languageId: identifier of the language.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaUnLoadLanguageDictionaries (Language languageId)

#else  /* __STDC__ */
void                TtaUnLoadLanguageDictionaries (languageId)
Language            languageId;

#endif /* __STDC__ */

{
   int                 i, j;

   i = (int) languageId;
   j = 0;
   while (LangTable[i].LangDico[j] != NULL && j < MAX_DICOS)
     {
	LibDictionnaire ((PtrDico *) & LangTable[i].LangDico[j]);
	LangTable[i].LangDico[j] = NULL;
     }
}				/*TtaUnLoadDictionaries */


/* ----------------------------------------------------------------------
   TtaUnLoadTypoDictionaries

   Unloads dictionaries associated with a given language.

   Parameters:
   languageId: identifier of the language.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaUnLoadTypoDictionaries (Language languageId)

#else  /* __STDC__ */
void                TtaUnLoadTypoDictionaries (languageId)
Language            languageId;

#endif /* __STDC__ */

{
   int                 i, j;

   i = (int) languageId;
   j = 0;
   while (TypoLangTable[i].LangDico[j] != NULL && j < MAX_DICOS)
     {
	LibDictionnaire ((PtrDico *) & TypoLangTable[i].LangDico[j]);
	TypoLangTable[i].LangDico[j] = NULL;
     }
}				/*TtaUnLoadTypoDictionaries */


/* ----------------------------------------------------------------------
   TtaGetPrincipalDictionary

   Returns a pointer to the principal dictionary associated to a language.

   Return value:
   the pointer to that dictionary or NULL if there is no dictionary for
   this language.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
Dictionary          TtaGetPrincipalDictionary (Language languageId)

#else  /* __STDC__ */
Dictionary          TtaGetPrincipalDictionary (languageId)
Language            languageId;

#endif /* __STDC__ */

{
   int                 i;

   i = (int) languageId;
   /* Verification du parametre */
   if (i >= FreeEntry)
     {
	TtaError (ERR_language_not_found);
	return NULL;
     }

   /* Il charger les dictionnaires s'il en existe */
   TtaLoadLanguageDictionaries (languageId);
   return (LangTable[i].LangDico[0]);
}				/*TtaGetPrincipalDictionary */


/* ----------------------------------------------------------------------
   TtaGetPrincipalTypoDictionary

   Returns a pointer to the principal dictionary associated to a language.

   Return value:
   the pointer to that dictionary or NULL if there is no dictionary for
   this language.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
Dictionary          TtaGetPrincipalTypoDictionary (Language languageId)

#else  /* __STDC__ */
Dictionary          TtaGetPrincipalTypoDictionary (languageId)
Language            languageId;

#endif /* __STDC__ */

{
   int                 i;

   i = (int) languageId;
   /* Verification du parametre */
   if (i >= FreeEntry)
     {
	TtaError (ERR_language_not_found);
	return NULL;
     }

   /* Il charger les dictionnaires s'il en existe */
   TtaLoadTypoDictionaries (languageId);
   return (TypoLangTable[i].LangDico[0]);
}				/*TtaGetPrincipalTypoDictionary */


/* ----------------------------------------------------------------------
   TtaGetSecondaryDictionary

   Returns a pointer to the secondary dictionary associated to a language.

   Return value:
   the pointer to that dictionary or NULL if there is no dictionary for
   this language.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
Dictionary          TtaGetSecondaryDictionary (Language languageId)

#else  /* __STDC__ */
Dictionary          TtaGetSecondaryDictionary (languageId)
Language            languageId;

#endif /* __STDC__ */

{
   int                 i;

   i = (int) languageId;
   /* Verification du parametre */
   if (i >= FreeEntry)
     {
	TtaError (ERR_language_not_found);
	return NULL;
     }
   return (LangTable[i].LangDico[1]);
}				/*TtaGetSecondaryDictionary */


/* ----------------------------------------------------------------------
   TtaGetSecondaryTypoDictionary

   Returns a pointer to the secondary dictionary associated to a language.

   Return value:
   the pointer to that dictionary or NULL if there is no dictionary for
   this language.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
Dictionary          TtaGetSecondaryTypoDictionary (Language languageId)

#else  /* __STDC__ */
Dictionary          TtaGetSecondaryTypoDictionary (languageId)
Language            languageId;

#endif /* __STDC__ */

{
   int                 i;

   i = (int) languageId;
   /* Verification du parametre */
   if (i >= FreeEntry)
     {
	TtaError (ERR_language_not_found);
	return NULL;
     }
   return (TypoLangTable[i].LangDico[1]);
}				/*TtaGetSecondaryTypoDictionary */

/* end of module */
