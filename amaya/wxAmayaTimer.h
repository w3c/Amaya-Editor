#ifdef _WX

#ifndef __WXAMAYATIMER_H__
#define __WXAMAYATIMER_H__

#include "wx/timer.h"

/* the context type */
typedef void * AmayaTimerContext;
/* the callback prototype */
typedef void (*AmayaTimerCallback) (AmayaTimerContext);

/*
 *  Description:  wxAmayaTimer is an interface to manage timers
 *                the parameters are a callback and a callback context (data)
 *                when the timer is launched, a timeout parameter is given
 *                when the timer occur, the callback is called with the given context
 *       Author:  Stephane GULLY
 *      Created:  29/03/2004 04:45:34 PM CET
 *     Revision:  none
*/
class wxAmayaTimer : public wxTimer
{
  DECLARE_DYNAMIC_CLASS(wxAmayaTimer)

 public:
  wxAmayaTimer( AmayaTimerCallback p_callback = NULL, AmayaTimerContext p_data = NULL);
  ~wxAmayaTimer();
  
  void Notify();

 protected:
  AmayaTimerCallback m_pCallback;
  AmayaTimerContext  m_pData;
};

#endif /* #ifndef __WXAMAYATIMER_H__ */

#endif /* _WX */
