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
 */

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "MENUconf.h"

/* Network menu options */
static int NetworkBase;
static boolean EnableCache;
static boolean CacheProtectedDocuments;
static CHAR CacheDirectory [MAX_LENGTH+1];
static int CacheSize;
static int MaxCacheFile;
static CHAR HttpProxy [MAX_LENGTH+1];
static CHAR NoProxy [MAX_LENGTH+1];
static int NetworkStatus;

/* Browser/Editing menu */
static int BrEdBase;
static int ToolTipDelay;
static int DoubleClickDelay;
static int Zoom;
static boolean Multikey;
static boolean LostUpdateCheck;
static boolean VerifyPublish;
static CHAR HomePage [MAX_LENGTH+1];
static CHAR ThotPrint [MAX_LENGTH+1];
static CHAR DefaultName [MAX_LENGTH+1];
static boolean BgImages;
static boolean DoubleClick;

/* Appearance menu options */
static int AppearanceBase;
static CHAR Lang [MAX_LENGTH+1];
static int FontMenuSize;
static CHAR ForegroundColor [MAX_LENGTH+1];

/* common local variables */
CHAR s[300]; /* general purpose buffer */

#ifdef __STDC__
static void         GetEnvString (const STRING name, STRING value);
static void         GetDefEnvToggle (const STRING name, boolean *value, int ref, int entry);
static void         GetDefEnvString (const STRING name, STRING value);
static void         NetworkCallbackDialog(int ref, int typedata, STRING data);
static void         RefreshNetworkMenu (void);
static void         GetNetworkConf (void);
static void         GetDefaultNetworkConf (void);
static void         SetNetworkConf (void);
static void	    BrEdCallbackDialog(int ref, int typedata, STRING data);
static void         RefreshBrEdMenu (void);
static void         GetBrEdConf (void);
static void         GetDefaultBrEdConf (void);
static void         SetBrEdConf (void);
static void         AppearanceCallbackDialog(int ref, int typedata, STRING data);
static void         RefreshAppearanceMenu (void);
static void         GetAppearanceConf (void);

#else
static void         GetEnvString (/* const STRING name, STRING value */);
static void         GetDefEnvToggle (/* const STRING name, boolean *value, int ref, int entry */);
static void         GetDefEnvString (/* const STRING name, STRING value */);
static void         NetworkCallbackDialog (/*ref, typedata, data*/);
static void	    LookCallbackDialog(/* int ref, int typedata, STRING data*/);
static void         RefreshNetworkMenu (/* void */);
static void         GetNetworkConf (/* void */);
static void         GetDefaultNetworkConf (/* void */);
static void         SetNetworkConf (/* void */);
static void	    BrEdCallbackDialog(/*int ref, int typedata, STRING data*/);
static void         RefreshBrEdMenu (/* void */);
static void         GetBrEdConf (/* void */);
static void         GetDefaultBrEdConf (/* void */);
static void         SetBrEdConf (/* void */);
static void         AppearanceCallbackDialog(/* int ref, int typedata, STRING data */);
static void         RefreshAppearanceMenu (/* void */);
static void         GetAppearanceConf (/* void */);
#endif

/*
** Common functions
*/

/*----------------------------------------------------------------------
  InitDefEnv
  Initializes the default options which are not setup by thot.ini.
  This protects us against a crash due to a  user's erasing thot.ini.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void InitDefEnv (void)
#else
void InitDefEnv ()
#endif /* __STDC__ */
{
  char *ptr;

  /* browsing editing options */
  TtaSetDefEnvString ("HOME_PAGE", "", FALSE);
  TtaSetDefEnvString ("TMPDIR", "", FALSE);
  TtaSetDefEnvString ("ENABLE_MULTIKEY", "no", FALSE);
  TtaSetDefEnvString ("ENABLE_BG_IMAGES", "yes", FALSE);
  TtaSetDefEnvString ("VERIFY_PUBLISH", "no", FALSE);
  TtaSetDefEnvString ("ENABLE_LOST_UPDATE_CHECK", "yes", FALSE);
  TtaSetDefEnvString ("DEFAULTNAME", "Overview.html", FALSE);

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
  TtaSetDefEnvString ("CACHE_DIR", "", FALSE);
  TtaSetDefEnvString ("CACHE_PROTECTED_DOCS", "yes", FALSE);
  ptr = TtaGetDefEnvString ("ENABLE_CACHE");
  TtaSetDefEnvString ("ENABLE_CACHE", "yes", FALSE);

  /* appearance */
  TtaSetDefEnvString ("FontMenuSize", "12", FALSE);

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
  InitDefEnv ();
  NetworkBase = TtaSetCallback (NetworkCallbackDialog, MAX_NETWORKMENU_DLG);
  BrEdBase = TtaSetCallback (BrEdCallbackDialog, MAX_BREDMENU_DLG);
  AppearanceBase = TtaSetCallback (AppearanceCallbackDialog,
				   MAX_APPEARANCEMENU_DLG);
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
#ifdef _WINDOWS
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
** Network configuration menu
***********************/

/*----------------------------------------------------------------------
  NetworkCallbackDialog
  callback of the network configuration menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         NetworkCallbackDialog (int ref, int typedata, STRING data)
#else
static void         NetworkCallbackDialog (ref, typedata, data)
int                 ref;
int                 typedata;
STRING              data;

#endif
{
  int                 val;

  if (ref == -1)
    {
      /* removes the network conf menu */
      TtaDestroyDialogue (NetworkBase + NetworkMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (int) data;
      switch (ref - NetworkBase)
	{
	case NetworkMenu:
	  switch (val) 
	    {
	    case 0:
	      TtaDestroyDialogue (ref);
	      break;
	    case 1:
	      SetNetworkConf ();
#ifdef AMAYA_JAVA
#else      
	      libwww_updateNetworkConf (NetworkStatus);
#endif /* !AMAYA_JAVA */
	      /* reset the status flag */
	      NetworkStatus = 0;
	      break;
	    case 2:
	      GetDefaultNetworkConf ();
	      RefreshNetworkMenu ();
	      /* always signal this as modified */
	      NetworkStatus = AMAYA_CACHE_RESTART | AMAYA_PROXY_RESTART;
	      break;
	    default:
	      break;
	    }
	  break;

	case mToggleCache:
	  switch (val) 
	    {
	    case 0:
	      NetworkStatus |= AMAYA_CACHE_RESTART;
	      EnableCache = !EnableCache;
	      break;
	    case 1:
	      NetworkStatus |= AMAYA_CACHE_RESTART;
	      CacheProtectedDocuments = !CacheProtectedDocuments;
	      break;
	    }
	  break;

	case mCacheDirectory:
	  NetworkStatus |= AMAYA_CACHE_RESTART;
	  if (data)
	    ustrcpy (CacheDirectory, data);
	  else
	    CacheDirectory [0] = EOS;
	  break;

	case mCacheSize:
	  NetworkStatus |= AMAYA_CACHE_RESTART;
	  CacheSize = val;
	  break;

	case mMaxCacheFile:
	  NetworkStatus |= AMAYA_CACHE_RESTART;
	  MaxCacheFile = val;
	  break;

	case mHttpProxy:
	  NetworkStatus |= AMAYA_PROXY_RESTART;
	  if (data)
	    ustrcpy (HttpProxy, data);
	  else
	    HttpProxy [0] = EOS;
	  break;

	case mNoProxy:
	  NetworkStatus |= AMAYA_PROXY_RESTART;
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
  GetNetworkConf
  Makes a copy of the current registry network values
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void GetNetworkConf (void)
#else
static void GetNetworkConf ()
#endif /* __STDC__ */
{
  TtaGetEnvBoolean ("ENABLE_CACHE", &EnableCache);
  TtaGetEnvBoolean 
    ("CACHE_PROTECTED_DOCUMENTS", &CacheProtectedDocuments);
  GetEnvString ("CACHE_DIR", CacheDirectory);
  TtaGetEnvInt ("CACHE_SIZE", &CacheSize);
  TtaGetEnvInt ("MAX_CACHE_ENTRY_SIZE", &MaxCacheFile);
  GetEnvString ("HTTP_PROXY", HttpProxy);
  GetEnvString ("NO_PROXY", NoProxy);
}

/*----------------------------------------------------------------------
  SetNetworkConf
  Updates the registry network values and calls the network functions
  to take into acocunt the changes
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void SetNetworkConf (void)
#else
static void SetNetworkConf ()
#endif /* __STDC__ */
{
  TtaSetEnvBoolean ("ENABLE_CACHE", EnableCache, TRUE);
  TtaSetEnvBoolean ("CACHE_PROTECTED_DOCUMENTS", 
		    CacheProtectedDocuments, TRUE);
  TtaSetEnvString ("CACHE_DIR", CacheDirectory, TRUE);
  TtaSetEnvInt ("CACHE_SIZE", CacheSize, TRUE);
  TtaSetEnvInt ("MAX_CACHE_ENTRY_SIZE", MaxCacheFile, TRUE);
  TtaSetEnvString ("HTTP_PROXY", HttpProxy, TRUE);
  TtaSetEnvString ("NO_PROXY", NoProxy, TRUE);
}

/*----------------------------------------------------------------------
  GetDefaultNetworkConf
  Updates the registry network values and calls the network functions
  to take into acocunt the changes
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void GetDefaultNetworkConf ()
#else
static void GetDefaultNetworkConf ()
#endif /*__STDC__*/
{
  /* read the default values */
  GetDefEnvToggle ("ENABLE_CACHE", &EnableCache, 
		    NetworkBase + mToggleCache, 0);
  GetDefEnvToggle 
    ("PROTECTED_DOCUMENTS", &CacheProtectedDocuments,
     NetworkBase + mToggleCache, 1);
  GetDefEnvString ("CACHE_DIR", CacheDirectory);
  TtaGetDefEnvInt ("CACHE_SIZE", &CacheSize);
  TtaGetDefEnvInt ("MAX_CACHE_ENTRY_SIZE", &MaxCacheFile);
  GetDefEnvString ("HTTP_PROXY", HttpProxy);
  GetDefEnvString ("NO_PROXY", NoProxy);
}

/*----------------------------------------------------------------------
  RefreshNetworkMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void RefreshNetworkMenu ()
#else
static void RefreshNetworkMenu ()
#endif /* __STDC__ */
{
  /* verify what happens when the option is NULL */

  /* set the menu entries to the current values */
#ifndef _WINDOWS
  TtaSetToggleMenu (NetworkBase + mToggleCache, 0, EnableCache);
  TtaSetToggleMenu (NetworkBase + mToggleCache, 1, CacheProtectedDocuments);
#endif /* _WINDOWS */
  if (CacheDirectory)
    TtaSetTextForm (NetworkBase + mCacheDirectory, CacheDirectory);
  TtaSetNumberForm (NetworkBase + mCacheSize, CacheSize);
  TtaSetNumberForm (NetworkBase + mMaxCacheFile, MaxCacheFile);
  TtaSetTextForm (NetworkBase + mHttpProxy, HttpProxy);
  TtaSetTextForm (NetworkBase + mNoProxy, NoProxy);
}

/*----------------------------------------------------------------------
  NetworkConfMenu
  Build and display the Conf Menu dialog box and prepare for input.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         NetworkConfMenu (Document document, View view)
#else
void         NetworkConfMenu (document, view)
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

   TtaNewSheet (NetworkBase + NetworkMenu, 
		TtaGetViewFrame (document, view),
	       "Network Configuration", 2, s, FALSE, 4, 'L', D_DONE);

   sprintf (s, "%s%c%s", "BEnable cache", EOS, 
	    "BCache protected documents");
   TtaNewToggleMenu (NetworkBase + mToggleCache,
		     NetworkBase + NetworkMenu,
		     "Cache options",
		     2,
		     s,
		     NULL,
		     TRUE);
   TtaNewTextForm (NetworkBase + mCacheDirectory,
		   NetworkBase + NetworkMenu,
		   "Cache directory",
		   20,
		   1,
		   TRUE);
   TtaNewNumberForm (NetworkBase + mCacheSize,
		     NetworkBase + NetworkMenu,
		     "Cache size (Mb)",
		     0,
		     100,
		     TRUE);
   TtaNewNumberForm (NetworkBase + mMaxCacheFile,
		     NetworkBase + NetworkMenu,
		     "Cache entry size limit (Mb)",
		     0,
		     5,
		     TRUE);
   TtaNewTextForm (NetworkBase + mHttpProxy,
		   NetworkBase + NetworkMenu,
		   "HTTP proxy",
		   20,
		   1,
		   TRUE);
   TtaNewTextForm (NetworkBase + mNoProxy,
		   NetworkBase + NetworkMenu,
		   "No proxy",
		   20,
		   1,
		   TRUE);

   /* load and display the current values */
   GetNetworkConf ();
   RefreshNetworkMenu ();
   /* clean the modified flags */
   NetworkStatus = 0;
  /* display the menu */
  TtaShowDialogue (NetworkBase + NetworkMenu, TRUE);
}

/**********************
** Browsing/Editing menu
***********************/

/*----------------------------------------------------------------------
   callback of the Browsing/Editing menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         BrEdCallbackDialog (int ref, int typedata, STRING data)
#else
static void         BrEdCallbackDialog (ref, typedata, data)
int                 ref;
int                 typedata;
STRING              data;

#endif
{
  int                 val;

  if (ref == -1)
    {
      /* removes the network conf menu */
      TtaDestroyDialogue (BrEdBase + BrEdMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (int) data;
      switch (ref - BrEdBase)
	{
	case BrEdMenu:
	  switch (val) 
	    {
	    case 0:
	      TtaDestroyDialogue (ref);
	      break;
	    case 1:
	      SetBrEdConf ();
	      break;
	    case 2:
	      GetDefaultBrEdConf ();
	      RefreshBrEdMenu ();
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

	case mMultikey:
	  Multikey = !Multikey;
	  break;

	case mBgImages:
	  BgImages = !BgImages;
	  break;

	case mDoubleClick:
	  DoubleClick = !DoubleClick;
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

	case mHomePage:
	  if (data)
	    ustrcpy (HomePage, data);
	  else
	    HomePage [0] = EOS;

	case mThotPrint:
	  if (data)
	    ustrcpy (ThotPrint, data);
	  else
	    ThotPrint [0] = EOS;
	  break;

	default:
	  break;
	}
    }
}

/*----------------------------------------------------------------------
  GetBrEdConf
  Makes a copy of the current registry BrEd values
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void GetBrEdConf (void)
#else
static void GetBrEdConf ()
#endif /* __STDC__ */
{
  TtaGetEnvInt ("TOOLTIPDELAY", &ToolTipDelay);
  TtaGetEnvInt ("DOUBLECLICKDELAY", &DoubleClickDelay);
  TtaGetEnvInt ("ZOOM", &Zoom);
  TtaGetEnvBoolean ("ENABLE_MULTIKEY", &Multikey);
  TtaGetEnvBoolean ("ENABLE_BG_IMAGES", &BgImages);
  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &DoubleClick);
  TtaGetEnvBoolean ("ENABLE_LOST_UPDATE_CHECK", &LostUpdateCheck);
  TtaGetEnvBoolean ("VERIFY_PUBLISH", &VerifyPublish);
  GetEnvString ("DEFAULTNAME", DefaultName);
  GetEnvString ("HOME_PAGE", HomePage);
  GetEnvString ("THOTPRINT", ThotPrint);
}

/*----------------------------------------------------------------------
  SetBrEdConf
  Updates the registry BrEd values and calls the BrEd functions
  to take into account the changes
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void SetBrEdConf (void)
#else
static void SetBrEdConf ()
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
  TtaSetEnvBoolean ("ENABLE_LOST_UPDATE_CHECK", LostUpdateCheck, TRUE);
  TtaSetEnvBoolean ("VERIFY_PUBLISH", VerifyPublish, TRUE);
  TtaSetEnvString ("DEFAULTNAME", DefaultName, TRUE);
  TtaSetEnvString ("HOME_PAGE", HomePage, TRUE);
  TtaSetEnvString ("THOTPRINT", ThotPrint, TRUE);
}

/*----------------------------------------------------------------------
  GetDefaultBrEdConf
  Loads the default registry BrEd values
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void GetDefaultBrEdConf ()
#else
static void GetDefaultBrEdConf ()
#endif /*__STDC__*/
{
  TtaGetDefEnvInt ("TOOLTIPDELAY", &ToolTipDelay);
  TtaGetDefEnvInt ("DOUBLECLICKDELAY", &DoubleClickDelay);
  TtaGetDefEnvInt ("ZOOM", &Zoom);
  GetDefEnvToggle ("ENABLE_MULTIKEY", &Multikey, 
		       BrEdBase + mMultikey, 0);
  GetDefEnvToggle ("ENABLE_BG_IMAGES", &BgImages,
		       BrEdBase + mBgImages, 0);
  GetDefEnvToggle ("ENABLE_DOUBLECLICK", &DoubleClick,
		       BrEdBase + mDoubleClick, 0);
  GetDefEnvToggle ("ENABLE_LOST_UPDATE_CHECK", &LostUpdateCheck, 
		    BrEdBase + mTogglePublish, 0);
  GetDefEnvToggle ("VERIFY_PUBLISH", &VerifyPublish,
		    BrEdBase + mTogglePublish, 1);
  GetDefEnvString ("DEFAULTNAME", DefaultName);
  GetDefEnvString ("HOME_PAGE", HomePage);
  GetDefEnvString ("THOTPRINT", ThotPrint);
}

/*----------------------------------------------------------------------
  RefreshBrEdMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void RefreshBrEdMenu ()
#else
static void RefreshBrEdMenu ()
#endif /* __STDC__ */
{
  TtaSetNumberForm (BrEdBase + mToolTipDelay, ToolTipDelay);
  TtaSetNumberForm (BrEdBase + mDoubleClickDelay, DoubleClickDelay);
  TtaSetNumberForm (BrEdBase + mZoom, Zoom);
#ifndef _WINDOWS
  TtaSetToggleMenu (BrEdBase + mMultikey, 0, Multikey);
  TtaSetToggleMenu (BrEdBase + mBgImages, 0, BgImages);
  TtaSetToggleMenu (BrEdBase + mDoubleClick, 0, DoubleClick);
  TtaSetToggleMenu (BrEdBase + mTogglePublish, 0, LostUpdateCheck);
  TtaSetToggleMenu (BrEdBase + mTogglePublish, 1, VerifyPublish);
#endif /* _WINDOWS */
  TtaSetTextForm (BrEdBase + mDefaultName, DefaultName);
  TtaSetTextForm (BrEdBase + mHomePage, HomePage);
  TtaSetTextForm (BrEdBase + mThotPrint, ThotPrint);
}

/*----------------------------------------------------------------------
  BrEdConfMenu
  Build and display the Browsing Editing conf Menu dialog box and prepare 
  for input.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         BrEdConfMenu (Document document, View view)
#else
void         BrEdConfMenu (document, view)
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

   TtaNewSheet (BrEdBase + BrEdMenu, 
		TtaGetViewFrame (document, view),
	       "Editing/Browsing Configuration", 2, s, FALSE, 11, 'L', D_DONE);
   TtaNewTextForm (BrEdBase + mHomePage,
		   BrEdBase + BrEdMenu,
		   "Home Page",
		   20,
		   1,
		   FALSE);
   TtaNewNumberForm (BrEdBase + mToolTipDelay,
		     BrEdBase + BrEdMenu,
		     "ToolTip delay (ms)",
		     0,
		     65000,
		     FALSE);   
   TtaNewNumberForm (BrEdBase + mDoubleClickDelay,
		     BrEdBase + BrEdMenu,
		     "Double Click delay (ms)",
		     0,
		     65000,
		     FALSE);   
   TtaNewNumberForm (BrEdBase + mZoom,
		     BrEdBase + BrEdMenu,
		     "Zoom",
		     -10,
		     10,
		     FALSE);   
   TtaNewToggleMenu (BrEdBase + mMultikey,
		     BrEdBase + BrEdMenu,
		     NULL,
		     1,
		     "BEnable Multikey",
		     NULL,
		     FALSE);
   TtaNewToggleMenu (BrEdBase + mBgImages,
		     BrEdBase + BrEdMenu,
		     NULL,
		     1,
		     "BShow background images",
		     NULL,
		     FALSE);
   TtaNewToggleMenu (BrEdBase + mDoubleClick,
		     BrEdBase + BrEdMenu,
		     NULL,
		     1,
		     "BDouble click activates anchor",
		     NULL,
		     FALSE);
   sprintf (s, "%s%c%s", "BUse ETAGS and preconditions", EOS, 
	    "BVerify each PUT with a GET");
   TtaNewToggleMenu (BrEdBase + mTogglePublish,
		     BrEdBase + BrEdMenu,
		     "Publishing options",
		     2,
		     s,
		     NULL,
		     FALSE);
   TtaNewTextForm (BrEdBase + mDefaultName,
		   BrEdBase + BrEdMenu,
		   "Default name for URLs finishing in \'/\'",
		   20,
		   1,
		   FALSE);
   TtaNewTextForm (BrEdBase + mThotPrint,
		   BrEdBase + BrEdMenu,
		   "Printer",
		   20,
		   1,
		   FALSE);
   /* load and display the current values */
   GetBrEdConf ();
   RefreshBrEdMenu ();
  /* display the menu */
  TtaShowDialogue (BrEdBase + BrEdMenu, TRUE);
}

/**********************
** Appearance Menu
**********************/

/*----------------------------------------------------------------------
   callback of the appearance configuration menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AppearanceCallbackDialog (int ref, int typedata, STRING data)
#else
static void         AppearanceCallbackDialog (ref, typedata, data)
int                 ref;
int                 typedata;
STRING              data;

#endif
{
  int val;

  if (ref == -1)
    {
      /* removes the network conf menu */
      TtaDestroyDialogue (AppearanceBase + AppearanceMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (int) data;
      switch (ref - AppearanceBase)
	{
	case AppearanceMenu:
	  switch (val) 
	    {
	    case 0:
	      TtaDestroyDialogue (ref);
	      break;
	    case 1:
	      /***
	      SetAppearanceConf ();
	      ***/
	      break;
	    case 2:
	      /**
	      GetDefaultAppearanceConf ();
	      RefreshAppearanceMenu ();
	      **/
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
  AppearanceConfMenu
  Build and display the Conf Menu dialog box and prepare for input.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         AppearanceConfMenu (Document document, View view)
#else
void         AppearanceConfMenu (document, view)
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
   TtaNewSheet (AppearanceBase + AppearanceMenu, 
		TtaGetViewFrame (document, view),
	       "Appearance Configuration", 2, s, TRUE, 3, 'L', D_DONE);
   TtaNewTextForm (AppearanceBase + mDialogueLang,
		   AppearanceBase + AppearanceMenu,
		   "Dialogue language",
		   20,
		   1,
		   FALSE);   
   TtaNewNumberForm (AppearanceBase + mFontMenuSize,
		     AppearanceBase + AppearanceMenu,
		     "Menu font size",
		     8,
		     20,
		     FALSE);   
   TtaNewTextForm (AppearanceBase + mForegroundColor,
		   AppearanceBase + AppearanceMenu,
		   "Foreground color",
		   20,
		   1,
		   FALSE);   
   /* load and display the current values */
   GetAppearanceConf ();
   RefreshAppearanceMenu ();
   /* display the menu */
   TtaShowDialogue (AppearanceBase + AppearanceMenu, TRUE);
}

/*----------------------------------------------------------------------
  RefreshAppearanceMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void RefreshAppearanceMenu ()
#else
static void RefreshAppearanceMenu ()
#endif /* __STDC__ */
{
  TtaSetNumberForm (AppearanceBase + mFontMenuSize, FontMenuSize);
  TtaSetTextForm (AppearanceBase + mForegroundColor, ForegroundColor);
  TtaSetTextForm (AppearanceBase + mDialogueLang, Lang);
}

/*----------------------------------------------------------------------
  GetAppearanceConf
  Makes a copy of the current registry appearance values
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void GetAppearanceConf (void)
#else
static void GetAppearanceConf ()
#endif /* __STDC__ */
{
  char *ptr;

  GetEnvString ("LANG", Lang);
  TtaGetEnvInt ("FontMenuSize", &FontMenuSize);
  GetEnvString ("ForegroundColor", ForegroundColor);
}
