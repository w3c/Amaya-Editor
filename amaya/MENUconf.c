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

#define MAX_GEOMETRY_LENGTH 24

static int CacheStatus;
static int ProxyStatus;

/* Cache menu options */
static int CacheBase;
static boolean EnableCache;
static boolean CacheProtectedDocs;
static boolean CacheDisconnectMode;
static boolean CacheExpireIgnore;
static CHAR CacheDirectory [MAX_LENGTH+1];
static int CacheSize;
static int MaxCacheFile;

/* Proxy menu options */
static int ProxyBase;
static CHAR HttpProxy [MAX_LENGTH+1];
static CHAR NoProxy [MAX_LENGTH+1];

/* General menu options */
static int GeneralBase;
static int ToolTipDelay;
static int DoubleClickDelay;
static int Zoom;
static boolean Multikey;
static CHAR ThotPrint [MAX_LENGTH+1];
static CHAR DefaultName [MAX_LENGTH+1];
static boolean BgImages;
static boolean DoubleClick;
static CHAR DialogueLang [MAX_LENGTH+1];
static int FontMenuSize;

/* Publish menu options */
static int PublishBase;
static boolean LostUpdateCheck;
static boolean VerifyPublish;
static CHAR HomePage [MAX_LENGTH+1];

/* Color menu options */
static int ColorBase;
static CHAR ForegroundColor [MAX_LENGTH+1];

/* Geometry menu options */
static int GeometryBase;
static CHAR FormattedView [MAX_GEOMETRY_LENGTH+1];
static CHAR StructureView [MAX_GEOMETRY_LENGTH+1];
static CHAR MathStructureView [MAX_GEOMETRY_LENGTH+1];
static CHAR GraphStructureView [MAX_GEOMETRY_LENGTH+1];
static CHAR AlternateView [MAX_GEOMETRY_LENGTH+1];
static CHAR LinksView [MAX_GEOMETRY_LENGTH+1];
static CHAR TableOfContentsView [MAX_GEOMETRY_LENGTH+1];

/* common local variables */
CHAR s[300]; /* general purpose buffer */

#ifdef __STDC__
static void         GetEnvString (const STRING name, STRING value);
static void         GetDefEnvToggle (const STRING name, boolean *value, int ref, int entry);
static void         GetDefEnvString (const STRING name, STRING value);
static void         CacheCallbackDialog (int ref, int typedata, STRING data);
static void         RefreshCacheMenu (void);
static void         GetCacheConf (void);
static void         GetDefaultCacheConf (void);
static void         SetCacheConf (void);
static void         ProxyCallbackDialog(int ref, int typedata, STRING data);
static void         RefreshProxyMenu (void);
static void         GetProxyConf (void);
static void         GetDefaultProxyConf (void);
static void         SetProxyConf (void);
static void	    GeneralCallbackDialog(int ref, int typedata, STRING data);
static void         RefreshGeneralMenu (void);
static void         GetGeneralConf (void);
static void         GetDefaultGeneralConf (void);
static void         SetGeneralConf (void);
static void	    PublishCallbackDialog(int ref, int typedata, STRING data);
static void         RefreshPublishMenu (void);
static void         GetPublishConf (void);
static void         GetDefaultPublishConf (void);
static void         SetPublishConf (void);
static void         ColorCallbackDialog(int ref, int typedata, STRING data);
static void         RefreshColorMenu (void);
static void         GetColorConf (void);
static void         GetDefaultColorConf (void);
static void         SetColorConf (void);
static void         GeometryCallbackDialog(int ref, int typedata, STRING data);
static void         RefreshGeometryMenu (void);
static void         GetGeometryConf (void);
static void         GetDefaultGeometryConf (void);
static void         SetGeometryConf (void);
#else
static void         GetEnvString (/* const STRING name, STRING value */);
static void         GetDefEnvToggle (/* const STRING name, boolean *value, int ref, int entry */);
static void         GetDefEnvString (/* const STRING name, STRING value */);
static void         CacheCallbackDialog (/* int ref, int typedata, STRING data */);
static void         RefreshCacheMenu (/* void */);
static void         GetCacheConf (/* void */);
static void         GetDefaultCacheConf (/* void */);
static void         SetCacheConf (/* void */);
static void         ProxyCallbackDialog(/* int ref, int typedata, STRING data */);
static void         RefreshProxyMenu (/* void */);
static void         GetProxyConf (/* void */);
static void         GetDefaultProxyConf (/* void */);
static void         SetProxyConf (/* void */);
static void	    GeneralCallbackDialog(/*int ref, int typedata, STRING data*/);
static void         RefreshGeneralMenu (/* void */);
static void         GetGeneralConf (/* void */);
static void         GetDefaultGeneralConf (/* void */);
static void         SetGeneralConf (/* void */);
static void	    PublishCallbackDialog(/*int ref, int typedata, STRING data*/);
static void         RefreshPublishMenu (/* void */);
static void         GetPublishConf (/* void */);
static void         GetDefaultPublishConf (/* void */);
static void         SetPublishConf (/* void */);
static void         ColorCallbackDialog(/* int ref, int typedata, STRING data */);
static void         RefreshColorMenu (/* void */);
static void         GetColorConf (/* void */);
static void         GetDefaultColorConf (/* void */);
static void         SetColorConf (/* void */);
static void         GeometryCallbackDialog(/* int ref, int typedata, STRING data */);
static void         RefreshGeometryMenu (/* void */);
static void         GetGeometryConf (/* void */);
static void         GetDefaultGeometryConf (/* void */);
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
  s = (STRING) TtaGetEnvString ("THOTDIR");
  if (s != NULL)
    {
      ustrcpy (HomePage, s);
      ustrcat (HomePage, AMAYA_PAGE);
    }
  else
    HomePage[0]  = EOS;
  TtaSetDefEnvString ("HOME_PAGE", HomePage, FALSE);
  HomePage[0] = EOS;
  /*** @@@ don't add it yet!
  TtaSetDefEnvString ("TMPDIR", "", FALSE);
  ***/
  TtaSetDefEnvString ("ENABLE_MULTIKEY", "no", FALSE);
  TtaSetDefEnvString ("ENABLE_BG_IMAGES", "yes", FALSE);
  TtaSetDefEnvString ("VERIFY_PUBLISH", "no", FALSE);
  TtaSetDefEnvString ("ENABLE_LOST_UPDATE_CHECK", "yes", FALSE);
  TtaSetDefEnvString ("DEFAULTNAME", "Overview.html", FALSE);
  TtaSetDefEnvString ("FontMenuSize", "12", FALSE);

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
  CacheBase = TtaSetCallback (CacheCallbackDialog, MAX_CACHEMENU_DLG);
  ProxyBase = TtaSetCallback (ProxyCallbackDialog, MAX_PROXYMENU_DLG);
  GeneralBase = TtaSetCallback (GeneralCallbackDialog, MAX_GENERALMENU_DLG);
  PublishBase = TtaSetCallback (PublishCallbackDialog, MAX_PUBLISHMENU_DLG);
  ColorBase = TtaSetCallback (ColorCallbackDialog,
			      MAX_COLORMENU_DLG);
  GeometryBase = TtaSetCallback (GeometryCallbackDialog,
				 MAX_GEOMETRYMENU_DLG);
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
  CHAR *ptr;
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
  CHAR *ptr;
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
  /* verify what happens when the option is NULL */

  /* set the menu entries to the current values */
#ifndef _WINDOWS
  TtaSetToggleMenu (CacheBase + mCacheOptions, 0, EnableCache);
  TtaSetToggleMenu (CacheBase + mCacheOptions, 1, CacheProtectedDocs);
  TtaSetToggleMenu (CacheBase + mCacheOptions, 2, CacheDisconnectMode);
  TtaSetToggleMenu (CacheBase + mCacheOptions, 3, CacheExpireIgnore);
#endif /* _WINDOWS */
  if (CacheDirectory)
    TtaSetTextForm (CacheBase + mCacheDirectory, CacheDirectory);
  TtaSetNumberForm (CacheBase + mCacheSize, CacheSize);
  TtaSetNumberForm (CacheBase + mMaxCacheFile, MaxCacheFile);
}

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
   int              i;

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], "Apply");
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], "Defaults");
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], "Flush Cache");
   TtaNewSheet (CacheBase + CacheMenu, 
		TtaGetViewFrame (document, view),
	       "Cache Configuration", 3, s, FALSE, 6, 'L', D_DONE);

   sprintf (s, "%s%c%s%c%s%c%s", "BEnable cache", EOS, 
	    "BCache protected documents", EOS,
	    "BDisconnected mode", EOS,
	    "BIgnore Expires: header");
   TtaNewToggleMenu (CacheBase + mCacheOptions,
		     CacheBase + CacheMenu,
		     NULL,
		     4,
		     s,
		     NULL,
		     TRUE);
   TtaNewTextForm (CacheBase + mCacheDirectory,
		   CacheBase + CacheMenu,
		   "Cache directory",
		   20,
		   1,
		   TRUE);
   TtaNewNumberForm (CacheBase + mCacheSize,
		     CacheBase + CacheMenu,
		     "Cache size (Mb)",
		     0,
		     100,
		     TRUE);
   TtaNewNumberForm (CacheBase + mMaxCacheFile,
		     CacheBase + CacheMenu,
		     "Cache entry size limit (Mb)",
		     0,
		     5,
		     TRUE);
   /* load and display the current values */
   GetCacheConf ();
   RefreshCacheMenu ();
   /* clean the modified flags */
   CacheStatus = 0;
  /* display the menu */
  TtaShowDialogue (CacheBase + CacheMenu, TRUE);
}

/*********************
** Proxy configuration menu
***********************/

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
   int              i;

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], "Apply");
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], "Defaults");

   TtaNewSheet (ProxyBase + ProxyMenu, 
		TtaGetViewFrame (document, view),
	       "Proxy Configuration", 2, s, FALSE, 6, 'L', D_DONE);

   TtaNewTextForm (ProxyBase + mHttpProxy,
		   ProxyBase + ProxyMenu,
		   "HTTP proxy",
		   20,
		   1,
		   TRUE);
   TtaNewTextForm (ProxyBase + mNoProxy,
		   ProxyBase + ProxyMenu,
		   "No proxy on these domains",
		   20,
		   1,
		   TRUE);

   /* load and display the current values */
   GetProxyConf ();
   RefreshProxyMenu ();
   /* clean the modified flags */
   ProxyStatus = 0;
  /* display the menu */
  TtaShowDialogue (ProxyBase + ProxyMenu, TRUE);
}

/**********************
** General configuration menu
***********************/

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

	case mThotPrint:
	  if (data)
	    ustrcpy (ThotPrint, data);
	  else
	    ThotPrint [0] = EOS;
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
  GetEnvString ("THOTPRINT", ThotPrint);
  GetEnvString ("LANG", DialogueLang);
  TtaGetEnvInt ("FontMenuSize", &FontMenuSize);

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
  int i;

  TtaSetEnvInt ("TOOLTIPDELAY", ToolTipDelay, TRUE);
  TtaSetEnvInt ("DOUBLECLICKDELAY", DoubleClickDelay, TRUE);
  TtaSetEnvInt ("ZOOM", Zoom, TRUE);
  /* recalibrate the zoom settings in all the active documents */
  for (i = 0; i < DocumentTableLength -1; i++)
    {
      if (DocumentURLs[i])
	RecalibrateZoom (i, 1);
    }
  TtaSetEnvBoolean ("ENABLE_MULTIKEY", Multikey, TRUE);
  TtaSetMultikey (Multikey);
  TtaSetEnvBoolean ("ENABLE_BG_IMAGES", BgImages, TRUE);
  TtaSetEnvBoolean ("ENABLE_DOUBLECLICK", DoubleClick, TRUE);
  TtaSetEnvString ("HOME_PAGE", HomePage, TRUE);
  TtaSetEnvString ("THOTPRINT", ThotPrint, TRUE);
  TtaSetEnvString ("LANG", DialogueLang, TRUE);
  TtaSetEnvInt ("FontMenuSize", FontMenuSize, TRUE);
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
  GetDefEnvString ("THOTPRINT", ThotPrint);
  GetDefEnvString ("LANG", DialogueLang);
  TtaGetDefEnvInt ("FontMenuSize", &FontMenuSize);

}

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
#ifndef _WINDOWS
  TtaSetToggleMenu (GeneralBase + mToggleGeneral, 0, Multikey);
  TtaSetToggleMenu (GeneralBase + mToggleGeneral, 1, BgImages);
  TtaSetToggleMenu (GeneralBase + mToggleGeneral, 2, DoubleClick);
#endif /* _WINDOWS */
  TtaSetTextForm (GeneralBase + mHomePage, HomePage);
  TtaSetTextForm (GeneralBase + mThotPrint, ThotPrint);
  TtaSetTextForm (GeneralBase + mDialogueLang, DialogueLang);
  TtaSetNumberForm (GeneralBase + mFontMenuSize, FontMenuSize);
}

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
   CHAR             s[MAX_LENGTH];
   int              i;

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], "Apply");
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], "Defaults");

   TtaNewSheet (GeneralBase + GeneralMenu, 
		TtaGetViewFrame (document, view),
	       "General Configuration", 2, s, TRUE, 2, 'L', D_DONE);
   /* first line */
   TtaNewTextForm (GeneralBase + mHomePage,
		   GeneralBase + GeneralMenu,
		   "Home Page",
		   40,
		   1,
		   FALSE);
   TtaNewLabel (GeneralBase + mGeneralEmpty1, GeneralBase + GeneralMenu, "");
   /* second line */
   TtaNewNumberForm (GeneralBase + mToolTipDelay,
		     GeneralBase + GeneralMenu,
		     "ToolTip delay (ms)",
		     0,
		     65000,
		     FALSE);   
   TtaNewNumberForm (GeneralBase + mDoubleClickDelay,
		     GeneralBase + GeneralMenu,
		     "Double Click delay (ms)",
		     0,
		     65000,
		     FALSE);   
   /* third line */
   TtaNewNumberForm (GeneralBase + mFontMenuSize,
		     GeneralBase + GeneralMenu,
		     "Menu font size",
		     8,
		     20,
		     FALSE);   

   TtaNewNumberForm (GeneralBase + mZoom,
		     GeneralBase + GeneralMenu,
		     "Zoom",
		     0,
		     10,
		     FALSE);   
   /* fourth line */
   TtaNewTextForm (GeneralBase + mThotPrint,
		   GeneralBase + GeneralMenu,
		   "Printer command",
		   20,
		   1,
		   FALSE);
   TtaNewLabel (GeneralBase + mGeneralEmpty3, GeneralBase + GeneralMenu, "");   
   /* fifth line */
   TtaNewTextForm (GeneralBase + mDialogueLang,
		   GeneralBase + GeneralMenu,
		   "Dialogue language",
		   10,
		   1,
		   FALSE);
   TtaNewLabel (GeneralBase + mGeneralEmpty2, GeneralBase + GeneralMenu, "");   
   /* sixth line */
   sprintf (s, "B%s%cB%s%cB%s", 
	    "Enable Multikey", EOS, 
	    "Show background images", EOS,
	    "Double click activates link");
   TtaNewToggleMenu (GeneralBase + mToggleGeneral,
		     GeneralBase + GeneralMenu,
		     NULL,
		     3,
		     s,
		     NULL,
		     FALSE);
   /* load and display the current values */
   GetGeneralConf ();
   RefreshGeneralMenu ();
  /* display the menu */
  TtaShowDialogue (GeneralBase + GeneralMenu, TRUE);
}

/**********************
** Publishing menu
***********************/

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
#ifndef _WINDOWS
  TtaSetToggleMenu (PublishBase + mTogglePublish, 0, LostUpdateCheck);
  TtaSetToggleMenu (PublishBase + mTogglePublish, 1, VerifyPublish);
#endif /* _WINDOWS */
  TtaSetTextForm (PublishBase + mDefaultName, DefaultName);
}

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
   CHAR             s[MAX_LENGTH];
   int              i;

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], "Apply");
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], "Defaults");

   TtaNewSheet (PublishBase + PublishMenu, 
		TtaGetViewFrame (document, view),
	       "Publishing Configuration", 2, s, FALSE, 11, 'L', D_DONE);
   sprintf (s, "%s%c%s", "BUse ETAGS and preconditions", EOS, 
	    "BVerify each PUT with a GET");
   TtaNewToggleMenu (PublishBase + mTogglePublish,
		     PublishBase + PublishMenu,
		     NULL,
		     2,
		     s,
		     NULL,
		     FALSE);
   TtaNewTextForm (PublishBase + mDefaultName,
		   PublishBase + PublishMenu,
		   "Default name for URLs finishing in \'/\'",
		   20,
		   1,
		   FALSE);
   /* load and display the current values */
   GetPublishConf ();
   RefreshPublishMenu ();
  /* display the menu */
  TtaShowDialogue (PublishBase + PublishMenu, TRUE);
}

/**********************
** Color Menu
**********************/

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
      /* removes the appareance conf menu */
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
	  
	default:
	  break;
	}
    }
}

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
   CHAR             s[MAX_LENGTH];
   int              i;

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], "Apply");
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], "Defaults");
   TtaNewSheet (ColorBase + ColorMenu, 
		TtaGetViewFrame (document, view),
	       "Color Configuration", 2, s, TRUE, 3, 'L', D_DONE);
   TtaNewTextForm (ColorBase + mForegroundColor,
		   ColorBase + ColorMenu,
		   "Foreground color",
		   20,
		   1,
		   FALSE);   

   /* load and display the current values */
   GetColorConf ();
   RefreshColorMenu ();
   /* display the menu */
   TtaShowDialogue (ColorBase + ColorMenu, TRUE);
}

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
  TtaSetTextForm (ColorBase + mForegroundColor, ForegroundColor);
}

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
  GetEnvString ("ForegroundColor", ForegroundColor);
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
  GetDefEnvString ("ForegroundColor", ForegroundColor);
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
  TtaSetEnvString ("ForegroundColor", ForegroundColor, TRUE);
}


/**********************
** Geometry Menu
**********************/

/*----------------------------------------------------------------------
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
      /* removes the appareance conf menu */
      TtaDestroyDialogue (GeometryBase + GeometryMenu);
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
	      break;
	    case 1:
	      SetGeometryConf ();
	      break;
	    case 2:
	      GetDefaultGeometryConf ();
	      RefreshGeometryMenu ();
	      break;
	    default:
	      break;
	    }
	  break;
	  
	case mStructureView:
	  if (data)
	    ustrcpy (StructureView, data);
	  else
	    StructureView[0] = EOS;
	  break;

	case mMathStructureView:
	  if (data)
	    ustrcpy (MathStructureView, data);
	  else
	     MathStructureView[0] = EOS;
	  break;

	case mGraphStructureView:
	  if (data)
	    ustrcpy (GraphStructureView, data);
	  else
	     GraphStructureView[0] = EOS;
	  break;

	case mAlternateView:
	  if (data)
	    ustrcpy (AlternateView, data);
	  else
	     AlternateView[0] = EOS;
	  break;

	case mLinksView:
	  if (data)
	    ustrcpy (LinksView, data);
	  else
	     LinksView[0] = EOS;
	  break;

	case mTableOfContentsView:
	  if (data)
	    ustrcpy (TableOfContentsView, data);
	  else
	     TableOfContentsView[0] = EOS;
	  break;

	default:
	  break;
	}
    }
}

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
   CHAR             s[MAX_LENGTH];
   int              i;

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], "Apply");
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], "Defaults");
   TtaNewSheet (GeometryBase + GeometryMenu, 
		TtaGetViewFrame (document, view),
	       "Geometry Configuration", 2, s, TRUE, 3, 'L', D_DONE);
   TtaNewTextForm (GeometryBase + mFormattedView,
		   GeometryBase + GeometryMenu,
		   "Formatted view",
		   20,
		   1,
		   FALSE);   
   TtaNewTextForm (GeometryBase + mStructureView,
		   GeometryBase + GeometryMenu,
		   "Structure view",
		   20,
		   1,
		   FALSE);
   TtaNewTextForm (GeometryBase + mMathStructureView,
		   GeometryBase + GeometryMenu,
		   "Math structure view",
		   20,
		   1,
		   FALSE);
   TtaNewTextForm (GeometryBase + mGraphStructureView,
		   GeometryBase + GeometryMenu,
		   "Graph structure view",
		   20,
		   1,
		   FALSE);
   TtaNewTextForm (GeometryBase + mAlternateView,
		   GeometryBase + GeometryMenu,
		   "Alternate View",
		   20,
		   1,
		   FALSE);
   TtaNewTextForm (GeometryBase + mLinksView,
		   GeometryBase + GeometryMenu,
		   "Links View",
		   20,
		   1,
		   FALSE);
   TtaNewTextForm (GeometryBase + mTableOfContentsView,
		   GeometryBase + GeometryMenu,
		   "Table of Contents view",
		   20,
		   1,
		   FALSE);

   /* load and display the current values */
   GetGeometryConf ();
   RefreshGeometryMenu ();
   /* display the menu */
   TtaShowDialogue (GeometryBase + GeometryMenu, TRUE);
}

/*----------------------------------------------------------------------
  RefreshGeometryMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void RefreshGeometryMenu ()
#else
static void RefreshGeometryMenu ()
#endif /* __STDC__ */
{
  TtaSetTextForm (GeometryBase + mFormattedView, FormattedView);
  TtaSetTextForm (GeometryBase + mStructureView, StructureView);
  TtaSetTextForm (GeometryBase + mMathStructureView, MathStructureView);
  TtaSetTextForm (GeometryBase + mGraphStructureView, GraphStructureView);
  TtaSetTextForm (GeometryBase + mAlternateView, AlternateView);
  TtaSetTextForm (GeometryBase + mLinksView, LinksView);
  TtaSetTextForm (GeometryBase + mTableOfContentsView, TableOfContentsView);
}

/*----------------------------------------------------------------------
  GetGeometryConf
  Makes a copy of the current registry geometry values
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void GetGeometryConf (void)
#else
static void GetGeometryConf ()
#endif /* __STDC__ */
{
  GetEnvString ("FormattedView", FormattedView);
  GetEnvString ("StructureView", StructureView);
  GetEnvString ("MathStructureView", MathStructureView);
  GetEnvString ("GraphStructureView", GraphStructureView);
  GetEnvString ("AlternateView", AlternateView);
  GetEnvString ("LinksView", LinksView);
  GetEnvString ("TableOfContentsView", TableOfContentsView);
}

/*----------------------------------------------------------------------
  GetDefaultGeometryConf
  Makes a copy of the default registry geometry values
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void GetDefaultGeometryConf (void)
#else
static void GetDefaultGeometryConf ()
#endif /* __STDC__ */
{
  GetDefEnvString ("FormattedView", FormattedView);
  GetDefEnvString ("StructureView", StructureView);
  GetDefEnvString ("MathStructureView", MathStructureView);
  GetDefEnvString ("GraphStructureView", GraphStructureView);
  GetDefEnvString ("AlternateView", AlternateView);
  GetDefEnvString ("LinksView", LinksView);
  GetDefEnvString ("TableOfContentsView", TableOfContentsView);
}


/*----------------------------------------------------------------------
  SetGeometryConf
  Updates the registry Geometry values
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void SetGeometryConf (void)
#else
static void SetGeometryConf ()
#endif /* __STDC__ */
{
  TtaSetEnvString ("FormattedView", FormattedView, TRUE);
  TtaSetEnvString ("StructureView", StructureView, TRUE);
  TtaSetEnvString ("MathStructureView", MathStructureView, TRUE);
  TtaSetEnvString ("GraphStructureView", GraphStructureView, TRUE);
  TtaSetEnvString ("AlternateView", AlternateView, TRUE);
  TtaSetEnvString ("LinksView", LinksView, TRUE);
  TtaSetEnvString ("TableOfContentsView", TableOfContentsView, TRUE);
}












