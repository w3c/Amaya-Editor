#ifdef _WX

#include "wx/wx.h"
#ifndef _GLPRINT
  #include "wx/xrc/xmlres.h"          // XRC XML resouces
#endif /* _GLPRINT */

#include "AmayaApp.h"

#define THOT_EXPORT extern
#include "amaya.h"


#include "wxAmayaSocketEventLoop.h"
#include "wxAmayaSocketEvent.h"

IMPLEMENT_APP(AmayaApp)

// Static attribut used to convert text from unicode to ISO-8859-1
// or from ISO-8859-1 to unicode
wxCSConv AmayaApp::conv_ascii(_T("ISO-8859-1"));

#ifndef _GLPRINT
// defined into EDITORAPP.c
extern int amaya_main (int argc, char** argv);
#else /* _GLPRINT */
// defined into print.c
extern int main (int argc, char** argv);
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
  WX_GL_MIN_ALPHA, 1, /* don't change the position of the entry (8) */
  WX_GL_STENCIL_SIZE, 1,
  WX_GL_DOUBLEBUFFER,
  0
};

#endif /* _GL */

bool AmayaApp::OnInit()
{
  // for debug : the output is stderr
  delete wxLog::SetActiveTarget( new wxLogStderr );

  // Required for images
  wxImage::AddHandler(new wxGIFHandler);
  wxImage::AddHandler(new wxPNGHandler);

#ifdef _GL
  // try to find a good configuration for opengl
  wxLogDebug( _T("AmayaApp - Try to find a valide opengl configuration.") );
  if ( !InitGLVisual(AttrList) )
    {
      wxLogDebug( _T("AmayaApp - ERROR -> Try to find another valide opengl configuration (simplier: without ALPHA channel).") );
      // error : try another configuration
      AttrList[8] = 0; /* remove the alpha channel: entry  (8)*/
      if ( !InitGLVisual(AttrList) )
	{
	  // error !
	  wxPrintf(_T("FATAL ERROR : Your OpenGL implementation does not support needed features!\n"));
	  wxExit();
	}
    }
  wxLogDebug( _T("AmayaApp - A valide opengl configuration has been found.") );
#endif /* _GL */
  
  // just convert arguments format (unicode to iso-8859-1) before passing it to amaya_main
  InitAmayaArgs();

#ifndef _GLPRINT
  /* initialize the Registry */
  TtaInitializeAppRegistry (amaya_argv[0]);

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

  // this is the amaya directory (need to be called after amaya_main or
  // TtaGetEnvString will return bad strings)
  wxString amaya_directory( TtaGetEnvString ("THOTDIR"), conv_ascii );

  // Now it's possible to load all the dialogs
  wxXmlResource::Get()->Load( amaya_directory+_T("/resources/xrc/InitConfirmDlgWX.xrc") );
  wxXmlResource::Get()->Load( amaya_directory+_T("/resources/xrc/OpenDocDlgWX.xrc") );
  wxXmlResource::Get()->Load( amaya_directory+_T("/resources/xrc/TitleDlgWX.xrc") );
  wxXmlResource::Get()->Load( amaya_directory+_T("/resources/xrc/SearchDlgWX.xrc") );
  // TODO: rajouter ici toutes les autres ressources a charger
  
  // just call amaya main from EDITORAPP.c
  amaya_main( amaya_argc, amaya_argv );
#endif /* #ifndef _GLPRINT */

#ifdef _GLPRINT
  main( amaya_argc, amaya_argv );
#endif /* #ifndef _GLPRINT */

  /* setup the socket event loop */
  /* when a socket is active, check every 100 ms if something happend on the socket */
  m_SocketEventLoop = new wxAmayaSocketEventLoop( 100 );
  wxAmayaSocketEvent::InitSocketEvent( m_SocketEventLoop );

  return true;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaApp
 *      Method:  OnExit
 * Description:  free all allocated memory
 *--------------------------------------------------------------------------------------
 */
int AmayaApp::OnExit()
{
  m_SocketEventLoop->Stop();
  delete m_SocketEventLoop;
  m_SocketEventLoop = NULL;

  // free arguments
  ClearAmayaArgs();

  // free internal amaya ressources
  TtaQuit();

  return 0;
}
  
/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaApp
 *      Method:  InitAmayaArgs
 * Description:  this methode convert wxApp::argc and wxApp::argv to amaya format
 *               amaya_argv must be free when closing application
 *--------------------------------------------------------------------------------------
 */
void AmayaApp::InitAmayaArgs()
{
  // convert argc and argv in order to be compatible with amaya
  amaya_argc = wxApp::argc;
  amaya_argv = new char*[amaya_argc];

  //  wxCSConv conv_ascii(_T("ISO-8859-1")); // to convert string in ASCII (ISO-8859-1)
  for ( int i = 0; i < amaya_argc; i++ )
  {
    // unicode to ascii convertion of every arguments
    wxString     amaya_arg( wxApp::argv[i] );
    wxASSERT_MSG( amaya_arg.IsAscii(), _T("arguments (argv) must contain only ascii char in order to be converted in ascii") );
    amaya_argv[i] = new char[amaya_arg.Length()+1];
    sprintf(amaya_argv[i],"%s", (const char*) amaya_arg.mb_str(conv_ascii));
  }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaApp
 *      Method:  ClearAmayaArgs
 * Description:  free arguments -> must be called when appli exit
 *--------------------------------------------------------------------------------------
 */
void AmayaApp::ClearAmayaArgs()
{
  for ( int i = 0; i < amaya_argc; i++ )
  {
    delete [] amaya_argv[i];
  }  
  delete [] amaya_argv;
}

void AmayaApp::OnIdle( wxIdleEvent& event )
{
  //  wxLogDebug( _T("AmayaApp::OnIdle") );

  event.Skip();
}

#ifdef _GL
int * AmayaApp::GetGL_AttrList()
{
  return AttrList;
}
#endif /* _GL */

BEGIN_EVENT_TABLE(AmayaApp, wxApp)
  EVT_IDLE( AmayaApp::OnIdle ) // Process a wxEVT_IDLE event  
END_EVENT_TABLE()


#endif /* #ifdef _WX */
