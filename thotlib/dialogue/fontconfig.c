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

#ifdef _FONTCONFIG

#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "appdialogue.h"
#include "application.h"

#ifdef _GTK
#include <gdk/gdkx.h>
#endif /*_GTK*/

/* Each Family can 
   have different 
   font style*/
typedef struct FontFamilyConfig
{
  char *highlight[6];
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
#endif

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
#ifdef _GTK
      fontlist = XListFonts (GDK_DISPLAY(), 
			     pattern, 
			     1, 
			     &count);
#else /*_GTK*/
      fontlist = XListFonts (TtDisplay, 
			     pattern, 
			     1, 
			     &count);
#endif /*_GTK*/
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
int isnum( char c )
{
  return( c >= '0' && c <= '9' ? 1 : 0 );
}

/*----------------------------------------------------------------------
   AdvanceNextWord                                                    
  ----------------------------------------------------------------------*/
static int AdvanceNextWord (unsigned char *line, int indline)
{
  
  while (!isnum (line[indline]) 
	 && line[indline] != '\0')
    {
      if (line[indline] == '#')
	while (line[indline] != EOL 
	       && line[indline] != EOS)
	  indline++;
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
   while ( line[indline] != ';' &&
	   line[indline] != EOS)
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
   /* skip all char if there are */
   while (line[indline] != EOL 
	  && line[indline] != EOS)
     {
       if (line[indline] == '=')
	 {
	   /* get the font-face highlight number*/
	   word[indword++] = line[indline -1 ];
	   word[indword++] = EOS;
	   indline++;
	   /*we return to the '1=' */
	   while (line[indline] >= SPACE && 
		  line[indline] != ';' &&
		  line[indline] != EOS)
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
   while ( !(line[indline-4] == EOL &&
	      line[indline-2] == EOL &&
	      line[indline] == EOL) &&
	  line[indline] != EOS)
#else     
   while ( !(line[indline-2] == EOL &&
	      line[indline-1] == EOL &&
	      line[indline] == EOL) &&
	  line[indline] != EOS)
#endif
     {
       if (isnum (line[indline]))
	 {
	   while (line[indline] > SPACE && 
		  line[indline] != ';' &&
		  line[indline] != EOS)
	     word[indword++] = line[indline++];
	   /* marque la fin du mot trouve' */
	   word[indword] = EOS;
	   return indline;
	 }
       indline++;     
     }
   return (indline);
}

#undef MAX_TXT_LEN
#define MAX_TXT_LEN 8144 
/*----------------------------------------------------------------------
   FontConfigLoad : Fill a structure for name - font correspondance
  ----------------------------------------------------------------------*/
static FontScript **FontConfigLoad ()
{  
  FontScript         **Fonts;
  FILE               *file;
  char                line[MAX_TXT_LEN];
  char                word[MAX_TXT_LEN];
  char                fname[MAX_TXT_LEN];
  char *Thot_Dir;
  int indline, script, highlight, family;
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
   Fonts = TtaGetMemory (31 * sizeof (FontScript **));
   for (script = 0; script < 30; script++)
     Fonts[script] = NULL;
   line[0] = EOS;
   while (fread (line, 1, MAX_TXT_LEN - 1, file))
       {
	 indline = 0;
	 while (line[indline] != EOS)
	   {
	     /*reads the script*/
	     indline = getWord (indline, line, word);
	     if (indline && word[0] != EOS)
	       {
		 script = atoi (word);
		 if (script < 30 && Fonts[script] == NULL)
		   {
		     Fonts[script] = TtaGetMemory (sizeof (FontScript));
		     for (family = 0; family < 6; family++)
		      Fonts[script]->family[family] = NULL;
		     family = 0;
		     /*reads all family for a script*/
		     while (indline != 0)
		       {
			 indline = getFontFamily (indline, line, word);
			 if (word[0] == EOS)
			   break;
			 family = atoi (word);	
			 if (family <= 6 && 
				 Fonts[script]->family[family] == NULL)
			   {
			     Fonts[script]->family[family] = 
			       TtaGetMemory (sizeof (FontScript));
			     /*reads all highlights*/
			     for (highlight = 0;highlight < 6; highlight++)
			       Fonts[script]->family[family]->highlight[highlight] = NULL;
			     highlight = 0;
			     while (indline != 0)
			       {			 
				 indline = getFontFace (indline, line, word);  
				 if (word[0] == EOS)
				   break;
				 highlight = atoi (word);
				 if (highlight < 6 && 
					 Fonts[script]->family[family]->highlight[highlight] == NULL)
				   {
				     /*Get the font-face in 
				       1=font-face 
				       string (so +1-1)*/
				     fontface = TtaGetMemory (sizeof (char) 
							      * (strlen (&word[2]) + 1));
				     strcpy (fontface, &word[2]);
				     Fonts[script]->family[family]->highlight[highlight]
				       = fontface;
				   }
			       }
			   }
		       }
		   }
	       }
	   }
       }
   TtaReadClose (file);
   return Fonts;
}

static FontScript **Fonttab = NULL;
/*----------------------------------------------------------------------
   FontLoadFromConfig : GEt a font dame upon its characteristics
  ----------------------------------------------------------------------*/
char *FontLoadFromConfig (char script, 
			  int family, 
			  int highlight)
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
	switch (family)
	{
	case 6:
	    family = 2;
	    highlight = 1;
	    break;
	case 7:
	    family = 2;
	    highlight = 2;
	    break;	  
	case 10:
	    family = 3;
	    highlight = 1;
	    break;
	default:
	    intscript = 20;
	    family = 1;
	    highlight = 1;
	    break;
	}
      break;
    case 'G':
      /*Symbols*/
      intscript = 20;
      family = 1;
      highlight = 1;
      break;
    case 'L':
      /* Latin ? */
      intscript = 1;
      break;
    case 'Z':
      /*unicode ??*/
      intscript = 0;
      family = 1;
      highlight = 1;
      break;
    default:
      intscript = atoi (&script);
      if (intscript < 0 || intscript > 9)
	intscript = 1;
      break;
    }

  if (intscript != 21)
    {
      switch (highlight)
	{
	 case 2:
     case 3:
	  highlight = 3;
	  break;
     case 1:
     case 4:
     case 5:
	  highlight = 2;
	  break;
	default:
	   highlight = 1;
	  break;
	}
    }
  
    if (family < 0 || family > 5)
    family = 1;

  if (Fonttab[intscript])
    if (Fonttab[intscript]->family[family])
		if (Fonttab[intscript]->family[family]->highlight[highlight])
		{

#ifdef _PCLFONTDEBUG
		g_print ("\n%s",
	       Fonttab[intscript]->family[family]->highlight[highlight]);
#endif /*_PCLFONTDEBUG*/
#ifndef _GL
      if (IsXLFDPatterneAFont (Fonttab[intscript]->family[family]->highlight[highlight]))
      	return (Fonttab[intscript]->family[family]->highlight[highlight]);
      else
	return NULL;
#else /*_GL*/
      return  (Fonttab[intscript]->family[family]->highlight[highlight]);
#endif /*_GL*/
    }
  
  return NULL;
}


/*----------------------------------------------------------------------
   FreeFontConfig : Free teh correspondance structure
  ----------------------------------------------------------------------*/
void FreeFontConfig ()
{
  int script, family, highlight;
  
  if (Fonttab == NULL)
    return;
  
  script = 0;
  while (script < 30)
    {
      if (Fonttab[script])
	{
	  family = 0;
	  while (family < 5)
	    {
	      if (Fonttab[script]->family[family])
		{
		  highlight = 0;
		  while (highlight < 5)
		    {
		      if (Fonttab[script]->family[family]->highlight[highlight])
			TtaFreeMemory (Fonttab[script]->family[family]->highlight[highlight]);
		      highlight++;
		    }
		  TtaFreeMemory (Fonttab[script]->family[family]);
		}
	      family++;
	    }
	  TtaFreeMemory (Fonttab[script]);
	}
      script++;
    }
}

#endif /* _FONTCONFIG */
