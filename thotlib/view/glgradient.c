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
  FillGradientLine
  Create a line of "length" pixels filled according to "gradient".
  gradStart: position on that line of the first pixel of the gradient vector
  (or center for a radial gradient)
  gradEnd: position on that line of the last pixel of the gradient vector
  (or radius for a radial gradient)
  ----------------------------------------------------------------------*/
static unsigned char *FillGradientLine (Gradient *gradient, int length,
					int gradStart, int gradEnd)
{
  GradientStop  *currentStop, *lastStop;
  int            lengthByte, i, len, gradLength;
  unsigned char *line, *p0, *pi, *pf, *pMax, *pc;
  double         curr_r, curr_g, curr_b, curr_a;
  double         delta_r, delta_g, delta_b, delta_a;
  double         stop_width;
  ThotBool       reverse;

  reverse = gradEnd < gradStart;
  /* get memory for the line of pixels */
  lengthByte = length * 4 * sizeof (unsigned char); /* 4 bytes per pixel:
        red, green, blue and alpha channel */
  line = (unsigned char *)malloc (lengthByte);
  memset (line, 0, lengthByte);
  /* pMax: position after the last byte of the last pixel of the line */
  pMax = line + lengthByte;

  /* create pixels along the line according to the gradient stops and the
     spreadMethod specified */

  /* pi: position in the line of the beginning of the gradient vector (x1
     for a linear gradient, center for a radial gradient) */
  pi = line + (gradStart * 4 * sizeof (unsigned char));
  /* pf: position in the line of the end of the gradient vector (x2 for
     a linear gradient, end of radius for a radial gradient) */
  pf = line + (gradEnd * 4 * sizeof (unsigned char));
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
	for (i = 0; i < len; i++) 
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
	      p0 = line;
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
	      /* start from position x1 and repeat the gradient vector down to
		 position 0 in alternating directions */
	      p0 = pi;  /*  x1  */
	      while (p0 >= line)
		{
		  /* pc: position of the pixel to be copied from the gradient
		     vector */
		  pc = pi;
		  while (pc < pf && p0 >= line)
		    {
		      /* copy a pixel (4 bytes: red, green, blue, alpha) */
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
		  while (pc > pi && p0 >= line)
		    *p0-- = *pc--;
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
		  pc = pf;
		  p0 -= 4;
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
	      while (p0 < pMax)
		{
		  p0++; pc++;
		  *p0 = *pc;
		}
	    }
	  else
	    {
	      while (p0 > line)
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
	      p0 = line;
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
	      while (p0 >= line)
		{
		  /* pc: position of the pixel to be copied from the gradient
		     vector */
		  pc = pf + 4; /* end of the gradient vector */
		  while (pc <= pi && p0 >= line)
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
		  while (pc > pf && p0 >= line)
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
	      while (p0 >= line)
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
  FillLinearGradientImage
  width, height: size in pixels of the shape to be filled with the gradient
  ----------------------------------------------------------------------*/
static unsigned char *FillLinearGradientImage (Gradient *gradient,
                                          int x, int y, int width, int height)
{
  unsigned char *line, *pixel, *p0, *pc;
  int            i, j, size, len, bWidth, bHeight;
  int            gradStart, gradEnd;
  double         xRatio, yRatio, Px, Py, den;
 
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
	  gradStart = (int)(gradient->gradX1 * width);
	  gradEnd = (int)(gradient->gradX2 * width);
	}
      /* fill a line of pixels according to the gradient */
      line = FillGradientLine (gradient, width, gradStart, gradEnd);
      /* get memory for the gradient bit map to be built */
      /* fill all lines in the gradient with a copy of the line we have just
	 filled */
      len = width * 4 * sizeof (unsigned char);   /* 4 bytes per pixel: rgba */
      size = height * len;
      pixel = (unsigned char *)malloc (size);
      memset (pixel, 0, size);
      p0 = pixel;
      for (j = height; j >0 ; j--)
	{
	  memcpy (p0, line, len);
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
	  gradStart = (int)(gradient->gradY1 * height);
	  gradEnd = (int)(gradient->gradY2 * height);
	}
      /* fill a line of pixels according to the gradient */
      line = FillGradientLine (gradient, height, gradStart, gradEnd);
      /* get memory for the gradient bit map to be built */
      size = height * width * 4 * sizeof (unsigned char);
             /* 4 bytes per pixel: rgba */
      pixel = (unsigned char *)malloc (size);
      memset (pixel, 0, size);
      /* fill all columns in the gradient with a copy of the line we have just
	 filled */
      p0 = pixel;
      pc = line + (height * 4 * sizeof (unsigned char));
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
  /* perform a rough approximation : as if the vector was the diagonal of the
     rectangle area to be filled */ /* @@@@@@@ */
  if (gradient->userSpace)
    /* gradientUnits = userSpaceOnUse */
    {
      xRatio = 1;
      yRatio = 1;
      bWidth = width;
      bHeight = height;
      den = width*width + height*height;
    }
  else
    /* gradientUnits = objectBoundingBox */
    /* it is as if the box was a square */
    {
      if (width > height)
	{
	  xRatio = 1;
	  yRatio = (double)width / (double)height;
	  bWidth = width;
	  bHeight = width;
	  den = width*width*2;
	}
      else
	{
	  xRatio = (double)height / (double)width;
	  yRatio = 1;
	  bWidth = height;
	  bHeight = height;
	  den = height*height*2;
	}
    }
  len = (int)sqrt (den) + 1;/* length of the diagonal */
  /* fill a line of pixels according to the gradient */
  line = FillGradientLine (gradient, len, 0, len);
  /* get memory for the gradient bit map to be built */
  size = height * width * 4 * sizeof (unsigned char);
         /* 4 bytes per pixel: rgba */
  pixel = (unsigned char *)malloc (size);
  /* fill the bit map based on the line */
  p0 = pixel;
  for (j = height - 1; j >= 0; j--)
    for (i = 0; i < width; i++)
      {
	/* projection of current pixel on the diagonal */
	Px = (bWidth*bWidth * i * xRatio + bWidth*bHeight * j * yRatio) / den;
        Py = (bWidth*bHeight * i * xRatio + bHeight*bHeight * j * yRatio) / den;
	len = (int)sqrt(Px*Px + Py*Py) * 4;
	*p0++ = line[len];
	*p0++ = line[len + 1];
	*p0++ = line[len + 2];
	*p0++ = line[len + 3];
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
  int            gradStart, gradEnd;
  double         cx, cy, gradLength, bx, by, xRatio, yRatio;

  if (gradient->gradType != Radial)
    /* here, we handle only radial gradients */
    return NULL;

  if (gradient->gradR == 0)
    /* radius = zero causes the area to be painted as a single color using the
       color and opacity of the last gradient stop */
    return FillColorLastStop (gradient, width, height);

  if (gradient->userSpace)
    /* gradientUnits = userSpaceOnUse */
    {
      cx = (gradient->gradCx - x) / width;
      cy = (gradient->gradCy - y) / height;
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
	  yRatio = width / height;
	}
      else
	{
	  gradLength = gradient->gradR * height;
	  xRatio = height / width;
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
  length = ((int)sqrt (bxPix*bxPix + byPix*byPix) + 1);
  gradStart = 0;
  gradEnd = (int)gradLength;

  /* fill a line of pixels according to the gradient */
  line = FillGradientLine (gradient, length, gradStart, gradEnd);

  /* we have a full line, now */
  /* get memory for the bitmap to be built */
  size = height * width * 4 * sizeof (unsigned char);
         /* 4 bytes per pixel: rgba */
  pixel = (unsigned char *)malloc (size);
  memset (pixel, 0, size);
  cxPix = (int)(cx * width);
  cyPix = (int)(cy * height);
  p0 = pixel;
  for (j = height; j > 0; j--)
    for (i = 0; i < width; i++)
      {
	/* distance of current pixel from the center */
	dx = (int)((i - cxPix) * xRatio);
	dy = (int)((j - cyPix) * yRatio);
	dist = (int)sqrt (dx*dx + dy*dy);
        beamPix = line + dist * 4;
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
