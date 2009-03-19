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

#ifdef _GL
	#ifdef _GTK
		#include <gtkgl/gtkglarea.h>
		#include <GL/gl.h>
		#include <GL/glu.h>
	#else /* _GTK */
		#ifdef _WINGUI
			#include <windows.h>
			#include <GL/gl.h>
			#include <GL/glu.h>
		#endif /* _WINGUI */
	#endif /* _GTK */
#endif /*  _GL */


#define EPSILON 1e-10

unsigned char *fill_linear_gradient_image (Gradient *gradient, 
					   int width, 
					   int height)
{
#ifdef _GL
  GradientStop *current_stop, *First, *Last;
  unsigned char *p0, *pixel;
  int     x, y;
  double  delta_r, delta_g, delta_b, delta_a;
  double  curr_r, curr_g, curr_b, curr_a;
  double  grad_width;
  int     size, int_grad_width;

  size = 4 * width * height * sizeof (unsigned char);
  pixel = (unsigned char *)malloc (size);
  memset (pixel, 0, size);

  First = gradient->firstStop;
  if (First->length > 0)
    /* the first stop is not zero. Create the part of the gradient before the
       first stop */
    {
      current_stop = (GradientStop *)TtaGetMemory (sizeof (GradientStop));
      gradient->firstStop = current_stop;
      current_stop->next = First;
      current_stop->el = NULL;
      current_stop->r = First->r;
      current_stop->g = First->g;
      current_stop->b = First->b;
      current_stop->a = First->a;
      current_stop->length = 0;
    }
  Last = First;
  while (Last->next)
     Last = Last->next;
  if (Last->length < 1)
    /* the last stop is less than 1. Create the part of the gradient after the
       last stop */
    {
      current_stop = (GradientStop *)TtaGetMemory (sizeof (GradientStop));
      Last->next = current_stop;
      current_stop->next = NULL;
      current_stop->el = NULL;
      current_stop->r = Last->r;
      current_stop->g = Last->g;
      current_stop->b = Last->b;
      current_stop->a = Last->a;
      current_stop->length = 1;
    }  

  current_stop = gradient->firstStop;
  p0 = pixel;
  while (current_stop->next)
    {
      grad_width = (current_stop->next->length - current_stop->length) * width;
      int_grad_width = (int) grad_width;
      if (current_stop->next->r - current_stop->r)
	delta_r = (current_stop->next->r - current_stop->r) / grad_width;
      else
	delta_r = 0;
      if (current_stop->next->g - current_stop->g)
	delta_g = (current_stop->next->g - current_stop->g) / grad_width;
      else
	delta_g = 0;      
      if (current_stop->next->b - current_stop->b)
	delta_b = (current_stop->next->b - current_stop->b) / grad_width;
      else
	delta_b = 0;      
      if (current_stop->next->a - current_stop->a)
	delta_a = (current_stop->next->a - current_stop->a) / grad_width;
      else 
	delta_a = 0;      
      x = 0;
      curr_r = current_stop->r;
      curr_g = current_stop->g;
      curr_b = current_stop->b;
      curr_a = current_stop->a;
      /* Create a unique gradient line 
       that will be copied height times*/
      while (x < int_grad_width) 
	{
	  *p0++ = (unsigned char) curr_r;
	  *p0++ = (unsigned char) curr_g;
	  *p0++ = (unsigned char) curr_b;
	  *p0++ = (unsigned char) curr_a;
	  curr_r += delta_r;
	  curr_g += delta_g;
	  curr_b += delta_b;
	  curr_a += delta_a;
	  x++;
	}
      current_stop = current_stop->next;
    }
  /* Fill all lines in the gradient */
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

