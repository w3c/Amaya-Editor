/*
 *
 *  COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Amaya browser functions called form Thot and declared in HTML.A.
 * These functions concern links and other HTML general features.
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
#define THOT_EXPORT extern
#define MAX_FUNCTIONS 500
#define MAX_LENGTH    30


/* determine either a profile is defined or not */
static ThotBool   defined_profile = FALSE;


/* Defintion of the table of profile functions */
static STRING     tab[MAX_FUNCTIONS];
static int        nb_elem = 0;


/*-------------------------------------------------------------------------
   InitProfileTable: Seek the current profile and init the function table
--------------------------------------------------------------------------*/

void InitProfileTable()
{

  STRING              Current_Profile;
  FILE *              proFILE;
  STRING              Profiles_File;
  ThotBool            found = FALSE;
  char                string[MAX_LENGTH];
  ThotBool            EOP = FALSE;

  /* Get the current profile */
  Current_Profile = TtaGetEnvString(TEXT("Profile"));
  Profiles_File = TtaGetEnvString(TEXT("Profiles_File"));

  if (Current_Profile != NULL)
    {
#ifdef DEBUG
      puts(Current_Profile);
#endif
      proFILE = fopen(Profiles_File,"r");
      if (proFILE != NULL)
	{    
	  /* seek the chosen profile in the profile file*/
	  while (!(found) && fgets(string, sizeof string, proFILE))
	    {
	      if (ustrncmp(Current_Profile,TEXT(string),ustrlen(Current_Profile)) == 0) 
		{ 
#ifdef DEBUG
		  fputs(string,stdout);
#endif
		  found = TRUE;		  
		}
	    } 
	  if (found)
	    {
	      /* Profile found - Fill in the Profile Table */
	      while (fgets(string, sizeof string, proFILE) && !EOP)
		{
		  /* Add a function to the table */
#ifdef DEBUG
		  fputs(string,stdout);	  
#endif
		  skip_spaces (string);
		  if ((string[0] == '[') && (string[strlen(string)-2] == ']'))
		    {
		      EOP = TRUE;
		    }
		  else
		    {
		      tab[nb_elem] = TtaGetMemory(sizeof string);
		      ustrcpy(tab[nb_elem],TEXT(string));
		      tab[nb_elem][ustrlen(tab[nb_elem])-1] = EOS;
		      nb_elem++;
		    }
		}
	    }
	  else
	    {
	      /* Error : Chosen profile NOT found in the profile file */
	    }
	  fclose (proFILE);
	}
      else
	{
	  /* error : Can not open the profile file */ 
	}
    }
  else
    {
      /* no profile defined */
    }
if (nb_elem > 0)
  {
    defined_profile = TRUE;
  }
}



/*-----------------------------------------------------------------------------------
   BelongProfileTable : Check if a function belongs to the profile functions table
------------------------------------------------------------------------------------*/
   
#ifdef __STDC__
ThotBool BelongProfileTable(STRING functionName)
#else  /* __STDC__ */
ThotBool BelongProfileTable(functionName)
STRING functionNam;
#endif /* __STDC__ */
{

  int              j = 0;
  ThotBool         found = FALSE;

  /* is a user profile defined ?*/
  if (!defined_profile)
    return TRUE;

  /* seek for functionName in the table */
#ifdef DEBUG
  puts(functionName);
#endif
  while (!(found) && (j < nb_elem))
    {
#ifdef DEBUG
      puts(tab[j]);
#endif
      if (ustrcmp(functionName, tab[j]) == 0)
	{
	  found = TRUE;
	}
      j ++;
    }
  return found;
}



/*------------------------------------------------------------------------
    RemoveBadSeparators : Remove bad separators in the menu interface
--------------------------------------------------------------------------*/


#ifdef __STDC__
ThotBool RemoveBadSeparators(Menu_Ctl *ptrmenu,int item,char LastItemType)
#else  /* __STDC__ */
ThotBool RemoveBadSeparators(*ptrmenu, item)
Menu_Ctl *ptrmenu;
int item;
#endif /* __STDC__ */
{

  
  if (( LastItemType ==  'S') || (item == 0) )
    return TRUE; 
  else
    return FALSE;

  LastItemType = ptrmenu->ItemsList[item].ItemType;

  
}

/*----------------------------------------------------------------------
    RemoveSubMenu : Remove a submenu
-----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool RemoveSubMenu(Menu_Ctl *ptrsubmenu)
#else  /* __STDC__ */
ThotBool RemoveSubMenu(ptrsubmenu)
Menu_Ctl *ptrsubmenu;

#endif /* __STDC__ */
{
  int      item    = 0;

  if (ptrsubmenu == NULL)
    return TRUE;
  if (ptrsubmenu->ItemsNb == 0)
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
    RemoveMenu : Remove a menu
-----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool RemoveMenu(Menu_Ctl *ptrmenu)
#else  /* __STDC__ */
ThotBool RemoveMenu(ptrmenu)
Menu_Ctl *ptrmenu;

#endif /* __STDC__ */
{
  int      item    = 0;

  return FALSE;
  if (ptrmenu == NULL)
    return TRUE;
  if (ptrmenu->ItemsNb == 0)
    return TRUE;
  while (item < ptrmenu->ItemsNb)
    {
      if (ptrmenu->ItemsList[item].ItemType != TEXT('S'))
	{
	  if (ptrmenu->ItemsList[item].ItemType == TEXT('M'))
	   {
	     if (!RemoveSubMenu (ptrmenu->ItemsList[item].SubMenu)) 
	       return FALSE;
	   } 
	  else
	    return FALSE;
	}
      item ++;
    }
  return TRUE;

}
/*------------------------------------------------------------------------
    ProfileCompilator : Generates the profile file
--------------------------------------------------------------------------*/

void ProfileCompilator()
{
  
}


void skip_spaces (char *string)
{
  int c = 0;
  int nbsp = 0;

  do
    { 
      while (string[c+nbsp] == ' ' || string[c+nbsp] == '\t')
	{
	  nbsp++;
	}
      
      string [c] = string[c+nbsp];
    }
  while (string [c++] != EOS);

}
  


