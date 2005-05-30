/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * xpmhandler.c  Pixmap V3.4.c
 *
 * Author: I. Vatton, N. Layaida (INRIA)
 */

#include "thot_gui.h"
#include "thot_sys.h"

#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "frame.h"
#include "libmsg.h"
#include "message.h"

#define THOT_EXPORT extern
#include "picture_tv.h"
#include "frame_tv.h"

#include "xpm.h"

#include "picture_f.h"
#include "inites_f.h"
#include "font_f.h"
#include "gifhandler_f.h"
#include "units_f.h"


/*----------------------------------------------------------------------
   XpmCreate reads and produces the bitmap read from the file      
   fn. updates the wif, hif, xif , yif                     
  ----------------------------------------------------------------------*/
ThotDrawable XpmCreate (char *fn, ThotPictInfo *imageDesc, int *xif, int *yif,
		    int *wif, int *hif, int bgColor, int *width,
		    int *height, int zoom)
{
  *width = 0;
  *height = 0;
  *wif = 0;
  *hif = 0;
  *xif = 0;
  *yif = 0;
  return (ThotDrawable) (NULL);
}


/*----------------------------------------------------------------------
   XpmPrint converts an xpm file to PostScript.                    
  ----------------------------------------------------------------------*/
void XpmPrint (char *fn, PictureScaling pres, int xif, int yif, int wif,
	       int hif, FILE *fd, int bgColor)
{
}			

/*----------------------------------------------------------------------
   IsXpmFormat check if the file header is of a pixmap                
  ----------------------------------------------------------------------*/
ThotBool IsXpmFormat (char *fn)
{
  FILE               *f;
  char                c;
  ThotBool            res;

  res = FALSE;
  f = TtaReadOpen (fn);
  if (f != NULL)
    {
      c = getc (f);
      if (c != EOF && c == '/')
	{
	  c = getc (f);
	  if (c != EOF && c == '*')
	    {
	      c = getc (f);
	      if (c != EOF && c == ' ')
		res = TRUE;
	    }
	}
    }
  TtaReadClose (f);
  return res;
}			
