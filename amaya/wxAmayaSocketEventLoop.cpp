#ifdef _WX

#include "wxAmayaSocketEventLoop.h"
#include "wxAmayaSocketEvent.h"

IMPLEMENT_DYNAMIC_CLASS(wxAmayaSocketEventLoop, wxTimer)

wxAmayaSocketEventLoop::wxAmayaSocketEventLoop( int polling_delay ) : wxTimer()
{
  m_Started = false;
  m_PollingDelay = polling_delay;
}

wxAmayaSocketEventLoop::~wxAmayaSocketEventLoop()
{
  Stop();
}

void wxAmayaSocketEventLoop::Start()
{
  if (!m_Started)
    {
      m_Started = true;
      wxTimer::Start(m_PollingDelay, false);
    }
}

void wxAmayaSocketEventLoop::Stop()
{
  if (m_Started)
    {
      m_Started = false;
      wxTimer::Stop();
    }
}

void wxAmayaSocketEventLoop::Notify()
{
  wxAmayaSocketEvent::CheckSocketStatus();
}
#endif /* _WX */
