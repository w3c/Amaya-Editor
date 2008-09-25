/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/log.h"
#include "logdebug.h"
#include "wxAmayaSocketEventLoop.h"
#include "wxAmayaSocketEvent.h"

#ifdef _WINDOWS
  #include "winsock.h"
  /* these define come from wwwsys.h */
  #define DESIRED_WINSOCK_VERSION 0x0101  /* we'd like winsock ver 1.1... */
  #define MINIMUM_WINSOCK_VERSION 0x0101  /* ...but we'll take ver 1.1 :) */
#endif /* _WINDOWS */

//IMPLEMENT_DYNAMIC_CLASS(wxAmayaSocketEventLoop, wxTimer)

wxAmayaSocketEventLoop::wxAmayaSocketEventLoop( int polling_delay ) : wxTimer()
{
  m_Started = false;
  m_PollingDelay = polling_delay;
}

wxAmayaSocketEventLoop::~wxAmayaSocketEventLoop()
{
  Stop();

  CleanupSocketLib();
}

void wxAmayaSocketEventLoop::InitSocketLib()
{
#ifdef _WINDOWS
  /*
  ** Initialise WinSock DLL. This must also be shut down!
  */
  {
    WSADATA wsadata;
    if (WSAStartup(DESIRED_WINSOCK_VERSION, &wsadata))
      {
	TTALOGDEBUG_0( TTA_LOG_SOCKET, _T("wxAmayaSocketEventLoop: Can't initialize WinSoc") );
	WSACleanup();
      }
    if (wsadata.wVersion < MINIMUM_WINSOCK_VERSION)
      {
	TTALOGDEBUG_0( TTA_LOG_SOCKET, _T("wxAmayaSocketEventLoop: Bad version of WinSoc") );
	WSACleanup();
      }
    TTALOGDEBUG_1( TTA_LOG_SOCKET, _T("wxAmayaSocketEventLoop: Using WinSoc version \"%x\""),
		   wsadata.wVersion );
  }
#endif /* _WINDOWS */
}

void wxAmayaSocketEventLoop::CleanupSocketLib()
{
#ifdef _WINDOWS
  WSACleanup();
#endif /* _WINDOWS */
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
