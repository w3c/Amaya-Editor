
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   hyphen.c :  gestion de la coupure des mots
   I. Vatton - Aout 92
   France Logiciel numero de depot 88-39-001-00
   Major changes:
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "language.h"
#include "dictionary.h"

#define EXPORT extern
#include "boxes_tv.h"

static char         sepcar[] =
{
   ' ', '.', ',', '`', '\47', '-', ';', ':', '[', ']', '(', ')', '{', '}', '<', '>',
   '/', '!', '?', '\240', '\241', '\277', '\253', '\273', '\212', '"', '\201', '\202'};

#include "font_f.h"

/* ---------------------------------------------------------------------- */
/* |    PatternHyphen  cherche pour le mot word le premier point de     | */
/* |            coupure qui pre'ce`de le caracte`re de rang length (le  | */
/* |            rang du premier caracte`re est 0) et retourne 0 si      | */
/* |            aucune coupure n'est possible ou le rang du caracte`re  | */
/* |            devant lequel on peut couper.                           | */
/* |            Si la coupure est possible, indique dans addHyphen s'il | */
/* |            faut inse'rer un tiret a` la position de coupure ou non.| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static int          PatternHyphen (char *word, int length, Language language, boolean * addHyphen)
#else  /* __STDC__ */
static int          PatternHyphen (word, length, language, addHyphen)
char               *word;
int                 length;
Language            language;
boolean            *addHyphen;

#endif /* __STDC__ */
{
   int                 i, k;
   int                 retour;
   int                *ptcoup;

   *addHyphen = FALSE;
   retour = 0;

   ptcoup = TtaGetPatternHyphenList (word, language);
   if (ptcoup == NULL)
      /* Pas de point de coupure */
      return retour;

   /* On recherche le dernier point de coupure <= length */
   i = 0;
   k = ptcoup[i];
   while (k != 0 && k <= length)
     {
	if (k <= length)
	  {
	     retour = k;
	     *addHyphen = TRUE;
	  }
	i++;
	k = ptcoup[i];
     }
   return retour;
}

/* ---------------------------------------------------------------------- */
/* | SeparateurMot teste si ce caractere est un separateur de mot       | */
/* |                 retourne TRUE si c'est un separateur               | */
/* |                          FALSE sinon                               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             SeparateurMot (char car)

#else  /* __STDC__ */
boolean             SeparateurMot (car)
char                car;

#endif /* __STDC__ */

{
   int                 i, lg;

   lg = sizeof (sepcar);
   for (i = 0; i < lg; i++)
     {
	if (car == sepcar[i])
	   return (TRUE);
     }
   return (FALSE);
}				/*SeparateurMot */


/* ---------------------------------------------------------------------- */
/* |    Minuscule convertit les caracte`res majuscules en minuscules.   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                Minuscule (char *word)

#else  /* __STDC__ */
void                Minuscule (word)
char               *word;

#endif /* __STDC__ */

{
   unsigned char       car;
   int                 j;

   j = 0;
   while (word[j] != 0)
     {
	car = word[j];
	if (car >= 65 && car <= 90)
	   word[j] = (char) (car + 32);		/* Majuscules */
	else if (car >= 192 && car <= 222)
	   word[j] = (char) (car + 32);		/* Majuscules accentue'es */
	j++;
     }
}

#ifdef IV
/* ---------------------------------------------------------------------- */
/* |    Voyelle rend la valeur VRAI si le caracte`re est une voyelle.   | */
/* |            La fonction ne prend pas en compte les majuscules.      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      Voyelle (unsigned char c)

#else  /* __STDC__ */
static boolean      Voyelle (c)
unsigned char       c;

#endif /* __STDC__ */

{
   if (c < 224)
     {
	/* lettre non accentuee */
	if (c == 97 || c == 101 | c == 105
	    || c == 111 || c == 117 || c == 121)
	   return TRUE;
	else
	   return FALSE;
     }
   else if (c <= 229)
      return TRUE;		/* a majuscule accente' */
   else if (c < 232)
      return FALSE;
   else if (c < 240)
      return TRUE;		/* e et i majuscules accente's */
   else if (c < 242)
      return FALSE;
   else if (c < 247)
      return TRUE;		/* o majuscule accente' */
   else if (c < 249)
      return FALSE;
   else if (c < 253)
      return TRUE;		/* u majuscule accente' */
   else
      return FALSE;
}

/* ---------------------------------------------------------------------- */
/* |    Consonne rend la valeur VRAI si le caracte`re est une consonne. | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      Consonne (unsigned char c)

#else  /* __STDC__ */
static boolean      Consonne (c)
unsigned char       c;

#endif /* __STDC__ */

{
   return (!Voyelle (c));
}

static char        *tablePrefixes[] =
{"anti", "inter", "p\351ri", "\351pi",
 "micro", "hyper", "super", "infra"}
                   ;

/* ---------------------------------------------------------------------- */
/* |    FrenchHyphen  cherche pour le mot word le premier point de      | */
/* |            coupure qui pre'ce`de le caracte`re de rang length (le  | */
/* |            rang du premier caracte`re est 0) et retourne 0 si      | */
/* |            aucune coupure n'est possible ou le rang du caracte`re  | */
/* |            devant lequel on peut couper.                           | */
/* |            Si la coupure est possible, indique dans addHyphen s'il | */
/* |            faut inse'rer un tiret a` la position de coupure ou non.| */
/* |            Cette proce'dure effectue une coupure syllabique pour   | */
/* |            la langue franc,aise uniquement, d'apre`s les re`gles   | */
/* |            de Grevisse (le bon usage).                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static int          FrenchHyphen (char *word, int length, boolean * addHyphen)

#else  /* __STDC__ */
static int          FrenchHyphen (word, length, addHyphen)
char               *word;
int                 length;
boolean            *addHyphen;

#endif /* __STDC__ */

{
   int                 index;
   int                 wordlength;
   int                 nbPrefixes, prefixe, lgPrefixe;
   boolean             stop;
   boolean             found;

   /* cherche s'il y a dans le mot un caractere de coupure */
   index = 1;
   stop = FALSE;
   do
      if (word[index] == '\0' || index >= length)
	 /* on a atteint la fin du mot ou la fin de la zone consideree */
	 /* dans le mot sans rien trouver */
	 stop = TRUE;
      else if (word[index] == '-' || word[index] == '/')
	 /* on a trouve' un caractere de coupure */
	{
	   *addHyphen = FALSE;
	   return (index + 1);
	}
      else
	 /* on passe au caractere suivant du mot */
	 index++;
   while (!stop);

   /* il n'y a pas de caractere de coupure dans la partie du mot qui */
   /* nous interesse */
   wordlength = strlen (word);	/* longueur du mot complet */
   /* on part de la fin de la partie du mot concernee */
   index = length;
   /* on doit laisser au moins 3 lettres apres la coupure */
   if (index > wordlength - 3)
      index = wordlength - 3;
   found = FALSE;
   /* on cherche d'abord s'il existe un prefixe dans la partie a couper */
   nbPrefixes = sizeof (tablePrefixes) / sizeof (char *);

   for (prefixe = 0; prefixe < nbPrefixes && !found; prefixe++)
      /* on compare le debut du mot avec chaque entree de la table */
      /* des prefixes */
     {
	lgPrefixe = strlen (tablePrefixes[prefixe]);
	if (strncmp (word, tablePrefixes[prefixe], lgPrefixe) == 0)
	   /* le mot commence par ce prefixe */
	   if (index < lgPrefixe)
	      /* on ne coupe pas un prefixe */
	     {
		index = 0;
		found = TRUE;
	     }
	   else if (index < lgPrefixe + 3)
	      /* on coupe juste apres le prefixe */
	     {
		index = lgPrefixe;
		found = TRUE;
	     }
     }

   /* on cherche une coupure syllabique dans la partie de mot a traiter */
   while (index > 2 && !found)	/* on doit laisser au moins 2 lettres */
      /* avant la coupure */
     {
	if (Consonne (word[index]))
	   /* le caractere courant est un consonne */
	  {
	     if (Voyelle (word[index + 1]))
	       {
		  /* c'est une consonne suivie d'une voyelle */
		  if (Voyelle (word[index - 1]))
		     /* et precedee d'une voyelle, on coupe V-CV */
		    {
		       found = TRUE;
		       /* sauf si on est dans le cas "cooCV", comme "cooperer" */
		       if (index == 3)
			  if (strncmp (word, "coop", 4) == 0)
			     found = FALSE;
		    }
		  else if (Consonne (word[index - 1]) && Voyelle (word[index - 2]))
		     /* sequence VC-CV */
		     if ((word[index] == 'l' || word[index] == 'r') &&
			 !(word[index - 1] == 'l' || word[index - 1] == 'r'))
		       {
			  if (index > 3)
			     /* on coupe V-CCV */
			    {
			       found = TRUE;
			       index--;
			    }
		       }
		     else if (((word[index] == 'h')
			       && (word[index - 1] == 'c'
				   || word[index - 1] == 'p'
				   || word[index - 1] == 't'))
			      || ((word[index] == 'n')
				  && (word[index - 1] == 'g')))
			/* on ne coupe pas ch ph th gn */
			found = FALSE;
		     else
			/* on coupe VC-CV */
			found = TRUE;
	       }
	     if (!found)
		if (Consonne (word[index - 1]) && Consonne (word[index - 2]))
		   /* sequence CC-C */
		  {
		     /* a priori, on coupe CC-C */
		     found = TRUE;
		     if ((word[index] == 'l' || word[index] == 'r') &&
			 !(word[index - 1] == 'l' || word[index - 1] == 'r'))
			if (index > 3)
			   index--;	/* coupe C-CC */
			else
			   found = FALSE;
		     if (found)
			if (((word[index] == 'h')
			     && (word[index - 1] == 'c'
				 || word[index - 1] == 'p'
				 || word[index - 1] == 't'))
			    || ((word[index] == 'n')
				&& (word[index - 1] == 'g')))
			   /* on ne coupe pas ch ph th gn */
			   found = FALSE;
		  }
	  }
	if (!found)
	   /* passe au caractere precedent du mot */
	   index--;
     }
   if (found && index > 0)
     {
	*addHyphen = TRUE;	/* il faudra inserer un tiret a la position */
	/* qu'on a trouvee */
	/* on coupe avant la lettre pointee par index */
	return index;
     }
   else
      /* pas de coupure possible dans la zone consideree du mot */
      return 0;
}
#endif

/* ---------------------------------------------------------------------- */
/* |    NextWord isole le mot suivant dans la suite des buffers de      | */
/* |            de texte. Ce mot peut e^tre vide.                       | */
/* |            Retourne le nombre de se'parateurs qui pre'ce`dent le   | */
/* |            de'but du mot.                                          | */
/* |            Rend la position a` laquelle le mot de'bute :           | */
/* |            - l'adresse du buffer du 1er caracte`re.                | */
/* |            - l'index dans ce buffer du 1er caracte`re.             | */
/* |            - le mot isole'.                                        | */
/* |            - la longueur des se'parateurs qui pre'ce`dent le       | */
/* |              de'but du mot.                                        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static int          NextWord (ptrfont font, PtrTextBuffer * buffer, int *rank, char word[MAX_CHAR], int *width)

#else  /* __STDC__ */
static int          NextWord (font, buffer, rank, word, width)
ptrfont             font;
PtrTextBuffer     *buffer;
int                *rank;
char                word[MAX_CHAR];
int                *width;

#endif /* __STDC__ */

{
   int                 i, j;
   int                 lg, nbcar;
   boolean             encore;
   boolean             changedebut;
   PtrTextBuffer      adbuff;

   /* Initialisations */
   word[0] = '\0';
   lg = 0;
   nbcar = 0;
   j = 0;
   encore = TRUE;
   /* La position du debut du mot */
   adbuff = *buffer;
   i = *rank;
   /* A priori le debut du mot est correctement repere */
   changedebut = FALSE;

   while (encore)
     {
	if (j == MAX_CHAR - 1)
	   /* Le mot est trop long */
	   encore = FALSE;
	else if (i >= adbuff->BuLength)
	   /* Il faut changer de buffer */
	   if (adbuff->BuNext == NULL)
	      encore = FALSE;
	   else
	     {
		adbuff = adbuff->BuNext;
		i = 0;
	     }
	else
	  {
	     /* FnCopy du caractere */
	     word[j] = adbuff->BuContent[i];
	     if (word[j] == '\'')
		/* Cas particulier du cote */
		if (j <= 1)
		  {
		     changedebut = TRUE;
		     lg += CarWidth ((unsigned char) word[j], font);
		     if (j == 1)
			/* Il faut comptabiliser le caractere precedent */
			lg += CarWidth ((unsigned char) word[j - 1], font);
		     nbcar += j + 1;
		     j = 0;
		  }
		else
		   encore = FALSE;
	     else if (SeparateurMot (word[j]))
	       {
		  /* On ne traite pas les separateurs en debut de mot */
		  if (j != 0)
		    {
		       word[j] = '\0';
		       encore = FALSE;
		    }
		  else
		    {
		       /* Le debut du mot est deplace */
		       changedebut = TRUE;
		       lg += CarWidth ((unsigned char) word[j], font);
		       nbcar++;
		    }
	       }
	     else
	       {
		  j++;
		  if (changedebut)
		    {
		       /* Repere le debut du mot */
		       changedebut = FALSE;
		       *buffer = adbuff;
		       *rank = i;	/* i pointe deja sur le 1er caractere du mot */
		    }
	       }

	     /* Passe au caractere suivant */
	     i++;
	  }
     }

   /* Termine le mot */
   word[j] = '\0';
   if (nbcar == 0)
      *width = 0;
   else
      *width = lg;
   return nbcar;
}

/* ---------------------------------------------------------------------- */
/* |    WordHyphen cherche un point de coupure du mot pour permettre    | */
/* |            d'inse'rer une partie du mot dans la largeur donne'e.   | */
/* |            Retourne le nombre de caracte`res qui pre'ce`dent le    | */
/* |            point de coupure, 0 si le mot ne peut pas e^tre coupe'. | */
/* |            Rend un indicateur qui vaut VRAI s'il faut engendrer un | */
/* |            tiret d'hyphe'nation.                                   | */
/* |            Le parame`tre language donne l'indice de la langue dans | */
/* |            la table des langues courante.                          | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static int          WordHyphen (char *word, int length, Language language, boolean * hyphen)

#else  /* __STDC__ */
static int          WordHyphen (word, length, language, hyphen)
char               *word;
int                 length;
Language            language;
boolean            *hyphen;

#endif /* __STDC__ */

{

   /* Convertit le mot en minuscule */
   Minuscule (word);
   return PatternHyphen (word, length, language, hyphen);
}

/* ---------------------------------------------------------------------- */
/* |    CutLasWord essaie de couper le dernier mot de la ligne dans la  | */
/* |            limite de lenght caracte`res et de la longueur width.   | */
/* |            Retourne le nombre de caracte`res avant la coupure      | */
/* |            ou 0 si le mot ne peut pas e^tre coupe'.                | */
/* |            Rend la position a` laquelle le point de coupure peut   | */
/* |            e^tre inse're' :                                        | */
/* |            - l'adresse du buffer du 1er caracte`re apre`s coupure. | */
/* |            - l'index dans ce buffer du 1er caracte`re.             | */
/* |            - la longueur de la premie`re partie  du mot, le tiret  | */
/* |              d'hyphe'nation compris.                               | */
/* |            - un indicateur qui vaut VRAI s'il faut engendrer un    | */
/* |              tiret d'hyphe'nation.                                 | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
int                 CutLastWord (ptrfont font, Language langue, PtrTextBuffer * buffer, int *rank, int *width, boolean * hyphen)

#else  /* __STDC__ */
int                 CutLastWord (font, langue, buffer, rank, width, hyphen)
ptrfont             font;
Language            langue;
PtrTextBuffer     *buffer;
int                *rank;
int                *width;
boolean            *hyphen;

#endif /* __STDC__ */

{
   PtrTextBuffer      adbuff;
   int                 i, lghyphen;
   int                 longueur, nbcar;
   int                 largeur, lgcar;
   int                 longretour, lgreste;
   int                 lgmot;
   char                mot[MAX_CHAR];

   /* Si la coupure de mots est active */
   longretour = 0;
   *hyphen = FALSE;

   if (*width > 0 && *buffer != NULL)
     {
	/* La position du debut du mot */
	adbuff = *buffer;

	i = *rank - 1;
	if (i >= adbuff->BuLength)
	   if (adbuff->BuNext != NULL)
	     {
		i = 0;
		adbuff = adbuff->BuNext;
	     }
	   else
	      return longretour;

	/* Longueur et largeur des separateurs avant le mot */
	nbcar = NextWord (font, &adbuff, &i, mot, &largeur);
	/* Largeur du tiret d'hyphenantion */
	lghyphen = CarWidth (173, font);
	/* Espace restant dans la ligne */
	lgreste = *width - largeur - lghyphen;
	/* Nombre de carateres maximum du mot pouvant entrer dans la ligne */

	if (mot != NULL)
	   /* On a isole un mot assez long */
	   lgmot = strlen (mot);	/* nombre de caraceteres du mot isole */
	if (lgmot > 4 && lgreste > 0)
	  {
	     /* Recherche le nombre de caracteres du mot qui rentrent */
	     /* dans la ligne */
	     longueur = 0;
	     lgcar = CarWidth ((unsigned char) mot[longueur], font);
	     while (lgreste >= lgcar && longueur < lgmot)
	       {
		  lgreste -= lgcar;
		  longueur++;
		  lgcar = CarWidth ((unsigned char) mot[longueur], font);
	       }

	     if (longueur > 1)
	       {
		  /* Recherche un point de coupure pour le mot */
		  longueur = WordHyphen (mot, longueur, langue, hyphen);
		  if (longueur > 0)
		    {
		       /* On a trouve un point de coupure */
		       if (*hyphen)
			  *width = largeur + lghyphen;	/* 1ere partie du mot */
		       else
			  *width = largeur;	/* 1ere partie du mot */

		       longretour = longueur + nbcar;	/* nombre de caracteres */
		       while (longueur > 0)
			 {
			    if (i >= adbuff->BuLength)
			      {
				 /* Il faut changer de buffer */
				 adbuff = adbuff->BuNext;
				 i = 0;
			      }
			    else
			      {
				 /* comptabilise le caractere */
				 longueur--;
				 *width += CarWidth ((unsigned char) (adbuff->BuContent[i++]), font);
			      }
			 }	/*while */

		       /* Indice dans le buffer 2eme partie du mot */
		       if (i >= adbuff->BuLength)
			 {
			    /* Il faut changer de buffer */
			    i++;
			    *rank = i - adbuff->BuLength;		/* nouvelle position */
			    adbuff = adbuff->BuNext;
			 }
		       else
			  *rank = i + 1;
		       *buffer = adbuff;
		    }
	       }		/*if (longueur > 1) */
	  }
     }
   return longretour;
}


/* ---------------------------------------------------------------------- */
/* |    Hyphenable rend de la valeur VRAI s'il existe un traitement de  | */
/* |            coupure des mots et l'autorisation de coupure pour la   | */
/* |            boi^te donne'e.                                         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             Hyphenable (PtrBox pBox)

#else  /* __STDC__ */
boolean             Hyphenable (pBox)
PtrBox            pBox;

#endif /* __STDC__ */

{
   Language            langue;

   if (!TextInserting && pBox->BxAbstractBox->AbHyphenate)
     {
	langue = pBox->BxAbstractBox->AbLanguage;
/*PhL */ if (TtaExistPatternList (langue))
	   /*PhL *//* il existe une table de pattern */
/*PhL */ return TRUE;
/*PhL */ 
	else
	  {
	     /* pas de table de patterns : on cherche a charger un dico */
	     if (langue == 0)
		/* On saute la langue ISOlatin-1 */
		return FALSE;
	     /*PhL *//*   else if (langue == TtaGetLanguageIdFromName("Fran\347ais")) */
	     /*PhL *//*      return TRUE; */
	     else if (TtaGetPrincipalDictionary (langue) != NULL)
		/* Traitement par le dictionnaire de la langue */
		return TRUE;
	     else if (TtaGetSecondaryDictionary (langue) != NULL)
		/* Pas de traitement de coupure possible */
		return FALSE;
	     else
		return FALSE;
/*PhL */ 
	  }
     }
   else
      return FALSE;
}
