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
#include "memory_f.h"
#include "platform_f.h"
#include "ustring_f.h"
#include "uconvert_f.h"
/* #define DEBUG_REGISTRY enable the Registry debug messages */


/* for Marc.Baudoin@hsc.fr (Marc Baudoin) */
#ifdef _WINDOWS
#include <direct.h>
#include <winbase.h>
#include "winsys.h"
#   define WIN_DEF_USERNAME       CUSTEXT("default")
#   define THOT_INI_FILENAME      CUSTEXT("win-thot.rc")
#   define WIN_USERS_HOME_DIR     TEXT("users")
#   define DEF_TMPDIR             CUSTEXT("c:\\temp")
STRING WIN_Home;
#else /* !_WINDOWS */
#   define THOT_INI_FILENAME      TEXT("unix-thot.rc")
#   define DEF_TMPDIR             "/tmp"
#endif /* ! _WINDOWS */

#define THOT_RC_FILENAME      CUSTEXT("thot.rc")
#define THOT_CONFIG_FILENAME  CUSTEXT("config")
#define THOT_BIN_FILENAME     TEXT("bin")
#define THOT_LIB_DEFAULTNAME  "thot_lib"
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
     struct struct_RegistryEntry* next;  /* chaining ! */
     RegistryLevel                level; /* exact level */
     char*                        appli; /* corresponding section */
     char*                        name;  /* name of the entry     */
     CharUnit*                    orig;  /* the original value (to be saved back) */
     CharUnit*                    value; /* user-level value */
  }
RegistryEntryBlk   , *RegistryEntry;

static int           AppRegistryInitialized = 0;
static int           AppRegistryModified = 0;
static RegistryEntry AppRegistryEntry = NULL;
static char*         AppRegistryEntryAppli = (char*) 0;
static CharUnit*     AppNameW;
static CharUnit      CurrentDir[MAX_PATH];
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
static char*       SkipToEqual (char* ptr)
#else
static char*       SkipToEqual (ptr)
char* ptr;
#endif
{
  while (*ptr != EOS && *ptr != '=' && *ptr != EOL && *ptr != __CR__)
    ptr++;
  return (ptr);
}

/*----------------------------------------------------------------------
   TtaSkipBlanks skips all spaces, tabs, linefeeds and newlines at the
   beginning of the string and returns the pointer to the new position. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char* TtaSkipBlanks (char* ptr)
#else
char* TtaSkipBlanks (ptr)
char* ptr;
#endif
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
#ifdef __STDC__
ThotBool            TtaIsBlank (char* ptr)
#else
ThotBool            TtaIsBlank (ptr)
char*               ptr;
#endif
{
  if (*ptr == SPACE || *ptr == BSPACE || *ptr == EOL ||
      *ptr == TAB || *ptr == __CR__)
    return (TRUE);
  else
    return (FALSE);
}

/*----------------------------------------------------------------------
   TtaIsCUSBlank returns True if the first character (CharUnit) is a 
   space, a tab, a linefeed or a newline.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            TtaIsCUSBlank (CharUnit* ptr)
#else
ThotBool            TtaIsCUSBlank (ptr)
CharUnit*           ptr;
#endif
{
  if (*ptr == CUS_SPACE || *ptr == CUS_BSPACE || *ptr == CUS_EOL ||
      *ptr == CUS_TAB || *ptr == CUS_CR)
    return (TRUE);
  else
    return (FALSE);
}

/*----------------------------------------------------------------------
   TtaIsWCBlank returns True if the first character (CHAR_T) is a space, 
   a tab, a linefeed or a newline.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            TtaIsWCBlank (CharUnit* ptr)
#else
ThotBool            TtaIsWCBlank (ptr)
CharUnit*           ptr;
#endif
{
  if (*ptr == WC_SPACE || *ptr == WC_BSPACE || *ptr == WC_EOL ||
      *ptr == WC_TAB || *ptr == WC_CR)
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
static void         DoVariableSubstitution (CharUnit* input, int i_len,
					    CharUnit* output, int o_len)
#else  /* __STDC__ */
static void         DoVariableSubstitution (input, i_len, output, o_len)
CharUnit*           input;
int                 i_len;
CharUnit*           output;
int                 o_len;
#endif
{
  CharUnit* cour = input;
  CharUnit* base = input;
  CharUnit* res  = output;
  CharUnit* value;
  CharUnit  save;
# if defined(_WINDOWS) && defined(_I18N_)
  char      baseA [MAX_LENGTH];
# endif /* _WINDOWS */

#define CHECK_OVERFLOW (((cour - input) > i_len) || ((res - output) >= (o_len - 1)))

  while (*cour)
    {
      if CHECK_OVERFLOW
	break;
      if (*cour != CUSTEXT('$'))
	{
	  *res++ = *cour++;
	  continue;
	}
      base = cour;		/* save the position to the $ */
      cour++;
      if CHECK_OVERFLOW
	break;
      if (*cour != CUSTEXT('('))
	{
	  *res++ = CUSTEXT('$');
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
      while (*cour != CUSTEXT(')') && !TtaIsCUSBlank (cour));
      if CHECK_OVERFLOW
	break;
      
      save = *cour;
      *cour = CUS_EOS;
      if (save != CUSTEXT(')'))
	fprintf (stderr, "invalid variable name %s in %s\n", base, THOT_INI_FILENAME);

      /* We are ready to fetch the base value from the Registry */
#     if defined(_WINDOWS) && defined(_I18N_)
      wc2iso_strcpy (baseA, base);
      value = TtaGetEnvString (baseA);
#     else  /* !(defined(_WINDOWS) && defined(_I18N_)) */
      value = TtaGetEnvString (base);
#     endif /* !(defined(_WINDOWS) && defined(_I18N_)) */
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
  *res = CUS_EOS;
}


/*----------------------------------------------------------------------
 NewRegisterEntry : add a fresh new entry in the Register.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          NewRegisterEntry (char* appli, char* name, CharUnit* value,
				      RegistryLevel level)
#else  /* __STDC__ */
static int          NewRegisterEntry (appli, name, value, level)
char*               appli;
char*               name;
CharUnit*           value;
RegistryLevel       level;

#endif
{
   CharUnit      resu[2000];
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
   DoVariableSubstitution (value, StringLength (value), resu, sizeof (resu) / sizeof (CharUnit));

   /*
    * allocate an entry, fill it and chain it.
    */

   cour = (RegistryEntry) TtaGetMemory (sizeof (RegistryEntryBlk));
   if (cour == NULL)
      return (-1);
   cour->appli = TtaStrdup (appli);
   cour->name  = TtaStrdup (name);
   cour->orig  = StringDuplicate (value);
   cour->value = StringDuplicate (resu);
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
static int    AddRegisterEntry (char* appli, char *name, CharUnit* value,
				      RegistryLevel level, int overwrite)
#else  /* __STDC__ */
static int    AddRegisterEntry (appli, name, value, level, overwrite)
char*         appli;
char*         name;
CharUnit*     value;
RegistryLevel level;
int           overwrite;
#endif
{
  CharUnit      resu[2000];
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

   if (cour != NULL)
     {
       /* there is aleady an entry */
       if (!overwrite)
	 return (0);

       DoVariableSubstitution (value, StringLength (value), resu, sizeof (resu));
       if (cour->value)
	 TtaFreeMemory (cour->value);
       if (cour->orig)
	 TtaFreeMemory (cour->orig);
       cour->orig = StringDuplicate (value);
       cour->value = StringDuplicate (resu);
       AppRegistryModified++;
     }
   else
     {
       /*
	* If the value is empty, we add it only if it's not present
	* in the thot library section.
	*/
       if (!overwrite && (value == NULL || *value == CUS_EOS))
	 {
	   cour = AppRegistryEntry;
	   while (cour != NULL)
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
      if (!strcasecmp (cour->appli, "System"))
	{
	  cour = cour->next;
	  continue;
	}

      /* print out the section */
      fprintf (output, "[%s]\n", cour->appli);
      /* add all the entries under the same appli name */
      if (cour->level == REGISTRY_USER 
		  && strcasecmp (cour->name, "TMPDIR")
		  && strcasecmp (cour->name, "APP_TMPDIR")
		  && strcasecmp (cour->name, "APP_HOME"))
	fprintf (output, "%s=%s\n", cour->name, cour->orig);
      next = cour->next;
      while (next != NULL && !strcasecmp (next->appli, cour->appli))
	{
	  if (next->level == REGISTRY_USER
	      && strcasecmp (cour->name, "TMPDIR")
#ifndef _WINDOWS
	      && ustrcasecmp (next->name, "APP_TMPDIR")
#endif /* _WINDOWS */
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
#ifdef __STDC__
ThotBool TtaGetEnvInt (char* name, int *value)
#else
ThotBool TtaGetEnvInt (name, value)
char*  name;
int *value;
#endif /* __STDC__ */
{
  CharUnit* strptr;

  if (!name || *name == EOS)
   {
     *value = 0;
     return FALSE;
   }

  strptr = TtaGetEnvString (name);

  /* the name entry doesn't exist */
  if (!strptr || *strptr == CUS_EOS)
   {
     *value = 0;
     return FALSE;
   }

 /* make the convertion */
 *value = custoi (strptr);

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
 CharUnit* strptr;

 if (!name || *name == EOS)
   {
     *value = FALSE;
     return FALSE;
   }

 strptr = TtaGetEnvString (name);

 /* the name entry doesn't exist */
 if (!strptr || *strptr == CUS_EOS)
   {
     *value = FALSE;
     return FALSE;
   }

 /* make the convertion */
 if ( StringCaseCompare (strptr, CUSTEXT("yes")))
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
CharUnit* TtaGetEnvString (char* name)
#else  /* __STDC__ */
CharUnit* TtaGetEnvString (name)
char* name;
#endif
{
   RegistryEntry cour;
   CharUnit*     value;
#  if defined(_WINDOWS) && defined(_I18N_)
   CharUnit*     tmp;   
#  endif /* !(if defined(_WINDOWS) && defined(_I18N_)) */

   if (AppRegistryInitialized == 0)
#     if defined(_WINDOWS) && defined(_I18N_)
      {
         CharUnit* tmp = TtaAllocCUString (strlen (name) + 1);
         iso2wc_strcpy (tmp, name);
         value = _wgetenv (tmp);
         TtaFreeMemory (tmp);
         return value;
      }
#     else /* !(defined(_WINDOWS) && defined(_I18N_)) */
      return (getenv (name));
#     endif /* !(defined(_WINDOWS) && defined(_I18N_)) */

      /* appname allows to get the application name */
   if (!strcasecmp ("appname", name))
#     if defined(_WINDOWS) && defined(_I18N_)
      {
          CharUnit* AppRegistryEntryAppliW = iso2wc_strdup (AppRegistryEntryAppli);
          return AppRegistryEntryAppliW;
      }
#     else  /* !(defined(_WINDOWS) && defined(_I18N_)) */ 
      return(AppRegistryEntryAppli);
#     endif /* !(defined(_WINDOWS) && defined(_I18N_)) */

   if ((!strcasecmp (name, "cwd")) || (!strcasecmp (name, "pwd")))
      return (cus_getcwd (&CurrentDir[0], sizeof(CurrentDir)));

   /* first lookup in the System defaults */
   cour = AppRegistryEntry;
   while (cour != NULL) {
         if (!strcasecmp (cour->appli, "System") && !strcmp (cour->name, name) && cour->value[0] != CUS_EOS) {
#           ifdef DEBUG_REGISTRY
            fprintf (stderr, "TtaGetEnvString(\"%s\") = %s\n", name, cour->value);
#           endif
            return (cour->value);
		 }
         cour = cour->next;
   }

   /* then lookup in the application user preferences */
   cour = AppRegistryEntry;
   while (cour != NULL) {
         if (!strcasecmp (cour->appli, AppRegistryEntryAppli)    && 
             !strcmp (cour->name, name) && cour->value[0] != CUS_EOS && 
             cour->level == REGISTRY_USER) {
#           ifdef DEBUG_REGISTRY
            fprintf (stderr, "TtaGetEnvString(\"%s\") = %s\n", name, cour->value);
#           endif
            return (cour->value);
		 }
         cour = cour->next;
   } 

   /* then lookup in the application defaults */
   cour = AppRegistryEntry;
   while (cour != NULL) {
         if (!strcasecmp (cour->appli, AppRegistryEntryAppli)    && 
             !strcmp (cour->name, name) && cour->value[0] != CUS_EOS && 
             cour->level == REGISTRY_SYSTEM) {
#           ifdef DEBUG_REGISTRY
            fprintf (stderr, "TtaGetEnvString(\"%s\") = %s\n", name, cour->value);
#           endif
            return (cour->value);
		 }
         cour = cour->next;
   } 

   /* then lookup in the Thot library defaults */
   cour = AppRegistryEntry;
   while (cour != NULL) {
         if (!strcasecmp (cour->appli, THOT_LIB_DEFAULTNAME) && !strcmp (cour->name, name) && cour->value[0] != CUS_EOS) {
#           ifdef DEBUG_REGISTRY
            fprintf (stderr, "TtaGetEnvString(\"%s\") = %s\n", name, cour->value);
#           endif
            return (cour->value);
		 }
         cour = cour->next;
   }

   /*
    * If still not found, look in the environment variables.
    * Hopefully this will be stored to the user registry
    * next time it will be saved.
    */

#  if defined(_WINDOWS) && defined(_I18N_)
   tmp = TtaAllocCUString (strlen (name) + 1);
   iso2wc_strcpy (tmp, name);
   value = _wgetenv (tmp);
   TtaFreeMemory (tmp);
#  else /* !(defined(_WINDOWS) && defined(_I18N_)) */
   value = getenv (name);
#  endif /* !(defined(_WINDOWS) && defined(_I18N_)) */

   if (value == NULL)
      TtaSetEnvString (name, CUSTEXT(""), FALSE); 
   else
       TtaSetEnvString (name, value, FALSE);
  
#  ifdef DEBUG_REGISTRY
   fprintf (stderr, "TtaGetEnvString(\"%s\") = %s\n", name, value);
#  endif
   return (value);
}

/*----------------------------------------------------------------------
 TtaClearEnvString : clears the value associated with an environment
                     string, in the user registry.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaClearEnvString (char *name)
#else  /* __STDC__ */
void                TtaClearEnvString (name)
CONST char         *name;
CONST STRING        value;
int                 overwrite;
#endif
{
   AddRegisterEntry (AppRegistryEntryAppli, name, CUSTEXT(""), REGISTRY_USER, TRUE);
}

/*----------------------------------------------------------------------
 TtaSetEnvInt : set the value associated to an environment string,
                for the current application.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetEnvInt (char *name, int value, int overwrite)
#else  /* __STDC__ */
void                TtaSetEnvInt (name, value, overwrite)
CONST char         *name;
CONST int           value;
int                 overwrite;
#endif
{
   /* hardcoded so that the biggest integer value has 5 digits: 65535 */
   CharUnit ptr[6];
   int      r_val;

   r_val = value % 65537;
   cus_sprintf (ptr, CUSTEXT("%d"), r_val);
   AddRegisterEntry (AppRegistryEntryAppli, name, ptr, REGISTRY_USER, overwrite);
}

/*----------------------------------------------------------------------
 TtaSetEnvBoolean : set the value associated to an environment string,
                    for the current application.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetEnvBoolean (char *name, ThotBool value, int overwrite)
#else  /* __STDC__ */
void                TtaSetEnvBoolean (name, value, overwrite)
CONST char         *name;
CONST ThotBool      value;
int                 overwrite;
#endif
{
   CharUnit* ptr;

   if (value)
      ptr = CUSTEXT("yes");
   else
       ptr = CUSTEXT("no");
   AddRegisterEntry (AppRegistryEntryAppli, name, ptr, REGISTRY_USER, overwrite);
}

/*----------------------------------------------------------------------
 TtaSetEnvString : set the value associated to an environment string,
                   for the current application.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetEnvString (char *name, CharUnit* value, int overwrite)
#else  /* __STDC__ */
void                TtaSetEnvString (name, value, overwrite)
CONST char*         name;
CONST CharUnit*     value;
int                 overwrite;
#endif
{
   CharUnit* tmp = value;
  
   if (!tmp)
      tmp = CUSTEXT("");
   AddRegisterEntry (AppRegistryEntryAppli, name, value, REGISTRY_USER, overwrite);
}

/*----------------------------------------------------------------------
 TtaSetDefEnvString : set the defaul value associated to an environment
                      string, for the current application.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetDefEnvString (char *name, CharUnit* value, 
					int overwrite)
#else  /* __STDC__ */
void                TtaSetDefEnvString (name, value, overwrite)
CONST char*         name;
CONST char*         value;
int                 overwrite;
#endif
{
  CharUnit* tmp = value; /* ??? */
                         /* ??? */
  if (!tmp)              /* ??? */
    tmp = CUSTEXT("");   /* ??? */

  AddRegisterEntry (AppRegistryEntryAppli, name, value, REGISTRY_SYSTEM, overwrite);
}

/*----------------------------------------------------------------------
  TtaGetDefEnvInt : read the default integer value associated to an 
  environment string.
  Returns TRUE if the env variables exists or FALSE if it isn't the case.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool TtaGetDefEnvInt (char* name, int *value)
#else
ThotBool TtaGetDefEnvInt (name, value)
STRING name;
int *value;
#endif /* __STDC__ */
{
 CharUnit* strptr;

 if (!name || *name == EOS)
   {
     *value = 0;
     return FALSE;
   }

 strptr = TtaGetDefEnvString (name);

 /* the name entry doesn't exist */
 if (!strptr || *strptr == CUS_EOS)
   {
     *value = 0;
     return FALSE;
   }

 /* make the convertion */
 *value = custoi (strptr);

 return TRUE;
}

/*----------------------------------------------------------------------
  TtaGetDefEnvBoolean : read the ThotBool value associated to an 
  environment string.
  Returns TRUE if the env variables exists or FALSE if it isn't the case.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool TtaGetDefEnvBoolean (char* name, ThotBool *value)
#else
ThotBool TtaGetDefEnvBoolean (name, value)
char*     name;
ThotBool* value;
#endif /* __STDC__ */
{
 CharUnit* strptr;

 if (!name || *name == EOS)
   {
     *value = FALSE;
     return FALSE;
   }

 strptr = TtaGetDefEnvString (name);

 /* the name entry doesn't exist */
 if (!strptr || *strptr == CUS_EOS)
   {
     *value = FALSE;
     return FALSE;
   }

 /* make the convertion */
 if (StringCaseCompare (strptr, CUSTEXT("yes")))
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
CharUnit* TtaGetDefEnvString (char* name)
#else  /* __STDC__ */
CharUnit* TtaGetDefEnvString (name)
char* name;
#endif
{
  RegistryEntry cour;
  CharUnit*     value;
# if defined(_WINDOWS) && defined(_I18N_)
  CharUnit      tmp [MAX_LENGTH];
# endif /* defined(_WINDOWS) && defined(_I18N_) */

  if (AppRegistryInitialized == 0)
#    if defined(_WINDOWS) && defined(_I18N_)
     {
        CharUnit* tmp = TtaAllocCUString (strlen (name) + 1);
        iso2wc_strcpy (tmp, name);
        value = _wgetenv (tmp);
        TtaFreeMemory (tmp);
        return value;
     }
#    else /* !(defined(_WINDOWS) && defined(_I18N_)) */
     return (getenv (name));
#    endif /* !(defined(_WINDOWS) && defined(_I18N_)) */

  /* appname allows to get the application name */
  if (!strcasecmp ("appname", name))
#    ifdef _WINDOWS
     {
         CharUnit* AppRegistryEntryAppliW = iso2wc_strdup (AppRegistryEntryAppli);
         return AppRegistryEntryAppliW;
     }
#    else  /* !_WINDOWS */
     return(AppRegistryEntryAppli);
#    endif /* !_WINDOWS */

  if ((!strcasecmp (name, "cwd")) || (!strcasecmp (name, "pwd")))
    {
      return (cus_getcwd (&CurrentDir[0], sizeof(CurrentDir)));
    }

  /* First lookup in the System defaults */
  cour = AppRegistryEntry;
  while (cour != NULL)
    {
      if (!strcasecmp (cour->appli, "System") && !strcmp (cour->name, name) 
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
      if (!strcasecmp (cour->appli, AppRegistryEntryAppli) 
	  && !strcmp (cour->name, name) 
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
      if (!strcasecmp (cour->appli, THOT_LIB_DEFAULTNAME) 
	  && !strcmp (cour->name, name) 
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
# if defined(_WINDOWS) && defined(_I18N_)
  iso2wc_strcpy (tmp, name);
  value = cus_getenv (tmp);
# else /* !(defined(_WINDOWS) && defined(_I18N_)) */
  value = getenv (name);
# endif /* !(defined(_WINDOWS) && defined(_I18N_)) */
  
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
static int          IsThotDir (CONST CharUnit* path)
#else  /* __STDC__ */
static int          IsThotDir (path)
CONST CharUnit*     path;
#endif				/* __STDC__ */
{
   CharUnit         filename[MAX_PATH];

   if (path == NULL)
      return (0);
   StringCopy (filename, path);
   StringConcat (filename, CUS_DIR_STR);
   StringConcat (filename, THOT_CONFIG_FILENAME);
   StringConcat (filename, CUS_DIR_STR);
   StringConcat (filename, THOT_INI_FILENAME);
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
  
  usprintf (textKey, TEXT("%s\\%s\\%s"), userBase, AppNameW, env);	                    
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

   usprintf (textKey,TEXT("%s\\%s\\%s"), userBase, AppNameW, key);	                    
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
   output = ufopen (filename, CUSTEXT("w"));
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
static void         ImportRegistryFile (CharUnit* filename, RegistryLevel level)
#else  /* __STDC__ */
static void         ImportRegistryFile (filename, level)
CharUnit*     filename;
RegistryLevel level;

#endif
{
   FILE*     input;
   char*     str; 
   char*     base;
   char      string[MAX_LENGTH];
   char      appli[MAX_LENGTH];
   char*     name;
   char*     value;
#  if defined(_WINDOWS) && defined(_I18N_)
   CharUnit* cus_value;
#  endif /* defined(_WINDOWS) && defined(_I18N_) */
   strcpy (appli, THOT_LIB_DEFAULTNAME);
   input = cus_fopen (filename, CUSTEXT("r"));
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
	str = TtaSkipBlanks (str);
	string[sizeof (string) - 1] = EOS;

	/* Comment starts with a semicolumn */
	if (*str == ';' || *str == '#')
	   continue;

	/* sections are indicated between brackets, e.g. [amaya] */
	if (*str == '[')
	  {
	     str++;
	     str = TtaSkipBlanks (str);
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
	str = SkipToEqual (str);
	if (*str != '=')
	   continue;
	*str++ = EOS;
	str = TtaSkipBlanks (str);
	value = str;
	str = SkipToEqual (str);
	*str = EOS;
#   if defined(_WINDOWS) && defined(_I18N_)
    cus_value = TtaAllocCUString (strlen (value) + 1);
    iso2wc_strcpy (cus_value, value);
	AddRegisterEntry (appli, name, cus_value, level, TRUE);
#   else /* !(defined(_WINDOWS) && defined(_I18N_)) */
	AddRegisterEntry (appli, name, value, level, TRUE);
#   endif /* !(defined(_WINDOWS) && defined(_I18N_)) */
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
   CharUnit* pT;
   CharUnit* Thot_Sys_Sch;
   CharUnit* Thot_Sch;

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
       StringNCopy (SchemaPath, Thot_Sch, MAX_PATH);
       StringConcat (SchemaPath,CUS_PATH_STR);
       StringConcat (SchemaPath, Thot_Sys_Sch);
     }
   else if (Thot_Sch != NULL)
       StringNCopy (SchemaPath, Thot_Sch, MAX_PATH);
   else if (Thot_Sys_Sch != NULL)
       StringNCopy (SchemaPath, Thot_Sys_Sch, MAX_PATH);
   else
       SchemaPath[0] = CUS_EOS;

   /* set up the default values common to all the thotlib applications */
   TtaSetDefEnvString ("LANG", CUSTEXT("en-us"), FALSE);
   TtaSetDefEnvString ("ZOOM", CUSTEXT("0"), FALSE);
   TtaSetDefEnvString ("TOOLTIPDELAY", CUSTEXT("500"), FALSE);
   TtaSetDefEnvString ("FontMenuSize", CUSTEXT("12"), FALSE);
   TtaSetDefEnvString ("ForegroundColor", CUSTEXT("Black"), FALSE);

#ifndef _WINDOWS
   TtaSetDefEnvString ("BackgroundColor", CUSTEXT("gainsboro"), FALSE);
#else
   TtaSetDefEnvString ("BackgroundColor", CUSTEXT("LightGrey1"), FALSE);
#endif /* _WINDOWS */

   /*
   ** set up the defaul TMPDIR 
   */

   /* set up a default TMPDIR */
   TtaSetDefEnvString ("TMPDIR", DEF_TMPDIR, TRUE);
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
       TtaSetEnvString ("TMPDIR", pT, TRUE);
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
void                TtaInitializeAppRegistry (CharUnit* appArgv0)
#else  /* __STDC__ */
void                TtaInitializeAppRegistry (appArgv0)
CharUnit*           appArgv0;
#endif
{
  CharUnit    app_home[MAX_PATH];
  CharUnit    filename[MAX_PATH];
  CharUnit*   my_path;
  CharUnit*   dir_end = NULL;
  CharUnit*   appName;
  CharUnit*   ptr;
# ifdef _WINDOWS
  /* name in Windows NT 4 is 20 chars */
  CharUnit    username[MAX_LENGTH];
  CharUnit    windir[MAX_PATH+1];
  DWORD       dwSize;
# ifndef __CYGWIN32__
  extern int    _fmode;
# endif /* __CYGWIN32__ */
  ThotBool    status;
#else /* ! _WINDOWS */
  struct stat stat_buf;
#endif /* _WINDOWS */
  int         execname_len;
  int         len, round;
  ThotBool    found, ok;
  
#ifdef _WINDOWS
#ifndef __CYGWIN32__
  _fmode = _O_BINARY;
#endif /* __CYGWIN32__ */
#endif /* _WINDOWS */
  if (AppRegistryInitialized != 0)
    return;
  AppRegistryInitialized++;
  /* Sanity check on the argument given. An error here should be
   * detected by programmers, since it's a application coding error.
   */
  if ((appArgv0 == NULL) || (*appArgv0 == CUS_EOS))
    {
#     ifdef _WINDOWS
      MessageBox (NULL, TEXT("TtaInitializeAppRegistry called with invalid argv[0] value"), TEXT("Amaya"), MB_OK);
#     else  /* !_WINDOWS */
      fprintf (stderr, "TtaInitializeAppRegistry called with invalid argv[0] value\n");
#     endif /* !_WINDOWS */
      exit (1);
    }

  /*
   * We are looking for the absolute pathname to the binary of the
   * application.
   *
   * First case, the argv[0] indicate that it's an absolute path name.
   * i.e. start with / on unixes or \ or ?:\ on Windows.
   */

# ifdef _WINDOWS
  if (appArgv0[0] == CUS_DIR_SEP || (appArgv0[1] == CUSTEXT(':') && appArgv0[2] == CUS_DIR_SEP))
     StringNCopy (&execname[0], appArgv0, sizeof (execname) / sizeof (CharUnit));
# else  /* !_WINDOWS */
  if (appArgv0[0] == CUS_DIR_SEP)
     strncpy (&execname[0], appArgv0, sizeof (execname) / sizeof (CharUnit));
# endif /* _WINDOWS */
   
  /*
   * second case, the argv[0] indicate a relative path name.
   * The exec name is obtained by appending the current directory.
   */
  else if (TtaFileExist (appArgv0))
    {
      cus_getcwd (&execname[0], sizeof (execname) / sizeof (CharUnit));
      StringConcat (execname, CUS_DIR_STR);
      StringConcat (execname, appArgv0);
    }
  else
    {
      /*
       * Last case, we were just given the application name.
       * Use the PATH environment variable to search the exact binary location.
       */
      my_path = cus_getenv (CUSTEXT("PATH"));
      if (my_path == NULL)
	{
	  fprintf (stderr, "TtaInitializeAppRegistry cannot found PATH environment\n");
	  exit (1);
	}
      /*
       * make our own copy of the string, in order to preserve the
       * enviroment variables. Then search for the binary along the
       * PATH.
       */
      len = (sizeof (path) / sizeof (CharUnit)) - 1;
      StringNCopy (path, my_path, len);
      path[len] = CUS_EOS;
       
      execname_len = sizeof (execname) / sizeof (CharUnit);
#     ifdef _WINDOWS
      MakeCompleteName (appArgv0, CUSTEXT("EXE"), path, execname, &execname_len);
#     else
      MakeCompleteName (appArgv0, "", path, execname, &execname_len);
#     endif
      if (execname[0] == CUS_EOS)
	{
	  fprintf (stderr, "TtaInitializeAppRegistry internal error\n");
	  fprintf (stderr, "\tcannot find path to binary : %s\n", appArgv0);
	  exit (1);
	}
    }
   
  /*
   * Now that we have a complete path up to the binary, extract the
   * application name.
   */
  appName = execname;
  while (*appName) /* go to the ending NUL */
    appName++;
  do
    appName--;
  while (appName > execname && *appName != CUS_DIR_SEP);
  
  if (*appName == CUS_DIR_SEP)
    /* dir_end used for relative links ... */
    dir_end = appName++;
   
#ifdef _WINDOWS
  /* remove the .exe extension. */
  ptr = StrChr (appName, CUSTEXT('.'));
  if (ptr && !StringCaseCompare (ptr, CUSTEXT(".exe")))
    *ptr = CUS_EOS;
  ptr = appName;
  while (*ptr)
    {
      *ptr = ToLower (*ptr);
      ptr++;
    }
#endif /* _WINDOWS */

# if defined(_WINDOWS) && defined(_I18N_)
  AppRegistryEntryAppli = wc2iso_strdup (appName);
# else /* !(defined(_WINDOWS) && defined(_I18N_)) */
  AppRegistryEntryAppli = TtaStrdup (appName);
# endif /* !(defined(_WINDOWS) && defined(_I18N_)) */

  AppNameW = StringDuplicate (appName);

#ifdef HAVE_LSTAT
   /*
    * on Unixes, the binary path started may be a softlink
    * to the real app in the real dir.
    */
  len = 1;
   while (lstat (execname, &stat_buf) == 0 &&
	  S_ISLNK (stat_buf.st_mode) &&
	  len > 0)
     {
       len = readlink (execname, filename, sizeof (filename) / sizeof (CharUnit));
       if (len > 0)
	 {
	   filename[len] = 0;
	   /*
	    * Two cases : can be an absolute link to the binary
	    * or a relative link.
	    */
	   if (filename[0] == CUS_DIR_SEP)
	     {
	       StringCopy (execname, filename);
	       dir_end = execname;
	       while (*dir_end)
		 dir_end++; /* go to the ending NUL */
	       while (dir_end > execname && *dir_end != CUS_DIR_SEP)
		 dir_end--;
	     }
	   else
	     StringCopy (dir_end + 1, filename);
	 } 
     }
#endif /* HAVE_LSTAT */
   
#ifdef DEBUG_REGISTRY
   fprintf (stderr, "path to binary %s : %s\n", appName, execname);
#endif
   
   /* get the THOTDIR for this application. It's under a bin dir */
   dir_end = execname;
   while (*dir_end) /* go to the ending NUL */
     dir_end++;
   
   /* remove the application name */
   ok = FALSE;
   do
     {
       dir_end--;
       ok = (dir_end <= execname || *dir_end == CUS_DIR_SEP);
     }
   while (!ok);

   if (*dir_end == CUS_DIR_SEP)
     {
       /* the name has been found */
       found = TRUE;
       *dir_end = CUS_EOS;
       /* save the binary directory in BinariesDirectory */
       StringNCopy (BinariesDirectory, execname, sizeof (BinariesDirectory) / sizeof (CharUnit));
       /* remove the binary directory */
       found = FALSE;
       ok = FALSE;
       round = 2;
       while (!found || round != 0)
	 {
	   do
	     {
               dir_end--;
               ok = (dir_end <= execname || *dir_end == CUS_DIR_SEP);
	     } while (!ok);

	   if (*dir_end == CUS_DIR_SEP)
	     {
               *dir_end = CUS_EOS;
               if (!StringCompare (&dir_end[1], CUSTEXT("..")))
		 round ++;
               else if (StringCompare (&dir_end[1], CUSTEXT(".")))
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
	   *dir_end = CUS_EOS;
	   if (IsThotDir (execname))
	     AddRegisterEntry ("System", "THOTDIR", execname, REGISTRY_INSTALL, TRUE);
	 }
#ifdef COMPILED_IN_THOTDIR
       /* Check a compiled-in value */
       else if (IsThotDir (COMPILED_IN_THOTDIR))
	 {
           StringCopy (execname, COMPILED_IN_THOTDIR);
           AddRegisterEntry ("System", "THOTDIR", COMPILED_IN_THOTDIR, REGISTRY_INSTALL, TRUE);
	 } 
#else /* COMPILED_IN_THOTDIR */
#ifdef COMPILED_IN_THOTDIR2
       /* Check a compiled-in value */
       else if (IsThotDir (COMPILED_IN_THOTDIR2))
	 {
           StringCopy (execname, COMPILED_IN_THOTDIR2);
           AddRegisterEntry ("System", "THOTDIR", COMPILED_IN_THOTDIR2, REGISTRY_INSTALL, TRUE);
	 }
#endif /* COMPILED_IN_THOTDIR2 */
#endif /* COMPILED_IN_THOTDIR */
      else
	{
	  fprintf (stderr, "Cannot find THOTDIR\n");
	  exit (1);
	} 
     }
   
#ifdef MACHINE
   /* if MACHINE is set up, add it to the registry */
   AddRegisterEntry ("System", "MACHINE", MACHINE, REGISTRY_INSTALL, TRUE);
#endif

   /* load the system settings, stored in THOTDIR/config/thot.ini */
   cus_sprintf (filename, CUSTEXT("%s%c%s%c%s"), execname, CUS_DIR_SEP, THOT_CONFIG_FILENAME, CUS_DIR_SEP, THOT_INI_FILENAME);
   if (TtaFileExist (filename))
     {
#ifdef DEBUG_REGISTRY
       fprintf (stderr, "reading system %s from %s\n", THOT_INI_FILENAME, filename);
#endif
       ImportRegistryFile (filename, REGISTRY_SYSTEM);
       *dir_end = CUS_EOS;
       dir_end -= 3;
     }
   else
     fprintf (stderr, "System wide %s not found at %s\n", THOT_INI_FILENAME, &filename[0]);
   
   /*
   ** find the APP_HOME directory name:
   ** Unix: $HOME/.appname,
   ** Win95: $THOTDIR/users/login-name/
   ** WinNT: c:\WINNT\profiles\login-name
   */
   /* No this should NOT be a call to TtaGetEnvString */
#ifdef _WINDOWS
   /* compute the default app_home value from the username and thotdir */
   dwSize = sizeof (username);
   status = GetUserName (username, &dwSize);
   if (status)
     ptr = username;
   else
     /* under win95, there may be no user name */
     ptr = WIN_DEF_USERNAME;
   if (IS_NT)
     /* winnt: apphome is windowsdir\profiles\username\appname */
     {
       dwSize = MAX_PATH;
       GetWindowsDirectory (windir, dwSize);
       cus_sprintf (app_home, CUSTEXT("%s\\profiles\\%s\\%s"), windir, ptr, AppNameW);
     }
   else
     /* win95: apphome is  thotdir\users\username */
     cus_sprintf (app_home, CUSTEXT("%s\\%s\\%s"), execname, WIN_USERS_HOME_DIR, ptr);   
#else /* !_WINDOWS */
   ptr = cus_getenv (CUSTEXT("HOME"));
   cus_sprintf (app_home, "%s%c.%s", ptr, CUS_DIR_SEP, AppNameW); 
#endif _WINDOWS
   /* store the value of APP_HOME in the registry */
   AddRegisterEntry (AppRegistryEntryAppli, "APP_HOME", app_home, REGISTRY_SYSTEM, TRUE);

   /* set the default APP_TMPDIR */
#ifdef _WINDOWS
   /* the tmpdir is DEF_TMPDIR\app-name */
   cus_sprintf (filename, CUSTEXT("%s%c%s"), DEF_TMPDIR, CUS_DIR_SEP, AppNameW);
   AddRegisterEntry (AppRegistryEntryAppli, "APP_TMPDIR", filename, REGISTRY_SYSTEM, TRUE);
#else
   /* under Unix, APP_TMPDIR == APP_HOME */
   AddRegisterEntry (AppRegistryEntryAppli, "APP_TMPDIR", app_home,
		     REGISTRY_SYSTEM, TRUE);
#endif /* _WINDOWS */
   /* read the user's preferences (if they exist) */
   if (app_home != NULL && *app_home != CUS_EOS)
     {
       cus_sprintf (filename, CUSTEXT("%s%c%s"), app_home, CUS_DIR_SEP, THOT_RC_FILENAME);
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
  TtaFreeMemory (AppNameW);
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
int                 SearchFile (CharUnit* fileName, int dir, CharUnit* fullName)
#else  /* __STDC__ */
int                 SearchFile (fileName, dir, fullName)
CharUnit*           fileName;
int                 dir;
CharUnit*           fullName;
#endif /* __STDC__ */
{
   CharUnit  tmpbuf[200];
   CharUnit* imagepath;
   int                 i, j;
   int                 ret;

   if (Thot_Dir != NULL)
      StringCopy (fullName, Thot_Dir);
   else
      *fullName = CUS_EOS;
   switch (dir)
	 {
	    case 1:
	       /* Lookup in schema and documents path */
	       StringConcat (fullName, fileName);
	       ret = TtaFileExist (fullName);
	       /* lookup in shemas path */
	       i = 0;
	       j = 0;
	       imagepath = SchemaPath;
	       while (ret == 0 && imagepath[i] != CUS_EOS)
		 {
		    while (imagepath[i] != CUS_EOS && imagepath[i] != CUS_PATH_SEP && i < 200)
		       tmpbuf[j++] = imagepath[i++];

		    tmpbuf[j] = CUS_EOS;
		    i++;
		    j = 0;
		    cus_sprintf (fullName, CUSTEXT("%s%s%s"), tmpbuf, CUS_DIR_STR, fileName);
		    ret = TtaFileExist (fullName);
		 }

	       /* lookup in document path */
	       i = 0;
	       j = 0;
	       imagepath = SchemaPath;
	       while (ret == 0 && imagepath[i] != CUS_EOS)
		 {
		    while (imagepath[i] != CUS_EOS && imagepath[i] != CUS_PATH_SEP && i < 200)
		       tmpbuf[j++] = imagepath[i++];

		    tmpbuf[j] = CUS_EOS;
		    i++;
		    j = 0;
		    cus_sprintf (fullName, CUSTEXT("%s%s%s"), tmpbuf, CUS_DIR_STR, fileName);
		    ret = TtaFileExist (fullName);
		 }
	       break;

	    case 2:
	       /* lookup in config */
	       StringConcat (fullName, CUS_DIR_STR);
	       StringConcat (fullName, CUSTEXT("config"));
	       StringConcat (fullName, CUS_DIR_STR);
	       StringConcat (fullName, fileName);
	       break;

	    case 3:
	       /* lookup in batch */
	       StringConcat (fullName, CUS_DIR_STR);
	       StringConcat (fullName, CUSTEXT("batch"));
	       StringConcat (fullName, CUS_DIR_STR);
	       StringConcat (fullName, fileName);
	       break;

	    default:
	       StringConcat (fullName, CUS_DIR_STR);
	       StringConcat (fullName, fileName);
	 }

   /* general search */
   ret = TtaFileExist (fullName);
   if (ret == 0)
     {
        StringCopy (fullName, fileName);
        ret = TtaFileExist (fullName);
     }
   return ret;
}
