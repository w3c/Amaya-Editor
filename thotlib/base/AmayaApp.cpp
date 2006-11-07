#ifdef _WX

#include "wx/wx.h"
#include "wx/clipbrd.h"
#include "wx/image.h"
#include "wx/imaglist.h"
#include "wx/snglinst.h"
#ifndef _GLPRINT
  #include "wx/xrc/xmlres.h"          // XRC XML resouces
#endif /* _GLPRINT */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "dialog.h"
#include "selection.h"
#include "application.h"
#include "document.h"
#include "message.h"
#include "libmsg.h"
#include "frame.h"
#include "logdebug.h"

#include "AmayaApp.h"

//#define THOT_EXPORT extern
//#include "amaya.h"


#include "appdialogue_wx.h"
#include "registry_wx.h"
#include "message_wx.h"

#include "AmayaLogDebug.h"
#include "wxAmayaSocketEventLoop.h"
#include "wxAmayaSocketEvent.h"
#include "AmayaAppInstance.h"
#include "AmayaWindow.h"

IMPLEMENT_APP(AmayaApp)

#ifndef _GLPRINT
// defined into EDITORAPP.c or print.c
extern int amaya_main (int argc, char** argv);
#else /* _GLPRINT */
/* TODO */
#endif /* #ifndef _GLPRINT */

#ifdef _GL
/*
  WX_GL_RGBA 	        Use true colour
  WX_GL_BUFFER_SIZE 	Bits for buffer if not WX_GL_RGBA
  WX_GL_LEVEL 	        0 for main buffer, >0 for overlay, <0 for underlay
  WX_GL_DOUBLEBUFFER 	Use doublebuffer
  WX_GL_STEREO 	        Use stereoscopic display
  WX_GL_AUX_BUFFERS 	Number of auxiliary buffers (not all implementation support this option)
  WX_GL_MIN_RED 	Use red buffer with most bits (> MIN_RED bits)
  WX_GL_MIN_GREEN 	Use green buffer with most bits (> MIN_GREEN bits)
  WX_GL_MIN_BLUE 	Use blue buffer with most bits (> MIN_BLUE bits)
  WX_GL_MIN_ALPHA 	Use alpha buffer with most bits (> MIN_ALPHA bits)
  WX_GL_DEPTH_SIZE 	Bits for Z-buffer (0,16,32)
  WX_GL_STENCIL_SIZE 	Bits for stencil buffer
  WX_GL_MIN_ACCUM_RED 	Use red accum buffer with most bits (> MIN_ACCUM_RED bits)
  WX_GL_MIN_ACCUM_GREEN Use green buffer with most bits (> MIN_ACCUM_GREEN bits)
  WX_GL_MIN_ACCUM_BLUE 	Use blue buffer with most bits (> MIN_ACCUM_BLUE bits)
  WX_GL_MIN_ACCUM_ALPHA Use blue buffer with most bits (> MIN_ACCUM_ALPHA bits
*/
int AmayaApp::AttrList[] =
{
  WX_GL_RGBA,
  WX_GL_MIN_RED, 1,
  WX_GL_MIN_GREEN , 1,
  WX_GL_MIN_BLUE, 1,
  WX_GL_MIN_ALPHA, 0, /* don't change the position of this entry (8) */
  WX_GL_STENCIL_SIZE, 1,
  WX_GL_DOUBLEBUFFER,
#ifdef _WINDOWS
  WX_GL_NOT_ACCELERATED, /* don't change the position of this entry (12) */
#endif /* _WINDOWS */
  0
};
#endif /* _GL */

wxImageList * AmayaApp::m_pDocImageList = NULL;
wxIcon AmayaApp::m_AppIcon = wxIcon();
#ifdef __WXDEBUG__
AmayaLogDebug * AmayaApp::m_pAmayaLogDebug = NULL;
#endif /* #ifdef __WXDEBUG__ */

/*----------------------------------------------------------------------
 *       Class:  AmayaApp
 *      Method:  OnInit
 * Description:  this is the entry point
  -----------------------------------------------------------------------*/
bool AmayaApp::OnInit()
{
  m_AmayaIsInit = false;
  // do not let wxWidgets exit Amaya when there is no opened windows
  // ** it can append after a crash when the backup restore dialog 
  // ** is popup before the main Window is created
  // ** this problem occured on MacOSX only but to be sure, just disable this feature
  SetExitOnFrameDelete(FALSE);

  // for debug : the output is stderr
  delete wxLog::SetActiveTarget( new wxLogStderr );

  // under X we usually want to use the primary selection by default (which
  // is shared with other apps)
  wxTheClipboard->UsePrimarySelection(false);

  // this flag is set to false because amaya_main is not allready called
  // the flag will be set to true when amaya_main will be called
  // (when the first idle event will be called)
  m_AmayaIsLaunched = FALSE;

  // Required for images
  wxImage::AddHandler(new wxGIFHandler);
  wxImage::AddHandler(new wxPNGHandler);
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  // check there is no other Amaya instance
  m_pAmayaInstance = new AmayaAppInstance( this );
  if (m_pAmayaInstance->IsAnotherAmayaRunning())
    {
      wxLogError(_T("Another instance is running"));
      
      wxString url;
      if (wxApp::argc % 2 == 0)
        /* The last argument in the command line is the document to be opened */
        url = wxApp::argv[wxApp::argc-1];
      m_pAmayaInstance->SendURLToOtherAmayaInstance( url );
      return false;
    }
  else
    {
      m_pAmayaInstance->StartURLGrabberServer();
    }  
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#ifdef _GL
  // try to find a good configuration for opengl
  TTALOGDEBUG_0( TTA_LOG_INIT, _T("AmayaApp - Try to find a valide opengl configuration."));
  if ( !InitGLVisual(GetGL_AttrList()) )
    {
      TTALOGDEBUG_0( TTA_LOG_INIT, _T("AmayaApp - ERROR -> Try to find another valide opengl configuration (simplier: without ALPHA channel)."));
      // error : try another configuration => wxWidgets default one
      if ( !InitGLVisual(NULL) )
        {
          // error !
          wxPrintf(_T("FATAL ERROR : Your OpenGL implementation does not support needed features!\n"));
          wxExit();
        }
    }
  TTALOGDEBUG_0( TTA_LOG_INIT, _T("AmayaApp - A valide opengl configuration has been found."));
#endif /* _GL */
  
  // just convert arguments format (unicode to UTF-8) before passing it to amaya_main
  InitAmayaArgs();
  /* initialize the Registry */
  TtaInitializeAppRegistry(amaya_argv[0]);
#ifndef _GLPRINT
  // Initialize all the XRC handlers. Always required (unless you feel like
  // going through and initializing a handler of each control type you will
  // be using (ie initialize the spinctrl handler, initialize the textctrl
  // handler). However, if you are only using a few control types, it will
  // save some space to only initialize the ones you will be using. See
  // wxXRC docs for details.
  wxXmlResource::Get()->InitAllHandlers();    
    
  // Load all of the XRC files that will be used. You can put everything
  // into one giant XRC file if you wanted, but then they become more 
  // diffcult to manage, and harder to reuse in later projects.   

  // Now it's possible to load all the dialogs (need to be called after amaya_main or
  // TtaGetEnvString will return bad strings)

  // Attention: rajouter ici toutes les autres ressources a charger  
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "AuthentDlgWX.xrc") );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "CheckedListDlgWX.xrc") );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "ConfirmCloseTab.xrc") );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "CreateTableDlgWX.xrc") );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "DocInfoDlgWX.xrc") );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "EnumListDlgWX.xrc") );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "HRefDlgWX.xrc") );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "ImageDlgWX.xrc") ); 
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "InitConfirmDlgWX.xrc" ) );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "ListDlgWX.xrc") );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "ListEditDlgWX.xrc") );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "MakeIdDlgWX.xrc") );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "NewTemplateDocDlgWX.xrc" ) );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "NumDlgWX.xrc") );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "ObjectDlgWX.xrc") ); 
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "OpenDocDlgWX.xrc" ) );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "Panel.xrc") );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "Panel_ApplyClass.xrc") );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "Panel_Attribute.xrc") );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "Panel_MathML.xrc") );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "Panel_SpeChar.xrc") );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "Panel_XHTML.xrc") );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "Panel_XML.xrc") );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "PreferenceDlgWX.xrc") );
#ifdef _WINDOWS
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "WinPrintDlgWX.xrc") );
#else /* _WINDOWS */
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "PrintDlgWX.xrc") );
#endif /* _WINDOWS */
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "SaveAsDlgWX.xrc") );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "SearchDlgWX.xrc") );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "SpellCheckDlgWX.xrc") );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "StyleDlgWX.xrc") ); 
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "TextDlgWX.xrc") ); 
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "TitleDlgWX.xrc") );
  wxXmlResource::Get()->Load( TtaGetResourcePathWX( WX_RESOURCES_XRC, "Toolbar.xrc") );
  // TODO: rajouter ici toutes les autres ressources a charger (pour les dialogues)

  /* setup the socket event loop */
  /* when a socket is active, check every 100 ms if something happend on the socket */
  m_SocketEventLoop = new wxAmayaSocketEventLoop( 10 );
  wxAmayaSocketEvent::InitSocketEvent( m_SocketEventLoop );

  /* setup the app icon */
#ifdef _WINDOWS
  m_AppIcon = wxIcon( TtaGetResourcePathWX( WX_RESOURCES_ICON_MISC, "logo.ico"),
                      wxBITMAP_TYPE_ICO );
#else /* _WINDOWS */
  m_AppIcon = wxIcon( TtaGetResourcePathWX( WX_RESOURCES_ICON_22X22, "logo.png"),
                      wxBITMAP_TYPE_PNG );
#endif /* _WINDOWS */

  // fill the icons list
  SetupDocumentIconList();
#endif /* _GLPRINT */
#ifdef IV
  // check there is no other Amaya instance
  m_pAmayaInstance = new AmayaAppInstance( this );
  if (m_pAmayaInstance->IsAnotherAmayaRunning())
    {
      wxLogError(_T("Another instance is running"));
      
      wxString url;
      if (wxApp::argc % 2 == 0)
        /* The last argument in the command line is the document to be opened */
        url = wxApp::argv[wxApp::argc-1];
      m_pAmayaInstance->SendURLToOtherAmayaInstance( url );
      return false;
    }
  else
    {
      m_pAmayaInstance->StartURLGrabberServer();
    }
#endif /* IV */
  m_AmayaIsInit = true;
  
  return true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaApp
 *      Method:  OnExit
 * Description:  free all allocated memory
  -----------------------------------------------------------------------*/
int AmayaApp::OnExit()
{
  TTALOGDEBUG_0( TTA_LOG_INIT, _T("AmayaApp::OnExit"));

  delete m_pAmayaInstance;
  m_pAmayaInstance = NULL;
  // flush the clipboard in order to keep current text for further use in other applications
  wxTheClipboard->Flush();

  // stop network loop
  m_SocketEventLoop->Stop();
  delete m_SocketEventLoop;
  m_SocketEventLoop = NULL;

  // free documents icons
  delete m_pDocImageList;
  m_pDocImageList = NULL;

  // free arguments
  ClearAmayaArgs();

  // free internal amaya ressources
  TtaQuit();

  return 0;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaApp
 *      Method:  InitAmayaArgs
 * Description:  this methode convert wxApp::argc and wxApp::argv to amaya format
 *               amaya_argv must be free when closing application
  -----------------------------------------------------------------------*/
void AmayaApp::InitAmayaArgs()
{
  // convert argc and argv in order to be compatible with amaya
  amaya_argc = wxApp::argc;
  amaya_argv = new char*[amaya_argc];
  
  for ( int i = 0; i < amaya_argc; i++ )
    {
      // unicode to ascii convertion of every arguments
      wxString amaya_arg( wxApp::argv[i] );
      amaya_argv[i] = new char[strlen((const char *)amaya_arg.mb_str(wxConvUTF8))+1];
      strcpy(amaya_argv[i], (const char*)amaya_arg.mb_str(wxConvUTF8));
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaApp
 *      Method:  ClearAmayaArgs
 * Description:  free arguments -> must be called when appli exit
  -----------------------------------------------------------------------*/
void AmayaApp::ClearAmayaArgs()
{
  for ( int i = 0; i < amaya_argc; i++ )
  {
    delete [] amaya_argv[i];
  }  
  delete [] amaya_argv;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaApp
 *      Method:  OnIdle
 * Description:  called when there is no more event to procced
  -----------------------------------------------------------------------*/
void AmayaApp::OnIdle( wxIdleEvent& event )
{
  // amaya_main is called only once when the first Idle event is received
  // amaya_main can't be called into OnInit because it can
  // launch user dialogs (ex: Confirm dialogs).
  // The dialogues can't be shown into OnInit because 
  // wxEventLoop is not ready at this place !
  if (!m_AmayaIsLaunched && m_AmayaIsInit)
    {
      m_AmayaIsLaunched = TRUE;
#ifndef _GLPRINT
      // just call amaya main from EDITORAPP.c or print.c
      amaya_main( amaya_argc, amaya_argv );
#else /* _GLPRINT */
	  /* TODO */
#endif /* _GLPRINT */

      // now let wxWidgets exit Amaya when there is no opened windows
      SetExitOnFrameDelete(TRUE);

      TtaSendStatsInfo();
    }

#ifdef DEBUG_FOCUS
    // track the window which has the focus in the status bar
    static wxWindow *s_windowFocus = (wxWindow *)NULL;
    wxWindow *focus = wxWindow::FindFocus();
    if ( focus && (focus != s_windowFocus) )
    {
      s_windowFocus = focus;
      
      wxString msg;
      msg.Printf(
#ifdef __WXMSW__
                _T("Focus: %s, HWND = %08x"),
#else
                _T("Focus: %s"),
#endif
                s_windowFocus->GetClassInfo()->GetClassName()
#ifdef __WXMSW__
                , (unsigned int) s_windowFocus->GetHWND()
#endif
		);
      TTALOGDEBUG_0( TTA_LOG_FOCUS, msg );
    }
#endif /* DEBUG_FOCUS */

  event.Skip();
}

#ifdef _GL
/*----------------------------------------------------------------------
 *       Class:  AmayaApp
 *      Method:  GetGL_AttrList
 * Description:  static methode which returns the opengl best display attributes
  -----------------------------------------------------------------------*/
int * AmayaApp::GetGL_AttrList()
{
#ifdef _WINDOWS
  // depending on thot.rc option set/unset OpenGL hardware acceleration

  // default is no acceleration because of strange bugs on
  // MSWindows platformes with buggy video card drivers
  TtaSetEnvBoolean("GL_ACCELERATED", FALSE, FALSE);
  // now check the user thot.rc state and adjust the opengl's attribute list
  ThotBool gl_accelerated;
  TtaGetEnvBoolean("GL_ACCELERATED", &gl_accelerated);
  if (gl_accelerated)
    AttrList[12] = 0; /* ok enable opengl hardware acceleration */
  else
    AttrList[12] = WX_GL_NOT_ACCELERATED; /* disable opengl hardware acceleration */
#endif /* _WINDOWS */
  return AttrList;
}
#endif /* _GL */

/*----------------------------------------------------------------------
 *       Class:  AmayaApp
 *      Method:  GetDocumentIconList
 * Description:  Returns the documents image list
 *               this is where the document's icons are stored (as mozilla)
  -----------------------------------------------------------------------*/
wxImageList * AmayaApp::GetDocumentIconList()
{
  return m_pDocImageList;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaApp
 *      Method:  SetupDocumentIconList
 * Description:  fill the image list with icons (order is important)
  -----------------------------------------------------------------------*/
void AmayaApp::SetupDocumentIconList()
{
  /* setup the documents image list
   * this is where the document's icons are stored (as mozilla) */
  m_pDocImageList = new wxImageList( 16, 16 );

  const char * icon_array[] = { "default",
                                "html",
                                "text",
                                "image",
                                "css",
                                "source",
                                "annot",
                                "log",
                                "svg",
                                "math",
                                "xml",
                                "library",
                                "bookmark",
                                "" };
  int i = 0;
  while ( icon_array[i][0] != '\0' )
    {
      char png_filename[128];
      sprintf(png_filename, "document_%s.png", icon_array[i] );
      wxBitmap icon( TtaGetResourcePathWX( WX_RESOURCES_ICON_16X16, png_filename), wxBITMAP_TYPE_PNG );
      m_pDocImageList->Add( icon );
      i++;
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaApp
 *      Method:  GetDocumentIconId
 * Description:  Returns the icon id corresponding to the document type name
 *               (see amaya.h => DocumentTypeNames)
  -----------------------------------------------------------------------*/
int AmayaApp::GetDocumentIconId(const char * p_name)
{
  if (!strcmp(p_name, "html"))
    return 1;
  else if (!strcmp(p_name, "text"))
    return 2;
  else if (!strcmp(p_name, "image"))
    return 3;
  else if (!strcmp(p_name, "css"))
    return 4;
  else if (!strcmp(p_name, "source"))
    return 5;
  else if (!strcmp(p_name, "annot"))
    return 6;
  else if (!strcmp(p_name, "log"))
    return 7;
  else if (!strcmp(p_name, "svg"))
    return 8;
  else if (!strcmp(p_name, "math"))
    return 9;
  else if (!strcmp(p_name, "xml"))
    return 10;
  else if (!strcmp(p_name, "library"))
    return 11;
  else if (!strcmp(p_name, "bookmark"))
    return 12;
  else
    return 0;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaApp
 *      Method:  GetAppIcon
 * Description:  Returns the application icon which is shown in the window manager task bar
  -----------------------------------------------------------------------*/
wxIcon AmayaApp::GetAppIcon()
{
  return m_AppIcon;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaApp
 *      Method:  RegisterOpenURLCallback
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaApp::RegisterOpenURLCallback( void (*callback) (void *) )
{
  m_pAmayaInstance->RegisterOpenURLCallback( callback );  
}

/*----------------------------------------------------------------------
 *       Class:  AmayaApp
 *      Method:  CallOpenURLCallback
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaApp::CallOpenURLCallback( char * url )
{
  m_pAmayaInstance->CallOpenURLCallback(url);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaApp
 *      Method:  OnChar
 * Description:  manage keyboard events
  -----------------------------------------------------------------------*/
void AmayaApp::OnChar(wxKeyEvent& event)
{
  TTALOGDEBUG_0( TTA_LOG_KEYINPUT, _T("AmayaApp::OnChar key=")+wxString(event.GetUnicodeKey()));

  // forward the key event to active window.
  // this code is only usfull on windows because on gtk,
  // the events are directly proceed in AmayaWindow class
  AmayaWindow * p_window = TtaGetActiveWindow();
  if (p_window && p_window->IsActive())
  {
    if(!TtaHandleUnicodeKey(event))
      event.Skip();
  }
  else
    event.Skip();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaApp
 *      Method:  OnKeyDown
 * Description:  manage keyboard events : CTRL+1,2,3 ... shortcuts
 *               shortcuts with numbers are managed differently than these with letter
  -----------------------------------------------------------------------*/
void AmayaApp::OnKeyDown(wxKeyEvent& event)
{
  // forward the key event to active window.
  // this code is only usfull on windows because on gtk,
  // the events are directly proceed in AmayaWindow class
  AmayaWindow * p_window = TtaGetActiveWindow();
  if (p_window && p_window->IsActive())
  {
  TTALOGDEBUG_1( TTA_LOG_KEYINPUT, _T("AmayaApp::OnKeyDown key=%d"), event.GetKeyCode());
    if(!TtaHandleSpecialKey(event))
      if (!TtaHandleShortcutKey(event))
        event.Skip();
  }
  else
    event.Skip();
}

#ifdef __WXDEBUG__
/*----------------------------------------------------------------------
 *       Class:  AmayaApp
 *      Method:  GetAmayaLogDebug
 * Description:  return an instance on the logdebug window
  -----------------------------------------------------------------------*/
AmayaLogDebug * AmayaApp::GetAmayaLogDebug(wxWindow * p_parent)
{
  if (!m_pAmayaLogDebug)
    m_pAmayaLogDebug = new AmayaLogDebug(p_parent);
  return m_pAmayaLogDebug;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaApp
 *      Method:  DestroyAmayaLogDebug
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaApp::DestroyAmayaLogDebug()
{
  if (m_pAmayaLogDebug)
    m_pAmayaLogDebug->Destroy();
  m_pAmayaLogDebug = NULL;
}

#endif /* #ifdef __WXDEBUG__ */

/*----------------------------------------------------------------------
 *       Class:  AmayaApp
 *      Method:  FilterEvent
 * Description:  This function is called before processing any event and allows the 
 *               application to preempt the processing of some events. If this method
 *               returns -1 the event is processed normally, otherwise either true or
 *               false should be returned and the event processing stops immediately
 *               considering that the event had been already processed (for the former
 *               return value) or that it is not going to be processed at all
 *               (for the latter one).
  -----------------------------------------------------------------------*/
int AmayaApp::FilterEvent(wxEvent& event)
{
  //  if (event.GetEventType() == wxEVT_CHAR)
  //    TTALOGDEBUG_1( TTA_LOG_MISC, _T("AmayaApp::FilterEvent type=%d"), event.GetEventType());
  return -1;
}

  
BEGIN_EVENT_TABLE(AmayaApp, wxApp)
  EVT_IDLE( AmayaApp::OnIdle ) // Process a wxEVT_IDLE event  
  //#if defined(_WINDOWS) && !defined(_WIN_PRINT)
  //  EVT_CHAR(     AmayaApp::OnChar )
  //  EVT_KEY_DOWN( AmayaApp::OnKeyDown )
  //#endif /* #if defined(_WINDOWS) && !defined(_WIN_PRINT) */
  EVT_KEY_DOWN( AmayaApp::OnKeyDown )
  EVT_CHAR(     AmayaApp::OnChar )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
