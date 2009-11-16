/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1998-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * GLgradient.c : handling low level drawing routines for SVG gradients
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
#include "windowdisplay_f.h"
#include "frame_f.h"
#include "animbox_f.h"
#include "picture_f.h"

#define EPSILON 1e-10

#ifdef _GL

/*----------------------------------------------------------------------
  FillGradientLine
  Create a line of "length" pixels filled according to "gradient".
  gradStart: position on that line of the first pixel of the gradient vector
  (or center for a radial gradient)
  gradEnd: position on that line of the last pixel of the gradient vector
  (or radius for a radial gradient)
  ----------------------------------------------------------------------*/
static unsigned char *FillGradientLine (Gradient *gradient, int length,
					int gradStart, int gradEnd, int *delta)
{
  GradientStop  *currentStop, *lastStop;
  int            lengthByte, i, len, gradLength, extraLen;
  unsigned char *line, *p0, *pi, *pf, *pMax, *pc;
  double         curr_r, curr_g, curr_b, curr_a;
  double         delta_r, delta_g, delta_b, delta_a;
  double         stop_width;
  ThotBool       reverse;

  *delta = 0;
  extraLen = 0;
  reverse = gradEnd < gradStart;
  /* if the gradient vector is outside of the line, change the origin and/or
     the length of the line to include the vector in the line */
  if (reverse)
    {
      if (gradEnd < 0)
	*delta = -gradEnd * 4;
      if (gradStart > length)
	extraLen = (gradStart - length) * 4;
    }
  else
    {
      if (gradStart < 0)
	*delta = -gradStart * 4;
      if (gradEnd > length)
	extraLen = (gradEnd - length) * 4;
    }
  /* get memory for the line of pixels (4 bytes per pixel: red, green, blue
     and alpha channel */
  lengthByte = *delta + length * 4 * sizeof (unsigned char) + extraLen;
  line = (unsigned char *)malloc (lengthByte);
  /* pMax: position after the last byte of the last pixel of the line */
  pMax = line + lengthByte;

  /* create pixels along the line according to the gradient stops and the
     spreadMethod specified */

  /* pi: position in the line of the beginning of the gradient vector (x1
     for a linear gradient, center for a radial gradient) */
  pi = line + *delta + (gradStart * 4 * sizeof (unsigned char));
  /* pf: position in the line of the end of the gradient vector (x2 for
     a linear gradient, end of radius for a radial gradient) */
  pf = line + *delta + (gradEnd * 4 * sizeof (unsigned char));
  if (reverse)
    gradLength = gradStart - gradEnd;
  else
    gradLength = gradEnd - gradStart;

  /* first fill the line between position x1 (or center for a radial gradient)
     and the first stop of the gradient */
  p0 = pi;   /* p0: current position in the line of pixels */
  if (gradient->firstStop->offset > 0)
    /* the first stop is after x1 (or center). fill that part of the gradient
       with the same color and opacity as the first stop */
    {
      /* uniform color to be used */
      curr_r = gradient->firstStop->r;
      curr_g = gradient->firstStop->g;
      curr_b = gradient->firstStop->b;
      curr_a = gradient->firstStop->a;
      /* create pixels of that color between x1 (or center) and the first
	 gradient stop */
      len = (int)(gradient->firstStop->offset * gradLength);
      if (reverse)
	for (i = 0; i <= len; i++) 
	  {
	    p0[0] = (unsigned char) curr_r;
	    p0[1] = (unsigned char) curr_g;
	    p0[2] = (unsigned char) curr_b;
	    p0[3] = (unsigned char) curr_a;
	    p0 -= 4;
	  }
      else
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
      stop_width = (currentStop->next->offset - currentStop->offset) * gradLength;
      len = (int) stop_width;
      /* compute the color difference between two successive pixels */
      if (currentStop->next->r - currentStop->r)
	delta_r = (double)(currentStop->next->r - currentStop->r) / stop_width;
      else
	delta_r = 0;
      if (currentStop->next->g - currentStop->g )
	delta_g = (double)(currentStop->next->g - currentStop->g) / stop_width;
      else
	delta_g = 0;      
      if (currentStop->next->b - currentStop->b)
	delta_b = (double)(currentStop->next->b - currentStop->b) / stop_width;
      else
	delta_b = 0;      
      if (currentStop->next->a - currentStop->a)
	delta_a = (double)(currentStop->next->a - currentStop->a) / stop_width;
      else 
	delta_a = 0;      
      /* create in the line the pixels corresponding to the current stop */
      curr_r = currentStop->r;
      curr_g = currentStop->g;
      curr_b = currentStop->b;
      curr_a = currentStop->a;
      for (i = 0; i < len; i++) 
	{
	  if (reverse)
	    {
	      p0[0] = (unsigned char) curr_r;
	      p0[1] = (unsigned char) curr_g;
	      p0[2] = (unsigned char) curr_b;
	      p0[3] = (unsigned char) curr_a;
	      p0 -= 4;
	    }
	  else
	    {
	      *p0++ = (unsigned char) curr_r;
	      *p0++ = (unsigned char) curr_g;
	      *p0++ = (unsigned char) curr_b;
	      *p0++ = (unsigned char) curr_a;
	    }
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

  if ((reverse && p0 > pf) || (!reverse && p0 < pf))
    /* the last stop is before x2 (or the end of the radius). Fill the part
       of the gradient between the last stop and x2 (or the end of the radius)
       with the same color as last stop */
    {
      /* color of the last stop */
      curr_r = lastStop->r;
      curr_g = lastStop->g;
      curr_b = lastStop->b;
      curr_a = lastStop->a;
      /* create pixels in the line */
      if (reverse)
	{
	  while (p0 > pf && p0 >= line)
	    {
	      p0[0] = (unsigned char) curr_r;
	      p0[1] = (unsigned char) curr_g;
	      p0[2] = (unsigned char) curr_b;
	      p0[3] = (unsigned char) curr_a;
	      p0 -= 4;
	    }
	}
      else
	{
	  while (p0 < pf && p0 < pMax) 
	    {
	      *p0++ = (unsigned char) curr_r;
	      *p0++ = (unsigned char) curr_g;
	      *p0++ = (unsigned char) curr_b;
	      *p0++ = (unsigned char) curr_a;
	    }
	}
    }
  /* the interval [x1, x2] (or the full radius) is now filled in the line
     of pixels */
  /* fill the part before x1 (does not apply to radial gradients) */
  if ((reverse && pi < pMax) || (!reverse && line < pi))
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
	  if (reverse)
	    /* fill the end of the line with this color */
	    {
	      p0 = pi;
	      while (p0 < pMax)
		{
		  *p0++ = (unsigned char) curr_r;
		  *p0++ = (unsigned char) curr_g;
		  *p0++ = (unsigned char) curr_b;
		  *p0++ = (unsigned char) curr_a;
		}
	    }
	  else
	    /* fill the beginning of the line of pixels with this color */
	    {
	      p0 = line + *delta;
	      while (p0 < pi) 
		{
		  *p0++ = (unsigned char) curr_r;
		  *p0++ = (unsigned char) curr_g;
		  *p0++ = (unsigned char) curr_b;
		  *p0++ = (unsigned char) curr_a;
		}
	    }
	}
      else if (gradient->spreadMethod == 2)
	/* spreadMethod = reflect */
	{
	  if (!reverse)
	    {
	      if (pf-pi > 4)
		{
		  /* start from position x1 and repeat the gradient vector down
		     to position 0 in alternating directions */
		  p0 = pi;  /*  x1 */
		  while (p0 >= line + *delta)
		    {
		      /* pc: position of the pixel to be copied from the
			 gradient vector */
		      pc = pi;
		      while (pc < pf - 4 && p0 >= line + *delta)
			{
			  p0 -= 4;
			  /* copy a pixel (4 bytes: red, green, blue, alpha) */
			  p0[0] = pc[0];
			  p0[1] = pc[1];
			  p0[2] = pc[2];
			  p0[3] = pc[3];
			  pc += 4;
			}
		      /* change direction and do the next copy of the vector */
		      pc = pf - 4;
		      while (pc > pi && p0 >= line + *delta)
			*p0-- = *pc--;
		    }
		}
	    }
	  else
	    /* reverse direction */
	    {
	      /* start from position x1 and repeat the gradient vector up to
		 the end of the line in alternating directions */
	      p0 = pi;  /*  x1  */
	      while (p0 < pMax)
		{
		  /* pc: position of the pixel to be copied from the gradient
		     vector */
		  pc = pi;
		  while (pc > pf && p0 < pMax)
		    {
		      /* copy a pixel (4 bytes: red, green, blue, alpha) */
		      p0[0] = pc[0];
		      p0[1] = pc[1];
		      p0[2] = pc[2];
		      p0[3] = pc[3];
		      pc -= 4;
		      p0 += 4;
		    }
		  /* change direction and do the next copy of the vector */
		  pc = pf + 4;
		  while (pc < pi && p0 < pMax)
		    *p0++ = *pc++;
		}
	    }
	}
      else if (gradient->spreadMethod == 3)
	/* spreadMethod = repeat */
	{
	  p0 = pi;
	  pc = pf;
	  if (reverse)
	    {
	      pi += 4;
	      while (p0 < pMax)
		{
		  p0++; pc++;
		  *p0 = *pc;
		}
	    }
	  else
	    {
	      while (p0 > line + *delta)
		{
		  p0--; pc--;
		  *p0 = *pc;
		}
	    }
	}
    }
  /* now, fill the part after x2 (or after the end of the radius) according to
     attribute spreadMethod */
  if (gradLength < length)
    {
      if (gradient->spreadMethod == 1)
	/* spreadMethod = pad */
	/* fill the rest of the line with the color of the last stop */
	{
	  /* get the color of the last stop */
	  curr_r = lastStop->r;
	  curr_g = lastStop->g;
	  curr_b = lastStop->b;
	  curr_a = lastStop->a;
	  /* fill the end of the line with this color */
	  if (reverse)
	    {
	      p0 = line + *delta;
	      while (p0 <= pf)
		{
		  *p0++ = (unsigned char) curr_r;
		  *p0++ = (unsigned char) curr_g;
		  *p0++ = (unsigned char) curr_b;
		  *p0++ = (unsigned char) curr_a;
		}
	    }
	  else
	    {
	      p0 = pf;  /* x2 or end of radius */
	      while (p0 < pMax) 
		{
		  *p0++ = (unsigned char) curr_r;
		  *p0++ = (unsigned char) curr_g;
		  *p0++ = (unsigned char) curr_b;
		  *p0++ = (unsigned char) curr_a;
		}
	    }
	}
      else if (gradient->spreadMethod == 2)
	/* spreadMethod = reflect */
	{
	  if (reverse)
	    {
	      /* start from position x2 and repeat the gradient vector up to
		 the beginning of the line in alternating directions */
	      p0 = pf;  /* x2 */
	      while (p0 >= line + *delta)
		{
		  /* pc: position of the pixel to be copied from the gradient
		     vector */
		  pc = pf + 4; /* end of the gradient vector */
		  while (pc <= pi && p0 >= line + *delta)
		    {
		      /* copy one pixel (4 bytes: red, green, blue, alpha) */
		      p0[0] = pc[0];
		      p0[1] = pc[1];
		      p0[2] = pc[2];
		      p0[3] = pc[3];
		      pc += 4;
		      p0 -= 4;
		    }
		  /* change direction and do the next copy of the vector or
		     radius*/
		  pc = pi;
		  while (pc > pf && p0 >= line + *delta)
		    *p0-- = *pc--;
		}
	    }
	  else
	    {
	      /* start from position x2 (or end of radius) and repeat the
		 gradient vector (or the radius) up to the end of the line
		 in alternating directions */
	      p0 = pf;  /* x2 or end of radius */
	      while (p0 < pMax)
		{
		  /* pc: position of the pixel to be copied from the gradient
		     vector (or radius) */
		  pc = pf - 4; /* end of the gradient vector or radius */
		  while (pc >= pi && p0 < pMax)
		    {
		      /* copy one pixel (4 bytes: red, green, blue, alpha) */
		      p0[0] = pc[0];
		      p0[1] = pc[1];
		      p0[2] = pc[2];
		      p0[3] = pc[3];
		      pc -= 4;
		      p0 += 4;
		    }
		  /* change direction and do the next copy of the vector or
		     radius*/
		  pc = pi;
		  while (pc < pf && p0 < pMax)
		    *p0++ = *pc++;
		}
	    }
	}
      else if (gradient->spreadMethod == 3)
	/* spreadMethod = repeat */
	{
	  p0 = pf;  /* x2 or end of radius */
	  pc = pi;
	  if (reverse)
	    {
	      while (p0 >= line + *delta)
		*p0-- = *pc--;	      
	    }
	  else
	    {
	      while (p0 < pMax)
		*p0++ = *pc++;
	    }
	}
    }
  return (line);
}

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
  FillLinearGradientImage
  width, height: size in pixels of the shape to be filled with the gradient
  ----------------------------------------------------------------------*/
static unsigned char *FillLinearGradientImage (Gradient *gradient,
                                          int x, int y, int width, int height)
{
  unsigned char *line, *pixel, *p0, *pc, *pLine;
  int            i, j, size, len, delta;
  int            gradStart, gradEnd;
  double         x1, y1, x2, y2, xRatio, yRatio, Px, Py, Ox, Oy, dx, dy,
                 a, b, c, a2, b2, ab, ac, bc, a2plusb2;
 
  if (gradient->gradType != Linear)
    /* here, we handle only linear gradients */
    return NULL;

  if (gradient->gradX1 == gradient->gradX2 &&
      gradient->gradY1 == gradient->gradY2)
    /* the area to be painted will be painted as a single color using
       the color and opacity of the last gradient stop */
    return FillColorLastStop (gradient, width, height);

  if (gradient->gradY1 == gradient->gradY2)
    /* horizontal gradient */
    {
      if (gradient->userSpace)
	{
	  gradStart = (int)(gradient->gradX1 - x);
	  gradEnd = (int)(gradient->gradX2 - x);
	}
      else
	{
	  /* coordinates are relative to the box width */
	  gradStart = (int)(gradient->gradX1 * width);
	  gradEnd = (int)(gradient->gradX2 * width);
	}
      /* fill a line of pixels according to the gradient */
      line = FillGradientLine (gradient, width, gradStart, gradEnd, &delta);
      /* get memory for the gradient bit map to be built */
      /* fill all lines in the gradient with a copy of the line we have just
	 filled */
      len = width * 4 * sizeof (unsigned char);   /* 4 bytes per pixel: rgba */
      size = height * len;
      pixel = (unsigned char *)malloc (size);
      p0 = pixel;
      for (j = height; j >0 ; j--)
	{
	  memcpy (p0, line + delta, len);
	  p0 += len;
	}
      free(line);
      return pixel;
    }

  if (gradient->gradX1 == gradient->gradX2)
    /* vertical gradient */
    {
      if (gradient->userSpace)
	{
	  gradStart = (int)(gradient->gradY1 - y);
	  gradEnd = (int)(gradient->gradY2 - y);
	}
      else
	{
	  /* coordinates are relative to the box height */
	  gradStart = (int)(gradient->gradY1 * height);
	  gradEnd = (int)(gradient->gradY2 * height);
	}
      /* fill a line of pixels according to the gradient */
      line = FillGradientLine (gradient, height, gradStart, gradEnd, &delta);
      /* get memory for the gradient bit map to be built */
      size = height * width * 4 * sizeof (unsigned char);
             /* 4 bytes per pixel: rgba */
      pixel = (unsigned char *)malloc (size);
      /* fill all columns in the gradient with a copy of the line we have just
	 filled */
      p0 = pixel;
      pc = line + delta + (height * 4 * sizeof (unsigned char));
      for (j = 0; j < height ; j++)
	{
	  pc -= 4;
	  for (i = 0; i < width; i++)
	    {
	      *p0++ = pc[0];
	      *p0++ = pc[1];
	      *p0++ = pc[2];
	      *p0++ = pc[3];
	    }
	}
      free(line);
      return pixel;
    }

  /* the gradient vector is neither horizontal nor vertical */
  /* the vector is on a line defined by:
     ax + by + c = 0
     with a = y2 - y1, b = x1 - x2, c = - b * y1 - a * x1
     (x1, y1) and (x2, y2) are the coordinates of the gradient vector ends.
     The gradient is first built on that line, then to compute the color of all
     pixels in the rectangle area to be filled, each pixel (coordinates i,j)
     is projected orthogonally on that line. Its color is the same as the
     projected point (coordinates Px,Py) :
     Px =  (b*b*i - a*b*j - a*c) / (a*a + b*b)
     Py = (-a*b*i + a*a*j - b*c) / (a*a + b*b)
  */
  if (gradient->userSpace)
    /* gradientUnits = userSpaceOnUse */
    {
      a = (double)(gradient->gradY2 - gradient->gradY1);
      b = (double)(gradient->gradX1 - gradient->gradX2);
      xRatio = 1;
      yRatio = 1;
      x1 = gradient->gradX1;
      x2 = gradient->gradX2;
      y1 = gradient->gradY1;
      y2 = gradient->gradY2;
      /* length of the diagonal */
      len = (int)sqrt ((float)(width * width + height * height)) + 1;
    }
  else
    /* gradientUnits = objectBoundingBox */
    /* it is as if the box was a square */
    {
      x = 0; y = 0;
      if (width > height)
	{
	  x1 = gradient->gradX1 * width;
	  x2 = gradient->gradX2 * width;
	  y1 = gradient->gradY1 * width;
	  y2 = gradient->gradY2 * width;
	  xRatio = 1;
	  yRatio = (double)width / (double)height;
	  a = (gradient->gradY2 - gradient->gradY1) * width;
	  b = (gradient->gradX1 - gradient->gradX2) * width;
          len = (int)sqrt ((float) (width * width * 2)) + 1;/* length of the diagonal */
	}
      else
	{
	  x1 = gradient->gradX1 * height;
	  x2 = gradient->gradX2 * height;
	  y1 = gradient->gradY1 * height;
	  y2 = gradient->gradY2 * height;
	  xRatio = (double)height / (double)width;
	  yRatio = 1;
	  a = (gradient->gradY2 - gradient->gradY1) * height;
	  b = (gradient->gradX1 - gradient->gradX2) * height;
	  len = (int)sqrt ((float)(height * height * 2)) + 1;/* length of the diagonal */
	}
    }
  c = (-b * y1) - (a * x1);
  b2 = b * b;
  a2 = a * a;
  ab = a * b;
  ac = a * c;
  bc = b * c;
  a2plusb2 = a2 + b2;
  /* origin of vector line */
  if (x1 < x2)
    {
      if (y1 < y2)
	{
	  /* origin is the orthogonal projection of point (0, 0) on
	     the line of the gradient vector */
	  Ox = -ac / a2plusb2;
	  Oy = -bc / a2plusb2;
	}
      else
	{
	  /* origin is the orthogonal projection of point (0, height) on
	     the line of the gradient vector */
	  Ox = (-ab * height * yRatio - ac) / a2plusb2;
	  Oy =  (a2 * height * yRatio - bc) / a2plusb2;
	}
    }
  else
    {
      if (y1 < y2)
	{
	  /* origin is the orthogonal projection of point (width, 0) on
	     the line of the gradient vector */
	  Ox =  (b2 * width * xRatio - ac) / a2plusb2;
	  Oy = (-ab * width * xRatio - bc) / a2plusb2;
	}
      else
	{
	  /* origin is the orthogonal projection of point (width, height) on
	     the line of the gradient vector */
	  Ox =  (b2 * width * xRatio - ab * height * yRatio - ac) / a2plusb2;
	  Oy = (-ab * width * xRatio + a2 * height * yRatio - bc) / a2plusb2;
	}
    }
  dx = x1 - Ox - x;
  dy = y1 - Oy - y;
  gradStart = (int)sqrt(dx*dx + dy*dy);
  dx = x2 - Ox - x;
  dy = y2 - Oy - y;
  gradEnd = (int)sqrt(dx*dx + dy*dy);
  /* fill a line of pixels according to the gradient */
  line = FillGradientLine (gradient, len, gradStart, gradEnd, &delta);
  /* get memory for the gradient bit map to be built */
  size = height * width * 4 * sizeof (unsigned char);
         /* 4 bytes per pixel: rgba */
  pixel = (unsigned char *)malloc (size);
  /* fill the bit map based on the line */
  p0 = pixel;
  pLine = line + delta;
  for (j = height - 1; j >= 0; j--)
    for (i = 0; i < width; i++)
      {
	/* projection of current pixel on the diagonal */
	Px =  (b2 * i * xRatio - ab * j * yRatio - ac) / a2plusb2;
        Py = (-ab * i * xRatio + a2 * j * yRatio - bc) / a2plusb2;
	dx = Px - Ox;
        dy = Py - Oy;
	/* position of the projected point on the vector line */
	len = (int)sqrt(dx*dx + dy*dy) * 4;
	/* copy the color and opacity of the projected point */
	*p0++ = pLine[len];
	*p0++ = pLine[len + 1];
	*p0++ = pLine[len + 2];
	*p0++ = pLine[len + 3];
      }
  free(line);
  return pixel;
}

/*----------------------------------------------------------------------
  FillRadialGradientImage
  width, height: size in pixels of the shape to be filled with the gradient
  ----------------------------------------------------------------------*/
static unsigned char *FillRadialGradientImage (Gradient *gradient,
					  int x, int y, int width, int height)
{
  unsigned char  *line, *pixel, *p0, *beamPix;
  int            i, j, length, size, bxPix, byPix, cxPix, cyPix, dist, dx, dy;
  int            gradStart, gradEnd, delta;
  double         cx, cy, gradLength, bx, by, xRatio, yRatio;

  if (gradient->gradType != Radial)
    /* here, we handle only radial gradients */
    return NULL;

  if (gradient->gradR == 0)
    /* radius = zero causes the area to be painted as a single color using the
       color and opacity of the last gradient stop */
    return FillColorLastStop (gradient, width, height);

  /* the focal point is ignored, as if it would always be the same ascoincide
     with the center of the largest circle */

  if (gradient->userSpace)
    /* gradientUnits = userSpaceOnUse */
    {
      cx = (gradient->gradCx - (double)x) / (double)width;
      cy = (gradient->gradCy - (double)y) / (double)height;
      gradLength = gradient->gradR;
      xRatio = 1;
      yRatio = 1;
    }
  else
    /* gradientUnits = objectBoundingBox */
    /* it is as if the box was a square */
    {
      cx = gradient->gradCx;
      cy = gradient->gradCy;
      if (width > height)
	{
	  gradLength = gradient->gradR * width;
	  xRatio = 1;
	  yRatio = (double)width / (double)height;
	}
      else
	{
	  gradLength = gradient->gradR * height;
	  xRatio = (double)height / (double)width;
	  yRatio = 1;
	}
    }
  /* compute length, the distance between the center (cx, cy) and the
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
  length = ((int)sqrt ((float)(bxPix*bxPix + byPix*byPix)) + 1);
  gradStart = 0;
  gradEnd = (int)gradLength;

  /* fill a line of pixels according to the gradient */
  line = FillGradientLine (gradient, length, gradStart, gradEnd, &delta);

  /* we have a full line, now */
  /* get memory for the bitmap to be built */
  size = height * width * 4 * sizeof (unsigned char);
         /* 4 bytes per pixel: rgba */
  pixel = (unsigned char *)malloc (size);
  cxPix = (int)(cx * width);
  cyPix = (int)(cy * height);
  p0 = pixel;
  for (j = height; j > 0; j--)
    for (i = 0; i < width; i++)
      {
	/* distance of current pixel from the center */
	dx = (int)((i - cxPix) * xRatio);
	dy = (int)((j - cyPix) * yRatio);
	dist = (int)sqrt ((float)(dx*dx + dy*dy));
        beamPix = line + delta + dist * 4;
        *p0++ = *beamPix++;
        *p0++ = *beamPix++;
        *p0++ = *beamPix++;
        *p0++ = *beamPix;
      }
  free (line);
  return (pixel);
}
#endif/*  _GL */

/*----------------------------------------------------------------------
  FillGradientImage
  width, height: size in pixels of the shape to be filled with the gradient
  ----------------------------------------------------------------------*/
unsigned char *FillGradientImage (Gradient *gradient, int x, int y,
				  int width, int height)
{
#ifdef _GL
  if (!gradient || width == 0 || height == 0)
    /* nothing to do */
    return NULL;
  if (!gradient->firstStop)
    /* no stop means fill = none */
    return NULL;
  if (gradient->gradType == Linear)
    return FillLinearGradientImage (gradient, x, y, width, height);
  else if (gradient->gradType == Radial)
    return FillRadialGradientImage (gradient, x, y, width, height);
#endif/*  _GL */
  return NULL; 
} 
