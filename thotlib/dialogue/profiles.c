/*
 *
 *  COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * profile.c: This module contains all the functions used to handle
 * the profile configuration in Amaya. Each function beginning with
 * "Prof_" prefix is called by an extern module.
 *
 * Authors: L. Bonameau - Amaya profiles
 *
 */

/* Included headerfiles */
#include "thot_sys.h"
#include "ustring.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "application.h"
#include "registry.h"

/* Definitions */
#define THOT_EXPORT extern
#define MAX_FUNCTIONS    1000    /* Maximum of functions defined in Amaya software */       
#define MAX_DEF          1000      /* Maximun of lines for the module definition file */
#define MAX_PRO          6         /* Maximun of profile displayed in the interface */
#define MAX_LENGTH       50       /* Maximum length of a string */
#define DEF_FILE         "/config/ProfileDefs"


/*-----------------------
  Functions prototypes
-----------------------*/
static void ProcessElements(char string[]);
static void InsertTable(STRING string, STRING Table[], int * nbelem);
static void FileToTable(FILE * File, STRING Table[],int * nbelem, int maxelem);
static void SortTable (STRING Table[], int nbelem);
static void DeleteTable(STRING Table[], int  *nbelem);
static int  SearchInTable(char * string, int elem_type, STRING Table[],
			  int nbelem, ThotBool sort);
static void Remove_module_sign(char * string);
static void SkipNewLineSymbol(char  string[]);
static void SkipAllBlanks (char string[]);
static char *  HookIt (char * string);
static void RemoveHooks (char  string[]);
static int  type(char * string);


/*--------------------------
  Local variables
---------------------------*/
/* determine either a profile is defined or not */
static ThotBool             defined_profile = FALSE;

/* Definition table : correspondence between Modules and functions */
static  STRING              Def_Table[MAX_DEF];
static  int                 Def_nbelem = 0;
/* Functions table */
static STRING               Fun_Table[MAX_FUNCTIONS];
static int                  Fun_nbelem = 0;

/* Profiles table : contains all the available profiles*/
static STRING               Pro_Table[MAX_PRO];
static int                  Pro_nbelem = 0;

/* Current profile : the current selected profile*/
static char                 CurrentProfile[MAX_LENGTH];

/* User Profile */  
static char                 UserProfile[MAX_LENGTH];
static char                 TempString[MAX_LENGTH];


/*----------------------------------------------------------------------
   Prof_WhichProfiles
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int  Prof_WhichProfiles(CHAR_T string[])
#else  /* !__STDC__ */
int  Prof_WhichProfiles(string[])
CHAR_T string[];
#endif /* !__STDC__ */
/* Get the text for the profile menu items. Returns the number of items */
{
  int                   i = 0;
  int                   j;

  for (j=0 ; j < Pro_nbelem ; j++)
    {
       usprintf (&string[i], TEXT("%s%s"), "B", Pro_Table[j]);
       i += ustrlen (&string[i]) + 1;
    }
  return j;
}


/*-----------------------------------------------------------------------
  Prof_ItemNumber2Profile : Conversion between item number in the profile
  menu and the profile name
  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING Prof_ItemNumber2Profile(int val)
#else  /* !__STDC__ */
STRING Prof_ItemNumber2Profile(val)
int val;
#endif /* !__STDC__ */
{
  return Pro_Table[val];
}


/*-----------------------------------------------------------------------
  Prof_Profile2ItemNumber : Conversion between profile name and item 
  number in the menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int Prof_Profile2ItemNumber(STRING Profile)
#else  /* !__STDC__ */
int Prof_Profile2ItemNumber(Profile)
STRING Profile;
#endif /* !__STDC__ */
{
  if (Profile)
    {
      return (SearchInTable(Profile, 0, Pro_Table, Pro_nbelem, FALSE));
    }
  else 
    return -1;
}

/*-----------------------------------------------------------------------
   Prof_InitTable: Seek the current profile and init the function table
  ----------------------------------------------------------------------*/
void Prof_InitTable()
{
  FILE *              Def_FILE;
  FILE *              Prof_FILE; 
  char *              Prof_File;
  char                string   [MAX_LENGTH];
  char                Def_File [MAX_LENGTH];
  char                thotdir  [MAX_LENGTH];
  
  Prof_File = TtaGetEnvString("Profiles_File");
  if (TtaGetEnvString ("Profile"))
    strcpy (UserProfile, HookIt ( TtaGetEnvString ("Profile") ) );
  
  strcpy (thotdir,  TtaGetEnvString("THOTDIR"));
  strcpy (Def_File, strcat(thotdir, DEF_FILE));


  Def_FILE = fopen(Def_File,"r");
  Prof_FILE = fopen(Prof_File,"r");

  if ((Def_FILE != NULL) && (Prof_FILE != NULL) && UserProfile)
     {    
       
       /* Fill a table for modules definition */
       FileToTable (Def_FILE, Def_Table, &Def_nbelem, MAX_DEF);

       /* Generate a functions table*/
       while (fgets(string, sizeof(string), Prof_FILE))
	 {
	   SkipAllBlanks(string);
  	   ProcessElements(string);
	 } 
       
       /* Sort the functions table */
       SortTable(Fun_Table, Fun_nbelem);

       /* delete the modules definition table */
       DeleteTable (Def_Table, &Def_nbelem);

     }       
  else
    {
      /* can NOT open profile files or no profile defined */
    }
  if (Fun_nbelem > 0)
    defined_profile = TRUE;

  /* Close the open files */
  if (Def_FILE != NULL) 
      fclose(Def_FILE);

  if (Prof_FILE != NULL)
      fclose(Prof_FILE);
}


/*----------------------------------------------------------------------
   Prof_BelongTable : Check if a function belongs to the profile functions
   table.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool Prof_BelongTable(STRING functionName)
#else  /* __STDC__ */
ThotBool Prof_BelongTable(functionName)
STRING functionName;
#endif /* __STDC__ */
{

  if (!defined_profile)
    return TRUE;

  /* seek for functionName in the table */
  return (SearchInTable (functionName, 0, Fun_Table, Fun_nbelem, TRUE) != -1);
}


/*----------------------------------------------------------------------
   Prof_DeleteFunTable : Deletes the function table (free allocated memory)
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void Prof_DeleteFunTable()
#else  /* __STDC__ */
void Prof_DeleteFunTable()
#endif /* __STDC__ */
{
  DeleteTable(Fun_Table, & Fun_nbelem);
}


/*-----------------------------------------------------------------------
    Prof_AddButton : Add a button if the function associated to that button
    belongs to the user profile
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool Prof_AddButton(STRING FunctionName)
#else  /* __STDC__ */
ThotBool Prof_AddButton(FunctionName)
STRING   FunctionName;
#endif /* __STDC__ */
{
  return (Prof_BelongTable(FunctionName));
}


/*----------------------------------------------------------------------
    Prof_RemoveSeparators : Remove bad separators in the menu interface
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool Prof_RemoveSeparators(Menu_Ctl *ptrmenu, int item, char LastItemType)
#else  /* __STDC__ */
ThotBool Prof_RemoveSeparators(*ptrmenu, item)
Menu_Ctl *ptrmenu;
int       item;
#endif /* __STDC__ */
{  
  return (( LastItemType ==  'S') || (item == 0) );
}

/*----------------------------------------------------------------------
    Prof_RemoveSubMenu : Check if a submenu has to be removed
-----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool   Prof_RemoveSubMenu(Menu_Ctl *ptrsubmenu)
#else  /* __STDC__ */
ThotBool   RProf_RemoveSubMenu(ptrsubmenu)
Menu_Ctl  *ptrsubmenu;
#endif /* __STDC__ */
{
  int      item    = 0;

  if ((ptrsubmenu->ItemsNb == 0) || (ptrsubmenu == NULL))
    return TRUE;
  while (item < ptrsubmenu->ItemsNb)
    {
      if (ptrsubmenu->ItemsList[item].ItemType != TEXT('S'))
	return FALSE;
      item ++;
    }
  return TRUE;
}

/*----------------------------------------------------------------------
    Prof_RemoveMenu : Remove a menu
-----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool   Prof_RemoveMenu(Menu_Ctl *ptrmenu)
#else  /* __STDC__ */
ThotBool   Prof_RemoveMenu(ptrmenu)
Menu_Ctl  *ptrmenu;
#endif /* __STDC__ */
{
  int      item    = 0;

  /* If profiles are not used, do not erase any menu */
  if (!defined_profile)
    return FALSE;
  /* this should not happen... */
  if (ptrmenu == NULL)
    return TRUE;
  /* check if the attr and select menu are in the profile */
  if (ptrmenu->MenuAttr)
    return (!Prof_BelongTable(TEXT("MenuAttribute")));
  if (ptrmenu->MenuSelect)
    return (!Prof_BelongTable(TEXT("MenuSelection")));    
  /* an empty menu has to be removed */
  if (ptrmenu->ItemsNb == 0)
    return TRUE;
  /* check if the menu is only composed of empty sub menus and separators */
  while (item < ptrmenu->ItemsNb)
    {
      if (ptrmenu->ItemsList[item].ItemType != TEXT('S'))
	{
	  if (ptrmenu->ItemsList[item].ItemType == TEXT('M'))
	   {
	     if (!Prof_RemoveSubMenu (ptrmenu->ItemsList[item].SubMenu)) 
	       /* there is at least a non empty sub menu */
	       return FALSE;
	   } 
	  else
	    /* there is at least a standard item */
	    return FALSE;
	}
      item ++;
    }
  return TRUE;
}

/*----------------------------------------------------------------------
  ProcessElements : Recursive function that helps
  building the profile table
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static   void ProcessElements(char string[])
#else  /* !__STDC__ */
static   void ProcessElements(string)
char     string[];
#endif /* !__STDC__ */ 
{
  ThotBool            EOM = FALSE;
  int                 i;
 
  switch (type(string))
    {
    case 1:
    /* the element is a profile start tag */
      strcpy(CurrentProfile, string);
      if (Pro_nbelem < MAX_PRO)
	{
	  RemoveHooks(string);
	  InsertTable(string, Pro_Table, &Pro_nbelem);
	}
      break;
      
    case 2:
    /* the element is a module */
      if (strcmp(CurrentProfile, UserProfile) == 0)
	{
	  Remove_module_sign(string);
	  i = SearchInTable(HookIt(string), 1, Def_Table, Def_nbelem, FALSE);
	  if (i>=0)
	    {
	      i++;
	      /* process the elements inside the module */
	      while (!EOM && (i<=Def_nbelem-1))
		{
		  ustrcpy(ISO2WideChar(string), Def_Table[i]);
		  EOM = (string[0] == '[');
		  
		  if (!EOM)
		    ProcessElements(string);
		  i++;
		}
	      EOM = FALSE;
	    }
	  else
	    {
	      /* Module not defined - Skip it */
	    }
	}
      break;
    case 0:
    /* the element is a function */
      if ((strcmp(CurrentProfile, UserProfile) == 0) && (strlen(string) > 0))
	{
	  /* Insert the element in the table of functions */
	  if (Fun_nbelem < MAX_FUNCTIONS)
	    InsertTable(string, Fun_Table, &Fun_nbelem);
	}
    }
}


/*-------------------
 * Table functions
 *-------------------*/
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void InsertTable(STRING string, STRING Table[], int * nbelem)
#else  /* !__STDC__ */
static void InsertTable(string[], Table[],nb_elem )
STRING         string;
STRING         Table;
int *          nbelem;
#endif /* !__STDC__ */
{
  Table[*nbelem] = TtaStrdup (string);
  *nbelem = *nbelem+1;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void FileToTable(FILE * File, STRING Table[],int * nbelem, int maxelem)
#else  /* !__STDC__ */
static void FileToTable(File,Table[], nbelem, maxelem)
STRING      Table;
FILE *      File;
int  *      nbelem;
int         maxelem;
#endif /* !__STDC__ */
{
  char   string[MAX_LENGTH];
  
  while (fgets (string, sizeof (string), File) && *nbelem < maxelem)
    {
      SkipAllBlanks(string);
      if (strlen(string) >=2 && !(string[0] == '#' && string[1] == '#'))
	InsertTable (ISO2WideChar(string), Table, nbelem);
    }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void DeleteTable(STRING Table[], int  *nbelem)
#else  /* __STDC__ */
static void DeleteTable(Table[], nbelem)
STRING      Table[];
int *       nbelem;
#endif /* __STDC__ */
{
  int        i;

  for ( i = 0; i < *nbelem; i++)
      TtaFreeMemory (Table[i]);
  *nbelem = 0;
}



/*----------------------------------------------------------------------
  look for a string in the table and return the number 
  of the line where the first occurence is found or -1 if not found
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int SearchInTable(char * string, int elem_type, STRING Table[],
			 int nbelem, ThotBool sort)
#else  /* !__STDC__ */
static int SearchInTable(string, elem_type, Table[], nbelem, sort) 
char * string;
int elem_type;
STRING Table[];
int nbelem;
ThotBool sort;
#endif /* !__STDC__ */
{
  int              j = 0;
  ThotBool         found = FALSE;
  int              left, right, middle;
  char             str [MAX_LENGTH];

  if (sort)
    {
      /* Dichotomic search */
      left = 0;
      right = nbelem - 1;
      
      while (left <= right && !found)
	{
	  middle = (right + left) / 2;
	  ustrcpy (ISO2WideChar(str), Table[middle]);
	  if (ustrcmp(str, string)==0) found = TRUE;
	  else if (ustrcmp(str,string) < 0)
	    left = middle + 1;
	  else right = middle - 1;
	}     
      if (found)
	return middle;
      else
	return -1;
    }
  else
    {
      /* simple sequential search */      
      for (j = 0; j < nbelem;j++)
	{
	  if (type (Table[j]) == elem_type && strcmp(Table[j],string) == 0)
	    return j;
	}
      return -1; 
    }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void SortTable (STRING Table[], int nbelem)
#else  /* !__STDC__ */
static void SortTable (Table[],nbelem)
STRING  Table[];
int nbelem;
#endif /* !__STDC__ */
{
  int i, j, k, kmax;
  char *     tempo;

  for (i=0 ; i < nbelem - 1 ; i++)
    {
      kmax = i;
      for (j = i+1; j < nbelem; j++)
        if (ustrcmp( Table[j] ,Table[kmax]) <= 0) kmax = j;
      tempo = Table[kmax];
      Table[kmax] = Table[i];
      Table[i] = tempo;
    }
}



/*--------------------------------------------
  String functions
--------------------------------------------*/
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int type(char * string)
#else  /* !__STDC__ */
static int type(string)
char * string;
#endif /* !__STDC__ */
{
  if ( string[0] == '[' )
    return 1;
  else if (string[0] == '#')
    return 2;
  else
    return 0;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void SkipNewLineSymbol(char string[])
#else  /* !__STDC__ */
static void SkipNewLineSymbol(string)
char  string[];
#endif /* !__STDC__ */
{
  int         c = 0;

  while (string[c] != EOS && string[c] != '\n')
    {c++;}
  if (string[c] == '\n')
    string[c] = EOS;    
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void SkipAllBlanks (char  string[])
#else  /* !__STDC__ */
static void SkipAllBlanks (string)
char  string[];
#endif /* !__STDC__ */
{
  int c = 0;
  int nbsp = 0;
  
  do
    { 
      while (string[c+nbsp] == SPACE || string[c+nbsp] == TAB)
	nbsp++;
      
      string [c] = string[c+nbsp];
    } 
  while (string [c++] != EOS);

  SkipNewLineSymbol(string);
 
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static char *  HookIt (char * string)
#else  /* !__STDC__ */
static char *  HookIt (string)
char * string;
#endif /* !__STDC__ */
{
  int          k = 0;

    TempString[k] = '[';
    do
      {
	TempString[k+1] = string[k];
	k++;
      }
    while (string[k] != EOS);
    
    TempString[k+1] = ']';
    TempString[k+2] = EOS;

    /*  strcpy (string, TempString); */
    return TempString;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void RemoveHooks (char string[])
#else  /* !__STDC__ */
static void RemoveHooks (string)
char string[];
#endif /* !__STDC__ */
{
  char         new[MAX_LENGTH];
  int          k = 0;

  while (string [k+1] != ']')
    {
      new[k] = string [k+1];
      k++;
    }
  new[k] = EOS;
  strcpy (string, new);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void Remove_module_sign(char * string)
#else  /* !__STDC__ */
static void Remove_module_sign(string)
char * string;
#endif /* !__STDC__ */
{
  int i=0;

  while (string[i+1] != EOS)
    {
      string[i] = string[i+1];
      i++;
    }
  string[i]=EOS;
}
