#ifdef _WX

#ifndef __AMAYAAPP_H__
#define __AMAYAAPP_H__

#include "wx/wx.h"
#include "wx/app.h"
#include "wx/strconv.h"
#include "wx/imaglist.h"

#ifdef _GL
  #include "wx/glcanvas.h"
#endif /* _GL */

class wxAmayaSocketEventLoop;

#ifdef _GL
class AmayaApp : public wxGLApp
#else /* _GL */
class AmayaApp : public wxApp
#endif /* _GL */ 
{
public:

  virtual bool 	OnInit();
  virtual int 	OnExit();

  void OnIdle( wxIdleEvent& event );

#ifdef _GL
  static int * GetGL_AttrList();
#endif /* _GL */

  static wxImageList * GetDocumentIconList();

protected:
  DECLARE_EVENT_TABLE()

  bool m_AmayaIsLaunched;


  wxAmayaSocketEventLoop * m_SocketEventLoop;

#ifdef _GL
  static int AttrList[];
#endif /* _GL */

  int		amaya_argc;
  char ** 	amaya_argv;
  void InitAmayaArgs();
  void ClearAmayaArgs();

  static wxImageList * m_pDocImageList; /* this is the list where documents icons are stored */

};

DECLARE_APP(AmayaApp)

#endif // __AMAYAAPP_H__
  
#endif /* #ifdef _WX */
