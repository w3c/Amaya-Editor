/*
 * Copyright (c) 1996 INRIA, All rights reserved
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
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "registry.h"
#include "application.h"

#define EXPORT
#include "platform_tv.h"

/* DEBUG_REGISTRY enable the Registry debug messages */

/*----------------------------------------------------------------------
   									
    Constants, types, global variable and settings for the Registry	
   									
  ----------------------------------------------------------------------*/

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

static char        *Thot_Dir;

#ifdef __STDC__
extern int          ThotFile_exist (char *);
extern void         MakeCompleteName (Name, char *, PathBuffer, PathBuffer, int *);

#else  /* __STDC__ */
extern int          ThotFile_exist ();
extern void         MakeCompleteName ();

#endif

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
     { while (((*(ptr)) != '\0') && ((*(ptr)) != '=') && \
              ((*(ptr)) != '\n') && ((*(ptr)) != '\r')) ptr++; }
#define GOTO_EOL(ptr) \
     { while (((*(ptr)) != '\0') && \
              ((*(ptr)) != '\n') && ((*(ptr)) != '\r')) ptr++; }

/*----------------------------------------------------------------------
   									
    First part : reading / writing values to/from the memory structures.	
   									
  ----------------------------------------------------------------------*/

/*
 * DoVariableSubstitution : do the substitution on an input
 *    string of all $(xxx) references by the values of xxx.
 *    and return a modified output string.
 */

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

	/*
	 * Ok, that the beginning of a variable name ...
	 */
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
	*cour = '\0';
	if (save != ')')
	  {
	     fprintf (stderr, "invalid variable name %s in %s\n",
		      base, THOT_INI_FILENAME);
	  }

	/*
	 * We are ready to fetch the base value from the Registry.
	 */
	value = TtaGetEnvString (base);
	if (value == NULL)
	  {
	     fprintf (stderr, "%s referencing undefined variable %s\n",
		      THOT_INI_FILENAME, base);
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
     {
	fprintf (stderr, "DoVariableSubstitution : Overflow on \"%s\"\n", input);
     }
   *res = '\0';
}

/*
 * NewRegisterEntry : add a fresh new entry in the Register.
 */

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

/*
 * AddRegisterEntry : add an entry in the Register, we first check
 *                      that it doesn't already exist especially if the
 *                      value is empty.
 */

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

	     /*
	      * Cannot superseed and INSTALL value.
	      */
	     if (cour->level == REGISTRY_INSTALL)
		return (0);
	     else if ((!strcasecmp (cour->appli, appli)) &&
		      (cour->level == level))
		break;
	  }
	cour = cour->next;
     }

   if (cour != NULL)
     {
	/*
	 * there is aleady an entry.
	 */
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
	if ((!overwrite) && ((value == NULL) || (*value == '\0')))
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

/*
 * PrintEnv : print the Registry to an open File.
 */

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
	/*
	 * don't save any system entry.
	 */
	if (!strcasecmp (cour->appli, "System"))
	  {
	     cour = cour->next;
	     continue;
	  }

	/*
	 * print out the section.
	 */

	fprintf (output, "[%s]\n", cour->appli);

	/*
	 * add all the entries under the same appli name.
	 */

	if (cour->level == REGISTRY_USER)
	   fprintf (output, "%s=%s\n", cour->name, cour->orig);
	next = cour->next;
	while ((next != NULL) &&
	       (!strcasecmp (next->appli, cour->appli)))
	  {
	     if (next->level == REGISTRY_USER)
		fprintf (output, "%s=%s\n", next->name, next->orig);
	     next = next->next;
	  }
	cour = next;

	fprintf (output, "\n");
     }
}

/*
 * SortEnv : sort the Registry by application and name entries.
 */

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
	     /*
	      * sorting order : First by appli name, then by entry name.
	      */
	     cmp = strcasecmp (cour->appli, (*start)->appli);
	     if ((cmp <= 0) ||
		 ((cmp == 0) &&
		  (strcasecmp (cour->name, (*start)->name) < 0)))
	       {

		  /*
		   * swap *start and cour.
		   */
		  prev->next = *start;
		  *start = cour;
		  tmp = cour->next;
		  cour->next = prev->next->next;
		  prev->next->next = tmp;

		  cour = prev->next;
	       }

	     /*
	      * next in the list
	      */
	     prev = cour;
	     if (cour != NULL)
		cour = cour->next;
	  }
	start = &((*start)->next);
     }
}

/*
 * TtaGetEnvString : read the value associated to an environment string
 *                  if not present return NULL.
 */

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
     {
	return (getenv (name));
     }

   /*
    * First lookup in the System defaults.
    */
   cour = AppRegistryEntry;
   while (cour != NULL)
     {
	if ((!strcasecmp (cour->appli, "System")) &&
	    (!strcmp (cour->name, name)) &&
	    (cour->value[0] != '\0'))
	  {
#ifdef DEBUG_REGISTRY
	     fprintf (stderr, "TtaGetEnvString(\"%s\") = %s\n", name, cour->value);
#endif
	     return (cour->value);
	  }
	cour = cour->next;
     }

   /*
    * Then lookup in the application defaults.
    */
   cour = AppRegistryEntry;
   while (cour != NULL)
     {
	if ((!strcasecmp (cour->appli, AppRegistryEntryAppli)) &&
	    (!strcmp (cour->name, name)) &&
	    (cour->value[0] != '\0'))
	  {
#ifdef DEBUG_REGISTRY
	     fprintf (stderr, "TtaGetEnvString(\"%s\") = %s\n", name, cour->value);
#endif
	     return (cour->value);
	  }
	cour = cour->next;
     }

   /*
    * Then lookup in the Thot library defaults.
    */
   cour = AppRegistryEntry;
   while (cour != NULL)
     {
	if ((!strcasecmp (cour->appli, THOT_LIB_DEFAULTNAME)) &&
	    (!strcmp (cour->name, name)) &&
	    (cour->value[0] != '\0'))
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

/*
 * TtaSetEnvString : set the value associated to an environment string,
 *                  for the current application.
 */

#ifdef __STDC__
void                TtaSetEnvString (char *name, char *value, int overwrite)
#else  /* __STDC__ */
void                TtaSetEnvString (name, value, overwrite)
const char         *name;
const char         *value;
int                 overwrite;

#endif
{
   AddRegisterEntry (AppRegistryEntryAppli, name, value,
		     REGISTRY_USER, overwrite);
}

/*----------------------------------------------------------------------
   									
    Second part : reading / writing values from / to filesystem or	
   		 registry.						
   									
  ----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
                                                                         
     IsThotDir : Check whether the given string is the THOTDIR value.    
         The heuristic is to find a subdir named "config" and containing 
         the registry file.                                              
                                                                         
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static int          IsThotDir (const char *path)

#else  /* __STDC__ */
static int          IsThotDir (const char *path)
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
   if (ThotFile_exist (filename))
      return (1);
   else
      return (0);
}

#ifdef WWW_MSWINDOWS
/*----------------------------------------------------------------------
   WINReg_ge - simulates getenv in the WIN32 registry              
   
   looks for <env> in                                                 
   HKEY_CURRENT_USER\Software\OPERA\Amaya\<var>                       
   HKEY_LOCAL_MACHINE\Software\OPERA\Amaya\<var>                      
  ----------------------------------------------------------------------*/
static char        *WINReg_get (const char *env)
{
   static const char   userBase[] = "Software\\OPERA\\Amaya";
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

#if 0				/* optional code - malloc per request. problem, no way 
				   to recover memory from last call */
{
   DWORD               dwcSubKeys;

   /Number of sub keys.

      DWORD dwcMaxSubKey;
   /Longest sub key size.

      DWORD dwcMaxClass;
   /Longest class string.

      DWORD dwcValues;
   /Number of values for this
      key.

	 DWORD dwcMaxValueName;
   /Longest Value name.

      DWORD dwcMaxValueData;
   /Longest Value data.

      DWORD dwcSecDesc;
   /Security descriptor.

      FILETIME ftLastWriteTime;
   /Last write time.

      success = RegQueryInfoKey (hKey, ret, &len,
				 NULL, /Reserved.
				 & dwcSubKeys, /Number of sub keys.
				 & dwcMaxSubKey, /Longest sub key size.
				 & dwcMaxClass, /Longest class string.
				 & dwcValues, /Number of values for this key.
				 & dwcMaxValueName, /Longest Value name.
				 & dwcMaxValueData, /Longest Value data.
				 & dwcSecDesc, /Security descriptor.
				 & ftLastWriteTime)
      /Last write time.
	 == ERROR_SUCCESS;
   if (ret)
      free (ret);		/* free from last call */
   ret = (char *) malloc (dwcMaxValueData);
}
#endif


/*----------------------------------------------------------------------
   WINIni_get - simulates getenv in the Windows/Amaya.ini file     
   
  ----------------------------------------------------------------------*/
static char        *WINIni_get (const char *env)
{
   DWORD               res;
   static char         ret[MAX_PATH];	/* thread unsafe! */

   res = GetPrivateProfileString ("Amaya", env, "", ret, sizeof (ret), "Amaya.ini");
   return res ? ret : NULL;
}
#endif /* WWW_MSWINDOWS */


/*
 * TtaSaveAppRegistry : Save the Registry the THOT_INI_FILENAME located
 *       in the user's directory.
 */

#ifdef __STDC__
void                TtaSaveAppRegistry (void)
#else  /* __STDC__ */
void                TtaSaveAppRegistry ()
#endif
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
	strcat (filename, THOT_INI_FILENAME);
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
}				/* TtaSaveAppRegistry */

/*
 * ImportRegistryFile : import a registry file.
 */

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
	string[sizeof (string) - 1] = '\0';

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
	     while ((*str != '\0') && (*str != ']'))
		str++;
	     if (*str == '\0')
	       {
		  fprintf (stderr, "Registry %s corrupted :\n\t\"%s\"\n",
			   filename, string);
		  continue;
	       }
	     *str = '\0';
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
	*str++ = '\0';
	SKIP_BLANK (str);
	value = str;
	GOTO_EOL (str);
	*str = '\0';
	AddRegisterEntry (appli, name, value, level, TRUE);
     }

   fclose (input);
}

/*
 * ThotDir returns the THOTDIR value.
 */
char               *ThotDir ()
{
   return Thot_Dir;
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
   char               *Thot_Sch;

   /* default values for various global variables */
   FirstCreation = FALSE;
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
      DocumentPath[0] = '\0';
   else
      strncpy (DocumentPath, pT, MAX_PATH);

   /* The predefined path to schemas */
   Thot_Sch = (char *) TtaGetEnvString ("THOTSCH");
   if (Thot_Sch == NULL)
      SchemaPath[0] = '\0';
   else
      strncpy (SchemaPath, Thot_Sch, MAX_PATH);
}

/*
 * TtaInitializeAppRegistry : initialize the Registry, the only argument
 *       given is a copy of the argv[0] received from the main().
 *       From this, we can deduce the installation directory of the programm,
 *       (using the PATH environment value if necessary) and the application
 *       name.
 *       We load the ressources file from the installation directory and
 *       the specific user values from the user HOME dir.
 */

#ifdef __STDC__
void                TtaInitializeAppRegistry (char *appArgv0)
#else  /* __STDC__ */
void                TtaInitializeAppRegistry (appArgv0)
char               *appArgv0;

#endif
{
   char               *home_dir;
   char                filename[MAX_PATH];
   int                 execname_len;
   PathBuffer          execname;
   PathBuffer          path;
   char               *my_path;
   char               *dir_end;
   char               *appName;

#ifdef S_ISLNK
   struct stat         stat_buf;

#endif /* S_ISLNK */

   if (AppRegistryInitialized != 0)
      return;
   AppRegistryInitialized++;

   /*
    * Sanity check on the argument given. An error here should be
    * detected by programmers, since it's a application coding error.
    */
   if ((appArgv0 == NULL) || (*appArgv0 == '\0'))
     {
	fprintf (stderr,
	    "TtaInitializeAppRegistry called with invalid argv[0] value\n");
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
#ifdef _WINDOWS
   if ((appArgv0[0] == DIR_SEP) ||
       ((appArgv0[1] == ':') && (appArgv0[2] == DIR_SEP)))
#else  /* !_WINDOWS */
   if (appArgv0[0] == DIR_SEP)
#endif /* !_WINDOWS */
     {
	strncpy (&execname[0], appArgv0, sizeof (execname));
     }
   /*
    * second case, the argv[0] indicate a relative path name.
    * The exec name is obtained by appending the current directory.
    */
   else if (ThotFile_exist (appArgv0))
     {
	getcwd (&execname[0], sizeof (execname));
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
	     fprintf (stderr,
		"TtaInitializeAppRegistry cannot found PATH environment\n");
	     exit (1);
	  }
	/*
	 * make our own copy of the string, in order to preserve the
	 * enviroment variables. Then search for the binary along the
	 * PATH.
	 */
	strncpy (path, my_path, sizeof (path) - 1);
	path[sizeof (path) - 1] = '\0';

	execname_len = sizeof (execname);
#ifdef _WINDOWS
	MakeCompleteName (appArgv0, "EXE", path, execname, &execname_len);
#else
	MakeCompleteName (appArgv0, "", path, execname, &execname_len);
#endif
	if (execname[0] == '\0')
	  {
	     fprintf (stderr,
		      "TtaInitializeAppRegistry internal error\n");
	     fprintf (stderr,
		      "\tcannot find path to binary : %s\n", appArgv0);
	     exit (1);
	  }
     }

   /*
    * Now that we have a complete path up to the binary, extract the
    * application name.
    */
   appName = &execname[0];
   while (*appName)
      appName++;		/* go to the ending NUL */
   do
      appName--;
   while ((appName > &execname[0]) && (*appName != DIR_SEP));
   if (*appName == DIR_SEP)
      dir_end = appName++;	/* dir_end used for relative links ... */
   appName = TtaStrdup (appName);
   AppRegistryEntryAppli = appName;

#ifdef S_ISLNK
   /*
    * on Unixes, the binary path started may be a softlink
    * to the real app in the real dir.
    */
   if ((lstat (execname, &stat_buf) == 0) &&
       (S_ISLNK (stat_buf.st_mode)))
     {
	int                 len = readlink (execname, &filename[0], sizeof (filename));

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
	       {
		  strcpy (dir_end + 1, filename);
	       }
	  }
     }
#endif /* S_ISLNK */

#ifdef DEBUG_REGISTRY
   fprintf (stderr, "path to binary %s : %s\n", appName, execname);
#endif

   /*
    * get the THOTDIR for this application. It's under a bin dir.
    */
   dir_end = &execname[0];
   while (*dir_end)
      dir_end++;		/* go to the ending NUL */

   do
      dir_end--;
   while ((dir_end > &execname[0]) && (*dir_end != DIR_SEP));
   if (*dir_end != DIR_SEP)
      goto thot_dir_not_found;

   *dir_end = '\0';

   /* save the binary directory in BinariesDirectory */
   strncpy (BinariesDirectory, &execname[0], sizeof (BinariesDirectory));

   if (IsThotDir (&execname[0]))
     {
	AddRegisterEntry ("System", "THOTDIR", execname,
			  REGISTRY_INSTALL, TRUE);
	goto load_system_settings;
     }

   do
      dir_end--;
   while ((dir_end > &execname[0]) && (*dir_end != DIR_SEP));
   if (*dir_end != DIR_SEP)
      goto thot_dir_not_found;

   *dir_end = '\0';
   if (IsThotDir (&execname[0]))
     {
	AddRegisterEntry ("System", "THOTDIR", execname,
			  REGISTRY_INSTALL, TRUE);
	goto load_system_settings;
     }

   do
      dir_end--;
   while ((dir_end > &execname[0]) && (*dir_end != DIR_SEP));
   if (*dir_end != DIR_SEP)
      goto thot_dir_not_found;

   *dir_end = '\0';
   if (IsThotDir (&execname[0]))
     {
	AddRegisterEntry ("System", "THOTDIR", execname,
			  REGISTRY_INSTALL, TRUE);
	goto load_system_settings;
     }

   do
      dir_end--;
   while ((dir_end > &execname[0]) && (*dir_end != DIR_SEP));
   if (*dir_end != DIR_SEP)
      goto thot_dir_not_found;

   *dir_end = '\0';
   if (IsThotDir (&execname[0]))
     {
	AddRegisterEntry ("System", "THOTDIR", execname,
			  REGISTRY_INSTALL, TRUE);
	goto load_system_settings;
     }

 thot_dir_not_found:

   fprintf (stderr, "Cannot find THOTDIR\n");
   exit (1);

 load_system_settings:

   /*
    * load the system settings, stored in THOTDIR/bin.
    */
   strcpy (filename, execname);
   strcat (filename, DIR_STR);
   strcat (filename, THOT_CONFIG_FILENAME);
   strcat (filename, DIR_STR);
   strcat (filename, THOT_INI_FILENAME);
   if (ThotFile_exist (filename))
     {
#ifdef DEBUG_REGISTRY
	fprintf (stderr, "reading system %s from %s\n",
		 THOT_INI_FILENAME, filename);
#endif
	ImportRegistryFile (filename, REGISTRY_SYSTEM);
	*dir_end = '\0';
	dir_end -= 3;
     }
   else
      fprintf (stderr, "System wide %s not found\n", THOT_INI_FILENAME);

   if (home_dir != NULL)
     {
	strcpy (filename, home_dir);
	strcat (filename, DIR_STR);
	strcat (filename, THOT_INI_FILENAME);
	if (ThotFile_exist (filename))
	  {
#ifdef DEBUG_REGISTRY
	     fprintf (stderr, "reading user's %s from %s\n",
		      THOT_INI_FILENAME, filename);
#endif
	     ImportRegistryFile (filename, REGISTRY_USER);
	  }
     }
   else
      fprintf (stderr, "User's %s not found\n", THOT_INI_FILENAME);

#ifdef DEBUG_REGISTRY
   PrintEnv (stderr);
#endif

   InitEnviron ();
   AppRegistryModified = 0;
}


/*----------------------------------------------------------------------
   SearchFile recherche un fichier en suivant les indications      
   donnees par dir.                                        
   Retourne 1 avec le nom absolu dans fullName si on       
   le trouve et 0 sinon.                                   
   Suivant la valeur de dir, on cherche dans:              
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

   if (ThotDir () != NULL)
      strcpy (fullName, ThotDir ());
   else
      *fullName = '\0';
   switch (dir)
	 {
	    case 1:
	       /* Recherche dans les schemas et les documents */
	       strcat (fullName, fileName);
	       ret = ThotFile_exist (fullName);
	       /* Recherche le fichier dans les directories de schemas */
	       i = 0;
	       j = 0;
	       imagepath = SchemaPath;
	       while (ret == 0 && imagepath[i] != '\0')
		 {
		    while (imagepath[i] != '\0' && imagepath[i] != PATH_SEP && i < 200)
		       tmpbuf[j++] = imagepath[i++];

		    tmpbuf[j] = '\0';
		    i++;
		    j = 0;
		    sprintf (fullName, "%s%s%s", tmpbuf, DIR_STR, fileName);
		    ret = ThotFile_exist (fullName);
		 }

	       /* continue la recheche dans les repertoires de documents */
	       i = 0;
	       j = 0;
	       imagepath = SchemaPath;
	       while (ret == 0 && imagepath[i] != '\0')
		 {
		    while (imagepath[i] != '\0' && imagepath[i] != PATH_SEP && i < 200)
		       tmpbuf[j++] = imagepath[i++];

		    tmpbuf[j] = '\0';
		    i++;
		    j = 0;
		    sprintf (fullName, "%s%s%s", tmpbuf, DIR_STR, fileName);
		    ret = ThotFile_exist (fullName);
		 }
	       break;

	    case 2:
	       /* Recherche dans config */
	       strcat (fullName, DIR_STR);
	       strcat (fullName, "config");
	       strcat (fullName, DIR_STR);
	       strcat (fullName, fileName);
	       break;

	    case 3:
	       /* Recherche dans batch */
	       strcat (fullName, DIR_STR);
	       strcat (fullName, "batch");
	       strcat (fullName, DIR_STR);
	       strcat (fullName, fileName);
	       break;

	    default:
	       strcat (fullName, DIR_STR);
	       strcat (fullName, fileName);
	 }

   /* on cherche le fichier */
   ret = ThotFile_exist (fullName);
   if (ret == 0)
     {
	strcpy (fullName, fileName);
	ret = ThotFile_exist (fullName);
     }
   return ret;
}
