#ifdef CONF_MENU
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
static CHAR CacheDirectory [MAX_LENGTH];
static int CacheSize;
static int MaxCacheFile;
static CHAR HttpProxy [MAX_LENGTH];
static CHAR NoProxy [MAX_LENGTH];

/* Browser/Editing menu */
static int BrEdBase;
static int ToolTipDelay;
static int DoubleClickDelay;
static int Zoom;
static boolean Multikey;
static boolean LostUpdateCheck;
static boolean VerifyPublish;
static CHAR HomePage [MAX_LENGTH];
static CHAR ThotPrint [MAX_LENGTH];
static CHAR DefaultName [MAX_LENGTH];
static boolean BgImages;
static boolean DoubleClick;

/* Appearance menu options */
static int AppearanceBase;

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
#endif

/* Common functions */
/*----------------------------------------------------------------------
   InitConfMenu: initialisation, called during Amaya initialisation
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitConfMenu (void)
#else
void                InitConfMenu ()
#endif /* __STDC__*/
{
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
      TtaSetToggleMenu (ref, entry, *value);
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
  int                 val, length;
  DisplayMode         oldDisplayMode;
  SSchema	       sch;
  Element	       elParent, elFound;
  Attribute	       attr;
  AttributeType       attrType;
  boolean	       found;
  CHAR		       buf [MAX_LENGTH];
  SearchDomain	       domain;

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
	      break;
	    case 2:
	      GetDefaultNetworkConf ();
	      RefreshNetworkMenu ();
	      break;
	    default:
	      break;
	    }
	  break;

	case mToggleCache:
	  switch (val) 
	    {
	    case 0:
	      EnableCache = !EnableCache;
	      break;
	    case 1:
	      CacheProtectedDocuments = !CacheProtectedDocuments;
	      break;
	    }
	  break;

	case mCacheDirectory:
	  if (data)
	    ustrcpy (CacheDirectory, data);
	  else
	    CacheDirectory [0] = EOS;
	  break;

	case mCacheSize:
	  CacheSize = val;
	  break;

	case mMaxCacheFile:
	  MaxCacheFile = val;
	  break;

	case mHttpProxy:
	  if (data)
	    ustrcpy (HttpProxy, data);
	  else
	    HttpProxy [0] = EOS;
	  break;

	case mNoProxy:
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
  TtaGetEnvInt ("CACHE_MAX_FILE_SIZE", &MaxCacheFile);
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
  TtaSetEnvBoolean ("ENABLE_CACHE", EnableCache, YES);
  TtaSetEnvBoolean ("CACHE_PROTECTED_DOCUMENTS", 
		    CacheProtectedDocuments, YES);
  TtaSetEnvString ("CACHE_DIR", CacheDirectory, YES);
  TtaSetEnvInt ("CACHE_SIZE", CacheSize, YES);
  TtaSetEnvInt ("CACHE_MAX_FILE_SIZE", MaxCacheFile, YES);
  TtaSetEnvString ("HTTP_PROXY", HttpProxy, YES);
  TtaSetEnvString ("NO_PROXY", NoProxy, YES);
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
  TtaGetDefEnvInt ("CACHE_MAX_FILE_SIZE", &MaxCacheFile);
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
  TtaSetToggleMenu (NetworkBase + mToggleCache, 0, EnableCache);
  TtaSetToggleMenu (NetworkBase + mToggleCache, 1, 
		    CacheProtectedDocuments);
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
STRING              pathname;

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
	       "Network Configuration", 3, s, TRUE, 3, 'L', D_DONE);

   sprintf (s, "%s%c%s", "BEnable cache", EOS, 
	    "BCache protected documents");
   TtaNewToggleMenu (NetworkBase + mToggleCache,
		     NetworkBase + NetworkMenu,
		     "Cache options",
		     2,
		     s,
		     NULL,
		     FALSE);
   TtaNewTextForm (NetworkBase + mCacheDirectory,
		   NetworkBase + NetworkMenu,
		   "Cache Directory",
		   20,
		   1,
		   FALSE);
   TtaNewNumberForm (NetworkBase + mCacheSize,
		     NetworkBase + NetworkMenu,
		     "Cache size (Mb)",
		     0,
		     100,
		     FALSE);
   TtaNewNumberForm (NetworkBase + mMaxCacheFile,
		     NetworkBase + NetworkMenu,
		     "Maximum file size (Mb)",
		     0,
		     5,
		     FALSE);
   TtaNewTextForm (NetworkBase + mHttpProxy,
		   NetworkBase + NetworkMenu,
		   "HTTP Proxy",
		   20,
		   1,
		   FALSE);
   TtaNewTextForm (NetworkBase + mNoProxy,
		   NetworkBase + NetworkMenu,
		   "No Proxy",
		   20,
		   1,
		   FALSE);

   /* load and display the current values */
   GetNetworkConf ();
   RefreshNetworkMenu ();
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
  int                 val, length;
  DisplayMode         oldDisplayMode;
  SSchema	       sch;
  Element	       elParent, elFound;
  Attribute	       attr;
  AttributeType       attrType;
  boolean	       found;
  CHAR		       buf [MAX_LENGTH];
  SearchDomain	       domain;

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

  TtaSetEnvInt ("TOOLTIPDELAY", ToolTipDelay, YES);
  TtaSetEnvInt ("DOUBLECLICKDELAY", DoubleClickDelay, YES);
  TtaSetEnvInt ("ZOOM", Zoom, YES);
  /* recalibrate the zoom settings in all the active documents */
  for (i = 0; i < DocumentTableLength -1; i++)
    {
      if (DocumentURLs[i])
	RecalibrateZoom (i, 1);
    }
  TtaSetEnvBoolean ("ENABLE_MULTIKEY", Multikey, YES);
  TtaSetMultikey (Multikey);
  TtaSetEnvBoolean ("ENABLE_BG_IMAGES", BgImages, YES);
  TtaSetEnvBoolean ("ENABLE_DOUBLECLICK", DoubleClick, YES);
  TtaSetEnvBoolean ("ENABLE_LOST_UPDATE_CHECK", LostUpdateCheck, YES);
  TtaSetEnvBoolean ("VERIFY_PUBLISH", VerifyPublish, YES);
  TtaSetEnvString ("DEFAULTNAME", DefaultName, YES);
  TtaSetEnvString ("HOME_PAGE", HomePage, YES);
  TtaSetEnvString ("THOTPRINT", ThotPrint, YES);
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
  TtaSetToggleMenu (BrEdBase + mMultikey, 0, Multikey);
  TtaSetToggleMenu (BrEdBase + mBgImages, 0, BgImages);
  TtaSetToggleMenu (BrEdBase + mDoubleClick, 0, DoubleClick);
  TtaSetToggleMenu (BrEdBase + mTogglePublish, 0, LostUpdateCheck);
  TtaSetToggleMenu (BrEdBase + mTogglePublish, 1, 
		    VerifyPublish);
  TtaSetTextForm (BrEdBase + mDefaultName, DefaultName);
  TtaSetTextForm (BrEdBase + mHomePage, HomePage);
  TtaSetTextForm (BrEdBase + mThotPrint, ThotPrint);
}

/*----------------------------------------------------------------------
  BrowsingEditingConfMenu
  Build and display the Conf Menu dialog box and prepare for input.
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
	       "Editing/Browsing Configuration", 2, s, TRUE, 3, 'L', D_DONE);
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
   TtaNewTextForm (BrEdBase + mHomePage,
		   BrEdBase + BrEdMenu,
		   "Home Page",
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

   TtaShowDialogue (AppearanceBase + AppearanceMenu, TRUE);
}

void InitEnv ()
{

}
#endif /* CONF_MENU */
















