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
  PointToPixel convert from points to pixels.
  ----------------------------------------------------------------------*/
int PointToPixel (int value)
{
  int         val;

  if (DOT_PER_INCHE == 0)
    return (value);
  else
    {
      val = (value * DOT_PER_INCHE + 36) / 72;
      if (val == 0 && value > 0)
	return (1);
      else
	return (val);
    }
}


/*----------------------------------------------------------------------
  PixelToPoint convert from pixels to points.
  ----------------------------------------------------------------------*/
int PixelToPoint (int value)
{
  int         val;

  if (DOT_PER_INCHE == 0)
    return (value);
  else
    {
      val = (value * 72 + DOT_PER_INCHE / 2) / DOT_PER_INCHE;
      if (val == 0 && value > 0)
	return (1);
      else
	return (val);
    }
}


