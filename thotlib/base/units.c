/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Module dedicated to font handling.
 *
 * Author: I. Vatton (INRIA)
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
#ifdef __STDC__
int                 PointToPixel (int value)
#else  /* __STDC__ */
int                 PointToPixel (value)
int                 value;

#endif /* __STDC__ */
{
   return ((value * DOT_PER_INCHE) / DOT_PER_INCHE);
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
   return ((value * DOT_PER_INCHE + DOT_PER_INCHE / 2) / DOT_PER_INCHE);
}


