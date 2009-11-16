/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * gltimer.c : handling of Time and Event routines, 
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
#endif /* _WINGUI*/

#ifdef _WX
#include "wx/wx.h"
#include "wx/glcanvas.h"
#else
#include <GL/gl.h>
#endif /* _WX */

#include "thot_gui.h"
#include "ustring.h"
#include "math.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"

#ifdef _WX
#include "AmayaPage.h"
#include "AmayaFrame.h"
#include "wxAmayaTimer.h"
#endif /* _WX */

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "font_tv.h"
#include "frame_tv.h"
#include "units_tv.h"
#include "edit_tv.h"
#include "thotcolor_tv.h"


#include "appdialogue_wx.h"
#include "boxlocate_f.h"
#include "buildlines_f.h"
#include "context_f.h"
#include "font_f.h"
#include "inites_f.h"
#include "memory_f.h"
#include "units_f.h"
#include "windowdisplay_f.h"
#include "tesse_f.h"

#include "glwindowdisplay.h"
#include "applicationapi_f.h"
#include "animbox_f.h"
#include "frame_f.h"
/*
  One Timer To Rule Them All 
  (ie : all animation, frame, and windows are using the same timer, 
  using start value substract to get their own time)
*/
#ifdef _WX
static wxAmayaTimer * AnimTimer = NULL; 
#endif /* _WX */
static ThotBool BadGLCard = FALSE;
/* Animation Smoothness*/
#define FPS 25 /*Frame Per Second*/
#define INTERVAL 0.02 /*1/FPS*/ /* should be 1/25 ... 1/50 */

#define FRAME_TIME 5 /*(INTERVAL*1000) */
/* milliseconds */

#define IS_ZERO(arg)                    (fabs(arg)<1.e-20)

/*----------------------------------------------------------------------
  TtaPlay : Activate/Deactivate Animation (restart)
  ----------------------------------------------------------------------*/
void TtaPlay (Document doc, View view)
{
  int frame;

  frame = GetWindowNumber (doc, view);
  if (frame && frame <= MAX_FRAME && FrameTable[frame].Animated_Boxes)
    TtaChangePlay (frame, TRUE);
  FrameTable[frame].BeginTime = 0;
  FrameTable[frame].LastTime = 0; 
}

/*----------------------------------------------------------------------
  TtaNoPlay : Make sure we deactivate/reset all Animation of this frame
  And reset Time (Stop)
  ----------------------------------------------------------------------*/
void TtaNoPlay (int frame)
{
  if (frame && frame < MAX_FRAME && FrameTable[frame].Anim_play)
      {
        TtaChangePlay (frame, FALSE);
        FrameTable[frame].BeginTime = 0;
        FrameTable[frame].LastTime = 0;
      }
}

/*----------------------------------------------------------------------
  TtaPause : Make sure we pause all Animation of this frame
  ----------------------------------------------------------------------*/
void TtaPause (int frame)
{
  if (frame && frame < MAX_FRAME && FrameTable[frame].Anim_play)
      {
        FrameTable[frame].BeginTime = 0;
        FrameTable[frame].LastTime = 0;
        TtaChangePlay (frame, FALSE);
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

#ifdef _WX
/*----------------------------------------------------------------------
  MyAnimTimerProcWX : Call drawing upon timer calls
  ----------------------------------------------------------------------*/
void MyAnimTimerProcWX( void * data )     
{ 
  GL_DrawAll ();
} 
#endif /* _WX */

/*----------------------------------------------------------------------
  TtaDestroyAnimTimer : destroy anim timer if necessary (necessary = no more animation running)
  check_other_animation is true if the function must check if there isn't running animations.
  check_other_animation is false if you know that there isn't running animations (to optimize)
  ----------------------------------------------------------------------*/
static void TtaDestroyAnimTimer( ThotBool check_other_animation )
{
  int frame;
  ThotBool other_animation;

  if (AnimTimer)
    {
      other_animation = FALSE;
      if (check_other_animation)
        {
          for (frame = 0; frame < MAX_FRAME; frame++)
            {
              if (FrameTable[frame].Anim_play)
                other_animation = TRUE;
            }
        }
      if (!other_animation)
        {
          AnimTimer->Stop();
          delete AnimTimer;
          AnimTimer = 0; 
        }
    }
}

/*----------------------------------------------------------------------
  TtaChangePlay : Activate Animation
  ----------------------------------------------------------------------*/
void TtaChangePlay (int frame, ThotBool status)
{
  if (frame && frame <= MAX_FRAME && FrameTable[frame].Animated_Boxes)
    {
      FrameTable[frame].Anim_play = status;
      if (status)
        {
          if (AnimTimer == NULL)
            {
              AnimTimer = new wxAmayaTimer( MyAnimTimerProcWX, NULL );
              /* start a one shot timer */
              AnimTimer->Start( FRAME_TIME , FALSE );
            }
          FrameTable[frame].BeginTime = 0;
          FrameTable[frame].LastTime = 0;
        }
      else
        TtaDestroyAnimTimer (TRUE);
    }
}


/*----------------------------------------------------------------------
  SetDocumentCurrentTime : Position current time
  ----------------------------------------------------------------------*/
void TtaSetDocumentCurrentTime (double current_time, Document doc)
{
  int frame;

  frame = GetWindowNumber (doc, 1);  
  Animate_boxes (frame, current_time);
  FrameTable[frame].LastTime = current_time;
  GL_DrawAll ();
}
/*----------------------------------------------------------------------
  SetBadCard :  handle video cards that flush backbuffer after each
  buffer swap
  ----------------------------------------------------------------------*/
void SetBadCard (ThotBool badbuffer)
{
  BadGLCard = badbuffer;
}

/*----------------------------------------------------------------------
  GetBadCard :  handle video cards that flush backbuffer after each
  buffer swap
  ----------------------------------------------------------------------*/
ThotBool GetBadCard ()
{
#ifdef _MACOS
  return FALSE;
#else /* _MACOS */
  return BadGLCard;
#endif /* _MACOS */
}

/*----------------------------------------------------------------------
  GL_DrawAll : Really Draws opengl !!
  This Function is called after any event any window receive,
  and by the Timer.
  If a frame need to be redrawn (FrameTable[frame].DblBuffNeedSwap),
  or animated (FrameTable[frame].Animated_Boxes && FrameTable[frame].Anim_play)
  Then we call a redrawframebottom
  ----------------------------------------------------------------------*/
ThotBool GL_DrawAll ()
{
  Document         doc;
  AnimTime         current_time;
  ThotBool         was_animation = FALSE; 
  char             out[128];
  unsigned int     i;
  int              frame, nb_animated_frame = 0;
  static ThotBool  frame_animating = FALSE;  
  static double    lastime;

  if (!FrameUpdating)
    {
      FrameUpdating = TRUE;     
      if (!frame_animating)
        {	  
          frame_animating = TRUE; 
          for (frame = 1; frame < MAX_FRAME; frame++)
            {
#ifdef _WX
              if (FrameTable[frame].WdFrame != 0)
#endif /* _WX */
                {
                  if (FrameTable[frame].Animated_Boxes && FrameTable[frame].Anim_play)
                    {
                      nb_animated_frame++;
                      current_time = ComputeThotCurrentTime (frame);
                      if ((current_time + 1) > 0.0001)
                        {
                          glDisable (GL_SCISSOR_TEST);
                          if (Animate_boxes (frame, current_time))
                            TtaPause (frame);
                          else
                            was_animation = TRUE;
                          FrameTable[frame].LastTime = current_time;
                          glEnable (GL_SCISSOR_TEST);
                        }
                      else
                        current_time = FrameTable[frame].LastTime;
                    }
                  else
                    current_time = FrameTable[frame].LastTime;
                  doc =  FrameTable[frame].FrDoc;
                  if (FrameTable[frame].DblBuffNeedSwap &&
                      doc && LoadedDocument[doc - 1] &&
                      documentDisplayMode[doc - 1] == DisplayImmediately)
                    {
#ifdef _WX
                      // do not draw anything if the animated canvas page is not raidsed
                      if (TtaFrameIsShown (frame))
#endif /* _WX */
                        if (GL_prepare (frame))
                          {
                            if (BadGLCard)
                              DefClip (frame, -1, -1, -1, -1);
                            /* prevent flickering*/
                            GL_SwapStop (frame);
                            RedrawFrameBottom (frame, 0, NULL); 
                            if (was_animation)
                              {
                                lastime = current_time - lastime;
                                if (IS_ZERO(lastime))
                                  {
                                    sprintf (out, "t: %2.3f s - %2.0f fps", 
                                             current_time, 
                                             (double) 1 / lastime);
				  
                                    i = 0;
                                    TtaSetStatus (FrameTable[frame].FrDoc, 
                                                  FrameTable[frame].FrView, 
                                                  out, NULL);
                                    lastime = current_time;
                                  }
                              }
                            GL_SwapEnable (frame);
                            GL_Swap (frame);  
                          }
#ifdef _GL_DEBUG
                      GL_Err ();
#endif /* _GL_DEBUG */
                    }
                }
            }
#ifdef _WX
#ifndef _MACOS
          //if (was_animation)
          //  TtaHandlePendingEvents();
#endif
#endif /* _WX */

          /* stop the animation timer if there is no animated frame */
          if (nb_animated_frame == 0)
            TtaDestroyAnimTimer( FALSE );
          frame_animating  = FALSE;      
        }  
      FrameUpdating = FALSE;     
    }
  return TRUE;
}


#endif /* _GL */
