/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
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

#include "thotkey.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "registry.h"
#include "application.h"
#include "thotdir.h"
#define THOT_EXPORT
#include "platform_tv.h"

#include "fileaccess_f.h"
#include "platform_f.h"
/* #define DEBUG_REGISTRY enable the Registry debug messages */


/* for Marc.Baudoin@hsc.fr (Marc Baudoin) */
#ifdef _WINDOWS
#include <direct.h>
#include <winbase.h>
#include "winsys.h"
#   define WIN_DEF_USERNAME       TEXT("default")
#   define THOT_INI_FILENAME      TEXT("win-thot.rc")
#   define WIN_USERS_HOME_DIR     TEXT("users")
#   define DEF_TMPDIR             TEXT("c:\\temp")
STRING WIN_Home;
#else /* !_WINDOWS */
#   define THOT_INI_FILENAME      TEXT("unix-thot.rc")
#   define DEF_TMPDIR             TEXT("/tmp")
#endif /* ! _WINDOWS */

#define THOT_RC_FILENAME      TEXT("thot.rc")
#define THOT_CONFIG_FILENAME  TEXT("config")
#define THOT_BIN_FILENAME     TEXT("bin")
#define THOT_LIB_DEFAULTNAME  TEXT("thot_lib")
#define MAX_REGISTRY_ENTRIES 100

typedef enum
  {
     REGISTRY_USER,		/* values which can be redefined by the user */
     REGISTRY_SYSTEM,		/* values fetched from the system config     */
     REGISTRY_INSTALL,		/* installation value e.g. THOTDIR, VERSION  */
     REGISTRY_MAX_CATEGORIES
  }
RegistryLevel;

typedef struct struct_RegistryEntry
  {
     struct struct_RegistryEntry *next;		/* chaining ! */
     RegistryLevel       level;	/* exact level */
     STRING appli;	/* corresponding section */
     STRING name;	/* name of the entry     */
     STRING orig;	/* the original value (to be saved back) */
     STRING value;	/* user-level value */
  }
RegistryEntryBlk   , *RegistryEntry;

static int           AppRegistryInitialized = 0;
static int           AppRegistryModified = 0;
static RegistryEntry AppRegistryEntry = NULL;
static char*         AppRegistryEntryAppli = NULL;
static CHAR_T        CurrentDir[MAX_PATH];
static STRING        Thot_Dir;

PathBuffer execname;
PathBuffer path;

#ifdef _WINDOWS
/* @@why do we need this here? */
int errno;
#endif

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static STRING       SkipToEqual (STRING ptr)
#else
static STRING       SkipToEqual (ptr)
STRING ptr;
#endif
{
  while (*ptr != EOS && *ptr != TEXT('=') && *ptr != EOL && *ptr != __CR__)
    ptr++;
  return (ptr);
}

/*----------------------------------------------------------------------
   TtaSkipBlanks skips all spaces, tabs, linefeeds and newlines at the
   beginning of the string and returns the pointer to the new position. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING TtaSkipBlanks (STRING ptr)
#else
STRING TtaSkipBlanks (ptr)
STRING ptr;
#endif
{
  while (*ptr == _SPACE_ || *ptr == BSPACE || *ptr == EOL ||
	  *ptr == TAB || *ptr == __CR__)
    ptr++;
  return (ptr);
}

/*----------------------------------------------------------------------
   TtaIsBlank returns True if the first character is a space, a tab, a
   linefeed or a newline.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            TtaIsBlank (STRING ptr)
#else
ThotBool            TtaIsBlank (ptr)
STRING ptr;
#endif
{
  if (*ptr == _SPACE_ || *ptr == BSPACE || *ptr == EOL ||
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
#ifdef __STDC__
static void         DoVariableSubstitution (char* input, int i_len,
					    STRING output, int o_len)
#else  /* __STDC__ */
static void         DoVariableSubstitution (input, i_len, output, o_len)
char*  input;
int                 i_len;
STRING output;
int                 o_len;
#endif
{
  STRING cour = input;
  char*  base = input;
  STRING res = output;
  char*  value;
  CHAR_T   save;
#define CHECK_OVERFLOW (((cour - input) > i_len) || ((res - output) >= (o_len - 1)))

  while (*cour)
    {
      if CHECK_OVERFLOW
	break;
      if (*cour != TEXT('$'))
	{
	  *res++ = *cour++;
	  continue;
	}
      base = cour;		/* save the position to the $ */
      cour++;
      if CHECK_OVERFLOW
	break;
      if (*cour != TEXT('('))
	{
	  *res++ = TEXT('$');
	  if CHECK_OVERFLOW
	    break;
	  *res++ = *cour++;
	  continue;
	}

      /* Ok, that the beginning of a variable name ... */
      base += 2;		/* skip the $(  header */
      do
	{
	  cour++;
	  if CHECK_OVERFLOW
	    break;
	}
      while (*cour != TEXT(')') && !TtaIsBlank (cour));
      if CHECK_OVERFLOW
	break;
      
      save = *cour;
      *cour = EOS;
      if (save != TEXT(')'))
	fprintf (stderr, "invalid variable name %s in %s\n", base, THOT_INI_FILENAME);

      /* We are ready to fetch the base value from the Registry */
      value = TtaGetEnvString (base);
      if (value == NULL)
	{
	  fprintf (stderr, "%s referencing undefined variable %s\n", THOT_INI_FILENAME, base);
	  *cour = save;
	  cour++;
	  if CHECK_OVERFLOW
	    break;
	  continue;
	}
      *cour = save;
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
#ifdef __STDC__
static int          NewRegisterEntry (STRING appli, STRING name, STRING value,
				      RegistryLevel level)
#else  /* __STDC__ */
static int          NewRegisterEntry (appli, name, value, level)
STRING appli;
STRING name;
STRING value;
RegistryLevel       level;

#endif
{
   CHAR_T                resu[2000];
   RegistryEntry       cour, ptr, previous;

   if (AppRegistryInitialized == 0)
      return (-1);

   /*
    * do not register volatile informations like the
    * home dir or the current working directory.
    */
   if (!ustrcasecmp (name, TEXT("pwd")))
      return (0);
   if (!ustrcasecmp (name, TEXT("home")))
      return (0);
   if (!ustrcasecmp (name, TEXT("user")))
      return (0);
   if (!ustrcasecmp (name, TEXT("cwd")))
      return (0);

   /*
    * substitute the $(xxxx) with their values.
    */
   DoVariableSubstitution (value, ustrlen (value), resu, sizeof (resu));

   /*
    * allocate an entry, fill it and chain it.
    */

   cour = (RegistryEntry) TtaGetMemory (sizeof (RegistryEntryBlk));
   if (cour == NULL)
      return (-1);
   cour->appli = TtaStrdup (appli);
   cour->name = TtaStrdup (name);
   cour->orig = TtaStrdup (value);
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
#ifdef __STDC__
static int          AddRegisterEntry (STRING appli, STRING name, STRING value,
				      RegistryLevel level, int overwrite)
#else  /* __STDC__ */
static int          AddRegisterEntry (appli, name, value, level, overwrite)
STRING appli;
STRING name;
STRING value;
RegistryLevel       level;
int                 overwrite;

#endif
{
  CHAR_T                resu[2000];
  RegistryEntry       cour;

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
       if (!ustrcmp (cour->name, name))
	 {
	     /* Cannot superseed and INSTALL value */
	   if (cour->level == REGISTRY_INSTALL)
	     return (0);
	   else if (!ustrcasecmp (cour->appli, appli) && cour->level == level)
	     break;
	 }
       cour = cour->next;
     }

   if (cour != NULL)
     {
       /* there is aleady an entry */
       if (!overwrite)
	 return (0);

       DoVariableSubstitution (value, ustrlen (value), resu, sizeof (resu));
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
	   while (cour != NULL)
	     {
	       if ((!ustrcasecmp (cour->appli, THOT_LIB_DEFAULTNAME)) &&
		   (!ustrcmp (cour->name, name)))
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
#ifdef __STDC__
static void         PrintEnv (FILE * output)
#else  /* __STDC__ */
static void         PrintEnv (output)
FILE               *output;
#endif
{
  RegistryEntry       cour, next;
  
  if (AppRegistryInitialized == 0)
    return;

  cour = AppRegistryEntry;
  while (cour != NULL)
    {
      /* don't save any system entry */
      if (!ustrcasecmp (cour->appli, TEXT("System")))
	{
	  cour = cour->next;
	  continue;
	}

      /* print out the section */
      fprintf (output, "[%s]\n", cour->appli);
      /* add all the entries under the same appli name */
      if (cour->level == REGISTRY_USER 
		  && ustrcasecmp (cour->name, TEXT("TMPDIR"))
		  && ustrcasecmp (cour->name, TEXT("APP_TMPDIR"))
		  && ustrcasecmp (cour->name, TEXT("APP_HOME")))
	fprintf (output, "%s=%s\n", cour->name, cour->orig);
      next = cour->next;
      while (next != NULL && !ustrcasecmp (next->appli, cour->appli))
	{
	  if (next->level == REGISTRY_USER
	      && ustrcasecmp (cour->name, TEXT("TMPDIR"))
#ifndef _WINDOWS
	      && ustrcasecmp (next->name, "APP_TMPDIR")
#endif /* _WINDOWS */
	      && ustrcasecmp (next->name, TEXT("APP_HOME")))
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
#ifdef __STDC__
static void         SortEnv (void)
#else  /* __STDC__ */
static void         SortEnv ()
#endif
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
	  cmp = ustrcasecmp (cour->appli, (*start)->appli);
	  if (cmp <= 0 ||
	      (cmp == 0 &&
	       (ustrcasecmp (cour->name, (*start)->name) < 0)))
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
#ifdef __STDC__
ThotBool TtaGetEnvInt (char* name, int *value)
#else
ThotBool TtaGetEnvInt (name, value)
char*  name;
int *value;
#endif /* __STDC__ */
{
 char* strptr;

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
 *value = uctoi (strptr);

 return TRUE;
}

/*----------------------------------------------------------------------
  TtaGetEnvBoolean : read the ThotBool value associated to an 
  environment string.
  Returns TRUE if the env variables exists or FALSE if it isn't the case.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool TtaGetEnvBoolean (char* name, ThotBool *value)
#else
ThotBool TtaGetEnvBoolean (name, value)
char* name;
ThotBool *value;
#endif /* __STDC__ */
{
 char* strptr;

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
 if ( ustrcasecmp (strptr, TEXT("yes")))
   *value = FALSE;
 else
   *value = TRUE;

 return TRUE;
}


/*----------------------------------------------------------------------
  TtaGetEnvString : read the value associated to an environment string
  if not present return NULL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char* TtaGetEnvString (char* name)
#else  /* __STDC__ */
char* TtaGetEnvString (name)
char* name;
#endif
{
  RegistryEntry       cour;
  STRING value;

  if (AppRegistryInitialized == 0)
    return (getenv (name));

  /* appname allows to get the application name */
  if (!strcasecmp ("appname", name))
    return(AppRegistryEntryAppli);

  if ((!ustrcasecmp (name, TEXT("cwd"))) || (!ustrcasecmp (name, TEXT("pwd"))))
    {
      return (ugetcwd (&CurrentDir[0], sizeof(CurrentDir)));
    }

  /* first lookup in the System defaults */
  cour = AppRegistryEntry;
  while (cour != NULL)
    {
      if (!ustrcasecmp (cour->appli, TEXT("System")) && !ustrcmp (cour->name, name) && cour->value[0] != EOS)
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
      if (!ustrcasecmp (cour->appli, AppRegistryEntryAppli)
		  && !ustrcmp (cour->name, name) && cour->value[0] != EOS
		  && cour->level == REGISTRY_USER)
	{
#ifdef DEBUG_REGISTRY
	  fprintf (stderr, "TtaGetEnvString(\"%s\") = %s\n", name, cour->value);
#endif
	  return (cour->value);
	}
      cour = cour->next;
    } 

    /* then lookup in the application defaults */
  cour = AppRegistryEntry;
  while (cour != NULL)
    {
      if (!ustrcasecmp (cour->appli, AppRegistryEntryAppli) 
		  && !ustrcmp (cour->name, name) && cour->value[0] != EOS
		  && cour->level == REGISTRY_SYSTEM)
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
      if (!ustrcasecmp (cour->appli, THOT_LIB_DEFAULTNAME) && !ustrcmp (cour->name, name) && cour->value[0] != EOS)
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
  value = ugetenv (name);
  if (value == NULL)
    TtaSetEnvString (name, _EMPTYSTR_, FALSE);
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
#ifdef __STDC__
void                TtaClearEnvString (STRING name)
#else  /* __STDC__ */
void                TtaClearEnvString (name)
CONST STRING        name;
CONST STRING        value;
int                 overwrite;
#endif
{
   AddRegisterEntry (AppRegistryEntryAppli, name, _EMPTYSTR_, REGISTRY_USER, TRUE);
}

/*----------------------------------------------------------------------
 TtaSetEnvInt : set the value associated to an environment string,
                for the current application.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetEnvInt (STRING name, int value, int overwrite)
#else  /* __STDC__ */
void                TtaSetEnvInt (name, value, overwrite)
CONST STRING        name;
CONST int           value;
int                 overwrite;
#endif
{
  /* hardcoded so that the biggest integer value has 5 digits:
     65535 */
  CHAR_T ptr[6];
  int  r_val;

  r_val = value % 65537;
    usprintf (ptr, TEXT("%d"), r_val);
  AddRegisterEntry (AppRegistryEntryAppli, name, ptr,
		    REGISTRY_USER, overwrite);
}

/*----------------------------------------------------------------------
 TtaSetEnvBoolean : set the value associated to an environment string,
                    for the current application.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetEnvBoolean (STRING name, ThotBool value, int overwrite)
#else  /* __STDC__ */
void                TtaSetEnvBoolean (name, value, overwrite)
CONST STRING        name;
CONST ThotBool      value;
int                 overwrite;
#endif
{
  STRING ptr;

  if (value)
    ptr = TEXT("yes");
  else
    ptr = TEXT("no");
  AddRegisterEntry (AppRegistryEntryAppli, name, ptr,
		    REGISTRY_USER, overwrite);
}

/*----------------------------------------------------------------------
 TtaSetEnvString : set the value associated to an environment string,
                   for the current application.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetEnvString (STRING name, STRING value, int overwrite)
#else  /* __STDC__ */
void                TtaSetEnvString (name, value, overwrite)
CONST STRING        name;
CONST STRING        value;
int                 overwrite;
#endif
{
  STRING tmp = value;
  
  if (!tmp)
    tmp = _EMPTYSTR_;
 
   AddRegisterEntry (AppRegistryEntryAppli, name, value,
		     REGISTRY_USER, overwrite);
}

/*----------------------------------------------------------------------
 TtaSetDefEnvString : set the defaul value associated to an environment
                      string, for the current application.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetDefEnvString (STRING name, STRING value, 
					int overwrite)
#else  /* __STDC__ */
void                TtaSetDefEnvString (name, value, overwrite)
CONST STRING        name;
CONST STRING        value;
int                 overwrite;
#endif
{
  STRING tmp = value;
  
  if (!tmp)
    tmp = _EMPTYSTR_;

   AddRegisterEntry (AppRegistryEntryAppli, name, value,
		     REGISTRY_SYSTEM, overwrite);
}

/*----------------------------------------------------------------------
  TtaGetDefEnvInt : read the default integer value associated to an 
  environment string.
  Returns TRUE if the env variables exists or FALSE if it isn't the case.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool TtaGetDefEnvInt (STRING name, int *value)
#else
ThotBool TtaGetDefEnvInt (name, value)
STRING name;
int *value;
#endif /* __STDC__ */
{
 STRING strptr;

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
 *value = uctoi (strptr);

 return TRUE;
}

/*----------------------------------------------------------------------
  TtaGetDefEnvBoolean : read the ThotBool value associated to an 
  environment string.
  Returns TRUE if the env variables exists or FALSE if it isn't the case.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool TtaGetDefEnvBoolean (STRING name, ThotBool *value)
#else
ThotBool TtaGetDefEnvBoolean (name, value)
STRING name;
ThotBool *value;
#endif /* __STDC__ */
{
 STRING strptr;

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
 if ( ustrcasecmp (strptr, TEXT("yes")))
   *value = FALSE;
 else
   *value = TRUE;

 return TRUE;
}

/*----------------------------------------------------------------------
  TtaGetDefEnvString : read the default value associated to an 
  environment string. If not present, returns NULL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING TtaGetDefEnvString (STRING name)
#else  /* __STDC__ */
STRING TtaGetDefEnvString (name)
STRING name;
#endif
{
  RegistryEntry       cour;
  STRING value;

  if (AppRegistryInitialized == 0)
    return (ugetenv (name));

  /* appname allows to get the application name */
  if (!ustrcasecmp(TEXT("appname"), name))
    return(AppRegistryEntryAppli);

  if ((!ustrcasecmp (name, TEXT("cwd"))) || (!ustrcasecmp (name, TEXT("pwd"))))
    {
      return (ugetcwd (&CurrentDir[0], sizeof(CurrentDir)));
    }

  /* First lookup in the System defaults */
  cour = AppRegistryEntry;
  while (cour != NULL)
    {
      if (!ustrcasecmp (cour->appli, TEXT("System")) && !ustrcmp (cour->name, name) 
	  && cour->level == REGISTRY_SYSTEM && cour->value[0] != EOS)
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
      if (!ustrcasecmp (cour->appli, AppRegistryEntryAppli) 
	  && !ustrcmp (cour->name, name) 
	  && cour->level == REGISTRY_SYSTEM && cour->value[0] != EOS)
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
      if (!ustrcasecmp (cour->appli, THOT_LIB_DEFAULTNAME) 
	  && !ustrcmp (cour->name, name) 
	  && cour->level == REGISTRY_SYSTEM && cour->value[0] != EOS)
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
  value = ugetenv (name);
  
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
#ifdef __STDC__
static int          IsThotDir (CONST STRING path)
#else  /* __STDC__ */
static int          IsThotDir (CONST STRING path)
#endif				/* __STDC__ */
{
   CHAR_T                filename[MAX_PATH];

   if (path == NULL)
      return (0);
   ustrcpy (filename, path);
   ustrcat (filename, DIR_STR);
   ustrcat (filename, THOT_CONFIG_FILENAME);
   ustrcat (filename, DIR_STR);
   ustrcat (filename, THOT_INI_FILENAME);
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

#ifdef _WINDOWS
#ifndef __GNUC__
/*----------------------------------------------------------------------
   WINReg_get - simulates getenv in the WIN32 registry              
   
   looks for <env> in                                                 
   HKEY_CURRENT_USER\Software\Amaya\<var>                                      
  ----------------------------------------------------------------------*/
static STRING    WINReg_get (CONST STRING env)
{
  static STRING  userBase;
  CHAR_T         textKey[MAX_PATH];
  HKEY           hKey;
  DWORD          type;
  LONG           success;
  static BYTE    ret[MAX_PATH];	/* thread unsafe! */
  DWORD          retLen = sizeof (ret);

  userBase = TEXT("Software");
  
  usprintf (textKey, TEXT("%s\\%s\\%s"), userBase, AppRegistryEntryAppli, env);	                    
  success = RegOpenKeyEx (HKEY_CURRENT_USER, textKey, 0, KEY_ALL_ACCESS,
			  &hKey);
  if (success == ERROR_SUCCESS)
    {
      success = RegQueryValueEx (hKey, NULL, NULL, &type, ret, &retLen);
      RegCloseKey (hKey);
    }
  return (success == ERROR_SUCCESS) ? ISO2WideChar(ret) : NULL;
}

/*----------------------------------------------------------------------
   WINReg_set - stores a value in the WIN32 registry           
   
   stores <key, value> in                                                 
   HKEY_CURRENT_USER\Software\app-name\<key>                                      
  ----------------------------------------------------------------------*/
static ThotBool WINReg_set (CONST STRING key, CONST STRING value)
{
   static STRING    userBase;
   CHAR_T           textKey[MAX_PATH];
   HKEY             hKey;
   LONG             success;
   BYTE             protValue[MAX_PATH];
   DWORD            protValueLen = sizeof (protValue);
   DWORD            dwDisposition;
 
   userBase = TEXT("Software");

   /* protect against values bigger than what we can write in
      the registry */
   ustrncpy (ISO2WideChar(protValue), value, protValueLen - 1);
   protValue[protValueLen-1] = EOS;

   usprintf (textKey,TEXT("%s\\%s\\%s"), userBase, AppRegistryEntryAppli, key);	                    
   success = RegCreateKeyEx (HKEY_CURRENT_USER, textKey, 0, 
	                         _EMPTYSTR_, REG_OPTION_VOLATILE, KEY_ALL_ACCESS,
		                     NULL, &hKey, &dwDisposition);  
   if (success == ERROR_SUCCESS)
   /* create the userBase entry */
   {
	 success = RegSetValueEx (hKey, NULL, 0, REG_SZ, protValue,
				  protValueLen);
	 RegCloseKey (hKey);
   }
   
   return (success == ERROR_SUCCESS) ? TRUE : FALSE;
}

/*----------------------------------------------------------------------
   WINIni_get - simulates getenv in the Windows/Amaya.ini file     
  ----------------------------------------------------------------------*/
static STRING          WINIni_get (CONST STRING env)
{
   DWORD               res;
   static CHAR_T         ret[MAX_PATH];	/* thread unsafe! */

   res = GetPrivateProfileString (TEXT("Amaya"), env, _EMPTYSTR_, ret, sizeof (ret), TEXT("Amaya.ini"));
   return res ? ret : NULL;
}
#endif
#endif /* _WINDOWS */


/*----------------------------------------------------------------------
  TtaSaveAppRegistry : Save the Registry in the THOT_RC_FILENAME located
  in the user's directory.
  ----------------------------------------------------------------------*/
void                TtaSaveAppRegistry ()
{

   STRING              app_home;
   CHAR_T              filename[MAX_PATH];
   FILE               *output;

   if (!AppRegistryInitialized)
      return;
   if (!AppRegistryModified)
      return;

   app_home = TtaGetEnvString ("APP_HOME");
   if (app_home != NULL)
     usprintf (filename, TEXT("%s%c%s"), app_home, DIR_SEP, THOT_RC_FILENAME);
   else
     {
	fprintf (stderr, "Cannot save Registry no APP_HOME dir\n");
	return;
     }
   output = ufopen (filename, _WriteMODE_);
   if (output == NULL)
     {
	fprintf (stderr, "Cannot save Registry to %s :\n", filename);
	return;
     }
   SortEnv ();
   PrintEnv (output);
   AppRegistryModified = 0;
   
   fclose (output);
}


/*----------------------------------------------------------------------
  ImportRegistryFile : import a registry file.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ImportRegistryFile (STRING filename, RegistryLevel level)
#else  /* __STDC__ */
static void         ImportRegistryFile (filename, level)
STRING filename;
RegistryLevel       level;

#endif
{
   FILE*  input;
   STRING str, base;
   CHAR_T   string[1000];
   CHAR_T   appli[1000];
   STRING name;
   STRING value;

   ustrcpy (appli, THOT_LIB_DEFAULTNAME);
   input = fopen (filename, "r");
   if (input == NULL)
     {
	fprintf (stderr, "Cannot read Registry from %s :\n", filename);
	return;
     }

   while (1)
     {
	/* read one line in string buffer */
	if (ufgets (&string[0], sizeof (string) - 1, input) == NULL)
	   break;

	str = string;
	str = TtaSkipBlanks (str);
	string[sizeof (string) - 1] = EOS;

	/* Comment starts with a semicolumn */
	if (*str == TEXT(';'))
	   continue;

	/* sections are indicated between brackets, e.g. [amaya] */
	if (*str == TEXT('['))
	  {
	     str++;
	     str = TtaSkipBlanks (str);
	     base = str;
	     while ((*str != EOS) && (*str != TEXT(']')))
		str++;
	     if (*str == EOS)
	       {
		  fprintf (stderr, "Registry %s corrupted :\n\t\"%s\"\n",
			   filename, string);
		  continue;
	       }
	     *str = EOS;
	     ustrcpy (&appli[0], base);
#ifdef DEBUG_REGISTRY
	     fprintf (stderr, "TtaInitializeAppRegistry section [%s]\n", appli);
#endif
	     continue;
	  }

	/* entries have the following form : name=value */
	name = str;
	str = SkipToEqual (str);
	if (*str != TEXT('='))
	   continue;
	*str++ = EOS;
	str = TtaSkipBlanks (str);
	value = str;
	str = SkipToEqual (str);
	*str = EOS;
	AddRegisterEntry (appli, name, value, level, TRUE);
     }
   fclose (input);
}


/*----------------------------------------------------------------------
  InitEnviron : initialize the standard environment (i.e global	
  variables) with values stored in the registry.			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InitEnviron ()
#else  /* __STDC__ */
static void         InitEnviron ()
#endif				/* __STDC__ */
{
   STRING pT;
   STRING Thot_Sys_Sch;
   STRING Thot_Sch;

   /* default values for various global variables */
   FirstCreation = FALSE;
   CurSaveInterval = 0;
   pT = TtaGetEnvString ("AUTOSAVE");
   if (pT != NULL)
     CurSaveInterval = uctoi(pT);
   if (CurSaveInterval <= 0)
     CurSaveInterval = DEF_SAVE_INTVL;
   HighlightBoxErrors = FALSE;
   InsertionLevels = 4;
#ifndef _WINDOWS
   TtaSetDefEnvString ("DOUBLECLICKDELAY", "500", TRUE);
   pT = TtaGetEnvString ("DOUBLECLICKDELAY");
   if (pT != NULL)
     DoubleClickDelay = atoi(pT);
   else 
       DoubleClickDelay = 500;
#endif /* _WINDOWS */

   /* The base of the Thot directory */
   Thot_Dir = TtaGetEnvString ("THOTDIR");
   if (Thot_Dir == NULL)
      fprintf (stderr, "missing environment variable THOTDIR\n");

   /* The predefined path to documents */
   pT = TtaGetEnvString ("THOTDOC");
   if (pT == NULL)
      DocumentPath[0] = EOS; 
   else
      ustrncpy (DocumentPath, pT, MAX_PATH);

   /* Read the schemas Paths */
   Thot_Sch = TtaGetEnvString ("THOTSCH");
   Thot_Sys_Sch = TtaGetEnvString ("THOTSYSSCH");

   /* set up SchemaPath accordingly */
   if ((Thot_Sch != NULL) && (Thot_Sys_Sch != NULL))
     {
       ustrncpy (SchemaPath, Thot_Sch, MAX_PATH);
       ustrcat (SchemaPath,PATH_STR);
       ustrcat (SchemaPath, Thot_Sys_Sch);
     }
   else if (Thot_Sch != NULL)
       ustrncpy (SchemaPath, Thot_Sch, MAX_PATH);
   else if (Thot_Sys_Sch != NULL)
       ustrncpy (SchemaPath, Thot_Sys_Sch, MAX_PATH);
   else
       SchemaPath[0] = EOS;

   /* set up the default values common to all the thotlib applications */
   TtaSetDefEnvString ("LANG", TEXT("en-us"), FALSE);
   TtaSetDefEnvString ("ZOOM", TEXT("0"), FALSE);
   TtaSetDefEnvString (_TOOLTIPDELAY_EVAR_, TEXT("500"), FALSE);
   TtaSetDefEnvString ("FontMenuSize", TEXT("12"), FALSE);
   TtaSetDefEnvString (_ForegroundColor_EVAR_, TEXT("Black"), FALSE);

#ifndef _WINDOWS
   TtaSetDefEnvString ("BackgroundColor", "gainsboro", FALSE);
#else
   TtaSetDefEnvString (TEXT("BackgroundColor"), TEXT("LightGrey1"), FALSE);
#endif /* _WINDOWS */

   /*
   ** set up the defaul TMPDIR 
   */

   /* set up a default TMPDIR */
   TtaSetDefEnvString (TEXT("TMPDIR"), DEF_TMPDIR, TRUE);
   /* get the tmpdir from the registry or use the default name if it
      doesn't exist */
   pT = TtaGetEnvString ("TMPDIR");
   if (!pT)
     pT = DEF_TMPDIR;
   /* create the TMPDIR dir if it doesn't exist */
   if (!TtaMakeDirectory (pT))
     {
       /* try to use the default tmpdir */
       pT= DEF_TMPDIR;
       if (!TtaMakeDirectory (pT))
	 {
	   fprintf (stderr, "Couldn't create directory %s\n", pT);
	   exit (1);
	 }	 
       /* update the registry entry */
       TtaSetEnvString (TEXT("TMPDIR"), pT, TRUE);
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
#ifdef __STDC__
void                TtaInitializeAppRegistry (STRING appArgv0)
#else  /* __STDC__ */
void                TtaInitializeAppRegistry (appArgv0)
STRING  appArgv0;
#endif
{
   CHAR_T     app_home[MAX_PATH];
   CHAR_T     filename[MAX_PATH];
   STRING     my_path;
   STRING     dir_end = NULL;
   STRING     appName;
   CHAR_T      *ptr;
#  ifdef _WINDOWS
   ThotBool   status;
   /* name in Windows NT 4 is 20 chars */
   TCHAR username[21];
   TCHAR windir[MAX_PATH+1];
   DWORD dwSize;
#  ifndef __CYGWIN32__
   extern int _fmode;
#  endif
#  else /* ! _WINDOWS */
   struct stat         stat_buf;
#  endif /* _WINDOWS */
   int                 execname_len;
   int                 len, round;
   ThotBool             found, ok;			  
  
#  ifdef _WINDOWS
#  ifndef __CYGWIN32__
   _fmode = _O_BINARY;
#  endif  /* __CYGWIN32__ */
#  endif  /* _WINDOWS */

   if (AppRegistryInitialized != 0)
      return;
   AppRegistryInitialized++;
   /** Sanity check on the argument given. An error here should be
    * detected by programmers, since it's a application coding error.
    */
   if ((appArgv0 == NULL) || (*appArgv0 == EOS)) {
      fprintf (stderr, "TtaInitializeAppRegistry called with invalid argv[0] value\n");
      exit (1);
   }

   /*
    * We are looking for the absolute pathname to the binary of the
    * application.
    *
    * First case, the argv[0] indicate that it's an absolute path name.
    * i.e. start with / on unixes or \ or ?:\ on Windows.
    */

#  ifdef _WINDOWS
   if (appArgv0[0] == DIR_SEP || (appArgv0[1] == TEXT(':') && appArgv0[2] == DIR_SEP))
      ustrncpy (&execname[0], appArgv0, sizeof (execname));
#  else  /* !_WINDOWS */
   if (appArgv0[0] == DIR_SEP)
      ustrncpy (&execname[0], appArgv0, sizeof (execname));
#  endif /* _WINDOWS */
   
   /*
    * second case, the argv[0] indicate a relative path name.
    * The exec name is obtained by appending the current directory.
    */
   else if (TtaFileExist (appArgv0)) {
        ugetcwd (&execname[0], sizeof (execname));
        ustrcat (execname, DIR_STR);
        ustrcat (execname, appArgv0);
   } else {
          /*
           * Last case, we were just given the application name.
           * Use the PATH environment variable to search the exact binary location.
           */
          my_path = ugetenv (_PATH_EVAR_);
          if (my_path == NULL) {
             fprintf (stderr, "TtaInitializeAppRegistry cannot found PATH environment\n");
             exit (1);
		  }
          /*
           * make our own copy of the string, in order to preserve the
           * enviroment variables. Then search for the binary along the
           * PATH.
           */
          len = sizeof (path) - 1;
          ustrncpy (path, my_path, len);
          path[len] = EOS;
       
          execname_len = sizeof (execname);
#         ifdef _WINDOWS
          MakeCompleteName (appArgv0, EXE_EXT2, path, execname, &execname_len);
#         else
          MakeCompleteName (appArgv0, "", path, execname, &execname_len);
#         endif
          if (execname[0] == EOS) {
             fprintf (stderr, "TtaInitializeAppRegistry internal error\n");
             fprintf (stderr, "\tcannot find path to binary : %s\n", appArgv0);
             exit (1);
		  } 
   } 
   
   /*
    * Now that we have a complete path up to the binary, extract the
    * application name.
    */
   appName = &execname[0];
   while (*appName) /* go to the ending NUL */
         appName++;
   
   do
     appName--;
   while (appName > execname && *appName != DIR_SEP);

   if (*appName == DIR_SEP)
      /* dir_end used for relative links ... */
      dir_end = appName++;
   
   appName = TtaStrdup (appName);
#  ifdef _WINDOWS
   /* remove the .exe extension. */
   ptr = ustrchr (appName, TEXT('.'));
   if (ptr && !ustrcasecmp (ptr, EXE_EXT))
      *ptr = EOS;
   ptr = appName;
   while (*ptr) {
         *ptr = utolower (*ptr);
         ptr++;
   }
   
#  endif /* _WINDOWS */
   AppRegistryEntryAppli = appName;
   
#  ifdef HAVE_LSTAT
   /*
    * on Unixes, the binary path started may be a softlink
    * to the real app in the real dir.
    */
   if (lstat (execname, &stat_buf) == 0 && S_ISLNK (stat_buf.st_mode)) {
      len = readlink (execname, filename, sizeof (filename));
      if (len > 0) {
         filename[len] = 0;
         /*
          * Two cases : can be an absolute link to the binary
          * or a relative link.
          */
	     if (filename[0] == DIR_SEP)
            ustrcpy (execname, filename);
         else
             ustrcpy (dir_end + 1, filename);
	  } 
   } 
#  endif /* HAVE_LSTAT */
   
#  ifdef DEBUG_REGISTRY
   fprintf (stderr, "path to binary %s : %s\n", appName, execname);
#  endif
   
   /* get the THOTDIR for this application. It's under a bin dir */
   dir_end = execname;
   while (*dir_end) /* go to the ending NUL */
         dir_end++;
   
   /* remove the application name */
   ok = FALSE;
   do {
      dir_end--;
      ok = (dir_end <= execname || *dir_end == DIR_SEP);
   } while (!ok);

   if (*dir_end == DIR_SEP) {
      /* the name has been found */
      found = TRUE;
      *dir_end = EOS;
      /* save the binary directory in BinariesDirectory */
      ustrncpy (BinariesDirectory, execname, sizeof (BinariesDirectory));

      /* remove the binary directory */
      found = FALSE;
      ok = FALSE;
      round = 2;
      while (!found || round != 0) {
            do {
               dir_end--;
               ok = (dir_end <= execname || *dir_end == DIR_SEP);
			} while (!ok);

            if (*dir_end == DIR_SEP) {
               *dir_end = EOS;
               if (!ustrcmp (&dir_end[1], TEXT("..")))
                  round ++;
               else if (ustrcmp (&dir_end[1], TEXT("."))) {
                    round --;
                    /* a directory name has been found */
                    found = TRUE;
			   } 
			} else {
                   /* no directory has been found */
                   found = TRUE;
                   ok = FALSE;
			}
	  } 
      if (ok) {
         *dir_end = EOS;
         if (IsThotDir (execname))
            AddRegisterEntry (TEXT("System"), "THOTDIR", execname, REGISTRY_INSTALL, TRUE);
	  }
#     ifdef COMPILED_IN_THOTDIR
      /* Check a compiled-in value */
      else if (IsThotDir (COMPILED_IN_THOTDIR)) {
           ustrcpy(execname, COMPILED_IN_THOTDIR);
           AddRegisterEntry ("System", "THOTDIR", COMPILED_IN_THOTDIR, REGISTRY_INSTALL, TRUE);
	  } 
#     else /* COMPILED_IN_THOTDIR */
#     ifdef COMPILED_IN_THOTDIR2
      /* Check a compiled-in value */
      else if (IsThotDir (COMPILED_IN_THOTDIR2)) {
           ustrcpy(execname, COMPILED_IN_THOTDIR2);
           AddRegisterEntry ("System", "THOTDIR", COMPILED_IN_THOTDIR2, REGISTRY_INSTALL, TRUE);
	  }
#     endif /* COMPILED_IN_THOTDIR2 */
#     endif /* COMPILED_IN_THOTDIR */
      else {
           fprintf (stderr, "Cannot find THOTDIR\n");
           exit (1);
	  } 
   } 
   
#  ifdef MACHINE
   /* if MACHINE is set up, add it to the registry */
   AddRegisterEntry ("System", "MACHINE", MACHINE, REGISTRY_INSTALL, TRUE);
#  endif

   /* load the system settings, stored in THOTDIR/config/thot.ini */
   usprintf (filename, TEXT("%s%c%s%c%s"), execname, DIR_SEP, THOT_CONFIG_FILENAME, DIR_SEP, THOT_INI_FILENAME);
   if (TtaFileExist (filename)) {
#     ifdef DEBUG_REGISTRY
      fprintf (stderr, "reading system %s from %s\n", THOT_INI_FILENAME, filename);
#     endif
      ImportRegistryFile (filename, REGISTRY_SYSTEM);
      *dir_end = EOS;
      dir_end -= 3;
   } else
         fprintf (stderr, "System wide %s not found at %s\n", THOT_INI_FILENAME, &filename[0]);

   /*
   ** find the APP_HOME directory name:
   ** Unix: $HOME/.appname,
   ** Win95: $THOTDIR/users/login-name/
   ** WinNT: c:\WINNT\profiles\login-name
   */
   /* No this should NOT be a call to TtaGetEnvString */
# ifdef _WINDOWS
   /* compute the default app_home value from the username and thotdir */
   dwSize = sizeof (username);
   status = GetUserName (username, &dwSize);
   if (status)
     ptr = (CHAR_T *) username;
   else
     /* under win95, there may be no user name */
     ptr = WIN_DEF_USERNAME;
   if (IS_NT)
     /* winnt: apphome is windowsdir\profiles\username\appname */
     {
       dwSize = MAX_PATH;
       GetWindowsDirectory (windir, dwSize);
       usprintf (app_home, TEXT("%s\\profiles\\%s\\%s"), windir, ptr, 
		 AppRegistryEntryAppli);
     }
   else
     /* win95: apphome is  thotdir\users\username */
     usprintf (app_home, TEXT("%s\\%s\\%s"), execname, WIN_USERS_HOME_DIR, ptr);   
# else /* !_WINDOWS */
   ptr = getenv ("HOME");
   usprintf (app_home, "%s%c.%s", ptr, DIR_SEP, AppRegistryEntryAppli); 
#endif _WINDOWS
   /* store the value of APP_HOME in the registry */
   AddRegisterEntry (AppRegistryEntryAppli, "APP_HOME", app_home, REGISTRY_SYSTEM, TRUE);

   /* set the default APP_TMPDIR */
#ifdef _WINDOWS
   /* the tmpdir is DEF_TMPDIR\app-name */
   usprintf (filename, TEXT("%s%c%s"), DEF_TMPDIR, DIR_SEP, AppRegistryEntryAppli);
   AddRegisterEntry (AppRegistryEntryAppli, TEXT("APP_TMPDIR"), filename,
		     REGISTRY_SYSTEM, TRUE);
#else
   /* under Unix, APP_TMPDIR == APP_HOME */
   AddRegisterEntry (AppRegistryEntryAppli, "APP_TMPDIR", app_home,
		     REGISTRY_SYSTEM, TRUE);
#endif /* _WINDOWS */
   /* read the user's preferences (if they exist) */
   if (app_home != NULL && *app_home != EOS)
     {
       usprintf (filename, TEXT("%s%c%s"), app_home, DIR_SEP, THOT_RC_FILENAME);
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
#  endif
   
#  ifdef DEBUG_REGISTRY
   PrintEnv (stderr);
#  endif
   InitEnviron ();
   AppRegistryModified = 0;
}

/*----------------------------------------------------------------------
  TtaFreeAppRegistry : frees the memory associated with the
  registry
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaFreeAppRegistry (void)
#else  /* __STDC__ */
void                TtaFreeAppRegistry ()
#endif
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
  AppRegistryInitialized = 0;
}

/*----------------------------------------------------------------------
   SearchFile look for a file following the guideline given by dir
   Returns 1 with fullName set to the absolute pathname if found,
   0 otherwise.                                   
   Depending on dir value, the file is looked for in:
   - 0 : /                                                 
   - 1 : ThotDir                                           
   - 2 : ThotDir/bin                                       
   - 3 : ThotDir/compil                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 SearchFile (STRING fileName, int dir, STRING fullName)
#else  /* __STDC__ */
int                 SearchFile (fileName, dir, fullName)
STRING fileName;
int                 dir;
STRING fullName;
#endif /* __STDC__ */
{
   CHAR_T   tmpbuf[200];
   STRING imagepath;
   int                 i, j;
   int                 ret;

   if (Thot_Dir != NULL)
      ustrcpy (fullName, Thot_Dir);
   else
      *fullName = EOS;
   switch (dir)
	 {
	    case 1:
	       /* Lookup in schema and documents path */
	       ustrcat (fullName, fileName);
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
		    usprintf (fullName, TEXT("%s%s%s"), tmpbuf, DIR_STR, fileName);
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
		    usprintf (fullName, TEXT("%s%s%s"), tmpbuf, DIR_STR, fileName);
		    ret = TtaFileExist (fullName);
		 }
	       break;

	    case 2:
	       /* lookup in config */
	       ustrcat (fullName, DIR_STR);
	       ustrcat (fullName, TEXT("config"));
	       ustrcat (fullName, DIR_STR);
	       ustrcat (fullName, fileName);
	       break;

	    case 3:
	       /* lookup in batch */
	       ustrcat (fullName, DIR_STR);
	       ustrcat (fullName, TEXT("batch"));
	       ustrcat (fullName, DIR_STR);
	       ustrcat (fullName, fileName);
	       break;

	    default:
	       ustrcat (fullName, DIR_STR);
	       ustrcat (fullName, fileName);
	 }

   /* general search */
   ret = TtaFileExist (fullName);
   if (ret == 0)
     {
	ustrcpy (fullName, fileName);
	ret = TtaFileExist (fullName);
     }
   return ret;
}
