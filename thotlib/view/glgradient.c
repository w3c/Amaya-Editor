/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1998-2003
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
#ifdef _GL

#include <GL/gl.h>
#include <GL/glu.h>

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

typedef struct _RgbaDef {
  double          r, g, b, a;   	/* color def */
  float           length;               /*length til next color*/  
  struct _RgbaDef *next;  
} RgbaDef;


static unsigned char *fill_linear_gradient_image (RgbaDef *First, 
						  int width, 
						  int height)
{
  RgbaDef *current_gradient;
  unsigned char *p0, *pixel;
  int     x, y;
  double  delta_r, delta_g, delta_b, delta_a;
  double  curr_r, curr_g, curr_b, curr_a;
  double  grad_width;
  int     int_grad_width;

  int_grad_width = 4 * width * height * sizeof (unsigned char);
  
  pixel = malloc (int_grad_width);
  memset (pixel, 0, int_grad_width);
  current_gradient = First;
  x = 0;
  p0 = pixel;
  
  while (current_gradient->next)
    {

      /* (current_gradient->len - 1) ? */
      grad_width = current_gradient->length * width;
      int_grad_width = (int) grad_width;
      
      if (current_gradient->next->r - current_gradient->r)
	delta_r = (current_gradient->next->r - current_gradient->r) / grad_width;
      else
	delta_r = 0;

      if (current_gradient->next->g - current_gradient->g)
	delta_g = (current_gradient->next->g - current_gradient->g) / grad_width;
      else
	delta_g = 0;
      
      if (current_gradient->next->b - current_gradient->b)
	delta_b = (current_gradient->next->b - current_gradient->b) / grad_width;
      else
	delta_b = 0;
      
      if (current_gradient->next->a - current_gradient->a)
	delta_a = (current_gradient->next->a - current_gradient->a) / grad_width;
      else 
	delta_a = 0;
      
      
      /* Create a unique gradient line 
       that will be copied height times*/
      x = 0;
      curr_r = current_gradient->r;
      curr_g = current_gradient->g;
      curr_b = current_gradient->b;
      curr_a = current_gradient->a;
      while (x < int_grad_width) 
	{
	  *p0++ = curr_r * 255.0;
	  *p0++ = curr_g * 255.0;
	  *p0++ = curr_b * 255.0;
	  *p0++ = curr_a * 255.0;

	  curr_r += delta_r;
	  curr_g += delta_g;
	  curr_b += delta_b;
	  curr_a += delta_a;

	  x++;
	}
      current_gradient = current_gradient->next;
    }
  /* Fill all row's image */
  p0 = pixel;
  width = width *4;  
  for (y = 1; y < height; y++)
    {
      p0 += width;
      memcpy (p0, pixel, width);
    }

  return pixel;  
} 

unsigned char *test_gradien_linear ()
{
  RgbaDef *First;
  unsigned char *image;
  
  First = malloc (sizeof (RgbaDef));
  
  First->r = 1.00;
  First->g = 0.00;
  First->b = 0.00;
  First->a = 1.0;
  First->length = 0.3;

  First->next = malloc (sizeof (RgbaDef));
  
  First->next->r = 0.0;
  First->next->g = 1.0;
  First->next->b = 0.0;
  First->next->a = 1.0;
  First->next->length = 0.7;

  First->next->next = malloc (sizeof (RgbaDef));
  
  First->next->next->r = 0.0;
  First->next->next->g = 0.0;
  First->next->next->b = 1.0;
  First->next->next->a = 1.0;
  First->next->next->length = 0.0;

  First->next->next->next = NULL;
  
  image = fill_linear_gradient_image (First, 100, 100);
  
  return image;  
}
#endif /* _GL */
