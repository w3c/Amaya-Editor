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
#include "profiles.h"


/*---------------------------
  Static Functions prototypes
---------------------------*/


static void ProcessElements(char * element);
static void InsertTable(STRING string, STRING * Table, int * nbelem);
static void FileToTable(FILE * File,  STRING * Table,
			int * nbelem, int maxelem);
static void SortTable (STRING * Table, int nbelem);
static void DeleteTable(STRING * Table, int  *nbelem);
static int  SearchInTable(char * StringToFind, STRING * Table, 
			  int nbelem, ThotBool sort);
static void SkipNewLineSymbol(char  * Astring);
static void SkipAllBlanks (char * Astring);
static char * AddHooks (char * Astring);
static void RemoveHooks (char  * Astring);


/*--------------------------
  Static vars
---------------------------*/

/* Determine either a profile is defined or not */
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

/* Edition table : contains the name of all the functions used for edition */
static STRING               Edition_Table[MAX_EDITION_FUNCTIONS];
static int                  Edition_nbelem = 0;

/* Current profile : the current selected profile */
static char                 CurrentProfile[MAX_PRO_LENGTH];
                
/* User Profile (taken from the thot.rc) */  
static char                 UserProfile[MAX_PRO_LENGTH];

static char                 TempString[MAX_PRO_LENGTH];

/* This boolean goes FALSE if the profile only contains browsing functions */
static ThotBool             Prof_ReadOnly = TRUE;


/*-----------------------------------------------------------------------
   Prof_InitTable: Seek the current profile and init the function table
  ----------------------------------------------------------------------*/



void Prof_InitTable(void)
{
  FILE *              Def_FILE;
  FILE *              Prof_FILE; 
  char *              ptr;
  char *              ptr2;
  char                TempString [MAX_PRO_LENGTH];
  int                 i = 0;


  /* Retrive thot.rc variables and open usefull files */
  ptr = TtaGetEnvString("Profiles_File");
  if (ptr && *ptr)
    Prof_FILE = fopen(ptr,"r");
  else
    Prof_FILE = NULL;
  
  ptr = TtaGetEnvString ("Profile");
  if (ptr && *ptr)
  {
    strcpy (UserProfile, AddHooks (ptr));	
  }
  else
     UserProfile[0] = EOS;
    
  
  ptr = TtaGetEnvString("THOTDIR");
  ptr2 = TtaGetMemory (strlen (ptr) + strlen (DEF_FILE) + 10);
  if (ptr2) 
    {
      sprintf (ptr2, "%s%c%s%c%s", ptr, DIR_SEP, "config", DIR_SEP, DEF_FILE);
      Def_FILE = fopen(ptr2, "r");
      TtaFreeMemory (ptr2);
    }
  else
    Def_FILE = NULL;
  
  if (Def_FILE && Prof_FILE && UserProfile)
     {    
       
       /* Fill a table for modules definition */
       FileToTable (Def_FILE, Def_Table, &Def_nbelem, MAX_DEF);
       
       /* Generate a functions table*/
       while (fgets(TempString, sizeof(TempString), Prof_FILE))
         {
	   
	   SkipAllBlanks(TempString);
  	   ProcessElements(TempString);
	 } 
       
       /* Sort the functions and edition table */
       SortTable(Fun_Table, Fun_nbelem);
       SortTable (Edition_Table, Edition_nbelem);
       /* delete the modules definition table */
       DeleteTable (Def_Table, &Def_nbelem);

       /* Check if the profile correspond to a read only profile */
       while (( i<Fun_nbelem) && (Prof_ReadOnly))
	 {
	   if (SearchInTable (Fun_Table[i], Edition_Table, Edition_nbelem, TRUE))
	     Prof_ReadOnly = FALSE;
	 }
       
     }       
  else
    {
      /* can NOT open profile files or no profile defined */
    }
  if (Fun_nbelem > 0)
    defined_profile = TRUE;
  
  /* Close the open files */
  if (Def_FILE) 
      fclose(Def_FILE);

  if (Prof_FILE)
      fclose(Prof_FILE);
}

/*--------------------------------------------------------
   Prof_RebuildProTable : Rebuild the Profiles Table
   Returns the number of elements if operation succeded or
   a 0 if operation failed.
----------------------------------------------------------*/

#ifdef __STDC__
int  Prof_RebuildProTable(STRING prof_file)
#else  /* !__STDC__ */
int  Prof_RebuildProTable(STRING prof_file)
#endif /* !__STDC__ */
{
  FILE *              Prof_FILE; 
  
  Prof_FILE = fopen(prof_file,"r");
  DeleteTable(Pro_Table, &Pro_nbelem);
  if (Prof_FILE)
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

/*----------------------------------------------------------------------
  ProcessElements : Recursive function that helps
  building the profile table
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static   void ProcessElements(char * element)
#else  /* !__STDC__ */
static   void ProcessElements(element)
char   *  element;
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
      /* Last case : the element is a function. */

      /* if the function is an edition function, insert in the edition table */
       if (element[0] == EDITION_REF)
	 {
	   /* remove edition tag */
	   while (element[j+1] != EOS)
	     {
	      element[j] = element[j+1];
	      j++;
	    }
	   element[j]=EOS;

	   /* insertion */
	   if (Edition_nbelem < MAX_EDITION_FUNCTIONS)
	     InsertTable(element, Edition_Table, &Edition_nbelem);
	   
	 }
     
       if ((strcmp(CurrentProfile, UserProfile) == 0) && (strlen(element) > 0))
	 {
	   
	   /* Insert the element in the table of functions */
	     
	     if (Fun_nbelem < MAX_FUNCTIONS)
	       InsertTable(element, Fun_Table, &Fun_nbelem);
	 }
    }
	
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
void Prof_DeleteFunTable(void)
#else  /* __STDC__ */
void Prof_DeleteFunTable(void)
#endif /* __STDC__ */
{
  DeleteTable(Fun_Table, & Fun_nbelem);
}


/*---------------------------------------------------------------
   Prof_GetProfilesItems :  Get the text for the profile menu items.
   Returns the number of items
------------------------------------------------------------------*/

#ifdef __STDC__
int  Prof_GetProfilesItems(STRING MenuText[])
#else  /* !__STDC__ */
int  Prof_GetProfilesItems(MenuText[])
STRING MenuText[];
#endif /* !__STDC__ */

{
  int                   nbelem = 0;
  int                   j;

  for (j=0 ; j < Pro_nbelem ; j++)
    {
      InsertTable( Pro_Table[j], MenuText, & nbelem);
    }
  return nbelem;
}


/*-----------------------------------------------------------------------------
  Prof_ItemNumber2Profile : Conversion between item number in the profile menu
  and the profile name
-----------------------------------------------------------------------------*/

#ifdef __STDC__
STRING Prof_ItemNumber2Profile(int ItemNumber)
#else  /* !__STDC__ */
STRING Prof_ItemNumber2Profile(ItemNumber)
int ItemNumber;
#endif /* !__STDC__ */
{
  if (Pro_nbelem)
    return Pro_Table[ItemNumber];
  else
    return NULL;
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


/*----------------------------------------------------------------------
    Prof_ShowSeparator : determines if a separator must be displayed
    in the menu interface. Two consecutive separators musn't be displayed
----------------------------------------------------------------------*/

#ifdef __STDC__
ThotBool Prof_ShowSeparator(Menu_Ctl *ptrmenu, int item, char LastItemType)
#else  /* __STDC__ */
ThotBool Prof_ShowSeparator(*ptrmenu, item, LastItemType)
Menu_Ctl *ptrmenu;
int       item;
char LastItemType;
#endif /* __STDC__ */
{  
  return !(( LastItemType ==  'S') || (item == 0) );
}


/*-----------------------------------------------------------------------
    Prof_ShowButton : Add a button if the function associated to that button
    belongs to the user profile
----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool Prof_ShowButton(STRING FunctionName)
#else  /* __STDC__ */
ThotBool Prof_ShowButton(FunctionName)
STRING   FunctionName;
#endif /* __STDC__ */
{
  return (Prof_BelongTable(FunctionName));
}


/*----------------------------------------------------------------------
    Prof_ShowSubMenu : Check if a submenu has to be displayed.
    A submenu musn't be displayed if it contains no entry
-----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool   Prof_ShowSubMenu(Menu_Ctl *ptrsubmenu)
#else  /* __STDC__ */
ThotBool   Prof_ShowSubMenu(ptrsubmenu)
Menu_Ctl  *ptrsubmenu;
#endif /* __STDC__ */
{
  int      item    = 0;
  
  if ((ptrsubmenu->ItemsNb == 0) || (ptrsubmenu == NULL))
    return FALSE;
  while (item < ptrsubmenu->ItemsNb)
    {
      if (ptrsubmenu->ItemsList[item].ItemType != TEXT('S'))
	return TRUE;
      item ++;
    }
  return FALSE;
}

/*----------------------------------------------------------------------
    Prof_ShowMenu : Check if a menu has to be displayed. A menu mustn't be
    displayed if it contains no entry.
-----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool   Prof_ShowMenu(Menu_Ctl *ptrmenu)
#else  /* __STDC__ */
ThotBool   Prof_ShowMenu(ptrmenu)
Menu_Ctl  *ptrmenu;
#endif /* __STDC__ */
{
  int      item    = 0;

  /* If profiles are not used, do not erase any menu */
  if (!defined_profile)
    return TRUE;
  /* this should not happen... */
  if (ptrmenu == NULL)
    return FALSE;
  /* check if the attr and select menu are in the profile */
  if (ptrmenu->MenuAttr)
    return (Prof_BelongTable(TEXT("MenuAttribute")));
  if (ptrmenu->MenuSelect)
    return (Prof_BelongTable(TEXT("MenuSelection")));    
  /* an empty menu has to be removed */
  if (ptrmenu->ItemsNb == 0)
    return FALSE;
  /* check if the menu is only composed of empty sub menus and separators */
  while (item < ptrmenu->ItemsNb)
    {
      if (ptrmenu->ItemsList[item].ItemType != TEXT('S'))
	{
	  if (ptrmenu->ItemsList[item].ItemType == TEXT('M'))
	    {
	      if (Prof_ShowSubMenu (ptrmenu->ItemsList[item].SubMenu)) 
		/* there is at least a non empty sub menu */
		return TRUE;
	   } 
	  else
	    /* there is at least a standard item */
	    return TRUE;
	}
      item ++;
    }
  return FALSE;
}



/*********************************************************************
  Table functions : set of functions to handle table manipulations :
  insertions, deletions, searchs, sorts
**********************************************************************/



/*----------------------------------------------------------------------
  InsertTable : Insert an element in a table
----------------------------------------------------------------------*/


#ifdef __STDC__
static void InsertTable(STRING function, STRING  * Table, int * nbelem)
#else  /* !__STDC__ */
static void InsertTable(function, Table, nbelem )
STRING         function;
STRING        * Table;
int           * nbelem;
#endif /* !__STDC__ */
{
  Table[*nbelem] = TtaStrdup (function);
  *nbelem = *nbelem+1;
}


/*----------------------------------------------------------------------
  FileToTable : Copy a text file in a table. The lines beginning with "#"
  are considered as comments and are not included
----------------------------------------------------------------------*/

#ifdef __STDC__
static void FileToTable(FILE * File, STRING * Table,int * nbelem, int maxelem)
#else  /* !__STDC__ */
static void FileToTable(File, Table, nbelem, maxelem)
STRING    *  Table;
FILE      *  File;
int       *  nbelem;
int         maxelem;
#endif /* !__STDC__ */
{
  
  while (fgets (TempString, sizeof (TempString), File) && *nbelem < maxelem)
    {
      SkipAllBlanks(TempString);
      if (strlen(TempString) >=2 && !(TempString[0] == '#' ))
	InsertTable (ISO2WideChar(TempString), Table, nbelem);
    }
}


/*-----------------------------------------------------------
  DeleteTable : Free all memory allocated for a table
-------------------------------------------------------------*/

#ifdef __STDC__
static void DeleteTable(STRING * Table, int  *nbelem)
#else  /* __STDC__ */
static void DeleteTable(Table, nbelem)
STRING      * Table;
int         * nbelem;
#endif /* __STDC__ */
{
  int        i;

  for ( i = 0; i < *nbelem; i++)
    TtaFreeMemory (Table[i]);
  *nbelem = 0;
}



/*----------------------------------------------------------------------
  Search a string in the table and return the line number of the first
  occurence or -1 if not found. The function can perform a dichotomic search 
  if the table given is sorted and if the boolean parameter is set TRUE.

  int SearchInTable(char * StringToFind, ** Researched string     
		    STRING Table[],      ** Table in which to search 
		    int nbelem,          ** Number of elements of the table
		    ThotBool sort)       ** Is the table sorted ?
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int SearchInTable(char * StringToFind, STRING  * Table,
			 int nbelem, ThotBool sort)
#else  /* !__STDC__ */
static int SearchInTable(stringToFind, Table, nbelem, sort) 
char * StringToFind;
STRING * Table;
int nbelem;
ThotBool sort;
#endif /* !__STDC__ */
{
  int              j = 0;
  ThotBool         found = FALSE;
  int              left, right, middle;
  char             temp [MAX_PRO_LENGTH];
  
  if (sort)
    {
      /* Dichotomic search */
      left = middle = 0;
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


/*------------------------------------------------------
  SortTable : Sort a table of strings in ascending order
--------------------------------------------------------*/

#ifdef __STDC__
static void SortTable (STRING * Table, int nbelem)
#else  /* !__STDC__ */
static void SortTable (Table, nbelem)
STRING  * Table;
int     nbelem;
#endif /* !__STDC__ */
{
  int i, j, kmax;
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



/*********************************************************
  String functions : set of useful functions for strings
***********************************************************/

/*-------------------------------------------------------------
  SkipNewLineSymbol : Remove the EOF ('end of line') character
------------------------------------------------------------*/

#ifdef __STDC__
static void SkipNewLineSymbol(char * Astring)
#else  /* !__STDC__ */
static void SkipNewLineSymbol(Astring)
char   * Astring;
#endif /* !__STDC__ */
{
  int         c = 0;

  while (Astring[c] != EOS && Astring[c] != EOL)
    {c++;}
  if (Astring[c] == EOL)
    Astring[c] = EOS;    
}


/*----------------------------------------------------
  SkipAllBlanks :  Remove all the spaces, tabulations,
  returns (CR) and "end of line" characters.
-----------------------------------------------------*/

#ifdef __STDC__
static void SkipAllBlanks (char  * Astring)
#else  /* !__STDC__ */
static void SkipAllBlanks (Astring)
char  * string;
#endif /* !__STDC__ */
{
  int c = 0;
  int nbsp = 0;
  
  do
    { 
      while (Astring[c+nbsp] == SPACE || Astring[c+nbsp] == TAB || Astring[c+nbsp] == __CR__)
	nbsp++;
      
      Astring [c] = Astring[c+nbsp];
    } 
  while (Astring [c++] != EOS);
  SkipNewLineSymbol(Astring);
 
}

/*--------------------------------------------------
  AddHooks : Jsut add the opening and closing hooks
  characters ('[' and ']') respectively at the beginning
  and at the end of the string
-----------------------------------------------------*/

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

  return TempString;
}

/*--------------------------------------------------
  RemoveHook : removes the character first which must
  be an opening hook ('[') and goes on recopying the
  string until the closing hook is found
----------------------------------------------------*/

#ifdef __STDC__
static void RemoveHooks (char * string)
#else  /* !__STDC__ */
static void RemoveHooks (string)
char * string;
#endif /* !__STDC__ */
{
  char         new[MAX_PRO_LENGTH];
  int          k = 0;

  while (string [k+1] != MODULE_END)
    {
      new[k] = string [k+1];
      k++;
    }
  new[k] = EOS;
  strcpy (string, new);
}
