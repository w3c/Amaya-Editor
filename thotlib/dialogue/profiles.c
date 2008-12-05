/*
 *
 *  COPYRIGHT MIT and INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * profile.c: This module contains all the functions used to handle
 * the profile configuration in Amaya. Each function beginning with
 * "Prof_" prefix is called by an extern module.
 * Profile files are coded in ISO-latin
 *
 * Authors: I. Vatton, L. Bonameau
 *
 */

/* Included headerfiles */
#include "thot_gui.h"
#include "thot_sys.h"
#include "string.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "application.h"
#include "registry.h"
#include "profiles.h"

#include "containers.h"

#define PROFILE_START              '<'
#define PROFILE_END                '>'
#define DOCTYPE_START              '{'
#define DOCTYPE_END                '}'
#define MODULE_START               '['
#define MODULE_END                 ']'
#define MODULE_REF                 '+'
#define EDITING_REF                '&'
#define GUI_REF                    '*'

#define MAX_ENTRIES 10

typedef struct _Profile_Ctl *PtrProCtl;
typedef struct _ProElement
{
  char                *ProName;      /* Name of the entry */
  PtrProCtl            ProSubModule; /* Pointer to a sub-module context */
  ThotBool             ProIsModule;  /* TRUE if it is a sub-module */
  ThotBool             ProEdit;      /* TRUE if it's a editing function */
  ThotBool             ProGui;       /* TRUE if it's a gui */
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
/* Doctypes table contains the name of all the available doctypes */
static PtrProCtl            DoctypeTable = NULL;
static int                  NbDoctypes = 0;
/* Modules table contains the name of all the available modules */
static PtrProCtl            ModuleTable = NULL;
static int                  NbModules = 0;
/* Functions table contains current list of available functions */
static PtrProCtl            FunctionTable = NULL;
static int                  NbFunctions = 0;
static char               **SortedFunctionTable = NULL;
static int                 *FunctionMask = NULL;
static ThotBool            *FunctionRO = NULL;

/* The first context of the current module or profile in progress */
static PtrProCtl            CurrentModule;
static int                  CurrentEntries;

/* Determine either a profile is defined or not */
static ThotBool             CheckProfile = FALSE;
/* User Profile (taken from the thot.rc) */
static char                 UserProfile[MAX_PRO_LENGTH];
static PtrProCtl            UserProfContext = NULL;
static char                 ProfileBuff[MAX_PRO_LENGTH];
/* This boolean goes FALSE if the profile only contains browsing functions */
static ThotBool             EnableEdit = TRUE;


/* List containing names of GUI allowed elements.*/
static DLList               GuiList = NULL;

#include "registry_f.h"

/*********************************************************
  String functions : set of useful functions for strings
***********************************************************/


/*----------------------------------------------------------------------
  SkipAllBlanks:  Remove all the spaces, tabulations and return
  returns (CR) and "end of line" characters.
  ----------------------------------------------------------------------*/
static void SkipAllBlanks (char *Astring)
{
  int         c = 0;
  int         nbsp = 0;

  /* locate the biginning of the string */
  while (Astring[c + nbsp] != EOS &&
         (Astring[c + nbsp] == SPACE || Astring[c + nbsp] == TAB ||
          Astring[c + nbsp] == __CR__))
    nbsp++;
  /* suppress blanks before */
  while (Astring[c] != EOS)
    {
      Astring[c] = Astring[c + nbsp];
      c++;
    }
  /* suppress blanks after the string */
  while (c > 0 &&
         (Astring[c - 1] == SPACE || Astring[c - 1] == TAB ||
          Astring[c - 1] == __CR__ || Astring[c - 1] == EOL))
    {
      c--;
      Astring[c] = EOS;
    }
  do
    {
      while (Astring[c + nbsp] == SPACE || Astring[c + nbsp] == TAB ||
             Astring[c + nbsp] == __CR__)
        nbsp++;
      Astring[c] = Astring[c + nbsp];
    }
  while (Astring[c++] != EOS);
}

/*----------------------------------------------------------------------
  AddInTable inserts an element in a table.
  The parameter name is the name of the new entry.
  The parameter isModule is TRUE if that entry is the name of a module.
  The parameter edit is TRUE if that entry is an editable function.
  The parameter gui is TRUE if that entry is gui.
  The parameter subModule points to the first ctxt of the sub-module. If
  t is NULL and isModule is TRUE the context is created.
  The parameter number give the current number of entries in the table.
  The parameter ctxt points to the first ctxt of the table.
  Return the first context of the new module.
  ----------------------------------------------------------------------*/
static PtrProCtl AddInTable (char *name, ThotBool isModule, ThotBool edit,
                             ThotBool gui,
                             PtrProCtl subModule, int number,
                             PtrProCtl ctxt)
{
  PtrProCtl           current, prev, new_;
  int                 i;

  i = number;
  current = ctxt;
  new_ = NULL;
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
          current->ProEntries[i].ProGui = gui;
          if (isModule && subModule == NULL)
            {
              /* allocate the context of the sub-module */
              new_ = (PtrProCtl) TtaGetMemory (sizeof (Profile_Ctl));
              memset (new_, 0, sizeof (Profile_Ctl));
              current->ProEntries[i].ProSubModule = new_;
            }
          else
            current->ProEntries[i].ProSubModule = subModule;
        }
    }
  return new_;
}

/*----------------------------------------------------------------------
  AddModule inserts a new module in the module table.
  Return the first context of the new module.
  ----------------------------------------------------------------------*/
static PtrProCtl AddModule (char *name)
{
  PtrProCtl     new_ = NULL;

  /* Register the new module */
  if (NbModules == 0)
    {
      ModuleTable = (PtrProCtl) TtaGetMemory (sizeof (Profile_Ctl));
      memset (ModuleTable, 0, sizeof (Profile_Ctl));
    }
  new_ = AddInTable (name, TRUE, FALSE, FALSE, NULL, NbModules, ModuleTable);
  NbModules++;
  return new_;
}

/*----------------------------------------------------------------------
  AddProfile inserts a new profile in the profile table.
  Return the first context of the new profile.
  ----------------------------------------------------------------------*/
static PtrProCtl AddProfile (char *name)
{
  PtrProCtl     new_ = NULL;

  if (!strcmp (name, UserProfile))
    {
      /* Register the new profile */
      if (NbProfiles == 0)
        {
          ProfileTable = (PtrProCtl) TtaGetMemory (sizeof (Profile_Ctl));
          memset (ProfileTable, 0, sizeof (Profile_Ctl));
        }
       new_ = AddInTable (name, TRUE, FALSE, FALSE, NULL, NbProfiles, ProfileTable);
       NbProfiles++;
       /* store the context of the user profile */
       if (UserProfContext == NULL)
         UserProfContext = new_;
    }
  return new_;
}

/*----------------------------------------------------------------------
  AddDoctype inserts a new doctype profile in the table.
  Return the first context of the new profile.
  ----------------------------------------------------------------------*/
static PtrProCtl AddDoctype (char *name)
{
  PtrProCtl     new_ = NULL;

  /* Register the new profile */
  if (NbDoctypes == 0)
    {
      DoctypeTable = (PtrProCtl) TtaGetMemory (sizeof (Profile_Ctl));
      memset (DoctypeTable, 0, sizeof (Profile_Ctl));
    }
  new_ = AddInTable (name, TRUE, FALSE, FALSE, NULL, NbDoctypes, DoctypeTable);
  NbDoctypes++;
  return new_;
}



/*----------------------------------------------------------------------
  DeleteTable frees all contexts allocated for the table.
  The parameter ctxt points to the first context of the table.
  The parameter recursive is TRUE if the referred context must be freed.
  ----------------------------------------------------------------------*/
static void DeleteTable (PtrProCtl ctxt, ThotBool recursive)
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
static ProElement *SearchModule (char *name)
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
static void ProcessDefinition (char *element)
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
  else if (*element == DOCTYPE_START)
    {
      /*
       * It's a document profile definition -> insert it in the doctype table
       * Remove the start tag and the end tag
       */
      i = 1;
      while (element[i] != DOCTYPE_END)
        i++;
      element[i] = EOS;
      /* The new profile in progress */
      CurrentModule = AddDoctype (&element[1]);
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
      AddInTable (&element[1], TRUE, FALSE, FALSE, ctxt, CurrentEntries, CurrentModule);
      CurrentEntries++;
    }
  else if (*element == GUI_REF)
    {
      /*
       * The element is a gui -> insert it in the module
       * or the profile in progress.
       * Skip the GUI_REF tag
       */

      /* add the new entry in the current profile or module */
      AddInTable (&element[1], FALSE, FALSE, TRUE, NULL, CurrentEntries, CurrentModule);
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
        AddInTable (&element[1], FALSE, TRUE, FALSE, NULL, CurrentEntries, CurrentModule);
      else
        AddInTable (element, FALSE, FALSE, FALSE, NULL, CurrentEntries, CurrentModule);
      CurrentEntries++;
    }
}

/*-----------------------------------------------------------------------
  AddFunctionsAndGuis keep in the function table (resp. gui) the list of
  functions (resp. gui)declared in the list of contexts.
  ----------------------------------------------------------------------*/
static void AddFunctionsAndGuis (PtrProCtl ctxt, PtrProCtl functionTable)
{
  int           i;

  while (ctxt)
    {
      i = 0;
      while (i < MAX_ENTRIES && ctxt->ProEntries[i].ProName)
        {
          if (ctxt->ProEntries[i].ProIsModule)
            /* add functions and gui of the sub-module */
            AddFunctionsAndGuis (ctxt->ProEntries[i].ProSubModule, functionTable);
          else
            {
              if(ctxt->ProEntries[i].ProGui)
                  DLList_Append(GuiList, TtaStrdup(ctxt->ProEntries[i].ProName));
              else
                {
                  AddInTable (ctxt->ProEntries[i].ProName, FALSE,
                              ctxt->ProEntries[i].ProEdit, FALSE, NULL,
                              NbFunctions, functionTable);
                  NbFunctions++;
                }
            }
          /* next entry */
          i++;
        }
      /* next context */
      ctxt = ctxt->ProNext;
    }
}

/*----------------------------------------------------------------------
  SortFunctionTable generates the function table in ascending
  order.
  The parameter checkEdit will return TRUE if alomst one editing function
  is available in the generated table.
  ----------------------------------------------------------------------*/
static void SortFunctionTable (PtrProCtl ctxt)
{
  char         *ptr;
  int           i, j, index;
  ThotBool      RO;

  /* copy the list of contexts in a large table */
  SortedFunctionTable = (char **) TtaGetMemory (NbFunctions * sizeof (char *));
  FunctionRO = (ThotBool *) TtaGetMemory (NbFunctions * sizeof (ThotBool));
  index = 0;
  EnableEdit = FALSE;
  while (ctxt && index < NbFunctions)
    {
      i = 0;
      while (i < MAX_ENTRIES && ctxt->ProEntries[i].ProName)
        {
          if (ctxt->ProEntries[i].ProEdit)
            {
              /* there is almost one editing function */
              EnableEdit = TRUE;
              FunctionRO[index] = FALSE;
            }
          else
            FunctionRO[index] = TRUE;
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
      RO =  FunctionRO[index];
      FunctionRO[index] =  FunctionRO[i];
      FunctionRO[i] = RO;
    }
}


/*-----------------------------------------------------------------------
  FunctionInProfile returns TRUE if the function is defined in the
  document profile.
  ----------------------------------------------------------------------*/
static ThotBool FunctionInProfile (char *name, PtrProCtl ctxt)
{
  int           i;

  while (ctxt)
    {
      i = 0;
      while (i < MAX_ENTRIES && ctxt->ProEntries[i].ProName)
        {
          if (ctxt->ProEntries[i].ProIsModule)
            {
              if (FunctionInProfile (name, ctxt->ProEntries[i].ProSubModule))
                /* stop if the function is founs this sub-module */
                return TRUE;
            }
          else if (!strcmp (name, ctxt->ProEntries[i].ProName))
            {
              /* the function is declared in that document profile */
              return TRUE;
            }
          /* next entry */
          i++;
        }
      /* next context */
      ctxt = ctxt->ProNext;
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  Prof_BelongTable searchs a function in the function table and returns
  TRUE if the function exists.
  ----------------------------------------------------------------------*/
ThotBool Prof_BelongTable (const char *name)
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


/*----------------------------------------------------------------------
  Prof_BelongDoctype searchs a function in the function table.
  Returns:
  -1: the function is unknown
  0: the function must be hidden
  1: the function is accepted
  ----------------------------------------------------------------------*/
int Prof_BelongDoctype (const char *name, int docProfile, ThotBool RO)
{
  int     left, right, middle, i;

  if (NbFunctions == 0 || docProfile == 0 || name == NULL)
    /* no specific profile */
    return -1;

  /* Dichotomic search */
  left = middle = 0;
  right = NbFunctions - 1;
  if (!strcmp (name, "CreateRuby") && docProfile == L_Strict)
    /* refused */
    return 0;
  if (!strcmp (name, "TtcUndo") ||
      !strcmp (name, "TtcRedo") ||
      !strcmp (name, "StopTransfer"))
    // don't change the current status
    return -1;
  if (!strcmp (name, "TtcCopySelection"))
    return 1; /* always active */
  if (!strcmp (name, "TtcCutSelection") ||
      !strcmp (name, "TtcDeleteSelection") ||
      !strcmp (name, "ZoomIn") ||
      !strcmp (name, "ZoomOut") ||
      !strcmp (name, "ZoomNormal") ||
      !strcmp (name, "SpellCheck") ||
      !strcmp (name, "PasteBuffer"))
    {
      if (RO)
        return 0; /* refused */
      else
        return 1; /* always active */
    }

  while (left <= right)
    {
      middle = (right + left) / 2;
      i = strcmp (SortedFunctionTable[middle], name);
      if (i == 0)
        {
          /* check the profile value */
          if (FunctionMask[middle] == 0)
            /* not defined */
            return -1;
          else if (FunctionMask[middle] & docProfile)
            {
              if (RO && !FunctionRO[middle])
                /* refused */
                return 0;
              else
                /* accepted */
                return 1;
            }
          else
            /* refused */
            return 0;
        }
      else if (i < 0)
        left = middle + 1;
      else
        right = middle - 1;
    }
  return -1;
}

/*-----------------------------------------------------------------------
  Prof_InitTable: Seek the current profile file and init tables
  ----------------------------------------------------------------------*/
void Prof_InitTable (char *profile)
{
  FILE               *profFile;
  char               *ptr, *prof;
  char                buffer[MAX_LENGTH];
  int                 i, j;
  
  /* open the profile file */
  ptr = TtaGetEnvString ("Profiles_File");
  if (ptr && *ptr)
    {
      if (SearchFile (ptr, 2, buffer))
        {
          profFile = TtaReadOpen (buffer);
          /* if the caller didn't specify any profile, we use the one
             given in the registry */
          strcpy (UserProfile, "Editor");
          if (profile && *profile)
            {
              if (strstr (profile, "(editor)"))
                /* avoid to use a dead profile */
                strcpy (UserProfile, "Editor");
              else if (strstr (profile, "browser"))
                /* avoid to use a dead profile */
                strcpy (UserProfile, "Browser");
              else if (strstr (profile, "display"))
                /* avoid to use a dead profile */
                strcpy (UserProfile, "Display");
              else
                strcpy (UserProfile, profile);
            }

          prof = TtaGetEnvString ("CURRENT_PROFILE");
          ptr = NULL;
          if (prof)
            ptr = strstr (prof, "Lite");
          if (ptr)
            {
              strcat (UserProfile, "_Lite");
              i = 0;
              do
                {
                  ptr = strstr (++ptr, "+");
                  i++;
                }
              while (ptr);
              if (i > 1)
                {
                   sprintf (buffer, "%d", i);
                   strcat (UserProfile, buffer);
                }
             }

          /* Fill a profile and module tables */
          while (fgets (ProfileBuff, sizeof (ProfileBuff), profFile))
            {
              SkipAllBlanks (ProfileBuff);
              /* skip comments */
              if (strlen (ProfileBuff) >=2 && ProfileBuff[0] != '#')
                ProcessDefinition (ProfileBuff);
            }
          TtaReadClose (profFile);
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

          GuiList = DLList_Create();
          GuiList->destroyElement = (Container_DestroyElementFunction)TtaFreeMemory;
          
          AddFunctionsAndGuis (UserProfContext, FunctionTable);

          /* generate a sorted list of available functions */
          SortFunctionTable (FunctionTable);
          /* delete the function table */
          if (FunctionTable)
            {
              DeleteTable (FunctionTable, FALSE);
              FunctionTable = NULL;
            }
        }
    }

  /* associate a mask to each function in the table */
  FunctionMask = (int *) TtaGetMemory (NbFunctions * sizeof (int));
  for (i = 0; i < NbFunctions; i++)
    {
      FunctionMask[i] = 0;
      for (j = 0; j < NbDoctypes; j++)
        {
          if (FunctionInProfile (SortedFunctionTable[i],
                                 DoctypeTable->ProEntries[j].ProSubModule))
            {
              if (strstr (DoctypeTable->ProEntries[j].ProName, "Transitional"))
                FunctionMask[i] = (FunctionMask[i] | L_TransitionalValue);
              else if (strstr (DoctypeTable->ProEntries[j].ProName, "Strict"))
                FunctionMask[i] = (FunctionMask[i] | L_StrictValue);
              else if (strstr (DoctypeTable->ProEntries[j].ProName, "Basic"))
                FunctionMask[i] = (FunctionMask[i] | L_BasicValue);
              else if (strstr (DoctypeTable->ProEntries[j].ProName, "1.1"))
                FunctionMask[i] = (FunctionMask[i] | L_StrictValue | L_RubyValue);
              else if (strstr (DoctypeTable->ProEntries[j].ProName, "Ruby"))
                FunctionMask[i] = (FunctionMask[i] | L_RubyValue);
              else if (strstr (DoctypeTable->ProEntries[j].ProName, "MathML"))
                FunctionMask[i] = (FunctionMask[i] | L_MathML);
              else if (strstr (DoctypeTable->ProEntries[j].ProName, "SVG"))
                FunctionMask[i] = (FunctionMask[i] | L_SVG);
              else if (strstr (DoctypeTable->ProEntries[j].ProName, "Annot"))
                FunctionMask[i] = (FunctionMask[i] | L_Annot);
              else if (strstr (DoctypeTable->ProEntries[j].ProName, "Bookmarks"))
                FunctionMask[i] = (FunctionMask[i] | L_Bookmarks);
              else if (strstr (DoctypeTable->ProEntries[j].ProName, "CSS"))
                FunctionMask[i] = (FunctionMask[i] | L_CSS);
              else if (strstr (DoctypeTable->ProEntries[j].ProName, "Text"))
                FunctionMask[i] = (FunctionMask[i] | L_TEXT);
            }
        }
    }
  /* delete the modules table */
  DeleteTable (ModuleTable, TRUE);
  ModuleTable = NULL;
  NbModules = 0;
  /* delete the doctype table */
  DeleteTable (DoctypeTable, TRUE);
  DoctypeTable = NULL;
  NbDoctypes = 0;
}


/*-----------------------------------------------------------------------
  Prof_FreeTable: Remove profile tables
  ----------------------------------------------------------------------*/
void Prof_FreeTable ()
{
  int i;

  /* delete the profiles table */
  DeleteTable (ProfileTable, TRUE);
  ProfileTable = NULL;
  NbProfiles = 0;
  UserProfContext = NULL;
  /* delete the function table */
  for (i = 0; i < NbFunctions; i++)
    TtaFreeMemory (SortedFunctionTable[i]);
  TtaFreeMemory (SortedFunctionTable);
  SortedFunctionTable = NULL;
  TtaFreeMemory (FunctionMask);
  FunctionMask = NULL;
  TtaFreeMemory (FunctionRO);
  FunctionRO = NULL;
  NbFunctions = 0;
  
  /* delete the gui list */
  DLList_Destroy(GuiList);
}

/*----------------------------------------------------------------------
  TtaCanEdit returns TRUE if there is almost one editing function active.
  ----------------------------------------------------------------------*/
ThotBool TtaCanEdit ()
{
  return (EnableEdit);
}

/*----------------------------------------------------------------------
  TtaGetProfileFileName:  Get the text for the profile file name.
  name is a provided buffer of length characters to receive the name.
  ----------------------------------------------------------------------*/
void TtaGetProfileFileName (char *name, int length)
{
  char   *ptr;
  char    buffer[MAX_LENGTH];

  name[0] = EOS;
  ptr = TtaGetEnvString ("Profiles_File");
  if (ptr && *ptr)
    {
      SearchFile (ptr, 2, buffer);
      if (strlen (buffer) < (size_t)length)
        strcpy (name, buffer);
    }
}

/*----------------------------------------------------------------------
  TtaGetProfilesItems:  Get the text for the profile menu items.
  listEntries is a provided list of length pointers.
  Returns the number of items
  ----------------------------------------------------------------------*/
int TtaGetProfilesItems (char **listEntries, int length)
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
ThotBool Prof_ShowSeparator (Menu_Ctl *ptrmenu, int item, char LastItemType)
{  
  return !(LastItemType == 'S' || item == 0);
}


/*-----------------------------------------------------------------------
  Prof_ShowButton : Add a button if the function associated to that button
  belongs to the user profile
  ----------------------------------------------------------------------*/
ThotBool Prof_ShowButton (char *FunctionName)
{
  return (Prof_BelongTable (FunctionName));
}


/*----------------------------------------------------------------------
  Prof_ShowSubMenu : Check if a submenu has to be displayed.
  A submenu musn't be displayed if it contains no entry
  ----------------------------------------------------------------------*/
ThotBool Prof_ShowSubMenu (Menu_Ctl *ptrsubmenu)
{
  int      item    = 0;
  
  if (ptrsubmenu->ItemsNb == 0 || ptrsubmenu == NULL)
    return FALSE;
  while (item < ptrsubmenu->ItemsNb)
    {
      if (ptrsubmenu->ItemsList[item].ItemType != 'S')
        return TRUE;
      item ++;
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  Prof_ShowMenu : Check if a menu has to be displayed. A menu mustn't be
  displayed if it contains no entry.
  -----------------------------------------------------------------------*/
ThotBool Prof_ShowMenu (Menu_Ctl *ptrmenu)
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
      if (ptrmenu->ItemsList[item].ItemType != 'S')
        {
          if (ptrmenu->ItemsList[item].ItemType == 'M')
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


/*----------------------------------------------------------------------
  Prof_ShowGUI : Check if a GUI element has to be displayed.
  -----------------------------------------------------------------------*/
ThotBool Prof_ShowGUI (const char* name)
{
  ThotBool res = FALSE;
  ForwardIterator iter = DLList_GetForwardIterator(GuiList);
  DLListNode      node;
  ITERATOR_FOREACH(iter, DLListNode, node)
    {
      if(!strcmp((const char*)node->elem, name))
        {
          res = TRUE;
          break;
        }
    }
  TtaFreeMemory(iter);
  return res;
}
