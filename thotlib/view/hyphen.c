/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2002
 *  Please first read the full copyright statement in file COPYRIGHT.
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
#include "ustring.h"
#include "constmedia.h"
#include "typemedia.h"
#include "dictionary.h"
#include "language.h"

#define THOT_EXPORT extern
#include "boxes_tv.h"

static CHAR_T sepcar[] =
{
 TEXT(' '), TEXT('.'), TEXT(','), TEXT('`'), TEXT('\47'), TEXT('-'),
 TEXT(';'), TEXT(':'), TEXT('['), TEXT(']'), TEXT('('), TEXT(')'),
 TEXT('{'), TEXT('}'), TEXT('<'), TEXT('>'), TEXT('/'), TEXT('!'),
 TEXT('?'), TEXT('\n'), TEXT('\240'), TEXT('\241'), TEXT('\277'),
 TEXT('\253'), TEXT('\273'), TEXT('\212'), TEXT('"'), TEXT('\201'),
 TEXT('\202')
};

#include "font_f.h"
#include "language_f.h"
#include "memory_f.h"
#include "uconvert_f.h"
#include "ustring_f.h"


/*----------------------------------------------------------------------
   SmallLettering convertit les caracte`res majuscules en minuscules.   
  ----------------------------------------------------------------------*/
void SmallLettering (unsigned char *word)
{
  unsigned char c;
  int           j;

  j = 0;
  while (word[j] != 0)
    {
      c = word[j];
      if (c >= 65 && c <= 90)
	word[j] = c + 32;		/* Majuscules */
      else if (c >= 192 && c <= 222)
	word[j] = c + 32;		/* Majuscules accentue'es */
      j++;
    }
}

/*----------------------------------------------------------------------
   PatternHyphen  cherche pour le mot word le premier point de     
   coupure qui pre'ce`de le caracte`re de rang length (le  
   rang du premier caracte`re est 0) et retourne 0 si      
   aucune coupure n'est possible ou le rang du caracte`re  
   devant lequel on peut couper.                           
   Si la coupure est possible, indique dans addHyphen s'il 
   faut inse'rer un tiret a` la position de coupure ou non.
  ----------------------------------------------------------------------*/
static int PatternHyphen (STRING word, int length, Language language,
			  ThotBool *addHyphen)
{
  char               *iso;
  int                 i, k;
  int                 status;
  int                *pHyphen;

   *addHyphen = FALSE;
   status = 0;
   /* current patterns concern only iso-latin characters */
   iso = TtaConvertCHARToIso (word, ISO_8859_1);
   /* Convertit le mot en minuscule */
   SmallLettering (iso);
   pHyphen = TtaGetPatternHyphenList (iso, language);
   TtaFreeMemory (iso);
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
ThotBool IsSeparatorChar (CHAR_T c)
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
static int NextWord (SpecFont font, PtrTextBuffer *buffer, int *rank,
		     CHAR_T *word, int *width)
{
   PtrTextBuffer       adbuff;
   int                 i, j;
   int                 lg, nbChars;
   ThotBool            still;
   ThotBool            changedebut;

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
		     lg += BoxCharacterWidth (word[j], font);
		     if (j == 1)
			/* Il faut comptabiliser le caractere precedent */
			lg += BoxCharacterWidth (word[j - 1], font);
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
		       lg += BoxCharacterWidth (word[j], font);
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
static int WordHyphen (STRING word, int length, Language language,
		       ThotBool *hyphen)
{
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
int HyphenLastWord (SpecFont font, Language language, PtrTextBuffer *buffer,
		    int *rank, int *width, ThotBool *hyphen)
{
  PtrTextBuffer       adbuff;
  CHAR_T              word[THOT_MAX_CHAR];
  int                 i, lghyphen;
  int                 length, nbChars;
  int                 w, charWidth;
  int                 retLength, rest;
  int                 wordLength;

  /* Si la coupure de words est active */
  retLength = 0;
  *hyphen = FALSE;
  if (*width > 0 && *buffer != NULL)
    {
      /* La position du debut du word */
      adbuff = *buffer;
      i = *rank - 1;
      if (i >= adbuff->BuLength)
	{
	  if (adbuff->BuNext != NULL)
	    {
	      i = 0;
	      adbuff = adbuff->BuNext;
	    }
	  else
	    return retLength;
	}

      /* Length et largeur des separateurs avant le word */
      nbChars = NextWord (font, &adbuff, &i, word, &w);
      /* Largeur du tiret d'hyphenantion */
      lghyphen = BoxCharacterWidth (173, font);
      /* Espace restant dans la ligne */
      rest = *width - w - lghyphen;
      /* Nombre de carateres maximum du word pouvant entrer dans la ligne */
      if (word != NULL)
	/* On a isole un word assez long */
	wordLength = ustrlen (word);	/* nombre de caraceteres du word isole */
      if (wordLength > 4 && rest > 0)
	{
	  /* Recherche le nombre de caracteres du word qui rentrent */
	  /* dans la ligne */
	  length = 0;
	  charWidth = BoxCharacterWidth (word[length], font);
	  while (rest >= charWidth && length < wordLength)
	    {
	      rest -= charWidth;
	      length++;
	      charWidth = BoxCharacterWidth (word[length], font);
	    }

	  if (length > 1)
	    {
	      /* Recherche un point de coupure pour le word */
	      length = WordHyphen (word, length, language, hyphen);
	      if (length > 0)
		{
		  /* On a trouve un point de coupure */
		  if (*hyphen)
		    *width = w + lghyphen; /* 1ere partie du word */
		  else
		    *width = w;	/* 1ere partie du word */
		  /* number of characters */
		  retLength = length + nbChars;
		  while (length > 0)
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
			  length--;
			  *width += BoxCharacterWidth (adbuff->BuContent[i++], font);
			}
		    }
		  
		  /* Indice dans le buffer 2eme partie du word */
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
  return retLength;
}


/*----------------------------------------------------------------------
   CanHyphen rend de la valeur VRAI s'il existe un traitement de  
   coupure des mots et l'autorisation de coupure pour la   
   boi^te donne'e.                                         
  ----------------------------------------------------------------------*/
ThotBool CanHyphen (PtrBox pBox)
{
  Language            language;

  if (!TextInserting && pBox->BxAbstractBox->AbHyphenate)
    {
      language = pBox->BxAbstractBox->AbLang;
      if (TtaExistPatternList (language))
	/* il existe une table de pattern */
	return TRUE;      
      else
	{
	  /* pas de table de patterns : on cherche a charger un dico */
	  if (language == 0)
	    /* On saute la langue ISOlatin-1 */
	    return FALSE;
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
