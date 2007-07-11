#ifdef _WX

#ifndef __WXAMAYASOCKETEVENTLOOP_H__
#define __WXAMAYASOCKETEVENTLOOP_H__

#include "wx/timer.h"

class wxAmayaSocketEventLoop : public wxTimer
{
//  DECLARE_DYNAMIC_CLASS(wxAmayaSocketEventLoop)
    
 public:
  wxAmayaSocketEventLoop( int polling_delay = 100 );
  virtual ~wxAmayaSocketEventLoop();
  
  void Start();
  void Stop();

  void Notify();

  static void InitSocketLib();
  static void CleanupSocketLib();

 protected:
  bool m_Started;
  int m_PollingDelay;
};

#endif /* __WXAMAYASOCKETEVENTLOOP_H__ */

#endif /* _WX */
