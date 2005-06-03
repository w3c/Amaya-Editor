/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * glevent.c : handling of Time and Event routines, 
 *               with Opengl library both for
 *                   MS-Windows (incomplete).
 *                   Unix
 *
 * Author: P. Cheyrou-lagreze (INRIA)
 *
 */
#ifdef _GL

#ifdef _WINGUI
#include <windows.h>
#else /* _WINGUI*/
#include <gtkgl/gtkglarea.h>
#endif /* _WINGUI*/

#if defined (_MACOS) && defined (_WX)
#include <gl.h>
#else /* _MACOS */
#include <GL/gl.h>
#endif /* _MACOS */

#include "ustring.h"
#include "math.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "font_tv.h"
#include "frame_tv.h"
#include "units_tv.h"
#include "edit_tv.h"
#include "thotcolor_tv.h"


#include "boxlocate_f.h"
#include "buildlines_f.h"
#include "context_f.h"
#include "font_f.h"
#include "inites_f.h"
#include "memory_f.h"
#include "units_f.h"
#include "xwindowdisplay_f.h"
#include "tesse_f.h"

#include "glwindowdisplay.h"

/*
One Timer To Rule Them All 
(ie : all animation, frame, and windows are using the same timer, 
using start value substract to get their own time)
*/
static int      AnimTimer = 0; 

/* Animation Smoothness*/
#define FPS 25 /*Frame Per Second*/
#define INTERVAL 0.02 /*1/FPS*/ /* should be 1/25 ... 1/50 */

#define FRAME_TIME 5 /*(INTERVAL*1000) */
/* milliseconds */

/*----------------------------------------------------------------------
  TtaPlay : Activate/Deactivate Animation (restart)
  ----------------------------------------------------------------------*/
void TtaPlay (Document doc, View view)
{
  int frame;

  frame = GetWindowNumber (doc, view);
  TtaChangePlay (frame);
  FrameTable[frame].BeginTime = 0;
  FrameTable[frame].LastTime = 0; 
}

/*----------------------------------------------------------------------
  TtaNoPlay : Make sure we deactivate/reset all Animation of this frame
  And reset Time (Stop)
  ----------------------------------------------------------------------*/
void TtaNoPlay (int frame)
{
  if (frame && frame < MAX_FRAME)
    if (FrameTable[frame].Anim_play)
      {
	TtaChangePlay (frame);
	FrameTable[frame].BeginTime = 0;
	FrameTable[frame].LastTime = 0;
      }
}

/*----------------------------------------------------------------------
  TtaPause : Make sure we pause all Animation of this frame
  ----------------------------------------------------------------------*/
void TtaPause (int frame)
{
  if (frame && frame < MAX_FRAME)
    if (FrameTable[frame].Anim_play)
      {
	TtaChangePlay (frame);
	FrameTable[frame].BeginTime = 0;
	FrameTable[frame].LastTime = 0;
      }
}
/*----------------------------------------------------------------------
  SetFrameCurrentTime : Position current time
  ----------------------------------------------------------------------*/
void TtaSetFrameCurrentTime (AnimTime current_time, int frame)
{
  FrameTable[frame].LastTime = current_time;
}

#ifdef _WINGUI
/*----------------------------------------------------------------------
  MyTimerProc : Call drawing upon timer calls
  ----------------------------------------------------------------------*/
VOID CALLBACK MyTimerProc (HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)     
{ 
 GL_DrawAll ();   
} 
#endif /*_WINGUI*/
/*----------------------------------------------------------------------
  TtaChangePlay : Activate Animation
  ----------------------------------------------------------------------*/
void TtaChangePlay (int frame)
{
  ThotBool remove;

  if (frame && frame <= MAX_FRAME)
    if (FrameTable[frame].Animated_Boxes)
      {
	FrameTable[frame].Anim_play = (FrameTable[frame].Anim_play ? FALSE : TRUE);
	if (FrameTable[frame].Anim_play)
	  {
	    if (AnimTimer == 0)
#ifdef _GTK
	      AnimTimer = gtk_timeout_add (FRAME_TIME,
					   (gpointer) GL_DrawAll, 
					   (gpointer)   NULL); 
	   	      
#else /*_GTK*/
#ifdef _WINGUI
	    {
	      /* SetTimer(FrMainRef[frame],                
		 frame,               
		 FRAME_TIME,                     
		 (TIMERPROC) MyTimerProc); 

		 AnimTimer = frame;*/
	      AnimTimer = SetTimer (NULL,                
				    frame,               
				    FRAME_TIME,                     
				    (TIMERPROC) MyTimerProc);
	    }    
#endif /*_WINGUI*/
#endif /*_GTK*/
	    FrameTable[frame].BeginTime = 0;
	    FrameTable[frame].LastTime = 0;
	  }
	else
	  if (AnimTimer)
	    {
	      remove = FALSE;
	      for (frame = 0; frame < MAX_FRAME; frame++)
		{
		  if (FrameTable[frame].Anim_play)
		    remove = TRUE;
		}
	      if (remove)
		{

#ifdef _GTK
		  gtk_timeout_remove (AnimTimer); 	
#else /*_GTK*/
#ifdef _WINGUI
		  /*KillTimer (FrMainRef[AnimTimer], AnimTimer);*/
		  KillTimer (NULL, AnimTimer);
#endif /*_WINGUI*/
#endif /*_GTK*/
		  AnimTimer = 0; 
		}
	    }
      }  
}

#endif /*_GL*/
