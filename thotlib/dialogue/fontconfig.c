/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2002
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Module dedicated to font selection upon a user file defining the fonts
 *
 * Author: P. Cheyrou-lagreze (INRIA)
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

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"

#ifdef _GTK
#include <gdk/gdkx.h>
#endif /*_GTK*/

/* Each Family can 
   have different 
   font style*/
typedef struct FontFamilyConfig
{
  ThotBool is_xlfd[6];
  ThotBool is_xlfd_checked[6];
  char     *highlight[6];
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
  FontFamilyConfig *family[6];
  /*
  FontFamily *serif;
  FontFamily *sansserif;
  FontFamily *monospace;
  FontFamily *cursive;
  FontFamily *fantasy;
  */
} FontScript;

#undef MAX_TXT_LEN
#define MAX_TXT_LEN 8144 
#define MAX_FONT_FACE 6
#define MAX_FONT_STYLE 6
static FontScript **Fonttab = NULL;


#ifdef O
/*----------------------------------------------------------------------
   FontConfigCreate
  ----------------------------------------------------------------------*/
static void FontConfigCreate ()
{
  
}
/*----------------------------------------------------------------------
   FontConfigUserSelect                                                    
  ----------------------------------------------------------------------*/
static void FontConfigUserSelect ()
{

}
#endif /*o*/

#ifndef _GL
/*----------------------------------------------------------------------
   IsXLFDName                                                    
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
   IsXLFDFont                                                    
  ----------------------------------------------------------------------*/
static int IsXLFDPatterneAFont (char *pattern)
{
#ifndef _WINDOWS
  char **fontlist;
  int count=0;  

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
  return ((c >= '0') && 
	  (c <= '9'));
}

/*----------------------------------------------------------------------
   PassOnComments                                                   
  ----------------------------------------------------------------------*/
static int PassOnComments (unsigned char *line, int indline)
{
  while (indline < MAX_TXT_LEN && 
	 line[indline] == '#')
    {
      while (indline < MAX_TXT_LEN &&
	     line[indline] != EOS && 
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
  
  while (indline < MAX_TXT_LEN &&
	 line[indline] != EOS &&
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
   while (indline < MAX_TXT_LEN &&
	  line[indline] != EOS &&
	  line[indline] != ';')
     word[indword++] = line[indline++];
   /* marque la fin du mot trouve' */
   word[indword] = EOS;
   if (indword == 0)
     {
       line[0] = EOS;
       return 0;
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
   /*place ourself next to a word*/
   indline = AdvanceNextWord (line, indline); 
   /* skip all char if there are */
   while (indline < MAX_TXT_LEN &&
	  line[indline] != EOS &&
	  line[indline] != EOL)
     {
       if (line[indline] == '=')
	 {
	   /* get the font-face highlight number*/
	   word[indword++] = line[indline -1 ];
	   word[indword++] = EOS;
	   indline++;
	   /*we return to the '1=' */
	   while (indline < MAX_TXT_LEN &&
		  line[indline] != EOS &&
		  line[indline] >= SPACE && 
		  line[indline] != ';')
	     word[indword++] = line[indline++];
	   indline++;	   
	   /* marque la fin du mot trouve' */
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
   while (line[indline] != EOS &&
	  indline < MAX_TXT_LEN &&
	  !(line[indline-4] == EOL &&
	    line[indline-2] == EOL &&
	    line[indline] == EOL))
#else     
     while (line[indline] != EOS &&
	    indline < MAX_TXT_LEN &&
	    !(line[indline-2] == EOL &&
	      line[indline-1] == EOL &&
	      line[indline] == EOL))
#endif
     {
       if (isnum (line[indline]))
	 {
	   while (indline < MAX_TXT_LEN &&
		  line[indline] != EOS &&
		  line[indline] > SPACE && 
		  line[indline] != ';')
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
   FontConfigLoad : Fill a structure for name - font correspondance
  ----------------------------------------------------------------------*/
static FontScript **FontConfigLoad ()
{  
  FontScript        **fontsscript_tab;
  FILE               *file;
  char                line[MAX_TXT_LEN];
  char                word[MAX_TXT_LEN];
  char                fname[MAX_TXT_LEN];
  char *Thot_Dir;
  int endfile, indline, script, font_style, font_face_index;
  char *fontface;

  Thot_Dir = TtaGetEnvString ("THOTDIR");
  strcpy (fname, Thot_Dir);

#ifndef _GL
#ifdef _WINDOWS
  strcat (fname, "/config/fonts.win");
#else /*_WINDOWS*/
  strcat (fname, "/config/fonts.unix");  
#endif /*_WINDOWS*/
#else /*_GL*/
#ifdef _WINDOWS
  strcat (fname, "/config/fonts.gl.win");  
#else /*_WINDOWS*/
  strcat (fname, "/config/fonts.gl");  
#endif /*_WINDOWS*/  
#endif /*_GL*/

  /* open the fonts definition file */
   file = TtaReadOpen (fname);
   if (file == NULL)
     {
	fprintf (stderr, "cannot open font definition file %s\n", fname);
	return NULL;
     }

   /*Big Alloc*/
   fontsscript_tab = TtaGetMemory (31 * sizeof (FontScript *));
   for (script = 0; script < 30; script++)
     fontsscript_tab[script] = NULL;
   line[0] = EOS;
   while ((endfile = fread (line, 1, MAX_TXT_LEN - 2, file)))
       {
	 line[endfile] = '\0';	 
	 indline = 0;
	 while (indline < MAX_TXT_LEN && 
		line[indline] != EOS)
	   {
	     /*reads the script*/
	     indline = getWord (indline, line, word);
	     if (indline < MAX_TXT_LEN && indline && word[0] != EOS)
	       {
		 script = atoi (word);
		 if (script >= 0 && 
		     script < 30 )
		   if ( fontsscript_tab[script] == NULL)
		   {
		     fontsscript_tab[script] = TtaGetMemory (sizeof (FontScript));
		     for (font_face_index = 0; font_face_index < MAX_FONT_FACE; font_face_index++)
		      fontsscript_tab[script]->family[font_face_index] = NULL;
		     font_face_index = 0;
		     /*reads all family for a script*/
		     while (indline != 0 &&
			    indline < MAX_TXT_LEN)
		       {
			 indline = getFontFamily (indline, line, word);
			 if (word[0] == EOS)
			   break;
			 font_face_index = atoi (word);	
			 if (font_face_index < MAX_FONT_FACE && 
			     font_face_index >= 0)
			   if (fontsscript_tab[script]->family[font_face_index] == NULL)
			   {
			     fontsscript_tab[script]->family[font_face_index] = 
			       TtaGetMemory (sizeof (FontFamilyConfig));
			     /*reads all highlights*/
			     for (font_style = 0; font_style < MAX_FONT_STYLE; font_style++)
			       {
				 fontsscript_tab[script]->family[font_face_index]->highlight[font_style] = NULL;
				 fontsscript_tab[script]->family[font_face_index]->is_xlfd_checked[font_style] = FALSE;
				 fontsscript_tab[script]->family[font_face_index]->is_xlfd[font_style] = FALSE;
			       }
			     
			     font_style = 0;
			     while (indline != 0 &&
				    indline < MAX_TXT_LEN)
			       {			 
				 indline = getFontFace (indline, line, word);  
				 if (word[0] == EOS)
				   break;
				 font_style = atoi (word);
				 if (font_style < MAX_FONT_STYLE && 
				     font_style >= 0)
				   if (fontsscript_tab[script]->family[font_face_index]->highlight[font_style] == NULL)
				   {
				     /*Get the font-face in 
				       1=font-face 
				       string (so +1-1)*/
				     fontface = TtaGetMemory (sizeof (char) 
							      * (strlen (&word[2]) + 1));
				     strcpy (fontface, &word[2]);
				     fontsscript_tab[script]->family[font_face_index]->highlight[font_style]
				       = fontface;
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
   TtaReadClose (file);
   return fontsscript_tab;
}

/*----------------------------------------------------------------------
   FontLoadFromConfig : GEt a font dame upon its characteristics
  ----------------------------------------------------------------------*/
char *FontLoadFromConfig (char script, 
			  int font_face_index, 
			  int font_style)
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
	switch (font_face_index)
	{
	case 6:
	    font_face_index = 2;
	    font_style = 1;
	    break;
	case 7:
	    font_face_index = 2;
	    font_style = 2;
	    break;	  
	case 10:
	    font_face_index = 3;
	    font_style = 1;
	    break;
	default:
	    intscript = 20;
	    font_face_index = 1;
	    font_style = 1;
	    break;
	}
      break;
    case 'G':
      /*Symbols*/
      intscript = 20;
      font_face_index = 1;
      font_style = 1;
      break;
    case 'L':
      /* Latin ? */
      intscript = 1;
      break;
    case 'Z':
      /*unicode ??*/
      intscript = 0;
      font_face_index = 1;
      font_style = 1;
      break;
    default:
      intscript = atoi (&script);
      if (intscript < 0 || intscript > 9)
	intscript = 1;
      break;
    }

  if (intscript != 21)
    {
      switch (font_style)
	{
	 case 2:
     case 3:
	  font_style = 3;
	  break;
     case 1:
     case 4:
     case 5:
	  font_style = 2;
	  break;
	default:
	   font_style = 1;
	  break;
	}
    }
  
    if (font_face_index < 0 || font_face_index >= MAX_FONT_FACE)
    font_face_index = 1;

  if (Fonttab[intscript] &&
      Fonttab[intscript]->family[font_face_index] &&
      Fonttab[intscript]->family[font_face_index]->highlight[font_style])
    {
      
#ifdef _PCLFONTDEBUG
      g_print ("\n%s",
	       Fonttab[intscript]->family[font_face_index]->highlight[font_style]);
#endif /*_PCLFONTDEBUG*/
#ifndef _GL
      if (Fonttab[intscript]->family[font_face_index]->is_xlfd_checked[font_style] == FALSE)
	{
	  Fonttab[intscript]->family[font_face_index]->is_xlfd[font_style] = IsXLFDPatterneAFont (Fonttab[intscript]->family[font_face_index]->highlight[font_style]);
	  Fonttab[intscript]->family[font_face_index]->is_xlfd_checked[font_style] = TRUE;
	}
      if (Fonttab[intscript]->family[font_face_index]->is_xlfd[font_style])
	return (Fonttab[intscript]->family[font_face_index]->highlight[font_style]);
      else
	return NULL;

#else /*_GL*/
      if (Fonttab[intscript]->family[font_face_index]->is_xlfd_checked[font_style] == FALSE)
	{
	  Fonttab[intscript]->family[font_face_index]->is_xlfd[font_style] = TtaFileExist (Fonttab[intscript]->family[font_face_index]->highlight[font_style]);
	  Fonttab[intscript]->family[font_face_index]->is_xlfd_checked[font_style] = TRUE;
	}
      if (Fonttab[intscript]->family[font_face_index]->is_xlfd[font_style])
	return (Fonttab[intscript]->family[font_face_index]->highlight[font_style]);
      else
	return NULL;

      return  (Fonttab[intscript]->family[font_face_index]->highlight[font_style]);
#endif /*_GL*/
    }
  return NULL;
}


/*----------------------------------------------------------------------
   FreeFontConfig : Free teh correspondance structure
  ----------------------------------------------------------------------*/
void FreeFontConfig ()
{
  int script, font_face_index, font_style;
  
  if (Fonttab == NULL)
    return;
  
  script = 0;
  while (script < 30)
    {
      if (Fonttab[script])
	{
	  font_face_index = 0;
	  while (font_face_index < MAX_FONT_FACE)
	    {
	      if (Fonttab[script]->family[font_face_index])
		{
		  font_style = 0;
		  while (font_style < MAX_FONT_STYLE)
		    {
		      if (Fonttab[script]->family[font_face_index]->highlight[font_style])
			TtaFreeMemory (Fonttab[script]->family[font_face_index]->highlight[font_style]);
		      font_style++;
		    }
		  TtaFreeMemory (Fonttab[script]->family[font_face_index]);
		}
	      font_face_index++;
	    }
	  TtaFreeMemory (Fonttab[script]);
	}
      script++;
    }
}
