/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/protocol/http.h"
#include "wx/socket.h"

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
#include "message_wx.h"
#include "libmsg.h"
#include "frame.h"
#include "registry_wx.h"
#include "thot_key.h"
#include "logdebug.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
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
#include "appdialogue_wx.h"
#include "input_f.h"

#include "AmayaStatsThread.h"
#include "AmayaLogDebug.h"

/*----------------------------------------------------------------------
 *       Class:  AmayaStatsThread
 *      Method:  AmayaStatsThread
 * Description:  create a new AmayaStatsThread
  -----------------------------------------------------------------------*/
AmayaStatsThread::AmayaStatsThread() : wxThread(wxTHREAD_JOINABLE/*wxTHREAD_DETACHED*/)
{
}

/*----------------------------------------------------------------------
 *       Class:  AmayaStatsThread
 *      Method:  ~AmayaStatsThread
 * Description:  destructor
  -----------------------------------------------------------------------*/
AmayaStatsThread::~AmayaStatsThread()
{
}

/*----------------------------------------------------------------------
 *       Class:  AmayaStatsThread
 *      Method:  
 * Description:  
  -----------------------------------------------------------------------*/
void * AmayaStatsThread::Entry()
{
  wxHTTP sock;

#if 0
  wxIPV4address addr;
  addr.Hostname(_T("wam.inrialpes.fr"));
  addr.Service(80);

  sock.Connect(addr, true);
  while ( !sock.WaitOnConnect(1) )
    {
      if (TestDestroy())
	{
	  return NULL;
	}
    }
  if ( sock.IsConnected() )
#endif /* 0 */

    if ( sock.Connect(_T("wam.inrialpes.fr"), 80) )
    {
      /* we are connected, send a simple request */
      char buffer[128];
      sock.SetHeader( wxT("User-Agent"),
		      TtaConvMessageToWX(TtaGetAppName())+_T("-")+TtaConvMessageToWX(TtaGetAppVersion())
#ifdef _WINDOWS
		      +_T(" (Windows)")
#endif /* _WINDOWS */
#ifdef _UNIX
		      +_T(" (Linux)")
#endif /* _UNIX */
		      );
      wxInputStream * p_inputstream = sock.GetInputStream(_T("http://wam.inrialpes.fr/software/amaya/stats/"));
      if (p_inputstream)
	{
	  p_inputstream->Read(buffer, 128);	  
	}
      delete p_inputstream;     
    }

  //  Exit();

  return NULL;
}


#endif /* #ifdef _WX */
