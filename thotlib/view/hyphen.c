/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
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
 * hyphen.c :  gestion de la coupure des mots
 *
 * Authors I. Vatton (INRIA)
 *         Ph. Louarn (IRISA) - multilingual hyphenation
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "language.h"
#include "dictionary.h"

#define THOT_EXPORT extern
#include "boxes_tv.h"

static char         sepcar[] =
{
   ' ', '.', ',', '`', '\47', '-', ';', ':', '[', ']', '(', ')', '{', '}', '<', '>',
   '/', '!', '?', '\240', '\241', '\277', '\253', '\273', '\212', '"', '\201', '\202'};

#include "font_f.h"

/*----------------------------------------------------------------------
   PatternHyphen  cherche pour le mot word le premier point de     
   coupure qui pre'ce`de le caracte`re de rang length (le  
   rang du premier caracte`re est 0) et retourne 0 si      
   aucune coupure n'est possible ou le rang du caracte`re  
   devant lequel on peut couper.                           
   Si la coupure est possible, indique dans addHyphen s'il 
   faut inse'rer un tiret a` la position de coupure ou non.
  ----------------------------------------------------------------------*/
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
   int                 status;
   int                *pHyphen;

   *addHyphen = FALSE;
   status = 0;

   pHyphen = TtaGetPatternHyphenList (word, language);
   if (pHyphen == NULL)
      /* Pas de point de coupure */
      return status;

   /* On recherche le dernier point de coupure <= length */
   i = 0;
   k = pHyphen[i];
   while (k != 0 && k <= length)
     {
	if (k <= length)
	  {
	     status = k;
	     *addHyphen = TRUE;
	  }
	i++;
	k = pHyphen[i];
     }
   return status;
}

/*----------------------------------------------------------------------
   IsSeparatorChar teste si ce caractere est un separateur de mot       
   retourne TRUE si c'est un separateur               
   FALSE sinon                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             IsSeparatorChar (char c)
#else  /* __STDC__ */
boolean             IsSeparatorChar (c)
char                c;
#endif /* __STDC__ */
{
   int              i, lg;

   lg = sizeof (sepcar);
   for (i = 0; i < lg; i++)
     {
	if (c == sepcar[i])
	   return (TRUE);
     }
   return (FALSE);
}


/*----------------------------------------------------------------------
   SmallLettering convertit les caracte`res majuscules en minuscules.   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SmallLettering (char *word)
#else  /* __STDC__ */
void                SmallLettering (word)
char               *word;
#endif /* __STDC__ */
{
   unsigned char       c;
   int                 j;

   j = 0;
   while (word[j] != 0)
     {
	c = word[j];
	if (c >= 65 && c <= 90)
	   word[j] = (char) (c + 32);		/* Majuscules */
	else if (c >= 192 && c <= 222)
	   word[j] = (char) (c + 32);		/* Majuscules accentue'es */
	j++;
     }
}


/*----------------------------------------------------------------------
   NextWord isole le mot suivant dans la suite des buffers de      
   de texte. Ce mot peut e^tre vide.                       
   Retourne le nombre de se'parateurs qui pre'ce`dent le   
   de'but du mot.                                          
   Rend la position a` laquelle le mot de'bute :           
   - l'adresse du buffer du 1er caracte`re.                
   - l'index dans ce buffer du 1er caracte`re.             
   - le mot isole'.                                        
   - la longueur des se'parateurs qui pre'ce`dent le       
   de'but du mot.                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          NextWord (ptrfont font, PtrTextBuffer * buffer, int *rank, char word[THOT_MAX_CHAR], int *width)

#else  /* __STDC__ */
static int          NextWord (font, buffer, rank, word, width)
ptrfont             font;
PtrTextBuffer      *buffer;
int                *rank;
char                word[THOT_MAX_CHAR];
int                *width;
#endif /* __STDC__ */
{
   int                 i, j;
   int                 lg, nbChars;
   boolean             still;
   boolean             changedebut;
   PtrTextBuffer       adbuff;

   /* Initialisations */
   word[0] = EOS;
   lg = 0;
   nbChars = 0;
   j = 0;
   still = TRUE;
   /* La position du debut du mot */
   adbuff = *buffer;
   i = *rank;
   /* A priori le debut du mot est correctement repere */
   changedebut = FALSE;

   while (still)
     {
	if (j == THOT_MAX_CHAR - 1)
	   /* Le mot est trop long */
	   still = FALSE;
	else if (i >= adbuff->BuLength)
	   /* Il faut changer de buffer */
	   if (adbuff->BuNext == NULL)
	      still = FALSE;
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
		     lg += CharacterWidth ((unsigned char) word[j], font);
		     if (j == 1)
			/* Il faut comptabiliser le caractere precedent */
			lg += CharacterWidth ((unsigned char) word[j - 1], font);
		     nbChars += j + 1;
		     j = 0;
		  }
		else
		   still = FALSE;
	     else if (IsSeparatorChar (word[j]))
	       {
		  /* On ne traite pas les separateurs en debut de mot */
		  if (j != 0)
		    {
		       word[j] = EOS;
		       still = FALSE;
		    }
		  else
		    {
		       /* Le debut du mot est deplace */
		       changedebut = TRUE;
		       lg += CharacterWidth ((unsigned char) word[j], font);
		       nbChars++;
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
   word[j] = EOS;
   if (nbChars == 0)
      *width = 0;
   else
      *width = lg;
   return nbChars;
}

/*----------------------------------------------------------------------
   WordHyphen cherche un point de coupure du mot pour permettre    
   d'inse'rer une partie du mot dans la largeur donne'e.   
   Retourne le nombre de caracte`res qui pre'ce`dent le    
   point de coupure, 0 si le mot ne peut pas e^tre coupe'. 
   Rend un indicateur qui vaut VRAI s'il faut engendrer un 
   tiret d'hyphe'nation.                                   
   Le parame`tre language donne l'indice de la langue dans 
   la table des langues courante.                          
  ----------------------------------------------------------------------*/
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
   SmallLettering (word);
   return PatternHyphen (word, length, language, hyphen);
}

/*----------------------------------------------------------------------
   CutLasWord essaie de couper le dernier mot de la ligne dans la  
   limite de lenght caracte`res et de la longueur width.   
   Retourne le nombre de caracte`res avant la coupure      
   ou 0 si le mot ne peut pas e^tre coupe'.                
   Rend la position a` laquelle le point de coupure peut   
   e^tre inse're' :                                        
   - l'adresse du buffer du 1er caracte`re apre`s coupure. 
   - l'index dans ce buffer du 1er caracte`re.             
   - la longueur de la premie`re partie  du mot, le tiret  
   d'hyphe'nation compris.                               
   - un indicateur qui vaut VRAI s'il faut engendrer un    
   tiret d'hyphe'nation.                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 HyphenLastWord (ptrfont font, Language language, PtrTextBuffer * buffer, int *rank, int *width, boolean * hyphen)
#else  /* __STDC__ */
int                 HyphenLastWord (font, language, buffer, rank, width, hyphen)
ptrfont             font;
Language            language;
PtrTextBuffer      *buffer;
int                *rank;
int                *width;
boolean            *hyphen;
#endif /* __STDC__ */
{
   PtrTextBuffer       adbuff;
   int                 i, lghyphen;
   int                 longueur, nbChars;
   int                 largeur, charWidth;
   int                 longretour, lgreste;
   int                 wordLength;
   char                mot[THOT_MAX_CHAR];

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
	nbChars = NextWord (font, &adbuff, &i, mot, &largeur);
	/* Largeur du tiret d'hyphenantion */
	lghyphen = CharacterWidth (173, font);
	/* Espace restant dans la ligne */
	lgreste = *width - largeur - lghyphen;
	/* Nombre de carateres maximum du mot pouvant entrer dans la ligne */

	if (mot != NULL)
	   /* On a isole un mot assez long */
	   wordLength = strlen (mot);	/* nombre de caraceteres du mot isole */
	if (wordLength > 4 && lgreste > 0)
	  {
	     /* Recherche le nombre de caracteres du mot qui rentrent */
	     /* dans la ligne */
	     longueur = 0;
	     charWidth = CharacterWidth ((unsigned char) mot[longueur], font);
	     while (lgreste >= charWidth && longueur < wordLength)
	       {
		  lgreste -= charWidth;
		  longueur++;
		  charWidth = CharacterWidth ((unsigned char) mot[longueur], font);
	       }

	     if (longueur > 1)
	       {
		  /* Recherche un point de coupure pour le mot */
		  longueur = WordHyphen (mot, longueur, language, hyphen);
		  if (longueur > 0)
		    {
		       /* On a trouve un point de coupure */
		       if (*hyphen)
			  *width = largeur + lghyphen;	/* 1ere partie du mot */
		       else
			  *width = largeur;	/* 1ere partie du mot */

		       longretour = longueur + nbChars;	/* nombre de caracteres */
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
				 *width += CharacterWidth ((unsigned char) (adbuff->BuContent[i++]), font);
			      }
			 }	/*while */

		       /* Indice dans le buffer 2eme partie du mot */
		       if (i >= adbuff->BuLength)
			 {
			    /* Il faut changer de buffer */
			    i++;
			    /* nouvelle position */
			    *rank = i - adbuff->BuLength;
			    adbuff = adbuff->BuNext;
			 }
		       else
			  *rank = i + 1;
		       *buffer = adbuff;
		    }
	       }
	  }
     }
   return longretour;
}


/*----------------------------------------------------------------------
   CanHyphen rend de la valeur VRAI s'il existe un traitement de  
   coupure des mots et l'autorisation de coupure pour la   
   boi^te donne'e.                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             CanHyphen (PtrBox pBox)
#else  /* __STDC__ */
boolean             CanHyphen (pBox)
PtrBox              pBox;
#endif /* __STDC__ */
{
  Language            language;

  if (!TextInserting && pBox->BxAbstractBox->AbHyphenate)
    {
      language = pBox->BxAbstractBox->AbLanguage;
      if (TtaExistPatternList (language))
	/* il existe une table de pattern */
	return TRUE;      
      else
	{
	  /* pas de table de patterns : on cherche a charger un dico */
	  if (language == 0)
	    /* On saute la langue ISOlatin-1 */
	    return FALSE;
	  /*   else if (language == TtaGetLanguageIdFromName("Fran\347ais")) */
	  /*      return TRUE; */
	  else if (TtaGetPrincipalDictionary (language) != NULL)
	    /* Traitement par le dictionnaire de la langue */
	    return TRUE;
	  else if (TtaGetSecondaryDictionary (language) != NULL)
	    /* Pas de traitement de coupure possible */
	    return FALSE;
	  else
	    return FALSE;	  
	}
    }
  else
    return FALSE;
}
