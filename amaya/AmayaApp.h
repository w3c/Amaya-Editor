#ifdef _WX

#ifndef __AMAYAAPP_H__
#define __AMAYAAPP_H__

#include "wx/wx.h"
#include "wx/app.h"
#include "wx/strconv.h"

class wxAmayaSocketEventLoop;

class AmayaApp : public wxApp
{
public:

  virtual bool 	OnInit();
  virtual int 	OnExit();

  void OnIdle( wxIdleEvent& event );

 public:
  static wxCSConv conv_ascii;

protected:
  DECLARE_EVENT_TABLE()

  wxAmayaSocketEventLoop * m_SocketEventLoop;

  int		amaya_argc;
  char ** 	amaya_argv;
  void InitAmayaArgs();
  void ClearAmayaArgs();
};

DECLARE_APP(AmayaApp)

#endif // __AMAYAAPP_H__
  
#endif /* #ifdef _WX */
