#ifdef _GL
#ifdef _GTK

/* Font Server */
#include <gdk/gdkx.h>
#include "X11/Xft/Xft.h"
#include "X11/Xft/XftFreetype.h"

#endif /* _GTK */

#ifdef _GTK

/* XFT_FAMILY XFT_FOUNDRY XFT_STYLE XFT_ENCODING "iso8859-1" 
   XFT_SLANT  XFT_WEIGHT XFT_SIZE  XFT_DPI */
int GetFontFilename(char script, int family, 
		   int highlight, int size, 
		    int UseLucidaFamily, int UseAdobeFamily,
		    char *filename)
{
  XftPattern	*match, *pat;
  XftResult     result;  
  char	*s;
  int ok = 0;
  int render, core;

  pat = XftPatternCreate ();
  render = False;
  core = True;
  (void) XftPatternGetBool (pat, XFT_RENDER, 0, &render);
  (void) XftPatternGetBool (pat, XFT_CORE, 0, &core);
  if (!pat)
    return ok;    
  if (script != 'L' && script != 'G')
    {      
      switch (script)
	{
	case '2':
	  XftPatternAddString (pat, XFT_ENCODING, "iso8859-2");
	  break;
	case '3':
	  XftPatternAddString (pat, XFT_ENCODING, "iso8859-3");
	  break;
	case '4':
	  XftPatternAddString (pat, XFT_ENCODING, "iso8859-4");
	  break;
	case '5':
	  XftPatternAddString (pat, XFT_ENCODING, "iso8859-5");
	  break;
	case '6':
	  /*ARABIC_CHARSET*/
	  XftPatternAddString (pat, XFT_ENCODING, "iso8859-6");
	  break;
	case '7':
	  /*  GREEK_CHARSET */
	  XftPatternAddString (pat, XFT_ENCODING, "iso8859-7");
	  break;
	case '8':
	  /* HEBREW_CHARSET */
	  XftPatternAddString (pat, XFT_ENCODING, "iso8859-8");
	  break;
	}
      if (highlight == 0 || highlight == 2 || highlight == 3)
	XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_MEDIUM);
      else
	XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_BOLD);
      if (highlight == 0 || highlight == 1)
	XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_ROMAN);
      else
	XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_OBLIQUE);
      if (size < 0)
	  size = 12;
    }
  else if (script == 'G' || family == 0)
    {
      XftPatternAddString (pat, XFT_FAMILY, "symbol");
      XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_MEDIUM);
      XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_ROMAN);
    }
  else
    {
      if (UseLucidaFamily)
	{
	  switch (family)
	    {
	    case 1:
	      XftPatternAddString (pat, XFT_FAMILY, "lucidabright");
	      break;
	    case 3:
	      XftPatternAddString (pat, XFT_FAMILY, "lucidatypewriter");
	      break;
	    default:
	      XftPatternAddString (pat, XFT_FAMILY, "lucida");
	      break;
	    }
	}
      else
	{
	  switch (family)
	    {
	    case 1:
	      if (UseAdobeFamily)
		XftPatternAddString (pat, XFT_FAMILY, "new century schoolbook");
	      else
		XftPatternAddString (pat, XFT_FAMILY, "times");
	      break;
	    case 2:
	      XftPatternAddString (pat, XFT_FAMILY, "helvetica");
	      break;
	    case 3:
	      XftPatternAddString (pat, XFT_FAMILY, "adobe-courier");
	      break;
	    }
	}
  
      switch (highlight)
	{
	case 0:
	  XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_MEDIUM);
	  XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_ROMAN);
	  break;
	case 1:
	  if (UseLucidaFamily && family == 1)
	    XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_DEMIBOLD);
	  else
	    XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_MEDIUM);	  
	  XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_ROMAN);
	  break;
	case 2:
	case 3:
	    XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_MEDIUM);
	  if (family == 2 || family == 3)
	    XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_OBLIQUE);
	  else
	    XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_ITALIC);
	  break;
	case 4:
	case 5:
	  if (UseLucidaFamily && family == 1)
	    {
	      XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_DEMIBOLD);
	      XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_ITALIC);
	    }
	  else if (family == 2 || family == 2)
	    {	    
	      XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_BOLD);	    
	      XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_OBLIQUE);
	    }
	  else
	    {
	    
	      XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_BOLD);	    
	      XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_ITALIC);
	    }
	  break;
	}
    }
  XftPatternAddDouble (pat, XFT_SIZE, ((double) size) / 10.0);
  /* Returns a pattern more precise that let us load fonts*/
  match = XftFontMatch (GDK_DISPLAY(), 0, pat, &result); 
  if (match) 
    {
     if (XftPatternGetString (match, XFT_FILE, 0, &s) == XftResultMatch)
       {
	 strcpy (filename, s);  
	 ok = 1;
       }
     XftPatternDestroy (match);
    }    
  XftPatternDestroy (pat); 
  return ok;
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
