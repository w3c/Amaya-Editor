/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001
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


/*----------------------------------------------------------------------
 *      PointToPixel convert from points to pixels.
  ----------------------------------------------------------------------*/
int                 PointToPixel (int value)
{
   if (DOT_PER_INCHE == 0)
     return (value);
   else
     return (int)(((float)(value * DOT_PER_INCHE)) / 72.);
}


/*----------------------------------------------------------------------
 *      PixelToPoint convert from pixels to points.
  ----------------------------------------------------------------------*/
int                 PixelToPoint (int value)
{
   if (DOT_PER_INCHE == 0)
      return (value);
   else
      return (int)(((float)(value * 72)) / (float)DOT_PER_INCHE);
}

