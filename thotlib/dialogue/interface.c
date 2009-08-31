/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * document windows API
 *
 * Authors: I. Vatton (INRIA)
 *          D. Veillard (W3C/INRIA) Multikey + Event rewrite
 *          R. Guetari (W3C/INRIA) - Windows version
 *
 */

#ifdef _WX
 #include "wx/wx.h"
#endif /* _WX */
#include "thot_gui.h"
#include "thot_sys.h"

#include "constmenu.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "libmsg.h"
#include "dialog.h"
#include "application.h"
#include "document.h"
#include "view.h"
#include "interface.h"
#include "message.h"
#include "appdialogue.h"
#include "typecorr.h"
#include "frame_f.h"
#ifdef _WINGUI
#include "wininclude.h"
#endif /* _WINGUI */
#include <locale.h>
#ifdef _GL
#include "glwindowdisplay.h"
#endif /*_GL*/

#define MAX_ARGS 20

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "font_tv.h"
#include "boxes_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

#include "actions_f.h"
#include "appdialogue_f.h"
#include "appli_f.h"
#include "applicationapi_f.h"
#include "boxlocate_f.h"
#include "callback_f.h"
#include "context_f.h"
#include "dialogapi_f.h"
#include "displaybox_f.h"
#include "font_f.h"
#include "inites_f.h"
#include "input_f.h"
#include "message_f.h"
#include "thotmsg_f.h"
#include "viewapi_f.h"
#include "views_f.h"


/*----------------------------------------------------------------------
   TtaFetchOneAvailableEvent
   retrieve one X-Windows Event from the queue if one is immediately
   available.
  ----------------------------------------------------------------------*/
ThotBool TtaFetchOneAvailableEvent (ThotEvent *ev)
{
#ifdef _WX
   return wxTheApp->Pending();
#endif /* _WX */
   return (FALSE);
}


/*----------------------------------------------------------------------
   TtaHandleOneEvent

   Processes one given event in the application.
  ----------------------------------------------------------------------*/
void TtaHandleOneEvent (ThotEvent *ev)
{
#ifdef _WINGUI
  if (ev->message != WM_QUIT)
    {
      TranslateMessage (ev);
      DispatchMessage (ev);
    }
#endif /* _WINGUI */
#ifdef _WX
  wxTheApp->Dispatch();
#endif /* _WX */
}

/*----------------------------------------------------------------------
  TtaHandlePendingEvents
  Processes all pending events in an application.
  ----------------------------------------------------------------------*/
void TtaHandlePendingEvents ()
{
#ifdef _WX
   while (wxTheApp->Pending())
     wxTheApp->Dispatch ();
#endif /* _WX */
}


/*----------------------------------------------------------------------
   TtaMainLoop
   Starts the main loop for processing all events in an application. This
   function must be called after all initializations have been made.
  ----------------------------------------------------------------------*/
void TtaMainLoop ()
{
  NotifyEvent         notifyEvt;
#ifndef _WX
  ThotEvent           ev;
#endif /* _WX */

  UserErrorCode = 0;
  /* Sets the current locale according to the program environment */
#ifdef _WX
  /* In order to get a "." even in a localised unix (ie: french becomes ",") */
  setlocale (LC_NUMERIC, "C");
#endif /* _WX */
#ifdef _WINGUI
  setlocale (LC_ALL, ".OCP");
  /* _setmbcp (_MB_CP_OEM); */
#endif /* _WINGUI */
  notifyEvt.event = TteInit; /* Sends the message Init.Pre */
  if (CallEventType (&notifyEvt, TRUE))
    {
      /* The application is not able to start the editor => quit */
      exit (0);
    }

  notifyEvt.event = TteInit; /* Sends the message Init.Post */
  CallEventType (&notifyEvt, FALSE);

#ifdef _WINGUI
  /* Loop wainting for the events */
  while (1)
    {
      if (GetMessage (&ev, NULL, 0, 0))
        {
          TtaHandleOneEvent (&ev);
#ifdef _GL
          /* buffer swapping, when needed*/
          GL_DrawAll ();
#endif/*  _GL */
        }
    }
#endif /* _WINGUI */
}

/*----------------------------------------------------------------------
   TtaGetMenuColor
   Returns the color used for the background of dialogue windows.
  ----------------------------------------------------------------------*/
ThotColor TtaGetMenuColor ()
{
   UserErrorCode = 0;
   return (ThotColor) BgMenu_Color;
}


/*----------------------------------------------------------------------
   TtaGetScreenDepth
   Returns the current screen depth.
  ----------------------------------------------------------------------*/
int TtaGetScreenDepth ()
{
   UserErrorCode = 0;
   return TtWDepth;
}

/*----------------------------------------------------------------------
  TtaClickElement
  Waits for a new click and returns the cliked document and element.
  ----------------------------------------------------------------------*/
void TtaClickElement (Document *document, Element *element)
{
  PtrAbstractBox      pAb;
  int                 frame;

  UserErrorCode = 0;
  if (element == NULL || document == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      *element = (Element) None;
      *document = (Document) None;
      GiveClickedAbsBox (&frame, &pAb);
      if (frame == 0 || pAb == NULL)
	return;
      else
	{
	  *element = (Element) pAb->AbElement;
	  *document = FrameTable[frame].FrDoc;
	}
    }
}


/*----------------------------------------------------------------------
  TtaGetClickedElement
  Returns the last clicked element.
  ----------------------------------------------------------------------*/
void TtaGetClickedElement (Document *document, Element *element)
{
  PtrAbstractBox pAb;

  UserErrorCode = 0;
  if (element == NULL || document == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      *element = (Element) None;
      *document = (Document) None;
      LocateSelectionInView (ClickFrame, ClickX, ClickY, 7, NULL);
      pAb = GetClickedAbsBox (ClickFrame, ClickX, ClickY);
      if (ClickFrame == 0 || pAb == NULL)
	return;
      else
	{
	  *element = (Element) pAb->AbElement;
	  *document = FrameTable[ClickFrame].FrDoc;
	}
    }
}


/*----------------------------------------------------------------------
   TtaGiveSelectPosition: returns the mouse position for the last click 
   with respect to the element (position in pixel)            
  ----------------------------------------------------------------------*/
void TtaGiveSelectPosition (Document document, Element element, View view,
			    int *X, int *Y)
{
   int                 frame;
   PtrAbstractBox      pAb;
   ViewFrame          *pFrame;

   /* verifies the parameter document */
   UserErrorCode = 0;
   if (document == 0)
      TtaError (ERR_invalid_document_parameter);
   else if (view == 0 || element == 0)
      TtaError (ERR_invalid_parameter);
   else
     {
	frame = GetWindowNumber (document, view);
	if (frame != ClickFrame)
	   TtaError (ERR_no_selection_in_view);
	else
	  {
	     /* Determine the mouse position in the specified box */
	     pFrame = &ViewFrameTable[frame - 1];
	     pAb = ((PtrElement) element)->ElAbstractBox[view - 1];
	     if (pAb == NULL)
		TtaError (ERR_no_selection_in_view);
	     else
	       {
		  while (pAb->AbPresentationBox && pAb->AbNext != NULL)
		     pAb = pAb->AbNext;
#ifndef _GL
		  *X = ClickX + pFrame->FrXOrg - pAb->AbBox->BxXOrg;
		  *Y = ClickY + pFrame->FrYOrg - pAb->AbBox->BxYOrg;
#else /* _GL */
		  *X = ClickX - pAb->AbBox->BxClipX;
		  *Y = ClickY - pAb->AbBox->BxClipY;
#endif /* _GL */
	       }
	  }
     }
}
