/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1999-2004
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef _MENUCONF_H__
#define _MENUCONF_H__
/*----------------------------------------------------------------------
  Definitions of the menu entries used in the configuration menus
  (MENUconf.c)
  Jose Kahan Feb 1999
  ----------------------------------------------------------------------*/
/* dialog and messages */

#define CacheMenu         1
#define mCacheOptions     2
#define mCacheDirectory   3
#define mCacheSize        4
#define mMaxCacheFile     5
#define mFlushCache       6
#define MAX_CACHEMENU_DLG 7

#define ProxyMenu         1
#define mHttpProxy        2
#define mProxyDomain      3
#define mProxyDomainInfo  4
#define mToggleProxy      5
#define MAX_PROXYMENU_DLG 6

#define GeneralMenu          1
#define mToggleGeneral       2
#define mZoom                3
#define mHomePage            4
#define mDialogueLang        5
#define mFontMenuSize        6
#define mGeneralEmpty1       7
#define mGeneralEmpty2       8
#define mGeneralEmpty3       9
#define mGeneralEmpty4       10
#define mGeneralAccessKey    11
#define MAX_GENERALMENU_DLG  12

#define BrowseMenu          1
#define mToggleBrowse       2
#define mScreenSelector     3
#define MAX_BROWSEMENU_DLG  4

#define PublishMenu         1
#define mTogglePublish      2
#define mDefaultName        3
#define mSafePutRedirect    4
#define mCharsetSelector    5
#define MAX_PUBLISHMENU_DLG 6

#define ColorMenu           1
#define mFgColor            2
#define mBgColor            3
#define mFgSelColor         4
#define mBgSelColor         5
#define mMenuFgColor        6
#define mMenuBgColor        7
#define mColorEmpty1        8
#define MAX_COLORMENU_DLG   9

#define GeometryMenu         1
#define mGeometryLabel1      2
#define mGeometryLabel2      3
#define MAX_GEOMETRYMENU_DLG 4

#define LanNegMenu           1
#define mLanNeg              2
#define MAX_LANNEGMENU_DLG   3

#define AnnotMenu               1
#define mAnnotUser              2
#define mAnnotPostServer        3
#define mAnnotServers           4
#define mToggleAnnot            5
#define mAnnotLAutoLoad         6
#define mAnnotRAutoLoad         7
#define mAnnotRAutoLoadRst      8
#define MAX_ANNOTMENU_DLG       9

typedef struct Prop_General_t
{
  int      Zoom;
  char     DialogueLang[MAX_LENGTH];
  int      AccesskeyMod;
  int      FontMenuSize;
  char     HomePage[MAX_LENGTH];
  ThotBool PasteLineByLine;
  ThotBool S_Buttons;
  ThotBool S_Address;
  ThotBool S_Targets;
  ThotBool S_AutoSave;
  ThotBool S_Geometry;
} Prop_General;

typedef struct Prop_Browse_t
{
  ThotBool LoadImages;
  ThotBool LoadObjects;
  ThotBool LoadCss;
  ThotBool DoubleClick;
  ThotBool EnableFTP;
  ThotBool BgImages;
  char     ScreenType[MAX_LENGTH];
  int      DoubleClickDelay; /* not used ? */
} Prop_Browse;

typedef struct Prop_Publish_t
{
  char     DefaultName[MAX_LENGTH];
  ThotBool UseXHTMLMimeType;
  ThotBool LostUpdateCheck;
  ThotBool ExportCRLF;
  ThotBool VerifyPublish;
  char     SafePutRedirect[MAX_LENGTH];
  char     CharsetType[MAX_LENGTH];
} Prop_Publish;

typedef struct Prop_Cache_t
{
  ThotBool EnableCache;
  ThotBool CacheProtectedDocs;
  ThotBool CacheDisconnectMode;
  ThotBool CacheExpireIgnore;
  char     CacheDirectory[MAX_LENGTH];
  int      CacheSize;
  int      MaxCacheFile;
} Prop_Cache;

typedef struct Prop_Proxy_t
{
  char     HttpProxy[MAX_LENGTH];
  char     ProxyDomain[MAX_LENGTH];
  ThotBool ProxyDomainIsOnlyProxy;
} Prop_Proxy;

typedef struct Prop_Color_t
{
  char     FgColor[MAX_LENGTH];
  char     BgColor[MAX_LENGTH];
  char     BgSelColor[MAX_LENGTH];
  char     FgSelColor[MAX_LENGTH];
  char     MenuFgColor[MAX_LENGTH];
  char     MenuBgColor[MAX_LENGTH];
} Prop_Color;

typedef struct Prop_LanNeg_t
{
  char     LanNeg[MAX_LENGTH];
} Prop_LanNeg;

typedef struct Prop_DAV_t
{
  char textUserReference[MAX_LENGTH];
  char textUserResources[MAX_LENGTH];
  char radioDepth[MAX_LENGTH];
  char radioTimeout[MAX_LENGTH];
  int  numberTimeout;
  char radioLockScope[MAX_LENGTH];
  ThotBool toggleAwareness1;
  ThotBool toggleAwareness2;
} Prop_DAV;

#endif /* _MENUCONF.h */
