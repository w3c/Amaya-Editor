/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1998-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * GLgradient.c : handling of low level drawing routines, 
 *                      for Opengl 
 *
 * Author:  P. Cheyrou-lagreze (INRIA)
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

/*----------------------------------------------------------------------
  fill_linear_gradient_image
  width, height: size in pixels of the shape to be filled with the gradient
  ----------------------------------------------------------------------*/
unsigned char *fill_linear_gradient_image (Gradient *gradient,
					   int width, int height)
{
#ifdef _GL
  GradientStop  *currentStop, *lastStop;
  unsigned char *pixel, *pMax, *p0, *pi, *pf, *pc;
  int            x, y;
  double         delta_r, delta_g, delta_b, delta_a;
  double         curr_r, curr_g, curr_b, curr_a;
  double         grad_width, stop_width, ratio;
  int            length, size, len;

  if (!gradient || width == 0 || height == 0)
    return NULL;
  if (!gradient->firstStop)
    /* no stop means fill = none */
    return NULL;

  /* get memory for the bit map to be built */
  length = width * 4 * sizeof (unsigned char);  /* 4 bytes per pixel: rgba */
  size = length * height;
  pixel = (unsigned char *)malloc (size);
  memset (pixel, 0, size);

  /* check the gradient vector, i.e. the interval [x1, x2] */
  grad_width = gradient->x2 - gradient->x1;
  if (gradient->userSpace)
    grad_width = grad_width / width;
  if (grad_width == 0)
    {
      if (gradient->userSpace)
	grad_width = (width - gradient->x1) / width;
      else
	grad_width = 1 - gradient->x1;
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
    pi = pixel + (int)(gradient->x1) * 4 * sizeof (unsigned char);
  else
    pi = pixel + (int)(gradient->x1 * width) * 4 * sizeof (unsigned char);
  /* pf: position in the line of the end of the gradient vector (x2) */
  if (gradient->userSpace)
    pf = pixel + ((int)(gradient->x2) + 1) * 4 * sizeof (unsigned char);
  else
    pf = pixel + ((int)(gradient->x2 * width) + 1) * 4 * sizeof (unsigned char);
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
      for (x = 0; x < len; x++) 
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
  if (gradient->x1 > 0)
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
  if ((!gradient->userSpace && gradient->x2 < 1) ||
      (gradient->userSpace && gradient->x2 < width))
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
  for (y = 1; y < height; y++)
    {
      p0 += width;
      memcpy (p0, pixel, width);
    }

  return pixel; 
#else
  return NULL; 
#endif/*  _GL */
} 




