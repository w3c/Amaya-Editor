/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wxAmayaTimer.h"
extern void TtaHandlePendingEvents ();

IMPLEMENT_DYNAMIC_CLASS(wxAmayaTimer, wxTimer)

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
wxAmayaTimer::wxAmayaTimer( AmayaTimerCallback p_callback, AmayaTimerContext p_data) : wxTimer()
{
  m_pCallback = p_callback;
  m_pData     = p_data;
}

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
wxAmayaTimer::~wxAmayaTimer()
{
  m_pCallback = NULL;
}
  
/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
void wxAmayaTimer::Notify()
{
  if (m_pCallback)
    (*m_pCallback)(m_pData);
  TtaHandlePendingEvents();
}

#endif /* _WX */
