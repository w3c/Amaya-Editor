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
 *
 */

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
#define THOT_RC_FILENAME	"thot.ini"
#else /* !_WINDOWS */
#define THOT_RC_FILENAME	".thotrc"
#endif /* ! _WINDOWS */

#define THOT_INI_FILENAME	"thot.ini"
#define THOT_CONFIG_FILENAME    "config"
#define THOT_BIN_FILENAME	"bin"
#define THOT_LIB_DEFAULTNAME	"thot_lib"
#define MAX_REGISTRY_ENTRIES 100

typedef enum
  {
     REGISTRY_INSTALL,		/* installation value e.g. THOTDIR, VERSION  */
     REGISTRY_SYSTEM,		/* values fetched from the system config     */
     REGISTRY_USER		/* values which can be redefined by the user */
  }
RegistryLevel;

typedef struct struct_RegistryEntry
  {
     struct struct_RegistryEntry *next;		/* chaining ! */
     RegistryLevel       level;	/* exact level */
     char               *appli;	/* corresponding section */
     char               *name;	/* name of the entry     */
     char               *orig;	/* the original value (to be saved back) */
     char               *value;	/* user-level value */
  }
RegistryEntryBlk   , *RegistryEntry;

static int          AppRegistryInitialized = 0;
static int          AppRegistryModified = 0;
static RegistryEntry AppRegistryEntry = NULL;
static char        *AppRegistryEntryAppli = NULL;
static char         CurrentDir[MAX_PATH];
static char        *Thot_Dir;


/*
 * A few macro needed to help building the parser
 */

#define IS_BLANK(ptr) \
     (((*(ptr)) == ' ') || ((*(ptr)) == '\b') || \
      ((*(ptr)) == '\n') || ((*(ptr)) == '\r'))
#define SKIP_BLANK(ptr) \
     { while (((*(ptr)) == ' ') || ((*(ptr)) == '\b') || \
              ((*(ptr)) == '\n') || ((*(ptr)) == '\r')) ptr++; }
#define GOTO_EQL(ptr) \
     { while (((*(ptr)) != EOS) && ((*(ptr)) != '=') && \
              ((*(ptr)) != '\n') && ((*(ptr)) != '\r')) ptr++; }
#define GOTO_EOL(ptr) \
     { while (((*(ptr)) != EOS) && \
              ((*(ptr)) != '\n') && ((*(ptr)) != '\r')) ptr++; }

/*----------------------------------------------------------------------
 DoVariableSubstitution : do the substitution on an input
    string of all $(xxx) references by the values of xxx.
   and return a modified output string.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DoVariableSubstitution (char *input, int i_len,
					    char *output, int o_len)
#else  /* __STDC__ */
static void         DoVariableSubstitution (input, i_len, output, o_len)
char               *input;
int                 i_len;
char               *output;
int                 o_len;
#endif
{
  char               *cour = input;
  char               *base = input;
  char               *res = output;
  char               *value;
  char                save;
#define CHECK_OVERFLOW (((cour - input) > i_len) || ((res - output) >= (o_len - 1)))

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
      do
	{
	  cour++;
	  if CHECK_OVERFLOW
	    break;
	}
      while ((*cour != ')') && (!(IS_BLANK (cour))));
      if CHECK_OVERFLOW
	break;
      
      save = *cour;
      *cour = EOS;
      if (save != ')')
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
static int          NewRegisterEntry (char *appli, char *name, char *value,
				      RegistryLevel level)
#else  /* __STDC__ */
static int          NewRegisterEntry (appli, name, value, level)
char               *appli;
char               *name;
char               *value;
RegistryLevel       level;

#endif
{
   char                resu[2000];
   RegistryEntry       cour;

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
   DoVariableSubstitution (value, strlen (value), resu, sizeof (resu));

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
   cour->next = AppRegistryEntry;
   AppRegistryEntry = cour;
   AppRegistryModified++;
   return (0);
}


/*----------------------------------------------------------------------
 AddRegisterEntry : add an entry in the Register, we first check
 that it doesn't already exist especially if the value is empty.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          AddRegisterEntry (char *appli, char *name, char *value,
				      RegistryLevel level, int overwrite)
#else  /* __STDC__ */
static int          AddRegisterEntry (appli, name, value, level, overwrite)
char               *appli;
char               *name;
char               *value;
RegistryLevel       level;
int                 overwrite;

#endif
{
  char                resu[2000];
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

       DoVariableSubstitution (value, strlen (value), resu, sizeof (resu));
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
      if (cour->level == REGISTRY_USER)
	fprintf (output, "%s=%s\n", cour->name, cour->orig);
      next = cour->next;
      while (next != NULL && !strcasecmp (next->appli, cour->appli))
	{
	  if (next->level == REGISTRY_USER)
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
	  if ((cmp <= 0) ||
	      ((cmp == 0) &&
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
  TtaGetEnvString : read the value associated to an environment string
  if not present return NULL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char               *TtaGetEnvString (char *name)
#else  /* __STDC__ */
char               *TtaGetEnvString (name)
char               *name;
#endif
{
  RegistryEntry       cour;
  char               *value;

  if (AppRegistryInitialized == 0)
    return (getenv (name));

  /* appname allows to get the application name */
  if (!strcasecmp("appname", name))
    return(AppRegistryEntryAppli);

  if ((!strcasecmp (name, "cwd")) || (!strcasecmp (name, "pwd")))
    {
#      ifndef _WINDOWS
      return(getcwd(&CurrentDir[0], sizeof(CurrentDir)));
#      else  /* _WINDOWS */
      return(_getcwd(&CurrentDir[0], sizeof(CurrentDir)));
#      endif /* _WINDPWS */
    }

  /* First lookup in the System defaults */
  cour = AppRegistryEntry;
  while (cour != NULL)
    {
      if (!strcasecmp (cour->appli, "System") && !strcmp (cour->name, name) && cour->value[0] != EOS)
	{
#ifdef DEBUG_REGISTRY
	  fprintf (stderr, "TtaGetEnvString(\"%s\") = %s\n", name, cour->value);
#endif
	  return (cour->value);
	}
      cour = cour->next;
    }

  /* Then lookup in the application defaults */
  cour = AppRegistryEntry;
  while (cour != NULL)
    {
      if (!strcasecmp (cour->appli, AppRegistryEntryAppli) && !strcmp (cour->name, name) && cour->value[0] != EOS)
	{
#ifdef DEBUG_REGISTRY
	  fprintf (stderr, "TtaGetEnvString(\"%s\") = %s\n", name, cour->value);
#endif
	  return (cour->value);
	}
      cour = cour->next;
    }
  
  /* Then lookup in the Thot library defaults */
  cour = AppRegistryEntry;
  while (cour != NULL)
    {
      if (!strcasecmp (cour->appli, THOT_LIB_DEFAULTNAME) && !strcmp (cour->name, name) && cour->value[0] != EOS)
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
 TtaSetEnvString : set the value associated to an environment string,
*                  for the current application.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetEnvString (char *name, char *value, int overwrite)
#else  /* __STDC__ */
void                TtaSetEnvString (name, value, overwrite)
CONST char         *name;
CONST char         *value;
int                 overwrite;
#endif
{
   AddRegisterEntry (AppRegistryEntryAppli, name, value,
		     REGISTRY_USER, overwrite);
}


/*----------------------------------------------------------------------
     IsThotDir : Check whether the given string is the THOTDIR value.    
         The heuristic is to find a subdir named "config" and containing 
         the registry file.                                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          IsThotDir (CONST char *path)
#else  /* __STDC__ */
static int          IsThotDir (CONST char *path)
#endif				/* __STDC__ */
{
   char                filename[MAX_PATH];

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

#ifdef WWW_MSWINDOWS
#ifndef __GNUC__
/*----------------------------------------------------------------------
   WINReg_ge - simulates getenv in the WIN32 registry              
   
   looks for <env> in                                                 
   HKEY_CURRENT_USER\Software\OPERA\Amaya\<var>                       
   HKEY_LOCAL_MACHINE\Software\OPERA\Amaya\<var>                      
  ----------------------------------------------------------------------*/
static char        *WINReg_get (CONST char *env)
{
   static CONST char   userBase[] = "Software\\OPERA\\Amaya";
   char                textKey[MAX_PATH];
   HKEY                hKey;
   DWORD               type;
   int                 success;
   static char         ret[MAX_PATH];	/* thread unsafe! */
   DWORD               retLen = sizeof (ret);

   /* not used but RegEnumValue fails if insufficient length */
   char                name[MAX_PATH];
   DWORD               nameLen = sizeof (name);

   sprintf (textKey, "%s\\%s", userBase, env);
   if (RegOpenKey (HKEY_CURRENT_USER, textKey, &hKey) == ERROR_SUCCESS)
     {
	RegCloseKey (HKEY_CURRENT_USER);
	success = RegEnumValue (hKey, 0,	/* we use only 1 value per entry */
			 name, &nameLen, NULL, &type, (BYTE *) ret, &retLen)
	   == ERROR_SUCCESS;
	RegCloseKey (hKey);
	return success ? ret : NULL;
     }
   if (RegOpenKey (HKEY_LOCAL_MACHINE, textKey, &hKey) != ERROR_SUCCESS)
      return NULL;
   RegCloseKey (HKEY_LOCAL_MACHINE);
   success = RegEnumValue (hKey, 0,	/* we use only 1 value per entry */
			 name, &nameLen, NULL, &type, (BYTE *) ret, &retLen)
      == ERROR_SUCCESS;
   RegCloseKey (hKey);
   return success ? ret : NULL;
}

/*----------------------------------------------------------------------
   WINIni_get - simulates getenv in the Windows/Amaya.ini file     
  ----------------------------------------------------------------------*/
static char        *WINIni_get (CONST char *env)
{
   DWORD               res;
   static char         ret[MAX_PATH];	/* thread unsafe! */

   res = GetPrivateProfileString ("Amaya", env, "", ret, sizeof (ret), "Amaya.ini");
   return res ? ret : NULL;
}
#endif
#endif /* WWW_MSWINDOWS */


/*----------------------------------------------------------------------
  TtaSaveAppRegistry : Save the Registry in the THOT_RC_FILENAME located
  in the user's directory.
  ----------------------------------------------------------------------*/
void                TtaSaveAppRegistry ()
{
#ifndef WWW_MSWINDOWS
   char               *home_dir;
   char                filename[MAX_PATH];
   FILE               *output;

#endif /* !WWW_MSWINDOWS */

   if (!AppRegistryInitialized)
      return;
   if (!AppRegistryModified)
      return;


#ifndef WWW_MSWINDOWS
   home_dir = TtaGetEnvString ("HOME");
   if (home_dir != NULL)
     {
	strcpy (filename, home_dir);
	strcat (filename, DIR_STR);
	strcat (filename, THOT_RC_FILENAME);
     }
   else
     {
	fprintf (stderr, "Cannot save Registry no HOME dir\n");
	return;
     }
   output = fopen (filename, "w");
   if (output == NULL)
     {
	fprintf (stderr, "Cannot save Registry to %s :\n", filename);
	perror ("fopen failed");
	return;
     }
   SortEnv ();
   PrintEnv (output);
   AppRegistryModified = 0;
#endif /* !WWW_MSWINDOWS */
}


/*----------------------------------------------------------------------
  ImportRegistryFile : import a registry file.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ImportRegistryFile (char *filename, RegistryLevel level)
#else  /* __STDC__ */
static void         ImportRegistryFile (filename, level)
char               *filename;
RegistryLevel       level;

#endif
{
   FILE               *input;
   char               *str, *base;
   char                string[1000];
   char                appli[1000] = THOT_LIB_DEFAULTNAME;
   char               *name;
   char               *value;

   input = fopen (filename, "r");
   if (input == NULL)
     {
	fprintf (stderr, "Cannot read Registry from %s :\n", filename);
	perror ("fopen failed");
	return;
     }

   while (1)
     {
	/*
	 * read one line in string buffer.
	 */
	if (fgets (&string[0], sizeof (string) - 1, input) == NULL)
	   break;

	str = &string[0];
	SKIP_BLANK (str);
	string[sizeof (string) - 1] = EOS;

	/*
	 * Comment starts with a semicolumn.
	 */
	if (*str == ';')
	   continue;

	/*
	 * sections are indicated between brackets, e.g. [amaya]
	 */
	if (*str == '[')
	  {
	     str++;
	     SKIP_BLANK (str);
	     base = str;
	     while ((*str != EOS) && (*str != ']'))
		str++;
	     if (*str == EOS)
	       {
		  fprintf (stderr, "Registry %s corrupted :\n\t\"%s\"\n",
			   filename, string);
		  continue;
	       }
	     *str = EOS;
	     strcpy (&appli[0], base);
#ifdef DEBUG_REGISTRY
	     fprintf (stderr, "TtaInitializeAppRegistry section [%s]\n", appli);
#endif
	     continue;
	  }

	/*
	 * entries have the following form :
	 *    name=value
	 */
	name = str;
	GOTO_EQL (str);
	if (*str != '=')
	   continue;
	*str++ = EOS;
	SKIP_BLANK (str);
	value = str;
	GOTO_EOL (str);
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
   char               *pT;
   char               *Thot_Sys_Sch;
   char               *Thot_Sch;

   /* default values for various global variables */
   FirstCreation = FALSE;
   CurSaveInterval = 0;
   pT = (char *)TtaGetEnvString ("AUTOSAVE");
   if (pT != NULL)
     CurSaveInterval = atoi(pT);
   if (CurSaveInterval <= 0)
     CurSaveInterval = DEF_SAVE_INTVL;
   HighlightBoxErrors = FALSE;
   InsertionLevels = 4;

   /* The base of the Thot directory */
   Thot_Dir = (char *) TtaGetEnvString ("THOTDIR");
   if (Thot_Dir == NULL)
      fprintf (stderr, "missing environment variable THOTDIR\n");

   /* The predefined path to documents */
   pT = (char *) TtaGetEnvString ("THOTDOC");
   if (pT == NULL)
      DocumentPath[0] = EOS;
   else
      strncpy (DocumentPath, pT, MAX_PATH);

   /* Read the schemas Paths */
   Thot_Sch = (char *) TtaGetEnvString ("THOTSCH");
   Thot_Sys_Sch = (char *) TtaGetEnvString ("THOTSYSSCH");

   /* set up SchemaPath accordingly */
   if ((Thot_Sch != NULL) && (Thot_Sys_Sch != NULL)) {
       strncpy (SchemaPath, Thot_Sch, MAX_PATH);
       strcat (SchemaPath,PATH_STR);
       strcat (SchemaPath, Thot_Sys_Sch);
   } else if (Thot_Sch != NULL)
       strncpy (SchemaPath, Thot_Sch, MAX_PATH);
   else if (Thot_Sys_Sch != NULL)
       strncpy (SchemaPath, Thot_Sys_Sch, MAX_PATH);
   else
       SchemaPath[0] = EOS;

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
void                TtaInitializeAppRegistry (char *appArgv0)
#else  /* __STDC__ */
void                TtaInitializeAppRegistry (appArgv0)
char               *appArgv0;
#endif
{
   PathBuffer          execname;
   PathBuffer          path;
   char               *home_dir;
   char                filename[MAX_PATH];
   char               *my_path;
   char               *dir_end = NULL;
   char               *appName;
#  ifdef _WINDOWS
#  ifndef __CYGWIN32__
   extern int _fmode;
#  endif
#  else /* ! _WINDOWS */
   struct stat         stat_buf;
#  endif /* _WINDOWS */
   int                 execname_len;
   int                 len, round;
   boolean             found, ok;			  

#  ifdef _WINDOWS
#  ifndef __CYGWIN32__
   _fmode = _O_BINARY;
#  endif  /* __CYGWIN32__ */
#  endif  /* _WINDOWS */

  if (AppRegistryInitialized != 0)
    return;
  AppRegistryInitialized++;
  /*
   * Sanity check on the argument given. An error here should be
   * detected by programmers, since it's a application coding error.
   */
  if ((appArgv0 == NULL) || (*appArgv0 == EOS))
    {
      fprintf (stderr, "TtaInitializeAppRegistry called with invalid argv[0] value\n");
      exit (1);
    }

  /* No this should NOT be a call to TtaGetEnvString */
  home_dir = getenv ("HOME");
  /*
   * We are looking for the absolute pathname to the binary of the
   * application.
   *
   * First case, the argv[0] indicate that it's an absolute path name.
   * i.e. start with / on unixes or \ or ?:\ on Windows.
   */

# ifdef _WINDOWS
  if (appArgv0[0] == DIR_SEP || (appArgv0[1] == ':' && appArgv0[2] == DIR_SEP))
     strncpy (&execname[0], appArgv0, sizeof (execname));
# else  /* 1_WINDOWS */
  if (appArgv0[0] == DIR_SEP)
     strncpy (&execname[0], appArgv0, sizeof (execname));
# endif /* _WINDOWS */

  /*
   * second case, the argv[0] indicate a relative path name.
   * The exec name is obtained by appending the current directory.
   */
  else if (TtaFileExist (appArgv0))
    {
#     ifndef _WINDOWS
      getcwd (&execname[0], sizeof (execname));
#     else  /* _WINDOWS */
      _getcwd (&execname[0], sizeof (execname));
#     endif /* _WINDOWS */
      strcat (execname, DIR_STR);
      strcat (execname, appArgv0);
    }
  /*
   * Last case, we were just given the application name.
   * Use the PATH environment variable to search the exact binary location.
   */
  else
    {
      my_path = getenv ("PATH");
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
      len = sizeof (path) - 1;
      strncpy (path, my_path, len);
      path[len] = EOS;

      execname_len = sizeof (execname);
#     ifdef _WINDOWS
      MakeCompleteName (appArgv0, "EXE", path, execname, &execname_len);
#     else
      MakeCompleteName (appArgv0, "", path, execname, &execname_len);
#     endif
      if (execname[0] == EOS)
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
  appName = &execname[0];
  while (*appName)
    /* go to the ending NUL */
    appName++;

  do
    appName--;
  while (appName > execname && *appName != DIR_SEP);
  if (*appName == DIR_SEP)
    /* dir_end used for relative links ... */
    dir_end = appName++;
  appName = TtaStrdup (appName);
  AppRegistryEntryAppli = appName;

#ifdef HAVE_LSTAT
  /*
   * on Unixes, the binary path started may be a softlink
   * to the real app in the real dir.
   */
  if (lstat (execname, &stat_buf) == 0 && S_ISLNK (stat_buf.st_mode))
    {
      len = readlink (execname, filename, sizeof (filename));
      if (len > 0)
	{
	  filename[len] = 0;
	  /*
	   * Two cases : can be an absolute link to the binary
	   * or a relative link.
	   */
	  if (filename[0] == DIR_SEP)
	    strcpy (execname, filename);
	  else
	    strcpy (dir_end + 1, filename);
	}
    }
#endif /* HAVE_LSTAT */

#ifdef DEBUG_REGISTRY
   fprintf (stderr, "path to binary %s : %s\n", appName, execname);
#endif

   /* get the THOTDIR for this application. It's under a bin dir */
   dir_end = execname;
   while (*dir_end)
     /* go to the ending NUL */
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
       strncpy (BinariesDirectory, execname, sizeof (BinariesDirectory));

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
	     }
	   while (!ok);
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
	     AddRegisterEntry ("System", "THOTDIR", execname, REGISTRY_INSTALL, TRUE);
	 }
#ifdef COMPILED_IN_THOTDIR
       /* Check a compiled-in value */
       else if (IsThotDir (COMPILED_IN_THOTDIR))
	 {
	   strcpy(execname, COMPILED_IN_THOTDIR);
	   AddRegisterEntry ("System", "THOTDIR", COMPILED_IN_THOTDIR,
			     REGISTRY_INSTALL, TRUE);
	 }
#else /* COMPILED_IN_THOTDIR */
#ifdef COMPILED_IN_THOTDIR2
       /* Check a compiled-in value */
       else if (IsThotDir (COMPILED_IN_THOTDIR2))
	 {
	   strcpy(execname, COMPILED_IN_THOTDIR2);
	   AddRegisterEntry ("System", "THOTDIR", COMPILED_IN_THOTDIR2,
			     REGISTRY_INSTALL, TRUE);
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
   strcpy (filename, execname);
   strcat (filename, DIR_STR);
   strcat (filename, THOT_CONFIG_FILENAME);
   strcat (filename, DIR_STR);
   strcat (filename, THOT_INI_FILENAME);
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
     fprintf (stderr, "System wide %s not found at %s\n", THOT_INI_FILENAME, &filename[0]);

#  ifndef _WINDOWS
   if (home_dir != NULL)
     {
	strcpy (filename, home_dir);
	strcat (filename, DIR_STR);
	strcat (filename, THOT_RC_FILENAME);
	if (TtaFileExist (&filename[0]))
	  {
#ifdef DEBUG_REGISTRY
	     fprintf (stderr, "reading user's %s from %s\n",
		      THOT_RC_FILENAME, filename);
#endif
	     ImportRegistryFile (filename, REGISTRY_USER);
	  }
	else {
	   char old_filename[MAX_PATH];
	   strcpy (old_filename, home_dir);
	   strcat (old_filename, DIR_STR);
	   strcat (old_filename, THOT_INI_FILENAME);
	   if (TtaFileExist (old_filename)) {
#ifdef DEBUG_REGISTRY
		fprintf (stderr, "reading user's %s from %s\n",
			 THOT_INI_FILENAME, old_filename);
#endif
		ImportRegistryFile (old_filename, REGISTRY_USER);
		TtaFileUnlink(old_filename);
		TtaSaveAppRegistry();
		fprintf (stderr, "user's preferences moved from %s to %s\n",
			 old_filename, filename);
	   }
	}
     }
   else
      fprintf (stderr, "User's %s not found\n", THOT_INI_FILENAME);
#  endif /* !_WINDOWS */

#ifdef DEBUG_REGISTRY
   PrintEnv (stderr);
#endif

   InitEnviron ();
   AppRegistryModified = 0;
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
int                 SearchFile (char *fileName, int dir, char *fullName)
#else  /* __STDC__ */
int                 SearchFile (fileName, dir, fullName)
char               *fileName;
int                 dir;
char               *fullName;
#endif /* __STDC__ */
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
