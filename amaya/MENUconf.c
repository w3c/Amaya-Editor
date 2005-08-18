/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1999-2005
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
 * Contributors: I. Vatton colors selection, browsing menu and access keys
 *
 * To do: remove the CACHE_RESTART option from some options, once we write
 * the code that should take it into account.
 */

/* Included headerfiles */

#ifdef _WX
#include "wx/wx.h"
#endif /* _WX */

#define THOT_EXPORT extern
#include "amaya.h"
#include "MENUconf.h"
#include "MENUconf_f.h"
#include "print.h"
#include "fileaccess.h"
#ifdef _WX
#include "wxdialogapi_f.h"
#endif /* _WX */

#ifdef DAV
#define WEBDAV_EXPORT extern
#include "davlib.h"
#include "davlibUI_f.h"
#endif /* DAV */

#ifdef _WINGUI
#include "resource.h"
#include "wininclude.h"
#include "constmedia.h"
#include "appdialogue.h"

extern HINSTANCE hInstance;
/* an optimization to say which common
menu buttons we want to initialize */
enum {
  AM_INIT_APPLY_BUTTON = 1,
  AM_INIT_DEFAULT_BUTTON = 2,
  AM_INIT_DONE_BUTTON = 4,
  AM_INIT_ALL = 0xFFFF
};
#endif /* _WINGUI */


/* ======> Preference dialog (contains eache sub dialog into tabs) */
#ifdef _WX
/* Preference dialog (contains eache sub dialog into tabs) */
static int PreferenceBase;
#endif /* _WX */


/* ============> Cache menu options */
static int        CacheBase;
static int        CacheStatus;
static Prop_Cache GProp_Cache;
#ifdef _WINGUI
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
#endif /* _WINGUI */


/* ============> Proxy menu options */
static int        ProxyBase;
static int        ProxyStatus;
static Prop_Proxy GProp_Proxy;
#ifdef _WINGUI
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
#endif /* _WINGUI */


/* ============> General menu options */
static int          GeneralBase;
static Prop_General GProp_General;
#define DEF_SAVE_INTVL 10	/* number of typed characters triggering 
				   automatic saving */
#ifdef _WINGUI
static char     AppHome[MAX_LENGTH];
static char     AppTmpDir[MAX_LENGTH];
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
	{IDC_LINES, AM_PASTE_LINE_BY_LINE},
	{IDC_AUTOSAVE, AM_AUTO_SAVE},
	{0, 0}
};
#endif /* _WINGUI */


/* ============> Browse menu options */
static int         BrowseBase;
static Prop_Browse GProp_Browse;
#ifndef _WX
  static int         CurrentScreen;
#endif /* _WX */
static int         InitOpeningLocation;
static ThotBool    InitWarnCTab;
static ThotBool    InitLoadImages;
static ThotBool    InitLoadObjects;
static ThotBool    InitLoadCss;
static ThotBool    InitBgImages;
static char        InitScreen[MAX_LENGTH];
#ifndef _WX
static char       *ScreensTxt[]={
  "handheld", "print", "projection", "screen", "tty", "tv"
};
#endif /* _WX */
#ifdef _WINGUI
static HWND        BrowseHwnd =  NULL;
static HWND        ScreensList;
static AM_WIN_MenuText WIN_BrowseMenuText[] = 
{
	{AM_INIT_ALL, AM_BROWSE_MENU},
	{IDC_LOADIMG, AM_LOAD_IMAGES},
	{IDC_LOADOBJ, AM_LOAD_OBJECTS},
	{IDC_BGIMAGES, AM_SHOW_BG_IMAGES},
	{IDC_LOADCSS, AM_LOAD_CSS},
	{IDC_DOUBLECLICK, AM_ENABLE_DOUBLECLICK},
	{IDC_ENABLEFTP, AM_ENABLE_FTP},
	{IDC_SCREEN, AM_SCREEN_TYPE},
	{IDC_TLANNEG, AM_LANG_NEGOTIATION},
	{0, 0}
};
#endif /* _WINGUI */


/* ============> Publish menu options */
static int          PublishBase;
static Prop_Publish GProp_Publish;
static int          SafePutStatus;
static int          CurrentCharset;
static char         NewCharset[MAX_LENGTH];
static char        *CharsetTxt[]={
  "us-ascii", "iso-8859-1", "utf-8"
};
#ifdef _WINGUI
static HWND     PublishHwnd =  NULL;
static HWND     CharsetList;
static AM_WIN_MenuText WIN_PublishMenuText[] = 
{
	{AM_INIT_ALL, AM_PUBLISH_MENU},
	{IDC_CHARSET_TITLE, AM_DEFAULT_CHARSET},
	{IDC_USEXHTMLMIMETYPE, AM_USE_XHTML_MIMETYPE},
	{IDC_LOSTUPDATECHECK, AM_USE_ETAGS},
	{IDC_VERIFYPUBLISH, AM_VERIFY_PUT},
	{IDC_CRLF, AM_EXPORT_CRLF},
	{IDC_TDEFAULTNAME, AM_DEFAULT_NAME},
	{IDC_TSAFEPUTREDIRECT, AM_SAFE_PUT_REDIRECT},
	{0, 0}
};
#endif /* _WINGUI */


/* ============> Color menu options */
static int        ColorBase;
static Prop_Color GProp_Color;
/* this one should be exported from the thotlib */
extern char  *ColorName (int num);
#ifdef _WINGUI
static HWND     ColorHwnd = NULL;
static AM_WIN_MenuText WIN_ColorMenuText[] = 
{
	{AM_INIT_ALL, AM_COLOR_MENU},
	{IDC_TFGCOLOR, AM_DOC_FG_COLOR},
	{IDC_TBGCOLOR, AM_DOC_BG_COLOR},
	{IDC_TFGSELCOLOR, AM_FG_SEL_COLOR},
	{IDC_TBGSELCOLOR, AM_BG_SEL_COLOR},
 	{IDC_CHANGCOLOR, AM_COLOR_PALETTE},
 	{IDC_CHANGCOLOR2, AM_COLOR_PALETTE},
	{0, 0}
};
#endif /* _WINGUI */


/* ============> Geometry menu options */
static int      GeometryBase;
static Document GeometryDoc = 0;
/* common local variables */
static char    s[MAX_LENGTH]; /* general purpose buffer */
#ifdef _WINGUI
HWND            GeometryHwnd = NULL;
static AM_WIN_MenuText WIN_GeometryMenuText[] = 
{
	{AM_INIT_DONE_BUTTON, AM_GEOMETRY_MENU},
	{IDC_GEOMCHANGE, AM_GEOMETRY_CHANGE},
	{ID_APPLY, AM_SAVE_GEOMETRY},
	{ID_DEFAULTS, AM_RESTORE_GEOMETRY},
	{0, 0}
};
#endif /* _WINGUI */


/* ============> Annotation menu option */
#ifdef ANNOTATIONS
int      AnnotBase;
static Prop_Annot  GProp_Annot;

#include "annotlib.h"
#include "ANNOTevent_f.h"
#ifdef _WINGUI
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
#endif /* _WINGUI */
#endif /* ANNOTATIONS */

/* ============> WebDAV menu option */
#ifdef DAV
int       DAVBase;
Prop_DAV  GProp_DAV;
#endif /* DAV */

#include "HTMLsave_f.h"
#include "init_f.h"
#include "query_f.h"

/*-----------------------------------------------------------------------
   _GetSysUserName
   Gives ptr the value of the system's user name. 
   If succesful, returns TRUE, FALSE otherwise.
  -----------------------------------------------------------------------*/
static ThotBool _GetSysUserName (char *username)
{
#ifdef _WINGUI
  ThotBool  status;
  DWORD     dwSize;

  /* compute the default app_home value from the username and thotdir */
  dwSize = MAX_LENGTH * sizeof (char);
  status = GetUserName (username, &dwSize);
  if (!status || *username == EOS)
    return FALSE;
  /* in principle, username is returned in Unicode */
#endif

#ifdef _WX
  /* TODO : a valider sous UNIX : OK */
  wxString loginname = wxGetUserId();
  if ( !loginname.IsEmpty() )
  {
    sprintf(username,"%s", (const char*)loginname.mb_str(*wxConvCurrent));
    return TRUE;
  }
  else
  {
    return FALSE;
  }
#endif /* _WX */

#if defined(_GTK) 
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
#endif /* #if defined(_GTK) */

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
      strcpy (GProp_General.HomePage, ptr);
      strcat (GProp_General.HomePage, AMAYA_PAGE);
    }
  else
    GProp_General.HomePage[0]  = EOS;
  TtaSetDefEnvString ("HOME_PAGE", GProp_General.HomePage, FALSE);
  GProp_General.HomePage[0] = EOS;
  TtaSetDefEnvString ("EXPORT_CRLF", "no", FALSE);
  TtaSetDefEnvString ("PASTE_LINE_BY_LINE", "yes", FALSE);
  TtaSetDefEnvString ("ENABLE_BG_IMAGES", "yes", FALSE);
  TtaSetDefEnvString ("LOAD_IMAGES", "yes", FALSE);
  TtaSetDefEnvString ("LOAD_OBJECTS", "yes", FALSE);
  TtaSetDefEnvString ("LOAD_CSS", "yes", FALSE);
  TtaSetDefEnvString ("VERIFY_PUBLISH", "no", FALSE);
  TtaSetDefEnvString ("ENABLE_LOST_UPDATE_CHECK", "yes", FALSE);
  TtaSetDefEnvString ("DEFAULTNAME", "Overview.html", FALSE);
  TtaSetDefEnvString ("FontMenuSize", "12", FALSE);
  TtaSetDefEnvString ("ENABLE_DOUBLECLICK", "yes", FALSE);
  /* @@@ */
  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &GProp_Browse.DoubleClick);
  /* @@@ */
  TtaSetDefEnvString ("ENABLE_FTP", "yes", FALSE);
  TtaSetDefEnvString ("SCREEN_TYPE", "screen", FALSE);
#ifndef _WINGUI
  TtaSetDefEnvString ("THOTPRINT", "lpr", FALSE);
#endif
  /* network configuration */
  TtaSetDefEnvString ("ENABLE_XHTML_MIMETYPE", "no", FALSE);
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
  ptr2 = (char *)TtaGetMemory ( strlen (ptr) + strlen ("annotations")
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
#ifdef _GTK
      TtaSetToggleMenu (ref, entry, *value);
#endif /* _GTK */
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
      if (dirname[strlen (dirname) -1] == DIR_SEP)
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

#ifdef _WINGUI
/*----------------------------------------------------------------------
  AmCopyFile
  Copies a file from one dir to another dir. If the file doesn't exist,
  doesn't do anything.  
  ----------------------------------------------------------------------*/
static void AmCopyFile (const char  *source_dir, const char  *dest_dir,
			const char  *filename)
{
 char   source_file[MAX_LENGTH];
 
 sprintf (source_file, "%s%c%s", source_dir, DIR_SEP, filename);
 if (TtaFileExist (source_file))
 {
   sprintf (s, "%s%c%s", dest_dir, DIR_SEP, filename);
   TtaFileCopy (source_file, s);
 }
}
#endif /* _WINGUI */

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

#ifdef _WINGUI
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
	    *t++ = *s++;
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
#endif /* _WINGUI */

/*********************
** Cache configuration menu
***********************/
/*----------------------------------------------------------------------
  GetCacheConf
  Makes a copy of the current registry cache values
  ----------------------------------------------------------------------*/
static void GetCacheConf (void)
{
  TtaGetEnvBoolean ("ENABLE_CACHE", &(GProp_Cache.EnableCache));
  TtaGetEnvBoolean ("CACHE_PROTECTED_DOCS", &(GProp_Cache.CacheProtectedDocs));
  TtaGetEnvBoolean ("CACHE_DISCONNECTED_MODE", &(GProp_Cache.CacheDisconnectMode));
  TtaGetEnvBoolean ("CACHE_EXPIRE_IGNORE", &(GProp_Cache.CacheExpireIgnore));
  GetEnvString ("CACHE_DIR", GProp_Cache.CacheDirectory);
  TtaGetEnvInt ("CACHE_SIZE", &(GProp_Cache.CacheSize));
  TtaGetEnvInt ("MAX_CACHE_ENTRY_SIZE", &( GProp_Cache.MaxCacheFile));
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

#ifdef _WINGUI
 /* validate the cache size */
 change = 1;
 if (GProp_Cache.CacheSize < 1)
   GProp_Cache.CacheSize =1;
 else if (GProp_Cache.CacheSize > 100)
   GProp_Cache.CacheSize = 100;
 else
   change = 0;
 if (change)
   SetDlgItemInt (CacheHwnd, IDC_CACHESIZE, GProp_Cache.CacheSize, FALSE);
 
 /* validate the cache entry size */
 change = 1;
 if ( GProp_Cache.MaxCacheFile < 1)
    GProp_Cache.MaxCacheFile = 1;
 else if ( GProp_Cache.MaxCacheFile > 5)
    GProp_Cache.MaxCacheFile = 5;
 else
   change = 0;
 if (change)
   SetDlgItemInt (CacheHwnd, IDC_MAXCACHEFILE,  GProp_Cache.MaxCacheFile, FALSE);
#endif /* _WINGUI */

 /* validate the cache dir */
 change = 0;
 change += CleanFirstLastSpace (GProp_Cache.CacheDirectory);
 change += CleanDirSep (GProp_Cache.CacheDirectory);
 /* remove the last DIR_SEP, if we have it */
 change += RemoveLastDirSep (GProp_Cache.CacheDirectory);
 if (GProp_Cache.CacheDirectory[0] == EOS)
 {
   GetDefEnvString ("CACHE_DIR", GProp_Cache.CacheDirectory);
   change = 1;
 }

 /* what we do is add a DIR_STRlibwww-cache */
 /* remove the last DIR_SEP, if we have it (twice, to 
    protect against a bad "user" default value */
 change += RemoveLastDirSep (GProp_Cache.CacheDirectory);
 /* n.b., this variable may be empty */
#ifdef _WINGUI
  change += NormalizeDirName (GProp_Cache.CacheDirectory, "\\libwww-cache");
#else
  change += NormalizeDirName (GProp_Cache.CacheDirectory, "/libwww-cache");
#endif /* _WINGUI */
  if (change)
#ifdef _WINGUI
    SetDlgItemText (CacheHwnd, IDC_CACHEDIRECTORY, GProp_Cache.CacheDirectory);
#else
    TtaSetTextForm (CacheBase + mCacheDirectory, GProp_Cache.CacheDirectory);
#endif /* _WINGUI */
}

/*----------------------------------------------------------------------
  SetCacheConf
  Updates the registry cache values
  ----------------------------------------------------------------------*/
static void SetCacheConf (void)
{
  TtaSetEnvBoolean ("ENABLE_CACHE", GProp_Cache.EnableCache, TRUE);
  TtaSetEnvBoolean ("CACHE_PROTECTED_DOCS", GProp_Cache.CacheProtectedDocs, TRUE);
  TtaSetEnvBoolean ("CACHE_DISCONNECTED_MODE", GProp_Cache.CacheDisconnectMode, TRUE);
  TtaSetEnvBoolean ("CACHE_EXPIRE_IGNORE", GProp_Cache.CacheExpireIgnore, TRUE);
  TtaSetEnvString ("CACHE_DIR", GProp_Cache.CacheDirectory, TRUE);
  TtaSetEnvInt ("CACHE_SIZE", GProp_Cache.CacheSize, TRUE);
  TtaSetEnvInt ("MAX_CACHE_ENTRY_SIZE",  GProp_Cache.MaxCacheFile, TRUE);

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
  GetDefEnvToggle ("ENABLE_CACHE", &(GProp_Cache.EnableCache),
		   CacheBase + mCacheOptions, 0);
  GetDefEnvToggle ("CACHE_PROTECTED_DOCS", &(GProp_Cache.CacheProtectedDocs),
		   CacheBase + mCacheOptions, 1);
  GetDefEnvToggle ("CACHE_DISCONNECTED_MODE", &(GProp_Cache.CacheDisconnectMode),
		   CacheBase + mCacheOptions, 2);
  GetDefEnvToggle ("CACHE_EXPIRE_IGNORE", &(GProp_Cache.CacheExpireIgnore),
		   CacheBase + mCacheOptions, 3);
  GetDefEnvString ("CACHE_DIR", GProp_Cache.CacheDirectory);
  TtaGetDefEnvInt ("CACHE_SIZE", &(GProp_Cache.CacheSize));
  TtaGetDefEnvInt ("MAX_CACHE_ENTRY_SIZE", &( GProp_Cache.MaxCacheFile));
}

#ifdef _WINGUI
/*----------------------------------------------------------------------
  WIN_RefreshCacheMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void WIN_RefreshCacheMenu (HWND hwnDlg)
{
  CheckDlgButton (hwnDlg, IDC_ENABLECACHE, (GProp_Cache.EnableCache)
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_CACHEPROTECTEDDOCS, (GProp_Cache.CacheProtectedDocs)
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_CACHEDISCONNECTEDMODE, (GProp_Cache.CacheDisconnectMode)
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_CACHEEXPIREIGNORE, (GProp_Cache.CacheExpireIgnore)
		  ? BST_CHECKED : BST_UNCHECKED);
  SetDlgItemText (hwnDlg, IDC_CACHEDIRECTORY, GProp_Cache.CacheDirectory);
  SetDlgItemInt (hwnDlg, IDC_CACHESIZE, GProp_Cache.CacheSize, FALSE);
  SetDlgItemInt (hwnDlg, IDC_MAXCACHEFILE,  GProp_Cache.MaxCacheFile, FALSE);
}
#else /* _WINGUI */
/*----------------------------------------------------------------------
  RefreshCacheMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void RefreshCacheMenu ()
{
#ifdef _GTK
  /* set the menu entries to the current values */
  TtaSetToggleMenu (CacheBase + mCacheOptions, 0, GProp_Cache.EnableCache);
  TtaSetToggleMenu (CacheBase + mCacheOptions, 1, GProp_Cache.CacheProtectedDocs);
  TtaSetToggleMenu (CacheBase + mCacheOptions, 2, GProp_Cache.CacheDisconnectMode);
  TtaSetToggleMenu (CacheBase + mCacheOptions, 3, GProp_Cache.CacheExpireIgnore);
  if (GProp_Cache.CacheDirectory)
    TtaSetTextForm (CacheBase + mCacheDirectory, GProp_Cache.CacheDirectory);
  TtaSetNumberForm (CacheBase + mCacheSize, GProp_Cache.CacheSize);
  TtaSetNumberForm (CacheBase + mMaxCacheFile,  GProp_Cache.MaxCacheFile);
#endif /* _GTK */
}
#endif /* !_WINGUI */

#ifdef _WINGUI
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
	      GetDlgItemText (hwnDlg, IDC_CACHEDIRECTORY, GProp_Cache.CacheDirectory,
			      sizeof (GProp_Cache.CacheDirectory) - 1);
	      CacheStatus |= AMAYA_CACHE_RESTART;
	      break;
	    case IDC_CACHESIZE:
	      GProp_Cache.CacheSize = GetDlgItemInt (hwnDlg, IDC_CACHESIZE,
						     FALSE, FALSE);
	      CacheStatus |= AMAYA_CACHE_RESTART;
	      break;
	    case IDC_MAXCACHEFILE:
	       GProp_Cache.MaxCacheFile = GetDlgItemInt (hwnDlg, IDC_MAXCACHEFILE,
							 FALSE, FALSE);
	      CacheStatus |= AMAYA_CACHE_RESTART;
	      break;	
	    default:
	      break;
	    }
	}
      switch (LOWORD (wParam))
	{
	case IDC_ENABLECACHE:
	  CacheStatus |= AMAYA_CACHE_RESTART;
	  GProp_Cache.EnableCache = !(GProp_Cache.EnableCache);
	  break;
	case IDC_CACHEPROTECTEDDOCS:
	  CacheStatus |= AMAYA_CACHE_RESTART;
	  GProp_Cache.CacheProtectedDocs = !(GProp_Cache.CacheProtectedDocs);
	  break;
	case IDC_CACHEDISCONNECTEDMODE:
	  CacheStatus |= AMAYA_CACHE_RESTART;
	  GProp_Cache.CacheDisconnectMode = !(GProp_Cache.CacheDisconnectMode);
	  break;
	case IDC_CACHEEXPIREIGNORE:
	  CacheStatus |= AMAYA_CACHE_RESTART;
	  GProp_Cache.CacheExpireIgnore = !(GProp_Cache.CacheExpireIgnore);
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
	case IDCANCEL:
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
#else /* _WINGUI */
/*----------------------------------------------------------------------
  CacheCallbackDialog
  callback of the cache configuration menu
  ----------------------------------------------------------------------*/
static void CacheCallbackDialog (int ref, int typedata, char *data)
{
  intptr_t            val;

  if (ref == -1)
    {
      /* removes the cache conf menu */
      TtaDestroyDialogue (CacheBase + CacheMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (intptr_t) data;
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
#ifdef _WX
	      /* force the cache restart because this is a long task to write the code to know if a widget status has changed or not */
	      CacheStatus |= AMAYA_CACHE_RESTART;
#endif /* _WX */
	      libwww_updateNetworkConf (CacheStatus);
	      /* reset the status flag */
	      CacheStatus = 0;
#ifndef _WX
	      TtaDestroyDialogue (ref);
#endif /* _WX */
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
	      GProp_Cache.EnableCache = !(GProp_Cache.EnableCache);
	      break;
	    case 1:
	      CacheStatus |= AMAYA_CACHE_RESTART;
	      GProp_Cache.CacheProtectedDocs = !(GProp_Cache.CacheProtectedDocs);
	      break;
	    case 2:
	      CacheStatus |= AMAYA_CACHE_RESTART;
	      GProp_Cache.CacheDisconnectMode = !(GProp_Cache.CacheDisconnectMode);
	      break;
	    case 3:
	      CacheStatus |= AMAYA_CACHE_RESTART;
	      GProp_Cache.CacheExpireIgnore = !(GProp_Cache.CacheExpireIgnore);
	      break;
	    default:
	      break;
	    }
	  break;
	case mCacheDirectory:
	  CacheStatus |= AMAYA_CACHE_RESTART;
	  if (data)
	    strcpy (GProp_Cache.CacheDirectory, data);
	  else
	    GProp_Cache.CacheDirectory[0] = EOS;
	  break;
	case mCacheSize:
	  CacheStatus |= AMAYA_CACHE_RESTART;
	  GProp_Cache.CacheSize = val;
	  break;
	case mMaxCacheFile:
	  CacheStatus |= AMAYA_CACHE_RESTART;
	   GProp_Cache.MaxCacheFile = val;
	  break;
	default:
	  break;
	}
    }
}
#endif /* !_WINGUI */

/*----------------------------------------------------------------------
  CacheConfMenu
  Build and display the Conf Menu dialog box and prepare for input.
  ----------------------------------------------------------------------*/
void CacheConfMenu (Document document, View view)
{
#ifndef _WINGUI
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
#endif /* !_WINGUI */
   /* reset the modified flag */
   CacheStatus = 0;
   /* load and display the current values */
   GetCacheConf ();
#ifndef _WINGUI
   RefreshCacheMenu ();
  /* display the menu */
   TtaSetDialoguePosition ();
   TtaShowDialogue (CacheBase + CacheMenu, TRUE);
#else /* !_WINGUI */
  if (!CacheHwnd)
    /* only activate the menu if it isn't active already */
	  DialogBox (hInstance, MAKEINTRESOURCE (CACHEMENU), NULL, 
		     (DLGPROC) WIN_CacheDlgProc);
  else
    SetFocus (CacheHwnd);
#endif /* !_WINGUI */
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
  GetEnvString ("HTTP_PROXY", GProp_Proxy.HttpProxy);
  GetEnvString ("PROXYDOMAIN", GProp_Proxy.ProxyDomain);
  TtaGetEnvBoolean ("PROXYDOMAIN_IS_ONLYPROXY",
		    &(GProp_Proxy.ProxyDomainIsOnlyProxy));
}

/*----------------------------------------------------------------------
  SetProxyConf
  Updates the registry proxy values
  ----------------------------------------------------------------------*/
static void SetProxyConf (void)
{
  TtaSetEnvString ("HTTP_PROXY", GProp_Proxy.HttpProxy, TRUE);
  TtaSetEnvString ("PROXYDOMAIN", GProp_Proxy.ProxyDomain, TRUE);
  TtaSetEnvBoolean ("PROXYDOMAIN_IS_ONLYPROXY",
		    GProp_Proxy.ProxyDomainIsOnlyProxy, TRUE);

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
  GetDefEnvString ("HTTP_PROXY", GProp_Proxy.HttpProxy);
  GetDefEnvString ("PROXYDOMAIN", GProp_Proxy.ProxyDomain);
  TtaGetDefEnvBoolean ("PROXYDOMAIN_IS_ONLYPROXY",
		       &(GProp_Proxy.ProxyDomainIsOnlyProxy));
}

#ifdef _WINGUI
/*----------------------------------------------------------------------
  WIN_RefreshProxyMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
void WIN_RefreshProxyMenu (HWND hwnDlg)
{
  SetDlgItemText (hwnDlg, IDC_HTTPPROXY, GProp_Proxy.HttpProxy);
  SetDlgItemText (hwnDlg, IDC_PROXYDOMAIN, GProp_Proxy.ProxyDomain);
  if (GProp_Proxy.ProxyDomainIsOnlyProxy)
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
  TtaSetTextForm (ProxyBase + mHttpProxy, GProp_Proxy.HttpProxy);
  TtaSetTextForm (ProxyBase + mProxyDomain, GProp_Proxy.ProxyDomain);
  TtaSetMenuForm (ProxyBase + mToggleProxy,GProp_Proxy. ProxyDomainIsOnlyProxy);
}
#endif /* !_WINGUI */

#ifdef _WINGUI
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
	      GetDlgItemText (hwnDlg, IDC_HTTPPROXY, GProp_Proxy.HttpProxy,
			      sizeof (GProp_Proxy.HttpProxy) - 1);
	      ProxyStatus |= AMAYA_PROXY_RESTART;
	      break;
	    case IDC_PROXYDOMAIN:
	      GetDlgItemText (hwnDlg, IDC_PROXYDOMAIN, GProp_Proxy.ProxyDomain,
			      sizeof (GProp_Proxy.ProxyDomain) - 1);
	      ProxyStatus |= AMAYA_PROXY_RESTART;
	      break;
	    }
	}
      switch (LOWORD (wParam))
	{
	  /* switch buttons */
	case IDC_NOPROXY:
	  GProp_Proxy.ProxyDomainIsOnlyProxy = FALSE;
	  ProxyStatus |= AMAYA_PROXY_RESTART;
	  break;
	case IDC_ONLYPROXY:
	  GProp_Proxy.ProxyDomainIsOnlyProxy = TRUE;
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
	case IDCANCEL:
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
#else /* _WINGUI */
/*----------------------------------------------------------------------
  ProxyCallbackDialog
  callback of the proxy configuration menu
  ----------------------------------------------------------------------*/
static void ProxyCallbackDialog (int ref, int typedata, char *data)
{
  intptr_t            val;

  if (ref == -1)
    {
      /* removes the proxy conf menu */
      TtaDestroyDialogue (ProxyBase + ProxyMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (intptr_t) data;
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
#ifndef _WX
	      TtaDestroyDialogue (ref);
#endif /* _WX */
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
	    strcpy (GProp_Proxy.HttpProxy, data);
	  else
	    GProp_Proxy.HttpProxy[0] = EOS;
	  break;

	case mProxyDomain:
	  ProxyStatus |= AMAYA_PROXY_RESTART;
	  if (data)
	    strcpy (GProp_Proxy.ProxyDomain, data);
	  else
	    GProp_Proxy.ProxyDomain[0] = EOS;
	  break;

	case mToggleProxy:
	  ProxyStatus |= AMAYA_PROXY_RESTART;
	  switch (val) 
	    {
	    case 0:
	      GProp_Proxy.ProxyDomainIsOnlyProxy = FALSE;
	      break;
	    case 1:
	      GProp_Proxy.ProxyDomainIsOnlyProxy = TRUE;
	      break;
	    }
	  break;

	default:
	  break;
	}
    }
}
#endif /* !_WINGUI */

/*----------------------------------------------------------------------
  ProxyConfMenu
  Build and display the Conf Menu dialog box and prepare for input.
  ----------------------------------------------------------------------*/
void         ProxyConfMenu (Document document, View view)
{
#ifndef _WINGUI
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
		  0 /* no maxlength */,
		  TRUE);
#endif /* !_WINGUI */

   /* reset the modified flag */
   ProxyStatus = 0;
   /* load and display the current values */
   GetProxyConf ();
#ifndef _WINGUI
   RefreshProxyMenu ();
  /* display the menu */
   TtaSetDialoguePosition ();
   TtaShowDialogue (ProxyBase + ProxyMenu, TRUE);
#else
  if (!ProxyHwnd)
    /* only activate the menu if it isn't active already */
     DialogBox (hInstance, MAKEINTRESOURCE (PROXYMENU), NULL,
		(DLGPROC) WIN_ProxyDlgProc);
  else
     SetFocus (ProxyHwnd);
#endif /* !_WINGUI */
}


/**********************
** General configuration menu
***********************/
/*----------------------------------------------------------------------
  GetGeneralConf
  Makes a copy of the current registry General values
  ----------------------------------------------------------------------*/
void GetGeneralConf (void)
{
  char       ptr[MAX_LENGTH];
  int        oldzoom;

  TtaGetEnvInt ("FontZoom", &(GProp_General.Zoom));
  if (GProp_General.Zoom == 0)
    {
      TtaGetEnvInt ("ZOOM", &oldzoom);
      if (oldzoom == 0)
	GProp_General.Zoom = 100;
      else
	/* old model */
	GProp_General.Zoom = 100 + (oldzoom * 10);
    }
  TtaGetEnvBoolean ("PASTE_LINE_BY_LINE", &(GProp_General.PasteLineByLine));
  TtaGetEnvInt ("AUTO_SAVE", &AutoSave_Interval);
  GProp_General.S_AutoSave = (AutoSave_Interval > 0);
  TtaGetEnvBoolean ("SHOW_BUTTONS", &(GProp_General.S_Buttons));
  TtaGetEnvBoolean ("SHOW_ADDRESS", &(GProp_General.S_Address));
  TtaGetEnvBoolean ("FONT_ALIASING", &(GProp_General.S_NoAliasing));
  TtaGetEnvBoolean ("ISO_DATE", &(GProp_General.S_DATE));
  TtaGetEnvBoolean ("SHOW_TARGET", &(GProp_General.S_Targets));
  TtaGetEnvBoolean ("SAVE_GEOMETRY", &(GProp_General.S_Geometry));
  GetEnvString ("HOME_PAGE", GProp_General.HomePage);
  GetEnvString ("LANG", GProp_General.DialogueLang);
  GetEnvString ("ACCESSKEY_MOD", ptr);
  if (!strcmp (ptr, "Alt"))
    GProp_General.AccesskeyMod = 0;
  else if (!strcmp (ptr, "Ctrl"))
    GProp_General.AccesskeyMod = 1;
  else
    GProp_General.AccesskeyMod = 2;
  TtaGetEnvInt ("FontMenuSize", &(GProp_General.FontMenuSize));
#ifdef _WINGUI
  GetEnvString ("APP_TMPDIR", AppTmpDir);
  GetEnvString ("APP_HOME", AppHome);
#endif /* _WINGUI */
}

/*----------------------------------------------------------------------
  ValidateGeneralConf
  Validates the entries in the General conf menu. If there's an invalid
  entry, we then use the default value. We need this because
  the Windows interface isn't rich enough to do it (e.g., negative numbers
  in the integer entries)
  ----------------------------------------------------------------------*/
void ValidateGeneralConf (void)
{
  int         change;
  char        lang[3];
  char       *ptr;
#ifdef _WINGUI
  char        old_AppTmpDir[MAX_LENGTH];
  int         i;

  /* normalize and validate the zoom factor */
  SetDlgItemInt (GeneralHwnd, IDC_ZOOM, GProp_General.Zoom, TRUE);

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
#endif /* _WINGUI */

  /* validate the dialogue language */
  change = 0;
  ptr = TtaGetEnvString ("THOTDIR");
  if (strcmp (GProp_General.DialogueLang, "en-US"))
    {
      change++;
      GProp_General.DialogueLang[2] = EOS;
    }
  strncpy (lang, GProp_General.DialogueLang, 2);
  lang[2] = EOS;
  sprintf (s, "%s%cconfig%c%s-amayamsg", ptr, DIR_SEP, DIR_SEP, lang);
  if (!TtaFileExist (s))
  {
    GetDefEnvString ("LANG", GProp_General.DialogueLang);
    change++;
  }
  if (change)
#ifdef _WINGUI
    SetDlgItemText (GeneralHwnd, IDC_DIALOGUELANG, GProp_General.DialogueLang);
#else
    TtaSetTextForm (GeneralBase + mDialogueLang, GProp_General.DialogueLang);
#endif /* WINDOWS */
}

/*----------------------------------------------------------------------
  RecalibrateZoom
  Moves the Zoom setting on all documents to the specified value
  ----------------------------------------------------------------------*/
static void RecalibrateZoom ()
{
  int               zoom;
  int               doc, view;

  /* recalibrate the zoom settings in all the active documents and
   active views */
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
  UpdateShowTargets
  Sets the show targets on all documents
  ----------------------------------------------------------------------*/
static void UpdateShowTargets ()
{
  int               visibility;
  int               doc;

  /* recalibrate settings in all the active documents and
   active views */
  for (doc = 1; doc < DocumentTableLength; doc++)
    {
      if (DocumentURLs[doc])
	{
	  visibility = TtaGetSensibility (doc, 1);
	  if ((visibility == 5 && GProp_General.S_Targets) ||
	      (visibility == 4 && !(GProp_General.S_Targets)))
	    /* change the status of the document */
	    ShowTargets (doc, 1);
	}
    }
}

#ifndef _WX
/*----------------------------------------------------------------------
  UpdateShowButtons
  Sets the show buttons on all documents
  ----------------------------------------------------------------------*/
static void UpdateShowButtons ()
{
  int               doc;

  for (doc = 1; doc < DocumentTableLength; doc++)
    {
      if (DocumentURLs[doc] &&
          SButtons[doc] != GProp_General.S_Buttons &&
          (DocumentTypes[doc] == docHTML ||
           DocumentTypes[doc] == docSVG ||
           DocumentTypes[doc] == docXml ||
           DocumentTypes[doc] == docMath))
        /* generate numbers */
        ShowButtons (doc, 1);
      else
        /* update only the indicator */
        SButtons[doc] = GProp_General.S_Buttons;
    }
}

/*----------------------------------------------------------------------
  UpdateShowAddress
  Sets the show address on all documents
  ----------------------------------------------------------------------*/
static void UpdateShowAddress ()
{
  int               doc;
  
  for (doc = 1; doc < DocumentTableLength; doc++)
    {
      if (DocumentURLs[doc] &&
          SAddress[doc] != GProp_General.S_Address &&
          (DocumentTypes[doc] == docHTML ||
           DocumentTypes[doc] == docSVG ||
           DocumentTypes[doc] == docXml ||
           DocumentTypes[doc] == docMath))
        /* generate numbers */
        ShowAddress (doc, 1);
      else
        /* update only the indicator */
        SAddress[doc] = GProp_General.S_Address;
    }
}
#endif /* _WX */


/*----------------------------------------------------------------------
  SetGeneralConf
  Updates the registry General values and calls the General functions
  to take into account the changes
  ----------------------------------------------------------------------*/
void SetGeneralConf (void)
{
  int         oldVal;
  ThotBool    old;

  TtaGetEnvInt ("FontZoom", &oldVal);
  if (oldVal != GProp_General.Zoom)
    {
      TtaSetEnvInt ("FontZoom", GProp_General.Zoom, TRUE);
      TtaSetFontZoom (GProp_General.Zoom);
      /* recalibrate the zoom settings in all the active documents */
      RecalibrateZoom ();
    }
  TtaSetEnvBoolean ("PASTE_LINE_BY_LINE", GProp_General.PasteLineByLine, TRUE);
#ifdef _WX
  /* wx use its own callbacks and use only the boolean value : S_AutoSave */
  if (GProp_General.S_AutoSave)
    AutoSave_Interval = DEF_SAVE_INTVL;
  else
    AutoSave_Interval = 0;
#endif /* _WX */
  TtaGetEnvInt ("AUTO_SAVE", &oldVal);
  if (oldVal != AutoSave_Interval)
    {
      TtaSetEnvInt ("AUTO_SAVE", AutoSave_Interval, TRUE);
      TtaSetDocumentBackUpInterval (AutoSave_Interval);
      GProp_General.S_AutoSave = (AutoSave_Interval > 0);
    }
  /* handling show buttons, address, targets and section numbering */
#ifndef _WX
  TtaGetEnvBoolean ("SHOW_BUTTONS", &old);
  TtaSetEnvBoolean ("SHOW_BUTTONS", GProp_General.S_Buttons, TRUE);
  if (old != GProp_General.S_Buttons)
    UpdateShowButtons ();
  TtaGetEnvBoolean ("SHOW_ADDRESS", &old);
  TtaSetEnvBoolean ("SHOW_ADDRESS", GProp_General.S_Address, TRUE);
  if (old != GProp_General.S_Address)
    UpdateShowAddress ();
#endif /* _WX */
  TtaGetEnvBoolean ("SHOW_TARGET", &old);
  TtaSetEnvBoolean ("SHOW_TARGET", GProp_General.S_Targets, TRUE);
  if (old != GProp_General.S_Targets)
    UpdateShowTargets ();

  TtaSetEnvBoolean ("FONT_ALIASING", GProp_General.S_NoAliasing, TRUE);
  TtaSetEnvBoolean ("ISO_DATE", GProp_General.S_DATE, TRUE);

  /* Save view geometry on exit */
  TtaSetEnvBoolean ("SAVE_GEOMETRY", GProp_General.S_Geometry, TRUE);

  TtaSetEnvString ("HOME_PAGE", GProp_General.HomePage, TRUE);
  TtaSetEnvString ("LANG", GProp_General.DialogueLang, TRUE);
  if (GProp_General.AccesskeyMod == 0)
    TtaSetEnvString ("ACCESSKEY_MOD", "Alt", TRUE);
  else if (GProp_General.AccesskeyMod == 1)
    TtaSetEnvString ("ACCESSKEY_MOD", "Ctrl", TRUE);
  else
    TtaSetEnvString ("ACCESSKEY_MOD", "None", TRUE);
#ifndef _WX
  TtaSetEnvInt ("FontMenuSize", GProp_General.FontMenuSize, TRUE);
#endif /* _WX */
#ifdef _WINGUI
  TtaSetEnvString ("APP_TMPDIR", AppTmpDir, TRUE);
  strcpy (TempFileDirectory, AppTmpDir);
  TtaAppendDocumentPath (TempFileDirectory);
#endif /* _WINGUI */

  TtaSaveAppRegistry ();
}

/*----------------------------------------------------------------------
  GetDefaultGeneralConf
  Loads the default registry General values
  ----------------------------------------------------------------------*/
void GetDefaultGeneralConf ()
{
  char       ptr[MAX_LENGTH];

  TtaGetDefEnvInt ("FontZoom", &(GProp_General.Zoom));
  if (GProp_General.Zoom == 0)
    GProp_General.Zoom = 100;
  GetDefEnvToggle ("PASTE_LINE_BY_LINE", &(GProp_General.PasteLineByLine), 
		   GeneralBase + mToggleGeneral, 0);
  TtaGetDefEnvInt ("AUTO_SAVE", &AutoSave_Interval);
  GProp_General.S_AutoSave = (AutoSave_Interval > 0);
  GetDefEnvToggle ("AUTO_SAVE", &(GProp_General.S_AutoSave),
		   GeneralBase + mToggleGeneral, 1);
  GetDefEnvToggle ("SAVE_GEOMETRY", &(GProp_General.S_Geometry),
		   GeneralBase + mToggleGeneral, 2);
  GetDefEnvToggle ("SHOW_BUTTONS", &(GProp_General.S_Buttons),
		   GeneralBase + mToggleGeneral, 3);
  GetDefEnvToggle ("SHOW_ADDRESS", &(GProp_General.S_Address),
		   GeneralBase + mToggleGeneral, 4);
  GetDefEnvToggle ("SHOW_TARGET", &(GProp_General.S_Targets),
		   GeneralBase + mToggleGeneral, 5);
#ifdef _WX
  GetDefEnvToggle ("ISO_DATE", &(GProp_General.S_DATE),
		   GeneralBase + mToggleGeneral, 6);
  GetDefEnvToggle ("FONT_ALIASING", &(GProp_General.S_NoAliasing),
		   GeneralBase + mToggleGeneral, 7);
#endif /* _WX */
  GetDefEnvString ("HOME_PAGE", GProp_General.HomePage);
  GetDefEnvString ("LANG", GProp_General.DialogueLang);
  GetDefEnvString ("ACCESSKEY_MOD", ptr);
  if (!strcmp (ptr, "Alt"))
    GProp_General.AccesskeyMod = 0;
  else if (!strcmp (ptr, "Ctrl"))
    GProp_General.AccesskeyMod = 1;
  else
    GProp_General.AccesskeyMod = 2;
  TtaGetDefEnvInt ("FontMenuSize", &(GProp_General.FontMenuSize));
#ifdef _WINGUI
  GetDefEnvString ("APP_TMPDIR", AppTmpDir);
  GetDefEnvString ("APP_HOME", AppHome);
#endif /* _WINGUI */
}

#ifdef _WINGUI
/*----------------------------------------------------------------------
  WIN_RefreshGeneralMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
void WIN_RefreshGeneralMenu (HWND hwnDlg)
{
  SetDlgItemText (hwnDlg, IDC_HOMEPAGE, GProp_General.HomePage);
  CheckDlgButton (hwnDlg, IDC_LINES, GProp_General.PasteLineByLine 
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_AUTOSAVE, GProp_General.S_AutoSave 
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_SAVE_GEOMETRY_EXIT, GProp_General.S_Geometry 
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_SHOWBUTTONS, GProp_General.S_Buttons 
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_SHOWADDRESS, GProp_General.S_Address 
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_SHOWTARGET, GProp_General.S_Targets
		  ? BST_CHECKED : BST_UNCHECKED);
  SetDlgItemText (hwnDlg, IDC_DIALOGUELANG, GProp_General.DialogueLang);
  switch (GProp_General.AccesskeyMod)
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
  SetDlgItemInt (hwnDlg, IDC_ZOOM, GProp_General.Zoom, TRUE);
  SetDlgItemText (hwnDlg, IDC_TMPDIR, AppTmpDir);
  SetDlgItemText (hwnDlg, IDC_APPHOME, AppHome);
}

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
      SetWindowText (GetDlgItem (hwnDlg, IDC_AUTOSAVE),
		     TtaGetMessage (AMAYA, AM_AUTO_SAVE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_SAVE_GEOMETRY_EXIT),
		     TtaGetMessage (AMAYA, AM_SAVE_GEOMETRY_ON_EXIT));
      SetWindowText (GetDlgItem (hwnDlg, IDC_SHOWBUTTONS),
		     TtaGetMessage (AMAYA, AM_SHOW_BUTTONBAR));
      SetWindowText (GetDlgItem (hwnDlg, IDC_SHOWADDRESS),
		     TtaGetMessage (AMAYA, AM_SHOW_TEXTZONE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_SHOWTARGET),
		     TtaGetMessage (AMAYA, AM_SHOW_TARGETS));
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
	      GetDlgItemText (hwnDlg, IDC_HOMEPAGE, GProp_General.HomePage, 
			      sizeof (GProp_General.HomePage) - 1);
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
	      GetDlgItemText (hwnDlg, IDC_DIALOGUELANG, GProp_General.DialogueLang,
			      sizeof (GProp_General.DialogueLang) - 1);
		  break;
	    case IDC_ZOOM:
	      GProp_General.Zoom = GetDlgItemInt (hwnDlg, IDC_ZOOM, FALSE, TRUE);
	      break;	
	    }
	}
      switch (LOWORD (wParam))
	{
	 case IDC_AALT:
	   GProp_General.AccesskeyMod = 0;
	   break;
  	 case IDC_ACTRL:
	   GProp_General.AccesskeyMod = 1;
	   break;
   	 case IDC_ANONE:
	   GProp_General.AccesskeyMod = 2;
	   break;
	case IDC_LINES:
	  GProp_General.PasteLineByLine = !(GProp_General.PasteLineByLine);
	  break;
	case IDC_AUTOSAVE:
	  GProp_General.S_AutoSave = !(GProp_General.S_AutoSave);
	  if (GProp_General.S_AutoSave)
	    AutoSave_Interval = DEF_SAVE_INTVL;
	  else
	    AutoSave_Interval = 0;
	  break;
	case IDC_SAVE_GEOMETRY_EXIT:
	  GProp_General.S_Geometry = !(GProp_General.S_Geometry);
	  break;
	case IDC_SHOWBUTTONS:
	  GProp_General.S_Buttons = !(GProp_General.S_Buttons);
	  break;
	case IDC_SHOWADDRESS:
	  GProp_General.S_Address = !(GProp_General.S_Address);
	  break;
	case IDC_SHOWTARGET:
	  GProp_General.S_Targets = !(GProp_General.S_Targets);
	  break;

	  /* action buttons */
	case ID_APPLY:
	  ValidateGeneralConf ();
	  SetGeneralConf ();	  
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DONE:
	case IDCANCEL:
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

#else /* _WINGUI */
/*----------------------------------------------------------------------
  RefreshGeneralMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void RefreshGeneralMenu ()
{
#ifdef _GTK
  TtaSetNumberForm (GeneralBase + mZoom, GProp_General.Zoom);
  TtaSetToggleMenu (GeneralBase + mToggleGeneral, 0, GProp_General.PasteLineByLine);
  TtaSetToggleMenu (GeneralBase + mToggleGeneral, 1, GProp_General.S_AutoSave);
  TtaSetToggleMenu (GeneralBase + mToggleGeneral, 2, GProp_General.S_Geometry);
  TtaSetToggleMenu (GeneralBase + mToggleGeneral, 3, GProp_General.S_Buttons);
  TtaSetToggleMenu (GeneralBase + mToggleGeneral, 4, GProp_General.S_Address);
  TtaSetToggleMenu (GeneralBase + mToggleGeneral, 5, GProp_General.S_Targets);
#ifdef _WX
  TtaSetToggleMenu (GeneralBase + mToggleGeneral, 6, GProp_General.S_DATE);
  TtaSetToggleMenu (GeneralBase + mToggleGeneral, 7, GProp_General.S_NoAliasing);
#endif /* _WX */
  TtaSetTextForm (GeneralBase + mHomePage, GProp_General.HomePage);
  TtaSetTextForm (GeneralBase + mDialogueLang, GProp_General.DialogueLang);
  TtaSetMenuForm (GeneralBase + mGeneralAccessKey, GProp_General.AccesskeyMod);
  TtaSetNumberForm (GeneralBase + mFontMenuSize, GProp_General.FontMenuSize);
#endif /* _GTK */
}

/*----------------------------------------------------------------------
   callback of the general menu
  ----------------------------------------------------------------------*/
static void GeneralCallbackDialog (int ref, int typedata, char *data)
{
  intptr_t            val;

  if (ref == -1)
    {
      /* removes the network conf menu */
      TtaDestroyDialogue (GeneralBase + GeneralMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (intptr_t) data;
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
#ifndef _WX
	      TtaDestroyDialogue (ref);
#endif /* _WX */
	      break;
	    case 2:
	      GetDefaultGeneralConf ();
	      RefreshGeneralMenu ();
	      break;
	    default:
	      break;
	    }
	  break;

	case mZoom:
	  GProp_General.Zoom = val;
	  break;

	case mHomePage:
	  if (data)
	    strcpy (GProp_General.HomePage, data);
	  else
	    GProp_General.HomePage[0] = EOS;
	  break;

	case mToggleGeneral:
	  switch (val) 
	    {
	    case 0:
	      GProp_General.PasteLineByLine = !(GProp_General.PasteLineByLine);
	      break;
	    case 1:
	      GProp_General.S_AutoSave = !(GProp_General.S_AutoSave);
	      if (GProp_General.S_AutoSave)
		AutoSave_Interval = DEF_SAVE_INTVL;
	      else
		AutoSave_Interval = 0;	      
	      break;
	    case 2:
	      GProp_General.S_Geometry = !(GProp_General.S_Geometry);
	      break;
	    case 3:
	      GProp_General.S_Buttons = !(GProp_General.S_Buttons);
	      break;
	    case 4:
	      GProp_General.S_Address = !(GProp_General.S_Address);
	      break;
	    case 5:
	      GProp_General.S_Targets = !(GProp_General.S_Targets);
	      break;
	    case 6:
	      GProp_General.S_DATE = !(GProp_General.S_DATE);
	      break;
	    case 7:
	      GProp_General.S_NoAliasing = !(GProp_General.S_NoAliasing);
	      break;
	    }
	  break;

#ifndef _WX
	case mFontMenuSize:
	  GProp_General.FontMenuSize = val;
	  break;
#endif /* _WX */
	  
	case mDialogueLang:
	  if (data)
	    strcpy (GProp_General.DialogueLang, data);
	  else
	    GProp_General.DialogueLang[0] = EOS;
	  break;
	  
	case mGeneralAccessKey:
	  GProp_General.AccesskeyMod = val;
	  break;

	default:
	  break;
	}
    }
}
#endif /* !_WINGUI */

/*----------------------------------------------------------------------
  GeneralConfMenu
  Build and display the Browsing Editing conf Menu dialog box and prepare 
  for input.
  ----------------------------------------------------------------------*/
void GeneralConfMenu (Document document, View view)
{
#ifdef _GTK
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
   /*TtaNewLabel (GeneralBase + mGeneralEmpty2, GeneralBase + GeneralMenu, " ");*/
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
		     10,
		     1000,
		     FALSE);   
   /* fourth line */
   TtaNewTextForm (GeneralBase + mDialogueLang,
		   GeneralBase + GeneralMenu,
		   TtaGetMessage (AMAYA, AM_DIALOGUE_LANGUAGE),
		   3,
		   1,
		   FALSE);
   TtaNewLabel (GeneralBase + mGeneralEmpty3, GeneralBase + GeneralMenu, " ");
   /*TtaNewLabel (GeneralBase + mGeneralEmpty4, GeneralBase + GeneralMenu, " ");*/
   /* second line */
   sprintf (s, "B%s%cB%s%cB%s%cB%s%cB%s%cB%s%c", 
	     TtaGetMessage (AMAYA, AM_PASTE_LINE_BY_LINE), EOS, 
	     TtaGetMessage (AMAYA, AM_AUTO_SAVE), EOS, 
	     TtaGetMessage (AMAYA, AM_SAVE_GEOMETRY_ON_EXIT), EOS, 
	     TtaGetMessage (AMAYA, AM_SHOW_BUTTONBAR), EOS,
	     TtaGetMessage (AMAYA, AM_SHOW_TEXTZONE), EOS,
	    TtaGetMessage (AMAYA, AM_SHOW_TARGETS), EOS);

   TtaNewToggleMenu (GeneralBase + mToggleGeneral,
		     GeneralBase + GeneralMenu,
		     NULL,
		     6,
		     s,
		     NULL,
		     FALSE);
   sprintf (s, "BAlt%cBCtrl%cB%s", EOS, EOS,
	    TtaGetMessage (AMAYA, AM_NONE));   
   TtaNewSubmenu (GeneralBase + mGeneralAccessKey,
		  GeneralBase + GeneralMenu,
		  0,
		  TtaGetMessage (AMAYA, AM_ACCESSKEY),
		  3,
		  s,
		  NULL,
		  0 /* no maxlength */,
		  FALSE);

   /* load the current values */
   GetGeneralConf ();

   RefreshGeneralMenu ();
   /* display the menu */
   TtaSetDialoguePosition ();
   TtaShowDialogue (GeneralBase + GeneralMenu, TRUE);
#endif /* _GTK */

#ifdef _WX
   PreferenceMenu( document, view );
#endif /* _WX */

#ifdef _WINGUI
   /* load the current values */
   GetGeneralConf ();

   if (!GeneralHwnd)
     /* only activate the menu if it isn't active already */
     DialogBox (hInstance, MAKEINTRESOURCE (GENERALMENU), NULL,
		(DLGPROC) WIN_GeneralDlgProc);
   else
     SetFocus (GeneralHwnd);
#endif /* !_WINGUI */
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
  TtaGetEnvBoolean ("ENABLE_XHTML_MIMETYPE", &(GProp_Publish.UseXHTMLMimeType));
  TtaGetEnvBoolean ("ENABLE_LOST_UPDATE_CHECK", &(GProp_Publish.LostUpdateCheck));
  TtaGetEnvBoolean ("VERIFY_PUBLISH", &(GProp_Publish.VerifyPublish));
  TtaGetEnvBoolean ("EXPORT_CRLF", &(GProp_Publish.ExportCRLF));
  GetEnvString ("DEFAULTNAME", GProp_Publish.DefaultName);
  GetEnvString ("SAFE_PUT_REDIRECT", GProp_Publish.SafePutRedirect);
  GetEnvString ("DOCUMENT_CHARSET", GProp_Publish.CharsetType);
}

/*----------------------------------------------------------------------
  SetPublishConf
  Updates the registry Publish values and calls the Publish functions
  to take into account the changes
  ----------------------------------------------------------------------*/
static void SetPublishConf (void)
{
  TtaSetEnvBoolean ("ENABLE_XHTML_MIMETYPE", GProp_Publish.UseXHTMLMimeType, TRUE);
  TtaSetEnvBoolean ("ENABLE_LOST_UPDATE_CHECK", GProp_Publish.LostUpdateCheck, TRUE);
  TtaSetEnvBoolean ("VERIFY_PUBLISH", GProp_Publish.VerifyPublish, TRUE);
  TtaSetEnvBoolean ("EXPORT_CRLF", GProp_Publish.ExportCRLF, TRUE);
  TtaSetEnvString ("DEFAULTNAME", GProp_Publish.DefaultName, TRUE);
  TtaSetEnvString ("SAFE_PUT_REDIRECT", GProp_Publish.SafePutRedirect, TRUE);
  TtaSetEnvString ("DOCUMENT_CHARSET", GProp_Publish.CharsetType, TRUE);

  TtaSaveAppRegistry ();
}

/*----------------------------------------------------------------------
  GetDefaultPublishConf
  Loads the default registry Publish values
  ----------------------------------------------------------------------*/
static void GetDefaultPublishConf ()
{
  GetDefEnvToggle ("ENABLE_XHTML_MIMETYPE", &(GProp_Publish.UseXHTMLMimeType), 
		   PublishBase + mTogglePublish, 0);
  GetDefEnvToggle ("ENABLE_LOST_UPDATE_CHECK", &(GProp_Publish.LostUpdateCheck), 
		    PublishBase + mTogglePublish, 1);
  GetDefEnvToggle ("VERIFY_PUBLISH", &(GProp_Publish.VerifyPublish),
		    PublishBase + mTogglePublish, 2);
  GetDefEnvToggle ("EXPORT_CRLF", &(GProp_Publish.ExportCRLF),
		    PublishBase + mTogglePublish, 3);
  GetDefEnvString ("DEFAULTNAME", GProp_Publish.DefaultName);
  GetDefEnvString ("SAFE_PUT_REDIRECT", GProp_Publish.SafePutRedirect);
  GetDefEnvString ("DOCUMENT_CHARSET", GProp_Publish.CharsetType);
}

#ifdef _WINGUI
/*----------------------------------------------------------------------
  BuildCharsetList builds the list allowing to select a charset
  (for windows)
  ----------------------------------------------------------------------*/
static void BuildCharsetList (void)
{
  int         nbcharset = sizeof(CharsetTxt) / sizeof(char *);
  int         i = 0;
  int         i_default = -1;

  CurrentCharset = -1;
  /* Get the propositions of the list */ 
  SendMessage (CharsetList, LB_RESETCONTENT, 0, 0);
  while (i < nbcharset && CharsetTxt[i] != EOS)
    {
      /* keep in mind the current selected entry */
      if (GProp_Publish.CharsetType &&
		  !strcmp (GProp_Publish.CharsetType, CharsetTxt[i]))
	CurrentCharset = i;
      if (!strcasecmp (CharsetTxt[i], "iso-8859-1"))
	i_default = i;
      SendMessage (CharsetList, LB_INSERTSTRING, i, (LPARAM) CharsetTxt[i]);
      i++;
    }
  if (CurrentCharset == -1)
    CurrentCharset = i_default;
  SendMessage (CharsetList, LB_SETCURSEL, (WPARAM)CurrentCharset, (LPARAM)0);

  if (GProp_Publish.CharsetType)
  strcpy (NewCharset, GProp_Publish.CharsetType);
}

/*----------------------------------------------------------------------
  WIN_RefreshPublishMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
void WIN_RefreshPublishMenu (HWND hwnDlg)
{
  CheckDlgButton (hwnDlg, IDC_LOSTUPDATECHECK, (GProp_Publish.LostUpdateCheck)
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_VERIFYPUBLISH, (GProp_Publish.VerifyPublish)
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_USEXHTMLMIMETYPE, (GProp_Publish.UseXHTMLMimeType)
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_CRLF, (GProp_Publish.ExportCRLF) 
		  ? BST_CHECKED : BST_UNCHECKED);
  SetDlgItemText (hwnDlg, IDC_DEFAULTNAME, GProp_Publish.DefaultName);
  SetDlgItemText (hwnDlg, IDC_SAFEPUTREDIRECT, GProp_Publish.SafePutRedirect);
  BuildCharsetList ();
}

/*----------------------------------------------------------------------
  WIN_PublishDlgProc
  Windows callback for the publish menu
  ----------------------------------------------------------------------*/
LRESULT CALLBACK WIN_PublishDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam,
				     LPARAM lParam)
{ 
 int       itemIndex = 0;
	
	switch (msg)
    {
    case WM_INITDIALOG:
      PublishHwnd = hwnDlg;
      CharsetList = GetDlgItem (hwnDlg, IDC_CHARSET_LIST);
      WIN_SetDialogfont (CharsetList);
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
	      GetDlgItemText (hwnDlg, IDC_DEFAULTNAME,
			      GProp_Publish.DefaultName,
			      sizeof (GProp_Publish.DefaultName) - 1);
	      break;

	    case IDC_SAFEPUTREDIRECT:
	      GetDlgItemText (hwnDlg, IDC_SAFEPUTREDIRECT,
			      GProp_Publish.SafePutRedirect,
			      sizeof (GProp_Publish.SafePutRedirect) - 1);
	      SafePutStatus |= AMAYA_SAFEPUT_RESTART;
	      break;
	    }
	}
      switch (LOWORD (wParam))
	{
	case IDC_USEXHTMLMIMETYPE:
	  GProp_Publish.UseXHTMLMimeType = !(GProp_Publish.UseXHTMLMimeType);
	  break;
	case IDC_LOSTUPDATECHECK:
	  GProp_Publish.LostUpdateCheck = !(GProp_Publish.LostUpdateCheck);
	  break;
	case IDC_VERIFYPUBLISH:
	  GProp_Publish.VerifyPublish = !(GProp_Publish.VerifyPublish);
	  break;
	case IDC_CRLF:
	  GProp_Publish.ExportCRLF = !(GProp_Publish.ExportCRLF);
	  break;
	  /* action buttons */
	case ID_APPLY:
	  if (strcmp (GProp_Publish.CharsetType, NewCharset))
	    strcpy (GProp_Publish.CharsetType, NewCharset);
	  SetPublishConf ();	  
	  libwww_updateNetworkConf (SafePutStatus);
	  /* reset the status flag */
	  SafePutStatus = 0;
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DONE:
	case IDCANCEL:
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

     switch (HIWORD (wParam))
	 {
	  case LBN_SELCHANGE:
	    itemIndex = SendMessage (CharsetList, LB_GETCURSEL, 0, 0);
	    CurrentCharset = SendMessage (CharsetList, LB_GETTEXT, itemIndex,
		  		     (LPARAM) NewCharset);
	     SetDlgItemText (hwnDlg, IDC_PROFILENAME, NewCharset);
	  break;
	 }
     break;
    default: return FALSE;
    }
  return TRUE;
}

#else /* _WINGUI */
/*----------------------------------------------------------------------
  RefreshPublishMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void RefreshPublishMenu ()
{
#ifdef _GTK
  TtaSetToggleMenu (PublishBase + mTogglePublish, 0, GProp_Publish.UseXHTMLMimeType);
  TtaSetToggleMenu (PublishBase + mTogglePublish, 1, GProp_Publish.LostUpdateCheck);
  TtaSetToggleMenu (PublishBase + mTogglePublish, 2, GProp_Publish.VerifyPublish);
  TtaSetTextForm (PublishBase + mDefaultName, GProp_Publish.DefaultName);
  TtaSetTextForm (PublishBase + mSafePutRedirect, GProp_Publish.SafePutRedirect);
#endif /* _GTK */
}

/*----------------------------------------------------------------------
  BuildCharsetSelector
  builds the list allowing to select a default charset (for unix)
  ----------------------------------------------------------------------*/
static void BuildCharsetSelector (void)
{
  int         i, i_default;
  int         nbcharset = sizeof(CharsetTxt) / sizeof(char *);
  int         indx, length;
  char       *entry;
  char        BufMenu[MAX_LENGTH];

  /* recopy the propositions  */
  i_default = -1;
  indx = 0;
  CurrentCharset = -1;
  for (i = 0; i < nbcharset; i++)
    {
      entry = CharsetTxt[i];
      /* keep in mind the current selected entry */
      if (GProp_Publish.CharsetType && !strcasecmp (GProp_Publish.CharsetType, entry))
	CurrentCharset = i;
      if (!strcasecmp (entry, "iso-8859-1"))
	i_default = i;
      length = strlen (entry) + 1;
      if (length + indx < MAX_LENGTH)  
	{
	  strcpy (&BufMenu[indx], entry);
	  indx += length;
	}
    }

  /* Set the default charset to utf-8 if it doesn't exist */
  if (CurrentCharset == -1)
    CurrentCharset = i_default;

  /* Fill in the charset form  */
  TtaNewSizedSelector (PublishBase + mCharsetSelector, PublishBase + PublishMenu,
		       TtaGetMessage (AMAYA, AM_DEFAULT_CHARSET), nbcharset,
		       ((i < 2) ? (char *)"" : BufMenu), 3, 2, NULL, FALSE, FALSE);
 if (nbcharset)
    TtaSetSelector (PublishBase + mCharsetSelector, CurrentCharset, NULL);
 strcpy (NewCharset, GProp_Publish.CharsetType);
}
/*----------------------------------------------------------------------
   callback of the Publishing menu
  ----------------------------------------------------------------------*/
static void PublishCallbackDialog (int ref, int typedata, char *data)
{
  intptr_t            val;

  if (ref == -1)
    {
      /* removes the network conf menu */
      TtaDestroyDialogue (PublishBase + PublishMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (intptr_t) data;
      switch (ref - PublishBase)
	{
	case PublishMenu:
	  switch (val) 
	    {
	    case 0:
	      TtaDestroyDialogue (ref);
	      break;
	    case 1:
#ifdef _WX
	      strcpy (NewCharset, GProp_Publish.CharsetType);
	      /* force SafePut refresh */
	      SafePutStatus |= AMAYA_SAFEPUT_RESTART;
#endif /* _WX */
	      strcpy (GProp_Publish.CharsetType, NewCharset);
	      SetPublishConf ();
	      libwww_updateNetworkConf (SafePutStatus);
	      /* reset the status flag */
	      SafePutStatus = 0;
#ifndef _WX
	      TtaDestroyDialogue (ref);
#endif /* _WX */
	      break;
	    case 2:
	      GetDefaultPublishConf ();
	      BuildCharsetSelector ();
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
	      GProp_Publish.UseXHTMLMimeType = !(GProp_Publish.UseXHTMLMimeType);
	      break;
	    case 1:
	      GProp_Publish.LostUpdateCheck = !(GProp_Publish.LostUpdateCheck);
	      break;
	    case 2:
	      GProp_Publish.VerifyPublish = !(GProp_Publish.VerifyPublish);
	      break;
	    }
	  break;

	case mDefaultName:
	  if (data)
	    strcpy (GProp_Publish.DefaultName, data);
	  else
	    GProp_Publish.DefaultName[0] = EOS;
	  break;

	case mSafePutRedirect:
	  if (data)
	    strcpy (GProp_Publish.SafePutRedirect, data);
	  else
	    GProp_Publish.SafePutRedirect[0] = EOS;
	  SafePutStatus |= AMAYA_SAFEPUT_RESTART;
	  break;

	case mCharsetSelector:
	  /* Get the desired charset from the item number */
	  strcpy (NewCharset, data);
	  break;
	  
	default:
	  break;
	}
    }
}
#endif /* !_WINGUI */

/*----------------------------------------------------------------------
  PublishConfMenu
  Build and display the Browsing Editing conf Menu dialog box and prepare 
  for input.
  ----------------------------------------------------------------------*/
void PublishConfMenu (Document document, View view)
{
#ifndef _WINGUI
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
   sprintf (s, "B%s%cB%s%cB%s", 
	    TtaGetMessage (AMAYA, AM_USE_XHTML_MIMETYPE), EOS,
	    TtaGetMessage (AMAYA, AM_USE_ETAGS), EOS, 
	    TtaGetMessage (AMAYA, AM_VERIFY_PUT));
   /* load the current values */
   GetPublishConf ();
   /* Build the charset selector */
   BuildCharsetSelector ();
   TtaNewToggleMenu (PublishBase + mTogglePublish,
		     PublishBase + PublishMenu,
		     NULL,
		     3,
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
#endif /* !_WINGUI */
   /* reset the modified flag */
   SafePutStatus = 0;

   /* display the menu */
#ifndef _WINGUI
   RefreshPublishMenu ();
   TtaSetDialoguePosition ();
   TtaShowDialogue (PublishBase + PublishMenu, TRUE);
#else
    /* load and display the current values */
   GetPublishConf ();

   if (!PublishHwnd)
	   DialogBox (hInstance, MAKEINTRESOURCE (PUBLISHMENU), NULL, 
	  (DLGPROC) WIN_PublishDlgProc);
  else
     SetFocus (PublishHwnd);
#endif /* !_WINGUI */
}


/**********************
** Browse menu
***********************/
/*----------------------------------------------------------------------
  GetBrowseConf
  Makes a copy of the current registry Browse values
  ----------------------------------------------------------------------*/
void GetBrowseConf (void)
{
  TtaGetEnvInt ("OPENING_LOCATION", &(GProp_Browse.OpeningLocation));
  TtaGetEnvBoolean ("LOAD_IMAGES", &(GProp_Browse.LoadImages));
  TtaGetEnvBoolean ("LOAD_OBJECTS", &(GProp_Browse.LoadObjects));
  TtaGetEnvBoolean ("ENABLE_BG_IMAGES", &(GProp_Browse.BgImages));
  TtaGetEnvBoolean ("LOAD_CSS", &(GProp_Browse.LoadCss));
  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &(GProp_Browse.DoubleClick));
  TtaGetEnvBoolean ("ENABLE_FTP", &(GProp_Browse.EnableFTP));
  TtaGetEnvBoolean ("SHOW_CONFIRM_CLOSE_TAB", &(GProp_Browse.WarnCTab));
  AHTFTPURL_flag_set (GProp_Browse.EnableFTP);
  GetEnvString ("SCREEN_TYPE", GProp_Browse.ScreenType);
  TtaGetEnvInt ("DOUBLECLICKDELAY", &(GProp_Browse.DoubleClickDelay));
  GetEnvString ("ACCEPT_LANGUAGES", GProp_Browse.LanNeg);
  TtaGetEnvInt ("MAX_URL_LIST", &(GProp_Browse.MaxURL));
}

/*----------------------------------------------------------------------
  SetBrowseConf
  Updates the registry Browse values and calls the Browse functions
  to take into account the changes
  ----------------------------------------------------------------------*/
void SetBrowseConf (void)
{
  TtaSetEnvInt ("OPENING_LOCATION", GProp_Browse.OpeningLocation, TRUE);
  TtaSetEnvBoolean ("LOAD_IMAGES", GProp_Browse.LoadImages, TRUE);
  TtaSetEnvBoolean ("LOAD_OBJECTS", GProp_Browse.LoadObjects, TRUE);
  TtaSetEnvBoolean ("ENABLE_BG_IMAGES", GProp_Browse.BgImages, TRUE);
  TtaSetEnvBoolean ("LOAD_CSS", GProp_Browse.LoadCss, TRUE);
  TtaSetEnvBoolean ("ENABLE_DOUBLECLICK", GProp_Browse.DoubleClick, TRUE);
  /* @@@ */
  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &(GProp_Browse.DoubleClick));
  /* @@@ */
  TtaSetEnvBoolean ("ENABLE_FTP", GProp_Browse.EnableFTP, TRUE);
  TtaSetEnvBoolean ("SHOW_CONFIRM_CLOSE_TAB", GProp_Browse.WarnCTab, TRUE);
  AHTFTPURL_flag_set (GProp_Browse.EnableFTP);
  TtaSetEnvString ("SCREEN_TYPE", GProp_Browse.ScreenType, TRUE);
  TtaSetEnvString ("ACCEPT_LANGUAGES", GProp_Browse.LanNeg, TRUE);
  /* change the current settings */
  libwww_updateNetworkConf (AMAYA_LANNEG_RESTART);
  TtaSetEnvInt ("MAX_URL_LIST", GProp_Browse.MaxURL, TRUE);
  TtaSaveAppRegistry ();
}

/*----------------------------------------------------------------------
  ApplyConfigurationChanges
  Updates displayed documents according to current changes.
  ----------------------------------------------------------------------*/
void ApplyConfigurationChanges (void)
{
  DisplayMode       dispMode;
  int               doc;
  
  for (doc = 1; doc < MAX_DOCUMENTS; doc++)
    {
      if (DocumentURLs[doc] &&
          (DocumentTypes[doc] == docHTML ||
           DocumentTypes[doc] == docSVG ||
           DocumentTypes[doc] == docMath))
        {
          dispMode = TtaGetDisplayMode (doc);
          TtaSetDisplayMode (doc, NoComputedDisplay);
          RedisplayDoc (doc);
          TtaSetDisplayMode (doc, dispMode);
        }
    }
}

/*----------------------------------------------------------------------
  GetDefaultBrowseConf
  Loads the default registry Browse values
  ----------------------------------------------------------------------*/
void GetDefaultBrowseConf ()
{
  GetDefEnvToggle ("LOAD_IMAGES", &(GProp_Browse.LoadImages),
		   BrowseBase + mToggleBrowse, 0);
  GetDefEnvToggle ("LOAD_OBJECTS", &(GProp_Browse.LoadObjects),
		   BrowseBase + mToggleBrowse, 1);
  GetDefEnvToggle ("ENABLE_BG_IMAGES", &(GProp_Browse.BgImages),
		   BrowseBase + mToggleBrowse, 2);
  GetDefEnvToggle ("LOAD_CSS", &(GProp_Browse.LoadCss),
		   BrowseBase + mToggleBrowse, 3);
  GetDefEnvToggle ("ENABLE_DOUBLECLICK", &(GProp_Browse.DoubleClick),
		   BrowseBase + mToggleBrowse, 4);
  GetDefEnvToggle ("ENABLE_FTP", &(GProp_Browse.EnableFTP),
		   BrowseBase + mToggleBrowse, 5);
  GetDefEnvString ("SCREEN_TYPE", GProp_Browse.ScreenType);
  TtaGetDefEnvInt ("DOUBLECLICKDELAY", &(GProp_Browse.DoubleClickDelay));
  GetDefEnvString ("ACCEPT_LANGUAGES", GProp_Browse.LanNeg);
  TtaGetDefEnvBoolean ("SHOW_CONFIRM_CLOSE_TAB", &(GProp_Browse.WarnCTab));
  TtaGetDefEnvInt ("OPENING_LOCATION", &(GProp_Browse.OpeningLocation));
  TtaGetDefEnvInt ("MAX_URL_LIST", &(GProp_Browse.MaxURL));
}

#ifdef _WINGUI
/*----------------------------------------------------------------------
  BuildScreensList builds the list allowing to select a screen type
  (for windows)
  ----------------------------------------------------------------------*/
static void BuildScreensList (void)
{
  int           nbscreens = sizeof(ScreensTxt) / sizeof(char *);
  int           i = 0;

  /* Get the propositions of the list */ 
  SendMessage (ScreensList, LB_RESETCONTENT, 0, 0);
  while (i < nbscreens && ScreensTxt[i] != EOS)
    {
      /* keep in mind the current selected entry */
      if (GProp_Browse.ScreenType && !strcmp (GProp_Browse.ScreenType, ScreensTxt[i]))
	CurrentScreen = i;
      SendMessage (ScreensList, LB_INSERTSTRING, i, (LPARAM) ScreensTxt[i]);
      i++;
    }
  SendMessage (ScreensList, LB_SETCURSEL, (WPARAM)CurrentScreen, (LPARAM)0);
}

/*----------------------------------------------------------------------
  WIN_RefreshBrowseMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
void WIN_RefreshBrowseMenu (HWND hwnDlg)
{
  CheckDlgButton (hwnDlg, IDC_LOADIMG, (GProp_Browse.LoadImages) 
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_LOADOBJ, (GProp_Browse.LoadObjects) 
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_BGIMAGES, (GProp_Browse.BgImages) 
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_LOADCSS, (GProp_Browse.LoadCss) 
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_DOUBLECLICK, (GProp_Browse.DoubleClick) 
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_ENABLEFTP, (GProp_Browse.EnableFTP) 
		  ? BST_CHECKED : BST_UNCHECKED);
  BuildScreensList ();
  SetDlgItemText (hwnDlg, IDC_LANNEG, GProp_Browse.LanNeg);
}

/*----------------------------------------------------------------------
  WIN_BrowseDlgProc
  Windows callback for the browse menu
  ----------------------------------------------------------------------*/
LRESULT CALLBACK WIN_BrowseDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam,
				    LPARAM lParam)
{ 
  int               itemIndex = 0;

  switch (msg)
    {
    case WM_INITDIALOG:
      BrowseHwnd = hwnDlg;
      ScreensList = GetDlgItem (hwnDlg, IDC_SCREENLIST);
      WIN_SetDialogfont (ScreensList);
      /* initialize the menu text */
      WIN_SetMenuText (hwnDlg, WIN_BrowseMenuText);
      /* write the current values in the dialog entries */
      WIN_RefreshBrowseMenu (hwnDlg);
      break;

    case WM_CLOSE:
    case WM_DESTROY:
      /* reset the status flag */
      BrowseHwnd = NULL;
      EndDialog (hwnDlg, ID_DONE);
      break; 

    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case IDC_LOADIMG:
	  GProp_Browse.LoadImages = !(GProp_Browse.LoadImages);
	  break;
	case IDC_LOADOBJ:
	  GProp_Browse.LoadObjects = !(GProp_Browse.LoadObjects);
	  break;
	case IDC_BGIMAGES:
	  GProp_Browse.BgImages = !(GProp_Browse.BgImages);
	  break;
	case IDC_LOADCSS:
	  GProp_Browse.LoadCss = !(GProp_Browse.LoadCss);
	  break;
	case IDC_DOUBLECLICK:
	  GProp_Browse.DoubleClick = !(GProp_Browse.DoubleClick);
	  break;
	case IDC_ENABLEFTP:
	  GProp_Browse.EnableFTP = !(GProp_Browse.EnableFTP);
	  break;
	case IDC_SCREENLIST:
	  CurrentScreen = SendMessage (ScreensList, LB_GETCURSEL, 0, 0);
	  CurrentScreen = SendMessage (ScreensList, LB_GETTEXT, CurrentScreen,
				   (LPARAM) GProp_Browse.ScreenType);
	case IDC_LANNEG:
	  GetDlgItemText (hwnDlg, IDC_LANNEG, GProp_Browse.LanNeg,
			  sizeof (GProp_Browse.LanNeg) - 1);
	  break;

	  /* action buttons */
	case ID_APPLY:
	  if (strcmp (GProp_Browse.ScreenType, InitScreen) ||
	      InitWarnCTab != GProp_Browse.WarnCTab ||
	      InitOpeningLocation != GProp_Browse.OpeningLocation ||
	      InitLoadImages != GProp_Browse.LoadImages ||
	      InitLoadObjects != GProp_Browse.LoadObjects ||	      
	      InitLoadCss != GProp_Browse.LoadCss)
	    {
	      strcpy (InitScreen, GProp_Browse.ScreenType);
	      SetBrowseConf ();
	      ApplyConfigurationChanges ();
	      InitOpeningLocation = GProp_Browse.OpeningLocation;
	      InitLoadImages = GProp_Browse.LoadImages;
	      InitLoadObjects = GProp_Browse.LoadObjects;	      
	      InitLoadCss = GProp_Browse.LoadCss;
        InitWarnCTab = GProp_Browse.WarnCTab;
	    }
	  else
	    SetBrowseConf ();
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DONE:
	case IDCANCEL:
	  /* reset the status flag */
	  BrowseHwnd = NULL;
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DEFAULTS:
	  /* always signal this as modified */
	  GetDefaultBrowseConf ();
	  WIN_RefreshBrowseMenu (hwnDlg);
	  break;
	}
      break;
    default: return FALSE;
    }
  return TRUE;
}

#else /* WINDOWS */
#ifndef _WX
/*----------------------------------------------------------------------
  BuildScreenSelector builds the list allowing to select a screen type
  (for unix)
  ----------------------------------------------------------------------*/
static void BuildScreenSelector (void)
{
  int         i;
  int         nbscreens = sizeof(ScreensTxt) / sizeof(char *);
  int         indx, length;
  char       *entry;
  char        BufMenu[MAX_LENGTH];
  
  /* recopy the propositions  */
  indx = 0;
  CurrentScreen = -1;
  for (i = 0; i < nbscreens; i++)
    {
      entry =  ScreensTxt[i];
      /* keep in mind the current selected entry */
      if (GProp_Browse.ScreenType &&
          !strcmp (GProp_Browse.ScreenType, ScreensTxt[i]))
        CurrentScreen = i;
      length = strlen (entry) + 1;
      if (length + indx < MAX_LENGTH)  
        {
          strcpy (&BufMenu[indx], entry);
	  indx += length;
        }
    }
  
  /* Fill in the screen list form  */
  TtaNewSizedSelector (BrowseBase + mScreenSelector, BrowseBase + BrowseMenu,
                       TtaGetMessage (AMAYA, AM_SCREEN_TYPE), nbscreens,
                       ((i < 2) ? (char *)"" : BufMenu), 3, 2, NULL, FALSE, FALSE);
  /* preselect the screen matching the user preference */
  TtaSetSelector (BrowseBase + mScreenSelector, CurrentScreen, NULL);
}
#endif /* _WX */

/*----------------------------------------------------------------------
  RefreshBrowseMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void RefreshBrowseMenu ()
{
#ifdef _GTK
  TtaSetToggleMenu (BrowseBase + mToggleBrowse, 0, GProp_Browse.LoadImages);
  TtaSetToggleMenu (BrowseBase + mToggleBrowse, 1, GProp_Browse.LoadObjects);
  TtaSetToggleMenu (BrowseBase + mToggleBrowse, 2, GProp_Browse.BgImages);
  TtaSetToggleMenu (BrowseBase + mToggleBrowse, 3, GProp_Browse.LoadCss);
  TtaSetToggleMenu (BrowseBase + mToggleBrowse, 4, GProp_Browse.DoubleClick);
  TtaSetToggleMenu (BrowseBase + mToggleBrowse, 5, GProp_Browse.EnableFTP);
  /* preselect the screen matching the user preference */
  BuildScreenSelector ();
  TtaSetTextForm (BrowseBase + mLanNeg, GProp_Browse.LanNeg);
#endif /* _GTK */
}

/*----------------------------------------------------------------------
   callback of the Browsing menu
  ----------------------------------------------------------------------*/
static void BrowseCallbackDialog (int ref, int typedata, char *data)
{
  intptr_t          val;

  if (ref == -1)
    /* removes the network conf menu */
    TtaDestroyDialogue (BrowseBase + BrowseMenu);
  else
    {
      /* has the user changed the options? */
      val = (intptr_t) data;
      switch (ref - BrowseBase)
        {
        case BrowseMenu:
          switch (val) 
            {

            case 0:
              TtaDestroyDialogue (ref);
              break;

            case 1:
              if (strcmp (GProp_Browse.ScreenType, InitScreen) ||
                  InitWarnCTab != GProp_Browse.WarnCTab ||
                  InitOpeningLocation != GProp_Browse.OpeningLocation ||
                  InitLoadImages != GProp_Browse.LoadImages ||
                  InitLoadObjects != GProp_Browse.LoadObjects ||
                  InitBgImages != GProp_Browse.BgImages ||
                  InitLoadCss != GProp_Browse.LoadCss )
                {
                  /* there is almost a change */
                  if (strcmp (GProp_Browse.ScreenType, InitScreen) ||
                      InitLoadImages != GProp_Browse.LoadImages ||
                      InitLoadObjects != GProp_Browse.LoadObjects ||
                      InitBgImages != GProp_Browse.BgImages ||
                      InitLoadCss != GProp_Browse.LoadCss)
                    {
                      /* redisplay documents after these changes */
                      strcpy (InitScreen, GProp_Browse.ScreenType);
                      SetBrowseConf ();
                      ApplyConfigurationChanges ();
                    }
                  else
                    SetBrowseConf ();
                  InitWarnCTab = GProp_Browse.WarnCTab;
                  InitOpeningLocation = GProp_Browse.OpeningLocation;
                  InitLoadImages = GProp_Browse.LoadImages;
                  InitLoadObjects = GProp_Browse.LoadObjects;
                  InitBgImages = GProp_Browse.BgImages;
                  InitLoadCss = GProp_Browse.LoadCss;
                }
              else
                SetBrowseConf ();
#ifndef _WX
              TtaDestroyDialogue (ref);
#endif /* _WX */
              break;

            case 2:
              GetDefaultBrowseConf ();
              RefreshBrowseMenu ();
              break;

            case 3:
              ClearURLList();
              break;

            default:
              break;
            }
          break;

        case mToggleBrowse:
          switch (val) 
            {
            case 0:
              GProp_Browse.LoadImages = !(GProp_Browse.LoadImages);
              break;
            case 1:
              GProp_Browse.LoadObjects = !(GProp_Browse.LoadObjects);
              break;
            case 2:
              GProp_Browse.BgImages = !(GProp_Browse.BgImages);
              break;
            case 3:
              GProp_Browse.LoadCss = !(GProp_Browse.LoadCss);
              break;
            case 4:
              GProp_Browse.DoubleClick = !(GProp_Browse.DoubleClick);
              break;
            case 5:
              GProp_Browse.EnableFTP = !(GProp_Browse.EnableFTP);
              break;
            }
          break;
        case mScreenSelector:
          /* Get the desired screen type from the item number */
          strcpy (GProp_Browse.ScreenType, data);
          break;

        case mLanNeg:
          if (data)
            strcpy (GProp_Browse.LanNeg, data);
          else
            GProp_Browse.LanNeg[0] = EOS;
          break;

        default:
          break;
        }
    }
}
#endif /* _WINGUI */

/*----------------------------------------------------------------------
  BrowseConfMenu
  Build and display the Browsing Editing conf Menu dialog box and prepare 
  for input.
  ----------------------------------------------------------------------*/
void BrowseConfMenu (Document document, View view)
{
#ifdef _GTK
  int              i;
#endif /* _GTK */

  /* reset the modified flag */
  SafePutStatus = 0;
  /* load the current values */
  GetBrowseConf ();
  /* keep initial values to detect an change */
  InitOpeningLocation = GProp_Browse.OpeningLocation;
  InitLoadImages = GProp_Browse.LoadImages;
  InitLoadObjects = GProp_Browse.LoadObjects;
  InitBgImages = GProp_Browse.BgImages;
  InitLoadCss = GProp_Browse.LoadCss;
  InitWarnCTab = GProp_Browse.WarnCTab;
  strcpy (InitScreen, GProp_Browse.ScreenType);
#ifdef _GTK
  /* Create the dialogue form */
  i = 0;
  strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));
  i += strlen (&s[i]) + 1;
  strcpy (&s[i], TtaGetMessage (AMAYA, AM_DEFAULT_BUTTON));
  
  TtaNewSheet (BrowseBase + BrowseMenu, 
	       TtaGetViewFrame (document, view),
	       TtaGetMessage (AMAYA, AM_BROWSE_MENU),
	       2, s, FALSE, 11, 'L', D_DONE);
  sprintf (s, "B%s%cB%s%cB%s%cB%s%cB%s%cB%s", 
	   TtaGetMessage (AMAYA, AM_LOAD_IMAGES), EOS,
	   TtaGetMessage (AMAYA, AM_LOAD_OBJECTS), EOS,
	   TtaGetMessage (AMAYA, AM_SHOW_BG_IMAGES), EOS, 
	   TtaGetMessage (AMAYA, AM_LOAD_CSS), EOS,
	   TtaGetMessage (AMAYA, AM_ENABLE_DOUBLECLICK), EOS, 
	   TtaGetMessage (AMAYA, AM_ENABLE_FTP));
  TtaNewToggleMenu (BrowseBase + mToggleBrowse,
		    BrowseBase + BrowseMenu,
		    NULL,
		    6, s,
		    NULL,
		    FALSE);
  BuildScreenSelector ();
  strcpy (InitScreen, GProp_Browse.ScreenType);
  /* last line */
  TtaNewTextForm (BrowseBase + mLanNeg, BrowseBase + BrowseMenu,
		   TtaGetMessage (AMAYA, AM_LANG_NEGOTIATION),
		   20, 1, FALSE);
  RefreshBrowseMenu ();
  TtaSetDialoguePosition ();
  TtaShowDialogue (BrowseBase + BrowseMenu, TRUE);
#endif /* _GTK */

#ifdef _WX
#endif /* _WX */

#ifdef _WINGUI
  if (!BrowseHwnd)
	   DialogBox (hInstance, MAKEINTRESOURCE (BROWSEMENU), NULL, 
		      (DLGPROC) WIN_BrowseDlgProc);
  else
    SetFocus (BrowseHwnd);
#endif /* _WINGUI */
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
  GetEnvString ("ForegroundColor", GProp_Color.FgColor);
  GetEnvString ("BackgroundColor", GProp_Color.BgColor);
  GetEnvString ("BgSelectColor", GProp_Color.BgSelColor);
  GetEnvString ("FgSelectColor", GProp_Color.FgSelColor);
#ifndef _WINGUI
  GetEnvString ("MenuFgColor", GProp_Color.MenuFgColor);
  GetEnvString ("MenuBgColor", GProp_Color.MenuBgColor);
#endif /* !_WINGUI */
}

/*----------------------------------------------------------------------
  GetDefaultColorConf
  Makes a copy of the default registry color values
  ----------------------------------------------------------------------*/
static void GetDefaultColorConf (void)
{
  GetDefEnvString ("ForegroundColor", GProp_Color.FgColor);
  GetDefEnvString ("BackgroundColor", GProp_Color.BgColor);
  GetDefEnvString ("BgSelectColor", GProp_Color.BgSelColor);
  GetDefEnvString ("FgSelectColor", GProp_Color.FgSelColor);
#ifndef _WINGUI
  GetDefEnvString ("MenuFgColor", GProp_Color.MenuFgColor);
  GetDefEnvString ("MenuBgColor", GProp_Color.MenuBgColor);
#endif /* !_WINGUI */
}

/*----------------------------------------------------------------------
  SetColorConf
  Updates the registry Color values
  ----------------------------------------------------------------------*/
static void SetColorConf (void)
{
  TtaSetEnvString ("ForegroundColor", GProp_Color.FgColor, TRUE);
  TtaSetEnvString ("BackgroundColor", GProp_Color.BgColor, TRUE);
  TtaSetEnvString ("BgSelectColor", GProp_Color.BgSelColor, TRUE);
  TtaSetEnvString ("FgSelectColor", GProp_Color.FgSelColor, TRUE);
#ifndef _WINGUI
  TtaSetEnvString ("MenuFgColor", GProp_Color.MenuFgColor, TRUE);
  TtaSetEnvString ("MenuBgColor", GProp_Color.MenuBgColor, TRUE);
#endif /* !_WINGUI */

  TtaSaveAppRegistry ();
  /* change the current settings */
  TtaUpdateEditorColors ();
}

#ifdef _WINGUI
/*----------------------------------------------------------------------
  WIN_RefreshColorMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
void WIN_RefreshColorMenu (HWND hwnDlg)
{
  SetDlgItemText (hwnDlg, IDC_FGCOLOR, GProp_Color.FgColor);
  SetDlgItemText (hwnDlg, IDC_BGCOLOR, GProp_Color.BgColor);
  SetDlgItemText (hwnDlg, IDC_FGSELCOLOR, GProp_Color.FgSelColor);
  SetDlgItemText (hwnDlg, IDC_BGSELCOLOR, GProp_Color.BgSelColor);
}
#else /* WINDOWS */
/*----------------------------------------------------------------------
  RefreshColorMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void RefreshColorMenu ()
{
  TtaSetTextForm (ColorBase + mFgColor, GProp_Color.FgColor);
  TtaSetTextForm (ColorBase + mBgColor, GProp_Color.BgColor);
  TtaSetTextForm (ColorBase + mBgSelColor, GProp_Color.BgSelColor);
  TtaSetTextForm (ColorBase + mFgSelColor, GProp_Color.FgSelColor);
  TtaSetTextForm (ColorBase + mMenuFgColor, GProp_Color.MenuFgColor);
  TtaSetTextForm (ColorBase + mMenuBgColor, GProp_Color.MenuBgColor);
}
#endif /* !_WINGUI */

#ifdef _WINGUI
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
	      GetDlgItemText (hwnDlg, IDC_FGCOLOR, GProp_Color.FgColor,
			      sizeof (GProp_Color.FgColor) - 1);
	      break;
	    case IDC_BGCOLOR:
	      GetDlgItemText (hwnDlg, IDC_BGCOLOR, GProp_Color.BgColor,
			      sizeof (GProp_Color.BgColor) - 1);
	      break;
	    case IDC_BGSELCOLOR:
	      GetDlgItemText (hwnDlg, IDC_BGSELCOLOR, GProp_Color.BgSelColor,
			      sizeof (GProp_Color.BgSelColor) - 1);
	      break;
	    case IDC_FGSELCOLOR:
	      GetDlgItemText (hwnDlg, IDC_FGSELCOLOR, GProp_Color.FgSelColor,
			      sizeof (GProp_Color.FgSelColor) - 1);
	      break;
	    }
	}
      switch (LOWORD (wParam))
	{
	  /* action buttons */
	case IDC_CHANGCOLOR:
	  TtcGetPaletteColors (&fgcolor, &bgcolor, TRUE);
	  if (fgcolor != -1)
	    strcpy (GProp_Color.FgColor, ColorName (fgcolor));
	  if (bgcolor != -1)
	    strcpy (GProp_Color.BgColor, ColorName (bgcolor));
	  WIN_RefreshColorMenu (ColorHwnd);
	  SetFocus (ColorHwnd);
	  break;
	case IDC_CHANGCOLOR2:
	  TtcGetPaletteColors (&fgcolor, &bgcolor, FALSE);
	  if (fgcolor != -1)
	    strcpy (GProp_Color.FgSelColor, ColorName (fgcolor));
	  if (bgcolor != -1)
	    strcpy (GProp_Color.BgSelColor, ColorName (bgcolor));
	  WIN_RefreshColorMenu (ColorHwnd);
	  SetFocus (ColorHwnd);
	  break;
	case ID_APPLY:
	  SetColorConf ();	  
	  /* reset the status flag */
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DONE:
    case IDCANCEL:
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
#else /* _WINGUI */
/*----------------------------------------------------------------------
   callback of the color configuration menu
  ----------------------------------------------------------------------*/
static void ColorCallbackDialog (int ref, int typedata, char *data)
{
  intptr_t val;

  if (ref == -1)
    {
      /* removes the color conf menu */
      TtaDestroyDialogue (ColorBase + ColorMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (intptr_t) data;
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
#ifndef _WX
	      TtaDestroyDialogue (ref);
#endif /* _WX */
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
	    strcpy (GProp_Color.FgColor, data);
	  else
	    GProp_Color.FgColor[0] = EOS;
	  break;
	case mBgColor:
	  if (data)
	    strcpy (GProp_Color.BgColor, data);
	  else
	    GProp_Color.BgColor[0] = EOS;
	  break;
	case mBgSelColor:
	  if (data)
	    strcpy (GProp_Color.BgSelColor, data);
	  else
	    GProp_Color.BgSelColor[0] = EOS;
	  break;
	case mFgSelColor:
	  if (data)
	    strcpy (GProp_Color.FgSelColor, data);
	  else
	    GProp_Color.FgSelColor[0] = EOS;
	  break;
	case mMenuFgColor:
	  if (data)
	    strcpy (GProp_Color.MenuFgColor, data);
	  else
	    GProp_Color.MenuFgColor[0] = EOS;
	  break;
	case mMenuBgColor:
	  if (data)
	    strcpy (GProp_Color.MenuBgColor, data);
	  else
	    GProp_Color.MenuBgColor[0] = EOS;
	  break;

	default:
	  break;
	}
    }
}
#endif /* !_WINGUI */

/*----------------------------------------------------------------------
  ColorConfMenu
  Build and display the Conf Menu dialog box and prepare for input.
  ----------------------------------------------------------------------*/
void         ColorConfMenu (Document document, View view)
{
#ifndef _WINGUI
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
		   25,
		   1,
		   FALSE);   
   TtaNewTextForm (ColorBase + mBgColor,
		   ColorBase + ColorMenu,
		   TtaGetMessage (AMAYA, AM_DOC_BG_COLOR),
		   25,
		   1,
		   FALSE);   
   /* second col */
   TtaNewTextForm (ColorBase + mFgSelColor,
		   ColorBase + ColorMenu,
		   TtaGetMessage (AMAYA, AM_FG_SEL_COLOR),
		   25,
		   1,
		   FALSE);   
   TtaNewTextForm (ColorBase + mBgSelColor,
		   ColorBase + ColorMenu,
		   TtaGetMessage (AMAYA, AM_BG_SEL_COLOR),
		   25,
		   1,
		   FALSE);   
   /* third col */
   TtaNewTextForm (ColorBase + mMenuFgColor,
		   ColorBase + ColorMenu,
		   TtaGetMessage (AMAYA, AM_MENU_FG_COLOR),
		   25,
		   1,
		   FALSE);   
   TtaNewTextForm (ColorBase + mMenuBgColor,
		   ColorBase + ColorMenu,
		   TtaGetMessage (AMAYA, AM_MENU_BG_COLOR),
		   25,
		   1,
		   FALSE);

   TtaNewLabel (ColorBase + mColorEmpty1, ColorBase + ColorMenu,
		TtaGetMessage (AMAYA, AM_GEOMETRY_CHANGE));
     
#endif /* !_WINGUI */
 
   /* load and display the current values */
   GetColorConf ();
#ifndef _WINGUI
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
#endif /* !_WINGUI */
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
static void SetEnvGeom (const char *view_name, Document doc)
{
  int x, y, w, h;
  int view;
  
#ifdef _WX
  view = 1;
#else /* _WX */
  if (strcmp (view_name, "Annot_Formatted_view") &&
      strcmp (view_name, "Source_view") &&
      strcmp (view_name, "Topics_Formatted_view"))
    {
      view = TtaGetViewFromName (doc, (char *)view_name);
      if (view == 0 || !TtaIsViewOpen (doc, view))
        return;
    }
  else
    /* takes the current size and position of the main view */
    view = 1;
#endif /* _WX */
  
  /* get current geometry */
  if (!TtaGetViewMaximized(doc, view))
    TtaSetEnvBoolean("WINDOW_MAXIMIZED", FALSE, TRUE);
  else
    TtaSetEnvBoolean("WINDOW_MAXIMIZED", TRUE, TRUE);      

  if (!TtaGetViewMaximized(doc, view) && !TtaGetViewIconized(doc, view) && !TtaGetViewFullscreen(doc, view))
  {
    // do not save the window size if the window is maximized, iconized or fullscreen
    // because this is a separated state
    TtaGetViewXYWH (doc, view, &x, &y, &w, &h);
    sprintf(s, "%d %d %d %d", x, y, w, h);
    TtaSetEnvString((char *)view_name, s, TRUE);
  }
}

/*----------------------------------------------------------------------
  RestoreDefaultGeometryConf
  Makes a copy of the default registry geometry values
  ----------------------------------------------------------------------*/
static void RestoreDefaultGeometryConf (void)
{
  int   doc;

  for (doc = 1; doc < DocumentTableLength; doc++)
    if (DocumentURLs[doc] != NULL &&
	DocumentTypes[doc] != docSource &&
	DocumentTypes[doc] != docLog && 
	DocumentTypes[doc] != docLibrary )
      {
	if (DocumentTypes[doc] == docAnnot)
	  RestoreDefEnvGeom ("Annot_Formatted_view", doc);
	else if (DocumentTypes[doc] == docBookmark)
	  RestoreDefEnvGeom ("Topics_Formatted_view", doc);
	else
	  {
#ifdef _WX
	    RestoreDefEnvGeom ("Wx_Window", doc);
#else /* _WX */
	    RestoreDefEnvGeom ("Formatted_view", doc);
	    RestoreDefEnvGeom ("Structure_view", doc);
	    RestoreDefEnvGeom ("Alternate_view", doc);
	    RestoreDefEnvGeom ("Links_view", doc);
	    RestoreDefEnvGeom ("Table_of_contents", doc);
	    if (DocumentSource[doc])
	      RestoreDefEnvGeom ("Source_view", DocumentSource[doc]);
#endif /* _WX */
	  }
      }
  /* save the options */
  TtaSaveAppRegistry ();
}

/*----------------------------------------------------------------------
  SetEnvCurrentGeometry stores the current doc geometry in the registry
  ----------------------------------------------------------------------*/
static void SetEnvCurrentGeometry (int doc, const char * view_name)
{
  /* only do the processing if the document exists */
#ifndef _WX
  if (doc &&
      DocumentTypes[doc] == docSource)
    SetEnvGeom ("Source_view", doc);
#endif /* _WX */
  if (doc &&
      DocumentURLs[doc] != NULL &&
      DocumentTypes[doc] != docSource &&
      DocumentTypes[doc] != docLog && 
      DocumentTypes[doc] != docLibrary)
    {
      if (!view_name)
        {
          if (DocumentTypes[doc] == docAnnot)
            SetEnvGeom ("Annot_Formatted_view", doc);
          else if (DocumentTypes[doc] == docBookmark)
            SetEnvGeom ("Topics_Formatted_view", doc);
          else
            {
#ifdef _WX
              SetEnvGeom ("Wx_Window", doc);
#else /* _WX */
              SetEnvGeom ("Formatted_view", doc);
              SetEnvGeom ("Structure_view", doc);
              SetEnvGeom ("Alternate_view", doc);
              SetEnvGeom ("Links_view", doc);
              SetEnvGeom ("Table_of_contents", doc);
              if (DocumentSource[doc])
                SetEnvGeom ("Source_view", DocumentSource[doc]);
#endif /* _WX */
            }
        }
      else
        SetEnvGeom (view_name, doc);
    }
}

/*----------------------------------------------------------------------
  SetGeometryConf
  Updates the registry Geometry values and redraws the windows
  ----------------------------------------------------------------------*/
void SetGeometryConf ( int document, const char * view_name )
{
  /* read the current values and save them into the registry */
  SetEnvCurrentGeometry ( document, view_name );

  /* save the options */
  TtaSaveAppRegistry ();
}

#ifdef _WINGUI
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
	  SetGeometryConf ( GeometryDoc, NULL );
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DONE:
    case IDCANCEL:
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
#else /* _WINGUI */
/*----------------------------------------------------------------------
  GeometryCallbackDialog
  callback of the geometry configuration menu
  ----------------------------------------------------------------------*/
static void GeometryCallbackDialog (int ref, int typedata, char *data)
{
  intptr_t val;

  TtaDestroyDialogue (GeometryBase + GeometryMenu);
  if (ref == -1)
    {
      /* removes the geometry conf menu */
      GeometryDoc = 0;
    }
  else
    {
      /* has the user changed the options? */
      val = (intptr_t) data;
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
	      SetGeometryConf ( GeometryDoc, NULL );
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
#endif /* !_WINGUI */

/*----------------------------------------------------------------------
  GeometryConfMenu
  Build and display the Conf Menu dialog box and prepare for input.
  ----------------------------------------------------------------------*/
void         GeometryConfMenu (Document document, View view)
{
#ifndef _WINGUI
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
#else /* !_WINGUI */
  if (GeometryHwnd)
    /* menu already active. We'll destroy it in order to have
       a menu that points to the current document */
    EndDialog (GeometryHwnd, ID_DONE);
  GeometryDoc = document;
  DialogBox (hInstance, MAKEINTRESOURCE (GEOMETRYMENU), NULL,
	     (DLGPROC) WIN_GeometryDlgProc);
#endif /* !_WINGUI */
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
  GetEnvString ("ANNOT_USER", GProp_Annot.AnnotUser);
  GetEnvString ("ANNOT_POST_SERVER", GProp_Annot.AnnotPostServer);
  GetEnvString ("ANNOT_SERVERS", GProp_Annot.AnnotServers);
  TtaGetEnvBoolean ("ANNOT_LAUTOLOAD", &(GProp_Annot.AnnotLAutoLoad));
  TtaGetEnvBoolean ("ANNOT_RAUTOLOAD", &(GProp_Annot.AnnotRAutoLoad));
  TtaGetEnvBoolean ("ANNOT_RAUTOLOAD_RST", &(GProp_Annot.AnnotRAutoLoadRst));

#ifdef _WINGUI
  /* we substitute spaces into \r for the configuration widget menu */
  ConvertSpaceNL (GProp_Annot.AnnotServers, TRUE);
#endif /* _WINGUI */
}

/*----------------------------------------------------------------------
  SetAnnotConf
  Updates the registry annotation values
  ----------------------------------------------------------------------*/
static void SetAnnotConf (void)
{
#ifdef _WINGUI
	/* we remove the \n added for the configuration menu widget */
  ConvertSpaceNL (GProp_Annot.AnnotServers, FALSE);
#endif /* _WINGUI */

  TtaSetEnvString ("ANNOT_USER", GProp_Annot.AnnotUser, TRUE);
  TtaSetEnvString ("ANNOT_POST_SERVER", GProp_Annot.AnnotPostServer, TRUE);
  TtaSetEnvString ("ANNOT_SERVERS", GProp_Annot.AnnotServers, TRUE);
  TtaSetEnvBoolean ("ANNOT_LAUTOLOAD", GProp_Annot.AnnotLAutoLoad, TRUE);
  TtaSetEnvBoolean ("ANNOT_RAUTOLOAD", GProp_Annot.AnnotRAutoLoad, TRUE);
  TtaSetEnvBoolean ("ANNOT_RAUTOLOAD_RST", GProp_Annot.AnnotRAutoLoadRst, TRUE);

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
  GetDefEnvString ("ANNOT_USER", GProp_Annot.AnnotUser);
  GetDefEnvString ("ANNOT_POST_SERVER", GProp_Annot.AnnotPostServer);
  GetDefEnvString ("ANNOT_SERVERS", GProp_Annot.AnnotServers);
  TtaGetDefEnvBoolean ("ANNOT_LAUTOLOAD", &(GProp_Annot.AnnotLAutoLoad));
  TtaGetDefEnvBoolean ("ANNOT_RAUTOLOAD", &(GProp_Annot.AnnotRAutoLoad));
  TtaGetDefEnvBoolean ("ANNOT_RAUTOLOAD_RST", &(GProp_Annot.AnnotRAutoLoadRst));
#ifdef _WINGUI
  /* we substitute spaces into \n for the configuration widget menu */
  ConvertSpaceNL (GProp_Annot.AnnotServers, TRUE);
#endif /* _WINGUI */
}

#ifdef _WINGUI
/*----------------------------------------------------------------------
  WIN_RefreshAnnotMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void WIN_RefreshAnnotMenu (HWND hwnDlg)
{
  SetDlgItemText (hwnDlg, IDC_ANNOTUSER, GProp_Annot.AnnotUser);
  SetDlgItemText (hwnDlg, IDC_ANNOTPOSTSERVER, GProp_Annot.AnnotPostServer);
  SetDlgItemText (hwnDlg, IDC_ANNOTSERVERS, GProp_Annot.AnnotServers);
  CheckDlgButton (hwnDlg, IDC_ANNOTLAUTOLOAD, (GProp_Annot.AnnotLAutoLoad) 
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_ANNOTRAUTOLOAD, (GProp_Annot.AnnotRAutoLoad) 
		  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (hwnDlg, IDC_ANNOTRAUTOLOADRST, (GProp_Annot.AnnotRAutoLoadRst) 
		  ? BST_CHECKED : BST_UNCHECKED);
}
#else /* WINDOWS */
/*----------------------------------------------------------------------
  RefreshAnnotMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void RefreshAnnotMenu ()
{
#ifdef _GTK
  /* set the menu entries to the current values */
  TtaSetTextForm (AnnotBase + mAnnotUser, GProp_Annot.AnnotUser);
  TtaSetTextForm (AnnotBase + mAnnotPostServer, GProp_Annot.AnnotPostServer);
  TtaSetTextForm (AnnotBase + mAnnotServers, GProp_Annot.AnnotServers);
  TtaSetToggleMenu (AnnotBase + mToggleAnnot, 0, GProp_Annot.AnnotLAutoLoad);
  TtaSetToggleMenu (AnnotBase + mToggleAnnot, 1, GProp_Annot.AnnotRAutoLoad);
  TtaSetToggleMenu (AnnotBase + mToggleAnnot, 2, GProp_Annot.AnnotRAutoLoadRst);
#endif /* _GTK */
}
#endif /* !_WINGUI */

#ifdef _WINGUI
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
	      GetDlgItemText (hwnDlg, IDC_ANNOTUSER, GProp_Annot.AnnotUser,
			      sizeof (GProp_Annot.AnnotUser) - 1);
	      break;
	    case IDC_ANNOTPOSTSERVER:
	      GetDlgItemText (hwnDlg, IDC_ANNOTPOSTSERVER, GProp_Annot.AnnotPostServer,
			      sizeof (GProp_Annot.AnnotPostServer) - 1);
	      break;
	    case IDC_ANNOTSERVERS:
	      GetDlgItemText (hwnDlg, IDC_ANNOTSERVERS, GProp_Annot.AnnotServers,
			      sizeof (GProp_Annot.AnnotServers) - 1);
	      break;
	    }
	}
      switch (LOWORD (wParam))
	{
	  /* toggle buttons */
	case IDC_ANNOTLAUTOLOAD:
	  GProp_Annot.AnnotLAutoLoad = !(GProp_Annot.AnnotLAutoLoad);
	  break;
	case IDC_ANNOTRAUTOLOAD:
	  GProp_Annot.AnnotRAutoLoad = !(GProp_Annot.AnnotRAutoLoad);
	  break;
	case IDC_ANNOTRAUTOLOADRST:
	  GProp_Annot.AnnotRAutoLoadRst = !(GProp_Annot.AnnotRAutoLoadRst);
	  break;

	  /* action buttons */
	case ID_APPLY:
	  SetAnnotConf ();	  
	  /* reset the status flag */
	  EndDialog (hwnDlg, ID_DONE);
	  break;
	case ID_DONE:
	case IDCANCEL:
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
#else /* _WINGUI */
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
#ifndef _WX
	      TtaDestroyDialogue (ref);
#endif /* _WX */
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
	    strcpy (GProp_Annot.AnnotUser, data);
	  else
	    GProp_Annot.AnnotUser[0] = EOS;
	  break;

	case mAnnotPostServer:
	  if (data)
	    strcpy (GProp_Annot.AnnotPostServer, data);
	  else
	    GProp_Annot.AnnotPostServer[0] = EOS;
	  break;

	case mAnnotServers:
	  if (data)
	    strcpy (GProp_Annot.AnnotServers, data);
	  else
	    GProp_Annot.AnnotServers[0] = EOS;
	  break;

	case mToggleAnnot:
	  switch (val) 
	    {
	    case 0:
	      GProp_Annot.AnnotLAutoLoad = !(GProp_Annot.AnnotLAutoLoad);
	      break;
	    case 1:
	      GProp_Annot.AnnotRAutoLoad = !(GProp_Annot.AnnotRAutoLoad);
	      break;
	    case 2:
	      GProp_Annot.AnnotRAutoLoadRst = !(GProp_Annot.AnnotRAutoLoadRst);
	      break;
	    }
	  break;

	default:
	  break;
	}
    }
}
#endif /* _WINGUI */
#endif /* ANNOTATIONS */

/*----------------------------------------------------------------------
  AnnotConfMenu
  Build and display the Conf Menu dialog box and prepare for input.
  ----------------------------------------------------------------------*/
void         AnnotConfMenu (Document document, View view)
{
#ifdef ANNOTATIONS
#ifdef _GTK
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
#endif /* _GTK */

   /* load and display the current values */
   GetAnnotConf ();

#ifdef _GTK
   RefreshAnnotMenu ();
  /* display the menu */
   TtaSetDialoguePosition ();
   TtaShowDialogue (AnnotBase + AnnotMenu, TRUE);
#endif /* _GTK */
#ifdef _WINGUI
  if (!AnnotHwnd)
    /* only activate the menu if it isn't active already */
     DialogBox (hInstance, MAKEINTRESOURCE (ANNOTMENU), NULL,
		(DLGPROC) WIN_AnnotDlgProc);
  else
     SetFocus (AnnotHwnd);
#endif /* _WINGUI */
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
  Returns a tab dialog reference (used into PreferenceDlgWX callbacks)
  ----------------------------------------------------------------------*/
int GetPrefGeneralBase()
{
  return GeneralBase;
}

/*----------------------------------------------------------------------
  Returns a tab dialog reference (used into PreferenceDlgWX callbacks)
  ----------------------------------------------------------------------*/
int GetPrefBrowseBase()
{
  return BrowseBase;
}

/*----------------------------------------------------------------------
  Returns a tab dialog reference (used into PreferenceDlgWX callbacks)
  ----------------------------------------------------------------------*/
int GetPrefCacheBase()
{
  return CacheBase;
}

/*----------------------------------------------------------------------
  Returns a tab dialog reference (used into PreferenceDlgWX callbacks)
  ----------------------------------------------------------------------*/
int GetPrefProxyBase()
{
  return ProxyBase;
}

/*----------------------------------------------------------------------
  Returns a tab dialog reference (used into PreferenceDlgWX callbacks)
  ----------------------------------------------------------------------*/
int GetPrefPublishBase()
{
  return PublishBase;
}

/*----------------------------------------------------------------------
  Returns a tab dialog reference (used into PreferenceDlgWX callbacks)
  ----------------------------------------------------------------------*/
int GetPrefColorBase()
{
  return ColorBase;
}

/*----------------------------------------------------------------------
  Returns a tab dialog reference (used into PreferenceDlgWX callbacks)
  ----------------------------------------------------------------------*/
int GetPrefGeometryBase()
{
  return GeometryBase;
}

/*----------------------------------------------------------------------
  Returns a tab dialog reference (used into PreferenceDlgWX callbacks)
  ----------------------------------------------------------------------*/
int GetPrefAnnotBase()
{
#ifdef ANNOTATIONS
  return AnnotBase;
#else /* ANNOTATIONS */
  return 0;
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
  Returns a tab dialog reference (used into PreferenceDlgWX callbacks)
  ----------------------------------------------------------------------*/
int GetPrefDAVBase()
{
#ifdef DAV
  return DAVBase;
#else /* DAV */
  return 0;
#endif /* DAV */
}

/*----------------------------------------------------------------------
  Use to set the Amaya global variables (General preferences)
  ----------------------------------------------------------------------*/
void SetProp_General( const Prop_General * prop )
{
#ifdef _WX
  GProp_General = *prop;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  Use to get the Amaya global variables (General preferences)
  ----------------------------------------------------------------------*/
Prop_General GetProp_General()
{
#ifdef _WX
  return GProp_General;
#else /* _WX */
  Prop_General prop;
  memset(&prop, 0, sizeof(Prop_General) );
  return prop;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  Use to set the Amaya global variables (Browse preferences)
  ----------------------------------------------------------------------*/
void SetProp_Browse( const Prop_Browse * prop )
{
#ifdef _WX
  GProp_Browse = *prop;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  Use to get the Amaya global variables (Browse preferences)
  ----------------------------------------------------------------------*/
Prop_Browse GetProp_Browse()
{
#ifdef _WX
  return GProp_Browse;
#else /* _WX */
  Prop_Browse prop;
  memset(&prop, 0, sizeof(Prop_Browse) );
  return prop;
#endif /* _WX */
}
 
/*----------------------------------------------------------------------
  Use to set the Amaya global variables (Publish preferences)
  ----------------------------------------------------------------------*/
void SetProp_Publish( const Prop_Publish * prop )
{
#ifdef _WX
  GProp_Publish = *prop;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  Use to get the Amaya global variables (Publish preferences)
  ----------------------------------------------------------------------*/
Prop_Publish GetProp_Publish()
{
#ifdef _WX
  return GProp_Publish;
#else /* _WX */
  Prop_Publish prop;
  memset(&prop, 0, sizeof(Prop_Publish) );
  return prop;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  Use to set the Amaya global variables (Cache preferences)
  ----------------------------------------------------------------------*/
void SetProp_Cache( const Prop_Cache * prop )
{
#ifdef _WX
  GProp_Cache = *prop;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  Use to get the Amaya global variables (Cache preferences)
  ----------------------------------------------------------------------*/
Prop_Cache GetProp_Cache()
{
#ifdef _WX
  return GProp_Cache;
#else /* _WX */
  Prop_Cache prop;
  memset(&prop, 0, sizeof(Prop_Cache) );
  return prop;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  Use to set the Amaya global variables (Proxy preferences)
  ----------------------------------------------------------------------*/
void SetProp_Proxy( const Prop_Proxy * prop )
{
#ifdef _WX
  GProp_Proxy = *prop;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  Use to get the Amaya global variables (Proxy preferences)
  ----------------------------------------------------------------------*/
Prop_Proxy GetProp_Proxy()
{
#ifdef _WX
  return GProp_Proxy;
#else /* _WX */
  Prop_Proxy prop;
  memset(&prop, 0, sizeof(Prop_Proxy) );
  return prop;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  Use to set the Amaya global variables (Color preferences)
  ----------------------------------------------------------------------*/
void SetProp_Color( const Prop_Color * prop )
{
#ifdef _WX
  GProp_Color = *prop;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  Use to get the Amaya global variables (Color preferences)
  ----------------------------------------------------------------------*/
Prop_Color GetProp_Color()
{
#ifdef _WX
  return GProp_Color;
#else /* _WX */
  Prop_Color prop;
  memset(&prop, 0, sizeof(Prop_Color) );
  return prop;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  Use to set the Amaya global variables (Annotations preferences)
  ----------------------------------------------------------------------*/
void SetProp_Annot( const Prop_Annot * prop )
{
#ifdef ANNOTATIONS
#ifdef _WX
  GProp_Annot = *prop;
#endif /* _WX */
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
  Use to get the Amaya global variables (Annotations preferences)
  ----------------------------------------------------------------------*/
Prop_Annot GetProp_Annot()
{
#ifdef ANNOTATIONS
#ifdef _WX
  return GProp_Annot;
#else /* _WX */
  Prop_Annot prop;
  memset(&prop, 0, sizeof(Prop_Annot) );
  return prop;
#endif /* _WX */
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
  Use to set the Amaya global variables (WebDAV preferences)
  ----------------------------------------------------------------------*/
void SetProp_DAV( const Prop_DAV * prop )
{
#ifdef DAV
#ifdef _WX
  GProp_DAV = *prop;
#endif /* _WX */
#endif /* DAV */
}

/*----------------------------------------------------------------------
  Use to get the Amaya global variables (WebDAV preferences)
  ----------------------------------------------------------------------*/
Prop_DAV GetProp_DAV()
{
#if defined(DAV) && defined(_WX)
  return GProp_DAV;
#else /* _WX && DAV */
  Prop_DAV prop;
  memset(&prop, 0, sizeof(Prop_DAV) );
  return prop;
#endif /* _WX && DAV */
}


/*----------------------------------------------------------------------
  PreferenceMenu
  Build and display the preference dialog
  ----------------------------------------------------------------------*/
void PreferenceMenu (Document document, View view)
{
#ifdef _WX
  /* ---> General Tab */
  GetGeneralConf (); /* load the current values => General tab */
  
  /* ---> Browse Tab */
  SafePutStatus = 0; /* reset the modified flag */
  GetBrowseConf (); /* load the current values => Browse tab */
  /* keep initial values to detect an change */
  InitOpeningLocation = GProp_Browse.OpeningLocation;
  InitLoadImages = GProp_Browse.LoadImages;
  InitLoadObjects =GProp_Browse. LoadObjects;
  InitBgImages = GProp_Browse.BgImages;
  InitLoadCss = GProp_Browse.LoadCss;
  InitWarnCTab = GProp_Browse.WarnCTab;

  /* ---> Publish Tab */
  GetPublishConf ();
  SafePutStatus = 0;  /* reset the modified flag */

  /* ---> Cache Tab */
  CacheStatus = 0; /* reset the modified flag */
  GetCacheConf (); /* load and display the current values */
  
  /* ---> Proxy Tab */
  ProxyStatus = 0; /* reset the modified flag */
  GetProxyConf (); /* load and display the current values */

  /* ---> Color Tab */   
  GetColorConf (); /* load and display the current values */

  /* ---> Geometry Tab */   
  GeometryDoc = document;

#ifdef ANNOTATIONS
  /* ---> Annot Tab */
  GetAnnotConf ();
#endif /* ANNOTATIONS */
#ifdef DAV
  /* ---> WebDAV Tab */
  GetDAVConf ();
#endif /* DAV */
  ThotBool created = CreatePreferenceDlgWX ( PreferenceBase,
					     TtaGetViewFrame (document, view),
					     URL_list );
  if (created)
    {
      TtaSetDialoguePosition ();
      TtaShowDialogue (PreferenceBase, TRUE);
    }
#endif /* _WX */
}

/*----------------------------------------------------------------------
   callback of the preference dialog
  ----------------------------------------------------------------------*/
static void PreferenceCallbackDialog (int ref, int typedata, char *data)
{
#ifdef _WX
  int val;

  if (ref == -1)
      TtaDestroyDialogue (PreferenceBase);
  else
    {
      /* has the user changed the options? */
      val = (int) data;
      switch (ref - PreferenceBase)
	{
	case 0:
	  switch (val) 
	    {
	    case 0: /* CANCEL */
	      TtaDestroyDialogue (ref);
	      break;
	    case 1: /* OK */
	    case 2: /* DEFAULT */
	    default:
	      break;
	    }
	  break;
	default:
	  break;
	}
    }
#endif /* _WX */
}

/*----------------------------------------------------------------------
   InitConfMenu: initialisation, called during Amaya initialisation
  ----------------------------------------------------------------------*/
void InitConfMenu (void)
{
  InitAmayaDefEnv ();

#ifdef _WX
  /* create a new dialog reference for Preferences */
  PreferenceBase = TtaSetCallback( (Proc)PreferenceCallbackDialog, 1 );
#endif /* _WX */

#ifndef _WINGUI
  CacheBase = TtaSetCallback ((Proc)CacheCallbackDialog, MAX_CACHEMENU_DLG);
  ProxyBase = TtaSetCallback ((Proc)ProxyCallbackDialog, MAX_PROXYMENU_DLG);
  GeneralBase = TtaSetCallback ((Proc)GeneralCallbackDialog, MAX_GENERALMENU_DLG);
  BrowseBase = TtaSetCallback ((Proc)BrowseCallbackDialog, MAX_BROWSEMENU_DLG);
  PublishBase = TtaSetCallback ((Proc)PublishCallbackDialog, MAX_PUBLISHMENU_DLG);
  ColorBase = TtaSetCallback ((Proc)ColorCallbackDialog,
			      MAX_COLORMENU_DLG);
  GeometryBase = TtaSetCallback ((Proc)GeometryCallbackDialog,
				 MAX_GEOMETRYMENU_DLG);
#ifdef ANNOTATIONS
  AnnotBase = TtaSetCallback ((Proc)AnnotCallbackDialog,
			      MAX_ANNOTMENU_DLG);
#endif /* ANNOTATIONS */
#ifdef DAV
  InitDAVPreferences ();
#endif /* DAV */
#endif /* _WINGUI */
}
