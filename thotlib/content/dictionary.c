/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Dictionnary managment                                       
 *
 * Authors: H. Richy, E. Picheral (INRIA)
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "application.h"
#include "language.h"
#include "constmenu.h"
#include "fileaccess.h"
#include "typecorr.h"
#include "libmsg.h"
#include "message.h"
#include "dictionary.h"
#include "fileaccess.h"
#include "thotdir.h"

#define MAX_DICTS        2	/* Maximum number of dictionaries related to a given language */
#define MAXLIGNE        80	/* Length of a line in the dictionary                         */
#define MaxDictionaries 15	/* Maximum number of simultaneous dictionaries                */

extern struct Langue_Ctl LangTable[MAX_LANGUAGES];
extern struct Langue_Ctl TypoLangTable[MAX_LANGUAGES];
extern int          FreeEntry;

static STRING       dictPath;	/* environment variable DICOPAR */
static PtrDict      dictTable[MaxDictionaries];

unsigned            ReverseCode[NbLtr];
UCHAR_T       Code[256];	/* Alphabet characters */

#include "memory_f.h"
#include "fileaccess_f.h"
#include "thotmsg_f.h"
#include "platform_f.h"


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void           LoadAlphabet ()
{
  FILE               *falpha;
  Buffer              alphaName;
  UCHAR_T       x;
  int                 i;
  
  if (dictPath != NULL)
    ustrcpy (alphaName, dictPath);
  else
    ustrcpy (alphaName, _EMPTYSTR_);

  ustrcat (alphaName, DIR_STR);
  ustrcat (alphaName, TEXT("alphabet"));
  
  if ((falpha = ufopen (alphaName, TEXT("r"))) != NULL)
    {
      for (i = 0; i < 256; i++)
	Code[i] = (UCHAR_T) 100;
      i = 1;
      while ((fscanf (falpha, "%c ", &x) != EOF) && (i < NbLtr))
	{
	  Code[x] = (UCHAR_T) i;
	  ReverseCode[i++] = (UCHAR_T) x;
	}
      fclose (falpha);
    }
  else
    TtaDisplaySimpleMessage (INFO, LIB, TMSG_MISSING_ALPHABET);
}


/*----------------------------------------------------------------------
  Resolves the common characters for two consecutive words 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TreateDictionary (PtrDict dict)
#else  /* __STDC__ */
void                TreateDictionary (dict)
PtrDict             dict;
#endif /* __STDC__ */
{
   int                 word, i;
   CHAR_T                lastWord[MAX_WORD_LEN];
   CHAR_T                currentWord[MAX_WORD_LEN];

   /* An empty dictionary is not considered  (DictNbWords = -1) */
   if (dict->DictNbWords >= 0)
     {
	lastWord[0] = 0;
	currentWord[0] = 0;

	for (word = 0; word < dict->DictNbWords; word++)
	  {
	     int                 k = 0;

	     ustrcpy (lastWord, currentWord);
	     ustrcpy (currentWord, &dict->DictString[dict->DictWords[word]]);

	     if (ustrlen (lastWord) != ustrlen (currentWord))
	       {
		  /* changing the size of the word */
		  /* => no calculation for the common letters */
		  dict->DictCommon[word] = 1;
	       }
	     else
	       {
		  /* looking for common letters for two consecutive words */
		  /* to avoid the remake of the calculation */
		  while (currentWord[k] == lastWord[k])
		     k++;
		  dict->DictCommon[word] = k + 1;
	       }
	  }
	for (i = word; i < dict->DictMaxWords; i++)
	   dict->DictCommon[i] = 1;
     }
}


/*----------------------------------------------------------------------
   ReleaseDictionary: Releases dictionaries which descriptor is    
   referenced by pDictionary.                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ReleaseDictionary (PtrDict * pDictionary)
#else  /* __STDC__ */
static void         ReleaseDictionary (pDictionary)
PtrDict            *pDictionary;
#endif /* __STDC__ */
{
   int                 d;

   if (*pDictionary != NULL)
      /* looks in the table for the descriptor of the dictionary to release */
     {
	d = 0;
	while (d < MaxDictionaries && dictTable[d] != *pDictionary)
	   d++;
	if (dictTable[d] == *pDictionary)
	  {
	     /* Releases the string and the list of words */
	     FreeStringInDict (*pDictionary);
	     FreeDictionary (*pDictionary);
	     dictTable[d] = NULL;
	     *pDictionary = NULL;
	  }
     }
}


/*----------------------------------------------------------------------
   CreateDictionary: Gets and intializes a dictionary context.            
   In return, pDictionary referenes the dictionary context or NULL if     
   there is a lack of memory.                                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CreateDictionary (PtrDict * pDictionary, PtrDocument document)
#else  /* __STDC__ */
static void         CreateDictionary (pDictionary, document)
PtrDict            *pDictionary;
PtrDocument         document;
#endif /* __STDC__ */
{
  int                 d;

  /* Looks for a pointer to the descriptor of a free dictionary */
  d = 0;
  while (d < MaxDictionaries && dictTable[d] != NULL)
    d++;
  
  if (d < MaxDictionaries && dictTable[d] != NULL)
    {
      /* If a dictionary FILE is loaded but not used, one release it */
      d = 0;
      while (d < MaxDictionaries && dictTable[d]->DictDoc == document)
	d++;
      
      if (d == MaxDictionaries || dictTable[d]->DictReadOnly == TRUE)
	{
	  /* Looking for a dictionary FILE */
	  d = 0;
	  while (d < MaxDictionaries && dictTable[d]->DictReadOnly == TRUE)
	    d++;
	}
      /* Flushing the dictionary FILE  */
      if (d < MaxDictionaries)
	{
	  TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_ERR_LOADING_DICO),
			     dictTable[d]->DictName);
	  ReleaseDictionary (&dictTable[d]);
	}
    }
  
  if (d < MaxDictionaries)
    {
      /* Getting a descriptor of a dictionary */
      GetDictionary (&dictTable[d]);
      *pDictionary = dictTable[d];
      (*pDictionary)->DictDoc = document;
    }
}


/*----------------------------------------------------------------------
   SearchDictName: Looks for a dictionary by its name and returns  
   a pointer (pDictionary) referencing its descriptor or NULL    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SearchDictName (PtrDict * pDictionary, STRING dictName, STRING dictDirectory)
#else  /* __STDC__ */
static void         SearchDictName (pDictionary, dictName, dictDirectory)
PtrDict            *pDictionary;
STRING              dictName;
STRING              dictDirectory;
#endif /* __STDC__ */
{
   int                 d;
   ThotBool            found;

   found = FALSE;
   d = 0;
   while (d < MaxDictionaries && (dictTable[d] != NULL) && (!found))
     {
	found = (ustrcmp (dictTable[d]->DictName, dictName) == 0
	       && ustrcmp (dictTable[d]->DictDirectory, dictDirectory) == 0);
	d++;
     }
   if (found)
      *pDictionary = dictTable[d - 1];
   else
      *pDictionary = NULL;
}


/*----------------------------------------------------------------------
   TestDictionary verifies if the file which name is dictName exists 
   returns -1 if the file is not found (inaccessible)                
   returns  0 if the file .DCT exists (not treated yet)              
   returns  1 if the file .DCT exists (treated)                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          TestDictionary (char* dictName, char* dictDirectory)
#else  /* __STDC__ */
static int          TestDictionary (dictName, dictDirectory)
char*               dictName;
char*               dictDirectory;

#endif /* __STDC__ */
{
  int                 ret, i;
  char                tempbuffer[THOT_MAX_CHAR];

  FindCompleteName (dictName, "dic", dictDirectory, tempbuffer, &i);
  if (TtaFileExist (tempbuffer) == 0)	/* Unknown file */
    {
      /* Looks for not pre-treated dictionary */
      FindCompleteName (dictName, TEXT("DCT"), dictDirectory, tempbuffer, &i);
      if (TtaFileExist (tempbuffer) == 0)
	{
	  /* File .DCT unknown: looks for a dictionary LEX not pre-treated */
	  FindCompleteName (dictName, _LEXCST_, dictDirectory, tempbuffer, &i);
	  if (TtaFileExist (tempbuffer) == 0)
	    /* unknown file */
	    ret = -1;
	  else
	    /* File .LEX exists */
	    ret = 2;
	}
      else
	/* File .DCT exists */
	ret = 0;
    }
  else
    /* file .dic */
    ret = 1;
  return (ret);
}


/*----------------------------------------------------------------------
   ReadDictionary reads a dictionary from a pre-treated file.       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ReadDictionary (FILE * dictFile, PtrDict dict)
#else  /* __STDC__ */
static void         ReadDictionary (dictFile, dict)
FILE               *dictFile;
PtrDict             dict;
#endif /* __STDC__ */
{
  int              i;

  if (!dict->DictLoaded)
    {
      /* Loading ... */
      i = 0;
      while (i < dict->DictNbChars)
	{
	  TtaReadByte (dictFile, &(dict->DictString[i]));
	  i++;
	}
      
      i = 0;
      while (i < dict->DictNbWords)
	{
	  TtaReadByte (dictFile, &(dict->DictCommon[i]));
	  i++;
	}
      
      for (i = 0; i < dict->DictNbWords; i++)
	TtaReadInteger (dictFile, &dict->DictWords[i]);
      
      for (i = 0; i < MAX_WORD_LEN; i++)
	TtaReadInteger (dictFile, &dict->DictLengths[i]);
      
      /* Loaded */
      dict->DictLoaded = TRUE;
    }
}


/*----------------------------------------------------------------------
   LoadDict returns 1 if a dictionary is loaded else returns 0
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          LoadDict (FILE * dictFile, PtrDict dict)
#else  /* __STDC__ */
static int          LoadDict (dictFile, dict)
FILE               *dictFile;
PtrDict             dict;
#endif /* __STDC__ */
{
   CHAR_T             wordGotten[MAX_WORD_LEN];
   CHAR_T             lineGotten[MAXLIGNE];
   STRING           plineGotten;
   int              i, k, length, nbGotten, last_word;
   int              MaxWord, maxWord;
   int              currentLength = 0;
   int              nbChar = 0;  /* Number of characters in the dictionary  */

   /* Dictionary being loaded ... */
   MaxWord = dict->DictMaxChars;
   maxWord = dict->DictMaxWords;
   /* pointer on the first character read */
   plineGotten = &lineGotten[0];

   /* Loading the dictionary */
   while (ufgets (plineGotten, MAXLIGNE, dictFile) != NULL)
     {
	nbGotten = usscanf (plineGotten, TEXT("%s"), wordGotten);
	if ((nbGotten > 0)
	    && (dict->DictNbWords < maxWord - 1)
	    && ((length = ustrlen (wordGotten)) < MAX_WORD_LEN)
	    && (length + nbChar + 1 < MaxWord - 1))
	  {
	     dict->DictNbWords++;
	     plineGotten = plineGotten + length;
	     dict->DictWords[dict->DictNbWords] = nbChar;
	     if (length != currentLength)
	       {
		  for (k = currentLength + 1; k <= length; k++)
		     dict->DictLengths[k] = dict->DictNbWords;
		  currentLength = length;
	       }
	     for (k = 0; k < length; k++)
		dict->DictString[nbChar++] = (CHAR_T) Code[(UCHAR_T) wordGotten[k]];
	     /* End of a word */
	     dict->DictString[nbChar++] = EOS;

	     /* going to the next word: reading the next line */
	     plineGotten = &lineGotten[0]; /* pointer on the first character read */
	  }
	else if (nbGotten != -1)
	  /* not the end of the dictionary */
	  {
	     /* impossible to load the dictionary */
	     TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_ERR_LOADING_DICO),
				dict->DictName);
	     /* Release the dictionary */
	     ReleaseDictionary (&dict);	/* => dict = nil */
	     return (0);
	  }
     }

   /* Adding an empty word at the end of the dictionary */
   last_word = dict->DictNbWords + 1;
   dict->DictWords[last_word] = nbChar;
   /* Updating the pointers */
   for (i = currentLength + 1; i < MAX_WORD_LEN; i++)
      dict->DictLengths[i] = last_word;
   dict->DictString[nbChar] = EOS;
   dict->DictNbChars = nbChar;
   dict->DictLoaded = TRUE;
   return (1);
}


/*----------------------------------------------------------------------
   PrepareDictionary : cherche a charger le dictionnaire dictName         
   de langue lang et de type tdico                                   
   Traite le dico si toTreat = TRUE.                                
   retourne dans pDictionary le pointeur sur son descripteur ou NULL        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PrepareDictionary (PtrDict * pDictionary, STRING dictName, PtrDocument document, STRING dictDirectory, Language lang, ThotBool readonly, ThotBool treated, ThotBool toTreat)
#else  /* __STDC__ */
static void         PrepareDictionary (pDictionary, dictName, document, dictDirectory, lang, readonly, treated, toTreat)
PtrDict            *pDictionary;
STRING              dictName;
PtrDocument         document;
STRING              dictDirectory;
Language            lang;
ThotBool            readonly;
ThotBool            treated;
ThotBool            toTreat;
#endif /* __STDC__ */
{
  CHAR_T              tempbuffer[THOT_MAX_CHAR];
  ThotBool            new = FALSE;
  ThotBool            ret;
  FILE*               dictFile;
  PtrDict             pdict;
  int                 i, im, ic;

  *pDictionary = NULL;
  /* Opening the file */
  if (treated)
    FindCompleteName (dictName, TEXT("dic"), dictDirectory, tempbuffer, &i);
  else
    {
      if (toTreat)
	FindCompleteName (dictName, TEXT("DCT"), dictDirectory, tempbuffer, &i);
      else
	FindCompleteName (dictName, _LEXCST_, dictDirectory, tempbuffer, &i);
    }
  if (readonly == FALSE)
    {
      /* Alterable dictionary */
      if (TtaFileExist (tempbuffer) != 0)
	{
	  dictFile = ufopen (tempbuffer, _RW_MODE_);
	  /* updating the dictionary */
	  TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_DICO), dictName);
	}
      else
	{
	  new = TRUE;
	  dictFile = ufopen (tempbuffer, _AppendMODE_);
	  /* new dictionary */
	  TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_NEW_DICO), dictName);
	}
    }
  else
    {
      /* READONLY dictionary (generally pre-treated) */
      if (treated == TRUE)
	dictFile = TtaReadOpen (tempbuffer);
      else
	dictFile = ufopen (tempbuffer, TEXT("r"));
      TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_DICO), dictName);
    }
  
  if (dictFile == NULL)
    /* Inacessible dictionary: *pDictionary = NULL */
    return;
  
  /* Loading the dictionary */
  /* creation of the structures of the dictionary */
  /* if no memory, a dictionary related to another document will be flushed */
  CreateDictionary (pDictionary, document);
  if (*pDictionary == NULL)
    {
      fclose (dictFile);
      /* no memory */
      return;
    }
  
  pdict = *pDictionary;
  pdict->DictDoc = document;
  pdict->DictLanguage = lang;
  ustrcpy (pdict->DictDirectory, dictDirectory);
  ustrcpy (pdict->DictName, dictName);
  pdict->DictReadOnly = readonly;
  
  /* calculation of the memory size needed by the dictionary */
  if (new == FALSE)
    {
      if (treated)
	/* dictionary already treated */
	{
	  ret = TtaReadInteger (dictFile, &i);
	  if (ret == FALSE)
	    {
	      TtaReadClose (dictFile);
	      /* no memory */
	      return;
	    }
	  pdict->DictMaxWords = i;
	  pdict->DictNbWords = i;
	  TtaReadInteger (dictFile, &i);
	  pdict->DictMaxChars = i;
	  pdict->DictNbChars = i;
	}
      else
	{
	  /* Get the length of strings required at the begenning of the file. */
	  tempbuffer[0] = EOS;
	  ufgets (tempbuffer, 100, dictFile);
	  if (tempbuffer[0] != EOS)
	    {
	      if (usscanf (tempbuffer, TEXT("%d%d"), &im, &ic) == 2)
		{
		  pdict->DictMaxWords = im;
		  pdict->DictMaxChars = ic;
		}
	      else
		{
		  /* The head of the file does not contain the words number ... */
		  /* -> one go to the begenning of the file */
		  /* fseek(dictFile, 0L, 0); */
		  /* impossible to load this dictionary */
		  TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_ERR_LOADING_DICO), dictName);
		  /* Release the dictionary */
		  ReleaseDictionary (pDictionary);
		  *pDictionary = NULL;
		  fclose (dictFile);
		  return;
		}
	    }
	  else
	    {
	      /* Only a FILE dictionary may be empty at the starting */
	      if (readonly != FALSE)
		{
		  fclose (dictFile);
		  /* Error while reading, the FILE dictionary is empty */
		  return;
		}
	    }
	}
    }
  
  /* Provide space for 50 words and 600 characters if not readonly */
  if (GetStringInDict (pDictionary, readonly) == -1)
    {
      /* Not enough memory to open this dictionary */
      TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_LIB_NO_LOAD), dictName);
      /* Release the allocated dictionary */
      ReleaseDictionary (pDictionary);
      *pDictionary = NULL;
    }
  else if (treated == TRUE)
    /* Read the pre-treated file */
    ReadDictionary (dictFile, pdict);
  else if (!pdict->DictLoaded)
    {
      /* Reading a not treated file */
      if (LoadDict (dictFile, pdict) == 1)
	{
	  if (toTreat)
	    /* pre-treatement of the dictionary */
	    TreateDictionary (pdict);
	}
      else
	/* The loading of the dictionary failed */
	*pDictionary = NULL;
    }
  fclose (dictFile);
}


/*----------------------------------------------------------------------
  LoadTreatedDict returns -1 if the dictionary can't be loaded.   
  0 if the dictionary was already loaded.
  1 if the dictionary is loaded.
  Returns in pDictionary : a pointer to dictionary and creates it if
  toCreate = TRUE
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 LoadTreatedDict (PtrDict * pDictionary, Language lang, PtrDocument document, STRING dictName, STRING dictDirectory, ThotBool readonly, ThotBool toCreate)
#else  /* __STDC__ */
int                 LoadTreatedDict (pDictionary, lang, document, dictName, dictDirectory, readonly, toCreate)
PtrDict            *pDictionary;
Language            lang;
PtrDocument         document;
STRING              dictName;
STRING              dictDirectory;
ThotBool            readonly;
ThotBool            toCreate;
#endif /* __STDC__ */
{
   PtrDict             pdict;
   int                 i;
   int                 ret = 0;

   pdict = *pDictionary;
   /* Is the dictionary already in dictTable */
   SearchDictName (&pdict, dictName, dictDirectory);
   if (pdict == NULL)
     {
	/* If the required dictionary exists it will be loaded  ... */
	i = TestDictionary (dictName, dictDirectory);
	switch (i)
	      {
		 case (-1):
		    if (readonly == TRUE)
		      {
			 /* file inaccesible */
			 *pDictionary = NULL;
			 ret = -1;
			 break;
		      }
		    if (toCreate != TRUE)
		      {
			 /* Do not create the not readonly dictionary yet */
			 *pDictionary = NULL;
			 ret = -1;
			 break;
		      }
		    /* else: create the new dictionary not readonly */
		 case (0):
		    /* file .DCT */
		    PrepareDictionary (&pdict, dictName, document,
				dictDirectory, lang, readonly, FALSE, TRUE);
		    ret = (pdict == NULL) ? -1 : 1;
		    break;
		 case (1):
		    /* file .dic */
		    PrepareDictionary (&pdict, dictName, document,
				dictDirectory, lang, readonly, TRUE, FALSE);
		    ret = (pdict == NULL) ? -1 : 1;
		    break;
		 case (2):
		    /* file .LEX */
		    PrepareDictionary (&pdict, dictName, document,
			       dictDirectory, lang, readonly, FALSE, FALSE);
		    ret = (pdict == NULL) ? -1 : 1;
		    break;
	      }

     }
   else
      /* The dictionary was already loaded by another document */
      /* just update the dictionary context */
      pdict->DictDoc = document;

   *pDictionary = pdict;
   return ret;
}


/*----------------------------------------------------------------------
   ReloadDictionary: reload a dictionary                            
   returns TRUE if the FILE dictionary is found and well loaded       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            ReloadDictionary (PtrDict * pDictionary)
#else  /* __STDC__ */
ThotBool            ReloadDictionary (pDictionary)
PtrDict            *pDictionary;
#endif /* __STDC__ */
{
   PtrDict             pdict;
   PtrDocument         document;
   int                 d;
   Name                dictName;

   document = NULL;
   if (*pDictionary == NULL)
      return (FALSE);

   pdict = *pDictionary;
   if (pdict != NULL)
      /* Looks for the descriptor of the dictionary to release into the table */
     {
	d = 0;
	while (d < MaxDictionaries && dictTable[d] != pdict)
	   d++;
	if (dictTable[d] == pdict)
	  {
	     /* Getting information about the dictionary */
	     ustrcpy (dictName, pdict->DictName);
	     document = pdict->DictDoc;
	     /* Release the string and the list of words ... */
	     FreeStringInDict (pdict);
	     FreeDictionary (pdict);
	     dictTable[d] = NULL;
	     pdict = NULL;
	  }
     }

   d = LoadTreatedDict (pDictionary, EOS, document, dictName,
		 document->DocDirectory, FALSE, TRUE);
   if (d == -1)
      return (FALSE);

   return (TRUE);
}


/*----------------------------------------------------------------------
   Dict_Init                                                       
  ----------------------------------------------------------------------*/
void                Dict_Init ()
{
   int                 i;

   /* Inititializing pointers of the dictionary */
   for (i = 0; i < MaxDictionaries; i++)
      dictTable[i] = NULL;

   /* Inititializing of environments needed by dictionarires */
   dictPath = TtaGetEnvString ("DICOPAR");
   if (dictPath == NULL)
     {
	/* The environment variable DICOPAR does not exist */
	TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_MISSING_DICOPAR), "DICOPAR");
     }
   LoadAlphabet ();
}


/*----------------------------------------------------------------------
   TtaLoadLanguageDictionaries

   Loads the dictionary associated with a language, if it is not loaded yet
   and registers that a dictionary associated with this language has been loaded.

   Returns -1 if the mandatory dictionary cann't be loaded.
   0 if no dictionary has been loaded
   1 if the mandatory dictionary is loaded.

   Parameters:
   languageId: name of the concerned language.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            TtaLoadLanguageDictionaries (Language languageId)
#else  /* __STDC__ */
ThotBool            TtaLoadLanguageDictionaries (languageId)
Language            languageId;
#endif /* __STDC__ */
{
   int                 lang;
   PtrDict             dictPtr;

   /* If the variable dictPath is not loaded -> do nothig */
   if (dictPath == NULL)
      return FALSE;

   lang = (int) languageId;
   /* Verifies if the main dictionary is already loaded */
   if (LangTable[lang].LangDict[0] == NULL)
     {
	/* Loading the main dictionary */
	if (LangTable[lang].LangPrincipal[0] != EOS)
	  {
	     LoadTreatedDict (&dictPtr, lang, NULL, LangTable[lang].LangPrincipal, dictPath, TRUE, FALSE);
	     if (dictPtr != NULL)
		LangTable[lang].LangDict[0] = (Dictionary) dictPtr;
	  }
     }

   /* Verifies if the secondary dictionary is already laded */
   if (LangTable[lang].LangDict[1] == NULL)
     {
	/* Loading the secondary dictionary */
	if (LangTable[lang].LangSecondary[0] != EOS)
	  {
	     LoadTreatedDict (&dictPtr, lang, NULL, LangTable[lang].LangSecondary, dictPath, TRUE, FALSE);
	     if (dictPtr != NULL)
		LangTable[lang].LangDict[1] = (Dictionary) dictPtr;
	  }
     }
   return (LangTable[lang].LangDict[0] != NULL || LangTable[lang].LangDict[1] != NULL);
}


/*----------------------------------------------------------------------
   TtaLoadTypoDictionaries

   Loads the dictionary associated with a typolanguage, if it is not loaded yet
   and registers that a dictionary associated with this language has been loaded.

   Returns -1 if the mandatory dictionary cann't be loaded.
   0 if no dictionary has been loaded
   1 if the mandatory dictionary is loaded.

   Parameters:
   languageId: name of the concerned language.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            TtaLoadTypoDictionaries (Language languageId)
#else  /* __STDC__ */
ThotBool            TtaLoadTypoDictionaries (languageId)
Language            languageId;
#endif /* __STDC__ */
{
   int                 lang;
   PtrDict             dictPtr;
   int                 ret;

   /* If the variable dictPath is not loaded -> do nothig */
   if (dictPath == NULL)
      return FALSE;

   ret = 1;
   lang = (int) languageId;
   /* Verifies if the main dictionary is already loaded */
   if (TypoLangTable[lang].LangDict[0] == NULL)
     {
	/* Loading the main dictionary */
	if (TypoLangTable[lang].LangPrincipal[0] != EOS)
	  {
	     ret = LoadTreatedDict (&dictPtr, lang, NULL,
		  TypoLangTable[lang].LangPrincipal, dictPath, TRUE, FALSE);
	     if (ret > 0)
		TypoLangTable[lang].LangDict[0] = (Dictionary) dictPtr;
	  }
     }

   /* Verifies if the secondary dictionary is already laded */
   if (TypoLangTable[lang].LangDict[1] == NULL)
     {
	/* Loading the secondary dictionary */
	if (TypoLangTable[lang].LangSecondary[0] != EOS)
	  {
	     ret = LoadTreatedDict (&dictPtr, lang, NULL,
		  TypoLangTable[lang].LangSecondary, dictPath, TRUE, FALSE);
	     if (ret > 0)
		TypoLangTable[lang].LangDict[1] = (Dictionary) dictPtr;
	  }
     }
   return (TypoLangTable[lang].LangDict[0] != NULL
	   || TypoLangTable[lang].LangDict[1] != NULL);
}


/*----------------------------------------------------------------------
   TtaUnLoadLanguageDictionaries

   Unloads dictionaries associated with a given language.

   Parameters:
   languageId: identifier of the language.

  ----------------------------------------------------------------------*/
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
   while (LangTable[i].LangDict[j] != NULL && j < MAX_DICTS)
     {
	ReleaseDictionary ((PtrDict *) & LangTable[i].LangDict[j]);
	LangTable[i].LangDict[j] = NULL;
     }
}


/*----------------------------------------------------------------------
   TtaUnLoadTypoDictionaries

   Unloads dictionaries associated with a given language.

   Parameters:
   languageId: identifier of the language.
  ----------------------------------------------------------------------*/
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
   while (TypoLangTable[i].LangDict[j] != NULL && j < MAX_DICTS)
     {
	ReleaseDictionary ((PtrDict *) & TypoLangTable[i].LangDict[j]);
	TypoLangTable[i].LangDict[j] = NULL;
     }
}


/*----------------------------------------------------------------------
   TtaGetPrincipalDictionary

   Returns a pointer to the principal dictionary associated to a language.

   Return value:
   the pointer to that dictionary or NULL if there is no dictionary for
   this language.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Dictionary          TtaGetPrincipalDictionary (Language languageId)
#else  /* __STDC__ */
Dictionary          TtaGetPrincipalDictionary (languageId)
Language            languageId;
#endif /* __STDC__ */
{
   int                 i;

   i = (int) languageId;
   /* Verification of the parameter */
   if (i >= FreeEntry)
     {
	TtaError (ERR_language_not_found);
	return NULL;
     }

   /* Loading dictionaries if exist */
   TtaLoadLanguageDictionaries (languageId);
   return (LangTable[i].LangDict[0]);
}


/*----------------------------------------------------------------------
   TtaGetPrincipalTypoDictionary

   Returns a pointer to the principal dictionary associated to a language.

   Return value:
   the pointer to that dictionary or NULL if there is no dictionary for
   this language.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Dictionary          TtaGetPrincipalTypoDictionary (Language languageId)
#else  /* __STDC__ */
Dictionary          TtaGetPrincipalTypoDictionary (languageId)
Language            languageId;
#endif /* __STDC__ */
{
   int                 i;

   i = (int) languageId;
   /* Verification of the parameter */
   if (i >= FreeEntry)
     {
	TtaError (ERR_language_not_found);
	return NULL;
     }

   /* Loading dictionaries if exist */
   TtaLoadTypoDictionaries (languageId);
   return (TypoLangTable[i].LangDict[0]);
}


/*----------------------------------------------------------------------
   TtaGetSecondaryDictionary

   Returns a pointer to the secondary dictionary associated to a language.

   Return value:
   the pointer to that dictionary or NULL if there is no dictionary for
   this language.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Dictionary          TtaGetSecondaryDictionary (Language languageId)
#else  /* __STDC__ */
Dictionary          TtaGetSecondaryDictionary (languageId)
Language            languageId;
#endif /* __STDC__ */
{
   int                 i;

   i = (int) languageId;
   /* Verification of the parameter */
   if (i >= FreeEntry)
     {
	TtaError (ERR_language_not_found);
	return NULL;
     }
   return (LangTable[i].LangDict[1]);
}


/*----------------------------------------------------------------------
   TtaGetSecondaryTypoDictionary

   Returns a pointer to the secondary dictionary associated to a language.

   Return value:
   the pointer to that dictionary or NULL if there is no dictionary for
   this language.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Dictionary          TtaGetSecondaryTypoDictionary (Language languageId)
#else  /* __STDC__ */
Dictionary          TtaGetSecondaryTypoDictionary (languageId)
Language            languageId;
#endif /* __STDC__ */
{
   int                 i;

   i = (int) languageId;
   /* Verification of the parameter */
   if (i >= FreeEntry)
     {
	TtaError (ERR_language_not_found);
	return NULL;
     }
   return (TypoLangTable[i].LangDict[1]);
}
