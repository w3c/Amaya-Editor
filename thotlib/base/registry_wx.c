/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#if defined(_WX)
#include "wx/wx.h"
#include "wx/string.h"
#include "wx/strconv.h"
#include "wx/utils.h"

#include "thot_gui.h"
#include "thotkey.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "registry.h"
#include "registry_wx.h"
#include "fileaccess.h"

/*----------------------------------------------------------------------
  TtaGetResourcesPathWX - 
  this function returns a amaya resource path.
  this path is independant of opearting system.
  differents resources types are:
  - WX_RESOURCES_ICON
  - WX_RESOURCES_XRC
  params:
  + type : type of required resource path
  + filename : the filename of the resource
  returns:
  + wxString : a string containing the right path
  ----------------------------------------------------------------------*/
wxString TtaGetResourcePathWX( wxResourceType type, const char * filename )
{
  wxString path;
  switch ( type )
    {
    case WX_RESOURCES_ICON_16X16:
      {
        wxString amaya_directory( TtaGetEnvString ("THOTDIR"), *wxConvCurrent );
#ifdef _WINDOWS
        path = amaya_directory + _T("\\resources\\icons\\16x16\\") + wxString( filename, *wxConvCurrent );
#endif /* _WINDOWS */
#ifdef _UNIX
        path = amaya_directory + _T("/resources/icons/16x16/") + wxString( filename, *wxConvCurrent );
#endif /* _UNIX */
      }
      break;
    case WX_RESOURCES_ICON_22X22:
      {
        wxString amaya_directory( TtaGetEnvString ("THOTDIR"), *wxConvCurrent );
#ifdef _WINDOWS
        path = amaya_directory + _T("\\resources\\icons\\22x22\\") + wxString( filename, *wxConvCurrent );
#endif /* _WINDOWS */
#ifdef _UNIX
        path = amaya_directory + _T("/resources/icons/22x22/") + wxString( filename, *wxConvCurrent );
#endif /* _UNIX */
      }
      break;
    case WX_RESOURCES_ICON_MISC:
      {
        wxString amaya_directory( TtaGetEnvString ("THOTDIR"), *wxConvCurrent );
#ifdef _WINDOWS
        path = amaya_directory + _T("\\resources\\icons\\misc\\") + wxString( filename, *wxConvCurrent );
#endif /* _WINDOWS */
#ifdef _UNIX
        path = amaya_directory + _T("/resources/icons/misc/") + wxString( filename, *wxConvCurrent );
#endif /* _UNIX */
      }
      break;
    case WX_RESOURCES_XRC:
      {
        wxString amaya_directory( TtaGetEnvString ("THOTDIR"), *wxConvCurrent );
#ifdef _WINDOWS
        path = amaya_directory + _T("\\resources\\xrc\\") + wxString( filename, *wxConvCurrent );
#endif /* _WINDOWS */
#ifdef _UNIX
        path = amaya_directory + _T("/resources/xrc/") + wxString( filename, *wxConvCurrent );
#endif /* _UNIX */
      }
      break;
    case WX_RESOURCES_SVG:
      {
        wxString amaya_directory( TtaGetEnvString ("THOTDIR"), *wxConvCurrent );
#ifdef _WINDOWS
        path = amaya_directory + _T("\\resources\\svg\\") + wxString( filename, *wxConvCurrent );
#endif /* _WINDOWS */
#ifdef _UNIX
        path = amaya_directory + _T("/resources/svg/") + wxString( filename, *wxConvCurrent );
#endif /* _UNIX */
      }
      break;
    }
  //TTALOGDEBUG_0( TTA_LOG_INIT, _T("TtaGetResourcePathWX: path=")+path);
  return path;
}

/*----------------------------------------------------------------------
  TtaGetSystemLanguage
  this function returns the system language
  -----------------------------------------------------------------------*/
int TtaGetSystemLanguage( )
{
  int m_Lang = 0;

  m_Lang = wxLocale::GetSystemLanguage();
  return m_Lang;
}

/*----------------------------------------------------------------------
  TtaGetHomeDir - 
  this function returns the user homedir
  on windows something like : C:\Doc... adn.. settings\username
  on unix something like : /home/username
  ----------------------------------------------------------------------*/
wxString TtaGetHomeDir()
{
  wxString wx_win_homedir;
#ifdef _WINDOWS
  //if (wx_win_homedir.IsEmpty())
    {
      wxChar      buffer[2000];
      DWORD       dwSize;

      typedef BOOL (STDMETHODCALLTYPE FAR * LPFNGETPROFILESDIRECTORY) (
                                                                       LPTSTR lpProfileDir,
                                                                       LPDWORD lpcchSize
                                                                       );
      HMODULE                  g_hUserEnvLib          = NULL;
      LPFNGETPROFILESDIRECTORY GetProfilesDirectory   = NULL;

      buffer[0] = EOS;
      g_hUserEnvLib = LoadLibrary (_T("userenv.dll"));
      if (g_hUserEnvLib)
        {
          GetProfilesDirectory =
            (LPFNGETPROFILESDIRECTORY) GetProcAddress (g_hUserEnvLib,
                                                       "GetProfilesDirectoryW");
          dwSize = MAX_PATH;
          GetProfilesDirectory (buffer, &dwSize);
        }
      if (buffer[0] == EOS)
        GetWindowsDirectory (buffer, dwSize);
      
      wxString wx_win_profiles_dir(buffer);      
      dwSize = MAX_PATH;
      wxGetUserName(buffer, dwSize);
      wxString wx_win_username(buffer);
      wx_win_homedir = wx_win_profiles_dir + _T("\\") + wx_win_username;
    }
#else /* _WINDOWS */
  wx_win_homedir = wxGetHomeDir();
#endif /* _WINDOWS */
  return wx_win_homedir;
}
#endif /* _WX */


/*----------------------------------------------------------------------
  TtaGetDocumentDir - 
  this function returns the default user document directory
  on windows something like : C:\Doc... adn.. settings\username\documents
  on unix something like : /home/username/documents
  ----------------------------------------------------------------------*/
char *TtaGetDocumentsDir()
{
  char               path[MAX_LENGTH], *s;
  wxString            homedir;

  s = TtaGetEnvString ("DOCUMENTS_PATH");
  if (s == NULL || s[0] == EOS)
    {
      homedir = TtaGetHomeDir();
#ifdef _WINDOWS
      strcpy (path, (const char *)(homedir.mb_str(wxConvUTF8)));
#else /* _WINDOWS */
      sprintf (path, "%s%cDocuments", (const char *)(homedir.mb_str(wxConvUTF8)), DIR_SEP);
#endif /* _WINDOWS */
      TtaSetEnvString ("DOCUMENTS_PATH", path, TRUE);
      s = TtaGetEnvString ("DOCUMENTS_PATH");
    }

  // check if the directory exists
  if (!TtaCheckDirectory(s))
    TtaCheckMakeDirectory(s, TRUE);
  return s;
}
