/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1999
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

#ifdef _WINDOWS
#include "resource.h"
#include "wininclude.h"

#include "constmedia.h"
#include "appdialogue.h"


extern HINSTANCE hInstance;
#endif /* _WINDOWS */

/* this one should be exported from the thotlib */
extern CHAR_T* ColorName (int num);

static int CacheStatus;
static int ProxyStatus;
static int SafePutStatus;

/* Cache menu options */
#ifdef _WINDOWS
static HWND CacheHwnd = NULL;
#endif /* _WINDOWS */
static int      CacheBase;
static ThotBool EnableCache;
static ThotBool CacheProtectedDocs;
static ThotBool CacheDisconnectMode;
static ThotBool CacheExpireIgnore;
static CHAR_T   CacheDirectory [MAX_LENGTH];
static int      CacheSize;
static int      MaxCacheFile;

/* Proxy menu options */
#ifdef _WINDOWS
static HWND     ProxyHwnd = NULL;
#endif /* _WINDOWS */
static int      ProxyBase;
static CHAR_T   HttpProxy [MAX_LENGTH];
static CHAR_T   ProxyDomain [MAX_LENGTH];
static ThotBool ProxyDomainIsOnlyProxy;

/* General menu options */
#ifdef _WINDOWS
static CHAR_T   AppHome [MAX_LENGTH];
static CHAR_T   AppTmpDir [MAX_LENGTH];
static HWND     GeneralHwnd = NULL;
#endif /* _WINDOWS */
static int      GeneralBase;
static int      DoubleClickDelay;
static int      Zoom;
static ThotBool Multikey;
static CHAR_T   DefaultName [MAX_LENGTH];
static ThotBool BgImages;
static ThotBool DoubleClick;
static CHAR_T   DialogueLang [MAX_LENGTH];
static int      FontMenuSize;
static CHAR_T   HomePage [MAX_LENGTH];

/* Publish menu options */
#ifdef _WINDOWS
static HWND     PublishHwnd =  NULL;
#endif /* _WINDOWS */
static int      PublishBase;
static ThotBool LostUpdateCheck;
static ThotBool VerifyPublish;
static CHAR_T   SafePutRedirect [MAX_LENGTH];

/* Color menu options */
#ifdef _WINDOWS
static HWND     ColorHwnd = NULL;
#endif /* _WINDOWS */
static int      ColorBase;
static CHAR_T   FgColor [MAX_LENGTH];
static CHAR_T   BgColor [MAX_LENGTH];
#ifndef _WINDOWS
static CHAR_T   MenuFgColor [MAX_LENGTH];
static CHAR_T   MenuBgColor [MAX_LENGTH];
#endif /* !_WINDOWS */

/* Geometry menu options */
static int      GeometryBase;
static Document GeometryDoc = 0;
#ifdef _WINDOWS
HWND            GeometryHwnd = NULL;
#endif /* _WINDOWS */
/* common local variables */
CHAR_T          s[MAX_LENGTH]; /* general purpose buffer */

/* Language negotiation menu options */
#ifdef _WINDOWS
static HWND     LanNegHwnd = NULL;
#endif /* _WINDOWS */
static int      LanNegBase;
static CHAR_T   LanNeg [MAX_LENGTH];

/* Profile menu options */
#ifdef _WINDOWS
static HWND     ProfileHwnd = NULL;
static HWND     wndProfilesList;
static HWND     wndProfile;
#endif /* _WINDOWS */
static int      ProfileBase;
static CHAR_T   Profile [MAX_LENGTH];
static CHAR_T   Profiles_File [MAX_LENGTH];
#define MAX_PRO 50
static char*    MenuText[MAX_PRO];

/* Templates menu option */
#ifdef _WINDOWS
static HWND     TemplatesHwnd = NULL;
#endif /* _WINDOWS */
static int      TemplatesBase;
static CHAR_T   TemplatesUrl [MAX_LENGTH];
static int      CurrentProfile = -1;

#include "query_f.h"
#include "init_f.h"


/*
** Common functions
*/
/*----------------------------------------------------------------------
   GetEnvString: front end to TtaGetEnvString. If the variable name doesn't
   exist, it sets the value to an empty ("") string
   ----------------------------------------------------------------------*/
#ifdef __STDC__
static void  GetEnvString (char* name, CHAR_T* value)
#else
static void  GetEnvString (name, value)
char*        name;
CHAR_T*      value;
#endif /* __STDC__ */
{
  CHAR_T*  ptr;

  ptr = TtaGetEnvString (name);
  if (ptr)
    {
      ustrncpy (value, ptr, MAX_LENGTH);
      value[MAX_LENGTH-1] = WC_EOS;
    }
  else
    value[0] = WC_EOS;
}

/*----------------------------------------------------------------------
  InitAmayaDefEnv
  Initializes the default Amaya options which are not setup by thot.ini file.
  This protects us against a crash due to a user's erasing that file.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void InitAmayaDefEnv (void)
#else
void InitAmayaDefEnv ()
#endif /* __STDC__ */
{
  CHAR_T* ptr;

  /* browsing editing options */
  ptr = TtaGetEnvString ("THOTDIR");
  if (ptr != NULL)
    {
      ustrcpy (HomePage, ptr);
      ustrcat (HomePage, AMAYA_PAGE);
    }
  else
    HomePage[0]  = WC_EOS;
  TtaSetDefEnvString ("HOME_PAGE", HomePage, FALSE);
  HomePage[0] = WC_EOS;
  TtaSetDefEnvString ("ENABLE_MULTIKEY", TEXT("no"), FALSE);
  TtaSetDefEnvString ("ENABLE_BG_IMAGES", TEXT("yes"), FALSE);
  TtaSetDefEnvString ("VERIFY_PUBLISH", TEXT("no"), FALSE);
  TtaSetDefEnvString ("ENABLE_LOST_UPDATE_CHECK", TEXT("yes"), FALSE);
  TtaSetDefEnvString ("DEFAULTNAME", TEXT("Overview.html"), FALSE);
  TtaSetDefEnvString ("FontMenuSize", TEXT("12"), FALSE);
  TtaSetDefEnvString ("ENABLE_DOUBLECLICK", TEXT("yes"), FALSE);
  /* @@@ */
  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &DoubleClick);
  /* @@@ */
  
#ifndef _WINDOWS
  TtaSetDefEnvString ("THOTPRINT", TEXT("lpr"), FALSE);
  /* A4 size */
  TtaSetDefEnvString ("PAPERSIZE", TEXT("0"), FALSE);
#endif
  /* network configuration */
  TtaSetDefEnvString ("SAFE_PUT_REDIRECT", TEXT(""), FALSE);
  TtaSetDefEnvString ("ENABLE_LOST_UPDATE_CHECK", TEXT("yes"), FALSE);
  TtaSetDefEnvString ("ENABLE_PIPELINING", TEXT("yes"), FALSE);
  TtaSetDefEnvString ("NET_EVENT_TIMEOUT", TEXT("60000"), FALSE);
  TtaSetDefEnvString ("PERSIST_CX_TIMEOUT", TEXT("60"), FALSE);
  TtaSetDefEnvString ("DNS_TIMEOUT", TEXT("1800"), FALSE);
  TtaSetDefEnvString ("MAX_SOCKET", TEXT("32"), FALSE);
  TtaSetDefEnvString ("ENABLE_MDA", TEXT("yes"), FALSE);
  TtaSetDefEnvString ("HTTP_PROXY", TEXT(""), FALSE);
  TtaSetDefEnvString ("PROXYDOMAIN", TEXT(""), FALSE);
  TtaSetDefEnvString ("PROXYDOMAIN_IS_ONLYPROXY", TEXT("no"), FALSE);
  TtaSetDefEnvString ("MAX_CACHE_ENTRY_SIZE", TEXT("3"), FALSE);
  TtaSetDefEnvString ("CACHE_SIZE", TEXT("10"), FALSE);
  if (TempFileDirectory)
  {
    usprintf (s, TEXT("%s%clibwww-cache"), TempFileDirectory, WC_DIR_SEP);
    TtaSetDefEnvString ("CACHE_DIR", s, FALSE);
	TtaSetDefEnvString ("ENABLE_CACHE", TEXT("yes"), FALSE);
  }
  else
  {
    TtaSetDefEnvString ("CACHE_DIR", TEXT(""), FALSE);
	TtaSetDefEnvString ("ENABLE_CACHE", TEXT("yes"), FALSE);
  }
  TtaSetDefEnvString ("CACHE_PROTECTED_DOCS", TEXT("yes"), FALSE);
  TtaSetDefEnvString ("CACHE_DISCONNECTED_MODE", TEXT("no"), FALSE);
  TtaSetDefEnvString ("CACHE_EXPIRE_IGNORE", TEXT("no"), FALSE);
  /* appearance */

}

/*----------------------------------------------------------------------
   GetDefEnvToggleBoolean: front end to TtaGetDefEnvBoolean. It takes
   care of switching the toggle button according to the status of the
   variable.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void GetDefEnvToggle (char* name, ThotBool *value, int ref, int entry)
#else
static void GetDefEnvToggle (name, value, ref, entry)
char*       name;
ThotBool*   value;
int ref;
int entry;
#endif /* __STDC__ */
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
#ifdef __STDC__
static void GetDefEnvString (char* name, CHAR_T* value)
#else
static void GetDefEnvString (name, value)
char*       name;
CHAR_T*     value;
#endif /* __STDC__ */
{
  CHAR_T* ptr;

  ptr = TtaGetDefEnvString (name);
  if (ptr) {
    ustrncpy (value, ptr, MAX_LENGTH);
    value[MAX_LENGTH-1] = WC_EOS;
  }
  else
    value[0] = WC_EOS;
}

/*----------------------------------------------------------------------
  NormalizeDirName
  verifies if dirname finishes with end_path. If this is so, it returns FALSE
  Otherwise, adds end_path to dirname and returns TRUE.
  end_path should begin with a DIR_SEP char 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int NormalizeDirName (STRING dirname, const STRING end_path)
#else
static int NormalizeDirName (dirname, end_path)
STRING dirname;
const STRING end_path;
#endif /* __STDC__ */
{
  int result = 0;
  STRING ptr;
  STRING dir_sep = NULL;

  if (dirname[0] != EOS)
    {
	  /* if dirname ends in DIR_SEP, we remove it */
	  if (dirname [ustrlen (dirname) -1] == DIR_SEP)
	  {
		  dir_sep = ustrrchr (dirname, DIR_SEP);
		  *dir_sep = EOS;
		  result = 1;
	  }
      ptr = ustrstr (dirname, end_path);
      if (ptr)
	{
	  if (ustrcasecmp (ptr, end_path))
	    /* end_path missing, add it to the parent dir */
	    {
	   	  ustrcat (dirname, end_path);
	      result = 1;
	    }
	}
      else
	/* no DIR_SEP, so we add the end_path */
	{
	  ustrcat (dirname, end_path);
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
#ifdef __STDC__
static void AmCopyFile (const CHAR_T* source_dir, const CHAR_T* dest_dir, const CHAR_T* filename)
#else
static void AmCopyFile (source_dir, dest_dir, filename)
const CHAR_T* source_dir;
const CHAR_T* dest_dir;
const CHAR_T* filename;
#endif /* __STDC__ */
{
 CHAR_T source_file [MAX_LENGTH];
 
 usprintf (source_file, TEXT("%s%c%s"), source_dir, DIR_SEP, filename);
 if (TtaFileExist (source_file))
 {
   usprintf (s, TEXT("%s%c%s"), dest_dir, DIR_SEP, filename);
   TtaFileCopy (source_file, s);
 }
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  CleanDirSep
  Removes double DIR_SEP strings in a name. Returns TRUE if such
  operation was done.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int CleanDirSep (CHAR_T* name)
#else
int CleanDirSep (name)
CHAR_T* name
#endif /* __STDC__ */
{
 int result = 0;
 int s, d;

  /* remove all double DIR_SEP */
  s = 0;
  d = 0;
  while (name[d] != WC_EOS)
  {
    if (name[d] == WC_DIR_SEP && name[d + 1] == WC_DIR_SEP)
	{
	  result = 1;
	  d++;
	  continue;
	}
	name[s] = name[d];
	s++;
	d++;
  }
  name[s] = WC_EOS;

  return (result);
}

/*----------------------------------------------------------------------
  CleanFirstLastSpace
  Removes the first and last space in a name. Returns TRUE if such
  operation was done.
  Returns 1 if it made any change, 0 otherwise (not boolean).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int CleanFirstLastSpace (CHAR_T* name)
#else
int CleanFirstLastSpace (name)
CHAR_T* name
#endif /* __STDC__ */
{
 int result = 0;
 int l, d;

 if (!name ||  *name == WC_EOS)
   return (0);

 /* start by removing the ending spaces */
 l = ustrlen (name) - 1;
 while (l > 0 && name[l] == TEXT(' '))
   l--;
 if (name[l+1] == TEXT(' '))
   {
     result = 1;
     name[l+1] = WC_EOS;
   }

 /* now remove the leading spaces */
 l = 0;
 while (name[l] == TEXT(' ') && name[l] != WC_EOS)
   l++;
 if (l > 0)
   {
     result = 1;
     d = 0;
     while (name[l] != WC_EOS)
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
#ifdef __STDC__
static int RemoveLastDirSep (CHAR_T* name)
#else
static int RemoveLastDirSep (name)
CHAR_T* name;
#endif
{
  int result;
  int last_char;

  result = 0;
  if (name) 
    { 
        last_char = ustrlen (name) - 1;
        if (name[last_char] == WC_DIR_SEP)
          {
                name[last_char] = WC_EOS;
                result = 1;
          }
    }

  return result;
}

/*********************
** Cache configuration menu
***********************/
/*----------------------------------------------------------------------
  GetCacheConf
  Makes a copy of the current registry cache values
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void GetCacheConf (void)
#else
static void GetCacheConf ()
#endif /* __STDC__ */
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
#ifdef __STDC__
static void ValidateCacheConf (void)
#else
static void ValidateCacheConf ()
#endif /* __STDC__ */
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
  change += NormalizeDirName (CacheDirectory, TEXT("\\libwww-cache"));
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
#ifdef __STDC__
static void SetCacheConf (void)
#else
static void SetCacheConf ()
#endif /* __STDC__ */
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
#ifdef __STDC__
static void GetDefaultCacheConf ()
#else
static void GetDefaultCacheConf ()
#endif /*__STDC__*/
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
#ifdef __STDC__
static void WIN_RefreshCacheMenu (HWND hwnDlg)
#else
static void WIN_RefreshCacheMenu (hwnDlg)
HWND hwnDlg;
#endif /* __STDC__ */
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
#ifdef __STDC__
static void RefreshCacheMenu ()
#else
static void RefreshCacheMenu ()
#endif /* __STDC__ */
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
#ifdef __STDC__
LRESULT CALLBACK WIN_CacheDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, 
				   LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK WIN_CacheDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{ 
  switch (msg)
    {
    case WM_INITDIALOG:
      CacheHwnd = hwnDlg;
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
#ifdef __STDC__
static void         CacheCallbackDialog (int ref, int typedata, STRING data)
#else
static void         CacheCallbackDialog (ref, typedata, data)
int                 ref;
int                 typedata;
STRING              data;

#endif
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
	    ustrcpy (CacheDirectory, data);
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
#ifdef __STDC__
void         CacheConfMenu (Document document, View view)
#else
void         CacheConfMenu (document, view)
Document     document;
View         view;
#endif
{
#ifndef _WINDOWS
   int              i;

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));
   i += ustrlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_DEFAULT_BUTTON));
   i += ustrlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_FLUSH_CACHE_BUTTON));
   TtaNewSheet (CacheBase + CacheMenu, 
		TtaGetViewFrame (document, view),
		TtaGetMessage (AMAYA, AM_CACHE_MENU),
		3, s, FALSE, 6, 'L', D_DONE);

   usprintf (s, "B%s%cB%s%cB%s%cB%s",
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
    {
      switch (app_lang)
	{ 
	case FR_LANG:
	  DialogBox (hInstance, MAKEINTRESOURCE (FR_CACHEMENU), NULL, 
		     (DLGPROC) WIN_CacheDlgProc);
	  break;
	case DE_LANG:
	  DialogBox (hInstance, MAKEINTRESOURCE (DE_CACHEMENU), NULL, 
		     (DLGPROC) WIN_CacheDlgProc);
	  break;
	default:
	  DialogBox (hInstance, MAKEINTRESOURCE (EN_CACHEMENU), NULL, 
		     (DLGPROC) WIN_CacheDlgProc);
	}
    }
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
#ifdef __STDC__
static void GetProxyConf (void)
#else
static void GetProxyConf ()
#endif /* __STDC__ */
{
  GetEnvString ("HTTP_PROXY", HttpProxy);
  GetEnvString ("PROXYDOMAIN", ProxyDomain);
  TtaGetEnvBoolean ("PROXYDOMAIN_IS_ONLYPROXY", &ProxyDomainIsOnlyProxy);
}

/*----------------------------------------------------------------------
  SetProxyConf
  Updates the registry proxy values
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void SetProxyConf (void)
#else
static void SetProxyConf ()
#endif /* __STDC__ */
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
#ifdef __STDC__
static void GetDefaultProxyConf ()
#else
static void GetDefaultProxyConf ()
#endif /*__STDC__*/
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
#ifdef __STDC__
void WIN_RefreshProxyMenu (HWND hwnDlg)
#else
void WIN_RefreshProxyMenu (hwnDlg)
HWND hwnDlg;
#endif /* __STDC__ */
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
#ifdef __STDC__
static void RefreshProxyMenu ()
#else
static void RefreshProxyMenu ()
#endif /* __STDC__ */
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
#ifdef __STDC__
LRESULT CALLBACK WIN_ProxyDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam,
				     LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK WIN_ProxyDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
  switch (msg)
    {
    case WM_INITDIALOG:
      ProxyHwnd = hwnDlg;
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
#ifdef __STDC__
static void         ProxyCallbackDialog (int ref, int typedata, STRING data)
#else
static void         ProxyCallbackDialog (ref, typedata, data)
int                 ref;
int                 typedata;
STRING              data;

#endif
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
	    ustrcpy (HttpProxy, data);
	  else
	    HttpProxy [0] = EOS;
	  break;

	case mProxyDomain:
	  ProxyStatus |= AMAYA_PROXY_RESTART;
	  if (data)
	    ustrcpy (ProxyDomain, data);
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
#ifdef __STDC__
void         ProxyConfMenu (Document document, View view)
#else
void         ProxyConfMenu (document, view)
Document            document;
View                view;
#endif
{

#ifndef _WINDOWS
   int              i;

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));
   i += ustrlen (&s[i]) + 1;
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
   usprintf (s, "T%s%cT%s", 
	     "No proxy on these domains", EOS,
	     "Only proxy these domains");
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
    {
	  switch (app_lang)
	   { 
	   case FR_LANG:
           DialogBox (hInstance, MAKEINTRESOURCE (FR_PROXYMENU), NULL, 
		  (DLGPROC) WIN_ProxyDlgProc);
	       break;
	   case DE_LANG:
		   DialogBox (hInstance, MAKEINTRESOURCE (DE_PROXYMENU), NULL, 
		  (DLGPROC) WIN_ProxyDlgProc);
	       break;
	   default:
		   DialogBox (hInstance, MAKEINTRESOURCE (EN_PROXYMENU), NULL, 
		  (DLGPROC) WIN_ProxyDlgProc);
		   break;
	   }
    }
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
#ifdef __STDC__
static void GetGeneralConf (void)
#else
static void GetGeneralConf ()
#endif /* __STDC__ */
{
  TtaGetEnvInt ("DOUBLECLICKDELAY", &DoubleClickDelay);
  TtaGetEnvInt ("ZOOM", &Zoom);
  TtaGetEnvBoolean ("ENABLE_MULTIKEY", &Multikey);
  TtaGetEnvBoolean ("ENABLE_BG_IMAGES", &BgImages);
  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &DoubleClick);
  GetEnvString ("HOME_PAGE", HomePage);
  GetEnvString ("LANG", DialogueLang);
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
#ifdef __STDC__
static void ValidateGeneralConf (void)
#else
static void ValidateGeneralConf ()
#endif /* __STDC__ */
{
  int change;
  CHAR_T lang[3];
  STRING ptr;
#ifdef _WINDOWS
  CHAR_T old_AppTmpDir [MAX_LENGTH];
  int i;

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
	  usprintf (s, TEXT("Error creating directory %s"), AppTmpDir);
	  MessageBox (GeneralHwnd, s, TEXT("MenuConf:VerifyGeneralConf"), MB_OK);
	  exit (1);
	} 
      else
	change++;
    }
  if (change)
    SetDlgItemText (GeneralHwnd, IDC_TMPDIR, AppTmpDir);

  /* if AppTmpDir changed, update the cache dir env variables */
  GetEnvString ("APP_TMPDIR", old_AppTmpDir);
  if (ustrcasecmp (AppTmpDir, old_AppTmpDir))
    {
      /* the new default cache value is AppTmpDir/libwww-cache */
      usprintf (s, TEXT("%s%clibwww-cache"), AppTmpDir, DIR_SEP);
      TtaSetDefEnvString ("CACHE_DIR", s, TRUE);

      /* if the cache was in AppTmpDir and AppTmpDir changed, move
	 the cache */
      usprintf (s, TEXT("%s%clibwww-cache"), old_AppTmpDir, DIR_SEP); 
      ptr = TtaGetEnvString ("CACHE_DIR");
      if (ptr && !ustrcasecmp (s, ptr))
	{
	  usprintf (s, TEXT("%s%clibwww-cache"), AppTmpDir, DIR_SEP);		  
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
      usprintf (s, TEXT("%s%c%d"), AppTmpDir, DIR_SEP, i);
      TtaMakeDirectory (s);
    }
#endif /* _WINDOWS */
  
  /* validate the dialogue language */
  change = 0;
  ptr = TtaGetEnvString ("THOTDIR");
  if (ustrcmp (DialogueLang, TEXT("en-US")))
    {
      change++;
      DialogueLang[2] = EOS;
    }
  ustrncpy (lang, DialogueLang, 2);
  lang[2] = EOS;
  usprintf (s, TEXT("%s%cconfig%c%s-amayamsg"), ptr, DIR_SEP, DIR_SEP, lang);
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
#ifdef __STDC__
static void      RecalibrateZoom ()
#else
static void      RecalibrateZoom ()
#endif
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
	    if (TtaIsViewOpened (doc, view))
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
#ifdef __STDC__
static void SetGeneralConf (void)
#else
static void SetGeneralConf ()
#endif /* __STDC__ */
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
  TtaSetEnvString ("HOME_PAGE", HomePage, TRUE);
  TtaSetEnvString ("LANG", DialogueLang, TRUE);
  TtaSetEnvInt ("FontMenuSize", FontMenuSize, TRUE);
#ifdef _WINDOWS
  TtaSetEnvString ("APP_TMPDIR", AppTmpDir, TRUE);
  ustrcpy (TempFileDirectory, AppTmpDir);
  TtaAppendDocumentPath (TempFileDirectory);
#endif /* _WINDOWS */

  TtaSaveAppRegistry ();
}

/*----------------------------------------------------------------------
  GetDefaultGeneralConf
  Loads the default registry General values
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void GetDefaultGeneralConf ()
#else
static void GetDefaultGeneralConf ()
#endif /*__STDC__*/
{
  TtaGetDefEnvInt ("DOUBLECLICKDELAY", &DoubleClickDelay);
  TtaGetDefEnvInt ("ZOOM", &Zoom);
  GetDefEnvToggle ("ENABLE_MULTIKEY", &Multikey, 
		       GeneralBase + mToggleGeneral, 0);
  GetDefEnvToggle ("ENABLE_BG_IMAGES", &BgImages,
		       GeneralBase + mToggleGeneral, 1);
  GetDefEnvToggle ("ENABLE_DOUBLECLICK", &DoubleClick,
		       GeneralBase + mToggleGeneral, 2);
  GetDefEnvString ("HOME_PAGE", HomePage);
  GetDefEnvString ("LANG", DialogueLang);
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
#ifdef __STDC__
void WIN_RefreshGeneralMenu (HWND hwnDlg)
#else
void WIN_RefreshGeneralMenu (hwnDlg)
HWND hwnDlg;
#endif /* __STDC__ */
{
  SetDlgItemText (hwnDlg, IDC_HOMEPAGE, HomePage);
  SetDlgItemInt (hwnDlg, IDC_FONTMENUSIZE, FontMenuSize, FALSE);
  CheckDlgButton (hwnDlg, IDC_MULTIKEY, (Multikey) 
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_BGIMAGES, (BgImages) 
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_DOUBLECLICK, (DoubleClick) 
		  ? BST_CHECKED : BST_UNCHECKED);
  SetDlgItemText (hwnDlg, IDC_DIALOGUELANG, DialogueLang);
  SetDlgItemInt (hwnDlg, IDC_ZOOM, Zoom, TRUE);
  SetDlgItemText (hwnDlg, IDC_TMPDIR, AppTmpDir);
  SetDlgItemText (hwnDlg, IDC_APPHOME, AppHome);
}
#else /* _WINDOWS */
/*----------------------------------------------------------------------
  RefreshGeneralMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void RefreshGeneralMenu ()
#else
static void RefreshGeneralMenu ()
#endif /* __STDC__ */
{
  TtaSetNumberForm (GeneralBase + mDoubleClickDelay, DoubleClickDelay);
  TtaSetNumberForm (GeneralBase + mZoom, Zoom);
  TtaSetToggleMenu (GeneralBase + mToggleGeneral, 0, Multikey);
  TtaSetToggleMenu (GeneralBase + mToggleGeneral, 1, BgImages);
  TtaSetToggleMenu (GeneralBase + mToggleGeneral, 2, DoubleClick);
  TtaSetTextForm (GeneralBase + mHomePage, HomePage);
  TtaSetTextForm (GeneralBase + mDialogueLang, DialogueLang);
  TtaSetNumberForm (GeneralBase + mFontMenuSize, FontMenuSize);
}
#endif /* !_WINDOWS */

#ifdef _WINDOWS
/*----------------------------------------------------------------------
  WIN_GeneralDlgProc
  Windows callback for the general menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK WIN_GeneralDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam,
				     LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK WIN_GeneralDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{ 
  switch (msg)
    {
    case WM_INITDIALOG:
      GeneralHwnd = hwnDlg;
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
	case IDC_MULTIKEY:
	  Multikey = !Multikey;
	  break;
	case IDC_BGIMAGES:
	  BgImages = !BgImages;
	  break;
	case IDC_DOUBLECLICK:
	  DoubleClick = !DoubleClick;
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
#ifdef __STDC__
static void         GeneralCallbackDialog (int ref, int typedata, STRING data)
#else
static void         GeneralCallbackDialog (ref, typedata, data)
int                 ref;
int                 typedata;
STRING              data;

#endif
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
	    ustrcpy (HomePage, data);
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
	    }
	  break;

	case mFontMenuSize:
	  FontMenuSize = val;
	  break;
	  
	case mDialogueLang:
	  if (data)
	    ustrcpy (DialogueLang, data);
	  else
	    DialogueLang [0] = EOS;
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
#ifdef __STDC__
void         GeneralConfMenu (Document document, View view)
#else
void         GeneralConfMenu (document, view)
Document            document;
View                view;
STRING              pathname;

#endif
{
#ifndef _WINDOWS 
   int              i;

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));
   i += ustrlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_DEFAULT_BUTTON));

   TtaNewSheet (GeneralBase + GeneralMenu, 
		TtaGetViewFrame (document, view),
		TtaGetMessage (AMAYA, AM_GENERAL_MENU),
		2, s, TRUE, 2, 'L', D_DONE);
   /* first line */
   TtaNewTextForm (GeneralBase + mHomePage,
		   GeneralBase + GeneralMenu,
		   TtaGetMessage (AMAYA, AM_HOME_PAGE),
		   40,
		   1,
		   FALSE);
   TtaNewLabel (GeneralBase + mGeneralEmpty1, GeneralBase + GeneralMenu, " ");
   /* second line */
   TtaNewNumberForm (GeneralBase + mDoubleClickDelay,
		     GeneralBase + GeneralMenu,
		     TtaGetMessage (AMAYA, AM_DOUBLECLICK_DELAY),
		     0,
		     65000,
		     FALSE);   
   TtaNewLabel (GeneralBase + mGeneralEmpty4, GeneralBase + GeneralMenu, " ");
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
   /* fourth line */
   TtaNewTextForm (GeneralBase + mDialogueLang,
		   GeneralBase + GeneralMenu,
		   TtaGetMessage (AMAYA, AM_DIALOGUE_LANGUAGE),
		   10,
		   1,
		   FALSE);
   TtaNewLabel (GeneralBase + mGeneralEmpty2, GeneralBase + GeneralMenu, " ");   
   /* fifth line */
   usprintf (s, "B%s%cB%s%cB%s", 
	    TtaGetMessage (AMAYA, AM_ENABLE_MULTIKEY), EOS, 
	    TtaGetMessage (AMAYA, AM_SHOW_BG_IMAGES), EOS, 
	    TtaGetMessage (AMAYA, AM_ENABLE_DOUBLECLICK));

   TtaNewToggleMenu (GeneralBase + mToggleGeneral,
		     GeneralBase + GeneralMenu,
		     NULL,
		     3,
		     s,
		     NULL,
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
     {
	   switch (app_lang)
	   { 
	   case FR_LANG:
           DialogBox (hInstance, MAKEINTRESOURCE (FR_GENERALMENU), NULL, 
		  (DLGPROC) WIN_GeneralDlgProc);
	       break;
	   case DE_LANG:
		   DialogBox (hInstance, MAKEINTRESOURCE (DE_GENERALMENU), NULL, 
		  (DLGPROC) WIN_GeneralDlgProc);
	       break;
	   default:
		   DialogBox (hInstance, MAKEINTRESOURCE (EN_GENERALMENU), NULL, 
		  (DLGPROC) WIN_GeneralDlgProc);
		   break;
	   }
     }
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
#ifdef __STDC__
static void GetPublishConf (void)
#else
static void GetPublishConf ()
#endif /* __STDC__ */
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
#ifdef __STDC__
static void SetPublishConf (void)
#else
static void SetPublishConf ()
#endif /* __STDC__ */
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
#ifdef __STDC__
static void GetDefaultPublishConf ()
#else
static void GetDefaultPublishConf ()
#endif /*__STDC__*/
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
#ifdef __STDC__
void WIN_RefreshPublishMenu (HWND hwnDlg)
#else
void WIN_RefreshPublishMenu (hwnDlg)
HWND hwnDlg;
#endif /* __STDC__ */
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
#ifdef __STDC__
static void RefreshPublishMenu ()
#else
static void RefreshPublishMenu ()
#endif /* __STDC__ */
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
#ifdef __STDC__
LRESULT CALLBACK WIN_PublishDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam,
				     LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK WIN_PublishDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{ 
  switch (msg)
    {
    case WM_INITDIALOG:
      PublishHwnd = hwnDlg;
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
#ifdef __STDC__
static void         PublishCallbackDialog (int ref, int typedata, STRING data)
#else
static void         PublishCallbackDialog (ref, typedata, data)
int                 ref;
int                 typedata;
STRING              data;

#endif
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
	    ustrcpy (DefaultName, data);
	  else
	    DefaultName[0] = EOS;
	  break;

	case mSafePutRedirect:
	  if (data)
	    ustrcpy (SafePutRedirect, data);
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
#ifdef __STDC__
void         PublishConfMenu (Document document, View view)
#else
void         PublishConfMenu (document, view)
Document            document;
View                view;
STRING              pathname;

#endif
{
#ifndef _WINDOWS
   int              i;

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));
   i += ustrlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_DEFAULT_BUTTON));

   TtaNewSheet (PublishBase + PublishMenu, 
		TtaGetViewFrame (document, view),
	       TtaGetMessage (AMAYA, AM_PUBLISH_MENU),
		2, s, FALSE, 11, 'L', D_DONE);
   usprintf (s, "B%s%cB%s", 
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
    /* only activate the menu if it isn't active already */
    {
	  switch (app_lang)
	   { 
	   case FR_LANG:
           DialogBox (hInstance, MAKEINTRESOURCE (FR_PUBLISHMENU), NULL, 
		  (DLGPROC) WIN_PublishDlgProc);
	       break;
	   case DE_LANG:
		   DialogBox (hInstance, MAKEINTRESOURCE (DE_PUBLISHMENU), NULL, 
		  (DLGPROC) WIN_PublishDlgProc);
	       break;
	   default:
		   DialogBox (hInstance, MAKEINTRESOURCE (EN_PUBLISHMENU), NULL, 
		  (DLGPROC) WIN_PublishDlgProc);
		   break;
	   }
    }
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
#ifdef __STDC__
static void GetColorConf (void)
#else
static void GetColorConf ()
#endif /* __STDC__ */
{
  GetEnvString ("ForegroundColor", FgColor);
  GetEnvString ("BackgroundColor", BgColor);
#ifndef _WINDOWS
  GetEnvString ("MenuFgColor", MenuFgColor);
  GetEnvString ("MenuBgColor", MenuBgColor);
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
  GetDefaultColorConf
  Makes a copy of the default registry color values
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void GetDefaultColorConf (void)
#else
static void GetDefaultColorConf ()
#endif /* __STDC__ */
{
  GetDefEnvString ("ForegroundColor", FgColor);
  GetDefEnvString ("BackgroundColor", BgColor);
#ifndef _WINDOWS
  GetDefEnvString ("MenuFgColor", MenuFgColor);
  GetDefEnvString ("MenuBgColor", MenuBgColor);
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
  SetColorConf
  Updates the registry Color values
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void SetColorConf (void)
#else
static void SetColorConf ()
#endif /* __STDC__ */
{
  TtaSetEnvString ("ForegroundColor", FgColor, TRUE);
  TtaSetEnvString ("BackgroundColor", BgColor, TRUE);
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
#ifdef __STDC__
void WIN_RefreshColorMenu (HWND hwnDlg)
#else
void WIN_RefreshColorMenu (hwnDlg)
HWND hwnDlg;
#endif /* __STDC__ */
{
  SetDlgItemText (hwnDlg, IDC_FGCOLOR, FgColor);
  SetDlgItemText (hwnDlg, IDC_BGCOLOR, BgColor);
}
#else /* WINDOWS */
/*----------------------------------------------------------------------
  RefreshColorMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void RefreshColorMenu ()
#else
static void RefreshColorMenu ()
#endif /* __STDC__ */
{
  TtaSetTextForm (ColorBase + mFgColor, FgColor);
  TtaSetTextForm (ColorBase + mBgColor, BgColor);
  TtaSetTextForm (ColorBase + mMenuFgColor, MenuFgColor);
  TtaSetTextForm (ColorBase + mMenuBgColor, MenuBgColor);
}
#endif /* !_WINDOWS */

#ifdef _WINDOWS
/*----------------------------------------------------------------------
  WIN_ColorDlgProc
  Windows callback for the color menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK WIN_ColorDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam,
				     LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK WIN_ColorDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
 int fgcolor, bgcolor;
  switch (msg)
    {
    case WM_INITDIALOG:
      ColorHwnd = hwnDlg;
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
	    }
	}
      switch (LOWORD (wParam))
	{
	  /* action buttons */
	case IDC_CHANGCOLOR:
	  TtcGetPaletteColors (&fgcolor, &bgcolor);
	  if (fgcolor != -1)
	    ustrcpy (FgColor, ColorName (fgcolor));
	  if (bgcolor != -1)
	    ustrcpy (BgColor, ColorName (bgcolor));
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
#ifdef __STDC__
static void         ColorCallbackDialog (int ref, int typedata, STRING data)
#else
static void         ColorCallbackDialog (ref, typedata, data)
int                 ref;
int                 typedata;
STRING              data;

#endif
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
	    ustrcpy (FgColor, data);
	  else
	    FgColor [0] = EOS;
	  break;
	case mBgColor:
	  if (data)
	    ustrcpy (BgColor, data);
	  else
	    BgColor [0] = EOS;
	  break;
	case mMenuFgColor:
	  if (data)
	    ustrcpy (MenuFgColor, data);
	  else
	    MenuFgColor [0] = EOS;
	  break;
	case mMenuBgColor:
	  if (data)
	    ustrcpy (MenuBgColor, data);
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
#ifdef __STDC__
void         ColorConfMenu (Document document, View view)
#else
void         ColorConfMenu (document, view)
Document            document;
View                view;
STRING              pathname;

#endif
{
#ifndef _WINDOWS
   int              i;

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));
   i += ustrlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_DEFAULT_BUTTON));

   TtaNewSheet (ColorBase + ColorMenu, 
		TtaGetViewFrame (document, view),
		TtaGetMessage (AMAYA, AM_COLOR_MENU),
		2, s, TRUE, 1, 'L', D_DONE);
   /* first line */
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
   /* second line */
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
    {
      switch (app_lang)
	{
	case FR_LANG:
	  DialogBox (hInstance, MAKEINTRESOURCE (FR_COLORMENU), NULL, 
		     (DLGPROC) WIN_ColorDlgProc);
	  break;
	case DE_LANG:
	  DialogBox (hInstance, MAKEINTRESOURCE (DE_COLORMENU), NULL, 
		     (DLGPROC) WIN_ColorDlgProc);
	  break;
	default:
	  DialogBox (hInstance, MAKEINTRESOURCE (EN_COLORMENU), NULL, 
		     (DLGPROC) WIN_ColorDlgProc);
	}
    }
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
#ifdef __STDC__
static void RestoreDefEnvGeom (char* env_var)
#else
static void RestoreDefEnvGeom (env_var)
char* env_var;
#endif /* _STDC_ */
{
  int x, y, w, h;

  CHAR_T EnvVar[MAX_LENGTH];
  iso2wc_strcpy (EnvVar, env_var);

  /* in order to read the default values from HTML.conf, we erase the 
     registry entry */
  TtaClearEnvString (env_var);
  TtaGetViewGeometryMM (GeometryDoc, EnvVar, &x, &y, &w, &h); 
  usprintf (s, TEXT("%d %d %d %d"), 
	   x,
	   y,
	   w,
	   h);

  TtaSetEnvString (env_var, s, TRUE);
}

/*----------------------------------------------------------------------
  SetEnvGeom
  Gets the current geometry for a view and saves it in the registry
  using the format "x y w h"
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void SetEnvGeom (char* view_name)
#else
static void SetEnvGeom (view_name)
char* view_name
#endif /* _STDC_ */
{
  int view;
  int x, y, w, h;
  

  TtaGetViewGeometryRegistry (GeometryDoc, view_name, &x, &y, &w, &h);

  view = TtaGetViewFromName (GeometryDoc, view_name);
  if (view != 0 && TtaIsViewOpened (GeometryDoc, view))
    {
      /* get current geometry */
      TtaGetViewWH (GeometryDoc, view, &w, &h);
      usprintf (s, TEXT("%d %d %d %d"), 
	       x,
	       y,
	       w,
	       h);
      
      TtaSetEnvString (view_name, s, TRUE);
    }
}

/*----------------------------------------------------------------------
  RestoreDefaultGeometryConf
  Makes a copy of the default registry geometry values
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void RestoreDefaultGeometryConf (void)
#else
static void RestoreDefaultGeometryConf ()
#endif /* __STDC__ */
{
  RestoreDefEnvGeom ("Formatted_view");
  RestoreDefEnvGeom ("Structure_view");
  RestoreDefEnvGeom ("Math_Structure_view");
  RestoreDefEnvGeom ("Graph_Structure_view");
  RestoreDefEnvGeom ("Alternate_view");
  RestoreDefEnvGeom ("Links_view");
  RestoreDefEnvGeom ("Table_of_contents");

  /* save the options */
  TtaSaveAppRegistry ();
}

/*----------------------------------------------------------------------
  SetEnvCurrentGeometry stores the current doc geometry in the registry
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void SetEnvCurrentGeometry ()
#else
static void SetEnvCurrentGeometry ()
#endif /* _STDC__ */
{
  /* only do the processing if the document exists */
  if (DocumentURLs[GeometryDoc])
    {
      SetEnvGeom ("Formatted_view");
      SetEnvGeom ("Structure_view");
      SetEnvGeom ("Math_Structure_view");
#ifdef GRAPHML
      SetEnvGeom ("Graph_Structure_view");
#endif /* GRAPHML */
      SetEnvGeom ("Alternate_view");
      SetEnvGeom ("Links_view");
      SetEnvGeom ("Table_of_contents");
    } /* if GeometryDoc exists */
}

/*----------------------------------------------------------------------
  SetGeometryConf
  Updates the registry Geometry values and redraws the windows
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void SetGeometryConf (void)
#else
static void SetGeometryConf ()
#endif /* __STDC__ */
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
#ifdef __STDC__
LRESULT CALLBACK WIN_GeometryDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam,
				      LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK WIN_GeometryDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndDlg; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
  switch (msg)
    {
    case WM_INITDIALOG:
      GeometryHwnd = hwnDlg;
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
#ifdef __STDC__
static void         GeometryCallbackDialog (int ref, int typedata, STRING data)
#else
static void         GeometryCallbackDialog (ref, typedata, data)
int                 ref;
int                 typedata;
STRING              data;

#endif
{
  int val;

  if (ref == -1)
    {
      /* removes the geometry conf menu */
      TtaDestroyDialogue (GeometryBase + GeometryMenu);
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
#ifdef __STDC__
void         GeometryConfMenu (Document document, View view)
#else
void         GeometryConfMenu (document, view)
Document            document;
View                view;
STRING              pathname;
#endif
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
  i += ustrlen (&s[i]) + 1;
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
  switch (app_lang)
    {
    case FR_LANG:
      DialogBox (hInstance, MAKEINTRESOURCE (FR_GEOMETRYMENU), NULL,
		 (DLGPROC) WIN_GeometryDlgProc);
      break;
    case DE_LANG:
      DialogBox (hInstance, MAKEINTRESOURCE (DE_GEOMETRYMENU), NULL,
		 (DLGPROC) WIN_GeometryDlgProc);
      break;
    default:
      DialogBox (hInstance, MAKEINTRESOURCE (EN_GEOMETRYMENU), NULL,
		 (DLGPROC) WIN_GeometryDlgProc);
      break;
    }
#endif /* !_WINDOWS */
}


/**********************
** LanNeg Menu
**********************/
/*----------------------------------------------------------------------
  GetLanNegConf
  Makes a copy of the current registry LanNeg values
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void GetLanNegConf (void)
#else
static void GetLanNegConf ()
#endif /* __STDC__ */
{
  GetEnvString ("ACCEPT_LANGUAGES", LanNeg);
}

/*----------------------------------------------------------------------
  GetDefaultLanNegConf
  Makes a copy of the default registry LanNeg values
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void GetDefaultLanNegConf (void)
#else
static void GetDefaultLanNegConf ()
#endif /* __STDC__ */
{
  GetDefEnvString ("ACCEPT_LANGUAGES", LanNeg);
}


/*----------------------------------------------------------------------
  SetLanNegConf
  Updates the registry LanNeg values
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void SetLanNegConf (void)
#else
static void SetLanNegConf ()
#endif /* __STDC__ */
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
#ifdef __STDC__
void WIN_RefreshLanNegMenu (HWND hwnDlg)
#else
void WIN_RefreshLanNegMenu (hwnDlg)
HWND hwnDlg;
#endif /* __STDC__ */
{
  SetDlgItemText (hwnDlg, IDC_LANNEG, LanNeg);
}

/*----------------------------------------------------------------------
  WIN_LanNegDlgProc
  Windows callback for the LanNeg menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK WIN_LanNegDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam,
				     LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK WIN_LanNegDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
  switch (msg)
    {
    case WM_INITDIALOG:
      LanNegHwnd = hwnDlg;
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
#ifdef __STDC__
static void RefreshLanNegMenu ()
#else
static void RefreshLanNegMenu ()
#endif /* __STDC__ */
{
  TtaSetTextForm (LanNegBase + mLanNeg, LanNeg);
}

/*----------------------------------------------------------------------
   callback of the LanNeg configuration menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         LanNegCallbackDialog (int ref, int typedata, STRING data)
#else
static void         LanNegCallbackDialog (ref, typedata, data)
int                 ref;
int                 typedata;
STRING              data;

#endif
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
	    ustrcpy (LanNeg, data);
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
#ifdef __STDC__
void         LanNegConfMenu (Document document, View view)
#else
void         LanNegConfMenu (document, view)
Document            document;
View                view;
STRING              pathname;

#endif
{
#ifndef _GTK
#ifndef _WINDOWS
   int              i;

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));
   i += ustrlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_DEFAULT_BUTTON));

   TtaNewSheet (LanNegBase + LanNegMenu, TtaGetViewFrame (document, view),
		TtaGetMessage (1, BConfigLanNeg), 2, s, TRUE, 1, 'L', D_DONE);
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
    {
      switch (app_lang)
	{
	case FR_LANG:
	  DialogBox (hInstance, MAKEINTRESOURCE (FR_LANNEGMENU), NULL, 
		     (DLGPROC) WIN_LanNegDlgProc);
	  break;
	case DE_LANG:
	  DialogBox (hInstance, MAKEINTRESOURCE (DE_LANNEGMENU), NULL, 
		     (DLGPROC) WIN_LanNegDlgProc);
	  break;
	default:
	  DialogBox (hInstance, MAKEINTRESOURCE (EN_LANNEGMENU), NULL, 
		     (DLGPROC) WIN_LanNegDlgProc);
	}
    }
   else
     SetFocus (LanNegHwnd);
#endif /* !_WINDOWS */
#endif /* _GTK */
}


/**********************
** Profile Menu
**********************/
/*----------------------------------------------------------------------
  GetProfileConf
  Makes a copy of the current registry Profile values
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void GetProfileConf (void)
#else
static void GetProfileConf ()
#endif /* __STDC__ */
{
  TtaGetProfileFileName (Profiles_File, MAX_LENGTH);
  GetEnvString ("Profile", Profile);
}

/*----------------------------------------------------------------------
  GetDefaultProfileConf
  Makes a copy of the default registry Profile values
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void GetDefaultProfileConf (void)
#else
static void GetDefaultProfileConf ()
#endif /* __STDC__ */
{
  TtaGetDefProfileFileName (Profiles_File, MAX_LENGTH);
  GetDefEnvString ("Profile", Profile);
}


/*----------------------------------------------------------------------
  SetProfileConf
  Updates the registry Profile values
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void SetProfileConf (void)
#else
static void SetProfileConf ()
#endif /* __STDC__ */
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
  CHAR_T*               ptr;
  int                   nbprofiles = 0;
  int                   i = 0;

  /* Get the propositions of the list */ 
  SendMessage (wndProfilesList, LB_RESETCONTENT, 0, 0);
  nbprofiles = TtaGetProfilesItems (MenuText, MAX_PRO);
  ptr = TtaGetEnvString ("Profile");
  /* normally ptr = ISO2WideChar (TtaGetEnvString ("Profile")); */
  while (i < nbprofiles && MenuText[i] != '\0')
    {
      /* keep in mind the current selected entry */
      if (ptr && !wc2iso_strcmp (ptr, MenuText[i]))
         CurrentProfile = i;
      SendMessage (wndProfilesList, LB_INSERTSTRING, i, (LPARAM) MenuText[i]);
      i++;
    }
}

/*----------------------------------------------------------------------
  WIN_RefreshProfileMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void WIN_RefreshProfileMenu (HWND hwnDlg)
#else
void WIN_RefreshProfileMenu (hwnDlg)
HWND hwnDlg;
#endif /* __STDC__ */
{		
  SetDlgItemText (hwnDlg, IDC_PROFILESLOCATION, Profiles_File);
  SetDlgItemText (hwnDlg, IDC_PROFILENAME, Profile);
  SendMessage (wndProfilesList, LB_RESETCONTENT, 0, 0);
}

/*----------------------------------------------------------------------
  WIN_ProfileDlgProc
  Windows callback for the Profile menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK WIN_ProfileDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam,
				     LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK WIN_ProfileDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;

#endif /* __STDC__ */
{

   int  itemIndex = 0;
   	

   switch (msg)
    {
    case WM_INITDIALOG:
      ProfileHwnd = hwnDlg; 
	       wndProfilesList = CreateWindow (TEXT("listbox"), NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD,
				  10, 90, 200, 90, hwnDlg, (HMENU) 1, 
				  (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
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
		  if (ustrlen(Profiles_File))
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
  int                   i;
  int                   nbprofiles = 0;
  int                   indx, length;
  STRING                ptr;
  STRING                entry;
  CHAR_T                BufMenu[MAX_LENGTH];

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
      if (ptr && !ustrcmp (ptr, entry))
	CurrentProfile = i;
       length = ustrlen (entry) + 1;
       if (length + indx < MAX_PRO * MAX_PRO_LENGTH)  
	 {
	   ustrcpy (&BufMenu[indx], entry);
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
#ifdef __STDC__
static void RefreshProfileMenu ()
#else
static void RefreshProfileMenu ()

#endif /* __STDC__ */
{
  TtaSetTextForm (ProfileBase + mProfiles_File, Profiles_File);
  TtaSetSelector (ProfileBase + mProfileSelector, CurrentProfile, NULL);

}

/*----------------------------------------------------------------------
   callback of the Profile configuration menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ProfileCallbackDialog (int ref, int typedata, STRING data)
#else
static void         ProfileCallbackDialog (ref, typedata, data)
int                 ref;
int                 typedata;
STRING              data;

#endif
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
	      ustrcpy (Profile, data);
	      RefreshProfileMenu();
	    }
	  else
	    Profile[0] = EOS;

	  break;

	case mProfiles_File:
	  if (data)
	    { 
	      /* did the profile file change ? */
	      if (ustrcmp (data, Profiles_File) !=0 ) 
		{
		   /* Yes, the profile file changed  : rescan the
		      profile definition file and display the new
		      profiles in the selector */
		  ustrcpy (Profiles_File, data);
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

#ifdef __STDC__
void         ProfileConfMenu (Document document, View view)
#else   /* __STDC__ */
void         ProfileConfMenu (document, view)
Document            document;
View                view;
STRING              pathname;
#endif   /* __STDC__ */
{
#ifndef _GTK
#ifndef _WINDOWS
   int                   i;
 
   /* load and display the current values */
   GetProfileConf ();

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));
   i += ustrlen (&s[i]) + 1;
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
     /* only activate the menu if it isn't active already */
     {
       switch (app_lang)
	 {
	 case FR_LANG:
	   DialogBox (hInstance, MAKEINTRESOURCE (FR_PROFILEMENU), NULL, 
		      (DLGPROC) WIN_ProfileDlgProc);
	   break;
	 case DE_LANG:
	   DialogBox (hInstance, MAKEINTRESOURCE (DE_PROFILEMENU), NULL, 
		      (DLGPROC) WIN_ProfileDlgProc);
	   break;
	 default:
	   DialogBox (hInstance, MAKEINTRESOURCE (EN_PROFILEMENU), NULL, 
		     (DLGPROC) WIN_ProfileDlgProc);
	 }
     }
   else
     SetFocus (ProfileHwnd);
#endif /* !_WINDOWS */
#endif /* _GTK */
}


/**********************
** Templates Menu
**********************/
/*----------------------------------------------------------------------
  GetTemplatesConf
  Makes a copy of the current registry Templates values
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void GetTemplatesConf (void)
#else
static void GetTemplatesConf ()
#endif /* __STDC__ */
{
  GetEnvString ("TEMPLATE_URL", TemplatesUrl);
}

/*----------------------------------------------------------------------
  GetDefaultTemplatesConf
  Makes a copy of the default registry Templates values
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void GetDefaultTemplatesConf (void)
#else
static void GetDefaultTemplatesConf ()
#endif /* __STDC__ */
{
  GetDefEnvString ("TEMPLATE_URL", TemplatesUrl);
}


/*----------------------------------------------------------------------
  SetTemplatesConf
  Updates the registry Templates values
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void SetTemplatesConf (void)
#else
static void SetTemplatesConf ()
#endif /* __STDC__ */
{
  TtaSetEnvString ("TEMPLATE_URL", TemplatesUrl, TRUE);
  TtaSaveAppRegistry ();
}

#ifdef _WINDOWS
/*----------------------------------------------------------------------
  WIN_RefreshTemplatesMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void WIN_RefreshTemplatesMenu (HWND hwnDlg)
#else
void WIN_RefreshTemplatesMenu (hwnDlg)
HWND hwnDlg;
#endif /* __STDC__ */
{
  SetDlgItemText (hwnDlg, IDC_TEMPLATESURL, TemplatesUrl);
}

/*----------------------------------------------------------------------
  WIN_TemplatesDlgProc
  Windows callback for the Templates menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK WIN_TemplatesDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam,
				     LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK WIN_TemplatesDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
  switch (msg)
    {
    case WM_INITDIALOG:
      TemplatesHwnd = hwnDlg;
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
#ifdef __STDC__
static void RefreshTemplatesMenu ()
#else
static void RefreshTemplatesMenu ()
#endif /* __STDC__ */
{
  TtaSetTextForm (TemplatesBase + mTemplates, TemplatesUrl);
}

/*----------------------------------------------------------------------
   callback of the Templates configuration menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         TemplatesCallbackDialog (int ref, int typedata, STRING data)
#else
static void         TemplatesCallbackDialog (ref, typedata, data)
int                 ref;
int                 typedata;
STRING              data;

#endif
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
	    ustrcpy (TemplatesUrl, data);
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
#ifdef __STDC__
void         TemplatesConfMenu (Document document, View view)
#else
void         TemplatesConfMenu (document, view)
Document            document;
View                view;
STRING              pathname;

#endif
{
#ifndef _GTK
#ifndef _WINDOWS
   int              i;

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));
   i += ustrlen (&s[i]) + 1;
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
    /* only activate the menu if it isn't active already */
    {
     switch (app_lang)
	{
	case FR_LANG:
	  DialogBox (hInstance, MAKEINTRESOURCE (FR_TEMPLATESMENU), NULL, 
		     (DLGPROC) WIN_TemplatesDlgProc);
	  break;
	case DE_LANG:
	  DialogBox (hInstance, MAKEINTRESOURCE (DE_TEMPLATESMENU), NULL, 
		     (DLGPROC) WIN_TemplatesDlgProc);
	 break;
	default:
		DialogBox (hInstance, MAKEINTRESOURCE (EN_TEMPLATESMENU), NULL, 
		     (DLGPROC) WIN_TemplatesDlgProc);
	}
    }
   else
     SetFocus (TemplatesHwnd);
#endif /* !_WINDOWS */
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   InitConfMenu: initialisation, called during Amaya initialisation
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitConfMenu (void)
#else
void                InitConfMenu ()
#endif /* __STDC__*/
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
#endif /* !_WINDOWS */
}








