/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Module dedicated to font handling.
 *
 * Author: I. Vatton (INRIA)
 *         R. Guetari (W3C/INRIA)
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#define THOT_EXPORT
#include "units_tv.h"
#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"

#ifdef _WINDOWS
#include "wininclude.h"

extern int ScreenDPI;
extern int PrinterDPI;
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
 *      PointToPixel convert from points to pixels.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 PointToPixel (int value)
#else  /* __STDC__ */
int                 PointToPixel (value)
int                 value;

#endif /* __STDC__ */
{
#  ifdef _WINDOWS
   if (DOT_PER_INCHE == 0)
      DOT_PER_INCHE = 72;
#  endif /* _WINDOWS */
   return ((value * DOT_PER_INCHE + 36) / 72);
}


/*----------------------------------------------------------------------
 *      PixelToPoint convert from pixels to points.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 PixelToPoint (int value)
#else  /* __STDC__ */
int                 PixelToPoint (value)
int                 value;

#endif /* __STDC__ */
{
#  ifdef _WINDOWS
   if (DOT_PER_INCHE == 0)
      DOT_PER_INCHE = 72;
#  endif /* _WINDOWS */
   return ((value * 72 + DOT_PER_INCHE / 2) / DOT_PER_INCHE);
}

