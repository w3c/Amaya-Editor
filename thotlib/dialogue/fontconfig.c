/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
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
#ifndef _WINDOWS
  return (k == 14) ? TRUE : FALSE;
#else /*_WINDOWS*/
  return (k == 2) ? TRUE : FALSE;
#endif /*_WINDOWS*/

}

/*----------------------------------------------------------------------
   IsXLFDFont returns TRUE if the font is an available X font.
  ----------------------------------------------------------------------*/
static int IsXLFDPatterneAFont (char *pattern)
{
#ifndef _WINDOWS
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
#else /*_WINDOWS*/
  return IsXLFDName (pattern);  
#endif /*_WINDOWS*/
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
   PassOnComments                                                   
  ----------------------------------------------------------------------*/
static int PassOnComments (unsigned char *line, int indline)
{
  while (indline < MAX_TXT_LEN && 
	 line[indline] == '#')
    {
      while (indline < MAX_TXT_LEN && line[indline] != EOS &&
	     line[indline] != EOL)
	indline++; 
    }
  if (indline == MAX_TXT_LEN)
    indline--;  
  return indline;
}

/*----------------------------------------------------------------------
   AdvanceNextWord                                                    
  ----------------------------------------------------------------------*/
static int AdvanceNextWord (unsigned char *line, int indline)
{
  while (indline < MAX_TXT_LEN && line[indline] != EOS &&
	 !isnum (line[indline]))
    {
      indline = PassOnComments (line, indline);
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
  while (indline < MAX_TXT_LEN && line[indline] != EOS &&
	 line[indline] != ';')
    word[indword++] = line[indline++];
  /* marque la fin du mot trouve' */
  word[indword] = EOS;
  if (indword == 0)
    {
      line[0] = EOS;
      return 0;
    }
  /*place ourself next to a word*/
  indline = AdvanceNextWord (line, indline);
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
  /* skip all char if there are */
  while (indline < MAX_TXT_LEN &&
	 line[indline] != EOS && line[indline] != EOL)
     {
       if (line[indline] == '=')
	 {
	   /* get the font-face highlight number*/
	   word[indword++] = line[indline-1];
	   word[indword++] = EOS;
	   indline++;
	   /*we return to the '1=' */
	   while (indline < MAX_TXT_LEN && line[indline] != EOS &&
		  line[indline] >= SPACE && line[indline] != ';')
	     word[indword++] = line[indline++];
	   indline++;
	   /* mark the end of the word */
	   word[indword] = EOS;
	   return indline;
	 }
       indline++;
     }
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
#ifdef _WINDOWS
   while (indline < MAX_TXT_LEN && line[indline] != EOS &&
	  (line[indline-4] != EOL ||
	   line[indline-2] != EOL ||
	   line[indline] != EOL))
#else     
     while (indline < MAX_TXT_LEN && line[indline] != EOS &&
	    (line[indline-2] != EOL ||
	     line[indline-1] != EOL ||
	     line[indline] != EOL))
#endif
     {
       if (isnum (line[indline]))
	 {
	   while (indline < MAX_TXT_LEN && line[indline] != EOS &&
		  line[indline] > SPACE && line[indline] != ';')
	     word[indword++] = line[indline++];
	   /* marque la fin du mot trouve' */
	   word[indword] = EOS;
	   return indline;
	 }
       indline++;     
     }
   return (indline);
}


/*----------------------------------------------------------------------
  FontLoadFile : Fill the structure for name - font correspondance
  Return TRUE if all entries are correct.
  ----------------------------------------------------------------------*/
static ThotBool FontLoadFile ( FILE *file, FontScript **fontsscript_tab)
{
  char                line[MAX_TXT_LEN];
  char                word[MAX_TXT_LEN];
  char               *fontface;
  int                 endfile, indline, script, style, face;
  ThotBool            complete;

  line[0] = EOS;
  complete = TRUE;
  while ((endfile = fread (line, 1, MAX_TXT_LEN - 1, file)))
    {
      line[endfile] = '\0';	 
      indline = 0;
      while (indline < MAX_TXT_LEN && line[indline] != EOS)
	{
	  /*reads the script*/
	  indline = getWord (indline, line, word);
	  if (indline < MAX_TXT_LEN && indline && word[0] != EOS)
	    {
	      script = atoi (word);
	      if (script >= 0 && script < 30)
		{
		  if (fontsscript_tab[script] == NULL)
		    {
		    /* first loading */
		      fontsscript_tab[script] = TtaGetMemory (sizeof (FontScript));
		      for (face = 0; face < MAX_FONT_FACE; face++)
			fontsscript_tab[script]->family[face] = NULL;
		    }
		  face = 0;
		  /* reads all family for a script */
		  while (indline != 0 && indline < MAX_TXT_LEN)
		    {
		      indline = getFontFamily (indline, line, word);
		      if (word[0] == EOS)
			break;
		      face = atoi (word);
		      if (face < MAX_FONT_FACE && face >= 0)
			{
			  if (fontsscript_tab[script]->family[face] == NULL)
			    {
			      /* first loading */
			      fontsscript_tab[script]->family[face] = TtaGetMemory (sizeof (FontFamilyConfig));
			      for (style = 0; style < MAX_FONT_STYLE; style++)
				fontsscript_tab[script]->family[face]->highlight[style] = NULL;
			    }
			  /* reads all highlights */
			  style = 0;
			  while (indline != 0 && indline < MAX_TXT_LEN)
			    {			 
			      indline = getFontFace (indline, line, word);  
			      if (word[0] == EOS)
				break;
			      style = atoi (word);
			      if (style < MAX_FONT_STYLE && style >= 0 &&
				  fontsscript_tab[script]->family[face]->highlight[style] == NULL)
				{
				  /*Get the font-face in 1=font-face string (so +1-1)*/
				  fontface = TtaStrdup (&word[2]);
#ifdef _GL
				  if (!TtaFileExist (fontface))
#else /* _GL */
				  if (!IsXLFDPatterneAFont (fontface))
#endif /* _GL */
				    {
				      complete = FALSE;
				      /*printf ("Font file %s not found\n", fontface)*/;
				    }
				  else
				    fontsscript_tab[script]->family[face]->highlight[style] = fontface;
				}
			    }
			}
		    }
		}
	    }
	  else
	    break;	     
	}
    }
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
  int                 script;
  ThotBool            complete;

  appHome = TtaGetEnvString ("APP_HOME");
#ifndef _GL
#ifdef _WINDOWS
  strcpy (word, "fonts.win");  
#else /*_WINDOWS*/
  strcpy (word, "fonts.unix");  
#endif /*_WINDOWS*/
#else /*_GL*/
#ifdef _WINDOWS
  strcpy (word, "fonts.gl.win");  
#else /*_WINDOWS*/
  strcpy (word, "fonts.gl");  
#endif /*_WINDOWS*/  
#endif /*_GL*/

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

  /*Allocate the table */
  fontsscript_tab = TtaGetMemory (31 * sizeof (FontScript *));
  for (script = 0; script < 30; script++)
    fontsscript_tab[script] = NULL;
  /* load the first config file */
  complete = FontLoadFile (file, fontsscript_tab);
  TtaReadClose (file);

  if (!complete)
    {
      /* try a second font file */
      strcat (word, ".deb");
      strcat (fname, ".deb");
      if (!SearchFile (fname, 0, name))
	SearchFile (word, 2, name);
      /* open the fonts definition file */
      file = TtaReadOpen (name);
      if (file)
	complete = FontLoadFile (file, fontsscript_tab);
    }
  return fontsscript_tab;
}

/*----------------------------------------------------------------------
   FontLoadFromConfig : GEt a font dame upon its characteristics
  ----------------------------------------------------------------------*/
char *FontLoadFromConfig (char script, int face, int style)
{
  int intscript = 1;

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
    case 'E':
      /* ESSTIX FONTS ???*/
      intscript = 21;
      switch (face)
	{
	case 6:
	  face = 2;
	  style = 1;
	  break;
	case 7:
	  face = 2;
	  style = 2;
	  break;	  
	case 10:
	  face = 3;
	  style = 1;
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
      intscript = atoi (&script);
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
			TtaFreeMemory (Fonttab[script]->family[face]->highlight[style]);
		      style++;
		    }
		  TtaFreeMemory (Fonttab[script]->family[face]);
		}
	      face++;
	    }
	  TtaFreeMemory (Fonttab[script]);
	}
      script++;
    }
}
