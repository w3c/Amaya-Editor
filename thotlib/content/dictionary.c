/*
 * Copyright (c) 1996 INRIA, All rights reserved
 */

/*----------------------------------------------------------------------
   
   Thot Toolkit: Application Program Interface                     
   Dictionnary managment                                       
  ----------------------------------------------------------------------*/

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
#include "thotfile.h"
#include "thotdir.h"

#define MAX_DICTS        2  /* Maximum number of dictionaries related to a given language */
#define MAXLIGNE        80  /* Length of a line in the dictionary                         */
#define MaxDictionaries 15  /* Maximum number of simultaneous dictionaries                */

extern struct Langue_Ctl LangTable[MAX_LANGUAGES];
extern struct Langue_Ctl TypoLangTable[MAX_LANGUAGES];
extern int          FreeEntry;

static char        *dictPath;	        /* environment variable DICOPAR */
static boolean      alphabetLoaded;
static unsigned     reverseCode[NbLtr];    
static PtrDict      dictTable[MaxDictionaries];

unsigned char       Code[256];	/* Alphabet characters */

#include "memory_f.h"
#include "fileaccess_f.h"
#include "thotmsg_f.h"

#include "platform_f.h"

/***************************** ALPHABET ************************************/
boolean             Corr_alphabet ()
{
   FILE               *falpha;
   Buffer              alphaName;
   boolean             ret = FALSE;
   unsigned char       x;
   int                 i;

   if (dictPath != NULL)
      strcpy (alphaName, dictPath);
   else
      strcpy (alphaName, "");

   strcat (alphaName, "/alphabet");	/* iso alphabet */
   if ((falpha = fopen (alphaName, "r")) != NULL)
     {
	ret = TRUE;
	for (i = 0; i < 256; i++)
	   Code[i] = (unsigned char) 100;
	i = 1;
	while ((fscanf (falpha, "%c ", &x) != EOF) && (i < NbLtr))
	  {
	     Code[x] = (unsigned char) i;
	     reverseCode[i++] = (unsigned char) x;
	  }
	fclose (falpha);
     }
   return (ret);
}

/***************************** asci2code *********************************/
#ifdef __STDC__
void                asci2code (char *string)
#else  /* __STDC__ */
void                asci2code (string)
char               *string;

#endif /* __STDC__ */
{
   int                 i;

   i = 0;
   while (string[i] != '\0')
     {
	string[i] = Code[(unsigned char) string[i]];
	i++;
     }
}				/*asci2code */


/***************************** code2asci *********************************/
#ifdef __STDC__
void                code2asci (char *string)
#else  /* __STDC__ */
void                code2asci (string)
char               *string;

#endif /* __STDC__ */
{
   int                 i = 0;

   while (string[i] != '\0')
     {
	string[i] = reverseCode[(unsigned char) string[i]];
	i++;
     }
}				/*code2asci */


/************************ Corr_pretraitement *****************************/
/* 
 * Resolves the common characters for two consecutive words 
 */
#ifdef __STDC__
void                Corr_pretraitement (PtrDict dict)
#else  /* __STDC__ */
void                Corr_pretraitement (dict)
PtrDict             dict;

#endif /* __STDC__ */
{
   int                 word, i;
   char                lastWord[MAX_WORD_LEN];
   char                currentWord[MAX_WORD_LEN];

   /* An empty dictionary is not considered  (nbmots = -1) */
   if (dict->nbmots >= 0)
     {
	lastWord[0] = 0;
	currentWord[0] = 0;

	for (word = 0; word < dict->nbmots; word++)
	  {
	     int                 k = 0;

	     strcpy (lastWord, currentWord);
	     strcpy (currentWord, &dict->chaine[dict->pdico[word]]);

	     if (strlen (lastWord) != strlen (currentWord))
	       {
		  /* changing the size of the word */
		  /* => no calculation for the common letters */
		  dict->commun[word] = 1;
	       }
	     else
	       {
		  /* looking for common letters for two consecutive words */
		  /* to avoid the remake of the calculation */
		  while (currentWord[k] == lastWord[k])
		     k++;
		  dict->commun[word] = k + 1;
	       }
	  }
	for (i = word; i < dict->MAXmots; i++)
	   dict->commun[i] = 1;	
     }
}				/*Corr_pretraitement */


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
	  TtaDisplayMessage (INFO, TtaGetMessage(LIB, TMSG_ERR_LOADING_DICO),
			     dictTable[d]->DictNom);
	  ReleaseDictionary (&dictTable[d]);
	  
	}
     }				/* end of if (dictTable[d] != NULL) */
  
  if (d < MaxDictionaries)
    
    {
      /* Getting a descriptor of a dictionary */
      GetDictionary (&dictTable[d]);
      *pDictionary = dictTable[d];
      (*pDictionary)->DictDoc = document;
    }
}				/*CreateDictionary */


/*----------------------------------------------------------------------
   SearchDictName: Looks for a dictionary by its name and returns  
   a pointer (pDictionary) referencing its descriptor or NULL    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SearchDictName (PtrDict * pDictionary, char *dictName, char *dictDirectory)
#else  /* __STDC__ */
static void         SearchDictName (pDictionary, dictName, dictDirectory)
PtrDict            *pDictionary;
char               *dictName;
char               *dictDirectory;

#endif /* __STDC__ */
{
   int                 d;
   boolean             found;

   found = FALSE;
   d = 0;
   while (d < MaxDictionaries && (dictTable[d] != NULL) && (!found))
     {
	found = (strcmp (dictTable[d]->DictNom, dictName) == 0
		  && strcmp (dictTable[d]->DictDirectory, dictDirectory) == 0);
	d++;
     }
   if (found)
      *pDictionary = dictTable[d - 1];
   else
      *pDictionary = NULL;
}				/*SearchDictName */


/*----------------------------------------------------------------------
   TestDictionary verifies if the file which name is dictName exists 
   returns -1 if the file is not found (inaccessible)                
   returns  0 if the file .DIC exists (not treated yet)              
   returns  1 if the file .DIC exists (treated)                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          TestDictionary (char *dictName, char *dictDirectory)
#else  /* __STDC__ */
static int          TestDictionary (dictName, dictDirectory)
char               *dictName;
char               *dictDirectory;

#endif /* __STDC__ */
{
   int                 ret, i;
   char                tempbuffer[MAX_CHAR];

   FindCompleteName (dictName, "dic", dictDirectory, tempbuffer, &i);
   if (ThotFile_exist (tempbuffer) == 0)	/* Unknown file */
     {
	/* Looks for not pre-treated dictionary */
	FindCompleteName (dictName, "DIC", dictDirectory, tempbuffer, &i);
	if (ThotFile_exist (tempbuffer) == 0)
	  {			/* File .DIC unknown */
	     /* Looks for a dictionary LEX not pre-treated */
	     FindCompleteName (dictName, "LEX", dictDirectory, tempbuffer, &i);
	     if (ThotFile_exist (tempbuffer) == 0)	/* unknown file */
		ret = -1;	/* unknown file */
	     else
		ret = 2;	/* File .LEX exists */
	  }
	else
	   ret = 0;		/* File .DIC exists */
     }
   else
      ret = 1;			/* file .dic */
   return (ret);
}				/*TestDictionary */


/*----------------------------------------------------------------------
   Create                                                                  
   Creates a dictionary from a pre-treated file.                           
   PROCEDURE PORTABLE ON VAX                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          Create (FILE * dictFile, PtrDict dict)
#else  /* __STDC__ */
static int          Create (dictFile, dict)
FILE               *dictFile;
PtrDict             dict;

#endif /* __STDC__ */
{
   int                 i;

   /* Loading ... */
   i = 1;
   while (i <= dict->nbcars)
     {
	BIOreadByte (dictFile, &(dict->chaine[i - 1]));
	i++;
     }

   i = 1;
   while (i <= dict->nbmots)
     {
	BIOreadByte (dictFile, &(dict->commun[i - 1]));
	i++;
     }

   for (i = 0; i < dict->nbmots; i++)
      BIOreadInteger (dictFile, &dict->pdico[i]);

   for (i = 0; i < MAX_WORD_LEN; i++)
      BIOreadInteger (dictFile, &dict->plgdico[i]);

   /* Loaded */
   dict->DictCharge = TRUE;
   return (1);			/* OK */
}				/* end of Create */


/*----------------------------------------------------------------------
   Load                                                               
   Returns 1 if a dictionary is loaded else returs 0                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          Load (FILE * dictFile, PtrDict dict)
#else  /* __STDC__ */
static int          Load (dictFile, dict)
FILE               *dictFile;
PtrDict             dict;

#endif /* __STDC__ */
{
   char                wordGotten[MAX_WORD_LEN];
   char                lineGotten[MAXLIGNE];
   char               *plineGotten;
   int                 i, k, length, nbGotten, last_word;
   int                 MaxWord, maxWord;
   int                 currentLength = 0;
   int                 nbChar = 0;	/* Number of characters in the dictionary  */

   /* Dictionary being loaded ... */
   MaxWord = dict->MAXcars;
   maxWord = dict->MAXmots;
   plineGotten = &lineGotten[0];	/* pointer on the first character read */

   /* Loading the dictionary */
   while (fgets (plineGotten, MAXLIGNE, dictFile) != NULL)
     {
	nbGotten = sscanf (plineGotten, "%s", wordGotten);
	if ((nbGotten > 0)
	    && (dict->nbmots < maxWord - 1)
	    && ((length = strlen (wordGotten)) < MAX_WORD_LEN)
	    && (length + nbChar + 1 < MaxWord - 1))
	  {
	     dict->nbmots++;
	     plineGotten = plineGotten + length;
	     dict->pdico[dict->nbmots] = nbChar;
	     if (length != currentLength)
	       {
		  for (k = currentLength + 1; k <= length; k++)
		     dict->plgdico[k] = dict->nbmots;
		  currentLength = length;
	       }
	     for (k = 0; k < length; k++)
		dict->chaine[nbChar++] = (char) Code[(unsigned char) wordGotten[k]];
	     dict->chaine[nbChar++] = '\0';	/* End of a word */

	     /* going to the next word: reading the next line */
	     plineGotten = &lineGotten[0];	/* pointer on the first character read */
	  }
	else if (nbGotten != -1)	/* not the end of the dictionary */
	      {
		/* impossible to load the dictionary */
		TtaDisplayMessage (INFO, TtaGetMessage(LIB, TMSG_ERR_LOADING_DICO),
				   dict->DictNom);
		/* Release the dictionary */
		ReleaseDictionary (&dict);	/* => dict = nil */
		return (0);
	      }
     }

   /* Adding an empty word at the end of the dictionary */
   last_word = dict->nbmots + 1;
   dict->pdico[last_word] = nbChar;
   /* Updating the pointers */
   for (i = currentLength + 1; i < MAX_WORD_LEN; i++)
      dict->plgdico[i] = last_word;
   dict->chaine[nbChar] = '\0';
   dict->nbcars = nbChar;
   dict->DictCharge = TRUE;
   return (1);
}				/*Load */


/*----------------------------------------------------------------------
   PrepareDictionary : cherche a charger le dictionnaire dictName         
   de langue lang et de type tdico                                   
   Traite le dico si toTreat = TRUE.                                
   retourne dans pDictionary le pointeur sur son descripteur ou NULL        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PrepareDictionary (PtrDict * pDictionary, char *dictName, PtrDocument document, char *dictDirectory, Language lang, boolean readonly, boolean treated, boolean toTreat)
#else  /* __STDC__ */
static void         PrepareDictionary (pDictionary, dictName, document, dictDirectory, lang, readonly, treated, toTreat)
PtrDict            *pDictionary;
char               *dictName;
PtrDocument         document;
char               *dictDirectory;
Language            lang;
boolean             readonly;
boolean             treated;
boolean             toTreat;

#endif /* __STDC__ */
{
   char                tempbuffer[MAX_CHAR];
   boolean             new = FALSE;
   boolean             ret;
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
	   FindCompleteName (dictName, "DIC", dictDirectory, tempbuffer, &i);
	else
	   FindCompleteName (dictName, "LEX", dictDirectory, tempbuffer, &i);
     }
   if (readonly == FALSE)
     {				/* Alterable dictionary */
	if (ThotFile_exist (tempbuffer) != 0)
	  {
	     dictFile = fopen (tempbuffer, "rw");	/* updating the dictionary */
	     TtaDisplayMessage (INFO, TtaGetMessage(LIB, TMSG_DICO), dictName);
	  }
	else
	  {
	     new = TRUE;
	     dictFile = fopen (tempbuffer, "w+");	/* new dictionary*/
	     TtaDisplayMessage (INFO, TtaGetMessage(LIB, TMSG_NEW_DICO), dictName);
	  }
     }
   else
     {
	/* READONLY dictionary (generally pre-treated) */
	if (treated == TRUE)
	   dictFile = BIOreadOpen (tempbuffer);
	else
	   dictFile = fopen (tempbuffer, "r");
	TtaDisplayMessage (INFO, TtaGetMessage(LIB, TMSG_DICO), dictName);
     }

   if (dictFile == NULL)
      return;			/* Inacessible dictionary: *pDictionary = NULL */

   /* Loading the dictionary */
   /* creation of the structures of the dictionary */
   /* if no memory, a dictionary related to another document will be flushed */
   CreateDictionary (pDictionary, document);
   if (*pDictionary == NULL)
     {
	fclose (dictFile);
	return;			/* no memory */
     }

   pdict = *pDictionary;
   pdict->DictDoc = document;
   pdict->DictLangue = lang;
   strcpy (pdict->DictDirectory, dictDirectory);
   strcpy (pdict->DictNom, dictName);
   pdict->DictReadOnly = readonly;

   /* calculation of the memory size needed by the dictionary */
   if (new == FALSE)
     {
	if (treated)		/* dictionary already treated */
	  {			
	     ret = BIOreadInteger (dictFile, &i);
	     if (ret == FALSE)
	       {
		  BIOreadClose (dictFile);
		  return;	/* no memory */
	       }
	     pdict->MAXmots = i;
	     pdict->nbmots = i;
	     BIOreadInteger (dictFile, &i);
	     pdict->MAXcars = i;
	     pdict->nbcars = i;
	  }
	else
	  {
	     /* Get the length of strings required at the begenning of the file. */
	     tempbuffer[0] = '\0';
	     fgets (tempbuffer, 100, dictFile);
	     if (tempbuffer[0] != '\0')
	       {
		  if (sscanf (tempbuffer, "%d%d", &im, &ic) == 2)
		    {
		       pdict->MAXmots = im;
		       pdict->MAXcars = ic;
		    }
		  else
		    {
		       /* The head of the file does not contain the words number ... */
		       /* -> one go to the begenning of the file */
		       /* fseek(dictFile, 0L, 0); */
		       /* impossible to load this dictionary */
		       TtaDisplayMessage (INFO, TtaGetMessage(LIB, TMSG_ERR_LOADING_DICO), dictName);
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
		       return;	/* Error while reading, the FILE dictionary is empty */
		    }

	       }		/* end of else (scanf) */
	  }			/* end of else (treated) */
     }				/* end of if (new == FALSE) */
   /* Provide space for 50 words and 600 characters if not readonly */
   if (GetStringInDict (pDictionary, readonly) == -1)
     {
	/* Not enough memory to open this dictionary */
	TtaDisplayMessage (INFO, TtaGetMessage(LIB, TMSG_LIB_NO_LOAD), dictName);
	/* Release the allocated dictionary */
	ReleaseDictionary (pDictionary);
	*pDictionary = NULL;
	fclose (dictFile);
	return;
     }				/* end of if GetStringInDict == -1 */

   if (treated == TRUE)
     {				/* Read the pre-treated file */
	if (Create (dictFile, pdict) != 1)
	   *pDictionary = NULL;	/* Problem while reading the pre-treated file */
     }
   else
     {				/* Reading a not treated file */
	if (Load (dictFile, pdict) == 1)
	  {
	     if (toTreat)
		/* pre-treatement of the dictionary */
		Corr_pretraitement (pdict);
	  }
	else
	   *pDictionary = NULL;	/* The loading of the dictionary failed */
     }				/* end of if (treated == TRUE) */
   fclose (dictFile);
}				/*PrepareDictionary */


/*----------------------------------------------------------------------
   LoadDict                                                         
   Returns -1 if the dictionary can't be loaded.                     
   0 if the dictionary was already loaded.                  
   1 if the dictionary is loaded.                           
   returns in pdico : a pointer to dictionary                        
   cree le dictionnaire si toCreate = TRUE                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static int          LoadDict (PtrDict * pDictionary, Language lang, PtrDocument document, char *dictName, char *dictDirectory, boolean readonly, boolean toCreate)

#else  /* __STDC__ */
static int          LoadDict (pDictionary, lang, document, dictName, dictDirectory, readonly, toCreate)
PtrDict            *pDictionary;
Language            lang;
PtrDocument         document;
char               *dictName;
char               *dictDirectory;
boolean             readonly;
boolean             toCreate;

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
		    /* else: crate the new dictionary not readonly */
		 case (0):
		    /* file .DIC */
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
	      }			/* end of switch */

     }				/* end of if (pdict == NULL) */
   else
      /* The dictionary was already loaded by anther document */
      /* just update the dictionary context */
      pdict->DictDoc = document;

   *pDictionary = pdict;
   return ret;
}				/*LoadDict */


/*----------------------------------------------------------------------
   Corr_ReloadDict: reload a dictionary                            
   returns TRUE if the FILE dictionary is found and well loaded       
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             Corr_ReloadDict (PtrDict * pDictionary)

#else  /* __STDC__ */
boolean             Corr_ReloadDict (pDictionary)
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
	     strcpy (dictName, pdict->DictNom);
	     document = pdict->DictDoc;
	     /* Release the string and the list of words ... */
	     FreeStringInDict (pdict);
	     FreeDictionary (pdict);
	     dictTable[d] = NULL;
	     pdict = NULL;
	  }
     }

   d = LoadDict (pDictionary, '\0', document, dictName,
		 document->DocDirectory, FALSE, TRUE);
   if (d == -1)
      return (FALSE);

   return (TRUE);
}				/*Corr_ReloadDict */


/*----------------------------------------------------------------------
   Dict_Init                                                       
  ----------------------------------------------------------------------*/
void                Dict_Init ()
{
   int                 i;

   /* Inititializing pointers of the dictionary */
   for (i = 0; i < MaxDictionaries; i++)
      dictTable[i] = NULL;

   /* Inititializing of environments needed by dictionarires*/
   dictPath = TtaGetEnvString ("DICOPAR");
   if (dictPath == NULL)
     {
	/* The environment variable DICOPAR does not exist */
	TtaDisplayMessage (INFO, TtaGetMessage(LIB, TMSG_MISSING_DICOPAR), "DICOPAR");
     }
   alphabetLoaded = Corr_alphabet ();
   if (alphabetLoaded == FALSE)
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_MISSING_ALPHABET);
}				/* end proc Dict_Init */


/*----------------------------------------------------------------------
   TtaLoadDocumentDictionary loads the document dictionary.          
   Returns -1 if the dictionary can't be loaded.                     
   Returns 1 if the dictionary is loaded.                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             TtaLoadDocumentDictionary (PtrDocument document, int *pDictionary, boolean ToCreate)
#else  /* __STDC__ */
boolean             TtaLoadDocumentDictionary (document, pDictionary, ToCreate)
PtrDocument         document;
int                *pDictionary;
boolean             ToCreate;

#endif /* __STDC__ */
{
   /* int        res; */
   char               *dicodoc;

   /* dictionary name = document name */
   *pDictionary = (int) NULL;

   dicodoc = (char *) TtaGetEnvString ("DICODOC");
   if (dicodoc != NULL)
      (void) LoadDict ((PtrDict *) pDictionary, 0, document, dicodoc,
                        document->DocDirectory, FALSE, ToCreate);
   else
      (void) LoadDict ((PtrDict *) pDictionary, 0, document, document->DocDName, document->DocDirectory, FALSE, ToCreate);
   return (*pDictionary != '\0');
}				/*TtaLoadDocumentDictionary */


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
boolean             TtaLoadLanguageDictionaries (Language languageId)

#else  /* __STDC__ */
boolean             TtaLoadLanguageDictionaries (languageId)
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
   if (LangTable[lang].LangDict[0] == NULL)
     {
	/* Loading the main dictionary */
	if (LangTable[lang].LangPrincipal[0] != '\0')
	  {
	     ret = LoadDict (&dictPtr, lang, NULL, LangTable[lang].LangPrincipal, dictPath, TRUE, FALSE);
	     if (ret > 0)
		LangTable[lang].LangDict[0] = (Dictionary) dictPtr;
	  }
     }

   /* Verifies if the secondary dictionary is already laded */
   if (LangTable[lang].LangDict[1] == NULL)
     {
	/* Loading the secondary dictionary */
	if (LangTable[lang].LangSecondary[0] != '\0')
	  {
	     ret = LoadDict (&dictPtr, lang, NULL, LangTable[lang].LangSecondary, dictPath, TRUE, FALSE);
	     if (ret > 0)
		LangTable[lang].LangDict[1] = (Dictionary) dictPtr;
	  }
     }
   return (LangTable[lang].LangDict[0] != NULL || LangTable[lang].LangDict[1] != NULL);
}				/*TtaLoadLanguageDictionaries */


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
boolean             TtaLoadTypoDictionaries (Language languageId)

#else  /* __STDC__ */
boolean             TtaLoadTypoDictionaries (languageId)
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
	if (TypoLangTable[lang].LangPrincipal[0] != '\0')
	  {
	     ret = LoadDict (&dictPtr, lang, NULL,
		  TypoLangTable[lang].LangPrincipal, dictPath, TRUE, FALSE);
	     if (ret > 0)
		TypoLangTable[lang].LangDict[0] = (Dictionary) dictPtr;
	  }
     }

   /* Verifies if the secondary dictionary is already laded */
   if (TypoLangTable[lang].LangDict[1] == NULL)
     {
	/* Loading the secondary dictionary */
	if (TypoLangTable[lang].LangSecondary[0] != '\0')
	  {
	     ret = LoadDict (&dictPtr, lang, NULL,
		  TypoLangTable[lang].LangSecondary, dictPath, TRUE, FALSE);
	     if (ret > 0)
		TypoLangTable[lang].LangDict[1] = (Dictionary) dictPtr;
	  }
     }
   return (TypoLangTable[lang].LangDict[0] != NULL
	   || TypoLangTable[lang].LangDict[1] != NULL);
}				/*TtaLoadTypoDictionaries */


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
}				/*TtaUnLoadDictionaries */


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
}				/*TtaUnLoadTypoDictionaries */


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
}				/*TtaGetPrincipalDictionary */


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
}				/*TtaGetPrincipalTypoDictionary */


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
}				/*TtaGetSecondaryDictionary */


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
