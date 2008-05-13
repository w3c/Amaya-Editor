/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2007
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
#include "thot_gui.h"
#include "thot_sys.h"
#include "ustring.h"
#include "constmedia.h"
#include "typemedia.h"
#include "dictionary.h"
#include "language.h"

#define THOT_EXPORT extern
#include "boxes_tv.h"

static int sepchar[] =
{
 /* TEXT(' '), TEXT('.'), TEXT(','), TEXT('`'), TEXT('\47'), TEXT('-') */
  32, 46, 44, 96, 39, 45,
  /* TEXT('#'), TEXT('$'), TEXT('%'), TEXT('&'), TEXT('*'), TEXT('+'), TEXT('=') */
  35, 36, 37, 38, 42, 43, 61,
 /* TEXT(';'), TEXT(':'), TEXT('['), TEXT(']'), TEXT('('), TEXT(')') */
  59, 58, 91, 93, 40, 41,
 /* TEXT('{'), TEXT('}'), TEXT('<'), TEXT('>'), TEXT('/'), TEXT('!') */
  123, 125, 60, 62, 47, 33,
  /* TEXT('\\'), TEXT("^'), TEXT('_'), TEXT('|'), TEXT('~') */
  92, 94, 95, 124, 126,
 /* TEXT('?'), TEXT('\n'), TEXT('\240'), TEXT('\241'), TEXT('\277'), TEXT('\253') */
  63, 10, 160, 161, 191, 171,
 /* TEXT('\273'), TEXT('\212'), TEXT('"'), TEXT('\201'), TEXT('\202'), TEXT('\0') */
  187, 138, 34, 129, 130, 0
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
  while (word[j] != EOS)
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
   iso = (char *)TtaConvertCHARToByte (word, ISO_8859_1);
   /* Convertit le mot en minuscule */
   SmallLettering ((unsigned char *)iso);
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
   int              i;
   
   for (i = 0; sepchar[i]; i++)
     {
       if (c == sepchar[i])
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
static int NextWord (SpecFont font, int variant, PtrTextBuffer *buffer, int *rank,
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
   /* A priori le debuTEXT('\253'),t du mot est correctement repere */
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
		     lg += BoxCharacterWidth (word[j], variant, font);
		     if (j == 1)
			/* Il faut comptabiliser le caractere precedent */
			lg += BoxCharacterWidth (word[j - 1], variant, font);
		     nbChars += j + 1;
		     j = 0;
		  }
		else
		   still = FALSE;
	     else if (IsSeparatorChar (word[j]))
	       {
		  /* On ne traite pas les separateurs en debut de mot */
		  if (j > 0)
		    {
		       word[j] = EOS;
		       still = FALSE;
		    }
		  else
		    {
		       /* Le debut du mot est deplace */
		       changedebut = TRUE;
		       lg += BoxCharacterWidth (word[j], variant, font);
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
  HyphenLastWord tries to hyphen the last word of the line within the
  width limit.
  Returns the number of characters before the break or 0 if the word cannot
  be cut.
  - buffer points to the buffer of the first character after the break.
  - rank is the rank of this first character within the buffer.
  - width is the width of the first part of the word including the hyphen.
  - hyphen is TRUE if an hyphen should be displayed.
  ----------------------------------------------------------------------*/
int HyphenLastWord (SpecFont font, int variant, Language language, PtrTextBuffer *buffer,
    int *rank, int *width, ThotBool *hyphen)
{
  PtrTextBuffer       adbuff;
  CHAR_T              word[THOT_MAX_CHAR];
  int                 i, lghyphen;
  int                 length, nbChars;
  int                 w, charWidth;
  int                 retLength, rest;
  int                 wordLength;

  retLength = 0;
  *hyphen = FALSE;
  if (*width > 0 && *buffer)
    {
      /* locate the first character of the word */
      adbuff = *buffer;
      i = *rank;
      if (i >= adbuff->BuLength)
        {
          if (adbuff->BuNext)
            {
              i = 0;
              adbuff = adbuff->BuNext;
            }
          else
            return retLength;
        }

      /* get the next word */
      nbChars = NextWord (font, variant, &adbuff, &i, word, &w);
      /* hyphen width */
      if (language < TtaGetFirstUserLanguage ())
        lghyphen = 0;
      else
        lghyphen = BoxCharacterWidth (173, 1, font);
      /* width available for the first part of the word */
      rest = *width - w - lghyphen;
      /* length of the word */
      wordLength = ustrlen (word);
      if (wordLength > 4 && rest > 0)
        {
          /* look for a break */
          length = 0;
          charWidth = BoxCharacterWidth (word[length], variant, font);
          while (rest >= charWidth && length < wordLength)
            {
              rest -= charWidth;
              length++;
              charWidth = BoxCharacterWidth (word[length], variant, font);
            }

          if (length > 1)
            {
              /* try to hyphanate the word */
              length = WordHyphen (word, length, language, hyphen);
              if (length > 0)
                {
                  /* found */
                  if (*hyphen)
                    *width = w + lghyphen;
                  else
                    *width = w;
                  /* number of characters */
                  retLength = length + nbChars;
                  while (length > 0)
                    {
                      if (i >= adbuff->BuLength)
                        {
                          adbuff = adbuff->BuNext;
                          i = 0;
                        }
                      else
                        {
                          /* width and length of the first part of the word */
                          length--;
                          *width += BoxCharacterWidth (adbuff->BuContent[i++], variant, font);
                        }
                    }

                  /* locate the second part of the word */
                  if (i >= adbuff->BuLength)
                    {
                      i = 0;
                      adbuff = adbuff->BuNext;
                    }
                  *rank = i;
                  *buffer = adbuff;
                }
            }
        }
    }
  return retLength;
}


/*----------------------------------------------------------------------
   CanHyphen returns TRUE if it's possible to hyphanate this box.
  ----------------------------------------------------------------------*/
ThotBool CanHyphen (PtrBox pBox)
{
  Language            language;

  if (!TextInserting &&
      pBox->BxScript != 'H' && pBox->BxScript != 'A' &&
      pBox->BxScript != 'G' &&
      pBox->BxAbstractBox->AbLeafType == LtText &&
      (pBox->BxAbstractBox->AbHyphenate || pBox->BxScript == 'Z'))
    {
      language = pBox->BxAbstractBox->AbLang;
      if (pBox->BxScript != TtaGetScript (pBox->BxAbstractBox->AbLang))
	/* cannot hyphenate this box */
	return FALSE;
      if (TtaExistPatternList (language))
	/* il existe une table de pattern */
	return TRUE;      
      else
	{
	  /* pas de table de patterns : on cherche a charger un dico */
	  if (language < FirstUserLang)
	    /* On saute la langue ISOlatin-1 */
	    return FALSE;
	  else if (TtaGetPrincipalDictionary (language))
	    /* Traitement par le dictionnaire de la langue */
	    return TRUE;
	  else if (TtaGetSecondaryDictionary (language))
	    /* Pas de traitement de coupure possible */
	    return FALSE;
	  else
	    return FALSE;	  
	}
    }
  else
    return FALSE;
}
