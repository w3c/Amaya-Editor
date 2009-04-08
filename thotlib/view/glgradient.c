/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1998-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * GLgradient.c : handling of low level drawing routines, 
 *                for Opengl 
 *
 * Author:  P. Cheyrou-Lagreze, V. Quint (INRIA)
 *
 */

#ifdef _WX
  #include "wx/wx.h"
#endif /* _WX */

#include "thot_gui.h"
#include "ustring.h"
#include "math.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "appdialogue.h"
#include "picture.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "font_tv.h"
#include "frame_tv.h"
#include "units_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"
#include "thotcolor_tv.h"

#include "appli_f.h"
#include "buildboxes_f.h"
#include "displaybox_f.h"
#include "displayselect_f.h" 
#include "boxlocate_f.h"
#include "buildlines_f.h"
#include "context_f.h"
#include "font_f.h"
#include "inites_f.h"
#include "memory_f.h" 
#include "units_f.h"
#include "xwindowdisplay_f.h"
#include "frame_f.h"
#include "animbox_f.h"
#include "picture_f.h"

#define EPSILON 1e-10

#ifdef _GL

/*----------------------------------------------------------------------
  FillColorLastStop
  return a bitmap of size width * height filled with the color of the last 
  stop of the gradient.
  ----------------------------------------------------------------------*/
static unsigned char *FillColorLastStop (Gradient *gradient, int width,
					 int height)
{
  GradientStop  *currentStop;
  unsigned char *pixel, *p0;
  int            length, size, i, j;

      /* get memory for the bitmap to be built */
      length = width * 4 * sizeof (unsigned char); /* 4 bytes per pixel: rgba */
      size = length * height;
      pixel = (unsigned char *)malloc (size);
      memset (pixel, 0, size);
      /* get the last stop */
      currentStop = gradient->firstStop;
      while (currentStop->next)
	currentStop = currentStop->next;
      /* fill the map with its color and opacity */
      i = 0; j = 0; p0 = pixel;
      for (j = 0; j < height; j++)   /* line by line */
	for (i = 0; i < width; i++)  /* pixel by pixel */
	  {
	    *p0++ = (unsigned char) currentStop->r;
	    *p0++ = (unsigned char) currentStop->g;
	    *p0++ = (unsigned char) currentStop->b;
	    *p0++ = (unsigned char) currentStop->a;
	  }
      /* that's it! */
      return pixel;
}

/*----------------------------------------------------------------------
  fill_linear_gradient_image
  width, height: size in pixels of the shape to be filled with the gradient
  ----------------------------------------------------------------------*/
static unsigned char *fill_linear_gradient_image (Gradient *gradient,
                                          int x, int y, int width, int height)
{
  GradientStop  *currentStop, *lastStop;
  unsigned char *pixel, *pMax, *p0, *pi, *pf, *pc;
  int            i, j;
  double         delta_r, delta_g, delta_b, delta_a;
  double         curr_r, curr_g, curr_b, curr_a;
  double         grad_width, stop_width, ratio;
  int            length, size, len;

  if (!gradient || width == 0 || height == 0)
    /* nothing to do */
    return NULL;
  if (!gradient->firstStop)
    /* no stop means fill = none */
    return NULL;
  if (gradient->gradType != Linear)
    /* here, we handle only linear gradients */
    return NULL;


  if (gradient->gradX1 == gradient->gradX2 &&
      gradient->gradY1 == gradient->gradY2)
    /* the area to be painted will be painted as a single color using
       the color and opacity of the last gradient stop */
    return FillColorLastStop (gradient, width, height);

  /* Get memory for the bit map to be built */
  length = width * 4 * sizeof (unsigned char);  /* 4 bytes per pixel: rgba */
  size = length * height;
  pixel = (unsigned char *)malloc (size);
  memset (pixel, 0, size);

  /* check the gradient vector, i.e. the interval [x1, x2] */
  grad_width = gradient->gradX2 - gradient->gradX1;
  if (gradient->userSpace)
    grad_width = grad_width / width;
  if (grad_width == 0)
    {
      if (gradient->userSpace)
	grad_width = (width - gradient->gradX1) / width;
      else
	grad_width = 1 - gradient->gradX1;
    }
  if (grad_width < 0)
    grad_width = -grad_width;
  if (grad_width < 1)
    ratio = grad_width;
  else
    ratio = 1;

  /* Create a unique line of pixels that will be copied height times */

  /* pi: position in the line of the beginning of the gradient vector (x1) */
  if (gradient->userSpace)
    pi = pixel + (int)(gradient->gradX1 - x) * 4 * sizeof (unsigned char);
  else
    pi = pixel + (int)(gradient->gradX1 * width) * 4 * sizeof (unsigned char);
  /* pf: position in the line of the end of the gradient vector (x2) */
  if (gradient->userSpace)
    pf = pixel + ((int)(gradient->gradX2) - x + 1) * 4 * sizeof (unsigned char);
  else
    pf = pixel + ((int)(gradient->gradX2 * width) + 1) * 4 * sizeof (unsigned char);
  /* pMax: position of the last pixel of the line */
  pMax = pixel + length;
  /* p0: current position in the line of pixels */
  p0 = pi;

  /* first fill the line between positions x1 and x2 of the gradient vector */

  if (gradient->firstStop->offset > 0)
    /* the first stop is after x1. Create the part of the gradient between
       x1 and the first stop: same color as first stop */
    {
      /* color to be used */
      curr_r = gradient->firstStop->r;
      curr_g = gradient->firstStop->g;
      curr_b = gradient->firstStop->b;
      curr_a = gradient->firstStop->a;
      /* create pixels of that color between x1 and the first gradient stop */
      len = (int)(gradient->firstStop->offset * ratio * width);
      for (i = 0; i < len; i++) 
	{
	  *p0++ = (unsigned char) curr_r;
	  *p0++ = (unsigned char) curr_g;
	  *p0++ = (unsigned char) curr_b;
	  *p0++ = (unsigned char) curr_a;
	}
    }

  /* process all gradient stops */
  currentStop = gradient->firstStop;
  while (currentStop->next)
    {
      /* number of pixels until next stop */
      stop_width = (currentStop->next->offset - currentStop->offset) * ratio * width;
      len = (int) stop_width;
      /* compute the color difference between two successive pixels */
      if (currentStop->next->r - currentStop->r)
	delta_r = (currentStop->next->r - currentStop->r) / stop_width;
      else
	delta_r = 0;
      if (currentStop->next->g - currentStop->g )
	delta_g = (currentStop->next->g - currentStop->g) / stop_width;
      else
	delta_g = 0;      
      if (currentStop->next->b - currentStop->b)
	delta_b = (currentStop->next->b - currentStop->b) / stop_width;
      else
	delta_b = 0;      
      if (currentStop->next->a - currentStop->a)
	delta_a = (currentStop->next->a - currentStop->a) / stop_width;
      else 
	delta_a = 0;      
      curr_r = currentStop->r;
      curr_g = currentStop->g;
      curr_b = currentStop->b;
      curr_a = currentStop->a;
      /* Create in the line the pixels corresponding to the current stop */
      for (x = 0; x < len; x++) 
	{
	  *p0++ = (unsigned char) curr_r;
	  *p0++ = (unsigned char) curr_g;
	  *p0++ = (unsigned char) curr_b;
	  *p0++ = (unsigned char) curr_a;
	  curr_r += delta_r;
	  if (curr_r < 0.) curr_r = 0.;
	  curr_g += delta_g;
	  if (curr_g < 0.) curr_g = 0.;
	  curr_b += delta_b;
	  if (curr_b < 0.) curr_b = 0.;
	  curr_a += delta_a;
	  if (curr_a < 0.) curr_a = 0.;
	}
      currentStop = currentStop->next;
    }
  lastStop = currentStop;

  if (p0 < pf)
    /* the last stop is before x2. Create the part of the gradient between
       the last stop and x2: same color as last stop */
    {
      /* color of the last stop */
      curr_r = lastStop->r;
      curr_g = lastStop->g;
      curr_b = lastStop->b;
      curr_a = lastStop->a;
      /* create pixels between the last gradient stop and x2 */
      while (p0 < pf && p0 < pMax) 
	{
	  *p0++ = (unsigned char) curr_r;
	  *p0++ = (unsigned char) curr_g;
	  *p0++ = (unsigned char) curr_b;
	  *p0++ = (unsigned char) curr_a;
	}
    }

  /* the interval [x1, x2] is now filled in the line of pixels */

  /* if the interval [x1, x2] is smaller than the interval [0, 1], fill the
     parts outside [x1, x2] according to attribute spreadMethod */
  /* first, fill the part before x1 */
  if (gradient->gradX1 > 0)
    {
      if (gradient->spreadMethod == 1)
	/* spreadMethod = pad */
	/* fill the interval [0, x1] with the color of the first stop */
	{
	  /* get the color of the first stop */
	  curr_r = gradient->firstStop->r;
	  curr_g = gradient->firstStop->g;
	  curr_b = gradient->firstStop->b;
	  curr_a = gradient->firstStop->a;
	  /* fill the beginning of the line of pixels with this color */
	  p0 = pixel;
	  while (p0 < pi) 
	    {
	      *p0++ = (unsigned char) curr_r;
	      *p0++ = (unsigned char) curr_g;
	      *p0++ = (unsigned char) curr_b;
	      *p0++ = (unsigned char) curr_a;
	    }
	}
      else if (gradient->spreadMethod == 2)
	/* spreadMethod = reflect */
	{
	  /* start from position x1 and repeat the gradient vector down to
	     position 0 in alternating directions */
	  p0 = pi;  /*  x1  */
	  while (p0 >= pixel)
	    {
	      /* pc: position of the pixel to be copied from the gradient
		 vector */
	      pc = pi;
	      while (pc < pf && p0 >= pixel)
		{
		  /* copy a pixel (4 bytes: rgba) */
	          p0[0] = pc[0];
	          p0[1] = pc[1];
	          p0[2] = pc[2];
	          p0[3] = pc[3];
		  pc += 4;
                  p0 -= 4;
		}
	      /* change direction and do the next copy of the vector */
	      pc = pf - 4;
              p0 += 4;
	      while (pc > pi && p0 >= pixel)
		*p0-- = *pc--;
	    }
	}
      else if (gradient->spreadMethod == 3)
	/* spreadMethod = repeat */
	{
	  p0 = pi;
	  pc = pf;
	  while (p0 > pixel)
	    {
	      p0--; pc--;
	      *p0 = *pc;
	    }
	}
    }
  /* now, fill the part after x2, if there is one */
  if ((!gradient->userSpace && gradient->gradX2 < 1) ||
      (gradient->userSpace && gradient->gradX2 < width + x))
    {
      if (gradient->spreadMethod == 1)
	/* spreadMethod = pad */
	/* fill the interval [x2, 1] with the color of the last stop */
	{
	  /* get the color of the last stop */
	  curr_r = lastStop->r;
	  curr_g = lastStop->g;
	  curr_b = lastStop->b;
	  curr_a = lastStop->a;
	  /* fill the end of the line with this color */
	  p0 = pf;
	  while (p0 < pMax) 
	    {
	      *p0++ = (unsigned char) curr_r;
	      *p0++ = (unsigned char) curr_g;
	      *p0++ = (unsigned char) curr_b;
	      *p0++ = (unsigned char) curr_a;
	    }
	}
      else if (gradient->spreadMethod == 2)
	/* spreadMethod = reflect */
	{
	  /* start from position x2 and repeat the gradient vector up to
	     position 1 in alternating directions */
	  p0 = pf;   /*  x2  */
	  while (p0 < pMax)
	    {
	      /* pc: position of the pixel to be copied from the gradient
		 vector */
	      pc = pf - 4; /* end of the gradient vector */
	      while (pc >= pi && p0 < pMax)
		{
		  /* copy one pixel (4 bytes: rgba) */
	          p0[0] = pc[0];
	          p0[1] = pc[1];
	          p0[2] = pc[2];
	          p0[3] = pc[3];
		  pc -= 4;
                  p0 += 4;
		}
	      /* change direction and do the next copy of the vector */
	      pc = pi;
	      while (pc < pf && p0 < pMax)
		*p0++ = *pc++;
	    }
	}
      else if (gradient->spreadMethod == 3)
	/* spreadMethod = repeat */
	{
	  p0 = pf;
	  pc = pi;
	  while (p0 < pMax)
	    *p0++ = *pc++;
	}
    }

  /* Fill all lines in the gradient with a copy of the line we have just filled */
  p0 = pixel;
  width = width * 4;
  for (j = 1; j < height; j++)
    {
      p0 += width;
      memcpy (p0, pixel, width);
    }

  return pixel;
}

/*----------------------------------------------------------------------
  fill_radial_gradient_image
  width, height: size in pixels of the shape to be filled with the gradient
  ----------------------------------------------------------------------*/
static unsigned char *fill_radial_gradient_image (Gradient *gradient,
					  int x, int y, int width, int height)
{
  GradientStop  *currentStop, *lastStop;
  int            beamLength, bxPix, byPix, len, span, i, j,
                 length, size, cxPix, cyPix, dist, dx, dy;
  double         cx, cy, r, bx, by, xRatio, yRatio;
  double         delta_r, delta_g, delta_b, delta_a;
  double         curr_r, curr_g, curr_b, curr_a;
  double         stop_width;
  unsigned char  *beam, *pixel, *pMax, *pf, *p0, *beamPix, *pc;

  if (!gradient || width == 0 || height == 0)
    /* nothing to do */
    return NULL;
  if (!gradient->firstStop)
    /* no stop means fill = none */
    return NULL;
  if (gradient->gradType != Radial)
    /* here, we handle only radial gradients */
    return NULL;

  if (gradient->gradR == 0)
    /* zero causes the area to be painted as a single color using the color
       and opacity of the last gradient stop */
    return FillColorLastStop (gradient, width, height);

  if (gradient->userSpace)
    /* gradientUnits = userSpaceOnUse */
    {
      span = sqrt (width*width + height*height);
      cx = (gradient->gradCx - x) / width;
      cy = (gradient->gradCy - y) / height;
      r = gradient->gradR / span;
      xRatio = 1;
      yRatio = 1;
    }
  else
    /* gradientUnits = objectBoundingBox */
    /* it is as if the box was a square */
    {
      if (width > height)
	{
	  span = width;
	  xRatio = 1;
	  yRatio = width / height;
	}
      else
	{
	  span = height;
	  xRatio = height / width;
	  yRatio = 1;
	}
      cx = gradient->gradCx;
      cy = gradient->gradCy;
      r = gradient->gradR;
    }
  /* compute beamLength, the distance between the center (cx, cy) and the
     farthest corner of the rectangle */
  if (cx > .5)
    bx = cx;
  else
    bx = 1 - cx;
  if (cy > .5)
    by = cy;
  else
    by = 1 - cy;
  if (gradient->userSpace)
    /* gradientUnits = userSpaceOnUse */
    {
      bxPix = (int)(bx * width);
      byPix = (int)(by * height);
    }
  else
    /* gradientUnits = objectBoundingBox */
    if (width > height)
      {
	bxPix = (int)(bx * width);
	byPix = (int)(by * width);
      }
    else
      {
	bxPix = (int)(bx * height);
	byPix = (int)(by * height);
      }
  beamLength = 4 * ((int)sqrt (bxPix*bxPix + byPix*byPix) + 1);

  /* get memory for the beam */
  beam = (unsigned char *)malloc (beamLength);
  memset (beam, 0, beamLength);
  pMax = beam + beamLength;

  /* create pixels along the beam according to the gradient stops and the
     spreadMethod */
  p0 = beam;
  /* first fill the beam between the center and the first stop with the color
     of the first stop */
  if (gradient->firstStop->offset > 0)
    {
      /* uniform color to be used in this part of the beam */
      curr_r = gradient->firstStop->r;
      curr_g = gradient->firstStop->g;
      curr_b = gradient->firstStop->b;
      curr_a = gradient->firstStop->a;
      /* create pixels of that color between the center and the first gradient
	 stop */
      len = (int)(gradient->firstStop->offset * r * span);
      for (i = 0; i < len; i++) 
	{
	  *p0++ = (unsigned char) curr_r;
	  *p0++ = (unsigned char) curr_g;
	  *p0++ = (unsigned char) curr_b;
	  *p0++ = (unsigned char) curr_a;
	}
    }
  /* now, process all gradient stops */
  currentStop = gradient->firstStop;
  while (currentStop->next)
    {
      /* number of pixels until next stop */
      stop_width = (currentStop->next->offset - currentStop->offset) * r * span;
      len = (int) stop_width;
      /* compute the color difference between two successive pixels */
      if (currentStop->next->r - currentStop->r)
	delta_r = (currentStop->next->r - currentStop->r) / stop_width;
      else
	delta_r = 0;
      if (currentStop->next->g - currentStop->g )
	delta_g = (currentStop->next->g - currentStop->g) / stop_width;
      else
	delta_g = 0;      
      if (currentStop->next->b - currentStop->b)
	delta_b = (currentStop->next->b - currentStop->b) / stop_width;
      else
	delta_b = 0;      
      if (currentStop->next->a - currentStop->a)
	delta_a = (currentStop->next->a - currentStop->a) / stop_width;
      else 
	delta_a = 0;      
      /* create in the beam the pixels corresponding to the current stop */
      curr_r = currentStop->r;
      curr_g = currentStop->g;
      curr_b = currentStop->b;
      curr_a = currentStop->a;
      for (i = 0; i < len; i++) 
	{
	  *p0++ = (unsigned char) curr_r;
	  *p0++ = (unsigned char) curr_g;
	  *p0++ = (unsigned char) curr_b;
	  *p0++ = (unsigned char) curr_a;
	  curr_r += delta_r;
	  if (curr_r < 0.) curr_r = 0.;
	  curr_g += delta_g;
	  if (curr_g < 0.) curr_g = 0.;
	  curr_b += delta_b;
	  if (curr_b < 0.) curr_b = 0.;
	  curr_a += delta_a;
	  if (curr_a < 0.) curr_a = 0.;
	}
      currentStop = currentStop->next;
    }
  /* remember the last stop */
  lastStop = currentStop;
  if (lastStop->offset < 1)
    /* the last stop is before the end of the radius the radius. Create the
       part of the gradient between the last stop and the end of the radius:
       same color as last stop */
    {
      /* color of the last stop */
      curr_r = lastStop->r;
      curr_g = lastStop->g;
      curr_b = lastStop->b;
      curr_a = lastStop->a;
      /* create the pixels on the beam */
      pf = beam + (int)(r * span) * 4 * sizeof (unsigned char);
      while (p0 < pf && p0 < pMax) 
	{
	  *p0++ = (unsigned char) curr_r;
	  *p0++ = (unsigned char) curr_g;
	  *p0++ = (unsigned char) curr_b;
	  *p0++ = (unsigned char) curr_a;
	}
    }
  /* the radius is now filled in the beam */
  /* if the radius is smaller than the beam, fill the part after the radius
     according to attribute spreadMethod */
  if (r * span < beamLength/4)
    {
      if (gradient->spreadMethod == 1)
	/* spreadMethod = pad */
	/* fill the end of the beam with the color of the last stop */
	{
	  /* get the color of the last stop */
	  curr_r = lastStop->r;
	  curr_g = lastStop->g;
	  curr_b = lastStop->b;
	  curr_a = lastStop->a;
	  /* fill the end of the beam with this color */
	  while (p0 < pMax) 
	    {
	      *p0++ = (unsigned char) curr_r;
	      *p0++ = (unsigned char) curr_g;
	      *p0++ = (unsigned char) curr_b;
	      *p0++ = (unsigned char) curr_a;
	    }
	}
      else if (gradient->spreadMethod == 2)
	/* spreadMethod = reflect */
	{
	  /* start from end of radius and repeat the gradient vector up to
	     the end of the beam in alternating directions */
	  pf = p0;   /*  end of radius  */
	  while (p0 < pMax)
	    {
	      /* pc: position of the pixel to be copied from the radius */
	      pc = pf - 4; /* end of the radius */
	      while (pc >= beam && p0 < pMax)
		{
		  /* copy one pixel (4 bytes: rgba) */
	          p0[0] = pc[0];
	          p0[1] = pc[1];
	          p0[2] = pc[2];
	          p0[3] = pc[3];
		  pc -= 4;
                  p0 += 4;
		}
	      /* change direction and do the next copy of the vector */
	      pc = beam;
	      while (pc < pf && p0 < pMax)
		*p0++ = *pc++;
	    }
	}
      else if (gradient->spreadMethod == 3)
	/* spreadMethod = repeat */
	{
	  pc = beam;
	  while (p0 < pMax)
	    *p0++ = *pc++;
	}
    }
  /* we have a full beam, now */

  /* get memory for the bitmap to be built */
  length = width * 4 * sizeof (unsigned char);  /* 4 bytes per pixel: rgba */
  size = length * height;
  pixel = (unsigned char *)malloc (size);
  memset (pixel, 0, size);
  p0 = pixel;
  cxPix = (int)(cx * width);
  cyPix = (int)(cy * height);
  i = 0; j = 0;
  for (j = height; j > 0; j--)
    for (i = 0; i < width; i++)
      {
	/* distance of current pixel from the center */
	dx = (int)((i - cxPix) * xRatio);
	dy = (int)((j - cyPix) * yRatio);
	dist = (int)sqrt (dx*dx + dy*dy);
        beamPix = beam + dist * 4;
        *p0++ = *beamPix++;
        *p0++ = *beamPix++;
        *p0++ = *beamPix++;
        *p0++ = *beamPix;
      }
  free (beam);
  return (pixel);
}
#endif/*  _GL */

/*----------------------------------------------------------------------
  fill_gradient_image
  width, height: size in pixels of the shape to be filled with the gradient
  ----------------------------------------------------------------------*/
unsigned char *fill_gradient_image (Gradient *gradient, int x, int y, int width, int height)
{
#ifdef _GL
  if (!gradient || width == 0 || height == 0)
    return NULL;
  if (!gradient->firstStop)
    /* no stop means fill = none */
    return NULL;
  if (gradient->gradType == Linear)
    return fill_linear_gradient_image (gradient, x, y, width, height);
  else if (gradient->gradType == Radial)
    return fill_radial_gradient_image (gradient, x, y, width, height);
#endif/*  _GL */
  return NULL; 
} 




