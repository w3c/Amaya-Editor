/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Module dedicated to font selection upon a user file defining the fonts
 *
 * Author: P. Cheyrou-lagreze (INRIA)
 *         I. Vatton (INRIA) test if referred files are available
 *
 */
#if defined(_WX) && defined(_WINDOWS)
#include "wx/wx.h"
#include "wx/utils.h"
#endif /* _WX && _WINDOWS */

#include "thot_gui.h"
#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "appdialogue.h"
#include "application.h"
#include "registry_f.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "font_tv.h"

#ifdef _GTK
  #include <gdk/gdkx.h>
#endif /*_GTK*/

#undef MAX_TXT_LEN
#define MAX_TXT_LEN 8142 
#define MAX_FONT_FACE 6
#define MAX_FONT_STYLE 7

/* Each Family can 
   have different 
   font style*/
typedef struct FontFamilyConfig
{
  char     *highlight[MAX_FONT_STYLE];
  /*
    char *bold;
    char *italic;
    char *normal;
  */
} FontFamilyConfig;

/* 
   CSS 2 generic font families 
   for each language 
*/
typedef struct FontScript
{
  FontFamilyConfig *family[MAX_FONT_FACE];
  /*
    FontFamily *serif;
    FontFamily *sansserif;
    FontFamily *monospace;
    FontFamily *cursive;
    FontFamily *fantasy;
  */
} FontScript;

static FontScript **Fonttab = NULL;


#ifndef _GL
/*----------------------------------------------------------------------
  IsXLFDName returns TRUE if the font is an available Windows font.
  ----------------------------------------------------------------------*/
static ThotBool IsXLFDName (char *font)
{
  int k = 0;
    
  while (*font)
    {
      if (*font++ == '-')
        k++;
    }
  
#ifdef _GTK
  return (k == 14) ? TRUE : FALSE;
#endif /* #ifdef _GTK */
#ifdef _WINGUI
  return (k == 2) ? TRUE : FALSE;
#endif /*_WINGUI*/  
}

/*----------------------------------------------------------------------
  IsXLFDFont returns TRUE if the font is an available X font.
  ----------------------------------------------------------------------*/
static int IsXLFDPatterneAFont (char *pattern)
{
#ifdef _GTK
  char    **fontlist;
  int       count = 0;  

  if (IsXLFDName (pattern))
    {
      fontlist = XListFonts (TtDisplay, pattern, 1, &count);
      if (count)
        {
          XFreeFontNames(fontlist);
          return 1;      
        }
    }
  return 0;
#endif /*#ifdef _GTK*/
#ifdef _WINGUI
  return IsXLFDName (pattern);  
#endif /*_WINGUI*/
}
#endif /*_GL*/

/*----------------------------------------------------------------------
  isnum                                                    
  ----------------------------------------------------------------------*/
int isnum (char c)
{
  return (c >= '0' && c <= '9');
}

/*----------------------------------------------------------------------
  AdvanceNextWord                                                    
  ----------------------------------------------------------------------*/
static int AdvanceNextWord (unsigned char *line, int indline)
{
  while (line[indline] != EOS && !isnum (line[indline]))
    {
      if (line[indline] == '#')
        {
          /* skip to the end of the current line */
          while (line[indline] != EOS && line[indline] != EOL)
            indline++;
        }
      else
        indline++;
    }
  return indline;
}

/*----------------------------------------------------------------------
  getWord                                                    
  ----------------------------------------------------------------------*/
static int getWord (int indline, unsigned char *line, char *word)
{
  int             indword;

  /*place ourself next to a word*/
  indline = AdvanceNextWord (line, indline);
  /* copy the word from the line*/
  indword = 0;
  word[0] = EOS;
  while (line[indline] == __CR__ || line[indline] == EOL)
    indline++;    
  while (line[indline] != EOS && line[indline] != ';' && line[indline] != EOL)
    word[indword++] = line[indline++];
  /* marque la fin du mot trouve' */
  word[indword] = EOS;
  if (indword == 0)
    {
      line[0] = EOS;
      return 0;
    }
  if (line[indline] == ';')
    indline++;
  /*place ourself next to a word*/
  indline = AdvanceNextWord (line, indline);
  return (indline);
}

/*----------------------------------------------------------------------
  getFontFamily                                                  
  ----------------------------------------------------------------------*/
static int getFontFamily (int indline, unsigned char *line, char *word)
{
  int             indword;

  /* copy the word from the line*/
  indword = 0;
  word[0] = EOS;
  if (line[indline] == EOL)
    indline++;
  if (line[indline] == __CR__)
    indline++;
  if (line[indline] == EOL)
    /* end of the current font family list */
    return indline;
  while (line[indline] != EOS && line[indline] != EOL)
    {
      if (isnum (line[indline]))
        {
          while (line[indline] != EOS &&
                 line[indline] > SPACE && line[indline] != ';')
            word[indword++] = line[indline++];
          if (line[indline] == ';')
            indline++;
          if (line[indline] == __CR__)
            indline++;
          /* word found */
          word[indword] = EOS;
          return indline;
        }
      else
        indline++;     
    }
  return (indline);
}

/*----------------------------------------------------------------------
  getFontFace                                                  
  ----------------------------------------------------------------------*/
static int getFontFace (int indline, unsigned char *line, char *word)
{
  int             indword;

  indword = 0;
  word[0] = EOS;  
  if (line[indline] == EOL)
    /* end of the current font face list */
    return indline;
  while (line[indline] != EOS && line[indline] != ';'
         && line[indline] != EOL)
    {
      /* get the font-face highlight number */
      word[indword] = line[indline];
      if (line[indline] == '=')
        /* get the font-face string */
        word[indword++] = EOS;
      else if (line[indline] == __CR__)
        /* get the font-face string */
        word[indword] = EOS;
      else
        indword++;
      indline++;
    }
  if (line[indline] != EOS)
    indline++;
  if (line[indline] == __CR__)
    indline++;
  /* mark the end of the word */
  word[indword] = EOS;
  return (indline);
}

/*----------------------------------------------------------------------
  FontLoadFile : Fill the structure for name - font correspondance
  Return TRUE if all entries are correct.
  ----------------------------------------------------------------------*/
static ThotBool FontLoadFile ( FILE *file, FontScript **fontsscript_tab)
{
  char                word[MAX_TXT_LEN];
  char               *line;
  char               *fontface;
  int                 endfile, indline, script, style, face;
  ThotBool            complete;

  line = NULL;
  complete = TRUE;
  /* get the size of the file */
  fseek (file, 0L, 2);	/* end of the file */
  endfile = ftell (file) + 1;
  fseek (file, 0L, 0);	/* beginning of the file */
  line = (char *)TtaGetMemory (endfile);
  indline = fread (line, 1, endfile, file);
  line[endfile-1] = EOS;
  indline = 0;
  while (indline < endfile && line[indline] != EOS)
    {
      /*reads the script*/
      indline = getWord (indline, (unsigned char*)line, word);
      if (indline < endfile && indline && word[0] != EOS)
        {
          script = atoi (word);
          if (script >= 0 && script < 30)
            {
              if (fontsscript_tab[script] == NULL)
                {
                  /* first loading */
                  fontsscript_tab[script] = (FontScript*)TtaGetMemory (sizeof (FontScript));
                  for (face = 0; face < MAX_FONT_FACE; face++)
                    fontsscript_tab[script]->family[face] = NULL;
                }
              face = 0;
              /* reads all family for a script */
              while (indline != 0 && indline < endfile)
                {
                  indline = getFontFamily (indline, (unsigned char*)line, word);
                  if (word[0] == EOS)
                    break;
                  face = atoi (word);
                  if (face < MAX_FONT_FACE && face >= 0)
                    {
                      if (fontsscript_tab[script]->family[face] == NULL)
                        {
                          /* first loading */
                          fontsscript_tab[script]->family[face] = (FontFamilyConfig*)TtaGetMemory (sizeof (FontFamilyConfig));
                          for (style = 0; style < MAX_FONT_STYLE; style++)
                            fontsscript_tab[script]->family[face]->highlight[style] = NULL;
                        }
                      /* reads all highlights */
                      style = 0;
                      while (indline != 0 && indline < endfile)
                        {
                          indline = getFontFace (indline, (unsigned char*)line, word);  
                          if (word[0] == EOS)
                            break;
                          style = atoi (word);
                          if (style < MAX_FONT_STYLE && style >= 0 &&
                              fontsscript_tab[script]->family[face]->highlight[style] == NULL)
                            {
                              /*Get the font-face in 1=font-face string (so +1-1)*/
#ifdef _GL
                              if (!strncmp (&word[2], "$THOTDIR", 8))
                                {
                                  char filename[MAX_TXT_LEN], *Thot_Dir;

                                  Thot_Dir = TtaGetEnvString ("THOTDIR");
                                  if (Thot_Dir)
                                    strcpy (filename, Thot_Dir);
                                  else
                                    filename[0] = EOS;
                                  strcat (filename, &word[10]);
                                  if (!TtaFileExist (filename))
                                    complete = FALSE;
                                  else
                                    {
                                      fontface = TtaStrdup (filename);
                                      fontsscript_tab[script]->family[face]->highlight[style] = fontface;
                                      /* note if STIX fonts are available */
                                      if (script == 21 && !StixExist)
                                        StixExist = TRUE;
                                    }
                                }
#if defined(_WX) && defined(_WINDOWS)
                              else if (!strncmp (&word[2], "$OSDIR", 6))
                                {
                                  char     filename[MAX_TXT_LEN];
                                  wxString wx_osdir = wxGetOSDirectory();

                                  if (wx_osdir.mb_str(wxConvUTF8))
                                    strcpy (filename, wx_osdir.mb_str(wxConvUTF8));
                                  else
                                    filename[0] = EOS;
                                  strcat (filename, &word[8]);
                                  if (!TtaFileExist (filename))
                                    complete = FALSE;
                                  else
                                    {
                                      fontface = TtaStrdup (filename);
                                      fontsscript_tab[script]->family[face]->highlight[style] = fontface;
                                      /* note if STIX fonts are available */
                                      if (script == 21 && !StixExist)
                                        StixExist = TRUE;
                                    }
                                }
#endif /* _WX && _WINDOWS */
                              else if (!TtaFileExist (&word[2]))
#else /* _GL */
                                if (!IsXLFDPatterneAFont (&word[2]))
#endif /* _GL */
                                  {
                                    complete = FALSE;
                                  }		
                                else
                                  {
                                    fontface = TtaStrdup (&word[2]);
                                    fontsscript_tab[script]->family[face]->highlight[style] = fontface;
                                  }
                            }
                        }
                    }
                }
            }
        }
      else
        break;	     
    }
  TtaFreeMemory (line);
  return complete;
}

/*----------------------------------------------------------------------
  FontConfigLoad : Fill a structure for name - font correspondance
  ----------------------------------------------------------------------*/
static FontScript **FontConfigLoad ()
{  
  FontScript        **fontsscript_tab;
  FILE               *file;
  char                fname[MAX_TXT_LEN], name[MAX_TXT_LEN];
  char                word[50];
  char               *appHome;
  ThotBool            complete;

  appHome = TtaGetEnvString ("APP_HOME");
#ifdef _GL
#ifdef _WINDOWS
  strcpy (word, "fonts.gl.win");  
#else /* _WINDOWS */
  strcpy (word, "fonts.gl");  
#endif /* _WINDOWS */
#else /* _GL */
  strcpy (word, "fonts.win");  
#endif /* _GL */

  strcpy (fname, appHome);
  strcat (fname, DIR_STR);
  strcat (fname, word);
  if (!SearchFile (fname, 0, name))
    SearchFile (word, 2, name);
  /* open the fonts definition file */
  file = TtaReadOpen (name);
  if (file == NULL)
    {
      fprintf (stderr, "cannot open font definition file %s\n", fname);
      return NULL;
    }

  /* Allocate the table */
  fontsscript_tab = (FontScript **)TtaGetMemory (31 * sizeof (FontScript *));
  memset(fontsscript_tab, 0, 31 * sizeof(FontScript *));
  /* load the first config file */
  complete = FontLoadFile (file, fontsscript_tab);
  TtaReadClose (file);
  return fontsscript_tab;
}

/*----------------------------------------------------------------------
  FontLoadFromConfig : Get a font name upon its characteristics
  ----------------------------------------------------------------------*/
char *FontLoadFromConfig (char script, int face, int style)
{
  char s[2];
  int  intscript = 1;

  if (Fonttab == NULL)
    Fonttab = FontConfigLoad ();
  if (Fonttab == NULL)
    return NULL;

  switch (script) 
    {
    case 'F':
      intscript = 13;
      break;
    case 'D':
      intscript = 15;      
      break;
    case 'X':
      intscript = 18;      
      break;
    case 'E':
      /* ESSTIX FONTS ???*/
      intscript = 21;
      switch (face)
        {
        case 1:
          face = 1;
          style = 1;
          break;
        case 2:
          face = 1;
          style = 2;
          break;
        case 3:
          face = 1;
          style = 3;
          break;
        case 4:
          face = 1;
          style = 4;
          break;
        case 5:
          face = 1;
          style = 5;
          break;	  
        case 6:
          face = 2;
          style = 1;
          break;
        case 7:
          face = 2;
          style = 2;
          break;	  
        case 8:
          face = 2;
          style = 3;
          break;
        case 9:
          face = 2;
          style = 4;
          break;	  
        case 10:
          face = 2;
          style = 5;
          break;
        case 11:
          face = 3;
          style = 1;
          break;
        case 12:
          face = 3;
          style = 2;
          break;	  
        case 13:
          face = 3;
          style = 3;
          break;
        case 14:
          face = 3;
          style = 4;
          break;
        case 15:
          face = 3;
          style = 5;
          break;
        case 16:
          face = 4;
          style = 1;
          break;
        case 17:
          face = 4;
          style = 2;
          break;	  
        default:
          intscript = 20;
          face = 1;
          style = 1;
          break;
        }
      break;
    case 'G':
      /*Symbols*/
      intscript = 20;
      face = 1;
      style = 1;
      break;
    case 'L':
      /* Latin ? */
      intscript = 1;
      break;
    case 'Z':
      /*unicode ??*/
      intscript = 0;
      face = 1;
      style = 1;
      break;
    default:
      s[0] = script;
      s[1] = EOS;
      intscript = atoi (s);
      if (intscript < 0 || intscript > 9)
        intscript = 1;
      break;
    }

  if (intscript != 21)
    {
      switch (style)
        {
        case 1:
          style = 2;
          break;
        case 0:
          style = 1;
          break;
        case 2:
          style = 3;
          break;
        case 5:
          style = 2;
          break;
        default:
          break;
        }
    }
  
  if (face < 0 || face >= MAX_FONT_FACE)
    face = 1;

  if (Fonttab[intscript] &&
      Fonttab[intscript]->family[face] &&
      Fonttab[intscript]->family[face]->highlight[style])
    return (Fonttab[intscript]->family[face]->highlight[style]);
  else
    return NULL;
}


/*----------------------------------------------------------------------
  FreeFontConfig : Free teh correspondance structure
  ----------------------------------------------------------------------*/
void FreeFontConfig ()
{
  int script, face, style;
  
  if (Fonttab == NULL)
    return;
  
  script = 0;
  while (script < 30)
    {
      if (Fonttab[script])
        {
          face = 0;
          while (face < MAX_FONT_FACE)
            {
              if (Fonttab[script]->family[face])
                {
                  style = 0;
                  while (style < MAX_FONT_STYLE)
                    {
                      if (Fonttab[script]->family[face]->highlight[style])
                        {
                          TtaFreeMemory (Fonttab[script]->family[face]->highlight[style]);
                          Fonttab[script]->family[face]->highlight[style] = NULL;
                        }
                      style++;
                    }
                  TtaFreeMemory (Fonttab[script]->family[face]);
                  Fonttab[script]->family[face] = NULL;
                }
              face++;
            }
          TtaFreeMemory (Fonttab[script]);
          Fonttab[script] = NULL;
        }
      script++;
    }
}
