/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Dictionnary managment                                       
 *
 * Authors: I. Vatton, H. Richy, E. Picheral (INRIA)
 *
 */
#include "thot_gui.h"
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

#define MAX_DICTS        2	/* Maximum number of dictionaries related to a given language */
#define MAXLIGNE        80	/* Length of a line in the dictionary                         */
#define MaxDictionaries 15	/* Maximum number of simultaneous dictionaries                */

extern struct Langue_Ctl LangTable[MAX_LANGUAGES];
extern int               FreeEntry;
//extern CHARSET           CharEncoding;

static char        *dictPath;	/* environment variable DICOPAR */
static PtrDict      dictTable[MaxDictionaries];

unsigned            ReverseCode[NbLtr];
unsigned char       Code[256];	/* Script characters */

#include "memory_f.h"
#include "fileaccess_f.h"
#include "thotmsg_f.h"
#include "platform_f.h"

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void LoadAlphabet ()
{
  FILE*         falpha;
  PathBuffer    alphaName;
  unsigned char x;
  int           i;
  
  if (dictPath != NULL)
    strcpy (alphaName, dictPath);
  else
    strcpy (alphaName, "");

  strcat (alphaName, WC_DIR_STR);
  strcat (alphaName, "alphabet");
  if ((falpha = TtaReadOpen (alphaName)) != NULL)
    {
      for (i = 0; i < 256; i++)
        Code[i] = (unsigned char) 100;
      i = 1;
      while ((fscanf (falpha, "%c ", &x) != EOF) && (i < NbLtr))
        {
          Code[x] = (unsigned char) i;
          ReverseCode[i++] = (unsigned char) x;
        }
      TtaReadClose (falpha);
    }
  else
    TtaDisplaySimpleMessage (INFO, LIB, TMSG_MISSING_ALPHABET);
}


/*----------------------------------------------------------------------
  Resolves the common characters for two consecutive words 
  ----------------------------------------------------------------------*/
void TreateDictionary (PtrDict dict)
{
  int                 word, i, k;
  char                lastWord[MAX_WORD_LEN];
  char                currentWord[MAX_WORD_LEN];

  /* An empty dictionary is not considered  (DictNbWords = 0) */
  if (dict->DictNbWords > 0)
    {
      lastWord[0] = 0;
      currentWord[0] = 0;

      for (word = 0; word < dict->DictNbWords; word++)
        {
          k = 0;
          strcpy (lastWord, currentWord);
          strcpy (currentWord, &dict->DictString[dict->DictWords[word]]);
          if (strlen (lastWord) != strlen (currentWord))
            /* changing the size of the word */
            /* => no calculation for the common letters */
            dict->DictCommon[word] = 1;
          else
            {
              /* looking for common letters for two consecutive words */
              /* to avoid the remake of the calculation */
              while (currentWord[k] == lastWord[k] && currentWord[k] != EOS)
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
static void ReleaseDictionary (PtrDict *pDictionary)
{
  int                 d;
  PtrDict             pdict;

  if (*pDictionary != NULL)
    /* looks in the table for the descriptor of the dictionary to release */
    {
      pdict = *pDictionary;
      d = 0;
      while (d < MaxDictionaries && dictTable[d] != pdict)
        d++;
      if (d < MaxDictionaries)
        {
          TtaFreeMemory (pdict->DictDirectory);
          /* Releases the string and the list of words */
          FreeStringInDict (pdict);
          FreeDictionary (pdict);
          dictTable[d] = NULL;
          pdict = NULL;
        }
    }
}


/*----------------------------------------------------------------------
  CreateDictionary: Gets and intializes a dictionary context.            
  In return, pDictionary referenes the dictionary context or NULL if     
  there is a lack of memory.                                             
  ----------------------------------------------------------------------*/
static void CreateDictionary (PtrDict *pDictionary, PtrDocument document)
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
        ReleaseDictionary (&dictTable[d]);
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
static void SearchDictName (PtrDict *pDictionary, char *dictName,
                            char *dictDirectory)
{
  int                 d;
  ThotBool            found;

  found = FALSE;
  d = 0;
  while (d < MaxDictionaries && (dictTable[d] != NULL) && (!found))
    {
      found = (strcmp (dictTable[d]->DictName, dictName) == 0
               && strcmp (dictTable[d]->DictDirectory, dictDirectory) == 0);
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
static int TestDictionary (char *dictName, char *dictDirectory)
{
  int                 ret, i;
  char                tempbuffer[THOT_MAX_CHAR];

  FindCompleteName (dictName, "dic", dictDirectory, tempbuffer, &i);
  if (!TtaFileExist (tempbuffer))	/* Unknown file */
    {
      /* Looks for not pre-treated dictionary */
      FindCompleteName (dictName, "DCT", dictDirectory, tempbuffer, &i);
      if (!TtaFileExist(tempbuffer))
        {
          /* File .DCT unknown: looks for a dictionary LEX not pre-treated */
          FindCompleteName (dictName, "LEX", dictDirectory, tempbuffer, &i);
          if (!TtaFileExist(tempbuffer))
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
static void ReadDictionary (FILE *dictFile, PtrDict dict)
{
  int              i;

  if (!dict->DictLoaded)
    {
      /* Loading ... */
      for (i = 0; i < dict->DictNbChars; i++)
        TtaReadByte (dictFile, (unsigned char *)&(dict->DictString[i]));
      for (i = 0; i < dict->DictNbWords; i++)
        TtaReadByte (dictFile, (unsigned char *)&(dict->DictCommon[i]));
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
static int LoadDict (FILE *dictFile, PtrDict dict)
{
  char             wordGotten[MAX_WORD_LEN];
  char             lineGotten[MAXLIGNE];
  char            *plineGotten;
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
  while (fgets (plineGotten, MAXLIGNE, dictFile) != NULL)
    {
      nbGotten = sscanf (plineGotten, "%s", wordGotten);
      if ((nbGotten > 0)
          && (dict->DictNbWords < maxWord - 1)
          && ((length = strlen (wordGotten)) < MAX_WORD_LEN)
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
            dict->DictString[nbChar++] = (char) Code[(unsigned char) wordGotten[k]];
          /* End of a word */
          dict->DictString[nbChar++] = EOS;

          /* going to the next word: reading the next line */
          plineGotten = &lineGotten[0]; /* pointer on the first character read */
        }
      else if (nbGotten != -1)
        /* not the end of the dictionary */
        {
          /* impossible to load the dictionary */
          /* Release the dictionary */
          ReleaseDictionary (&dict);	/* => dict = nil */
          return (0);
        }
    }

  /* Adding an empty word at the end of the dictionary */
  last_word = dict->DictNbWords;
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
static void PrepareDictionary (PtrDict *pDictionary, char *dictName,
                               PtrDocument document, char *dictDirectory,
                               Language lang, ThotBool readonly,
                               ThotBool treated, ThotBool toTreat)
{
  char                tempbuffer[THOT_MAX_CHAR];
  ThotBool            new_ = FALSE;
  ThotBool            ret;
  FILE               *dictFile;
  PtrDict             pdict;
  int                 i, im, ic;

  *pDictionary = NULL;
  /* Opening the file */
  if (treated)
    FindCompleteName (dictName, "dic", dictDirectory, tempbuffer, &i);
  else
    {
      if (toTreat)
        FindCompleteName (dictName, "DCT", dictDirectory, tempbuffer, &i);
      else
        FindCompleteName (dictName, "LEX", dictDirectory, tempbuffer, &i);
    }
  if (readonly == FALSE)
    {
      /* Alterable dictionary */
      if (TtaFileExist (tempbuffer))
        dictFile = TtaRWOpen (tempbuffer);
      else
        {
          /* new dictionary */
          new_ = TRUE;
          dictFile = TtaWriteOpen (tempbuffer);
        }
    }
  else
    /* READONLY dictionary (generally pre-treated) */
    dictFile = TtaReadOpen (tempbuffer);
  
  if (dictFile == NULL)
    /* Inacessible dictionary: *pDictionary = NULL */
    return;
  
  /* Loading the dictionary */
  /* creation of the structures of the dictionary */
  /* if no memory, a dictionary related to another document will be flushed */
  CreateDictionary (pDictionary, document);
  if (*pDictionary == NULL)
    {
      TtaReadClose (dictFile);
      /* no memory */
      return;
    }
  
  pdict = *pDictionary;
  pdict->DictDoc = document;
  pdict->DictLanguage = lang;
  pdict->DictDirectory = TtaStrdup (dictDirectory);
  strcpy (pdict->DictName, dictName);
  pdict->DictReadOnly = readonly;
  
  /* calculation of the memory size needed by the dictionary */
  if (!new_)
    {
      if (treated)
        /* dictionary already treated */
        {
          ret = TtaReadInteger (dictFile, &i);
          if (!ret)
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
          fgets (tempbuffer, 100, dictFile);
          if (tempbuffer[0] != EOS)
            {
              if (sscanf (tempbuffer, "%d%d", &im, &ic) == 2)
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
                  TtaWriteClose (dictFile);
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
      /* Release the allocated dictionary */
      ReleaseDictionary (pDictionary);
      *pDictionary = NULL;
    }
  else if (treated)
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
  TtaReadClose (dictFile);
}


/*----------------------------------------------------------------------
  LoadTreatedDict returns -1 if the dictionary can't be loaded.   
  0 if the dictionary was already loaded.
  1 if the dictionary is loaded.
  Returns in pDictionary : a pointer to dictionary and creates it if
  toCreate = TRUE
  ----------------------------------------------------------------------*/
int LoadTreatedDict (PtrDict *pDictionary, Language lang, PtrDocument document,
                     char *dictName, char *dictDirectory, ThotBool readonly,
                     ThotBool toCreate)
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
          if (readonly)
            {
              /* file inaccesible */
              *pDictionary = NULL;
              ret = -1;
              break;
            }
          if (!toCreate)
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
ThotBool ReloadDictionary (PtrDict *pDictionary)
{
  PtrDict             pdict;
  PtrDocument         document;
  int                 d;
  char                dictName[MAX_NAME_LENGTH];
  char                *dictDirectory;

  document = NULL;
  if (*pDictionary == NULL)
    return (FALSE);

  dictDirectory = NULL;
  pdict = *pDictionary;
  if (pdict != NULL)
    /* Looks for the descriptor of the dictionary to release into the table */
    {
      d = 0;
      while (d < MaxDictionaries && dictTable[d] != pdict)
        d++;
      if (d < MaxDictionaries)
        {
          /* Getting information about the dictionary */
          strcpy (dictName, pdict->DictName);
          document = pdict->DictDoc;
          /* save the dictionary directory */
          dictDirectory = pdict->DictDirectory;
          /* Release the string and the list of words ... */
          FreeStringInDict (pdict);
          FreeDictionary (pdict);
          dictTable[d] = NULL;
          pdict = NULL;
        }
    }

  d = LoadTreatedDict (pDictionary, 0, document, dictName, dictDirectory,
                       FALSE, TRUE);
  TtaFreeMemory (dictDirectory);

  if (d == -1)
    return (FALSE);

  return (TRUE);
}


/*----------------------------------------------------------------------
  Dict_Init                                                       
  ----------------------------------------------------------------------*/
void Dict_Init ()
{
  int                 i;

  /* Inititializing pointers of the dictionary */
  for (i = 0; i < MaxDictionaries; i++)
    dictTable[i] = NULL;

  /* Inititializing of environments needed by dictionarires */
  dictPath = TtaGetEnvString ("DICOPAR");
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
ThotBool TtaLoadLanguageDictionaries (Language languageId)
{
  int                 lang;
  PtrDict             dictPtr;

  /* If the variable dictPath is not loaded -> do nothig */
  if (dictPath == NULL)
    return FALSE;

  lang = (int) languageId - FirstUserLang;
  /* Verifies if the main dictionary is already loaded */
  if (LangTable[lang].LangDict[0] == NULL)
    {
      /* Loading the main dictionary */
      if (LangTable[lang].LangPrincipal[0] != EOS && !LangTable[lang].LangDict[0])
        {
          dictPtr = NULL;
          LoadTreatedDict (&dictPtr, lang, NULL, LangTable[lang].LangPrincipal,
                           dictPath, TRUE, FALSE);
          if (dictPtr != NULL)
            LangTable[lang].LangDict[0] = (Dictionary) dictPtr;
        }
    }

  /* Verifies if the secondary dictionary is already laded */
  if (LangTable[lang].LangDict[1] == NULL)
    {
      /* Loading the secondary dictionary */
      if (LangTable[lang].LangSecondary[0] != EOS && !LangTable[lang].LangDict[1])
        {
          dictPtr = NULL;
          LoadTreatedDict (&dictPtr, lang, NULL, LangTable[lang].LangSecondary,
                           dictPath, TRUE, FALSE);
          if (dictPtr != NULL)
            LangTable[lang].LangDict[1] = (Dictionary) dictPtr;
        }
    }
  return (LangTable[lang].LangDict[0] != NULL ||
          LangTable[lang].LangDict[1] != NULL);
}

/*----------------------------------------------------------------------
  TtaUnLoadLanguageDictionaries

  Unloads dictionaries associated with a given language.
  Parameters:
  languageId: identifier of the language.
  ----------------------------------------------------------------------*/
void TtaUnLoadLanguageDictionaries (Language languageId)
{
  int                 i, j;

  i = (int) languageId - FirstUserLang;
  j = 0;
  while (LangTable[i].LangDict[j] != NULL && j < MAX_DICTS)
    {
      ReleaseDictionary ((PtrDict *) & LangTable[i].LangDict[j]);
      LangTable[i].LangDict[j] = NULL;
    }
}


/*----------------------------------------------------------------------
  TtaGetPrincipalDictionary

  Returns a pointer to the principal dictionary associated to a language.
  Return value:
  the pointer to that dictionary or NULL if there is no dictionary for
  this language.
  ----------------------------------------------------------------------*/
Dictionary TtaGetPrincipalDictionary (Language languageId)
{
  int                 i;

  i = (int) languageId - FirstUserLang;
  /* Verification of the parameter */
  if (i < 0 || i >= FreeEntry)
    {
      TtaError (ERR_language_not_found);
      return NULL;
    }

  /* Loading dictionaries if exist */
  TtaLoadLanguageDictionaries (languageId);
  return (LangTable[i].LangDict[0]);
}


/*----------------------------------------------------------------------
  TtaGetSecondaryDictionary

  Returns a pointer to the secondary dictionary associated to a language.
  Return value:
  the pointer to that dictionary or NULL if there is no dictionary for
  this language.
  ----------------------------------------------------------------------*/
Dictionary TtaGetSecondaryDictionary (Language languageId)
{
  int                 i;

  i = (int) languageId - FirstUserLang;
  /* Verification of the parameter */
  if (i < 0 || i >= FreeEntry)
    {
      TtaError (ERR_language_not_found);
      return NULL;
    }
  return (LangTable[i].LangDict[1]);
}
