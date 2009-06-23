/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1999-2009
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
#define mToolPanelLayout     12
#define MAX_GENERALMENU_DLG  13

#define BrowseMenu          1
#define mToggleBrowse       2
#define mScreenSelector     3
#define mLanNeg             4
#define MAX_BROWSEMENU_DLG  5

#define PublishMenu         1
#define mTogglePublish      2
#define mDefaultName        3
#define mSafePutRedirect    4
#define mCharsetSelector    5
#define mExportLength       6
#define MAX_PUBLISHMENU_DLG 7

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
#define mToggleGeom          4
#define MAX_GEOMETRYMENU_DLG 5

#define DAVMenu           1
#define MAX_DAVMENU_DLG   2

#define AnnotMenu               1
#define mAnnotUser              2
#define mAnnotPostServer        3
#define mAnnotServers           4
#define mToggleAnnot            5
#define mAnnotLAutoLoad         6
#define mAnnotRAutoLoad         7
#define mAnnotRAutoLoadRst      8
#define MAX_ANNOTMENU_DLG       9

#define TemplatesMenu              1
#define mToggleTemplatesFrame      2
#define MAX_TEMPLATEMENU_DLG       3

#define EmailsMenu               1
#define mEmailsServerAddress     2
#define mEmailsServerPort        3
#define mEmailsFromAddress       4
#define MAX_EMAILSMENU_DLG       5

#define PasswordsMenu              1
#define mTogglePasswordsFrame      2
#define MAX_PASSWORDMENU_DLG       3

#define RDFaMenu                   1
#define mToggleRDFaFrame           2
#define MAX_RDFaMENU_DLG           3


typedef struct Prop_General_t
{
  int      Zoom;
  char     DialogueLang[MAX_LENGTH];
  int      AccesskeyMod;
  int      FontMenuSize;
  char     HomePage[MAX_LENGTH];
  ThotBool XMLEdit;
  ThotBool PasteLineByLine;
  ThotBool S_Buttons;
  ThotBool S_Address;
  ThotBool S_NoAliasing;
  ThotBool S_DATE;
  ThotBool S_Targets;
  ThotBool S_AutoSave;
  ThotBool S_NBSP;
  ThotBool S_Shortcuts;
  ThotBool S_Templates;
  ThotBool WarnCTab;
  ThotBool ShowTipsStartup;
  int      ToolPanelLayout; /* 0=left, 1=right, 2=free */
} Prop_General;

typedef struct Prop_Browse_t
{
  int      OpeningLocation;
  ThotBool LoadImages;
  ThotBool LoadObjects;
  ThotBool LoadCss;
  ThotBool DoubleClick;
  ThotBool EnableFTP;
  ThotBool BgImages;
  ThotBool WarnIDs;
  char     ScreenType[MAX_LENGTH];
  char     LanNeg[MAX_LENGTH];
  int      DoubleClickDelay; /* not used ? */
  int      MaxURL;
} Prop_Browse;

typedef struct Prop_Publish_t
{
  char     DefaultName[MAX_LENGTH];
  ThotBool UseXHTMLMimeType;
  ThotBool LostUpdateCheck;
  ThotBool ExportCRLF;
  ThotBool VerifyPublish;
  ThotBool GenerateMathPI;
  int      ExportLength;
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
  ThotBool ColorSource;
} Prop_Color;

typedef struct Prop_Annot_t
{
  char     AnnotUser[MAX_LENGTH];
  char     AnnotPostServer[MAX_LENGTH];
  char     AnnotServers[MAX_LENGTH];
  ThotBool AnnotLAutoLoad;
  ThotBool AnnotRAutoLoad;
  ThotBool AnnotRAutoLoadRst;
} Prop_Annot;

typedef struct Prop_DAV_Path_t {
  char                   *Path;
  struct Prop_DAV_Path_t *NextPath;
} Prop_DAV_Path;

extern Prop_DAV_Path *DAV_Paths;

typedef struct Prop_DAV_t
{
  char textUserReference[MAX_LENGTH];
  char radioDepth[MAX_LENGTH];
  char radioTimeout[MAX_LENGTH];
  int  numberTimeout;
  char radioLockScope[MAX_LENGTH];
} Prop_DAV;

typedef struct Prop_Templates_Path_t {
  char                         *Path;
  struct Prop_Templates_Path_t *NextPath;
} Prop_Templates_Path;

extern Prop_Templates_Path *TemplateRepositoryPaths;

typedef struct Prop_Emails_t
{
  char serverAddress[MAX_LENGTH];
  int  serverPort;
  char fromAddress[MAX_LENGTH];
} Prop_Emails;

typedef struct Prop_Passwords_Site_t {
  char Site[MAX_LENGTH];
  struct Prop_Passwords_Site_t* NextSite;
} Prop_Passwords_Site;

typedef struct Prop_Passwords_t
{
  ThotBool S_Passwords;
} Prop_Passwords;

typedef struct Prop_RDFa_Path_t {
  char Path[MAX_LENGTH];
  struct Prop_RDFa_Path_t* NextPath;
} Prop_RDFa_Path;

typedef struct Prop_RDFa_t
{
  ThotBool S_RDFa;
  Prop_RDFa_Path* FirstPath;
} Prop_RDFa;

#endif /* _MENUCONF.h */
