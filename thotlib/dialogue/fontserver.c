/*
 *
 *  (c) COPYRIGHT INRIA, 2001-2002
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Module dedicated to XFT font server.
 *
 * Author: P. Cheyrou-Lagreze (INRIA)
 */

#ifdef _GL

#ifdef _GTK
/* Font Server */
#include <string.h>
#include <gdk/gdkx.h>
#include "X11/Xft/Xft.h"
#include "X11/Xft/XftFreetype.h"
#else /* _GTK */
#include <windows.h>
#include <stdio.h>
#endif  /* _GTK */


/* XFT_FAMILY XFT_FOUNDRY XFT_STYLE XFT_ENCODING "iso8859-1" 
   XFT_SLANT  XFT_WEIGHT XFT_SIZE  XFT_DPI */
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int GetFontFilename (char script, int family, int highlight, int size, 
		     int UseLucidaFamily, int UseAdobeFamily,
		     char *filename)
{
#ifdef _GTK
  XftPattern	*match, *pat;
  XftResult     result;  
  char	*s;
  int ok = 0;

  pat = XftPatternCreate ();
  if (!pat)
    return ok;   
  /* XftPatternAddBool (pat, XFT_RENDER, True); */
  /* XftPatternAddBool (pat, XFT_CORE, True); */
  /* XftPatternAddBool (pat, XFT_ANTIALIAS, True);  */
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
      if (highlight == 0)
	XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_LIGHT);
      else if (highlight == 2 || highlight == 3)
	XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_LIGHT);
	/* XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_MEDIUM); */
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
      XftPatternAddString (pat, XFT_FOUNDRY, "adobe"); 
      XftPatternAddString (pat, XFT_FOUNDRY, "microsoft"); 

      XftPatternAddString (pat, XFT_FAMILY, "symbol");
      XftPatternAddString (pat, XFT_FAMILY, "Symbol"); 
      /* XftPatternAddString (pat, XFT_FAMILY, "Standard Symbols L");  */
      
      XftPatternAddString (pat, XFT_ENCODING, "fontspecific");  
      
      XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_MEDIUM); 
      /*XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_ROMAN); */
    }
  else
    {
      /*XftPatternAddString (pat, XFT_ENCODING, "iso8859-1");*/
      XftPatternAddString (pat, XFT_FOUNDRY, "adobe");
      XftPatternAddString (pat, XFT_FOUNDRY, "microsoft");
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
		XftPatternAddString (pat, XFT_FAMILY, "Times New Roman");
		XftPatternAddString (pat, XFT_FAMILY, "times");
		XftPatternAddString (pat, XFT_FAMILY, "Times");
		XftPatternAddString (pat, XFT_FAMILY, "lucidux");
		XftPatternAddString (pat, XFT_FAMILY, "Nimbus Roman No9 L");
		XftPatternAddString (pat, XFT_FAMILY, "terminus");
		XftPatternAddString (pat, XFT_FAMILY, "lucidabright");
		XftPatternAddString (pat, XFT_FAMILY, "new century schoolbook");
		XftPatternAddString (pat, XFT_FAMILY, "utopia");
	      XftPatternAddString (pat, XFT_FAMILY, "Utopia");
		/* XftPatternAddString (pat, XFT_FAMILY, "charter"); */
		XftPatternAddString (pat, XFT_FAMILY, "terminal");
		XftPatternAddString (pat, XFT_FAMILY, "georgia");
	      break;
	    case 2:
	      XftPatternAddString (pat, XFT_FAMILY, "helvetica");
	      XftPatternAddString (pat, XFT_FAMILY, "Helvetica");
	      XftPatternAddString (pat, XFT_FAMILY, "ArmNet Helvetica");
	      XftPatternAddString (pat, XFT_FAMILY, "Arial");
	      XftPatternAddString (pat, XFT_FAMILY, "arial");	      
	      XftPatternAddString (pat, XFT_FAMILY, "verdana");
	      XftPatternAddString (pat, XFT_FAMILY, "Verdana");
	      XftPatternAddString (pat, XFT_FAMILY, "Nimbus Sans L");
	      XftPatternAddString (pat, XFT_FAMILY, "lucidux");
	      XftPatternAddString (pat, XFT_FAMILY, "terminus");
	      XftPatternAddString (pat, XFT_FAMILY, "lucidabright");
	      XftPatternAddString (pat, XFT_FAMILY, "new century schoolbook");
	      XftPatternAddString (pat, XFT_FAMILY, "utopia");
	      XftPatternAddString (pat, XFT_FAMILY, "Utopia");
	      /* XftPatternAddString (pat, XFT_FAMILY, "charter"); */
	      XftPatternAddString (pat, XFT_FAMILY, "terminal");
	      XftPatternAddString (pat, XFT_FAMILY, "trebuchet");
	      break;
	    case 3:
	      XftPatternAddString (pat, XFT_FAMILY, "courier");
	      XftPatternAddString (pat, XFT_FAMILY, "courier new");
	      XftPatternAddString (pat, XFT_FAMILY, "Courier");
	      XftPatternAddString (pat, XFT_FAMILY, "Courier New");
	      XftPatternAddString (pat, XFT_FAMILY, "mono");
	      XftPatternAddString (pat, XFT_FAMILY, "sans");
	      XftPatternAddString (pat, XFT_FAMILY, "serif");
	      XftPatternAddString (pat, XFT_FAMILY, "Monotype");
	      XftPatternAddString (pat, XFT_FAMILY, "Monotype.com");
	      XftPatternAddString (pat, XFT_FAMILY, "Andale Mono");
	      XftPatternAddString (pat, XFT_FAMILY, "Nimbus Mono L");
	      XftPatternAddString (pat, XFT_FAMILY, "Arial");
	      XftPatternAddString (pat, XFT_FAMILY, "arial");
	      XftPatternAddString (pat, XFT_FAMILY, "Utopia");	      	      
	      break;
	    }
	}
    
      switch (highlight)
	{
	case 0:
	  XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_LIGHT);
	  /* XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_MEDIUM); */
	  XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_ROMAN);
	  break;
	case 1:
	  if (UseLucidaFamily && family == 1)
	    XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_DEMIBOLD);
	  else
	    XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_BOLD);	  
	  XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_ROMAN);
	  break;
	case 2:
	case 3:
	  XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_LIGHT);
	  /* XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_MEDIUM); */
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
	  else if (family == 2 || family == 3)
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
#ifdef _PCLDEBUGFONT
	 g_print ("\tXFT selection : %s", filename);
#endif /*_PCLDEBUG*/
	 ok = 1;
       }
     XftPatternDestroy (match);
    }    
  XftPatternDestroy (pat); 
  return ok;
#else /* _GTK */

  char *s;
  

  if (script == 'G')
  {
	  GetWindowsDirectory (filename , 1024);  
	  strcat (filename, "\\fonts\\"); 
	  strcat (filename, "Times");
  }
  else
	{
  GetWindowsDirectory (filename , 1024);  
  strcat (filename, "\\fonts\\"); 
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
#endif /* _GTK */
}



#endif /* _GL */
