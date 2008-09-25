/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "dialog.h"
#include "selection.h"
#include "application.h"
#include "dialog.h"
#include "document.h"
#include "message.h"
#include "libmsg.h"
#include "frame.h"
#include "registry_wx.h"
#include "thot_key.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "font_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"
#include "font_f.h"
#include "appli_f.h"
#include "profiles_f.h"
#include "appdialogue_f.h"
#include "boxparams_f.h"
#include "dialogapi_f.h"
#include "callback_f.h"
#include "AmayaParams.h"
#include "appdialogue_wx_f.h"
#include "input_f.h"

#include "AmayaWindowIterator.h"
#include "AmayaWindow.h"
#include "AmayaNormalWindow.h"

void AmayaWindowIterator::first()
{
  m_CurrentId = 0;
  next(); // search the first entry
}

void AmayaWindowIterator::next()
{
  m_CurrentId++;
  while(!isDone() && !WindowTable[m_CurrentId].WdWindow)
    m_CurrentId++;
}

bool AmayaWindowIterator::isDone()
{
  return (m_CurrentId >= MAX_WINDOW);
}

void * AmayaWindowIterator::currentElement()
{
  if (!isDone())
    return WindowTable[m_CurrentId].WdWindow;
  else
    return NULL;
}

int AmayaWindowIterator::currentWindowId()
{
  if (currentElement())
    return ((AmayaWindow *)currentElement())->GetWindowId();
  else
    return 0;
}


/*----------------------------------------------------------------------
  TtaUpdateToolPanelLayout
  ----------------------------------------------------------------------*/
void TtaUpdateToolPanelLayout ()
{
  AmayaWindowIterator it;
  for( it.first(); !it.isDone(); it.next() )
    {
      AmayaWindow* win = (AmayaWindow*)it.currentElement();
      AmayaNormalWindow* thewin = wxDynamicCast(win, AmayaNormalWindow);
      if(thewin)
        thewin->UpdateToolPanelLayout();
    }
}

#endif /* #ifdef _WX */
