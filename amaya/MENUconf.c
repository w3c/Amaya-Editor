/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1999-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * MENUconf.c: This module contains all the functions used to handle
 * the configuration menus in Amaya. Each configuration menu (set of options)
 * is associated with the following (let's suppose it's the Network men): 
 * - a menu definition in MENUconf.h
 * - a set of static variables that correspond to the menu options
 * - an initialization function for displaying the menu (NetworkConfMenu).
 *   This function should be exported via f/MENUconf_f.h
 * - a callback function to handle the interaction with the user and the
 *   update of the static variables (NetworkCallbackdialogue)
 * - a function to refresh the menu whenever the static variables change
 *   (RefreshNetworkMenu)
 * - a function to download the current values of the environment variables
 *   into the above static variables (GetNetworkConf) (the environment
 *   variables that describe the menu options)
 * - a function to write the value of the static variables to the 
 *   corresponding environment variables (SetNetworkConf)
 * - a function to download the default values of the environment variables
 *   (GetDefaulNetworkConf)
 *
 * In addition, each menu should be initialized in the InitConfMenu
 * function.
 *
 * Authors: J. Kahan
 * Contributors: Luc Bonameau for profiles and templates
 *               I. Vatton selelction colors and access keys
 *
 * To do: remove the CACHE_RESTART option from some options, once we write
 * the code that should take it into account.
 */

/* Included headerfiles */
#define THOT_EXPORT extern

#include "amaya.h"
#include "MENUconf.h"
#include "print.h"
#include "fileaccess.h"
#include "profiles.h"

#ifdef ANNOTATIONS
#include "annotlib.h"
#include "ANNOTevent_f.h"
#endif /* ANNOTATIONS */

#ifdef _WINDOWS
#include "resource.h"
#include "wininclude.h"

#include "constmedia.h"
#include "appdialogue.h"

extern HINSTANCE hInstance;

/* an optimization to say which common
menu buttons we want to initialize */
enum {AM_INIT_APPLY_BUTTON = 1,
	  AM_INIT_DEFAULT_BUTTON = 2,
	  AM_INIT_DONE_BUTTON = 4,
	  AM_INIT_ALL = 0xFFFF};

#endif /* _WINDOWS */

/* this one should be exported from the thotlib */
extern char  *ColorName (int num);

static int CacheStatus;
static int ProxyStatus;
static int SafePutStatus;

/* Cache menu options */
#ifdef _WINDOWS
static HWND CacheHwnd = NULL;
typedef struct _AM_WIN_MenuText {
   int idc;
   int message;
} AM_WIN_MenuText;

/* the message table text to IDC convertion table */
static AM_WIN_MenuText WIN_CacheMenuText[] = 
{
	{AM_INIT_ALL, AM_CACHE_MENU},
	{IDC_ENABLECACHE, AM_ENABLE_CACHE},
	{IDC_CACHEPROTECTEDDOCS, AM_CACHE_PROT_DOCS},
	{IDC_CACHEDISCONNECTEDMODE, AM_DISCONNECTED_MODE},
	{IDC_CACHEEXPIREIGNORE, AM_IGNORE_EXPIRES},
	{IDC_IDC_TCACHEDIRECTORY, AM_CACHE_DIR},
	{IDC_TCACHESIZE, AM_CACHE_SIZE},
	{IDC_TMAXCACHEFILE, AM_CACHE_ENTRY_SIZE},
	{ID_FLUSHCACHE, AM_FLUSH_CACHE_BUTTON},
	{0, 0}
};
#endif /* _WINDOWS */

static int      CacheBase;
static ThotBool EnableCache;
static ThotBool CacheProtectedDocs;
static ThotBool CacheDisconnectMode;
static ThotBool CacheExpireIgnore;
static char     CacheDirectory [MAX_LENGTH];
static int      CacheSize;
static int      MaxCacheFile;

/* Proxy menu options */
#ifdef _WINDOWS
static HWND     ProxyHwnd = NULL;
/* the message table text to IDC convertion table */
static AM_WIN_MenuText WIN_ProxyMenuText[] = 
{
	{AM_INIT_ALL, AM_PROXY_MENU},
	{IDC_THTTPPROXY, AM_HTTP_PROXY},
	{IDC_TPROXYDOMAIN, AM_PROXY_DOMAIN},
	{IDC_TSEPENTRIESSPACE, AM_PROXY_DOMAIN_INFO},
	{IDC_NOPROXY, AM_DONT_PROXY_DOMAIN},
	{IDC_ONLYPROXY, AM_ONLY_PROXY_DOMAIN},
	{0, 0}
};
#endif /* _WINDOWS */
static int      ProxyBase;
static char     HttpProxy [MAX_LENGTH];
static char     ProxyDomain [MAX_LENGTH];
static ThotBool ProxyDomainIsOnlyProxy;

/* General menu options */
#ifdef _WINDOWS
static char     AppHome [MAX_LENGTH];
static char     AppTmpDir [MAX_LENGTH];
static HWND     GeneralHwnd = NULL;
/* the message table text to IDC convertion table */
static AM_WIN_MenuText WIN_GeneralMenuText[] = 
{
	{AM_INIT_ALL, AM_GENERAL_MENU},
	{IDC_TAPPHOME, AM_USER_DIR},
	{IDC_TTMPDIR, AM_TMP_DIR},
	{IDC_THOMEPAGE, AM_HOME_PAGE},
	{IDC_TZOOM , AM_ZOOM},
	{IDC_TDIALOGUELANG,AM_DIALOGUE_LANGUAGE},
	{IDC_ACCESSKEY, AM_ACCESSKEY},
	{IDC_ANONE, AM_NONE},
	{IDC_MULTIKEY, AM_ENABLE_MULTIKEY},
	{IDC_BGIMAGES, AM_SHOW_BG_IMAGES},
	{IDC_DOUBLECLICK, AM_ENABLE_DOUBLECLICK},
	{IDC_ENABLEFTP, AM_ENABLE_FTP},
	{0, 0}
};
#endif /* _WINDOWS */
static int      GeneralBase;
static int      DoubleClickDelay;
static int      Zoom;
static ThotBool Multikey;
static char     DefaultName [MAX_LENGTH];
static ThotBool BgImages;
static ThotBool DoubleClick;
static char     DialogueLang [MAX_LENGTH];
static int      AccesskeyMod;
static int      FontMenuSize;
static char     HomePage [MAX_LENGTH];
static ThotBool EnableFTP;

/* Publish menu options */
#ifdef _WINDOWS
static HWND     PublishHwnd =  NULL;
static AM_WIN_MenuText WIN_PublishMenuText[] = 
{
	{AM_INIT_ALL, AM_PUBLISH_MENU},
	{IDC_LOSTUPDATECHECK, AM_USE_ETAGS},
	{IDC_VERIFYPUBLISH, AM_VERIFY_PUT},
	{IDC_TDEFAULTNAME, AM_DEFAULT_NAME},
	{IDC_TSAFEPUTREDIRECT, AM_SAFE_PUT_REDIRECT},
	{0, 0}
};
#endif /* _WINDOWS */
static int      PublishBase;
static ThotBool LostUpdateCheck;
static ThotBool VerifyPublish;
static char     SafePutRedirect [MAX_LENGTH];

/* Color menu options */
#ifdef _WINDOWS
static HWND     ColorHwnd = NULL;
static AM_WIN_MenuText WIN_ColorMenuText[] = 
{
	{AM_INIT_ALL, AM_COLOR_MENU},
	{IDC_TFGCOLOR, AM_DOC_FG_COLOR},
	{IDC_TBGCOLOR, AM_DOC_BG_COLOR},
	{IDC_TSECOLOR, AM_DOC_SEL_COLOR},
	{IDC_TINCOLOR, AM_DOC_INS_COLOR},
 	{IDC_CHANGCOLOR, AM_COLOR_PALETTE},
 	{IDC_CHANGCOLOR2, AM_COLOR_PALETTE},
	{0, 0}
};
#endif /* _WINDOWS */
static int      ColorBase;
static char     FgColor [MAX_LENGTH];
static char     BgColor [MAX_LENGTH];
static char     SelColor [MAX_LENGTH];
static char     InsColor [MAX_LENGTH];
#ifndef _WINDOWS
static char     MenuFgColor [MAX_LENGTH];
static char     MenuBgColor [MAX_LENGTH];
#endif /* !_WINDOWS */

/* Geometry menu options */
static int      GeometryBase;
static Document GeometryDoc = 0;
#ifdef _WINDOWS
HWND            GeometryHwnd = NULL;
static AM_WIN_MenuText WIN_GeometryMenuText[] = 
{
	{AM_INIT_DONE_BUTTON, AM_GEOMETRY_MENU},
	{IDC_GEOMCHANGE, AM_GEOMETRY_CHANGE},
	{ID_APPLY, AM_SAVE_GEOMETRY},
	{ID_DEFAULTS, AM_RESTORE_GEOMETRY},
	{0, 0}
};
#endif /* _WINDOWS */
/* common local variables */
static char    s[MAX_LENGTH]; /* general purpose buffer */

/* Language negotiation menu options */
#ifdef _WINDOWS
static HWND     LanNegHwnd = NULL;
static AM_WIN_MenuText WIN_LanNegMenuText[] = 
{
	{AM_INIT_ALL, AM_LANNEG_MENU},
	{IDC_TLANNEG, AM_LANG_NEGOTIATION},
	{0, 0}
};
#endif /* _WINDOWS */
static int      LanNegBase;
static char     LanNeg [MAX_LENGTH];

/* Profile menu options */
#ifdef _WINDOWS
static HWND     ProfileHwnd = NULL;
static HWND     wndProfilesList;
static HWND     wndProfile;
static AM_WIN_MenuText WIN_ProfileMenuText[] = 
{
	{AM_INIT_ALL, AM_PROFILE_MENU},
	{IDC_TPROFILESLOCA, AM_PROFILES_FILE},
	{IDC_TPROFILESELECT, AM_PROFILE_SELECT},
	{IDC_TPROFILECHANGE, AM_PROFILE_CHANGE},
	{0, 0}
};
#endif /* _WINDOWS */
static int      ProfileBase;
static char     Profile [MAX_LENGTH];
static char     Profiles_File [MAX_LENGTH];
#define MAX_PRO 50
static char    *MenuText[MAX_PRO];

/* Templates menu option */
#ifdef _WINDOWS
static HWND     TemplatesHwnd = NULL;
static AM_WIN_MenuText WIN_TemplatesMenuText[] = 
{
	{AM_INIT_ALL, AM_TEMPLATES_MENU},
	{IDC_TTEMPLATESURL, AM_TEMPLATES_SERVER},
	{0, 0}
};
#endif /* _WINDOWS */
static int      TemplatesBase;
static char     TemplatesUrl [MAX_LENGTH];
static int      CurrentProfile = -1;

#ifdef ANNOTATIONS
/* Annotation menu option */
#ifdef _WINDOWS
static HWND     AnnotHwnd = NULL;
static AM_WIN_MenuText WIN_AnnotMenuText[] = 
{
	{AM_INIT_ALL, AM_ANNOT_CONF_MENU},
	{IDC_TANNOTUSER, AM_ANNOT_USER},
	{IDC_TANNOTPOSTSERVER, AM_ANNOT_POST_SERVER},
	{IDC_TANNOTSERVERS, AM_ANNOT_SERVERS},
	{IDC_ANNOTLAUTOLOAD, AM_ANNOT_LAUTOLOAD},
	{IDC_ANNOTRAUTOLOAD, AM_ANNOT_RAUTOLOAD},
	{IDC_ANNOTRAUTOLOADRST, AM_ANNOT_RAUTOLOAD_RST},
	{0, 0}
};
#endif /* _WINDOWS */
static int      AnnotBase;
static char     AnnotUser [MAX_LENGTH];
static char     AnnotPostServer [MAX_LENGTH];
static char     AnnotServers [MAX_LENGTH];
static ThotBool AnnotLAutoLoad;
static ThotBool AnnotRAutoLoad;
static ThotBool AnnotRAutoLoadRst;
#endif /* ANNOTATIONS */

#include "query_f.h"
#include "init_f.h"



/*
** Common functions
*/

/*-----------------------------------------------------------------------
   _GetSysUserName
   Gives ptr the value of the system's user name. 
   If succesful, returns TRUE, FALSE otherwise.
  -----------------------------------------------------------------------*/
static ThotBool _GetSysUserName (char *username)
{
#ifdef _WINDOWS
  ThotBool  status;
  DWORD     dwSize;

  /* compute the default app_home value from the username and thotdir */
  dwSize = MAX_LENGTH * sizeof (char);
  status = GetUserName (username, &dwSize);
  if (!status || *username == EOS)
    return FALSE;
  /* in principle, username is returned in Unicode */
#else 
  uid_t           uid;
  struct passwd  *pwd;
  char           *pw_name;

  uid = getuid ();
  pwd = getpwuid (uid);
  if (!pwd)
    return FALSE;

  pw_name = pwd->pw_name;
  if (!pw_name || *pw_name == EOS)
    return FALSE;
  strncpy (username, pw_name, MAX_LENGTH - 1);
  username[MAX_LENGTH - 1] = EOS;
#endif /* _WINDOWS */

  return TRUE;
}

/*----------------------------------------------------------------------
   GetEnvString: front end to TtaGetEnvString. If the variable name doesn't
   exist, it sets the value to an empty ("") string
   ----------------------------------------------------------------------*/
static void  GetEnvString (char *name, char  *value)
{
  char   *ptr;

  ptr = TtaGetEnvString (name);
  if (ptr)
    {
      strncpy (value, ptr, MAX_LENGTH);
      value[MAX_LENGTH-1] = EOS;
    }
  else
    value[0] = EOS;
}

/*----------------------------------------------------------------------
  InitAmayaDefEnv
  Initializes the default Amaya options which are not setup by thot.ini file.
  This protects us against a crash due to a user's erasing that file.
  ----------------------------------------------------------------------*/
void InitAmayaDefEnv (void)
{
  char        *ptr,  *ptr2;
  char         username[MAX_LENGTH];
  ThotBool     annot_rautoload, annot_rautoload_rst;

  /* browsing editing options */
  ptr = TtaGetEnvString ("THOTDIR");
  if (ptr != NULL)
    {
      strcpy (HomePage, ptr);
      strcat (HomePage, AMAYA_PAGE);
    }
  else
    HomePage[0]  = EOS;
  TtaSetDefEnvString ("HOME_PAGE", HomePage, FALSE);
  HomePage[0] = EOS;
  TtaSetDefEnvString ("ENABLE_MULTIKEY", "no", FALSE);
  TtaSetDefEnvString ("ENABLE_BG_IMAGES", "yes", FALSE);
  TtaSetDefEnvString ("VERIFY_PUBLISH", "no", FALSE);
  TtaSetDefEnvString ("ENABLE_LOST_UPDATE_CHECK", "yes", FALSE);
  TtaSetDefEnvString ("DEFAULTNAME", "Overview.html", FALSE);
  TtaSetDefEnvString ("FontMenuSize", "12", FALSE);
  TtaSetDefEnvString ("ENABLE_DOUBLECLICK", "yes", FALSE);
  /* @@@ */
  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &DoubleClick);
  /* @@@ */
  TtaSetDefEnvString ("ENABLE_FTP", "no", FALSE);
  
#ifndef _WINDOWS
  TtaSetDefEnvString ("THOTPRINT", "lpr", FALSE);
#endif
  /* network configuration */
  TtaSetDefEnvString ("SAFE_PUT_REDIRECT", "", FALSE);
  TtaSetDefEnvString ("ENABLE_LOST_UPDATE_CHECK", "yes", FALSE);
  TtaSetDefEnvString ("ENABLE_PIPELINING", "yes", FALSE);
  TtaSetDefEnvString ("NET_EVENT_TIMEOUT", "60000", FALSE);
  TtaSetDefEnvString ("PERSIST_CX_TIMEOUT", "60", FALSE);
  TtaSetDefEnvString ("DNS_TIMEOUT", "1800", FALSE);
  TtaSetDefEnvString ("MAX_SOCKET", "32", FALSE);
  TtaSetDefEnvString ("ENABLE_MDA", "yes", FALSE);
  TtaSetDefEnvString ("HTTP_PROXY", "", FALSE);
  TtaSetDefEnvString ("PROXYDOMAIN", "", FALSE);
  TtaSetDefEnvString ("PROXYDOMAIN_IS_ONLYPROXY", "no", FALSE);
  TtaSetDefEnvString ("MAX_CACHE_ENTRY_SIZE", "3", FALSE);
  TtaSetDefEnvString ("CACHE_SIZE", "10", FALSE);
  if (TempFileDirectory)
  {
    sprintf (s, "%s%clibwww-cache", TempFileDirectory, DIR_SEP);
    TtaSetDefEnvString ("CACHE_DIR", s, FALSE);
	TtaSetDefEnvString ("ENABLE_CACHE", "yes", FALSE);
  }
  else
  {
    TtaSetDefEnvString ("CACHE_DIR", "", FALSE);
	TtaSetDefEnvString ("ENABLE_CACHE", "yes", FALSE);
  }
  TtaSetDefEnvString ("CACHE_PROTECTED_DOCS", "yes", FALSE);
  TtaSetDefEnvString ("CACHE_DISCONNECTED_MODE", "no", FALSE);
  TtaSetDefEnvString ("CACHE_EXPIRE_IGNORE", "no", FALSE);

  /* annotations */
#ifdef ANNOTATIONS
  TtaSetDefEnvString ("ANNOT_POST_SERVER", "", FALSE);
  TtaSetDefEnvString ("ANNOT_SERVERS", "localhost", FALSE);
  TtaSetDefEnvString ("ANNOT_LAUTOLOAD", "no", FALSE);
  TtaSetDefEnvString ("ANNOT_RAUTOLOAD", "no", FALSE);
  TtaSetDefEnvString ("ANNOT_RAUTOLOAD_RST", "yes", FALSE);
  TtaSetEnvString ("ANNOT_MAIN_INDEX", "annot.index", FALSE);
  ptr = TtaGetEnvString ("APP_HOME");
  ptr2 = TtaGetMemory ( strlen (ptr) + strlen ("annotations")
			+ 2);
  sprintf (ptr2, "%s%c%s", ptr, DIR_SEP, "annotations");
  TtaSetDefEnvString ("ANNOT_DIR", ptr2, FALSE);
  TtaFreeMemory (ptr2);
  /* set up the default annotation user name */
  if (!_GetSysUserName (username))
    username[0] = EOS;
  TtaSetDefEnvString ("ANNOT_USER", username, FALSE);
  /* reset remote annotations autoload ?*/
  TtaGetEnvBoolean ("ANNOT_RAUTOLOAD_RST", &annot_rautoload_rst);
  TtaGetEnvBoolean ("ANNOT_RAUTOLOAD", &annot_rautoload);
  if (annot_rautoload_rst && annot_rautoload)
    TtaSetEnvBoolean ("ANNOT_RAUTOLOAD", FALSE, TRUE);
#endif /* ANNOTATIONS */

  /* appearance */
}

/*----------------------------------------------------------------------
   GetDefEnvToggleBoolean: front end to TtaGetDefEnvBoolean. It takes
   care of switching the toggle button according to the status of the
   variable.
  ----------------------------------------------------------------------*/
static void GetDefEnvToggle (char *name, ThotBool *value, int ref, int entry)
{
  ThotBool old = *value;

  TtaGetDefEnvBoolean (name, value);
  if (*value != old)
    /* change the toggle button state */
    {
#ifndef _WINDOWS
      TtaSetToggleMenu (ref, entry, *value);
#endif /* WINDOWS */
    }
}

/*----------------------------------------------------------------------
   GetDefEnvString: front end to TtaGetDefEnvString. If the variable name 
   doesn't exist, it sets the value to an empty ("") string
  ----------------------------------------------------------------------*/
static void GetDefEnvString (char *name, char  *value)
{
  char  *ptr;

  ptr = TtaGetDefEnvString (name);
  if (ptr)
    {
      strncpy (value, ptr, MAX_LENGTH);
      value[MAX_LENGTH-1] = EOS;
    }
  else
    value[0] = EOS;
}

/*----------------------------------------------------------------------
  NormalizeDirName
  verifies if dirname finishes with end_path. If this is so, it returns FALSE
  Otherwise, adds end_path to dirname and returns TRUE.
  end_path should begin with a DIR_SEP char 
  ----------------------------------------------------------------------*/
static int NormalizeDirName (char *dirname, const char *end_path)
{
  int           result = 0;
  char         *ptr;
  char         *dir_sep = NULL;

  if (dirname[0] != EOS)
    {
      /* if dirname ends in DIR_SEP, we remove it */
      if (dirname [strlen (dirname) -1] == DIR_SEP)
	{
	  dir_sep = strrchr (dirname, DIR_SEP);
	  *dir_sep = EOS;
	  result = 1;
	}
      ptr = strstr (dirname, end_path);
      if (ptr)
	{
	  if (strcasecmp (ptr, end_path))
	    /* end_path missing, add it to the parent dir */
	    {
	      strcat (dirname, end_path);
	      result = 1;
	    }
	}
      else
	/* no DIR_SEP, so we add the end_path */
	{
	  strcat (dirname, end_path);
	  result = 1;
	}
    }
  else
    /* empty dirname! */
    result = 1;
  
  return result;
}

#ifdef _WINDOWS
/*----------------------------------------------------------------------
  AmCopyFile
  Copies a file from one dir to another dir. If the file doesn't exist,
  doesn't do anything.  
  ----------------------------------------------------------------------*/
static void AmCopyFile (const char  *source_dir, const char  *dest_dir,
			const char  *filename)
{
 char   source_file [MAX_LENGTH];
 
 sprintf (source_file, "%s%c%s", source_dir, DIR_SEP, filename);
 if (TtaFileExist (source_file))
 {
   sprintf (s, "%s%c%s", dest_dir, DIR_SEP, filename);
   TtaFileCopy (source_file, s);
 }
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  CleanDirSep
  Removes double DIR_SEP strings in a name. Returns TRUE if such
  operation was done.
  ----------------------------------------------------------------------*/
int CleanDirSep (char  *name)
{
 int result = 0;
 int s, d;

  /* remove all double DIR_SEP */
  s = 0;
  d = 0;
  while (name[d] != EOS)
  {
    if (name[d] == DIR_SEP && name[d + 1] == DIR_SEP)
      {
	result = 1;
	d++;
	continue;
      }
    name[s] = name[d];
    s++;
    d++;
  }
  name[s] = EOS;

  return (result);
}

/*----------------------------------------------------------------------
  CleanFirstLastSpace
  Removes the first and last space in a name. Returns TRUE if such
  operation was done.
  Returns 1 if it made any change, 0 otherwise (not boolean).
  ----------------------------------------------------------------------*/
int CleanFirstLastSpace (char  *name)
{
 int result = 0;
 int l, d;

 if (!name ||  *name == EOS)
   return (0);

 /* start by removing the ending spaces */
 l = strlen (name) - 1;
 while (l > 0 && name[l] == ' ')
   l--;
 if (name[l+1] == ' ')
   {
     result = 1;
     name[l+1] = EOS;
   }

 /* now remove the leading spaces */
 l = 0;
 while (name[l] == ' ' && name[l] != EOS)
   l++;
 if (l > 0)
   {
     result = 1;
     d = 0;
     while (name[l] != EOS)
       {
	 name[d] = name[l];
	 d++;
	 l++;
       }
     name[d] = name[l];
   }

  return (result);
}

/*----------------------------------------------------------------------
  RemoveLastDirSep
  Removes the last char of name if it is a DIR_SEP. Return TRUE if it
  does this operation.
  ----------------------------------------------------------------------*/
static int RemoveLastDirSep (char  *name)
{
  int result;
  int last_char;

  result = 0;
  if (name) 
    { 
        last_char = strlen (name) - 1;
        if (name[last_char] == DIR_SEP)
          {
                name[last_char] = EOS;
                result = 1;
          }
    }

  return result;
}

#ifdef _WINDOWS

/*----------------------------------------------------------------------
  FilterSpaces
  If removeAll is true, all spaces in string are removed, otherwise, all
  duplicate spaces are converted into a single space.
  If any spaces were found, it returns 1. 0 otherwise.
  ----------------------------------------------------------------------*/
static int FilterSpaces (char  *string, ThotBool removeAll)
{
  char   *target, *source;
  int result = 0;

  if (string)
  {
   source = target = string;
   while (*source)
   {
	if (*source == ' ')
	{
		result = 1;
		/* skip to the first non space char */
       while (*source == ' ')
			source++;
	   /* filter the space */
 	   if (!removeAll)
	   {
	      *target = ' ';
		  target++;
	   }
	   if (*source == EOS)
		   break;
	}
	  /* copy the char and pass on to the next one */
	  *target = *source;
	  source++;
	  target++;
	}
   *target = *source;
   }
  return result;
}

/*----------------------------------------------------------------------
  ConvertSpaceNLI
  Converts spaces in source into \n if the toNL is TRUE. Otherwise, does
  the opposite convertion.
  Returns 1 if it did any substitutions, 0 otherwise.
  ----------------------------------------------------------------------*/
static int ConvertSpaceNL (char  *source, ThotBool toNL)
{
  int result;
  char   target[MAX_LENGTH];
  char   *s, *t;

  if (source) 
    {
	  /* remove all spaces before starting */
	  if (!toNL)
         result = FilterSpaces (source, TRUE);
	  s = source;
	  t = target;
      while (*s)
	  {
		  if (toNL && *s == ' ')
		  {
			  *t++ = 13;
			  *t++ = 10;
			  s++;
			  result = 1;
		  }
		  else if (!toNL && *s == 13)
		  {
		     *t++ = ' ';
			  s++;
			  s++;
			  result = 1;
		  }
		  else 
		  {
	          *t++ = *s++;
		  }
	  }
	  *t = *s;
	  if (result)
	  {
		  if (!toNL)
		    /* remove duplicate spaces, coming from empty lines */
		    FilterSpaces (target, FALSE);
		  strcpy (source, target);
	  }
  }
  return result;
}
#endif /*_WINDOWS */

#ifdef _WINDOWS

/*----------------------------------------------------------------------
  WIN_SetCommonText
  Writes the local text that corresponds to buttons common to all the
  menus
  ----------------------------------------------------------------------*/
static void WIN_SetCommonText (HWND hwnDlg, int flags)
{
  if (flags & AM_INIT_APPLY_BUTTON)
    SetWindowText (GetDlgItem (hwnDlg, ID_APPLY),
		   TtaGetMessage (AMAYA, AM_APPLY_BUTTON));
  if (flags & AM_INIT_DEFAULT_BUTTON)
    SetWindowText (GetDlgItem (hwnDlg, ID_DEFAULTS),
		   TtaGetMessage (AMAYA, AM_DEFAULT_BUTTON));	    
  if (flags & AM_INIT_DONE_BUTTON)
    SetWindowText (GetDlgItem (hwnDlg, ID_DONE),
		   TtaGetMessage (LIB, TMSG_DONE));
}

/*----------------------------------------------------------------------
  WIN_SetMenuText
  Writes the local text message from a given message table to the idc
  dialogue identifier.
  ----------------------------------------------------------------------*/
static void WIN_SetMenuText (HWND hwnDlg, AM_WIN_MenuText menu[])
{
  AM_WIN_MenuText *field;
  int i;
  
  if (!hwnDlg || !menu)
    return;
  /* intialize the window title */
  field = &menu[0];
  /* for the moment, we consider all messages come from the AMAYA table */
  SetWindowText (hwnDlg, TtaGetMessage (AMAYA, field->message));
  
  /* initialize the common buttons, 
     the idc field says which buttons we want to intialize */
  WIN_SetCommonText (hwnDlg, field->idc);
  
  /* intialize the menu fields */
  i = 1;
  field = &menu[i];
  while (field->idc != 0 && field->message != 0)
    {
      SetWindowText (GetDlgItem (hwnDlg, field->idc),
		     TtaGetMessage (AMAYA, field->message));
      i++;
      field = &menu[i];
    }
}

#endif /* _WINDOWS */
  
/*********************
** Cache configuration menu
***********************/
/*----------------------------------------------------------------------
  GetCacheConf
  Makes a copy of the current registry cache values
  ----------------------------------------------------------------------*/
static void GetCacheConf (void)
{
  TtaGetEnvBoolean ("ENABLE_CACHE", &EnableCache);
  TtaGetEnvBoolean ("CACHE_PROTECTED_DOCS", &CacheProtectedDocs);
  TtaGetEnvBoolean ("CACHE_DISCONNECTED_MODE", &CacheDisconnectMode);
  TtaGetEnvBoolean ("CACHE_EXPIRE_IGNORE", &CacheExpireIgnore);
  GetEnvString ("CACHE_DIR", CacheDirectory);
  TtaGetEnvInt ("CACHE_SIZE", &CacheSize);
  TtaGetEnvInt ("MAX_CACHE_ENTRY_SIZE", &MaxCacheFile);
}

/*----------------------------------------------------------------------
  ValidateCacheConf
  Validates the entries in the Cache nonf menu. If there's an invalid
  entry, we then use the default value. We need this because
  the Windows interface isn't rich enough to do it (e.g., negative numbers
  in the integer entries)
  ----------------------------------------------------------------------*/
static void ValidateCacheConf (void)
{
 int change;

#ifdef _WINDOWS
 /* validate the cache size */
 change = 1;
 if (CacheSize < 1)
   CacheSize =1;
 else if (CacheSize > 100)
   CacheSize = 100;
 else
   change = 0;
 if (change)
   SetDlgItemInt (CacheHwnd, IDC_CACHESIZE, CacheSize, FALSE);
 
 /* validate the cache entry size */
 change = 1;
 if (MaxCacheFile < 1)
   MaxCacheFile = 1;
 else if (MaxCacheFile > 5)
   MaxCacheFile = 5;
 else
   change = 0;
 if (change)
   SetDlgItemInt (CacheHwnd, IDC_MAXCACHEFILE, MaxCacheFile, FALSE);
#endif /* _WINDOWS */

 /* validate the cache dir */
 change = 0;
 change += CleanFirstLastSpace (CacheDirectory);
 change += CleanDirSep (CacheDirectory);
 /* remove the last DIR_SEP, if we have it */
 change += RemoveLastDirSep (CacheDirectory);
 if (CacheDirectory[0] == EOS)
 {
   GetDefEnvString ("CACHE_DIR", CacheDirectory);
   change = 1;
 }

 /* what we do is add a DIR_STRlibwww-cache */
 /* remove the last DIR_SEP, if we have it (twice, to 
    protect against a bad "user" default value */
 change += RemoveLastDirSep (CacheDirectory);
 /* n.b., this variable may be empty */
#ifdef _WINDOWS
  change += NormalizeDirName (CacheDirectory, "\\libwww-cache");
#else
  change += NormalizeDirName (CacheDirectory, "/libwww-cache");
#endif /* _WINDOWS */
  if (change)
#ifdef _WINDOWS
    SetDlgItemText (CacheHwnd, IDC_CACHEDIRECTORY, CacheDirectory);
#else
    TtaSetTextForm (CacheBase + mCacheDirectory, CacheDirectory);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  SetCacheConf
  Updates the registry cache values
  ----------------------------------------------------------------------*/
static void SetCacheConf (void)
{
  TtaSetEnvBoolean ("ENABLE_CACHE", EnableCache, TRUE);
  TtaSetEnvBoolean ("CACHE_PROTECTED_DOCS", CacheProtectedDocs, TRUE);
  TtaSetEnvBoolean ("CACHE_DISCONNECTED_MODE", CacheDisconnectMode, TRUE);
  TtaSetEnvBoolean ("CACHE_EXPIRE_IGNORE", CacheExpireIgnore, TRUE);
  TtaSetEnvString ("CACHE_DIR", CacheDirectory, TRUE);
  TtaSetEnvInt ("CACHE_SIZE", CacheSize, TRUE);
  TtaSetEnvInt ("MAX_CACHE_ENTRY_SIZE", MaxCacheFile, TRUE);

  TtaSaveAppRegistry ();
}

/*----------------------------------------------------------------------
  GetDefaultCacheConf
  Updates the registry cache values and calls the cache functions
  to take into acocunt the changes
  ----------------------------------------------------------------------*/
static void GetDefaultCacheConf ()
{
  /* read the default values */
  GetDefEnvToggle ("ENABLE_CACHE", &EnableCache, CacheBase + mCacheOptions, 0);
  GetDefEnvToggle ("CACHE_PROTECTED_DOCS", &CacheProtectedDocs, CacheBase + mCacheOptions, 1);
  GetDefEnvToggle ("CACHE_DISCONNECTED_MODE", &CacheDisconnectMode, CacheBase + mCacheOptions, 2);
  GetDefEnvToggle ("CACHE_EXPIRE_IGNORE", &CacheExpireIgnore, CacheBase + mCacheOptions, 3);
  GetDefEnvString ("CACHE_DIR", CacheDirectory);
  TtaGetDefEnvInt ("CACHE_SIZE", &CacheSize);
  TtaGetDefEnvInt ("MAX_CACHE_ENTRY_SIZE", &MaxCacheFile);
}

#ifdef _WINDOWS
/*----------------------------------------------------------------------
  WIN_RefreshCacheMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void WIN_RefreshCacheMenu (HWND hwnDlg)
{
  CheckDlgButton (hwnDlg, IDC_ENABLECACHE, (EnableCache)
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_CACHEPROTECTEDDOCS, (CacheProtectedDocs)
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_CACHEDISCONNECTEDMODE, (CacheDisconnectMode)
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_CACHEEXPIREIGNORE, (CacheExpireIgnore)
		  ? BST_CHECKED : BST_UNCHECKED);
  SetDlgItemText (hwnDlg, IDC_CACHEDIRECTORY, CacheDirectory);
  SetDlgItemInt (hwnDlg, IDC_CACHESIZE, CacheSize, FALSE);
  SetDlgItemInt (hwnDlg, IDC_MAXCACHEFILE, MaxCacheFile, FALSE);
}
#else /* WINDOWS */
/*----------------------------------------------------------------------
  RefreshCacheMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void RefreshCacheMenu ()
{
  /* set the menu entries to the current values */
  TtaSetToggleMenu (CacheBase + mCacheOptions, 0, EnableCache);
  TtaSetToggleMenu (CacheBase + mCacheOptions, 1, CacheProtectedDocs);
  TtaSetToggleMenu (CacheBase + mCacheOptions, 2, CacheDisconnectMode);
  TtaSetToggleMenu (CacheBase + mCacheOptions, 3, CacheExpireIgnore);
  if (CacheDirectory)
    TtaSetTextForm (CacheBase + mCacheDirectory, CacheDirectory);
  TtaSetNumberForm (CacheBase + mCacheSize, CacheSize);
  TtaSetNumberForm (CacheBase + mMaxCacheFile, MaxCacheFile);
}
#endif /* !_WINDOWS */

#ifdef _WINDOWS
/*----------------------------------------------------------------------
  WIN_CacheDlgProc
  windows callback for the cache configuration menu
  ----------------------------------------------------------------------*/
LRESULT CALLBACK WIN_CacheDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, 
				   LPARAM lParam)
{ 
  switch (msg)
    {
    case WM_INITDIALOG:
      CacheHwnd = hwnDlg;
      /* initialize the menu text */
      WIN_SetMenuText (hwnDlg, WIN_CacheMenuText);
      /* write the current values in the dialog entries */
      WIN_RefreshCacheMenu (hwnDlg);
      break;
      
    case WM_CLOSE:
    case WM_DESTROY:
      /* reset the status flag */
      CacheHwnd = NULL;
      EndDialog (hwnDlg, ID_DONE);
      break;

    case WM_COMMAND:
      if (HIWORD (wParam) == EN_UPDATE)
	{
          switch (LOWORD (wParam))
	    {
	    case IDC_CACHEDIRECTORY:
	      GetDlgItemText (hwnDlg, IDC_CACHEDIRECTORY, CacheDirectory,
			      sizeof (CacheDirectory) - 1);
		  CacheStatus |= AMAYA_CACHE_RESTART;
		  break;
	    case IDC_CACHESIZE:
	      CacheSize = GetDlgItemInt (hwnDlg, IDC_CACHESIZE, FALSE, FALSE);
	      CacheStatus |= AMAYA_CACHE_RESTART;
	      break;
	    case IDC_MAXCACHEFILE:
	      MaxCacheFile = GetDlgItemInt (hwnDlg, IDC_MAXCACHEFILE, FALSE, FALSE);
	      CacheStatus |= AMAYA_CACHE_RESTART;
          break;			
	    }
	}
      switch (LOWORD (wParam))
	{
	case IDC_ENABLECACHE:
      CacheStatus |= AMAYA_CACHE_RESTART;
	  EnableCache = !EnableCache;
	  break;
	case IDC_CACHEPROTECTEDDOCS:
	  CacheStatus |= AMAYA_CACHE_RESTART;
	  CacheProtectedDocs = !CacheProtectedDocs;
	  break;
	case IDC_CACHEDISCONNECTEDMODE:
	  CacheStatus |= AMAYA_CACHE_RESTART;
	  CacheDisconnectMode = !CacheDisconnectMode;
	  break;
	case IDC_CACHEEXPIREIGNORE:
	  CacheStatus |= AMAYA_CACHE_RESTART;
	  CacheExpireIgnore = !CacheExpireIgnore;
	  break;

	  /* action buttons */
	case ID_APPLY:
	  ValidateCacheConf ();
	  SetCacheConf ();
	  libwww_updateNetworkConf (CacheStatus);
	  CacheStatus = 0;
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_FLUSHCACHE:
	  StopAllRequests (1);
	  libwww_CleanCache ();
	  break;
	case ID_DONE:
	  CacheHwnd = NULL;
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DEFAULTS:
	  GetDefaultCacheConf ();
	  WIN_RefreshCacheMenu (hwnDlg);
	  /* always signal this as modified */
      CacheStatus |= AMAYA_CACHE_RESTART;
	  break;
	}
      break;	     
    default: return FALSE;
    }
  return TRUE;
}
#else /* _WINDOWS */
/*----------------------------------------------------------------------
  CacheCallbackDialog
  callback of the cache configuration menu
  ----------------------------------------------------------------------*/
static void     CacheCallbackDialog (int ref, int typedata, char *data)
{
  int                 val;

  if (ref == -1)
    {
      /* removes the cache conf menu */
      TtaDestroyDialogue (CacheBase + CacheMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (int) data;
      switch (ref - CacheBase)
	{
	case CacheMenu:
	  switch (val) 
	    {
	    case 0:
	      TtaDestroyDialogue (ref);
	      break;
	    case 1:
	      ValidateCacheConf ();
	      SetCacheConf ();
	      libwww_updateNetworkConf (CacheStatus);
	      /* reset the status flag */
	      CacheStatus = 0;
	      TtaDestroyDialogue (ref);
	      break;
	    case 2:
	      GetDefaultCacheConf ();
	      RefreshCacheMenu ();
	      /* always signal this as modified */
	      CacheStatus |= AMAYA_CACHE_RESTART;
	      break;
	    case 3:
	      /* @@ docid isn't used! */
	      StopAllRequests (1);
	      libwww_CleanCache ();
	      break;

	    default:
	      break;
	    }
	  break;

	case mCacheOptions:
	  switch (val) 
	    {
	    case 0:
	      CacheStatus |= AMAYA_CACHE_RESTART;
	      EnableCache = !EnableCache;
	      break;
	    case 1:
	      CacheStatus |= AMAYA_CACHE_RESTART;
	      CacheProtectedDocs = !CacheProtectedDocs;
	      break;
	    case 2:
	      CacheStatus |= AMAYA_CACHE_RESTART;
	      CacheDisconnectMode = !CacheDisconnectMode;
	      break;
	    case 3:
	      CacheStatus |= AMAYA_CACHE_RESTART;
	      CacheExpireIgnore = !CacheExpireIgnore;
	      break;

	    default:
	      break;
	    }
	  break;
	  
	case mCacheDirectory:
	  CacheStatus |= AMAYA_CACHE_RESTART;
	  if (data)
	    strcpy (CacheDirectory, data);
	  else
	    CacheDirectory [0] = EOS;
	  break;
	case mCacheSize:
	  CacheStatus |= AMAYA_CACHE_RESTART;
	  CacheSize = val;
	  break;
	case mMaxCacheFile:
	  CacheStatus |= AMAYA_CACHE_RESTART;
	  MaxCacheFile = val;
	  break;

	default:
	  break;
	}
    }
}
#endif /* !_WINDOWS */

/*----------------------------------------------------------------------
  CacheConfMenu
  Build and display the Conf Menu dialog box and prepare for input.
  ----------------------------------------------------------------------*/
void         CacheConfMenu (Document document, View view)
{
#ifndef _WINDOWS
   int              i;

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_DEFAULT_BUTTON));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_FLUSH_CACHE_BUTTON));
   TtaNewSheet (CacheBase + CacheMenu, 
		TtaGetViewFrame (document, view),
		TtaGetMessage (AMAYA, AM_CACHE_MENU),
		3, s, FALSE, 6, 'L', D_DONE);

   sprintf (s, "B%s%cB%s%cB%s%cB%s",
	    TtaGetMessage (AMAYA, AM_ENABLE_CACHE), EOS, 
	    TtaGetMessage (AMAYA, AM_CACHE_PROT_DOCS), EOS,
	    TtaGetMessage (AMAYA, AM_DISCONNECTED_MODE), EOS,
	    TtaGetMessage (AMAYA, AM_IGNORE_EXPIRES));
   TtaNewToggleMenu (CacheBase + mCacheOptions,
		     CacheBase + CacheMenu,
		     NULL,
		     4,
		     s,
		     NULL,
		     TRUE);
   TtaNewTextForm (CacheBase + mCacheDirectory,
		   CacheBase + CacheMenu,
		   TtaGetMessage (AMAYA, AM_CACHE_DIR),
		   40,
		   1,
		   TRUE);
   TtaNewNumberForm (CacheBase + mCacheSize,
		     CacheBase + CacheMenu,
		     TtaGetMessage (AMAYA, AM_CACHE_SIZE),
		     1,
		     100,
		     TRUE);
   TtaNewNumberForm (CacheBase + mMaxCacheFile,
		     CacheBase + CacheMenu,
		     TtaGetMessage (AMAYA, AM_CACHE_ENTRY_SIZE),
		     1,
		     5,
		     TRUE);
#endif /* !_WINDOWS */
   /* reset the modified flag */
   CacheStatus = 0;
   /* load and display the current values */
   GetCacheConf ();
#ifndef _WINDOWS
   RefreshCacheMenu ();
  /* display the menu */
   TtaSetDialoguePosition ();
   TtaShowDialogue (CacheBase + CacheMenu, TRUE);
#else /* !_WINDOWS */
  if (!CacheHwnd)
    /* only activate the menu if it isn't active already */
	  DialogBox (hInstance, MAKEINTRESOURCE (CACHEMENU), NULL, 
		     (DLGPROC) WIN_CacheDlgProc);
  else
    SetFocus (CacheHwnd);
#endif /* !_WINDOWS */
}


/*********************
** Proxy configuration menu
***********************/
/*----------------------------------------------------------------------
  GetProxyConf
  Makes a copy of the current registry proxy values
  ----------------------------------------------------------------------*/
static void GetProxyConf (void)
{
  GetEnvString ("HTTP_PROXY", HttpProxy);
  GetEnvString ("PROXYDOMAIN", ProxyDomain);
  TtaGetEnvBoolean ("PROXYDOMAIN_IS_ONLYPROXY", &ProxyDomainIsOnlyProxy);
}

/*----------------------------------------------------------------------
  SetProxyConf
  Updates the registry proxy values
  ----------------------------------------------------------------------*/
static void SetProxyConf (void)
{
  TtaSetEnvString ("HTTP_PROXY", HttpProxy, TRUE);
  TtaSetEnvString ("PROXYDOMAIN", ProxyDomain, TRUE);
  TtaSetEnvBoolean ("PROXYDOMAIN_IS_ONLYPROXY", ProxyDomainIsOnlyProxy,
		    TRUE);

  TtaSaveAppRegistry ();
}

/*----------------------------------------------------------------------
  GetDefaultProxyConf
  Updates the registry proxy values and calls the proxy functions
  to take into acocunt the changes
  ----------------------------------------------------------------------*/
static void GetDefaultProxyConf ()
{
  /* read the default values */
  GetDefEnvString ("HTTP_PROXY", HttpProxy);
  GetDefEnvString ("PROXYDOMAIN", ProxyDomain);
  TtaGetDefEnvBoolean ("PROXYDOMAIN_IS_ONLYPROXY", &ProxyDomainIsOnlyProxy);
}

#ifdef _WINDOWS
/*----------------------------------------------------------------------
  WIN_RefreshProxyMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
void WIN_RefreshProxyMenu (HWND hwnDlg)
{
  SetDlgItemText (hwnDlg, IDC_HTTPPROXY, HttpProxy);
  SetDlgItemText (hwnDlg, IDC_PROXYDOMAIN, ProxyDomain);
  if (ProxyDomainIsOnlyProxy)
    CheckRadioButton (hwnDlg, IDC_NOPROXY, IDC_ONLYPROXY, IDC_ONLYPROXY);
  else
    CheckRadioButton (hwnDlg, IDC_NOPROXY, IDC_ONLYPROXY, IDC_NOPROXY);
}
#else /* WINDOWS */
/*----------------------------------------------------------------------
  RefreshProxyMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void RefreshProxyMenu ()
{
  /* set the menu entries to the current values */
  TtaSetTextForm (ProxyBase + mHttpProxy, HttpProxy);
  TtaSetTextForm (ProxyBase + mProxyDomain, ProxyDomain);
  TtaSetMenuForm (ProxyBase + mToggleProxy, ProxyDomainIsOnlyProxy);
}
#endif /* !_WINDOWS */

#ifdef _WINDOWS
/*----------------------------------------------------------------------
  WIN_ProxyDlgProc
  Windows callback for the proxy menu
  ----------------------------------------------------------------------*/
LRESULT CALLBACK WIN_ProxyDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam,
				     LPARAM lParam)
{
  switch (msg)
    {
    case WM_INITDIALOG:
      ProxyHwnd = hwnDlg;
      /* initialize the menu text */
      WIN_SetMenuText (hwnDlg, WIN_ProxyMenuText);
      /* write the current values in the dialog entries */
      WIN_RefreshProxyMenu (hwnDlg);
      break;
      
    case WM_CLOSE:
    case WM_DESTROY:
      /* reset the status flag */
      ProxyHwnd = NULL;
      EndDialog (hwnDlg, ID_DONE);
      break;

    case WM_COMMAND:
      if (HIWORD (wParam) == EN_UPDATE)
	{
          switch (LOWORD (wParam))
	    {
	    case IDC_HTTPPROXY:
	      GetDlgItemText (hwnDlg, IDC_HTTPPROXY, HttpProxy,
			      sizeof (HttpProxy) - 1);
	      ProxyStatus |= AMAYA_PROXY_RESTART;
	      break;
	    case IDC_PROXYDOMAIN:
	      GetDlgItemText (hwnDlg, IDC_PROXYDOMAIN, ProxyDomain,
			      sizeof (ProxyDomain) - 1);
	      ProxyStatus |= AMAYA_PROXY_RESTART;
	      break;
	    }
	}
      switch (LOWORD (wParam))
	{
	  /* switch buttons */
	case IDC_NOPROXY:
	  ProxyDomainIsOnlyProxy = FALSE;
	  ProxyStatus |= AMAYA_PROXY_RESTART;
	  break;
	case IDC_ONLYPROXY:
	  ProxyDomainIsOnlyProxy = TRUE;
	  ProxyStatus |= AMAYA_PROXY_RESTART;
	  break;

	  /* action buttons */
	case ID_APPLY:
	  SetProxyConf ();	  
	  libwww_updateNetworkConf (ProxyStatus);
	  /* reset the status flag */
	  ProxyStatus = 0;
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DONE:
	  /* reset the status flag */
	  ProxyStatus = 0;
	  ProxyHwnd = NULL;
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DEFAULTS:
	  /* always signal this as modified */
	  ProxyStatus |= AMAYA_PROXY_RESTART;
	  GetDefaultProxyConf ();
	  WIN_RefreshProxyMenu (hwnDlg);
	  break;
	}
      break;	     
    default: return FALSE;
    }
  return TRUE;
}
#else /* _WINDOWS */
/*----------------------------------------------------------------------
  ProxyCallbackDialog
  callback of the proxy configuration menu
  ----------------------------------------------------------------------*/
static void         ProxyCallbackDialog (int ref, int typedata, char *data)
{
  int                 val;

  if (ref == -1)
    {
      /* removes the proxy conf menu */
      TtaDestroyDialogue (ProxyBase + ProxyMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (int) data;
      switch (ref - ProxyBase)
	{
	case ProxyMenu:
	  switch (val) 
	    {
	    case 0:
	      TtaDestroyDialogue (ref);
	      break;
	    case 1:
	      SetProxyConf ();
	      libwww_updateNetworkConf (ProxyStatus);
	      /* reset the status flag */
	      ProxyStatus = 0;
	      TtaDestroyDialogue (ref);
	      break;
	    case 2:
	      GetDefaultProxyConf ();
	      RefreshProxyMenu ();
	      /* always signal this as modified */
	      ProxyStatus |= AMAYA_PROXY_RESTART;
	      break;
	    default:
	      break;
	    }
	  break;

	case mHttpProxy:
	  ProxyStatus |= AMAYA_PROXY_RESTART;
	  if (data)
	    strcpy (HttpProxy, data);
	  else
	    HttpProxy [0] = EOS;
	  break;

	case mProxyDomain:
	  ProxyStatus |= AMAYA_PROXY_RESTART;
	  if (data)
	    strcpy (ProxyDomain, data);
	  else
	    ProxyDomain [0] = EOS;
	  break;

	case mToggleProxy:
	  ProxyStatus |= AMAYA_PROXY_RESTART;
	  switch (val) 
	    {
	    case 0:
	      ProxyDomainIsOnlyProxy = FALSE;
	      break;
	    case 1:
	      ProxyDomainIsOnlyProxy = TRUE;
	      break;
	    }
	  break;

	default:
	  break;
	}
    }
}
#endif /* !_WINDOWS */

/*----------------------------------------------------------------------
  ProxyConfMenu
  Build and display the Conf Menu dialog box and prepare for input.
  ----------------------------------------------------------------------*/
void         ProxyConfMenu (Document document, View view)
{
#ifndef _WINDOWS
   int              i;

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_DEFAULT_BUTTON));

   TtaNewSheet (ProxyBase + ProxyMenu, 
		TtaGetViewFrame (document, view),
		TtaGetMessage (AMAYA, AM_PROXY_MENU),
		2, s, FALSE, 6, 'L', D_DONE);

   TtaNewTextForm (ProxyBase + mHttpProxy,
		   ProxyBase + ProxyMenu,
		   TtaGetMessage (AMAYA, AM_HTTP_PROXY),
		   20,
		   1,
		   TRUE);
   TtaNewTextForm (ProxyBase + mProxyDomain,
		   ProxyBase + ProxyMenu,
		   TtaGetMessage (AMAYA, AM_PROXY_DOMAIN),
		   20,
		   1,
		   TRUE);
   TtaNewLabel (GeneralBase + mProxyDomainInfo, ProxyBase + ProxyMenu,
		TtaGetMessage (AMAYA, AM_PROXY_DOMAIN_INFO));
   sprintf (s, "T%s%cT%s", 
	     TtaGetMessage (AMAYA, AM_DONT_PROXY_DOMAIN), EOS,
	     TtaGetMessage (AMAYA, AM_ONLY_PROXY_DOMAIN));
   TtaNewSubmenu (ProxyBase + mToggleProxy,
		  ProxyBase + ProxyMenu,
		  0,
		  NULL,
		  2,
		  s,
		  NULL,
		  TRUE);
#endif /* !_WINDOWS */

   /* reset the modified flag */
   ProxyStatus = 0;
   /* load and display the current values */
   GetProxyConf ();
#ifndef _WINDOWS
   RefreshProxyMenu ();
  /* display the menu */
   TtaSetDialoguePosition ();
   TtaShowDialogue (ProxyBase + ProxyMenu, TRUE);
#else
  if (!ProxyHwnd)
    /* only activate the menu if it isn't active already */
     DialogBox (hInstance, MAKEINTRESOURCE (PROXYMENU), NULL, (DLGPROC) WIN_ProxyDlgProc);
  else
     SetFocus (ProxyHwnd);
#endif /* !_WINDOWS */
}


/**********************
** General configuration menu
***********************/
/*----------------------------------------------------------------------
  GetGeneralConf
  Makes a copy of the current registry General values
  ----------------------------------------------------------------------*/
static void GetGeneralConf (void)
{
  char       ptr[MAX_LENGTH];

  TtaGetEnvInt ("DOUBLECLICKDELAY", &DoubleClickDelay);
  TtaGetEnvInt ("ZOOM", &Zoom);
  TtaGetEnvBoolean ("ENABLE_MULTIKEY", &Multikey);
  TtaGetEnvBoolean ("ENABLE_BG_IMAGES", &BgImages);
  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &DoubleClick);
  TtaGetEnvBoolean ("ENABLE_FTP", &EnableFTP);
  GetEnvString ("HOME_PAGE", HomePage);
  GetEnvString ("LANG", DialogueLang);
  GetEnvString ("ACCESSKEY_MOD", ptr);
  if (!strcmp (ptr, "Alt"))
    AccesskeyMod = 0;
  else if (!strcmp (ptr, "Ctrl"))
    AccesskeyMod = 1;
  else
    AccesskeyMod = 2;
  TtaGetEnvInt ("FontMenuSize", &FontMenuSize);
#ifdef _WINDOWS
  GetEnvString ("APP_TMPDIR", AppTmpDir);
  GetEnvString ("APP_HOME", AppHome);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  ValidateGeneralConf
  Validates the entries in the General conf menu. If there's an invalid
  entry, we then use the default value. We need this because
  the Windows interface isn't rich enough to do it (e.g., negative numbers
  in the integer entries)
  ----------------------------------------------------------------------*/
static void ValidateGeneralConf (void)
{
  int         change;
  char        lang[3];
  char       *ptr;
#ifdef _WINDOWS
  char        old_AppTmpDir [MAX_LENGTH];
  int         i;

  /* normalize and validate the zoom factor */
  change = 1;
  if (Zoom > 10)
    Zoom = 10;
  else if (Zoom < -10)
    Zoom = -10;
  else 
    change = 0;
  SetDlgItemInt (GeneralHwnd, IDC_ZOOM, Zoom, TRUE);

  /* 
  **validate the tmp dir
  */
  change = 0;
  change += CleanFirstLastSpace (AppTmpDir);
  change += CleanDirSep (AppTmpDir);
  /* remove the last DIR_SEP, if we have it */
  change += RemoveLastDirSep (AppTmpDir);
  if (AppTmpDir[0] == EOS)
  {
    GetDefEnvString ("APP_TMPDIR", AppTmpDir);
    change = 1;
  }
  /* remove the last DIR_SEP, if we have it, twice to
     protect against user "default values" */
  change += RemoveLastDirSep (AppTmpDir);

  /* try to create the directory. If it doesn't work, then
 	 restore the default value */
  if (!TtaMakeDirectory (AppTmpDir))
    { 
      GetDefEnvString ("APP_TMPDIR", AppTmpDir);
      if (!TtaMakeDirectory (AppTmpDir))
	{
	  sprintf (s, "Error creating directory %s", AppTmpDir);
	  MessageBox (GeneralHwnd, s, "MenuConf:VerifyGeneralConf", MB_OK);
	  exit (1);
	} 
      else
	change++;
    }
  if (change)
    SetDlgItemText (GeneralHwnd, IDC_TMPDIR, AppTmpDir);

  /* if AppTmpDir changed, update the cache dir env variables */
  GetEnvString ("APP_TMPDIR", old_AppTmpDir);
  if (strcasecmp (AppTmpDir, old_AppTmpDir))
    {
      /* the new default cache value is AppTmpDir/libwww-cache */
      sprintf (s, "%s%clibwww-cache", AppTmpDir, DIR_SEP);
      TtaSetDefEnvString ("CACHE_DIR", s, TRUE);

      /* if the cache was in AppTmpDir and AppTmpDir changed, move
	 the cache */
      sprintf (s, "%s%clibwww-cache", old_AppTmpDir, DIR_SEP); 
      ptr = TtaGetEnvString ("CACHE_DIR");
      if (ptr && !strcasecmp (s, ptr))
	{
	  sprintf (s, "%s%clibwww-cache", AppTmpDir, DIR_SEP);		  
	  TtaSetEnvString ("CACHE_DIR", s, TRUE);
	  libwww_updateNetworkConf (AMAYA_CACHE_RESTART);
	}
    }
  
  /*
  **  add here other files that you'd like to copy to the new APP_TMPDIR
  */

  /* create the temporary subdirectories that go inside APP_TMPDIR */
  for (i = 0; i < DocumentTableLength; i++)
    {
      sprintf (s, "%s%c%d", AppTmpDir, DIR_SEP, i);
      TtaMakeDirectory (s);
    }
#endif /* _WINDOWS */

  /* validate the dialogue language */
  change = 0;
  ptr = TtaGetEnvString ("THOTDIR");
  if (strcmp (DialogueLang, "en-US"))
    {
      change++;
      DialogueLang[2] = EOS;
    }
  strncpy (lang, DialogueLang, 2);
  lang[2] = EOS;
  sprintf (s, "%s%cconfig%c%s-amayamsg", ptr, DIR_SEP, DIR_SEP, lang);
  if (!TtaFileExist (s))
  {
    GetDefEnvString ("LANG", DialogueLang);
    change++;
  }
  if (change)
#ifdef _WINDOWS
    SetDlgItemText (GeneralHwnd, IDC_DIALOGUELANG, DialogueLang);
#else
    TtaSetTextForm (GeneralBase + mDialogueLang, DialogueLang);
#endif /* WINDOWS */
}

/*----------------------------------------------------------------------
  RecalibrateZoom
  Moves the Zoom setting on all documents to the specified value
  ----------------------------------------------------------------------*/
static void      RecalibrateZoom ()
{
  int               zoom;
  int               doc, view;

  /* recalibrate the zoom settings in all the active documents and
   active views*/
  for (doc = 1; doc < DocumentTableLength; doc++)
    {
      if (DocumentURLs[doc])
	{
	  /* calculate the new zoom for each open view*/
	  for (view = 1; view < AMAYA_MAX_VIEW_DOC; view++)
	    if (TtaIsViewOpen (doc, view))
	    {
	      zoom = TtaGetZoom (doc, view);
	      TtaSetZoom (doc, view, zoom);
	    }
	}
    }
}

/*----------------------------------------------------------------------
  SetGeneralConf
  Updates the registry General values and calls the General functions
  to take into account the changes
  ----------------------------------------------------------------------*/
static void SetGeneralConf (void)
{
  int oldZoom;

  TtaSetEnvInt ("DOUBLECLICKDELAY", DoubleClickDelay, TRUE);
  TtaGetEnvInt ("ZOOM", &oldZoom);
  if (oldZoom != Zoom)
    {
      TtaSetEnvInt ("ZOOM", Zoom, TRUE);
      TtaSetFontZoom (Zoom);
      /* recalibrate the zoom settings in all the active documents */
      RecalibrateZoom ();
    }
  TtaSetEnvBoolean ("ENABLE_MULTIKEY", Multikey, TRUE);
  TtaSetMultikey (Multikey);
  TtaSetEnvBoolean ("ENABLE_BG_IMAGES", BgImages, TRUE);
  TtaSetEnvBoolean ("ENABLE_DOUBLECLICK", DoubleClick, TRUE);
  /* @@@ */
  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &DoubleClick);
  /* @@@ */
  TtaSetEnvBoolean ("ENABLE_FTP", EnableFTP, TRUE);
  AHTFTPURL_flag_set (EnableFTP);
  TtaSetEnvString ("HOME_PAGE", HomePage, TRUE);
  TtaSetEnvString ("LANG", DialogueLang, TRUE);
  if (AccesskeyMod == 0)
    TtaSetEnvString ("ACCESSKEY_MOD", "Alt", TRUE);
  else if (AccesskeyMod == 1)
    TtaSetEnvString ("ACCESSKEY_MOD", "Ctrl", TRUE);
  else
    TtaSetEnvString ("ACCESSKEY_MOD", "None", TRUE);
  TtaSetEnvInt ("FontMenuSize", FontMenuSize, TRUE);
#ifdef _WINDOWS
  TtaSetEnvString ("APP_TMPDIR", AppTmpDir, TRUE);
  strcpy (TempFileDirectory, AppTmpDir);
  TtaAppendDocumentPath (TempFileDirectory);
#endif /* _WINDOWS */

  TtaSaveAppRegistry ();
}

/*----------------------------------------------------------------------
  GetDefaultGeneralConf
  Loads the default registry General values
  ----------------------------------------------------------------------*/
static void GetDefaultGeneralConf ()
{
  char       ptr[MAX_LENGTH];

  TtaGetDefEnvInt ("DOUBLECLICKDELAY", &DoubleClickDelay);
  TtaGetDefEnvInt ("ZOOM", &Zoom);
  GetDefEnvToggle ("ENABLE_MULTIKEY", &Multikey, 
		       GeneralBase + mToggleGeneral, 0);
  GetDefEnvToggle ("ENABLE_BG_IMAGES", &BgImages,
		       GeneralBase + mToggleGeneral, 1);
  GetDefEnvToggle ("ENABLE_DOUBLECLICK", &DoubleClick,
		       GeneralBase + mToggleGeneral, 2);
  GetDefEnvToggle ("ENABLE_FTP", &EnableFTP,
		       GeneralBase + mToggleGeneral, 3);
  GetDefEnvString ("HOME_PAGE", HomePage);
  GetDefEnvString ("LANG", DialogueLang);
  GetDefEnvString ("ACCESSKEY_MOD", ptr);
  if (!strcmp (ptr, "Alt"))
    AccesskeyMod = 0;
  else if (!strcmp (ptr, "Ctrl"))
    AccesskeyMod = 1;
  else
    AccesskeyMod = 2;
  TtaGetDefEnvInt ("FontMenuSize", &FontMenuSize);
#ifdef _WINDOWS
  GetDefEnvString ("APP_TMPDIR", AppTmpDir);
  GetDefEnvString ("APP_HOME", AppHome);
#endif /* _WINDOWS */
}

#ifdef _WINDOWS
/*----------------------------------------------------------------------
  WIN_RefreshGeneralMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
void WIN_RefreshGeneralMenu (HWND hwnDlg)
{
  SetDlgItemText (hwnDlg, IDC_HOMEPAGE, HomePage);
  CheckDlgButton (hwnDlg, IDC_MULTIKEY, (Multikey) 
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_BGIMAGES, (BgImages) 
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_DOUBLECLICK, (DoubleClick) 
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_ENABLEFTP, (EnableFTP) 
		  ? BST_CHECKED : BST_UNCHECKED);
  SetDlgItemText (hwnDlg, IDC_DIALOGUELANG, DialogueLang);
  switch (AccesskeyMod)
  {
  case 0:
	  CheckRadioButton (hwnDlg, IDC_AALT, IDC_ANONE, IDC_AALT);
	  break;
   case 1:
   	  CheckRadioButton (hwnDlg, IDC_AALT, IDC_ANONE, IDC_ACTRL);
	   break;
   case 2:
	  CheckRadioButton (hwnDlg, IDC_AALT, IDC_ANONE, IDC_ANONE);
       break;
  }
  SetDlgItemInt (hwnDlg, IDC_ZOOM, Zoom, TRUE);
  SetDlgItemText (hwnDlg, IDC_TMPDIR, AppTmpDir);
  SetDlgItemText (hwnDlg, IDC_APPHOME, AppHome);
}
#else /* _WINDOWS */
/*----------------------------------------------------------------------
  RefreshGeneralMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void RefreshGeneralMenu ()
{
  TtaSetNumberForm (GeneralBase + mDoubleClickDelay, DoubleClickDelay);
  TtaSetNumberForm (GeneralBase + mZoom, Zoom);
  TtaSetToggleMenu (GeneralBase + mToggleGeneral, 0, Multikey);
  TtaSetToggleMenu (GeneralBase + mToggleGeneral, 1, BgImages);
  TtaSetToggleMenu (GeneralBase + mToggleGeneral, 2, DoubleClick);
  TtaSetToggleMenu (GeneralBase + mToggleGeneral, 3, EnableFTP);
  TtaSetTextForm (GeneralBase + mHomePage, HomePage);
  TtaSetTextForm (GeneralBase + mDialogueLang, DialogueLang);
  TtaSetMenuForm (GeneralBase + mGeneralAccessKey, AccesskeyMod);
  TtaSetNumberForm (GeneralBase + mFontMenuSize, FontMenuSize);
}
#endif /* !_WINDOWS */

#ifdef _WINDOWS
/*----------------------------------------------------------------------
  WIN_GeneralDlgProc
  Windows callback for the general menu
  ----------------------------------------------------------------------*/
LRESULT CALLBACK WIN_GeneralDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam,
				     LPARAM lParam)
{ 
  switch (msg)
    {
    case WM_INITDIALOG:
      GeneralHwnd = hwnDlg;
      /* initialize the menu text */
      WIN_SetMenuText (hwnDlg, WIN_GeneralMenuText);
      /* write the current values in the dialog entries */
      WIN_RefreshGeneralMenu (hwnDlg);
      break;
   
    case WM_CLOSE:
    case WM_DESTROY:
      /* reset the status flag */
      GeneralHwnd = NULL;
      EndDialog (hwnDlg, ID_DONE);
      break;
   
    case WM_COMMAND:
      if (HIWORD (wParam) == EN_UPDATE)
	{
          switch (LOWORD (wParam))
	    {
	    case IDC_HOMEPAGE:
	      GetDlgItemText (hwnDlg, IDC_HOMEPAGE, HomePage, 
			      sizeof (HomePage) - 1);
	      break;
	    case IDC_APPHOME:
	      GetDlgItemText (hwnDlg, IDC_APPHOME, AppHome,
			      sizeof (AppHome) - 1);
	      break;
	    case IDC_TMPDIR:
	      GetDlgItemText (hwnDlg, IDC_TMPDIR, AppTmpDir,
			      sizeof (AppTmpDir) - 1);
	      break;
	    case IDC_DIALOGUELANG:
	      GetDlgItemText (hwnDlg, IDC_DIALOGUELANG, DialogueLang,
			      sizeof (DialogueLang) - 1);
		  break;
	    case IDC_ZOOM:
	      Zoom = GetDlgItemInt (hwnDlg, IDC_ZOOM, FALSE, TRUE);
	      break;	
	    }
	}
      switch (LOWORD (wParam))
	{
	 case IDC_AALT:
	   AccesskeyMod = 0;
	   break;
  	 case IDC_ACTRL:
	   AccesskeyMod = 1;
	   break;
   	 case IDC_ANONE:
	   AccesskeyMod = 2;
	   break;
	case IDC_MULTIKEY:
	  Multikey = !Multikey;
	  break;
	case IDC_BGIMAGES:
	  BgImages = !BgImages;
	  break;
	case IDC_DOUBLECLICK:
	  DoubleClick = !DoubleClick;
	  break;
	case IDC_ENABLEFTP:
	  EnableFTP = !EnableFTP;
	  break;

	  /* action buttons */
	case ID_APPLY:
	  ValidateGeneralConf ();
	  SetGeneralConf ();	  
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DONE:
	  GeneralHwnd = NULL;
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DEFAULTS:
	  GetDefaultGeneralConf ();
	  WIN_RefreshGeneralMenu (hwnDlg);
	  break;
	}
      break;	     
    default: return FALSE;
    }
  return TRUE;
}
#else /* _WINDOWS */
/*----------------------------------------------------------------------
   callback of the general menu
  ----------------------------------------------------------------------*/
static void GeneralCallbackDialog (int ref, int typedata, char *data)
{
  int                 val;

  if (ref == -1)
    {
      /* removes the network conf menu */
      TtaDestroyDialogue (GeneralBase + GeneralMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (int) data;
      switch (ref - GeneralBase)
	{
	case GeneralMenu:
	  switch (val) 
	    {
	    case 0:
	      TtaDestroyDialogue (ref);
	      break;
	    case 1:
	      ValidateGeneralConf ();
	      SetGeneralConf ();
	      TtaDestroyDialogue (ref);
	      break;
	    case 2:
	      GetDefaultGeneralConf ();
	      RefreshGeneralMenu ();
	      break;
	    default:
	      break;
	    }
	  break;

	case mDoubleClickDelay:
	  DoubleClickDelay = val;
	  break;

	case mZoom:
	  Zoom = val;
	  break;

	case mHomePage:
	  if (data)
	    strcpy (HomePage, data);
	  else
	    HomePage [0] = EOS;
	  break;

	case mToggleGeneral:
	  switch (val) 
	    {
	    case 0:
	      Multikey = !Multikey;
	      break;
	    case 1:
	      BgImages = !BgImages;
	      break;
	    case 2:
	      DoubleClick = !DoubleClick;
	      break;
	    case 3:
	      EnableFTP = !EnableFTP;
	      break;
	    }
	  break;

	case mFontMenuSize:
	  FontMenuSize = val;
	  break;
	  
	case mDialogueLang:
	  if (data)
	    strcpy (DialogueLang, data);
	  else
	    DialogueLang [0] = EOS;
	  break;
	  
	case mGeneralAccessKey:
	  AccesskeyMod = val;
	  break;
	default:
	  break;
	}
    }
}
#endif /* !_WINDOWS */

/*----------------------------------------------------------------------
  GeneralConfMenu
  Build and display the Browsing Editing conf Menu dialog box and prepare 
  for input.
  ----------------------------------------------------------------------*/
void         GeneralConfMenu (Document document, View view)
{
#ifndef _WINDOWS 
   int              i;

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_DEFAULT_BUTTON));

   TtaNewSheet (GeneralBase + GeneralMenu, 
		TtaGetViewFrame (document, view),
		TtaGetMessage (AMAYA, AM_GENERAL_MENU),
		2, s, TRUE, 3, 'L', D_DONE);
   /* first line */
   TtaNewTextForm (GeneralBase + mHomePage,
		   GeneralBase + GeneralMenu,
		   TtaGetMessage (AMAYA, AM_HOME_PAGE),
		   40,
		   1,
		   FALSE);
   TtaNewLabel (GeneralBase + mGeneralEmpty1, GeneralBase + GeneralMenu, " ");
   TtaNewLabel (GeneralBase + mGeneralEmpty2, GeneralBase + GeneralMenu, " ");
   /* second line */
   sprintf (s, "B%s%cB%s%cB%s%cB%s", 
	     TtaGetMessage (AMAYA, AM_ENABLE_MULTIKEY), EOS, 
	     TtaGetMessage (AMAYA, AM_SHOW_BG_IMAGES), EOS, 
	     TtaGetMessage (AMAYA, AM_ENABLE_DOUBLECLICK), EOS,
	     TtaGetMessage (AMAYA, AM_ENABLE_FTP));

   TtaNewToggleMenu (GeneralBase + mToggleGeneral,
		     GeneralBase + GeneralMenu,
		     NULL,
		     4,
		     s,
		     NULL,
		     FALSE);
   TtaNewLabel (GeneralBase + mGeneralEmpty3, GeneralBase + GeneralMenu, " ");
   sprintf (s, "BAlt%cBCtrl%cB%s", EOS, EOS,
	    TtaGetMessage (AMAYA, AM_NONE));   
   TtaNewSubmenu (GeneralBase + mGeneralAccessKey,
		  GeneralBase + GeneralMenu,
		  0,
		  TtaGetMessage (AMAYA, AM_ACCESSKEY),
		  3,
		  s,
		  NULL,
		  FALSE);

   /* third line */
   TtaNewNumberForm (GeneralBase + mFontMenuSize,
		     GeneralBase + GeneralMenu,
		     TtaGetMessage (AMAYA, AM_MENU_FONT_SIZE),
		     8,
		     20,
		     FALSE);   

   TtaNewNumberForm (GeneralBase + mZoom,
		     GeneralBase + GeneralMenu,
		     TtaGetMessage (AMAYA, AM_ZOOM),
		     -10,
		     10,
		     FALSE);   
   TtaNewNumberForm (GeneralBase + mDoubleClickDelay,
		     GeneralBase + GeneralMenu,
		     TtaGetMessage (AMAYA, AM_DOUBLECLICK_DELAY),
		     0,
		     65000,
		     FALSE);   
   /* fourth line */
   TtaNewTextForm (GeneralBase + mDialogueLang,
		   GeneralBase + GeneralMenu,
		   TtaGetMessage (AMAYA, AM_DIALOGUE_LANGUAGE),
		   3,
		   1,
		   FALSE);
#endif /* !_WINDOWS */
   /* load the current values */
   GetGeneralConf ();

#ifndef _WINDOWS
   RefreshGeneralMenu ();
   /* display the menu */
   TtaSetDialoguePosition ();
   TtaShowDialogue (GeneralBase + GeneralMenu, TRUE);
#else /* !_WINDOWS */

   if (!GeneralHwnd)
     /* only activate the menu if it isn't active already */
     DialogBox (hInstance, MAKEINTRESOURCE (GENERALMENU), NULL, (DLGPROC) WIN_GeneralDlgProc);
   else
     SetFocus (GeneralHwnd);
#endif /* !_WINDOWS */
}

/**********************
** Publishing menu
***********************/
/*----------------------------------------------------------------------
  GetPublishConf
  Makes a copy of the current registry Publish values
  ----------------------------------------------------------------------*/
static void GetPublishConf (void)
{
  TtaGetEnvBoolean ("ENABLE_LOST_UPDATE_CHECK", &LostUpdateCheck);
  TtaGetEnvBoolean ("VERIFY_PUBLISH", &VerifyPublish);
  GetEnvString ("DEFAULTNAME", DefaultName);
  GetEnvString ("SAFE_PUT_REDIRECT", SafePutRedirect);
}

/*----------------------------------------------------------------------
  SetPublishConf
  Updates the registry Publish values and calls the Publish functions
  to take into account the changes
  ----------------------------------------------------------------------*/
static void SetPublishConf (void)
{
  TtaSetEnvBoolean ("ENABLE_LOST_UPDATE_CHECK", LostUpdateCheck, TRUE);
  TtaSetEnvBoolean ("VERIFY_PUBLISH", VerifyPublish, TRUE);
  TtaSetEnvString ("DEFAULTNAME", DefaultName, TRUE);
  TtaSetEnvString ("SAFE_PUT_REDIRECT", SafePutRedirect, TRUE);

  TtaSaveAppRegistry ();
}

/*----------------------------------------------------------------------
  GetDefaultPublishConf
  Loads the default registry Publish values
  ----------------------------------------------------------------------*/
static void GetDefaultPublishConf ()
{
  GetDefEnvToggle ("ENABLE_LOST_UPDATE_CHECK", &LostUpdateCheck, 
		    PublishBase + mTogglePublish, 0);
  GetDefEnvToggle ("VERIFY_PUBLISH", &VerifyPublish,
		    PublishBase + mTogglePublish, 1);
  GetDefEnvString ("DEFAULTNAME", DefaultName);
  GetDefEnvString ("SAFE_PUT_REDIRECT", SafePutRedirect);
}

#ifdef _WINDOWS
/*----------------------------------------------------------------------
  WIN_RefreshPublishMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
void WIN_RefreshPublishMenu (HWND hwnDlg)
{
  CheckDlgButton (hwnDlg, IDC_LOSTUPDATECHECK, (LostUpdateCheck)
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_VERIFYPUBLISH, (VerifyPublish)
		  ? BST_CHECKED : BST_UNCHECKED);
  SetDlgItemText (hwnDlg, IDC_DEFAULTNAME, DefaultName);
  SetDlgItemText (hwnDlg, IDC_SAFEPUTREDIRECT, SafePutRedirect);
}
#else /* WINDOWS */
/*----------------------------------------------------------------------
  RefreshPublishMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void RefreshPublishMenu ()
{
  TtaSetToggleMenu (PublishBase + mTogglePublish, 0, LostUpdateCheck);
  TtaSetToggleMenu (PublishBase + mTogglePublish, 1, VerifyPublish);
  TtaSetTextForm (PublishBase + mDefaultName, DefaultName);
  TtaSetTextForm (PublishBase + mSafePutRedirect, SafePutRedirect);
}
#endif /* !_WINDOWS */

#ifdef _WINDOWS
/*----------------------------------------------------------------------
  WIN_PublishDlgProc
  Windows callback for the publish menu
  ----------------------------------------------------------------------*/
LRESULT CALLBACK WIN_PublishDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam,
				     LPARAM lParam)
{ 
  switch (msg)
    {
    case WM_INITDIALOG:
      PublishHwnd = hwnDlg;
      /* initialize the menu text */
      WIN_SetMenuText (hwnDlg, WIN_PublishMenuText);
      /* write the current values in the dialog entries */
      WIN_RefreshPublishMenu (hwnDlg);
      break;

    case WM_CLOSE:
    case WM_DESTROY:
      /* reset the status flag */
      PublishHwnd = NULL;
      EndDialog (hwnDlg, ID_DONE);
      break; 

    case WM_COMMAND:
      if (HIWORD (wParam) == EN_UPDATE)
	{
          switch (LOWORD (wParam))
	    {
	    case IDC_DEFAULTNAME:
	      GetDlgItemText (hwnDlg, IDC_DEFAULTNAME, DefaultName,
			      sizeof (DefaultName) - 1);
	      break;

	    case IDC_SAFEPUTREDIRECT:
	      GetDlgItemText (hwnDlg, IDC_SAFEPUTREDIRECT, SafePutRedirect,
			      sizeof (SafePutRedirect) - 1);
	      SafePutStatus |= AMAYA_SAFEPUT_RESTART;
	      break;
	    }
	}
      switch (LOWORD (wParam))
	{
	case IDC_LOSTUPDATECHECK:
	  LostUpdateCheck = !LostUpdateCheck;
	  break;
	case IDC_VERIFYPUBLISH:
	  VerifyPublish = !VerifyPublish;
	  break;

	  /* action buttons */
	case ID_APPLY:
	  SetPublishConf ();	  
	  libwww_updateNetworkConf (SafePutStatus);
	  /* reset the status flag */
	  SafePutStatus = 0;
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DONE:
	  /* reset the status flag */
	  SafePutStatus = 0;
	  PublishHwnd = NULL;
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DEFAULTS:
	  /* always signal this as modified */
	  SafePutStatus |= AMAYA_SAFEPUT_RESTART;
	  GetDefaultPublishConf ();
	  WIN_RefreshPublishMenu (hwnDlg);
	  break;
	}
      break;	     
    default: return FALSE;
    }
  return TRUE;
}
#else /* _WINDOWS */
/*----------------------------------------------------------------------
   callback of the Publishing menu
  ----------------------------------------------------------------------*/
static void PublishCallbackDialog (int ref, int typedata, char *data)
{
  int                 val;

  if (ref == -1)
    {
      /* removes the network conf menu */
      TtaDestroyDialogue (PublishBase + PublishMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (int) data;
      switch (ref - PublishBase)
	{
	case PublishMenu:
	  switch (val) 
	    {
	    case 0:
	      TtaDestroyDialogue (ref);
	      break;
	    case 1:
	      SetPublishConf ();
	      libwww_updateNetworkConf (SafePutStatus);
	      /* reset the status flag */
	      SafePutStatus = 0;
	      TtaDestroyDialogue (ref);
	      break;
	    case 2:
	      GetDefaultPublishConf ();
	      RefreshPublishMenu ();
	      /* always signal this as modified */
	      SafePutStatus |= AMAYA_SAFEPUT_RESTART;
	      break;
	    default:
	      break;
	    }
	  break;

	case mTogglePublish:
	  switch (val) 
	    {
	    case 0:
	      LostUpdateCheck = !LostUpdateCheck;
	      break;
	    case 1:
	      VerifyPublish = !VerifyPublish;
	      break;
	    }
	  break;

	case mDefaultName:
	  if (data)
	    strcpy (DefaultName, data);
	  else
	    DefaultName[0] = EOS;
	  break;

	case mSafePutRedirect:
	  if (data)
	    strcpy (SafePutRedirect, data);
	  else
	    SafePutRedirect[0] = EOS;
	  SafePutStatus |= AMAYA_SAFEPUT_RESTART;
	  break;

	default:
	  break;
	}
    }
}
#endif /* !_WINDOWS */

/*----------------------------------------------------------------------
  PublishConfMenu
  Build and display the Browsing Editing conf Menu dialog box and prepare 
  for input.
  ----------------------------------------------------------------------*/
void         PublishConfMenu (Document document, View view)
{
#ifndef _WINDOWS
   int              i;

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_DEFAULT_BUTTON));

   TtaNewSheet (PublishBase + PublishMenu, 
		TtaGetViewFrame (document, view),
	       TtaGetMessage (AMAYA, AM_PUBLISH_MENU),
		2, s, FALSE, 11, 'L', D_DONE);
   sprintf (s, "B%s%cB%s", 
	    TtaGetMessage (AMAYA, AM_USE_ETAGS), EOS, 
	    TtaGetMessage (AMAYA, AM_VERIFY_PUT));
   TtaNewToggleMenu (PublishBase + mTogglePublish,
		     PublishBase + PublishMenu,
		     NULL,
		     2,
		     s,
		     NULL,
		     FALSE);
   TtaNewTextForm (PublishBase + mDefaultName,
		   PublishBase + PublishMenu,
		   TtaGetMessage (AMAYA, AM_DEFAULT_NAME),
		   20,
		   1,
		   FALSE);
   TtaNewTextForm (PublishBase + mSafePutRedirect,
		   PublishBase + PublishMenu,
		   TtaGetMessage (AMAYA, AM_SAFE_PUT_REDIRECT),
		   20,
		   1,
		   FALSE);
#endif /* !_WINDOWS */
   /* reset the modified flag */
   SafePutStatus = 0;
   /* load the current values */
   GetPublishConf ();

   /* display the menu */
#ifndef _WINDOWS
   RefreshPublishMenu ();
   TtaSetDialoguePosition ();
   TtaShowDialogue (PublishBase + PublishMenu, TRUE);
#else
  if (!PublishHwnd)
	   DialogBox (hInstance, MAKEINTRESOURCE (PUBLISHMENU), NULL, 
	  (DLGPROC) WIN_PublishDlgProc);
  else
     SetFocus (PublishHwnd);
#endif /* !_WINDOWS */
}


/**********************
** Color Menu
**********************/
/*----------------------------------------------------------------------
  GetColorConf
  Makes a copy of the current registry color values
  ----------------------------------------------------------------------*/
static void GetColorConf (void)
{
  GetEnvString ("ForegroundColor", FgColor);
  GetEnvString ("BackgroundColor", BgColor);
  GetEnvString ("DocSelectColor", SelColor);
  GetEnvString ("InserPointColor", InsColor);
#ifndef _WINDOWS
  GetEnvString ("MenuFgColor", MenuFgColor);
  GetEnvString ("MenuBgColor", MenuBgColor);
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
  GetDefaultColorConf
  Makes a copy of the default registry color values
  ----------------------------------------------------------------------*/
static void GetDefaultColorConf (void)
{
  GetDefEnvString ("ForegroundColor", FgColor);
  GetDefEnvString ("BackgroundColor", BgColor);
  GetDefEnvString ("DocSelectColor", SelColor);
  GetDefEnvString ("InserPointColor", InsColor);
#ifndef _WINDOWS
  GetDefEnvString ("MenuFgColor", MenuFgColor);
  GetDefEnvString ("MenuBgColor", MenuBgColor);
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
  SetColorConf
  Updates the registry Color values
  ----------------------------------------------------------------------*/
static void SetColorConf (void)
{
  TtaSetEnvString ("ForegroundColor", FgColor, TRUE);
  TtaSetEnvString ("BackgroundColor", BgColor, TRUE);
  TtaSetEnvString ("DocSelectColor", SelColor, TRUE);
  TtaSetEnvString ("InserPointColor", InsColor, TRUE);
#ifndef _WINDOWS
  TtaSetEnvString ("MenuFgColor", MenuFgColor, TRUE);
  TtaSetEnvString ("MenuBgColor", MenuBgColor, TRUE);
#endif /* !_WINDOWS */

  TtaSaveAppRegistry ();
  /* change the current settings */
  TtaUpdateEditorColors ();
}

#ifdef _WINDOWS
/*----------------------------------------------------------------------
  WIN_RefreshColorMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
void WIN_RefreshColorMenu (HWND hwnDlg)
{
  SetDlgItemText (hwnDlg, IDC_FGCOLOR, FgColor);
  SetDlgItemText (hwnDlg, IDC_BGCOLOR, BgColor);
  SetDlgItemText (hwnDlg, IDC_SECOLOR, SelColor);
  SetDlgItemText (hwnDlg, IDC_INCOLOR, InsColor);
}
#else /* WINDOWS */
/*----------------------------------------------------------------------
  RefreshColorMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void RefreshColorMenu ()
{
  TtaSetTextForm (ColorBase + mFgColor, FgColor);
  TtaSetTextForm (ColorBase + mBgColor, BgColor);
  TtaSetTextForm (ColorBase + mSeColor, SelColor);
  TtaSetTextForm (ColorBase + mInColor, InsColor);
  TtaSetTextForm (ColorBase + mMenuFgColor, MenuFgColor);
  TtaSetTextForm (ColorBase + mMenuBgColor, MenuBgColor);
}
#endif /* !_WINDOWS */

#ifdef _WINDOWS
/*----------------------------------------------------------------------
  WIN_ColorDlgProc
  Windows callback for the color menu
  ----------------------------------------------------------------------*/
LRESULT CALLBACK WIN_ColorDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam,
				     LPARAM lParam)
{
 int fgcolor, bgcolor;
  switch (msg)
    {
    case WM_INITDIALOG:
      ColorHwnd = hwnDlg;
      /* initialize the menu text */
      WIN_SetMenuText (hwnDlg, WIN_ColorMenuText);
      /* write the current values in the dialog entries */
      WIN_RefreshColorMenu (hwnDlg);
      break;

    case WM_CLOSE:
    case WM_DESTROY:
      /* reset the status flag */
      ColorHwnd = NULL;
      EndDialog (hwnDlg, ID_DONE);
      break;

    case WM_COMMAND:
      if (HIWORD (wParam) == EN_UPDATE)
	{
          switch (LOWORD (wParam))
	    {
	    case IDC_FGCOLOR:
	      GetDlgItemText (hwnDlg, IDC_FGCOLOR, FgColor,
			      sizeof (FgColor) - 1);
	      break;
	    case IDC_BGCOLOR:
	      GetDlgItemText (hwnDlg, IDC_BGCOLOR, BgColor,
			      sizeof (BgColor) - 1);
	      break;
	    case IDC_SECOLOR:
	      GetDlgItemText (hwnDlg, IDC_SECOLOR, SelColor,
			      sizeof (SelColor) - 1);
	      break;
	    case IDC_INCOLOR:
	      GetDlgItemText (hwnDlg, IDC_INCOLOR, InsColor,
			      sizeof (InsColor) - 1);
	      break;
	    }
	}
      switch (LOWORD (wParam))
	{
	  /* action buttons */
	case IDC_CHANGCOLOR:
	  TtcGetPaletteColors (&fgcolor, &bgcolor, TRUE);
	  if (fgcolor != -1)
	    strcpy (FgColor, ColorName (fgcolor));
	  if (bgcolor != -1)
	    strcpy (BgColor, ColorName (bgcolor));
	  WIN_RefreshColorMenu (ColorHwnd);
	  SetFocus (ColorHwnd);
	  break;
	case IDC_CHANGCOLOR2:
	  TtcGetPaletteColors (&fgcolor, &bgcolor, FALSE);
	  if (fgcolor != -1)
	    strcpy (SelColor, ColorName (fgcolor));
	  if (bgcolor != -1)
	    strcpy (InsColor, ColorName (bgcolor));
	  WIN_RefreshColorMenu (ColorHwnd);
	  SetFocus (ColorHwnd);
	  break;
	case ID_APPLY:
	  SetColorConf ();	  
	  /* reset the status flag */
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DONE:
	  /* reset the status flag */
	  ColorHwnd = NULL;
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DEFAULTS:
	  /* always signal this as modified */
	  GetDefaultColorConf ();
	  WIN_RefreshColorMenu (hwnDlg);
	  break;
	}
      break;	     
    default: return FALSE;
    }
  return TRUE;
}
#else /* _WINDOWS */
/*----------------------------------------------------------------------
   callback of the color configuration menu
  ----------------------------------------------------------------------*/
static void ColorCallbackDialog (int ref, int typedata, char *data)
{
  int val;

  if (ref == -1)
    {
      /* removes the color conf menu */
      TtaDestroyDialogue (ColorBase + ColorMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (int) data;
      switch (ref - ColorBase)
	{
	case ColorMenu:
	  switch (val) 
	    {
	    case 0:
	      TtaDestroyDialogue (ref);
	      break;
	    case 1:
	      SetColorConf ();
	      TtaDestroyDialogue (ref);
	      break;
	    case 2:
	      GetDefaultColorConf ();
	      RefreshColorMenu ();
	      break;
	    default:
	      break;
	    }
	  break;
	  
	case mFgColor:
	  if (data)
	    strcpy (FgColor, data);
	  else
	    FgColor [0] = EOS;
	  break;
	case mBgColor:
	  if (data)
	    strcpy (BgColor, data);
	  else
	    BgColor [0] = EOS;
	  break;
	case mSeColor:
	  if (data)
	    strcpy (SelColor, data);
	  else
	    SelColor [0] = EOS;
	  break;
	case mInColor:
	  if (data)
	    strcpy (InsColor, data);
	  else
	    InsColor [0] = EOS;
	  break;
	case mMenuFgColor:
	  if (data)
	    strcpy (MenuFgColor, data);
	  else
	    MenuFgColor [0] = EOS;
	  break;
	case mMenuBgColor:
	  if (data)
	    strcpy (MenuBgColor, data);
	  else
	    MenuBgColor [0] = EOS;
	  break;

	default:
	  break;
	}
    }
}
#endif /* !_WINDOWS */

/*----------------------------------------------------------------------
  ColorConfMenu
  Build and display the Conf Menu dialog box and prepare for input.
  ----------------------------------------------------------------------*/
void         ColorConfMenu (Document document, View view)
{
#ifndef _WINDOWS
   int              i;

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_DEFAULT_BUTTON));

   TtaNewSheet (ColorBase + ColorMenu, 
		TtaGetViewFrame (document, view),
		TtaGetMessage (AMAYA, AM_COLOR_MENU),
		2, s, TRUE, 2, 'L', D_DONE);
   /* first col */
   TtaNewTextForm (ColorBase + mFgColor,
		   ColorBase + ColorMenu,
		   TtaGetMessage (AMAYA, AM_DOC_FG_COLOR),
		   20,
		   1,
		   FALSE);   
   TtaNewTextForm (ColorBase + mBgColor,
		   ColorBase + ColorMenu,
		   TtaGetMessage (AMAYA, AM_DOC_BG_COLOR),
		   20,
		   1,
		   FALSE);   
   /* second col */
   TtaNewTextForm (ColorBase + mInColor,
		   ColorBase + ColorMenu,
		   TtaGetMessage (AMAYA, AM_DOC_INS_COLOR),
		   20,
		   1,
		   FALSE);   
   TtaNewTextForm (ColorBase + mSeColor,
		   ColorBase + ColorMenu,
		   TtaGetMessage (AMAYA, AM_DOC_SEL_COLOR),
		   20,
		   1,
		   FALSE);   
   /* third col */
   TtaNewTextForm (ColorBase + mMenuFgColor,
		   ColorBase + ColorMenu,
		   TtaGetMessage (AMAYA, AM_MENU_FG_COLOR),
		   20,
		   1,
		   FALSE);   
   TtaNewTextForm (ColorBase + mMenuBgColor,
		   ColorBase + ColorMenu,
		   TtaGetMessage (AMAYA, AM_MENU_BG_COLOR),
		   20,
		   1,
		   FALSE);

   TtaNewLabel (ColorBase + mColorEmpty1, ColorBase + ColorMenu,
		TtaGetMessage (AMAYA, AM_GEOMETRY_CHANGE));
     
#endif /* !_WINDOWS */
 
   /* load and display the current values */
   GetColorConf ();
#ifndef _WINDOWS
   RefreshColorMenu ();
   /* display the menu */
   TtaSetDialoguePosition ();
   TtaShowDialogue (ColorBase + ColorMenu, TRUE);
#else 
   if (!ColorHwnd)
    /* only activate the menu if it isn't active already */
	  DialogBox (hInstance, MAKEINTRESOURCE (COLORMENU), NULL, 
		     (DLGPROC) WIN_ColorDlgProc);
   else
     SetFocus (ColorHwnd);
#endif /* !_WINDOWS */
}


/**********************
** Geometry Menu
**********************/
/*----------------------------------------------------------------------
  RestoreDefEnvGeom
  Restores the default integer geometry values that are stored in a 
  registry entry under the form "x y w h"
  ----------------------------------------------------------------------*/
static void RestoreDefEnvGeom (char *env_var, Document doc)
{
  /* in order to read the default values from HTML.conf, we erase the 
     registry entry */
  TtaClearEnvString (env_var);
}

/*----------------------------------------------------------------------
  SetEnvGeom
  Gets the current geometry for a view and saves it in the registry
  using the format "x y w h"
  ----------------------------------------------------------------------*/
static void SetEnvGeom (char *view_name, Document doc)
{
  int view;
  int x, y, w, h;
  

  /* @@ I should be able to remove this line */
  TtaGetViewGeometry (doc, view_name, &x, &y, &w, &h);
  if (!strcmp (view_name, "Source_view"))
    /* takes the current size and position of the main view */
    view = 1;
  else
    view = TtaGetViewFromName (doc, view_name);
  if (view != 0 && TtaIsViewOpen (doc, view))
    {
      /* get current geometry */
      TtaGetViewXYWH (doc, view, &x, &y, &w, &h);
      sprintf (s, "%d %d %d %d", x, y, w, h);
      TtaSetEnvString (view_name, s, TRUE);
    }
}

/*----------------------------------------------------------------------
  RestoreDefaultGeometryConf
  Makes a copy of the default registry geometry values
  ----------------------------------------------------------------------*/
static void RestoreDefaultGeometryConf (void)
{
  int   i;

  RestoreDefEnvGeom ("Formatted_view", GeometryDoc);
  RestoreDefEnvGeom ("Structure_view", GeometryDoc);
  RestoreDefEnvGeom ("Alternate_view", GeometryDoc);
  RestoreDefEnvGeom ("Links_view", GeometryDoc);
  RestoreDefEnvGeom ("Table_of_contents", GeometryDoc);
  i = DocumentSource[GeometryDoc];
  if (i)
    RestoreDefEnvGeom ("Source_view", i);
  /* save the options */
  TtaSaveAppRegistry ();
}

/*----------------------------------------------------------------------
  SetEnvCurrentGeometry stores the current doc geometry in the registry
  ----------------------------------------------------------------------*/
static void SetEnvCurrentGeometry ()
{
  int i;

  /* only do the processing if the document exists */
  if (DocumentURLs[GeometryDoc])
    {
      SetEnvGeom ("Formatted_view", GeometryDoc);
      SetEnvGeom ("Structure_view", GeometryDoc);
      SetEnvGeom ("Alternate_view", GeometryDoc);
      SetEnvGeom ("Links_view", GeometryDoc);
      SetEnvGeom ("Table_of_contents", GeometryDoc);
      i = DocumentSource[GeometryDoc];
      if (i)
	  SetEnvGeom ("Source_view", i);
    } /* if GeometryDoc exists */
}

/*----------------------------------------------------------------------
  SetGeometryConf
  Updates the registry Geometry values and redraws the windows
  ----------------------------------------------------------------------*/
static void SetGeometryConf (void)
{
  /* read the current values and save them into the registry */
  SetEnvCurrentGeometry ();

  /* save the options */
  TtaSaveAppRegistry ();
}

#ifdef _WINDOWS
/*----------------------------------------------------------------------
  WIN_GeometryDlgProc
  Windows callback for the geometry menu
  ----------------------------------------------------------------------*/
LRESULT CALLBACK WIN_GeometryDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam,
				      LPARAM lParam)
{
  switch (msg)
    {
    case WM_INITDIALOG:
      GeometryHwnd = hwnDlg;
      /* initialize the menu text */
      WIN_SetMenuText (hwnDlg, WIN_GeometryMenuText);
      /* write the current values in the dialog entries */
      break;

    case WM_CLOSE:
    case WM_DESTROY:
      /* reset the status flag */
      GeometryDoc = 0;
	  GeometryHwnd = NULL;
      EndDialog (hwnDlg, ID_DONE);
      break;

    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	  /* action buttons */
	case ID_APPLY:
	  SetGeometryConf ();
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DONE:
	  GeometryDoc = 0;
	  GeometryHwnd = NULL;
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DEFAULTS:
	  RestoreDefaultGeometryConf ();
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	}
      break;	     
    default: return FALSE;
    }
  return TRUE;
}
#else /* _WINDOWS */
/*----------------------------------------------------------------------
  GeometryCallbackDialog
  callback of the geometry configuration menu
  ----------------------------------------------------------------------*/
static void GeometryCallbackDialog (int ref, int typedata, char *data)
{
  int val;

  TtaDestroyDialogue (GeometryBase + GeometryMenu);
  if (ref == -1)
    {
      /* removes the geometry conf menu */
      GeometryDoc = 0;
    }
  else
    {
      /* has the user changed the options? */
      val = (int) data;
      switch (ref - GeometryBase)
	{
	case GeometryMenu:
	  switch (val) 
	    {
	    case 0:
	      TtaDestroyDialogue (ref);
	      GeometryDoc = 0;
	      TtaDestroyDialogue (ref);
	      break;
	    case 1:
	      SetGeometryConf ();
	      break;
	    case 2:
	      RestoreDefaultGeometryConf ();
	      break;
	    default:
	      break;
	    }
	  break;
	  
	default:
	  break;
	}
    }
}
#endif /* !_WINDOWS */

/*----------------------------------------------------------------------
  GeometryConfMenu
  Build and display the Conf Menu dialog box and prepare for input.
  ----------------------------------------------------------------------*/
void         GeometryConfMenu (Document document, View view)
{
#ifndef _WINDOWS
  int i;

  if (GeometryDoc)
    {
      /* menu already active, so we'll destroy it in order to
	 have a menu that points to the current document */
      TtaDestroyDialogue (GeometryBase + GeometryMenu);
    }
  GeometryDoc = document;
  /* Create the dialogue form */
  i = 0;
  strcpy (&s[i], TtaGetMessage (AMAYA, AM_SAVE_GEOMETRY));
  i += strlen (&s[i]) + 1;
  strcpy (&s[i], TtaGetMessage (AMAYA, AM_RESTORE_GEOMETRY));
  
  TtaNewSheet (GeometryBase + GeometryMenu, 
	       TtaGetViewFrame (document, view),
	       TtaGetMessage (AMAYA, AM_GEOMETRY_MENU),
	       2, s, TRUE, 2, 'L', D_DONE);
  TtaNewLabel (GeometryBase + mGeometryLabel1,
	       GeometryBase + GeometryMenu,
	       TtaGetMessage (AMAYA, AM_GEOMETRY_CHANGE)
	       );
  TtaNewLabel (GeometryBase + mGeometryLabel2,
	       GeometryBase + GeometryMenu,
	       " "
	       );
  /* display the menu */
  TtaSetDialoguePosition ();
  TtaShowDialogue (GeometryBase + GeometryMenu, TRUE);
#else /* !_WINDOWS */
  if (GeometryHwnd)
	 /* menu already active. We'll destroy it in order to have
	  a menu that points to the current document */
	 EndDialog (GeometryHwnd, ID_DONE);
  GeometryDoc = document;
      DialogBox (hInstance, MAKEINTRESOURCE (GEOMETRYMENU), NULL,
		 (DLGPROC) WIN_GeometryDlgProc);
#endif /* !_WINDOWS */
}


/**********************
** LanNeg Menu
**********************/
/*----------------------------------------------------------------------
  GetLanNegConf
  Makes a copy of the current registry LanNeg values
  ----------------------------------------------------------------------*/
static void GetLanNegConf (void)
{
  GetEnvString ("ACCEPT_LANGUAGES", LanNeg);
}

/*----------------------------------------------------------------------
  GetDefaultLanNegConf
  Makes a copy of the default registry LanNeg values
  ----------------------------------------------------------------------*/
static void GetDefaultLanNegConf (void)
{
  GetDefEnvString ("ACCEPT_LANGUAGES", LanNeg);
}

/*----------------------------------------------------------------------
  SetLanNegConf
  Updates the registry LanNeg values
  ----------------------------------------------------------------------*/
static void SetLanNegConf (void)
{
  TtaSetEnvString ("ACCEPT_LANGUAGES", LanNeg, TRUE);
  TtaSaveAppRegistry ();

  /* change the current settings */
  libwww_updateNetworkConf (AMAYA_LANNEG_RESTART);
}

#ifdef _WINDOWS
/*----------------------------------------------------------------------
  WIN_RefreshLanNegMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
void WIN_RefreshLanNegMenu (HWND hwnDlg)
{
  SetDlgItemText (hwnDlg, IDC_LANNEG, LanNeg);
}

/*----------------------------------------------------------------------
  WIN_LanNegDlgProc
  Windows callback for the LanNeg menu
  ----------------------------------------------------------------------*/
LRESULT CALLBACK WIN_LanNegDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam,
				     LPARAM lParam)
{
  switch (msg)
    {
    case WM_INITDIALOG:
      LanNegHwnd = hwnDlg;
      /* initialize the menu text */
      WIN_SetMenuText (hwnDlg, WIN_LanNegMenuText);
      /* write the current values in the dialog entries */
      WIN_RefreshLanNegMenu (hwnDlg);
      break;

    case WM_CLOSE:
    case WM_DESTROY:
      /* reset the status flag */
      LanNegHwnd = NULL;
      EndDialog (hwnDlg, ID_DONE);
      break;

    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case IDC_LANNEG:
	  GetDlgItemText (hwnDlg, IDC_LANNEG, LanNeg,
			  sizeof (LanNeg) - 1);
	  break;
	  /* action buttons */
	case ID_APPLY:
	  SetLanNegConf ();	  
	  /* reset the status flag */
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DONE:
	  /* reset the status flag */
	  LanNegHwnd = NULL;
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DEFAULTS:
	  /* always signal this as modified */
	  GetDefaultLanNegConf ();
	  WIN_RefreshLanNegMenu (hwnDlg);
	  break;
	}
      break;	     
    default: return FALSE;
    }
  return TRUE;
}

#else /* _WINDOWS */
/*----------------------------------------------------------------------
  RefreshLanNegMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void RefreshLanNegMenu ()
{
  TtaSetTextForm (LanNegBase + mLanNeg, LanNeg);
}

/*----------------------------------------------------------------------
   callback of the LanNeg configuration menu
  ----------------------------------------------------------------------*/
static void LanNegCallbackDialog (int ref, int typedata, char *data)
{
  int val;

  if (ref == -1)
    {
      /* removes the LanNeg conf menu */
      TtaDestroyDialogue (LanNegBase + LanNegMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (int) data;
      switch (ref - LanNegBase)
	{
	case LanNegMenu:
	  switch (val) 
	    {
	    case 0:
	      TtaDestroyDialogue (ref);
	      break;
	    case 1:
	      SetLanNegConf ();
	      TtaDestroyDialogue (ref);
	      break;
	    case 2:
	      GetDefaultLanNegConf ();
	      RefreshLanNegMenu ();
	      break;
	    default:
	      break;
	    }
	  break;

	case mLanNeg:
	  if (data)
	    strcpy (LanNeg, data);
	  else
	    LanNeg [0] = EOS;
	  break;
	  
	default:
	  break;
	}
    }
}
#endif /* !_WINDOWS */


/*----------------------------------------------------------------------
  LanNegConfMenu
  Build and display the Conf Menu dialog box and prepare for input.
  ----------------------------------------------------------------------*/
void         LanNegConfMenu (Document document, View view)
{
#ifndef _WINDOWS
   int              i;

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_DEFAULT_BUTTON));

   TtaNewSheet (LanNegBase + LanNegMenu, TtaGetViewFrame (document, view),
		TtaGetMessage (AMAYA, AM_LANNEG_MENU), 2, s, TRUE, 1, 
		'L', D_DONE);
   /* first line */
   TtaNewTextForm (LanNegBase + mLanNeg, LanNegBase + LanNegMenu,
		   TtaGetMessage (AMAYA, AM_LANG_NEGOTIATION),
		   20, 1, FALSE);
#endif   /* !_WINDOWS */
 
   /* load and display the current values */
   GetLanNegConf ();
#ifndef _WINDOWS
   RefreshLanNegMenu ();
   /* display the menu */
   TtaSetDialoguePosition ();
   TtaShowDialogue (LanNegBase + LanNegMenu, TRUE);
#else 
   if (!LanNegHwnd)
    /* only activate the menu if it isn't active already */
	  DialogBox (hInstance, MAKEINTRESOURCE (LANNEGMENU), NULL, 
		     (DLGPROC) WIN_LanNegDlgProc);
   else
     SetFocus (LanNegHwnd);
#endif /* !_WINDOWS */
}


/**********************
** Profile Menu
**********************/
/*----------------------------------------------------------------------
  GetProfileConf
  Makes a copy of the current registry Profile values
  ----------------------------------------------------------------------*/
static void GetProfileConf (void)
{
  TtaGetProfileFileName (Profiles_File, MAX_LENGTH);
  GetEnvString ("Profile", Profile);
}

/*----------------------------------------------------------------------
  GetDefaultProfileConf
  Makes a copy of the default registry Profile values
  ----------------------------------------------------------------------*/
static void GetDefaultProfileConf (void)
{
  TtaGetDefProfileFileName (Profiles_File, MAX_LENGTH);
  GetDefEnvString ("Profile", Profile);
}


/*----------------------------------------------------------------------
  SetProfileConf
  Updates the registry Profile values
  ----------------------------------------------------------------------*/
static void SetProfileConf (void)
{
  TtaSetEnvString ("Profiles_File", Profiles_File, TRUE);
  TtaSetEnvString ("Profile", Profile, TRUE);
  TtaSaveAppRegistry ();
}

#ifdef _WINDOWS
/*---------------------------------------------------------------
  BuildProfileList builds the list allowing to select a profile
  (for windows)
------------------------------------------------------------------*/
static void BuildProfileList (void)
{
  char         *ptr;
  int           nbprofiles = 0;
  int           i = 0;

  /* Get the propositions of the list */ 
  SendMessage (wndProfilesList, LB_RESETCONTENT, 0, 0);
  nbprofiles = TtaGetProfilesItems (MenuText, MAX_PRO);
  ptr = TtaGetEnvString ("Profile");
  /* normally ptr = ISO2WideChar (TtaGetEnvString ("Profile")); */
  while (i < nbprofiles && MenuText[i] != '\0')
    {
      /* keep in mind the current selected entry */
      if (ptr && !strcmp (ptr, MenuText[i]))
         CurrentProfile = i;
      SendMessage (wndProfilesList, LB_INSERTSTRING, i, (LPARAM) MenuText[i]);
      i++;
    }
}

/*----------------------------------------------------------------------
  WIN_RefreshProfileMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
void WIN_RefreshProfileMenu (HWND hwnDlg)
{		
  SetDlgItemText (hwnDlg, IDC_PROFILESLOCATION, Profiles_File);
  SetDlgItemText (hwnDlg, IDC_PROFILENAME, Profile);
  SendMessage (wndProfilesList, LB_RESETCONTENT, 0, 0);
}

/*----------------------------------------------------------------------
  WIN_ProfileDlgProc
  Windows callback for the Profile menu
  ----------------------------------------------------------------------*/
LRESULT CALLBACK WIN_ProfileDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam,
				     LPARAM lParam)
{

   int  itemIndex = 0;
   	

   switch (msg)
    {
    case WM_INITDIALOG:
      ProfileHwnd = hwnDlg;
	       wndProfilesList = CreateWindow ("listbox", NULL, 
				  WS_CHILD | WS_VISIBLE | LBS_STANDARD,
				  10, 90, 200, 90, hwnDlg, (HMENU) 1, 
				  (HINSTANCE) GetWindowLong (hwnDlg, 
				  GWL_HINSTANCE), NULL);
      WIN_SetMenuText (hwnDlg, WIN_ProfileMenuText);
      SetDlgItemText (hwnDlg, IDC_PROFILESLOCATION, Profiles_File);
      SetDlgItemText (hwnDlg, IDC_PROFILENAME, Profile);
      break;

    case WM_CLOSE:
    case WM_DESTROY:

		/* reset the status flag */
      ProfileHwnd = NULL;
      EndDialog (hwnDlg, ID_DONE);
      break;

    case WM_COMMAND:
      switch (LOWORD (wParam)) 
	  {
	    case IDC_PROFILESLOCATION:
	      GetDlgItemText (hwnDlg, IDC_PROFILESLOCATION, Profiles_File,
			      sizeof (Profiles_File) - 1);
	      /* if the text entry changed */
	      if (HIWORD(wParam) == EN_UPDATE)
		{	
		  if (strlen(Profiles_File))
		    {
		      TtaRebuildProTable(Profiles_File); 
		      BuildProfileList();
		    }
		}
		break;		
		/* action buttons */
	    case ID_APPLY:
	          SetProfileConf ();	  
	          /* reset the status flag */
	          EndDialog (hwnDlg, ID_DONE);
	          break;
	    case ID_DONE:
	          /* reset the status flag */
	          ProfileHwnd = NULL;
	          EndDialog (hwnDlg, ID_DONE);
	          break;
	    case ID_DEFAULTS:
	          /* always signal this as modified */
	          GetDefaultProfileConf ();
			  WIN_RefreshProfileMenu (ProfileHwnd);
	          break;
	  }
	  switch (HIWORD (wParam))
	  {
		case LBN_SELCHANGE:
			  	itemIndex = SendMessage (wndProfilesList, LB_GETCURSEL, 0, 0);
	            itemIndex = SendMessage (wndProfilesList, LB_GETTEXT, itemIndex, (LPARAM) Profile);
				SetDlgItemText (hwnDlg, IDC_PROFILENAME, Profile);
		        break;		 
	  }
      break;
	     
    default: return FALSE;
   }	     

  return TRUE; 
}

#else /* _WINDOWS */
/*---------------------------------------------------------------
  BuildProfileSelector builds the list allowing to select a profile
  (for unix)
------------------------------------------------------------------*/
static void BuildProfileSelector ()
{
  int                  i;
  int                  nbprofiles = 0;
  int                  indx, length;
  char *               ptr;
  char *               entry;
  char                 BufMenu[MAX_LENGTH];

  /* Get the propositions of the selector */ 
   nbprofiles = TtaGetProfilesItems (MenuText, MAX_PRO);
  ptr = TtaGetEnvString ("Profile");
  /* normally ptr = ISO2WideChar (TtaGetEnvString ("Profile")); */
    
   /* recopy the propositions  */
   indx = 0;
   for (i = 0; i < nbprofiles; i++)
     {
       entry =  MenuText[i];
      /* keep in mind the current selected entry */
      if (ptr && !strcmp (ptr, entry))
	CurrentProfile = i;
       length = strlen (entry) + 1;
       if (length + indx < MAX_PRO * MAX_PRO_LENGTH)  
	 {
	   strcpy (&BufMenu[indx], entry);
	   indx += length;
	 }
     }

   /* no entry */
   entry = NULL;
   /* Fill in the profile form  */
   TtaNewSelector (ProfileBase + mProfileSelector, ProfileBase + ProfileMenu,
		   NULL, nbprofiles,
		   ((i < 2) ? "" : BufMenu), 4, entry, TRUE, FALSE);
   
/* preselect the profile matching the user current profile in use if present  */

   if (nbprofiles)
     TtaSetSelector (ProfileBase + mProfileSelector, CurrentProfile, NULL);
   else
     TtaSetSelector (ProfileBase + mProfileSelector, -1, "");
}

/*----------------------------------------------------------------------
  RefreshProfileMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void RefreshProfileMenu ()
{
  TtaSetTextForm (ProfileBase + mProfiles_File, Profiles_File);
  TtaSetSelector (ProfileBase + mProfileSelector, CurrentProfile, NULL);

}

/*----------------------------------------------------------------------
   callback of the Profile configuration menu
  ----------------------------------------------------------------------*/
static void ProfileCallbackDialog (int ref, int typedata, char *data)
{
  int val;
 
  if (ref == -1)
    {
      /* removes the Profile conf menu */
      TtaDestroyDialogue (ProfileBase + ProfileMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (int) data;
      switch (ref - ProfileBase)
	{
	case ProfileMenu:
	  switch (val) 
	    {
	    case 0:
	      TtaDestroyDialogue (ref);
	      break;
	    case 1:
	      SetProfileConf ();
	      TtaDestroyDialogue (ref);
	      break;
	    case 2:
	      GetDefaultProfileConf ();
	      RefreshProfileMenu ();
	      break;
	    default:
	      break;
	    }
	  break;

	case mProfileSelector:
	  /* Get the desired profile from the item number */
	  if (data)
	    {
	      strcpy (Profile, data);
	      RefreshProfileMenu();
	    }
	  else
	    Profile[0] = EOS;

	  break;

	case mProfiles_File:
	  if (data)
	    { 
	      /* did the profile file change ? */
	      if (strcmp (data, Profiles_File) !=0 ) 
		{
		   /* Yes, the profile file changed  : rescan the
		      profile definition file and display the new
		      profiles in the selector */
		  strcpy (Profiles_File, data);
		  TtaRebuildProTable (Profiles_File);
		  BuildProfileSelector ();
		  RefreshProfileMenu();
		}
	    }
	  else
	    Profiles_File [0] = EOS;
 
	  break;

	default:
	  break;
	}
    }
}
#endif /* !_WINDOWS */


/*----------------------------------------------------------------------
  ProfileConfMenu
  Build and display the Conf Menu dialog box and prepare for input.
  ----------------------------------------------------------------------*/
void         ProfileConfMenu (Document document, View view)
{
#ifndef _WINDOWS
   int                   i;
 
   /* load and display the current values */
   GetProfileConf ();

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_DEFAULT_BUTTON));
   TtaNewSheet (ProfileBase + ProfileMenu, TtaGetViewFrame (document, view),
		TtaGetMessage(AMAYA, AM_PROFILE_MENU), 2, s, TRUE, 1, 'L', D_DONE);

   TtaNewTextForm (ProfileBase + mProfiles_File, ProfileBase + ProfileMenu,
		   TtaGetMessage (AMAYA, AM_PROFILES_FILE),
		   40, 1, FALSE);

   TtaNewLabel (ProfileBase + mProfileEmpty1, ProfileBase + ProfileMenu,
		TtaGetMessage (AMAYA, AM_PROFILE_SELECT));     
   BuildProfileSelector();
  
   /* message "changes will take effect after Amaya restarts" */
   TtaNewLabel (ProfileBase + mProfileEmpty2, ProfileBase + ProfileMenu,
		TtaGetMessage (AMAYA, AM_PROFILE_CHANGE));
   RefreshProfileMenu ();
   /* display the menu */
   TtaSetDialoguePosition ();
   TtaShowDialogue (ProfileBase + ProfileMenu, TRUE);
#else /* !_WINDOWS */
 
   /* load and display the current values */
   GetProfileConf ();

   if (!ProfileHwnd)
	   DialogBox (hInstance, MAKEINTRESOURCE (PROFILEMENU), NULL, 
		     (DLGPROC) WIN_ProfileDlgProc);
   else
     SetFocus (ProfileHwnd);
#endif /* !_WINDOWS */
}


/**********************
** Templates Menu
**********************/
/*----------------------------------------------------------------------
  GetTemplatesConf
  Makes a copy of the current registry Templates values
  ----------------------------------------------------------------------*/
static void GetTemplatesConf (void)
{
  GetEnvString ("TEMPLATE_URL", TemplatesUrl);
}

/*----------------------------------------------------------------------
  GetDefaultTemplatesConf
  Makes a copy of the default registry Templates values
  ----------------------------------------------------------------------*/
static void GetDefaultTemplatesConf (void)
{
  GetDefEnvString ("TEMPLATE_URL", TemplatesUrl);
}


/*----------------------------------------------------------------------
  SetTemplatesConf
  Updates the registry Templates values
  ----------------------------------------------------------------------*/
static void SetTemplatesConf (void)
{
  TtaSetEnvString ("TEMPLATE_URL", TemplatesUrl, TRUE);
  TtaSaveAppRegistry ();
}

#ifdef _WINDOWS
/*----------------------------------------------------------------------
  WIN_RefreshTemplatesMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
void WIN_RefreshTemplatesMenu (HWND hwnDlg)
{
  SetDlgItemText (hwnDlg, IDC_TEMPLATESURL, TemplatesUrl);
}

/*----------------------------------------------------------------------
  WIN_TemplatesDlgProc
  Windows callback for the Templates menu
  ----------------------------------------------------------------------*/
LRESULT CALLBACK WIN_TemplatesDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam,
				       LPARAM lParam)
{
  switch (msg)
    {
    case WM_INITDIALOG:
      TemplatesHwnd = hwnDlg;
      /* initialize the menu text */
      WIN_SetMenuText (hwnDlg, WIN_TemplatesMenuText);
      /* write the current values in the dialog entries */
      WIN_RefreshTemplatesMenu (hwnDlg);
      break;

    case WM_CLOSE:
    case WM_DESTROY:
      /* reset the status flag */
      TemplatesHwnd = NULL;
      EndDialog (hwnDlg, ID_DONE);
      break;

    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case IDC_TEMPLATESURL:
	  GetDlgItemText (hwnDlg, IDC_TEMPLATESURL, TemplatesUrl,
			  sizeof (TemplatesUrl) - 1);
	  break;
	  /* action buttons */
	case ID_APPLY:
	  SetTemplatesConf ();	  
	  /* reset the status flag */
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DONE:
	  /* reset the status flag */
	  TemplatesHwnd = NULL;
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DEFAULTS:
	  /* always signal this as modified */
	  GetDefaultTemplatesConf ();
	  WIN_RefreshTemplatesMenu (hwnDlg);
	  break;
	}
      break;	     
    default: return FALSE;
    }
  return TRUE; 
}

#else /* _WINDOWS */
/*----------------------------------------------------------------------
  RefreshTemplatesMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void RefreshTemplatesMenu ()
{
  TtaSetTextForm (TemplatesBase + mTemplates, TemplatesUrl);
}

/*----------------------------------------------------------------------
   callback of the Templates configuration menu
  ----------------------------------------------------------------------*/
static void TemplatesCallbackDialog (int ref, int typedata, char *data)
{
  int val;

  if (ref == -1)
    {
      /* removes the Templates conf menu */
      TtaDestroyDialogue (TemplatesBase + TemplatesMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (int) data;
      switch (ref - TemplatesBase)
	{
	case TemplatesMenu:
	  switch (val) 
	    {
	    case 0:
	      TtaDestroyDialogue (ref);
	      break;
	    case 1:
	      SetTemplatesConf ();
	      TtaDestroyDialogue (ref);
	      break;
	    case 2:
	      GetDefaultTemplatesConf ();
	      RefreshTemplatesMenu ();
	      break;
	    default:
	      break;
	    }
	  break;

	case mTemplates:
	  if (data)
	    strcpy (TemplatesUrl, data);
	  else
	    TemplatesUrl [0] = EOS;
	  break;
	  
	default:
	  break;
	}
    }
}
#endif /* !_WINDOWS */


/*----------------------------------------------------------------------
  TemplatesConfMenu
  Build and display the Conf Menu dialog box and prepare for input.
  ----------------------------------------------------------------------*/
void TemplatesConfMenu (Document document, View view)
{
#ifndef _WINDOWS
   int              i;

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_DEFAULT_BUTTON));

   TtaNewSheet (TemplatesBase + TemplatesMenu, TtaGetViewFrame (document, view),
		TtaGetMessage(AMAYA, AM_TEMPLATES_MENU), 2, s, TRUE, 1, 'L', D_DONE);
   
   TtaNewTextForm (TemplatesBase + mTemplates, TemplatesBase + TemplatesMenu,
		   TtaGetMessage (AMAYA, AM_TEMPLATES_SERVER),
		   20, 1, FALSE);
  
#endif   /* !_WINDOWS */
 
   /* load and display the current values */
   GetTemplatesConf ();
#ifndef _WINDOWS
   RefreshTemplatesMenu ();
   /* display the menu */
   TtaSetDialoguePosition ();
   TtaShowDialogue (TemplatesBase + TemplatesMenu, TRUE);
#else 
   if (!TemplatesHwnd)
		DialogBox (hInstance, MAKEINTRESOURCE (TEMPLATESMENU), NULL, 
		     (DLGPROC) WIN_TemplatesDlgProc);
   else
     SetFocus (TemplatesHwnd);
#endif /* !_WINDOWS */
}

/*********************
** Annotations configuration menu
***********************/

#ifdef ANNOTATIONS
/*----------------------------------------------------------------------
  GetAnnotConf
  Makes a copy of the current registry annotation values
  ----------------------------------------------------------------------*/
static void GetAnnotConf (void)
{
  GetEnvString ("ANNOT_USER", AnnotUser);
  GetEnvString ("ANNOT_POST_SERVER", AnnotPostServer);
  GetEnvString ("ANNOT_SERVERS", AnnotServers);
  TtaGetEnvBoolean ("ANNOT_LAUTOLOAD", &AnnotLAutoLoad);
  TtaGetEnvBoolean ("ANNOT_RAUTOLOAD", &AnnotRAutoLoad);
  TtaGetEnvBoolean ("ANNOT_RAUTOLOAD_RST", &AnnotRAutoLoadRst);

#ifdef _WINDOWS
  /* we substitute spaces into \r for the configuration widget menu */
  ConvertSpaceNL (AnnotServers, TRUE);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  SetAnnotConf
  Updates the registry annotation values
  ----------------------------------------------------------------------*/
static void SetAnnotConf (void)
{
#ifdef _WINDOWS
	/* we remove the \n added for the configuration menu widget */
  ConvertSpaceNL (AnnotServers, FALSE);
#endif /* _WINDOWS */

  TtaSetEnvString ("ANNOT_USER", AnnotUser, TRUE);
  TtaSetEnvString ("ANNOT_POST_SERVER", AnnotPostServer, TRUE);
  TtaSetEnvString ("ANNOT_SERVERS", AnnotServers, TRUE);
  TtaSetEnvBoolean ("ANNOT_LAUTOLOAD", AnnotLAutoLoad, TRUE);
  TtaSetEnvBoolean ("ANNOT_RAUTOLOAD", AnnotRAutoLoad, TRUE);
  TtaSetEnvBoolean ("ANNOT_RAUTOLOAD_RST", AnnotRAutoLoadRst, TRUE);

  TtaSaveAppRegistry ();

  /* and restart the annotlib */
  /* @@ we could make this finer granularity, and more elegant  */
  StopAllRequests (1);
  ANNOT_FreeConf ();
  ANNOT_Init ();
}

/*----------------------------------------------------------------------
  GetDefaultAnnotConf
  Gets the registry default annotation values.
  ----------------------------------------------------------------------*/
static void GetDefaultAnnotConf ()
{
  /* read the default values */
  GetDefEnvString ("ANNOT_USER", AnnotUser);
  GetDefEnvString ("ANNOT_POST_SERVER", AnnotPostServer);
  GetDefEnvString ("ANNOT_SERVERS", AnnotServers);
  TtaGetDefEnvBoolean ("ANNOT_LAUTOLOAD", &AnnotLAutoLoad);
  TtaGetDefEnvBoolean ("ANNOT_RAUTOLOAD", &AnnotRAutoLoad);
  TtaGetDefEnvBoolean ("ANNOT_RAUTOLOAD_RST", &AnnotRAutoLoadRst);
#ifdef _WINDOWS
  /* we substitute spaces into \n for the configuration widget menu */
  ConvertSpaceNL (AnnotServers, TRUE);
#endif /* _WINDOWS */
}

#ifdef _WINDOWS
/*----------------------------------------------------------------------
  WIN_RefreshAnnotMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void WIN_RefreshAnnotMenu (HWND hwnDlg)
{
  SetDlgItemText (hwnDlg, IDC_ANNOTUSER, AnnotUser);
  SetDlgItemText (hwnDlg, IDC_ANNOTPOSTSERVER, AnnotPostServer);
  SetDlgItemText (hwnDlg, IDC_ANNOTSERVERS, AnnotServers);
  CheckDlgButton (hwnDlg, IDC_ANNOTLAUTOLOAD, (AnnotLAutoLoad) 
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_ANNOTRAUTOLOAD, (AnnotRAutoLoad) 
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_ANNOTRAUTOLOADRST, (AnnotRAutoLoadRst) 
		  ? BST_CHECKED : BST_UNCHECKED);
}
#else /* WINDOWS */
/*----------------------------------------------------------------------
  RefreshAnnotMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void RefreshAnnotMenu ()
{
  /* set the menu entries to the current values */
  TtaSetTextForm (AnnotBase + mAnnotUser, AnnotUser);
  TtaSetTextForm (AnnotBase + mAnnotPostServer, AnnotPostServer);
  TtaSetTextForm (AnnotBase + mAnnotServers, AnnotServers);
  TtaSetToggleMenu (AnnotBase + mToggleAnnot, 0, AnnotLAutoLoad);
  TtaSetToggleMenu (AnnotBase + mToggleAnnot, 1, AnnotRAutoLoad);
  TtaSetToggleMenu (AnnotBase + mToggleAnnot, 2, AnnotRAutoLoadRst);
}
#endif /* !_WINDOWS */

#ifdef _WINDOWS
/*----------------------------------------------------------------------
  WIN_AnnotDlgProc
  Windows callback for the annot menu
  ----------------------------------------------------------------------*/
LRESULT CALLBACK WIN_AnnotDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam,
				     LPARAM lParam)
{
  switch (msg)
    {
    case WM_INITDIALOG:
      AnnotHwnd = hwnDlg;
      /* initialize the menu text */
      WIN_SetMenuText (hwnDlg, WIN_AnnotMenuText);
      /* write the current values in the dialog entries */
      WIN_RefreshAnnotMenu (hwnDlg);
      break;
      
    case WM_CLOSE:
    case WM_DESTROY:
      /* reset the status flag */
      AnnotHwnd = NULL;
      EndDialog (hwnDlg, ID_DONE);
      break;

    case WM_COMMAND:
      if (HIWORD (wParam) == EN_UPDATE)
	{
          switch (LOWORD (wParam))
	    {
	    case IDC_ANNOTUSER:
	      GetDlgItemText (hwnDlg, IDC_ANNOTUSER, AnnotUser,
			      sizeof (AnnotUser) - 1);
	      break;
	    case IDC_ANNOTPOSTSERVER:
	      GetDlgItemText (hwnDlg, IDC_ANNOTPOSTSERVER, AnnotPostServer,
			      sizeof (AnnotPostServer) - 1);
	      break;
	    case IDC_ANNOTSERVERS:
	      GetDlgItemText (hwnDlg, IDC_ANNOTSERVERS, AnnotServers,
			      sizeof (AnnotServers) - 1);
	      break;
	    }
	}
      switch (LOWORD (wParam))
	{
	  /* toggle buttons */
	case IDC_ANNOTLAUTOLOAD:
	  AnnotLAutoLoad = !AnnotLAutoLoad;
	  break;
	case IDC_ANNOTRAUTOLOAD:
	  AnnotRAutoLoad = !AnnotRAutoLoad;
	  break;
	case IDC_ANNOTRAUTOLOADRST:
	  AnnotRAutoLoadRst = !AnnotRAutoLoadRst;
	  break;

	  /* action buttons */
	case ID_APPLY:
	  SetAnnotConf ();	  
	  /* reset the status flag */
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DONE:
	  /* reset the status flag */
	  AnnotHwnd = NULL;
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DEFAULTS:
	  GetDefaultAnnotConf ();
	  WIN_RefreshAnnotMenu (hwnDlg);
	  break;
	}
      break;	     
    default: return FALSE;
    }
  return TRUE;
}
#else /* _WINDOWS */
/*----------------------------------------------------------------------
  AnnotCallbackDialog
  callback of the annotation configuration menu
  ----------------------------------------------------------------------*/
static void AnnotCallbackDialog (int ref, int typedata, char *data)
{
  int                 val;

  if (ref == -1)
    {
      /* removes the annot conf menu */
      TtaDestroyDialogue (AnnotBase + AnnotMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (int) data;
      switch (ref - AnnotBase)
	{
	case AnnotMenu:
	  switch (val) 
	    {
	    case 0:
	      TtaDestroyDialogue (ref);
	      break;
	    case 1:
	      SetAnnotConf ();
	      TtaDestroyDialogue (ref);
	      break;
	    case 2:
	      GetDefaultAnnotConf ();
	      RefreshAnnotMenu ();
	      break;
	    default:
	      break;
	    }
	  break;

	case mAnnotUser:
	  if (data)
	    strcpy (AnnotUser, data);
	  else
	    AnnotUser [0] = EOS;
	  break;

	case mAnnotPostServer:
	  if (data)
	    strcpy (AnnotPostServer, data);
	  else
	    AnnotPostServer [0] = EOS;
	  break;

	case mAnnotServers:
	  if (data)
	    strcpy (AnnotServers, data);
	  else
	    AnnotServers [0] = EOS;
	  break;

	case mToggleAnnot:
	  switch (val) 
	    {
	    case 0:
	      AnnotLAutoLoad = !AnnotLAutoLoad;
	      break;
	    case 1:
	      AnnotRAutoLoad = !AnnotRAutoLoad;
	      break;
	    case 2:
	      AnnotRAutoLoadRst = !AnnotRAutoLoadRst;
	      break;
	    }
	  break;

	default:
	  break;
	}
    }
}
#endif /* !_WINDOWS */
#endif /* ANNOTATIONS */

/*----------------------------------------------------------------------
  AnnotConfMenu
  Build and display the Conf Menu dialog box and prepare for input.
  ----------------------------------------------------------------------*/
void         AnnotConfMenu (Document document, View view)
{
#ifdef ANNOTATIONS
#ifndef _WINDOWS
   int              i;

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_DEFAULT_BUTTON));

   TtaNewSheet (AnnotBase + AnnotMenu, 
		TtaGetViewFrame (document, view),
		TtaGetMessage (AMAYA, AM_ANNOT_CONF_MENU),
		2, s, FALSE, 6, 'L', D_DONE);

   TtaNewTextForm (AnnotBase + mAnnotUser,
		   AnnotBase + AnnotMenu,
		   TtaGetMessage (AMAYA, AM_ANNOT_USER),
		   30,
		   1,
		   TRUE);
   TtaNewTextForm (AnnotBase + mAnnotPostServer,
		   AnnotBase + AnnotMenu,
		   TtaGetMessage (AMAYA, AM_ANNOT_POST_SERVER),
		   30,
		   1,
		   TRUE);
   TtaNewTextForm (AnnotBase + mAnnotServers,
		   AnnotBase + AnnotMenu,
		   TtaGetMessage (AMAYA, AM_ANNOT_SERVERS),
		   30,
		   1,
		   TRUE);
   sprintf (s, "B%s%cB%s%cB%s",
	     TtaGetMessage (AMAYA, AM_ANNOT_LAUTOLOAD), EOS,
	     TtaGetMessage (AMAYA, AM_ANNOT_RAUTOLOAD), EOS,
	     TtaGetMessage (AMAYA, AM_ANNOT_RAUTOLOAD_RST));
   TtaNewToggleMenu (AnnotBase + mToggleAnnot,
		     AnnotBase + AnnotMenu,
		     NULL,
		     3,
		     s,
		     NULL,
		     FALSE);
#endif /* !_WINDOWS */

   /* load and display the current values */
   GetAnnotConf ();

#ifndef _WINDOWS
   RefreshAnnotMenu ();
  /* display the menu */
   TtaSetDialoguePosition ();
   TtaShowDialogue (AnnotBase + AnnotMenu, TRUE);
#else
  if (!AnnotHwnd)
    /* only activate the menu if it isn't active already */
     DialogBox (hInstance, MAKEINTRESOURCE (ANNOTMENU), NULL, (DLGPROC) WIN_AnnotDlgProc);
  else
     SetFocus (AnnotHwnd);
#endif /* !_WINDOWS */
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
   InitConfMenu: initialisation, called during Amaya initialisation
  ----------------------------------------------------------------------*/
void                InitConfMenu (void)
{
  InitAmayaDefEnv ();
#ifndef _WINDOWS
  CacheBase = TtaSetCallback (CacheCallbackDialog, MAX_CACHEMENU_DLG);
  ProxyBase = TtaSetCallback (ProxyCallbackDialog, MAX_PROXYMENU_DLG);
  GeneralBase = TtaSetCallback (GeneralCallbackDialog, MAX_GENERALMENU_DLG);
  PublishBase = TtaSetCallback (PublishCallbackDialog, MAX_PUBLISHMENU_DLG);
  ColorBase = TtaSetCallback (ColorCallbackDialog,
			      MAX_COLORMENU_DLG);
  GeometryBase = TtaSetCallback (GeometryCallbackDialog,
				 MAX_GEOMETRYMENU_DLG);
  LanNegBase = TtaSetCallback (LanNegCallbackDialog,
			       MAX_LANNEGMENU_DLG);
  ProfileBase = TtaSetCallback (ProfileCallbackDialog,
			       MAX_PROFILEMENU_DLG);
  TemplatesBase = TtaSetCallback (TemplatesCallbackDialog,
			       MAX_LANNEGMENU_DLG);
#ifdef ANNOTATIONS
  AnnotBase = TtaSetCallback (AnnotCallbackDialog,
			      MAX_ANNOTMENU_DLG);
#endif /* ANNOTATIONS */
#endif /* !_WINDOWS */
}
