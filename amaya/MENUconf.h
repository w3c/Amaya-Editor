/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1999.
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
#define mDoubleClickDelay    2
#define mZoom                3
#define mHomePage            4
#define mDialogueLang        5
#define mFontMenuSize        6
#define mGeneralEmpty1       7
#define mGeneralEmpty2       8
#define mGeneralEmpty3       9
#define mGeneralEmpty4       10
#define mToggleGeneral       11
#define MAX_GENERALMENU_DLG  12

#define PublishMenu         1
#define mTogglePublish      2
#define mDefaultName        3
#define mSafePutRedirect    4
#define MAX_PUBLISHMENU_DLG 5

#define ColorMenu           1
#define mFgColor            2
#define mBgColor            3
#define mMenuFgColor        4
#define mMenuBgColor        5
#define mColorEmpty1        6
#define MAX_COLORMENU_DLG   7

#define GeometryMenu         1
#define mGeometryLabel1      2
#define mGeometryLabel2      3
#define MAX_GEOMETRYMENU_DLG 4

#define LanNegMenu           1
#define mLanNeg              2
#define MAX_LANNEGMENU_DLG   3

#define ProfileMenu           1
#define mProfiles_File        2
#define mProfileEmpty1        3
#define mProfileEmpty2        4
#define mProfileSelector      5
#define MAX_PROFILEMENU_DLG   6
static void BuildProfileSelector();

#define TemplatesMenu           1
#define mTemplates              2
#define mTemplatesEmpty1        3
#define MAX_TEMPLATESMENU_DLG   4

#endif /* _MENUCONF.h */









