#ifdef _GL
#ifdef _GTK

/* Font Server */
#include <gdk/gdkx.h>
#include "X11/Xft/Xft.h"
#include "X11/Xft/XftFreetype.h"

#endif /* _GTK */

#ifdef _GTK

int GetFontFilename(const char *xlfd, char *filename)
{
  XftPattern	*match, *pat;
  XftResult     result;  
  XftValue	v;
  XftResult	r;

  pat = XftXlfdParse (xlfd, False, False); 
  if (!pat)
    return 0;
  match = XftFontMatch (GDK_DISPLAY(), 0, pat, &result);  
  if (!match)
    return 0;
  XftPatternDestroy (pat);
  // That would be the good code, but it segfault...
  //	XftPatternGetString (match, XFT_FILE, 0, &filename); 
  r = XftPatternGet (match, XFT_FILE, 0, &v);
  if (r != XftResultMatch)
    return 0;
  if (v.type != XftTypeString)
    return 0;
  strcpy (filename, v.u.s);
  XftPatternDestroy (match);
  return 1;
}

#else /* _GTK */
int GetFontFilename(const char *xlfd, char *filename)
{
  char name[1024];
  int name_ptr;
  
  name_ptr = sprintf (&name[0], "c:\\windows\\font\\"); 
  
  switch (family)
     {
     case 0:   
       sprintf (&name[name_ptr], "Symbol");
       break;
     case 1:
       name_ptr += sprintf (&name[name_ptr], "Times New Roman");
       break;
     case 2:       
       name_ptr += sprintf (&name[name_ptr], "Arial");
       break;
     case 3:
       name_ptr += sprintf (&name[name_ptr], "Courier New");
       break;
     default:
       name_ptr += sprintf (&name[name_ptr], "Arial");
     }
  switch (highlight)
     {
     case 0:
       break;
     case 2:
     case 3:
       name_ptr += sprintf (&name[name_ptr], "i");
       break;
     case 1:
     case 4:
     case 5:
       name_ptr += sprintf (&name[name_ptr], "bd");
       break;
     default:
       break;
     }
  sprintf (&name[name_ptr], ".ttf");
  return 1;
}

#endif /* _GTK */
#endif /* _GL */
