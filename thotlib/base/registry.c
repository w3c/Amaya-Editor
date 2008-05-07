/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2007
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * registry.c : common access method for all the configuration
 *              dependancies of Thot.
 *
 * On the Unix platforms, the registry entries are stored in
 * appname.ini file, appname standing for the real application
 * name, e.g. thot.ini or amaya.ini . This file is store in the
 * user home directory or in a system wide configuration directory
 * e.g. /usr/local/thot/bin/thot.ini .
 *
 * On Windows platforms, this is of course based on the standard
 * registry mechanism.
 *
 * Author: D. Veillard (INRIA)
 * Extensions: J. KAHAN (INRIA/W3C)
 *
 */
#ifdef _WX
#include "wx/wx.h"
#if !defined(_UNIX)
#include "wx/utils.h"
#endif /* _UNIX */
#endif /* _WX */

#include "thot_gui.h"
#include "thotkey.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "registry.h"
#include "application.h"
#include "zlib.h"
#include "registry_wx.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "platform_tv.h"
#include "edit_tv.h"

#include "applicationapi_f.h"
#include "fileaccess_f.h"
#include "language_f.h"
#include "memory_f.h"
#include "platform_f.h"
#include "uconvert_f.h"
/* #define DEBUG_REGISTRY enable the Registry debug messages */

/* for Marc.Baudoin@hsc.fr (Marc Baudoin) */
#ifdef _WINDOWS
#include <direct.h>
#include <winbase.h>
#include "winsys.h"
#define WIN_DEF_USERNAME       "default"
#define THOT_INI_FILENAME      "win-thot.rc"
#define WIN_USERS_HOME_DIR     "users"
static char         EnVar[MAX_PATH];	/* thread unsafe! */
#else /* _WINDOWS */
#define THOT_INI_FILENAME      "unix-thot.rc"
#endif /*  _WINDOWS */

#define THOT_RC_FILENAME      "thot.rc"
#define THOT_CONFIG_FILENAME  "config"
#define THOT_BIN_FILENAME     "bin"
#define THOT_LIB_DEFAULTNAME  "thot_lib"
#define MAX_REGISTRY_ENTRIES 100

typedef enum
  {
    REGISTRY_USER,	/* values which can be redefined by the user */
    REGISTRY_SYSTEM,	/* values fetched from the system config     */
    REGISTRY_INSTALL,	/* installation value e.g. THOTDIR, VERSION  */
    REGISTRY_MAX_CATEGORIES
  }
RegistryLevel;

typedef struct struct_RegistryEntry
{
  struct struct_RegistryEntry *next;  /* chaining ! */
  RegistryLevel                level; /* exact level */
  char                        *appli; /* corresponding section */
  char                        *name;  /* name of the entry     */
  char                        *orig;  /* original value (to be saved back) */
  char                        *value; /* user-level value */
}
RegistryEntryBlk , *RegistryEntry;

static int           AppRegistryInitialized = 0;
static int           AppRegistryModified = 0;
static RegistryEntry AppRegistryEntry = NULL;
static char         *AppRegistryEntryAppli = (char*) 0;
static char         *AppNameW;
static char          CurrentDir[MAX_PATH];
static char         *Thot_Dir;
static char          StandardLANG[3] = {EOS,EOS,EOS};

#ifdef COMPILED_IN_THOTDIR
char  UCOMPILED_IN_THOTDIR[MAX_TXT_LEN];
#endif /* COMPILED_IN_THOTDIR */

#ifdef MACHINE
char  UMACHINE[MAX_TXT_LEN];
#endif  /* MACHINE */

PathBuffer execname;
PathBuffer path;

#ifdef _WINDOWS
/* @@why do we need this here? */
/*#undef errno
  int errno = 0;
  int * __cdecl _errno(void)
  {
	return & errno;
  }*/
#endif


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static const char *SkipToEqual (const char *ptr)
{
  while (*ptr != EOS && *ptr != '=' && *ptr != EOL && *ptr != __CR__)
    ptr++;
  return (ptr);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static const char *SkipToEndValue (const char *ptr)
{
  while (*ptr != EOS && *ptr != EOL && *ptr != __CR__)
    ptr++;
  return (ptr);
}

/*----------------------------------------------------------------------
  TtaSkipBlanks skips all spaces, tabs, linefeeds and newlines at the
  beginning of the string and returns the pointer to the new position. 
  ----------------------------------------------------------------------*/
const char *TtaSkipBlanks (const char *ptr)
{
  while (*ptr == SPACE || *ptr == BSPACE || *ptr == EOL ||
         *ptr == TAB || *ptr == __CR__)
    ptr++;
  return (ptr);
}

/*----------------------------------------------------------------------
  TtaIsBlank returns True if the first character is a space, a tab, a
  linefeed or a newline.
  ----------------------------------------------------------------------*/
ThotBool TtaIsBlank (const char *ptr)
{
  if (*ptr == SPACE || *ptr == BSPACE || *ptr == EOL ||
      *ptr == TAB || *ptr == __CR__)
    return (TRUE);
  else
    return (FALSE);
}


/*----------------------------------------------------------------------
  DoVariableSubstitution : do the substitution on an input
  string of all $(xxx) references by the values of xxx.
  and return a modified output string.
  ----------------------------------------------------------------------*/
static void DoVariableSubstitution (const char *input, int i_len, char *output,
                                    int o_len)
{
  const char *cour = input;
  const char *base = input;
  char *res  = output;
  char *value;
  char  name[80];
  char *nameptr;

#define CHECK_OVERFLOW (cour - input > i_len || res - output >= o_len - 1)
  while (*cour)
    {
      if CHECK_OVERFLOW
        break;
      if (*cour != '$')
        {
          *res++ = *cour++;
          continue;
        }
      base = cour;		/* save the position to the $ */
      cour++;
      if CHECK_OVERFLOW
        break;
      if (*cour != '(')
        {
          *res++ = '$';
          if CHECK_OVERFLOW
            break;
          *res++ = *cour++;
          continue;
        }

      /* Ok, that the beginning of a variable name ... */
      base += 2;		/* skip the $(  header */
      nameptr = name;
      do
        {
          *nameptr = *cour;
          cour++;
          nameptr++;
          if CHECK_OVERFLOW
            break;
        }
      while (*cour != ')' && !TtaIsBlank (cour));
      if CHECK_OVERFLOW
        break;

      *nameptr = EOS;
      if (*cour != ')')
        fprintf (stderr, "invalid variable name %s in %s\n", base, THOT_INI_FILENAME);
      /* We are ready to fetch the base value from the Registry */
      value = TtaGetEnvString (name+1);
      if (value == NULL)
        {
          fprintf (stderr, "%s referencing undefined variable %s\n", THOT_INI_FILENAME, name);
          cour++;
          if CHECK_OVERFLOW
            break;
          continue;
        }
      while (*value)
        {
          *res++ = *value++;
          if CHECK_OVERFLOW
            break;
        }
      cour++;
      if CHECK_OVERFLOW
        break;
    }
  if CHECK_OVERFLOW
    fprintf (stderr, "DoVariableSubstitution : Overflow on \"%s\"\n", input);
  *res = EOS;
}


/*----------------------------------------------------------------------
  NewRegisterEntry : add a fresh new entry in the Register.
  ----------------------------------------------------------------------*/
static int NewRegisterEntry (const char *appli, const char *name, const char *value,
                             RegistryLevel level)
{
  char        resu[2000];
  RegistryEntry cour, ptr, previous;

  if (AppRegistryInitialized == 0)
    return (-1);

  /*
   * do not register volatile informations like the
   * home dir or the current working directory.
   */
  if (!strcasecmp (name, "pwd"))
    return (0);
  if (!strcasecmp (name, "home"))
    return (0);
  if (!strcasecmp (name, "user"))
    return (0);
  if (!strcasecmp (name, "cwd"))
    return (0);

  /*
   * substitute the $(xxxx) with their values.
   */
  DoVariableSubstitution (value, strlen (value), resu, sizeof (resu) / sizeof (char));

  /*
   * allocate an entry, fill it and chain it.
   */

  cour = (RegistryEntry) TtaGetMemory (sizeof (RegistryEntryBlk));
  if (cour == NULL)
    return (-1);
  cour->appli = TtaStrdup (appli);
  cour->name  = TtaStrdup (name);
  cour->orig  = TtaStrdup (value);
  cour->value = TtaStrdup (resu);
  cour->level = level;

  /*
   * sort the new entry according to its level
   */

  if (!AppRegistryEntry)
    /* it's the first entry */
    {
      cour->next = NULL;
      AppRegistryEntry = cour;
    }
  else
    {
      /* find the first level entry */
      ptr = AppRegistryEntry;
      previous = AppRegistryEntry;
      while (ptr && ptr->level < level)
        {
          previous = ptr;
          ptr = ptr->next;

        }
      if (!ptr)
        /* insert it at the end*/
        {
          cour->next = NULL;
          previous->next = cour;
        }
      else if (level <= ptr->level)
        /* insert cour before ptr */
        {
          cour->next = ptr;
          if (AppRegistryEntry == ptr)
            /* it's the first item in the list */
            AppRegistryEntry = cour;
          else
            previous->next = cour;
        }
      else if (level > ptr->level)
        /* insert cour after ptr */
        {
          cour->next = ptr ->next;
          ptr->next = cour;
        }
    }
  AppRegistryModified++;
  return (0);
}


/*----------------------------------------------------------------------
  AddRegisterEntry : add an entry in the Register, we first check
  that it doesn't already exist especially if the value is empty.
  ----------------------------------------------------------------------*/
static int AddRegisterEntry (const char *appli, const char *name, const char *value,
                             RegistryLevel level, int overwrite)
{
  char          resu[2000];
  RegistryEntry cour;

  if (AppRegistryInitialized == 0)
    return (-1);

#ifdef DEBUG_REGISTRY
  fprintf (stderr, "AddRegisterEntry(\"%s\",\"%s\",\"%s\", %d, %d)\n",
           appli, name, value, level, overwrite);
#endif

  /*
   * Lookup in the application defaults.
   */
  cour = AppRegistryEntry;
  while (cour != NULL)
    {
      if (!strcmp (cour->name, name))
        {
          /* Cannot superseed and INSTALL value */
          if (cour->level == REGISTRY_INSTALL)
            return (0);
          else if (!strcasecmp (cour->appli, appli) && cour->level == level)
            break;
        }
      cour = cour->next;
    }

  if (cour)
    {
      /* there is aleady an entry */
      if (cour->value && !overwrite)
        return (0);

      DoVariableSubstitution (value, strlen (value), resu, sizeof (resu) / sizeof (char));
      if (cour->value)
        TtaFreeMemory (cour->value);
      if (cour->orig)
        TtaFreeMemory (cour->orig);
      cour->orig = TtaStrdup (value);
      cour->value = TtaStrdup (resu);
      AppRegistryModified++;
    }
  else
    {
      /*
       * If the value is empty, we add it only if it's not present
       * in the thot library section.
       */
      if (!overwrite && (value == NULL || *value == EOS))
        {
          cour = AppRegistryEntry;
          while (cour)
            {
              if ((!strcasecmp (cour->appli, THOT_LIB_DEFAULTNAME)) &&
                  (!strcmp (cour->name, name)))
                return (0);
              cour = cour->next;
            }
        }

      return NewRegisterEntry (appli, name, value, level);
    }
  return (0);
}

/*----------------------------------------------------------------------
  PrintEnv : print the Registry to an open File.
  ----------------------------------------------------------------------*/
static void PrintEnv (FILE *output)
{
  RegistryEntry       cour, next;
  
  if (AppRegistryInitialized == 0)
    return;

  cour = AppRegistryEntry;
  while (cour != NULL)
    {
      /* don't save any system entry */
      if (!strcasecmp (cour->appli, "System"))
        {
          cour = cour->next;
          continue;
        }

      /* print out the section */
      fprintf (output, "[%s]\n", cour->appli);
      /* add all the entries under the same appli name */
      if (cour->level == REGISTRY_USER 
          && strcasecmp (cour->name, "APP_TMPDIR")
          && strcasecmp (cour->name, "APP_HOME"))
        fprintf (output, "%s=%s\n", cour->name, cour->orig);
      next = cour->next;
      while (next != NULL && !strcasecmp (next->appli, cour->appli))
        {
          if (next->level == REGISTRY_USER
              && strcasecmp (next->name, "APP_TMPDIR")
              && strcasecmp (next->name, "APP_HOME"))
            fprintf (output, "%s=%s\n", next->name, next->orig);
          next = next->next;
        }
      cour = next;
      fprintf (output, "\n");
    }
}

/*----------------------------------------------------------------------
  SortEnv : sort the Registry by application and name entries.
  ----------------------------------------------------------------------*/
static void SortEnv (void)
{
  RegistryEntry       prev, cour, tmp;
  RegistryEntry      *start;
  int                 cmp;

  if (AppRegistryInitialized == 0)
    return;
#ifdef DEBUG_REGISTRY
  fprintf (stderr, "SortEnv()\n");
#endif

  start = &AppRegistryEntry;
  while (*start != NULL)
    {
      prev = *start;
      cour = prev->next;
      if (cour == NULL)
        return;
      while (cour != NULL)
        {
          /* sorting order : First by appli name, then by entry name */
          cmp = strcasecmp (cour->appli, (*start)->appli);
          if (cmp <= 0 ||
              (cmp == 0 &&
               (strcasecmp (cour->name, (*start)->name) < 0)))
            {
              /* swap *start and cour */
              prev->next = *start;
              *start = cour;
              tmp = cour->next;
              cour->next = prev->next->next;
              prev->next->next = tmp;
              cour = prev->next;
            }

          /* next in the list */
          prev = cour;
          if (cour != NULL)
            cour = cour->next;
        }
      start = &((*start)->next);
    }
}

/*----------------------------------------------------------------------
  TtaGetEnvInt : read the integer value associated to an 
  environment string.
  Returns TRUE if the env variables exists or FALSE if it isn't the case.
  ----------------------------------------------------------------------*/
ThotBool TtaGetEnvInt (const char *name, int *value)
{
  char *strptr;

  if (!name || *name == EOS)
    {
      *value = 0;
      return FALSE;
    }
  strptr = TtaGetEnvString (name);
  /* the name entry doesn't exist */
  if (!strptr || *strptr == EOS)
    {
      *value = 0;
      return FALSE;
    }
  /* make the convertion */
  *value = atoi (strptr);
  return TRUE;
}

/*----------------------------------------------------------------------
  TtaGetEnvBoolean : read the ThotBool value associated to an 
  environment string.
  Returns TRUE if the env variables exists or FALSE if it isn't the case.
  ----------------------------------------------------------------------*/
ThotBool TtaGetEnvBoolean (const char *name, ThotBool *value)
{
  char *strptr;

  if (!name || *name == EOS)
    {
      *value = FALSE;
      return FALSE;
    }

  strptr = TtaGetEnvString (name);

  /* the name entry doesn't exist */
  if (!strptr || *strptr == EOS)
    {
      *value = FALSE;
      return FALSE;
    }

  /* make the convertion */
  if ( strcasecmp (strptr, "yes"))
    *value = FALSE;
  else
    *value = TRUE;

  return TRUE;
}


/*----------------------------------------------------------------------
  TtaGetEnvString : read the value associated to an environment string
  if not present return NULL.
  ----------------------------------------------------------------------*/
char *TtaGetEnvString (const char *name)
{
  RegistryEntry  cour;
  char          *value;

  if (AppRegistryInitialized == 0)
    return getenv (name);

  /* appname allows to get the application name */
  if (!strcasecmp ("appname", name))
    return (AppRegistryEntryAppli);

  if (!strcasecmp (name, "cwd") || !strcasecmp (name, "pwd"))
    return (getcwd (&CurrentDir[0], sizeof(CurrentDir)));

  // shortcut to get the current language
  if (!strcmp (name, "LANG") && StandardLANG[0] != EOS)
    {
      return (StandardLANG);
    }

  /* first lookup in the System defaults */
  cour = AppRegistryEntry;
  while (cour != NULL)
    {
      if (!strcasecmp (cour->appli, "System") &&
          !strcmp (cour->name, name) && cour->value[0] != EOS)
        {
#ifdef DEBUG_REGISTRY
          fprintf (stderr, "TtaGetEnvString(\"%s\") = %s\n", name, cour->value);
#endif
          return (cour->value);
        }
      cour = cour->next;
    }

  /* then lookup in the application user preferences */
  cour = AppRegistryEntry;
  while (cour != NULL)
    {
      if (!strcasecmp (cour->appli, AppRegistryEntryAppli) && 
          !strcmp (cour->name, name) && cour->value[0] != EOS && 
          cour->level == REGISTRY_USER)
        {
#ifdef DEBUG_REGISTRY
          fprintf (stderr, "TtaGetEnvString(\"%s\") = %s\n", name, cour->value);
#endif
          if (!strcmp (name, "LANG"))
            {
              strncpy (StandardLANG, (char *)cour->value, 2);
              StandardLANG[2] = EOS;
            }
          return (cour->value);
        }
      cour = cour->next;
    }

  /* then lookup in the application defaults */
#ifdef _WX
  if (!strcmp (name, "LANG"))
    {
      int lang = TtaGetSystemLanguage();
      // get the platform language
      strncpy (StandardLANG, TtaGetISO639Code(lang), 2);
      StandardLANG[2] = EOS;
      return (StandardLANG);
    }
#endif /* _WX */
  cour = AppRegistryEntry;
  while (cour != NULL)
    {
      if (!strcasecmp (cour->appli, AppRegistryEntryAppli) && 
          !strcmp (cour->name, name) && cour->value[0] != EOS && 
          cour->level == REGISTRY_SYSTEM)
        {
#ifdef DEBUG_REGISTRY
          fprintf (stderr, "TtaGetEnvString(\"%s\") = %s\n", name, cour->value);
#endif
          return (cour->value);
        }
      cour = cour->next;
    }

  /* then lookup in the Thot library defaults */
  cour = AppRegistryEntry;
  while (cour != NULL)
    {
      if (!strcasecmp (cour->appli, THOT_LIB_DEFAULTNAME) &&
          !strcmp (cour->name, name) && cour->value[0] != EOS)
        {
#ifdef DEBUG_REGISTRY
          fprintf (stderr, "TtaGetEnvString(\"%s\") = %s\n", name, cour->value);
#endif
          return (cour->value);
        }
      cour = cour->next;
    }

  /*
   * If still not found, look in the environment variables.
   * Hopefully this will be stored to the user registry
   * next time it will be saved.
   */
  if (!strcmp (name, "LANG"))
    {
     strcpy (StandardLANG, "en");
     return (StandardLANG);
    }

  value = getenv (name);
  if (value == NULL)
    TtaSetEnvString (name, "", FALSE); 
  else
    TtaSetEnvString (name, value, FALSE);
  
#ifdef DEBUG_REGISTRY
  fprintf (stderr, "TtaGetEnvString(\"%s\") = %s\n", name, value);
#endif
  return (value);
}

/*----------------------------------------------------------------------
  TtaClearEnvString : clears the value associated with an environment
  string, in the user registry.
  ----------------------------------------------------------------------*/
void TtaClearEnvString (const char *name)
{
  AddRegisterEntry (AppRegistryEntryAppli, name, "", REGISTRY_USER, TRUE);
}

/*----------------------------------------------------------------------
  TtaSetEnvInt : set the value associated to an environment string,
  for the current application.
  ----------------------------------------------------------------------*/
void TtaSetEnvInt (const char *name, int value, int overwrite)
{
  /* hardcoded so that the biggest integer value has 5 digits: 65535 */
  char ptr[6];
  int    r_val;

  r_val = value % 65537;
  sprintf (ptr, "%d", r_val);
  AddRegisterEntry (AppRegistryEntryAppli, name, ptr, REGISTRY_USER, overwrite);
}

/*----------------------------------------------------------------------
  TtaSetEnvBoolean : set the value associated to an environment string,
  for the current application.
  ----------------------------------------------------------------------*/
void TtaSetEnvBoolean (const char *name, ThotBool value, int overwrite)
{
  const char *ptr;

  if (value)
    ptr = "yes";
  else
    ptr = "no";
  AddRegisterEntry (AppRegistryEntryAppli, name, ptr, REGISTRY_USER, overwrite);
}

/*----------------------------------------------------------------------
  TtaSetEnvString : set the value associated to an environment string,
  for the current application.
  ----------------------------------------------------------------------*/
void TtaSetEnvString (const char *name, const char *value, int overwrite)
{
  if (!strcmp (name, "LANG") && value && overwrite)
  {
    strncpy (StandardLANG, value, 2);
    StandardLANG[2] = EOS;
  }
  if(value)
    AddRegisterEntry (AppRegistryEntryAppli, name, value, REGISTRY_USER, overwrite);
  else
    AddRegisterEntry (AppRegistryEntryAppli, name, "", REGISTRY_USER, overwrite);
}

/*----------------------------------------------------------------------
  TtaSetDefEnvString : set the default value associated to an environment
  string, for the current application.
  ----------------------------------------------------------------------*/
void TtaSetDefEnvString (const char *name, const char *value, int overwrite)
{
  /* make sure that value isn't NULL */
  if (value)
    AddRegisterEntry (AppRegistryEntryAppli, name, value, REGISTRY_SYSTEM, overwrite);
  else
    AddRegisterEntry (AppRegistryEntryAppli, name, "", REGISTRY_SYSTEM, overwrite);
}

/*----------------------------------------------------------------------
  TtaGetDefEnvInt : read the default integer value associated to an 
  environment string.
  Returns TRUE if the env variables exists or FALSE if it isn't the case.
  ----------------------------------------------------------------------*/
ThotBool TtaGetDefEnvInt (const char *name, int *value)
{
  char *strptr;

  if (!name || *name == EOS)
    {
      *value = 0;
      return FALSE;
    }
  strptr = TtaGetDefEnvString (name);
  /* the name entry doesn't exist */
  if (!strptr || *strptr == EOS)
    {
      *value = 0;
      return FALSE;
    }
  /* make the convertion */
  *value = atoi (strptr);
  return TRUE;
}

/*----------------------------------------------------------------------
  TtaGetDefEnvBoolean : read the ThotBool value associated to an 
  environment string.
  Returns TRUE if the env variables exists or FALSE if it isn't the case.
  ----------------------------------------------------------------------*/
ThotBool TtaGetDefEnvBoolean (const char *name, ThotBool *value)
{
  char   *strptr;

  if (!name || *name == EOS)
    {
      *value = FALSE;
      return FALSE;
    }

  strptr = TtaGetDefEnvString (name);

  /* the name entry doesn't exist */
  if (!strptr || *strptr == EOS)
    {
      *value = FALSE;
      return FALSE;
    }

  /* make the convertion */
  if (strcasecmp (strptr, "yes"))
    *value = FALSE;
  else
    *value = TRUE;

  return TRUE;
}

/*----------------------------------------------------------------------
  TtaGetDefEnvString : read the default value associated to an 
  environment string. If not present, returns NULL.
  ----------------------------------------------------------------------*/
char *TtaGetDefEnvString (const char *name)
{
  RegistryEntry  cour;
  char          *value;

  if (AppRegistryInitialized == 0)
    return getenv (name);

  /* appname allows to get the application name */
  if (!strcasecmp ("appname", name))
    return(AppRegistryEntryAppli);

  if (!strcasecmp (name, "cwd") || !strcasecmp (name, "pwd"))
    return (getcwd (&CurrentDir[0], sizeof(CurrentDir)));

#ifdef _WX
  if (!strcmp (name, "LANG"))
    {
      int lang = TtaGetSystemLanguage();
      // get the platform language
      strncpy (StandardLANG, TtaGetISO639Code(lang), 2);
      StandardLANG[2] = EOS;
      return (StandardLANG);
    }
#endif /* _WX */

  /* First lookup in the System defaults */
  cour = AppRegistryEntry;
  while (cour != NULL)
    {
      if (!strcasecmp (cour->appli, "System") && !strcmp (cour->name, name) &&
          cour->level == REGISTRY_SYSTEM && cour->value[0] != EOS)
        {
#ifdef DEBUG_REGISTRY
          fprintf (stderr, "TtaGetDefEnvString(\"%s\") = %s\n", name, cour->value);
#endif
          return (cour->value);
        }
      cour = cour->next;
    }

  /* Then lookup in the application defaults */
  cour = AppRegistryEntry;
  while (cour != NULL)
    {
      if (!strcasecmp (cour->appli, AppRegistryEntryAppli) &&
          !strcmp (cour->name, name) &&
          cour->level == REGISTRY_SYSTEM && cour->value[0] != EOS)
        {
#ifdef DEBUG_REGISTRY
          fprintf (stderr, "TtaGetDefEnvString(\"%s\") = %s\n", name, cour->value);
#endif
          return (cour->value);
        }
      cour = cour->next;
    }
  
  /* Then lookup in the Thot library defaults */
  cour = AppRegistryEntry;
  while (cour != NULL)
    {
      if (!strcasecmp (cour->appli, THOT_LIB_DEFAULTNAME) &&
          !strcmp (cour->name, name) &&
          cour->level == REGISTRY_SYSTEM && cour->value[0] != EOS)
        {
#ifdef DEBUG_REGISTRY
          fprintf (stderr, "TtaGetDefEnvString(\"%s\") = %s\n", name, cour->value);
#endif
          return (cour->value);
        }
      cour = cour->next;
    }

  /*
   * If still not found, look in the environment variables.
   * Hopefully this will be stored to the user registry
   * next time it will be saved.
   */
  value = getenv (name);
  
#ifdef DEBUG_REGISTRY
  fprintf (stderr, "TtaGetDefEnvString(\"%s\") = %s\n", name, value);
#endif
  return (value);
}

/*----------------------------------------------------------------------
  IsThotDir : Check whether the given string is the THOTDIR value.    
  The heuristic is to find a subdir named "config" and containing 
  the registry file.                                              
  ----------------------------------------------------------------------*/
static int IsThotDir (CONST char *path)
{
  char           filename[MAX_PATH];

  if (path == NULL)
    return (0);
  strcpy (filename, path);
  strcat (filename, DIR_STR);
  strcat (filename, THOT_CONFIG_FILENAME);
  strcat (filename, DIR_STR);
  strcat (filename, THOT_INI_FILENAME);
#ifdef DEBUG_REGISTRY
  fprintf (stderr, "TtaFileExist (%s)\n", filename);
#endif
  if (TtaFileExist (filename))
    {
#ifdef DEBUG_REGISTRY
      fprintf (stderr, "IsThotDir(%s) : True\n", path);
#endif
      return (1);
    }
  else
    {
#ifdef DEBUG_REGISTRY
      fprintf (stderr, "IsThotDir(%s) : False\n", path);
#endif
      return (0);
    }
}


#ifdef _WINGUI
#ifndef __GNUC__
/*----------------------------------------------------------------------
  WINReg_get - simulates getenv in the WIN32 registry
  looks for <env> in HKEY_CURRENT_USER\Software\Amaya\<var>
  ----------------------------------------------------------------------*/
static char *WINReg_get (CONST char *env)
{
  char           textKey[MAX_PATH];
  HKEY           hKey;
  DWORD          type;
  LONG           success;
  DWORD          retLen = sizeof (EnVar);

  sprintf (textKey, "Software\\%s\\%s", AppNameW, env);    
  success = RegOpenKeyEx (HKEY_CURRENT_USER, textKey, 0, KEY_ALL_ACCESS, &hKey);
  if (success == ERROR_SUCCESS)
    {
      success = RegQueryValueEx (hKey, NULL, NULL, &type, (LPVOID)EnVar, &retLen);
      RegCloseKey (hKey);
    }
  return (success == ERROR_SUCCESS) ? EnVar : NULL;
}

/*----------------------------------------------------------------------
  WINReg_set - stores a value in the WIN32 registry           
   
  stores <key, value> in                                                 
  HKEY_CURRENT_USER\Software\app-name\<key>                                      
  ----------------------------------------------------------------------*/
static ThotBool WINReg_set (CONST char *key, CONST char *value)
{
  char             textKey[MAX_PATH];
  HKEY             hKey;
  LONG             success;
  char             protValue[MAX_PATH];
  DWORD            protValueLen = sizeof (protValue);
  DWORD            dwDisposition;
 
  /* protect against values bigger than what we can write in
     the registry */
  strncpy (protValue, value, protValueLen - 1);
  protValue[protValueLen-1] = EOS;
  sprintf (textKey,"Software\\%s\\%s", AppNameW, key);
  success = RegCreateKeyEx (HKEY_CURRENT_USER, textKey, 0,
                            "", REG_OPTION_VOLATILE, KEY_ALL_ACCESS,
                            NULL, &hKey, &dwDisposition);  
  if (success == ERROR_SUCCESS)
    /* create the userBase entry */
    {
      success = RegSetValueEx (hKey, NULL, 0, REG_SZ, (LPVOID)protValue,
                               protValueLen);
      RegCloseKey (hKey);
    }
  return (success == ERROR_SUCCESS) ? TRUE : FALSE;
}

/*----------------------------------------------------------------------
  WINIni_get - simulates getenv in the Windows/Amaya.ini file     
  ----------------------------------------------------------------------*/
static char *WINIni_get (CONST char *env)
{
  DWORD               res;

  res = GetPrivateProfileString ("Amaya", env, "", EnVar, sizeof (EnVar),
                                 "Amaya.ini");
  return res ? EnVar : NULL;
}
#endif
#endif /* _WINGUI */

/*----------------------------------------------------------------------
  TtaSaveAppRegistry : Save the Registry in the THOT_RC_FILENAME located
  in the user's directory.
  ----------------------------------------------------------------------*/
void TtaSaveAppRegistry ()
{
  char              filename[MAX_PATH];
  char             *app_home;
  FILE             *output;

  if (!AppRegistryInitialized)
    return;
  if (!AppRegistryModified)
    return;
  app_home = TtaGetEnvString ("APP_HOME");
  if (app_home != NULL)
    sprintf (filename, "%s%c%s", app_home, DIR_SEP, THOT_RC_FILENAME);
  else
    {
      fprintf (stderr, "Cannot save Registry no APP_HOME dir\n");
      return;
    }
  output = TtaWriteOpen (filename);
  if (output == NULL)
    {
      fprintf (stderr, "Cannot save Registry to %s :\n", filename);
      return;
    }
  SortEnv ();
  PrintEnv (output);
  AppRegistryModified = 0;
  TtaWriteClose (output);
}


/*----------------------------------------------------------------------
  ImportRegistryFile : import a registry file.
  ----------------------------------------------------------------------*/
static void ImportRegistryFile (char *filename, RegistryLevel level)
{
  FILE     *input;
  char     *str; 
  char     *base;
  char      string[MAX_LENGTH];
  char      appli[MAX_LENGTH];
  char     *name;
  char     *value;

  strcpy (appli, THOT_LIB_DEFAULTNAME);
  input = TtaReadOpen (filename);
  if (input == NULL)
    {
      fprintf (stderr, "Cannot read Registry from %s :\n", filename);
      return;
    }

  while (1)
    {
      /* read one line in string buffer */
      if (fgets (&string[0], sizeof (string) - 1, input) == NULL)
        break;

      str = string;
      str = (char*)TtaSkipBlanks (str);
      string[sizeof (string) - 1] = EOS;
      /* Comment starts with a semicolumn */
      if (*str == ';' || *str == '#')
        continue;
      /* sections are indicated between brackets, e.g. [amaya] */
      if (*str == '[')
        {
          str++;
          str = (char*)TtaSkipBlanks (str);
          base = str;
          while ((*str != EOS) && (*str != ']'))
            str++;
          if (*str == EOS)
            {
              fprintf (stderr, "Registry %s corrupted :\n\t\"%s\"\n", filename, string);
              continue;
            }
          *str = EOS;
          strcpy (&appli[0], base);
#ifdef DEBUG_REGISTRY
          fprintf (stderr, "TtaInitializeAppRegistry section [%s]\n", appli);
#endif
          continue;
        }

      /* entries have the following form : name=value */
      name = str;
      str = (char*)SkipToEqual (str);
      if (*str != '=')
        continue;
      *str++ = EOS;
      str = (char*)TtaSkipBlanks (str);
      value = str;
      str = (char*)SkipToEndValue (str);
      *str = EOS;
      AddRegisterEntry (appli, name, value, level, TRUE);
    }
  TtaReadClose (input);
}


/*----------------------------------------------------------------------
  InitEnviron : initialize the standard environment (i.e global	
  variables) with values stored in the registry.			
  ----------------------------------------------------------------------*/
static void InitEnviron ()
{
  char *pT;
  char *Thot_Sys_Sch;
  char *Thot_Sch;

  /* default values for various global variables */
  FirstCreation = FALSE;
  DocBackUpInterval = 0;
  pT = TtaGetEnvString ("AUTO_SAVE");
  if (pT != NULL)
    {
      DocBackUpInterval = atoi (pT);
      if (DocBackUpInterval <= 0)
        DocBackUpInterval = 0;
    }
  HighlightBoxErrors = FALSE;
  InsertionLevels = 4;

  /* browsing default values */
#ifndef _WINDOWS
  TtaSetDefEnvString ("DOUBLECLICKDELAY", "500", TRUE);
  pT = TtaGetEnvString ("DOUBLECLICKDELAY");
  if (pT != NULL)
    DoubleClickDelay = atoi (pT);
  else 
    DoubleClickDelay = 500;
#endif /* _WINDOWS */
  TtaSetDefEnvString ("OPENING_LOCATION", "0", TRUE);
  TtaSetDefEnvString ("SHOW_CONFIRM_CLOSE_TAB", "yes", TRUE);

  /* The base of the Thot directory */
  Thot_Dir = TtaGetEnvString ("THOTDIR");
  if (Thot_Dir == NULL)
    fprintf (stderr, "missing environment variable THOTDIR\n");

  /* The predefined path to documents */
  pT = TtaGetEnvString ("THOTDOC");
  if (pT == NULL)
    DocumentPath[0] = EOS; 
  else
    strncpy (DocumentPath, pT, MAX_PATH);

  /* Read the schemas Paths */
  Thot_Sch = TtaGetEnvString ("THOTSCH");
  Thot_Sys_Sch = TtaGetEnvString ("THOTSYSSCH");

  /* set up SchemaPath accordingly */
  if ((Thot_Sch != NULL) && (Thot_Sys_Sch != NULL))
    {
      strncpy (SchemaPath, Thot_Sch, MAX_PATH);
      strcat (SchemaPath, PATH_STR);
      strcat (SchemaPath, Thot_Sys_Sch);
    }
  else if (Thot_Sch != NULL)
    strncpy (SchemaPath, Thot_Sch, MAX_PATH);
  else if (Thot_Sys_Sch != NULL)
    strncpy (SchemaPath, Thot_Sys_Sch, MAX_PATH);
  else
    SchemaPath[0] = EOS;

  /* set up the default values common to all the thotlib applications */
  TtaSetDefEnvString ("LANG", "en-us", FALSE);
  TtaSetDefEnvString ("ZOOM", "0", FALSE);
  TtaSetDefEnvString ("TOOLTIPDELAY", "500", FALSE);
  TtaSetDefEnvString ("FontMenuSize", "12", FALSE);
  TtaSetDefEnvString ("ForegroundColor", "Black", FALSE);
  TtaSetDefEnvString ("BackgroundColor", "White", FALSE);
  TtaSetDefEnvString ("FgSelectColor", "White", FALSE);
  TtaSetDefEnvString ("BgSelectColor", "#008BB2", FALSE);
  TtaSetDefEnvString ("MenuFgColor", "Black", FALSE);
  TtaSetDefEnvString ("MenuBgColor", "Grey", FALSE);
  pT = TtaGetEnvString ("APP_TMPDIR");
  /* APP_TMPDIR not defined for compilers */
  if (!pT)
#ifdef _WINDOWS
    pT = TtaStrdup("c:\temp");
#else /* _WINDOWS */
  pT = TtaStrdup("/tmp");
#endif /* _WINDOWS */
  /* create the TMPDIR dir if it doesn't exist */
  if (!TtaMakeDirectory (pT))
    {
      fprintf (stderr, "Couldn't create directory %s\n", pT);
      ThotExit (1);
    }
}

/*----------------------------------------------------------------------
  TtaInitializeAppRegistry : initialize the Registry, the only argument
  given is a copy of the argv[0] received from the main().
  From this, we can deduce the installation directory of the programm,
  (using the PATH environment value if necessary) and the application
  name.
  We load the ressources file from the installation directory and
  the specific user values from the user HOME dir.
  ----------------------------------------------------------------------*/
void TtaInitializeAppRegistry (char *appArgv0)
{
  char        app_home[MAX_PATH] = "";
  char        filename[MAX_PATH] = "";
  char       *my_path;
  char       *dir_end = NULL;
  const char *appName = "amaya";
  char       *ptr;
#ifdef _WINGUI
  /* name in Windows NT 4 is 20 chars */
  char        username[MAX_LENGTH];
  char        windir[MAX_PATH+1];
  DWORD       dwSize;
  ThotBool    status;
  char       *ptr2, *ptr3;
#endif /* _WINGUI */
#ifdef _UNIX
  struct stat stat_buf;
  char        c_execname[MAX_LENGTH];
  char        c_filename[MAX_LENGTH];
  char       *c_end;
#endif /* _UNIX */
#ifdef _MACOS
#define       MACOSX_LIBRARY      "Library"
#define       MACOSX_PREFERENCES  "Application Support"
  char        realexecname[MAX_LENGTH];
  char        app_home_mac[MAX_PATH];
  char        app_home_orig[MAX_PATH];
  char        filename_orig[MAX_PATH];
#endif /* _MACOS */
  int         execname_len;
  int         len, round;
  ThotBool    found, ok, amaya_exe = TRUE;
  
#ifdef _WINDOWS
  _fmode = _O_BINARY;
#endif /* _WINDOWS */
  if (AppRegistryInitialized != 0)
    return;
  AppRegistryInitialized++;

  /* Sanity check on the argument given. An error here should be
   * detected by programmers, since it's a application coding error.
   */
  if (appArgv0 == NULL || *appArgv0 == EOS)
    {
#ifdef _WINGUI
      MessageBox (NULL, "TtaInitializeAppRegistry called with invalid argv[0] value", "Amaya", MB_OK);
#else  /* _WINGUI */
      fprintf (stderr, "TtaInitializeAppRegistry called with invalid argv[0] value\n");
#endif /* _WINGUI */
       ThotExit (1);
    }
  /*
   * We are looking for the absolute pathname to the binary of the
   * application.
   *
   * First case, the argv[0] indicate that it's an absolute path name.
   * i.e. start with / on unixes or \ or ?:\ on Windows.
   */
#if defined(_WX) && defined(_MACOS)
  /* for MACOS, 'getcws' returns the path to the current bundle if it exists */
  /* In this case, we append the real directory to the path */
  getcwd (&execname[0], sizeof (execname) / sizeof (char));
  strcat (execname, DIR_STR);
  strcat (execname, "amaya.app");
  strcat (execname, DIR_STR);
  strcat (execname, "Contents");
  strcat (execname, DIR_STR);
  strcat (execname, "MacOS");
  strcat (execname, DIR_STR);
  strcpy (realexecname, execname);
  strcat (realexecname, appArgv0);
  my_path = getenv("PATH");
#endif /* _MACOS & _WX */
#ifdef _WINDOWS
  if (appArgv0[0] == DIR_SEP || (appArgv0[1] == ':' && appArgv0[2] == DIR_SEP))
    strncpy (&execname[0], appArgv0, sizeof (execname) / sizeof (char));
#else  /* _WINDOWS */
  if (appArgv0[0] == DIR_SEP)
    strncpy (&execname[0], appArgv0, sizeof (execname) / sizeof (char));
#endif /* _WINDOWS */
  /*
   * second case, the argv[0] indicate a relative path name.
   * The exec name is obtained by appending the current directory.
   */
  else if (TtaFileExist (appArgv0))
    {
      getcwd (&execname[0], sizeof (execname) / sizeof (char));
      strcat (execname, DIR_STR);
      strcat (execname, appArgv0);
    }
#ifdef _MACOS
  else if (TtaFileExist (realexecname))
   	strcpy (execname, realexecname);
#endif /* _MACOS */
  else
    {
      /*
       * Last case, we were just given the application name.
       * Use the PATH environment variable to search the exact binary location.
       */
      my_path = getenv("PATH");
      if (my_path == NULL)
        {
          fprintf (stderr, "TtaInitializeAppRegistry cannot found PATH environment\n");
          ThotExit (1);
        }
      /*
       * make our own copy of the string, in order to preserve the
       * enviroment variables. Then search for the binary along the
       * PATH.
       */
      len = (sizeof (path) / sizeof (char)) - 1;
      strncpy (path, my_path, len);
      path[len] = EOS;
       
      execname_len = sizeof (execname) / sizeof (char);
#ifdef _WINDOWS
      MakeCompleteName (appArgv0, "EXE", path, execname, &execname_len);
#else /* _WINDOWS */
      MakeCompleteName (appArgv0, "", path, execname, &execname_len);
#endif /* _WINDOWS */
      if (execname[0] == EOS)
        {
          fprintf (stderr, "TtaInitializeAppRegistry internal error\n");
          fprintf (stderr, "\tcannot find path to binary : %s\n", appArgv0);
          ThotExit (1);
        }
    }

  /*
   * Now that we have a complete path up to the binary, extract the
   * application name.
   */
  /* IV june 2004: force the appName to use the amaya temporary directory
     with thot compilers */
  AppRegistryEntryAppli = TtaStrdup (appName);
  AppNameW = TtaStrdup (appName);
#ifdef HAVE_LSTAT
  /*
   * on Unixes, the binary path started may be a softlink
   * to the real app in the real dir.
   */
  len = 1;
  strcpy (c_execname, execname);
  while (lstat (c_execname, &stat_buf) == 0 &&
         S_ISLNK (stat_buf.st_mode) &&
         len > 0)
    {
      len = readlink (c_execname, c_filename,
                      sizeof (filename) / sizeof (char));
      if (len > 0)
        {
          /*
           * Two cases : can be an absolute link to the binary
           * or a relative link.
           */
          c_filename[len] = 0;
          if (c_filename[0] == DIR_SEP)
            strcpy (c_execname, c_filename);
          else
            {
              c_end = c_execname;
              while (*c_end)
                c_end++; /* go to the ending NUL */
              while (c_end > c_execname && *c_end != DIR_SEP)
                c_end--;
              strcpy (c_end + 1, c_filename);
            }
        } 
    }
  strcpy (execname, c_execname);
#endif /* HAVE_LSTAT */
#ifdef _UNIX
  /* amaya_exe is FALSE when running a compiler */
  dir_end = &execname[strlen (execname)-1];
  while (dir_end > execname && dir_end[-1] != DIR_SEP)
    dir_end--;
  amaya_exe = (strlen (dir_end) > 4 &&
               (!strstr (dir_end, "amaya") ||
               !strstr (dir_end, "print")));
#endif /* _UNIX */
   
#ifdef DEBUG_REGISTRY
  fprintf (stderr, "path to binary %s : %s\n", appName, execname);
#endif
   
  /* get the THOTDIR for this application. It's under a bin dir */
  dir_end = execname;
  while (*dir_end) /* go to the ending NULL */
    dir_end++;
   
  /* remove the application name */
  ok = FALSE;
  do
    {
      dir_end--;
      ok = (dir_end <= execname || *dir_end == DIR_SEP);
    }
  while (!ok);

  if (*dir_end == DIR_SEP)
    {
      /* the name has been found */
      found = TRUE;
      *dir_end = EOS;
      /* save the binary directory in BinariesDirectory */
      strncpy (BinariesDirectory, execname,
               sizeof (BinariesDirectory) / sizeof (char));
      /* remove the binary directory */
      found = FALSE;
      ok = FALSE;
      round = 2;
      while (!found || round != 0)
        {
          do
            {
              dir_end--;
              ok = (dir_end <= execname || *dir_end == DIR_SEP);
            } while (!ok);

          if (*dir_end == DIR_SEP)
            {
              *dir_end = EOS;
              if (!strcmp (&dir_end[1], ".."))
                round ++;
              else if (strcmp (&dir_end[1], "."))
                {
                  round --;
                  /* a directory name has been found */
                  found = TRUE;
                } 
            }
          else
            {
              /* no directory has been found */
              found = TRUE;
              ok = FALSE;
            }
        } 

      if (ok)
        {
          *dir_end = EOS;
          if (IsThotDir (execname))
            AddRegisterEntry ("System", "THOTDIR", execname,
                              REGISTRY_INSTALL, TRUE);
        }
#ifdef COMPILED_IN_THOTDIR
      /* Check a compiled-in value */
      else if (IsThotDir (UCOMPILED_IN_THOTDIR))
        {
          strcpy (UCOMPILED_IN_THOTDIR, COMPILED_IN_THOTDIR);
          strcpy (execname, UCOMPILED_IN_THOTDIR);
          AddRegisterEntry ("System", "THOTDIR", UCOMPILED_IN_THOTDIR,
                            REGISTRY_INSTALL, TRUE);
        } 
#endif /* COMPILED_IN_THOTDIR */
      else
        {
          fprintf (stderr, "Cannot find THOTDIR\n");
          ThotExit (1);
        } 
    }
   
#ifdef MACHINE
  /* if MACHINE is set up, add it to the registry */
  strcpy (UMACHINE, MACHINE);
  AddRegisterEntry ("System", "MACHINE", UMACHINE, REGISTRY_INSTALL, TRUE);
#endif

  /* load the system settings, stored in THOTDIR/config/thot.ini */
  sprintf (filename, "%s%c%s%c%s", execname, DIR_SEP, THOT_CONFIG_FILENAME,
           DIR_SEP, THOT_INI_FILENAME);
  if (TtaFileExist (filename))
    {
#ifdef DEBUG_REGISTRY
      fprintf (stderr, "reading system %s from %s\n", THOT_INI_FILENAME, filename);
#endif
      ImportRegistryFile (filename, REGISTRY_SYSTEM);
      *dir_end = EOS;
      dir_end -= 3;
    }
  else
    fprintf (stderr, "System wide %s not found at %s\n", THOT_INI_FILENAME,
             &filename[0]);
   
  /*
  ** find the APP_HOME directory name:
  ** Unix: $HOME/.appname,
  ** Win95: $THOTDIR/users/login-name/
  ** WinNT: c:\WINNT\profiles\login-name
  ** Win2000/XP: $HOMEDRIVE\Documents and Settings\username\appname
  **              or
  **             Documents and settings\All Users\login-name
  **              or
  **             the same thing as WinNT.
  */

  app_home[0] = EOS;
  /* IV 18/08/2003 Check the variable AMAYA_USER_HOME first */
  ptr = getenv ("AMAYA_USER_HOME");
  if (ptr && TtaDirExists (ptr))
    strncpy (app_home, ptr, MAX_PATH);

  if (app_home[0] == EOS && amaya_exe)
    {
#ifdef _WINGUI
      /* compute the default app_home value from the username and thotdir */
      dwSize = sizeof (username);
      status = GetUserName (username, &dwSize);
      if (status)
        {
          if (strchr (username, '*'))
            /* don't use a username that include stars */
            ptr = WIN_DEF_USERNAME;
          else
            ptr = username;
        }
      else
        /* under win95, there may be no user name */
        ptr = WIN_DEF_USERNAME;

      /* Define the app_home directory */
      if (IS_NT)
        /* winnt: apphome is windowsdir\profiles\username\appname */
        {
          typedef BOOL (STDMETHODCALLTYPE FAR * LPFNGETPROFILESDIRECTORY) (
                                                                           LPTSTR lpProfileDir,
                                                                           LPDWORD lpcchSize
                                                                           );
          HMODULE                  g_hUserEnvLib          = NULL;
          LPFNGETPROFILESDIRECTORY GetProfilesDirectory   = NULL;

          windir[0] = EOS;
          g_hUserEnvLib = LoadLibrary ("userenv.dll");
          if (g_hUserEnvLib)
            {
              GetProfilesDirectory =
                (LPFNGETPROFILESDIRECTORY) GetProcAddress (g_hUserEnvLib,
                                                           "GetProfilesDirectoryA");
              dwSize = MAX_PATH;
              GetProfilesDirectory (windir, &dwSize);
            }
          if (windir[0] == EOS)
            GetWindowsDirectory (windir, dwSize);

          /* Check if a previous app_home directory existed.
             If yes use it, else we try to create it using new conventions. */
          /* Windows NT convention */
          sprintf (app_home, "%s\\profiles\\%s\\%s", windir, ptr, AppNameW);
          ptr2 = getenv ("HOMEDRIVE");
          if (!TtaDirExists (app_home))
            {
              sprintf (app_home, "%s\\%s\\%s", windir, ptr, AppNameW);
              if (!TtaDirExists (app_home))
                app_home[0] = EOS;
            }
          if (app_home[0] == EOS)
            {
              /* use the HOMEDRIVE and HOMEPATH environment variables first */
              ptr3 = getenv ("HOMEPATH");
              if (ptr2 && *ptr2 && ptr3)
                {
                  sprintf (app_home, "%s%s\\%s", ptr2, ptr3, AppNameW);
                  if (!TtaDirExists (app_home))
                    app_home[0] = EOS;
                }
            }
          if (app_home[0] == EOS)
            {
              sprintf (app_home, "%s\\%s\\%s", windir, ptr, AppNameW);
              if (!TtaMakeDirectory (app_home))
                {
                  /* another possible Windows 2000/XP convention */
                  sprintf (app_home, "%s\\Documents and Settings\\%s\\%s",
                           ptr2, ptr, AppNameW);
                  if (!TtaMakeDirectory (app_home))
                    app_home[0] = EOS;
                }
            }

          /* At this point app_home has a value if the directory existed.
             Otherwise, we'll try to create a new one */
          if (app_home[0] == EOS)
            {
              /* try to use one of the system home dirs */
              /* the Windows 2000/XP convention */
              if (ptr2 && *ptr2 && ptr3)
                sprintf (app_home, "%s%s", ptr2, ptr3);
              else
                sprintf (app_home, "%s\\Documents and Settings\\%s",
                         windir, ptr);

              if (!TtaDirExists (app_home))
                /* the Windows NT convention */
                sprintf (app_home, "%s\\profiles\\%s", windir, ptr);

              /* add the end suffix */
              strcat (app_home, "\\");
              strcat (app_home, AppNameW);
              if (!TtaMakeDirectory (app_home))
                app_home[0] = EOS;
            }
        }

      if (app_home[0] == EOS)
        {
          /* win95: apphome is  thotdir\users\username */
          sprintf (app_home, "%s\\%s", execname, WIN_USERS_HOME_DIR);
          TtaMakeDirectory (app_home);
          sprintf (app_home, "%s\\%s\\%s", execname, WIN_USERS_HOME_DIR, ptr);
          TtaMakeDirectory (app_home);
        }

#else /* _WINGUI */
#ifdef _UNIX
      ptr = getenv ("HOME");
#ifdef _MACOS
      sprintf (app_home_mac, "%s%c%s%c%s%c",
	 ptr, DIR_SEP, MACOSX_LIBRARY, DIR_SEP, MACOSX_PREFERENCES, DIR_SEP);
      if (!TtaDirExists (app_home_mac))
	  sprintf (app_home, "%s%c.%s", ptr, DIR_SEP, AppNameW); 
      else	
          {
             sprintf (app_home, "%s%c%s%c%s%c%s",
	     ptr, DIR_SEP, MACOSX_LIBRARY, DIR_SEP, MACOSX_PREFERENCES, DIR_SEP, AppNameW); 
       	     sprintf (app_home_orig, "%s%c.%s", ptr, DIR_SEP, AppNameW); 
          }
#else /* _MACOS */
      sprintf (app_home, "%s%c.%s", ptr, DIR_SEP, AppNameW); 
#endif /* _MACOS */
#else /* _UNIX */

#if defined(_WX) && defined(_WINDOWS)
      wxString wx_win_homedir = TtaGetHomeDir();
      sprintf (app_home, "%s%c%s", 
               (const char*)wx_win_homedir.mb_str(wxConvUTF8), DIR_SEP, AppNameW);
#endif /* _WX && _WINDOWS */

#endif /* _UNIX */
#endif /*_WINGUI */
     }

  /* get the app_home again from the registry, as the user may have
     overriden it using the global configuration files */
  ptr = TtaGetEnvString ("APP_HOME");
  if (ptr && TtaDirExists (ptr))
    strcpy (app_home, ptr);
  // IV: May 2006  replace TtaMakeDirectory (app_home) by TtaCheckMakeDirectory (app_home, TRUE)
  if (app_home[0] != EOS && !TtaDirExists (app_home) && !TtaCheckMakeDirectory (app_home, TRUE))
    app_home[0] = EOS;

  /* store the value of APP_HOME in the registry */
  if (amaya_exe)
    AddRegisterEntry (AppRegistryEntryAppli, "APP_HOME", app_home,
                      REGISTRY_SYSTEM, FALSE);

  /* read the user's preferences (if they exist) */
  if (app_home[0] != EOS)
    {
#ifdef _MACOS
      if (!TtaDirExists (app_home_mac))
          sprintf (filename, "%s%c%s", app_home, DIR_SEP, THOT_RC_FILENAME);
      else
      {	
         sprintf (filename, "%s%c%s", app_home, DIR_SEP, THOT_RC_FILENAME);
         if (!TtaFileExist (&filename[0]))
            {
	      /* The first time, the thot.rc file is copied from the previous location */
              sprintf (filename_orig, "%s%c%s", app_home_orig, DIR_SEP, THOT_RC_FILENAME);
              TtaFileCopy (filename_orig, filename);
            }
      }
#else /* _MACOS */
      sprintf (filename, "%s%c%s", app_home, DIR_SEP, THOT_RC_FILENAME);
#endif /* _MACOS */
      if (TtaFileExist (&filename[0]))
        {
#ifdef DEBUG_REGISTRY
          fprintf (stderr, "reading user's %s from %s\n",
                   THOT_RC_FILENAME, filename);
#endif
          ImportRegistryFile (filename, REGISTRY_USER);
        }
#ifdef DEBUG_REGISTRY
      else
        fprintf (stderr, "User's %s not found\n", app_home);
#endif
    }
#ifdef DEBUG_REGISTRY
  else
    fprintf (stderr, "User's %s not found\n", THOT_RC_FILENAME);
#endif
   
#ifdef DEBUG_REGISTRY
  PrintEnv (stderr);
#endif
  /* set the default APP_TMPDIR == APP_HOME */
  AddRegisterEntry (AppRegistryEntryAppli, "APP_TMPDIR", app_home,
                    REGISTRY_SYSTEM, TRUE);
  /* initialize the standard environment (i.e global variables) with 
     values stored in the registry. */
  InitEnviron ();
  /* reset the status flag to say we don't need to save the registry right now */
  AppRegistryModified = 0;
}

/*----------------------------------------------------------------------
  TtaFreeAppRegistry : frees the memory associated with the
  registry
  ----------------------------------------------------------------------*/
void TtaFreeAppRegistry (void)
{
  RegistryEntry cour, next;

  if (AppRegistryInitialized == 0)
    return;

  cour = AppRegistryEntry;
  
  while (cour) {
    if (cour->appli)
      TtaFreeMemory (cour->appli);
    if (cour->name)
      TtaFreeMemory (cour->name);
    if (cour->orig)
      TtaFreeMemory (cour->orig); 
    if (cour->value)
      TtaFreeMemory (cour->value);
    next = cour->next;
    TtaFreeMemory (cour);
    cour = next;
  }
  TtaFreeMemory (AppRegistryEntryAppli);
  TtaFreeMemory (AppNameW);
  AppRegistryInitialized = 0;
}

/*----------------------------------------------------------------------
  SearchFile look for a file following the guideline given by dir
  Returns 1 with fullName set to the absolute pathname if found,
  0 otherwise.                                   
  Depending on dir value, the file is looked for in:
  - 0 : /                                                 
  - 1 : ThotDir/document paths
  - 2 : ThotDir/config     
  - 3 : ThotDir/batch
  ----------------------------------------------------------------------*/
int SearchFile (char *fileName, int dir, char *fullName)
{
  char                tmpbuf[200];
  char               *imagepath;
  int                 i, j;
  int                 ret;

  if (Thot_Dir != NULL)
    strcpy (fullName, Thot_Dir);
  else
    *fullName = EOS;
  switch (dir)
    {
    case 1:
      /* Lookup in schema and documents path */
      strcat (fullName, fileName);
      ret = TtaFileExist (fullName);
      /* lookup in shemas path */
      i = 0;
      j = 0;
      imagepath = SchemaPath;
      while (ret == 0 && imagepath[i] != EOS)
        {
          while (imagepath[i] != EOS && imagepath[i] != PATH_SEP && i < 200)
            tmpbuf[j++] = imagepath[i++];

          tmpbuf[j] = EOS;
          i++;
          j = 0;
          sprintf (fullName, "%s%s%s", tmpbuf, DIR_STR, fileName);
          ret = TtaFileExist (fullName);
        }

      /* lookup in document path */
      i = 0;
      j = 0;
      imagepath = SchemaPath;
      while (ret == 0 && imagepath[i] != EOS)
        {
          while (imagepath[i] != EOS && imagepath[i] != PATH_SEP && i < 200)
            tmpbuf[j++] = imagepath[i++];

          tmpbuf[j] = EOS;
          i++;
          j = 0;
          sprintf (fullName, "%s%s%s", tmpbuf, DIR_STR, fileName);
          ret = TtaFileExist (fullName);
        }
      break;
    case 2:
      /* lookup in config */
      strcat (fullName, DIR_STR);
      strcat (fullName, "config");
      strcat (fullName, DIR_STR);
      strcat (fullName, fileName);
      break;
    case 3:
      /* lookup in batch */
      strcat (fullName, DIR_STR);
      strcat (fullName, "batch");
      strcat (fullName, DIR_STR);
      strcat (fullName, fileName);
      break;
    default:
      strcat (fullName, DIR_STR);
      strcat (fullName, fileName);
    }

  /* general search */
  ret = TtaFileExist (fullName);
  if (ret == 0)
    {
      strcpy (fullName, fileName);
      ret = TtaFileExist (fullName);
    }
  return ret;
}
