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

typedef void (*OpenURLCallback) (void * );

class wxAmayaSocketEventLoop;
class wxSingleInstanceChecker;
class AmayaAppInstance;
class AmayaLogDebug;
class AmayaActionEvent;

#ifdef _GL
class AmayaApp : public wxGLApp
#else /* _GL */
class AmayaApp : public wxApp
#endif /* _GL */ 
{
public:
  void RegisterOpenURLCallback( OpenURLCallback callback );
  void CallOpenURLCallback( char * url );
  int FilterEvent(wxEvent& event);
#ifdef _MACOS
  short MacHandleAEODoc(const WXEVENTREF event, WXEVENTREF WXUNUSED(reply));
#endif /* _MACOS */
  void OpenQueryDocs();

#ifdef _GL
  static int * GetGL_AttrList();
#endif /* _GL */

  static wxImageList * GetDocumentIconList();
  static int GetDocumentIconId(const char * p_name);
  static wxIcon GetAppIcon();

  
  static void PostAmayaAction(const AmayaActionEvent& event);
  
#ifdef __WXDEBUG__
  static AmayaLogDebug * GetAmayaLogDebug( wxWindow * p_parent );
  static void DestroyAmayaLogDebug();
#endif /* #ifdef __WXDEBUG__ */

 protected:
  DECLARE_EVENT_TABLE()
  virtual bool 	OnInit();
  virtual int 	OnExit();
  void OnIdle( wxIdleEvent& event );
  void OnChar( wxKeyEvent& event );
  void OnKeyDown( wxKeyEvent& event );

  void OnAction(AmayaActionEvent& event);
  
  static void SetupDocumentIconList();

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
  static wxIcon m_AppIcon;

  AmayaAppInstance * m_pAmayaInstance;

  bool m_AmayaIsInit;

#ifdef __WXDEBUG__
  static AmayaLogDebug * m_pAmayaLogDebug;
#endif /* #ifdef __WXDEBUG__ */
};

DECLARE_APP(AmayaApp)

#endif // __AMAYAAPP_H__
  
#endif /* #ifdef _WX */
