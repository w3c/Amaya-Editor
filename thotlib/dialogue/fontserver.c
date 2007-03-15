/*
 *
 *  (c) COPYRIGHT INRIA, 2001-2005
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

#ifdef _GTK
  /* Font Server */
  #include <gdk/gdkx.h>
  #include <ft2build.h>
  #include "Xft.h"
#endif  /* _GTK */

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


/* XFT_FAMILY XFT_FOUNDRY XFT_STYLE XFT_ENCODING "iso8859-1" 
   XFT_SLANT  XFT_WEIGHT XFT_SIZE  XFT_DPI */
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
      /*strcat (filename, "msmincho");
        strcat (filename, ".ttc\0");*/
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

#else /* #ifdef _WINDOWS */

#ifdef _WX
  if (GetFontFilenameFromConfig (script, family, highlight,  size, filename))
    return 1;
  else if (family > 1)
    return GetFontFilename (script, 1, highlight, size, filename);
  // wxASSERT(FALSE); // should never append !
  else
    return 0;
#endif /* _WX */
#endif /* #ifdef _WINDOWS */

#ifdef _GTK
  XftPattern	*match, *pat;
  XftResult     result;
  char	        *s;
  int           ok = 0;

#ifndef _GL
  if (!Printing)
#endif /* _GL */
    if (GetFontFilenameFromConfig (script, family, highlight,  size, filename))
      return 1;
  
  pat = XftPatternCreate ();
  if (!pat)
    return ok;  
  /*Directs Xft to use client-side fonts*/
  /*XftPatternAddBool (pat, XFT_RENDER, True); */

  /*Directs Xft to use server-side fonts*/
  /*XftPatternAddBool (pat, XFT_CORE, True); */

  /*Selects whether glyphs are anti-aliased*/
  XftPatternAddBool (pat, XFT_ANTIALIAS, True);

  /* One font contains multiple size*/
  XftPatternAddBool (pat, XFT_SCALABLE, True);

  if (script != 'G' && script != 'L' 
      && script != 'Z' && script != 'E')
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
        case '9':
          /* TURKISH */
          XftPatternAddString (pat, XFT_ENCODING, "iso8859-9");
          break;
        case 'D':
          XftPatternAddString (pat, XFT_ENCODING, "iso8859-13");
          break;
        case 'F':
          XftPatternAddString (pat, XFT_ENCODING, "iso8859-15");
          break;
        }

      XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_LIGHT);

      if (highlight == 0 || highlight == 1)
        XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_ROMAN);
      else
        XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_OBLIQUE);

      /*
        if (script == 'F')
        strcpy (encoding, "15");
        else if (script == 'D')
        strcpy (encoding, "13");
        else
        sprintf (encoding, "%c", script);
        sprintf (xftencoding, "iso8859-%s",  encoding);
        XftPatternAddString (pat, XFT_ENCODING, xftencoding);

        if (highlight == 0 || highlight == 2 || highlight == 3)
        XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_LIGHT);
        else
        XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_BOLD);
        if (highlight == 0 || highlight == 1)
        XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_ROMAN);
        else
        XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_OBLIQUE);
      */
      if (size < 0)
        size = 12;

    }
  else if (script == 'G' || family == 0)
    {

      XftPatternAddString (pat, XFT_FOUNDRY, "adobe");
      XftPatternAddString (pat, XFT_FOUNDRY, "urw");
      
      /*XftPatternAddString (pat, XFT_FOUNDRY, "microsoft"); */
      /*XftPatternAddString (pat, XFT_FOUNDRY, "monotype");*/
           
      XftPatternAddString (pat, XFT_FAMILY, "symbol");
      XftPatternAddString (pat, XFT_FAMILY, "Symbol");
      
      XftPatternAddString (pat, XFT_FAMILY, "standard symbols l");
      XftPatternAddString (pat, XFT_FAMILY, "Standard Symbols L");
   
      XftPatternAddString (pat, XFT_ENCODING, "adobe-fontspecific");
    }
  else if (script == 'E')
    {
      switch (family)
        {
        case 1:
          XftPatternAddString (pat, XFT_FAMILY, "esstixone"); 
          break;
        case 2:
          XftPatternAddString (pat, XFT_FAMILY, "esstixtwo"); 
          break;	  
        case 3: 
          XftPatternAddString (pat, XFT_FAMILY, "esstixthree"); 
          break;
        case 4:
          XftPatternAddString (pat, XFT_FAMILY, "esstixfour"); 
          break;
        case 5:
          XftPatternAddString (pat, XFT_FAMILY, "esstixfive"); 
          break;	  
        case 6:
          XftPatternAddString (pat, XFT_FAMILY, "esstixsix"); 
          break;
        case 7:
          XftPatternAddString (pat, XFT_FAMILY, "esstixseven"); 
          break;	  
        case 8:
          XftPatternAddString (pat, XFT_FAMILY, "esstixeight"); 
          break;
        case 9:
          XftPatternAddString (pat, XFT_FAMILY, "esstixnine"); 
          break;	  
        case 10: 
          XftPatternAddString (pat, XFT_FAMILY, "esstixten"); 
          break;
        case 11:
          XftPatternAddString (pat, XFT_FAMILY, "esstixeleven"); 
          break;
        case 12:
          XftPatternAddString (pat, XFT_FAMILY, "esstixtwelve"); 
          break;	  
        case 13: 
          XftPatternAddString (pat, XFT_FAMILY, "esstixthirteen"); 
          break;
        case 14:
          XftPatternAddString (pat, XFT_FAMILY, "esstixfourteen"); 
          break;	  
        case 15:
          XftPatternAddString (pat, XFT_FAMILY, "esstixfifteen"); 
          break;
        case 16:
          XftPatternAddString (pat, XFT_FAMILY, "esstixsixteen"); 
          break;	  
        case 17:
          XftPatternAddString (pat, XFT_FAMILY, "esstixseventeen"); 
          break;	  
        default:
          break;
        }
    }
  else if (script == 'Z')
    {
      
      /*iso10646-1*/
      /*XftPatternAddString (pat, XFT_ENCODING, "iso10646-1");*/
 
      XftPatternAddString (pat, XFT_ENCODING, "jisx0201.1976-*");
      XftPatternAddString (pat, XFT_ENCODING, "jisx0208.1983-*");
      XftPatternAddString (pat, XFT_ENCODING, "jisx0212.1990-*");
      XftPatternAddString (pat, XFT_ENCODING, "iso-2022-jp");
      
      XftPatternAddString (pat, XFT_FOUNDRY, "jiis");
      XftPatternAddString (pat, XFT_FOUNDRY, "wadalab");
      XftPatternAddString (pat, XFT_FOUNDRY, "watanabe");
      XftPatternAddString (pat, XFT_FOUNDRY, "sony");
      XftPatternAddString (pat, XFT_FOUNDRY, "misc");
      XftPatternAddString (pat, XFT_FOUNDRY, "dynalab");      

      /*
        XftPatternAddString (pat, XFT_FOUNDRY, "mincho");
        XftPatternAddString (pat, XFT_FOUNDRY, "gothic");
        XftPatternAddString (pat, XFT_FOUNDRY, "fixed");
        XftPatternAddString (pat, XFT_FOUNDRY, "unknown");
        XftPatternAddString (pat, XFT_FAMILY, "dfghoticu_w5");
        XftPatternAddString (pat, XFT_FOUNDRY, "dfminchou_w3");
        XftPatternAddString (pat, XFT_FAMILY, "dfghoticu_w3");
      */

      XftPatternAddString (pat, XFT_FAMILY, "dfminchou_w3");
      XftPatternAddString (pat, XFT_FAMILY, "dfghoticu_w5");

      if (highlight == 0 || highlight == 2 || highlight == 3)
        XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_LIGHT);
      else
        XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_BOLD);
      
      if (highlight == 0 || highlight == 1)
        XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_ROMAN);
      else
        XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_OBLIQUE);
      
      if (size < 0)
        size = 13;
    }
  else
    {
      XftPatternAddString (pat, XFT_ENCODING, "iso8859-1");
      /*XftPatternAddString (pat, XFT_FOUNDRY, "adobe");*/
      /*XftPatternAddString (pat, XFT_FOUNDRY, "microsoft");*/
      switch (family)
        {
        case 1:
          XftPatternAddString (pat, XFT_FAMILY, "Times New Roman");
          XftPatternAddString (pat, XFT_FAMILY, "times");
          XftPatternAddString (pat, XFT_FAMILY, "Times");
          XftPatternAddString (pat, XFT_FAMILY, "georgia");
          XftPatternAddString (pat, XFT_FAMILY, "lucidux");
          XftPatternAddString (pat, XFT_FAMILY, "Nimbus Roman No9 L");
          XftPatternAddString (pat, XFT_FAMILY, "terminus");
          XftPatternAddString (pat, XFT_FAMILY, "lucidabright");
          XftPatternAddString (pat, XFT_FAMILY, "new century schoolbook");
          XftPatternAddString (pat, XFT_FAMILY, "utopia");
          XftPatternAddString (pat, XFT_FAMILY, "Utopia");
          /* XftPatternAddString (pat, XFT_FAMILY, "charter"); */
          XftPatternAddString (pat, XFT_FAMILY, "terminal");
          break;
        case 2:
          XftPatternAddString (pat, XFT_FAMILY, "Arial");
          XftPatternAddString (pat, XFT_FAMILY, "arial");	      
          XftPatternAddString (pat, XFT_FAMILY, "verdana");
          XftPatternAddString (pat, XFT_FAMILY, "Verdana");
          XftPatternAddString (pat, XFT_FAMILY, "helvetica");
          XftPatternAddString (pat, XFT_FAMILY, "Helvetica");
          XftPatternAddString (pat, XFT_FAMILY, "ArmNet Helvetica");
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
      
      switch (highlight)
        {
        case 0:
          XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_LIGHT);
          XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_MEDIUM);
          XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_ROMAN);
          break;
        case 1:
          XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_LIGHT);
          XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_BOLD);	  
          XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_ROMAN);
          break;
        case 2:
          XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_LIGHT);
          XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_MEDIUM);
          if (family == 2 || family == 3)
            XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_OBLIQUE);
          else
            XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_ITALIC);
          break;
        case 3:
          XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_LIGHT);
          XftPatternAddInteger (pat, XFT_WEIGHT, XFT_WEIGHT_MEDIUM);
          if (family == 2 || family == 3)
            XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_OBLIQUE);
          else
            XftPatternAddInteger (pat, XFT_SLANT, XFT_SLANT_ITALIC);
          break;
        case 4:
        case 5:
          if (family == 2 || family == 3)
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

  if (script != 'E')
    XftPatternAddDouble (pat, XFT_SIZE, ((double) size) / 10.0);

  match = XftFontMatch (GDK_DISPLAY(), 0, pat, &result);   
  if (match) 
    {
      if (XftPatternGetString (match, XFT_FILE, 0, &s) == XftResultMatch)
        {
          strcpy (filename, s);  
          ok = 1;
          /* if (strstr (filename, "gz")) */
          /* 	   ok = 0; */
          if (script == 'E')
            if (strstr (filename, "esstix") == NULL)
              ok = 0;
#ifdef _PCLFONTDEBUG 
          g_print ("\n %s \t[script : %c (%i) family : %i] \t=> %i", 
                   filename, script, script, family, ok);
#endif /*_PCLFONTDEBUG*/
        }
      XftPatternDestroy (match);
    }
  XftPatternDestroy (pat); 
  return ok;
#endif /* (_GTK) || (defined(_WX) && !defined(_WINDOWS) && !defined(_MACOS)) */

}

#endif /* _GL */
