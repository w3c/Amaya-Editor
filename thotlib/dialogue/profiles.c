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
#define PROFILE_START    '['
#define MODULE_START     '['
#define PROFILE_END    ']'
#define MODULE_END     ']'
#define MODULE_REF     '#'

/*-----------------------
  Functions prototypes
-----------------------*/
static void ProcessElements(char element[]);
static void InsertTable(STRING string, STRING Table[], int * nbelem);
static void FileToTable(FILE * File, STRING Table[],int * nbelem, int maxelem);
static void SortTable (STRING Table[], int nbelem);
static void DeleteTable(STRING Table[], int  *nbelem);
static int  SearchInTable(char * StringToFind, STRING Table[],
			  int nbelem, ThotBool sort);

static void SkipNewLineSymbol(char  Astring[]);
static void SkipAllBlanks (char Astring[]);
static char * AddHooks (char * Astring);
static void RemoveHooks (char  Astring[]);



/*--------------------------
  Static variables
---------------------------*/

/* determine either a profile is defined or not */
static ThotBool             defined_profile = FALSE;

/* Definition table : correspondence between Modules and functions */
static  STRING              Def_Table[MAX_DEF];
static  int                 Def_nbelem = 0;

/* Functions table : Table of functions defined in the user profile */
static STRING               Fun_Table[MAX_FUNCTIONS];
static int                  Fun_nbelem = 0;

/* Profiles table : contains the name of all the available profiles*/
static STRING               Pro_Table[MAX_PRO];
static int                  Pro_nbelem = 0;

/* Current profile : the current selected profile*/
static char                 CurrentProfile[MAX_LENGTH];

/* User Profile */  
static char                 UserProfile[MAX_LENGTH];

static char                 TempString[MAX_LENGTH];


/*--------------------------------------------------------
   Prof_RebuildProTable : Rebuild the Profiles Table
   Returns the number of elements if operation succeded or
   a 0 if operation failed.
----------------------------------------------------------*/

#ifdef __STDC__
int  Prof_RebuildProTable(STRING Prof_file)
#else  /* !__STDC__ */
int  Prof_RebuildProTable(STRING Prof_file)
#endif /* !__STDC__ */
{
  FILE *              Prof_FILE; 
  char *              Prof_File;

  
  Prof_FILE = fopen(Prof_file,"r");
  DeleteTable(Pro_Table, &Pro_nbelem);
   if (Prof_FILE != NULL)
     {
       while (fgets(TempString, sizeof(TempString), Prof_FILE))
	 {
	   SkipAllBlanks(TempString);
  	   if (TempString[0] == PROFILE_START)
	     {
	       if (Pro_nbelem < MAX_PRO)
		 {
		   RemoveHooks(TempString);
		   InsertTable(TempString, Pro_Table, & Pro_nbelem);
		 }
	     }
	 } 
       fclose(Prof_FILE);
       return (Pro_nbelem > 0);
     }
   else
     {
       /* file not found, table is empty */
       return 0;
     }
}


/*---------------------------------------------------------------
   Prof_GetProfilesItems :  Get the text for the profile menu items.
   Returns the number of items
------------------------------------------------------------------*/

#ifdef __STDC__
int  Prof_GetProfilesItems(CHAR_T MenuText[])
#else  /* !__STDC__ */
int  Prof_GetProfilesItems(MenuText[])
CHAR_T MenuText[];
#endif /* !__STDC__ */

{
  int                   i = 0;
  int                   j;

  for (j=0 ; j < Pro_nbelem ; j++)
    {
       usprintf (&MenuText[i], TEXT("%s%s"), "B", Pro_Table[j]);
       i += ustrlen (&MenuText[i]) + 1;
    }
  return j;
}



/*-----------------------------------------------------------------------------
  Prof_ItemNumber2Profile : Conversion between item number in the profile menu
  and the profile name
------------------------------------------------------------------------------*/

#ifdef __STDC__
STRING Prof_ItemNumber2Profile(int ItemNumber)
#else  /* !__STDC__ */
STRING Prof_ItemNumber2Profile(ItemNumber)
int ItemNumber;
#endif /* !__STDC__ */
{
//  if (Pro_nbelem)
    return Pro_Table[ItemNumber];
//  else
 //   return NULL;
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
     
      return (SearchInTable(Profile, Pro_Table, Pro_nbelem, FALSE));
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
  char                Def_File [MAX_LENGTH];
  char                thotdir  [MAX_LENGTH];
  char                TempString [MAX_LENGTH];

  Prof_File = TtaGetEnvString("Profiles_File");
  if (TtaGetEnvString ("Profile"))
    strcpy (UserProfile, AddHooks ( TtaGetEnvString ("Profile") ) );
  
  strcpy (thotdir,  TtaGetEnvString("THOTDIR"));
  strcpy (Def_File, strcat(thotdir, DEF_FILE));


  Def_FILE = fopen(Def_File,"r");
  Prof_FILE = fopen(Prof_File,"r");

  if ((Def_FILE != NULL) && (Prof_FILE != NULL) && UserProfile)
     {    
       
       /* Fill a table for modules definition */
       FileToTable (Def_FILE, Def_Table, &Def_nbelem, MAX_DEF);

       /* Generate a functions table*/
       while (fgets(TempString, sizeof(TempString), Prof_FILE))
         {		
	   SkipAllBlanks(TempString);
  	   ProcessElements(TempString);
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
ThotBool Prof_BelongTable(STRING FunctionName)
#else  /* __STDC__ */
ThotBool Prof_BelongTable(FunctionName)
STRING functionName;
#endif /* __STDC__ */
{

  if (!defined_profile)
    return TRUE;

  /* seek for functionName in the table */
 
  return (SearchInTable (FunctionName, Fun_Table, Fun_nbelem, TRUE) != -1);
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
static   void ProcessElements(char element[])
#else  /* !__STDC__ */
static   void ProcessElements(element)
char     element[];
#endif /* !__STDC__ */ 
{
  ThotBool            EOM = FALSE;
  int                 i;
  int                 j = 0;
 
  
  if ( element[0] == PROFILE_START )
    {
     /* Case 1 : the element is a profile start tag. Insert it in the Profile table */
      strcpy(CurrentProfile, element);
      if (Pro_nbelem < MAX_PRO)
	{
	  RemoveHooks(element);
	  InsertTable(element, Pro_Table, &Pro_nbelem);
	}
    }
  else if (element[0] == MODULE_REF)
    {
      /* Case 2 : the element is a module. Process the element inside the module
	 with a recursive call */
      if (strcmp(CurrentProfile, UserProfile) == 0)
	{

	  /* Remove the module start tag */
	  while (element[j+1] != EOS)
	    {
	      element[j] = element[j+1];
	      j++;
	    }
	  element[j]=EOS;
	  
	 
	  i = SearchInTable(AddHooks(element), Def_Table, Def_nbelem, FALSE);
	  if (i>=0)
	    {
	      i++;
	      /* process the elements inside the module */
	      while (!EOM && (i<=Def_nbelem-1))
		{
		  ustrcpy(ISO2WideChar(element), Def_Table[i]);
		  EOM = (element[0] == MODULE_START);
		  
		  if (!EOM)
		    ProcessElements(element);
		  i++;
		}
	      EOM = FALSE;
	    }
	  else
	    {
	      /* Module not defined - Skip it */
	    }
	}
    }
  else
    {
      /* Last case : the element is a function. Insert it in the functions table */
      if ((strcmp(CurrentProfile, UserProfile) == 0) && (strlen(element) > 0))
	{
	  /* Insert the element in the table of functions */
	  if (Fun_nbelem < MAX_FUNCTIONS)
	    InsertTable(element, Fun_Table, &Fun_nbelem);
	}
    }
}


/*------------------------------------------------------------------
  Table functions : set of functions to handle table manipulations :
  insert, delete, search, sort
---------------------------------------------------------------------*/


#ifdef __STDC__
static void InsertTable(STRING function, STRING Table[], int * nbelem)
#else  /* !__STDC__ */
static void InsertTable(function, Table[],nbelem )
STRING         function;
STRING         Table;
int *          nbelem;
#endif /* !__STDC__ */
{
  Table[*nbelem] = TtaStrdup (function);
  *nbelem = *nbelem+1;
}


/*----------------------------------------------------------------------
  FileToTable : Copy a text file in a table. The lines beginning with "##"
  are considered as comments and are not included
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void FileToTable(FILE * File, STRING Table[],int * nbelem, int maxelem)
#else  /* !__STDC__ */
static void FileToTable(File, Table[], nbelem, maxelem)
STRING      Table;
FILE *      File;
int  *      nbelem;
int         maxelem;
#endif /* !__STDC__ */
{
  
  while (fgets (TempString, sizeof (TempString), File) && *nbelem < maxelem)
    {
      SkipAllBlanks(TempString);
      if (strlen(TempString) >=2 && !(TempString[0] == MODULE_REF && TempString[1] == MODULE_REF ))
	InsertTable (ISO2WideChar(TempString), Table, nbelem);
    }
}



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
  Search a string in the table and return the number 
  of the line of the first occurence or -1 if not found

  int SearchInTable(char * StringToFind, ** Researched string     
		    STRING Table[],      ** Table in which the search is achieved
		    int nbelem,          ** Number of elements of the table
		    ThotBool sort)       ** Is the table presorted ?
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int SearchInTable(char * StringToFind, STRING Table[],
			 int nbelem, ThotBool sort)
#else  /* !__STDC__ */
static int SearchInTable(stringToFind, Table[], nbelem, sort) 
char * StringToFind;
STRING Table[];
int nbelem;
ThotBool sort;
#endif /* !__STDC__ */
{
  int              j = 0;
  ThotBool         found = FALSE;
  int              left, right, middle;
  char             temp [MAX_LENGTH];

  if (sort)
    {
      /* Dichotomic search */
      left = 0;
      right = nbelem - 1;
      
      while (left <= right && !found)
	{
	  middle = (right + left) / 2;
	  ustrcpy (ISO2WideChar(temp), Table[middle]);
	  if (ustrcmp(temp, StringToFind)==0)
	    found = TRUE;
	  else if (ustrcmp(temp, StringToFind) < 0)
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
	  if (strcmp(Table[j], StringToFind) == 0)
	    return j;
	}
      return -1; 
    }
}



/*--------------------------------------------------
  SortTable : Sort a table of strings
----------------------------------------------------*/

#ifdef __STDC__
static void SortTable (STRING Table[], int nbelem)
#else  /* !__STDC__ */
static void SortTable (Table[], nbelem)
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



/*----------------------------------------------------------
  String functions : set of useful functions for strings
----------------------------------------------------------*/

#ifdef __STDC__
static void SkipNewLineSymbol(char Astring[])
#else  /* !__STDC__ */
static void SkipNewLineSymbol(Astring)
char  Astring[];
#endif /* !__STDC__ */
{
  int         c = 0;

  while (Astring[c] != EOS && Astring[c] != EOL)
    {c++;}
  if (Astring[c] == EOL)
    Astring[c] = EOS;    
}



#ifdef __STDC__
static void SkipAllBlanks (char  Astring[])
#else  /* !__STDC__ */
static void SkipAllBlanks (Astring)
char  string[];
#endif /* !__STDC__ */
{
  int c = 0;
  int nbsp = 0;
  
  do
    { 
      while (Astring[c+nbsp] == SPACE || Astring[c+nbsp] == TAB)
	nbsp++;
      
      Astring [c] = Astring[c+nbsp];
    } 
  while (Astring [c++] != EOS);

  SkipNewLineSymbol(Astring);
 
}


#ifdef __STDC__
static char *  AddHooks (char * Astring)
#else  /* !__STDC__ */
static char *  AddHooks (Astring)
char * Astring;
#endif /* !__STDC__ */
{
  int          k = 0;
  
  TempString[k] = MODULE_START;
  
  do
    {
      TempString[k+1] = Astring[k];
      k++;
    }
  while (Astring[k] != EOS);
  
  TempString[k+1] = MODULE_END;
  TempString[k+2] = EOS;
  
  /*  strcpy (string, TempString); */
  return TempString;
}



#ifdef __STDC__
static void RemoveHooks (char string[])
#else  /* !__STDC__ */
static void RemoveHooks (string)
char string[];
#endif /* !__STDC__ */
{
  char         new[MAX_LENGTH];
  int          k = 0;

  while (string [k+1] != MODULE_END)
    {
      new[k] = string [k+1];
      k++;
    }
  new[k] = EOS;
  strcpy (string, new);
}

