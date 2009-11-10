/*
 *
 *  (c) COPYRIGHT INRIA, 2001-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Module dedicated to XFT font server.
 *
 * Author: P. Cheyrou-Lagreze (INRIA)
 */

#ifdef _GL
#ifdef _WX
  #include "wx/wx.h"
#endif /* _WX */

#include "thot_gui.h"
#include "thot_sys.h"
#include "application.h"
#include "fontconfig.h"
#ifdef _WINGUI
  #include <windows.h>
#endif    /* _WINGUI */


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static int GetFontFilenameFromConfig (char script, int family, int highlight, 
                                      int size, char *filename)
{
  char *response;

  response = (char *) FontLoadFromConfig (script,  family, highlight);
  if (response == NULL || !TtaFileExist (response))
    {
      /* Bad Configuration: get back to normal loading */
      return 0;
    }
  strcpy (filename, response);
  return 1;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int GetFontFilename (char script, int family, int highlight, int size, 
                     char *filename)
{
#ifdef _WINDOWS
  if (GetFontFilenameFromConfig (script, family, highlight,  size, filename))
    return 1;
#ifdef _WX
  wxChar buff[MAX_PATH];
  GetWindowsDirectory (buff , MAX_PATH);
  wxString winpath = buff;
  sprintf( filename, "%s", (const char *)winpath.mb_str(*wxConvCurrent));
#else /* _WX */
  GetWindowsDirectory (filename , 1024);
#endif /* _WX */
  strcat (filename, "\\fonts\\"); 
  if (script == 'G' || family == 0)
    strcat (filename, "Symbol");
  else if (script == 'E')
    {
      switch (family)
        {
        case 6:
          strcat (filename,  "esstix6_"); 
          break;
        case 7:
          strcat (filename,  "esstix7_"); 
          break;	  
        case 10: 
          strcat (filename,  "esstix10"); 
          break;
        default:
          break;
        }
    }
  else if (script == 'Z')
    {
      strcat (filename, "arialu");
      strcat (filename, ".ttf\0");
      return 1;
    }
  else
    {
      /*charset ???*/
      switch (family)
        {
        case 0:		 
          strcat (filename, "Symbol");
          break;
        case 1:
          strcat (filename, "Times");
          break;
        case 2:       
          strcat (filename, "Arial");
          break;
        case 3:
          strcat (filename, "Cour");
          break;
        default:
          strcat (filename, "Verdana");
        }
      switch (highlight)
        {
        case 0:
          break;
        case 2:
        case 3:
          strcat (filename, "i");
          break;
        case 1:
        case 4:
        case 5:
          strcat (filename, "bd");
          break;
        default:
          break;
        }
    }
  strcat (filename, ".ttf\0");
  return 1;
#else /* _WINDOWS */
  if (GetFontFilenameFromConfig (script, family, highlight,  size, filename))
    return 1;
  else if (family > 1)
    return GetFontFilename (script, 1, highlight, size, filename);
  else
    return 0;
#endif /* _WINDOWS */
}
#endif /* _GL */
