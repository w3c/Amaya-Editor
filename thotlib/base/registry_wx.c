/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2004
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
#include "logdebug.h"

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
    }
  TTALOGDEBUG_0( TTA_LOG_INIT, _T("TtaGetResourcePathWX: path=")+path);
  return path;
}

/*----------------------------------------------------------------------
  TtaGetHomeDir - 
  this function returns the user homedir
  on windows something like : C:\Doc... adn.. settings\username
  on unix something like : /home/username
  ----------------------------------------------------------------------*/
wxString TtaGetHomeDir()
{
#ifdef _WINDOWS
	wxChar      buffer[MAX_PATH+1];
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

   wxGetUserName(buffer, sizeof(buffer));
   wxString wx_win_username(buffer);

   wxString wx_win_homedir = wx_win_profiles_dir + _T("\\") + wx_win_username;
   return wx_win_homedir;
#endif /* _WINDOWS */

#ifdef _UNIX
   return wxGetHomeDir();
#endif /* _UNIX */
}
#endif /* _WX */
