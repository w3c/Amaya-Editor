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
 * Profile files are coded in ISO-latin
 *
 * Authors: L. Bonameau - Amaya profiles
            I. Vatton - review of the code
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

#include "ustring_f.h"

#define MAX_ENTRIES 10
typedef struct _Profile_Ctl *PtrProCtl;
typedef struct _ProElement
{
  char*                ProName;      /* Name of the entry */
  PtrProCtl            ProSubModule; /* Pointer to a sub-module context */
  ThotBool             ProIsModule;    /* TRUE if it is a sub-module */
  ThotBool             ProEdit;      /* TRUE if it's a editing function */
} ProElement;

typedef struct _Profile_Ctl
{
  ProElement           ProEntries[MAX_ENTRIES]; /* list of names */
  PtrProCtl            ProNext;                 /* next list */
} Profile_Ctl;


/*--------------------------
  Static vars
---------------------------*/
/* Profiles table contains the name of all the available profiles */
static PtrProCtl            ProfileTable = NULL;
static int                  NbProfiles = 0;
/* Modules table contains the name of all the available modules */
static PtrProCtl            ModuleTable = NULL;
static int                  NbModules = 0;
/* Functions table contains current list of available functions */
static PtrProCtl            FunctionTable = NULL;
static int                  NbFunctions = 0;
static char**               SortedFunctionTable = NULL;

/* The first context of the current module or profile in progress */
static PtrProCtl            CurrentModule;
static int                  CurrentEntries;
/* Determine either a profile is defined or not */
static ThotBool             CheckProfile = FALSE;
/* User Profile (taken from the thot.rc) */
static char                 UserProfile[MAX_PRO_LENGTH];
static PtrProCtl            UserProfContext = NULL;
static char                 ProfileBuff[MAX_PRO_LENGTH];
static char                 ProfileBuffA[MAX_PRO_LENGTH];
/* This boolean goes FALSE if the profile only contains browsing functions */
static ThotBool             EnableEdit = TRUE;

#include "registry_f.h"

/*********************************************************
  String functions : set of useful functions for strings
***********************************************************/
/*----------------------------------------------------------------------
  SkipNewLineSymbol : Remove the EOF ('end of line') character
----------------------------------------------------------------------*/
#ifdef __STDC__
static void   SkipNewLineSymbol (char* Astring)
#else  /* !__STDC__ */
static void   SkipNewLineSymbol (Astring)
char*         Astring;
#endif /* !__STDC__ */
{
  int         c = 0;

  while (Astring[c] != EOS && Astring[c] != EOL)
    c++;
  if (Astring[c] == EOL)
    Astring[c] = EOS;    
}


/*----------------------------------------------------------------------
  SkipAllBlanks:  Remove all the spaces, tabulations and return
  returns (CR) and "end of line" characters.
----------------------------------------------------------------------*/
#ifdef __STDC__
static void   SkipAllBlanks (char* Astring)
#else  /* !__STDC__ */
static void   SkipAllBlanks (Astring)
char*         string;
#endif /* !__STDC__ */
{
  int         c = 0;
  int         nbsp = 0;
  
  do
    {
      while (Astring[c+nbsp] == WC_SPACE || Astring[c+nbsp] == WC_TAB || Astring[c+nbsp] == WC_CR)
	nbsp++;
      Astring[c] = Astring[c+nbsp];
    } 
  while (Astring[c++] != WC_EOS);

  SkipNewLineSymbol (Astring);
}


/*----------------------------------------------------------------------
  AddInTable inserts an element in a table.
  The parameter name is the name of the new entry.
  The parameter isModule is TRUE if that entry is the name of a module.
  The parameter edit is TRUE if that entry is an editable function.
  The parameter subModule points to the first ctxt of the sub-module. If
  t is NULL and isModule is TRUE the context is created.
  The parameter number give the current number of entries in the table.
  The parameter ctxt points to the first ctxt of the table.
  Return the first context of the new module.
----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrProCtl   AddInTable (char* name, ThotBool isModule, ThotBool edit, PtrProCtl subModule, int number, PtrProCtl ctxt)
#else  /* !__STDC__ */
static PtrProCtl   AddInTable (name, isModule, edit, subModule, number, ctxt)
char*              name;
ThotBool           isModule;
ThotBool           edit;
int                number;
PtrProCtl          table;
#endif /* !__STDC__ */
{
  PtrProCtl           current, prev, new;
  int                 i;

  i = number;
  current = ctxt;
  new = NULL;
  prev = NULL;
  /* skip full contexts */
  while (i >= MAX_ENTRIES && current)
    {
      prev = current;
      current = current->ProNext;
      i -= MAX_ENTRIES;
    }
  /* add a next context if it is necessary */
  if (prev && i == 0)
    {
      i = 0;
      current = (PtrProCtl) TtaGetMemory (sizeof (Profile_Ctl));
      memset (current, 0, sizeof (Profile_Ctl));
      prev->ProNext = current;
    }

  /* add the new entry */
  if (current && i < MAX_ENTRIES)
    {
      if (name)
	{
	  current->ProEntries[i].ProName = TtaStrdup (name);
	  current->ProEntries[i].ProIsModule = isModule;
	  current->ProEntries[i].ProEdit = edit;
	  if (isModule && subModule == NULL)
	    {
	      /* allocate the context of the sub-module */
	      new = (PtrProCtl) TtaGetMemory (sizeof (Profile_Ctl));
	      memset (new, 0, sizeof (Profile_Ctl));
	      current->ProEntries[i].ProSubModule = new;
	    }
	  else
	    current->ProEntries[i].ProSubModule = subModule;
	}
    }
  return new;
}

/*----------------------------------------------------------------------
  AddModule inserts a new module in the module table.
  Return the first context of the new module.
----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrProCtl    AddModule (char* name)
#else  /* !__STDC__ */
static PtrProCtl    AddModule (name)
char*               name;
#endif /* !__STDC__ */
{
  PtrProCtl     new = NULL;

  /* Register the new module */
  if (NbModules == 0)
    {
      ModuleTable = (PtrProCtl) TtaGetMemory (sizeof (Profile_Ctl));
      memset (ModuleTable, 0, sizeof (Profile_Ctl));
    }
  new = AddInTable (name, TRUE, FALSE, NULL, NbModules, ModuleTable);
  NbModules++;
  return new;
}


/*----------------------------------------------------------------------
  AddProfile inserts a new profile in the profile table.
  Return the first context of the new profile.
----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrProCtl    AddProfile (char* name)
#else  /* !__STDC__ */
static PtrProCtl    AddProfile (name)
char*               name;
#endif /* !__STDC__ */
{
  PtrProCtl     new = NULL;

  /* Register the new profile */
  if (NbProfiles == 0)
    {
      ProfileTable = (PtrProCtl) TtaGetMemory (sizeof (Profile_Ctl));
      memset (ProfileTable, 0, sizeof (Profile_Ctl));
    }
  new = AddInTable (name, TRUE, FALSE, NULL, NbProfiles, ProfileTable);
  NbProfiles++;
  /* store the context of the user profile */
  if (UserProfContext == NULL && !strcmp (name, UserProfile))
    UserProfContext = new;
  return new;
}


/*----------------------------------------------------------------------
  DeleteTable frees all contexts allocated for the table.
  The parameter ctxt points to the first context of the table.
  The parameter recursive is TRUE if the referred context must be freed.
----------------------------------------------------------------------*/
#ifdef __STDC__
static void    DeleteTable (PtrProCtl ctxt, ThotBool recursive)
#else  /* __STDC__ */
static void    DeleteTable (ctxt, recursive)
PtrProCtl      ctxt;
ThotBool       recursive;
#endif /* __STDC__ */
{
  PtrProCtl           next;
  int                 i;

  while (ctxt)
    {
      i = 0;
      while (i < MAX_ENTRIES && ctxt->ProEntries[i].ProName)
	{
	  TtaFreeMemory (ctxt->ProEntries[i].ProName);
	  if (recursive)
	    DeleteTable (ctxt->ProEntries[i].ProSubModule, FALSE);
	  i++;
	}
      /* free the current context and manage the next one */
      next = ctxt->ProNext;
      TtaFreeMemory (ctxt);
      ctxt = next;
    }
}


/*----------------------------------------------------------------------
  SearchModule searchs a module in the module table and return the
  pointer to current entry.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ProElement  *SearchModule (char* name)
#else  /* !__STDC__ */
static ProElement  *SearchModule (name) 
char*               name;
#endif /* !__STDC__ */
{
  PtrProCtl        current;
  int              i = 0;
  ThotBool         found;

  if (name == NULL)
    return NULL;

  /* check the current list of modules */
  current = ModuleTable;
  found = FALSE;
  while (!found && current)
    {
      i = 0;
      while (i < MAX_ENTRIES && current->ProEntries[i].ProName &&
	     strcmp (name, current->ProEntries[i].ProName))
	i++;
      found = (i < MAX_ENTRIES && current->ProEntries[i].ProName);
      if (!found)
	current = current->ProNext;
    }
  if (found)
    return &(current->ProEntries[i]);
  else
    return NULL;
}


/*----------------------------------------------------------------------
  ProcessDefinition : Recursive function that helps
  building the profile table
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void    ProcessDefinition (char* element)
#else  /* !__STDC__ */
static void    ProcessDefinition (element)
char*         element;
#endif /* !__STDC__ */ 
{
  ProElement      *pEntry;
  PtrProCtl        ctxt;
  int              i;
  
  if (*element == PROFILE_START)
    {
      /*
       * It's a profile definition -> insert it in the Profile table
       * Remove the start tag and the end tag
       */
      i = 1;
      while (element[i] != PROFILE_END)
	i++;
      element[i] = EOS;
      /* The new profile in progress */
      CurrentModule = AddProfile (&element[1]);
      CurrentEntries = 0;
    }
  else if (*element == MODULE_START)
    {
      /*
       * It's a module definition -> insert it in the Definiton table
       * Remove the start tag and the end tag
       */
      i = 1;
      while (element[i] != MODULE_END)
	i++;
      element[i] = EOS;

      /* Is the module already declared? */
      pEntry = SearchModule (&element[1]);
      if (pEntry == NULL)
	/* The new profile in progress */
	CurrentModule = AddModule (&element[1]);
      else
	/* The new profile in progress */
	CurrentModule = pEntry->ProSubModule;
      CurrentEntries = 0;
    }
  else if (*element == MODULE_REF)
    {
      /*
       * The element is a module inclusion -> insert it in the module
       * or the profile in progress.
       * Skip the MODULE_REF tag
       */
      pEntry = SearchModule (&element[1]);
      if (pEntry == NULL)
	{
	  /* the module is not already declared */
	  ctxt = AddModule (&element[1]);
	}
      else
	ctxt = pEntry->ProSubModule;
      /* add the new entry in the current profile or module */
      AddInTable (&element[1], TRUE, FALSE, ctxt, CurrentEntries, CurrentModule);
      CurrentEntries++;
    }
  else
    {
      /*
       * The element is a function -> insert it in the module
       * or the profile in progress.
       * Look for and skip the EDITION_REF tag
       */

      /* add the new entry in the current profile or module */
       if (*element == EDITING_REF)
	 AddInTable (&element[1], FALSE, TRUE, NULL, CurrentEntries, CurrentModule);
       else
	 AddInTable (element, FALSE, FALSE, NULL, CurrentEntries, CurrentModule);
      CurrentEntries++;
    }
}


/*-----------------------------------------------------------------------
   AddFunctions keep in the function table the list of functions
   declared in the list of contexts.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void AddFunctions (PtrProCtl ctxt)
#else  /* !__STDC__ */
static void AddFunctions (ctxt)
PtrProCtl   ctxt;
#endif /* !__STDC__ */
{
  int           i;

  while (ctxt)
    {
      i = 0;
      while (i < MAX_ENTRIES && ctxt->ProEntries[i].ProName)
	{
	  if (ctxt->ProEntries[i].ProIsModule)
	    /* add functions of the sub-module */
	    AddFunctions (ctxt->ProEntries[i].ProSubModule);
	  else
	    {
	      AddInTable (ctxt->ProEntries[i].ProName, FALSE, ctxt->ProEntries[i].ProEdit, NULL, NbFunctions, FunctionTable);
	      NbFunctions++;
	    }
	  /* next entry */
	  i++;
	}
      /* next context */
      ctxt = ctxt->ProNext;
    }
}


/*----------------------------------------------------------------------
  SortFunctionTable generates a sorted the function table in ascending
  order.
  ----------------------------------------------------------------------*/
static void SortFunctionTable ()
{
  PtrProCtl     ctxt;
  char*         ptr;
  int           i, j, index;

  /* copy the list of contexts in a large table */
  SortedFunctionTable = (char**) TtaGetMemory (NbFunctions * sizeof (char*));
  index = 0;
  ctxt = FunctionTable;
  EnableEdit = FALSE;
  while (ctxt && index < NbFunctions)
    {
      i = 0;
      while (i < MAX_ENTRIES && ctxt->ProEntries[i].ProName)
	{
	  if (!EnableEdit && ctxt->ProEntries[i].ProEdit)
	    /* there is almost one editing function */
	    EnableEdit = TRUE;
	  SortedFunctionTable[index] = TtaStrdup (ctxt->ProEntries[i].ProName);
	  index++;
	  /* next entry */
	  i++;
	}
      /* next context */
      ctxt = ctxt->ProNext;
    }

  /* sort the large table */
  for (i = 0 ; i < NbFunctions; i++)
    {
      index = i;
      for (j = i+1; j < NbFunctions; j++)
        if (strcmp (SortedFunctionTable[j], SortedFunctionTable[index]) <= 0)
	  index = j;
      ptr = SortedFunctionTable[index];
      SortedFunctionTable[index] = SortedFunctionTable[i];
      SortedFunctionTable[i] = ptr;
    }

  /* delete the function table */
  if (FunctionTable)
    {
      DeleteTable (FunctionTable, FALSE);
      FunctionTable = NULL;
    }
}


/*----------------------------------------------------------------------
  Prof_BelongTable searchs a function in the function table and return TRUE
  if the function exists.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool    Prof_BelongTable (char* name)
#else  /* !__STDC__ */
ThotBool    Prof_BelongTable (name) 
char*       name;
#endif /* !__STDC__ */
{
  int              left, right, middle, i;
  ThotBool         found = FALSE;

  if (NbFunctions == 0)
    /* All functions are allowed */
    return TRUE;

  /* Dichotomic search */
  left = middle = 0;
  right = NbFunctions - 1;
 
  while (left <= right && !found)
    {
      middle = (right + left) / 2;
      i = strcmp (SortedFunctionTable[middle], name);
      if (i == 0)
	found = TRUE;
      else if (i < 0)
	left = middle + 1;
      else
	right = middle - 1;
    }
  return found;
}


/*-----------------------------------------------------------------------
   Prof_InitTable: Seek the current profile file and init tables
  ----------------------------------------------------------------------*/
void Prof_InitTable ()
{
  FILE*     profFile;
  CHAR_T*   ptr;
  CHAR_T    buffer[MAX_LENGTH];

  /* TODO: use a ISO functions for TtaGetEnvString and TtaStrdup */
  /* Retreive thot.rc variables and open the profile file */
  ptr = TtaGetEnvString ("Profiles_File");

  if (ptr && *ptr)
    {
      if (SearchFile (ptr, 2, buffer))
	{
	  profFile = ufopen (buffer, TEXT("r"));
	  /* what is the current active profile */  
	  ptr = TtaGetEnvString ("Profile");
	  if (ptr && *ptr)
	    wc2iso_strcpy (UserProfile, ptr);	
	  else
	    UserProfile[0] = WC_EOS;

	  /* Fill a profile and module tables */
	  while (fgets (ProfileBuff, sizeof (ProfileBuff), profFile))
	    {
	      SkipAllBlanks (ProfileBuff);
	      /* skip comments */
	      if (strlen (ProfileBuff) >=2 && ProfileBuff[0] != '#')
		ProcessDefinition (ProfileBuff);
	    }
	  fclose (profFile);

	}
    }

  /* All functions are available when:
   * - the profiles file doesn't exist
   * - or there is no specific user profile
   * - or the user profile is None
   */
  if (NbProfiles > 0 && UserProfile[0] != EOS &&  strcmp (UserProfile, "None"))
    {
      CheckProfile = TRUE;
      /* Now build the list of current available functions */
      if (UserProfContext)
	{
	  FunctionTable = (PtrProCtl) TtaGetMemory (sizeof (Profile_Ctl));
	  memset (FunctionTable, 0, sizeof (Profile_Ctl));
	  AddFunctions (UserProfContext);
	  /* generate a sorted list of available functions */
	  SortFunctionTable ();
	}
    }
  /* TODO: remove that clean up as soon as we are able to update Amaya UI */
  /* delete the modules table */
  DeleteTable (ModuleTable, TRUE);
  ModuleTable = NULL;
  NbModules = 0;
}


/*-----------------------------------------------------------------------
   Prof_FreeTable: Remove profile tables
  ----------------------------------------------------------------------*/
void Prof_FreeTable ()
{
  int i;

  /* delete the profiles table */
  DeleteTable (ProfileTable, TRUE);
  NbProfiles = 0;
  UserProfContext = NULL;
  for (i = 0; i < NbFunctions; i++)
    TtaFreeMemory (SortedFunctionTable[i]);
  TtaFreeMemory (SortedFunctionTable);
  SortedFunctionTable = NULL;
  NbFunctions = 0;
}


/*----------------------------------------------------------------------
  TtaRebuildProTable: Rebuild the Profiles Table
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void     TtaRebuildProTable (CHAR_T* prof_file)
#else  /* !__STDC__ */
void     TtaRebuildProTable (prof_file)
CHAR_T*  prof_file;
#endif /* !__STDC__ */
{
  /* delete the profiles table */
  Prof_FreeTable ();
  Prof_InitTable ();
}


/*----------------------------------------------------------------------
  TtaCanEdit returns TRUE if there is almost one editing function active.
  ----------------------------------------------------------------------*/
ThotBool    TtaCanEdit ()
{
  return (EnableEdit);
}

/*----------------------------------------------------------------------
   TtaGetProfileFileName:  Get the text for the profile file name.
   name is a provided buffer of length characters to receive the name.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void     TtaGetProfileFileName (CHAR_T* name, int length)
#else  /* !__STDC__ */
void     TtaGetProfileFileName (name, length)
CHAR_T*  name;
#endif /* !__STDC__ */
{
  CHAR_T*   ptr;
  CHAR_T    buffer[MAX_LENGTH];

  name[0] = EOS;
  ptr = TtaGetEnvString ("Profiles_File");
  if (ptr && *ptr)
    {
      SearchFile (ptr, 2, buffer);
      if (ustrlen (buffer) < (size_t)length)
	ustrcpy (name, buffer);
    }
}

/*----------------------------------------------------------------------
   TtaGetDefProfileFileName:  Get the text for the default profile file name.
   name is a provided buffer of length characters to receive the name.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void     TtaGetDefProfileFileName (CHAR_T* name, int length)
#else  /* !__STDC__ */
void     TtaGetDefProfileFileName (name, length)
CHAR_T*  name;
#endif /* !__STDC__ */
{
  CHAR_T* ptr;
  CHAR_T  buffer[200];

  name[0] = WC_EOS;
  ptr = TtaGetDefEnvString ("Profiles_File");
  if (ptr && *ptr)
    {
      SearchFile (ptr, 2, buffer);
      if (ustrlen (buffer) < (size_t)length)
	ustrcpy (name, buffer);
    }
}


/*----------------------------------------------------------------------
   TtaGetProfilesItems:  Get the text for the profile menu items.
   listEntries is a provided list of length pointers.
   Returns the number of items
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int      TtaGetProfilesItems (char** listEntries, int length)
#else  /* !__STDC__ */
int      TtaGetProfilesItems (listEntries, length)
char**   listEntries;
#endif /* !__STDC__ */
{
  PtrProCtl     ctxt;
  int           nbelem = 0;
  int           i;

  ctxt = ProfileTable;
  while (ctxt && nbelem < NbProfiles && nbelem < length)
    {
      i = 0;
      while (i < MAX_ENTRIES && ctxt->ProEntries[i].ProName)
	{
	  listEntries[nbelem] = ctxt->ProEntries[i].ProName;
	  i++;
	  nbelem++;
	}
      ctxt = ctxt->ProNext;
    }
  return nbelem;
}


/*----------------------------------------------------------------------
    Prof_ShowSeparator : determines if a separator must be displayed
    in the menu interface. Two consecutive separators musn't be displayed
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool     Prof_ShowSeparator (Menu_Ctl *ptrmenu, int item, char LastItemType)
#else  /* __STDC__ */
ThotBool     Prof_ShowSeparator (*ptrmenu, item, LastItemType)
Menu_Ctl    *ptrmenu;
int          item;
char         LastItemType;
#endif /* __STDC__ */
{  
  return !(LastItemType == 'S' || item == 0);
}


/*-----------------------------------------------------------------------
    Prof_ShowButton : Add a button if the function associated to that button
    belongs to the user profile
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool    Prof_ShowButton (char* FunctionName)
#else  /* __STDC__ */
ThotBool    Prof_ShowButton (FunctionName)
char*       FunctionName;
#endif /* __STDC__ */
{
  return (Prof_BelongTable (FunctionName));
}


/*----------------------------------------------------------------------
    Prof_ShowSubMenu : Check if a submenu has to be displayed.
    A submenu musn't be displayed if it contains no entry
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool   Prof_ShowSubMenu (Menu_Ctl *ptrsubmenu)
#else  /* __STDC__ */
ThotBool   Prof_ShowSubMenu (ptrsubmenu)
Menu_Ctl  *ptrsubmenu;
#endif /* __STDC__ */
{
  int      item    = 0;
  
  if (ptrsubmenu->ItemsNb == 0 || ptrsubmenu == NULL)
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
ThotBool   Prof_ShowMenu (Menu_Ctl *ptrmenu)
#else  /* __STDC__ */
ThotBool   Prof_ShowMenu (ptrmenu)
Menu_Ctl  *ptrmenu;
#endif /* __STDC__ */
{
  int      item    = 0;

  if (!CheckProfile)
    /* there is no user profile, do not erase any menu */
    return TRUE;
  else if (ptrmenu == NULL)
    /* this should not happen... */
    return FALSE;
  else if (ptrmenu->MenuAttr)
    /* check if the attr and select menu are in the profile */
    return (Prof_BelongTable ("MenuAttribute"));
  else if (ptrmenu->MenuSelect)
    return (Prof_BelongTable ("MenuSelection"));    
  else if (ptrmenu->ItemsNb == 0)
    /* an empty menu has to be removed */
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
