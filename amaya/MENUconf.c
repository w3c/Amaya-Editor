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
 *
 * To do: remove the CACHE_RESTART option from some options, once we write
 * the code that should take it into account.
 */

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "MENUconf.h"
#include "print.h"
#include "init_f.h"
#ifndef AMAYA_JAVA
#include "query_f.h"
#endif
#ifdef _WINDOWS
#include "resource.h"
#include "wininclude.h"
extern HINSTANCE hInstance;
#endif /* _WINDOWS */

static int CacheStatus;
static int ProxyStatus;

/* Cache menu options */
#ifdef _WINDOWS
static HWND CacheHwnd = NULL;
#endif _WINDOWS
static int CacheBase;
static boolean EnableCache;
static boolean CacheProtectedDocs;
static boolean CacheDisconnectMode;
static boolean CacheExpireIgnore;
static CHAR_T CacheDirectory [MAX_LENGTH+1];
static int CacheSize;
static int MaxCacheFile;

/* Proxy menu options */
#ifdef _WINDOWS
static HWND ProxyHwnd = NULL;
boolean ColorMenuConf = FALSE;
#endif _WINDOWS
static int ProxyBase;
static CHAR_T HttpProxy [MAX_LENGTH+1];
static CHAR_T NoProxy [MAX_LENGTH+1];

/* General menu options */
#ifdef _WINDOWS
static CHAR_T AppHome [MAX_LENGTH+1];
static CHAR_T TmpDir [MAX_LENGTH+1];
static HWND GeneralHwnd = NULL;
#endif _WINDOWS
static int GeneralBase;
static int ToolTipDelay;
static int DoubleClickDelay;
static int Zoom;
static boolean Multikey;
static CHAR_T DefaultName [MAX_LENGTH+1];
static boolean BgImages;
static boolean DoubleClick;
static CHAR_T DialogueLang [MAX_LENGTH+1];
static int FontMenuSize;

/* Publish menu options */
#ifdef _WINDOWS
static HWND PublishHwnd =  NULL;
#endif _WINDOWS
static int PublishBase;
static boolean LostUpdateCheck;
static boolean VerifyPublish;
static CHAR_T HomePage [MAX_LENGTH+1];

/* Color menu options */
#ifdef _WINDOWS
static HWND ColorHwnd = NULL;
#endif _WINDOWS
static int ColorBase;
static CHAR_T FgColor [MAX_LENGTH+1];
static CHAR_T BgColor [MAX_LENGTH+1];
#ifndef _WINDOWS
static CHAR_T MenuFgColor [MAX_LENGTH+1];
static CHAR_T MenuBgColor [MAX_LENGTH+1];
#endif /* !_WINDOWS */

/* Geometry menu options */
static int GeometryBase;
static Document GeometryDoc = 0;
#ifdef _WINDOWS
HWND   GeometryHwnd = NULL;
#endif /* _WINDOWS */
/* common local variables */
CHAR_T s[MAX_LENGTH+1]; /* general purpose buffer */

/* 
** function prototypes
*/

#ifdef _WINDOWS
#ifdef __STDC__
LRESULT CALLBACK WIN_GeneralDlgProc (HWND, UINT, WPARAM, LPARAM);
static void WIN_RefreshGeneralMenu (HWND hwnDlg);
LRESULT CALLBACK WIN_CacheDlgProc (HWND, UINT, WPARAM, LPARAM);
static void WIN_RefreshCacheMenu (HWND hwnDlg);
LRESULT CALLBACK WIN_ProxyDlgProc (HWND, UINT, WPARAM, LPARAM);
static void WIN_RefreshProxyMenu (HWND hwnDlg);
LRESULT CALLBACK WIN_PublishDlgProc (HWND, UINT, WPARAM, LPARAM);
static void WIN_RefreshPublishMenu (HWND hwnDlg);
LRESULT CALLBACK WIN_GeometryDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WIN_ColorDlgProc (HWND, UINT, WPARAM, LPARAM);
static void WIN_RefreshColorMenu (HWND hwnDlg);
#else
LRESULT CALLBACK WIN_GeneralDlgProc (HWND, UINT, WPARAM, LPARAM);
static void WIN_RefreshGeneralMenu (/* HWND hwnDlg */);
LRESULT CALLBACK WIN_CacheDlgProc (HWND, UINT, WPARAM, LPARAM);
static void WIN_RefreshCacheMenu (/* HWND hwnDlg */);
LRESULT CALLBACK WIN_ProxyDlgProc (HWND, UINT, WPARAM, LPARAM);
static void WIN_RefreshProxyMenu (/* HWND hwnDlg */);
LRESULT CALLBACK WIN_PublishDlgProc (HWND, UINT, WPARAM, LPARAM);
static void WIN_RefreshPublishMenu (/* HWND hwnDlg */);
LRESULT CALLBACK WIN_GeometryDlgProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WIN_ColorDlgProc (HWND, UINT, WPARAM, LPARAM);
static void WIN_RefreshColorMenu (/* HWND hwnDlg */);
#endif /* __STDC__ */
#endif /* _WINDOWS */

#ifdef __STDC__
static void         GetEnvString (const STRING name, STRING value);
static void         GetDefEnvToggle (const STRING name, boolean *value, int ref, int entry);
static void         GetDefEnvString (const STRING name, STRING value);
#ifndef _WINDOWS
static void         CacheCallbackDialog (int ref, int typedata, STRING data);
static void         RefreshCacheMenu (void);
#endif /* !_WINDOWS */
static void         GetCacheConf (void);
static void         GetDefaultCacheConf (void);
static void         SetCacheConf (void);
#ifndef _WINDOWS
static void         ProxyCallbackDialog(int ref, int typedata, STRING data);
static void         RefreshProxyMenu (void);
#endif /* !_WINDOWS */
static void         GetProxyConf (void);
static void         GetDefaultProxyConf (void);
static void         SetProxyConf (void);
#ifndef _WINDOWS
static void	    GeneralCallbackDialog(int ref, int typedata, STRING data);
static void         RefreshGeneralMenu (void);
#endif /* !_WINDOWS */
static void         GetGeneralConf (void);
static void         GetDefaultGeneralConf (void);
static void         SetGeneralConf (void);
#ifndef _WINDOWS
static void	    PublishCallbackDialog(int ref, int typedata, STRING data);
static void         RefreshPublishMenu (void);
#endif /* !_WINDOWS */
static void         GetPublishConf (void);
static void         GetDefaultPublishConf (void);
static void         SetPublishConf (void);
#ifndef _WINDOWS
static void         ColorCallbackDialog(int ref, int typedata, STRING data);
static void         RefreshColorMenu (void);
#endif /* !_WINDOWS */
static void         GetColorConf (void);
static void         GetDefaultColorConf (void);
static void         SetColorConf (void);
#ifndef _WINDOWS
static void         GeometryCallbackDialog(int ref, int typedata, STRING data);
#endif /* !_WINDOWS */
static void         RestoreDefaultGeometryConf (void);
static void         SetGeometryConf (void);
#else
static void         GetEnvString (/* const STRING name, STRING value */);
static void         GetDefEnvToggle (/* const STRING name, boolean *value, int ref, int entry */);
static void         GetDefEnvString (/* const STRING name, STRING value */);
#ifndef _WINDOWS
static void         CacheCallbackDialog (/* int ref, int typedata, STRING data */);
static void         RefreshCacheMenu (/* void */);
#endif /* !_WINDOWS */
static void         GetCacheConf (/* void */);
static void         GetDefaultCacheConf (/* void */);
static void         SetCacheConf (/* void */);
#ifndef _WINDOWS
static void         ProxyCallbackDialog(/* int ref, int typedata, STRING data */);
static void         RefreshProxyMenu (/* void */);
#endif /* !_WINDOWS */
static void         GetProxyConf (/* void */);
static void         GetDefaultProxyConf (/* void */);
static void         SetProxyConf (/* void */);
#ifndef _WINDOWS
static void	    GeneralCallbackDialog(/*int ref, int typedata, STRING data*/);
static void         RefreshGeneralMenu (/* void */);
#endif /* !_WINDOWS */
static void         GetGeneralConf (/* void */);
static void         GetDefaultGeneralConf (/* void */);
static void         SetGeneralConf (/* void */);
#ifndef _WINDOWS
static void	    PublishCallbackDialog(/*int ref, int typedata, STRING data*/);
static void         RefreshPublishMenu (/* void */);
#endif /* !_WINDOWS */
static void         GetPublishConf (/* void */);
static void         GetDefaultPublishConf (/* void */);
static void         SetPublishConf (/* void */);
#ifndef _WINDOWS
static void         ColorCallbackDialog(/* int ref, int typedata, STRING data */);
static void         RefreshColorMenu (/* void */);
#endif /* !_WINDOWS */
static void         GetColorConf (/* void */);
static void         GetDefaultColorConf (/* void */);
static void         SetColorConf (/* void */);
#ifndef _WINDOWS
static void         GeometryCallbackDialog(/* int ref, int typedata, STRING data */);
#endif /* !_WINDOWS */
static void         RestoreDefaultGeometryConf (/* void */);
static void         SetGeometryConf (/* void */);
#endif

/*
** Common functions
*/

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
  STRING s;

  /* browsing editing options */
  s = TtaGetEnvString ("THOTDIR");
  if (s != NULL)
    {
      ustrcpy (HomePage, s);
      ustrcat (HomePage, AMAYA_PAGE);
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
#ifndef _WINDOWS
  TtaSetDefEnvString ("THOTPRINT", "lpr", FALSE);
  /* A4 size */
  TtaSetDefEnvString ("PAPERSIZE", "0", FALSE);
#endif
  /* network configuration */
  TtaSetDefEnvString ("ENABLE_LOST_UPDATE_CHECK", "yes", FALSE);
  TtaSetDefEnvString ("ENABLE_PIPELINING", "yes", FALSE);
  TtaSetDefEnvString ("NET_EVENT_TIMEOUT", "60000", FALSE);
  TtaSetDefEnvString ("PERSIST_CX_TIMEOUT", "60", FALSE);
  TtaSetDefEnvString ("DNS_TIMEOUT", "1800", FALSE);
  TtaSetDefEnvString ("MAX_SOCKET", "32", FALSE);
  TtaSetDefEnvString ("ENABLE_MDA", "yes", FALSE);
  TtaSetDefEnvString ("NO_PROXY", "", FALSE);
  TtaSetDefEnvString ("HTTP_PROXY", "", FALSE);
  TtaSetDefEnvString ("MAX_CACHE_ENTRY_SIZE", "3", FALSE);
  TtaSetDefEnvString ("CACHE_SIZE", "10", FALSE);
  if (TempFileDirectory)
    TtaSetDefEnvString ("CACHE_DIR", TempFileDirectory, FALSE);
  else
    TtaSetDefEnvString ("CACHE_DIR", "", FALSE);
  TtaSetDefEnvString ("CACHE_PROTECTED_DOCS", "yes", FALSE);
  TtaSetDefEnvString ("ENABLE_CACHE", "yes", FALSE);
  TtaSetDefEnvString ("CACHE_DISCONNECTED_MODE", "no", FALSE);
  TtaSetDefEnvString ("CACHE_EXPIRE_IGNORE", "no", FALSE);
  /* appearance */

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
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
   GetEnvString: front end to TtaGetEnvString. If the variable name doesn't
   exist, it sets the value to an empty ("") string
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void GetEnvString (const STRING name, STRING value)
#else
static void GetEnvString (name, value)
const STRING name;
STRING value;
#endif /* __STDC__ */
{
  CHAR_T *ptr;
  ptr = TtaGetEnvString (name);
  if (ptr) {
    ustrncpy (value, ptr, MAX_LENGTH);
    value[MAX_LENGTH] = EOS;
  }
  else
    value[0] = EOS;
}

/*----------------------------------------------------------------------
   GetDefEnvToggleBoolean: front end to TtaGetDefEnvBoolean. It takes
   care of switching the toggle button according to the status of the
   variable.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void GetDefEnvToggle (const STRING name, boolean *value, int ref, int entry)
#else
static void GetDefEnvToggle (name, value, ref, entry)
const STRING name;
boolean *value;
int ref;
int entry;
#endif /* __STDC__ */
{
  boolean old = *value;

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
static void GetDefEnvString (const STRING name, STRING value)
#else
static void GetDefEnvString (name, value)
const STRING name;
STRING value;
#endif /* __STDC__ */
{
  CHAR_T *ptr;

  ptr = TtaGetDefEnvString (name);
  if (ptr) {
    ustrncpy (value, ptr, MAX_LENGTH);
    value[MAX_LENGTH] = EOS;
  }
  else
    value[0] = EOS;
}

/*********************
** Cache configuration menu
***********************/

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
	  SetCacheConf ();
	  libwww_updateNetworkConf (CacheStatus);
	  CacheStatus = 0;
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
#endif /* _WINDOWS */

#ifndef _WINDOWS
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
	      SetCacheConf ();
#ifdef AMAYA_JAVA
#else      
	      libwww_updateNetworkConf (CacheStatus);
#endif /* !AMAYA_JAVA */
	      /* reset the status flag */
	      CacheStatus = 0;
	      break;
	    case 2:
	      GetDefaultCacheConf ();
	      RefreshCacheMenu ();
	      /* always signal this as modified */
	      CacheStatus |= AMAYA_CACHE_RESTART;
	      break;
	    case 3:
#if defined(AMAYA_JAVA) || defined(AMAYA_ILU)
#else
	      /* @@ docid isn't used! */
	      StopAllRequests (1);
	      libwww_CleanCache ();
#endif /* AMAYA_JAVA */
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
  TtaGetEnvBoolean 
    ("CACHE_PROTECTED_DOCS", &CacheProtectedDocs);
  TtaGetEnvBoolean 
    ("CACHE_DISCONNECTED_MODE", &CacheDisconnectMode);
  TtaGetEnvBoolean ("CACHE_EXPIRE_IGNORE", &CacheExpireIgnore);
  GetEnvString ("CACHE_DIR", CacheDirectory);
  TtaGetEnvInt ("CACHE_SIZE", &CacheSize);
  TtaGetEnvInt ("MAX_CACHE_ENTRY_SIZE", &MaxCacheFile);
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
  TtaSetEnvBoolean ("CACHE_PROTECTED_DOCS", 
		    CacheProtectedDocs, TRUE);
  TtaSetEnvBoolean ("CACHE_DISCONNECTED_MODE", 
		    CacheDisconnectMode, TRUE);
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
  GetDefEnvToggle ("ENABLE_CACHE", &EnableCache, 
		    CacheBase + mCacheOptions, 0);
  GetDefEnvToggle 
    ("CACHE_PROTECTED_DOCS", &CacheProtectedDocs,
     CacheBase + mCacheOptions, 1);
  GetDefEnvToggle 
    ("CACHE_DISCONNECTED_MODE", &CacheDisconnectMode,
     CacheBase + mCacheOptions, 2);
  GetDefEnvToggle ("CACHE_EXPIRE_IGNORE", &CacheExpireIgnore, 
		   CacheBase + mCacheOptions, 3);
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
#endif /* WINDOWS */

#ifndef _WINDOWS
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

/*----------------------------------------------------------------------
  CacheConfMenu
  Build and display the Conf Menu dialog box and prepare for input.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         CacheConfMenu (Document document, View view)
#else
void         CacheConfMenu (document, view)
Document            document;
View                view;
#endif
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
		   20,
		   1,
		   TRUE);
   TtaNewNumberForm (CacheBase + mCacheSize,
		     CacheBase + CacheMenu,
		     TtaGetMessage (AMAYA, AM_CACHE_SIZE),
		     0,
		     100,
		     TRUE);
   TtaNewNumberForm (CacheBase + mMaxCacheFile,
		     CacheBase + CacheMenu,
		     TtaGetMessage (AMAYA, AM_CACHE_ENTRY_SIZE),
		     0,
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
	    case IDC_NOPROXY:
	      GetDlgItemText (hwnDlg, IDC_NOPROXY, NoProxy,
			      sizeof (NoProxy) - 1);
	      ProxyStatus |= AMAYA_PROXY_RESTART;
	      break;
	    }
	}
      switch (LOWORD (wParam))
	{
	  /* action buttons */
	case ID_APPLY:
	  SetProxyConf ();	  
	  libwww_updateNetworkConf (ProxyStatus);
	  /* reset the status flag */
	  ProxyStatus = 0;
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
#endif /* _WINDOWS */

#ifndef _WINDOWS
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
#ifdef AMAYA_JAVA
#else      
	      libwww_updateNetworkConf (ProxyStatus);
#endif /* !AMAYA_JAVA */
	      /* reset the status flag */
	      ProxyStatus = 0;
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

	case mNoProxy:
	  ProxyStatus |= AMAYA_PROXY_RESTART;
	  if (data)
	    ustrcpy (NoProxy, data);
	  else
	    NoProxy [0] = EOS;
	  break;

	default:
	  break;
	}
    }
}
#endif /* !_WINDOWS */

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
  GetEnvString ("NO_PROXY", NoProxy);
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
  TtaSetEnvString ("NO_PROXY", NoProxy, TRUE);

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
  GetDefEnvString ("NO_PROXY", NoProxy);
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
  SetDlgItemText (hwnDlg, IDC_NOPROXY, NoProxy);
}
#endif /* WINDOWS */

#ifndef _WINDOWS
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
  TtaSetTextForm (ProxyBase + mNoProxy, NoProxy);
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
   TtaNewTextForm (ProxyBase + mNoProxy,
		   ProxyBase + ProxyMenu,
		   TtaGetMessage (AMAYA, AM_NO_PROXY),
		   20,
		   1,
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
		  GetDlgItemText (hwnDlg, IDC_TMPDIR, TmpDir,
			       sizeof (TmpDir) - 1);
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
	  SetGeneralConf ();	  
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
#endif /* _WINDOWS */

#ifndef _WINDOWS
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
	      SetGeneralConf ();
	      break;
	    case 2:
	      GetDefaultGeneralConf ();
	      RefreshGeneralMenu ();
	      break;
	    default:
	      break;
	    }
	  break;

	case mToolTipDelay:
	  ToolTipDelay = val;
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
  GetGeneralConf
  Makes a copy of the current registry General values
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void GetGeneralConf (void)
#else
static void GetGeneralConf ()
#endif /* __STDC__ */
{
  TtaGetEnvInt ("TOOLTIPDELAY", &ToolTipDelay);
  TtaGetEnvInt ("DOUBLECLICKDELAY", &DoubleClickDelay);
  TtaGetEnvInt ("ZOOM", &Zoom);
  TtaGetEnvBoolean ("ENABLE_MULTIKEY", &Multikey);
  TtaGetEnvBoolean ("ENABLE_BG_IMAGES", &BgImages);
  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &DoubleClick);
  GetEnvString ("HOME_PAGE", HomePage);
  GetEnvString ("LANG", DialogueLang);
  TtaGetEnvInt ("FontMenuSize", &FontMenuSize);
#ifdef _WINDOWS
  GetEnvString ("TMPDIR", TmpDir);
  GetEnvString ("APP_HOME", AppHome);
#endif /* _WINDOWS */
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

  TtaSetEnvInt ("TOOLTIPDELAY", ToolTipDelay, TRUE);
  TtaSetEnvInt ("DOUBLECLICKDELAY", DoubleClickDelay, TRUE);
  TtaGetEnvInt ("ZOOM", &oldZoom);
  if (oldZoom != Zoom)
    {
      TtaSetEnvInt ("ZOOM", Zoom, TRUE);
      /* recalibrate the zoom settings in all the active documents */
      GotoZoom (Zoom - oldZoom);
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
  TtaSetEnvString ("TMPDIR", TmpDir, TRUE);
  TtaSetEnvString ("APP_HOME", AppHome, TRUE);
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
  TtaGetDefEnvInt ("TOOLTIPDELAY", &ToolTipDelay);
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
  GetDefEnvString ("TMPDIR", TmpDir);
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
  SetDlgItemInt (hwnDlg, IDC_ZOOM, Zoom, FALSE);
  SetDlgItemText (hwnDlg, IDC_TMPDIR, TmpDir);
  SetDlgItemText (hwnDlg, IDC_APPHOME, AppHome);
}
#endif _WINDOWS

#ifndef _WINDOWS
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
  TtaSetNumberForm (GeneralBase + mToolTipDelay, ToolTipDelay);
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
   i += strlen (&s[i]) + 1;
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
   TtaNewNumberForm (GeneralBase + mToolTipDelay,
		     GeneralBase + GeneralMenu,
		     TtaGetMessage (AMAYA, AM_TOOLTIP_DELAY),
		     0,
		     65000,
		     FALSE);   
   TtaNewNumberForm (GeneralBase + mDoubleClickDelay,
		     GeneralBase + GeneralMenu,
		     TtaGetMessage (AMAYA, AM_DOUBLECLICK_DELAY),
		     0,
		     65000,
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
   /* fourth line */
   TtaNewTextForm (GeneralBase + mDialogueLang,
		   GeneralBase + GeneralMenu,
		   TtaGetMessage (AMAYA, AM_DIALOGUE_LANGUAGE),
		   10,
		   1,
		   FALSE);
   TtaNewLabel (GeneralBase + mGeneralEmpty2, GeneralBase + GeneralMenu, " ");   
   /* fifth line */
   sprintf (s, "B%s%cB%s%cB%s", 
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
	  /* reset the status flag */
	  break;
	case ID_DONE:
	  /* reset the status flag */
	  PublishHwnd = NULL;
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DEFAULTS:
	  /* always signal this as modified */
	  GetDefaultPublishConf ();
	  WIN_RefreshPublishMenu (hwnDlg);
	  break;
	}
      break;	     
    default: return FALSE;
    }
  return TRUE;
}
#endif /* _WINDOWS */

#ifndef _WINDOWS
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
	      break;
	    case 2:
	      GetDefaultPublishConf ();
	      RefreshPublishMenu ();
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
	    DefaultName [0] = EOS;
	  break;

	default:
	  break;
	}
    }
}
#endif /* !_WINDOWS */

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
}
#endif /* WINDOWS */

#ifndef _WINDOWS
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
#endif /* !_WINDOWS */
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
  switch (msg)
    {
    case WM_INITDIALOG:
      ColorHwnd = hwnDlg;
      WIN_RefreshColorMenu (hwnDlg);
      ColorMenuConf = TRUE;
      break;

    case WM_CLOSE:
    case WM_DESTROY:
      /* reset the status flag */
      ColorHwnd = NULL;
      EndDialog (hwnDlg, ID_DONE);
      break;

    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	  
	  /* action buttons */
    case IDC_CHANGCOLOR:
         ThotCreatePalette (200, 200);
         break;

	case ID_APPLY:
	  SetColorConf ();	  
	  /* reset the status flag */
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
#endif /* _WINDOWS */

#ifndef _WINDOWS
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
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_DEFAULT_BUTTON));

   TtaNewSheet (ColorBase + ColorMenu, 
		TtaGetViewFrame (document, view),
		TtaGetMessage (AMAYA, AM_COLOR_MENU),
		2, s, TRUE, 2, 'L', D_DONE);
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
#endif /* WINDOWS */

#ifndef _WINDOWS
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


/**********************
** Geometry Menu
**********************/

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
#endif /* _WINDOWS */

#ifndef _WINDOWS
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

/*----------------------------------------------------------------------
  RestoreDefEnvGeom
  Restores the default integer geometry values that are stored in a 
  registry entry under the form "x y w h"
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void RestoreDefEnvGeom (STRING env_var)
#else
static void RestoreDefEnvGeom (env_var
STRING env_var;
#endif /* _STDC_ */
{
  int x, y, w, h;

  /* in order to read the default values from HTML.conf, we erase the 
     registry entry */
  TtaClearEnvString (env_var);
  TtaGetViewGeometryMM (GeometryDoc, env_var, &x, &y, &w, &h);
  sprintf (s, "%d %d %d %d", 
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
static void SetEnvGeom (STRING view_name)
#else
static void SetEnvGeom (view_name)
STRING view_name
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
      sprintf (s, "%d %d %d %d", 
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
#ifdef MATHML
      SetEnvGeom ("Math_Structure_view");
#endif /* MATHML */
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









